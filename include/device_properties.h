#pragma once
#include <cuda_runtime.h>
namespace OP {
  class GPUProperties {
    private:
      static GPUProperties* instance;

      cudaDeviceProp deviceProp;

      GPUProperties() {
        int deviceCount = 0;
        cudaGetDeviceCount(&deviceCount);
        
        if( deviceCount == 0) {
          std::cout << "No CUDA devices found" << std::endl;
        } else {
          cudaGetDeviceProperties(&deviceProp, 0);
        }
      }
      
    public:
        GPUProperties(const GPUProperties&) = delete;
        GPUProperties& operator=(const GPUProperties&) = delete;

        static GPUProperties* get() {
          if( instance == nullptr ) {
            instance = new GPUProperties();
          }
          return instance;
        }

        cudaDeviceProp getProperties() {
          return deviceProp;
        }
        size_t getFreeMem() {
          size_t free, tot;
          cudaMemGetInfo( &free, &tot );
          return free;
        }
        void displayProperties() {
                std::cout << "GPU Name: " << deviceProp.name << std::endl;
                std::cout << "Total Global Memory: " << deviceProp.totalGlobalMem / 1024 / 1024 << " MB" << std::endl;
                std::cout << "Max Threads Per Block: " << deviceProp.maxThreadsPerBlock << std::endl;
                std::cout << "Max Block Dimensions: "
                          << deviceProp.maxThreadsDim[0] << "x"
                          << deviceProp.maxThreadsDim[1] << "x"
                          << deviceProp.maxThreadsDim[2] << std::endl;
            }
  };
}
