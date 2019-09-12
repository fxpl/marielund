#ifndef ITERABLE_HPP_
#define ITERABLE_HPP_

#include "BoundaryIterator.hpp"

namespace Haparanda {
namespace Iterators {

	/**
	 * An interface for data structures whose elements can be iterated over.
	 * regions.
	 *
	 * @tparam ORDER Dimensionality of the data structure
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2014, 2017
	 */
	template <std::size_t ORDER>
	class Iterable
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
		virtual BoundaryIterator<ORDER> *getBoundaryIterator() const = 0;

		/**
		 * Get an iterator which iterates over the whole data structure. Note
		 * that that you as a caller has the responsibility to delete the
		 * iterator when you are done with it!
		 *
		 * @return An iterator for the whole data structure
		 */
		virtual FieldIterator<ORDER> *getInnerIterator() const = 0;

	};

	template <std::size_t ORDER>
	Iterable<ORDER>::~Iterable() {
	}

} /* namespace Iterators */
} /* namespace Haparanda */

#endif /* ITERABLE_HPP_ */
