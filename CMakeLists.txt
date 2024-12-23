cmake_minimum_required(VERSION 3.18)
project(cool LANGUAGES CXX CUDA)

find_package(CUDAToolkit REQUIRED)
find_package(TBB REQUIRED)

# Set the C++ and CUDA standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 17)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

# Specify the source files
set(SOURCES
    src/main.cu
)

# Add the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link CUDA to the project
target_link_libraries(${PROJECT_NAME} PRIVATE CUDA::cudart)
# Link TBB
target_link_libraries(${PROJECT_NAME} PRIVATE TBB::tbb)

# Optional: Set specific CUDA properties
# Enable a specific compute capability (e.g., 7.5 for Turing GPUs)
set_target_properties(${PROJECT_NAME} PROPERTIES
    CUDA_ARCHITECTURES 75
)

# Optional: Add compile definitions or options
# For example, enable verbose compilation for CUDA
# target_compile_options(${PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:--verbose>)

# Include directories (if needed)
target_include_directories(${PROJECT_NAME} PRIVATE include)



set(CMAKE_BUILD_TYPE Debug)
