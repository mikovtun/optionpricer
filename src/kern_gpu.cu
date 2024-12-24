#include <iostream>
#include <random>

#include <cuda_runtime.h>
#include <curand_kernel.h>
#include <stock.h>
#include <device_properties.h>

namespace OP {


// Generates log(a + b*norm_rand)
__global__ void gen_prices(double* nums, size_t n, const double start, const double logMean, const double logStdDev, unsigned long seed) {
  size_t id = threadIdx.x + blockIdx.x * blockDim.x;

  if( id < n ) {
    curandState state;
    curand_init(seed, id, 0, &state);
    nums[id] = start * curand_log_normal_double(&state, logMean, logStdDev);
  }
}

__global__ void sumReduce(double* input, float* output) {
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
double LogNormalStock<Device::gpu>::getPrice(float u) {
  std::cout << "test" << std::endl;
  return 0.0;
}

template <>
void LogNormalStock<Device::gpu>::getPrices(size_t N, double* out, float u) {
  std::random_device rd;

  //const double vol2 = volatility*volatility;
  //const double logMean = (bias - vol2*0.5)*u;
  //const double logStdDev = vol2*u;
  const double vol2 = volatility*volatility;
  const double logMean = bias * u - 0.5*vol2*u;
  const double logStdDev = volatility*sqrt(u);

  auto gpuInstance = GPUProperties::get();
  auto gpuProps = gpuInstance->getProperties();

  size_t maxThreadsPerBlock = gpuProps.maxThreadsPerBlock;
  size_t blocksPerRun = (N + maxThreadsPerBlock - 1) / maxThreadsPerBlock;
  size_t freeMem = gpuInstance->getFreeMem();

  double* p_dev;
  cudaError_t error = cudaMalloc( &p_dev, N*sizeof(double));

  if( error != cudaSuccess) 
    std::cout << cudaGetErrorString(error) << std::endl;
  
  gen_prices<<<blocksPerRun, maxThreadsPerBlock>>>( p_dev, N, start, logMean, logStdDev, rd());

  error = cudaMemcpy( out, p_dev, N*sizeof(double), cudaMemcpyDeviceToHost);

  if( error != cudaSuccess) 
    std::cout << cudaGetErrorString(error) << std::endl;

  error = cudaFree( p_dev);

  if( error != cudaSuccess) 
    std::cout << cudaGetErrorString(error) << std::endl;
  
}


}
