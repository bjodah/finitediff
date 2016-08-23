#include "finitediff_templated.hpp"
#include <vector>
#include <string>
#include <iostream>

int main(){
    constexpr unsigned n = 5;
    const unsigned max_deriv = 2;
    std::vector<std::string> labels {"0th derivative (interpolation)", "1st derivative", "2nd derivative"};
    std::vector<double> x_optim { 0,  3, -3, 6, -6};
    std::vector<double> x_naive {-6, -3,  0, 3,  6};
    std::vector<double> mapping {4, 2, 0, 1, 3};
    auto c_optim = finitediff::generate_weights(x_optim, max_deriv);
    auto c_naive = finitediff::generate_weights(x_naive, max_deriv);
    for (unsigned deriv_i = 0; deriv_i <= max_deriv; deriv_i++){
        std::cout << labels[deriv_i] << ": ";
        for (unsigned idx = 0; idx < n; idx++){
            const unsigned idx_naive = deriv_i*n + idx;
            const unsigned idx_optim = deriv_i*n + mapping[idx];
            std::cout << c_naive[idx_naive] - c_optim[idx_optim] << " ";
        }
        std::cout << std::endl;
    }
}
