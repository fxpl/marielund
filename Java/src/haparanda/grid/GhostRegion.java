package haparanda.grid;

import mpi.*;
import java.nio.DoubleBuffer;
import java.util.Arrays;

import haparanda.iterators.*;
import haparanda.iterators.Iterable;
import haparanda.utils.*;

/**
 * A class representing ghost regions of a computational block.
 *
 * @author Malin Kallen 2017-2019
 */
public class GhostRegion implements Iterable
{
	// Dimensionality of the block of which this is a ghost region
	private static final int DIMENSIONALITY = Integer.getInteger("DIM");
	
	private BoundaryId boundary;		// Boundary along which the ghost region is located
	private int width;			// Size along boundary.dimension
	private int elementsPerDim;	// Size along the other dimensions
	private int totalSize;		// Total number of elements
	private double[] values;
	private DoubleBuffer valuesBuf;	// Direct buffer in which values will be received

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
	
	/**
	 * Copy all values from buffer to values array. 
	 */
	public void fetchBufferValues() {
		valuesBuf.get(values);
		valuesBuf.clear();
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
	 * Initialize a receive from the process with the specified rank in the
	 * communicator given as argument. The values will be stored in this
	 * ghost region. The tag will be 2*D if I am at the upper boundary and
	 * 2*D+1 if I an at the lower boundary, where D is the dimension of my
	 * boundary. (If I am on the lower boundary, I receive data from the
	 * upper boundary and vice versa.)
	 *
	 * @param communicator Communicator in which the data is communicated
	 * @param rank Rank of sending process
	 * @return The request created by recvInit
	 * @throws MPIException
	 */
	public final Prequest initializeReceive(Comm communicator, int rank) throws MPIException {
		int tag = 2 * this.boundary.getDimension();
		if(this.boundary.isLowerSide()) tag += 1;
		return communicator.recvInit(valuesBuf, totalSize, MPI.DOUBLE, rank, tag);
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
	 * Initialize the member variables boundary, elementsPerDim, width,
	 * totalSize and values to the value of the argument with the same name.
	 * Create direct buffer for communication of values.
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
		this.totalSize = HaparandaMath.power(elementsPerDim, DIMENSIONALITY-1)  * width;
		this.values = values;
		valuesBuf = MPI.newDoubleBuffer(totalSize);
	}
}

