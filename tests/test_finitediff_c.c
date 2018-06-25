#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <finitediff_c.h>



int test_calculate_weights_3(){
    double x3[3] = {-1, 0, 1};
    const int ldw = 7;
    const int len_g = 3;
    const int max_deriv = 1;
    double * const w3 = malloc(sizeof(double)*ldw*(max_deriv+1));
    const double around = 0.0;
    finitediff_calculate_weights(w3, ldw, x3, len_g, max_deriv, around);
    
    assert(fabs(w3[0] - 0) < 1e-15);
    assert(fabs(w3[1] - 1) < 1e-15);
    assert(fabs(w3[2] - 0) < 1e-15);
                 
    assert(fabs(w3[ldw + 0] + .5) < 1e-15);
    assert(fabs(w3[ldw + 1] - .0) < 1e-15);
    assert(fabs(w3[ldw + 2] - .5) < 1e-15);

    free(w3);
    return 0;
}

int test_calculate_weights_5(){
    double x5[5] = {-2, -1, 0, 1, 2};
    const int ldw = 7;
    const int len_g = 5;
    const int max_deriv = 2;
    double * const w5 = malloc(sizeof(double)*ldw*(max_deriv+1));
    const double around = 0.0;
    finitediff_calculate_weights(w5, ldw, x5, len_g, max_deriv, around);
    
    assert(fabs(w5[ldw*0 + 0] - 0) < 1e-15);
    assert(fabs(w5[ldw*0 + 1] - 0) < 1e-15);
    assert(fabs(w5[ldw*0 + 2] - 1) < 1e-15);
    assert(fabs(w5[ldw*0 + 3] - 0) < 1e-15);
    assert(fabs(w5[ldw*0 + 4] - 0) < 1e-15);
                 
    assert(fabs(w5[ldw*1 + 0] - 1/12.) < 1e-14);
    assert(fabs(w5[ldw*1 + 1] + 2/3.) < 1e-14);
    assert(fabs(w5[ldw*1 + 2] - .0) < 1e-14);
    assert(fabs(w5[ldw*1 + 3] - 2/3.) < 1e-14);
    assert(fabs(w5[ldw*1 + 4] + 1/12.) < 1e-14);
                 
    assert(fabs(w5[ldw*2 + 0] + 1/12.) < 1e-14);
    assert(fabs(w5[ldw*2 + 1] - 4/3.) < 1e-14);
    assert(fabs(w5[ldw*2 + 2] + 5/2.) < 1e-14);
    assert(fabs(w5[ldw*2 + 3] - 4/3.) < 1e-14);
    assert(fabs(w5[ldw*2 + 4] + 1/12.) < 1e-14);

    free(w5);
    return 0;
}

int get_ref_out_(
    double * ref,
    double * out,
    const int ldout,
    double * grid,
    int grid_len,
    const double x,
    const int maxord)
{
    int idx, order, flag;
    const int nsets = 1;
    double * const values = malloc(sizeof(double)*grid_len);
    for (idx=0; idx < grid_len; ++idx){
        values[idx] = grid[idx]*exp(-grid[idx]);  /* f(x) = x*exp(-x) */
    }
    for (order=0; order <= maxord; ++order){
        ref[order] = pow(-1, order)*(x - order)*exp(-x);
    }
    flag = finitediff_calc_and_apply_fd(out, ldout, nsets, maxord, grid_len, grid, values, grid_len, x);
    free(values);
    if (flag){
        printf("Error, flag=%d", flag);
        return 1;
    }
    return 0;
}

int check_x_exp_mx_(
    const int maxord,
    const double x,
    double * grid,
    int grid_len,
    double lg_atol0,
    double degrade_factor
    ){
    int order, flag;
    const int ldout = maxord+1;
    double atol, adiff;
    double * ref, * out;
    ref = malloc(sizeof(double)*ldout);
    out = malloc(sizeof(double)*ldout);
    flag = get_ref_out_(ref, out, ldout, grid, grid_len, x, maxord);
    if(flag) {
        goto early_exit;
    }
    for (order=0; order <= maxord; ++order){
        atol = pow(10.0, lg_atol0 + degrade_factor*order);
        adiff = fabs(ref[order] - out[order]);
        if ( adiff > atol ){
            flag = 1;
            goto early_exit;
        }
    }
early_exit:
    free(ref);
    free(out);
    return flag;
}

