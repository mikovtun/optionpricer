#pragma once
#include <stock.h>
#include <memory>

namespace OP {

class CallOption {
  public:
    std::shared_ptr<Stock> underlying;
    double strike, expiration;

    CallOption() = delete;
    CallOption(std::shared_ptr<Stock> und, double st, float exp) : 
      underlying(und), strike(st), expiration(exp) { };


  double getPriceKern(size_t N, double* prices);
  std::pair<double, double> getPrice(float accuracy = 100.0);

};

}
