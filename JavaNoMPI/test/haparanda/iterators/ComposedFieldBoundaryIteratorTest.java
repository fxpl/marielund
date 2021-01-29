package haparanda.iterators;

import static org.junit.Assert.*;
import org.junit.*;

import haparanda.utils.BoundaryId;

/**
 * Unit test for ComposedFieldBoundaryIterator.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017
 */
public class ComposedFieldBoundaryIteratorTest extends FieldIteratorTest
{
	protected BoundaryIterator iterator;
	protected int elementsPerDim;
	protected int extent;
	private double[] values;
	BoundaryId l2, u0;
	
	@Before
	public void setUp() {
		elementsPerDim = 5;
		final int[] sizes =	{elementsPerDim, elementsPerDim, elementsPerDim};
		super.setUp(sizes);

		values = new double[totalSize];
		for (int i=0; i<totalSize; i++) {
			values[i] = 33.0 * i;
		}

		extent = 4;
		// Initialize ghost region iterators
		FieldIterator[][] ghostIterators = new FieldIterator[FIELD_ORDER][2];
		for (int i=0; i<FIELD_ORDER; i++) {
			int[] ghostSizes = sizes.clone();
			ghostSizes[i] = extent;
			int totalGhostSize = 1;
			for (int j=0; j<FIELD_ORDER; j++) {
				totalGhostSize *= ghostSizes[j];
			}
			for (int j=0; j<2; j++) {
				double ghostValue = 1000*i + 100*j;
				double[] ghostValues = new double[totalGhostSize];
				for (int k=0; k<totalGhostSize; k++) {
					ghostValues[k] = ghostValue++;
				}
				ghostIterators[i][j] = new ValueFieldBoundaryIterator(
						ghostSizes, ghostValues, 0);
			}
		}
		iterator = new ComposedFieldBoundaryIterator(this.sizes, values, 0, ghostIterators);
		l2 = new BoundaryId(2, true);
		u0 = new BoundaryId(0, false);
	}

	/*** Test methods ***/

	/**
	 * Verify that currentIndex returns the index along the dimension given
	 * as argument. The verification is done for all points on all
	 * boundaries and all 3 dimensions. (Thereby, this method also verifies
	 * that setBoundaryToIterate makes the iterator traverse the correct
	 * boundary.)
	 */
	@Test
	public final void testCurrentIndex() {
		super.testCurrentIndex(iterator);
	}

	/**
	 * Verify that an AssertionError is thrown if currentIndex is called when
	 * the iterator is outside the actual boundary, in this case the lower
	 * boundary along dimension 2.
	 */
	@Test (expected=AssertionError.class)
	public final void testCurrentIndex_outsideL2() {
		iterator.setBoundaryToIterate(l2);
		stepIteratorOutsideField(iterator);
		iterator.currentIndex(0);
	}
	/**
	 * Verify that an AssertionError is thrown if currentIndex is called when
	 * the iterator is outside the actual boundary, in this case the upper
	 * boundary along dimension 0.
	 */
	@Test (expected=AssertionError.class)
	public final void testCurrentIndex_outsideU0() {
		iterator.setBoundaryToIterate(u0);
		stepIteratorOutsideField(iterator);
		iterator.currentIndex(0);
	}

	/**
	 * Verify that currentNeighbor returns the value of the requested
	 * neighbor. The verification is done for the midpoint of each boundary
	 * and for all neighbors, including the one in the ghost region.
	 */
	@Test
	public final void testCurrentNeighbor() {
		for (int d=0; d<FIELD_ORDER; d++) {
			for (int side=0; side<2; side++) {
				verifyCurrentNeighbor(new BoundaryId(d, 0==side));
			}
		}
	}

	/**
	 * Verify that an AssertionError is thrown if we try to get the value of a
	 * neighbor located outside the current boundary, in this case the lower
	 * boundary along dimension 2.
	 */
	@Test (expected=AssertionError.class)
	public final void testCurrentNeighbor_outsideL2() {
		iterator.setBoundaryToIterate(l2);
		stepIteratorToMiddleOfBoundary(iterator, l2);
		iterator.currentNeighbor(2, -extent-1);
	}
	/**
	 * Verify that an AssertionError is thrown if we try to get the value of a
	 * neighbor located outside the current boundary, in this case the upper
	 * boundary along dimension 0.
	 */
	@Test (expected=AssertionError.class)
	public final void testCurrentNeighbor_outsideU0() {
		iterator.setBoundaryToIterate(u0);
		stepIteratorToMiddleOfBoundary(iterator, u0);
		iterator.currentNeighbor(0, extent+1);
	}

