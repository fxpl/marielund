package haparanda.iterators;

import static org.junit.Assert.*;
import org.junit.*;

import haparanda.utils.BoundaryId;

/**
 * Unit test for ValueFieldIterator.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017
 */
public class ValueFieldIteratorTest  extends FieldIteratorTest
{
	private double[] values;
	protected FieldIterator iterator;
	
	@Before
	public void setUp() {
		super.setUp();
		values = new double[totalSize];
		for (int i=0; i<totalSize; i++) {
			values[i] = 1.43 * i;
		}
		iterator = new ValueFieldIterator(sizes, values, 0);
	}


	/**
	 * Verify that currentIndex returns the index along the dimension given
	 * as argument. The verification is done for all points and all 3
	 * dimensions. (Thereby, this method also verifies that the whole field
	 * is stepped over.)
	 */
	@Test
	public final void testCurrentIndex() {
		super.testCurrentIndex(iterator);
	}

	/**
	 * Verify that an AssertionError is thrown if currentIndex is called when
	 * the iterator is outside the field.
	 */
	@Test (expected=AssertionError.class)
	public final void testCurrentIndex_outside() {
		stepIteratorOutsideField(iterator);
		iterator.currentIndex(0);
	}

	/**
	 * Verify that currentNeighbor returns the value of the requested
	 * neighbor (when it is inside the field).
	 */
	@Test
	public final void testCurrentNeighbor() {
		BoundaryId boundary = new BoundaryId();

		// Middle of each boundary (boundary starts at lower 0:th boundary!)
		for (int d=0; d<FIELD_ORDER; d++) {
			// Lower boundary
			iterator.first();
			verifyCurrentNeighbor(boundary);
			boundary.step();

			// Upper boundary
			verifyCurrentNeighbor(boundary);
			boundary.step();
		}

		// Middle of field
		iterator.first();
		stepIteratorToMiddleOfField(iterator);
		verifyCurrentNeighbor();
	}

	/**
	 * Verify that an AssertionError is thrown if we try to get the value of a
	 * neighbor located outside the field:
	 * outside the lower boundary along dimension 2...
	 */
	@Test (expected=AssertionError.class)
	public final void testCurrentNeighbor_outsideL2() {
		stepIteratorToMiddleOfBoundary(iterator, new BoundaryId(2, true));
		iterator.currentNeighbor(2, -1);
	}

	/**
	 * ...and outside the upper boundary along dimension 0.
	 */
	@Test (expected=AssertionError.class)
	public final void testCurrentNeighbor_outsideU0() {
		stepIteratorToMiddleOfBoundary(iterator, new BoundaryId(0, false));
		iterator.currentNeighbor(0, 1);
	}

	/**
	 * Verify that currentValue returns the value of the element currently
	 * pointed at.
	 */
	@Test
	public final void testCurrentValue() {
		iterator.first();
		for (int i=0; i<totalSize; i++) {
			assertEquals(values[i], iterator.currentValue(), 4*Math.ulp(values[i]));
			iterator.next();
		}
	}

