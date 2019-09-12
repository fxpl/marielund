//=======================================================================
//
//  HAParaNDA: High-dimensional Adaptive Parallel Numerical Dynamics
//  Copyright (2011) Magnus Gustafsson, Uppsala University
//			(2014) Malin Kallen, Uppsala University
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//  Contact: Magnus Gustafsson <magnus.gustafsson@it.uu.se>
//
//=======================================================================

#ifndef CONSTFD8STENCIL_HPP_
#define CONSTFD8STENCIL_HPP_

#include "MultuncialStencil.hpp"

namespace Haparanda {
namespace Numerics {

#define ORDER_OF_ACCURACY 8
	/**
	 * Class representing a 8:th order FD stencil approximating the Laplacian.
	 *
	 * @tparam DIMENSIONALITY Dimensionality of the stencil
	 * @author Malin Kallen, Magnus Grandin
	 */
	template<std::size_t DIMENSIONALITY>
	class ConstFD8Stencil: public MultuncialStencil<ConstFD8Stencil<DIMENSIONALITY>, ORDER_OF_ACCURACY>
	{
	public:
		/**
		 * Create a stencil that approximates the Laplacian.
		 *
		 * @param stepLength Step lengths of the block on which the stencil will be applied
		 */
		ConstFD8Stencil(const std::array<double, DIMENSIONALITY>& stepLength);

		virtual ~ConstFD8Stencil();

	protected:
		template<typename IteratorType>
		double getWeight(const Iterators::FieldIterator<IteratorType>& iterator, std::size_t dim, int weightIndex) const;

	private:
		double weights[DIMENSIONALITY][ORDER_OF_ACCURACY+1];

		/**
		 * Initialize the weights of the stencil
		 *
		 * @param weightGenerator Generator for the weights of the stencil
		 * @param stepLength Step lengths (in each dimension) of the block on which the stencil will be applied
		 */
		void initializeWeights(const std::array<double, DIMENSIONALITY>& stepLength);

		friend class MultuncialStencil<ConstFD8Stencil<DIMENSIONALITY>, ORDER_OF_ACCURACY>;
	};

	template<std::size_t DIMENSIONALITY>
	ConstFD8Stencil<DIMENSIONALITY>::ConstFD8Stencil(const std::array<double, DIMENSIONALITY>& stepLength) {
		initializeWeights(stepLength);
	}

	template<std::size_t DIMENSIONALITY>
	ConstFD8Stencil<DIMENSIONALITY>::~ConstFD8Stencil() {
	}


	/*** Protected methods ***/
	template<std::size_t DIMENSIONALITY>
	template<typename IteratorType>
	inline double ConstFD8Stencil<DIMENSIONALITY>::getWeight(const Iterators::FieldIterator<IteratorType>& iterator, std::size_t dim, int weightIndex) const {
		return weights[dim][weightIndex];
	}


	/*** Private methods ***/
	template<std::size_t DIMENSIONALITY>
	void ConstFD8Stencil<DIMENSIONALITY>::initializeWeights(const std::array<double, DIMENSIONALITY>& stepLength) {
		for (unsigned short i=0; i<DIMENSIONALITY; i++) {
			double hSquared = stepLength[i]*stepLength[i];
			weights[i][0] = -1.0/(560.0*hSquared);
			weights[i][1] = 8.0/(315.0*hSquared);
			weights[i][2] = -1.0/(5.0*hSquared);
			weights[i][3] = 8.0/(5.0*hSquared);
			weights[i][4] = -205.0/(72.0*hSquared);
			weights[i][5] = weights[i][3];
			weights[i][6] = weights[i][2];
			weights[i][7] = weights[i][1];
			weights[i][8] = weights[i][0];
		}
	}

} // namespace Numerics;
} // namespace Haparanda

#endif /* CONSTFD8STENCIL_HPP_ */
