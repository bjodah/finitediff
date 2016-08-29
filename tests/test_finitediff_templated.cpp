#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main()
#include "catch.hpp"
#include "finitediff_templated.hpp"
#include <vector>
#include <cmath>

template<typename T>
T inline abs_(T v) { return v < 0 ? -v : v; }

TEST_CASE( "weight correctness", "finitediff::generate_weights") {

    std::vector<double> x3 {-1, 0, 1};
    auto coeffs3 = finitediff::generate_weights(x3);
    REQUIRE( coeffs3.size() == 3*3);

    // Zeroth order
    REQUIRE( coeffs3[0] == 0);
    REQUIRE( coeffs3[1] == 1);
    REQUIRE( coeffs3[2] == 0);

    // First order
    REQUIRE( coeffs3[3] == -0.5);
    REQUIRE( coeffs3[4] == 0);
    REQUIRE( coeffs3[5] == 0.5);

    // Second order
    REQUIRE( coeffs3[6] == 1);
    REQUIRE( coeffs3[7] == -2);
    REQUIRE( coeffs3[8] == 1);


    std::vector<double> x5 {-2, -1, 0, 1, 2};
    auto coeffs5 = finitediff::generate_weights(x5, 2);
    REQUIRE( coeffs5.size() == 5*3);

    // Zeroth order
    REQUIRE( coeffs5[0] == 0);
    REQUIRE( coeffs5[1] == 0);
    REQUIRE( coeffs5[2] == 1);
    REQUIRE( coeffs5[3] == 0);
    REQUIRE( coeffs5[4] == 0);

    // First order
    REQUIRE( abs_(coeffs5[5] - 1/12.) < 1e-14 );
    REQUIRE( abs_(coeffs5[6] + 2/3.) < 1e-14 );
    REQUIRE( abs_(coeffs5[7] - 0) < 1e-14 );
    REQUIRE( abs_(coeffs5[8] - 2/3.) < 1e-14 );
    REQUIRE( abs_(coeffs5[9] + 1/12.) < 1e-14 );

    // Second order
    REQUIRE( abs_(coeffs5[10] + 1/12.) < 1e-14 );
    REQUIRE( abs_(coeffs5[11] - 4/3.) < 1e-14 );
    REQUIRE( abs_(coeffs5[12] + 5/2.) < 1e-14 );
    REQUIRE( abs_(coeffs5[13] - 4/3.) < 1e-14 );
    REQUIRE( abs_(coeffs5[14] + 1/12.) < 1e-14 );

}

TEST_CASE( "zero noise", "finitediff::generate_weights_optim") {

    constexpr unsigned n = 5;
    const unsigned max_deriv = 2;
    std::vector<std::string> labels {"0th derivative (interpolation)", "1st derivative", "2nd derivative"};
    std::vector<double> x_refer { 0,  3, -3, 6, -6};
    std::vector<double> x_naive {-6, -3,  0, 3,  6};
    std::vector<double> mapping {4, 2, 0, 1, 3};
    auto c_refer = finitediff::generate_weights(x_refer, max_deriv);
    auto c_naive = finitediff::generate_weights(x_naive, max_deriv);
    auto c_optim = finitediff::generate_weights_optim(x_naive, max_deriv);
    for (unsigned deriv_i = 0; deriv_i <= max_deriv; deriv_i++){
        for (unsigned idx = 0; idx < n; idx++){
            const unsigned idx_naive = deriv_i*n + idx;
            const unsigned idx_refer = deriv_i*n + mapping[idx];
            const double absdiff_naive = std::abs(c_naive[idx_naive] - c_refer[idx_refer]);
            const double absdiff_optim = std::abs(c_optim[idx_naive] - c_refer[idx_refer]);
            REQUIRE( absdiff_naive*1e16 < 1 );
            REQUIRE( absdiff_optim*1e17 < 1 );
        }
    }

}


