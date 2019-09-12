#ifndef FIELDSTEPPINGSTRATEGY_HPP_
#define FIELDSTEPPINGSTRATEGY_HPP_

#include "src/utils/MagicNumber.hpp"

#include <array>
#include <cassert>

#ifdef _OPENMP
#include <omp.h>
#define OMP_THREAD_ID omp_get_thread_num()
#define OMP_NUM_THREADS omp_get_num_threads()
#define OMP_MAX_NUM_THREADS omp_get_max_threads()
#else
#define OMP_THREAD_ID 0
#define OMP_NUM_THREADS 1
#define OMP_MAX_NUM_THREADS 1
#endif

using namespace Haparanda::Utils;

namespace Haparanda {
namespace Iterators {
	template <std::size_t ORDER> class FieldIterator;
	template <std::size_t ORDER> class PureFieldIterator;

	/**
	 * Strategy that defines how an iterator is stepped through a field (or
	 * tensor) of the specified order.
	 *
	 * @tparam ORDER Order of the field which is stepped through.
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2013-2014, 2016-2017
	 */
	template <std::size_t ORDER>
	class FieldSteppingStrategy
	{
	public:
		/**
		 * Initialize the size array using the specified sizes and set index to
		 * 0.
		 *
		 * @param sizes Size of the field which is to be iterated through, in each dimension
		 */
		FieldSteppingStrategy(const std::array<std::size_t, ORDER>& sizes);

		virtual ~FieldSteppingStrategy();

	protected:
		/**
		 * @param dimension Specifies which index should be retrieved, see description of the return value
		 * @return The <code>dimension</code>:th coordinate of the index of the element currently pointed at by the iterator
		 */
		std::size_t currentIndex(std::size_t dimension) const;

		/**
		 * Restart the iterator: Set it to point at its first element.
		 */
		void first();

		/**
		 * @return false if the iterator points outside the field to be iterated, true otherwise
		 */
		bool isInField() const;

		/**
		 * Find the internal index of the element located at a specified
		 * distance from the one currently pointed at by the iterator. The
		 * internal index is the index that is used internally (by the
		 * ValueType strategy) to identify an element and should never be known
		 * by the caller of the iterator!
		 *
		 * @param dimension Dimension in which the distance from the current element to the one in question is > 0
		 * @param offset Distance from the current element to the one in question. A positive value means that the element is located ''above'' (i.e. has higher index than) the current one while a negative sign means that the element is located ''below'' (i.e. has lower index than) the current one.
		 * @return Internal index of the element in question
		 */
		std::size_t linearNeighborIndex(std::size_t dimension, int offset) const;

		/**
		 * Check if the element on a specified distance from the one currently
		 * pointed at by the iterator is inside the field iterated over.
		 *
		 * @param dimension Dimension in which the distance from the current element to the investigated one is > 0
		 * @param offset Distance from the current element to the one investigated. A positive value means that the element is located ''above'' (i.e. has higher index than) the current one while a negative sign means that the element is located ''below'' (i.e. has lower index than) the current one.
		 * @return true if the element in question is inside the field, false otherwise
		 */
		bool neighborInField(std::size_t dimension, int offset) const;

		/**
		 * Advance the iterator one step.
		 */
		virtual void next() = 0;

		std::size_t minIndex, maxIndex, index;
		std::array<uint32_t, ORDER+1> stride;
		std::array<uint32_t, ORDER> size;
		std::size_t totalSize;

		std::array<MagicNumber, ORDER+1> magicStrideNumbers;
		std::array<MagicNumber, ORDER> magicSizeNumbers;

		friend class FieldIterator<ORDER>;
		friend class PureFieldIterator<ORDER>;
	};

	template <std::size_t ORDER>
	FieldSteppingStrategy<ORDER>::FieldSteppingStrategy(const std::array<std::size_t, ORDER>& sizes) {
		totalSize = 1;
		stride[0] = 1;
		magicStrideNumbers[0] = MagicNumber::getMagicNumbers(stride[0]);
		for (std::size_t i=0; i<ORDER; i++) {
			assert(sizes[i] <= 4294967295); // as it must fit in an uint32!
			size[i] = sizes[i];
			totalSize *= this->size[i];
			magicSizeNumbers[i] = MagicNumber::getMagicNumbers(size[i]);
			stride[i+1] = stride[i] * size[i];
			magicStrideNumbers[i+1] = MagicNumber::getMagicNumbers(stride[i+1]);
		}
	}

	template <std::size_t ORDER>
	FieldSteppingStrategy<ORDER>::~FieldSteppingStrategy() {
	}


	template <std::size_t ORDER>
	inline std::size_t FieldSteppingStrategy<ORDER>::currentIndex(std::size_t dimension) const {
		assert(isInField());
		// indexAlongDimension = index/stride[dimension]
		uint64_t indexAlongDimension =
		(((index * magicStrideNumbers[dimension].M) >> 32) +
			index * magicStrideNumbers[dimension].a) >>
				magicStrideNumbers[dimension].s;
		// n = indexAlongDimension / size[dimension]
		uint64_t n = (((indexAlongDimension * magicSizeNumbers[dimension].M) >> 32) +
				indexAlongDimension * magicSizeNumbers[dimension].a) >>
				magicSizeNumbers[dimension].s;
		// return indexAlongDimension % size[dimension]
		return indexAlongDimension - size[dimension] * n;
	}

	template <std::size_t ORDER>
	inline bool FieldSteppingStrategy<ORDER>::isInField() const {
		return index>=minIndex && index<=maxIndex;
	}

	template <std::size_t ORDER>
	inline void FieldSteppingStrategy<ORDER>::first() {
		index = minIndex;
	}

	template <std::size_t ORDER>
	inline std::size_t FieldSteppingStrategy<ORDER>::linearNeighborIndex(std::size_t dimension, int offset) const {
		assert(neighborInField(dimension, offset));
		return index + offset * (int)stride[dimension];
	}

	template <std::size_t ORDER>
	inline bool FieldSteppingStrategy<ORDER>::neighborInField(std::size_t dimension, int offset) const {
		const long neighborIndexInDimension = offset + currentIndex(dimension);
		return neighborIndexInDimension >= 0
				&& neighborIndexInDimension < static_cast<long>(size[dimension]);
	}

} /* namespace Iterators */
} /* namespace Haparanda */

#endif /* FIELDSTEPPINGSTRATEGY_HPP_ */
