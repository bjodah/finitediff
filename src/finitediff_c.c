#include <stdlib.h> /* malloc & free */
#include <string.h> /* memset */
#include "finitediff_c.h"
#include "newton_interval.h"

#ifdef FINITEDIFF_OPENMP
#include <omp.h>
#else
#define omp_get_thread_num() 0
#endif

void finitediff_calculate_weights(
    FINITEDIFF_REAL * const FINITEDIFF_RESTRICT w,
    const int ldw,
    const FINITEDIFF_REAL * const FINITEDIFF_RESTRICT grid,
    const int len_g,
    const int max_deriv,
    const FINITEDIFF_REAL around
)
{
    int i, j, k, mn;
    FINITEDIFF_REAL c1, c2, c2_r, c3, c3_r, c4, c5;
    c1 = 1;
    c4 = grid[0] - around;
    memset(w, 0, sizeof(FINITEDIFF_REAL)*ldw*(max_deriv+1));
    w[0] = 1;
    for (i = 1; i < len_g; ++i){
        mn = FINITEDIFF_MIN(i, max_deriv);
        c2 = 1;
        c5 = c4;
        c4 = grid[i] - around;
        for (j = 0; j < i; ++j){
            c3 = grid[i] - grid[j];
            c3_r = 1/c3;
            c2 = c2*c3;
            if (j == i-1){
                c2_r = 1/c2;
                for (k = mn; k >= 1; --k){
                    w[i + k*ldw] = c1*(k*w[i - 1 + (k-1)*ldw] - c5*w[i - 1 + k*ldw])*c2_r;
                }
                w[i] = -c1*c5*w[i-1]*c2_r;
            }
            for (k = mn; k >= 1; --k){
                w[j + k*ldw] = (c4*w[j + k*ldw] - k*w[j + (k-1)*ldw])*c3_r;
            }
            w[j] = c4*w[j]*c3_r;
        }
        c1 = c2;
    }
}

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
)
{
    int i, j, k;
    FINITEDIFF_REAL tmp;
    for (i=0; i<nsets; ++i){
        for (j=0; j <= max_deriv; ++j){
            tmp = 0;
            for (k=0; k<len_grid; ++k){
                tmp += w[k + j*ldw] * ydata[ldy*i + k];
            }
            out[i*ld_out + j] = tmp;
        }
    }
}

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
    )
{
    int status = FINITEDIFF_STATUS_SUCCESS;
    FINITEDIFF_REAL * w;
    const int ldw=len_grid;
    w = (FINITEDIFF_REAL *)malloc(sizeof(FINITEDIFF_REAL)*ldw*(max_deriv+1));
    if (!w) {
        status = FINITEDIFF_STATUS_ERR_BAD_ALLOC;
        goto exit0;
    }
    if (len_grid < max_deriv + 1){
        status = FINITEDIFF_STATUS_ERR_TOO_SMALL_GRID;
        goto exit1;
    }
    if (ld_out < max_deriv + 1) {
        status = FINITEDIFF_STATUS_ERR_WRONG_LEADING_DIMENSION;
        goto exit1;
    }
    finitediff_calculate_weights(w, ldw, grid, len_grid, max_deriv, xtgt);
    finitediff_apply_fd(out, ld_out, w, ldw, nsets, max_deriv, len_grid, ydata, ldy);
exit1:
    free(w);
exit0:
    return status;
}

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
)
{
    FINITEDIFF_REAL xtgt;
    int tgt_idx, j=0, status=0, n_threads=1;
    const int nin = nhead + ntail;
    FINITEDIFF_REAL *w, *wp;
    const int elem_strides_w_1 = FINITEDIFF_MIN(len_grid, nin);
#ifndef FINITEDIFF_OPENMP
    const int elem_strides_w_0 = elem_strides_w_1*(max_deriv+1);
#else
    const int elem_strides_w_0 = FINITEDIFF_ROUND_L1(elem_strides_w_1*(max_deriv+1));
    char * num_threads_var;
    num_threads_var = getenv("FINITEDIFF_NUM_THREADS");
    if (num_threads_var) {
        n_threads = atoi(num_threads_var);
        if (!n_threads) {
            status = FINITEDIFF_STATUS_ERR_ILLEGAL_ENV_VAR;
            goto exit0;
        }
    } else {
        n_threads = omp_get_num_threads();
    }
#endif
    if (len_grid < max_deriv + 1){
        status = FINITEDIFF_STATUS_ERR_TOO_SMALL_GRID;
        goto exit0;
    }
    if (nin < max_deriv + 1){
        status = FINITEDIFF_STATUS_ERR_TOO_FEW_POINTS;
        goto exit0;
    }
    w = (FINITEDIFF_REAL *)malloc(sizeof(FINITEDIFF_REAL)*elem_strides_w_0*n_threads);
    if (!w) {
        status = FINITEDIFF_STATUS_ERR_BAD_ALLOC;
        goto exit0;
    }
#ifdef FINITEDIFF_OPENMP
#pragma omp parallel for private(xtgt, wp) firstprivate(j) schedule(static) num_threads(n_threads)
#endif
    for (tgt_idx=0; tgt_idx<len_targets; ++tgt_idx) {
        xtgt = xtgts[tgt_idx];
        j = get_interval_from_guess(grid, len_grid, xtgt, j) - nhead;
        j = FINITEDIFF_MAX(0, FINITEDIFF_MIN(j, len_grid - nin));
        wp = w + omp_get_thread_num()*elem_strides_w_0;
        finitediff_calculate_weights(wp, elem_strides_w_1, grid+j, elem_strides_w_1, max_deriv, xtgt);
        finitediff_apply_fd(out + tgt_idx*elem_strides_out_0, elem_strides_out_1,
                            wp, elem_strides_w_1, nsets,
                            max_deriv, elem_strides_w_1, ydata + j, ldy);
    }
    free(w);
exit0:
    return status;
}
    
