#!/bin/bash
# build-dsp.sh — Reproducible Linux/Wine DSP build for OpenX32
# Builds DSP1 and/or DSP2 using CCES 2.12.1 headless tools under Wine.
# See DSP-BUILD.md for prerequisites and documentation.
set -euo pipefail

# ── Defaults ──────────────────────────────────────────────────────────────────
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"
DEFAULT_CCES_HOME="$HOME/.wine/drive_c/Analog Devices/CrossCore Embedded Studio 2.12.1"
CCES_HOME="${CCES_HOME:-$DEFAULT_CCES_HOME}"
OUTPUT_DIR=""
TARGETS=()

# ── Cleanup tracking ─────────────────────────────────────────────────────────
CLEANUP_DIRS=()
cleanup() {
    for d in "${CLEANUP_DIRS[@]}"; do
        rm -rf "$d" 2>/dev/null || true
    done
}
trap cleanup EXIT

# ── Usage ─────────────────────────────────────────────────────────────────────
usage() {
    cat <<'EOF'
Usage: build-dsp.sh [OPTIONS] [dsp1|dsp2|all]

Build DSP firmware reproducibly under Wine using CCES 2.12.1 headless tools.

Targets:
  dsp1          Build DSP1 only
  dsp2          Build DSP2 only
  all           Build both DSP1 and DSP2 (default)

Options:
  --output-dir=DIR   Output directory for artifacts (default: ./dsp-build-output)
  --help             Show this help

Environment:
  CCES_HOME          Path to CCES installation inside the Wine prefix
                     Default: ~/.wine/drive_c/Analog Devices/CrossCore Embedded Studio 2.12.1
EOF
    exit 0
}

# ── Argument parsing ──────────────────────────────────────────────────────────
for arg in "$@"; do
    case "$arg" in
        --output-dir=*) OUTPUT_DIR="${arg#*=}" ;;
        --help|-h)      usage ;;
        dsp1|dsp2|all)  TARGETS+=("$arg") ;;
        *)              echo "Error: unknown argument: $arg" >&2; usage ;;
    esac
done

