#include <complex>
#include <gtest/gtest.h>

/**
 * If the program is run in debug mode, verify that an assertion in a function
 * will fail. Otherwise, do nothing.
 *
 * @param functionCall Function call that will make the assertion fail.
 */
#ifndef NDEBUG
#define EXPECT_ASSERT_FAIL(functionCall) \
		EXPECT_DEBUG_DEATH(functionCall, ".*Assertion .* failed.");
#else
#define EXPECT_ASSERT_FAIL(functionCall)
#endif

/**
 * Super class for all unit and integration tests in HAParaNDA.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2014, 2016
 */
class HaparandaTest : public testing::Test
{
protected:
	/**
	 * Verify that two integers are equal.
	 *
	 * @param expected Expected value of the other argument
	 * @param actual  Value to compare to the expected one.
	 */
	void expect_equal(int expected, int actual) const;

	/**
	 * Verify that two size_t:s are equal.
	 *
	 * @param expected Expected value of the other argument
	 * @param actual  Value to compare to the expected one.
	 */
	void expect_equal(std::size_t expected, std::size_t actual) const;

	/**
	 * Verify that two real double-precision floating point values are equal.
	 *
	 * @param expected Expected value of the other argument
	 * @param actual  Value to compare to the expected one.
	 */
	void expect_equal(double expected, double actual) const;

	/**
	 * Verify that two complex double-precision floating point values are equal.
	 *
	 * @param expected Expected value of the other argument
	 * @param actual  Value to compare to the expected one.
	 */
	void expect_equal(std::complex<double> expected, std::complex<double> actual) const;

	/**
	 * Verify that all elements in two arrays are equal.
	 *
	 * @tparam ElementType Data type of the elements in the arrays
	 * @param expected Expected value of each element in the array
	 * @param actual Actual array values
	 * @param n Number of elements in the arrays
	 */
	template <typename ElementType>
	void expect_equal(const ElementType *expected, const ElementType *actual, std::size_t n) const;

	/**
	 * Verify that all elements in an array have the specified value.
	 *
	 * @tparam ElementType Data type of the elements in the array
	 * @param expected Expected value of all elements in the array
	 * @param actual Actual array values
	 * @param n Number of elements in the array
	 */
	template <typename ElementType>
	void expect_equal(const ElementType expected, const ElementType *actual, std::size_t n) const;

	/**
	 * Verify that the specified (real) value is in the interval [min upper).
	 *
	 * @param value Value to check
	 * @param min Smallest allowed value
	 * @param upper The value should be smaller than this
	 */
	template <typename ElementType>
	void expect_in_interval(ElementType value, ElementType min, ElementType upper) const;

	/**
	 * Verify that both the real and the imaginary part of the specified value
	 * is in the interval [min upper).
	 *
	 * @param value Value to check
	 * @param min Smallest allowed value
	 * @param upper The value should be smaller than this
	 */
	template <typename ComplexElementType>
	void expect_in_interval(std::complex<ComplexElementType> value, std::complex<ComplexElementType> min, std::complex<ComplexElementType> upper) const;

	/**
	 * Verify that the difference between two (real) numbers is not larger than
	 * the specified value.
	 *
	 * @param expected Reference value
	 * @param actual Value to check
	 * @param error Max allowed difference between the values
	 */
	template <typename ElementType>
	void expect_near(ElementType expected, ElementType actual, ElementType error) const;

	/**
	 * Verify that neither the difference between the real parts of two values
	 * nor the difference between the imaginary parts of the same values exceed
	 * the specified value.
	 *
	 * @param expected Reference value
	 * @param actual Value to check
	 * @param error Max allowed difference between the real / imag part of the values
	 */
	template <typename ComplexElementType>
	void expect_near(std::complex<ComplexElementType> expected, std::complex<ComplexElementType> actual, ComplexElementType error) const;

