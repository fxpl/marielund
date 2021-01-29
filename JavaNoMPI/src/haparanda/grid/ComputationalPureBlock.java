package haparanda.grid;

import haparanda.iterators.*;
import haparanda.utils.Task;

/**
 * Computational block without ghost regions.
 *
 * Note that this type of block assumes the element indices to be
 * consecutive!
 *
 * @author Malin Kallen 2017-2018
 */
public class ComputationalPureBlock extends ComputationalBlock
{
	/**
	 * Set the size of the inner region of this block to elementsPerDim and
	 * initialize the values of the block using the array given as argument.
	 * @param elementsPerDim Number of elements along each dimension
	 * @param values Array containing all function values which this block will buffer
	 */
	public ComputationalPureBlock(int elementsPerDim, double[] values) {
		super(elementsPerDim, values);
	}
	
	public BoundaryIterator getBoundaryIterator() {
		return getBoundaryIterator(null);
	}

	public BoundaryIterator getBoundaryIterator(Task currentTask) {
		assert(null != this.values);
		int[] sizes = getSizeArray();
		return new ValueFieldBoundaryIterator(
				sizes, this.values, this.smallestIndex);
	}

	public FieldIterator getInnerIterator() {
		return getInnerIterator(null);
	}
	
	public FieldIterator getInnerIterator(Task currentTask) {
		assert(null != this.values);
		int[] sizes = getSizeArray();
		return new ValueFieldIterator(sizes, this.values, this.smallestIndex, currentTask);
	}
}
