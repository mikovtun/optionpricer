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

template <Device d>
void LogNormalStockDividend<d>::getPrices(size_t N, float* out, float u) {
  LogNormalStock<d>::getPrices(N, out, u);
  for(size_t i(0); i < N; i++) {
    out[i] *= std::exp( -1.0 * dividendRate * u );
  }
}

template <Device d>
void LogNormalStockDiscreteDividend<d>::getPrices(size_t N, float* out, float u) {
  LogNormalStock<d>::getPrices(N, out, u);
  int numDividendsPaid = std::floor((u - daysTillFirstDividend) / dividendInterval);
  if( numDividendsPaid > 0) {
    for(size_t i(0); i < N; i++) {
      out[i] *= std::pow(1.0 - dividendRate, numDividendsPaid);
    }
  }
}

  template void LogNormalStockDividend<Device::cpu>::getPrices(size_t N, float* out, float u);
  template void LogNormalStockDividend<Device::gpu>::getPrices(size_t N, float* out, float u);

  template void LogNormalStockDiscreteDividend<Device::cpu>::getPrices(size_t N, float* out, float u);
  template void LogNormalStockDiscreteDividend<Device::gpu>::getPrices(size_t N, float* out, float u);

}
