/* --------------------------------------------------------------------------
CppAD: C++ Algorithmic Differentiation: Copyright (C) 2011 Ciengis

CppAD is distributed under multiple licenses. This distribution is under
the terms of the 
                    Common Public License Version 1.0.

A copy of this license is included in the COPYING file of this distribution.
Please visit http://www.coin-or.org/CppAD/ for information on other licenses.
-------------------------------------------------------------------------- */

#include <cppad_codegen/cppad_codegen.hpp>

#include "gcc_load_dynamic.hpp"

namespace { // Begin empty namespace

    bool CondExp_pvvv(void) {
        using namespace CppAD;
        using namespace std;

        bool ok = true;

        // independent variable vector
        std::vector<double> u(3);
        u[0] = 0.;
        u[1] = 1.;
        u[2] = 2.;

        CPPAD_TEST_VECTOR< AD<double> > X(3);
        X[0] = u[0];
        X[1] = u[1];
        X[2] = u[2];
        Independent(X);

        // parameter value
        AD<double> one = 1.;

        // dependent variable vector 
        CPPAD_TEST_VECTOR< AD<double> > Y(5);

        // CondExp(parameter, variable, variable, variable)
        Y[0] = CondExpLt(one, X[0], X[1], X[2]);
        Y[1] = CondExpLe(one, X[0], X[1], X[2]);
        Y[2] = CondExpEq(one, X[0], X[1], X[2]);
        Y[3] = CondExpGe(one, X[0], X[1], X[2]);
        Y[4] = CondExpGt(one, X[0], X[1], X[2]);

        // create f: X -> Y 
        ADFunCodeGen<double> f(X, Y);

        ok &= test0nJac("CondExp_pvvv", f, u, Y);

        return ok;
    }

    bool CondExp_vpvv(void) {
        using namespace CppAD;
        using namespace std;

        bool ok = true;

        // independent variable vector
        std::vector<double> u(3);
        u[0] = 0.;
        u[1] = 1.;
        u[2] = 2.;

        CPPAD_TEST_VECTOR< AD<double> > X(3);
        X[0] = u[0];
        X[1] = u[1];
        X[2] = u[2];
        Independent(X);

        // parameter value
        AD<double> one = 1.;

        // dependent variable vector 
        CPPAD_TEST_VECTOR< AD<double> > Y(5);

        // CondExp(variable, parameter, variable, variable)
        Y[0] = CondExpLt(X[0], one, X[1], X[2]);
        Y[1] = CondExpLe(X[0], one, X[1], X[2]);
        Y[2] = CondExpEq(X[0], one, X[1], X[2]);
        Y[3] = CondExpGe(X[0], one, X[1], X[2]);
        Y[4] = CondExpGt(X[0], one, X[1], X[2]);

        // create f: X -> Y 
        ADFunCodeGen<double> f(X, Y);


        ok &= test0nJac("CondExp_vpvv", f, u, Y);

        return ok;
    }

    bool CondExp_vvpv(void) {
        using namespace CppAD;
        using namespace std;

        bool ok = true;

        // independent variable vector
        std::vector<double> u(3);
        u[0] = 0.;
        u[1] = 1.;
        u[2] = 2.;

        CPPAD_TEST_VECTOR< AD<double> > X(3);
        X[0] = u[0];
        X[1] = u[1];
        X[2] = u[2];
        Independent(X);

        // parameter value
        AD<double> three = 3.;

        // dependent variable vector 
        CPPAD_TEST_VECTOR< AD<double> > Y(5);

        // CondExp(variable, variable, parameter, variable)
        Y[0] = CondExpLt(X[0], X[1], three, X[2]);
        Y[1] = CondExpLe(X[0], X[1], three, X[2]);
        Y[2] = CondExpEq(X[0], X[1], three, X[2]);
        Y[3] = CondExpGe(X[0], X[1], three, X[2]);
        Y[4] = CondExpGt(X[0], X[1], three, X[2]);

        // create f: X -> Y 
        ADFunCodeGen<double> f(X, Y);

        ok &= test0nJac("CondExp_vvpv", f, u, Y);

        return ok;
    }

    bool CondExp_vvvp(void) {
        using namespace CppAD;
        using namespace std;

        bool ok = true;

        // independent variable vector
        std::vector<double> u(3);
        u[0] = 0.;
        u[1] = 1.;
        u[2] = 2.;

        CPPAD_TEST_VECTOR< AD<double> > X(3);
        X[0] = u[0];
        X[1] = u[1];
        X[2] = u[2];
        Independent(X);

        // parameter value
        AD<double> three = 3.;

        // dependent variable vector 
        CPPAD_TEST_VECTOR< AD<double> > Y(5);

        // CondExp(variable, variable, variable, parameter)
        Y[0] = CondExpLt(X[0], X[1], X[2], three);
        Y[1] = CondExpLe(X[0], X[1], X[2], three);
        Y[2] = CondExpEq(X[0], X[1], X[2], three);
        Y[3] = CondExpGe(X[0], X[1], X[2], three);
        Y[4] = CondExpGt(X[0], X[1], X[2], three);

        // create f: X -> Y 
        ADFunCodeGen<double> f(X, Y);

        ok &= test0nJac("CondExp_vvvp", f, u, Y);

        return ok;
    }

#include <limits>

