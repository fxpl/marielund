package haparanda.iterators;

import static org.junit.Assert.*;
import org.junit.*;

/**
 * Unit test for the value fetching strategy ValueArray.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017
 */
public class ValueArrayTest 
{
	private int totalSize;
	private double[] values;
	private ValueArray strategy;

	@Before
	public void setUp() {
		int[] sizes = {3, 4, 5};
		totalSize = sizes[0] * sizes[1] * sizes[2];

		values = new double[totalSize];
		for (int i=0; i<totalSize; i++) {
			values[i] = initDoubleValue(i);
		}
		strategy = new ValueArray(values, 0);
	}

	/**
	 * For each index in the field: Verify that getValue in the strategy
	 * objects return the corresponding value of the field.
	 */
	@Test
	public void testGetValue() {
		for (int i=0; i<totalSize; i++) {
			assertEquals(values[i], strategy.getValue(i), 4*Math.ulp(values[i]));
		}
	}

	/**
	 * For each index in the field: Verify that setValue in the strategy
	 * objects changes the corresponding value of the field.
	 */
	@Test
	public void testSetValue() {
		// Set new values
		for (int i=0; i<totalSize; i++) {
			strategy.setValue(i, otherDoubleValue(i));
		}

		// Verify that the values were changed
		for (int i=0; i<totalSize; i++) {
			assertEquals(otherDoubleValue(i), values[i], 4*Math.ulp(values[i]));
		}
	}

	/**
	 * @return A standard double precision floating point value, which is intended to be assigned to the double array at set up
	 */
	private double initDoubleValue(int i) {
		return 1.2 * i;
	}

	/**
	 * @return A standard double precision floating point value, which is *not* intended to be assigned to the double array at set up
	 */
	private double otherDoubleValue(int i) {
		return 1.3*i;
	}
}
