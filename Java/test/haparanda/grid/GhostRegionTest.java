package haparanda.grid;

import static org.junit.Assert.*;
import org.junit.*;
import java.util.Arrays;

import haparanda.iterators.*;
import haparanda.utils.BoundaryId;
import haparanda.utils.HaparandaMath;

/**
 * Unit test for GhostRegion.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017-2018
 */
public class GhostRegionTest 
{
	// Dimensionality of test blocks
	protected final static int DIMENSIONALITY = Integer.getInteger("DIM");
	
	protected int width;
	protected int elementsPerDim;
	
	private int totalSize;
	private double[] values;
	private BoundaryId lower0 = new BoundaryId();
	private GhostRegion region;

	@Before
	public void setUp() {
		assertEquals(3, DIMENSIONALITY);
		width = 2;
		elementsPerDim = 8;
		totalSize = HaparandaMath.power(elementsPerDim, DIMENSIONALITY-1) * width;

		values = new double[totalSize];
		for (int i=0; i<totalSize; i++) {
			values[i] = 1.2 * i;
		}
		region = new GhostRegion(lower0, elementsPerDim, width, values);
	}

	/**
	 * Verify that the constructor that takes the boundary and the size
	 * parameters as arguments creates a ghost region whose size along the
	 * dimension of the boundary id is width, and whose size along all other
	 * dimensions is elementsPerDim.
	 */
	@Test
	public final void testConstructor() {
		GhostRegion emptyRegion = new GhostRegion(lower0, elementsPerDim, width, copyOfValues());
		FieldIterator iterator = emptyRegion.getInnerIterator();
		assertEquals(width, iterator.size(0));
		assertEquals(elementsPerDim, iterator.size(1));
		assertEquals(elementsPerDim, iterator.size(2));
		/* Different boundaries are implicitly tested by tests for
		 * getInnerIterator() final and getBoundaryIterator(). */
	}


	/**
	 * Verify that getBoundaryIterator returns a ValueFieldIterator whose size in
	 * each dimension is the same as for the ghost region. Also verify that its
	 * first value is the first value of the ghost region.
	 */
	@Test
	public final void testGetBoundaryIterator() {
		BoundaryIterator iterator = region.getBoundaryIterator();
		assertEquals(iterator.getClass(), ValueFieldBoundaryIterator.class);
		assertEquals(width, iterator.size(0));
		assertEquals(elementsPerDim, iterator.size(1));
		assertEquals(elementsPerDim, iterator.size(2));
		iterator.setBoundaryToIterate(lower0);
		assertEquals(values[0], iterator.currentValue(), 4*Math.ulp(values[0]));

		BoundaryId right = new BoundaryId(0, false);
		GhostRegion rightRegion = new GhostRegion(right, elementsPerDim, width, copyOfValues());
		BoundaryIterator rightIterator = rightRegion.getBoundaryIterator();
		assertEquals(width, rightIterator.size(0));
		assertEquals(elementsPerDim, rightIterator.size(1));
		assertEquals(elementsPerDim, rightIterator.size(2));
		rightIterator.setBoundaryToIterate(lower0);
		assertEquals(values[0], rightIterator.currentValue(), 4*Math.ulp(values[0]));

		BoundaryId below = new BoundaryId(1,true);
		GhostRegion belowRegion = new GhostRegion(below, elementsPerDim, width, copyOfValues());
		BoundaryIterator belowIterator = belowRegion.getBoundaryIterator();
		assertEquals(elementsPerDim, belowIterator.size(0));
		assertEquals(width, belowIterator.size(1));
		assertEquals(elementsPerDim, belowIterator.size(2));
		belowIterator.setBoundaryToIterate(lower0);
		assertEquals(values[0], belowIterator.currentValue(), 4*Math.ulp(values[0]));

		BoundaryId behind = new BoundaryId(2, false);
		GhostRegion behindRegion = new GhostRegion(behind, elementsPerDim, width, copyOfValues());
		BoundaryIterator behindIterator = behindRegion.getBoundaryIterator();
		assertEquals(elementsPerDim, behindIterator.size(0));
		assertEquals(elementsPerDim, behindIterator.size(1));
		assertEquals(width, behindIterator.size(2));
		behindIterator.setBoundaryToIterate(lower0);
		assertEquals(values[0], behindIterator.currentValue(), 4*Math.ulp(values[0]));
	}

	/**
	 * Verify that getInnerIterator returns a ValueFieldIterator whose size in
	 * each dimension is the same as for the ghost region. Also verify that its
	 * first value is the first value of the ghost region.
	 */
	@Test
	public final void testGetInnerIterator() {
		FieldIterator doubleIterator = region.getInnerIterator();
		assertEquals(ValueFieldIterator.class, doubleIterator.getClass());
		assertEquals(doubleIterator.getClass(), ValueFieldIterator.class);
		assertEquals(width, doubleIterator.size(0));
		assertEquals(elementsPerDim, doubleIterator.size(1));
		assertEquals(elementsPerDim, doubleIterator.size(2));
		assertEquals(values[0], doubleIterator.currentValue(), 4*Math.ulp(values[0]));

		BoundaryId right = new BoundaryId(0, false);
		GhostRegion rightRegion = new GhostRegion(right, elementsPerDim, width, copyOfValues());
		FieldIterator rightIterator = rightRegion.getInnerIterator();
		assertEquals(width, rightIterator.size(0));
		assertEquals(elementsPerDim, rightIterator.size(1));
		assertEquals(elementsPerDim, rightIterator.size(2));
		assertEquals(values[0], rightIterator.currentValue(), 4*Math.ulp(values[0]));

		BoundaryId below = new BoundaryId(1,true);
		GhostRegion belowRegion = new GhostRegion(below, elementsPerDim, width, copyOfValues());
		FieldIterator belowIterator = belowRegion.getInnerIterator();
		assertEquals(elementsPerDim, belowIterator.size(0));
		assertEquals(width, belowIterator.size(1));
		assertEquals(elementsPerDim, belowIterator.size(2));
		assertEquals(values[0], belowIterator.currentValue(), 4*Math.ulp(values[0]));

		BoundaryId behind = new BoundaryId(2, false);
		GhostRegion behindRegion = new GhostRegion(behind, elementsPerDim, width, copyOfValues());
		FieldIterator behindIterator = behindRegion.getInnerIterator();
		assertEquals(elementsPerDim, behindIterator.size(0));
		assertEquals(elementsPerDim, behindIterator.size(1));
		assertEquals(width, behindIterator.size(2));
		assertEquals(values[0], behindIterator.currentValue(), 4*Math.ulp(values[0]));
	}

	private final double[] copyOfValues() {
		return Arrays.copyOf(values, values.length);
	}
}
