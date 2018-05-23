#pragma once
#ifndef FINITEDIFF_REAL
#define FINITEDIFF_REAL double
#endif
#ifdef FINITEDIFF_WITH_RESTRICT
  #ifdef _MSC_VER
    #define FINITEDIFF_RESTRICT __restrict
  #else
    #define FINITEDIFF_RESTRICT restrict
  #endif
#else
  #define FINITEDIFF_RESTRICT
#endif


#ifdef __cplusplus
extern "C" {
#endif

/*
  calculate_weights
  =================
  
  Parameters
  ----------
  weights[len_g, max_deriv+1]: weights of order 0 to max_deriv
      (output argument, contiguous memory, column major order, no need to clear before call)
  ld_weights: leading dimension of ``weights_out``
  grid[len_g]: array with grid point locations
  len_g: length of grid
  max_deriv: highest derivative.
  around: location where approximations are to be accurate
  
  Requirements
  ------------
  a) ``len_g < max_deriv + 1``
  b) all values in ``grid`` are unique

 
  References
  ----------
  Generation of Finite Difference Formulas on Arbitrarily
  Spaced Grids, Bengt Fornberg,
  Mathematics of compuation, 51, 184, 1988, 699-706
*/
void calculate_weights(
    FINITEDIFF_REAL * const FINITEDIFF_RESTRICT weights,
    const int ld_weights,
    const FINITEDIFF_REAL * const FINITEDIFF_RESTRICT grid,
    const int len_g,
    const int max_deriv,
    const FINITEDIFF_REAL around
);

/*
  apply_fd
  ========
 
  Parameters
  ----------
  out : array of derivatives at ``xtgt``
  ld_out : leading  dimension of ``out``
  nsets : outer diension of ydata
  max_deriv : highest derivative to calculate for
  len_g : length of ``xdata``
  xdata : grid points
  ydata : (sets of) values (``nsets`` x ``len_g``)
  ldy : leading dimension of ``ydata`` (usually ``len_g``)
  xtgt : value of x at which derivatives are to be estimated

  Returns
  -------
  0: success
  1: malloc failed
  2: ``len_g < max_deriv + 1``
  3: ``ld_out < max_deriv + 1``

*/
int apply_fd(
    FINITEDIFF_REAL * const FINITEDIFF_RESTRICT out,
    const int ld_out,
    const int nsets,
    const int max_deriv,
    const int len_g,
    const FINITEDIFF_REAL * const FINITEDIFF_RESTRICT xdata,
    const FINITEDIFF_REAL * const FINITEDIFF_RESTRICT ydata,
    const int ldy,
    const FINITEDIFF_REAL xtgt
);

#ifdef __cplusplus
}
#endif
