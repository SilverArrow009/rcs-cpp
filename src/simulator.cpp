#include "rcs/simulator.hpp"
#include "rcs/gates_internal.hpp"
#include <cmath>
#include <iostream>

namespace rcs {

Simulator::Simulator(int num_qubits) : num_qubits_(num_qubits) {
    reset();
}

void Simulator::reset() {
    size_t size = 1ULL << num_qubits_;
    state_.assign(size, Complex(0.0, 0.0));
    state_[0] = Complex(1.0, 0.0);
}

void Simulator::run(const Circuit& circuit) {
    for (const auto& gate : circuit.gates()) {
        apply_gate(gate);
    }
}

void Simulator::apply_gate(const Gate& gate) {
    switch (gate.type) {
        case GateType::H: apply_single_qubit_gate(gate.qubits[0], internal::gate_matrices[0]); break;
        case GateType::X: apply_single_qubit_gate(gate.qubits[0], internal::gate_matrices[1]); break;
        case GateType::Y: apply_single_qubit_gate(gate.qubits[0], internal::gate_matrices[2]); break;
        case GateType::Z: apply_single_qubit_gate(gate.qubits[0], internal::gate_matrices[3]); break;
        case GateType::S: apply_single_qubit_gate(gate.qubits[0], internal::gate_matrices[4]); break;
        case GateType::T: apply_single_qubit_gate(gate.qubits[0], internal::gate_matrices[5]); break;
        case GateType::CX: apply_cx(gate.qubits[0], gate.qubits[1]); break;
        case GateType::RX: apply_rx(gate.qubits[0], gate.theta); break;
        case GateType::RY: apply_ry(gate.qubits[0], gate.theta); break;
        case GateType::RZ: apply_rz(gate.qubits[0], gate.theta); break;
        case GateType::CCNOT: apply_ccnot(gate.qubits[0], gate.qubits[1], gate.qubits[2]); break;
        case GateType::FSIM: apply_fsim(gate.qubits[0], gate.qubits[1], gate.theta, gate.phi); break;
    }
}

void Simulator::apply_single_qubit_gate(int qubit, const std::complex<double> matrix[2][2]) {
    size_t size = 1ULL << num_qubits_;
    size_t spacing = 1ULL << qubit;

    // Single-qubit gate application:
    // We iterate over pairs of amplitudes (v0, v1) where v0 has the target bit as 0
    // and v1 has the target bit as 1.
    for (size_t i = 0; i < size; i += (spacing * 2)) {
        for (size_t j = 0; j < spacing; ++j) {
            size_t i0 = i + j;
            size_t i1 = i + j + spacing;

            Complex v0 = state_[i0];
            Complex v1 = state_[i1];

            // Apply the 2x2 unitary matrix
            state_[i0] = matrix[0][0] * v0 + matrix[0][1] * v1;
            state_[i1] = matrix[1][0] * v0 + matrix[1][1] * v1;
        }
    }
}

void Simulator::apply_cx(int control, int target) {
    size_t size = 1ULL << num_qubits_;
    size_t control_mask = 1ULL << control;
    size_t target_mask = 1ULL << target;

    // Controlled-X application:
    // If the control bit is set (1), we flip the target bit.
    // This is equivalent to swapping amplitudes state[...0...] and state[...1...]
    // for the target qubit's position, conditioned on the control qubit.
    for (size_t i = 0; i < size; ++i) {
        if ((i & control_mask) && !(i & target_mask)) {
            size_t i_target_set = i | target_mask;
            std::swap(state_[i], state_[i_target_set]);
        }
    }
}

void Simulator::apply_rx(int qubit, double theta) {
    Complex c = std::cos(theta / 2.0);
    Complex s = Complex(0.0, -std::sin(theta / 2.0));
    Complex matrix[2][2] = {
        {c, s},
        {s, c}
    };
    apply_single_qubit_gate(qubit, matrix);
}

void Simulator::apply_ry(int qubit, double theta) {
    double c = std::cos(theta / 2.0);
    double s = std::sin(theta / 2.0);
    Complex matrix[2][2] = {
        {c, -s},
        {s, c}
    };
    apply_single_qubit_gate(qubit, matrix);
}

void Simulator::apply_rz(int qubit, double theta) {
    Complex p = std::exp(Complex(0.0, -theta / 2.0));
    Complex m = std::exp(Complex(0.0, theta / 2.0));
    Complex matrix[2][2] = {
        {p, 0.0},
        {0.0, m}
    };
    apply_single_qubit_gate(qubit, matrix);
}

void Simulator::apply_ccnot(int control1, int control2, int target) {
    size_t size = 1ULL << num_qubits_;
    size_t c1_mask = 1ULL << control1;
    size_t c2_mask = 1ULL << control2;
    size_t target_mask = 1ULL << target;

    for (size_t i = 0; i < size; ++i) {
        if ((i & c1_mask) && (i & c2_mask) && !(i & target_mask)) {
            size_t i_target_set = i | target_mask;
            std::swap(state_[i], state_[i_target_set]);
        }
    }
}

void Simulator::apply_fsim(int q1, int q2, double theta, double phi) {
    size_t size = 1ULL << num_qubits_;
    size_t m1 = 1ULL << q1;
    size_t m2 = 1ULL << q2;

    Complex cos_t = std::cos(theta);
    Complex isin_t = Complex(0.0, -std::sin(theta));
    Complex phase_phi = std::exp(Complex(0.0, -phi));

    for (size_t i = 0; i < size; ++i) {
        if (!(i & m1) && (i & m2)) {
            size_t i01 = i;
            size_t i10 = (i ^ m2) | m1;
            
            Complex v01 = state_[i01];
            Complex v10 = state_[i10];
            
            state_[i01] = cos_t * v01 + isin_t * v10;
            state_[i10] = isin_t * v01 + cos_t * v10;
        } else if ((i & m1) && (i & m2)) {
            state_[i] *= phase_phi;
        }
    }
}

} // namespace rcs