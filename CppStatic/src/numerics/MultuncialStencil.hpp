#ifndef MULTUNCIALSTENCIL_HPP_
#define MULTUNCIALSTENCIL_HPP_

#include "BlockOperator.hpp"

namespace Haparanda {
namespace Numerics {

	template<class OperatorType, std::size_t ORDER_OF_ACCURACY> class MultuncialStencil;

	/**
	 * Class representing a multuncial stencil.
	 *
	 * @tparam OperatorType Type of concrete sub class implementing this class (see CRTP)
	 * @tparam DIMENSIONALITY Dimensionality of the stencil
	 * @tparam ORDER_OF_ACCURACY Order of accuracy of the stencil, i.e. the extent * 2
	 * @author Malin Kallen, Magnus Grandin
	 */
	template<template<std::size_t>class OperatorType, std::size_t DIMENSIONALITY, std::size_t ORDER_OF_ACCURACY>
	class MultuncialStencil<OperatorType<DIMENSIONALITY>, ORDER_OF_ACCURACY>
	: public BlockOperator<OperatorType<DIMENSIONALITY>, ORDER_OF_ACCURACY>
	{
	public:
		virtual ~MultuncialStencil();

	protected:
		typedef typename BlockOperator<OperatorType<DIMENSIONALITY>, ORDER_OF_ACCURACY>::InputBlockType InputBlockType;
		typedef typename BlockOperator<OperatorType<DIMENSIONALITY>, ORDER_OF_ACCURACY>::OutputBlockType OutputBlockType;
		typedef typename BlockOperator<OperatorType<DIMENSIONALITY>, ORDER_OF_ACCURACY>::InputBlock InputBlock;
		typedef typename BlockOperator<OperatorType<DIMENSIONALITY>, ORDER_OF_ACCURACY>::OutputBlock OutputBlock;
		typedef typename BlockOperator<OperatorType<DIMENSIONALITY>, ORDER_OF_ACCURACY>::InputBoundaryIterator InputBoundaryIterator;
		typedef typename BlockOperator<OperatorType<DIMENSIONALITY>, ORDER_OF_ACCURACY>::OutputBoundaryIterator OutputBoundaryIterator;
		typedef typename InputBlockType::InnerIteratorType InputInnerIteratorType;
		typedef typename BlockOperator<OperatorType<DIMENSIONALITY>, ORDER_OF_ACCURACY>::OutputInnerIteratorType OutputInnerIteratorType;
		typedef FieldIterator<InputInnerIteratorType> InputInnerIterator;
		typedef FieldIterator<OutputInnerIteratorType> OutputInnerIterator;

		static const unsigned int EXTENT = ORDER_OF_ACCURACY/2;

		virtual void applyInBoundaryRegion(const InputBlock& input, OutputBlock *result, const BoundaryId& boundary) const;

		virtual void applyInInnerRegion(const InputBlock& input, OutputBlock *result) const;

		/**
		 * Get the stencil weight at the specified index along the specified
		 * dimension for the point currently pointed at by the provided
		 * iterator.
		 *
		 * @tparam IteratorType Concrete type of iterator
		 * @param iterator Iterator of the current point
		 * @param dim Dimension for which the weight will be fetched
		 * @param weightIndex Index of the weight (in the specified dimension)
		 */
		template<typename IteratorType>
		double getWeight(const Iterators::FieldIterator<IteratorType>& iterator, std::size_t dim, int weightIndex) const;
	};

	template<template<std::size_t>class OperatorType, std::size_t DIMENSIONALITY, std::size_t ORDER_OF_ACCURACY>
	MultuncialStencil<OperatorType<DIMENSIONALITY>, ORDER_OF_ACCURACY>::~MultuncialStencil() {
	}


	/*** Protected methods ***/
	template<template<std::size_t>class OperatorType, std::size_t DIMENSIONALITY, std::size_t ORDER_OF_ACCURACY>
	void MultuncialStencil<OperatorType<DIMENSIONALITY>, ORDER_OF_ACCURACY>::applyInBoundaryRegion(const InputBlock& input, OutputBlock *result, const BoundaryId& boundary) const {
#pragma omp parallel
		{
			InputBoundaryIterator *inputIterator = input.getBoundaryIterator();
			inputIterator->setBoundaryToIterate(boundary);
			OutputBoundaryIterator *resultIterator = result->getBoundaryIterator();
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

	template<template<std::size_t>class OperatorType, std::size_t DIMENSIONALITY, std::size_t ORDER_OF_ACCURACY>
	void MultuncialStencil<OperatorType<DIMENSIONALITY>, ORDER_OF_ACCURACY>::applyInInnerRegion(const InputBlock& input, OutputBlock *result) const {
		std::size_t sizePerDim = input.getElementsPerDim();
#pragma omp parallel
		{
			InputInnerIterator *inputIterator = input.getInnerIterator();
			OutputInnerIterator *resultIterator = result->getInnerIterator();
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

	template<template<std::size_t>class OperatorType, std::size_t DIMENSIONALITY, std::size_t ORDER_OF_ACCURACY>
	template<typename IteratorType>
	double MultuncialStencil<OperatorType<DIMENSIONALITY>, ORDER_OF_ACCURACY>::getWeight(const Iterators::FieldIterator<IteratorType>& iterator, std::size_t dim, int weightIndex) const {
		return static_cast<const OperatorType<DIMENSIONALITY>&>(*this).getWeight(iterator, dim, weightIndex);
	}

} /* namespace Numerics */
} /* namespace Haparanda */

#endif /* MULTUNCIALSTENCIL_HPP_ */
