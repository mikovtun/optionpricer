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
    
    float avg = 100;
    float bias = 0.0;
    float std = 0.1;
    size_t N = 10000000;
    std::vector<float> vec_c(N);
    std::vector<float> vec_g(N);

    float time = 45;

    std::cout << "=== CPU ===" << std::endl;
    auto goog_c = LogNormalStock<Device::cpu>( avg, bias, std);
    
    goog_c.getPrices(N, vec_c.data(), time);
    auto meanval_c = mean(vec_c.size(), vec_c.data());
    std::cout << "mean:   " <<  meanval_c << std::endl;
    std::cout << "stddev: " << stddev(vec_c.size(), vec_c.data(), meanval_c) << std::endl;
    

    std::cout << "=== GPU ===" << std::endl;

    auto goog_g = LogNormalStock<Device::gpu>( avg, bias, std);
    
    goog_g.getPrices(N, vec_g.data(), time);
    auto meanval_g = mean(vec_g.size(), vec_g.data());
    std::cout << "mean:   " <<  meanval_g << std::endl;
    std::cout << "stddev: " << stddev(vec_g.size(), vec_g.data(), meanval_g) << std::endl;
    
     
    std::shared_ptr<Stock> stockptr = std::make_shared<LogNormalStock<Device::cpu>>(goog_c);
    OptionPosition blah( stockptr );
    blah.buyCall(60, 100);
    blah.buyPut(90, 100);
    auto optMean = blah.getPrice(0.01);
    std::cout << "option price: " << optMean  << std::endl;


    return 0;
  }

}


int main(int argc, char* argv[]) {
  OP::main( argc, argv);
}
