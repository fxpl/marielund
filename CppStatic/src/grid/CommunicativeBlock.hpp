#ifndef COMMUNICATIVEBLOCK_HPP_
#define COMMUNICATIVEBLOCK_HPP_

#include "ComputationalBlock.hpp"
#include "src/utils/Timer.hpp"

#include <mpi.h>

namespace Haparanda {
namespace Grid {

	using namespace Utils;

	/**
	 * Computational block with ability to communicate boundary data using MPI.
	 *
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2019
	 */
	template <class InnerIteratorType, class BoundaryIteratorType> class CommunicativeBlock;

	template <template<std::size_t>class InnerIteratorType, template<std::size_t>class BoundaryIteratorType, std::size_t DIMENSIONALITY>
	class CommunicativeBlock<InnerIteratorType<DIMENSIONALITY>, BoundaryIteratorType<DIMENSIONALITY>>
		: public ComputationalBlock<InnerIteratorType<DIMENSIONALITY>, BoundaryIteratorType<DIMENSIONALITY>>
	{
	public:
		CommunicativeBlock(std::size_t elementsPerDim);

		CommunicativeBlock(std::size_t elementsPerDim, double *values);

		virtual ~CommunicativeBlock();

		/**
		 * @return The total time spent on communication in this object, in seconds
		 */
		virtual double communicationTime() const;

		/**
		 * Wait for all started send requests to finish.
		 */
		virtual void finishCommunication();

		/**
		 * @param dim Specified dimension (See return value)
		 * @return Coordinate of the current processor in processor grid along the specified dimension
		 */
		int procGridCoord(int dim) const;

		/**
		 * @param dim Specified dimension (See return value.)
		 * @return Number of processors in processor grid along the specified dimension
		 */
		int procGridSize(int dim) const;

		/**
		 * Make sure that all data is received at a boundary (and computations
		 * can be done), and initialize the argument of the method with that
		 * boundary.
		 *
		 * @param boundary Will be set to the boundary at which a side region is initialized
		 */
		virtual void receiveDoneAt(BoundaryId *boundary) = 0;

		/**
		 * Start sending and receiving data.
		 */
		virtual void startCommunication();

	protected:
		Utils::Timer *communicationTimer;
		int numProcessors[DIMENSIONALITY];
		int processorCoordinates[DIMENSIONALITY];
		int neighborRank[DIMENSIONALITY][2];
		MPI::Prequest receiveRequest[2*DIMENSIONALITY];
		MPI::Request sendRequest[2*DIMENSIONALITY];
		MPI::Cartcomm communicator;

		/**
		 * Create block data types to be sent to other processors.
		 */
		virtual void initializeBlockDataTypes() = 0;

		/**
		 * Set up the processor topology (Cartesian).
		 */
		virtual void initializeProcessorGrid();

		/**
		 * Set up processor topology and create block types for communication.
		 * This only needs to be done once!
		 */
		virtual void prepareCommunication();

		/**
		 * Get ready to receive data.
		 */
		virtual void startReceive() = 0;

		/**
		 * Start sending data.
		 */
		virtual void startSend() = 0;
	};

	template <template<std::size_t>class InnerIteratorType, template<std::size_t>class BoundaryIteratorType, std::size_t DIMENSIONALITY>
	CommunicativeBlock<InnerIteratorType<DIMENSIONALITY>, BoundaryIteratorType<DIMENSIONALITY>>::CommunicativeBlock(std::size_t elementsPerDim)
	: ComputationalBlock<InnerIteratorType<DIMENSIONALITY>, BoundaryIteratorType<DIMENSIONALITY>>(elementsPerDim) {
		this->communicationTimer = new Utils::Timer();
	}

	template <template<std::size_t>class InnerIteratorType, template<std::size_t>class BoundaryIteratorType, std::size_t DIMENSIONALITY>
	CommunicativeBlock<InnerIteratorType<DIMENSIONALITY>, BoundaryIteratorType<DIMENSIONALITY>>::CommunicativeBlock(std::size_t elementsPerDim, double *values)
	: ComputationalBlock<InnerIteratorType<DIMENSIONALITY>, BoundaryIteratorType<DIMENSIONALITY>>(elementsPerDim, values) {
		this->communicationTimer = new Utils::Timer();
	}

