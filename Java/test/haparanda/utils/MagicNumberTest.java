package haparanda.utils;

import static org.junit.Assert.*;
import org.junit.*;

/**
 * Unit test for MagicNumber.
 * 
 * Note: M in MagicNumber is unsigned, but its value is verified by
 * assertEquals. This is fine, because the byte representation should be the
 * same as for the provided expected value (which I conceptually also regard as
 * unsigned).
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017
 */
public class MagicNumberTest 
{
	/**
	 * Verify that that getMagicNumber returns the right values for the divisor
	 * 0. (Division by 0 is not allowed, but as some unit tests for the
	 * iterator classes use the domain size 0, we must be able to handle the
	 * divisor 0. The values computed in this case are never used.) 
	 */
	@Test
	public void testGetMagicNumbers0() {
		MagicNumber m = MagicNumber.getMagicNumber(0);
		assertEquals("M", 0x7FFFFFFF, m.M);
		assertEquals("a", 0, m.a);
		assertEquals("s", 0, m.s);
	}
	
	/**
	 * Verify that that getMagicNumber returns the right values for the divisor
	 * 1.
	 */
	@Test
	public void testGetMagicNumbers1() {
		MagicNumber m = MagicNumber.getMagicNumber(1);
		assertEquals("M", 0, m.M);
		assertEquals("a", 1, m.a);
		assertEquals("s", 0, m.s);
	}
	
	/**
	 * Verify that that getMagicNumber returns the right values for the divisor
	 * 2.
	 */
	@Test
	public void testGetMagicNumbers2() {
		MagicNumber m = MagicNumber.getMagicNumber(2);
		assertEquals("M", 0x80000000, m.M);
		assertEquals("a", 0, m.a);
		assertEquals("s", 0, m.s);
	}
	
	/**
	 * Verify that that getMagicNumber returns the right values for the divisor
	 * 3.
	 */
	@Test
	public void testGetMagicNumbers3() {
		MagicNumber m = MagicNumber.getMagicNumber(3);
		assertEquals("M", 0xAAAAAAAB, m.M);
		assertEquals("a", 0, m.a);
		assertEquals("s", 1, m.s);
	}
	
	/**
	 * Verify that that getMagicNumber returns the right values for the divisor
	 * 4.
	 */
	@Test
	public void testGetMagicNumbers4() {
		MagicNumber m = MagicNumber.getMagicNumber(4);
		assertEquals("M", 0x40000000, m.M);
		assertEquals("a", 0, m.a);
		assertEquals("s", 0, m.s);
	}
	
	/**
	 * Verify that that getMagicNumber returns the right values for the divisor
	 * 5.
	 */
	@Test
	public void testGetMagicNumbers5() {
		MagicNumber m = MagicNumber.getMagicNumber(5);
		assertEquals("M", 0xCCCCCCCD, m.M);
		assertEquals("a", 0, m.a);
		assertEquals("s", 2, m.s);
	}
	
	/**
	 * Verify that that getMagicNumber returns the right values for the divisor
	 * 7. (This is the only case where the add indicator is 1.)
	 */
	@Test
	public void testGetMagicNumbers7() {
		MagicNumber m = MagicNumber.getMagicNumber(7);
		assertEquals("M", 0x24924925, m.M);
		assertEquals("a", 1, m.a);
		assertEquals("s", 3, m.s);
	}
	
	/**
	 * Verify that that getMagicNumber returns the right values for the divisor
	 * 11.
	 */
	@Test
	public void testGetMagicNumbers11() {
		MagicNumber m = MagicNumber.getMagicNumber(11);
		assertEquals("M", 0xBA2E8BA3, m.M);
		assertEquals("a", 0, m.a);
		assertEquals("s", 3, m.s);
	}
	
	/**
	 * Verify that that getMagicNumber returns the right values for the divisor
	 * 12.
	 */
	@Test
	public void testGetMagicNumbers12() {
		MagicNumber m = MagicNumber.getMagicNumber(12);
		assertEquals("M", 0xAAAAAAAB, m.M);
		assertEquals("a", 0, m.a);
		assertEquals("s", 3, m.s);
	}
	
	/**
	 * Verify that that getMagicNumber returns the right values for the divisor
	 * 25.
	 */
	@Test
	public void testGetMagicNumbers25() {
		MagicNumber m = MagicNumber.getMagicNumber(25);
		assertEquals("M", 0x51EB851F, m.M);
		assertEquals("a", 0, m.a);
		assertEquals("s", 3, m.s);
	}
	
	/**
	 * Verify that that getMagicNumber returns the right values for the divisor
	 * 60.
	 */
	@Test
	public void testGetMagicNumbers60() {
		MagicNumber m = MagicNumber.getMagicNumber(60);
		assertEquals("M", 0x88888889, m.M);
		assertEquals("a", 0, m.a);
		assertEquals("s", 5, m.s);
	}
	
	/**
	 * Verify that that getMagicNumber returns the right values for the divisor
	 * 125.
	 */
	@Test
	public void testGetMagicNumbers125() {
		MagicNumber m = MagicNumber.getMagicNumber(125);
		assertEquals("M", 0x10624DD3, m.M);
		assertEquals("a", 0, m.a);
		assertEquals("s", 3, m.s);
	}
	
	/**
	 * Verify that that getMagicNumber returns the right values for the divisor
	 * 625.
	 */
	@Test
	public void testGetMagicNumbers625() {
		MagicNumber m = MagicNumber.getMagicNumber(625);
		assertEquals("M", 0xD1B71759, m.M);
		assertEquals("a", 0, m.a);
		assertEquals("s", 9, m.s);
	}
}
