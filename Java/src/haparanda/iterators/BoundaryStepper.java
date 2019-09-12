package haparanda.iterators;

import haparanda.utils.*;

/**
 * Field stepping strategy used when only boundary elements of a field are
 * to be stepped through by the iterator.
 *
 * @author Malin Kallen 2017-2018
 */
class BoundaryStepper extends FieldSteppingStrategy
{
	BoundaryId boundary;

	/**
	 * @param sizes Size of the field which is to be iterated through, in each dimension
	 */
	public BoundaryStepper(final int[] sizes) {
		super(sizes);
	}
	
	/**
	 * Choose boundary (@f$x_i@f$ constant) to be iterated and point at the
	 * first element that this iterator will traverse on that boundary. This
	 * method assumes that it is called from a serial context!
	 *
	 * @param boundary Id of the new boundary to iterate
	 */
	public void setBoundaryToIterate(final BoundaryId boundary) {
		setBoundaryToIterate(boundary, null);
	}

	/**
	 * Choose boundary (@f$x_i@f$ constant) to be iterated and point at the
	 * first element that this iterator will traverse on that boundary. Use
	 * this method when calling from a thread parallel context!
	 *
	 * @param boundary Id of the new boundary to iterate
	 * @param taskId Task from which the method is called
	 */
	public void setBoundaryToIterate(final BoundaryId boundary, Task currentTask) {
		assert(ORDER > boundary.getDimension());
		this.boundary = boundary;
		if (null==currentTask) {
			setIndexLimits(0, 1);
		} else {
			setIndexLimits(currentTask.getId(), currentTask.getNumTasks());
		}
		this.first();
	}

	/**
	 * Advance the iterator one step.
	 * Let the axes @f${x_0, x_1, ..., x_{order-1}}@f$ span the
	 * field. The iterators step along the @f$f_i@f$ axis before it steps
	 * along the @f$f_{i+1}@f$ axis. It steps along all axes but the one
	 * which has a constant value along the boundary in question.
	 */
	protected void next() {
		assert(this.isInField());
		final int stride = this.stride[boundary.getDimension()];
		if ((this.index+1) % stride != 0) {
			this.index++;
		} else {
			assert(boundary.getDimension() < ORDER);
			final int strideNextDim = this.stride[boundary.getDimension()+1];
			this.index += strideNextDim - (stride - 1);
		}
	}

	/**
	 * Set the member variables minIndex and maxIndex. Note that before
	 * calling this method, you have to initialize dimension and lower.
	 * 
	 * @param taskId ID of the current task
	 * @param numTask Number of tasks sharing the work 
	 */
	final private void setIndexLimits(int taskId, int numTasks) {
		final int boundarySize = 0 == this.totalSize ? 0 : this.totalSize/this.size[boundary.getDimension()];
		int chunk = boundarySize/numTasks;
		int offset = boundarySize%numTasks;
		if (taskId<offset) chunk++;
		if (0 == boundarySize) {
			this.minIndex = 1;
			this.maxIndex = 0;
		} else {
			final int stride = this.stride[boundary.getDimension()];
			final int nextStride = this.stride[boundary.getDimension()+1];
			final int minIndexOnBoundary = boundary.isLowerSide() ? 0
					: stride * (this.size[boundary.getDimension()] - 1);
			
			int stepsToMin = chunk * taskId;
			if (taskId >= offset) stepsToMin += offset;
			int stepsToMax = stepsToMin + chunk-1;
	
			this.minIndex = minIndexOnBoundary + stepsToMin%stride + stepsToMin/stride * nextStride;
			this.maxIndex = minIndexOnBoundary + stepsToMax%stride + stepsToMax/stride * nextStride;
		}
	}
}
