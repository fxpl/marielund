package haparanda.iterators;

import haparanda.utils.Task;

/**
 * A class representing an iterator over a composed field. The field
 * consists of a main region with the actual values of the field and one
 * side region along each boundary. When a neighbor which is located outside
 * the main region is requested, it is fetched from a side region, if
 * possible.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017-2018
 */
public abstract class ComposedFieldIterator implements FieldIterator
{
	protected final static int ORDER = Integer.getInteger("DIM");
	
	protected FieldIterator mainIterator;
	FieldIterator[][] sideIterators;

	
	public int currentIndex(int dimension) {
		return mainIterator.currentIndex(dimension);
	}

	public double currentNeighbor(int dimension, int offset) {
		final int neighborIndexInDimension = offset + this.currentIndex(dimension);
		if(neighborIndexInDimension < 0) {
			// The neighbor is ''below'' the main region
			int offsetInSideRegion = neighborIndexInDimension + 1;
			return sideIterators[dimension][0].currentNeighbor(dimension, offsetInSideRegion);
		} else if (neighborIndexInDimension >= mainIterator.size(dimension)) {
			// The neighbor is ''above'' the main region
			int offsetInSideRegion = neighborIndexInDimension-mainIterator.size(dimension);
			return sideIterators[dimension][1].currentNeighbor(dimension, offsetInSideRegion);
		} else {
			return mainIterator.currentNeighbor(dimension, offset);
		}
	}

	public double currentValue() {
		return mainIterator.currentValue();
	}

	public void first() {
		mainIterator.first();
		for (int i=0; i<ORDER; i++) {
			for (int j=0; j<2; j++) {
				sideIterators[i][j].first();
			}
		}
	}

	public boolean isInField() {
		return mainIterator.isInField();
	}

	public void setCurrentValue(double newValue) {
		mainIterator.setCurrentValue(newValue);
	}

	public void setCurrentNeighbor(int dimension, int offset, double newValue) {
		final int neighborIndexInDimension = offset + this.currentIndex(dimension);
		if(neighborIndexInDimension < 0) {
			// The neighbor is ''below'' the main region
			int offsetInSideRegion = neighborIndexInDimension + 1;
			sideIterators[dimension][0].setCurrentNeighbor(dimension, offsetInSideRegion, newValue);
		} else if (neighborIndexInDimension >= mainIterator.size(dimension)) {
			// The neighbor is ''above'' the main region
			int offsetInSideRegion = neighborIndexInDimension-mainIterator.size(dimension);
			sideIterators[dimension][1].setCurrentNeighbor(dimension, offsetInSideRegion, newValue);
		} else {
			mainIterator.setCurrentNeighbor(dimension, offset, newValue);
		}
	}

	public int size(int dimension) {
		return mainIterator.size(dimension)
				+ sideIterators[dimension][0].size(dimension)
				+ sideIterators[dimension][1].size(dimension);
	}
	
	/**
	 * Initialize the iterator for the main region.
	 *
	 * @param sizes Size of the main region, in each dimension
	 * @param data Array with values to be iterated over
	 * @param firstIndex Index of data where first element is stored
	 * @param currentTask Task from which the method is called. Can be set to null in a serial context.
	 */
	protected abstract void createMainIterator(final int[] sizes, double[] data, int firstIndex, Task currentTask);

	/**
	 * Initialize the iterators for the side regions.
	 * The array containing the side iterators is structured the same way as
	 * the argument array, i.e. it is an array of length order, where each
	 * element in turn is an array of length 2. Element [i][0] in this array
	 * contains the side region ''below'' the main region along dimension i,
	 * while element [i][1] contains the side region ''above'' the main
	 * region along the same dimension.
	 *
	 * @sideRegions Array of side iterators. The expected structure of this array is described above.
	 */
	protected final void initializeSideIterators(FieldIterator[][] sideIterators) {
		this.sideIterators = sideIterators;
	}

	/**
	 * @return A reference to the iterator of the side region outside the boundary which is currently iterated over
	 */
	protected abstract FieldIterator currentSideIterator();

	/**
	 * Create the iterators for the main region and the side regions
	 * respectively.
	 *
	 * @param sizes Size of the main region, in each dimension
	 * @param data Array containing the values of the main region
	 * @param firstIndex Index of data where first element is stored
	 * @param sideRegions Array containing the side iterators
	 * @param currentTask Task from which the method is called. Can be set to null in a serial context.
	 */
	protected final void initialize(final int[] sizes, double[] data, int firstIndex, FieldIterator[][] sideIterators, Task currentTask) {
		createMainIterator(sizes, data, firstIndex, currentTask);
		initializeSideIterators(sideIterators);
	}
}
