#include "rcs/circuit.hpp"
#include "rcs/simulator.hpp"
#include "rcs/generator.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>

void print_statevector(const std::vector<rcs::Complex>& sv) {
    std::cout << "Statevector (Index: Amplitude): [" << std::endl;
    for (size_t i = 0; i < sv.size(); ++i) {
        std::cout << "  " << std::setw(3) << i << ": " 
                  << std::fixed << std::setprecision(6) 
                  << "(" << std::setw(9) << sv[i].real() << ", " << std::setw(9) << sv[i].imag() << ")";
        if (i < sv.size() - 1) std::cout << ",";
        std::cout << std::endl;
    }
    std::cout << "]" << std::endl;
}

void print_probabilities(const std::vector<rcs::Complex>& sv) {
    std::cout << "Probabilities: [" << std::endl;
    double total_prob = 0.0;
    for (size_t i = 0; i < sv.size(); ++i) {
        double prob = std::norm(sv[i]);
        total_prob += prob;
        std::cout << "  " << std::fixed << std::setprecision(6) << prob;
        if (i < sv.size() - 1) std::cout << ",";
        std::cout << std::endl;
    }
    std::cout << "]" << std::endl;
    std::cout << "Total Probability: " << std::fixed << std::setprecision(10) << total_prob << std::endl;
}

int main(int argc, char* argv[]) {
    int num_qubits = 4;
    int depth = 5;

    if (argc > 1) num_qubits = std::atoi(argv[1]);
    if (argc > 2) depth = std::atoi(argv[2]);

    std::cout << "Generating Random Circuit Benchmark..." << std::endl;
    std::cout << "Qubits: " << num_qubits << ", Depth: " << depth << std::endl;

    // rcs::Generator gen(42); // Fixed seed for reproducibility
    rcs::Generator gen;
    rcs::Circuit circuit = gen.generate_random_circuit(num_qubits, depth);

    std::cout << "Circuit generated with " << circuit.gates().size() << " gates." << std::endl;

    rcs::Simulator sim(num_qubits);
    
    auto start = std::chrono::high_resolution_clock::now();
    sim.run(circuit);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> diff = end - start;
    std::cout << "Simulation completed in " << diff.count() << " seconds." << std::endl;

    if (num_qubits <= 4) {
        print_statevector(sim.get_statevector());
        std::cout << std::endl;
        print_probabilities(sim.get_statevector());
    }

    return 0;
}
