#include "src/iterators/FieldIterator.hpp"
#include "test/HaparandaTest.hpp"

#define FIELD_ORDER 3

namespace Haparanda {
namespace Iterators {

	/**
	 * Base class of unit tests for concrete FieldIterator classes.
	 *
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2014, 2016, 2017
	 */
	class FieldIteratorTest : public HaparandaTest
	{
	public:

		/**
		 * Set up the test for a region of size 3x4x5 elements.
		 */
		virtual void SetUp();

		/**
		 * @param sizes: The size (in each dimension) of the field to be iterated
		 */
		void SetUp(std::array<std::size_t, FIELD_ORDER> sizes);


	protected:
		std::array<std::size_t, FIELD_ORDER> sizes;
		std::size_t totalSize;
		std::array<std::size_t, FIELD_ORDER> strides;

		/**
		 * Check if an iterator points at a given index.
		 *
		 * @param iterator Iterator to check
		 * @param index Index which we are looking for
		 * @return true if iterator points at index, false otherwise
		 */
		template<class IteratorType>
		inline bool iteratorAtIndex(const IteratorType& iterator, const std::array<std::size_t, FIELD_ORDER>& index);

		/**
		 * Calculate the linear index of the specified element.
		 *
		 * @param index Element index in each dimension
		 * @return The linear index of the specified set of indices
		 */
		std::size_t linearIndexOf(const std::array<std::size_t, FIELD_ORDER>& index);

		/**
		 * Move an iterator forwards until it is outside the field.
		 *
		 * @param iterator Iterator to advance
		 */
		template<class IteratorType>
		void stepIteratorOutsideField(IteratorType *iterator);

		/**
		 * Move an iterator forwards until it is reaches the mid point of the
		 * specified boundary. setBoundaryToIterate does not need to be called
		 * prior to calling this method.
		 *
		 * @param iterator Iterator to advance
		 * @param boundary Id of the boundary which will be stepped to
		 * @throws runtime_error if the iterator has already passed the requested point when the method is called
		 */
		template<class IteratorType>
		void stepIteratorToMiddleOfBoundary_boundary(IteratorType *iterator, const BoundaryId& boundary);

		/**
		 * Move an iterator forwards until it is reaches the mid point of the
		 * specified boundary.
		 *
		 * @param iterator Iterator to advance
		 * @param boundary Id of the boundary which will be stepped to
		 * @throws runtime_error if the iterator has already passed the requested point when the method is called
		 */
		template<class IteratorType>
		void stepIteratorToMiddleOfBoundary(IteratorType *iterator, const BoundaryId& boundary);

		/**
		 * Move an iterator forwards until it is reaches the mid point of the
		 * field.
		 *
		 * @param iterator Iterator to advance
		 * @throws runtime_error if the iterator has already passed the requested point when the method is called
		 */
		template<class IteratorType>
		void stepIteratorToMiddleOfField(IteratorType *iterator);

		/**
		 * Test method for iterators which traverse the boundaries of a field.
		 * For every element on each boundary, verify that currentIndex returns
		 * the index along the dimension given as argument. The verification is
		 * done for all 3 dimensions.
		 *
		 * Note that this method assumes that ORDER = 3.
		 *
		 * @param iterator Iterator for which the verification is done
		 */
		template<class IteratorType>
		void testCurrentIndex_boundary(IteratorType *iterator);

		/**
		 * Test method for iterators which traverse the whole field. For every
		 * element in the field, verify that currentIndex returns the index
		 * along the dimension given as argument. The verification is done for
		 * all 3 dimensions.
		 *
		 * @param iterator Iterator for which the verification is done
		 */
		template<class IteratorType>
		void testCurrentIndex(IteratorType *iterator);

		/**
		 * Verify that currentIndex causes the program to abort if it is run in
		 * debug mode and the method is called when the iterator is outside the
		 * field.
		 *
		 * @param iterator Iterator to check
		 */
		template<class IteratorType>
		void testCurrentIndex_outside(IteratorType *iterator);

		/**
		 * For the element in the middle of each boundary: Verify that
		 * requesting the value of the closest neighbor outside the field causes
		 * the program to abort if it is run in debug mode.
		 *
		 * @param iterator Iterator to check
		 */
		template<class IteratorType>
		void testCurrentNeighbor_outside_boundary(IteratorType *iterator);
		template<class IteratorType>
		void testCurrentNeighbor_outside(IteratorType *iterator);

