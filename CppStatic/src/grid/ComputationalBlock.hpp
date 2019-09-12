#ifndef COMPUTATIONALBLOCK_HPP_
#define COMPUTATIONALBLOCK_HPP_

#include "src/iterators/Iterable.hpp"

namespace Haparanda {
namespace Grid {

using namespace Iterators;

	template <class InnerIteratorType, class BoundaryIteratorType> class ComputationalBlock;

	/**
	 * A block containing data of a domain block.
	 *
	 * @tparam DIMENSIONALITY Dimensionality of the block
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2013-2014, 2017-2018
	 */
	template <template<std::size_t>class InnerIteratorType, template<std::size_t>class BoundaryIteratorType, std::size_t DIMENSIONALITY>
	class ComputationalBlock<InnerIteratorType<DIMENSIONALITY>, BoundaryIteratorType<DIMENSIONALITY>>
			: virtual public Iterable<InnerIteratorType<DIMENSIONALITY>, BoundaryIteratorType<DIMENSIONALITY>>
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
		void setValues(double *values);

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

	template <template<std::size_t>class InnerIteratorType, template<std::size_t>class BoundaryIteratorType, std::size_t DIMENSIONALITY>
	ComputationalBlock<InnerIteratorType<DIMENSIONALITY>, BoundaryIteratorType<DIMENSIONALITY>>::ComputationalBlock(std::size_t elementsPerDim) {
		initializeMemberVariables(elementsPerDim);
	}

	template <template<std::size_t>class InnerIteratorType, template<std::size_t>class BoundaryIteratorType, std::size_t DIMENSIONALITY>
	ComputationalBlock<InnerIteratorType<DIMENSIONALITY>, BoundaryIteratorType<DIMENSIONALITY>>::ComputationalBlock(std::size_t elementsPerDim, double *values) {
		initializeMemberVariables(elementsPerDim);
		setValues(values);
	}

	template <template<std::size_t>class InnerIteratorType, template<std::size_t>class BoundaryIteratorType, std::size_t DIMENSIONALITY>
	ComputationalBlock<InnerIteratorType<DIMENSIONALITY>, BoundaryIteratorType<DIMENSIONALITY>>::~ComputationalBlock() {
	}

	template <template<std::size_t>class InnerIteratorType, template<std::size_t>class BoundaryIteratorType, std::size_t DIMENSIONALITY>
	std::size_t ComputationalBlock<InnerIteratorType<DIMENSIONALITY>, BoundaryIteratorType<DIMENSIONALITY>>::getElementsPerDim() const {
		return elementsPerDim;
	}

	template <template<std::size_t>class InnerIteratorType, template<std::size_t>class BoundaryIteratorType, std::size_t DIMENSIONALITY>
	inline void ComputationalBlock<InnerIteratorType<DIMENSIONALITY>, BoundaryIteratorType<DIMENSIONALITY>>::setValues(double *values) {
		this->values = values;
	}


	/*** Protected methods ***/
	template <template<std::size_t>class InnerIteratorType, template<std::size_t>class BoundaryIteratorType, std::size_t DIMENSIONALITY>
	std::array<std::size_t, DIMENSIONALITY> ComputationalBlock<InnerIteratorType<DIMENSIONALITY>, BoundaryIteratorType<DIMENSIONALITY>>::getSizeArray() const {
		std::array<std::size_t, DIMENSIONALITY> sizes;
		sizes.fill(this->elementsPerDim);
		return sizes;
	}

	/*** Private methods ***/
	template <template<std::size_t>class InnerIteratorType, template<std::size_t>class BoundaryIteratorType, std::size_t DIMENSIONALITY>
	inline void ComputationalBlock<InnerIteratorType<DIMENSIONALITY>, BoundaryIteratorType<DIMENSIONALITY>>::initializeMemberVariables(std::size_t elementsPerDim) {
		this->smallestIndex = 0;	// Default value; may be changed in the initialization
		this->elementsPerDim = elementsPerDim;
	}

} /* namespace Grid */
} /* namespace Haparanda */

#endif /* COMPUTATIONALBLOCK_HPP_ */
