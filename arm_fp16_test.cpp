/* Testing if my ARM device supports native fp16 data types.  
 **/


#include <iostream>
#include <chrono>
#include <vector>

void print_compile_time_support() {
  std::cout << "===1. Compile-Time Macros ===" << std::endl;
#if defined(__ARM_FEATURE_FP16_SCALAR_ARITHMETIC)
  std::cout << "__ARM_FEATURE_FP16_SCALAR_ARITHMETIC is defined." << std::endl;
#endif
}


int main() {
  print_compile_time_support();
}
