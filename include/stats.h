#pragma once
#include <cmath>

namespace OP {

  template <typename T>
  T mean(size_t N, T* ptr) {
    T accumulator = 0.0;
    for(size_t i(0); i<N; i++)
      accumulator += *(ptr + i);
    return accumulator/N;
  }

  template <typename T>
  T stddev(size_t N, T* ptr, T mean_input) {
    T accumulator = 0.0;
    for(size_t i(0); i<N; i++) {
      const auto tmp = (*(ptr + i) - mean_input);
      accumulator += tmp*tmp;
    }
    accumulator /= (N - 1);
    return sqrt(accumulator);
  }
}
