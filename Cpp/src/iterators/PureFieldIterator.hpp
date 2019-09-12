#ifndef PUREFIELDITERATOR_HPP_
#define PUREFIELDITERATOR_HPP_

#include "FieldIterator.hpp"

namespace Haparanda {
namespace Iterators {

	/**
	 * Abstract class representing an iterator for a non-composed data structure
	 * with dimensionality information included, e.g. a field or a tensor.
	 *
	 * @tparam ORDER Order/dimensionality of the data structure
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2014, 2016, 2017
	 */
	template <std::size_t ORDER>
	class PureFieldIterator : virtual public FieldIterator<ORDER>
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

		virtual std::size_t currentIndex(std::size_t dimension) const;

		virtual double currentNeighbor(std::size_t dimension, int offset) const;

		virtual double currentValue() const;

		virtual void first();

		virtual bool isInField() const;

		virtual void next();

		virtual void setCurrentValue(double newValue);

		virtual void setCurrentNeighbor(std::size_t dimension, int offset, double newValue);

		virtual std::size_t size(std::size_t dimension) const;

	protected:
		FieldSteppingStrategy<ORDER> *stepper;
		ValueType *getter;

		/**
		 * Initialize getter.
		 *
		 * @param data Pointer to the value of the first element in the field
		 */
		virtual void createGetter(double *data) = 0;

		/**
		 * Initialize stepper.
		 *
		 * @param sizes Size of the field which is to be iterated through, in each dimension
		 */
		virtual void createStepper(const std::array<std::size_t, ORDER>& sizes) = 0;

		/**
		 * Create the stepper and the getter using the abstract methods
		 * createGetter(double *) and createSetter. (See further the GoF
		 * pattern "Factory method".)
		 *
		 * @param sizes Size of the field which is to be iterated through, in each dimension
		 * @param data Pointer to the value of the first element in the field
		 */
		void initialize(const std::array<std::size_t, ORDER>& sizes, double *data);
	};

	template <std::size_t ORDER>
	PureFieldIterator<ORDER>::PureFieldIterator() {
		stepper = NULL;
		getter = NULL;
	}

	template <std::size_t ORDER>
	PureFieldIterator<ORDER>::~PureFieldIterator() {
		if (NULL != stepper) {
			delete stepper;
		}
		if (NULL != getter) {
			delete getter;
		}
	}

	template <std::size_t ORDER>
	inline std::size_t PureFieldIterator<ORDER>::currentIndex(std::size_t dimension) const {
		assert(this->stepper->isInField());
		return this->stepper->currentIndex(dimension);
	}

	template <std::size_t ORDER>
	inline double PureFieldIterator<ORDER>::currentNeighbor(std::size_t dimension, int offset) const {
		assert(this->stepper->neighborInField(dimension, offset));
		std::size_t neighborIndex = this->stepper->linearNeighborIndex(dimension, offset);
		return this->getter->getValue(neighborIndex);
	}

	template <std::size_t ORDER>
	inline double PureFieldIterator<ORDER>::currentValue() const {
		assert(this->stepper->isInField());
		return this->getter->getValue(this->stepper->index);
	}

	template <std::size_t ORDER>
	inline void PureFieldIterator<ORDER>::first() {
		this->stepper->first();
	}

	template <std::size_t ORDER>
	inline bool PureFieldIterator<ORDER>::isInField() const {
		return this->stepper->isInField();
	}

	template <std::size_t ORDER>
	inline void PureFieldIterator<ORDER>::next() {
		this->stepper->next();
	}

	template<std::size_t ORDER>
	inline void PureFieldIterator<ORDER>::setCurrentValue(double newValue) {
		assert(this->stepper->isInField());
		this->getter->setValue(this->stepper->index, newValue);
	}

	template<std::size_t ORDER>
	inline void PureFieldIterator<ORDER>::setCurrentNeighbor(std::size_t dimension, int offset, double newValue) {
		assert(this->stepper->neighborInField(dimension, offset));
		std::size_t neighborIndex = this->stepper->linearNeighborIndex(dimension, offset);
		this->getter->setValue(neighborIndex, newValue);
	}

	template<std::size_t ORDER>
	inline std::size_t PureFieldIterator<ORDER>::size(std::size_t dimension) const {
		return this->stepper->size[dimension];
	}


	/** Protected methods ***/
	template<std::size_t ORDER>
	inline void PureFieldIterator<ORDER>::initialize(const std::array<std::size_t, ORDER>& sizes, double *data) {
		createStepper(sizes);
		createGetter(data);
	}

} /* namespace Iterators */
} /* namespace Haparanda */

#endif /* PUREFIELDITERATOR_HPP_ */