	template <template<std::size_t>class InnerIteratorType, template<std::size_t>class BoundaryIteratorType, std::size_t DIMENSIONALITY>
	CommunicativeBlock<InnerIteratorType<DIMENSIONALITY>, BoundaryIteratorType<DIMENSIONALITY>>::~CommunicativeBlock() {
		communicator.Free();
		delete communicationTimer;
	}

	template <template<std::size_t>class InnerIteratorType, template<std::size_t>class BoundaryIteratorType, std::size_t DIMENSIONALITY>
	double CommunicativeBlock<InnerIteratorType<DIMENSIONALITY>, BoundaryIteratorType<DIMENSIONALITY>>::communicationTime() const {
		return communicationTimer->totalElapsedTime();
	}

	template <template<std::size_t>class InnerIteratorType, template<std::size_t>class BoundaryIteratorType, std::size_t DIMENSIONALITY>
	void CommunicativeBlock<InnerIteratorType<DIMENSIONALITY>, BoundaryIteratorType<DIMENSIONALITY>>::finishCommunication() {
		this->communicationTimer->start();
		MPI::Request::Waitall(2*DIMENSIONALITY, sendRequest);
		this->communicationTimer->stop();
	}

	template <template<std::size_t>class InnerIteratorType, template<std::size_t>class BoundaryIteratorType, std::size_t DIMENSIONALITY>
	int CommunicativeBlock<InnerIteratorType<DIMENSIONALITY>, BoundaryIteratorType<DIMENSIONALITY>>::procGridCoord(int dim) const {
		return processorCoordinates[dim];
	}

	template <template<std::size_t>class InnerIteratorType, template<std::size_t>class BoundaryIteratorType, std::size_t DIMENSIONALITY>
	int CommunicativeBlock<InnerIteratorType<DIMENSIONALITY>, BoundaryIteratorType<DIMENSIONALITY>>::procGridSize(int dim) const {
		return numProcessors[dim];
	}

	template <template<std::size_t>class InnerIteratorType, template<std::size_t>class BoundaryIteratorType, std::size_t DIMENSIONALITY>
	void CommunicativeBlock<InnerIteratorType<DIMENSIONALITY>, BoundaryIteratorType<DIMENSIONALITY>>::startCommunication() {
		startReceive();
		startSend();
	}


	/*** Protected methods ***/
	template <template<std::size_t>class InnerIteratorType, template<std::size_t>class BoundaryIteratorType, std::size_t DIMENSIONALITY>
	void CommunicativeBlock<InnerIteratorType<DIMENSIONALITY>, BoundaryIteratorType<DIMENSIONALITY>>::initializeProcessorGrid() {
		bool periodicBV[DIMENSIONALITY];
		std::fill_n(periodicBV, DIMENSIONALITY, true);
		std::fill_n(this->numProcessors, DIMENSIONALITY, 0);
		int totalNumProcessors = MPI::COMM_WORLD.Get_size();
		// Set up Cartesian topology
		MPI::Compute_dims(totalNumProcessors, DIMENSIONALITY, this->numProcessors);
		this->communicator = MPI::COMM_WORLD.Create_cart(DIMENSIONALITY, this->numProcessors, periodicBV, false);
		int rank = communicator.Get_rank();
		communicator.Get_coords(rank, DIMENSIONALITY, this->processorCoordinates);
		for (std::size_t d=0; d<DIMENSIONALITY; d++) {
			communicator.Shift(d, 1, neighborRank[d][0], neighborRank[d][1]);
		}
	}

	template <template<std::size_t>class InnerIteratorType, template<std::size_t>class BoundaryIteratorType, std::size_t DIMENSIONALITY>
	void CommunicativeBlock<InnerIteratorType<DIMENSIONALITY>, BoundaryIteratorType<DIMENSIONALITY>>::prepareCommunication() {
		initializeProcessorGrid();
		initializeBlockDataTypes();
	}

} /* namespace Grid */
} /* namespace Haparanda */

#endif /* COMMUNICATIVEBLOCK_HPP_ */
