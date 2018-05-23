#include <stdlib.h> /* malloc & free */
#include <string.h> /* memset */
#include "finitediff_c.h"

#define FINITEDIFF_MIN(x, y) (((x) < (y)) ? (x) : (y))

void calculate_weights(
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
)
{
    int i, j, k;
    const int ldw = len_g;
    FINITEDIFF_REAL tmp;
    int ret = 0;
    FINITEDIFF_REAL * w;
    w = malloc(sizeof(FINITEDIFF_REAL)*ldw*(max_deriv+1));
    if (!w) {
        ret = 1;
        goto exit0;
    }
    if (len_g < max_deriv + 1){
        ret = 2;
        goto exit1;
    }
    if (ld_out < max_deriv + 1) {
        ret = 3;
        goto exit1;
    }
    calculate_weights(w, ldw, xdata, len_g, max_deriv, xtgt);
    for (i=0; i<nsets; ++i){
        for (j=0; j <= max_deriv; ++j){
            tmp = 0;
            for (k=0; k<len_g; ++k){
                tmp += w[k + j*ldw] * ydata[ldy*i + k];
            }
            out[i*ld_out + j] = tmp;
        }
    }
exit1:
    free(w);
exit0:
    return ret;
}
