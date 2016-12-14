#ifndef _4VDGFDQT35GE5HR5B32FYDDODQ
#define _4VDGFDQT35GE5HR5B32FYDDODQ

#ifdef __cplusplus
extern "C" {
#endif

void fornberg_apply_fd(int nin, int maxorder, const double * const xdata,
                       const double * const ydata, double xtgt, double * const out);

void fornberg_populate_weights(double z, const double * const x, int nd,
                               int m, double * const c);

#ifdef __cplusplus
}
#endif

#endif /* _4VDGFDQT35GE5HR5B32FYDDODQ */
