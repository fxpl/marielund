#include "src/grid/ComputationalComposedBlock.hpp"
#include "src/utils/Math.hpp"
#include "test/HaparandaTest.hpp"

#define DIM 3  // Dimensionality of the test blocks

using namespace Haparanda::Grid;
using namespace Haparanda::Math;

/**
 * Unit test for ComputationalComposedBlocks.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2013-2014, 2017-2018
 */
class ComputationalComposedBlockTest : public HaparandaTest
{
public:

	virtual void SetUp() {
		elementsPerDim = 10;
		totalSize = power(elementsPerDim, DIM);
		extent = 4;

		values = new double[totalSize];
		for (std::size_t i=0; i<totalSize; i++) {
			values[i] = 1.2 * i;
		}

		block = new ComputationalComposedBlock<DIM>(elementsPerDim, extent, values);
	}

	virtual void TearDown() {
		delete block;
		delete []values;
	}

protected:
	std::size_t elementsPerDim;
	std::size_t totalSize;
	std::size_t extent;
	ComputationalComposedBlock<DIM> *block;

	/**
	 * Verify that the constructor that does not take values as an argument
	 * - Sets elementsPerDim to the value provided
	 * - Can be initialized with setValues
	 * The latter means (among other things) that the ghost regions probably are
	 * created correctly.
	 */
	void testConstructorNoValues() {
		ComputationalComposedBlock<DIM> lateInitBlock(elementsPerDim, extent);
		expect_equal(elementsPerDim, lateInitBlock.getElementsPerDim());

		double *lateValues = new double[totalSize];
		for (std::size_t i=0; i<totalSize; i++) {
			lateValues[i] = 1.2 * i;
		}
		lateInitBlock.setValues(lateValues);
		lateInitBlock.startCommunication();
		verifyInnerValues(lateInitBlock, lateValues);
		verifyGhostRegionValues(lateInitBlock);
		delete []lateValues;
		lateInitBlock.finishCommunication();
	}

	/**
	 * Verify that the constructor that takes values as an argument
	 * - Sets elementsPerDim to the value provided
	 * - Sets the value array to the one provided
	 * - Creates ghost regions and initializes them using periodic boundary conditions
	 * Note that this test must not be run when there is > 1 processor in the
	 * simulation
	 */
	void testConstructorValues() {
		block->startCommunication();
		expect_equal(elementsPerDim, block->getElementsPerDim());
		verifyInnerValues(*block, values);
		verifyGhostRegionValues(*block);
		block->finishCommunication();
	}

	/**
	 * Verify that getBoundaryIterator returns a ComposedFieldBoundaryIterator
	 * whose size in each dimension is the with of the block (including ghost
	 * regions) and whose first value is the the one specified by the value
	 * array and the smallest index.
	 */
	void testGetBoundaryIterator() {
		BoundaryIterator<DIM> *iterator = block->getBoundaryIterator();
		// Verify type
		EXPECT_EQ(typeid(ComposedFieldBoundaryIterator<DIM>), typeid(*iterator));
		// Verify size
		for (std::size_t i=0; i<DIM; i++) {
			EXPECT_EQ(elementsPerDim+2*extent, iterator->size(i));
		}
		// Verify (first) value
		iterator->setBoundaryToIterate(BoundaryId(0, true));
		expect_equal(values[0], iterator->currentValue());
		delete iterator;
	}

	/**
	 * Verify that getInnerIterator returns a ValueFieldIterator whose size in
	 * each dimension is the with of the block (excluding ghost regions) and
	 * whose first value is the the one specified by the value array and the
	 * smallest index.
	 */
	void testGetInnerIterator() {
		FieldIterator<DIM> *iterator = block->getInnerIterator();
		// Verify type
		EXPECT_EQ(typeid(ValueFieldIterator<DIM>), typeid(*iterator));
		// Verify size
		for (std::size_t i=0; i<DIM; i++) {
			EXPECT_EQ(elementsPerDim, iterator->size(i));
		}
		// Verify (first) value
		iterator->first();
		expect_equal(values[0], iterator->currentValue());
		delete iterator;
	}