	/**
	 * Verify that an AssertionError is thrown if currentValue is called when
	 * the iterator is outside the field.
	 */
	@Test (expected=AssertionError.class)
	public final void testCurrentValue_outside() {
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
	 * setCurrentNeighbor changes the value of the requested neighbor. This
	 * is checked for every value that is inside the field.
	 */
	@Test
	public final void testSetCurrentNeighbor() {
		iterator.first();

		// Middle of lower boundaries
		for (int d=FIELD_ORDER-1; d>=0; d--) {
			verifySetCurrentNeighbor(new BoundaryId(d, true));
		}

		// Middle of field
		stepIteratorToMiddleOfField(iterator);
		verifySetCurrentNeighbor();

		// Middle of upper boundaries
		for (int d=0; d<FIELD_ORDER; d++) {
			verifySetCurrentNeighbor(new BoundaryId(d, false));
		}
	}

	/**
	 * Verify that an AssertionError is thrown if we try to set the value of a
	 * neighbor located outside the field:
	 * outside the lower boundary along dimension 2...
	 */
	@Test (expected=AssertionError.class)
	public final void testSetCurrentNeighbor_outsideL2() {
		stepIteratorToMiddleOfBoundary(iterator, new BoundaryId(2, true));
		iterator.setCurrentNeighbor(2, -1, 73);
	}

	/**
	 * ...and outside the upper boundary along dimension 0.
	 */
	@Test (expected=AssertionError.class)
	public final void testSetCurrentNeighbor_outsideU0() {
		stepIteratorToMiddleOfBoundary(iterator, new BoundaryId(0, false));
		iterator.setCurrentNeighbor(0, 1, 73);
	}

	/**
	 * Verify that setValue changes the value of the element currently
	 * pointed at.
	 */
	@Test
	public final void testSetCurrentValue() {
		// Change the values
		iterator.first();
		for (int i=0; i<totalSize; i++) {
			iterator.setCurrentValue(93.5*i);
			iterator.next();
		}

		// Verify that they were changed
		iterator.first();
		for (int i=0; i<totalSize; i++) {
			double expectedValue = 93.5*i;
			assertEquals(expectedValue, iterator.currentValue(), 4*Math.ulp(expectedValue));
			iterator.next();
		}
	}

	/**
	 * Verify that an AssertionError is thrown if setCurrentValue is called when
	 * the iterator is outside the field.
	 */
	@Test (expected=AssertionError.class)
	public final void testSetCurrentValue_outside() {
		stepIteratorOutsideField(iterator);
		iterator.setCurrentValue(783.0);
	}

	@Test
	public final void testSize() {
		super.testSize(iterator);
	}


	/*** Helper methods ***/

	/**
	 * Check the value of each neighbor or the element currently pointed at.
	 * This method requires that iterator does not point at a boundary
	 * element.
	 */
	protected final void verifyCurrentNeighbor() {
		int[] neighborIndex ={
				iterator.currentIndex(0),
				iterator.currentIndex(1),
				iterator.currentIndex(2)};
		for (int d=0; d<FIELD_ORDER; d++) {
				neighborIndex[d]--;
				double expectedValue = values[linearIndexOf(neighborIndex)];
				assertEquals(expectedValue, iterator.currentNeighbor(d, -1), 4*Math.ulp(expectedValue));
				neighborIndex[d]++;
				neighborIndex[d]++;
				expectedValue = values[linearIndexOf(neighborIndex)];
				assertEquals(expectedValue, iterator.currentNeighbor(d, 1), 4*Math.ulp(expectedValue));
				neighborIndex[d]--;
		}
	}

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
	 * Change the value of each neighbor and verify that the value was
	 * changed. This method requires that iterator does not point at a
	 * boundary element.
	 */
	protected final void verifySetCurrentNeighbor() {
		// Change the value of each neighbor
		for (int d=0; d<FIELD_ORDER; d++) {
			iterator.setCurrentNeighbor(d, -1, d + 5.1);
			iterator.setCurrentNeighbor(d, 1, d + 5.2);
		}
		// ...and check the values
		for (int d=0; d<FIELD_ORDER; d++) {
			double expectedValue = d + 5.1;
			assertEquals(expectedValue, iterator.currentNeighbor(d, -1), 4*Math.ulp(expectedValue));
			expectedValue = d + 5.2;
			assertEquals(expectedValue, iterator.currentNeighbor(d, 1), 4*Math.ulp(expectedValue));
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
				iterator.setCurrentNeighbor(d, -1, 10*boundary.getDimension() + d + 0.1);
			}
			if (d != boundary.getDimension() || boundary.isLowerSide()) {
				iterator.setCurrentNeighbor(d, 1, 10*boundary.getDimension() + d + 0.2);
			}
		}
		// ...and check the values
		for (int d=0; d<FIELD_ORDER; d++) {
			if (d != boundary.getDimension() || !boundary.isLowerSide()) {
				double expectedValue = 10*boundary.getDimension() + d + 0.1;
				assertEquals(expectedValue, iterator.currentNeighbor(d, -1), 4*Math.ulp(expectedValue));
			}
			if (d != boundary.getDimension() || boundary.isLowerSide()) {
				double expectedValue = 10*boundary.getDimension() + d + 0.2; 
				assertEquals(expectedValue, iterator.currentNeighbor(d, 1), 4*Math.ulp(expectedValue));
			}
		}
	}
}
