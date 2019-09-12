#include "src/grid/ComputationalPureBlock.hpp"
#include "test/HaparandaTest.hpp"
#include "src/utils/Math.hpp"

#define DIM 3  // Dimensionality of the test blocks

using namespace Haparanda::Grid;
using namespace Haparanda::Math;

/**
 * Unit test for ComputationalPureBlocks.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2014, 2017
 */
class ComputationalPureBlockTest : public HaparandaTest
{
public:

	virtual void SetUp() {
		elementsPerDim = 10;
		totalSize = power(elementsPerDim, DIM);

		values = new double[totalSize];
		for (std::size_t i=0; i<totalSize; i++) {
			values[i] = 1.2 * i;
		}
		block = new ComputationalPureBlock<DIM>(elementsPerDim, values);
	}

	virtual void TearDown() {
		delete block;
		delete []values;
	}

protected:
	std::size_t elementsPerDim;
	ComputationalPureBlock<DIM> *block;

	/**
	 * Verify that the constructor that only takes the values as argument
	 * creates a block with the first value of the value array as its first
	 * element.
	 */
	void testConstructor() {
		ComputationalPureBlock<DIM> blockWithoutIndexBlock(elementsPerDim, values);
		FieldIterator<DIM> *iterator = blockWithoutIndexBlock.getInnerIterator();
		// Verify (first) value
		iterator->first();
		expect_equal(values[0], iterator->currentValue());
		delete iterator;
	}

	/**
	 * Verify that getBoundaryIterator returns a ValueFieldBoundaryIterator
	 * whose size in each dimension is elementsPerDim and whose first value is
	 * the first one in the value array.
	 */
	void testGetBoundaryIterator() {
		BoundaryIterator<DIM> *iterator = block->getBoundaryIterator();
		// Verify type
		EXPECT_EQ(typeid(ValueFieldBoundaryIterator<DIM>), typeid(*iterator));
		// Verify size
		for (std::size_t i=0; i<DIM; i++) {
			EXPECT_EQ(elementsPerDim, iterator->size(i));
		}
		// Verify (first) value
		iterator->setBoundaryToIterate(BoundaryId(0, true));
		expect_equal(values[0], iterator->currentValue());
		delete iterator;
	}

	/**
	 * Verify that getInnerIterator returns a ValueFieldIterator whose size in
	 * each dimension is elementsPerDim and whose first value is the one first
	 * one in the value array.
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


private:
	std::size_t totalSize;
	double *values;
};

/**
 * Verify the behavior of the constructor which only takes a value array as
 * its argument.
 */
TEST_F(ComputationalPureBlockTest, TestConstructor) {
	testConstructor();
}

/**
 * Verify that getBoundaryIterator creates an adequate iterator for a 3
 * dimensional block.
 */
TEST_F(ComputationalPureBlockTest, TestBoundaryIterator) {
	testGetBoundaryIterator();
}

/**
 * Verify that getInnerIterator creates an adequate iterator for a 3
 * dimensional block.
 */
TEST_F(ComputationalPureBlockTest, TestInnerIterator) {
	testGetInnerIterator();
}
