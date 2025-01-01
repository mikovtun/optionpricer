#pragma once
#include <stock.h>
#include <memory>
#include <iostream>

namespace OP {

class Option {
  protected:
    Option(std::shared_ptr<Stock> und, float stk, float exp, bool shrt):
        underlying(und), strike(stk), expiration(exp), isShort(shrt) { };
    Option() = delete;
  public:
    bool isShort = false;
    float strike, expiration;
    std::shared_ptr<Stock> underlying;
    // Non-destructively get option value from a list of realized stock prices
    virtual float getPriceKern(size_t N, const float* stockPrices) = 0;
    virtual ~Option() = default;
};
class CallOption : public Option {
  public:
    CallOption() = delete;
    CallOption(std::shared_ptr<Stock> und, float st, float exp, bool sht): Option(und, st, exp, sht) {};

  float getPriceKern(size_t N, const float* prices) override;
};
class PutOption : public Option {
  public:
    PutOption() = delete;
    PutOption(std::shared_ptr<Stock> und, float st, float exp, bool sht): Option(und, st, exp, sht) {};

  float getPriceKern(size_t N, const float* prices) override;
};


// OptionPosition = stock + options for one underlying
class OptionPosition {
  public:
    std::shared_ptr<Stock> underlying;
    int stockPosition = 0;

    std::vector<std::shared_ptr<Option>> options;

    OptionPosition() = delete;
    OptionPosition(std::shared_ptr<Stock> und) { setUnderlying(und); }

    void setUnderlying(std::shared_ptr<Stock> und) {
      if( !und ) {
        std::cerr << "OptionPosition: Received nullptr stock" << std::endl;
        throw;
      }
      this->underlying = std::move(und);
      // Update options underlying too
      for( auto o : options )
        o->underlying = this->underlying;
    }


    float getPrice(float accuracy = 0.01, float time = -1.0);
    
    void longShares(size_t num) {
      stockPosition += num;
    }
    void shortShares(size_t num) {
      stockPosition -= num;
    }
    void setShares(int num) {
      stockPosition = num;
    }

    void longCall(float str, float exp) { 
      std::shared_ptr<Option> op = std::make_shared<CallOption>( underlying, str, exp, false);
      options.emplace_back( op );
    }
    void longPut(float str, float exp) { 
      std::shared_ptr<Option> op = std::make_shared<PutOption>( underlying, str, exp, false);
      options.emplace_back( op );
    }
    void shortCall(float str, float exp) { 
      std::shared_ptr<Option> op = std::make_shared<CallOption>( underlying, str, exp, true);
      options.emplace_back( op );
    }
    void shortPut(float str, float exp) { 
      std::shared_ptr<Option> op = std::make_shared<PutOption>( underlying, str, exp, true);
      options.emplace_back( op );
    }
    void clear() {
      stockPosition = 0;
      options.clear();
    }

};

}
