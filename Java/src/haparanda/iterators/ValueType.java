package haparanda.iterators;

/**
 * Strategy that defines how values of a field (or tensor) are retrieved, given
 * a linear index.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017
 */
abstract class ValueType
{
	/**
	 * Get the value stored at the specified (linear) index.
	 *
	 * @param index Linear index of the field
	 * @return Value stored at the specified index
	 */
	protected abstract double getValue(int index);

	/**
	 * Change the value stored at the specified (linear) index.
	 *
	 * @param index Linear index of the field
	 * @param newValue Value which the element value is to be changed to
	 */
	protected abstract void setValue(int index, double newValue);
}
