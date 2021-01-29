package haparanda.grid;

import java.util.Arrays;

import haparanda.iterators.Iterable;

/**
 * A block containing data of a domain block.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017-2018
 */
public abstract class ComputationalBlock implements Iterable
{
	protected final static int DIMENSIONALITY = Integer.getInteger("DIM");
	
	protected int smallestIndex;
	protected int elementsPerDim;
	protected double[] values;
	
	/**
	 * @param elementsPerDim Number of elements along each dimension
	 */
	public ComputationalBlock(int elementsPerDim) {
		initializeMemberVariables(elementsPerDim);
	}
	
	/**
	 * Set the size of the inner region of this block to the size of a
	 * DomainBlock and the width along dimension i of ghost region i to the
	 * value given as parameter. Ghost region i is the ghost region located
	 * outside the boundary where @f$x_i=0@f$.
	 * Also initialize the values of the block using the array given as
	 * argument.
	 * @param elementsPerDim Number of elements along each dimension
	 * @param values Array containing all function values which this block will buffer
	 */
	public ComputationalBlock(int elementsPerDim, double[] values) {
		initializeMemberVariables(elementsPerDim);
		setValues(values);
	}

	/**
	 * @return The number of elements along each dimension of the block
	 */
	public final int getElementsPerDim() {
		return elementsPerDim;
	}

	/**
	 * Change the values of the block to the ones stored in the array given
	 * as argument and start initialization of side regions.
	 *
	 * @param values Array containing all function values that this block will buffer
	 */
	public void setValues(double[] values) {
		this.values = values;
	}

	/**
	 * @return An array with dimensionality elements, all initialized to elementsPerDim
	 */
	protected int[] getSizeArray() {
		int[] sizes = new int[DIMENSIONALITY];
		Arrays.fill(sizes, this.elementsPerDim);
		return sizes;
	}

	/**
	 * Initialize the member variables that are initialized by all
	 * constructors: smallestIndex and elementsPerDim.
	 * @param elementsPerDim Number of elements along each dimension
	 */
	private final void initializeMemberVariables(int elementsPerDim) {
		this.smallestIndex = 0;	// Default value; may be changed in the initialization
		this.elementsPerDim = elementsPerDim;
	}
}
