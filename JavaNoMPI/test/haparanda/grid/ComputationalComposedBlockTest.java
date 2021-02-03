package haparanda.grid;

import static org.junit.Assert.*;
import org.junit.*;

import haparanda.iterators.BoundaryIterator;
import haparanda.iterators.ComposedFieldBoundaryIterator;
import haparanda.iterators.FieldIterator;
import haparanda.iterators.ValueFieldIterator;
import haparanda.utils.*;

/**
 * Unit test for ComputationalComposedBlocks.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017-2019, 2021
 */
public class ComputationalComposedBlockTest 
{
	protected static final int DIMENSIONALITY = Integer.getInteger("DIM");
	protected int elementsPerDim;
	protected int totalSize;
	protected int extent;
	protected ComputationalComposedBlock block;
	private double[] values;

	@Before
	public void setUp() {
		elementsPerDim = 10;
		totalSize = HaparandaMath.power(elementsPerDim, DIMENSIONALITY);
		extent = 4;

		values = new double[totalSize];
		for (int i=0; i<totalSize; i++) {
			values[i] = 1.2 * i;
		}

		block = new ComputationalComposedBlock(elementsPerDim, extent, values);
	}
	
	/**
	 * Verify that the constructor that does not take values as an argument
	 * - Sets elementsPerDim to the value provided
	 * - Can be initialized with setValues
	 * The latter means (among other things) that the ghost regions probably are
	 * created correctly.
	 */
	@Test
	public final void testConstructorNoValues() {
		ComputationalComposedBlock lateInitBlock = new ComputationalComposedBlock(elementsPerDim, extent);
		assertEquals(elementsPerDim, lateInitBlock.getElementsPerDim());

		double[] lateValues = new double[totalSize];
		for (int i=0; i<totalSize; i++) {
			lateValues[i] = 1.2 * i;
		}
		lateInitBlock.setValues(lateValues);
		lateInitBlock.initializeSideRegions();
		verifyInnerValues(lateInitBlock, lateValues);
		checkGhostRegions(lateInitBlock);
	}

	/**
	 * Verify that the constructor that takes values as an argument
	 * - Sets elementsPerDim to the value provided
	 * - Sets the value array to the one provided
	 * - Creates ghost regions and initializes them using periodic boundary conditions
	 */
	@Test
	public final void testConstructorValues() {
		assertEquals(elementsPerDim, block.getElementsPerDim());
		verifyInnerValues(block, values);
		block.initializeSideRegions();
		checkGhostRegions(block);
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
	 * each dimension is the width of the block (excluding ghost regions) and
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
	 * Verify that initializeSideRegions initializes each region with values
	 * from the opposite boundary when the block is its own neighbor in all
	 * dimensions.
	 */
	@Test
	public final void testInitializeSideRegions() {
		block.initializeSideRegions();
		BoundaryIterator initializedIterator = block.getBoundaryIterator();
		// Values are the same in all blocks.
		BoundaryIterator oppositeIterator = block.getBoundaryIterator();
		BoundaryId initialized = new BoundaryId();
		for (int d=0; d<2*DIMENSIONALITY; d++) {
			initialized.setDimension(d/2);
			initialized.setIsLowerSide(1==d%2);
			checkGhostRegionValues(initialized, initializedIterator, oppositeIterator);
		}
	}
	
	/**
	 * Verify that initializeSideRegions initializes side regions with values
	 * from its neighbor when it is not its own neighbor.
	 */
	@Test
	public final void testInitializeSideRegions_otherBlocks() {
		double[] leftVals = new double[totalSize];
		double[] rightVals = new double[totalSize];
		for (int i=0; i<totalSize; i++) {
			leftVals[i] = -12 * i;
			rightVals[i] = 12 * i;
		}
		ComputationalComposedBlock leftNeigh = new ComputationalComposedBlock(elementsPerDim, extent, leftVals);
		ComputationalComposedBlock rightNeigh = new ComputationalComposedBlock(elementsPerDim, extent, rightVals);
		block.setNeighbor(new BoundaryId(0, true), leftNeigh);
		block.setNeighbor(new BoundaryId(0, false), rightNeigh);
		block.initializeSideRegions();
		
		BoundaryId left = new BoundaryId(0, true);
		BoundaryIterator blockIterator = block.getBoundaryIterator();
		BoundaryIterator neighborIterator = leftNeigh.getBoundaryIterator();
		checkGhostRegionValues(left, blockIterator, neighborIterator);
		
		BoundaryId right = left.oppositeSide();
		neighborIterator = rightNeigh.getBoundaryIterator();
		checkGhostRegionValues(right, blockIterator, neighborIterator);
	}

	/**
	 * Verify that setValues changes the values stored in a block to the ones in
	 * the array provided as argument.
	 */
	@Test
	public final void testSetValues() {
		double[] newValues = new double[totalSize];
		for (int i=0; i<totalSize; i++) {
			newValues[i] = 7.8*i;
		}
		block.setValues(newValues);
		block.initializeSideRegions();
		verifyInnerValues(block, newValues);
		checkGhostRegions(block);
	}

	/**
	 * Verify that the ghost values of the block provided as argument are
	 * initialized using periodic boundary conditions.
	 *
	 * @param block Block for investigation
	 */
	private final void checkGhostRegions(ComputationalComposedBlock block) {
		BoundaryIterator boundaryIterator = block.getBoundaryIterator();
		BoundaryIterator expectedValuesIterator = block.getBoundaryIterator();
		BoundaryId boundary = new BoundaryId();
		for (int d=0; d<2*DIMENSIONALITY; d++) {
			boundary.setDimension(d/2);
			boundary.setIsLowerSide(1==d%2);
			checkGhostRegionValues(boundary, boundaryIterator, expectedValuesIterator);
		}
	}
	
	/**
	 * Verify that values in the ghost region along the specified boundary is
	 * initialized with values from the boundary of another block.
	 * 
	 * @param boundary Boundary where the ghost region to check resides
	 * @param actualIterator Boundary iterator for the block to be checked
	 * @param expectedIterator Boundary iterator for the block from which ghost values are supposed to be fetched
	 */
	private final void checkGhostRegionValues(final BoundaryId boundary, BoundaryIterator actualIterator, BoundaryIterator expectedIterator) {
		actualIterator.setBoundaryToIterate(boundary);
		BoundaryId oppositeBoundary = boundary.oppositeSide();
		expectedIterator.setBoundaryToIterate(oppositeBoundary);
		while (expectedIterator.isInField()) {
			for (int distance=0; distance<extent; distance++) {
				int dir = boundary.isLowerSide() ? -1 : 1;
				double expected = expectedIterator.currentNeighbor(boundary.getDimension(), dir * distance);
				double actual = actualIterator.currentNeighbor(boundary.getDimension(), dir * (1+distance));
				assertEquals(expected, actual, 4*Math.ulp(expected));
			}
			expectedIterator.next();
			actualIterator.next();
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
