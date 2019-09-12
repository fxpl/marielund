package haparanda.utils;

/**
 * Class identifying a boundary of a domain.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017
 */
public class BoundaryId {

	/* The dimension along which the boundary is located (i.e. index of the
	   coordinate which is constant along the boundary) */
	private int dimension;
	/* If lower==true, x_i obtains its minimum on the boundary.
	 * If lower==false, x_i obtains its maximum on the boundary. */
	private boolean lower;

	/**
	 * Create a boundary id which identifies the lower boundary along dimension
	 * 0.
	 */
	public BoundaryId() {
		reset();
	}

	/**
	 * @param dimension The coordinate which is constant on this boundary
	 * @param lower If this is set to true, x_dimension obtains its minimum value on this boundary. Otherwise it obtains its maximum value.
	 */
	public BoundaryId(int dimension, boolean lower) {
		this.dimension = dimension;
		this.lower = lower;
	}

	/**
	 * @return The index of the dimension along which the boundary is located
	 */
	public final int getDimension() {
		return this.dimension;
	}

	/**
	 * @return True if the elements obtain their minimum value (along the represented dimension) on the boundary, false otherwise
	 */
	public final boolean isLowerSide() {
		return this.lower;
	}

	/**
	 * @return Id of the other boundary along the dimension of this boundary. Note that you as a caller has the responsibility to delete the id when you are done with it.
	 */
	public final BoundaryId oppositeSide() {
		return new BoundaryId(this.dimension, !this.lower);
	}

	/**
	 * Reset the boundary id: Let it point at the lower boundary along dimension
	 * 0.
	 */
	public final void reset() {
		this.dimension = 0;
		this.lower = true;
	}
	
	public final void setDimension(int newDimension) {
		this.dimension = newDimension;
	}
	
	public final void setIsLowerSide(boolean newLower) {
		this.lower = newLower;
	}

	/**
	 * "Step" the boundary id one step forward, such that it points at the next
	 * boundary. The order in which the boundaries are stepped over is (0,true),
	 * (0,false), (1,true), (1,false),...
	 */
	public final void step () {
		this.lower = !this.lower;
		if (this.lower) (this.dimension)++;
	}
}

