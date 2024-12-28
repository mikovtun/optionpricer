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
    float start;
    virtual ~Stock() = default;
    // Get N prices at time T
    virtual void getPrices(size_t N, float* out, float u) = 0;
};

// Constant volatility, no dividend stock model
// with instantanteous rate of return (bias)
template <Device d>
class LogNormalStock : public Stock {
  public:
    float bias, volatility;
    LogNormalStock() = delete;
    LogNormalStock(float s, float b, float v):
     bias(b), volatility(v) { this->start = s;};
    
    // Get N prices at time u (parallelized)
    virtual void getPrices(size_t N, float* out, float u) override;
};


// The above, but with a continuous dividend payment
template <Device d>
class LogNormalStockDividend : public LogNormalStock<d> {
  public:
    float dividendRate;
    LogNormalStockDividend() = delete;
    LogNormalStockDividend(float s, float b, float v, float div):
     dividendRate(div), LogNormalStock<d>(s,b,v) { };
    
    // Get N prices at time u (parallelized)
    void getPrices(size_t N, float* out, float u) override;
};


// The above, but with a continuous dividend payment
template <Device d>
class LogNormalStockDiscreteDividend : public LogNormalStock<d> {
  public:
    // How much each dividend pays as %, days until first payment, days between payments
    float dividendRate, daysTillFirstDividend, dividendInterval;
    LogNormalStockDiscreteDividend() = delete;
    LogNormalStockDiscreteDividend(float s, float b, float v, float divRate, float daysTillFirstDiv, float divInterval): dividendRate(divRate), daysTillFirstDividend(daysTillFirstDiv), dividendInterval(divInterval), 
     LogNormalStock<d>(s,b,v) { };
    
    // Get N prices at time u (parallelized)
    void getPrices(size_t N, float* out, float u) override;
};

}
