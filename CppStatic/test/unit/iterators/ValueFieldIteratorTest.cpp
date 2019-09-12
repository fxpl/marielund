#include "src/iterators/ValueFieldIterator.hpp"
#include "FieldIteratorTest.hpp"

namespace Haparanda {
namespace Iterators {

	/**
	 * Unit test for ValueFieldIterator.
	 *
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2014, 2016, 2017
	 */
	class ValueFieldIteratorTest : public FieldIteratorTest
	{
	public:
		virtual void SetUp() {
			FieldIteratorTest::SetUp();
			values = new double[totalSize];
			for (std::size_t i=0; i<totalSize; i++) {
				values[i] = 1.43 * i;
			}
			iterator = new IteratorType(sizes, values);
		}

		virtual void TearDown() {
			FieldIteratorTest::TearDown();
			delete []values;
			delete iterator;
		}


	protected:
		typedef ValueFieldIterator<FIELD_ORDER> IteratorType;
		FieldIterator<IteratorType> *iterator;

		/*** Test methods ***/

		/**
		 * Verify that currentIndex returns the index along the dimension given
		 * as argument. The verification is done for all points and all 3
		 * dimensions. (Thereby, this method also verifies that the whole field
		 * is stepped over.)
		 */
		void testCurrentIndex() {
			FieldIteratorTest::testCurrentIndex(iterator);
		}

		/**
		 * Verify that currentNeighbor returns the value of the requested
		 * neighbor (when it is inside the field).
		 */
		void testCurrentNeighbor() {
			BoundaryId boundary;

			// Middle of each boundary (boundary starts at lower 0:th boundary!)
			for (int d=0; d<FIELD_ORDER; d++) {
				// Lower boundary
				iterator->first();
				verifyCurrentNeighbor(boundary);
				++boundary;

				// Upper boundary
				verifyCurrentNeighbor(boundary);
				++boundary;
			}

			// Middle of field
			iterator->first();
			stepIteratorToMiddleOfField(iterator);
			verifyCurrentNeighbor();
		}

