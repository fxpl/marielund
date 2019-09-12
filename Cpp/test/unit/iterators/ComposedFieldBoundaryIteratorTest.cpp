#include "src/iterators/ComposedFieldBoundaryIterator.hpp"
#include "src/iterators/ValueFieldBoundaryIterator.hpp"
#include "FieldIteratorTest.hpp"

namespace Haparanda {
namespace Iterators {

	/**
	 * Unit test for ComposedFieldBoundaryIterator.
	 *
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2014, 2016, 2017
	 */
	class ComposedFieldBoundaryIteratorTest : public FieldIteratorTest
	{
	public:
		virtual void SetUp() {
			elementsPerDim = 5;
			sizes = {elementsPerDim, elementsPerDim, elementsPerDim};
			FieldIteratorTest::SetUp(sizes);

			values = new double[totalSize];
			for (std::size_t i=0; i<totalSize; i++) {
				values[i] = 33.0 * i;
			}

			extent = 4;
			// Initialize ghost region iterators
			ghostValues = new double**[FIELD_ORDER];
			ghostIterators = new FieldIterator<FIELD_ORDER>**[FIELD_ORDER];
			for (std::size_t i=0; i<FIELD_ORDER; i++) {
				ghostValues[i] = new double*[2];
				ghostIterators[i] = new FieldIterator<FIELD_ORDER>*[2];
				std::array<std::size_t, FIELD_ORDER> ghostSizes;
				ghostSizes.fill(elementsPerDim);
				ghostSizes[i] = extent;
				std::size_t totalGhostSize = 1;
				for (std::size_t j=0; j<FIELD_ORDER; j++) {
					totalGhostSize *= ghostSizes[j];
				}
				for (std::size_t j=0; j<2; j++) {
					ghostValues[i][j] = new double[totalGhostSize];
					double ghostValue = 1000*i + 100*j;
					for (std::size_t k=0; k<totalGhostSize; k++) {
						ghostValues[i][j][k] = ghostValue++;
					}
					ghostIterators[i][j] = new ValueFieldBoundaryIterator<FIELD_ORDER>(
							ghostSizes, ghostValues[i][j]);
				}
			}
			iterator = new ComposedFieldBoundaryIterator<FIELD_ORDER>(sizes, values, ghostIterators);
		}

		virtual void TearDown() {
			FieldIteratorTest::TearDown();
			for (std::size_t i=0; i<FIELD_ORDER; i++) {
				for (std::size_t j=0; j<2; j++) {
					delete[] ghostValues[i][j];
				}
				delete[] ghostValues[i];
			}
			delete []ghostValues;
			delete []values;
			delete iterator;
		}


	protected:
		BoundaryIterator<FIELD_ORDER> *iterator;
		std::size_t elementsPerDim;
		std::size_t extent;
		FieldIterator<FIELD_ORDER> ***ghostIterators;
		double ***ghostValues;

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
		 * neighbor. The verification is done for the midpoint of each boundary
		 * and for all neighbors, including the one in the ghost region.
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
							double expectedValue = values[index];
							expect_equal(expectedValue, iterator->currentValue());
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
		 * verification is done for every neighbor, in the field and in the
		 * ghost region.
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

		/**
		 * Verify that size returns the size of the field, in each dimension.
		 */
		void testSize() {
			ASSERT_EQ(sizes[0]+2*extent, iterator->size(0));
			ASSERT_EQ(sizes[1]+2*extent, iterator->size(1));
			ASSERT_EQ(sizes[2]+2*extent, iterator->size(2));
		}


		/*** Helper methods ***/

