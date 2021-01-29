package haparanda.iterators;

import haparanda.utils.Task;

/**
 * Iterator for traversing all elements, stored in an array, of a data
 * structure with dimensionality information included, e.g. a field or a
 * tensor.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017-2018
 */
public class ValueFieldIterator extends PureFieldIterator
{
	/**
	 * Create an iterator which iterates through all values of a field with
	 * values stored in an array. This constructor can only be called in a
	 * serial context.
	 * 
	 * @param sizes Size of the field which is to be iterated through, in each dimension
	 * @param values The values stored in the field
	 * @param firstIndex Index of values where first element is stored
	 */
	public ValueFieldIterator(final int sizes[], double[] values, int firstIndex) {
		this(sizes, values, firstIndex, null);
	}
	
	/**
	 * Create an iterator which iterates through all values of a field with
	 * values stored in an array. Use this constructor in parallel contexts!
	 * 
	 * @param sizes Size of the field which is to be iterated through, in each dimension
	 * @param values The values stored in the field
	 * @param firstIndex Index of values where first element is stored
	 * @param currentTask Task from which the method is called. Can be set to null in a serial context.
	 */
	public ValueFieldIterator(final int sizes[], double[] values, int firstIndex, Task currentTask) {
		super();
		this.initialize(sizes, values, firstIndex, currentTask);
	}

	/**
	 * Initialize getter with a ValueArray object containing the values
	 * in the array given as argument.
	 *
	 * @param values The values stored in the field
	 * @param firstIndex Index of values where first element is stored
	 */
	protected void createGetter(double[] values, int firstIndex) {
		this.getter = new ValueArray(values, firstIndex);
	}

	protected void createStepper(final int sizes[], Task currentTask) {
		this.stepper = new WholeFieldStepper(sizes, currentTask);
	}
}
