#include <math.h>    /* sqrt() */
#include "newton_interval.h"

#define BOOL int // fastest way

inline static int ceil_away0(double d);

inline static int ceil_away0(double d){
  return (d>0.0) ? ceil(d) : floor(d);
}

int get_interval_from_guess(const double arr[], const int N, const double t, int i){
  // Version of get_interval which takes an index guess as last arg
  /*
    get_interval locates the index `i` for which  is
      t > arr[i] and arr[i+1] > t
  */
  int sqrt_nt = (int)sqrt((double)N)+1; /* Delta i for estimated derivative */
  int h; /* step in num der */
  int di; /* Delta i (update step in root finding) */
  double dtdi;
  int lower_bound = -1; // excluded
  int upper_bound = N; // excluded
  BOOL gteq_ti;    /* t >= t[i] */
  BOOL lt_tip1;    /* t < t[i+1] */
  BOOL eq_tip1;    /* t == t[i+1] */

  if (N <= 2)
    return 0;
  if (t < arr[0])
    return -1;

  /* 0 i <= N - 2 */
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

int get_interval(const double arr[], const int N, const double t){
  double t0 = arr[0];
  double tend = arr[N-1];
  int i = ((t - t0)/(tend - t0) * N);
  return get_interval_from_guess(arr, N, t, i);
}
