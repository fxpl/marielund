#include "src/utils/Math.hpp"
#include "test/HaparandaTest.hpp"

using namespace Haparanda::Math;

/**
 * Test class for Math.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2013-2014, 2016-2017
 */
class MathTest : public HaparandaTest
{
protected:

	/**
	 * Verify the behavior of power in the following cases:
	 * 5^0 = 1
	 * 5^1 = 5
	 * 5^3 = 125
	 * 2^10 = 1024
	 * 2^11 = 2048
	 */
	void testPower() {
		expect_equal((std::size_t)1, power(5, 0));
		expect_equal((std::size_t)5, power(5, 1));
		expect_equal((std::size_t)125, power(5, 3));
		expect_equal((std::size_t)1024, power(2, 10));
		expect_equal((std::size_t)2048, power(2, 11));
	}

	/**
	 * Verify the behavior of power for 64 bit integer bases.
	 */
	void testPower_base64() {
		EXPECT_TRUE(power(2200000000, 1) > 0);
		expect_equal((std::size_t)4840000000000000000, power(2200000000, 2));
		expect_equal((std::size_t)5000000000, power(5000000000, 1));
	}
};

TEST_F(MathTest, TestBasicMathFunctions) {
	testPower();
	testPower_base64();
}
