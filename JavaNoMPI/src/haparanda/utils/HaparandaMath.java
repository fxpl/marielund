package haparanda.utils;

public class HaparandaMath
{
	/**
	 * Calculate base^exp for an integer base and a positive integer exponent.
	 *
	 * @param base Base of the expression
	 * @param exp Exponent of the expression. This value has to be >= 0.
	 * @return base^exp
	 */
	public static int power(int base, int exp) {
		int result = 1;
		for (int i=0; i<exp; i++) {
			result *= base;
		}
		return result;
	}
}