package haparanda.iterators;

import haparanda.utils.BoundaryId;
import haparanda.utils.Task;

/**
 * Iterator for a the boundary of a non-composed data structure of arbitrary
 * dimensionality.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017
 */
public class ValueFieldBoundaryIterator
	extends PureFieldIterator
	implements BoundaryIterator
{
	protected BoundaryId currentBoundary;
	
	/**
	 * Create an iterator which iterates over the boundary of a field with
	 * values stored in an array. Note that before starting to iterate, you
	 * have to choose which boundary to iterate over using the method
	 * setBoundaryToIterate!
	 * @param sizes Size of the field which is to be iterated through, in each dimension
	 * @param values The values stored in the field
	 * @param firstIndex Index of data where first element is stored
	 */
	public ValueFieldBoundaryIterator(final int sizes[], double[] values, int firstIndex) {
		// Task data is not used when creating a BoundaryStepper
		this.initialize(sizes, values, firstIndex, null);
	}

	public void setBoundaryToIterate(final BoundaryId boundary) {
		this.setBoundaryToIterate(boundary, null);
	}
	
	public void setBoundaryToIterate(final BoundaryId boundary, Task currentTask) {
		this.currentBoundary = boundary;
		((BoundaryStepper)(this.stepper)).setBoundaryToIterate(this.currentBoundary, currentTask);
		this.first();
	}

	/**
	 * Initialize getter with a ValueArray object containing the values
	 * in the array given as argument.
	 *
	 * @param data The values stored in the field
	 * @param firstIndex Index of data where first element is stored
	 */
	protected void createGetter(double[] data, int fistIndex) {
		this.getter = new ValueArray(data, fistIndex);
	}

	protected void createStepper(final int[] sizes) {
		this.stepper = new BoundaryStepper(sizes);
	}
	
	protected void createStepper(final int[] sizes, Task currentTask) {
		this.stepper = new BoundaryStepper(sizes);
	}
}

