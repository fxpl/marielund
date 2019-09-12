#include "src/grid/GhostRegion.hpp"
#include "src/utils/Math.hpp"
#include "test/HaparandaTest.hpp"

#define DIM 3  // Dimensionality of the test blocks

using namespace Haparanda::Math;

namespace Haparanda {
namespace Grid {
/**
 * Unit test for GhostRegion.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017
 */
class GhostRegionTest : public HaparandaTest
{
public:

	virtual void SetUp() {
		width = 2;
		elementsPerDim = 8;
		totalSize = power(elementsPerDim, DIM-1) * width;

		values = new double[totalSize];
		for (std::size_t i=0; i<totalSize; i++) {
			values[i] = 1.2 * i;
		}
		region = new GhostRegion<DIM>(lower0, elementsPerDim, width, values);
	}

	virtual void TearDown() {
		delete region;
	}

protected:
	std::size_t width;
	std::size_t elementsPerDim;

	/**
	 * Verify that the constructor that takes the boundary and the size
	 * parameters as arguments creates a ghost region whose size along the
	 * dimension of the boundary id is width, and whose size along all other
	 * dimensions is elementsPerDim.
	 */
	void testConstructor() {
		GhostRegion<DIM> emptyRegion(lower0, elementsPerDim, width, copyOfValues());
		FieldIterator<DIM> *iterator = emptyRegion.getInnerIterator();
		EXPECT_EQ(width, iterator->size(0));
		EXPECT_EQ(elementsPerDim, iterator->size(1));
		EXPECT_EQ(elementsPerDim, iterator->size(2));
		delete iterator;
		/* Different boundaries are implicitly tested by tests for
		 * getInnerIterator() and getBoundaryIterator(). */
	}


	/**
	 * Verify that getBoundaryIterator returns a ValueFieldIterator whose size in
	 * each dimension is the same as for the ghost region. Also verify that its
	 * first value is the first value of the ghost region.
	 */
	void testGetBoundaryIterator() {
		BoundaryIterator<DIM> *iterator = region->getBoundaryIterator();
		EXPECT_EQ(typeid(ValueFieldBoundaryIterator<DIM>), typeid(*iterator));
		EXPECT_EQ(width, iterator->size(0));
		EXPECT_EQ(elementsPerDim, iterator->size(1));
		EXPECT_EQ(elementsPerDim, iterator->size(2));
		iterator->setBoundaryToIterate(lower0);
		expect_equal(values[0], iterator->currentValue());
		delete iterator;

		BoundaryId right(0, false);
		GhostRegion<DIM> rightRegion(right, elementsPerDim, width, copyOfValues());
		BoundaryIterator<DIM> *rightIterator = rightRegion.getBoundaryIterator();
		EXPECT_EQ(width, rightIterator->size(0));
		EXPECT_EQ(elementsPerDim, rightIterator->size(1));
		EXPECT_EQ(elementsPerDim, rightIterator->size(2));
		rightIterator->setBoundaryToIterate(lower0);
		expect_equal(values[0], rightIterator->currentValue());
		delete rightIterator;

		BoundaryId below(1,true);
		GhostRegion<DIM> belowRegion(below, elementsPerDim, width, copyOfValues());
		BoundaryIterator<DIM> *belowIterator = belowRegion.getBoundaryIterator();
		EXPECT_EQ(elementsPerDim, belowIterator->size(0));
		EXPECT_EQ(width, belowIterator->size(1));
		EXPECT_EQ(elementsPerDim, belowIterator->size(2));
		belowIterator->setBoundaryToIterate(lower0);
		expect_equal(values[0], belowIterator->currentValue());
		delete belowIterator;

		BoundaryId behind(2, false);
		GhostRegion<DIM> behindRegion(behind, elementsPerDim, width, copyOfValues());
		BoundaryIterator<DIM> *behindIterator = behindRegion.getBoundaryIterator();
		EXPECT_EQ(elementsPerDim, behindIterator->size(0));
		EXPECT_EQ(elementsPerDim, behindIterator->size(1));
		EXPECT_EQ(width, behindIterator->size(2));
		behindIterator->setBoundaryToIterate(lower0);
		expect_equal(values[0], behindIterator->currentValue());
		delete behindIterator;
	}

	/**
	 * Verify that getInnerIterator returns a ValueFieldIterator whose size in
	 * each dimension is the same as for the ghost region. Also verify that its
	 * first value is the first value of the ghost region.
	 */
	void testGetInnerIterator() {
		FieldIterator<DIM> *doubleIterator = region->getInnerIterator();
		EXPECT_EQ(typeid(ValueFieldIterator<DIM>), typeid(*doubleIterator));
		EXPECT_EQ(width, doubleIterator->size(0));
		EXPECT_EQ(elementsPerDim, doubleIterator->size(1));
		EXPECT_EQ(elementsPerDim, doubleIterator->size(2));
		expect_equal(values[0], doubleIterator->currentValue());
		delete doubleIterator;

		BoundaryId right(0, false);
		GhostRegion<DIM> rightRegion(right, elementsPerDim, width, copyOfValues());
		FieldIterator<DIM> *rightIterator = rightRegion.getInnerIterator();
		EXPECT_EQ(width, rightIterator->size(0));
		EXPECT_EQ(elementsPerDim, rightIterator->size(1));
		EXPECT_EQ(elementsPerDim, rightIterator->size(2));
		expect_equal(values[0], rightIterator->currentValue());
		delete rightIterator;

		BoundaryId below(1,true);
		GhostRegion<DIM> belowRegion(below, elementsPerDim, width, copyOfValues());
		FieldIterator<DIM> *belowIterator = belowRegion.getInnerIterator();
		EXPECT_EQ(elementsPerDim, belowIterator->size(0));
		EXPECT_EQ(width, belowIterator->size(1));
		EXPECT_EQ(elementsPerDim, belowIterator->size(2));
		expect_equal(values[0], belowIterator->currentValue());
		delete belowIterator;

		BoundaryId behind(2, false);
		GhostRegion<DIM> behindRegion(behind, elementsPerDim, width, copyOfValues());
		FieldIterator<DIM> *behindIterator = behindRegion.getInnerIterator();
		EXPECT_EQ(elementsPerDim, behindIterator->size(0));
		EXPECT_EQ(elementsPerDim, behindIterator->size(1));
		EXPECT_EQ(width, behindIterator->size(2));
		expect_equal(values[0], behindIterator->currentValue());
		delete behindIterator;
	}

private:
	double *copyOfValues() {
		double *result = new double[totalSize];
		for (std::size_t i=0; i<totalSize; i++) {
			result[i] = values[i];
		}
		return result;
	}

	std::size_t totalSize;
	double *values;
	BoundaryId lower0;
	GhostRegion<DIM> *region;
};


/**
 * Verify the behavior of the constructor that only takes a value array as
 * its argument.
 */
TEST_F(GhostRegionTest, TestConstructor) {
	testConstructor();
}

/**
 * Verify that getBoundaryIterator creates an adequate iterator for a 3
 * dimensional block.
 */
TEST_F(GhostRegionTest, TestBoundaryIterator) {
	testGetBoundaryIterator();
}

/**
 * Verify that getInnerIterator creates an adequate iterator for a 3
 * dimensional block.
 */
TEST_F(GhostRegionTest, TestInnerIterator) {
	testGetInnerIterator();
}

} /* namespace Grid */
} /* namespace Haparanda */