	/**
	 * Verify that procGridCoords returns a positive number smaller than the
	 * size of the processor grid in each dimension.
	 */
	void testProcGridCoord() {
		for (std::size_t d=0; d<DIM; d++) {
			expect_in_interval(block->procGridCoord(d), 0, block->procGridSize(d));
		}
	}

	/**
	 * Verify that procGridSize returns a number between 1 and the total number
	 * of processors in all dimensions.
	 */
	void testProcGridSize() {
		for (std::size_t d=0; d<DIM; d++) {
			expect_in_interval(block->procGridSize(d), 1, MPI::COMM_WORLD.Get_size()+1);
		}
	}

	/**
	 * Verify that the ghost region of a side of the block is initialized with
	 * values from the opposite boundary (Values are the same on all nodes!) when
	 * receiveDoneAt returns and the argument is initialized with values of the
	 * actual boundary. Do this for every boundary of the block.
	 */
	void testReceiveDoneAt() {
		block->startCommunication();
		BoundaryId initialized;
		BoundaryIterator<DIM> *initializedIterator = block->getBoundaryIterator();
		// Values are the same in all blocks.
		BoundaryIterator<DIM> *oppositeIterator = block->getBoundaryIterator();
		for (int i=0; i<2*DIM; i++) {
			block->receiveDoneAt(&initialized);
			initializedIterator->setBoundaryToIterate(initialized);
			BoundaryId *oppositeBoundary = initialized.oppositeSide();
			oppositeIterator->setBoundaryToIterate(*oppositeBoundary);
			while (oppositeIterator->isInField()) {
				// Check ghost region values.
				for (std::size_t distance=0; distance<extent; distance++) {
					int dir = initialized.isLowerSide() ? -1 : 1;
					double expected = oppositeIterator->currentNeighbor(initialized.getDimension(), dir * distance);
					double actual = initializedIterator->currentNeighbor(initialized.getDimension(), dir * (1+distance));
					expect_equal(expected, actual);
				}
				oppositeIterator->next();
				initializedIterator->next();
			}
			delete oppositeBoundary;
		}
		delete oppositeIterator;
		delete initializedIterator;
		block->finishCommunication();
	}

	/**
	 * Verify that setValues changes the values stored in a block to the ones in
	 * the array provided as argument and initializes the ghost regions
	 * correctly.
	 */
	void testSetValues() {
		double *newValues = new double[totalSize];
		for (std::size_t i=0; i<totalSize; i++) {
			newValues[i] = 7.8*i;
		}
		block->setValues(newValues);
		block->startCommunication();
		verifyInnerValues(*block, newValues);
		verifyGhostRegionValues(*block);
		block->finishCommunication();
		delete []newValues;
	}

private:
	double *values;

	/**
	 * Verify that the ghost values of the block provided as argument are
	 * initialized using periodic boundary conditions.
	 *
	 * @param block Block for investigation
	 */
	void verifyGhostRegionValues(ComputationalComposedBlock<DIM>& block) {
		BoundaryIterator<DIM> *boundaryIterator = block.getBoundaryIterator();
		BoundaryIterator<DIM> *expectedValuesIterator = block.getBoundaryIterator();
		BoundaryId boundary;
		for (std::size_t d=0; d<2*DIM; d++) {
			block.receiveDoneAt(&boundary);
			boundaryIterator->setBoundaryToIterate(boundary);
			BoundaryId *oppositeBoundary = boundary.oppositeSide();
			expectedValuesIterator->setBoundaryToIterate(*oppositeBoundary);
			while (boundaryIterator->isInField()) {
				// Check the ghost values outside the current element.
				for (std::size_t offset=1; offset<extent; offset++) {
					int neighborOffset = offset-1;
					int directedOffset = boundary.isLowerSide() ? -offset : offset;
					int directedNeighborOffset = boundary.isLowerSide() ? -neighborOffset : neighborOffset;
					double expected = expectedValuesIterator->currentNeighbor(boundary.getDimension(), directedNeighborOffset);
					double actual = boundaryIterator->currentNeighbor(boundary.getDimension(), directedOffset);
					expect_equal(expected, actual);
				}
				boundaryIterator->next();
				expectedValuesIterator->next();
			}
			delete oppositeBoundary;
		}
		delete expectedValuesIterator;
		delete boundaryIterator;
	}

