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
    calculate_weights(w3, ldw, x3, len_g, max_deriv, around);
    
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
    calculate_weights(w5, ldw, x5, len_g, max_deriv, around);
    
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
    flag = apply_fd(out, ldout, nsets, maxord, grid_len, grid, values, grid_len, x);
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
        assert( adiff < atol );
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


int main(){
    if (test_calculate_weights_3() ||
        test_calculate_weights_5() ||
        test_apply_fd()) {
        return 1;
    }
    return 0;
}
