package haparanda.numerics;

import haparanda.grid.*;
import haparanda.iterators.*;
import haparanda.utils.*;

/**
 * Class representing a multuncial stencil.
 *
 * @author Malin Kallen
 */
public abstract class MultuncialStencil extends BlockOperator
{
	public MultuncialStencil() {
		super();
	}
	
	protected final static int ORDER_OF_ACCURACY = Integer.getInteger("OOA", 8);
	protected final static int EXTENT = ORDER_OF_ACCURACY/2;

	protected void applyInBoundaryRegion(ComputationalBlock input, ComputationalBlock result, final BoundaryId boundary) {
		FixedThreadPoolExecutor.execute(
				(taskId, numTasks) -> {
					return new BoundaryRegionApplication(taskId, numTasks, input, result, boundary);
				}
		);
	}
	
	private class BoundaryRegionApplication extends Task {
		final ComputationalBlock input;
		ComputationalBlock result;
		BoundaryId boundary;
		
		private BoundaryRegionApplication(int taskId, int numTasks, ComputationalBlock input, ComputationalBlock result, final BoundaryId boundary) {
			super(taskId, numTasks);
			this.input = input;
			this.result = result;
			this.boundary = boundary;
		}
		
		public Void call() {
			BoundaryIterator inputIterator = input.getBoundaryIterator(this);
			inputIterator.setBoundaryToIterate(boundary, this);
			BoundaryIterator resultIterator = result.getBoundaryIterator(this);
			resultIterator.setBoundaryToIterate(boundary, this);
			int dim = boundary.getDimension();
			int lowestWeightIndex = boundary.isLowerSide() ? 0 : EXTENT + 1;
			int dir = boundary.isLowerSide() ? 1 : -1;
			int maxDistanceFromBoundary = dir * EXTENT;
			while (inputIterator.isInField()) {
				for (int distanceFromBoundary=0; distanceFromBoundary!=maxDistanceFromBoundary; distanceFromBoundary+=dir) {
					/* Apply left part of stencil if being on the lower boundary
					   and the right part of the stencil if being on the upper one. */
					double resultValue = resultIterator.currentNeighbor(dim, distanceFromBoundary);
					for (int i=0; i<EXTENT; i++) {
						int weightIndex = lowestWeightIndex + i;
						int offset = lowestWeightIndex - EXTENT + distanceFromBoundary + i;
						resultValue += getWeight(inputIterator, dim, weightIndex) * inputIterator.currentNeighbor(dim, offset);
					}
					resultIterator.setCurrentNeighbor(dim, distanceFromBoundary, resultValue);
				}
				inputIterator.next();
				resultIterator.next();
			}
			assert(!resultIterator.isInField());
			return null;
		}
	}

	protected void applyInInnerRegion(final ComputationalBlock input, ComputationalBlock result) {
		FixedThreadPoolExecutor.execute(
				(taskId, numTasks) -> {
					return new InnerRegionApplication(taskId, numTasks, input, result);
				}
		);
	}
	
	private class InnerRegionApplication extends Task {
		final ComputationalBlock input;
		ComputationalBlock result;
		
		private InnerRegionApplication(int taskId, int numTasks, final ComputationalBlock input, ComputationalBlock result) {
			super(taskId, numTasks);
			this.input = input;
			this.result = result;
		}
		
		public Void call() {
			int sizePerDim = input.getElementsPerDim();
			FieldIterator inputIterator = input.getInnerIterator(this);
			FieldIterator resultIterator = result.getInnerIterator(this);
			while(inputIterator.isInField()) {
				// Apply the stencil in each dimension
				double resultValue = 0;
				for (int d=0; d < DIMENSIONALITY; d++) {
					int indexAlongD = inputIterator.currentIndex(d);
					// Left part of stencil
					if (indexAlongD >= EXTENT) {
						for (int i=0; i<EXTENT; i++) {
							resultValue += getWeight(inputIterator, d, i) * inputIterator.currentNeighbor(d, -EXTENT+i);
						}
					}
					// Center weight
					resultValue += getWeight(inputIterator, d, EXTENT) * inputIterator.currentValue();
					// Right part of stencil
					if (indexAlongD+EXTENT < sizePerDim) {
						for (int i=1; i<=EXTENT; i++) {
							resultValue += getWeight(inputIterator, d, EXTENT+i) * inputIterator.currentNeighbor(d, i);
						}
					}
				}
				resultIterator.setCurrentValue(resultValue);
				inputIterator.next();
				resultIterator.next();
			}
			assert(!resultIterator.isInField());
			return null;
		}
	}

	/**
	 * Get the stencil weight at the specified index along the specified
	 * dimension for the point currently pointed at by the provided
	 * iterator.
	 *
	 * @param iterator Iterator of the current point
	 * @param dim Dimension for which the weight will be fetched
	 * @param weightIndex Index of the weight (in the specified dimension)
	 */
	protected abstract double getWeight(final FieldIterator iterator, int dim, int weightIndex);
}