	/**
	 * For each value in an array: Verify that the difference between this value
	 * and the value at the same index in another array is not larger than the
	 * specified value.
	 *
	 * @param expected Reference values
	 * @param actual Values to check
	 * @param n Number of elements in the arrays
	 * @param error Max allowed difference between the values
	 */
	template <typename ElementType>
	void expect_near(const ElementType *expected, const ElementType *actual, std::size_t n, ElementType error) const;

	/**
	 * For each pair of values, i.e. the values that are stored at the same
	 * index in two arrays: Verify that neither the difference between the real
	 * parts of two values nor the difference between the imaginary parts of the
	 * same values exceed the specified value.
	 *
	 * @param expected Reference values
	 * @param actual Values to check
	 * @param n Number of elements in the arrays
	 * @param error Max allowed difference between the real / imaginary parts of the values
	 */
	template <typename ComplexElementType>
	void expect_near(const std::complex<ComplexElementType> *expected, const std::complex<ComplexElementType> *actual, std::size_t n, ComplexElementType error) const;
};


inline void HaparandaTest::expect_equal(int expected, int actual) const {
	EXPECT_EQ(expected, actual);
}

inline void HaparandaTest::expect_equal(std::size_t expected, std::size_t actual) const {
	EXPECT_EQ(expected, actual);
}

inline void HaparandaTest::expect_equal(double expected, double actual) const {
	EXPECT_DOUBLE_EQ(expected, actual);
}

inline void HaparandaTest::expect_equal(std::complex<double> expected, std::complex<double> actual) const {
	EXPECT_DOUBLE_EQ(expected.real(), actual.real());
	EXPECT_DOUBLE_EQ(expected.imag(), actual.imag());
}

template <typename ElementType>
inline void HaparandaTest::expect_equal(const ElementType *expected, const ElementType *actual, std::size_t n) const {
	for (std::size_t i=0; i<n; i++) {
		expect_equal(expected[i], actual[i]);
	}
}

template <typename ElementType>
inline void HaparandaTest::expect_equal(const ElementType expected, const ElementType *actual, std::size_t n) const {
	for (std::size_t i=0; i<n; i++) {
		EXPECT_EQ(expected, actual[i]);
	}
}

template <typename ElementType>
inline void HaparandaTest::expect_in_interval(ElementType value, ElementType min, ElementType upper) const {
	EXPECT_LE(min, value);
	EXPECT_GT(upper, value);
}

template <typename ComplexElementType>
inline void HaparandaTest::expect_in_interval(std::complex<ComplexElementType> value, std::complex<ComplexElementType> min, std::complex<ComplexElementType> upper) const {
	EXPECT_LE(min.real(), value.real());
	EXPECT_LE(min.imag(), value.imag());
	EXPECT_GT(upper.real(), value.real());
	EXPECT_GE(upper.imag(), value.imag());	// TODO
}

template <typename ElementType>
inline void HaparandaTest::expect_near(ElementType expected, ElementType actual, ElementType error) const {
	EXPECT_NEAR(expected, actual, error);
}

template <typename ComplexElementType>
inline void HaparandaTest::expect_near(std::complex<ComplexElementType> expected, std::complex<ComplexElementType> actual, ComplexElementType error) const {
	EXPECT_NEAR(expected.real(), actual.real(), error);
	EXPECT_NEAR(expected.imag(), actual.imag(), error);
}

template <typename ElementType>
inline void HaparandaTest::expect_near(const ElementType *expected, const ElementType *actual, std::size_t n, ElementType error) const {
	for (std::size_t i=0; i<n; i++) {
		expect_near(expected[i], actual[i], error);
	}
}

template <typename ComplexElementType>
inline void HaparandaTest::expect_near(const std::complex<ComplexElementType> *expected, const std::complex<ComplexElementType> *actual, std::size_t n, ComplexElementType error) const {
	for (std::size_t i=0; i<n; i++) {
		expect_near(expected[i], actual[i], error);
	}
}
