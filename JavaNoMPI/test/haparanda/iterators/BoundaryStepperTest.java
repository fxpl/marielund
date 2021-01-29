package haparanda.iterators;

import static org.junit.Assert.*;

import java.util.*;
import org.junit.*;

import haparanda.utils.*;

/**
 * Unit test for the stepping strategy BoundaryStepper.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017-2018
 */
public class BoundaryStepperTest 
{
	protected final static int ORDER = Integer.getInteger("DIM");
	
	protected int[] boundarySizes;
	protected BoundaryStepper strategy;
	
	private int[] strides;
	private int[] sizes;
	private int totalSize;

	private int[] lowerMinIndices;
	private int[] upperMinIndices;
	private int[] lowerMaxIndices;
	private int[] upperMaxIndices;


	@Before
	public void setUp() {
		assertEquals(3, ORDER);
		sizes = new int[ORDER];
		sizes[0] = 3; sizes[1] = 4; sizes[2] = 5;
		strides = new int[ORDER];
		strides[0] = 1;
		strides[1] = sizes[0];
		strides[2] = sizes[0] * sizes[1];
		totalSize = sizes[0] * sizes[1] * sizes[2];
		strategy = new BoundaryStepper(sizes);

		lowerMinIndices = new int[ORDER];
		lowerMaxIndices = new int[ORDER];
		upperMinIndices = new int[ORDER];
		upperMaxIndices = new int[ORDER];
		boundarySizes = new int[ORDER];

		for (int i=0; i<ORDER; i++) {
			upperMaxIndices[i] = totalSize-1;
			upperMinIndices[i] = (sizes[i]-1) * strides[i];
			for (int j=0; j<ORDER; j++) {
				if (i!=j) {
					lowerMaxIndices[i] += (sizes[j]-1) * strides[j];
				}
			}
			boundarySizes[i] = totalSize/sizes[i];
		}
	}

	/**
	 * For each boundary: Verify that isInField returns true until next has
	 * been called x times, and thereafter returns false.
	 * x = total number of elements on the boundary.
	 */
	@Test
	public final void testIsInField() {
		for (int i=0; i<ORDER; i++) {
			verifyIsInField(new BoundaryId(i, true));
			verifyIsInField(new BoundaryId(i, false));
		}
	}

	/**
	 * Verify the behavior of first: Irrespective of which boundary it
	 * currently iterates over, it should make the iterator point at the
	 * first element of that boundary.
	 */
	@Test
	public final void testFirst() {
		for (int i=0; i<ORDER; i++) {
			// Lower boundaries
			strategy.setBoundaryToIterate(new BoundaryId(i, true));
			// Set index to something else than the min indices
			strategy.index = 100000;
			strategy.first();
			assertEquals(lowerMinIndices[i], strategy.index);

			// Upper boundaries
			strategy.setBoundaryToIterate(new BoundaryId(i, false));
			// Set index to something else than the min indices
			strategy.index = 0;
			strategy.first();
			assertEquals(upperMinIndices[i], strategy.index);
		}
	}

	/**
	 * For all boundaries of the field: verify that next steps through all
	 * elements on the boundary.
	 */
	@Test
	public final void testNext() {
		for (int d=2; d<ORDER; d++) {
			int otherDim1 = 0==d ? 1 : 0;
			int otherDim2 = 2==d ? 1 : 2;
			// Lower boundaries
			strategy.setBoundaryToIterate(new BoundaryId(d, true));
			for (int i2=0; i2<sizes[otherDim2]; i2++) {
				for (int i1=0; i1<sizes[otherDim1]; i1++) {
					assertEquals(i2*strides[otherDim2] + i1*strides[otherDim1],
							strategy.index);
					strategy.next();
				}
			}
			// Upper boundaries
			strategy.setBoundaryToIterate(new BoundaryId(d, false));
			for (int i2=0; i2<sizes[otherDim2]; i2++) {
				for (int i1=0; i1<sizes[otherDim1]; i1++) {
					assertEquals(upperMinIndices[d] + i2*strides[otherDim2] + i1*strides[otherDim1],
							strategy.index);
					strategy.next();
				}
			}
		}
	}
	
	/**
	 * Verify that next throws an AssertionError if called from the last
	 * element of the upper boundary along dimension 0.
	 */
	@Test (expected=AssertionError.class)
	public final void testNext_endU0() {
		strategy.setBoundaryToIterate(new BoundaryId(0, false));
		// Step to last element
		for (int i=0; i<boundarySizes[0]; i++) {
			strategy.next();
		}
		// This call should cause an AssertionError
		strategy.next();
	}
	
	/**
	 * Verify that next throws an AssertionError if called from the last
	 * element of the lower boundary along dimension 2.
	 */
	@Test (expected=AssertionError.class)
	public final void testNext_endL2() {
		strategy.setBoundaryToIterate(new BoundaryId(2, true));
		// Step to last element
		for (int i=0; i<boundarySizes[2]; i++) {
			strategy.next();
		}
		// This call should cause an AssertionError
		strategy.next();
	}
	