TEST_CASE( "shifted", "finitediff::generate_weights_optim") {

    constexpr unsigned n = 7;
    std::vector<std::string> labels {"0th derivative (interpolation)", "1st derivative", "2nd derivative"};
    std::vector<double> x_refer {100,  103, 97, 113, 87, 170, 30};
    std::vector<double> x_naive {30, 87, 97, 100, 103, 113, 170};
    std::vector<double> mapping {6, 4, 2, 0, 1, 3, 5};
    auto c_refer = finitediff::generate_weights(x_refer, -1, 100.0);
    auto c_naive = finitediff::generate_weights(x_naive, -1, 100.0);
    auto c_optim = finitediff::generate_weights_optim(x_naive, -1, 100.0);
    const unsigned ncols = c_refer.size() / n;
    for (unsigned deriv_i = 0; deriv_i < ncols; deriv_i++){
        for (unsigned idx = 0; idx < n; idx++){
            const unsigned idx_naive = deriv_i*n + idx;
            const unsigned idx_refer = deriv_i*n + mapping[idx];
            const double absdiff_naive = std::abs(c_naive[idx_naive] - c_refer[idx_refer]);
            const double absdiff_optim = std::abs(c_optim[idx_naive] - c_refer[idx_refer]);
            REQUIRE( absdiff_naive*1e15 < 1 );
            REQUIRE( absdiff_optim*1e16 < 1 );
        }
    }

    std::vector<double> calc_naive(n*(n+3)/2);
    std::vector<double> calc_optim(n*(n+3)/2);
    finitediff::calculate_weights(&x_naive[0], n, (n+1)/2, &calc_naive[0], 100.0);
    finitediff::calculate_weights_optim(&x_naive[0], n, (n+1)/2, &calc_optim[0], 100.0);
    for (unsigned deriv_i = 0; deriv_i < ncols; deriv_i++){
        for (unsigned idx = 0; idx < n; idx++){
            const unsigned idx_naive = deriv_i*n + idx;
            const unsigned idx_refer = deriv_i*n + mapping[idx];
            const double absdiff_naive = std::abs(calc_naive[idx_naive] - c_refer[idx_refer]);
            const double absdiff_optim = std::abs(calc_optim[idx_naive] - c_refer[idx_refer]);
            REQUIRE( absdiff_naive*1e15 < 1 );
            REQUIRE( absdiff_optim*1e16 < 1 );
        }
    }
}

std::pair<std::vector<double>, std::vector<double>> get_ref_out_(std::vector<double> grid,
                                                                 const double x, const unsigned maxord, bool optim){
    std::vector<double> values(grid.size());
    std::vector<double> ref(maxord + 1);
    std::vector<double> out(maxord + 1);

    for (unsigned idx=0; idx < grid.size(); ++idx){
        values[idx] = grid[idx]*std::exp(-grid[idx]);
    }
    for (unsigned order=0; order <= maxord; ++order){
        ref[order] = std::pow(-1, order)*(x - order)*std::exp(-x);
    }
    if (optim)
        finitediff::apply_fd_optim(grid.size(), maxord, &grid[0], &values[0], x, &out[0]);
    else
        finitediff::apply_fd(grid.size(), maxord, &grid[0], &values[0], x, &out[0]);

    return std::pair<std::vector<double>, std::vector<double>>(ref, out);
}

void check_x_exp_mx_(const unsigned maxord, const double x, std::vector<double> grid, double lg_atol0, double degrade_factor, bool optim){
    auto ref_out = get_ref_out_(grid, x, maxord, optim);

    for (unsigned order=0; order <= maxord; ++order){
        const double atol = std::pow(10.0, lg_atol0 + degrade_factor*order);
        const double adiff = std::abs(ref_out.first[order] - ref_out.second[order]);
        REQUIRE( adiff < atol );
    }
}

TEST_CASE( "x_exp_mx", "finitediff::apply_fd" ) {
    check_x_exp_mx_(3, 1.0122333444455555, {0.8, 0.9, 1.0, 1.1, 1.2}, -7.0, 1.8, false);
    check_x_exp_mx_(5, 1.0122333444455555, {0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3}, -9.0, 1.4, false);
}
