#include "finitediff_templated.hpp"
#include <vector>
#include <string>
#include <iostream>

int main(){
    const unsigned max_deriv = 2;
    std::vector<std::string> labels {"0th derivative (interpolation)", "1st derivative", "2nd derivative"};
    std::vector<double> x {0, 1, -1, 2, -2};  // Fourth order of accuracy
    auto coeffs = finitediff::generate_weights(x, max_deriv);
    for (unsigned deriv_i = 0; deriv_i <= max_deriv; deriv_i++){
        std::cout << labels[deriv_i] << ": ";
        for (unsigned idx = 0; idx < x.size(); idx++){
            std::cout << coeffs[deriv_i*x.size() + idx] << " ";
        }
        std::cout << std::endl;
    }
}
