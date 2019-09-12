package haparanda.grid;

import static org.junit.Assert.*;
import org.junit.*;
import mpi.*;

import haparanda.iterators.BoundaryIterator;
import haparanda.iterators.ComposedFieldBoundaryIterator;
import haparanda.iterators.FieldIterator;
import haparanda.iterators.ValueFieldIterator;
import haparanda.utils.*;

/**
 * Unit test for ComputationalComposedBlocks.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017-2019
 */
public class ComputationalComposedBlockTest 
{
	protected static final int DIMENSIONALITY = Integer.getInteger("DIM");
	protected int elementsPerDim;
	protected int totalSize;
	protected int extent;
	protected ComputationalComposedBlock block;
	private double[] values;
	
	@BeforeClass
	public static void initMPI() throws MPIException {
		MPI.Init(new String[0]);
	}

	@Before
	public void setUp() throws MPIException {
		elementsPerDim = 10;
		totalSize = HaparandaMath.power(elementsPerDim, DIMENSIONALITY);
		extent = 4;

		values = new double[totalSize];
		for (int i=0; i<totalSize; i++) {
			values[i] = 1.2 * i;
		}

		block = new ComputationalComposedBlock(elementsPerDim, extent, values);
	}
	
	@After
	public void tearDown() throws MPIException {
		block.free();
	}
	
	@AfterClass
	public static void finalizeMPI() throws MPIException {
		MPI.Finalize();
	}
	
	/**
	 * Verify that the constructor that does not take values as an argument
	 * - Sets elementsPerDim to the value provided
	 * - Can be initialized with setValues
	 * The latter means (among other things) that the ghost regions probably are
	 * created correctly.
	 * @throws MPIException 
	 */
	@Test
	public final void testConstructorNoValues() throws MPIException {
		ComputationalComposedBlock lateInitBlock = new ComputationalComposedBlock(elementsPerDim, extent);
		assertEquals(elementsPerDim, lateInitBlock.getElementsPerDim());

		double[] lateValues = new double[totalSize];
		for (int i=0; i<totalSize; i++) {
			lateValues[i] = 1.2 * i;
		}
		lateInitBlock.setValues(lateValues);
		lateInitBlock.startCommunication();
		verifyInnerValues(lateInitBlock, lateValues);
		verifyGhostRegionValues(lateInitBlock);
		lateInitBlock.finishCommunication();
		lateInitBlock.free();

	}

	/**
	 * Verify that the constructor that takes values as an argument
	 * - Sets elementsPerDim to the value provided
	 * - Sets the value array to the one provided
	 * - Creates ghost regions and initializes them using periodic boundary conditions
	 * Note that this test must not be run when there is > 1 processor in the
	 * simulation
	 * @throws MPIException 
	 */
	@Test
	public final void testConstructorValues() throws MPIException {
		block.startCommunication();
		assertEquals(elementsPerDim, block.getElementsPerDim());
		verifyInnerValues(block, values);
		verifyGhostRegionValues(block);
		block.finishCommunication();
	}
	
	/**
	 * On the middle of the boundary 0, false: Verify that the boundary
	 * iterator causes the program to abort if it is requested to return the
	 * closest neighbor outside the ghost region outside the boundary in
	 * question.
	 */
	@Test (expected=AssertionError.class)
	public final void testCurrentBoundaryNeighbor() {
		int boundarySize = totalSize/elementsPerDim;
		BoundaryIterator bdIterator = block.getBoundaryIterator();
		bdIterator.setBoundaryToIterate(new BoundaryId(0, false));
		// Step to middle of the boundary
		for (int i=0; i< boundarySize/2; i++) {
			bdIterator.next();
		}
		bdIterator.currentNeighbor(0, extent + 1);
	}

	/**
	 * Verify that procGridCoords returns a positive number smaller than the
	 * size of the processor grid in each dimension.
	 */
	@Test
	public final void testProcGridCoord() {
		for (int d=0; d<DIMENSIONALITY; d++) {
			assertTrue(0 <= block.procGridCoord(d)
					&& block.procGridCoord(d)<block.procGridSize(d));
		}
	}

	/**
	 * Verify that procGridSize returns a number between 1 and the total number
	 * of processors in all dimensions.
	 * @throws MPIException 
	 */
	@Test
	public final void testProcGridSize() throws MPIException {
		for (int d=0; d<DIMENSIONALITY; d++) {
			assertTrue(0 < block.procGridSize(d)
					&& block.procGridSize(d) <= MPI.COMM_WORLD.getSize());
		}
	}
	
	/**
	 * Verify that getBoundaryIterator returns a ComposedFieldBoundaryIterator
	 * whose size in each dimension is the with of the block (including ghost
	 * regions) and whose first value is the the one specified by the value
	 * array and the smallest index.
	 */
	@Test
	public final void testGetBoundaryIterator() {
		BoundaryIterator iterator = block.getBoundaryIterator();
		// Verify type
		assertEquals(ComposedFieldBoundaryIterator.class, iterator.getClass());
		// Verify size
		for (int i=0; i<DIMENSIONALITY; i++) {
			assertEquals(elementsPerDim+2*extent, iterator.size(i));
		}
		// Verify (first) value
		iterator.setBoundaryToIterate(new BoundaryId(0, true));
		assertEquals(values[0], iterator.currentValue(), 4*Math.ulp(values[0]));
	}

