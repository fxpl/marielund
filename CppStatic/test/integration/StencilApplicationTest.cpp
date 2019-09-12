#ifndef STENCILAPPLICATION_HPP_
#define STENCILAPPLICATION_HPP_

#include "src/grid/ComputationalPureBlock.hpp"
#include "src/grid/ComputationalComposedBlock.hpp"
#include "src/numerics/ConstFD8Stencil.hpp"
#include "test/HaparandaTest.hpp"

#include <cmath>
#include <fstream>
#include <time.h>

#define PI 3.14159265358979323846
#define DIM 3
// Max error after one application
#define EXPECTED_ACCURACY1 0.002
// Max error after two applications
#define EXPECTED_ACCURACY2 0.15
// These accuracies should be enough at least for DIM<=7

using namespace Haparanda::Grid;
using namespace Haparanda::Numerics;

/**
 * Integration test for a stencil application (and everything it uses).
 *
 * @author Malin Kallen
 * @copyright Malin Kallen 2014, 2017-2019
 */
class StencilApplicationTest : public HaparandaTest
{
public:
	virtual void SetUp () {
		SetUp(10);
	}

	/**
	 * Create a stencil and blocks that store values on which
	 * the stencil will be applied, and the result of the application
	 * respectively. Initialize the input block with
	 * @f$sin(2*pi*x0) + ... + sin(2*pi*x{d-1})@f$
	 *
	 * The minimum global coordinate value is 0 and the maximum global
	 * coordinate value is 1-h, were h=step length. However, the minimum and
	 * maximum coordinate of each block depends on its position in the
	 * Cartesian processor grid.
	 *
	 * @param pointsPerBlock The number of grid points in each dimension of the block on which the stencil will be applied
	 */
	virtual void SetUp(std::size_t pointsPerBlock) {
		this->pointsPerBlock = pointsPerBlock;

		inputBlock = new InputBlockType(pointsPerBlock, ORDER_OF_ACCURACY/2);
		for (std::size_t d=0; d<DIM; d++) {
			std::size_t pointsPerUnit = pointsPerBlock * inputBlock->procGridSize(d);
			stepLength[d] = 1.0/pointsPerUnit;
			smallestCoordinate[d] = inputBlock->procGridCoord(d) * stepLength[d] * pointsPerBlock;
		}

		elementsPerBlock = Haparanda::Math::power(pointsPerBlock, DIM);
		inputValues = new double[elementsPerBlock];
		initializeInput();
		inputBlock->setValues(inputValues);

		resultValues = new double[elementsPerBlock];
		resultBlock = new OutputBlockType(pointsPerBlock, resultValues);
		expectedValues = new double[elementsPerBlock];

		stencil = new OperatorType(stepLength);
	}

	virtual void TearDown() {
		delete inputBlock;
		delete resultBlock;
		delete []inputValues;
		delete []resultValues;
		delete []expectedValues;
		delete stencil;
	}


protected:
	/**
	 * Apply the stencil twice on the input block and verify that the result
	 * values equal the expected values (in a floating point number sense)
	 * after each application. Let the blocks swap value arrays between the
	 * applications.
	 */
	void testStencilApplication() {
		// Verify the result of the stencil application
		applyStencil();
		initializeExpected2ndDer();
		checkValues(EXPECTED_ACCURACY1);
		// Swap value arrays
		double *tmp = resultValues;
		resultValues = inputValues;
		inputValues = tmp;
		inputBlock->setValues(inputValues);
		resultBlock->setValues(resultValues);
		// Verify that the stencil can be applied repeatedly
		applyStencil();
		initializeExpected4thDer();
		checkValues(EXPECTED_ACCURACY2);
	}

private:
	typedef ConstFD8Stencil<DIM> OperatorType;
	typedef ComputationalComposedBlock<DIM> InputBlockType;
	typedef ComputationalPureBlock<DIM> OutputBlockType;
	typedef typename InputBlockType::InnerIteratorType InputInnerIteratorType;
	typedef typename InputBlockType::BoundaryIteratorType InputBoundaryIteratorType;
	typedef typename OutputBlockType::InnerIteratorType OutputInnerIteratorType;
	typedef typename OutputBlockType::BoundaryIteratorType OutputBoundaryIteratorType;

