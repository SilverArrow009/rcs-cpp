cmake_minimum_required(VERSION 3.10)
project(rcs-cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Compile-time options for benchmark hardness
set(CLIFFORD_RATIO 0.5 CACHE STRING "Proportion of Clifford gates (0.0 to 1.0)")
add_definitions(-DCLIFFORD_RATIO=${CLIFFORD_RATIO})

# Library source files
set(LIB_SOURCES
    src/circuit.cpp
    src/simulator.cpp
    src/generator.cpp
)

# Create library
add_library(rcs_lib ${LIB_SOURCES})

# Main executable
add_executable(rcs_benchmark src/main.cpp)
target_link_libraries(rcs_benchmark rcs_lib)
