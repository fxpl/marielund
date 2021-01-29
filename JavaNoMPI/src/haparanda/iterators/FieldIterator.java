package haparanda.iterators;

/**
 * Interface for iterators of data structures with dimensionality information
 * included, e.g. a field or a tensor.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017
 */
public interface FieldIterator
{
	/**
	 * @param dimension Specifies which index should be retrieved, see description of the return value
	 * @return The <code>dimension</code>:th coordinate of the index of the element currently pointed at by the iterator
	 */
	public  int currentIndex(int dimension);

	/**
	 * Get the value of a neighbor of the element currently pointed at by
	 * the iterator.
	 *
	 * @param dimension The dimension in which the requested neighbor is located
	 * @param offset The distance from the element at index to the requested neighbor. A positive value means that the neighbor is located ''above'' (i.e. has higher index than) the element at index, while a negative sign means that the neighbor is located ''below'' (i.e. has lower index than) the element at index.
	 * @return The value of the requested neighbor
	 */
	public  double currentNeighbor(int dimension, int offset);

	/**
	 * @return The value of the element currently pointed at by the iterator
	 */
	public  double currentValue();

	/**
	 * Restart the iterator: Set it to point at the first element.
	 */
	public  void first();

	/**
	 * @return true if the iterator points at an element in the field, false if it is outside
	 */
	public  boolean isInField();

	/**
	 * Advance the iterator one step.
	 */
	public  void next();

	/**
	 * Change the value of a neighbor of the element currently pointed at
	 * by the iterator.
	 *
	 * @param newValue Value to change the current element to
	 */
	public  void setCurrentNeighbor(int dimension, int offset, double newValue);

	/**
	 * Change the value of the element currently pointed at by the iterator.
	 *
	 * @param dimension The dimension in which the requested neighbor is located
	 * @param offset The distance from the element at index to the requested neighbor. A positive value means that the neighbor is located ''above'' (i.e. has higher index than) the element at index, while a negative sign means that the neighbor is located ''below'' (i.e. has lower index than) the element at index.
	 * @param newValue Value to change the current element to
	 */
	public  void setCurrentValue(double newValue);

	/**
	 * @param dimension The dimension along which the size should be retrieved
	 * @return The size of the field along the specified dimension
	 */
	public  int size(int dimension);
}
