package haparanda.iterators;

import haparanda.utils.BoundaryId;
import haparanda.utils.Task;

/**
 * A class representing an iterator over the boundary of a composed field.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017
 */
public class ComposedFieldBoundaryIterator extends ComposedFieldIterator implements BoundaryIterator
{
	protected BoundaryId currentBoundary;
	
	/**
	 * @param sizes Size of the main region of the field which is to be iterated through, in each dimension
	 * @param data The values stored in the main region
	 * @param firstIndex Index of data where first element is stored
	 * @param sideIterators Array of side iterators
	 */
	public ComposedFieldBoundaryIterator(final int[] sizes, double[] data, int firstIndex, FieldIterator[][] sideIterators) {
		super();
		// We can set current task to null, since its data is not used when creating boundary steppers.
		this.initialize(sizes, data, firstIndex, sideIterators, null);
	}

	public void first() {
		this.mainIterator.first();
		currentSideIterator().first();
	}

	public void next() {
		this.mainIterator.next();
		currentSideIterator().next();
	}

	public void setBoundaryToIterate(final BoundaryId boundary) {
		this.setBoundaryToIterate(boundary, null);
	}
		
	public void setBoundaryToIterate(final BoundaryId boundary, Task currentTask) {
		this.currentBoundary = boundary;
		((BoundaryIterator)this.mainIterator).setBoundaryToIterate(this.currentBoundary, currentTask);
		BoundaryId oppositeBoundary = this.currentBoundary.oppositeSide();
		((BoundaryIterator)currentSideIterator()).setBoundaryToIterate(oppositeBoundary, currentTask);
		this.first();
	}

	protected void createMainIterator(final int[] sizes, double[] data, int firstIndex, Task currentTask) {
		// Current task is here for consistency, but it is not used at BoundaryStepper creation.
		this.mainIterator = new ValueFieldBoundaryIterator(sizes, data, firstIndex);
	}

	protected FieldIterator currentSideIterator() {
		int currentDimension = this.currentBoundary.getDimension();
		int currentSide = this.currentBoundary.isLowerSide() ? 0 : 1;
		return this.sideIterators[currentDimension][currentSide];
	}
}
