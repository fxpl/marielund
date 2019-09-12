#include "src/iterators/WholeFieldStepper.hpp"
#include "test/HaparandaTest.hpp"

#define ORDER 3

namespace Haparanda {
namespace Iterators {

	/**
	 * Unit test for the stepping strategy WholeFieldStepper.
	 *
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2013-2014, 2016
	 */
	class WholeFieldStepperTest : public HaparandaTest
	{
	public:
		virtual void SetUp() {
			size = {3, 4, 5};
			stride = {1,
					size[0],
					size[0]*size[1]};
			totalSize = size[0] * size[1] * size[2];
			strategy = new WholeFieldStepper<ORDER>(size);
		}

		virtual void TearDown() {
			delete strategy;
		}

	protected:
		WholeFieldStepper<ORDER> *strategy;
		std::array<std::size_t, ORDER> stride;
		std::array<std::size_t, ORDER> size;
		std::size_t totalSize;

		/**
		 * Verify that currentIndex returns the index in the specified
		 * dimension.
		 */
		void testCurrentIndex() {
			strategy->first();
			for (std::size_t i2=0; i2<size[2]; i2++) {
				for (std::size_t i1=0; i1<size[1]; i1++) {
					for (std::size_t i0=0; i0<size[0]; i0++) {
						EXPECT_EQ(i0, strategy->currentIndex((std::size_t)0));
						EXPECT_EQ(i1, strategy->currentIndex((std::size_t)1));
						EXPECT_EQ(i2, strategy->currentIndex((std::size_t)2));
						strategy->next();
					}
				}
			}
		}

		/**
		 * Verify that isInField returns true as long as the iterator has not
		 * stepped out of the field.
		 */
		void testIsInField() {
			strategy->first();
			for (std::size_t i=0; i<totalSize; i++) {
				EXPECT_TRUE(strategy->isInField());
				strategy->next();
			}
			EXPECT_FALSE(strategy->isInField());
		}

		/**
		 * Verify the behavior of first: It should (re)set the iterator to 0.
		 */
		void testFirst() {
			std::size_t expected = 0;
			strategy->first();
			EXPECT_EQ(expected, strategy->index);
		}

		/**
		 * For each element in the field: verify that linearNeighborIndex
		 * returns current iterator index + offset * s
		 * where s is the stride in the dimension in which the neighbor is
		 * located, when called for a neighbor which is inside the field.
		 */
		void testLinearNeighborIndex() {
			strategy->first();
			std::size_t index = 0;
			for (std::size_t i2=0; i2<size[2]; i2++) {
				for (std::size_t i1=0; i1<size[1]; i1++) {
					for (std::size_t i0=0; i0<size[0]; i0++) {
						// Closest neighbor in dimension 0
						if (i0>0) {
							EXPECT_EQ(index-stride[0], strategy->linearNeighborIndex(0, -1));
						}
						if (i0<size[0]-1) {
							EXPECT_EQ(index+stride[0], strategy->linearNeighborIndex(0, 1));
						}
						// Closest neighbor in dimension 1
						if (i1>0) {
							EXPECT_EQ(index-stride[1], strategy->linearNeighborIndex(1, -1));
						}
						if (i1<size[1]-1) {
							EXPECT_EQ(index+stride[1], strategy->linearNeighborIndex(1, 1));
						}
						// Closest neighbor in dimension 2
						if (i2>0) {
							EXPECT_EQ(index-stride[2], strategy->linearNeighborIndex(2, -1));
						}
						if (i2<size[2]-1) {
							EXPECT_EQ(index+stride[2], strategy->linearNeighborIndex(2, 1));
						}
						// Neighbor further away in dimension 2
						if (i2>2) {
							EXPECT_EQ(index-3*stride[2], strategy->linearNeighborIndex(2, -3));
						}
						if (i2<size[2]-3) {
							EXPECT_EQ(index+3*stride[2], strategy->linearNeighborIndex(2, 3));
						}
						strategy->next();
						index++;
					}
				}
			}
		}

		/**
		 * For each element in the field: Call neighborInField for the closest
		 * neighbor in each direction and verify that the method returns true
		 * if the neighbor is inside the field and false otherwise.
		 */
		void testNeighborInField() {
			strategy->first();
			for (std::size_t i2=0; i2<size[2]; i2++) {
				for (std::size_t i1=0; i1<size[1]; i1++) {
					for (std::size_t i0=0; i0<size[0]; i0++) {
						EXPECT_EQ(0!=i0, strategy->neighborInField(0, -1));
						EXPECT_EQ(size[0]-1!=i0, strategy->neighborInField(0, 1));
						EXPECT_EQ(0!=i1, strategy->neighborInField(1, -1));
						EXPECT_EQ(size[1]-1!=i1, strategy->neighborInField(1, 1));
						EXPECT_EQ(0!=i2, strategy->neighborInField(2, -1));
						EXPECT_EQ(size[2]-1!=i2, strategy->neighborInField(2, 1));
						strategy->next();
					}
				}
			}
		}

