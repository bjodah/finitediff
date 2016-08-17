#include "finitediff_templated.hpp"
#include <vector>
#include <iostream>

int main(){
  std::vector<double> x {-1, 0, 1};
  auto coeffs = finitediff::generate_weights(0.0, x, 2);
  std::cout << "Zeroth order: " << coeffs[0] << " " << coeffs[1] << " " << coeffs[2] << std::endl;
  std::cout << "First order: "  << coeffs[3] << " " << coeffs[4] << " " << coeffs[5] << std::endl;
  std::cout << "Second order: " << coeffs[6] << " " << coeffs[7] << " " << coeffs[8] << std::endl;
}
