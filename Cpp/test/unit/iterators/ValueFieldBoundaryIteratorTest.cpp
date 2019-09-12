#include "src/iterators/ValueFieldBoundaryIterator.hpp"
#include "FieldIteratorTest.hpp"

namespace Haparanda {
namespace Iterators {

	/**
	 * Unit test for ValueFieldBoundaryIterator.
	 *
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2014, 2016
	 */
	class ValueFieldBoundaryIteratorTest : public FieldIteratorTest
	{
	public:
		virtual void SetUp() {
			FieldIteratorTest::SetUp();
			values = new double[totalSize];
			for (std::size_t i=0; i<totalSize; i++) {
				values[i] = 7.87 * i;
			}
			iterator = new ValueFieldBoundaryIterator<FIELD_ORDER>(sizes, values);
		}

		virtual void TearDown() {
			FieldIteratorTest::TearDown();
			delete []values;
			delete iterator;
		}


	protected:
		BoundaryIterator<FIELD_ORDER> *iterator;


		/*** Test methods ***/

		/**
		 * Verify that currentIndex returns the index along the dimension given
		 * as argument. The verification is done for all points on all
		 * boundaries and all 3 dimensions. (Thereby, this method also verifies
		 * that setBoundaryToIterate makes the iterator traverse the correct
		 * boundary.)
		 */
		void testCurrentIndex() {
			FieldIteratorTest::testCurrentIndex(iterator);
		}

		/**
		 * Verify that currentNeighbor returns the value of the requested
		 * neighbor (when it is inside the field). The verification is done for
		 * the midpoint of each boundary.
		 */
		void testCurrentNeighbor() {
			for (std::size_t d=0; d<FIELD_ORDER; d++) {
				for (std::size_t side=0; side<2; side++) {
					verifyCurrentNeighbor(BoundaryId(d, 0==side));
				}
			}
		}

		/**
		 * Verify that currentValue returns the value of the element currently
		 * pointed at. The verification is done for every element on every
		 * boundary.
		 *
		 * Note that this method assumes that FIELD_ORDER = 3.
		 */
		void testCurrentValue() {
			assert(3 == FIELD_ORDER);
			for (std::size_t d=0; d<FIELD_ORDER; d++) {
				std::size_t otherDim1 = 0==d ? 1 : 0;
				std::size_t otherDim2 = 2==d ? 1 : 2;
				for (int side=0; side<2; side++) {
					iterator->setBoundaryToIterate(BoundaryId(d, 0==side));
					for (std::size_t i2=0; i2<sizes[otherDim2]; i2++) {
						for (std::size_t i1=0; i1<sizes[otherDim1]; i1++) {
							std::size_t index =
								side * (sizes[d] -1) * strides[d]
								+ i1 * strides[otherDim1]
								+ i2 * strides[otherDim2];
							expect_equal(values[index], iterator->currentValue());
							iterator->next();
						}
					}
				}
			}
		}

		void testFirst() {
			FieldIteratorTest::testFirst(iterator);
		}

		void testIsInField() {
			FieldIteratorTest::testIsInField(iterator);
		}

		/**
		 * For the element in the middle of each boundary: Verify that
		 * setCurrentNeighbor changes the value of the requested neighbor. The
		 * verification is done for every neighbor that is inside the field.
		 */
		void testSetCurrentNeighbor() {
			for (std::size_t d=0; d<FIELD_ORDER; d++) {
				for (std::size_t side=0; side<2; side++) {
					verifySetCurrentNeighbor(BoundaryId(d, 0==side));
				}
			}
		}

		/**
		 * Verify that setValue changes the value of the element currently
		 * pointed at. The verification is done for the first element on each
		 * boundary.
		 */
		void testSetCurrentValue() {
			for (std::size_t d=0; d<FIELD_ORDER; d++) {
				for (std::size_t side=0; side<2; side++) {
					std::size_t boundarySize = totalSize/sizes[d];
					iterator->setBoundaryToIterate(BoundaryId(d, 0==side));
					// Change the values
					for (std::size_t i=0; i<boundarySize; i++) {
						iterator->setCurrentValue(3.2*i);
						iterator->next();
					}
					// Verify that they were changed
					iterator->first();
					for (std::size_t i=0; i<boundarySize; i++) {
						double expectedValue = 3.2*i;
						expect_equal(expectedValue, iterator->currentValue());
						iterator->next();
					}
				}
			}
		}

		void testSize() {
			FieldIteratorTest::testSize(*iterator);
		}


		/*** Helper methods ***/

