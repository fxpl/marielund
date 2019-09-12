#ifndef BOUNDARYITERATOR_HPP_
#define BOUNDARYITERATOR_HPP_

#include "BoundaryStepper.hpp"

namespace Haparanda {
namespace Iterators {

	/**
	 * Interface of iterators for a the boundary of a data structure of
	 * arbitrary dimensionality.
	 *
	 * @tparam ORDER Order/dimensionality of the data structure
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2013-2014, 2017
	 */
	template <std::size_t ORDER>
	class BoundaryIterator : virtual public FieldIterator<ORDER>
	{
	public:
		virtual ~BoundaryIterator();

		/**
		 * Choose boundary (@f$x_i@f$ constant) to be iterated by the iterator,
		 * and set iterator to point at the first element of that boundary.
		 *
		 * @param boundary Id of the new boundary to iterate
		 */
		virtual void setBoundaryToIterate(const BoundaryId& boundary) = 0;

	protected:
		BoundaryId currentBoundary;
	};

	template <std::size_t ORDER>
	BoundaryIterator<ORDER>::~BoundaryIterator() {
	}

} /* namespace Iterators */
} /* namespace Haparanda */

#endif /* BOUNDARYITERATOR_HPP_ */
