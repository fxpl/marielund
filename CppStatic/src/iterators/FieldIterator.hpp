#ifndef FIELDITERATOR_HPP_
#define FIELDITERATOR_HPP_

#include "FieldSteppingStrategy.hpp"
#include "ValueType.hpp"
#include "src/utils/BoundaryId.hpp"

namespace Haparanda {
namespace Iterators {

using namespace Utils;

	/**
	 * Abstract class representing an iterator for a data structure with
	 * dimensionality information included, e.g. a field or a tensor.
	 *
	 * @tparam IteratorType Type of concrete sub class implementing this class (see CRTP)
	 * @tparam ORDER Order/dimensionality of the data structure
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2013-2014, 2017
	 */
	template <template<std::size_t>class IteratorType, std::size_t ORDER>
	class FieldIterator<IteratorType<ORDER>>
	{
	public:
		virtual ~FieldIterator();

		/**
		 * @param dimension Specifies which index should be retrieved, see description of the return value
		 * @return The <code>dimension</code>:th coordinate of the index of the element currently pointed at by the iterator
		 */
		std::size_t currentIndex(std::size_t dimension) const;

		/**
		 * Get the value of a neighbor of the element currently pointed at by
		 * the iterator.
		 *
		 * @param dimension The dimension in which the requested neighbor is located
		 * @param offset The distance from the element at index to the requested neighbor. A positive value means that the neighbor is located ''above'' (i.e. has higher index than) the element at index, while a negative sign means that the neighbor is located ''below'' (i.e. has lower index than) the element at index.
		 * @return The value of the requested neighbor
		 */
		double currentNeighbor(std::size_t dimension, int offset) const;

		/**
		 * @return The value of the element currently pointed at by the iterator
		 */
		double currentValue() const;

		/**
		 * Restart the iterator: Set it to point at the first element.
		 */
		void first();

		/**
		 * @return true if the iterator points at an element in the field, false if it is outside
		 */
		bool isInField() const;

		/**
		 * Advance the iterator one step.
		 */
		void next();

		/**
		 * Choose boundary (@f$x_i@f$ constant) to be iterated by the iterator,
		 * and set iterator to point at the first element of that boundary.
		 *
		 * Note that for non-boundary iterators, this method will throw an
		 * exception!
		 *
		 * @param boundary Id of the new boundary to iterate
		 */
		void setBoundaryToIterate(const BoundaryId& boundary);

		/**
		 * Change the value of a neighbor of the element currently pointed at
		 * by the iterator.
		 *
		 * @param newValue Value to change the current element to
		 */
		void setCurrentNeighbor(std::size_t dimension, int offset, double newValue);

		/**
		 * Change the value of the element currently pointed at by the iterator.
		 *
		 * @param dimension The dimension in which the requested neighbor is located
		 * @param offset The distance from the element at index to the requested neighbor. A positive value means that the neighbor is located ''above'' (i.e. has higher index than) the element at index, while a negative sign means that the neighbor is located ''below'' (i.e. has lower index than) the element at index.
		 * @param newValue Value to change the current element to
		 */
		void setCurrentValue(double newValue);

		/**
		 * @param dimension The dimension along which the size should be retrieved
		 * @return The size of the field along the specified dimension
		 */
		std::size_t size(std::size_t dimension) const;
	};

	template <template<std::size_t>class IteratorType, std::size_t ORDER>
	FieldIterator<IteratorType<ORDER>>::~FieldIterator() {
	}

	template <template<std::size_t>class IteratorType, std::size_t ORDER>
	inline std::size_t FieldIterator<IteratorType<ORDER>>::currentIndex(std::size_t dimension) const {
		return static_cast<const IteratorType<ORDER>&>(*this).currentIndex(dimension);
	}

	template <template<std::size_t>class IteratorType, std::size_t ORDER>
	inline double FieldIterator<IteratorType<ORDER>>::currentNeighbor(std::size_t dimension, int offset) const {
		return static_cast<const IteratorType<ORDER>&>(*this).currentNeighbor(dimension, offset);
	}

	template <template<std::size_t>class IteratorType, std::size_t ORDER>
	inline double FieldIterator<IteratorType<ORDER>>::currentValue() const {
		return static_cast<const IteratorType<ORDER>&>(*this).currentValue();
	}

	template <template<std::size_t>class IteratorType, std::size_t ORDER>
	inline void FieldIterator<IteratorType<ORDER>>::first() {
		static_cast<IteratorType<ORDER>&>(*this).first();
	}

	template <template<std::size_t>class IteratorType, std::size_t ORDER>
	inline void FieldIterator<IteratorType<ORDER>>::next() {
		static_cast<IteratorType<ORDER>&>(*this).next();
	}

	template <template<std::size_t>class IteratorType, std::size_t ORDER>
	inline bool FieldIterator<IteratorType<ORDER>>::isInField() const {
		return static_cast<const IteratorType<ORDER>&>(*this).isInField();
	}

	template <template<std::size_t>class IteratorType, std::size_t ORDER>
	inline void FieldIterator<IteratorType<ORDER>>::setBoundaryToIterate(const BoundaryId& boundary) {
		static_cast<IteratorType<ORDER>&>(*this).setBoundaryToIterate(boundary);
	}

	template <template<std::size_t>class IteratorType, std::size_t ORDER>
	inline void FieldIterator<IteratorType<ORDER>>::setCurrentNeighbor(std::size_t dimension, int offset, double newValue) {
		static_cast<IteratorType<ORDER>&>(*this).setCurrentNeighbor(dimension, offset, newValue);
	}

	template <template<std::size_t>class IteratorType, std::size_t ORDER>
	inline void FieldIterator<IteratorType<ORDER>>::setCurrentValue(double newValue) {
		static_cast<IteratorType<ORDER>&>(*this).setCurrentValue(newValue);
	}

	template <template<std::size_t>class IteratorType, std::size_t ORDER>
	inline std::size_t FieldIterator<IteratorType<ORDER>>::size(std::size_t dimension) const {
		return static_cast<const IteratorType<ORDER>&>(*this).size(dimension);
	}

} /* namespace Iterators */
} /* namespace Haparanda */

#endif /* FIELDITERATOR_HPP_ */
