#pragma once
#include <complex>
#include <cmath>

namespace rcs {
namespace internal {

using Complex = std::complex<double>;

inline const Complex gate_matrices[6][2][2] = {
    // H
    {{{1.0/std::sqrt(2.0), 0.0}, {1.0/std::sqrt(2.0), 0.0}},
     {{1.0/std::sqrt(2.0), 0.0}, {-1.0/std::sqrt(2.0), 0.0}}},
    // X
    {{{0.0, 0.0}, {1.0, 0.0}},
     {{1.0, 0.0}, {0.0, 0.0}}},
    // Y
    {{{0.0, 0.0}, {0.0, -1.0}},
     {{0.0, 1.0}, {0.0, 0.0}}},
    // Z
    {{{1.0, 0.0}, {0.0, 0.0}},
     {{0.0, 0.0}, {-1.0, 0.0}}},
    // S
    {{{1.0, 0.0}, {0.0, 0.0}},
     {{0.0, 0.0}, {0.0, 1.0}}},
    // T
    {{{1.0, 0.0}, {0.0, 0.0}},
     {{0.0, 0.0}, {std::cos(M_PI/4.0), std::sin(M_PI/4.0)}}}
};

} // namespace internal
} // namespace rcs