	/**
	 * Verify that each element on the boundary of the field which is
	 * iterated over is touched exactly once and other elements are touched
	 * 0 times when the iteration is thread parallel.
	 * @throws InterruptedException If the parallel executor was interrupted
	 */
	@Test
	public void testParallelStepping() throws InterruptedException {
		int[] timesTouched = new int[totalSize];
		for (int d=0; d<ORDER; d++) {
			for (int l=0; l<2; l++) {
				boolean lower = 0==l;
				Arrays.fill(timesTouched, 0);
				BoundaryId boundary = new BoundaryId(d, lower);
				FixedThreadPoolExecutor.execute(
						(taskId, numTasks) -> {
							return new ParallelStepper(taskId, numTasks, timesTouched, boundary);
						}
				);
				int nextStride = ORDER-1 == d ? totalSize : strides[d+1];
				for (int i=0; i<totalSize; i++) {
					if ((i+l*strides[d])%nextStride < strides[d]) {
						// i is an index on the current boundary
						assertEquals(1, timesTouched[i]);
					} else {
						assertEquals(0, timesTouched[i]);
					}
				}
			}
		}
	}
	
	/**
	 * Wrapper class for BoundaryStepper
	 */
	private class ParallelStepper extends Task {
		private int[] timesTouched;
		BoundaryId boundary;	// The boundary to iterate over in the current test case
		
		/**
		 * @param taskId ID of the task
		 * @param numTasks Number of tasks that shares the work (including this one)
		 * @param timesTouched Counter that tracks how many times each index has bee touched
		 */
		private ParallelStepper(int taskId, int numTasks, int[] timesTouched, BoundaryId boundaryToIterate) {
			super(taskId, numTasks);
			this.timesTouched = timesTouched;
			this.boundary = boundaryToIterate;
		}
		
		/*
		 * Create a stepper, step over its elements, and count the number of
		 * times each index is touched.
		 */
		public Void call() {
			BoundaryStepper parallelStepper = new BoundaryStepper(sizes);
			parallelStepper.setBoundaryToIterate(boundary, this);
			while (parallelStepper.isInField()) {
				timesTouched[parallelStepper.index]++;
				parallelStepper.next();
			}
			return null;
		}
	}

	/**
	 * Verify that setBoundaryToIterate sets dimension and lower to the
	 * values given as arguments and calculates the minimum and maximum
	 * indices of the boundary.
	 */
	@Test
	public final void testSetBoundaryToIterate() {
		// Lower boundaries
		for (int i=0; i<ORDER; i++) {
			strategy.setBoundaryToIterate(new BoundaryId(i, true));
			assertEquals(i, strategy.boundary.getDimension());
			assertEquals(true, strategy.boundary.isLowerSide());
			assertEquals(lowerMinIndices[i], strategy.minIndex);
			assertEquals(lowerMaxIndices[i], strategy.maxIndex);
		}
		// Upper boundaries
		for (int i=0; i<ORDER; i++) {
			strategy.setBoundaryToIterate(new BoundaryId(i, false));
			assertEquals(i, strategy.boundary.getDimension());
			assertEquals(false, strategy.boundary.isLowerSide());
			assertEquals(upperMinIndices[i], strategy.minIndex);
			assertEquals(upperMaxIndices[i], strategy.maxIndex);
		}
	}

	/**
	 * Verify that the stepper is always considered to be outside the field
	 * when the size of the field is 0.
	 */
	@Test
	public final void testSetBoundaryToIterate_emptyField() {
		int[] sizes = new int[ORDER];
		BoundaryStepper emptyStepper = new BoundaryStepper(sizes);
		for(int d=0; d<ORDER; d++) {
			for(int dir=0; dir<2; dir++) {
				emptyStepper.setBoundaryToIterate(new BoundaryId(d, dir==0));
				assertFalse(emptyStepper.isInField());
			}
		}
	}
	
	/**
	 * Verify that setBoundaryToIterate throws an AssertionError if it is given
	 * the order of the field as dimension argument.
	 */
	@Test (expected=AssertionError.class)
	public final void testSetBoundaryToIterate_order() {
		strategy.setBoundaryToIterate(new BoundaryId(ORDER, true));
	}
	
	/**
	 * Verify that setBoundaryToIterate throws an AssertionError if it is given
	 * a value greater than the order of the field as dimension argument.
	 */
	@Test (expected=AssertionError.class)
	public final void testSetBoundaryToIterate_gtOrder() {
		strategy.setBoundaryToIterate(new BoundaryId(ORDER+5, true));
	}


	/**
	 * Verify the behavior of isInField on the specified boundary
	 *
	 * @param boundary The boundary on which the behavior is to be verified
	 */
	final private void verifyIsInField(BoundaryId boundary) {
		strategy.setBoundaryToIterate(boundary);
		for (int j = 0; j < boundarySizes[boundary.getDimension()]; j++) {
			assertTrue(strategy.isInField());
			strategy.next();
		}
		assertFalse(strategy.isInField());
	}

}