		/**
		 * Verify that currentValue causes the program to abort if it is run in
		 * debug mode and the method is called when the iterator is outside the
		 * field.
		 *
		 * @param iterator Iterator to check
		 */
		template<class IteratorType>
		void testCurrentValue_outside(IteratorType *iterator);

		/**
		 * For each boundary, verify that first makes an iterator point at the
		 * first element of the boundary.
		 *
		 * @param iterator Iterator to check
		 */
		template<class IteratorType>
		void testFirst_boundary(IteratorType *iterator);

		/**
		 * Verify that first makes an iterator point at the first element of
		 * the field.
		 *
		 * @param iterator Iterator to check
		 */
		template<class IteratorType>
		void testFirst(IteratorType *iterator);

		/**
		 * Test method for iterators which traverse the boundaries: For each
		 * boundary, verify that isInField returns true as long as the iterator
		 * has not stepped outside the boundary, and thereafter false.
		 *
		 * @param iterator Iterator for which the verification is done
		 */
		template<class IteratorType>
		void testIsInField_boundary(IteratorType *iterator);

		/**
		 * Test method for iterators which traverse the whole field: Verify
		 * that isInField returns true as long as the iterator has not stepped
		 * outside the field, and thereafter false.
		 *
		 * @param iterator Iterator for which the verification is done
		 */
		template<class IteratorType>
		void testIsInField(IteratorType *iterator);

		/**
		 * For the element in the middle of each boundary: Verify that
		 * trying to change the value of the closest neighbor outside the field
		 * causes the program to abort if it is run in debug mode.
		 *
		 * @param iterator Iterator for which the verification is done
		 */
		template<class IteratorType>
		void testSetCurrentNeighbor_outside_boundary(IteratorType *iterator);
		template<class IteratorType>
		void testSetCurrentNeighbor_outside(IteratorType *iterator);

		/**
		 * Verify that setCurrentValue causes the program to abort if it is run
		 * in debug mode and the method is called when the iterator is outside
		 * the field.
		 *
		 * @param iterator Iterator for which the verification is done
		 */
		template<class IteratorType>
		void testSetCurrentValue_outside(IteratorType *iterator);

		/**
		 * Verify that size returns the size of the field, in each dimension.
		 *
		 * @param iterator Iterator which iterates over the field
		 */
		template<class IteratorType>
		void testSize(const IteratorType& iterator);
	};

	/*** Public methods ***/

	void FieldIteratorTest::SetUp() {
		sizes = {3, 4, 5};
		SetUp(sizes);
	}

	void FieldIteratorTest::SetUp(std::array<std::size_t, FIELD_ORDER> sizes) {
		this->sizes = sizes;
		totalSize = sizes[0];
		strides[0] = 1;
		for (std::size_t i=1; i<FIELD_ORDER; i++) {
			totalSize = totalSize * sizes[i];
			strides[i] = strides[i-1] * sizes[i-1];
		}
	}

	/*** Protected methods ***/
	template<class IteratorType>
	inline bool FieldIteratorTest::iteratorAtIndex(const IteratorType& iterator, const std::array<std::size_t, FIELD_ORDER>& index) {
		for (std::size_t i=0; i<FIELD_ORDER; i++) {
			if (iterator.currentIndex(i) != index[i]) {
				return false;
			}
		}
		return true;
	}

	std::size_t FieldIteratorTest::linearIndexOf(const std::array<std::size_t, FIELD_ORDER>& index) {
		return  index[0]*strides[0] + index[1]*strides[1] + index[2]*strides[2];
	}

	template<class IteratorType>
	void FieldIteratorTest::stepIteratorOutsideField(IteratorType *iterator) {
		while (iterator->isInField()) {
			iterator->next();
		}
	}

	template<class IteratorType>
	void FieldIteratorTest::stepIteratorToMiddleOfBoundary_boundary(IteratorType *iterator, const BoundaryId& boundary) {
		iterator->setBoundaryToIterate(boundary);
		stepIteratorToMiddleOfBoundary(iterator, boundary);
	}

