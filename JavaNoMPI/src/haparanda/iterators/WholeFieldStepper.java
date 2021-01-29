package haparanda.iterators;

import haparanda.utils.Task;

/**
 * Field stepping strategy used when all elements in a field are to be
 * stepped through by the iterator, and indices span
 * from 0 to total size - 1.
 *
 * @author Malin Kallen 2017-2018
 */
class WholeFieldStepper extends FieldSteppingStrategy
{	
	/**
	 * This constructor is intended to be called from a parallel context
	 * 
	 * @param sizes Size of the field which is to be iterated through, in each dimension
	 * @param task (Parallel) task that called this constructor. Can be set to null in a serial context
	 */
	public WholeFieldStepper(final int[] sizes, Task task) {
		super(sizes);
		if (null == task) {
			setIndexLimits(0, 1);
		} else {
			setIndexLimits(task.getId(), task.getNumTasks());
		}
		this.first();
	}

	/**
	 * Advance the iterator one step.
	 * Let the axes @f${x_0, x_1, ..., x_{order-1}}@f$ span the field.
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
	protected void next() {
		assert(this.isInField());
		this.index++;
	}
	
	/**
	 * Set the member variables minIndex and maxIndex
	 * 
	 * @param taskId ID of the current task
	 * @param numTask Number of tasks sharing the work 
	 */
	private void setIndexLimits(int taskId, int numTasks) {
		int chunk = this.totalSize / numTasks;
		int offset = this.totalSize % numTasks;
		if (taskId < offset) {
			chunk++;
			this.minIndex = chunk * taskId;
		} else {
			this.minIndex = chunk * taskId + offset;
		}
		this.maxIndex = this.minIndex + chunk - 1;
	}
}
