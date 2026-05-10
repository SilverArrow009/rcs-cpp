cmake_minimum_required(VERSION 3.10)
project(rcs-cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set crosscompiling parameters
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv64)

# Specify the cross compiler
set(CMAKE_C_COMPILER riscv64-unknown-elf-gcc)
set(CMAKE_CXX_COMPILER riscv64-unknown-elf-g++)

# Include directories
include_directories(include)

# Compile-time options for benchmark hardness
set(CLIFFORD_RATIO 0.2 CACHE STRING "Proportion of Clifford gates (0.0 to 1.0)")
add_definitions(-DCLIFFORD_RATIO=${CLIFFORD_RATIO})

# Compile-time option for Vector Length (VLEN)
set(VLEN 1024 CACHE STRING "Target VLEN in bits (128, 256, 512, 1024, 2048, 4096)")
add_definitions(-DVLEN=${VLEN})

# Compile-time defines for the standard and complex vector extensions
option(USE_CMPLX_EXT "Use custom complex vector extensions" OFF)
option(NO_USE_CMPLX_EXT "Use standard RVV-1.0 for complex operations" OFF)

if(USE_CMPLX_EXT)
    add_definitions(-DUSE_CMPLX_EXT)
endif()

if(NO_USE_CMPLX_EXT)
    add_definitions(-DNO_USE_CMPLX_EXT)
endif()

if(USE_CMPLX_EXT OR NO_USE_CMPLX_EXT)
    # Ensure we use the RISC-V toolchain flags if needed
    add_compile_options(-march=rv64gcv)
endif()

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
