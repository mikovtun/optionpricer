cmake_minimum_required(VERSION 3.18)
project(OptionPricer LANGUAGES CXX CUDA)
set(EXE_NAME    testsuite)    # name of the test executable
set(MODULE_NAME optionpricer) # name of the python module


set(CMAKE_BUILD_TYPE Release)


find_package(CUDAToolkit REQUIRED)
find_package(TBB REQUIRED)

# Set the C++ and CUDA standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

# Specify the source files
set(SOURCES
    src/kern_gpu.cu
    src/kern_cpu.cxx
    src/option.cxx
    src/main.cxx
    src/stock.cxx
    src/python.cxx
)
# Add the executable
add_executable(${EXE_NAME} ${SOURCES})

# Set to your CUDA_ARCHITECTURE
set_target_properties(${EXE_NAME} PROPERTIES
    CUDA_ARCHITECTURES 75
)

target_include_directories(${EXE_NAME} PRIVATE include)

# Find python
find_package(Python3 REQUIRED COMPONENTS Interpreter Development)

include(FetchContent)

# Get the counter-based RNG
# This will become part of the standard for C++26
FetchContent_Declare(
    cpp_counter_based_engine
    GIT_REPOSITORY https://github.com/johnsalmon/cpp-counter-based-engine.git
    GIT_TAG master
)
FetchContent_MakeAvailable(cpp_counter_based_engine)

# Get pybind11
FetchContent_Declare(
  pybind11
  GIT_REPOSITORY https://github.com/pybind/pybind11.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(pybind11)

# Includes
target_include_directories(${EXE_NAME} PRIVATE ${cpp_counter_based_engine_SOURCE_DIR})
target_include_directories(${EXE_NAME} PRIVATE ${pybind11_SOURCE_DIR}/include)
target_include_directories(${EXE_NAME} PRIVATE ${Python3_INCLUDE_DIRS})


# Links
target_link_libraries(${EXE_NAME} PRIVATE CUDA::cudart)
target_link_libraries(${EXE_NAME} PRIVATE TBB::tbb)
target_link_libraries(${EXE_NAME} PRIVATE ${Python3_LIBRARIES})


### Create a Python module using pybind11
pybind11_add_module(${MODULE_NAME} ${SOURCES})


target_include_directories(${MODULE_NAME} PRIVATE include)
target_include_directories(${MODULE_NAME} PRIVATE ${cpp_counter_based_engine_SOURCE_DIR})
target_include_directories(${MODULE_NAME} PRIVATE ${pybind11_SOURCE_DIR}/include)
target_include_directories(${MODULE_NAME} PRIVATE ${Python3_INCLUDE_DIRS})


# Links
target_link_libraries(${MODULE_NAME} PRIVATE CUDA::cudart)
target_link_libraries(${MODULE_NAME} PRIVATE TBB::tbb)
target_link_libraries(${MODULE_NAME} PRIVATE ${Python3_LIBRARIES})