	std::size_t pointsPerBlock;	// Number of elements in each dimension
	std::size_t elementsPerBlock;	// Total number of elements
	std::array<double, DIM> stepLength;
	double smallestCoordinate[DIM];
	BlockOperator<OperatorType, ORDER_OF_ACCURACY> *stencil;
	double *inputValues;
	double *resultValues;
	double *expectedValues;
	CommunicativeBlock<InputInnerIteratorType, InputBoundaryIteratorType> *inputBlock;
	ComputationalBlock<OutputInnerIteratorType, OutputBoundaryIteratorType> *resultBlock;

	/**
	 * Apply the stencil on inputBlock and write the results to resultBlock.
	 */
	void applyStencil() {
		inputBlock->startCommunication();
		stencil->apply(*inputBlock, resultBlock);
		inputBlock->finishCommunication();
	}

	/**
	 * Verify that each value in resultValues differ from the corresponding
	 * value in expectedValues with at most the specified error.
	 *
	 * @param maxError Max allowed difference between the elements
	 */
	void checkValues(double maxError) {
		for(std::size_t i=0; i<elementsPerBlock; i++) {
			expect_near(expectedValues[i], resultValues[i], maxError);
		}
	}

	/**
	 * Initialize expectedValues with the 2nd derivative of
	 * @f$f = sin(2*pi*x0)*...*sin(2*pi*x{d-1})@f$ where
	 * @f$(x0, ..., x{d-1})@f$ is the coordinate of the point of the result
	 * block that corresponds to the actual element.
	 *
	 * The 2nd derivative of f is @f$-4*pi*pi*(sin(2*pi*x0)+...+sin(2*pi*x{d-1}))@f$
	 */
	void initializeExpected2ndDer() {
		for (std::size_t i=0; i<elementsPerBlock; i++) {
			double expectedValue = 0;
			for (std::size_t d=0; d<DIM; d++) {
				std::size_t stride = Haparanda::Math::power(pointsPerBlock, d);
				std::size_t indexAlongD = (i/stride)%pointsPerBlock;
				double x_i = smallestCoordinate[d] + indexAlongD * stepLength[d];
				expectedValue -= 4*PI*PI*sin(2*PI*x_i);
			}
			expectedValues[i] = expectedValue;
		}
	}

	/**
	 * Initialize expectedValues with the 4th derivative of
	 * @f$f = sin(2*pi*x0)*...*sin(2*pi*x{d-1})@f$ where
	 * @f$(x0, ..., x{d-1})@f$ is the coordinate of the point of the result
	 * block that corresponds to the actual element.
	 *
	 * The 2nd derivative of f is @f$16*pi^4*(sin(2*pi*x0)+...+sin(2*pi*x{d-1}))@f$
	 */
	void initializeExpected4thDer() {
		for (std::size_t i=0; i<elementsPerBlock; i++) {
			double expectedValue = 0;
			for (std::size_t d=0; d<DIM; d++) {
				std::size_t stride = Haparanda::Math::power(pointsPerBlock, d);
				std::size_t indexAlongD = (i/stride)%pointsPerBlock;
				double x_i = smallestCoordinate[d] + indexAlongD * stepLength[d];
				expectedValue += 16*pow(PI, 4)*sin(2*PI*x_i);
			}
			expectedValues[i] = expectedValue;
		}
	}

	/**
	 * Initialize inputBlock with the function values
	 * @f$sin(2*pi*x0)*...*sin(2*pi*x{d-1})@f$ where @f$(x0, ..., x{d-1})@f$ is
	 * the coordinate of the point of the result block that corresponds to the
	 * actual element.
	 */
	void initializeInput() {
		for (std::size_t i=0; i<elementsPerBlock; i++) {
			double inputValue = 0;
			for (std::size_t d=0; d<DIM; d++) {
				std::size_t stride = Haparanda::Math::power(pointsPerBlock, d);
				std::size_t indexAlongD = (i/stride)%pointsPerBlock;
				double x_i = smallestCoordinate[d] + indexAlongD * stepLength[d];
				inputValue += sin(2*PI*x_i);
			}
			inputValues[i] = inputValue;
		}
	}
};


/**
 * Apply an 8:th order constant multuncial FD-stencil on an two dimensional
 * block and verify that the result is close enough to the second derivative
 * of the initial function value at each point.
 *
 * Copyright Malin Kallen 2014, 2017-2018
 */
TEST_F(StencilApplicationTest, TestApplyStencil) {
	testStencilApplication();
}

#endif /* STENCILAPPLICATION_HPP_ */
