#include <iostream>
#include <execution>
#include <algorithm>
#include <random>
#include <stock.h>
#include <stats.h>
#include <vector>

#include "counter_based_engine.hpp"
#include "philox_prf.hpp"
namespace OP {

using eng_t = std::philox4x64;
using prf_t = eng_t::prf_type;

template <>
void LN<Device::cpu>::getLNModelPrices(size_t N, float* out, float u, float start, float bias, float volatility) {
  std::random_device rd;
  std::counter_based_engine<prf_t, 1> gen({2*N});
  gen.seed({rd()});

  //float logStdDev = volatility*volatility*u;
  const double vol2 = volatility*volatility;
  const double vol = volatility;
  const double logMean = bias * u - 0.5*vol2*u;
  const double logStdDev = vol*sqrt(u);

  // Generate N random numbers
  std::vector<size_t> rOut(2*N);
  gen(rOut.begin(), rOut.end());

  constexpr double two_pi = 2.0 * M_PI;

  for(size_t i(0); i < N; i++) {
    // size_t rand to [0,1] rand
    const double normed1 = static_cast<double>(rOut[2*i]) / static_cast<double>(std::numeric_limits<size_t>::max());
    const double normed2 = static_cast<double>(rOut[2*i+1]) / static_cast<double>(std::numeric_limits<size_t>::max());
    // Box-Muller transform from [0,1] to gaussian
    const double z    = std::sqrt(-2.0 * std::log(normed1))
                      * std::cos(two_pi * normed2);
    // gaussian to log-norm
    out[i] = start * std::exp(logMean + logStdDev * z);
  }
}

}
