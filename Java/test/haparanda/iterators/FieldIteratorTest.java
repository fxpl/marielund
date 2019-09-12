package haparanda.iterators;

import haparanda.utils.BoundaryId;
import static org.junit.Assert.*;

/**
 * Base class of unit tests for concrete FieldIterator classes.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017
 */
public class FieldIteratorTest
{
	protected final static int FIELD_ORDER = Integer.getInteger("DIM");
	protected int[] sizes;
	protected int totalSize;
	protected int[] strides;

	/**
	 * Set up the test for a region of size 3x4x5 elements.
	 */
	protected void setUp() {
		assertEquals(3, FIELD_ORDER);
		int[] sizes = new int[FIELD_ORDER];
		sizes[0] = 3; sizes[1] = 4; sizes[2] = 5;
		setUp(sizes);
	}

	/**
	 * @param field_order: Order/dimensionality of the field to public be iterated; length of sizes
	 * @param sizes: The size (in each dimension) of the field to be iterated
	 */
	protected void setUp(final int[] sizes) {
		this.sizes = new int[FIELD_ORDER];
		this.sizes[0] = sizes[0];
		strides = new int[FIELD_ORDER];
		strides[0] = 1;
		totalSize = sizes[0];
		for (int i=1; i<FIELD_ORDER; i++) {
			this.sizes[i] = sizes[i];
			totalSize = totalSize * sizes[i];
			strides[i] = strides[i-1] * sizes[i-1];
		}
	}

	/**
	 * Check if an iterator points at a given index.
	 *
	 * @param iterator Iterator to check
	 * @param index Index which we are looking for
	 * @return true if iterator points at index, false otherwise
	 */
	protected final boolean iteratorAtIndex(final FieldIterator iterator, final int[] index) {
		for (int i=0; i<FIELD_ORDER; i++) {
			if (iterator.currentIndex(i) != index[i]) {
				return false;
			}
		}
		return true;
	}

	/**
	 * Calculate the linear index of the specified element.
	 *
	 * @param index Element index in each dimension
	 * @return The linear index of the specified set of indices
	 */
	protected final int linearIndexOf(final int[] index) {
		return  index[0]*strides[0] + index[1]*strides[1] + index[2]*strides[2];
	}

	/**
	 * Move an iterator forwards until it is outside the field.
	 *
	 * @param iterator Iterator to advance
	 */
	protected final void stepIteratorOutsideField(FieldIterator iterator) {
		while (iterator.isInField()) {
			iterator.next();
		}
	}

	/**
	 * Move an iterator forwards until it is reaches the mid point of the
	 * specified boundary. setBoundaryToIterate does not need to be called
	 * prior to calling this method.
	 *
	 * @param iterator Iterator to advance
	 * @param boundary Id of the boundary which will be stepped to
	 * @throws runtime_error if the iterator has already passed the requested point when the method is called
	 */
	protected final void stepIteratorToMiddleOfBoundary(BoundaryIterator iterator, final BoundaryId boundary) {
		iterator.setBoundaryToIterate(boundary);
		stepIteratorToMiddleOfBoundary((FieldIterator)iterator, boundary);
	}

	/**
	 * Move an iterator forwards until it is reaches the mid point of the
	 * specified boundary.
	 *
	 * @param iterator Iterator to advance
	 * @param boundary Id of the boundary which will be stepped to
	 * @throws runtime_error if the iterator has already passed the requested point when the method is called
	 */
	protected final void stepIteratorToMiddleOfBoundary(FieldIterator iterator, final BoundaryId boundary) {
		int[] requestedIndex = new int[FIELD_ORDER];
		for (int d=0; d<FIELD_ORDER; d++) {
			if (boundary.getDimension() != d) {
				requestedIndex[d] = sizes[d]/2;
			} else {
				if (boundary.isLowerSide()) {
					requestedIndex[d] = 0;
				} else {
					requestedIndex[d] = sizes[d]-1;
				}
			}
		}
	
		while (!iteratorAtIndex(iterator, requestedIndex)) {
			if (!iterator.isInField()) {
				throw new RuntimeException("Iterator has stepped through requested index.");
			}
			iterator.next();
		}
	}

	/**
	 * Move an iterator forwards until it is reaches the mid point of the
	 * field.
	 *
	 * @param iterator Iterator to advance
	 * @throws runtime_error if the iterator has already passed the requested point when the method is called
	 */
	protected final void stepIteratorToMiddleOfField(FieldIterator iterator) {
		int[] requestedIndex = new int[FIELD_ORDER];
		for (int d=0; d<FIELD_ORDER; d++) {
			requestedIndex[d] = sizes[d]/2;
		}
	
		while (!iteratorAtIndex(iterator, requestedIndex)) {
			if (!iterator.isInField()) {
				throw new RuntimeException("Iterator has stepped through requested index.");
			}
			iterator.next();
		}
	}

