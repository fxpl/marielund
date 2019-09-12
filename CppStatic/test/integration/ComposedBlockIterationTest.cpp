#include "src/grid/ComputationalComposedBlock.hpp"
#include "test/HaparandaTest.hpp"

using namespace Haparanda;
using namespace Grid;

#define DIM 3

/**
 * Integration test for ComputationalComposedBlock + ComposedFieldBoundaryIterator.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2014, 2017-2018
 */
class ComposedBlockIterationTest : public HaparandaTest
{
public:
	virtual void SetUp() {
		elementsPerDim = 10;
		extent = 4;
		std::size_t numElements = Math::power(elementsPerDim, DIM);
		boundarySize = numElements/elementsPerDim;
		stride = {1, elementsPerDim, elementsPerDim*elementsPerDim};
		interBlockStride = {numElements, 3*numElements, 0};

		values = new double[numElements];
		for (std::size_t i=0; i<numElements; i++) {
			values[i] = 2.1 * i;
		}
		composedBlock = new ComputationalComposedBlock<DIM>(elementsPerDim, extent, values);
		boundaryIterator = composedBlock->getBoundaryIterator();
	}

	virtual void TearDown() {
		delete boundaryIterator;
		delete composedBlock;
		delete []values;
	}


protected:
	typedef typename ComputationalComposedBlock<DIM>::InnerIteratorType InnerIteratorType;
	typedef typename ComputationalComposedBlock<DIM>::BoundaryIteratorType BoundaryIteratorType;

	std::size_t extent;
	std::size_t boundarySize;
	FieldIterator<BoundaryIteratorType> *boundaryIterator;

	/**
	 * Step through the boundaries of the main region of the composed block and
	 * try to fetch all neighbors in direction i, where |i| is the dimension
	 * in which the value is constant on the current boundary and the sign of i
	 * is set so that the requested neighbor is in a the ghost region. Verify
	 * that a value between 0 and 1 was returned.
	 */
	void testCurrentBoundaryNeighborInGhostRegion() {
		composedBlock->startCommunication();
		FieldIterator<BoundaryIteratorType> *expectedIterator = composedBlock->getBoundaryIterator(); // Values are the same on all nodes
		BoundaryId boundary;
		for (std::size_t d=0; d<2*DIM; d++) {
			composedBlock->receiveDoneAt(&boundary);
				boundaryIterator->setBoundaryToIterate(boundary);
				BoundaryId *oppositeBoundary = boundary.oppositeSide();
			expectedIterator->setBoundaryToIterate(*oppositeBoundary);
				for (std::size_t i2=0; i2<elementsPerDim; i2++) {
					for (std::size_t i1=0; i1<elementsPerDim; i1++) {
						for (std::size_t offset=1; offset<=extent; offset++) {
						double expectedValue, actualValue;
						if (boundary.isLowerSide()) {
								expectedValue = expectedIterator->currentNeighbor(boundary.getDimension(), -offset+1);
								actualValue = boundaryIterator->currentNeighbor(boundary.getDimension(), -offset);
							} else {
								expectedValue = expectedIterator->currentNeighbor(boundary.getDimension(), offset-1);
								actualValue = boundaryIterator->currentNeighbor(boundary.getDimension(), offset);
							}
							expect_equal(expectedValue, actualValue);
						}
						boundaryIterator->next();
						expectedIterator->next();
					}
				}
			delete oppositeBoundary;
		}
		delete expectedIterator;
		composedBlock->finishCommunication();
	}

	/**
	 * Step through the boundaries of the main region of the composed block and
	 * try to fetch the closest neighbor in direction i, where |i| is the
	 * dimension in which the value is constant on the current boundary and the
	 * sign of i is set so that the requested neighbor is inside the main
	 * region. Verify that the right value was returned.
	 */
	void testCurrentBoundaryNeighborInside() {
		for (std::size_t d=0; d<DIM; d++) {
			std::size_t otherDim1 = 0==d ? 1 : 0;
			std::size_t otherDim2 = 2==d ? 1 : 2;
			for (std::size_t side=0; side<2; side++) {
				boundaryIterator->setBoundaryToIterate(BoundaryId(d, 0==side));
				for (std::size_t i2=0; i2<elementsPerDim; i2++) {
					for (std::size_t i1=0; i1<elementsPerDim; i1++) {
						std::size_t index = side*(elementsPerDim-1)*stride[d]
								+ i1*stride[otherDim1]+ i2*stride[otherDim2];
						int dir = 0==side ? 1 : -1;
						expect_equal(values[index+dir*stride[d]], boundaryIterator->currentNeighbor(d, dir));
						boundaryIterator->next();
					}
				}
			}
		}
	}

	/**
	 * Fetch the value of each boundary element in the composed block and verify
	 * that the correct value was returned.
	 */
	void testCurrentBoundaryValue() {
		for (std::size_t d=0; d<DIM; d++) {
			std::size_t otherDim1 = 0==d ? 1 : 0;
			std::size_t otherDim2 = 2==d ? 1 : 2;
			for (std::size_t side=0; side<2; side++) {
				boundaryIterator->setBoundaryToIterate(BoundaryId(d, 0==side));
				for (std::size_t i2=0; i2<elementsPerDim; i2++) {
					for (std::size_t i1=0; i1<elementsPerDim; i1++) {
						std::size_t index = side*(elementsPerDim-1)*stride[d]
								+ i1*stride[otherDim1]+ i2*stride[otherDim2];
						expect_equal(values[index], boundaryIterator->currentValue());
						boundaryIterator->next();
					}
				}
			}
		}
	}

private:
	std::size_t elementsPerDim;
	std::array<std::size_t, DIM> stride;
	std::array<std::size_t, DIM> interBlockStride;
	double *values;

	ComputationalComposedBlock<DIM> *composedBlock;
};


/**
 * For a non-cubic ComputationalComposedBlock, verify that values (including
 * neighbor values) are fetched correctly along the boundary.
 */
TEST_F(ComposedBlockIterationTest, TestBoundaryIterator) {
	testCurrentBoundaryValue();
	testCurrentBoundaryNeighborInside();
	testCurrentBoundaryNeighborInGhostRegion();
}
