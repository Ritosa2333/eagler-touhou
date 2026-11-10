#include "../../src/native_easing.hpp"
#include <bit>
#include <cstdint>
#include <iostream>
int main() {
  int mode;
  uint32_t bits;
  while (std::cin >> mode >> bits)
    std::cout << std::bit_cast<uint32_t>(th12::native_easing::sample(std::bit_cast<float>(bits),mode)) << '\n';
}