		/**
		 * Verify that currentValue returns the value of the element currently
		 * pointed at.
		 */
		void testCurrentValue() {
			iterator->first();
			for (std::size_t i=0; i<totalSize; i++) {
				expect_equal(values[i], iterator->currentValue());
				iterator->next();
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
		 * setCurrentNeighbor changes the value of the requested neighbor. This
		 * is checked for every value that is inside the field.
		 */
		void testSetCurrentNeighbor() {
			iterator->first();

			// Middle of lower boundaries
			for (int d=FIELD_ORDER-1; d>=0; d--) {
				verifySetCurrentNeighbor(BoundaryId(d, true));
			}

			// Middle of field
			stepIteratorToMiddleOfField(iterator);
			verifySetCurrentNeighbor();

			// Middle of upper boundaries
			for (int d=0; d<FIELD_ORDER; d++) {
				verifySetCurrentNeighbor(BoundaryId(d, false));
			}
		}

		/**
		 * Verify that setValue changes the value of the element currently
		 * pointed at.
		 */
		void testSetCurrentValue() {
			// Change the values
			iterator->first();
			for (std::size_t i=0; i<totalSize; i++) {
				iterator->setCurrentValue(93.5*i);
				iterator->next();
			}

			// Verify that they were changed
			iterator->first();
			for (std::size_t i=0; i<totalSize; i++) {
				double expectedValue = 93.5*i;
				expect_equal(expectedValue, iterator->currentValue());
				iterator->next();
			}
		}

		void testSize() {
			FieldIteratorTest::testSize(*iterator);
		}


		/*** Helper methods ***/

		/**
		 * Check the value of each neighbor or the element currently pointed at.
		 * This method requires that iterator does not point at a boundary
		 * element.
		 */
		void verifyCurrentNeighbor() {
			std::array<std::size_t, FIELD_ORDER> neighborIndex ={
					iterator->currentIndex(0),
					iterator->currentIndex(1),
					iterator->currentIndex(2)
			};
			for (std::size_t d=0; d<FIELD_ORDER; d++) {
					neighborIndex[d]--;
					double expectedValue = values[linearIndexOf(neighborIndex)];
					expect_equal(expectedValue, iterator->currentNeighbor(d, -1));
					neighborIndex[d]++;
					neighborIndex[d]++;
					expectedValue = values[linearIndexOf(neighborIndex)];
					expect_equal(expectedValue, iterator->currentNeighbor(d, 1));
					neighborIndex[d]--;
			}
		}

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
					double expectedValue = values[linearIndexOf(neighborIndex)];
					expect_equal(expectedValue, iterator->currentNeighbor(d, -1));
					neighborIndex[d]++;
				}
				// Neighbor above
				if (d != boundary.getDimension() || boundary.isLowerSide()) {
					neighborIndex[d]++;
					double expectedValue = values[linearIndexOf(neighborIndex)];
					expect_equal(expectedValue, iterator->currentNeighbor(d, 1));
					neighborIndex[d]--;
				}
				// Neighbor further away
				if (d == boundary.getDimension()) {
					if (boundary.isLowerSide()) {
						neighborIndex[d]+=2;
						double expectedValue = values[linearIndexOf(neighborIndex)];
						expect_equal(expectedValue, iterator->currentNeighbor(d, 2));
						neighborIndex[d]-=2;
					} else {
						neighborIndex[d]-=2;
						double expectedValue = values[linearIndexOf(neighborIndex)];
						expect_equal(expectedValue, iterator->currentNeighbor(d, -2));
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
		void verifySetCurrentNeighbor() {
			// Change the value of each neighbor
			for (std::size_t d=0; d<FIELD_ORDER; d++) {
				iterator->setCurrentNeighbor(d, -1, d + 5.1);
				iterator->setCurrentNeighbor(d, 1, d + 5.2);
			}
			// ...and check the values
			for (std::size_t d=0; d<FIELD_ORDER; d++) {
				double expectedValue = d + 5.1;
				expect_equal(expectedValue, iterator->currentNeighbor(d, -1));
				expectedValue = d + 5.2;
				expect_equal(expectedValue, iterator->currentNeighbor(d, 1));
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
					iterator->setCurrentNeighbor(d, -1, 10*boundary.getDimension() + d + 0.1);
				}
				if (d != boundary.getDimension() || boundary.isLowerSide()) {
					iterator->setCurrentNeighbor(d, 1, 10*boundary.getDimension() + d + 0.2);
				}
			}
			// ...and check the values
			for (std::size_t d=0; d<FIELD_ORDER; d++) {
				if (d != boundary.getDimension() || !boundary.isLowerSide()) {
					double expectedValue = 10 * boundary.getDimension() + d + 0.1;
					expect_equal(expectedValue, iterator->currentNeighbor(d, -1));
				}
				if (d != boundary.getDimension() || boundary.isLowerSide()) {
					double expectedValue = 10 * boundary.getDimension() + d + 0.2;
					expect_equal(expectedValue, iterator->currentNeighbor(d, 1));
				}
			}
		}


	private:
		double *values;
	};


	/**
	 * Unit test that aborts for ValueFieldIterator.
	 *
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2014
	 */
	class ValueFieldIteratorDeathTest : public ValueFieldIteratorTest
	{
	public:
		virtual void SetUp() {
			ValueFieldIteratorTest::SetUp();
			::testing::FLAGS_gtest_death_test_style = "threadsafe";
		}


	protected:
		void testCurrentIndex() {
			FieldIteratorTest::testCurrentIndex_outside(iterator);
		}

		void testCurrentNeighbor() {
			FieldIteratorTest::testCurrentNeighbor_outside(iterator);
		}

		void testCurrentValue() {
			FieldIteratorTest::testCurrentValue_outside(iterator);
		}

		void testSetCurrentNeighbor() {
			FieldIteratorTest::testSetCurrentNeighbor_outside(iterator);
		}

		void testSetCurrentValue() {
			FieldIteratorTest::testSetCurrentValue_outside(iterator);
		}
	};


	/**
	 * Verify the behavior of the basic methods of a ValueFieldIterator.
	 */
	TEST_F(ValueFieldIteratorTest, TestBasics) {
		testSize();
		testIsInField();
	}

	/**
	 * Verify the behavior of the getter methods of a ValueFieldIterator.
	 */
	TEST_F(ValueFieldIteratorTest, TestGetters) {
		testCurrentIndex();
		testCurrentValue();
		testCurrentNeighbor();
	}

	/**
	 * Verify the behavior of the methods that moves a ValueFieldIterator.
	 */
	TEST_F(ValueFieldIteratorTest, TestIteration) {
		testFirst();
		// next is implicitly tested by the other tests.
	}

	/**
	 * Verify the behavior of the setter methods of a ValueFieldIterator.
	 */
	TEST_F(ValueFieldIteratorTest, TestSetters) {
		testSetCurrentValue();
		testSetCurrentNeighbor();
	}


#ifndef MPI_LIB
	/**
	 * Verify the behavior of the getter methods of a ValueFieldIterator, when
	 * the iterator is outside the field.
	 */
	TEST_F(ValueFieldIteratorDeathTest, TestGetters) {
		testCurrentIndex();
		testCurrentValue();
		testCurrentNeighbor();
	}

	/**
	 * Verify the behavior of the setter methods of a ValueFieldIterator, when
	 * the iterator is outside the field.
	 */
	TEST_F(ValueFieldIteratorDeathTest, TestSetters) {
		testSetCurrentValue();
		testSetCurrentNeighbor();
	}
#endif
}
}
