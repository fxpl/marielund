package haparanda.iterators;

/**
 * A strategy for retrieving values of a field (of tensor) when the values
 * are stored in an array.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017
 */
class ValueArray extends ValueType
{
	private double[] values;
	private int firstIndex;

	/**
	 * @param values Array containing values to be iterated over
	 * @param firstIndex Index in values where the first value to be iterated over is stored
	 */
	public ValueArray(double[] values, int firstIndex) {
		this.firstIndex = firstIndex;
		this.values = values;
	}

	protected double getValue(int index) {
		return values[firstIndex+index];
	}

	protected void setValue(int index, double newValue) {
		values[firstIndex+index] = newValue;
	}
}


