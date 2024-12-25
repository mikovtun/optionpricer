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
void LogNormalStock<Device::cpu>::getPrices(size_t N, float* out, float u) {
  std::random_device rd;
  std::counter_based_engine<prf_t, 1> gen({N});
  gen.seed({rd()});

  //float logMean = (bias - volatility*volatility/2.0)*u;
  //float logStdDev = volatility*volatility*u;
  const double vol2 = volatility*volatility;
  const double logMean = log(start) + bias * u - 0.5*vol2*u;
  const double logStdDev = volatility*sqrt(u);
  std::lognormal_distribution<float> dist(logMean, logStdDev);

  // Generate N random numbers
  std::vector<size_t> rOut(2*N);
  gen(rOut.begin(), rOut.end());

  constexpr double two_pi = 2.0 * M_PI;
  
  for(size_t i(0); i < N; i++) {
    double normed1 = static_cast<double>(rOut[2*i]) / static_cast<double>(std::numeric_limits<size_t>::max());
    double normed2 = static_cast<double>(rOut[2*i+1]) / static_cast<double>(std::numeric_limits<size_t>::max());

    // Box-Muller transform from [0,1] to gaussian
    auto z    = std::sqrt( -2.0f * std::log(normed1)) 
              * std::cos(two_pi * normed2);
    // gaussian to log-norm
    out[i] = std::exp( logMean + logStdDev * z);
  }
}



}
