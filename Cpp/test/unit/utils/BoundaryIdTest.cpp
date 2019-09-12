#include "src/utils/BoundaryId.hpp"
#include "test/HaparandaTest.hpp"

using namespace Haparanda::Utils;

/**
 * Unit test for BoundaryId.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2014, 2016
 */
class BoundaryIdTest : public HaparandaTest
{
public:
	virtual void SetUp() {
		defaultId = new BoundaryId;
		upper13 = new BoundaryId(13, false);
	}

	virtual void TearDown() {
		delete defaultId;
		delete upper13;
	}

protected:
	/**
	 * Verify that the default constructor creates a BoundaryId object that
	 * represents the lower boundary in dimension 0.
	 */
	void testDefaultConstructor() {
		expect_equal((std::size_t)0, defaultId->getDimension());
		EXPECT_TRUE(defaultId->isLowerSide());
	}

	/**
	 * Verify that oppositeSide returns a new BoundaryId that represents the
	 * boundary along the current dimension, but on the other side of the
	 * region.
	 */
	void testOppositeSide() {
		BoundaryId *oppositeDefault = defaultId->oppositeSide();
		expect_equal((std::size_t)0, oppositeDefault->getDimension());
		EXPECT_FALSE(oppositeDefault->isLowerSide());
		delete oppositeDefault;

		BoundaryId *oppositeUpper13 = upper13->oppositeSide();
		expect_equal((std::size_t)13, oppositeUpper13->getDimension());
		EXPECT_TRUE(oppositeUpper13->isLowerSide());
		delete oppositeUpper13;
	}

	/**
	 * Verify that the constructor that takes arguments creates a BoundaryId
	 * object as specified by the arguments.
	 */
	void testParametrizedConstructor() {
		expect_equal((std::size_t)13, upper13->getDimension());
		EXPECT_FALSE(upper13->isLowerSide());
	}

	/**
	 * Verify that the ++ operator advances to the next boundary (i.e. the upper
	 * along the current dimension if currently located at the lower, and the
	 * lower in the next dimension otherwise).
	 */
	void testPlusPlus() {
		// We start at the lowermost boundary.
		expect_equal((std::size_t)0, defaultId->getDimension());
		EXPECT_TRUE(defaultId->isLowerSide());

		// Step to the upper boundary.
		++(*defaultId);
		expect_equal((std::size_t)0, defaultId->getDimension());
		EXPECT_FALSE(defaultId->isLowerSide());

		// Step to the lower boundary along the next dimension.
		++(*defaultId);
		expect_equal((std::size_t)1, defaultId->getDimension());
		EXPECT_TRUE(defaultId->isLowerSide());
	}

	/**
	 * Verify that the reset method makes a BoundaryId object represent the
	 * lower boundary in dimension 0.
	 */
	void testReset() {
		upper13->reset();
		expect_equal((std::size_t)0, upper13->getDimension());
		EXPECT_TRUE(upper13->isLowerSide());

		// The state of default object does not change.
		defaultId->reset();
		expect_equal((std::size_t)0, defaultId->getDimension());
		EXPECT_TRUE(defaultId->isLowerSide());
	}


	BoundaryId *defaultId;
	BoundaryId *upper13;
};


/**
 * Verify the behavior of the constructors in BoundaryId.
 */
TEST_F(BoundaryIdTest, TestConstructors) {
	testDefaultConstructor();
	testParametrizedConstructor();
}

/**
 * Verify the behavior of reset.
 */
TEST_F(BoundaryIdTest, TestReset) {
	testReset();
}

/**
 * Verify the behavior of the ++ operator.
 */
TEST_F(BoundaryIdTest, TestPlusPlus) {
	testPlusPlus();
}

/**
 * Verify the behavior of oppositeSide.
 */
TEST_F(BoundaryIdTest, TestOppositeSide) {
	testOppositeSide();
}
