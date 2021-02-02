package haparanda.grid;

import haparanda.iterators.*;
import haparanda.utils.*;

/**
 * Computational block with the inner values and the ghost regions in
 * separate structures.
 *
 * Note that this type of block assumes the element indices to be
 * consecutive!
 *
 * @author Malin Kallen 2017-2019, 2021
 */
public class ComputationalComposedBlock extends ComputationalBlock
{
	GhostRegion[][] ghostRegions;
	private int extent;  // Size in dimension i of ghost regions located along the boundaries where x_i is constant
	private ComputationalBlock[] neighbors = new ComputationalBlock[2*DIMENSIONALITY];
	
	/**
	 * Initialize size-related variables and neighbor array, and create ghost
	 * regions.
	 * 
	 * Note that the block is set as its own neighbor in each
	 * direction. When several blocks are present, neighbors need to be sed
	 * explicitly!
	 *
	 * @param elementsPerDim Block size in each dimension
	 * @param extent Width of ghost regions
	 */
	public ComputationalComposedBlock(int elementsPerDim, int extent) {
		super(elementsPerDim);
		this.extent = extent;
		createGhostRegions();
		for (int i=0; i<2*DIMENSIONALITY; i++) {
			this.neighbors[i] = this;
			// TODO: Egen metod! Metod för att sätta andra!
		}
	}
	
	/**
	 * Initialize size-related variables and neighbor array, and create ghost
	 * regions. Initialize the block with its values.
	 * 
	 * Note that the block is set as its own neighbor in each
	 * direction. When several blocks are present, neighbors need to be sed
	 * explicitly!
	 *
	 * @param elementsPerDim Block size in each dimension
	 * @param extent Width of ghost regions
	 * @param values Array containing values to be stored in this block
	 * @throws MPIException 
	 */
	public ComputationalComposedBlock(int elementsPerDim, int extent, double[] values) {
		super(elementsPerDim, values);
		this.extent = extent;
		createGhostRegions();
		for (int i=0; i<2*DIMENSIONALITY; i++) {
			this.neighbors[i] = this;
			// TODO: Egen metod! Metod för att sätta andra!
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
	
	/**
	 * Initialize all side regions with values from neighbors.
	 */
	public void initializeSideRegions() {
		BoundaryId boundary = new BoundaryId();
		// Loop over the boundaries
		for (int i=0; i<DIMENSIONALITY; i++) {
			for (int j=0; j<2; j++) {
				boundary.setDimension(i);
				boundary.setIsLowerSide(0==j);
				initializeSideRegion(neighbors[2*i + j], boundary);
			}
		}
	}
	
	/**
	 * Initialize the ghost region at the side specified by boundary using
	 * values from a neighboring block.
	 *
	 * @param neighbor Neighboring block to fetch values from
	 * @param boundary Representation of the side at which the ghost region to initialize is located
	 */
	private void initializeSideRegion(final ComputationalBlock neighbor, final BoundaryId boundary) {
		// Iterator for fetching values
		BoundaryIterator neighborIterator = neighbor.getBoundaryIterator();
		BoundaryId oppositeBoundary = boundary.oppositeSide();
		neighborIterator.setBoundaryToIterate(oppositeBoundary);
		
		// Iterator for the ghost region that is to be initialized
		int dirIndex = boundary.isLowerSide() ? 0 : 1;
		BoundaryIterator ghostIterator = ghostRegions[boundary.getDimension()][dirIndex].getBoundaryIterator();
		ghostIterator.setBoundaryToIterate(oppositeBoundary);
		
		int direction = boundary.isLowerSide() ? -1 : 1;
		
		// Copy data into ghost region
		while (neighborIterator.isInField()) {
			for (int i=0; i<this.extent; i++) {
				ghostIterator.setCurrentNeighbor(boundary.getDimension(), i*direction,
						neighborIterator.currentNeighbor(boundary.getDimension(),  i*direction));
			}
			neighborIterator.next();
			ghostIterator.next();
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
	}
}
