#!/bin/bash
set -euo pipefail

# ==============================================================================
# OpenX32 ADSP-21371 CCES Simulator Automated Test Runner (under Wine)
# ==============================================================================

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../../.." && pwd)"

CCES_DIR="${CCES_DIR:-$HOME/.wine/drive_c/Analog Devices/CrossCore Embedded Studio 2.12.1}"
CC21K="$CCES_DIR/cc21k.exe"
RUNNER="$CCES_DIR/CCES_runner.exe"

OUT_DIR="/tmp/openx32-cces-sim"
mkdir -p "$OUT_DIR"

TARGET_NAME="ADSP-2136x Family Simulator"
PLATFORM_NAME="ADSP-2136x Simulator"
PROCESSOR_NAME="ADSP-21371"

RUN_SMOKE=1
RUN_SUITE=1
INTENTIONAL_FAIL=0

for arg in "$@"; do
    case "$arg" in
        --smoke-only)
            RUN_SMOKE=1
            RUN_SUITE=0
            ;;
        --suite-only)
            RUN_SMOKE=0
            RUN_SUITE=1
            ;;
        --fail-smoke)
            RUN_SMOKE=1
            RUN_SUITE=0
            INTENTIONAL_FAIL=1
            ;;
        --clean)
            echo "Cleaning temporary simulator artifacts in $OUT_DIR..."
            rm -rf "$OUT_DIR"
            exit 0
            ;;
        -h|--help)
            echo "Usage: $0 [--smoke-only | --suite-only | --fail-smoke | --clean]"
            exit 0
            ;;
        *)
            echo "Unknown argument: $arg"
            exit 1
            ;;
    esac
done

if [ ! -f "$CC21K" ]; then
    echo "ERROR: cc21k.exe not found at '$CC21K'" >&2
    exit 1
fi

if [ ! -f "$RUNNER" ]; then
    echo "ERROR: CCES_runner.exe not found at '$RUNNER'" >&2
    exit 1
fi

to_wine_path() {
    local unix_path="$1"
    winepath -w "$unix_path"
}

run_dxe_in_sim() {
    local test_name="$1"
    local dxe_unix_path="$2"
    local expect_failure="${3:-0}"
    local log_file="$OUT_DIR/${test_name}_sim.log"

    local dxe_wine_path
    dxe_wine_path="$(to_wine_path "$dxe_unix_path")"

    echo ""
    echo ">>> Launching Simulator: $test_name"
    echo "    DXE: $dxe_wine_path"

    # Run CCES_runner under Wine with full logging and 10s safety timeout
    set +e
    (
        cd "$CCES_DIR"
        WINEDEBUG=-all wine "$RUNNER" \
            --target "$TARGET_NAME" \
            --platform "$PLATFORM_NAME" \
            --processor "$PROCESSOR_NAME" \
            -v2 \
            --timeout 10000 \
            "$dxe_wine_path"
    ) > "$log_file" 2>&1
    local runner_code=$?
    set -e

    # Print filtered test program output
    echo "--- Target Output [$test_name] ---"
    sed -n '/=== ADSP-21371/,/Closing session/p' "$log_file" | grep -vE "^(State changed|Breakpoint hit|1 cores|Finishes running|Closing session)" || cat "$log_file"
    echo "-----------------------------------"

    local hit_term=0
    local hit_fatal=0
    local hit_pass=0

    if grep -q "Breakpoint hit on core 0: ___lib_prog_term" "$log_file"; then
        hit_term=1
    fi

    if grep -q "Breakpoint hit on core 0: __fatal_error" "$log_file" || \
       grep -q "A non-recoverable error or exception has occurred" "$log_file" || \
       grep -q "RESULT: FAILED" "$log_file"; then
        hit_fatal=1
    fi

    if grep -q "RESULT: ALL PASS" "$log_file"; then
        hit_pass=1
    fi

    if [ "$expect_failure" -eq 1 ]; then
        if [ "$hit_fatal" -eq 1 ]; then
            echo "[PASS] Expected failure detected successfully by test harness for '$test_name'!"
            return 0
        else
            echo "[FAIL] Harness failed to detect expected failure for '$test_name'!" >&2
            return 1
        fi
    else
        if [ "$hit_fatal" -eq 1 ] || [ "$hit_pass" -eq 0 ] || [ "$hit_term" -eq 0 ]; then
            echo "[FAIL] Simulation test '$test_name' failed!" >&2
            echo "       hit_term=$hit_term, hit_fatal=$hit_fatal, hit_pass=$hit_pass, runner_code=$runner_code" >&2
            return 1
        else
            echo "[SUCCESS] Simulation test '$test_name' passed all assertions."
            return 0
        fi
    fi
}

