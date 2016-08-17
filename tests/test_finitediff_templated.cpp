#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main()
#include "catch.hpp"
#include "finitediff_templated.hpp"
#include <vector>

template<typename T>
T inline abs_(T v) { return v < 0 ? -v : v; }

TEST_CASE( "weight correctness", "finitediff::generate_weights") {

    std::vector<double> x3 {-1, 0, 1};
    auto coeffs3 = finitediff::generate_weights(0.0, x3, 2);
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
    auto coeffs5 = finitediff::generate_weights(0.0, x5, 2);
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
