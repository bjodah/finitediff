#ifndef _NEWTON_INTERVAL_H_
#define _NEWTON_INTERVAL_H_

 /*
   int is usually sufficient (so far for me)
   to override use e.g. `gcc -DNI_SIZE_T size_t ...` 
 */
#define NI_SIZE_T int

#ifdef __cplusplus
extern "C" {
#endif

int get_interval_from_guess(const double * const arr, const NI_SIZE_T N, const double t, NI_SIZE_T i);
int get_interval(const double * const arr, const NI_SIZE_T N, const double t);
int check_nan(const double * const arr, NI_SIZE_T n);
int check_strict_monotonicity(const double * const arr, NI_SIZE_T n);

#ifdef __cplusplus
}
#endif


#endif /* _NEWTON_INTERVAL_H_ */