	/**
	 * Verify that getInnerIterator returns a ValueFieldIterator whose size in
	 * each dimension is the with of the block (excluding ghost regions) and
	 * whose first value is the the one specified by the value array and the
	 * smallest index.
	 */
	@Test
	public final void testGetInnerIterator() {
		FieldIterator iterator = block.getInnerIterator();
		// Verify type
		assertEquals(ValueFieldIterator.class, iterator.getClass());
		// Verify size
		for (int i=0; i<DIMENSIONALITY; i++) {
			assertEquals(elementsPerDim, iterator.size(i));
		}
		// Verify (first) value
		iterator.first();
		assertEquals(values[0], iterator.currentValue(), 4*Math.ulp(values[0]));
	}
	
	/**
	 * Verify that the ghost region of a side of the block is initialized with
	 * values from the opposite boundary (Values are the same on all nodes!) when
	 * receiveDoneAt returns and the argument is initialized with values of the
	 * actual boundary. Do this for every boundary of the block.
	 * @throws MPIException 
	 */
	@Test
	public final void testReceiveDoneAt() throws MPIException {
		block.startCommunication();
		BoundaryId initialized = new BoundaryId();
		BoundaryIterator initializedIterator = block.getBoundaryIterator();
		// Values are the same in all blocks.
		BoundaryIterator oppositeIterator = block.getBoundaryIterator();
		for (int i=0; i<2*DIMENSIONALITY; i++) {
			block.receiveDoneAt(initialized);
			initializedIterator.setBoundaryToIterate(initialized);
			BoundaryId oppositeBoundary = initialized.oppositeSide();
			oppositeIterator.setBoundaryToIterate(oppositeBoundary);
			while (oppositeIterator.isInField()) {
				// Check ghost region values.
				for (int distance=0; distance<extent; distance++) {
					int dir = initialized.isLowerSide() ? -1 : 1;
					double expected = oppositeIterator.currentNeighbor(initialized.getDimension(), dir * distance);
					double actual = initializedIterator.currentNeighbor(initialized.getDimension(), dir * (1+distance));
					assertEquals(expected, actual, 4*Math.ulp(expected));
				}
				oppositeIterator.next();
				initializedIterator.next();
			}
		}
		block.finishCommunication();
	}

	/**
	 * Verify that setValues changes the values stored in a block to the ones in
	 * the array provided as argument.
	 * @throws MPIException 
	 */
	@Test
	public final void testSetValues() throws MPIException {
		double[] newValues = new double[totalSize];
		for (int i=0; i<totalSize; i++) {
			newValues[i] = 7.8*i;
		}
		block.setValues(newValues);
		block.startCommunication();
		verifyInnerValues(block, newValues);
		verifyGhostRegionValues(block);
		block.finishCommunication();
	}

	/**
	 * Verify that the ghost values of the block provided as argument are
	 * initialized using periodic boundary conditions.
	 *
	 * @param block Block for investigation
	 * @throws MPIException 
	 */
	private final void verifyGhostRegionValues(ComputationalComposedBlock block) throws MPIException {
		BoundaryIterator boundaryIterator = block.getBoundaryIterator();
		BoundaryIterator expectedValuesIterator = block.getBoundaryIterator();
		BoundaryId boundary = new BoundaryId();
		for (int d=0; d<2*DIMENSIONALITY; d++) {
			block.receiveDoneAt(boundary);
			boundaryIterator.setBoundaryToIterate(boundary);
			BoundaryId oppositeBoundary = boundary.oppositeSide();
			expectedValuesIterator.setBoundaryToIterate(oppositeBoundary);
			while (boundaryIterator.isInField()) {
				// Check the ghost values outside the current element.
				for (int offset=1; offset<extent; offset++) {
					int neighborOffset = offset-1;
					int directedOffset = boundary.isLowerSide() ? -offset : offset;
					int directedNeighborOffset = boundary.isLowerSide() ? -neighborOffset : neighborOffset;
					double expected = expectedValuesIterator.currentNeighbor(boundary.getDimension(), directedNeighborOffset);
					double actual = boundaryIterator.currentNeighbor(boundary.getDimension(), directedOffset);
					assertEquals(expected, actual, 4*Math.ulp(expected));
				}
				boundaryIterator.next();
				expectedValuesIterator.next();
			}
		}
	}

	/**
	 * Verify that the inner values in the block provided as argument are the
	 * same as the elements of the array.
	 *
	 * @param block Block for investigation
	 * @param expectedValues Expected values of the block
	 */
	private final void verifyInnerValues(ComputationalBlock block, double[] expectedValues) {
		FieldIterator iterator = block.getInnerIterator();
		for (int i=0; i<totalSize; i++) {
			assertEquals(expectedValues[i], iterator.currentValue(), 4*Math.ulp(expectedValues[i]));
			iterator.next();
		}
	}
}
