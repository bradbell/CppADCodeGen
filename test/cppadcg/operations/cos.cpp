/* --------------------------------------------------------------------------
 *  CppADCodeGen: C++ Algorithmic Differentiation with Source Code Generation:
 *    Copyright (C) 2012 Ciengis
 *
 *  CppADCodeGen is distributed under multiple licenses:
 *
 *   - Eclipse Public License Version 1.0 (EPL1), and
 *   - GNU General Public License Version 3 (GPL3).
 *
 *  EPL1 terms and conditions can be found in the file "epl-v10.txt", while
 *  terms and conditions for the GPL3 can be found in the file "gpl3.txt".
 * ----------------------------------------------------------------------------
 * Author: Joao Leal
 */
#include "CppADCGOperationTest.hpp"
#include "cos.hpp"

using namespace CppAD;

TEST_F(CppADCGOperationTest, cos) {
    // independent variable vector
    std::vector<double> u(1);
    std::vector<std::vector<double> > uV;
    u[0] = 1.0;
    uV.push_back(u);
    u[0] = 0.0;
    uV.push_back(u);

    test0nJac("cos", &CosFunc<double >, &CosFunc<CG<double> >, uV);
}