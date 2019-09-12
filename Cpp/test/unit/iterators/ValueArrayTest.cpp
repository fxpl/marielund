#include "src/iterators/ValueArray.hpp"
#include "test/HaparandaTest.hpp"

#include <complex>

namespace Haparanda {
namespace Iterators {

	/**
	 * Unit test for the value fetching strategy ValueArray.
	 *
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2013-2014, 2016
	 */
	class ValueArrayTest : public HaparandaTest
	{
	public:
		virtual void SetUp() {
			std::size_t sizes[3] = {3, 4, 5};
			totalSize = sizes[0] * sizes[1] * sizes[2];

			values = new double[totalSize];
			for (std::size_t i=0; i<totalSize; i++) {
				values[i] = initDoubleValue(i);
			}
			strategy = new ValueArray(values);
		}

		virtual void TearDown() {
			delete strategy;
			delete []values;
		}

	protected:
		/**
		 * For each index in the field: Verify that getValue in the strategy
		 * objects return the corresponding value of the field.
		 */
		void testGetValue() {
			for (std::size_t i=0; i<totalSize; i++) {
				expect_equal(values[i], strategy->getValue(i));
			}
		}

		/**
		 * For each index in the field: Verify that setValue in the strategy
		 * objects changes the corresponding value of the field.
		 */
		void testSetValue() {
			// Set new values
			for (std::size_t i=0; i<totalSize; i++) {
				strategy->setValue(i, otherDoubleValue(i));
			}

			// Verify that the values were changed
			for (std::size_t i=0; i<totalSize; i++) {
				expect_equal(otherDoubleValue(i), values[i]);
			}
		}

	private:
		std::size_t totalSize;
		double *values;
		ValueArray *strategy;

		/**
		 * @return A standard double precision floating point value, which is intended to be assigned to the double array at set up
		 */
		double initDoubleValue(std::size_t i) {
			return 1.2 * i;
		}

		/**
		 * @return A standard double precision floating point value, which is *not* intended to be assigned to the double array at set up
		 */
		double otherDoubleValue(std::size_t i) {
			return 1.3*i;
		}
	};

	/**
	 * Verify the behavior of the getter (getValue) in ValueArrays for
	 * non-cubic fields of order 3.
	 */
	TEST_F(ValueArrayTest, TestGet) {
		testGetValue();
	}

	/**
	 * Verify the behavior of the setter (setValue) in ValueArrays
	 * for a non-cubic fields of order 3.
	 */
	TEST_F(ValueArrayTest, TestSet) {
		testSetValue();
	}
}
}
