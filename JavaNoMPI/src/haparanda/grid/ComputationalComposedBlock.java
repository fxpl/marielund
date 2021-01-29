package haparanda.grid;

import mpi.*;
import java.nio.DoubleBuffer;

import haparanda.iterators.*;
import haparanda.utils.*;

/**
 * Computational block with the inner values and the ghost regions in
 * separate structures.
 *
 * Note that this type of block assumes the element indices to be
 * consecutive!
 *
 * @author Malin Kallen 2017-2019
 */
public class ComputationalComposedBlock extends CommunicativeBlock
{
	GhostRegion[][] ghostRegions;
	private int extent;  // Size in dimension i of ghost regions located along the boundaries where x_i is constant
	private Datatype[] commDataBlockTypes = new Datatype[DIMENSIONALITY];
	private DoubleBuffer sendBuffer;
	
	/**
	 * Create ghost regions and initialize everything MPI related.
	 *
	 * @param elementsPerDim Block size in each dimension
	 * @param extent Width of ghost regions
	 * @throws MPIException 
	 */
	public ComputationalComposedBlock(int elementsPerDim, int extent) throws MPIException {
		super(elementsPerDim);
		this.extent = extent;
		createGhostRegions();
		prepareCommunication();
	}
	
	/**
	 * Create ghost regions and initialize everything MPI related.
	 * Initialize the block with its values.
	 *
	 * @param elementsPerDim Block size in each dimension
	 * @param extent Width of ghost regions
	 * @param values Array containing values to be stored in this block
	 * @throws MPIException 
	 */
	public ComputationalComposedBlock(int elementsPerDim, int extent, double[] values) throws MPIException {
		super(elementsPerDim, values);
		this.extent = extent;
		createGhostRegions();
		prepareCommunication();
	}
	
	public void free() throws MPIException {
		super.free();
		for (int i=0; i<DIMENSIONALITY; i++) {
			commDataBlockTypes[i].free();
		}
	}
	
	public BoundaryIterator getBoundaryIterator() {
		return getBoundaryIterator(null);
	}

	public BoundaryIterator getBoundaryIterator(Task currentTask) {
		assert(null != this.values);
		int[] sizes = this.getSizeArray();
		FieldIterator[][] ghostIterators = new FieldIterator[DIMENSIONALITY][2];
		for (int i=0; i<DIMENSIONALITY; i++) {
			for (int j=0; j<2; j++) {
				ghostIterators[i][j] = ghostRegions[i][j].getBoundaryIterator();
			}
		}
		return new ComposedFieldBoundaryIterator(
				sizes, this.values, this.smallestIndex, ghostIterators);
	}
	
	public FieldIterator getInnerIterator() {
		return getInnerIterator(null);
	}

	public FieldIterator getInnerIterator(Task currentTask) {
		assert(null != this.values);
		int[] sizes = this.getSizeArray();
		return new ValueFieldIterator(sizes, this.values, this.smallestIndex, currentTask);
	}
	
	public void receiveDoneAt(BoundaryId boundary) throws MPIException {
		this.communicationTimer.start(false);
		int index = Request.waitAny(this.receiveRequest);
		boundary.setDimension(index/2);
		boundary.setIsLowerSide(1==index%2);
		int upper = boundary.isLowerSide() ? 0 : 1;
		ghostRegions[boundary.getDimension()][upper].fetchBufferValues();
		this.communicationTimer.stop();
	}
	
	protected void initializeBlockDataTypes() throws MPIException {
		int[] stride = new int[DIMENSIONALITY];
		stride[0] = 1;
		for (int d=1; d<DIMENSIONALITY; d++) {
			stride[d] = stride[d-1] * this.elementsPerDim;
		}
		Datatype[] tmpTypes = new Datatype[DIMENSIONALITY+1];
		tmpTypes[0] = MPI.DOUBLE;
		int MPI_DOUBLE_SIZE = 8;	// In bytes
		for (int i=0; i<DIMENSIONALITY; i++) {
			for (int j=0; j<DIMENSIONALITY; j++) {
				if (i==j) {
					tmpTypes[j+1] = Datatype.createHVector(extent, 1, stride[j] * MPI_DOUBLE_SIZE, tmpTypes[j]);
				} else {
					tmpTypes[j+1] = Datatype.createHVector(this.elementsPerDim, 1, stride[j] * MPI_DOUBLE_SIZE, tmpTypes[j]);
				}
			}
			commDataBlockTypes[i] = tmpTypes[DIMENSIONALITY];
			commDataBlockTypes[i].commit();
		}
	}
	
	/**
	 * Note that the requests are only initialized and started if values is
	 * set! 
	 */
	protected void startReceive() throws MPIException {
		if (null != this.values) {
			for (int i=0; i<DIMENSIONALITY; i++) {
				this.receiveRequest[2*i+1] = ghostRegions[i][0].initializeReceive(
						this.communicator, this.neighborRank[i][0]);
				this.receiveRequest[2*i] = ghostRegions[i][1].initializeReceive(
						this.communicator, this.neighborRank[i][1]);
			}
			Prequest.startAll(this.receiveRequest);
		}
	}

	/**
	 * Note that the requests are only initialized and started if values is
	 * set!
	 * @throws MPIException 
	 */
	protected final void startSend() throws MPIException {
		if (null != this.values) {
			startSendingGhostData();
		}
	}

	/**
	 * Allocate memory for the ghost regions and create send buffer.
	 */
	private final void createGhostRegions() {
		ghostRegions = new GhostRegion[DIMENSIONALITY][2];
		for (int i=0; i<DIMENSIONALITY; i++) {
			for (int j=0; j<2; j++) {
				BoundaryId boundary = new BoundaryId(i, 0==j);
				ghostRegions[i][j] = new GhostRegion(boundary, this.elementsPerDim, this.extent);
			}
		}
		int totalSize = HaparandaMath.power(this.elementsPerDim, DIMENSIONALITY);
		sendBuffer = MPI.newDoubleBuffer(totalSize);
	}

	/**
	 * Start initialization of all ghost regions.
	 * 
	 * @throws MPIException 
	 */
	private final void startSendingGhostData() throws MPIException {
		this.communicationTimer.start(false);
		sendBuffer.rewind();
		sendBuffer.put(values);
		for (int d=0; d<DIMENSIONALITY; d++) {
			this.sendRequest[2*d] = this.communicator.iSend(sendBuffer, 1,
					commDataBlockTypes[d], this.neighborRank[d][0], 2*d);
			int stride = HaparandaMath.power(this.elementsPerDim, d);
			int startIndex = (this.elementsPerDim - this.extent) * stride;
			this.sendRequest[2*d+1] = this.communicator.iSend(MPI.slice(sendBuffer, startIndex),
					1, commDataBlockTypes[d], this.neighborRank[d][1], (2*d)+1);
		}
		this.communicationTimer.stop();
	}
}
