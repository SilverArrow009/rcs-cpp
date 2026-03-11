#pragma once

#include <vector>
#include <string>
#include <complex>
#include <variant>

namespace rcs {

using Complex = std::complex<double>;

/**
 * @brief Types of gates supported by the simulator.
 */
enum class GateType {
    H, X, Y, Z, S, T, CX, RX, RY, RZ, CCNOT, FSIM
};

/**
 * @brief Represents a quantum gate operation.
 */
struct Gate {
    GateType type;
    std::vector<int> qubits; // Indices of qubits the gate acts on
    double theta = 0.0;      // Rotation angle or first parameter
    double phi = 0.0;        // Second parameter for gates like FSIM
};

/**
 * @brief A quantum circuit consisting of a series of gates.
 */
class Circuit {
public:
    Circuit(int num_qubits) : num_qubits_(num_qubits) {}

    void add_gate(GateType type, const std::vector<int>& qubits, double theta = 0.0, double phi = 0.0) {
        gates_.push_back({type, qubits, theta, phi});
    }

    int num_qubits() const { return num_qubits_; }
    const std::vector<Gate>& gates() const { return gates_; }

private:
    int num_qubits_;
    std::vector<Gate> gates_;
};

} // namespace rcs
