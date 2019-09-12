#ifndef STENCILAPPLICATION_HPP_
#define STENCILAPPLICATION_HPP_

#include "src/grid/ComputationalPureBlock.hpp"
#include "src/grid/ComputationalComposedBlock.hpp"
#include "src/numerics/ConstFD8Stencil.hpp"

#include <fstream>
#include <time.h>

#define DIM 2

namespace Haparanda {
	using namespace Grid;
	using namespace Numerics;

	/**
	 * Class that can be used for testing application of a constant 8:th order
	 * finite difference stencil.
	 *
	 * @tparam DIMENSIONALITY Dimensionality of the area on which the stencil is applied
	 * @author Malin Kallen
	 * @copyright Malin Kallen 2014, 2017
	 */
	template <std::size_t DIMENSIONALITY>
	class StencilApplication
	{
	public:
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
		StencilApplication(std::size_t pointsPerUnit);

		virtual ~StencilApplication();

		/**
		 * Apply the stencil on the input block the specified number of times
		 * and write the execution times to the specified file.
		 *
		 * @param nSteps Number of times the stencil will be applied
		 * @param outputFileName Path to the file to which the execution times will be written.
		 */
		void run(int nSteps, std::string& outputFileName);

	private:
		std::size_t pointsPerUnit;	// Number of points along each dimension (Domain is [0 1]^DIM.)
		std::size_t numPoints;		// Total number of points
		BlockOperator<DIMENSIONALITY, ORDER_OF_ACCURACY> *stencil;
		double *inputValues;
		double *resultValues;
		CommunicativeBlock<DIMENSIONALITY> *inputBlock;
		ComputationalBlock<DIMENSIONALITY> *resultBlock;
		Timer *setUpTimer;
		Timer *totalTimer;

		/**
		 * Apply the stencil the specified number of times. In order to simulate
		 * time stepping, the pointers to the grid functions are swapped after
		 * each application. This means that the stencil is always applied on
		 * the result of the previous stencil application.
		 *
		 * @param nSteps Number of times the stencil will be applied
		 */
		void applyStencil(int nSteps);

		/**
		 * Initialize each value in the array with a random value >=0 and <1.
		 */
		void initializeInputRandom();

		/**
		 * Print the configuration of the current application and the total
		 * execution time, setup time, time spent on computations and time spent
		 * on communication to the specified file.
		 *
		 * @param nSteps Number of times the stencil will be applied
		 * @param outputFileName Path to the file to which the execution times will be written.
		 */
		void reportResults(int nSteps, std::string& outputFileName);
	};

	template <std::size_t DIMENSIONALITY>
	StencilApplication<DIMENSIONALITY>::StencilApplication(std::size_t pointsPerUnit) {
		/* Create and start the timers */
		setUpTimer = new Timer();
		setUpTimer->start();
		totalTimer = new Timer();
		totalTimer->start();

		this->pointsPerUnit = pointsPerUnit;
		// One unit per block
		inputBlock = new ComputationalComposedBlock<DIMENSIONALITY>(pointsPerUnit, ORDER_OF_ACCURACY/2);

		/* Create and initialize the blocks. */
		std::array<double, DIMENSIONALITY> stepLength;
		stepLength.fill(1.0/pointsPerUnit);

		numPoints = Math::power(pointsPerUnit, DIMENSIONALITY);
		inputValues = new double[numPoints];
		initializeInputRandom();
		inputBlock->setValues(inputValues);

		resultValues = new double[numPoints];
		resultBlock = new ComputationalPureBlock<DIMENSIONALITY>(pointsPerUnit, resultValues);

		/* Create the stencil */
		stencil = new ConstFD8Stencil<DIMENSIONALITY>(stepLength);

		setUpTimer->stop();
		MPI::COMM_WORLD.Barrier();
		totalTimer->stop();
	}

	template <std::size_t DIMENSIONALITY>
	StencilApplication<DIMENSIONALITY>::~StencilApplication() {
		delete inputBlock;
		delete resultBlock;
		delete []inputValues;
		delete []resultValues;
		delete stencil;
		delete setUpTimer;
		delete totalTimer;
	}

	template <std::size_t DIMENSIONALITY>
	void StencilApplication<DIMENSIONALITY>::run(int nSteps, std::string& outputFileName) {
		totalTimer->start();
		applyStencil(nSteps);
		totalTimer->stop();

		reportResults(nSteps, outputFileName);
	}

