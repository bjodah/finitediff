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
#define FINITEDIFF_MIN(x, y) (((x) < (y)) ? (x) : (y))
#define FINITEDIFF_MAX(x, y) (((x) > (y)) ? (x) : (y))
/* We're assuming the L1 cache of the CPU is 64 bytes long (used to avoid false sharing): */
#define FINITEDIFF_ROUND_L1(x) ((((unsigned)(sizeof(FINITEDIFF_REAL)*x) + 63u) & ~63u)/sizeof(FINITEDIFF_REAL))

#ifdef __cplusplus
extern "C" {
#endif

enum FINITEDIFF_STATUS_CODES {
    FINITEDIFF_STATUS_SUCCESS=0,
    FINITEDIFF_STATUS_ERR_BAD_ALLOC=1,
    FINITEDIFF_STATUS_ERR_TOO_SMALL_GRID=2,
    FINITEDIFF_STATUS_ERR_WRONG_LEADING_DIMENSION=3,
    FINITEDIFF_STATUS_ERR_TOO_FEW_POINTS=4,
    FINITEDIFF_STATUS_ERR_ILLEGAL_ENV_VAR=5
};

/*
  finitediff_calculate_weights
  ============================
  
  Parameters
  ----------
  weights[len_g, max_deriv+1]: weights of order 0 to max_deriv
      (output argument, contiguous memory, column major order, no need to clear before call)
  ld_weights: leading dimension of ``weights_out``
  grid[len_grid]: array with grid point locations
  len_grid: length of grid
  max_deriv: highest derivative.
  around: location where approximations are to be accurate
  
  Requirements
  ------------
  a) ``len_grid < max_deriv + 1``
  b) all values in ``grid`` are unique

 
  References
  ----------
  Generation of Finite Difference Formulas on Arbitrarily
  Spaced Grids, Bengt Fornberg,
  Mathematics of compuation, 51, 184, 1988, 699-706
*/
void finitediff_calculate_weights(
    FINITEDIFF_REAL * const FINITEDIFF_RESTRICT weights,
    const int ld_weights,
    const FINITEDIFF_REAL * const FINITEDIFF_RESTRICT grid,
    const int len_grid,
    const int max_deriv,
    const FINITEDIFF_REAL around
);

/*
  finitediff_apply_fd
  ===================
 
  Parameters
  ----------
  out : array of derivatives
  ld_out : leading dimension of ``out``
  w : array of weights
  ldw : leading dimension of ``w``
  nsets : outer diension of ydata
  max_deriv : highest derivative to calculate for
  len_grid : length of ``xdata``
  ydata : (sets of) values (``nsets`` x ``len_grid``)
  ldy : leading dimension of ``ydata`` (usually ``len_grid``)

*/
void finitediff_apply_fd(
    FINITEDIFF_REAL * const FINITEDIFF_RESTRICT out,
    const int ld_out,
    FINITEDIFF_REAL * const FINITEDIFF_RESTRICT w,
    const int ldw,
    const int nsets,
    const int max_deriv,
    const int len_grid,
    const FINITEDIFF_REAL * const FINITEDIFF_RESTRICT ydata,
    const int ldy
);

/*
  finitediff_calc_apply_fd
  ========================
 
  Parameters
  ----------
  out : array of derivatives at ``xtgt``
  ld_out : leading  dimension of ``out``
  nsets : outer diension of ydata
  max_deriv : highest derivative to calculate for
  len_grid : length of ``xdata``
  xdata : grid points
  ydata : (sets of) values (``nsets`` x ``len_grid``)
  ldy : leading dimension of ``ydata`` (usually ``len_grid``)
  xtgt : value of x at which derivatives are to be estimated

  Returns
  -------
  0: success
  1: malloc failed
  2: ``len_grid < max_deriv + 1``
  3: ``ld_out < max_deriv + 1``

*/
int finitediff_calc_and_apply_fd(
    FINITEDIFF_REAL * const FINITEDIFF_RESTRICT out,
    const int ld_out,
    const int nsets,
    const int max_deriv,
    const int len_grid,
    const FINITEDIFF_REAL * const FINITEDIFF_RESTRICT grid,
    const FINITEDIFF_REAL * const FINITEDIFF_RESTRICT ydata,
    const int ldy,
    const FINITEDIFF_REAL xtgt
);

int finitediff_interpolate_by_finite_diff(
    FINITEDIFF_REAL * const FINITEDIFF_RESTRICT out, /* C-order: out[tgt_idx, set_idx, deriv_idx] */
    const int len_targets,
    const int nsets,
    const int max_deriv,
    const int elem_strides_out_0,  /* tgt_idx * elem_strides_out_0 */
    const int elem_strides_out_1,  /* set_idx * elem_strides_out_1,  strides_out_2 assumed to be 1 */
    const int ntail,
    const int nhead,
    const FINITEDIFF_REAL * const FINITEDIFF_RESTRICT grid,
    const int len_grid,
    const FINITEDIFF_REAL * const FINITEDIFF_RESTRICT ydata, /* C-order: ydata[set_idx, grid_idx] */
    const int ldy,
    const FINITEDIFF_REAL * const FINITEDIFF_RESTRICT xtgts /* len(xtgts) == len_targets */
);

#ifdef __cplusplus
}
#endif
