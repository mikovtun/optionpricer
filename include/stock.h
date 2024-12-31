#pragma once
#include "defines.h"
#include <cmath>
#include <vector>
#include <memory>
#include <iostream>

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
class LN : public Stock {
  public:
    float bias, volatility;
    LN() = delete;
    LN(const LN& ) = default;
    LN(LN&& ) = default;
    LN(float s, float b, float v):
     bias(b), volatility(v / (100.0 * std::sqrt(TRADING_DAYS_PER_ANNUM))) { this->start = s;};
    
    // Get N prices at time u (parallelized)
    static void getLNModelPrices(size_t N, float* out, float u, float start, float bias, float vol);
    virtual void getPrices(size_t N, float* out, float u) override;
    std::shared_ptr<Stock> pointer() {
      return std::make_shared<LN>(*this);
    }
};

// Zero dividend stock model
// with instantanteous rate of return (bias)
// Piecewise defined bias and volatility
template <Device d>
class LNPiecewise : public Stock {
  public:
    std::vector<float> bias;        // << List of biases, must have at least one entry
    std::vector<float> volatility;  // << List of volatilities, must have at least one entry
    std::vector<float> times;       // << List of time interval endpoints, must have at least zero entries

    LNPiecewise() = delete;
    LNPiecewise(const LNPiecewise& ) = default;
    LNPiecewise(LNPiecewise&& ) = default;
    LNPiecewise(float s, std::vector<float> b, std::vector<float> v, std::vector<float> t):
     bias(b), volatility(v), times(t) { 
       this->start = s;
       // Scale volatility
       for( int i(0); i < v.size(); i++ )
         v[i] /= 100.0 * std::sqrt(TRADING_DAYS_PER_ANNUM);

       // Check dimensions
      if( bias.size() != volatility.size() ) {
        std::cerr << "Bias and volatility vectors must be same size" << std::endl;
        throw;
      }
      if( bias.size() == 0 or volatility.size() == 0 ) {
        std::cerr << "Bias and volatility vectors must have at least one entry" << std::endl;
        throw;
      }
      if( bias.size() != (times.size() + 1) or volatility.size() != (times.size() + 1) ) {
        std::cerr << "Times vector must have one less entry than bias/volatility vectors" << std::endl;
        throw;
      }
    }
    
    // Get N prices at time u (parallelized)
    virtual void getPrices(size_t N, float* out, float u) override;
    std::shared_ptr<Stock> pointer() {
      return std::make_shared<LN>(*this);
    }
};

// The above, but with a continuous dividend payment
template <Device d>
class LNDividend : public LN<d> {
  public:
    float dividendYield;
    LNDividend() = delete;
    LNDividend(const LNDividend& ) = default;
    LNDividend(LNDividend&& ) = default;
    LNDividend(float s, float b, float v, float div):
     dividendYield(div / TRADING_DAYS_PER_ANNUM), LN<d>(s,b,v) { };
    
    // Get N prices at time u (parallelized)
    void getPrices(size_t N, float* out, float u) override;
    std::shared_ptr<Stock> pointer() {
      return std::make_shared<LNDividend>(*this);
    }
};


// The above, but with a continuous dividend payment
template <Device d>
class LNDiscreteDividend : public LN<d> {
  public:
    // How much each dividend pays as %, days until first payment, days between payments
    float dividendYield, daysTillFirstDividend, dividendInterval;
    LNDiscreteDividend() = delete;
    LNDiscreteDividend(const LNDiscreteDividend& ) = default;
    LNDiscreteDividend(LNDiscreteDividend&& ) = default;
    LNDiscreteDividend(float s, float b, float v, float divRate, float daysTillFirstDiv, float divInterval): dividendYield(divRate), daysTillFirstDividend(daysTillFirstDiv), dividendInterval(divInterval), 
     LN<d>(s,b,v) { };
    
    // Get N prices at time u (parallelized)
    void getPrices(size_t N, float* out, float u) override;
    std::shared_ptr<Stock> pointer() {
      return std::make_shared<LNDiscreteDividend>(*this);
    }
};

}
