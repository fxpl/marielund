#ifndef VALUEFIELDBOUNDARYITERATOR_HPP_
#define VALUEFIELDBOUNDARYITERATOR_HPP_

#include "PureFieldIterator.hpp"
#include "ValueArray.hpp"
#include "BoundaryStepper.hpp"

namespace Haparanda {
namespace Iterators {

	/**
	 * Iterator for a the boundary of a non-composed data structure of arbitrary
	 * dimensionality.
	 *
	 * @tparam ORDER Order/dimensionality of the data structure
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2014, 2016, 2017
	 */
	template <std::size_t ORDER>
	class ValueFieldBoundaryIterator
			: public PureFieldIterator<ValueFieldBoundaryIterator<ORDER>, BoundaryStepper<ORDER>, ValueArray>
	{
	public:
		/**
		 * Create an iterator which iterates over the boundary of a field with
		 * values stored in an array. Note that before starting to iterate, you
		 * have to choose which boundary to iterate over using the method
		 * setBoundaryToIterate!
		 *
		 * @param sizes Size of the field which is to be iterated through, in each dimension
		 * @param values The values stored in the field
		 */
		ValueFieldBoundaryIterator(const std::array<std::size_t, ORDER>& sizes, double *values);

		virtual ~ValueFieldBoundaryIterator();

		void setBoundaryToIterate(const BoundaryId& boundary);

	private:
		BoundaryId currentBoundary;
	};

	template <std::size_t ORDER>
	ValueFieldBoundaryIterator<ORDER>::ValueFieldBoundaryIterator(
				const std::array<std::size_t, ORDER>& sizes, double *values) {
		this->initialize(sizes, values);
	}

	template <std::size_t ORDER>
	ValueFieldBoundaryIterator<ORDER>::~ValueFieldBoundaryIterator() {
	}

	template <std::size_t ORDER>
	void ValueFieldBoundaryIterator<ORDER>::setBoundaryToIterate(const BoundaryId& boundary) {
		this->currentBoundary = boundary;
		(static_cast<BoundaryStepper<ORDER> &>(*this->stepper))
				.setBoundaryToIterate(this->currentBoundary);
		this->first();
	}

} /* namespace Iterators */
} /* namespace Haparanda */

#endif /* VALUEFIELDBOUNDARYITERATOR_HPP_ */
