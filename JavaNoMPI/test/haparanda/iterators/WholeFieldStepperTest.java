package haparanda.iterators;

import static org.junit.Assert.*;

import org.junit.*;

import haparanda.utils.FixedThreadPoolExecutor;
import haparanda.utils.Task;

/**
 * Unit test for the stepping strategy WholeFieldStepper.
 *
 * @author Malin Kallen 2017-2018
 */
public class WholeFieldStepperTest 
{
	protected final static int ORDER = Integer.getInteger("DIM");
	
	protected WholeFieldStepper strategy;
	protected int[] stride;
	protected int[] size;
	protected int totalSize;
	
	@Before
	public void setUp() {
		assertEquals(3, ORDER);
		size = new int[ORDER];
		size[0]=3; size[1]=4; size[2]=5;
		stride = new int[ORDER];
		stride[0] = 1;
		stride[1] = size[0];
		stride[2] = size[0]*size[1];
		totalSize = size[0] * size[1] * size[2];
		strategy = new WholeFieldStepper(size, null);
	}
	
	/**
	 * Verify that currentIndex returns the index in the specified
	 * dimension.
	 */
	@Test
	public final void testCurrentIndex() {
		strategy.first();
		for (int i2=0; i2<size[2]; i2++) {
			for (int i1=0; i1<size[1]; i1++) {
				for (int i0=0; i0<size[0]; i0++) {
					assertEquals(i0, strategy.currentIndex(0));
					assertEquals(i1, strategy.currentIndex(1));
					assertEquals(i2, strategy.currentIndex(2));
					strategy.next();
				}
			}
		}
	}
	
	/**
	 * Verify that currentIndex throws an AssertionError if it is called when
	 * the iterator is outside the field.
	 */
	@Test (expected=AssertionError.class)
	public final void testCurrentIndex_outside0() {
		strategy.index=totalSize;
		strategy.currentIndex(0);
	}
	@Test (expected=AssertionError.class)
	public final void testCurrentIndex_outside2() {
		strategy.index=totalSize;
		strategy.currentIndex(2);
	}

	/**
	 * Verify that isInField returns true as long as the iterator has not
	 * stepped out of the field.
	 */
	@Test
	public final void testIsInField() {
		strategy.first();
		for (int i=0; i<totalSize; i++) {
			assertTrue(strategy.isInField());
			strategy.next();
		}
		assertFalse(strategy.isInField());
	}

	/**
	 * Verify the behavior of first: It should (re)set the iterator to 0.
	 */
	@Test
	public final void testFirst() {
		int expected = 0;
		strategy.first();
		assertEquals(expected, strategy.index);
	}

	/**
	 * For each element in the field: verify that linearNeighborIndex
	 * returns current iterator index + offset * s
	 * where s is the stride in the dimension in which the neighbor is
	 * located, when called for a neighbor which is inside the field.
	 */
	@Test
	public final void testLinearNeighborIndex() {
		strategy.first();
		int index = 0;
		for (int i2=0; i2<size[2]; i2++) {
			for (int i1=0; i1<size[1]; i1++) {
				for (int i0=0; i0<size[0]; i0++) {
					// Closest neighbor in dimension 0
					if (i0>0) {
						assertEquals(index-stride[0], strategy.linearNeighborIndex(0, -1));
					}
					if (i0<size[0]-1) {
						assertEquals(index+stride[0], strategy.linearNeighborIndex(0, 1));
					}
					// Closest neighbor in dimension 1
					if (i1>0) {
						assertEquals(index-stride[1], strategy.linearNeighborIndex(1, -1));
					}
					if (i1<size[1]-1) {
						assertEquals(index+stride[1], strategy.linearNeighborIndex(1, 1));
					}
					// Closest neighbor in dimension 2
					if (i2>0) {
						assertEquals(index-stride[2], strategy.linearNeighborIndex(2, -1));
					}
					if (i2<size[2]-1) {
						assertEquals(index+stride[2], strategy.linearNeighborIndex(2, 1));
					}
					// Neighbor further away in dimension 2
					if (i2>2) {
						assertEquals(index-3*stride[2], strategy.linearNeighborIndex(2, -3));
					}
					if (i2<size[2]-3) {
						assertEquals(index+3*stride[2], strategy.linearNeighborIndex(2, 3));
					}
					strategy.next();
					index++;
				}
			}
		}
	}
	
	
	/**
	 * Verify that linearNeighborIndex throws an AssertionError if it is called
	 * for neighbors outside the field.
	 */
	@Test (expected=AssertionError.class)
	public final void testLinearNeighborIndex_outside_lower2() {
		// Middle of boundary i2=0
		final int midIndex = stride[1]*size[1]/2 + stride[0]*size[0]/2;
		strategy.first();
		for (int i=0; i<midIndex; i++) {
			strategy.next();
		}
		strategy.linearNeighborIndex(2, -1);
	}
	@Test (expected=AssertionError.class)
	public final void testLinearNeighborIndex_outside_upper0() {
		// Middle of upper i0 boundary
		final int midIndex = stride[2]*size[2]/2 + stride[1]*size[1]/2 + size[0]-1;
		strategy.first();
		for (int i=0; i<midIndex; i++) {
			strategy.next();
		}
		strategy.linearNeighborIndex(0, 1);
	}


