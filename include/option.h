#pragma once
#include <stock.h>
#include <memory>

namespace OP {

class Option {
  protected:
    Option() = default;
  public:
    float strike, expiration;
    std::shared_ptr<Stock> underlying;
    // Non-destructively get option value from a list of realized stock prices
    virtual float getPriceKern(size_t N, const float* stockPrices) = 0;
    virtual ~Option() = default;
};
class CallOption : public Option {
  public:
    CallOption() = delete;
    CallOption(std::shared_ptr<Stock> und, float st, float exp) {
      this->underlying = und;
      this->strike = st;
      this->expiration = exp;
    }

  float getPriceKern(size_t N, const float* prices) override;
};
class PutOption : public Option {
  public:
    PutOption() = delete;
    PutOption(std::shared_ptr<Stock> und, float st, float exp) {
      this->underlying = und;
      this->strike = st;
      this->expiration = exp;
    }

  float getPriceKern(size_t N, const float* prices) override;
};


// OptionPosition = stock + options for one underlying
class OptionPosition {
  public:
    std::shared_ptr<Stock> underlying;
    int stockPosition = 0;

    std::vector<std::shared_ptr<Option>> options;


    OptionPosition() = delete;
    OptionPosition(std::shared_ptr<Stock> und): underlying(und) {};

    float getPrice(float accuracy = 0.01);
    
    void addShares(int num) {
      stockPosition = num;
    }
    void addCall(float str, float exp) { 
      std::shared_ptr<Option> op = std::make_shared<CallOption>( underlying, str, exp);
      options.emplace_back( op );
    }
    void addPut(float str, float exp) { 
      std::shared_ptr<Option> op = std::make_shared<PutOption>( underlying, str, exp);
      options.emplace_back( op );
    }
};
}