	/**
	 * Verify that currentValue returns the value of the element currently
	 * pointed at. The verification is done for every element on every
	 * boundary.
	 *
	 * Note that this method assumes that field_order = 3.
	 */
	@Test
	public final void testCurrentValue() {
		assert(3 == FIELD_ORDER);
		for (int d=0; d<FIELD_ORDER; d++) {
			int otherDim1 = 0==d ? 1 : 0;
			int otherDim2 = 2==d ? 1 : 2;
			for (int side=0; side<2; side++) {
				iterator.setBoundaryToIterate(new BoundaryId(d, 0==side));
				for (int i2=0; i2<sizes[otherDim2]; i2++) {
					for (int i1=0; i1<sizes[otherDim1]; i1++) {
						int index =
							side * (sizes[d] -1) * strides[d]
							+ i1 * strides[otherDim1]
							+ i2 * strides[otherDim2];
						
						double expectedValue = values[index];
						assertEquals(expectedValue, iterator.currentValue(), 4*Math.ulp(expectedValue));
						iterator.next();
					}
				}
			}
		}
	}

	/**
	 * Verify that an AssertionError is thrown if currentValue is called when
	 * the iterator is outside the current boundary (in this case the lower one
	 * along dimension 2).
	 */
	@Test (expected=AssertionError.class)
	public final void testCurrentValue_outsideL2() {
		iterator.setBoundaryToIterate(l2);
		stepIteratorOutsideField(iterator);
		iterator.currentValue();
	}
	/**
	 * Verify that an AssertionError is thrown if currentValue is called when
	 * the iterator is outside the current boundary (in this case the upper one
	 * along dimension 0).
	 */
	@Test (expected=AssertionError.class)
	public final void testCurrentValue_outsideU0() {
		iterator.setBoundaryToIterate(u0);
		stepIteratorOutsideField(iterator);
		iterator.currentValue();
	}

	@Test
	public final void testFirst() {
		super.testFirst(iterator);
	}

	@Test
	public final void testIsInField() {
		super.testIsInField(iterator);
	}

	/**
	 * For the element in the middle of each boundary: Verify that
	 * setCurrentNeighbor changes the value of the requested neighbor. The
	 * verification is done for every neighbor, in the field and in the
	 * ghost region.
	 */
	@Test
	public final void testSetCurrentNeighbor() {
		for (int d=0; d<FIELD_ORDER; d++) {
			for (int side=0; side<2; side++) {
				verifySetCurrentNeighbor(new BoundaryId(d, 0==side));
			}
		}
	}

	/**
	 * Verify that an AssertionError is thrown if we try to set the value of a
	 * neighbor located outside the current boundary, in this case the lower
	 * boundary along dimension 2.
	 */
	@Test (expected=AssertionError.class)
	public final void testSetCurrentNeighbor_outsideL2() {
		iterator.setBoundaryToIterate(l2);
		stepIteratorToMiddleOfBoundary(iterator, l2);
		iterator.setCurrentNeighbor(2, -extent-1, 73);
	}
	/**
	 * Verify that an AssertionError is thrown if we try to set the value of a
	 * neighbor located outside the current boundary, in this case the upper
	 * boundary along dimension 0.
	 */
	@Test (expected=AssertionError.class)
	public final void testSetCurrentNeighbor_outsideU0() {
		iterator.setBoundaryToIterate(u0);
		stepIteratorToMiddleOfBoundary(iterator, u0);
		iterator.setCurrentNeighbor(0, extent+1, 73);
	}

	/**
	 * Verify that setValue changes the value of the element currently
	 * pointed at. The verification is done for the first element on each
	 * boundary.
	 */
	@Test
	public final void testSetCurrentValue() {
		for (int d=0; d<FIELD_ORDER; d++) {
			for (int side=0; side<2; side++) {
				int boundarySize = totalSize/sizes[d];
				iterator.setBoundaryToIterate(new BoundaryId(d, 0==side));
				// Change the values
				for (int i=0; i<boundarySize; i++) {
					iterator.setCurrentValue(3.2*i);
					iterator.next();
				}
				// Verify that they were changed
				iterator.first();
				for (int i=0; i<boundarySize; i++) {
					double expectedValue = 3.2*i;
					assertEquals(expectedValue, iterator.currentValue(), 4*Math.ulp(expectedValue));
					iterator.next();
				}
			}
		}
	}

	/**
	 * Verify that an AssertionError is thrown if setCurrentValue is called when
	 * the iterator is outside the current boundary, in this case the lower
	 * boundary along dimension 2.
	 */
	@Test (expected=AssertionError.class)
	public final void testSetCurrentValue_outsideL2() {
		iterator.setBoundaryToIterate(l2);
		stepIteratorOutsideField(iterator);
		iterator.setCurrentValue(784.0);
	}
	/**
	 * Verify that an AssertionError is thrown if setCurrentValue is called when
	 * the iterator is outside the current boundary, in this case the upper
	 * boundary along dimension 0.
	 */
	@Test (expected=AssertionError.class)
	public final void testSetCurrentValue_outsideU0() {
		iterator.setBoundaryToIterate(u0);
		stepIteratorOutsideField(iterator);
		iterator.setCurrentValue(785.0);
	}

