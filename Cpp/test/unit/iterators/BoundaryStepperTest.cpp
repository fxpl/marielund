#include "src/iterators/BoundaryStepper.hpp"
#include "test/HaparandaTest.hpp"

#define ORDER 3

namespace Haparanda {
namespace Iterators {

	/**
	 * Unit test for the stepping strategy BoundaryStepper.
	 *
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2013-2014, 2016
	 */
	class BoundaryStepperTest : public HaparandaTest
	{
	public:
		virtual void SetUp() {
			sizes = {3, 4, 5};
			strides = {1, sizes[0], sizes[0]*sizes[1]};
			totalSize = sizes[0] * sizes[1] * sizes[2];
			strategy = new BoundaryStepper<ORDER>(sizes);

			lowerMinIndices = new std::size_t[ORDER];
			lowerMaxIndices = new std::size_t[ORDER];
			upperMinIndices = new std::size_t[ORDER];
			upperMaxIndices = new std::size_t[ORDER];
			boundarySizes = new std::size_t[ORDER];

			std::fill_n(lowerMinIndices, ORDER, 0);
			std::fill_n(lowerMaxIndices, ORDER, 0);
			std::fill_n(upperMaxIndices, ORDER, totalSize-1);
			for (std::size_t i=0; i<ORDER; i++) {
				upperMinIndices[i] = (sizes[i]-1) * strides[i];
				for (std::size_t j=0; j<ORDER; j++) {
					if (i!=j) {
						lowerMaxIndices[i] += (sizes[j]-1) * strides[j];
					}
				}
				boundarySizes[i] = totalSize/sizes[i];
			}
		}

		virtual void TearDown() {
			delete strategy;
			delete []lowerMinIndices;
			delete []upperMinIndices;
			delete []lowerMaxIndices;
			delete []upperMaxIndices;
			delete []boundarySizes;
		}

	protected:
		std::size_t *boundarySizes;
		BoundaryStepper<ORDER> *strategy;

		/**
		 * For each boundary: Verify that isInField returns true until next has
		 * been called x times, and thereafter returns false.
		 * x = total number of elements on the boundary.
		 */
		void testIsInField() {
			for (std::size_t i=0; i<ORDER; i++) {
				verifyIsInField(BoundaryId(i, true));
				verifyIsInField(BoundaryId(i, false));
			}
		}

		/**
		 * Verify the behavior of first: Irrespective of which boundary it
		 * currently iterates over, it should make the iterator point at the
		 * first element of that boundary.
		 */
		void testFirst() {
			for (std::size_t i=0; i<ORDER; i++) {
				// Lower boundaries
				strategy->setBoundaryToIterate(BoundaryId(i, true));
				// Set index to something else than the min indices
				strategy->index = 100000;
				strategy->first();
				EXPECT_EQ(lowerMinIndices[i], strategy->index);

				// Upper boundaries
				strategy->setBoundaryToIterate(BoundaryId(i, false));
				// Set index to something else than the min indices
				strategy->index = 0;
				strategy->first();
				EXPECT_EQ(upperMinIndices[i], strategy->index);
			}
		}

		/**
		 * For all boundaries of the field: verify that next steps through all
		 * elements on the boundary.
		 */
		void testNext() {
			for (std::size_t d=2; d<ORDER; d++) {
				std::size_t otherDim1 = 0==d ? 1 : 0;
				std::size_t otherDim2 = 2==d ? 1 : 2;
				// Lower boundaries
				strategy->setBoundaryToIterate(BoundaryId(d, true));
				for (std::size_t i2=0; i2<sizes[otherDim2]; i2++) {
					for (std::size_t i1=0; i1<sizes[otherDim1]; i1++) {
						EXPECT_EQ(i2*strides[otherDim2] + i1*strides[otherDim1],
								strategy->index);
						strategy->next();
					}
				}
				// Upper boundaries
				strategy->setBoundaryToIterate(BoundaryId(d, false));
				for (std::size_t i2=0; i2<sizes[otherDim2]; i2++) {
					for (std::size_t i1=0; i1<sizes[otherDim1]; i1++) {
						EXPECT_EQ(upperMinIndices[d] + i2*strides[otherDim2] + i1*strides[otherDim1],
								strategy->index);
						strategy->next();
					}
				}
			}
		}

		/**
		 * Verify that each element on the boundary of the field which is
		 * iterated over is touched exactly once and other elements are touched
		 * 0 times when the iteration is thread parallel.
		 */
		void testParallelStepping() {
			int *timesTouched = new int[totalSize];
			for (size_t d=0; d<ORDER; d++) {
				for (size_t l=0; l<2; l++) {
					bool lower = 0==l;
					std::fill_n(timesTouched, totalSize, 0);
					#pragma omp parallel
					{
						BoundaryStepper<ORDER> *parallelStrategy = new BoundaryStepper<ORDER>(sizes);
						parallelStrategy->setBoundaryToIterate(BoundaryId(d, lower));
						while (parallelStrategy->isInField()) {
							timesTouched[parallelStrategy->index]++;
							parallelStrategy->next();
						}
						delete parallelStrategy;
					}
					size_t nextStride = ORDER-1 == d ? totalSize : strides[d+1];
					for (size_t i=0; i<totalSize; i++) {
						if ((i+l*strides[d])%nextStride < strides[d]) {
							// i is an index on the current boundary
							EXPECT_EQ(1, timesTouched[i]);
						} else {
							EXPECT_EQ(0, timesTouched[i]);
						}
					}
				}
			}
			delete []timesTouched;
		}

