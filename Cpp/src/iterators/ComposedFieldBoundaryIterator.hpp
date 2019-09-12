#ifndef COMPOSEDFIELDBOUNDARYITERATOR_HPP_
#define COMPOSEDFIELDBOUNDARYITERATOR_HPP_

#include "BoundaryIterator.hpp"
#include "ComposedFieldIterator.hpp"
#include "ValueFieldBoundaryIterator.hpp"

namespace Haparanda {
namespace Iterators {

	/**
	 * A class representing an iterator over the boundary of a composed field.
	 *
	 * @tparam ORDER Order/dimensionality of the field
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2014, 2016, 2017
	 */
	template <std::size_t ORDER>
	class ComposedFieldBoundaryIterator : public ComposedFieldIterator<ORDER>, public BoundaryIterator<ORDER>
	{
	public:
		/**
		 * Note that the side iterators are deleted by the destructor!
		 *
		 * @param sizes Size of the main region of the field which is to be iterated through, in each dimension
		 * @param data The values stored in the main region
		 * @param sideIterators Array of side iterators
		 */
		ComposedFieldBoundaryIterator(const std::array<std::size_t, ORDER>& sizes, double *data, FieldIterator<ORDER> ***sideIterators);

		virtual ~ComposedFieldBoundaryIterator();

		virtual void first();

		virtual void next();

		virtual void setBoundaryToIterate(const BoundaryId& boundary);

	protected:
		virtual void createMainIterator(const std::array<std::size_t, ORDER>& sizes, double *data);

		virtual FieldIterator<ORDER> *currentSideIterator() const;
	};

	template <std::size_t ORDER>
	ComposedFieldBoundaryIterator<ORDER>::ComposedFieldBoundaryIterator(
			const std::array<std::size_t, ORDER>& sizes, double *data, FieldIterator<ORDER> ***sideIterators) {
		this->initialize(sizes, data, sideIterators);
	}

	template <std::size_t ORDER>
	ComposedFieldBoundaryIterator<ORDER>::~ComposedFieldBoundaryIterator() {
	}

	template <std::size_t ORDER>
	inline void ComposedFieldBoundaryIterator<ORDER>::first() {
		this->mainIterator->first();
		currentSideIterator()->first();
	}

	template <std::size_t ORDER>
	inline void ComposedFieldBoundaryIterator<ORDER>::next() {
		this->mainIterator->next();
		currentSideIterator()->next();
	}

	template <std::size_t ORDER>
	void ComposedFieldBoundaryIterator<ORDER>::setBoundaryToIterate(const BoundaryId& boundary) {
		this->currentBoundary = boundary;
		dynamic_cast<BoundaryIterator<ORDER>&>(*this->mainIterator).setBoundaryToIterate(this->currentBoundary);
		BoundaryId *oppositeBoundary = this->currentBoundary.oppositeSide();
		dynamic_cast<BoundaryIterator<ORDER>&>(*currentSideIterator()).setBoundaryToIterate(*oppositeBoundary);
		delete oppositeBoundary;
		this->first();
	}


	/*** Protected methods ***/
	template <std::size_t ORDER>
	void ComposedFieldBoundaryIterator<ORDER>::createMainIterator(const std::array<std::size_t, ORDER>& sizes, double *data) {
		this->mainIterator = new ValueFieldBoundaryIterator<ORDER>(sizes, data);
	}

	template <std::size_t ORDER>
	inline FieldIterator<ORDER> *ComposedFieldBoundaryIterator<ORDER>::currentSideIterator() const {
		std::size_t currentDimension = this->currentBoundary.getDimension();
		std::size_t currentSide = this->currentBoundary.isLowerSide() ? 0 : 1;
		return this->sideIterators[currentDimension][currentSide];
	}


} /* namespace Iterators */
} /* namespace Haparanda */

#endif /* COMPOSEDFIELDBOUNDARYITERATOR_HPP_ */