		/**
		 * Move the iterator to the mid point of the specified boundary and
		 * check the value of each neighbor. Also check the neighbor two steps
		 * inwards and and the one in the other end of the ghost region.
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
				// Indices in ghost regions
				std::size_t lowerGhostMidIndex = 0;
				for (std::size_t i=0; i<FIELD_ORDER; i++) {
					if (i < d) {
						lowerGhostMidIndex += sizes[i]/2 * strides[i];
					} else if (i > d) {
						lowerGhostMidIndex += sizes[i]/2 * strides[i] / sizes[d] * extent;
					}
				}
				std::size_t upperGhostMidIndex = lowerGhostMidIndex + (extent-1)*strides[d];

				// Closest neighbor below
				if (d == boundary.getDimension() && boundary.isLowerSide()) {
					// Neighbor is in ghost region
					double expectedValue = 1000.0*d + upperGhostMidIndex;
					expect_equal(expectedValue, iterator->currentNeighbor(d, -1));
				} else {
					// Neighbor is inside field
					neighborIndex[d]--;
					double expectedValue = values[linearIndexOf(neighborIndex)];
					expect_equal(expectedValue, iterator->currentNeighbor(d, -1));
					neighborIndex[d]++;
				}

				// Closest neighbor above
				if (d == boundary.getDimension() && !boundary.isLowerSide()) {
					// Neighbor is in ghost region
					double expectedValue = 1000.0*d + 100 + lowerGhostMidIndex;
					expect_equal(expectedValue, iterator->currentNeighbor(d, 1));
				} else {
					// Neighbor is inside field
					neighborIndex[d]++;
					double expectedValue = values[linearIndexOf(neighborIndex)];
					expect_equal(expectedValue, iterator->currentNeighbor(d, 1));
					neighborIndex[d]--;
				}

				// Neighbors further away
				if (d == boundary.getDimension()) {
					if (boundary.isLowerSide()) {
						// Neighbor below, on other side of ghost region
						double expectedValue = 1000*d + lowerGhostMidIndex;
						expect_equal(expectedValue, iterator->currentNeighbor(d, -extent));
						// Neighbor 2 steps above (inside region)
						neighborIndex[d]+=2;
						expectedValue = values[linearIndexOf(neighborIndex)];
						expect_equal(expectedValue, iterator->currentNeighbor(d, 2));
						neighborIndex[d]-=2;
					} else {
						// Neighbor 2 steps below (inside region)
						neighborIndex[d]-=2;
						double expectedValue = values[linearIndexOf(neighborIndex)];
						expect_equal(expectedValue, iterator->currentNeighbor(d, -2));
						neighborIndex[d]+=2;
						// Neighbor above, on other side of ghost region
						expectedValue = 1000*d + 100 + upperGhostMidIndex;
						expect_equal(expectedValue, iterator->currentNeighbor(d, extent));
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
		void verifySetCurrentNeighbor(const BoundaryId& boundary) {
			stepIteratorToMiddleOfBoundary(iterator, boundary);
			// Change the value of each neighbor
			for (std::size_t d=0; d<FIELD_ORDER; d++) {
				iterator->setCurrentNeighbor(d, -1, d + 101.0);
				iterator->setCurrentNeighbor(d, 1, d + 102.0);
			}
			// ...and check the values
			for (std::size_t d=0; d<FIELD_ORDER; d++) {
				double expectedValue = d + 101.0;
				expect_equal(expectedValue, iterator->currentNeighbor(d, -1));
				expectedValue = d + 102.0;
				expect_equal(expectedValue, iterator->currentNeighbor(d, 1));
			}
		}


	private:
		double *values;
	};


	/**
	 * Unit test that abort for ComposedFieldBoundaryIterator.
	 *
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2014
	 */
	class ComposedFieldBoundaryIteratorDeathTest : public ComposedFieldBoundaryIteratorTest
	{
	public:
		virtual void SetUp() {
			ComposedFieldBoundaryIteratorTest::SetUp();
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

		/**
		 * For the element in the middle of each boundary: Verify that
		 * requesting the value of a neighbor outside the ghost region causes
		 * the program to abort if it is run in debug mode.
		 */
		void testCurrentNeighbor() {
			for (std::size_t d=0; d<FIELD_ORDER; d++) {
				for (std::size_t side=0; side<2; side++) {
					stepIteratorToMiddleOfBoundary(iterator, BoundaryId(d, 0==side));
					int offset = 0==side ? -extent-1 : extent+1;
					EXPECT_ASSERT_FAIL(iterator->currentNeighbor(d, offset));
				}
			}
		}

		void testCurrentValue() {
			for (std::size_t d=0; d<FIELD_ORDER; d++) {
				for (std::size_t side=0; side<2; side++) {
					iterator->setBoundaryToIterate(BoundaryId(d, 0==side));
					FieldIteratorTest::testCurrentValue_outside(iterator);
				}
			}
		}

		/**
		 * For the element in the middle of each boundary: Verify that
		 * trying to change the value of a neighbor outside the ghost region
		 * causes the program to abort if it is run in debug mode.
		 */
		void testSetCurrentNeighbor() {
			for (std::size_t d=0; d<FIELD_ORDER; d++) {
				for (std::size_t side=0; side<2; side++) {
					stepIteratorToMiddleOfBoundary(iterator, BoundaryId(d, 0==side));
					int offset = 0==side ? -extent-1 : extent+1;
					EXPECT_ASSERT_FAIL(iterator->setCurrentNeighbor(d, offset, 76.0));
				}
			}
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
	 * ComposedFieldBoundaryIterator.
	 */
	TEST_F(ComposedFieldBoundaryIteratorTest, TestBasics) {
		// setBoundaryToIterate is implicitly tested by testFirst and testCurrentIndex.
		testSize();
		testIsInField();
	}

	/**
	 * Verify the behavior of the getter methods of a
	 * ComposedFieldBoundaryIterator.
	 */
	TEST_F(ComposedFieldBoundaryIteratorTest, TestGetters) {
		testCurrentIndex();
		testCurrentValue();
		testCurrentNeighbor();
	}

	/**
	 * Verify the behavior of the methods which moves a
	 * ComposedFieldBoundaryIterator.
	 */
	TEST_F(ComposedFieldBoundaryIteratorTest, TestIteration) {
		testFirst();
		// next is implicitly tested by the other tests.
	}

	/**
	 * Verify the behavior of the setters of a ComposedFieldBoundaryIterator.
	 */
	TEST_F(ComposedFieldBoundaryIteratorTest, TestSetters) {
		testSetCurrentValue();
		testSetCurrentNeighbor();
	}


#ifndef MPI_LIB
	/**
	 * Verify the behavior of the getter methods of a
	 * ComposedFieldBoundaryIterator, when the iterator is outside the field.
	 */
	TEST_F(ComposedFieldBoundaryIteratorDeathTest, TestGetters) {
		testCurrentIndex();
		testCurrentValue();
		testCurrentNeighbor();
	}

	/**
	 * Verify the behavior of the setters of a ComposedFieldBoundaryIterator,
	 * when the iterator is outside the field.
	 */
	TEST_F(ComposedFieldBoundaryIteratorDeathTest, TestSetters) {
		testSetCurrentValue();
		testSetCurrentNeighbor();
	}
#endif

}
}

