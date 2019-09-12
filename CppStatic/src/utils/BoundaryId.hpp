#ifndef BOUNDARYID_HPP_
#define BOUNDARYID_HPP_

#include <cstdlib>

namespace Haparanda {
namespace Utils {

	/**
	 * Class identifying a boundary of a domain.
	 *
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2014, 2016
	 */
	class BoundaryId {

	public:
		/**
		 * Create a boundary id which identifies the lower boundary along dimension
		 * 0.
		 */
		BoundaryId();

		/**
		 * @param dimension The coordinate which is constant on this boundary
		 * @param lower If this is set to true, x_dimension obtains its minimum value on this boundary. Otherwise it obtains its maximum value.
		 */
		BoundaryId(std::size_t dimension, bool lower);

		/**
		 * @return The index of the dimension along which the boundary is located
		 */
		std::size_t getDimension() const;

		/**
		 * @return True if the elements obtain their minimum value (along the represented dimension) on the boundary, false otherwise
		 */
		bool isLowerSide() const;

		/**
		 * @return Id of the other boundary along the dimension of this boundary. Note that you as a caller has the responsibility to delete the id when you are done with it.
		 */
		BoundaryId *oppositeSide() const;

		/**
		 * Reset the boundary id: Let it point at the lower boundary along dimension
		 * 0.
		 */
		void reset();

		void setDimension(std::size_t newDimension);

		void setIsLowerSide(bool newLower);

		/**
		 * "Step" the boundary id one step forward, such that it points at the next
		 * boundary. The order in which the boundaries are stepped over is (0,true),
		 * (0,false), (1,true), (1,false),...
		 */
		inline void operator++();

	private:
		/* The dimension along which the boundary is located (i.e. index of the
		   coordinate which is constant along the boundary) */
		std::size_t dimension;
		/* If lower==true, x_i obtains its minimum on the boundary.
		 * If lower==false, x_i obtains its maximum on the boundary. */
		bool lower;
	};

	BoundaryId::BoundaryId()  {
		reset();
	}

	BoundaryId::BoundaryId(std::size_t dimension, bool lower) {
		this->dimension = dimension;
		this->lower = lower;
	}

	std::size_t BoundaryId::getDimension() const {
		return this->dimension;
	}

	bool BoundaryId::isLowerSide() const {
		return this->lower;
	}

	BoundaryId *BoundaryId::oppositeSide() const {
		return new BoundaryId(this->dimension, !this->lower);
	}

	void BoundaryId::reset()  {
		this->dimension = 0;
		this->lower = true;
	}

	void BoundaryId::setDimension(std::size_t newDimension) {
		this->dimension = newDimension;
	}

	void BoundaryId::setIsLowerSide(bool newLower) {
		this->lower = newLower;
	}

	inline void BoundaryId::operator ++()  {
		this->lower = !this->lower;
		if (this->lower) (this->dimension)++;
	}

} /* namespace Utils */
} /* namespace Haparanda */

#endif /* BOUNDARYID_HPP_ */
