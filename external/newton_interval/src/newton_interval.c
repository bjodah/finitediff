#include <math.h>    /* sqrt() */
#include "newton_interval.h"

inline static int ceil_away0(double d){
    return (d>0.0) ? ceil(d) : floor(d);
}

int get_interval_from_guess(const double * const arr, const NI_SIZE_T N, 
			    const double t, NI_SIZE_T i){
    /*
    Version of get_interval which takes an index guess as last arg
        get_interval locates the index `i` for which  is
        t > arr[i] and arr[i+1] > t
    */
    // Delta i for estimated derivative 
    NI_SIZE_T sqrt_nt = (NI_SIZE_T)sqrt((double)N)+1; 
    NI_SIZE_T h; /* step in num der */
    NI_SIZE_T di; /* Delta i (update step in root finding) */
    double dtdi;
    NI_SIZE_T lower_bound = -1; // excluded
    NI_SIZE_T upper_bound = N; // excluded
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


	/* Check we're not out of explored boundaries; */
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

int get_interval(const double * const arr, const NI_SIZE_T N, const double t){
    double t0 = arr[0];
    double tend = arr[N-1];
    NI_SIZE_T i = ((t - t0)/(tend - t0) * N);
    return get_interval_from_guess(arr, N, t, i);
}

int check_nan(const double * const arr, NI_SIZE_T n){
    // from math.h (link with -lm)
    for (NI_SIZE_T i=0; i<n; ++i)
	if (isnan(arr[i]))
	    return i;
    return -1; // if no NaN is encountered, -1 is returned
}

int check_strict_monotonicity(const double * const arr, NI_SIZE_T n){
    double x, old_x;
    old_x = arr[0];
    for (NI_SIZE_T i=1; i<n; ++i){
        x = arr[i];
	if (x <= old_x)
            return 0;
        old_x = x;
    }
    return 1;
}
