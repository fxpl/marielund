#ifndef ITERABLE_HPP_
#define ITERABLE_HPP_

#include "FieldIterator.hpp"

namespace Haparanda {
namespace Iterators {

	template<class InnerIteratorType, class BoundaryIteratorType> class Iterable;

	/**
	 * An interface for data structures whose elements can be iterated over.
	 * regions.
	 *
	 * @tparam ORDER Dimensionality of the data structure
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2014, 2017
	 */
	template<template<std::size_t>class InnerIteratorType, template<std::size_t>class BoundaryIteratorType, std::size_t ORDER>
	class Iterable<InnerIteratorType<ORDER>, BoundaryIteratorType<ORDER>>
	{
	public:
		virtual ~Iterable();

		/**
		 * Get an iterator which iterates over the boundary of the data
		 * structure. Note that that you as a caller has the responsibility to
		 * delete the iterator when you are done with it!
		 *
		 * @return An iterator for the boundary of the data structure
		 */
		virtual FieldIterator<BoundaryIteratorType<ORDER>> *getBoundaryIterator() const = 0;

		/**
		 * Get an iterator which iterates over the whole data structure. Note
		 * that that you as a caller has the responsibility to delete the
		 * iterator when you are done with it!
		 *
		 * @return An iterator for the whole data structure
		 */
		virtual FieldIterator<InnerIteratorType<ORDER>> *getInnerIterator() const = 0;

	};

	template<template<std::size_t>class InnerIteratorType, template<std::size_t>class BoundaryIteratorType, std::size_t ORDER>
	Iterable<InnerIteratorType<ORDER>, BoundaryIteratorType<ORDER>>::~Iterable() {
	}

} /* namespace Iterators */
} /* namespace Haparanda */

#endif /* ITERABLE_HPP_ */
