#pragma once
#include <cuda_runtime.h>
#include <curand_kernel.h>
#include <stock.h>
#include <device_properties.h>

namespace OP {


// Generates log(a + b*norm_rand)
__global__ void gen_prices(float* nums, size_t n, const float start, const float logMean, const float logStdDev, unsigned long seed) {
  size_t id = threadIdx.x + blockIdx.x * blockDim.x;

  if( id < n ) {
    curandState state;
    curand_init(seed, id, 0, &state);
    nums[id] = curand_log_normal(&state, logMean, logStdDev);
  }
}

__global__ void sumReduce(float* input, float* output) {
  size_t id = 2*threadIdx.x;
  for( size_t stride = 1; stride <= blockDim.x; stride *= 2) {
    if( threadIdx.x % stride == 0 )
      input[id] += input[id + stride];
    __syncthreads();
  }
  if(threadIdx.x == 0)
    *output = input[0];
}



template <>
float LogNormalStock<Device::gpu>::getPrice(float u) {
  std::cout << "test" << std::endl;
}

template <>
std::vector<float> LogNormalStock<Device::gpu>::getPrices(size_t N, float u) {
  std::vector<float> prices(N);
  std::random_device rd;

  auto p = prices.data();
  //const float vol2 = volatility*volatility;
  //const float logMean = (bias - vol2*0.5)*u;
  //const float logStdDev = vol2*u;
  const float vol2 = volatility*volatility;
  const float logMean = log(start) + bias * u - 0.5*vol2*u;
  const float logStdDev = volatility*sqrt(u);

  auto gpuInstance = GPUProperties::get();
  auto gpuProps = gpuInstance->getProperties();

  size_t maxThreadsPerBlock = gpuProps.maxThreadsPerBlock;
  size_t blocksPerRun = (N + maxThreadsPerBlock - 1) / maxThreadsPerBlock;
  size_t freeMem = gpuInstance->getFreeMem();

  float* p_dev;
  cudaError_t error = cudaMalloc( &p_dev, N*sizeof(float));

  if( error != cudaSuccess) 
    std::cout << cudaGetErrorString(error) << std::endl;
  
  gen_prices<<<blocksPerRun, maxThreadsPerBlock>>>( p_dev, N, start, logMean, logStdDev, rd());

  error = cudaMemcpy( p, p_dev, N*sizeof(float), cudaMemcpyDeviceToHost);

  if( error != cudaSuccess) 
    std::cout << cudaGetErrorString(error) << std::endl;

  error = cudaFree( p_dev);

  if( error != cudaSuccess) 
    std::cout << cudaGetErrorString(error) << std::endl;
  
  
  return prices;
  
}


}