	/**
	 * Test method for iterators which traverse the boundaries of a field.
	 * For every element on each boundary, verify that currentIndex returns
	 * the index along the dimension given as argument. The verification is
	 * done for all 3 dimensions.
	 *
	 * Note that this method assumes that field_order = 3.
	 *
	 * @param iterator Iterator for which the verification is done
	 */
	protected final void testCurrentIndex(BoundaryIterator iterator) {
		assert(3 == FIELD_ORDER);
		for (int d=0; d<FIELD_ORDER; d++) {
			int otherDim1 = 0==d ? 1 : 0;
			int otherDim2 = 2==d ? 1 : 2;
			for (int side=0; side<2; side++) {
				iterator.setBoundaryToIterate(new BoundaryId(d, 0==side));
				for (int i2=0; i2<sizes[otherDim2]; i2++) {
					for (int i1=0; i1<sizes[otherDim1]; i1++) {
						assertEquals(side*(sizes[d]-1), iterator.currentIndex((int)d));
						assertEquals(i1, iterator.currentIndex(otherDim1));
						assertEquals(i2, iterator.currentIndex(otherDim2));
						iterator.next();
					}
				}
			}
		}
	}

	/**
	 * Test method for iterators which traverse the whole field. For every
	 * element in the field, verify that currentIndex returns the index
	 * along the dimension given as argument. The verification is done for
	 * all 3 dimensions.
	 *
	 * @param iterator Iterator for which the verification is done
	 */
	protected final void testCurrentIndex(FieldIterator iterator) {
		iterator.first();
		for (int i2 = 0; i2 < sizes[2]; i2++) {
			for (int i1 = 0; i1 < sizes[1]; i1++) {
				for (int i0 = 0; i0 < sizes[0]; i0++) {
					assertEquals(i0, iterator.currentIndex(0));
					assertEquals(i1, iterator.currentIndex(1));
					assertEquals(i2, iterator.currentIndex(2));
					iterator.next();
				}
			}
		}
	}


	/**
	 * For each boundary, verify that first makes an iterator point at the
	 * first element of the boundary.
	 *
	 * @param iterator Iterator to check
	 */
	protected final void testFirst(BoundaryIterator iterator) {
		for (int d=0; d<FIELD_ORDER; d++) {
			for (int side=0; side<2; side++) {
				iterator.setBoundaryToIterate(new BoundaryId(d, 0==side));
				iterator.first();
				for(int i=0; i<FIELD_ORDER; i++) {
					/* All indices are 0 on the lower boundaries. The index is
					 * also 0 in dimensions != d */
					if (1 == side && i == d) {
						/* Index in dimension d obtains its max value on the
						   upper boundary along dimension d. */
						assertEquals(sizes[d]-1, iterator.currentIndex(i));
					} else {
						/* Index = 0 in all other dimensions, and all indices
						   are 0 on lower boundaries. */
						assertEquals((int)0, iterator.currentIndex(i));
					}
				}
			}
		}
	}

	/**
	 * Verify that first makes an iterator point at the first element of
	 * the field.
	 *
	 * @param iterator Iterator to check
	 */
	protected final void testFirst(FieldIterator iterator) {
		iterator.first();
		for(int i=0; i<FIELD_ORDER; i++) {
			assertEquals((int)0, iterator.currentIndex(i));
		}
	}

	/**
	 * Test method for iterators which traverse the boundaries: For each
	 * boundary, verify that isInField returns true as long as the iterator
	 * has not stepped outside the boundary, and thereafter false.
	 *
	 * @param iterator Iterator for which the verification is done
	 */
	protected final void testIsInField(BoundaryIterator iterator) {
		for (int d=0; d<FIELD_ORDER; d++) {
			for (int side=0; side<2; side++) {
				iterator.setBoundaryToIterate(new BoundaryId(d, 0==side));
				int boundarySize = totalSize/sizes[d];
				for (int i = 0; i < boundarySize; i++) {
					assertTrue(iterator.isInField());
					iterator.next();
				}
				assertFalse(iterator.isInField());
			}
		}
	}

	/**
	 * Test method for iterators which traverse the whole field: Verify
	 * that isInField returns true as long as the iterator has not stepped
	 * outside the field, and thereafter false.
	 *
	 * @param iterator Iterator for which the verification is done
	 */
	protected final void testIsInField(FieldIterator iterator) {
		iterator.first();
		for (int i = 0; i < totalSize; i++) {
			assertTrue(iterator.isInField());
			iterator.next();
		}
		assertFalse(iterator.isInField());
	}

	/**
	 * Verify that size returns the size of the field, in each dimension.
	 *
	 * @param iterator Iterator which iterates over the field
	 */
	protected final void testSize(final FieldIterator iterator) {
		assertEquals(sizes[0], iterator.size(0));
		assertEquals(sizes[1], iterator.size(1));
		assertEquals(sizes[2], iterator.size(2));
	}
}
