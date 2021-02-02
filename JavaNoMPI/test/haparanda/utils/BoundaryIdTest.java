package haparanda.utils;

import static org.junit.Assert.*;
import org.junit.*;

/**
 * Unit test for BoundaryId.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017
 */
public class BoundaryIdTest 
{
	private BoundaryId defaultId;
	private BoundaryId upper13;
	
	@Before
	public void setUp() {
		defaultId = new BoundaryId();
		upper13 = new BoundaryId(13, false);
	}

	/**
	 * Verify that the default constructor creates a BoundaryId object that
	 * represents the lower boundary in dimension 0.
	 */
	@Test
	public void testDefaultConstructor() {
		assertEquals(0, defaultId.getDimension());
		assertTrue(defaultId.isLowerSide());
	}

	/**
	 * Verify that oppositeSide returns a new BoundaryId that represents the
	 * boundary along the current dimension, but on the other side of the
	 * region.
	 */
	@Test
	public void testOppositeSide() {
		BoundaryId oppositeDefault = defaultId.oppositeSide();
		assertEquals(0, oppositeDefault.getDimension());
		assertFalse(oppositeDefault.isLowerSide());

		BoundaryId oppositeUpper13 = upper13.oppositeSide();
		assertEquals(13, oppositeUpper13.getDimension());
		assertTrue(oppositeUpper13.isLowerSide());
	}

	/**
	 * Verify that the constructor that takes arguments creates a BoundaryId
	 * object as specified by the arguments.
	 */
	@Test
	public void testParametrizedConstructor() {
		assertEquals(13, upper13.getDimension());
		assertFalse(upper13.isLowerSide());
	}

	/**
	 * Verify that step advances to the next boundary (i.e. the upper
	 * along the current dimension if currently located at the lower, and the
	 * lower in the next dimension otherwise).
	 */
	@Test
	public void testStep() {
		// We start at the lowermost boundary.
		assertEquals(0, defaultId.getDimension());
		assertTrue(defaultId.isLowerSide());

		// Step to the upper boundary.
		defaultId.step();
		assertEquals(0, defaultId.getDimension());
		assertFalse(defaultId.isLowerSide());

		// Step to the lower boundary along the next dimension.
		defaultId.step();
		assertEquals(1, defaultId.getDimension());
		assertTrue(defaultId.isLowerSide());
	}

	/**
	 * Verify that the reset method makes a BoundaryId object represent the
	 * lower boundary in dimension 0.
	 */
	@Test
	public void testReset() {
		upper13.reset();
		assertEquals(0, upper13.getDimension());
		assertTrue(upper13.isLowerSide());

		// The state of default object does not change.
		defaultId.reset();
		assertEquals(0, defaultId.getDimension());
		assertTrue(defaultId.isLowerSide());
	}
}
