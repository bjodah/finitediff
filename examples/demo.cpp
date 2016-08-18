#include "finitediff_templated.hpp"
#include <vector>
#include <string>
#include <iostream>

int main(){
    const unsigned maxorder = 2;
    std::vector<std::string> labels {"Zeroth order", "First order", "Second order"};
    std::vector<double> x {0, 1, -1, 2, -2};  // Fourth order of accuracy
    auto coeffs = finitediff::generate_weights(0.0, x, maxorder);
    for (unsigned order = 0; order <= maxorder; order++){
        std::cout << labels[order] << ": ";
        for (unsigned idx = 0; idx < x.size(); idx++){
            std::cout << coeffs[order*x.size() + idx] << " ";
        }
        std::cout << std::endl;
    }
}
