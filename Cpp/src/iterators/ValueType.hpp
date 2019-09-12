#ifndef VALUETYPE_HPP_
#define VALUETYPE_HPP_

#include <cstdlib>

namespace Haparanda {
namespace Iterators {
	/**
	 * Interface for a strategy that defines how values of a field (or tensor)
	 * are retrieved, given a linear index.
	 *
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2013-2014, 2016
	 */
	class ValueType
	{
	public:
		virtual ~ValueType();

	protected:
		/**
		 * Get the value stored at the specified (linear) index.
		 *
		 * @param index Linear index of the field
		 * @return Value stored at the specified index
		 */
		virtual double getValue(std::size_t index) const = 0;

		/**
		 * Change the value stored at the specified (linear) index.
		 *
		 * @param index Linear index of the field
		 * @param newValue Value which the element value is to be changed to
		 */
		virtual void setValue(std::size_t index, double newValue) = 0;

		template<std::size_t ORDER> friend class FieldIterator;
		template<std::size_t ORDER> friend class PureFieldIterator;
	};

	ValueType::~ValueType() {
	}
} /* namespace Iterators */
} /* namespace Haparanda */

#endif /* VALUETYPE_HPP_ */
