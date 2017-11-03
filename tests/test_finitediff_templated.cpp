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


std::pair<std::vector<double>, std::vector<double>> get_ref_out_(std::vector<double> grid,
                                                                 const double x, const unsigned maxord){
    std::vector<double> values(grid.size());
    std::vector<double> ref(maxord + 1);
    std::vector<double> out(maxord + 1);

    for (unsigned idx=0; idx < grid.size(); ++idx){
        values[idx] = grid[idx]*std::exp(-grid[idx]);
    }
    for (unsigned order=0; order <= maxord; ++order){
        ref[order] = std::pow(-1, order)*(x - order)*std::exp(-x);
    }
    finitediff::apply_fd(grid.size(), maxord, &grid[0], &values[0], x, &out[0]);

    return std::pair<std::vector<double>, std::vector<double>>(ref, out);
}

void check_x_exp_mx_(const unsigned maxord, const double x, std::vector<double> grid, double lg_atol0, double degrade_factor){
    auto ref_out = get_ref_out_(grid, x, maxord);

    for (unsigned order=0; order <= maxord; ++order){
        const double atol = std::pow(10.0, lg_atol0 + degrade_factor*order);
        const double adiff = std::abs(ref_out.first[order] - ref_out.second[order]);
        REQUIRE( adiff < atol );
    }
}

TEST_CASE( "x_exp_mx", "finitediff::apply_fd" ) {
    const double x = 1.0122333444455555;
    check_x_exp_mx_(2, 0, {-0.1, 0.0, 0.1}, -4.0, 1.8);
    check_x_exp_mx_(2, 0, {-0.1, 0.0, 0.1}, -4.0, 1.8);
    check_x_exp_mx_(2, x, {0.9, 1.0, 1.1}, -4.6, 1.8);
    check_x_exp_mx_(3, x, {0.8, 0.9, 1.0, 1.1, 1.2}, -7.0, 1.8);
    check_x_exp_mx_(5, x, {0.7, 0.8, 0.9, 1.0, 1.1, 1.2, 1.3}, -9.0, 1.4);
    check_x_exp_mx_(5, x, {0.5, 0.98, 0.99, 1.0, 1.2, 1.3, 1.4}, -9.3, 1.6);
    check_x_exp_mx_(5, x, {0.5, 0.0118, 1.0120, 1.0122, 1.2, 1.3, 1.4}, -9.4, 2.3);
}
