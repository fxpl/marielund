package haparanda.performanceTest;

import java.io.*;
import java.util.Arrays;
import java.util.concurrent.ThreadLocalRandom;

import haparanda.grid.*;
import haparanda.numerics.*;
import haparanda.utils.*;

/**
 * Class that can be used for testing application of a constant 8:th order
 * finite difference stencil.
 *
 * @author Malin Kallen 2017-2019, 2021
 */
public class StencilApplication
{
	private static final int DIMENSIONALITY = Integer.getInteger("DIM");
	private static final int ORDER_OF_ACCURACY = Integer.getInteger("OOA", 8);
	
	private int pointsPerUnit;	// Number of points along each dimension (Domain is [0 1]^dimensionality.)
	private BlockOperator stencil;
	private double[] inputValues;
	private double[] resultValues;
	private ComputationalComposedBlock inputBlock;
	private ComputationalBlock resultBlock;
	private Timer setUpTimer;
	private Timer totalTimer;
	
	/**
	 * Usage: stencil_application <block size in each dimension> <name of output file> <number of applications of the stencil to the area> <number of warm up applications>
	 * 
	 * The dimensionality (n) is given by the system constant DIM.
	 *
	 * Apply an 8:th order constant multuncial stencil on an n-dimensional
	 * block whose size in each dimension is given by the first argument to the
	 * program.
	 *
	 * Write the total time, the setup time and the time needed for computations and
	 * communication respectively to the file whose name is specified by the third
	 * argument to the program.
	 *
	 * Before the actual applications, perform w warm up applications of the
	 * stencil, where w is given by the third argument of the program.
	 *
	 * The application is done several times. The number of times can be specified
	 * as the fourth argument to the program. The default is 10.
	 *
	 * Copyright Malin Kallen 2014, 2017-2019
	 */
	public static void main(String[] args) {
		if (4 != args.length) {
			System.out.println("Usage: stencil_haparanda <block size in each dimension> <name of output file> <number of applications of the stencil to the area> <number of warm up applications>");
		}
		int size = Integer.parseInt(args[0]);
		String fileName = args[1];
		int nSteps = Integer.parseInt(args[2]);
		int nWarmUp = Integer.parseInt(args[3]);
		
		final StencilApplication application = new StencilApplication(size);
		application.run(nSteps, nWarmUp, fileName);
		FixedThreadPoolExecutor.destroy();
	}

	/**
	 * Create a stencil and blocks that store values on which
	 * the stencil will be applied, and the result of the application
	 * respectively. Initialize the input block with random values.
	 *
	 * The minimum coordinate value of the grid functions is 0 and the
	 * maximum coordinate value is 1.
	 *
	 * @param pointsPerUnit The number of grid points in each dimension of the block on which the stencil will be applied
	 */
	public StencilApplication(int pointsPerUnit) {
		/* Create and start the timers */
		setUpTimer = new Timer();
		setUpTimer.start(false);
		totalTimer = new Timer();
		totalTimer.start(false);
	
		this.pointsPerUnit = pointsPerUnit;
		// One unit per block
		inputBlock = new ComputationalComposedBlock(pointsPerUnit, ORDER_OF_ACCURACY/2);
	
		/* Create and initialize the blocks. */
		int numElements = HaparandaMath.power(pointsPerUnit, DIMENSIONALITY);
		inputValues = new double[numElements];
		initializeInputRandom();
		inputBlock.setValues(inputValues);
		
		resultValues = new double[numElements];
		resultBlock = new ComputationalPureBlock(pointsPerUnit, resultValues);
	
		/* Create the stencil */
		double[] stepLength = new double[DIMENSIONALITY];
		Arrays.fill(stepLength, 1.0/pointsPerUnit);
		stencil = new ConstFD8Stencil(DIMENSIONALITY, stepLength);
	
		setUpTimer.stop();
		totalTimer.stop();
	}

	/**
	 * Apply the stencil on the input block the specified number of times
	 * and write the execution times to the specified file. Before the actual
	 * stencil applications, perform the specified number of warm up applications.
	 *
	 * @param nSteps Number of times the stencil will be applied
	 * @param outputFileName Path to the file to which the execution times will be written.
	 * @param nWarmUp Number of warm up applications
	 */
	public final void run(int nSteps, int nWarmUp, String outputFileName) {
		totalTimer.start(false);
		applyStencil(nSteps, nWarmUp);
		totalTimer.stop();
	
		reportResults(nSteps, nWarmUp, outputFileName);
	}

