#include <iostream>
#include <chrono>
#include "device_properties.h"
#include "stats.h"
#include "stock.h"
#include "option.h"

#include <pybind11/pybind11.h>

namespace OP {

  GPUProperties* GPUProperties::instance = nullptr;

  int main(int argc, char* argv[]) {

    GPUProperties* gpuProps = GPUProperties::get();
    
    //constexpr float tradingDaysInYear = 252;
    constexpr float tradingDaysInYear = 365;
    float avg = 100;
    float bias = 0.0;
    float std = 25;
    size_t N = 1000000;

    float divvy_percent = 0.05;
    float divvy_interval = 21;
    float divvy_first_payment = 5;

    std::vector<float> vec_c(N);
    std::vector<float> vec_g(N);

    
    //std::cout << "=== CPU ===" << std::endl;
    auto goog_c = LogNormalStockDividend<Device::cpu>( avg, bias, std / (100.0 * sqrt(tradingDaysInYear)), divvy_percent / tradingDaysInYear);
    std::cout << "tgt mean: " << avg << std::endl;
    std::cout << "tgt std:  " << std<< std::endl;
    
    //goog_c.getPrices(N, vec_c.data(), time);
    //auto meanval_c = mean(vec_c.size(), vec_c.data());
    //std::cout << "mean:   " <<  meanval_c << std::endl;
    //std::cout << "stddev: " << stddev(vec_c.size(), vec_c.data(), meanval_c) / sqrt(time/tradingDaysInYear) << std::endl;
    

    //std::cout << "=== GPU ===" << std::endl;

    //auto goog_g = LogNormalStock<Device::gpu>( avg, bias, std);
    //
    //goog_g.getPrices(N, vec_g.data(), time);
    //auto meanval_g = mean(vec_g.size(), vec_g.data());
    //std::cout << "mean:   " <<  meanval_g << std::endl;
    //std::cout << "stddev: " << stddev(vec_g.size(), vec_g.data(), meanval_g) << std::endl;
    
     
    std::shared_ptr<Stock> stockptr = std::make_shared<LogNormalStockDividend<Device::cpu>>(goog_c);
    OptionPosition blah( stockptr );
    blah.longCall(100, 45);
    //blah.addPut(100, 365);
    //blah.addShares(100);
    //
    std::cout << "CALL VALUE: " << blah.getPrice(0.01) << std::endl;
    
    //float h = 0.1;
    //float optAcc = 0.0001;
    //stockptr->start -= h;
    //auto optMeanMinusH = blah.getPrice(optAcc);
    //stockptr->start += 2.0*h;
    //auto optMeanPlusH = blah.getPrice(optAcc);
    //stockptr->start -= h;
    //auto delta = (optMeanPlusH - optMeanMinusH) / (2.0*h);
    //std::cout << "option price: " << (optMeanPlusH + optMeanMinusH) / 2.0 << std::endl;
    //std::cout << "delta: " << delta << std::endl;


    return 0;
  }

}


int main(int argc, char* argv[]) {
  OP::main( argc, argv);
}
