#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main()
#include "catch.hpp"
#include "finitediff_templated.hpp"
#include <vector>

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
