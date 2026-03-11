# RCS-CPP: Random Circuit Sampling Benchmark in C++

## Description
A minimal C++ implementation of Random Circuit Sampling (RCS) benchmarks. Parameterizable with number of qubits and depth of gates. This work is intended to serve as a minimal reference workload for architectural simulators like Gem5 to aid in the accelerator research for CPUs. The code is intended to be eventually published as a Gem5 workload.

## Features
- Statevector simulator with LSB-first qubit ordering (matching Qiskit).
- Random circuit generator with adjustable "hardness".
- Support for standard gates: H, X, Y, Z, S, T, CX, RX, RY, RZ, CCNOT, and **fSim**.
- No external dependencies except the C++ standard library.
- CMake-based build system with compile-time hardness configuration.

## Hardness Adjustment
You can adjust the ratio of Clifford gates to non-Clifford gates using a CMake variable. A lower ratio increases the simulation "hardness" by introducing more complex gates like T, CCNOT, and fSim.

```bash
# High hardness (10% Clifford, 90% Non-Clifford)
cmake -DCLIFFORD_RATIO=0.1 ..
make

# Purely Clifford (100% Clifford)
cmake -DCLIFFORD_RATIO=1.0 ..
make
```

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