	/**
	 * Verify that the inner values in the block provided as argument are the
	 * same as the elements of the array.
	 *
	 * @param block Block for investigation
	 * @param expectedValues Expected values of the block
	 */
	void verifyInnerValues(ComputationalBlock<DIM>& block, double *expectedValues) {
		FieldIterator<DIM> *iterator = block.getInnerIterator();
		for (std::size_t i=0; i<totalSize; i++) {
			expect_equal(expectedValues[i], iterator->currentValue());
			iterator->next();
		}
		delete iterator;
	}
};


class ComputationalComposedBlockDeathTest : public ComputationalComposedBlockTest
{
public:
	virtual void SetUp() {
		ComputationalComposedBlockTest::SetUp();
		::testing::FLAGS_gtest_death_test_style = "threadsafe";
	}

protected:
	/**
	 * On an element in the middle of each boundary: Verify that the boundary
	 * iterator causes the program to abort if it is requested to return the
	 * closest neighbor outside the ghost region outside the boundary in
	 * question, and the program is run in debug mode.
	 */
	void testCurrentBoundaryNeighbor() {
		std::size_t boundarySize = totalSize/elementsPerDim;
		BoundaryIterator<DIM> *bdIterator = block->getBoundaryIterator();
		for (std::size_t d=0; d<DIM; d++) {
			for (std::size_t side=0; side<2; side++) {
				bdIterator->setBoundaryToIterate(BoundaryId(d, 0==side));
				// Middle of the boundary
				for (std::size_t i=0; i< boundarySize/2; i++) {
					bdIterator->next();
				}
				int offset = 0==side ? -extent-1 : extent+1;
				EXPECT_ASSERT_FAIL(bdIterator->currentNeighbor(d, offset));
			}
		}
		delete bdIterator;
	}

};


/**
 * Verify the constructor
 */
TEST_F(ComputationalComposedBlockTest, TestConstructors) {
	testConstructorValues();
	testConstructorNoValues();
}

/**
 * Verify the behavior of getBoundaryIterator for a 3 dimensional block.
 */
TEST_F(ComputationalComposedBlockTest, TestBoundaryIterator) {
	testGetBoundaryIterator();
}

/**
 * Verify the behavior of getInnerIterator for a 3 dimensional block.
 */
TEST_F(ComputationalComposedBlockTest, TestInnerIterator) {
	testGetInnerIterator();
}

/**
 * Verify the behavior of receiveDoneAt (and implicitly startCommunication).
 */
TEST_F(ComputationalComposedBlockTest, TestCommunication) {
	testReceiveDoneAt();
}

/**
 * Verify the behavior of procGridCoord and procGridSize.
 */
TEST_F(ComputationalComposedBlockTest, TestProcGrid) {
	testProcGridCoord();
	testProcGridSize();
}

/**
 * Verify the behavior of setValues.
 */
TEST_F(ComputationalComposedBlockTest, TestSetValues) {
	testSetValues();
}

#ifndef MPI_LIB
/**
 * Verify the behavior of the boundary iterator when trying to access elements
 * outside the ghost regions.
 */
TEST_F(ComputationalComposedBlockDeathTest, TestBoundaryIterator) {
	testCurrentBoundaryNeighbor();
}
#endif
