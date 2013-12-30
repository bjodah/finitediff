#ifndef _NEWTON_INTERVAL_H_
#define _NEWTON_INTERVAL_H_

#ifdef __cplusplus
extern "C" {
#endif

int get_interval_from_guess(const double arr[], const int N, const double t, int i);
int get_interval(const double arr[], const int N, const double t);
int check_nan(const double * const arr, int n);
int check_strict_monotonicity(const double * const arr, int n);

#ifdef __cplusplus
}
#endif


#endif /* _NEWTON_INTERVAL_H_ */
