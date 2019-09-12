package haparanda.iterators;

import haparanda.utils.Task;

/**
 * An interface for data structures whose elements can be iterated over.
 * regions.
 *
 * @author Malin Kallen 2017-2018
 */
public interface Iterable
{
	/**
	 * Get an iterator which iterates over the boundary of the data
	 * structure. This method should only be used i serial contexts.
	 *
	 * @return An iterator for the boundary of the data structure
	 */
	public BoundaryIterator getBoundaryIterator();
	
	/**
	 * Get an iterator which iterates over the boundary of the data
	 * structure. This method can be used in parallel contexts.
	 *
	 * @param currentTask Task from which the method is called
	 * @return An iterator for the boundary of the data structure
	 */
	public BoundaryIterator getBoundaryIterator(Task currentTask);

	/**
	 * Get an iterator which iterates over the whole data structure. This
	 * method should only be used in serial contexts.
	 *
	 * @return An iterator for the whole data structure
	 */
	public FieldIterator getInnerIterator();
	
	/**
	 * Get an iterator which iterates over the whole data structure. This
	 * method can be used in parallel contexts.
	 *
	 * @param currentTask Task from which the method is called
	 * @return An iterator for the whole data structure
	 */
	public FieldIterator getInnerIterator(Task currentTask);

}

