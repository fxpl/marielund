#ifndef COMPUTATIONALCOMPOSEDBLOCKPARTEST_HPP_
#define COMPUTATIONALCOMPOSEDBLOCKPARTEST_HPP_

#include "src/grid/ComputationalComposedBlock.hpp"
#include "src/utils/Math.hpp"
#include "test/HaparandaTest.hpp"

#include <math.h>
#include <stdexcept>

#define DIM 2

namespace Haparanda {
namespace Grid {
	/**
	 * Test of boundary data communication in ComputationalComposedBlock.
	 *
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2018
	 */
	class ComputationalComposedBlockParTest : public HaparandaTest
	{
		typedef typename ComputationalComposedBlock<DIM>::BoundaryIteratorType BoundaryIteratorType;
		typedef typename ComputationalComposedBlock<DIM>::InnerIteratorType InnerIteratorType;

	public:
		virtual void SetUp () {
			block = new ComputationalComposedBlock<DIM>(elementsPerDim, extent);

			for (int d=0; d<DIM; d++) {
				numProcs[d] = block->procGridSize(d);
			}

			strides[0] = 1;
			procStrides[0] = 1;
			for (int d=1; d<DIM; d++) {
				strides[d] = strides[d-1] * elementsPerDim;
				procStrides[d] = procStrides[d-1] * numProcs[d-1];
			}
			strides[DIM] = strides[DIM-1] * elementsPerDim;

			int cartRank = 0;
			for (int d=0; d<DIM; d++) {
				cartRank += block->procGridCoord(d) * procStrides[d];
			}

			values = new double[numElements];
			for (size_t i=0; i<numElements; i++) {
				values[i] = cartRank * numElements + i;
			}
			block->setValues(values);
		}

		virtual void TearDown() {
			block->finishCommunication();
			delete []values;
			delete block;
		}


	protected:
		/**
		 * Verify that when ghostRegionInitialized returns, the ghost region on the
		 * side specified by the (output) argument is initialized with values from
		 * the boundary of the neighboring block in that direction.
		 */
		void testCommunication() {
			block->startCommunication();

			BoundaryId boundary;
			FieldIterator<BoundaryIteratorType> *bdIterator = block->getBoundaryIterator();

			for (std::size_t i = 0; i < 2 * DIM; i++) {
				// Wait for ghost region to be initialized
				block->receiveDoneAt(&boundary);
				bdIterator->setBoundaryToIterate(boundary);

				// Search in the right direction
				int offset;
				if (boundary.isLowerSide()) {
					offset = -1;
				} else {
					offset = 1;
				}

				// Compute expected value of first ghost region element
				int neighborCoords[DIM];
				for (int d=0; d<DIM; d++) {
					neighborCoords[d] = block->procGridCoord(d);
				}
				neighborCoords[boundary.getDimension()] =
						(neighborCoords[boundary.getDimension()]+numProcs[boundary.getDimension()]+offset) % numProcs[boundary.getDimension()];
				int neighborCartRank = 0;
				for (int d=0; d<DIM; d++) {
					neighborCartRank += neighborCoords[d] * procStrides[d];
				}
				double expectedValue = neighborCartRank * numElements;
				if (boundary.isLowerSide()) {
					expectedValue += strides[boundary.getDimension()] * (elementsPerDim - 1);
				}

				// Verify ghost region values
				while(bdIterator->isInField()) {
					double actualValue = bdIterator->currentNeighbor(boundary.getDimension(), offset);
					expect_equal(expectedValue, actualValue);
					int stride = strides[boundary.getDimension()];
					if (((int)expectedValue + 1) % stride == 0) {
						int strideNextDim = strides[boundary.getDimension()+1];
						expectedValue += strideNextDim - (stride - 1);
					} else {
						expectedValue++;
					}
					bdIterator->next();
				}

			}
			delete bdIterator;
			block->finishCommunication();
		}

	private:
		CommunicativeBlock<InnerIteratorType, BoundaryIteratorType> *block;
		int numProcs[DIM];
		int procStrides[DIM];
		const std::size_t elementsPerDim = 3;
		const std::size_t numElements = Haparanda::Math::power(elementsPerDim, DIM);
		const std::size_t extent = 1;
		// Distance between element on boundary along <array index>
		std::size_t strides[DIM+1];
		double *values;

	};


	/**
	 * Test that ghost regions are initialized correctly with data from
	 * neighbor nodes.
	 */
	TEST_F(ComputationalComposedBlockParTest, TestCommunication) {
		testCommunication();
	}

}	/* namespace Grid */
}	/* namespace Haparanda */

#endif /* COMPUTATIONALCOMPOSEDBLOCKPARTEST_HPP_ */
