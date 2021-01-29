package haparanda.utils;

public class MagicNumber
{
	public int M;		// Magic number (unsigned int!)
	public int a;		// "add" indicator,
	public int s;		// and shift amount.

	/**
	 * This is a slightly modified version of the method presented in figure
	 * 10-2 in Hacker's Delight by Henry S. Warren (2nd edition, page 233).
	 * It is also discussed at stackoverflow:
	 * http://stackoverflow.com/questions/28003334/modulus-optimization-in-c
	 *
	 * Compute a magic number result r such that x/divisor can be written
	 * (((x * r.M) / 2^32) + x * r.a) / 2^s
	 * where / denotes integer division
	 * for any positive 32 bit integer x and divisor
	 *
	 * @param divisor Divisor for which the magic numbers described above will be computed. (Is treated like un unsigned int!)
	 */
	static public MagicNumber getMagicNumber(int divisor) {
		// Must have 1 <= d <= 2**32-1.
		int p;
		int nc, delta, q1, r1, q2, r2;	// All of these are unsigned ints!
		MagicNumber magic = new MagicNumber();
	
		magic.a = 0;					// Initialize "add" indicator.
		if (0 == divisor) {	// The unsignedness doesn't matter here.
			// Division with 0 is illegal, but some test cases bring us here
			// We use a very rough approximation :)
			magic.M = 0x7FFFFFFF;			// 2^31 - 1
			magic.s = 0;
		} else {
			nc = -1 - Integer.remainderUnsigned(-divisor, divisor);
			p = 31;						 					// Init. p.
			q1 = Integer.divideUnsigned(0x80000000, nc);	// Init. q1 = 2**p/nc.
			r1 = 0x80000000 - q1*nc;						// Init. r1 = rem(2**p, nc).
			q2 = Integer.divideUnsigned(0x7FFFFFFF, divisor);// Init. q2 = (2**p - 1)/d.
			r2 = 0x7FFFFFFF - q2*divisor;   				// Init. r2 = rem(2**p - 1, d).
			do {
				p = p + 1;
				if (Integer.compareUnsigned(r1, nc-r1) >=0) {
					q1 = 2*q1 + 1;
					r1 = 2*r1 - nc;
				} else {
					q1 = 2*q1;
					r1 = 2*r1;
				}
				if (Integer.compareUnsigned(r2+1, divisor-r2) >= 0) {
					if (Integer.compareUnsigned(q2, 0x7FFFFFFF) >= 0) {
						magic.a = 1;
					}
					q2 = 2*q2 + 1;
					r2 = 2*r2 + 1 - divisor;
				} else {
					if (Integer.compareUnsigned(q2, 0x80000000) >= 0) {
						magic.a = 1;
					}
					q2 = 2*q2;
					r2 = 2*r2 + 1;
				}
				delta = divisor - 1 - r2;
			} while (Integer.compareUnsigned(p, 64) < 0 &&
				// Unsignedness doesn't matter when looking for equality
				(Integer.compareUnsigned(q1, delta) < 0 || (q1 == delta && r1 == 0)));
	
			magic.M = q2 + 1;	// Magic number
			magic.s = p - 32;	// and shift amount to return
		}
		return magic;		// (magic.a was set above).
	}
}