		/**
		 * Verify that each element in the field which is iterated over is
		 * touched and considered being in the field exactly once when the
		 * iteration is thread parallel.
		 */
		void testParallelStepping() {
			int *timesTouched = new int[totalSize];
			std::fill_n(timesTouched, totalSize, 0);
			#pragma omp parallel
			{
				WholeFieldStepper<ORDER> *parallelStrategy = new WholeFieldStepper<ORDER>(size);
				while (parallelStrategy->isInField()) {
					timesTouched[parallelStrategy->index]++;
					parallelStrategy->next();
				}
				delete parallelStrategy;
			}
			for (size_t i=0; i<totalSize; i++) {
				EXPECT_EQ(1, timesTouched[i]);
			}
			delete []timesTouched;
		}
	};


	class WholeFieldStepperDeathTest : public WholeFieldStepperTest
	{
	public:
		virtual void SetUp() {
			WholeFieldStepperTest::SetUp();
			::testing::FLAGS_gtest_death_test_style = "threadsafe";
		}

	protected:
		/**
		 * Verify that currentIndex causes the program to abort if it is called
		 * when the iterator is outside the field and the program is run in
		 * debug mode.
		 */
		void testCurrentIndex() {
			strategy->index = totalSize;
			for (std::size_t i=0; i<ORDER; i++) {
				EXPECT_ASSERT_FAIL(strategy->currentIndex(i));
			}
		}

		/**
		 * For the element in the middle of each boundary: Verify that
		 * requesting the index of the closest neighbor outside the field causes
		 * the program to abort if it is run in debug mode.
		 */
		void testLinearNeighborIndex() {
			strategy->first();
			std::size_t mid0 = size[0]/2; std::size_t max0 = size[0]-1;
			std::size_t mid1 = size[1]/2; std::size_t max1 = size[1]-1;
			std::size_t mid2 = size[2]/2; std::size_t max2 = size[2]-1;
			std::size_t index = 0;

			// Middle of boundary i2=0
			while (index < stride[1]*mid1 + stride[0]*mid0) {
				strategy->next();
				index++;
			}
			EXPECT_ASSERT_FAIL(strategy->linearNeighborIndex(2, -1));

			// Middle of boundary i1=0
			while (index < stride[2]*mid2 + stride[0]*mid0) {
				strategy->next();
				index++;
			}
			EXPECT_ASSERT_FAIL(strategy->linearNeighborIndex(1, -1));

			// Middle of boundary i0=0
			while (index < stride[2]*mid2 + stride[1]*mid1) {
				strategy->next();
				index++;
			}
			EXPECT_ASSERT_FAIL(strategy->linearNeighborIndex(0, -1));

			// Middle of boundary i0=size0
			while (index < stride[2]*mid2 + stride[1]*mid1 + stride[0]*max0) {
				strategy->next();
				index++;
			}
			EXPECT_ASSERT_FAIL(strategy->linearNeighborIndex(0, 1));

			// Middle of boundary i1=size1
			while (index < stride[2]*mid2 + stride[1]*max1 + stride[0]*mid0) {
				strategy->next();
				index++;
			}
			EXPECT_ASSERT_FAIL(strategy->linearNeighborIndex(1, 1));

			// Middle of boundary i2=size2
			while (index < stride[2]*max2 + stride[1]*mid1 + stride[0]*mid0) {
				strategy->next();
				index++;
			}
			EXPECT_ASSERT_FAIL(strategy->linearNeighborIndex(2, 1));
		}

		/**
		 * Verify that next causes the program to abort if it is run in debug
		 * mode and the method is called more times than there are elements to
		 * be iterated over.
		 */
		void testNext() {
			strategy->first();
			for (std::size_t i=0; i<totalSize; i++) {
				strategy->next();
			}
			EXPECT_ASSERT_FAIL(strategy->next());
		}

	};


	/**
	 * Verify the behavior of the resetting method (first) and
	 * the methods checking whether an element is inside the field
	 * (isInField and neighborInField).
	 */
	TEST_F(WholeFieldStepperTest, TestBasics) {
		testFirst();
		testIsInField();
		testNeighborInField();
	}

	/**
	 * Verify the behavior of or the index getters (currentIndex and
	 * linearNeighborIndex) of a stepping strategy for a non-cubic field of
	 * order 3.
	 */
	TEST_F(WholeFieldStepperTest, TestIndex) {
		testCurrentIndex();
		testLinearNeighborIndex();
		testNeighborInField();
	}

	/**
	 * Verify that the stepping is performed correctly when more than one thread
	 * is present.
	 */
	TEST_F(WholeFieldStepperTest, TestParallel) {
		testParallelStepping();
	}


#ifndef MPI_LIB
	/**
	 * Verify the assertions in the index getters (currentIndex and
	 * neighborIndex) of a stepping strategy for a non-cubic field of order 3.
	 */
	TEST_F(WholeFieldStepperDeathTest, TestIndex) {
		testCurrentIndex();
		testLinearNeighborIndex();
	}

	/**
	 * Verify the assertion in next in a stepping strategy for a non-cubic field
	 * of order 3.
	 */
	TEST_F(WholeFieldStepperDeathTest, TestForward) {
		testNext();
	}
#endif
}
}