		/**
		 * Move the iterator to the mid point of the specified boundary and
		 * check the value of each neighbor that is inside the field.
		 *
		 * @param boundary Boundary on which currentNeigbor will be verified.
		 */
		void verifyCurrentNeighbor(const BoundaryId& boundary) {
			stepIteratorToMiddleOfBoundary(iterator, boundary);
			std::array<std::size_t, FIELD_ORDER> neighborIndex = {
					iterator->currentIndex(0),
					iterator->currentIndex(1),
					iterator->currentIndex(2)
			};
			for (std::size_t d=0; d<FIELD_ORDER; d++) {
				// Neighbor below
				if (d != boundary.getDimension() || !boundary.isLowerSide()) {
					neighborIndex[d]--;
					expect_equal(values[linearIndexOf(neighborIndex)], iterator->currentNeighbor(d, -1));
					neighborIndex[d]++;
				}
				// Neighbor above
				if (d != boundary.getDimension() || boundary.isLowerSide()) {
					neighborIndex[d]++;
					expect_equal(values[linearIndexOf(neighborIndex)], iterator->currentNeighbor(d, 1));
					neighborIndex[d]--;
				}
				// Neighbor further away
				if (d == boundary.getDimension()) {
					if (boundary.isLowerSide()) {
						neighborIndex[d]+=2;
						expect_equal(values[linearIndexOf(neighborIndex)], iterator->currentNeighbor(d, 2));
						neighborIndex[d]-=2;
					} else {
						neighborIndex[d]-=2;
						expect_equal(values[linearIndexOf(neighborIndex)], iterator->currentNeighbor(d, -2));
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
		void verifySetCurrentNeighbor(const BoundaryId& boundary) {
			stepIteratorToMiddleOfBoundary(iterator, boundary);
			// Change the value of each neighbor that is inside the field
			for (std::size_t d=0; d<FIELD_ORDER; d++) {
				if (d != boundary.getDimension() || !boundary.isLowerSide()) {
					iterator->setCurrentNeighbor(d, -1, 10*boundary.getDimension() + d + 0.3);
				}
				if (d != boundary.getDimension() || boundary.isLowerSide()) {
					iterator->setCurrentNeighbor(d, 1, 10*boundary.getDimension() + d + 0.4);
				}
			}
			// ...and check the values
			for (std::size_t d=0; d<FIELD_ORDER; d++) {
				if (d != boundary.getDimension() || !boundary.isLowerSide()) {
					expect_equal(10*boundary.getDimension() + d + 0.3, iterator->currentNeighbor(d, -1));
				}
				if (d != boundary.getDimension() || boundary.isLowerSide()) {
					expect_equal(10*boundary.getDimension() + d + 0.4, iterator->currentNeighbor(d, 1));
				}
			}
		}


	private:
		double *values;
	};


	/**
	 * Unit test that aborts for ValueFieldBoundaryIterator.
	 *
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2014
	 */
	class ValueFieldBoundaryIteratorDeathTest : public ValueFieldBoundaryIteratorTest
	{
	public:
		virtual void SetUp() {
			ValueFieldBoundaryIteratorTest::SetUp();
			::testing::FLAGS_gtest_death_test_style = "threadsafe";
		}


	protected:
		void testCurrentIndex() {
			for (std::size_t d=0; d<FIELD_ORDER; d++) {
				for (std::size_t side=0; side<2; side++) {
					iterator->setBoundaryToIterate(BoundaryId(d, 0==side));
					FieldIteratorTest::testCurrentIndex_outside(iterator);
				}
			}
		}

		void testCurrentNeighbor() {
			FieldIteratorTest::testCurrentNeighbor_outside(iterator);
		}

		void testCurrentValue() {
			for (std::size_t d=0; d<FIELD_ORDER; d++) {
				for (std::size_t side=0; side<2; side++) {
					iterator->setBoundaryToIterate(BoundaryId(d, 0==side));
					FieldIteratorTest::testCurrentValue_outside(iterator);
				}
			}
		}

		void testSetCurrentNeighbor() {
			FieldIteratorTest::testSetCurrentNeighbor_outside(iterator);
		}

		void testSetCurrentValue() {
			for (std::size_t d=0; d<FIELD_ORDER; d++) {
				for (std::size_t side=0; side<2; side++) {
					iterator->setBoundaryToIterate(BoundaryId(d, 0==side));
					FieldIteratorTest::testSetCurrentValue_outside(iterator);
				}
			}
		}
	};


	/**
	 * Verify the behavior of the basic methods of a
	 * ValueFieldBoundaryIterator.
	 */
	TEST_F(ValueFieldBoundaryIteratorTest, TestBasics) {
		// setBoundaryToIterate is implicitly tested by testFirst and testCurrentIndex.
		testSize();
		testIsInField();
	}

	/**
	 * Verify the behavior of the getter methods of a
	 * ValueFieldBoundaryIterator.
	 */
	TEST_F(ValueFieldBoundaryIteratorTest, TestGetters) {
		testCurrentIndex();
		testCurrentValue();
		testCurrentNeighbor();
	}

	/**
	 * Verify the behavior of the methods which moves a
	 * ValueFieldBoundaryIterator.
	 */
	TEST_F(ValueFieldBoundaryIteratorTest, TestIteration) {
		testFirst();
		// next is implicitly tested by the other tests.
	}

	/**
	 * Verify the behavior of the setters of a ValueFieldBoundaryIterator.
	 */
	TEST_F(ValueFieldBoundaryIteratorTest, TestSetters) {
		testSetCurrentValue();
		testSetCurrentNeighbor();
	}


#ifndef MPI_LIB
	/**
	 * Verify the behavior of the getter methods of a
	 * ValueFieldBoundaryIterator, when the iterator is outside the field.
	 */
	TEST_F(ValueFieldBoundaryIteratorDeathTest, TestGetters) {
		testCurrentIndex();
		testCurrentValue();
		testCurrentNeighbor();
	}

	/**
	 * Verify the behavior of the setter methods of a
	 * ValueFieldBoundaryIterator, when the iterator is outside the field.
	 */
	TEST_F(ValueFieldBoundaryIteratorDeathTest, TestSetters) {
		testSetCurrentValue();
		testSetCurrentNeighbor();
	}
#endif
}
}

