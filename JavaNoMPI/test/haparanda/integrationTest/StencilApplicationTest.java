package haparanda.integrationTest;

import static org.junit.Assert.*;
import org.junit.*;

import haparanda.grid.*;
import haparanda.numerics.*;
import haparanda.utils.*;
import mpi.MPI;
import mpi.MPIException;

/**
 * Integration test for a stencil application (and everything it uses).
 *
 * Apply an 8:th order constant multuncial FD-stencil on an two dimensional
 * block and verify that the result is close enough to the second derivative
 * of the initial function value at each point.
 *
 * @copyright Malin Kallen 2018-2019
 */
public class StencilApplicationTest 
{
	protected final static int DIMENSIONALITY = Integer.getInteger("DIM");
	private static final int ORDER_OF_ACCURACY = Integer.getInteger("OOA", 8);
	// Max error after one application
	private static final double  EXPECTED_ACCURACY1 = 0.002;
	// Max error after two applications
	private static final double  EXPECTED_ACCURACY2 = 0.15;
	// These accuracies should be enough at least for DIM<=7

	private int pointsPerBlock;		// Number of elements in each dimension
	private int elementsPerBlock;	// Total number of elements
	double[] stepLength;
	double[] smallestCoordinate = new double[DIMENSIONALITY];
	private BlockOperator stencil;
	private double[] inputValues;
	private double[] resultValues;
	private double[] expectedValues;
	private CommunicativeBlock inputBlock;
	private ComputationalBlock resultBlock;
	
	@BeforeClass
	public static void initMPI() throws MPIException {
		MPI.Init(new String[0]);
	}

	@Before
	public void setUp () throws MPIException {
		setUp(10);
	}
	
	@AfterClass
	public static void finalizeMPI() throws MPIException {
		MPI.Finalize();
	}

	/**
	 * Create a stencil and blocks that store values on which
	 * the stencil will be applied, and the result of the application
	 * respectively. Initialize the input block with
	 * sin(2*pi*x0)+sin(2*pi*x1)*...*sin(2*pi*x{d-1}
	 *
	 * The minimum global coordinate value is 0 and the maximum global
	 * coordinate value is 1-h, were h=step length. However, the minimum and
	 * maximum coordinate of each block depends on its position in the
	 * Cartesian processor grid.
	 *
	 * @param pointsPerBlock The number of grid points in each dimension of the block on which the stencil will be applied
	 * @throws MPIException 
	 */
	public void setUp(int pointsPerBlock) throws MPIException {
		this.pointsPerBlock = pointsPerBlock;
		stepLength = new double[DIMENSIONALITY];

		inputBlock = new ComputationalComposedBlock(pointsPerBlock, ORDER_OF_ACCURACY/2);
		for (int d=0; d<DIMENSIONALITY; d++) {
			int pointsPerUnit = pointsPerBlock * inputBlock.procGridSize(d);
			stepLength[d] = 1.0/pointsPerUnit;
			smallestCoordinate[d] = inputBlock.procGridCoord(d) * stepLength[d] * pointsPerBlock;
		}
		elementsPerBlock = HaparandaMath.power(pointsPerBlock, DIMENSIONALITY);
		inputValues = new double[elementsPerBlock];
		initializeInput();
		inputBlock.setValues(inputValues);
		
		resultValues = new double[elementsPerBlock];
		resultBlock = new ComputationalPureBlock(pointsPerBlock, resultValues);
		expectedValues = new double[elementsPerBlock];

		stencil = new ConstFD8Stencil(DIMENSIONALITY, stepLength);
	}

	/**
	 * Apply the stencil twice on the input block and verify that the result
	 * values equal the expected values (in a floating point number sense)
	 * after each application. Let the blocks swap value arrays between the
	 * applications.
	 * @throws MPIException 
	 */
	@Test
	public final void testStencilApplication() throws MPIException {
		// Verify the result of the stencil application
		applyStencil();
		initializeExpected2ndDer();
		checkValues(EXPECTED_ACCURACY1);
		// Swap value arrays
		double[] tmp = resultValues;
		resultValues = inputValues;
		inputValues = tmp;
		inputBlock.setValues(inputValues);
		resultBlock.setValues(resultValues);
		// Verify that the stencil can be applied repeatedly
		applyStencil();
		initializeExpected4thDer();
		checkValues(EXPECTED_ACCURACY2);
	}
	
