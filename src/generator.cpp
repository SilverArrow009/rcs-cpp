#include "rcs/generator.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>

#ifndef CLIFFORD_RATIO
#define CLIFFORD_RATIO 0.5
#endif

namespace rcs {

Generator::Generator(unsigned int seed) : rng_(seed) {}

Circuit Generator::generate_random_circuit(int num_qubits, int depth) {
    Circuit circuit(num_qubits);
    
    std::vector<GateType> clifford_single = {
        GateType::H, GateType::X, GateType::Y, GateType::Z, GateType::S
    };
    
    std::vector<GateType> non_clifford_single = {
        GateType::T, GateType::RX, GateType::RY, GateType::RZ
    };

    std::uniform_real_distribution<double> angle_dist(0.0, 2.0 * M_PI);
    std::uniform_real_distribution<double> category_dist(0.0, 1.0);

    for (int d = 0; d < depth; ++d) {
        std::vector<int> available_qubits(num_qubits);
        std::iota(available_qubits.begin(), available_qubits.end(), 0);
        std::shuffle(available_qubits.begin(), available_qubits.end(), rng_);

        while (available_qubits.size() >= 1) {
            double cat_p = category_dist(rng_);
            bool use_clifford = (cat_p < CLIFFORD_RATIO);

            std::uniform_real_distribution<double> dist(0.0, 1.0);
            double p = dist(rng_);

            if (use_clifford) {
                // Clifford path
                if (p < 0.4 && available_qubits.size() >= 2) {
                    // CX is the primary Clifford multi-qubit gate
                    int q1 = available_qubits.back(); available_qubits.pop_back();
                    int q2 = available_qubits.back(); available_qubits.pop_back();
                    circuit.add_gate(GateType::CX, {q1, q2});
                } else {
                    int q = available_qubits.back(); available_qubits.pop_back();
                    std::uniform_int_distribution<int> gate_dist(0, clifford_single.size() - 1);
                    circuit.add_gate(clifford_single[gate_dist(rng_)], {q});
                }
            } else {
                // Non-Clifford path
                if (p < 0.2 && available_qubits.size() >= 3) {
                    // CCNOT
                    int q1 = available_qubits.back(); available_qubits.pop_back();
                    int q2 = available_qubits.back(); available_qubits.pop_back();
                    int q3 = available_qubits.back(); available_qubits.pop_back();
                    circuit.add_gate(GateType::CCNOT, {q1, q2, q3});
                } else if (p < 0.5 && available_qubits.size() >= 2) {
                    // FSIM
                    int q1 = available_qubits.back(); available_qubits.pop_back();
                    int q2 = available_qubits.back(); available_qubits.pop_back();
                    circuit.add_gate(GateType::FSIM, {q1, q2}, angle_dist(rng_), angle_dist(rng_));
                } else {
                    int q = available_qubits.back(); available_qubits.pop_back();
                    std::uniform_int_distribution<int> gate_dist(0, non_clifford_single.size() - 1);
                    GateType type = non_clifford_single[gate_dist(rng_)];
                    circuit.add_gate(type, {q}, angle_dist(rng_));
                }
            }
        }
    }

    return circuit;
}

} // namespace rcs