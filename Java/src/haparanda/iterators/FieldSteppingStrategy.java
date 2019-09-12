package haparanda.iterators;

import haparanda.utils.*;

/**
 * Strategy that defines how an iterator is stepped through a field (or tensor)
 * of the specified order.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2017
 */
abstract class FieldSteppingStrategy
{
	protected final static int ORDER = Integer.getInteger("DIM");
	
	int minIndex, maxIndex, index;
	protected int order;
	protected int[] stride;
	protected MagicNumber[] magicStrideNumbers;
	protected int[] size;
	protected MagicNumber[] magicSizeNumbers;
	protected int totalSize;
	
	/**
	 * Initialize the size array using the specified sizes and set index to
	 * 0.
	 *
	 * @param sizes Size of the field which is to be iterated through, in each dimension
	 */
	public FieldSteppingStrategy(final int[] sizes) {
		size = new int[ORDER];
		magicSizeNumbers = new MagicNumber[ORDER];
		stride = new int[ORDER+1];
		magicStrideNumbers = new MagicNumber[ORDER+1];
		totalSize = 1;
		stride[0] = 1;
		magicStrideNumbers[0] = MagicNumber.getMagicNumber(stride[0]);
		for (int i=0; i<ORDER; i++) {
			size[i] = sizes[i];
			totalSize *= this.size[i];
			magicSizeNumbers[i] = MagicNumber.getMagicNumber(size[i]);
			stride[i+1] = stride[i] * size[i];
			magicStrideNumbers[i+1] = MagicNumber.getMagicNumber(stride[i+1]);
		}
	}

	/**
	 * @param dimension Specifies which index should be retrieved, see description of the return value
	 * @return The <code>dimension</code>:th coordinate of the index of the element currently pointed at by the iterator
	 */
	final protected int currentIndex(int dimension) {
		assert(isInField());
		long strideM = magicStrideNumbers[dimension].M & 0x00000000FFFFFFFFL;
		long sizeM = magicSizeNumbers[dimension].M & 0x00000000FFFFFFFFL;
		// indexAlongDimension = index/stride[dimension]
		long indexAlongDimension =
				(((index * strideM) >>> 32) +
				index * magicStrideNumbers[dimension].a) >>>
				magicStrideNumbers[dimension].s;
		// n = indexAlongDimension / size[dimension]
		long n = (((indexAlongDimension * sizeM) >>> 32) +
				indexAlongDimension * magicSizeNumbers[dimension].a) >>>
				magicSizeNumbers[dimension].s;
		// return indexAlongDimension % size[dimension]
		return (int)(indexAlongDimension - size[dimension] * n);
	}

	/**
	 * Restart the iterator: Set it to point at its first element.
	 */
	final protected void first() {
		index = minIndex;
	}

	/**
	 * @return false if the iterator points outside the field to be iterated, true otherwise
	 */
	final protected boolean isInField() {
		return index>=minIndex && index<=maxIndex;
	}

	/**
	 * Find the internal index of the element located at a specified
	 * distance from the one currently pointed at by the iterator. The
	 * internal index is the index that is used internally (by the
	 * ValueType strategy) to identify an element and should never be known
	 * by the caller of the iterator!
	 *
	 * @param dimension Dimension in which the distance from the current element to the one in question is > 0
	 * @param offset Distance from the current element to the one in question. A positive value means that the element is located ''above'' (i.e. has higher index than) the current one while a negative sign means that the element is located ''below'' (i.e. has lower index than) the current one.
	 * @return Internal index of the element in question
	 */
	final protected int linearNeighborIndex(int dimension, int offset) {
		assert(neighborInField(dimension, offset));
		return index + offset * stride[dimension];
	}

	/**
	 * Check if the element on a specified distance from the one currently
	 * pointed at by the iterator is inside the field iterated over.
	 *
	 * @param dimension Dimension in which the distance from the current element to the investigated one is > 0
	 * @param offset Distance from the current element to the one investigated. A positive value means that the element is located ''above'' (i.e. has higher index than) the current one while a negative sign means that the element is located ''below'' (i.e. has lower index than) the current one.
	 * @return true if the element in question is inside the field, false otherwise
	 */
	final protected boolean neighborInField(int dimension, int offset) {
		final int neighborIndexInDimension = offset + currentIndex(dimension);
		return neighborIndexInDimension >= 0
				&& neighborIndexInDimension < size[dimension];
	}

	/**
	 * Advance the iterator one step.
	 */
	protected abstract void next();
}


