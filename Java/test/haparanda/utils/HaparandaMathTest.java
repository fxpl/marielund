package haparanda.utils;

import static org.junit.Assert.*;
import org.junit.*;

/**
 * Test class for HaparandaMath.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017
 */
public class HaparandaMathTest
{
	/**
	 * Verify the behavior of power in the following cases:
	 * 5^0 = 1
	 * 5^1 = 5
	 * 5^3 = 125
	 * (-5)^3 = -125
	 * 2^10 = 1024
	 * 2^11 = 2048
	 */
	@Test
	public final void testPower() {
		assertEquals(1, HaparandaMath.power(5, 0));
		assertEquals(5, HaparandaMath.power(5, 1));
		assertEquals(125, HaparandaMath.power(5, 3));
		assertEquals(-125, HaparandaMath.power(-5, 3));
		assertEquals(1024, HaparandaMath.power(2, 10));
		assertEquals(2048, HaparandaMath.power(2, 11));
	}
}
