#ifndef BOUNDARYSTEPPER_HPP_
#define BOUNDARYSTEPPER_HPP_

#include "FieldSteppingStrategy.hpp"
#include "src/utils/BoundaryId.hpp"

using namespace Haparanda::Utils;

namespace Haparanda {
namespace Iterators {

	/**
	 * Field stepping strategy used when only boundary elements of a field are
	 * to be stepped through by the iterator.
	 *
	 * @tparam DIMENSIONALITY The dimensionality of the block whose boundary will be stepped along by this stepper
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2013-2014, 2016
	 */
	template <std::size_t DIMENSIONALITY>
	class BoundaryStepper: public FieldSteppingStrategy<DIMENSIONALITY>
	{
	public:
		/**
		 * @param sizes Size of the field which is to be iterated through, in each dimension
		 */
		BoundaryStepper(const std::array<std::size_t, DIMENSIONALITY>& sizes);

		virtual ~BoundaryStepper();

		/**
		 * Choose boundary (@f$x_i@f$ constant) to be iterated and point at the
		 * first element that this iterator will traverse on that boundary.
		 *
		 * @param boundary Id of the new boundary to iterate
		 */
		virtual void setBoundaryToIterate(const BoundaryId& boundary);

	protected:
		/**
		 * Advance the iterator one step.
		 * Let the axes @f${x_0, x_1, ..., x_{DIMENSIONALITY-1}}@f$ span the
		 * field. The iterators step along the @f$f_i@f$ axis before it steps
		 * along the @f$f_{i+1}@f$ axis. It steps along all axes but the one
		 * which has a constant value along the boundary in question.
		 */
		virtual void next();

		friend class BoundaryStepperTest;
		friend class BoundaryStepperDeathTest;
		template<std::size_t O> friend class BoundaryIterator;

	private:
		BoundaryId boundary;

		/**
		 * Set the member variables minIndex and maxIndex. Note that before
		 * calling this method, you have to initialize dimension and lower.
		 */
		void setIndexLimits();
	};

	template <std::size_t DIMENSIONALITY>
	BoundaryStepper<DIMENSIONALITY>::BoundaryStepper(const std::array<std::size_t, DIMENSIONALITY>& sizes)
	: FieldSteppingStrategy<DIMENSIONALITY>(sizes) {
	}

	template <std::size_t DIMENSIONALITY>
	BoundaryStepper<DIMENSIONALITY>::~BoundaryStepper() {
	}

	template <std::size_t DIMENSIONALITY>
	inline void BoundaryStepper<DIMENSIONALITY>::next() {
		assert(this->isInField());
		const std::size_t stride = this->stride[boundary.getDimension()];
		if ((this->index+1) % stride != 0) {
			this->index++;
		} else {
			assert(boundary.getDimension() < DIMENSIONALITY);
			const std::size_t strideNextDim = this->stride[boundary.getDimension()+1];
			this->index += strideNextDim - (stride - 1);
		}
	}

	template <std::size_t DIMENSIONALITY>
	inline void BoundaryStepper<DIMENSIONALITY>::setBoundaryToIterate(const BoundaryId& boundary) {
		assert(DIMENSIONALITY > boundary.getDimension());
		this->boundary = boundary;
		setIndexLimits();
		this->first();
	}


	/*** Private methods ***/
	template <std::size_t DIMENSIONALITY>
	inline void BoundaryStepper<DIMENSIONALITY>::setIndexLimits() {
		const std::size_t boundarySize = 0 == this->totalSize ? 0 : this->totalSize/this->size[boundary.getDimension()];
		size_t threadId = OMP_THREAD_ID;
		size_t numThreads = OMP_NUM_THREADS;
		size_t offset = boundarySize%numThreads;
		size_t chunk = boundarySize/numThreads;
		if (threadId < offset) chunk++;
		if (0 == boundarySize) {
			this->minIndex = 1;
			this->maxIndex = 0;
		} else {
			const std::size_t stride = this->stride[boundary.getDimension()];
			const std::size_t nextStride = this->stride[boundary.getDimension()+1];
			const size_t minIndexOnBoundary = boundary.isLowerSide() ? 0
					: stride * (this->size[boundary.getDimension()] - 1);

			size_t stepsToMin = chunk*threadId;
			if (threadId >= offset) stepsToMin += offset;
			size_t stepsToMax = stepsToMin + chunk - 1;

			this->minIndex = minIndexOnBoundary + stepsToMin%stride + stepsToMin/stride * nextStride;
			this->maxIndex = minIndexOnBoundary + stepsToMax%stride + stepsToMax/stride * nextStride;
		}
	}

} /* namespace Iterators */
} /* namespace Haparanda */

#endif /* BOUNDARYSTEPPER_HPP_ */
