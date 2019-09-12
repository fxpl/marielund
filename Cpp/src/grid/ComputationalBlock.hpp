#ifndef COMPUTATIONALBLOCK_HPP_
#define COMPUTATIONALBLOCK_HPP_

#include "src/iterators/Iterable.hpp"

namespace Haparanda {
namespace Grid {

using namespace Iterators;

	/**
	 * A block containing data of a domain.
	 *
	 * @tparam DIMENSIONALITY Dimensionality of the block
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2013-2014, 2017-2018
	 */
	template <std::size_t DIMENSIONALITY>
	class ComputationalBlock : virtual public Iterable<DIMENSIONALITY>
	{
	public:
		/**
		 * @param elementsPerDim Number of elements along each dimension
		 */
		ComputationalBlock(std::size_t elementsPerDim);

		/**
		 * @param elementsPerDim Number of elements along each dimension
		 * @param values Array containing all function values which this block will buffer
		 */
		ComputationalBlock(std::size_t elementsPerDim, double *values);

		virtual ~ComputationalBlock();

		/**
		 * @return The number of elements along each dimension of the block
		 */
		std::size_t getElementsPerDim() const;

		/**
		 * Set the values of the block to the ones stored in the array given as
		 * argument and start initialization of side regions.
		 *
		 * @param values Array containing all function values that this block will buffer
		 */
		virtual void setValues(double *values);

	protected:
		std::size_t smallestIndex;
		std::size_t elementsPerDim;
		double *values = NULL;

		/**
		 * @return An array with DIMENSIONALITY elements, all initialized to elementsPerDim
		 */
		std::array<std::size_t, DIMENSIONALITY> getSizeArray() const;

	private:
		/**
		 * Initialize the member variables that are initialized by all
		 * constructors: smallestIndex and elementsPerDim.
		 *
		 * @param elementsPerDim Number of elements along each dimension
		 */
		void initializeMemberVariables(std::size_t elementsPerDim);
	};

	template <std::size_t DIMENSIONALITY>
	ComputationalBlock<DIMENSIONALITY>::ComputationalBlock(std::size_t elementsPerDim) {
		initializeMemberVariables(elementsPerDim);
	}

	template <std::size_t DIMENSIONALITY>
	ComputationalBlock<DIMENSIONALITY>::ComputationalBlock(std::size_t elementsPerDim, double *values) {
		initializeMemberVariables(elementsPerDim);
		this->values = values;
	}

	template <std::size_t DIMENSIONALITY>
	ComputationalBlock<DIMENSIONALITY>::~ComputationalBlock() {
	}

	template <std::size_t DIMENSIONALITY>
	std::size_t ComputationalBlock<DIMENSIONALITY>::getElementsPerDim() const {
		return elementsPerDim;
	}

	template <std::size_t DIMENSIONALITY>
	inline void ComputationalBlock<DIMENSIONALITY>::setValues(double *values) {
		this->values = values;
	}


	/*** Protected methods ***/
	template<std::size_t DIMENSIONALITY>
	std::array<std::size_t, DIMENSIONALITY> ComputationalBlock<DIMENSIONALITY>::getSizeArray() const {
		std::array<std::size_t, DIMENSIONALITY> sizes;
		sizes.fill(this->elementsPerDim);
		return sizes;
	}

	/*** Private methods ***/
	template <std::size_t DIMENSIONALITY>
	inline void ComputationalBlock<DIMENSIONALITY>::initializeMemberVariables(std::size_t elementsPerDim) {
		this->smallestIndex = 0;	// Default value; may be changed in the initialization
		this->elementsPerDim = elementsPerDim;
	}

} /* namespace Grid */
} /* namespace Haparanda */

#endif /* COMPUTATIONALBLOCK_HPP_ */
