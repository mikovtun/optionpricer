#pragma once
#include <stock.h>
#include <algorithm>
#include <execution>
#include <random>

namespace OP {



template <>
float LogNormalStock<Device::cpu>::getPrice(float u) {
  std::random_device rd;
  std::mt19937 gen(rd());
  float price = log(start);
  //float logMean = (bias - volatility*volatility/2.0)*u;
  //float logStdDev = volatility*volatility*u;
  const float vol2 = volatility*volatility;
  const float logMean = log(start) + bias * u - 0.5*vol2*u;
  const float logStdDev = volatility*sqrt(u);
  std::lognormal_distribution<float> dist(logMean, logStdDev);
  return dist(gen);
}

template <>
std::vector<float> LogNormalStock<Device::cpu>::getPrices(size_t N, float u) {
  std::vector<float> prices(N);
  std::random_device rd;
  std::mt19937 gen(rd());
  //float logMean = (bias - volatility*volatility/2.0)*u;
  //float logStdDev = volatility*volatility*u;
  const float vol2 = volatility*volatility;
  const float logMean = log(start) + bias * u - 0.5*vol2*u;
  const float logStdDev = volatility*sqrt(u);
  std::lognormal_distribution<float> dist(logMean, logStdDev);
  for(size_t i(0); i<N; i++) {
    prices[i] = dist(gen);
  }
  return prices;



  //auto bound_function = std::bind(&LogNormalStock<Device::cpu>::getPrice, this, T, dT);
  //std::for_each(std::execution::par, prices.begin(), prices.end(),
  //    [&bound_function](float& value) {
  //      value = bound_function();
  //    });
  //return prices;
}


}
