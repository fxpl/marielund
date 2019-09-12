#ifndef GHOSTREGION_HPP_
#define GHOSTREGION_HPP_

#include "ComputationalBlock.hpp"
#include "src/iterators/ValueFieldIterator.hpp"
#include "src/iterators/ValueFieldBoundaryIterator.hpp"
#include "src/utils/Math.hpp"

#include <mpi.h>

using namespace Haparanda::Iterators;

namespace Haparanda {
namespace Grid {

	/**
	 * A class representing ghost regions of a computational block.
	 *
	 * @tparam DIMENSIONALITY Dimensionality of the block
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2017, 2019
	 */
	template <std::size_t DIMENSIONALITY>
	class GhostRegion: public Iterable<ValueFieldIterator<DIMENSIONALITY>, ValueFieldBoundaryIterator<DIMENSIONALITY>>
	{
	public:
		typedef ValueFieldIterator<DIMENSIONALITY> InnerIteratorType;
		typedef ValueFieldBoundaryIterator<DIMENSIONALITY> BoundaryIteratorType;

		/**
		 * This constructor allocates memory for the values, but does not
		 * initialize them with values.
		 *
		 * @param boundary Boundary at which the ghost region is located
		 * @param size The size of the ghost region in all dimensions but boundary.dimension
		 * @param width The width of the ghost region in boundary.dimension (= extent of the stencil)
		 */
		GhostRegion(BoundaryId& boundary, std::size_t size, std::size_t width);

		virtual ~GhostRegion();

		virtual FieldIterator<BoundaryIteratorType> *getBoundaryIterator() const;

		virtual FieldIterator<InnerIteratorType> *getInnerIterator() const;

		/**
		 * Initialize a receive from the process with the specified rank in the
		 * communicator given as argument. The values will be stored in this
		 * ghost region. The tag will be 2*D if I am at the upper boundary and
		 * 2*D+1 if I an at the lower boundary, where D is the dimension of my
		 * boundary. (If I am on the lower boundary, I receive data from the
		 * upper boundary and vice versa.)
		 *
		 * @param communicator Communicator in which the data is communicated
		 * @param rank Rank of sending process
		 * @return The request created by Recv_init
		 */
		MPI::Request initializeReceive(MPI::Comm& communicator, int rank) const;

	private:
		BoundaryId boundary;		// Boundary along which the ghost region is located
		std::size_t width;			// Size along boundary.dimension
		std::size_t elementsPerDim;	// Size along the other dimensions
		double *values;

		/**
		 * This constructor is for testing purposes.
		 *
		 * @param boundary Boundary at which the ghost region is located
		 * @param size The size of the ghost region in all dimensions but boundary.dimension
		 * @param width The width of the ghost region in boundary.dimension (= extent of the stencil)
		 * @param values Array containing all the ghost values. Note that values is deleted by the destructor of this class!
		 */
		GhostRegion(BoundaryId& boundary, std::size_t size, std::size_t width, double *values);

		/**
		 * Create an array containing the size of the ghost region in each
		 * dimension (that is, elementsPerDim in all directions but the one
		 * represented by boundary)
		 *
		 * @return The array described above
		 */
		std::array<std::size_t, DIMENSIONALITY> getSizeArray() const;

		/**
		 * Initialize each member variable to the value of the argument with
		 * the same name.
		 *
		 * @param boundary
		 * @param width
		 * @param elementsPerDim
		 * @param values
		 */
		void initializeMemberVariables(BoundaryId boundary, std::size_t elementsPerDim, std::size_t width, double *values);

		friend class GhostRegionTest;
	};


	template <std::size_t DIMENSIONALITY>
	GhostRegion<DIMENSIONALITY>::GhostRegion(BoundaryId& boundary, std::size_t size, std::size_t width) {
		std::size_t totalSize = Math::power(size, DIMENSIONALITY-1) * width;
		double *values = new double[totalSize];
		initializeMemberVariables(boundary, size, width, values);
	}

	template <std::size_t DIMENSIONALITY>
	GhostRegion<DIMENSIONALITY>::~GhostRegion() {
		if (NULL != this->values) {
			delete []this->values;
		}
	}

	template <std::size_t DIMENSIONALITY>
	inline FieldIterator<typename GhostRegion<DIMENSIONALITY>::BoundaryIteratorType> *GhostRegion<DIMENSIONALITY>::getBoundaryIterator() const {
		return new ValueFieldBoundaryIterator<DIMENSIONALITY>(getSizeArray(), values);
	}

	template <std::size_t DIMENSIONALITY>
	inline FieldIterator<typename GhostRegion<DIMENSIONALITY>::InnerIteratorType> *GhostRegion<DIMENSIONALITY>::getInnerIterator() const {
		return new ValueFieldIterator<DIMENSIONALITY>(getSizeArray(), values);
	}

	template <std::size_t DIMENSIONALITY>
	MPI::Request GhostRegion<DIMENSIONALITY>::initializeReceive(MPI::Comm& communicator, int rank) const {
		int tag = 2 * this->boundary.getDimension() + this->boundary.isLowerSide();
		std::size_t totalSize = Math::power(this->elementsPerDim, DIMENSIONALITY-1) * this->width;
		return communicator.Recv_init(this->values, totalSize, MPI::DOUBLE, rank, tag);
	}


	template <std::size_t DIMENSIONALITY>
	GhostRegion<DIMENSIONALITY>::GhostRegion(BoundaryId& boundary, std::size_t size, std::size_t width, double *values) {
		initializeMemberVariables(boundary, size, width, values);
	}

	template <std::size_t DIMENSIONALITY>
	std::array<std::size_t, DIMENSIONALITY> GhostRegion<DIMENSIONALITY>::getSizeArray() const {
		std::array<std::size_t, DIMENSIONALITY> sizes;
		sizes.fill(elementsPerDim);
		sizes[boundary.getDimension()] = width;
		return sizes;
	}

	template <std::size_t DIMENSIONALITY>
	void GhostRegion<DIMENSIONALITY>::initializeMemberVariables(BoundaryId boundary, std::size_t elementsPerDim, std::size_t width, double *values) {
		this->boundary = boundary;
		this->elementsPerDim = elementsPerDim;
		this->width = width;
		this->values = values;
	}

} /* namespace Grid */
} /* namespace Haparanda */

#endif /* GHOSTREGION_HPP_ */
