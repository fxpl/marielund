package haparanda.iterators;

import haparanda.utils.Task;

/**
 * Abstract class representing an iterator for a non-composed data structure
 * with dimensionality information included, e.g. a field or a tensor.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017-2018
 */
public abstract class PureFieldIterator implements FieldIterator
{
	protected FieldSteppingStrategy stepper;
	protected ValueType getter;
	
	/**
	 * Create an empty iterator, without stepper or getter. (This
	 * iterator will segfault if you try to do something with it without
	 * initializing the strategies in another way; don't forget to call
	 * initialize or the create* methods in the concrete class!)
	 */
	public PureFieldIterator() {
		stepper = null;
		getter = null;
	}

	public int currentIndex(int dimension) {
		assert(this.stepper.isInField());
		return this.stepper.currentIndex(dimension);
	}

	public double currentNeighbor(int dimension, int offset) {
		assert(this.stepper.neighborInField(dimension, offset));
		int neighborIndex = this.stepper.linearNeighborIndex(dimension, offset);
		return this.getter.getValue(neighborIndex);
	}

	public double currentValue() {
		assert(this.stepper.isInField());
		return this.getter.getValue(this.stepper.index);
	}

	public void first() {
		this.stepper.first();
	}

	public boolean isInField() {
		return this.stepper.isInField();
	}

	public void next() {
		this.stepper.next();
	}

	public void setCurrentValue(double newValue) {
		assert(this.stepper.isInField());
		this.getter.setValue(this.stepper.index, newValue);
	}

	public void setCurrentNeighbor(int dimension, int offset, double newValue) {
		assert(this.stepper.neighborInField(dimension, offset));
		int neighborIndex = this.stepper.linearNeighborIndex(dimension, offset);
		this.getter.setValue(neighborIndex, newValue);
	}

	public int size(int dimension) {
		return this.stepper.size[dimension];
	}

	/**
	 * Initialize getter.
	 *
	 * @param data Array containing values to be iterated over
	 * @param firstIndex Index of data where first element is stored
	 */
	protected abstract void createGetter(double[] data, int firstIndex);

	/**
	 * Initialize stepper.
	 *
	 * @param sizes Size of the field which is to be iterated through, in each dimension
	 * @param currentTask Task is running this method. Set to null in serial contexts.
	 */
	protected abstract void createStepper(final int[] sizes, Task currentTask);

	/**
	 * Create the stepper and the getter using the abstract methods
	 * createGetter(double *) and createSetter. (See further the GoF
	 * pattern "Factory method".)
	 *
	 * @param sizes Size of the field which is to be iterated through, in each dimension
	 * @param data Array containing values to be iterated over
	 * @param firstIndex Index of data where first element is stored
	 * @param currentTask Task is running this method. Set to null in serial contexts.
	 */
	protected final void initialize(final int[] sizes, double[] data, int firstIndex, Task currentTask) {
		createStepper(sizes, currentTask);
		createGetter(data, firstIndex);
	}
}


