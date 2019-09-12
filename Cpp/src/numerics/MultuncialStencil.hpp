#ifndef MULTUNCIALSTENCIL_HPP_
#define MULTUNCIALSTENCIL_HPP_

#include "BlockOperator.hpp"

namespace Haparanda {
namespace Numerics {

	/**
	 * Class representing a multuncial stencil.
	 *
	 * @tparam DIMENSIONALITY Dimensionality of the stencil
	 * @tparam ORDER_OF_ACCURACY Order of accuracy of the stencil, i.e. the extent * 2
	 * @author Malin Kallen, Magnus Grandin
	 */
	template<std::size_t DIMENSIONALITY, std::size_t ORDER_OF_ACCURACY>
	class MultuncialStencil: public BlockOperator<DIMENSIONALITY, ORDER_OF_ACCURACY>
	{
	public:
		virtual ~MultuncialStencil();

	protected:
		typedef typename BlockOperator<DIMENSIONALITY, ORDER_OF_ACCURACY>::CommunicativeBlock CommunicativeBlock;
		typedef typename BlockOperator<DIMENSIONALITY, ORDER_OF_ACCURACY>::ComputationalBlock ComputationalBlock;
		typedef Iterators::BoundaryIterator<DIMENSIONALITY> BoundaryIterator;
		typedef Iterators::FieldIterator<DIMENSIONALITY> FieldIterator;

		static const unsigned int EXTENT = ORDER_OF_ACCURACY/2;

		virtual void applyInBoundaryRegion(const ComputationalBlock& input, ComputationalBlock *result, const BoundaryId& boundary) const;

		virtual void applyInInnerRegion(const ComputationalBlock& input, ComputationalBlock *result) const;

		/**
		 * Get the stencil weight at the specified index along the specified
		 * dimension for the point currently pointed at by the provided
		 * iterator.
		 *
		 * @param iterator Iterator of the current point
		 * @param dim Dimension for which the weight will be fetched
		 * @param weightIndex Index of the weight (in the specified dimension)
		 */
		virtual double getWeight(const Iterators::FieldIterator<DIMENSIONALITY>& iterator, std::size_t dim, int weightIndex) const = 0;
	};

	template<std::size_t DIMENSIONALITY, std::size_t ORDER_OF_ACCURACY>
	MultuncialStencil<DIMENSIONALITY, ORDER_OF_ACCURACY>::~MultuncialStencil() {
	}


	/*** Protected methods ***/
	template<std::size_t DIMENSIONALITY, std::size_t ORDER_OF_ACCURACY>
	void MultuncialStencil<DIMENSIONALITY, ORDER_OF_ACCURACY>::applyInBoundaryRegion(const ComputationalBlock& input, ComputationalBlock *result, const BoundaryId& boundary) const {
#pragma omp parallel
		{
			BoundaryIterator *inputIterator = input.getBoundaryIterator();
			inputIterator->setBoundaryToIterate(boundary);
			BoundaryIterator *resultIterator = result->getBoundaryIterator();
			resultIterator->setBoundaryToIterate(boundary);
			std::size_t dim = boundary.getDimension();
			int lowestWeightIndex = boundary.isLowerSide() ? 0 : EXTENT + 1;
			int dir = boundary.isLowerSide() ? 1 : -1;
			int maxDistanceFromBoundary = dir * EXTENT;
			while (inputIterator->isInField()) {
				for (int distanceFromBoundary=0; distanceFromBoundary!=maxDistanceFromBoundary; distanceFromBoundary+=dir) {
					/* Apply left part of stencil if being on the lower boundary
					   and the right part of the stencil if being on the upper one. */
					double resultValue = resultIterator->currentNeighbor(dim, distanceFromBoundary);
					for (std::size_t i=0; i<EXTENT; i++) {
						int weightIndex = lowestWeightIndex + i;
						int offset = lowestWeightIndex - EXTENT + distanceFromBoundary + i;
						resultValue += getWeight(*inputIterator, dim, weightIndex) * inputIterator->currentNeighbor(dim, offset);
					}
					resultIterator->setCurrentNeighbor(dim, distanceFromBoundary, resultValue);
				}
				inputIterator->next();
				resultIterator->next();
			}
			assert(!resultIterator->isInField());
			delete inputIterator;
			delete resultIterator;
		} // pragma omp parallel
	}

	template<std::size_t DIMENSIONALITY, std::size_t ORDER_OF_ACCURACY>
	void MultuncialStencil<DIMENSIONALITY, ORDER_OF_ACCURACY>::applyInInnerRegion(const ComputationalBlock& input, ComputationalBlock *result) const {
		std::size_t sizePerDim = input.getElementsPerDim();
#pragma omp parallel
		{
			FieldIterator *inputIterator = input.getInnerIterator();
			FieldIterator *resultIterator = result->getInnerIterator();

			while(inputIterator->isInField()) {
				// Apply the stencil in each dimension
				double resultValue = 0;
				for (std::size_t d=0; d<DIMENSIONALITY; d++) {
					std::size_t indexAlongD = inputIterator->currentIndex(d);
					// Left part of stencil
					if (indexAlongD >= EXTENT) {
						for (std::size_t i=0; i<EXTENT; i++) {
							resultValue += getWeight(*inputIterator, d, i) * inputIterator->currentNeighbor(d, -EXTENT+i);
						}
					}
					// Center weight
					resultValue += getWeight(*inputIterator, d, EXTENT) * inputIterator->currentValue();
					// Right part of stencil
					if (indexAlongD+EXTENT < sizePerDim) {
						for (std::size_t i=1; i<=EXTENT; i++) {
							resultValue += getWeight(*inputIterator, d, EXTENT+i) * inputIterator->currentNeighbor(d, i);
						}
					}
				}
				resultIterator->setCurrentValue(resultValue);
				inputIterator->next();
				resultIterator->next();
			}
			assert(!resultIterator->isInField());
			delete inputIterator;
			delete resultIterator;
		} // pragma omp parallel
	}

} /* namespace Numerics */
} /* namespace Haparanda */

#endif /* MULTUNCIALSTENCIL_HPP_ */
