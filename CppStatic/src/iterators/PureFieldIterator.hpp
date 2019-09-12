#ifndef PUREFIELDITERATOR_HPP_
#define PUREFIELDITERATOR_HPP_

#include "FieldIterator.hpp"

#include <stdexcept>
#include <sstream>
#include <typeinfo>

namespace Haparanda {
namespace Iterators {

	/**
	 * Abstract class representing an iterator for a non-composed data structure
	 * with dimensionality information included, e.g. a field or a tensor.
	 *
	 * @tparam IteratorType Type of concrete sub class implementing this class (see CRTP)
	 * @tparam StepperType Type of the concrete sub class of the stepper used in this class (see the CRTP pattern)
	 * @tparam GetterType Type of the concrete sub class of the getter used in this class (see the CRTP pattern)
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2014, 2016-2017
	 */
	template<class IteratorType, class StepperType, class GetterType> class PureFieldIterator;

	/**
	 * This specialization is needed as ORDER cannot be retrieved from the sub
	 * class as it is not created yet when this class is created.
	 *
	 * @tparam ORDER Order/dimensionality of the field iterated over
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2014, 2016-2017
	 */
	template <template<std::size_t>class IteratorType, template<std::size_t>class StepperType, class GetterType, std::size_t ORDER>
	class PureFieldIterator<IteratorType<ORDER>, StepperType<ORDER>, GetterType>
	: public FieldIterator<IteratorType<ORDER> >
	{
	public:
		/**
		 * Create an empty iterator, without stepper or getter. (This
		 * iterator will segfault if you try to do something with it without
		 * initializing the strategies in another way; don't forget to call
		 * initialize or the create* methods in the concrete class!)
		 */
		PureFieldIterator();

		/**
		 * Destroy the getter and the stepper if they are initialized.
		 */
		virtual ~PureFieldIterator();

		std::size_t currentIndex(std::size_t dimension) const;

		double currentNeighbor(std::size_t dimension, int offset) const;

		double currentValue() const;

		void first();

		bool isInField() const;

		void next();

		void setBoundaryToIterate(const BoundaryId& boundary);

		void setCurrentNeighbor(std::size_t dimension, int offset, double newValue);

		void setCurrentValue(double newValue);

		std::size_t size(std::size_t dimension) const;

	protected:
		FieldSteppingStrategy<StepperType<ORDER>> *stepper;
		ValueType<GetterType> *getter;

		/**
		 * Create the stepper and the getter.
		 *
		 * @param sizes Size of the field which is to be iterated through, in each dimension
		 * @param data Pointer to the value of the first element in the field
		 */
		void initialize(const std::array<std::size_t, ORDER>& sizes, double *data);
	};

	template <template<std::size_t>class IteratorType, template<std::size_t>class StepperType, class GetterType, std::size_t ORDER>
	PureFieldIterator<IteratorType<ORDER>, StepperType<ORDER>, GetterType>::PureFieldIterator() {
		stepper = NULL;
		getter = NULL;
	}

	template <template<std::size_t>class IteratorType, template<std::size_t>class StepperType, class GetterType, std::size_t ORDER>
	PureFieldIterator<IteratorType<ORDER>, StepperType<ORDER>, GetterType>::~PureFieldIterator() {
		if (NULL != stepper) {
			delete stepper;
		}
		if (NULL != getter) {
			delete getter;
		}
	}

	template <template<std::size_t>class IteratorType, template<std::size_t>class StepperType, class GetterType, std::size_t ORDER>
	inline std::size_t PureFieldIterator<IteratorType<ORDER>, StepperType<ORDER>, GetterType>::currentIndex(std::size_t dimension) const {
		assert(this->stepper->isInField());
		return this->stepper->currentIndex(dimension);
	}

	template <template<std::size_t>class IteratorType, template<std::size_t>class StepperType, class GetterType, std::size_t ORDER>
	inline double PureFieldIterator<IteratorType<ORDER>, StepperType<ORDER>, GetterType>::currentNeighbor(std::size_t dimension, int offset) const {
		assert(this->stepper->neighborInField(dimension, offset));
		std::size_t neighborIndex = this->stepper->linearNeighborIndex(dimension, offset);
		return this->getter->getValue(neighborIndex);
	}

	template <template<std::size_t>class IteratorType, template<std::size_t>class StepperType, class GetterType, std::size_t ORDER>
	inline double PureFieldIterator<IteratorType<ORDER>, StepperType<ORDER>, GetterType>::currentValue() const {
		assert(this->stepper->isInField());
		return this->getter->getValue(this->stepper->index);
	}

	template <template<std::size_t>class IteratorType, template<std::size_t>class StepperType, class GetterType, std::size_t ORDER>
	inline void PureFieldIterator<IteratorType<ORDER>, StepperType<ORDER>, GetterType>::first() {
		this->stepper->first();
	}

	template <template<std::size_t>class IteratorType, template<std::size_t>class StepperType, class GetterType, std::size_t ORDER>
	inline bool PureFieldIterator<IteratorType<ORDER>, StepperType<ORDER>, GetterType>::isInField() const {
		return this->stepper->isInField();
	}

	template <template<std::size_t>class IteratorType, template<std::size_t>class StepperType, class GetterType, std::size_t ORDER>
	inline void PureFieldIterator<IteratorType<ORDER>, StepperType<ORDER>, GetterType>::next() {
		this->stepper->next();
	}

	template <template<std::size_t>class IteratorType, template<std::size_t>class StepperType, class GetterType, std::size_t ORDER>
	inline void PureFieldIterator<IteratorType<ORDER>, StepperType<ORDER>, GetterType>::setBoundaryToIterate(const BoundaryId& boundary) {
		std::stringstream errorMsg;
		errorMsg << "setBoundaryToIterate cannot be called for " << typeid(*this).name() << ".";
		throw std::runtime_error(errorMsg.str());
	}

	template <template<std::size_t>class IteratorType, template<std::size_t>class StepperType, class GetterType, std::size_t ORDER>
	inline void PureFieldIterator<IteratorType<ORDER>, StepperType<ORDER>, GetterType>::setCurrentValue(double newValue) {
		assert(this->stepper->isInField());
		this->getter->setValue(this->stepper->index, newValue);
	}

	template <template<std::size_t>class IteratorType, template<std::size_t>class StepperType, class GetterType, std::size_t ORDER>
	inline void PureFieldIterator<IteratorType<ORDER>, StepperType<ORDER>, GetterType>::setCurrentNeighbor(std::size_t dimension, int offset, double newValue) {
		assert(this->stepper->neighborInField(dimension, offset));
		std::size_t neighborIndex = this->stepper->linearNeighborIndex(dimension, offset);
		this->getter->setValue(neighborIndex, newValue);
	}

	template <template<std::size_t>class IteratorType, template<std::size_t>class StepperType, class GetterType, std::size_t ORDER>
	inline std::size_t PureFieldIterator<IteratorType<ORDER>, StepperType<ORDER>, GetterType>::size(std::size_t dimension) const {
		return this->stepper->size[dimension];
	}


	/** Protected methods ***/
	template <template<std::size_t>class IteratorType, template<std::size_t>class StepperType, class GetterType, std::size_t ORDER>
	inline void PureFieldIterator<IteratorType<ORDER>, StepperType<ORDER>, GetterType>::initialize(const std::array<std::size_t, ORDER>& sizes, double *data) {
		this->stepper = new StepperType<ORDER>(sizes);
		this->getter = new GetterType(data);
	}

} /* namespace Iterators */
} /* namespace Haparanda */

#endif /* PUREFIELDITERATOR_HPP_ */
