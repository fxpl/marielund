#ifndef COMPUTATIONALCOMPOSEDBLOCK_HPP_
#define COMPUTATIONALCOMPOSEDBLOCK_HPP_

#include "CommunicativeBlock.hpp"
#include "GhostRegion.hpp"
#include "src/iterators/ComposedFieldBoundaryIterator.hpp"

#include <algorithm>

using namespace Haparanda::Iterators;

namespace Haparanda {
namespace Grid {

	/**
	 * Computational block with the inner values and the ghost regions in
	 * separate structures.
	 *
	 * Note that this type of block assumes the element indices to be
	 * consecutive!
	 *
	 * @tparam DIMENSIONALITY Dimensionality of the block
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2013-2014, 2017-2019
	 */
	template <std::size_t DIMENSIONALITY>
	class ComputationalComposedBlock: public CommunicativeBlock<DIMENSIONALITY>
	{
	public:
		/**
		 * Create ghost regions and initialize everything MPI related.
		 *
		 * @param elementsPerDim Block size in each dimension
		 * @param extent Width of ghost regions
		 */
		ComputationalComposedBlock(std::size_t elementsPerDim, std::size_t extent);

		/**
		 * Create ghost regions and initialize everything MPI related.
		 * Initialize the block with its values.
		 *
		 * @param elementsPerDim Block size in each dimension
		 * @param extent Width of ghost regions
		 * @param values Array containing values to be stored in this block
		 */
		ComputationalComposedBlock(std::size_t elementsPerDim, std::size_t extent, double *values);

		virtual ~ComputationalComposedBlock();

		virtual BoundaryIterator<DIMENSIONALITY> *getBoundaryIterator() const;

		virtual FieldIterator<DIMENSIONALITY> *getInnerIterator() const;

		virtual void receiveDoneAt(BoundaryId *boundary);

	protected:
		virtual void initializeBlockDataTypes();

		/**
		 * Note that the requests are only initialized and started if values is
		 * set!
		 */
		virtual void startReceive();

		/**
		 * Note that the requests are only initialized and started if values is
		 * set!
		 */
		virtual void startSend();

	private:
		GhostRegion<DIMENSIONALITY> *ghostRegions[DIMENSIONALITY][2];
		std::size_t extent;  // Size in dimension i of ghost regions located along the boundaries where x_i is constant
		MPI::Datatype commDataBlockTypes[DIMENSIONALITY];

		/**
		 * Allocate memory for the ghost regions.
		 */
		void createGhostRegions();

		/**
		 * Start initialization of all ghost regions.
		 *
		 */
		void startSendingGhostData();
	};

	template <std::size_t DIMENSIONALITY>
	ComputationalComposedBlock<DIMENSIONALITY>::ComputationalComposedBlock(std::size_t elementsPerDim, std::size_t extent)
	: CommunicativeBlock<DIMENSIONALITY>(elementsPerDim) {
		this->extent = extent;
		createGhostRegions();
		this->prepareCommunication();
	}

	template <std::size_t DIMENSIONALITY>
	ComputationalComposedBlock<DIMENSIONALITY>::ComputationalComposedBlock(std::size_t elementsPerDim, std::size_t extent, double *values)
	: CommunicativeBlock<DIMENSIONALITY>(elementsPerDim, values) {
		this->extent = extent;
		createGhostRegions();
		this->prepareCommunication();
	}

	template <std::size_t DIMENSIONALITY>
	ComputationalComposedBlock<DIMENSIONALITY>::~ComputationalComposedBlock() {
		for (std::size_t i=0; i<DIMENSIONALITY; i++) {
			for (std::size_t j=0; j<2; j++) {
				delete ghostRegions[i][j];
			}
			commDataBlockTypes[i].Free();
		}
	}

	template <std::size_t DIMENSIONALITY>
	inline BoundaryIterator<DIMENSIONALITY> *ComputationalComposedBlock<DIMENSIONALITY>::getBoundaryIterator() const {
		assert(NULL != this->values);
		std::array<std::size_t, DIMENSIONALITY> sizes = this->getSizeArray();
		FieldIterator<DIMENSIONALITY> ***ghostIterators
		= new FieldIterator<DIMENSIONALITY>**[DIMENSIONALITY];
		for (std::size_t i=0; i<DIMENSIONALITY; i++) {
			ghostIterators[i] = new FieldIterator<DIMENSIONALITY>*[2];
			for (std::size_t j=0; j<2; j++) {
				ghostIterators[i][j] = ghostRegions[i][j]->getBoundaryIterator();
			}
		}
		return new ComposedFieldBoundaryIterator<DIMENSIONALITY>(
				sizes, &(this->values[this->smallestIndex]), ghostIterators);
	}

