package haparanda.grid;

import static org.junit.Assert.*;
import org.junit.*;

import haparanda.iterators.*;
import haparanda.utils.*;

/**
 * Unit test for ComputationalPureBlocks.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017
 */
public class ComputationalPureBlockTest 
{
	protected static final int DIMENSIONALITY = Integer.getInteger("DIM");
	
	private int totalSize;
	private double[] values;
	protected int elementsPerDim;
	protected ComputationalPureBlock block;

	@Before
	public void setUp() {
		elementsPerDim = 10;
		totalSize = HaparandaMath.power(elementsPerDim, DIMENSIONALITY);

		values = new double[totalSize];
		for (int i=0; i<totalSize; i++) {
			values[i] = 1.2 * i;
		}
		block = new ComputationalPureBlock(elementsPerDim, values);
	}

	/**
	 * Verify that the constructor that only takes the values as argument
	 * creates a block with the first value of the value array as its first
	 * element.
	 */
	@Test
	public final void testConstructor() {
		ComputationalPureBlock blockWithoutIndexBlock = new ComputationalPureBlock(elementsPerDim, values);
		FieldIterator iterator = blockWithoutIndexBlock.getInnerIterator();
		// Verify (first) value
		iterator.first();
		assertEquals(values[0], iterator.currentValue(), 4*Math.ulp(values[0]));
	}

	/**
	 * Verify that getBoundaryIterator returns a ValueFieldBoundaryIterator
	 * whose size in each dimension is elementsPerDim and whose first value is
	 * the first one in the value array.
	 */
	@Test
	public final void testGetBoundaryIterator() {
		BoundaryIterator iterator = block.getBoundaryIterator();
		// Verify type
		assertEquals(ValueFieldBoundaryIterator.class, iterator.getClass());
		// Verify size
		for (int i=0; i<DIMENSIONALITY; i++) {
			assertEquals(elementsPerDim, iterator.size(i));
		}
		// Verify (first) value
		iterator.setBoundaryToIterate(new BoundaryId(0, true));
		assertEquals(values[0], iterator.currentValue(), 4*Math.ulp(values[0]));
	}

	/**
	 * Verify that getInnerIterator returns a ValueFieldIterator whose size in
	 * each dimension is elementsPerDim and whose first value is the one first
	 * one in the value array.
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
}
