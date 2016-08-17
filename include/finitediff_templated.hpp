#ifndef FINITEDIFF_TEMPLATED_HPP_IWIDN4CP6RGFHFVKSWG4HICEZE
#define FINITEDIFF_TEMPLATED_HPP_IWIDN4CP6RGFHFVKSWG4HICEZE

// Pre-processor macro __cplusplus == 201103L in ISO C++11 compliant compilers. (e.g. GCC >= 4.7.0)
#if __cplusplus > 199711L
#include <stdexcept>
#endif

namespace finitediff {

    template <typename Real_t>
    void populate_weights(const Real_t z, const Real_t * const __restrict__ x, const int nd,
                          const int m, Real_t * const __restrict__ c) {
        // Parameters
        // ----------
        // z: location where approximations are to be accurate
        // x(0:nd): grid point locations
        // nd: dimension of x- and c-arrays (len(x) - 1)
        // c(0:nd, 0:m) derivatives weights of order 0:m (column major order)
        // m: highest derivative.
        //
        // References
        // ----------
        // Generation of Finite Difference Formulas on Arbitrarily
        // Spaced Grids, Bengt Fornberg,
        // Mathematics of compuation, 51, 184, 1988, 699-706

        Real_t c1, c2, c3, c4, c5;
        c1 = 1;
        c4 = x[0] - z;
        for (int i=0; i < (nd+1)*(m+1); ++i)
            c[i] = 0;
        c[0] = 1;
        for (int i=1; i <= nd; ++i){
            const int mn = (i < m) ? i : m; // min(i, m)
            c2 = 1;
            c5 = c4;
            c4 = x[i] - z;
            for (int j=0; j<i; ++j){
                c3 = x[i] - x[j];
                c2 = c2*c3;
                if (j == i-1){
                    for (int k=mn; k>=1; --k){
                        const Real_t tmp1 = c[i - 1 + (k-1)*(nd+1)];
                        const Real_t tmp2 = c[i - 1 + k*(nd+1)];
                        c[i + k*(nd+1)] = c1*(k*tmp1 - c5*tmp2)/c2;
                    }
                    c[i] = -c1*c5*c[i-1]/c2;
                }
                for (int k=mn; k>=1; --k){
                    const Real_t tmp1 = c[j + k*(nd+1)];
                    const Real_t tmp2 = c[j + (k-1)*(nd+1)];
                    c[j + k*(nd+1)] = (c4*tmp1 - k*tmp2)/c3;
                }
                c[j] = c4*c[j]/c3;
            }
            c1 = c2;
        }
    }

    template <typename Real_t>
    void apply_fd(const int nin, const int maxorder,
                  const Real_t * const __restrict__ xdata,
                  const Real_t * const __restrict__ ydata,
                  const Real_t xtgt,
                  Real_t * const __restrict__ out){
        Real_t * const c = new Real_t[nin * (maxorder+1)];
        populate_weights<Real_t>(xtgt, xdata, nin-1, maxorder, c);
        for (int j=0; j <= maxorder; ++j){
            out[j] = 0;
            for (int i=0; i<nin; ++i)
                out[j] += c[i + j*nin] * ydata[i];
        }
        delete []c;
    }


// Pre-processor macro __cplusplus == 201103L in ISO C++11 compliant compilers. (e.g. GCC >= 4.7.0)
#if __cplusplus > 199711L
    template<typename Real_t, template<typename, typename...> class Cont, typename... Args>
    Cont<Real_t, Args...> generate_weights(const Real_t z, const Cont<Real_t, Args...>& x, const unsigned maxorder){
        Cont<Real_t, Args...> coeffs(x.size()*(maxorder+1));
        if (x.size() < maxorder + 1){
            throw std::logic_error("size of x insufficient");
        }
        populate_weights<Real_t>(z, &x[0], x.size() - 1, maxorder, &coeffs[0]);
        return coeffs;
    }
#endif
}



#endif /* FINITEDIFF_TEMPLATED_HPP_IWIDN4CP6RGFHFVKSWG4HICEZE */