	template <std::size_t DIMENSIONALITY>
	inline FieldIterator<DIMENSIONALITY> *ComputationalComposedBlock<DIMENSIONALITY>::getInnerIterator() const {
		assert(NULL != this->values);
		std::array<std::size_t, DIMENSIONALITY> sizes = this->getSizeArray();
		return new ValueFieldIterator<DIMENSIONALITY>(sizes, &(this->values[this->smallestIndex]));
	}

	template <std::size_t DIMENSIONALITY>
	void ComputationalComposedBlock<DIMENSIONALITY>::receiveDoneAt(BoundaryId *boundary) {
		this->communicationTimer->start();
		int index = MPI::Request::Waitany(2*DIMENSIONALITY, this->receiveRequest);
		boundary->setDimension(index/2);
		boundary->setIsLowerSide(1==index%2);
		this->communicationTimer->stop();
	}


	/*** Protected methods ***/
	template <std::size_t DIMENSIONALITY>
	void ComputationalComposedBlock<DIMENSIONALITY>::initializeBlockDataTypes() {
		std::size_t stride[DIMENSIONALITY];
		stride[0] = 1;
		for (std::size_t d=1; d<DIMENSIONALITY; d++) {
			stride[d] = stride[d-1] * this->elementsPerDim;
		}
		MPI::Datatype tmpTypes[DIMENSIONALITY+1];
		tmpTypes[0] = MPI::DOUBLE;
		int doubleSize = MPI::DOUBLE.Get_size();
		for (std::size_t i=0; i<DIMENSIONALITY; i++) {
			for (std::size_t j=0; j<DIMENSIONALITY; j++) {
				if (i==j) {
					tmpTypes[j+1] = tmpTypes[j].Create_hvector(extent, 1, stride[j] * doubleSize);
				} else {
					tmpTypes[j+1] = tmpTypes[j].Create_hvector(this->elementsPerDim, 1, stride[j] * doubleSize);
				}
			}
			commDataBlockTypes[i] = tmpTypes[DIMENSIONALITY];
			commDataBlockTypes[i].Commit();
		}
	}

	template <std::size_t DIMENSIONALITY>
	void ComputationalComposedBlock<DIMENSIONALITY>::startReceive() {
		if (NULL != this->values) {
			for (size_t i=0; i<DIMENSIONALITY; i++) {
				this->receiveRequest[2*i+1] = ghostRegions[i][0]->initializeReceive(
						this->communicator, this->neighborRank[i][0]);
				this->receiveRequest[2*i] = ghostRegions[i][1]->initializeReceive(
						this->communicator, this->neighborRank[i][1]);
			}
			MPI::Prequest::Startall(2*DIMENSIONALITY, this->receiveRequest);
		}
	}

	template <std::size_t DIMENSIONALITY>
	void ComputationalComposedBlock<DIMENSIONALITY>::startSend() {
		if (NULL != this->values) {
			startSendingGhostData();
		}
	}


	/*** Private methods ***/
	template <std::size_t DIMENSIONALITY>
	inline void ComputationalComposedBlock<DIMENSIONALITY>::createGhostRegions() {
		for (std::size_t i=0; i<DIMENSIONALITY; i++) {
			for (std::size_t j=0; j<2; j++) {
				BoundaryId boundary(i, 0==j);
				ghostRegions[i][j] = new GhostRegion<DIMENSIONALITY>(boundary, this->elementsPerDim, this->extent);
			}
		}
	}

	template <std::size_t DIMENSIONALITY>
	void ComputationalComposedBlock<DIMENSIONALITY>::startSendingGhostData() {
		this->communicationTimer->start();
		for (std::size_t d=0; d<DIMENSIONALITY; d++) {
			this->sendRequest[2*d] = this->communicator.Isend(this->values, 1,
					commDataBlockTypes[d], this->neighborRank[d][0], 2*d);
			std::size_t stride = Math::power(this->elementsPerDim, d);
			std::size_t startIndex = (this->elementsPerDim - this->extent) * stride;
			this->sendRequest[2*d+1] = this->communicator.Isend(&(this->values[startIndex]), 1,
					commDataBlockTypes[d], this->neighborRank[d][1], (2*d)+1);
		}
		this->communicationTimer->stop();
	}


} /* namespace Grid */
} /* namespace Haparanda */

#endif /* COMPUTATIONALCOMPOSEDBLOCK_HPP_ */
