#include <iostream>
#include <chrono>
#include <device_properties.h>
#include <stats.h>
#include <stock.h>
#include <option.h>

namespace OP {

  GPUProperties* GPUProperties::instance = nullptr;

  int main(int argc, char* argv[]) {

    GPUProperties* gpuProps = GPUProperties::get();
    
    constexpr float tradingDaysInYear = 252;
    float avg = 100;
    float bias = 0.0;
    float std = 10;
    size_t N = 1000000;

    std::vector<float> vec_c(N);
    std::vector<float> vec_g(N);

    float time = 45;
    
    //std::cout << "=== CPU ===" << std::endl;
    auto goog_c = LogNormalStock<Device::cpu>( avg, bias, std / (100.0 * sqrt(tradingDaysInYear)));
    std::cout << "tgt mean: " << avg << std::endl;
    std::cout << "tgt std:  " << std<< std::endl;
    
    goog_c.getPrices(N, vec_c.data(), time);
    auto meanval_c = mean(vec_c.size(), vec_c.data());
    std::cout << "mean:   " <<  meanval_c << std::endl;
    std::cout << "stddev: " << stddev(vec_c.size(), vec_c.data(), meanval_c) / sqrt(time/tradingDaysInYear) << std::endl;
    

    //std::cout << "=== GPU ===" << std::endl;

    //auto goog_g = LogNormalStock<Device::gpu>( avg, bias, std);
    //
    //goog_g.getPrices(N, vec_g.data(), time);
    //auto meanval_g = mean(vec_g.size(), vec_g.data());
    //std::cout << "mean:   " <<  meanval_g << std::endl;
    //std::cout << "stddev: " << stddev(vec_g.size(), vec_g.data(), meanval_g) << std::endl;
    
     
    std::shared_ptr<Stock> stockptr = std::make_shared<LogNormalStock<Device::cpu>>(goog_c);
    OptionPosition blah( stockptr );
    blah.addCall(100, 45);
    //blah.addPut(140, 100);
    //blah.addShares(100);
    auto optMean = blah.getPrice(0.001);
    std::cout << "option price: " << optMean << std::endl;


    return 0;
  }

}


int main(int argc, char* argv[]) {
  OP::main( argc, argv);
}
