package haparanda.iterators;

import static org.junit.Assert.*;
import org.junit.*;

import haparanda.utils.BoundaryId;

/**
 * Unit test for ValueFieldBoundaryIterator.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017
 */
public class ValueFieldBoundaryIteratorTest extends FieldIteratorTest
{
	protected BoundaryIterator iterator;
	private double[] values;
	private BoundaryId u0, l2;

	@Before
	public void setUp() {
		super.setUp();
		values = new double[totalSize];
		for (int i=0; i<totalSize; i++) {
			values[i] = 7.87 * i;
		}
		iterator = new ValueFieldBoundaryIterator(sizes, values, 0);
		u0 = new BoundaryId(0, false);
		l2 = new BoundaryId(2, true);
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
		super.stepIteratorOutsideField(iterator);
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
		super.stepIteratorOutsideField(iterator);
		iterator.currentIndex(0);
	}

	/**
	 * Verify that currentNeighbor returns the value of the requested
	 * neighbor (when it is inside the field). The verification is done for
	 * the midpoint of each boundary.
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
	 * neighbor located outside the current boundary:
	 * outside the lower boundary along dimension 2...
	 */
	@Test (expected=AssertionError.class)
	public final void testCurrentNeighbor_outsideL2() {
		iterator.setBoundaryToIterate(l2);
		stepIteratorToMiddleOfBoundary(iterator, l2);
		iterator.currentNeighbor(2, -2);
	}
	/**
	 * ...and outside the upper boundary along dimension 0.
	 */
	@Test (expected=AssertionError.class)
	public final void testCurrentNeighbor_outsideU0() {
		iterator.setBoundaryToIterate(u0);
		stepIteratorToMiddleOfBoundary(iterator, u0);
		iterator.currentNeighbor(0, 2);
	}
	
	/**
	 * Verify that currentValue returns the value of the element currently
	 * pointed at. The verification is done for every element on every
	 * boundary.
	 *
	 * Note that this method assumes that FIELD_ORDER = 3.
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
						assertEquals(values[index], iterator.currentValue(), 4*Math.ulp(values[index]));
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
	 * verification is done for every neighbor that is inside the field.
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
		iterator.setCurrentNeighbor(2, -1, 73);
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
		iterator.setCurrentNeighbor(0, 1, 73);
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

	@Test
	public final void testSize() {
		super.testSize(iterator);
	}


	/*** Helper methods ***/

	/**
	 * Move the iterator to the mid point of the specified boundary and
	 * check the value of each neighbor that is inside the field.
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
			// Neighbor below
			if (d != boundary.getDimension() || !boundary.isLowerSide()) {
				neighborIndex[d]--;
				double expectedValue = values[linearIndexOf(neighborIndex)];
				assertEquals(expectedValue, iterator.currentNeighbor(d, -1), 4*Math.ulp(expectedValue));
				neighborIndex[d]++;
			}
			// Neighbor above
			if (d != boundary.getDimension() || boundary.isLowerSide()) {
				neighborIndex[d]++;
				double expectedValue = values[linearIndexOf(neighborIndex)];
				assertEquals(expectedValue, iterator.currentNeighbor(d, 1), 4*Math.ulp(expectedValue));
				neighborIndex[d]--;
			}
			// Neighbor further away
			if (d == boundary.getDimension()) {
				if (boundary.isLowerSide()) {
					neighborIndex[d]+=2;
					double expectedValue = values[linearIndexOf(neighborIndex)];
					assertEquals(expectedValue, iterator.currentNeighbor(d, 2), 4*Math.ulp(expectedValue));
					neighborIndex[d]-=2;
				} else {
					neighborIndex[d]-=2;
					double expectedValue = values[linearIndexOf(neighborIndex)];
					assertEquals(expectedValue, iterator.currentNeighbor(d, -2), 4*Math.ulp(expectedValue));
					neighborIndex[d]+=2;
				}
			}
		}
	}

	/**
	 * Move the iterator to the mid point of the specified boundary, change
	 * the value of each neighbor that is inside the field and verify that
	 * the values were changed correctly.
	 *
	 * @param boundary Boundary on which currentNeigbor will be verified.
	 */
	protected final void verifySetCurrentNeighbor(final BoundaryId boundary) {
		stepIteratorToMiddleOfBoundary(iterator, boundary);
		// Change the value of each neighbor that is inside the field
		for (int d=0; d<FIELD_ORDER; d++) {
			if (d != boundary.getDimension() || !boundary.isLowerSide()) {
				iterator.setCurrentNeighbor(d, -1, 10*boundary.getDimension() + d + 0.3);
			}
			if (d != boundary.getDimension() || boundary.isLowerSide()) {
				iterator.setCurrentNeighbor(d, 1, 10*boundary.getDimension() + d + 0.4);
			}
		}
		// ...and check the values
		for (int d=0; d<FIELD_ORDER; d++) {
			if (d != boundary.getDimension() || !boundary.isLowerSide()) {
				double expcectedValue = 10*boundary.getDimension() + d + 0.3;
				assertEquals(expcectedValue, iterator.currentNeighbor(d, -1), 4*Math.ulp(expcectedValue));
			}
			if (d != boundary.getDimension() || boundary.isLowerSide()) {
				double expectedValue = 10*boundary.getDimension() + d + 0.4;
				assertEquals(expectedValue, iterator.currentNeighbor(d, 1), 4*Math.ulp(expectedValue));
			}
		}
	}
}
