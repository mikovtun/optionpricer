#pragma once
#include <cmath>
#include <vector>

namespace OP {

enum class Device : size_t {
  cpu,
  gpu
};

class Stock {
  protected:
    Stock() = default;
  public:
    virtual ~Stock() = default;
    // Get N prices at time T
    virtual void getPrices(size_t N, float* out, float u) = 0;
};

template <Device d>
class LogNormalStock : public Stock {
  public:
    float bias, volatility, start;     
    LogNormalStock() = delete;
    LogNormalStock(float s, float b, float v):
     bias(b), volatility(v), start(s) {};
    
    float getPrice(float u);
    // Get N prices at time u (parallelized)
    void getPrices(size_t N, float* out, float u) override;
};

}
