#include <math.h>    /* sqrt(), ceil(), floor(), isnan() */
#include "newton_interval.h"


inline static int ceil_away0(double d){
    // ceil away from zero
    return (d>0.0) ? ceil(d) : floor(d);
}

int get_interval_from_guess(const double * const arr, const int N, 
			    const double t, int i){
    // Quadratic convergence in for lookup in monotonic arrays.
    // if arr[0] <= t < arr[N-1] then the index `j` is returned for which:
    //      arr[j] < t < arr[j+1]
    // holds (means that 0 <= j < N-2).
    // if t < arr[0], -1 is returned
    // if t >= arr[N-1], N-1 is returned
    //
    // Parameters
    // ----------
    // arr: pointer to strictily monotonic array (optimally well behaved)
    // N:   lenth of `arr`
    // t:   value for which j is sought: arr[j] < t <= arr[j+1]
    // i:   intial guess of j

    // Delta i for estimated derivative
    int sqrt_nt = (int)sqrt((double)N)+1; 
    int h; // step in num der
    int di; // Delta i (update step in root finding)
    double dtdi;
    int lower_bound = -1; // excluded
    int upper_bound = N; // excluded
    int gteq_ti;    // t >= t[i]
    int lt_tip1;    // t < t[i+1]
    int eq_tip1;    // t == t[i+1]

    if (N <= 2)
        return 0;
    if (t < arr[0])
        return -1;

    // 0 i <= N - 2
    if (i > N - 2)
        i = N - 2;
    else if (i < 0)
        i = 0;

    gteq_ti = (t >= arr[i]);
    lt_tip1 = (t < arr[i + 1]);
    eq_tip1 = (t == arr[i + 1]);

    if (eq_tip1) // Unlikekly for floating point numbers
        return i + 1;

    while (!(gteq_ti & lt_tip1)){
        if (gteq_ti)
            h = sqrt_nt;
        else
            h = -sqrt_nt;

        /* Check we're not out of explored boundaries */
        while (((i + h) >= upper_bound) || ((i + h) <= lower_bound)){
            h /= 2;
            if (h == 0){
                h = (upper_bound - lower_bound)-i;
                break;
            }
        }
        dtdi = (arr[i+h] - arr[i]) / h;
        di = ceil_away0((t - arr[i]) / dtdi);
        /* Check we're not out of explored boundaries; */
        while (((i + di) >= upper_bound) || ((i + di) <= lower_bound)){
            di /= 2;
            if (di == 0)
                di = (upper_bound - lower_bound)-i;
        }
        /* Update i */
        i += di;
        /* Update loop conditions; */
        gteq_ti = (t >= arr[i]);
        lt_tip1 = (t < arr[i + 1]);
        eq_tip1 = (t == arr[i + 1]);
        if (eq_tip1) return i + 1;
        /* Update lower and upper boundaries; */
        if ((gteq_ti) && (i > lower_bound))
            lower_bound = i;
        if ((lt_tip1) && (i < upper_bound))
            upper_bound = i;
        if ((i == lower_bound) && (i+1 == upper_bound)) return i;
    }
    return i;
}

int get_interval(const double * const arr, const int N, const double t){
    // This function is a convenience wrapper around get_interval_from guess using
    // an inital guess from linear interpolation between end points of array.
    double t0 = arr[0];
    double tend = arr[N-1];
    int i = ((t - t0)/(tend - t0) * N);
    return get_interval_from_guess(arr, N, t, i);
}

int check_nan(const double * const arr, int n){
    // Returns the index of the first occurence of NaN
    // in input array, returns -1 if no NaN is encountered
    //
    // Parameters
    // ----------
    // arr: pointer to array of doubles to be checked for occurence of NaN
    // n: length of array
    //
    // Notes
    // -----
    // isnan is defined in math.h (link with -lm)
    // from math.h (link with -lm)
    int i;
    for (i=0; i<n; ++i)
	if (isnan(arr[i]))
	    return i;
    return -1; // if no NaN is encountered, -1 is returned
}

int check_strict_monotonicity(const double * const arr, int n){
    // Checks for strict monotonicity (a[i+1] > a[i]) in array of doubles.
    // Returns the index of the value not being in strict monotonicity,
    // if all values are strictly monotonic -1 is returned
    //
    // Parameters
    // ----------
    // arr: pointer to array of doubles to be checked for occurence of NaN
    // n: length of array
    //
    // Notes
    // -----
    // isnan is defined in math.h (link with -lm)
    int i;
    double x, old_x;
    old_x = arr[0];
    for (i=1; i<n; ++i){
        x = arr[i];
        if (x <= old_x)
            return i;
        old_x = x;
    }
    return -1;
}
