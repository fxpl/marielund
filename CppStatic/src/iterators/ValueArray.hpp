#ifndef VALUEARRAY_HPP_
#define VALUEARRAY_HPP_

#include "ValueType.hpp"

namespace Haparanda {
namespace Iterators {
	/**
	 * A strategy for retrieving values of a field (of tensor) when the values
	 * are stored in an array.
	 *
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2013-2014, 2017
	 */
	class ValueArray: public ValueType<ValueArray>
	{
	public:
		/**
		 * @param values Pointer to the first element of the array containing the values to be iterated over
		 */
		ValueArray(double *values);
		virtual ~ValueArray();

	protected:
		double getValue(std::size_t index) const;
		void setValue(std::size_t index, double newValue);

	private:
		double *values;

		friend class ValueArrayTest;
		friend class ValueType<ValueArray>;
	};

	ValueArray::ValueArray(double *values) {
		this->values = values;
	}

	ValueArray::~ValueArray() {
	}

	inline double ValueArray::getValue(std::size_t index) const {
		return values[index];
	}

	inline void ValueArray::setValue(std::size_t index, double newValue) {
		values[index] = newValue;
	}

} /* namespace Iterators */
} /* namespace Haparanda */

#endif /* VALUEARRAY_HPP_ */