	template<class IteratorType>
	void FieldIteratorTest::stepIteratorToMiddleOfBoundary(IteratorType *iterator, const BoundaryId& boundary) {
		std::array<std::size_t, FIELD_ORDER> requestedIndex;
		for (std::size_t d=0; d<FIELD_ORDER; d++) {
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

		while (!iteratorAtIndex(*iterator, requestedIndex)) {
			if (!iterator->isInField()) {
				throw std::runtime_error("Iterator has stepped through requested index.");
			}
			iterator->next();
		}
	}

	template<class IteratorType>
	void FieldIteratorTest::stepIteratorToMiddleOfField(IteratorType *iterator) {
		std::array<std::size_t, FIELD_ORDER> requestedIndex;
		for (std::size_t d=0; d<FIELD_ORDER; d++) {
			requestedIndex[d] = sizes[d]/2;
		}

		while (!iteratorAtIndex(*iterator, requestedIndex)) {
			if (!iterator->isInField()) {
				throw std::runtime_error("Iterator has stepped through requested index.");
			}
			iterator->next();
		}
	}

	template<class IteratorType>
	void FieldIteratorTest::testCurrentIndex_boundary(IteratorType *iterator) {
		assert(3 == FIELD_ORDER);
		for (std::size_t d=0; d<FIELD_ORDER; d++) {
			std::size_t otherDim1 = 0==d ? 1 : 0;
			std::size_t otherDim2 = 2==d ? 1 : 2;
			for (std::size_t side=0; side<2; side++) {
				iterator->setBoundaryToIterate(BoundaryId(d, 0==side));
				for (std::size_t i2=0; i2<sizes[otherDim2]; i2++) {
					for (std::size_t i1=0; i1<sizes[otherDim1]; i1++) {
						EXPECT_EQ(side*(sizes[d]-1), iterator->currentIndex((std::size_t)d));
						EXPECT_EQ(i1, iterator->currentIndex((std::size_t)otherDim1));
						EXPECT_EQ(i2, iterator->currentIndex((std::size_t)otherDim2));
						iterator->next();
					}
				}
			}
		}
	}

	template<class IteratorType>
	void FieldIteratorTest::testCurrentIndex(IteratorType *iterator) {
		iterator->first();
		for (std::size_t i2 = 0; i2 < sizes[2]; i2++) {
			for (std::size_t i1 = 0; i1 < sizes[1]; i1++) {
				for (std::size_t i0 = 0; i0 < sizes[0]; i0++) {
					expect_equal(i0, iterator->currentIndex(0));
					expect_equal(i1, iterator->currentIndex(1));
					expect_equal(i2, iterator->currentIndex(2));
					iterator->next();
				}
			}
		}
	}

	template<class IteratorType>
	void FieldIteratorTest::testCurrentIndex_outside(IteratorType *iterator) {
		stepIteratorOutsideField(iterator);
		EXPECT_ASSERT_FAIL(iterator->currentIndex(0));
	}

	template<class IteratorType>
	void FieldIteratorTest::testCurrentNeighbor_outside_boundary(IteratorType *iterator) {
		for (std::size_t d=0; d<FIELD_ORDER; d++) {
			for (std::size_t side=0; side<2; side++) {
				stepIteratorToMiddleOfBoundary_boundary(iterator, BoundaryId(d, 0==side));
				int offset = 0==side ? -1 : 1;
				EXPECT_ASSERT_FAIL(iterator->currentNeighbor(d, offset));
			}
		}
	}

	template<class IteratorType>
	void FieldIteratorTest::testCurrentNeighbor_outside(IteratorType *iterator) {
		iterator->first();
		// Middle of lower boundaries
		for (int d=FIELD_ORDER-1; d>=0; d--) {
			stepIteratorToMiddleOfBoundary(iterator, BoundaryId(d, true));
			EXPECT_ASSERT_FAIL(iterator->currentNeighbor(d, -1));
		}
		// Middle of upper boundaries
		for (int d=0; d<FIELD_ORDER; d++) {
			stepIteratorToMiddleOfBoundary(iterator, BoundaryId(d, false));
			EXPECT_ASSERT_FAIL(iterator->currentNeighbor(d, 1));
		}
	}

	template<class IteratorType>
	void FieldIteratorTest::testCurrentValue_outside(IteratorType *iterator) {
		stepIteratorOutsideField(iterator);
		EXPECT_ASSERT_FAIL(iterator->currentValue());
	}

	template<class IteratorType>
	void FieldIteratorTest::testFirst_boundary(IteratorType *iterator) {
		for (std::size_t d=0; d<FIELD_ORDER; d++) {
			for (std::size_t side=0; side<2; side++) {
				iterator->setBoundaryToIterate(BoundaryId(d, 0==side));
				iterator->first();
				for(std::size_t i=0; i<FIELD_ORDER; i++) {
					/* All indices are 0 on the lower boundaries. The index is
					 * also 0 in dimensions != d */
					if (1 == side && i == d) {
						/* Index in dimension d obtains its max value on the
						   upper boundary along dimension d. */
						expect_equal(sizes[d]-1, iterator->currentIndex(i));
					} else {
						/* Index = 0 in all other dimensions, and all indices
						   are 0 on lower boundaries. */
						expect_equal((std::size_t)0, iterator->currentIndex(i));
					}
				}
			}
		}
	}

	template<class IteratorType>
	void FieldIteratorTest::testFirst(IteratorType *iterator) {
		iterator->first();
		for(std::size_t i=0; i<FIELD_ORDER; i++) {
			expect_equal((std::size_t)0, iterator->currentIndex(i));
		}
	}

	template<class IteratorType>
	void FieldIteratorTest::testIsInField_boundary(IteratorType *iterator) {
		for (std::size_t d=0; d<FIELD_ORDER; d++) {
			for (std::size_t side=0; side<2; side++) {
				iterator->setBoundaryToIterate(BoundaryId(d, 0==side));
				std::size_t boundarySize = totalSize/sizes[d];
				for (std::size_t i = 0; i < boundarySize; i++) {
					EXPECT_TRUE(iterator->isInField());
					iterator->next();
				}
				EXPECT_FALSE(iterator->isInField());
			}
		}
	}

	template<class IteratorType>
	void FieldIteratorTest::testIsInField(IteratorType *iterator) {
		iterator->first();
		for (std::size_t i = 0; i < totalSize; i++) {
			EXPECT_TRUE(iterator->isInField());
			iterator->next();
		}
		EXPECT_FALSE(iterator->isInField());
	}

	template<class IteratorType>
	void FieldIteratorTest::testSetCurrentNeighbor_outside_boundary(IteratorType *iterator) {
		for (std::size_t d=0; d<FIELD_ORDER; d++) {
			for (std::size_t side=0; side<2; side++) {
				stepIteratorToMiddleOfBoundary_boundary(iterator, BoundaryId(d, 0==side));
				int offset = 0==side ? -1 : 1;
				EXPECT_ASSERT_FAIL(iterator->setCurrentNeighbor(d, offset, 73.0));
			}
		}
	}

	template<class IteratorType>
	void FieldIteratorTest::testSetCurrentNeighbor_outside(IteratorType *iterator) {
		iterator->first();
		// Middle of lower boundaries
		for (int d=FIELD_ORDER-1; d>=0; d--) {
			stepIteratorToMiddleOfBoundary(iterator, BoundaryId(d, true));
			EXPECT_ASSERT_FAIL(iterator->setCurrentNeighbor(d, -1, 73.0));
		}
		// Middle of upper boundaries
		for (int d=0; d<FIELD_ORDER; d++) {
			stepIteratorToMiddleOfBoundary(iterator, BoundaryId(d, false));
			EXPECT_ASSERT_FAIL(iterator->setCurrentNeighbor(d, 1, 73.0));
		}
	}

	template<class IteratorType>
	void FieldIteratorTest::testSetCurrentValue_outside(IteratorType *iterator) {
		stepIteratorOutsideField(iterator);
		EXPECT_ASSERT_FAIL(iterator->setCurrentValue(783.0));
	}

	template<class IteratorType>
	void FieldIteratorTest::testSize(const IteratorType& iterator) {
		ASSERT_EQ(sizes[0], iterator.size(0));
		ASSERT_EQ(sizes[1], iterator.size(1));
		ASSERT_EQ(sizes[2], iterator.size(2));
	}

}
}
