#ifndef COMPOSEDFIELDBOUNDARYITERATOR_HPP_
#define COMPOSEDFIELDBOUNDARYITERATOR_HPP_

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
	class ComposedFieldBoundaryIterator
			: public ComposedFieldIterator<ComposedFieldBoundaryIterator<ORDER>, ValueFieldBoundaryIterator<ORDER>, ValueFieldBoundaryIterator<ORDER>>
	{
	public:
		typedef ValueFieldBoundaryIterator<ORDER> MainIteratorType;
		typedef ValueFieldBoundaryIterator<ORDER> SideIteratorType;

		/**
		 * Note that the side iterators are deleted by the destructor!
		 *
		 * @param sizes Size of the main region of the field which is to be iterated through, in each dimension
		 * @param data The values stored in the main region
		 * @param sideIterators Array of side iterators
		 */
		ComposedFieldBoundaryIterator(const std::array<std::size_t, ORDER>& sizes, double *data, FieldIterator<SideIteratorType> ***sideIterators);

		virtual ~ComposedFieldBoundaryIterator();

		void first();

		void next();

		void setBoundaryToIterate(const BoundaryId& boundary);

	protected:
		FieldIterator<SideIteratorType> *currentSideIterator() const;

	private:
		BoundaryId currentBoundary;
	};

	template <std::size_t ORDER>
	ComposedFieldBoundaryIterator<ORDER>::ComposedFieldBoundaryIterator(
			const std::array<std::size_t, ORDER>& sizes, double *data, FieldIterator<SideIteratorType> ***sideIterators) {
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
		this->mainIterator->setBoundaryToIterate(this->currentBoundary);
		BoundaryId *oppositeBoundary = this->currentBoundary.oppositeSide();
		currentSideIterator()->setBoundaryToIterate(*oppositeBoundary);
		delete oppositeBoundary;
		this->first();
	}

	/*** Protected methods ***/
	template <std::size_t ORDER>
	inline FieldIterator<typename ComposedFieldBoundaryIterator<ORDER>::SideIteratorType>
	*ComposedFieldBoundaryIterator<ORDER>::currentSideIterator() const {
		std::size_t currentDimension = this->currentBoundary.getDimension();
		std::size_t currentSide = this->currentBoundary.isLowerSide() ? 0 : 1;
		return this->sideIterators[currentDimension][currentSide];
	}


} /* namespace Iterators */
} /* namespace Haparanda */

#endif /* COMPOSEDFIELDBOUNDARYITERATOR_HPP_ */
