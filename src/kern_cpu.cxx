#include <iostream>
#include <execution>
#include <algorithm>
#include <random>
#include <stock.h>
#include <stats.h>
#include <vector>

namespace OP {



template <>
void LogNormalStock<Device::cpu>::getPrices(size_t N, double* out, float u) {
  std::random_device rd;
  std::mt19937_64 gen(rd());
  //double logMean = (bias - volatility*volatility/2.0)*u;
  //double logStdDev = volatility*volatility*u;
  const double vol2 = volatility*volatility;
  const double logMean = bias * u - 0.5*vol2*u;
  const double logStdDev = volatility*sqrt(u);
  std::lognormal_distribution<double> dist(logMean, logStdDev);

  std::for_each(std::execution::par, out, out+N,
      [&](double& value) {
        value = start*dist(gen);
      });
}



}
