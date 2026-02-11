# RCS-CPP: Random Circuit Sampling Benchmark in C++

## Description
A minimal C++ implementation of Random Circuit Sampling (RCS) benchmarks. Parameterizable with number of qubits and depth of gates. This work is intended to serve as a minimal reference workload for architectural simulators like Gem5 to aid in the accelerator research for CPUs. The code is intended to be eventually published as a Gem5 workload.

## Features
- Random circuit generator with parameterizable qubit count and depth.
- Support for standard gates: H, X, Y, Z, S, T, and CX (CNOT).
- No external dependencies except the C++ standard library.

## Project Structure
- `include/rcs/`: Header files for the circuit, simulator, and generator.
- `src/`: Implementation of the core logic.
- `src/main.cpp`: Benchmark entry point.

## Building the Project
```bash
cmake -B ./build
make -C ./build
```

## Running the Benchmark
The benchmark takes two optional arguments: `num_qubits` and `circuit_depth`.
```bash
./build/rcs_benchmark <num_qubits> <circuit_depth>
```
