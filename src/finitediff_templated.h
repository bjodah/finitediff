
template <typename T>
void populate_weights(const T z, const T * const __restrict__ x, const int nd,
                      const int m, T * const __restrict__ c) {
    // Parameters
    // ----------
    // z: location where approximations are to be accurate
    // x(0:nd): grid point locations, found in x(0:n)
    // nd: dimension of x- and c-arrays:
    // x(0:nd): grid locations
    // c(0:nd,0:m) derivatives weights of order 0:m
    // m: highest derivative.
    //
    // References
    // ----------
    // Generation of Finite Difference Formulas on Arbitrarily
    // Spaced Grids, Bengt Fornberg,
    // Mathematics of compuation, 51, 184, 1988, 699-706
    //
    // Notes
    // -----
    // If c is to be used repeatedly, consider transposing.
    T c1, c2, c3, c4, c5;
    c1 = (T)1;
    c4 = x[0]-z;
    for (int i=0; i<(nd+1)*(m+1); ++i)
        c[i] = (T)0;
    c[0] = (T)1;
    for (int i=1; i<nd+1; ++i){
        int mn = (i < m) ? i : m; // min(i, m)
        c2 = (T)1;
        c5 = c4;
        c4 = x[i] - z;
        for (int j=0; j<i; ++j){
            c3 = x[i] - x[j];
            c2 = c2*c3;
            if (j == i-1){
                for (int k=mn; k>=1; --k){
                    c[i*(nd+1) + k] = c1*(k*c[(i-1)*(nd+1) + k-1] - \
                                          c5*c[(i-1)*(nd+1) + k])/c2;
                }
                c[i*(nd+1) + 0] = -c1*c5*c[(i-1)*(nd+1) + 0]/c2;
            }
            for (int k=mn; k>=1; --k){
                c[j*(nd+1) + k] = (c4*c[j*(nd+1) + k] - \
                                   k*c[j*(nd+1) + k-1])/c3;
            }
            c[j*(nd+1) + 0] = c4*c[j*(nd+1) + 0]/c3;
        }
        c1 = c2;
    }
}

template <typename T>
void apply_fd(const int nin, const int maxorder, 
              const T * const __restrict__ xdata,
              const T * const __restrict__ ydata,
              const T xtgt,
              T * const __restrict__ out){
    T * const c = new T[nin*(maxorder+1)];
    populate_weights<T>(xtgt, xdata, nin-1, maxorder, c);
    for (int j=0; j<nin; ++j){
        out[j] = 0;
        for (int i=0; i<maxorder+1; ++i)
            out[j] += c[j*nin + i]*ydata[j];
    }
    delete []c;
}