		/**
		 * Verify that setBoundaryToIterate sets dimension and lower to the
		 * values given as arguments and calculates the minimum and maximum
		 * indices of the boundary.
		 */
		void testSetBoundaryToIterate() {
			// Lower boundaries
			for (std::size_t i=0; i<ORDER; i++) {
				strategy->setBoundaryToIterate(BoundaryId(i, true));
				EXPECT_EQ(i, strategy->boundary.getDimension());
				EXPECT_EQ(true, strategy->boundary.isLowerSide());
				EXPECT_EQ(lowerMinIndices[i], strategy->minIndex);
				EXPECT_EQ(lowerMaxIndices[i], strategy->maxIndex);
			}
			// Upper boundaries
			for (std::size_t i=0; i<ORDER; i++) {
				strategy->setBoundaryToIterate(BoundaryId(i, false));
				EXPECT_EQ(i, strategy->boundary.getDimension());
				EXPECT_EQ(false, strategy->boundary.isLowerSide());
				EXPECT_EQ(upperMinIndices[i], strategy->minIndex);
				EXPECT_EQ(upperMaxIndices[i], strategy->maxIndex);
			}
		}

		/**
		 * Verify that the stepper is always considered to be outside the field
		 * when the size of the field is 0.
		 */
		void testSetBoundaryToIterate_emptyField() {
			std::array<std::size_t, ORDER>sizes = {0, 0, 0};
			BoundaryStepper<ORDER>emptyStepper(sizes);
			for(std::size_t d=0; d<ORDER; d++) {
				for(std::size_t dir=0; dir<2; dir++) {
					emptyStepper.setBoundaryToIterate(BoundaryId(d, dir==0));
					EXPECT_FALSE(emptyStepper.isInField());
				}
			}
		}


	private:
		std::array<std::size_t, ORDER> strides;
		std::array<std::size_t, ORDER> sizes;
		std::size_t totalSize;

		std::size_t *lowerMinIndices;
		std::size_t *upperMinIndices;
		std::size_t *lowerMaxIndices;
		std::size_t *upperMaxIndices;


		/**
		 * Verify the behavior of isInField on the specified boundary
		 *
		 * @param boundary The boundary on which the behavior is to be verified
		 */
		void verifyIsInField(BoundaryId boundary) {
			strategy->setBoundaryToIterate(boundary);
			for (std::size_t j = 0; j < boundarySizes[boundary.getDimension()]; j++) {
				EXPECT_TRUE(strategy->isInField());
				strategy->next();
			}
			EXPECT_FALSE(strategy->isInField());
		}

	};


	class BoundaryStepperDeathTest : public BoundaryStepperTest
	{
	public:
		virtual void SetUp() {
			BoundaryStepperTest::SetUp();
			::testing::FLAGS_gtest_death_test_style = "threadsafe";
		}

	protected:
		/**
		 * For each boundary: verify that next causes the program to abort if
		 * the program is run in debug mode and the method is called more times
		 * than there are elements on the boundary.
		 */
		void testNext() {
			for (std::size_t i=0; i<ORDER; i++) {
				verifyNext(BoundaryId(i, true));
				verifyNext(BoundaryId(i, false));
			}
		}

		/**
		 * Verify that setBoundaryToIterate causes the program to abort if the
		 * method is given a dimension >= the dimensionality of the block.
		 */
		void testSetBoundaryToIterate() {
			EXPECT_ASSERT_FAIL(strategy->setBoundaryToIterate(BoundaryId(ORDER, 0)));
			EXPECT_ASSERT_FAIL(strategy->setBoundaryToIterate(BoundaryId(ORDER+5, 0)));
		}

	private:
		/**
		 * For the specified boundary: verify that next causes the program to
		 * abort if the program is run in debug mode and the method is called
		 * more times than there are elements on the boundary.
		 *
		 * @param boundary The boundary on which the behavior is to be verified
		 */
		void verifyNext(BoundaryId boundary) {
			strategy->setBoundaryToIterate(boundary);
			for (std::size_t j=0; j<boundarySizes[boundary.getDimension()]; j++) {
				strategy->next();
			}
			EXPECT_ASSERT_FAIL(strategy->next());
		}

	};


	/**
	 * Verify the behavior of the resetting methods (first and
	 * setBoundaryToIterate) for each boundary of a non-cubic field of order 3.
	 */
	TEST_F(BoundaryStepperTest, TestBasics) {
		testIsInField();
		testFirst();
		testSetBoundaryToIterate();
		testSetBoundaryToIterate_emptyField();
	}

	/**
	 * Verify the behavior of next on each boundary of a non-cubic field of
	 * order 3.
	 */
	TEST_F(BoundaryStepperTest, TestForward) {
		testNext();
	}

    /**
     * Verify that the stepping is performed correctly when more than one thread
     * is present.
     */
    TEST_F(BoundaryStepperTest, TestParallel) {
        testParallelStepping();
    }


#ifndef MPI_LIB
	/**
	 * Verify the assertion in setBoundaryToIterate.
	 */
	TEST_F(BoundaryStepperDeathTest, TestBasics) {
		testSetBoundaryToIterate();
	}

	/**
	 * Verify the assertion in next.
	 */
	TEST_F(BoundaryStepperDeathTest, TestForward) {
		testNext();
	}
#endif

}
}
