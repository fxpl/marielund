#ifndef COMPUTATIONALPUREBLOCK_HPP_
#define COMPUTATIONALPUREBLOCK_HPP_

#include "ComputationalBlock.hpp"
#include "src/iterators/ValueFieldBoundaryIterator.hpp"
#include "src/iterators/ValueFieldIterator.hpp"

using namespace Haparanda::Iterators;

namespace Haparanda {
namespace Grid {

	/**
	 * Computational block without ghost regions.
	 *
	 * Note that this type of block assumes the element indices to be
	 * consecutive!
	 *
	 * @tparam DIMENSIONALITY Dimensionality of the block
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2014, 2017
	 */
	template <std::size_t DIMENSIONALITY>
	class ComputationalPureBlock: public ComputationalBlock<DIMENSIONALITY>
	{
	public:
		/**
		 * Set the size of the inner region of this block to elementsPerDim and
		 * initialize the values of the block using the array given as argument.
		 *
		 * @param elementsPerDim Number of elements along each dimension
		 * @param values Array containing all function values which this block will buffer
		 */
		ComputationalPureBlock(std::size_t elementsPerDim, double *values);

		virtual ~ComputationalPureBlock();

		virtual BoundaryIterator<DIMENSIONALITY> *getBoundaryIterator() const;

		virtual FieldIterator<DIMENSIONALITY> *getInnerIterator() const;

	};

	template <std::size_t DIMENSIONALITY>
	ComputationalPureBlock<DIMENSIONALITY>::ComputationalPureBlock(std::size_t elementsPerDim, double *values)
	: ComputationalBlock<DIMENSIONALITY>(elementsPerDim, values) {
	}

	template <std::size_t DIMENSIONALITY>
	ComputationalPureBlock<DIMENSIONALITY>::~ComputationalPureBlock() {
	}

	template <std::size_t DIMENSIONALITY>
	inline BoundaryIterator<DIMENSIONALITY> *ComputationalPureBlock<DIMENSIONALITY>::getBoundaryIterator() const {
		assert(NULL != this->values);
		std::array<std::size_t, DIMENSIONALITY> sizes = this->getSizeArray();
		return new ValueFieldBoundaryIterator<DIMENSIONALITY>(
				sizes, &(this->values[this->smallestIndex]));
	}

	template <std::size_t DIMENSIONALITY>
	inline FieldIterator<DIMENSIONALITY> *ComputationalPureBlock<DIMENSIONALITY>::getInnerIterator() const {
		assert(NULL != this->values);
		std::array<std::size_t, DIMENSIONALITY> sizes = this->getSizeArray();
		return new ValueFieldIterator<DIMENSIONALITY>(sizes, &(this->values[this->smallestIndex]));
	}

} /* namespace Grid */
} /* namespace Haparanda */

#endif /* COMPUTATIONALPUREBLOCK_HPP_ */
