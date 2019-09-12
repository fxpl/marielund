#ifndef VALUEFIELDITERATOR_HPP_
#define VALUEFIELDITERATOR_HPP_

#include "PureFieldIterator.hpp"
#include "ValueArray.hpp"
#include "WholeFieldStepper.hpp"

namespace Haparanda {
namespace Iterators {

	/**
	 * Iterator for traversing all elements, stored in an array, of a data
	 * structure with dimensionality information included, e.g. a field or a
	 * tensor.
	 *
	 * @tparam ORDER Order/dimensionality of the field
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2014, 2016, 2017
	 */
	template <std::size_t ORDER>
	class ValueFieldIterator: public PureFieldIterator<ValueFieldIterator<ORDER>, WholeFieldStepper<ORDER>, ValueArray>
	{
	public:
		/**
		 * Create an iterator which iterates through all values of a field with
		 * values stored in an array.
		 *
		 * @param sizes Size of the field which is to be iterated through, in each dimension
		 * @param values The values stored in the field
		 */
		ValueFieldIterator(const std::array<std::size_t, ORDER>& sizes, double *values);

		virtual ~ValueFieldIterator();
	};

	template <std::size_t ORDER>
	ValueFieldIterator<ORDER>::ValueFieldIterator(
				const std::array<std::size_t, ORDER>& sizes, double *values) {
		this->initialize(sizes, values);
	}

	template <std::size_t ORDER>
	ValueFieldIterator<ORDER>::~ValueFieldIterator() {
	}

} /* namespace Iterators */
} /* namespace Haparanda */

#endif /* VALUEFIELDITERATOR_HPP_ */
