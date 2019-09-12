#ifndef MATH_HPP_
#define MATH_HPP_

#include <complex>

namespace Haparanda {
namespace Math {

	/**
	 * Calculate base^exp for an unsigned 64 bit integer base and an unsigned
	 * integer exponent.
	 *
	 * @param base Base of the expression
	 * @param exp Exponent of the expression.
	 * @return base^exp
	 */
	std::size_t power(std::size_t base, unsigned int exp) {
		std::size_t result = 1;
		for (unsigned int i=0; i<exp; i++) {
			result *= base;
		}
		return result;
	}

} // namespace Math
} // namespace Haparanda

#endif /* MATH_HPP_ */
