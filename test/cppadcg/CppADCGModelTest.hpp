#ifndef CPPAD_CG_TEST_CPPADCGMODELTEST_INCLUDED
#define CPPAD_CG_TEST_CPPADCGMODELTEST_INCLUDED
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
#include "CppADCGTest.hpp"

namespace CppAD {

    class CppADCGModelTest : public CppADCGTest {
    public:
        typedef CG<double> CGD;
        typedef AD<CGD> ADCG;
    public:

        inline CppADCGModelTest(bool verbose = false, bool printValues = false) :
            CppADCGTest(verbose, printValues) {
        }

        /**
         * Compares the results from Hessian, Jacobian, sparse Hessian and 
         * sparse Jacobian.
         * 
         * @param model
         * @param fun
         * @param x independent vector values
         * @param epsilonR relative error
         * @param epsilonA absolute error
         */
        void testModelResults(GenericModel<Base>& model,
                              ADFun<CGD>& fun,
                              const std::vector<Base>& x,
                              double epsilonR = 1e-14,
                              double epsilonA = 1e-14) {
            // dimensions
            ASSERT_EQ(model.Domain(), fun.Domain());
            ASSERT_EQ(model.Range(), fun.Range());

            /**
             */
            std::vector<CGD> x2(x.size());
            for (size_t i = 0; i < x.size(); i++) {
                x2[i] = x[i];
            }

            // forward zero
            std::vector<CGD> dep = fun.Forward(0, x2);
            std::vector<Base> depCGen = model.ForwardZero(x);
            ASSERT_TRUE(compareValues(depCGen, dep, epsilonR, epsilonA));

            // Jacobian
            std::vector<CGD> jac = fun.Jacobian(x2);
            depCGen = model.Jacobian(x);
            ASSERT_TRUE(compareValues(depCGen, jac, epsilonR, epsilonA));

            // Hessian
            std::vector<CGD> w2(fun.Range(), 1.0);
            std::vector<Base> w(fun.Range(), 1.0);

            std::vector<CGD> hess = fun.Hessian(x2, w2);
            depCGen = model.Hessian(x, w);
            ASSERT_TRUE(compareValues(depCGen, hess, epsilonR, epsilonA));

            // sparse Jacobian
            std::vector<Base> jacCGen;
            std::vector<size_t> row, col;
            model.SparseJacobian(x, jacCGen, row, col);
            std::vector<Base> jacCGenDense(jac.size());
            for (size_t i = 0; i < jacCGen.size(); i++) {
                jacCGenDense[row[i] * x.size() + col[i]] = jacCGen[i];
            }

            ASSERT_TRUE(compareValues(jacCGenDense, jac, epsilonR, epsilonA));

            // sparse Hessian
            std::vector<Base> hessCGen;
            model.SparseHessian(x, w, hessCGen, row, col);
            std::vector<Base> hessCGenDense(hess.size());
            for (size_t i = 0; i < hessCGen.size(); i++) {
                hessCGenDense[row[i] * x.size() + col[i]] = hessCGen[i];
            }

            ASSERT_TRUE(compareValues(hessCGenDense, hess, epsilonR, epsilonA));
        }

        inline ::testing::AssertionResult compareValues(const std::vector<double>& depCGen,
                                                        const std::vector<CppAD::CG<double> >& dep,
                                                        double epsilonR = 1e-14, double epsilonA = 1e-14) {

            std::vector<double> depd(dep.size());

            for (size_t i = 0; i < depd.size(); i++) {
                depd[i] = dep[i].getValue();
            }

            return CppADCGTest::compareValues(depCGen, depd, epsilonR, epsilonA);
        }

    };

}
#endif