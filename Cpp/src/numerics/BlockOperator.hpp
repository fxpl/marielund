#ifndef BLOCKOPERATOR_HPP_
#define BLOCKOPERATOR_HPP_

#include "src/grid/ComputationalComposedBlock.hpp"

namespace Haparanda {
namespace Numerics {

	/**
	 * Class representing a local operator that can be applied on one block at
	 * the time.
	 *
	 * @tparam DIMENSIONALITY Dimensionality of the operator
	 * @tparam ORDER_OF_ACCURACY Order of accuracy of the operator, i.e. the extent * 2
	 * @author Malin Kallen, Magnus Grandin
	 */
	template<std::size_t DIMENSIONALITY, std::size_t ORDER_OF_ACCURACY>
	class BlockOperator
	{
	public:
		typedef Grid::CommunicativeBlock<DIMENSIONALITY> CommunicativeBlock;
		typedef Grid::ComputationalBlock<DIMENSIONALITY> ComputationalBlock;

		BlockOperator();

		virtual ~BlockOperator();

		/**
		 * Apply the operator on the provided data.
		 *
		 * @param input Block representing the data on which the operator will be applied
		 * @param result Block to which the result will be written
		 */
		void apply(CommunicativeBlock& input, ComputationalBlock *result) const;

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
		virtual void applyInBoundaryRegion(const ComputationalBlock& input, ComputationalBlock *result, const BoundaryId& boundary) const = 0;

		/**
		 * Apply the operator on the boundary part, i.e. where receiving of ghost
		 * data must be finished before the application can be done.
		 *
		 * @param input Block representing the data on which the stencil will be applied
		 * @param result Block to which the result will be written
		 */
		void applyInBoundaryRegions(CommunicativeBlock& input, ComputationalBlock *result) const;

		/**
		 * Apply the operator in the inner part of the region represented by the
		 * arguments.
		 *
		 * @param input Block representing the data on which the operator will be applied
		 * @param result Block to which the result will be written
		 */
		virtual void applyInInnerRegion(const ComputationalBlock& input, ComputationalBlock *result) const = 0;

	};

	template<std::size_t DIMENSIONALITY, std::size_t ORDER_OF_ACCURACY>
	BlockOperator<DIMENSIONALITY, ORDER_OF_ACCURACY>::BlockOperator() {
		computationTimer   = new Utils::Timer();
	}

	template<std::size_t DIMENSIONALITY, std::size_t ORDER_OF_ACCURACY>
	BlockOperator<DIMENSIONALITY, ORDER_OF_ACCURACY>::~BlockOperator() {
		delete computationTimer;
	}

	template<std::size_t DIMENSIONALITY, std::size_t ORDER_OF_ACCURACY>
	void BlockOperator<DIMENSIONALITY, ORDER_OF_ACCURACY>::apply(CommunicativeBlock& input, ComputationalBlock *result) const {
		// Compute the inner parts.
		computationTimer->start();
		applyInInnerRegion(input, result);
		computationTimer->stop();

		// Compute boundary parts
		applyInBoundaryRegions(input, result);
	}

	template<std::size_t DIMENSIONALITY, std::size_t ORDER_OF_ACCURACY>
	void BlockOperator<DIMENSIONALITY, ORDER_OF_ACCURACY>::applyInBoundaryRegions(CommunicativeBlock& input, ComputationalBlock *result) const {
		BoundaryId boundary;
		for (std::size_t d = 0; d < 2 * DIMENSIONALITY; d++) {
			// Find a ghost region that is initialized
			input.receiveDoneAt(&boundary);
			this->computationTimer->start();
			applyInBoundaryRegion(input, result, boundary);
			this->computationTimer->stop();
		}
	}

	template<std::size_t DIMENSIONALITY, std::size_t ORDER_OF_ACCURACY>
	double BlockOperator<DIMENSIONALITY, ORDER_OF_ACCURACY>::computationTime() const {
		return computationTimer->totalElapsedTime();
	}

} /* namespace Numerics */
} /* namespace Haparanda */

#endif /* BLOCKOPERATOR_HPP_ */
