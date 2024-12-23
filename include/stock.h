#pragma once
#include <cmath>
#include <vector>

namespace OP {

enum class Device : size_t {
  cpu,
  gpu
};

template <Device d>
class Stock {
  protected:
    Stock() = default;
  public:
    virtual ~Stock() = default;
    // Get N prices at time T
    std::vector<float> getPrices(size_t N, float u);

};

template <Device d>
class LogNormalStock {
  public:
    float bias, volatility, start;     
    LogNormalStock() = delete;
    LogNormalStock(float s, float b, float v): bias(b), volatility(v), start(s) {};
    
    float getPrice(float u);
    // Get N prices at time u (parallelized)
    std::vector<float> getPrices(size_t N, float u);
};

}