    bool SecondOrderReverse(void) { // Bradley M. Bell 2009-07-04 
        // Reverse mode for CExpOp was only modifying the highest order partial
        // This test demonstrated the bug
        bool ok = true;
        using CppAD::AD;
        using CppAD::NearEqual;
        double eps = 10. * std::numeric_limits<double>::epsilon();

        size_t n = 1;
        CPPAD_TEST_VECTOR< AD<double> > X(n);
        X[0] = 2.;
        CppAD::Independent(X);

        size_t m = 2;
        CPPAD_TEST_VECTOR< AD<double> > Y(m);

        AD<double> left = X[0];
        AD<double> right = X[0] * X[0];
        AD<double> exp_if_true = left;
        AD<double> exp_if_false = right;

        // Test of reverse mode using exp_if_true case
        // For this value of X, should be the same as Z = X[0]
        AD<double> Z = CondExpLt(left, right, exp_if_true, exp_if_false);
        Y[0] = Z * Z;

        // Test of reverse mode  using exp_if_false case
        exp_if_false = left;
        exp_if_true = right;
        Z = CondExpGt(left, right, exp_if_true, exp_if_false);
        Y[1] = Z * Z;

        CppAD::ADFun<double> f(X, Y);

        // first order forward
        CPPAD_TEST_VECTOR<double> dx(n);
        size_t p = 1;
        dx[0] = 1.;
        f.Forward(p, dx);

        // second order reverse (test exp_if_true case)
        CPPAD_TEST_VECTOR<double> w(m), dw(2 * n);
        w[0] = 1.;
        w[1] = 0.;
        p = 2;
        dw = f.Reverse(p, w);

        // check first derivative in dw
        double check = 2. * Value(X[0]);
        ok &= NearEqual(dw[0], check, eps, eps);

        // check second derivative in dw
        check = 2.;
        ok &= NearEqual(dw[1], check, eps, eps);

        // test exp_if_false case
        w[0] = 0.;
        w[1] = 1.;
        p = 2;
        dw = f.Reverse(p, w);

        // check first derivative in dw
        check = 2. * Value(X[0]);
        ok &= NearEqual(dw[0], check, eps, eps);

        // check second derivative in dw
        check = 2.;
        ok &= NearEqual(dw[1], check, eps, eps);

        return ok;
    }

    double Infinity(double zero) {
        return 1. / zero;
    }

    bool OldExample(void) {
        bool ok = true;

        using CppAD::AD;
        using CppAD::NearEqual;
        using CppAD::log;
        using CppAD::abs;
        double eps = 100. * std::numeric_limits<double>::epsilon();

        // domain space vector
        size_t n = 5;
        CPPAD_TEST_VECTOR< AD<double> > X(n);
        size_t j;
        for (j = 0; j < n; j++)
            X[j] = 1.;

        // declare independent variables and start tape recording
        CppAD::Independent(X);

        // sum with respect to j of log of absolute value of X[j]
        // sould be - infinity if any of the X[j] are zero
        AD<double> MinusInfinity = -Infinity(0.);
        AD<double> Sum = 0.;
        AD<double> Zero(0);
        for (j = 0; j < n; j++) { // if X[j] > 0
            Sum += CppAD::CondExpGt(X[j], Zero, log(X[j]), Zero);

            // if X[j] < 0
            Sum += CppAD::CondExpLt(X[j], Zero, log(-X[j]), Zero);

            // if X[j] == 0
            Sum += CppAD::CondExpEq(X[j], Zero, MinusInfinity, Zero);
        }

        // range space vector 
        size_t m = 1;
        CPPAD_TEST_VECTOR< AD<double> > Y(m);
        Y[0] = Sum;

        // create f: X -> Y and stop tape recording
        CppAD::ADFun<double> f(X, Y);

        // vectors for arguments to the function object f
        CPPAD_TEST_VECTOR<double> x(n); // argument values
        CPPAD_TEST_VECTOR<double> y(m); // function values 
        CPPAD_TEST_VECTOR<double> w(m); // function weights 
        CPPAD_TEST_VECTOR<double> dw(n); // derivative of weighted function

        // a case where abs( x[j] ) > 0 for all j
        double check = 0.;
        double sign = 1.;
        for (j = 0; j < n; j++) {
            sign *= -1.;
            x[j] = sign * double(j + 1);
            check += log(abs(x[j]));
        }

        // function value 
        y = f.Forward(0, x);
        ok &= (y[0] == check);

        // compute derivative of y[0]
        w[0] = 1.;
        dw = f.Reverse(1, w);
        for (j = 0; j < n; j++) {
            if (x[j] > 0.)
                ok &= NearEqual(dw[j], 1. / abs(x[j]), eps, eps);
            else ok &= NearEqual(dw[j], -1. / abs(x[j]), eps, eps);
        }

        // a case where x[0] is equal to zero
        sign = 1.;
        for (j = 0; j < n; j++) {
            sign *= -1.;
            x[j] = sign * double(j);
        }

        // function value 
        y = f.Forward(0, x);
        ok &= (y[0] == -Infinity(0.));

        // compute derivative of y[0]
        w[0] = 1.;
        dw = f.Reverse(1, w);
        for (j = 0; j < n; j++) {
            if (x[j] > 0.)
                ok &= NearEqual(dw[j], 1. / abs(x[j]), eps, eps);
            else if (x[j] < 0.)
                ok &= NearEqual(dw[j], -1. / abs(x[j]), eps, eps);
            else { // in this case computing dw[j] ends up multiplying 
                // -infinity * zero and hence results in Nan
            }
        }

        return ok;
    }
} // end empty namespace

bool CondExp(void) {
    bool ok = true;
    ok &= CondExp_pvvv();
    ok &= CondExp_vpvv();
    ok &= CondExp_vvpv();
    ok &= CondExp_vvvp();
    return ok;
}
// END PROGRAM