echo "=============================================================================="
echo "OpenX32 ADSP-21371 Simulator Test Suite"
echo "CCES Path: $CCES_DIR"
echo "=============================================================================="

# ------------------------------------------------------------------------------
# Smoke Test Build & Execution
# ------------------------------------------------------------------------------
if [ "$RUN_SMOKE" -eq 1 ]; then
    SMOKE_DXE="$OUT_DIR/test_smoke.dxe"
    SMOKE_C="$SCRIPT_DIR/src/test_smoke.c"
    SMOKE_C_WINE="$(to_wine_path "$SMOKE_C")"
    SMOKE_DXE_WINE="$(to_wine_path "$SMOKE_DXE")"
    INC_WINE="$(to_wine_path "$SCRIPT_DIR/include")"

    COMPILE_DEFS=()
    EXPECT_FAIL=0
    if [ "$INTENTIONAL_FAIL" -eq 1 ]; then
        COMPILE_DEFS+=("-DTRIGGER_FAIL_ASSERT")
        EXPECT_FAIL=1
        echo "--> Compiling Smoke Test (WITH INTENTIONAL FAILURE ASSERTION)..."
    else
        echo "--> Compiling Smoke Test..."
    fi

    (
        cd "$CCES_DIR"
        WINEDEBUG=-all wine "$CC21K" \
            -proc "$PROCESSOR_NAME" \
            -O \
            "${COMPILE_DEFS[@]}" \
            -flags-link -MDUSE_SDRAM,-e \
            -I "$INC_WINE" \
            -o "$SMOKE_DXE_WINE" \
            "$SMOKE_C_WINE"
    )

    run_dxe_in_sim "smoke_test" "$SMOKE_DXE" "$EXPECT_FAIL"
fi

# ------------------------------------------------------------------------------
# Full Test Suite Build & Execution
# ------------------------------------------------------------------------------
if [ "$RUN_SUITE" -eq 1 ]; then
    SUITE_DXE="$OUT_DIR/test_suite.dxe"
    SUITE_CPP="$SCRIPT_DIR/src/test_suite.cpp"
    SUITE_CPP_WINE="$(to_wine_path "$SUITE_CPP")"
    SUITE_DXE_WINE="$(to_wine_path "$SUITE_DXE")"
    INC_WINE="$(to_wine_path "$SCRIPT_DIR/include")"
    DSP2_SRC_WINE="$(to_wine_path "$REPO_ROOT/dsp/dsp2/src")"

    BUFFER_CPP_WINE="$(to_wine_path "$REPO_ROOT/dsp/dsp2/src/buffer.cpp")"
    HELPER_CPP_WINE="$(to_wine_path "$REPO_ROOT/dsp/dsp2/src/helperFcn.cpp")"
    FAST_MATH_CPP_WINE="$(to_wine_path "$REPO_ROOT/dsp/dsp2/src/fastApproxMath.cpp")"

    echo "--> Compiling Full Simulator Test Suite..."
    (
        cd "$CCES_DIR"
        WINEDEBUG=-all wine "$CC21K" \
            -proc "$PROCESSOR_NAME" \
            -O \
            -flags-link -MDUSE_SDRAM,-e \
            -I "$INC_WINE" \
            -I "$DSP2_SRC_WINE" \
            -o "$SUITE_DXE_WINE" \
            "$SUITE_CPP_WINE" \
            "$BUFFER_CPP_WINE" \
            "$HELPER_CPP_WINE" \
            "$FAST_MATH_CPP_WINE"
    )

    run_dxe_in_sim "dsp_full_suite" "$SUITE_DXE" 0
fi

echo ""
echo "=============================================================================="
echo "ALL REQUESTED SIMULATOR TESTS COMPLETED SUCCESSFULLY"
echo "=============================================================================="
