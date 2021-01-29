package haparanda.iterators;

import haparanda.utils.BoundaryId;
import haparanda.utils.Task;

/**
 * Interface of iterators for the boundary of a data structure of
 * arbitrary dimensionality.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017
 */
public interface BoundaryIterator extends FieldIterator
{
	/**
	 * Choose boundary (@f$x_i@f$ constant) to be iterated by the iterator,
	 * and set iterator to point at the first element of that boundary. This
	 * method should only be called from a serial context.
	 *
	 * @param boundary Id of the new boundary to iterate
	 */
	public abstract void setBoundaryToIterate(final BoundaryId boundary);
	
	/**
	 * Choose boundary (@f$x_i@f$ constant) to be iterated by the iterator,
	 * and set iterator to point at the first element of that boundary. Use
	 * this method when calling from a parallel context!
	 *
	 * @param boundary Id of the new boundary to iterate
	 * @param currentTask Task from which the method is called. Can be set to null in a serial context.
	 */
	public abstract void setBoundaryToIterate(final BoundaryId boundary, Task currentTask);
}
