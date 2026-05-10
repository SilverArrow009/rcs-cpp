#include "rcs/simulator.hpp"
#include "rcs/gates_internal.hpp"
#include <cmath>

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

#if defined(USE_CMPLX_EXT) || defined(NO_USE_CMPLX_EXT)
#ifndef VLEN
#define VLEN 1024
#endif
    constexpr size_t max_vl = VLEN / 64;
    alignas(16) double m[max_vl + 8] = {
        matrix[0][0].real(), matrix[0][0].imag(),
        matrix[0][1].real(), matrix[0][1].imag(),
        matrix[1][0].real(), matrix[1][0].imag(),
        matrix[1][1].real(), matrix[1][1].imag()
    };

    for (size_t i = 0; i < size; i += (spacing * 2)) {
        size_t j = 0;
        while (j < spacing) {
            size_t vl_double;
            Complex* p0 = &state_[i + j];
            Complex* p1 = &state_[i + j + spacing];
            size_t count = (spacing - j) * 2;
            if (count > max_vl) count = max_vl;

#ifdef USE_CMPLX_EXT
            asm volatile(
                "vsetvli %[vl], %[count], e64, m1\n\t"
                "vle64.v v10, (%[m])\n\t"
                "addi t0, %[m], 16\n\t"
                "vle64.v v11, (t0)\n\t"
                "addi t0, %[m], 32\n\t"
                "vle64.v v12, (t0)\n\t"
                "addi t0, %[m], 48\n\t"
                "vle64.v v13, (t0)\n\t"
                
                "vid.v v14\n\t"
                "vand.vi v14, v14, 1\n\t"
                
                "vrgather.vv v20, v10, v14\n\t"
                "vrgather.vv v21, v11, v14\n\t"
                "vrgather.vv v22, v12, v14\n\t"
                "vrgather.vv v23, v13, v14\n\t"

                "vle64.v v1, (%[p0])\n\t"
                "vle64.v v2, (%[p1])\n\t"
                
                "vcfmul.vv v3, v1, v20\n\t"
                "vcfmacc.vv v3, v21, v2\n\t"
                
                "vcfmul.vv v4, v1, v22\n\t"
                "vcfmacc.vv v4, v23, v2\n\t"
                
                "vse64.v v3, (%[p0])\n\t"
                "vse64.v v4, (%[p1])\n\t"
                : [vl] "=r"(vl_double)
                : [count] "r"(count), [p0] "r"(p0), [p1] "r"(p1), [m] "r"(m)
                : "t0", "v1", "v2", "v3", "v4", "v10", "v11", "v12", "v13", "v14",
                  "v20", "v21", "v22", "v23", "memory"
            );
#else
            asm volatile(
                "vsetvli %[vl], %[count], e64, m1\n\t"
                "vle64.v v1, (%[p0])\n\t"
                "vle64.v v2, (%[p1])\n\t"
                
                "vid.v v3\n\t"
                "vand.vi v4, v3, 1\n\t"
                "vmseq.vi v0, v4, 0\n\t" 
                "vxor.vi v3, v3, 1\n\t"
                
                "vrgather.vv v4, v1, v3\n\t"
                "vrgather.vv v5, v2, v3\n\t"
                
                "vlse64.v v10, (%[m]), x0\n\t"
                "addi t0, %[m], 8\n\t"
                "vlse64.v v11, (t0), x0\n\t"
                "addi t0, %[m], 16\n\t"
                "vlse64.v v12, (t0), x0\n\t"
                "addi t0, %[m], 24\n\t"
                "vlse64.v v13, (t0), x0\n\t"
                "addi t0, %[m], 32\n\t"
                "vlse64.v v14, (t0), x0\n\t"
                "addi t0, %[m], 40\n\t"
                "vlse64.v v15, (t0), x0\n\t"
                "addi t0, %[m], 48\n\t"
                "vlse64.v v16, (t0), x0\n\t"
                "addi t0, %[m], 56\n\t"
                "vlse64.v v17, (t0), x0\n\t"
                
                "vfsgnjn.vv v11, v11, v11, v0.t\n\t"
                "vfsgnjn.vv v13, v13, v13, v0.t\n\t"
                "vfsgnjn.vv v15, v15, v15, v0.t\n\t"
                "vfsgnjn.vv v17, v17, v17, v0.t\n\t"
                
                "vfmul.vv v6, v1, v10\n\t"
                "vfmacc.vv v6, v4, v11\n\t"
                "vfmul.vv v7, v2, v12\n\t"
                "vfmacc.vv v7, v5, v13\n\t"
                "vfadd.vv v6, v6, v7\n\t"
                "vse64.v v6, (%[p0])\n\t"
                
                "vfmul.vv v8, v1, v14\n\t"
                "vfmacc.vv v8, v4, v15\n\t"
                "vfmul.vv v9, v2, v16\n\t"
                "vfmacc.vv v9, v5, v17\n\t"
                "vfadd.vv v8, v8, v9\n\t"
                "vse64.v v8, (%[p1])\n\t"
                : [vl] "=r"(vl_double)
                : [count] "r"(count), [p0] "r"(p0), [p1] "r"(p1), [m] "r"(m)
                : "t0", "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7", "v8", "v9",
                  "v10", "v11", "v12", "v13", "v14", "v15", "v16", "v17", "memory"
            );
#endif
            j += (vl_double / 2);
        }
    }
#else
    for (size_t i = 0; i < size; i += (spacing * 2)) {
        for (size_t j = 0; j < spacing; ++j) {
            size_t i0 = i + j;
            size_t i1 = i + j + spacing;
            Complex v0 = state_[i0];
            Complex v1 = state_[i1];
            state_[i0] = matrix[0][0] * v0 + matrix[0][1] * v1;
            state_[i1] = matrix[1][0] * v0 + matrix[1][1] * v1;
        }
    }
#endif
}

void Simulator::apply_cx(int control, int target) {
    size_t size = 1ULL << num_qubits_;
    size_t control_mask = 1ULL << control;
    size_t target_mask = 1ULL << target;
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
    Complex matrix[2][2] = {{c, s}, {s, c}};
    apply_single_qubit_gate(qubit, matrix);
}

void Simulator::apply_ry(int qubit, double theta) {
    double c = std::cos(theta / 2.0);
    double s = std::sin(theta / 2.0);
    Complex matrix[2][2] = {{c, -s}, {s, c}};
    apply_single_qubit_gate(qubit, matrix);
}

void Simulator::apply_rz(int qubit, double theta) {
    Complex p = std::exp(Complex(0.0, -theta / 2.0));
    Complex m = std::exp(Complex(0.0, theta / 2.0));
    Complex matrix[2][2] = {{p, 0.0}, {0.0, m}};
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