	/**
	 * Apply the stencil the specified number of times. In order to simulate
	 * time stepping, the pointers to the grid functions are swapped after
	 * each application. This means that the stencil is always applied on
	 * the result of the previous stencil application.
	 *
	 * Before the actual applications of the stencil, perform the specified number
	 * of warm up applications on the input values.
	 *
	 * @param nSteps Number of times the stencil will be applied
	 * @param nWarmUp Number of warm up applications
	 */
	private final void applyStencil(int nSteps, int nWarmUp) {
		warmUp(nWarmUp);
		for(int t=0; t<nSteps; t++) {
			System.out.println("Application " + t + ": " + (System.currentTimeMillis() / 1000L));
			inputBlock.initializeSideRegions();
			// Apply the stencil
			stencil.apply(inputBlock, resultBlock);
			// Each application is done on the output from the previous one
			double[] tmp = resultValues;
			resultValues = inputValues;
			inputValues = tmp;
			inputBlock.setValues(inputValues);
			resultBlock.setValues(resultValues);
		}
		System.out.println(nSteps + " applications done: " + (System.currentTimeMillis() / 1000L));
	}
	
	/**
	 * Initialize each value in the array with a random value >=0 and <1.
	 *
	 * @param array Array to initialize
	 * @param length Length of the array
	 */
	private void initializeInputRandom() {
		FixedThreadPoolExecutor.execute(
				(taskId, numTasks) -> {
					return new RandInitialization(taskId, numTasks);
				}
		);		
	}
	
	private class RandInitialization extends Task {
		private RandInitialization(int taskId, int numTasks) {
			super(taskId, numTasks);
		}
		
		public Void call() {
			int chunkSize = inputValues.length / numTasks;
			int offset = inputValues.length % numTasks;
			int minIndex, maxIndex;
			if (id < offset) {
				chunkSize++;
				minIndex = id * chunkSize;
			} else {
				minIndex = id * chunkSize + offset;
			}
			maxIndex = minIndex + chunkSize - 1;
			
			for (int i=minIndex; i<maxIndex; i++) {
				inputValues[i] = ThreadLocalRandom.current().nextDouble();
			}
			return null;
		}
	}

	/**
	 * Print the configuration of the current application and the total
	 * execution time, setup time, time spent on computations and time spent
	 * on communication to the specified file.
	 *
	 * @param nSteps Number of times the stencil was applied
	 * @param nWarmUp Number of warm up applications done
	 * @param outputFileName Path to the file to which the execution times will be written.
	 * @throws FileNotFoundException 
	 */
	private final void reportResults(int nSteps, int nWarmUp, String outputFileName) {
		double totalTime = totalTimer.totalElapsedTime(false);
		double setUpTime = setUpTimer.totalElapsedTime(false);
		double compTime = stencil.computationTime();
		double commTime = 0;
		double compCommTime = compTime + commTime;
		
		int nProcesses = 1;
		int nThreads = FixedThreadPoolExecutor.getThreadPoolExecutor().getMaximumPoolSize();
	
		try {
			java.io.File outputFile = new java.io.File(outputFileName);
			final Writer writer = new FileWriter(outputFile, true);
			writer.write(DIMENSIONALITY + "," + this.pointsPerUnit + "," + ORDER_OF_ACCURACY + ",");
			writer.write(nProcesses + "," + nThreads + "," + nSteps + "," + nWarmUp + ",");
			writer.write(totalTime + "," + setUpTime + ",");
			writer.write(compTime + "," + commTime + "," + compCommTime + "\n");
			writer.close();
		} catch (IOException e) {
			System.out.println("Error when writing to " + outputFileName + ":" + e.getMessage());
		}
	}

	/**
	 * Apply the stencil in the input values the specified number of times, in
	 * order to warm up the VM.
	 * @param n Number of warm up applications
	 */
	private final void warmUp(int n) {
		setUpTimer.start(false);
		// Create warm up objects
		// Separate input block is needed since it records communication time
		ComputationalComposedBlock warmUpInput = new ComputationalComposedBlock(pointsPerUnit, ORDER_OF_ACCURACY/2);
		warmUpInput.setValues(inputValues);
		// Separate stencil is needed since it records computation time
		double[] stepLength = new double[DIMENSIONALITY];
		Arrays.fill(stepLength, 1.0);
		BlockOperator warmUpStencil = new ConstFD8Stencil(DIMENSIONALITY, stepLength);
		// Apply warm up stencil
		for (int i=0; i<n; i++) {
			System.out.println("Warm up application " + i + ": " + (System.currentTimeMillis() / 1000L));
			warmUpInput.initializeSideRegions();
			warmUpStencil.apply(warmUpInput, resultBlock);
		}
		setUpTimer.stop();
		System.out.println(n + " warm up applications done: " + (System.currentTimeMillis() / 1000L));
	}
}
