#ifndef COMPOSEDFIELDITERATOR_HPP_
#define COMPOSEDFIELDITERATOR_HPP_

#include "FieldIterator.hpp"

namespace Haparanda {
namespace Iterators {

	/**
	 * A class representing an iterator over a composed field. The field
	 * consists of a main region with the actual values of the field and one
	 * side region along each boundary. When a neighbor which is located outside
	 * the main region is requested, it is fetched from a side region, if
	 * possible.
	 *
	 * @tparam ORDER Order/dimensionality of the field
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2014, 2016, 2017
	 */
	template <std::size_t ORDER>
	class ComposedFieldIterator : virtual public FieldIterator<ORDER>
	{
	public:
		virtual ~ComposedFieldIterator();

		virtual std::size_t currentIndex(std::size_t dimension) const;

		virtual double currentNeighbor(std::size_t dimension, int offset) const;

		virtual double currentValue() const;

		virtual void first();

		virtual bool isInField() const;

		virtual void setCurrentValue(double newValue);

		virtual void setCurrentNeighbor(std::size_t dimension, int offset, double newValue);

		virtual std::size_t size(std::size_t dimension) const;

	protected:
		FieldIterator<ORDER> *mainIterator;
		FieldIterator<ORDER> ***sideIterators;

		/**
		 * Initialize the iterator for the main region.
		 *
		 * @param sizes Size of the main region, in each dimension
		 * @param data Pointer to the value of the first element in the field
		 */
		virtual void createMainIterator(const std::array<std::size_t, ORDER>& sizes, double *data) = 0;

		/**
		 * Initialize the iterators for the side regions.
		 * The array containing the side iterators is structured the same way as
		 * the argument array, i.e. it is an array of length ORDER, where each
		 * element in turn is an array of length 2. Element [i][0] in this array
		 * contains the side region ''below'' the main region along dimension i,
		 * while element [i][1] contains the side region ''above'' the main
		 * region along the same dimension.
		 *
		 * @sideRegions Array of side iterators. The expected structure of this array is described above.
		 */
		void initializeSideIterators(FieldIterator<ORDER> ***sideIterators);

		/**
		 * @return A reference to the iterator of the side region outside the boundary which is currently iterated over
		 */
		virtual FieldIterator<ORDER> *currentSideIterator() const = 0;

		/**
		 * Create the iterators for the main region and the side regions
		 * respectively.
		 *
		 * @param sizes Size of the main region, in each dimension
		 * @param data Pointer to the value of the first element in the main region
		 * @param sideRegions Array containing the side iterators
		 */
		void initialize(const std::array<std::size_t, ORDER>& sizes, double *data, FieldIterator<ORDER> ***sideIterators);
	};

	template <std::size_t ORDER>
	ComposedFieldIterator<ORDER>::~ComposedFieldIterator() {
		delete mainIterator;
		for (std::size_t i=0; i<ORDER; i++) {
			for(std::size_t j=0; j<2; j++) {
				if (NULL != sideIterators[i][j]) {
					delete sideIterators[i][j];
				}
			}
			delete []sideIterators[i];
		}
		delete []sideIterators;
	}


	template <std::size_t ORDER>
	inline std::size_t ComposedFieldIterator<ORDER>::currentIndex(std::size_t dimension) const {
		return mainIterator->currentIndex(dimension);
	}

	template <std::size_t ORDER>
	inline double ComposedFieldIterator<ORDER>::currentNeighbor(std::size_t dimension, int offset) const {
		const long neighborIndexInDimension = offset + this->currentIndex(dimension);
		if(neighborIndexInDimension < 0) {
			// The neighbor is ''below'' the main region
			int offsetInSideRegion = neighborIndexInDimension + 1;
			return sideIterators[dimension][0]->currentNeighbor(dimension, offsetInSideRegion);
		} else if (neighborIndexInDimension >= static_cast<long>(mainIterator->size(dimension))) {
			// The neighbor is ''above'' the main region
			int offsetInSideRegion = neighborIndexInDimension-mainIterator->size(dimension);
			return sideIterators[dimension][1]->currentNeighbor(dimension, offsetInSideRegion);
		} else {
			return mainIterator->currentNeighbor(dimension, offset);
		}
	}

	template <std::size_t ORDER>
	inline double ComposedFieldIterator<ORDER>::currentValue() const {
		return mainIterator->currentValue();
	}

	template <std::size_t ORDER>
	inline void ComposedFieldIterator<ORDER>::first() {
		mainIterator->first();
		for (std::size_t i=0; i<ORDER; i++) {
			for (std::size_t j=0; j<2; j++) {
				sideIterators[i][j]->first();
			}
		}
	}

	template <std::size_t ORDER>
	inline bool ComposedFieldIterator<ORDER>::isInField() const {
		return mainIterator->isInField();
	}

	template<std::size_t ORDER>
	inline void ComposedFieldIterator<ORDER>::setCurrentNeighbor(std::size_t dimension, int offset, double newValue) {
		const long neighborIndexInDimension = offset + this->currentIndex(dimension);
		if(neighborIndexInDimension < 0) {
			// The neighbor is ''below'' the main region
			int offsetInSideRegion = neighborIndexInDimension + 1;
			sideIterators[dimension][0]->setCurrentNeighbor(dimension, offsetInSideRegion, newValue);
		} else if (neighborIndexInDimension >= static_cast<long>(mainIterator->size(dimension))) {
			// The neighbor is ''above'' the main region
			int offsetInSideRegion = neighborIndexInDimension-mainIterator->size(dimension);
			sideIterators[dimension][1]->setCurrentNeighbor(dimension, offsetInSideRegion, newValue);
		} else {
			mainIterator->setCurrentNeighbor(dimension, offset, newValue);
		}
	}

	template<std::size_t ORDER>
	void ComposedFieldIterator<ORDER>::setCurrentValue(double newValue) {
		mainIterator->setCurrentValue(newValue);
	}

	template<std::size_t ORDER>
	inline std::size_t ComposedFieldIterator<ORDER>::size(std::size_t dimension) const {
		return mainIterator->size(dimension)
				+ sideIterators[dimension][0]->size(dimension)
				+ sideIterators[dimension][1]->size(dimension);
	}


	/*** Protected methods ***/
	template <std::size_t ORDER>
	inline void ComposedFieldIterator<ORDER>::initializeSideIterators(FieldIterator<ORDER> ***sideIterators) {
		this->sideIterators = sideIterators;
	}

	template <std::size_t ORDER>
	inline void ComposedFieldIterator<ORDER>::initialize(const std::array<std::size_t, ORDER>& sizes, double *data, FieldIterator<ORDER> ***sideIterators) {
		createMainIterator(sizes, data);
		initializeSideIterators(sideIterators);
	}

} /* namespace Iterators */
} /* namespace Haparanda */

#endif /* COMPOSEDFIELDITERATOR_HPP_ */
