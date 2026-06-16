#!/bin/bash
set -e

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
BUILD_DIR="${SCRIPT_DIR}/build"
OUTPUT_DIR="${SCRIPT_DIR}/../../bin"

if [ -n "$QT_QMAKE" ]; then
    QMAKE="$QT_QMAKE"
elif [ -x /opt/cross/bin/qmake ]; then
    QMAKE=/opt/cross/bin/qmake
elif [ -x /opt/cross/bin/qmake6 ]; then
    QMAKE=/opt/cross/bin/qmake6
elif [ -x /opt/cross/bin/qmake-qt5 ]; then
    QMAKE=/opt/cross/bin/qmake-qt5
else
    QMAKE=$(command -v qmake || true)
fi

if [ -z "$QMAKE" ]; then
    echo "qmake not found. Set QT_QMAKE to the target Qt qmake executable."
    exit 1
fi

mkdir -p "$BUILD_DIR" "$OUTPUT_DIR"
cd "$BUILD_DIR"

"$QMAKE" ../qtbench.pro
make -j"$(nproc)"
cp qtbench "$OUTPUT_DIR/qtbench"