	/**
	 * Apply the stencil on inputBlock and write the results to resultBlock.
	 * @throws MPIException 
	 */
	private void applyStencil() throws MPIException {
		inputBlock.startCommunication();
		stencil.apply(inputBlock, resultBlock);
		inputBlock.finishCommunication();
	}
	
	/**
	 * Verify that each value in resultValues differ from the corresponding
	 * value in expectedValues with at most the specified error.
	 *
	 * @param maxError Max allowed difference between the elements
	 */
	void checkValues(double maxError) {
		for(int i=0; i<elementsPerBlock; i++) {
			assertEquals(expectedValues[i], resultValues[i], maxError);
		}
	}

	/**
	 * Initialize expectedValues with the 2nd derivative of
	 * f = sin(2*pi*x0)*...*sin(2*pi*x{d-1}) where (x0, ..., x{d-1}) is the
	 * coordinate of the point of the result block that corresponds to the
	 * actual element.
	 * 
	 * The 2nd derivative of f is -4*pi*pi*(sin(2*pi*x0)+...+sin(2*pi*x{d-1}))
	 */
	private final void initializeExpected2ndDer() {
		final double PI = Math.PI;
		for (int i=0; i<elementsPerBlock; i++) {
			double expectedValue = 0;
			for (int d=0; d<DIMENSIONALITY; d++) {
				int stride = HaparandaMath.power(pointsPerBlock, d);
				int indexAlongD = (i/stride)%pointsPerBlock;
				double x_i = smallestCoordinate[d] + indexAlongD * stepLength[d];
				expectedValue -= 4*PI*PI*Math.sin(2*PI*x_i);
			}
			expectedValues[i] = expectedValue;
		}
	}
	
	/**
	 * Initialize expectedValues with the 4th derivative of
	 * f = sin(2*pi*x0)*...*sin(2*pi*x{d-1}) where (x0, ..., x{d-1}) is the
	 * coordinate of the point of the result block that corresponds to the
	 * actual element.
	 * 
	 * The 4th derivative of f is 16*pi^4*(sin(2*pi*x0)+...+sin(2*pi*x{d-1}))
	 */
	private final void initializeExpected4thDer() {
		final double PI = Math.PI;
		for (int i=0; i<elementsPerBlock; i++) {
			double expectedValue = 0;
			for (int d=0; d<DIMENSIONALITY; d++) {
				int stride = HaparandaMath.power(pointsPerBlock, d);
				int indexAlongD = (i/stride)%pointsPerBlock;
				double x_i = smallestCoordinate[d] + indexAlongD * stepLength[d];
				expectedValue += 16*Math.pow(PI, 4)*Math.sin(2*PI*x_i);
			}
			expectedValues[i] = expectedValue;
		}
	}

	/**
	 * Initialize inputBlock with the function values
	 * sin(2*pi*x0)*...*sin(2*pi*x{d-1}) where (x0, ..., x{d-1}) is the
	 * coordinate of the point of the result block that corresponds to the
	 * actual element.
	 */
	private final void initializeInput() {
		final double PI = Math.PI;
		for (int i=0; i<elementsPerBlock; i++) {
			double inputValue = 0;
			for (int d=0; d<DIMENSIONALITY; d++) {
				int stride = HaparandaMath.power(pointsPerBlock, d);
				int indexAlongD = (i/stride)%pointsPerBlock;
				double x_i = smallestCoordinate[d] + indexAlongD * stepLength[d];
				inputValue += Math.sin(2*PI*x_i);
			}
			inputValues[i] = inputValue;
		}
	}
}

