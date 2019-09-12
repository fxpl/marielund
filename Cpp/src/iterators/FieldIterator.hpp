#ifndef FIELDITERATOR_HPP_
#define FIELDITERATOR_HPP_

#include "FieldSteppingStrategy.hpp"
#include "ValueType.hpp"

namespace Haparanda {
namespace Iterators {

	/**
	 * Abstract class representing an iterator for a data structure with
	 * dimensionality information included, e.g. a field or a tensor.
	 *
	 * @tparam ORDER Order/dimensionality of the data structure
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2013-2014, 2017
	 */
	template <std::size_t ORDER>
	class FieldIterator
	{
	public:
		virtual ~FieldIterator();

		/**
		 * @param dimension Specifies which index should be retrieved, see description of the return value
		 * @return The <code>dimension</code>:th coordinate of the index of the element currently pointed at by the iterator
		 */
		virtual std::size_t currentIndex(std::size_t dimension) const = 0;

		/**
		 * Get the value of a neighbor of the element currently pointed at by
		 * the iterator.
		 *
		 * @param dimension The dimension in which the requested neighbor is located
		 * @param offset The distance from the element at index to the requested neighbor. A positive value means that the neighbor is located ''above'' (i.e. has higher index than) the element at index, while a negative sign means that the neighbor is located ''below'' (i.e. has lower index than) the element at index.
		 * @return The value of the requested neighbor
		 */
		virtual double currentNeighbor(std::size_t dimension, int offset) const = 0;

		/**
		 * @return The value of the element currently pointed at by the iterator
		 */
		virtual double currentValue() const = 0;

		/**
		 * Restart the iterator: Set it to point at the first element.
		 */
		virtual void first() = 0;

		/**
		 * @return true if the iterator points at an element in the field, false if it is outside
		 */
		virtual bool isInField() const = 0;

		/**
		 * Advance the iterator one step.
		 */
		virtual void next() = 0;

		/**
		 * Change the value of a neighbor of the element currently pointed at
		 * by the iterator.
		 *
		 * @param newValue Value to change the current element to
		 */
		virtual void setCurrentNeighbor(std::size_t dimension, int offset, double newValue) = 0;

		/**
		 * Change the value of the element currently pointed at by the iterator.
		 *
		 * @param dimension The dimension in which the requested neighbor is located
		 * @param offset The distance from the element at index to the requested neighbor. A positive value means that the neighbor is located ''above'' (i.e. has higher index than) the element at index, while a negative sign means that the neighbor is located ''below'' (i.e. has lower index than) the element at index.
		 * @param newValue Value to change the current element to
		 */
		virtual void setCurrentValue(double newValue) = 0;

		/**
		 * @param dimension The dimension along which the size should be retrieved
		 * @return The size of the field along the specified dimension
		 */
		virtual std::size_t size(std::size_t dimension) const = 0;
	};

	template <std::size_t ORDER>
	FieldIterator<ORDER>::~FieldIterator() {
	}

} /* namespace Iterators */
} /* namespace Haparanda */

#endif /* FIELDITERATOR_HPP_ */
