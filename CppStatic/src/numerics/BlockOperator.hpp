#ifndef BLOCKOPERATOR_HPP_
#define BLOCKOPERATOR_HPP_

#include "src/grid/ComputationalComposedBlock.hpp"
#include "src/grid/ComputationalPureBlock.hpp"

namespace Haparanda {
namespace Numerics {

	template<class OperatorType, std::size_t ORDER_OF_ACCURACY> class BlockOperator;

	/**
	 * Class representing a local operator that can be applied on one block at
	 * the time.
	 *
	 * @tparam OperatorType Type of concrete sub class implementing this class (see CRTP)
	 * @tparam DIMENSIONALITY Dimensionality of the operator
	 * @tparam ORDER_OF_ACCURACY Order of accuracy of the operator, i.e. the extent * 2
	 * @author Malin Kallen, Magnus Grandin
	 */
	template<template<std::size_t>class OperatorType, std::size_t DIMENSIONALITY, std::size_t ORDER_OF_ACCURACY>
	class BlockOperator<OperatorType<DIMENSIONALITY>, ORDER_OF_ACCURACY>
	{
	public:
		typedef Grid::ComputationalComposedBlock<DIMENSIONALITY> InputBlockType;
		typedef Grid::ComputationalPureBlock<DIMENSIONALITY> OutputBlockType;
		typedef typename InputBlockType::BoundaryIteratorType InputBoundaryIteratorType;
		typedef typename OutputBlockType::BoundaryIteratorType OutputBoundaryIteratorType;
		typedef FieldIterator<InputBoundaryIteratorType> InputBoundaryIterator;
		typedef FieldIterator<OutputBoundaryIteratorType> OutputBoundaryIterator;
		typedef typename InputBlockType::InnerIteratorType InputInnerIteratorType;
		typedef typename OutputBlockType::InnerIteratorType OutputInnerIteratorType;
		typedef Grid::CommunicativeBlock<InputInnerIteratorType, InputBoundaryIteratorType> InputBlock;
		typedef Grid::ComputationalBlock<OutputInnerIteratorType, OutputBoundaryIteratorType> OutputBlock;

		BlockOperator();

		virtual ~BlockOperator();

		/**
		 * Apply the operator on the provided data.
		 *
		 * @param input Block representing the data on which the operator will be applied
		 * @param result Block to which the result will be written
		 */
		void apply(InputBlock& input, OutputBlock *result) const;

		/**
		* @return The total time spent on actual computations by this stencil, in seconds
		*/
		double computationTime() const;

	protected:

		// Timer for the computations
		Haparanda::Utils::Timer *computationTimer;

		/**
		 * Apply the operator close to the boundary represented by the last
		 * argument, that is where receiving of ghost data outside that boundary
		 * must be finished before the application can be done.
		 *
		 * @param input Block representing the data on which the stencil will be applied
		 * @param result Block to which the result will be written
		 * @param boundary Boundary along which the stencil will be applied
		 */
		virtual void applyInBoundaryRegion(const InputBlock& input, OutputBlock* result, const BoundaryId& boundary) const = 0;

		/**
		 * Apply the operator on the boundary part, i.e. where receiving of ghost
		 * data must be finished before the application can be done.
		 *
		 * @param input Block representing the data on which the stencil will be applied
		 * @param result Block to which the result will be written
		 */
		void applyInBoundaryRegions(InputBlock& input, OutputBlock *result) const;

		/**
		 * Apply the operator in the inner part of the region represented by the
		 * arguments.
		 *
		 * @param input Block representing the data on which the operator will be applied
		 * @param result Block to which the result will be written
		 */
		virtual void applyInInnerRegion(const InputBlock& input, OutputBlock *result) const = 0;

	};

	template<template<std::size_t>class OperatorType, std::size_t DIMENSIONALITY, std::size_t ORDER_OF_ACCURACY>
	BlockOperator<OperatorType<DIMENSIONALITY>, ORDER_OF_ACCURACY>::BlockOperator() {
		computationTimer = new Utils::Timer();
	}

	template<template<std::size_t>class OperatorType, std::size_t DIMENSIONALITY, std::size_t ORDER_OF_ACCURACY>
	BlockOperator<OperatorType<DIMENSIONALITY>, ORDER_OF_ACCURACY>::~BlockOperator() {
		delete computationTimer;
	}

	template<template<std::size_t>class OperatorType, std::size_t DIMENSIONALITY, std::size_t ORDER_OF_ACCURACY>
	void BlockOperator<OperatorType<DIMENSIONALITY>, ORDER_OF_ACCURACY>::apply(InputBlock& input, OutputBlock *result) const {
		// Compute the inner parts.
		computationTimer->start();
		applyInInnerRegion(input, result);
		computationTimer->stop();

		// Compute boundary parts
		applyInBoundaryRegions(input, result);
	}

	template<template<std::size_t>class OperatorType, std::size_t DIMENSIONALITY, std::size_t ORDER_OF_ACCURACY>
	void BlockOperator<OperatorType<DIMENSIONALITY>, ORDER_OF_ACCURACY>::applyInBoundaryRegions(InputBlock& input, OutputBlock *result) const {
		BoundaryId boundary;
		for (std::size_t d = 0; d < 2 * DIMENSIONALITY; d++) {
			// Find a ghost region that is initialized
			input.receiveDoneAt(&boundary);
			this->computationTimer->start();
			applyInBoundaryRegion(input, result, boundary);
			this->computationTimer->stop();
		}
	}

	template<template<std::size_t>class OperatorType, std::size_t DIMENSIONALITY, std::size_t ORDER_OF_ACCURACY>
	double BlockOperator<OperatorType<DIMENSIONALITY>, ORDER_OF_ACCURACY>::computationTime() const {
		return computationTimer->totalElapsedTime();
	}

} /* namespace Numerics */
} /* namespace Haparanda */

#endif /* BLOCKOPERATOR_HPP_ */
