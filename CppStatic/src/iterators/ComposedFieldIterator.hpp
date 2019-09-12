#ifndef COMPOSEDFIELDITERATOR_HPP_
#define COMPOSEDFIELDITERATOR_HPP_

#include "FieldIterator.hpp"

#include <stdexcept>
#include <sstream>
#include <typeinfo>

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
	 * @tparam IteratorType Type of concrete sub class implementing this class (see CRTP)
	 * @tparam MainIteratorType Concrete type of the iterator for the main region
	 * @tparam SideIteratorType Concrete type of the iterators for the side regions
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2014, 2016, 2017
	 */
	template<class IteratorType, class MainIteratorType, class SideIteratorType> class ComposedFieldIterator;

	/**
	 * This specialization is needed as ORDER cannot be retrieved from the sub
	 * class as it is not created yet when this class is created.
	 *
	 * @tparam ORDER Order/dimensionality of the field iterated over
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2014, 2016, 2017
	 */
	template <template<std::size_t>class IteratorType, template<std::size_t>class MainIteratorType, template<std::size_t>class SideIteratorType, std::size_t ORDER>
	class ComposedFieldIterator<IteratorType<ORDER>, MainIteratorType<ORDER>, SideIteratorType<ORDER>>
	: public FieldIterator<IteratorType<ORDER>>
	{
	public:
		virtual ~ComposedFieldIterator();

		std::size_t currentIndex(std::size_t dimension) const;

		double currentNeighbor(std::size_t dimension, int offset) const;

		double currentValue() const;

		void first();

		bool isInField() const;

		void setBoundaryToIterate(const BoundaryId& boundary);

		void setCurrentNeighbor(std::size_t dimension, int offset, double newValue);

		void setCurrentValue(double newValue);

		std::size_t size(std::size_t dimension) const;

	protected:
		typedef FieldIterator<MainIteratorType<ORDER>> MainIterator;
		typedef FieldIterator<SideIteratorType<ORDER>> SideIterator;
		MainIterator *mainIterator;
		SideIterator ***sideIterators;

		/**
		 * @return A reference to the iterator of the side region outside the boundary which is currently iterated over
		 */
		SideIterator *currentSideIterator() const;

		/**
		 * Create the iterators for the main region and the side regions
		 * respectively.
		 *
		 * @param sizes Size of the main region, in each dimension
		 * @param data Pointer to the value of the first element in the main region
		 * @param sideRegions Array containing the side iterators
		 */
		void initialize(const std::array<std::size_t, ORDER>& sizes, double *data, SideIterator ***sideIterators);

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
		void initializeSideIterators(SideIterator ***sideIterators);
	};

	template <template<std::size_t>class IteratorType, template<std::size_t>class MainIteratorType, template<std::size_t>class SideIteratorType, std::size_t ORDER>
	ComposedFieldIterator<IteratorType<ORDER>, MainIteratorType<ORDER>, SideIteratorType<ORDER>>::~ComposedFieldIterator() {
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


	template <template<std::size_t>class IteratorType, template<std::size_t>class MainIteratorType, template<std::size_t>class SideIteratorType, std::size_t ORDER>
	inline std::size_t ComposedFieldIterator<IteratorType<ORDER>, MainIteratorType<ORDER>, SideIteratorType<ORDER>>::currentIndex(std::size_t dimension) const {
		return mainIterator->currentIndex(dimension);
	}

	template <template<std::size_t>class IteratorType, template<std::size_t>class MainIteratorType, template<std::size_t>class SideIteratorType, std::size_t ORDER>
	inline double ComposedFieldIterator<IteratorType<ORDER>, MainIteratorType<ORDER>, SideIteratorType<ORDER>>::currentNeighbor(std::size_t dimension, int offset) const {
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

	template <template<std::size_t>class IteratorType, template<std::size_t>class MainIteratorType, template<std::size_t>class SideIteratorType, std::size_t ORDER>
	inline double ComposedFieldIterator<IteratorType<ORDER>, MainIteratorType<ORDER>, SideIteratorType<ORDER>>::currentValue() const {
		return mainIterator->currentValue();
	}

	template <template<std::size_t>class IteratorType, template<std::size_t>class MainIteratorType, template<std::size_t>class SideIteratorType, std::size_t ORDER>
	inline void ComposedFieldIterator<IteratorType<ORDER>, MainIteratorType<ORDER>, SideIteratorType<ORDER>>::first() {
		mainIterator->first();
		for (std::size_t i=0; i<ORDER; i++) {
			for (std::size_t j=0; j<2; j++) {
				sideIterators[i][j]->first();
			}
		}
	}

	template <template<std::size_t>class IteratorType, template<std::size_t>class MainIteratorType, template<std::size_t>class SideIteratorType, std::size_t ORDER>
	inline bool ComposedFieldIterator<IteratorType<ORDER>, MainIteratorType<ORDER>, SideIteratorType<ORDER>>::isInField() const {
		return mainIterator->isInField();
	}

	template <template<std::size_t>class IteratorType, template<std::size_t>class MainIteratorType, template<std::size_t>class SideIteratorType, std::size_t ORDER>
	inline void ComposedFieldIterator<IteratorType<ORDER>, MainIteratorType<ORDER>, SideIteratorType<ORDER>>::setBoundaryToIterate(const BoundaryId& boundary) {
		std::stringstream errorMsg;
		errorMsg << "setBoundaryToIterate cannot be called for " << typeid(*this).name() << ".";
		throw std::runtime_error(errorMsg.str());
	}

	template <template<std::size_t>class IteratorType, template<std::size_t>class MainIteratorType, template<std::size_t>class SideIteratorType, std::size_t ORDER>
	inline void ComposedFieldIterator<IteratorType<ORDER>, MainIteratorType<ORDER>, SideIteratorType<ORDER>>::setCurrentNeighbor(std::size_t dimension, int offset, double newValue) {
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

	template <template<std::size_t>class IteratorType, template<std::size_t>class MainIteratorType, template<std::size_t>class SideIteratorType, std::size_t ORDER>
	void ComposedFieldIterator<IteratorType<ORDER>, MainIteratorType<ORDER>, SideIteratorType<ORDER>>::setCurrentValue(double newValue) {
		mainIterator->setCurrentValue(newValue);
	}

	template <template<std::size_t>class IteratorType, template<std::size_t>class MainIteratorType, template<std::size_t>class SideIteratorType, std::size_t ORDER>
	inline std::size_t ComposedFieldIterator<IteratorType<ORDER>, MainIteratorType<ORDER>, SideIteratorType<ORDER>>::size(std::size_t dimension) const {
		return mainIterator->size(dimension)
				+ sideIterators[dimension][0]->size(dimension)
				+ sideIterators[dimension][1]->size(dimension);
	}


	/*** Protected methods ***/
	template <template<std::size_t>class IteratorType, template<std::size_t>class MainIteratorType, template<std::size_t>class SideIteratorType, std::size_t ORDER>
	typename ComposedFieldIterator<IteratorType<ORDER>, MainIteratorType<ORDER>, SideIteratorType<ORDER>>::SideIterator *
		ComposedFieldIterator<IteratorType<ORDER>, MainIteratorType<ORDER>, SideIteratorType<ORDER>>::currentSideIterator() const {
		return static_cast<IteratorType<ORDER>&>(*this).currentSideIterator();
	}

	template <template<std::size_t>class IteratorType, template<std::size_t>class MainIteratorType, template<std::size_t>class SideIteratorType, std::size_t ORDER>
	inline void ComposedFieldIterator<IteratorType<ORDER>, MainIteratorType<ORDER>, SideIteratorType<ORDER>>::initialize(const std::array<std::size_t, ORDER>& sizes, double *data, SideIterator ***sideIterators) {
		this->mainIterator = new MainIteratorType<ORDER>(sizes, data);
		initializeSideIterators(sideIterators);
	}

	template <template<std::size_t>class IteratorType, template<std::size_t>class MainIteratorType, template<std::size_t>class SideIteratorType, std::size_t ORDER>
	inline void ComposedFieldIterator<IteratorType<ORDER>, MainIteratorType<ORDER>, SideIteratorType<ORDER>>::initializeSideIterators(SideIterator ***sideIterators) {
		this->sideIterators = sideIterators;
	}


} /* namespace Iterators */
} /* namespace Haparanda */

#endif /* COMPOSEDFIELDITERATOR_HPP_ */
