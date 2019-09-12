#ifndef VALUEFIELDBOUNDARYITERATOR_HPP_
#define VALUEFIELDBOUNDARYITERATOR_HPP_

#include "BoundaryIterator.hpp"
#include "PureFieldIterator.hpp"
#include "ValueArray.hpp"

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
			: public PureFieldIterator<ORDER>,
			  public BoundaryIterator<ORDER>
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

		virtual void setBoundaryToIterate(const BoundaryId& boundary);

	protected:
		/**
		 * Initialize getter with a ValueArray object containing the values
		 * in the array given as argument.
		 *
		 * @param data The values stored in the field
		 */
		virtual void createGetter(double *data);

		/**
		 * Initialize stepper with a BoundaryStepper object.
		 *
		 * @param sizes Size of the field which is to be iterated through, in each dimension
		 */
		virtual void createStepper(const std::array<std::size_t, ORDER>& sizes);
	};

	template <std::size_t ORDER>
	ValueFieldBoundaryIterator<ORDER>::ValueFieldBoundaryIterator(
				const std::array<std::size_t, ORDER>& sizes, double *values) {
		this->initialize(sizes, values);
	}

	template <std::size_t ORDER>
	ValueFieldBoundaryIterator<ORDER>::~ValueFieldBoundaryIterator() {
	}


	/*** Protected methods ***/
	template <std::size_t ORDER>
	inline void ValueFieldBoundaryIterator<ORDER>::createGetter(double *values) {
		this->getter = new ValueArray(values);
	}

	template <std::size_t ORDER>
	inline void ValueFieldBoundaryIterator<ORDER>::createStepper(const std::array<std::size_t, ORDER>& sizes) {
		this->stepper = new BoundaryStepper<ORDER>(sizes);
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
