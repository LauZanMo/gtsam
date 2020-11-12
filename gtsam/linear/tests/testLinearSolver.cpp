/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010, Georgia Tech Research Corporation,
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file    testLinearSolver.cpp
 * @brief   Tests for Common Interface for Linear Solvers
 * @author  Fan Jiang
 */

#include <gtsam/linear/LinearSolver.h>
#include <gtsam/linear/GaussianFactorGraph.h>
#include <gtsam/nonlinear/LevenbergMarquardtParams.h>
#include <gtsam/base/TestableAssertions.h>
#include <CppUnitLite/TestHarness.h>

using namespace gtsam;

/* ************************************************************************* */
/// Factor graph with 2 2D factors on 3 2D variables
static GaussianFactorGraph createSimpleGaussianFactorGraph() {
  GaussianFactorGraph fg;
  Key x1 = 2, x2 = 0, l1 = 1;
  SharedDiagonal unit2 = noiseModel::Unit::Create(2);
  // linearized prior on x1: c[_x1_]+x1=0 i.e. x1=-c[_x1_]
  fg += JacobianFactor(x1, 10 * I_2x2, -1.0 * Vector::Ones(2), unit2);
  // odometry between x1 and x2: x2-x1=[0.2;-0.1]
  fg += JacobianFactor(x2, 10 * I_2x2, x1, -10 * I_2x2, Vector2(2.0, -1.0), unit2);
  // measurement between x1 and l1: l1-x1=[0.0;0.2]
  fg += JacobianFactor(l1, 5 * I_2x2, x1, -5 * I_2x2, Vector2(0.0, 1.0), unit2);
  // measurement between x2 and l1: l1-x2=[-0.2;0.3]
  fg += JacobianFactor(x2, -5 * I_2x2, l1, 5 * I_2x2, Vector2(-1.0, 1.5), unit2);
  return fg;
}

/* ************************************************************************* */
TEST(EigenOptimizer, optimizeEigenQR) {
  GaussianFactorGraph gfg = createSimpleGaussianFactorGraph();

  VectorValues expected;
  expected.insert(2, Vector2(-0.1, -0.1));
  expected.insert(0, Vector2(0.1, -0.2));
  expected.insert(1, Vector2(-0.1, 0.1));

  LinearSolverParams params;
  params.linearSolverType = LinearSolverParams::EIGEN_QR;
  params.ordering = Ordering::Colamd(gfg);

  auto solver = LinearSolver::fromLinearSolverParams(params);
  VectorValues actual = solver->solve(gfg);
  EXPECT(assert_equal(expected, actual));
}

/* ************************************************************************* */
TEST(EigenOptimizer, optimizeEigenCholesky) {
  GaussianFactorGraph gfg = createSimpleGaussianFactorGraph();

  VectorValues expected;
  expected.insert(2, Vector2(-0.1, -0.1));
  expected.insert(0, Vector2(0.1, -0.2));
  expected.insert(1, Vector2(-0.1, 0.1));

  LinearSolverParams params;
  params.linearSolverType = LinearSolverParams::EIGEN_CHOLESKY;
  params.ordering = Ordering::Colamd(gfg);

  auto solver = LinearSolver::fromLinearSolverParams(params);
  VectorValues actual = solver->solve(gfg);
  EXPECT(assert_equal(expected, actual));
}

/* ************************************************************************* */
int main() {
  TestResult tr;
  return TestRegistry::runAllTests(tr);
}
/* ************************************************************************* */