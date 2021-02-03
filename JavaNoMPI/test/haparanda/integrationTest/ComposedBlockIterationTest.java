package haparanda.integrationTest;

import static org.junit.Assert.*;
import org.junit.*;
import haparanda.grid.*;
import haparanda.iterators.*;
import haparanda.utils.*;

/**
 * Integration test for ComputationalComposedBlock + ComposedFieldBoundaryIterator.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017-2019, 2021
 */
public class ComposedBlockIterationTest 
{
	protected static final int DIMENSIONALITY = Integer.getInteger("DIM", 3);
	protected int extent;
	protected int boundarySize;
	protected BoundaryIterator boundaryIterator;
	private int elementsPerDim;
	private int[] stride;
	private double[] values;
	private ComputationalComposedBlock composedBlock;

	@Before
	public void setUp() {
		assertEquals(3, DIMENSIONALITY);
		elementsPerDim = 10;
		extent = 4;
		int numElements = HaparandaMath.power(elementsPerDim, DIMENSIONALITY);
		boundarySize = numElements/elementsPerDim;
		stride = new int[DIMENSIONALITY];
		stride[0]=1; stride[1]=elementsPerDim; stride[2]=elementsPerDim*elementsPerDim;

		values = new double[numElements];
		for (int i=0; i<numElements; i++) {
			values[i] = 2.1 * i;
		}
		composedBlock = new ComputationalComposedBlock(elementsPerDim, extent, values);
		composedBlock.initializeSideRegions();
		boundaryIterator = composedBlock.getBoundaryIterator();
	}
	
	/**
	 * Step through the boundaries of the main region of the composed block and
	 * try to fetch all neighbors in direction i, where |i| is the dimension
	 * in which the value is constant on the current boundary and the sign of i
	 * is set so that the requested neighbor is in a the ghost region. Verify
	 * that a value between 0 and 1 was returned.
	 */
	@Test
	public final void testCurrentBoundaryNeighborInGhostRegion() {
		BoundaryIterator expectedIterator = composedBlock.getBoundaryIterator(); // Values are the same on all nodes
		BoundaryId boundary = new BoundaryId();
		for (int d=0; d<2*DIMENSIONALITY; d++) {
			boundary.setDimension(d/2);
			boundary.setIsLowerSide(1==d%2);
			boundaryIterator.setBoundaryToIterate(boundary);
			BoundaryId oppositeBoundary = boundary.oppositeSide();
			expectedIterator.setBoundaryToIterate(oppositeBoundary);
			for (int i2=0; i2<elementsPerDim; i2++) {
				for (int i1=0; i1<elementsPerDim; i1++) {
					for (int offset=1; offset<=extent; offset++) {
						double expectedValue, actualValue;
						if (boundary.isLowerSide()) {
							expectedValue = expectedIterator.currentNeighbor(boundary.getDimension(), -offset+1);
							actualValue = boundaryIterator.currentNeighbor(boundary.getDimension(), -offset);
						} else {
							expectedValue = expectedIterator.currentNeighbor(boundary.getDimension(), offset-1);
							actualValue = boundaryIterator.currentNeighbor(boundary.getDimension(), offset);
						}
						assertEquals(expectedValue, actualValue, 4*Math.ulp(expectedValue));
					}
					boundaryIterator.next();
					expectedIterator.next();
				}
			}
		}
	}

	/**
	 * Step through the boundaries of the main region of the composed block and
	 * try to fetch the closest neighbor in direction i, where |i| is the
	 * dimension in which the value is constant on the current boundary and the
	 * sign of i is set so that the requested neighbor is inside the main
	 * region. Verify that the right value was returned.
	 */
	@Test
	public final void testCurrentBoundaryNeighborInside() {
		for (int d=0; d<DIMENSIONALITY; d++) {
			int otherDim1 = 0==d ? 1 : 0;
			int otherDim2 = 2==d ? 1 : 2;
			for (int side=0; side<2; side++) {
				boundaryIterator.setBoundaryToIterate(new BoundaryId(d, 0==side));
				for (int i2=0; i2<elementsPerDim; i2++) {
					for (int i1=0; i1<elementsPerDim; i1++) {
						int index = side*(elementsPerDim-1)*stride[d]
								+ i1*stride[otherDim1]+ i2*stride[otherDim2];
						int dir = 0==side ? 1 : -1;
						double expectedValue = values[index+dir*stride[d]]; 
						assertEquals(expectedValue, boundaryIterator.currentNeighbor(d, dir), 4*Math.ulp(expectedValue));
						boundaryIterator.next();
					}
				}
			}
		}
	}
	
	/**
	 * Verify that an AssertionError is thrown if a value outside the block
	 * (including ghost region) is accessed. In this case outside the upper
	 * ghost region along dimension 0.
	 */
	@Test (expected=AssertionError.class)
	public final void testCurrentBoundaryNeighbor_outsideU0() {
		boundaryIterator.setBoundaryToIterate(new BoundaryId(0, false));
		// Step middle of boundary
		for (int i=0; i<boundarySize/2; i++) {
			boundaryIterator.next();
		}
		boundaryIterator.currentNeighbor(0, extent+1);
	}
	
	/**
	 * Verify that an AssertionError is thrown if a value outside the block
	 * (including ghost region) is accessed. In this case outside the lower
	 * ghost region along dimension 2.
	 */
	@Test (expected=AssertionError.class)
	public final void testCurrentBoundaryNeighbor_outsideL2() {
		boundaryIterator.setBoundaryToIterate(new BoundaryId(2, true));
		// Step middle of boundary
		for (int i=0; i<boundarySize/2; i++) {
			boundaryIterator.next();
		}
		boundaryIterator.currentNeighbor(2, -extent-1);
	}
	
	/**
	 * Fetch the value of each boundary element in the composed block and verify
	 * that the correct value was returned.
	 */
	@Test
	public final void testCurrentBoundaryValue() {
		for (int d=0; d<DIMENSIONALITY; d++) {
			int otherDim1 = 0==d ? 1 : 0;
			int otherDim2 = 2==d ? 1 : 2;
			for (int side=0; side<2; side++) {
				boundaryIterator.setBoundaryToIterate(new BoundaryId(d, 0==side));
				for (int i2=0; i2<elementsPerDim; i2++) {
					for (int i1=0; i1<elementsPerDim; i1++) {
						int index = side*(elementsPerDim-1)*stride[d]
								+ i1*stride[otherDim1]+ i2*stride[otherDim2];
						double expectedValue = values[index];
						assertEquals(expectedValue, boundaryIterator.currentValue(), 4*Math.ulp(expectedValue));
						boundaryIterator.next();
					}
				}
			}
		}
	}
}