double g1[3] = {-0.1, 0.0, 0.1};
double g2[3] = {-0.1, 0.0, 0.1};
double g3[3] = {0.9, 1.0, 1.1};
double g4[5] = {0.8, 0.9, 1.0, 1.1, 1.2};
double g5[7] = {0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3};
double g6[7] = {0.5, 0.98, 0.99, 1.0, 1.2, 1.3, 1.4};
double g7[7] = {0.5, 0.0118, 1.0120, 1.0122, 1.2, 1.3, 1.4};

int test_apply_fd()
{
    const double x = 1.0122333444455555;
    if (check_x_exp_mx_(2, 0, g1, 3, -4.0, 1.8) |
        check_x_exp_mx_(2, 0, g2, 3, -4.0, 1.8) |
        check_x_exp_mx_(2, x, g3, 3, -4.6, 1.8) |
        check_x_exp_mx_(3, x, g4, 5, -7.0, 1.8) |
        check_x_exp_mx_(5, x, g5, 7, -9.0, 1.4) |
        check_x_exp_mx_(5, x, g6, 7, -9.3, 1.6) |
        check_x_exp_mx_(5, x, g7, 7, -9.4, 2.3))
    {
        return 1;
    }
    return 0;
}

int test_interpolate_by_finite_diff() {
    double * out;
    const int len_tgts = 5, nsets = 4, max_deriv = 2;
    const int out_strd1 = max_deriv+1;
    const int out_strd0 = out_strd1*nsets;
    const int len_grid = 3;
    const int ntail=2, nhead=2;
    const double grid[3] = {0.0, 1.0, 2.0};
    const double ydata[3*4] = {2.0, 3.0, 5.0,
                               3.0, 4.0, 7.0,
                               7.0, 8.0, 9.0,
                               3.0, 4.0, 6.0};
    const int ldy = len_grid;
    const double xtgts[5] = {0.5 , 0.75, 1.  , 1.25, 1.5};

    const double ref[60] = {
    /* import numpy as np */
    /* from finitediff import get_weights */
    /* grid = np.array([0.0, 1.0, 2.0]) */
    /* ydata = np.array([[2.0, 3, 5], [3, 4, 7], [7, 8, 9], [3, 4, 6]]) */
    /* xtgts = np.linspace(0.5, 1.5, 5) */
    /* ref = [] */
    /* maxorder=2 */
    /* for i in range(xtgts.size): */
    /*     w = get_weights(grid, xtgts[i], -1, maxorder) */
    /*     for j in range(ydata.shape[0]): */
    /*         for k in range(maxorder+1): */
    /*             ref.append(np.dot(w[:, k], ydata[j, :])) */
    /* print(ref) */
        2.375, 1.0, 1.0, 3.25, 1.0, 2.0, 7.5, 1.0, 0.0, 3.375, 1.0,
        1.0, 2.65625, 1.25, 1.0, 3.5625, 1.5, 2.0, 7.75, 1.0, 0.0,
        3.65625, 1.25, 1.0, 3.0, 1.5, 1.0, 4.0, 2.0, 2.0, 8.0, 1.0,
        0.0, 4.0, 1.5, 1.0, 3.40625, 1.75, 1.0, 4.5625, 2.5, 2.0,
        8.25, 1.0, 0.0, 4.40625, 1.75, 1.0, 3.875, 2.0, 1.0, 5.25,
        3.0, 2.0, 8.5, 1.0, 0.0, 4.875, 2.0, 1.0
    };
    int i, flag = 0;
    out = (double *)malloc(sizeof(double)*len_tgts*nsets*(max_deriv+1));
    if (!out){
        flag = -1;
        goto exit0;
    }
    finitediff_interpolate_by_finite_diff(out, len_tgts, nsets, max_deriv, out_strd0, out_strd1,
                                          ntail, nhead, grid, len_grid, ydata, ldy, xtgts);
    for (i=0; i<len_tgts*nsets*(max_deriv+1); ++i){
        if (fabs(out[i] - ref[i]) > 1e-14){
            flag = i+1;
            goto exit1;
        }
    }
exit1:
    free(out);
exit0:
    return flag;
}


int main(){
    if (test_calculate_weights_3() ||
        test_calculate_weights_5() ||
        test_apply_fd() ||
        test_interpolate_by_finite_diff()
        ) {
        return 1;
    }
    return 0;
}
