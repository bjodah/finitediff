#include <cmath>
#include <vector>
#include "c_fornberg.h"

// C++11 source calling into Fortran 2003 c_fornberg using ISO_C_BINDING
// see Makefile for example of how to compile and link.

using std::vector;

int test_apply_fd(){
    vector<double> xdata {0, .5, 1};
    vector<double> ydata {0, .25, 1};
    double out[3];
    fornberg_apply_fd(xdata.size(), 2, &xdata[0], &ydata[0], .5, &out[0]);
    if (fabs(out[0] - .25) > 1e-10) {return 1;}
    if (fabs(out[1] - 1.0) > 1e-10) {return 1;}
    if (fabs(out[2] - 2.0) > 1e-10) {return 1;}
    return 0;
}

int test_populate_weights(){
    vector<double> xdata {-1, 0, 1};
    double c[6];
    int maxorder = 1;
    fornberg_populate_weights(0, &xdata[0], xdata.size()-1, maxorder, &c[0]);
    if (fabs(c[0] - 0.0) > 1e-10) {return 2;}
    if (fabs(c[1] - 1.0) > 1e-10) {return 2;}
    if (fabs(c[2] - 0.0) > 1e-10) {return 2;}
    if (fabs(c[3] + 0.5) > 1e-10) {return 2;}
    if (fabs(c[4] - 0.0) > 1e-10) {return 2;}
    if (fabs(c[5] - 0.5) > 1e-10) {return 2;}
    return 0;
}

int main(){
    int result = 0;
    result += test_apply_fd();
    result += test_populate_weights();
    return result;
}