	/**
	 * For each element in the field: Call neighborInField for the closest
	 * neighbor in each direction and verify that the method returns true
	 * if the neighbor is inside the field and false otherwise.
	 */
	@Test
	public final void testNeighborInField() {
		strategy.first();
		for (int i2=0; i2<size[2]; i2++) {
			for (int i1=0; i1<size[1]; i1++) {
				for (int i0=0; i0<size[0]; i0++) {
					assertEquals(0!=i0, strategy.neighborInField(0, -1));
					assertEquals(size[0]-1!=i0, strategy.neighborInField(0, 1));
					assertEquals(0!=i1, strategy.neighborInField(1, -1));
					assertEquals(size[1]-1!=i1, strategy.neighborInField(1, 1));
					assertEquals(0!=i2, strategy.neighborInField(2, -1));
					assertEquals(size[2]-1!=i2, strategy.neighborInField(2, 1));
					strategy.next();
				}
			}
		}
	}
	
	/**
	 * Verify that next throws an AssertionError if it is called after having
	 * stepped outside the field.
	 */
	@Test (expected=AssertionError.class)
	public final void testNext_outside() {
		strategy.first();
		// Step to the last element
		for (int i=0; i<totalSize; i++) {
			strategy.next();
		}
		// This call should cause an AssertionError
		strategy.next();
	}
	
	/**
	 * Verify that each element in the field which is iterated over is
	 * touched and considered being in the field exactly once when the
	 * iteration is thread parallel.
	 * @throws InterruptedException If the parallel executor was interrupted
	 */
	@Test
	public final void testParallelStepping() throws InterruptedException {
		int[] timesTouched = new int[totalSize];
		FixedThreadPoolExecutor.execute(
				(taskId, numTasks) -> {
					return new ParallelStepper(taskId, numTasks, timesTouched);
				}
		);
		for (int i=0; i<totalSize; i++) {
			assertEquals(1, timesTouched[i]);
		}
	}
	
	/**
	 * Wrapper class for WholeFieldStepper.
	 */
	private class ParallelStepper extends Task  {
		private int[] timesTouched;
		
		/**
		 * @param taskId ID of the task
		 * @param numTasks Number of tasks that shares the work (including this one)
		 * @param timesTouched Counter that tracks how many times each index has bee touched
		 */
		private ParallelStepper(int taskId, int numTasks, int[] timesTouched) {
			super(taskId, numTasks);
			this.timesTouched = timesTouched;
		}
		
		/*
		 * Create a stepper, step over its elements, and count the number of
		 * times each index is touched.
		 */
		public Void call() {
			FieldSteppingStrategy parallelStepper = new WholeFieldStepper(size, this);
			while (parallelStepper.isInField()) {
				timesTouched[parallelStepper.index]++;
				parallelStepper.next();
			}
			return null;
		}		
	}
}
