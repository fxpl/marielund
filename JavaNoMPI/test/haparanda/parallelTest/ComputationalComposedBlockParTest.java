package haparanda.parallelTest;

import mpi.MPI;
import mpi.MPIException;
import static org.junit.Assert.*;
import org.junit.*;

import haparanda.grid.*;
import haparanda.iterators.BoundaryIterator;
import haparanda.utils.*;

/**
 * Test of boundary data communication in ComputationalComposedBlock.
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2019
 */
public class ComputationalComposedBlockParTest 
{
	private final static int DIM = Integer.getInteger("DIM");
	private CommunicativeBlock block;
	int[] numProcs = new int[DIM];
	int[] procStrides = new int [DIM];
	final int elementsPerDim = 3;
	final int numElements = HaparandaMath.power(elementsPerDim, DIM);
	final int extent = 1;
	// Distance between element on boundary along <array index>
	int[] strides = new int[DIM+1];
	private double[] values;

	@BeforeClass
	public static void initMPI() throws MPIException {
		MPI.Init(new String[0]);
	}

	@Before
	public void setUp () throws MPIException {
		block = new ComputationalComposedBlock(elementsPerDim, extent);

		for (int d=0; d<DIM; d++) {
			numProcs[d] = block.procGridSize(d);
		}

		strides[0] = 1;
		procStrides[0] = 1;
		for (int d=1; d<DIM; d++) {
			strides[d] = strides[d-1] * elementsPerDim;
			procStrides[d] = procStrides[d-1] * numProcs[d-1];
		}
		strides[DIM] = strides[DIM-1] * elementsPerDim;

		int cartRank = 0;
		for (int d=0; d<DIM; d++) {
			cartRank += block.procGridCoord(d) * procStrides[d];
		}

		values = new double[numElements];
		for (int i=0; i<numElements; i++) {
			values[i] = cartRank * numElements + i;
		}
		block.setValues(values);
	}

	@After
	public void tearDown() throws MPIException {
		block.finishCommunication();
	}

	@AfterClass
	public static void finalizeMPI() throws MPIException {
		MPI.Finalize();
	}


	/**
	 * Verify that when ghostRegionInitialized returns, the ghost region on the
	 * side specified by the (output) argument is initialized with values from
	 * the boundary of the neighboring block in that direction.
	 * @throws MPIException 
	 */
	@Test
	public final void testCommunication() throws MPIException {
		block.startCommunication();

		BoundaryId boundary = new BoundaryId();
		BoundaryIterator bdIterator = block.getBoundaryIterator();

		for (int i = 0; i < 2 * DIM; i++) {
			// Wait for ghost region to be initialized
			block.receiveDoneAt(boundary);
			bdIterator.setBoundaryToIterate(boundary);

			// Search in the right direction
			int offset;
			if (boundary.isLowerSide()) {
				offset = -1;
			} else {
				offset = 1;
			}

			// Compute expected value of first ghost region element
			int[] neighborCoords = new int[DIM];
			for (int d=0; d<DIM; d++) {
				neighborCoords[d] = block.procGridCoord(d);
			}
			neighborCoords[boundary.getDimension()] =
					(neighborCoords[boundary.getDimension()]+numProcs[boundary.getDimension()]+offset) % numProcs[boundary.getDimension()];
			int neighborCartRank = 0;
			for (int d=0; d<DIM; d++) {
				neighborCartRank += neighborCoords[d] * procStrides[d];
			}
			double expectedValue = neighborCartRank * numElements;
			if (boundary.isLowerSide()) {
				expectedValue += strides[boundary.getDimension()] * (elementsPerDim - 1);
			}

			// Verify ghost region values
			while(bdIterator.isInField()) {
				double actualValue = bdIterator.currentNeighbor(boundary.getDimension(), offset);
				assertEquals(expectedValue, actualValue, 4*Math.ulp(expectedValue));
				int stride = strides[boundary.getDimension()];
				if (((int)expectedValue + 1) % stride == 0) {
					int strideNextDim = strides[boundary.getDimension()+1];
					expectedValue += strideNextDim - (stride - 1);
				} else {
					expectedValue++;
				}
				bdIterator.next();
			}

		}
		block.finishCommunication();
	}
}

