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
  const float vol2 = volatility*volatility;
  const float logMean = bias * u - 0.5*vol2*u;
  const float logStdDev = volatility*sqrt(u);
  std::lognormal_distribution<float> dist(logMean, logStdDev);
  


  for(size_t i(0); i < N; i++) {
    //std::counter_based_engine<prf_t, 1> gen{{seed, i}};
    //out[i] = dist(gen) * start;
    out[i] = dist(gen) * start;
  }

  //std::for_each(std::execution::par, out, out+N,
  //    [&](float& value) {
  //      value = start*dist(gen);
  //    });
}



}
