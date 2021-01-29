package haparanda.numerics;

import haparanda.grid.*;
import haparanda.utils.*;
import mpi.MPIException;

/**
 * Class representing a local operator that can be applied on one block at
 * the time.
 *
 * @author Malin Kallen, Magnus Grandin
 */
public abstract class BlockOperator
{	
	protected final static int DIMENSIONALITY = Integer.getInteger("DIM");
	// Timer for the computations
	protected Timer computationTimer;

	public BlockOperator() {
		computationTimer = new Timer();
	}

	/**
	 * Apply the operator on the provided data.
	 *
	 * @param input Block representing the data on which the operator will be applied
	 * @param result Block to which the result will be written
	 * @throws MPIException 
	 */
	public final void apply(CommunicativeBlock input, ComputationalBlock result) throws MPIException {
		// Compute the inner parts.
		computationTimer.start(false);
		applyInInnerRegion(input, result);
		computationTimer.stop();
	
		// Compute boundary parts
		applyInBoundaryRegions(input, result);
	}

	/**
	* @return The total time spent on actual computations by this stencil, in seconds
	*/
	public final double computationTime() {
		return computationTimer.totalElapsedTime(false);
	}

	/**
	 * Apply the operator close to the boundary represented by the last
	 * argument, that is where receiving of ghost data outside that boundary
	 * must be finished before the application can be done.
	 *
	 * @param input Block representing the data on which the stencil will be applied
	 * @param result Block to which the result will be written
	 * @param boundary Boundary along which the stencil will be applied
	 */
	protected abstract void applyInBoundaryRegion(ComputationalBlock input, ComputationalBlock result, final BoundaryId boundary);

	/**
	 * Apply the operator on the boundary part, i.e. where receiving of ghost
	 * data must be finished before the application can be done.
	 *
	 * @param input Block representing the data on which the stencil will be applied
	 * @param result Block to which the result will be written
	 * @throws MPIException 
	 */
	protected final void applyInBoundaryRegions(CommunicativeBlock input, ComputationalBlock result) throws MPIException {
		BoundaryId boundary = new BoundaryId();
		for (int d = 0; d < 2 * DIMENSIONALITY; d++) {
			// Find a ghost region that is initialized
			input.receiveDoneAt(boundary);
			computationTimer.start(false);
			applyInBoundaryRegion(input, result, boundary);
			computationTimer.stop();
		}
	}

	/**
	 * Apply the operator in the inner part of the region represented by the
	 * arguments.
	 *
	 * @param input Block representing the data on which the operator will be applied
	 * @param result Block to which the result will be written
	 */
	protected abstract void applyInInnerRegion(final ComputationalBlock input, ComputationalBlock result);
}
