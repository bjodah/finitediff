# Cython pxd file (http://docs.cython.org/src/tutorial/pxd_files.html)
# for c_fornberg.f90 (Fortran 2003 module wrapping fornberg.f90 using ISO_C_BINDING)
cdef extern void fornberg_apply_fd(int nin, int maxorder,
    const double * const xdata, const double * const ydata,
    double xtgt, double * const out)

cdef extern void fornberg_populate_weights(double z, const double * const x,
    int nd, int m, double * const c)