# Default target: all
if [[ ${#TARGETS[@]} -eq 0 ]]; then
    TARGETS=(all)
fi

# Expand "all" to both targets
EXPANDED_TARGETS=()
for t in "${TARGETS[@]}"; do
    if [[ "$t" == "all" ]]; then
        EXPANDED_TARGETS+=(dsp1 dsp2)
    else
        EXPANDED_TARGETS+=("$t")
    fi
done

# Default output dir
if [[ -z "$OUTPUT_DIR" ]]; then
    OUTPUT_DIR="$SCRIPT_DIR/dsp-build-output"
fi
mkdir -p "$OUTPUT_DIR"
OUTPUT_DIR="$(cd "$OUTPUT_DIR" && pwd)"

# ── Prerequisite checks ──────────────────────────────────────────────────────
err=0

check_cmd() {
    if ! command -v "$1" &>/dev/null; then
        echo "Error: required command not found: $1" >&2
        err=1
    fi
}

check_file() {
    if [[ ! -f "$1" ]]; then
        echo "Error: required file not found: $1" >&2
        err=1
    fi
}

check_cmd wine
check_cmd sha256sum
check_file "$CCES_HOME/Eclipse/ccesc.exe"
check_file "$CCES_HOME/cc21k.exe"
check_file "$CCES_HOME/elfloader.exe"
check_file "$CCES_HOME/SHARC/ldr/371_spi.dxe"

for t in "${EXPANDED_TARGETS[@]}"; do
    check_file "$SCRIPT_DIR/$t/.cproject"
    check_file "$SCRIPT_DIR/$t/.project"
done

if [[ $err -ne 0 ]]; then
    echo "Aborting: missing prerequisites. See dsp/DSP-BUILD.md." >&2
    exit 1
fi

echo "CCES_HOME: $CCES_HOME"
echo "Output:    $OUTPUT_DIR"
echo "Targets:   ${EXPANDED_TARGETS[*]}"
echo ""

# ── Wine-prefix compatibility links (idempotent) ─────────────────────────────
WINE_C="$HOME/.wine/drive_c"

# DSP2 .cproject references C:\Programme2\AnalogDevices\CCES2
PROG2_DIR="$WINE_C/Programme2/AnalogDevices"
if [[ ! -e "$PROG2_DIR/CCES2" ]]; then
    echo "Creating compatibility symlink: Programme2/AnalogDevices/CCES2 → CCES install"
    mkdir -p "$PROG2_DIR"
    ln -s "$CCES_HOME" "$PROG2_DIR/CCES2"
fi

# CCES headless tools need System/SHARC → SHARC
SYSTEM_DIR="$CCES_HOME/System"
if [[ ! -e "$SYSTEM_DIR/SHARC" ]]; then
    echo "Creating compatibility symlink: System/SHARC → SHARC"
    ln -s "$CCES_HOME/SHARC" "$SYSTEM_DIR/SHARC"
fi

# ── increaseVersion.bat shim ─────────────────────────────────────────────────
# The .cproject pre-build step calls increaseVersion.bat to bump version.h.
# For reproducible builds we provide a no-op shim that copies version.h to
# version.tmp without modification.
#
# Wine's cmd.exe resolves batch files via the Windows PATH. The most reliable
# location is C:\windows (always on PATH). The shim is idempotent — it never
# modifies version.h — and is required for any CCES headless build.
SHIM_TARGET="$WINE_C/windows/increaseVersion.bat"
if [[ ! -f "$SHIM_TARGET" ]]; then
    echo "Installing increaseVersion.bat shim in Wine C:\\windows"
fi
# Always write the shim to ensure correct content
cat > "$SHIM_TARGET" <<'SHIM'
@echo off
rem No-op shim for reproducible builds: copies version.h to version.tmp without modification
if not "%2"=="" type %1 > %2 2>nul
exit /b 0
SHIM

# ── CCES workspace (temporary) ───────────────────────────────────────────────
WS_DIR="$(mktemp -d)"
CLEANUP_DIRS+=("$WS_DIR")

# ── Build function ────────────────────────────────────────────────────────────
build_target() {
    local target="$1"
    local project_dir="$SCRIPT_DIR/$target"
    local release_dir="$project_dir/Release"
    local ccesc="$CCES_HOME/Eclipse/ccesc.exe"
    local log_file="$OUTPUT_DIR/${target}_build.log"

    echo "═══════════════════════════════════════════════════════════════"
    echo "  Building $target (Release)"
    echo "═══════════════════════════════════════════════════════════════"

    # Fresh workspace for each target
    rm -rf "$WS_DIR"/* 2>/dev/null || true

    # Clean Release directory (preserve tracked CCES_MMV.exe)
    if [[ -d "$release_dir" ]]; then
        find "$release_dir" -mindepth 1 -not -name "CCES_MMV.exe" -delete 2>/dev/null || true
    fi

    # Run headless build.
    # DISPLAY="" prevents CCES_MMV.exe from opening a GUI window.
    # The post-build MMV step will fail (exit 1) but the makefile ignores it
    # with the - prefix. The map.xml is produced by the linker, not MMV.
    echo "Running CCES headless build..."
    DISPLAY="" wine "$ccesc" \
        -nosplash \
        -consoleLog \
        -application com.analog.crosscore.headlesstools \
        -data "Z:$WS_DIR" \
        -project "Z:$project_dir" \
        -cleanBuild Release \
        > "$log_file" 2>&1 || true

    # Wine may return nonzero even on success (stderr noise from fixme messages).
    # Check the build log for the authoritative pass/fail result.
    if ! grep -q "PASSED.*Succeeded to clean_build" "$log_file" 2>/dev/null; then
        echo "FAILED: $target build did not pass. See $log_file" >&2
        echo ""
        echo "--- Last 30 lines of build log ---"
        tail -30 "$log_file" >&2
        return 1
    fi

    # Verify expected artifacts exist
    local missing=0
    for f in "$target.dxe" "$target.ldr" "$target.map.xml" "linker_log.xml"; do
        if [[ ! -f "$release_dir/$f" ]]; then
            echo "FAILED: expected artifact missing: $release_dir/$f" >&2
            missing=1
        fi
    done
    if [[ $missing -ne 0 ]]; then
        return 1
    fi

    # Copy artifacts to output directory
    cp "$release_dir/$target.dxe"     "$OUTPUT_DIR/"
    cp "$release_dir/$target.ldr"     "$OUTPUT_DIR/"
    cp "$release_dir/$target.map.xml" "$OUTPUT_DIR/"
    cp "$release_dir/linker_log.xml"  "$OUTPUT_DIR/${target}_linker_log.xml"
    # Build log is already in output dir

    echo "✓ $target build succeeded"
    echo "  Artifacts: $target.dxe, $target.ldr, $target.map.xml, ${target}_linker_log.xml"
    echo ""
    return 0
}

# ── Run builds ────────────────────────────────────────────────────────────────
FAILED=()
for target in "${EXPANDED_TARGETS[@]}"; do
    if ! build_target "$target"; then
        FAILED+=("$target")
    fi
done

# ── Generate checksums ────────────────────────────────────────────────────────
echo "═══════════════════════════════════════════════════════════════"
echo "  Generating checksums"
echo "═══════════════════════════════════════════════════════════════"

(
    cd "$OUTPUT_DIR"
    # LDR files are fully deterministic (the firmware binary flashed to DSP).
    # DXE files contain a random link-project-guid from the SHARC linker
    # that changes every build, so they are excluded from the primary checksum.
    CHECKSUM_FILES=()
    for target in "${EXPANDED_TARGETS[@]}"; do
        if [[ -f "$target.ldr" ]]; then
            CHECKSUM_FILES+=("$target.ldr")
        fi
    done
    if [[ ${#CHECKSUM_FILES[@]} -gt 0 ]]; then
        sha256sum "${CHECKSUM_FILES[@]}" > sha256sums.txt
        echo "Checksums (deterministic firmware binaries):"
        cat sha256sums.txt
    fi

    # Full checksums for all artifacts (informational — timestamps will vary in logs)
    ALL_FILES=()
    for target in "${EXPANDED_TARGETS[@]}"; do
        for ext in dxe ldr map.xml; do
            [[ -f "$target.$ext" ]] && ALL_FILES+=("$target.$ext")
        done
        [[ -f "${target}_linker_log.xml" ]] && ALL_FILES+=("${target}_linker_log.xml")
        [[ -f "${target}_build.log" ]] && ALL_FILES+=("${target}_build.log")
    done
    if [[ ${#ALL_FILES[@]} -gt 0 ]]; then
        sha256sum "${ALL_FILES[@]}" > sha256sums_all.txt
    fi
)

echo ""

# ── Summary ───────────────────────────────────────────────────────────────────
if [[ ${#FAILED[@]} -gt 0 ]]; then
    echo "═══════════════════════════════════════════════════════════════"
    echo "  BUILD FAILED: ${FAILED[*]}"
    echo "═══════════════════════════════════════════════════════════════"
    exit 1
fi

echo "═══════════════════════════════════════════════════════════════"
echo "  All builds succeeded"
echo "  Output: $OUTPUT_DIR"
echo "═══════════════════════════════════════════════════════════════"
exit 0
