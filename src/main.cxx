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
    
    double avg = 100;
    double bias = 0.0;
    double std = 0.1;
    size_t N = 10000000;
    std::vector<double> vec_c(N);
    std::vector<double> vec_g(N);

    double time = 45;

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
    
    CallOption goog_option( stockptr, 60, 200 );
    auto [optMean, optStd] = goog_option.getPrice(0.001);
    std::cout << "option price: " << optMean << " (" << optStd << ")" << std::endl;


    return 0;
  }

}


int main(int argc, char* argv[]) {
  OP::main( argc, argv);
}