	/**
	 * Verify that size returns the size of the field, in each dimension.
	 */
	@Test
	public final void testSize() {
		assertEquals(sizes[0]+2*extent, iterator.size(0));
		assertEquals(sizes[1]+2*extent, iterator.size(1));
		assertEquals(sizes[2]+2*extent, iterator.size(2));
	}


	/*** Helper methods ***/

	/**
	 * Move the iterator to the mid point of the specified boundary and
	 * check the value of each neighbor. Also check the neighbor two steps
	 * inwards and and the one in the other end of the ghost region.
	 *
	 * @param boundary Boundary on which currentNeigbor will be verified.
	 */
	protected final void verifyCurrentNeighbor(final BoundaryId boundary) {
		stepIteratorToMiddleOfBoundary(iterator, boundary);
		int[] neighborIndex = {
				iterator.currentIndex(0),
				iterator.currentIndex(1),
				iterator.currentIndex(2)};
		for (int d=0; d<FIELD_ORDER; d++) {
			// Indices in ghost regions
			int lowerGhostMidIndex = 0;
			for (int i=0; i<FIELD_ORDER; i++) {
				if (i < d) {
					lowerGhostMidIndex += sizes[i]/2 * strides[i];
				} else if (i > d) {
					lowerGhostMidIndex += sizes[i]/2 * strides[i] / sizes[d] * extent;
				}
			}
			int upperGhostMidIndex = lowerGhostMidIndex + (extent-1)*strides[d];

			// Closest neighbor below
			if (d == boundary.getDimension() && boundary.isLowerSide()) {
				// Neighbor is in ghost region
				double expectedValue = 1000.0*d + upperGhostMidIndex;
				assertEquals(expectedValue, iterator.currentNeighbor(d, -1), 4*Math.ulp(expectedValue));
			} else {
				// Neighbor is inside field
				neighborIndex[d]--;
				double expectedValue = values[linearIndexOf(neighborIndex)]; 
				assertEquals(expectedValue, iterator.currentNeighbor(d, -1), 4*Math.ulp(expectedValue));
				neighborIndex[d]++;
			}

			// Closest neighbor above
			if (d == boundary.getDimension() && !boundary.isLowerSide()) {
				// Neighbor is in ghost region
				double expectedValue = 1000.0*d + 100 + lowerGhostMidIndex;
				assertEquals(expectedValue, iterator.currentNeighbor(d, 1), 4*Math.ulp(expectedValue));
			} else {
				// Neighbor is inside field
				neighborIndex[d]++;
				double expectedValue = values[linearIndexOf(neighborIndex)]; 
				assertEquals(expectedValue, iterator.currentNeighbor(d, 1), 4*Math.ulp(expectedValue));
				neighborIndex[d]--;
			}

			// Neighbors further away
			if (d == boundary.getDimension()) {
				if (boundary.isLowerSide()) {
					// Neighbor below, on other side of ghost region
					double expectedValue = 1000*d + lowerGhostMidIndex;
					assertEquals(expectedValue, iterator.currentNeighbor(d, -extent), 4*Math.ulp(expectedValue));
					// Neighbor 2 steps above (inside region)
					neighborIndex[d]+=2;
					expectedValue = values[linearIndexOf(neighborIndex)]; 
					assertEquals(expectedValue, iterator.currentNeighbor(d, 2), 4*Math.ulp(expectedValue));
					neighborIndex[d]-=2;
				} else {
					// Neighbor 2 steps below (inside region)
					neighborIndex[d]-=2;
					double expectedValue = values[linearIndexOf(neighborIndex)]; 
					assertEquals(expectedValue, iterator.currentNeighbor(d, -2), 4*Math.ulp(expectedValue));
					neighborIndex[d]+=2;
					// Neighbor above, on other side of ghost region
					expectedValue = 1000*d + 100 + upperGhostMidIndex;
					assertEquals(expectedValue, iterator.currentNeighbor(d, extent), 4*Math.ulp(expectedValue));
				}
			}
		}
	}

	/**
	 * Move the iterator to the mid point of the specified boundary, change
	 * the value of each neighbor (also those in the ghost regions) and
	 * verify that the values were changed correctly.
	 *
	 * @param boundary Boundary on which currentNeigbor will be verified.
	 */
	protected final void verifySetCurrentNeighbor(final BoundaryId boundary) {
		stepIteratorToMiddleOfBoundary(iterator, boundary);
		// Change the value of each neighbor
		for (int d=0; d<FIELD_ORDER; d++) {
			iterator.setCurrentNeighbor(d, -1, d + 101.0);
			iterator.setCurrentNeighbor(d, 1, d + 102.0);
		}
		// ...and check the values
		for (int d=0; d<FIELD_ORDER; d++) {
			
			double expectedValue = d + 101.0;
			assertEquals(expectedValue, iterator.currentNeighbor(d, -1), 4*Math.ulp(expectedValue));
			expectedValue = d + 102;
			assertEquals(expectedValue, iterator.currentNeighbor(d, 1), 4*Math.ulp(expectedValue));
		}
	}
}
