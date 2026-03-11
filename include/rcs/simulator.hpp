#pragma once

#include "circuit.hpp"
#include <vector>
#include <complex>

namespace rcs {

/**
 * @brief A statevector-based quantum simulator.
 * 
 * This simulator maintains a complex statevector representing the quantum state.
 * It uses LSB-first indexing to match Qiskit's convention, where qubit 0
 * corresponds to the least significant bit of the state index.
 */
class Simulator {
public:
    /**
     * @brief Construct a new Simulator object.
     * @param num_qubits The number of qubits in the system.
     */
    explicit Simulator(int num_qubits);

    /**
     * @brief Executes the given circuit and updates the internal state.
     * 
     * Iterates through all gates in the circuit and applies them sequentially
     * to the statevector.
     * 
     * @param circuit The circuit to simulate.
     */
    void run(const Circuit& circuit);

    /**
     * @brief Returns the current statevector.
     * @return A const reference to the vector of complex amplitudes.
     */
    const std::vector<Complex>& get_statevector() const { return state_; }

    /**
     * @brief Resets the statevector to the ground state |0...0>.
     */
    void reset();

private:
    /**
     * @brief Dispatches a gate to the appropriate application method.
     * @param gate The gate to apply.
     */
    void apply_gate(const Gate& gate);

    /**
     * @brief Applies a single-qubit gate defined by a 2x2 matrix.
     * @param qubit The index of the qubit to act on.
     * @param matrix The 2x2 unitary matrix representing the gate.
     */
    void apply_single_qubit_gate(int qubit, const std::complex<double> matrix[2][2]);

    /**
     * @brief Applies a Controlled-X (CNOT) gate.
     * @param control The index of the control qubit.
     * @param target The index of the target qubit.
     */
    void apply_cx(int control, int target);

    /**
     * @brief Applies a rotation around the X axis.
     */
    void apply_rx(int qubit, double theta);

    /**
     * @brief Applies a rotation around the Y axis.
     */
    void apply_ry(int qubit, double theta);

    /**
     * @brief Applies a rotation around the Z axis.
     */
    void apply_rz(int qubit, double theta);

    /**
     * @brief Applies a Toffoli (CCNOT) gate.
     */
    void apply_ccnot(int control1, int control2, int target);

    /**
     * @brief Applies an fSim (Fermionic simulation) gate.
     */
    void apply_fsim(int q1, int q2, double theta, double phi);

    int num_qubits_;
    std::vector<Complex> state_;
};

} // namespace rcs
