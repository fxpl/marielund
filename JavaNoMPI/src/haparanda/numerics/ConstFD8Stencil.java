package haparanda.numerics;

import haparanda.iterators.*;

/**
 * Class representing a 8:th order FD stencil approximating the Laplacian.
 *
 * @author Malin Kallen, Magnus Grandin
 */
public class ConstFD8Stencil extends MultuncialStencil
{
	double[][] weights = new double[DIMENSIONALITY][ORDER_OF_ACCURACY+1];

	/**
	 * Create a stencil that approximates the Laplacian.
	 *
	 * @param dim dimensionality of the operator
	 * @param stepLength Step lengths of the block on which the stencil will be applied
	 */
	public ConstFD8Stencil(int dim, final double[] stepLength) {
		super();
		assert ORDER_OF_ACCURACY == 8: "Order of accuracy (OOA) must be 8 when ConstFD8Stencil is used!";
		initializeWeights(stepLength);
	}

	protected double getWeight(final FieldIterator iterator, int dim, int weightIndex) {
		return weights[dim][weightIndex];
	}

	/**
	 * Initialize the weights of the stencil
	 *
	 * @param weightGenerator Generator for the weights of the stencil
	 * @param stepLength Step lengths (in each dimension) of the block on which the stencil will be applied
	 */
	private final void initializeWeights(final double[] stepLength) {
		for (short i=0; i<DIMENSIONALITY; i++) {
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
}
