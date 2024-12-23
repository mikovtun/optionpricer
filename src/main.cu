#include <iostream>
#include <chrono>
#include <device_properties.h>
#include <kern_cpu.h>
#include <kern_gpu.h>
#include <stock.h>
#include <stats.h>

namespace OP {

  GPUProperties* GPUProperties::instance = nullptr;

  int main(int argc, char* argv[]) {

    GPUProperties* gpuProps = GPUProperties::get();
    
    float avg = 50;
    float bias = 0.0;
    float std = 0.10;
    size_t N = 10000000;

    float time = 45;

    std::cout << "=== CPU ===" << std::endl;
    auto goog_c = LogNormalStock<Device::cpu>( avg, bias, std);
    
    auto vec_c = goog_c.getPrices(N, time);
    auto meanval_c = mean(vec_c.size(), vec_c.data());
    std::cout << "mean:   " <<  meanval_c << std::endl;
    std::cout << "stddev: " << stddev(vec_c.size(), vec_c.data(), meanval_c) << std::endl;
    

    std::cout << "=== GPU ===" << std::endl;

    auto goog_g = LogNormalStock<Device::gpu>( avg, bias, std);
    
    auto vec_g = goog_g.getPrices(N, time);
    auto meanval_g = mean(vec_g.size(), vec_g.data());
    std::cout << "mean:   " <<  meanval_g << std::endl;
    std::cout << "stddev: " << stddev(vec_g.size(), vec_g.data(), meanval_g) << std::endl;
    
    return 0;
  }

}


int main(int argc, char* argv[]) {
  OP::main( argc, argv);
}
