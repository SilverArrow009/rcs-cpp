#include "rcs/generator.hpp"
#include <algorithm>
#include <numeric>

namespace rcs {

Generator::Generator(unsigned int seed) : rng_(seed) {}

Circuit Generator::generate_random_circuit(int num_qubits, int depth) {
    Circuit circuit(num_qubits);
    
    std::vector<GateType> single_qubit_gates = {
        GateType::H, GateType::X, GateType::Y, GateType::Z, GateType::S, GateType::T
    };

    for (int d = 0; d < depth; ++d) {
        std::vector<int> available_qubits(num_qubits);
        std::iota(available_qubits.begin(), available_qubits.end(), 0);
        std::shuffle(available_qubits.begin(), available_qubits.end(), rng_);

        while (available_qubits.size() >= 1) {
            std::uniform_real_distribution<double> dist(0.0, 1.0);
            double p = dist(rng_);

            if (p < 0.3 && available_qubits.size() >= 2) {
                // 30% chance for CX gate if at least 2 qubits available
                int q1 = available_qubits.back(); available_qubits.pop_back();
                int q2 = available_qubits.back(); available_qubits.pop_back();
                circuit.add_gate(GateType::CX, {q1, q2});
            } else {
                // Otherwise single qubit gate
                int q = available_qubits.back(); available_qubits.pop_back();
                std::uniform_int_distribution<int> gate_dist(0, single_qubit_gates.size() - 1);
                circuit.add_gate(single_qubit_gates[gate_dist(rng_)], {q});
            }
        }
    }

    return circuit;
}

} // namespace rcs
