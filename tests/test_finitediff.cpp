#include <cmath>
#include <vector>
#include <iostream>
#include "c_finitediff_fort.h"
#include "finitediff_templated.hpp"

// C++11 source calling into Fortran 2003 c_fornberg using ISO_C_BINDING
// see Makefile for example of how to compile and link.

using std::vector;

typedef void (*apply_fd_ptr)(int, int, const double * const, 
                             const double * const, double, double * const);
typedef void (*populate_weights_ptr)(double, const double * const, int, 
                                     int, double * const);

int test_apply_fd(apply_fd_ptr cb){
    vector<double> xdata {0, .5, 1};
    vector<double> ydata {0, .25, 1};
    double out[3] {-42, -42, -42};
    (*cb)(xdata.size(), 2, &xdata[0], &ydata[0], .5, &out[0]);
    if (fabs(out[0] - .25) > 1e-10) {std::cout << 0 << out[0]; return 1;}
    if (fabs(out[1] - 1.0) > 1e-10) {std::cout << 1 << out[1]; return 1;}
    if (fabs(out[2] - 2.0) > 1e-10) {std::cout << 2 << out[2]; return 1;}
    return 0;
}

int test_populate_weights(populate_weights_ptr cb, bool colmaj){
    vector<double> xdata {-1, 0, 1};
    double c[6];
    int maxorder = 1;
    int rs, cs;
    if (colmaj){
        rs=1; cs=3;
    }else{
        rs=3; cs=1;
    }
    (*cb)(0, &xdata[0], xdata.size()-1, maxorder, &c[0]);
    if (fabs(c[0*rs + 0*cs] - 0.0) > 1e-10) {return 4;}
    if (fabs(c[1*rs + 0*cs] - 1.0) > 1e-10) {return 4;}
    if (fabs(c[2*rs + 0*cs] - 0.0) > 1e-10) {return 4;}
    if (fabs(c[0*rs + 1*cs] + 0.5) > 1e-10) {return 4;}
    if (fabs(c[1*rs + 1*cs] - 0.0) > 1e-10) {return 4;}
    if (fabs(c[2*rs + 1*cs] - 0.5) > 1e-10) {return 4;}
    return 0;
}

int main(){
    int result = 0;
    result += test_apply_fd(&fornberg_apply_fd);
    result += test_populate_weights(&fornberg_populate_weights, true);
    result += 2*test_apply_fd(&finitediff::apply_fd<double>);
    result += 2*test_populate_weights(&finitediff::populate_weights<double>, true);
    return result;
}
