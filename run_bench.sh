#!/bin/bash
# run_bench.sh

# Paths to gem5 and configs (relative to project root)
GEM5_EXE="../gem5-cmac/build/RISCV/gem5.opt"
BASELINE_CONFIG="../gem5-cmac/configs/cvext/ara_baseline.py"
CUSTOM_CONFIG="../gem5-cmac/configs/cvext/ara_custom.py"
DEBUG_OPT="--debug-flags=Minor"

# Parameters
NUM_QUBITS="4 8 12 14"
DEPTH=8
VLENS="512 1024 2048 4096"

# Function to build and run
run_simulation() {
    local flavor=$1
    local define=$2
    local config=$3
    local vlen=$4
    local args=($5)
    local build_dir="build_${flavor}"

    echo "=== Building $flavor flavor ==="
    mkdir -p "$build_dir"
    cd "$build_dir"
    cmake -D"$define"=ON -DVLEN=$vlen ..
    make -j$(nproc)
    cd ..

    echo "=== Running $flavor simulation on gem5 ==="
    if [ -f "$GEM5_EXE" ]; then
        "$GEM5_EXE" --outdir="./m5out/${flavor}_${args[0]}_${vlen}" "$config" --vlen=$vlen --elen=64 "./$build_dir/rcs_benchmark" --bin-args "${args[0]} ${args[1]}"
    else
        echo "Error: gem5 binary not found at $GEM5_EXE"
    fi
}


for vlen in $VLENS; do
    for qubits in $NUM_QUBITS; do
        # Run Baseline
        run_simulation "baseline" "NO_USE_CMPLX_EXT" "$BASELINE_CONFIG" $vlen "${qubits} ${DEPTH}"
        # Run Custom
        run_simulation "custom" "USE_CMPLX_EXT" "$CUSTOM_CONFIG" $vlen "${qubits} ${DEPTH}"
    done
done
