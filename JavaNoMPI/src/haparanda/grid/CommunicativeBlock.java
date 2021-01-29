package haparanda.grid;

import mpi.*;
import java.util.Arrays;

import haparanda.utils.*;

/**
 * Computational block with ability to communicate boundary data using MPI.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2019
 */
public abstract class CommunicativeBlock extends ComputationalBlock
{
	protected Timer communicationTimer;
	int[] numProcessors = new int[DIMENSIONALITY];
	int[] processorCoordinates = new int[DIMENSIONALITY];
	int[][] neighborRank = new int[DIMENSIONALITY][2];	// TODO: Är detta en bra idé???
	Prequest[] receiveRequest = new Prequest[2*DIMENSIONALITY];
	Request[] sendRequest = new Request[2*DIMENSIONALITY];
	protected CartComm communicator;
	
	public CommunicativeBlock(int elementsPerDim) {
		super(elementsPerDim);
		this.communicationTimer = new Timer();
	}

	public CommunicativeBlock(int elementsPerDim, double[] values) {
		super(elementsPerDim, values);
		this.communicationTimer = new Timer();
	}
	
	/**
	 * Free MPI related stuff.
	 * @throws MPIException 
	 */
	public void free() throws MPIException {
		communicator.free();
	}
	
	/**
	 * @return The total time spent on communication in this object, in seconds
	 */
	public double communicationTime() {
		return communicationTimer.totalElapsedTime(false);
	}

	/**
	 * Wait for all started send requests to finish.
	 * @throws MPIException 
	 */
	public void finishCommunication() throws MPIException {
		this.communicationTimer.start(false);
		Request.waitAll(sendRequest);
		this.communicationTimer.stop();
	}

	/**
	 * @param dim Specified dimension (See return value)
	 * @return Coordinate of the current processor in processor grid along the specified dimension
	 */
	public final int procGridCoord(int dim) {
		return processorCoordinates[dim];
	}

	/**
	 * @param dim Specified dimension (See return value.)
	 * @return Number of processors in processor grid along the specified dimension
	 */
	public final int procGridSize(int dim) {
		return numProcessors[dim];
	}

	/**
	 * Make sure that all data is received at a boundary (and computations
	 * can be done), and initialize the argument of the method with that
	 * boundary.
	 *
	 * @param boundary Will be set to the boundary at which a side region is initialized
	 * @throws MPIException
	 */
	public abstract void receiveDoneAt(BoundaryId boundary) throws MPIException;

	/**
	 * Start sending and receiving data.
	 * @throws MPIException
	 */
	public void startCommunication() throws MPIException {
		startReceive();
		startSend();
	}

	/**
	 * Create block data types to be sent to other processors.
	 * @throws MPIException 
	 */
	protected abstract void initializeBlockDataTypes() throws MPIException;

	/**
	 * Set up the processor topology (Cartesian).
	 * @throws MPIException 
	 */
	protected void initializeProcessorGrid() throws MPIException {	
		boolean[] periodicBV = new boolean[DIMENSIONALITY];
		Arrays.fill(periodicBV, true);
		int totalNumProcessors = MPI.COMM_WORLD.getSize();
		// Set up Cartesian topology
		CartComm.createDims(totalNumProcessors, this.numProcessors);
		this.communicator = MPI.COMM_WORLD.createCart(this.numProcessors, periodicBV, false);
		int rank = communicator.getRank();
		this.processorCoordinates = communicator.getCoords(rank);
		for (int d=0; d<DIMENSIONALITY; d++) {
			ShiftParms shiftRanks = communicator.shift(d, 1);
			neighborRank[d][0] = shiftRanks.getRankSource();
			neighborRank[d][1] = shiftRanks.getRankDest();
		}
	}

	/**
	 * Set up processor topology and create block types for communication.
	 * This only needs to be done once!
	 * @throws MPIException
	 */
	protected void prepareCommunication() throws MPIException {
		initializeProcessorGrid();
		initializeBlockDataTypes();
	}

	/**
	 * Get ready to receive data.
	 * @throws MPIException
	 */
	protected abstract void startReceive() throws MPIException;

	/**
	 * Start sending data.
	 * @throws MPIException
	 */
	protected abstract void startSend() throws MPIException;
}
