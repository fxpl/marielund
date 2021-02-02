package haparanda.grid;

import java.util.Arrays;

import haparanda.iterators.*;
import haparanda.iterators.Iterable;
import haparanda.utils.*;

/**
 * A class representing ghost regions of a computational block.
 *
 * @author Malin Kallen 2017-2019, 2021
 */
public class GhostRegion implements Iterable
{
	// Dimensionality of the block of which this is a ghost region
	private static final int DIMENSIONALITY = Integer.getInteger("DIM");
	
	private BoundaryId boundary;		// Boundary along which the ghost region is located
	private int width;			// Size along boundary.dimension
	private int elementsPerDim;	// Size along the other dimensions
	private double[] values;

	/**
	 * This constructor allocates memory for the values, but does not
	 * initialize it with values.
	 *
	 * @param boundary Boundary at which the ghost region is located
	 * @param size The size of the ghost region in all dimensions but boundary.dimension
	 * @param width The width of the ghost region in boundary.dimension (= extent of the stencil)
	 */
	public GhostRegion(BoundaryId boundary, int size, int width) {
		int totalSize = HaparandaMath.power(size, DIMENSIONALITY-1) * width;
		double[] values = new double[totalSize];
		initializeMemberVariables(boundary, size, width, values);
	}

	/**
	 * This constructor is for testing purposes.
	 * 
	 * @param boundary Boundary at which the ghost region is located
	 * @param size The size of the ghost region in all dimensions but boundary.dimension
	 * @param width The width of the ghost region in boundary.dimension (= extent of the stencil)
	 * @param values Array containing all the ghost values.
	 */
	GhostRegion(BoundaryId boundary, int size, int width, double[] values) {
		initializeMemberVariables(boundary, size, width, values);
	}
	
	public BoundaryIterator getBoundaryIterator() {
		return getBoundaryIterator(null);
	}

	public BoundaryIterator getBoundaryIterator(Task currentTask) {
		int[] sizes = getSizeArray();
		BoundaryIterator result =
				new ValueFieldBoundaryIterator(sizes, values, 0);
		return result;
	}

	public FieldIterator getInnerIterator() {
		return getInnerIterator(null);
	}
	
	public FieldIterator getInnerIterator(Task currentTask) {
		int[] sizes = getSizeArray();
		FieldIterator result =
				new ValueFieldIterator(sizes, values, 0, currentTask);
		return result;
	}

	/**
	 * Create an array containing the size of the ghost region in each
	 * dimension (that is, elementsPerDim in all directions but the one
	 * represented by boundary). Note that you as a caller has the
	 * responsibility to delete the array when you are done with it!
	 *
	 * @return The array described above
	 */
	private final int[] getSizeArray() {
		int[] sizes = new int[DIMENSIONALITY];
		Arrays.fill(sizes, this.elementsPerDim);
		sizes[boundary.getDimension()] = width;
		return sizes;
	}


	/**
	 * Initialize the member variables boundary, elementsPerDim, width and
	 * values to the value of the argument with the same name.
	 *
	 * @param boundary
	 * @param elementsPerDim
	 * @param width
	 * @param values
	 */
	private final void initializeMemberVariables(BoundaryId boundary, int elementsPerDim, int width, double[] values) {
		this.boundary = boundary;
		this.elementsPerDim = elementsPerDim;
		this.width = width;
		this.values = values;
	}
}

