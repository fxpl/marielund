#ifndef VALUETYPE_HPP_
#define VALUETYPE_HPP_

#include <cstdlib>

namespace Haparanda {
namespace Iterators {
	/**
	 * Interface for a strategy that defines how values of a field (or tensor)
	 * of a specified order are retrieved, given a linear index.
	 *
	 * @tparam GetterType Type of concrete sub class implementing this interface (see CRTP)
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2013-2014, 2016-2017
	 */
	template <class GetterType>
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
		double getValue(std::size_t index) const;

		/**
		 * Change the value stored at the specified (linear) index.
		 *
		 * @param index Linear index of the field
		 * @param newValue Value which the element value is to be changed to
		 */
		void setValue(std::size_t index, double newValue);

		template<class> friend class FieldIterator;
		template<class, class, class> friend class PureFieldIterator;
	};

	template <class GetterType>
	ValueType<GetterType>::~ValueType() {
	}

	template <class GetterType>
	inline double ValueType<GetterType>::getValue(std::size_t index) const {
		return static_cast<const GetterType&>(*this).getValue(index);
	}

	template <class GetterType>
	inline void ValueType<GetterType>::setValue(std::size_t index, double newValue) {
		static_cast<GetterType&>(*this).setValue(index, newValue);
	}

} /* namespace Iterators */
} /* namespace Haparanda */

#endif /* VALUETYPE_HPP_ */
