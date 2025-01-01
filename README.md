This is a python library that calculates the price of European options using a Monte-Carlo approach... no Black-Scholes here!
This is nice because it can price options for stocks with variable or stochastic volatility.
Stock prices can be calculated on the CPU using a counter-based RNG for good spectral properties. The default mt19937 one is not robust enough.
The GPU RNG uses whatever the default is (XORWOW I think). Either way, the test suite compares CPU and GPU log normal means and standard deviations and it LGTM.

The only prerequisites are python, CUDA, and a C++ compiler (C++20 required). Python 3.10, CUDA 12.3, and gcc 11 work on my machine.
CMake will fetch pybind11 and cpp_counter_based_engine.

Before building, check your CUDA compute capability with `nvidia-smi --query-gpu=compute_cap --format=csv` and make sure it matches the `CUDA_ARCHITECTURES` variable in `CMakeLists.txt`.
Building is easy as pie... Make a `build` folder, cd into it, and `cmake ..`

Todo list:
* ARCH/GARCH stock models
* Greeks... or partial derivatives in general. You can do it manually in python though.
* GPU option pricing reduction kernels
* Compile-time option to disable GPU
* Automatic GPU tuning
* More robust python integration (not enough encapsulation as-is)
* Better cpp_counter_based_engine usage
* Better convergence checking
* Piecewise log normal stock with dividends

As a general disclaimer, I don't know what I'm doing, use this at your own risk, etc etc...
