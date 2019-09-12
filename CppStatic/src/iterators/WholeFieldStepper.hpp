#ifndef WHOLEFIELDSTEPPER_HPP_
#define WHOLEFIELDSTEPPER_HPP_

#include "FieldSteppingStrategy.hpp"

namespace Haparanda {
namespace Iterators {
	/**
	 * Field stepping strategy used when all elements in a field are to be
	 * stepped through by the iterator, and indices span
	 * from 0 to total size - 1.
	 *
	 * @tparam ORDER Order of the field which this stepper steps through
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2013-2014, 2016
	 */
	template <std::size_t ORDER>
	class WholeFieldStepper: public FieldSteppingStrategy<WholeFieldStepper<ORDER>>
	{
	public:

		/**
		 * @param sizes Size of the field which is to be iterated through, in each dimension
		 */
		WholeFieldStepper(const std::array<std::size_t, ORDER>& sizes);

		virtual ~WholeFieldStepper();

	protected:
		/**
		 * Advance the iterator one step.
		 * Let the axes @f${x_0, x_1, ..., x_{ORDER-1}}@f$ span the field.
		 * The iterator starts by stepping along the @f$x_0@f$ axis. When
		 * having reached the end, it takes one step along the @f$x_1@f$
		 * axis, and again advances @f$x_0@f$ from @f$0@f$ to its max value.
		 * When having reached the end of the @f$x_1@f$ axis, it increases
		 * the @f$x_2@f$ value and so on.
		 *
		 * In other words, the indices traversed by the iterator is, in order:
		 * (0,0,0,0,...,0) (1,0,0,0,...,0) ... (s0,0,0,0,...,0)
		 * (0,1,0,0...,0) (1,1,0,0...,0) ... (s0,1,0,0,...,0)
		 *  ...
		 * (0,s1,0,0,...,0) (1,s1,0,0,...,0) ... (s0,s1,0,0,...,0)
		 * (0,0,1,0,...,0) (1,0,1,0,...,0) ... (s0,0,1,0,...,0)
		 *  ...
		 * (0,0,s2,0,...,0) (1,0,s2,0,...,0) ... (s0,0,s2,0,...,0)
		 *  ...
		 * where si is the size in dimension i
		 */
		void next();

	private:
		friend class WholeFieldStepperTest;
		friend class WholeFieldStepperDeathTest;
		friend class FieldSteppingStrategy<WholeFieldStepper<ORDER>>;

		/**
		 * Set the member variables minIndex and maxIndex.
		 */
			void setIndexLimits();
		};

	template <std::size_t ORDER>
	WholeFieldStepper<ORDER>::WholeFieldStepper(const std::array<std::size_t, ORDER>& sizes)
	: FieldSteppingStrategy<WholeFieldStepper<ORDER>>(sizes) {
		setIndexLimits();
		this->first();
	}

	template <std::size_t ORDER>
	WholeFieldStepper<ORDER>::~WholeFieldStepper() {
	}

	template <std::size_t ORDER>
	inline void WholeFieldStepper<ORDER>::next() {
		assert(this->isInField());
		this->index++;
	}

	template <std::size_t ORDER>
	inline void WholeFieldStepper<ORDER>::setIndexLimits() {
		int chunk = this->totalSize / OMP_NUM_THREADS;
		int offset = this->totalSize % OMP_NUM_THREADS;
		if (OMP_THREAD_ID < offset) {
			chunk++;
			this->minIndex = chunk * OMP_THREAD_ID;
		} else {
			this->minIndex = chunk * OMP_THREAD_ID + offset;
		}
		this->maxIndex = this->minIndex + chunk-1;
	}

} /* namespace Iterators */
} /* namespace Haparanda */

#endif /* WHOLEFIELDSTEPPER_HPP_ */
