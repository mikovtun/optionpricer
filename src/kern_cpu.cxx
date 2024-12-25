#include <iostream>
#include <execution>
#include <algorithm>
#include <random>
#include <stock.h>
#include <stats.h>
#include <vector>

namespace OP {



template <>
void LogNormalStock<Device::cpu>::getPrices(size_t N, float* out, float u) {
  std::random_device rd;
  std::mt19937_64 gen(rd());
  //float logMean = (bias - volatility*volatility/2.0)*u;
  //float logStdDev = volatility*volatility*u;
  const float vol2 = volatility*volatility;
  const float logMean = bias * u - 0.5*vol2*u;
  const float logStdDev = volatility*sqrt(u);
  std::lognormal_distribution<float> dist(logMean, logStdDev);

  std::for_each(std::execution::par, out, out+N,
      [&](float& value) {
        value = start*dist(gen);
      });
}



}