	/*** Private methods ***/
	template <std::size_t DIMENSIONALITY>
	void StencilApplication<DIMENSIONALITY>::applyStencil(int nSteps) {
		for(int t=0; t<nSteps; t++) {
			std::cout << "Application " << t << ": " << time(NULL) << std::endl;
			// Apply the stencil
			inputBlock->startCommunication();
			stencil->apply(*inputBlock, resultBlock);
			inputBlock->finishCommunication();
			// Each application is done on the output from the previous one
			double *tmp = resultValues;
			resultValues = inputValues;
			inputValues = tmp;
			inputBlock->setValues(inputValues);
			resultBlock->setValues(resultValues);
		}
		std::cout << nSteps << " applications done: " << time(NULL) << std::endl;
	}

	template <std::size_t DIMENSIONALITY>
	void StencilApplication<DIMENSIONALITY>::initializeInputRandom() {
		unsigned int randState[OMP_MAX_NUM_THREADS];
		for (int i=0; i<OMP_MAX_NUM_THREADS; i++) {
			randState[i] = i+1;
		}
#pragma omp parallel for
		for (std::size_t i=0; i<numPoints; i++) {
			/* The initialization does not scale if we don't copy the array element
			 * into a local variable before calling rand_r. */
			unsigned int myState = randState[OMP_THREAD_ID];
			inputValues[i] = (double)rand_r(&myState)/RAND_MAX;
			randState[OMP_THREAD_ID] = myState;
		}
	}

	template <std::size_t DIMENSIONALITY>
	void StencilApplication<DIMENSIONALITY>::reportResults(int nSteps, std::string& outputFileName) {
		double localTotalTime = totalTimer->totalElapsedTime();
		double localSetUpTime = setUpTimer->totalElapsedTime();
		double localCompTime = stencil->computationTime();
		double localCommTime = inputBlock->communicationTime();
		double localCompCommTime = localCompTime + localCommTime;
        double globalTotalTime, globalSetUpTime, globalCompTime, globalCommTime, globalCompCommTime;
        int nProcesses = MPI::COMM_WORLD.Get_size();
		int nThreads = OMP_MAX_NUM_THREADS;
        MPI::COMM_WORLD.Reduce(&localTotalTime, &globalTotalTime, 1, MPI::DOUBLE, MPI::MAX, 0);
        MPI::COMM_WORLD.Reduce(&localSetUpTime, &globalSetUpTime, 1, MPI::DOUBLE, MPI::MAX, 0);
        MPI::COMM_WORLD.Reduce(&localCompTime, &globalCompTime, 1, MPI::DOUBLE, MPI::MAX, 0);
        MPI::COMM_WORLD.Reduce(&localCommTime, &globalCommTime, 1, MPI::DOUBLE, MPI::MAX, 0);
        MPI::COMM_WORLD.Reduce(&localCompCommTime, &globalCompCommTime, 1, MPI::DOUBLE, MPI::MAX, 0);
        if (0 == MPI::COMM_WORLD.Get_rank()) {
			std::ofstream outputFile(outputFileName, std::ofstream::app);
			outputFile << DIMENSIONALITY << "," << this->pointsPerUnit << "," << ORDER_OF_ACCURACY << "," << \
						nProcesses << "," << nThreads << "," << nSteps << "," << \
						globalTotalTime << "," << globalSetUpTime << "," \
						<< globalCompTime << "," << globalCommTime << "," << globalCompCommTime << "\n";
			outputFile.close();
        }
	}

} /* namespace Haparanda */

/**
 * Usage: stencil_application <block size in each dimension> <name of output file> <number of applications of the stencil to the area>
 *
 * Apply an 8:th order constant multuncial stencil on an NUM_DIMENSIONS
 * dimensional block whose size in each dimension is given by the first
 * argument to the program.
 *
 * Write the total time, the setup time and the time needed for computations and
 * communication respectively to the file whose name is specified by the second
 * argument to the program.
 *
 * The application is done several times. The number of times can be specified
 * as the third argument to the program. The default is 10.
 *
 * Copyright Malin Kallen 2014, 2017
 */
int main(int argc, char *args[]) {
	if (argc<3 || argc>4) {
		throw new std::runtime_error("Usage: stencil_haparanda <block size in each dimension> <name of output file> <number of applications of the stencil to the area>");
	}
	std::size_t size = atoi(args[1]);
	std::string fileName = args[2];
	int nSteps = argc > 3 ? atoi(args[3]) : 10;

	MPI::Init();
	Haparanda::StencilApplication<DIM> *application = new Haparanda::StencilApplication<DIM>(size);
	application->run(nSteps, fileName);
	delete application;
	MPI::COMM_WORLD.Barrier();
	MPI::Finalize();
	return 0;
}

#endif /* STENCILAPPLICATION_HPP_ */
