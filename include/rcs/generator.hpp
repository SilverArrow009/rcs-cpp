#pragma once

#include "circuit.hpp"
#include <random>

namespace rcs {

/**
 * @brief Generates random circuits for benchmarking.
 * 
 * The generator produces circuits with a mix of single-qubit and two-qubit gates,
 * structured in layers to maximize qubit utilization per depth level.
 */
class Generator {
public:
    /**
     * @brief Construct a new Generator object.
     * @param seed Optional seed for the random number generator.
     */
    explicit Generator(unsigned int seed = std::random_device{}());

    /**
     * @brief Generates a random circuit.
     * 
     * The algorithm attempts to fill each layer with as many gates as possible
     * without reusing qubits within the same layer.
     * 
     * @param num_qubits Number of qubits in the circuit.
     * @param depth Number of layers to generate.
     * @return A generated Circuit object.
     */
    Circuit generate_random_circuit(int num_qubits, int depth);

private:
    std::mt19937 rng_;
};

} // namespace rcs
