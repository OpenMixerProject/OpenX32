#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${ROOT_DIR}/build/xm32"
OUT_DIR="${ROOT_DIR}/build/output/xm32"
DOCKER_IMAGE="openx32-builder:trixie"
DOCKER_CONFIG_DIR="${BUILD_DIR}/docker-config"

if [[ -n "${DOCKER_HOST:-}" ]]; then
    DOCKER_HOST_URI="${DOCKER_HOST}"
elif [[ -S "${HOME}/.docker/run/docker.sock" ]]; then
    DOCKER_HOST_URI="unix://${HOME}/.docker/run/docker.sock"
else
    DOCKER_HOST_URI="unix:///var/run/docker.sock"
fi

mkdir -p "${BUILD_DIR}" "${OUT_DIR}" "${DOCKER_CONFIG_DIR}"

DOCKER_CONFIG="${DOCKER_CONFIG_DIR}" DOCKER_HOST="${DOCKER_HOST_URI}" DOCKER_BUILDKIT=0 \
docker build -t "${DOCKER_IMAGE}" - <<'DOCKERFILE'
FROM debian:trixie
ENV DEBIAN_FRONTEND=noninteractive
RUN dpkg --add-architecture armel \ 
        && apt-get update \
        && apt-get install -y --no-install-recommends \
            sudo \
            wget \
            git \
            libncurses-dev \
            gawk \
            flex \
            bison \
            openssl \
            libssl-dev \
            dkms \
            libelf-dev \
            libudev-dev \
            libpci-dev \
            libiberty-dev \
            autoconf \
            llvm \
            fakeroot \
            build-essential \
            devscripts \
            gcc-arm-none-eabi \
            binutils-arm-none-eabi \
            gcc-arm-linux-gnueabi \
            g++-arm-linux-gnueabi \
            binutils-arm-linux-gnueabi \
            u-boot-tools \
            bc \
            cpio \
            dropbear-bin \
            zlib1g-dev:armel libz-dev:armel libudev-dev:armel \
  && rm -rf /var/lib/apt/lists/*
DOCKERFILE

DOCKER_CONFIG="${DOCKER_CONFIG_DIR}" DOCKER_HOST="${DOCKER_HOST_URI}" \
docker run --rm \
  -i \
  -u "$(id -u):$(id -g)" \
  -v "${ROOT_DIR}:${ROOT_DIR}" \
  -w "${ROOT_DIR}" \
  -e ROOT_DIR="${ROOT_DIR}" \
  -e BUILD_DIR="${BUILD_DIR}" \
  "${DOCKER_IMAGE}" \
  bash -s <<'BUILD'

set -euo pipefail

# extract toolchain to directory ${BUILD_DIR}/opt/cross
tar -xf toolchains/cross-arm-arm926ej.tar.xz -C ${BUILD_DIR}/

# download, compile and install libz into ${BUILD_DIR}/opt/cross
cd /tmp
wget https://zlib.net/zlib-1.3.2.tar.gz
tar -xf zlib-1.3.2.tar.gz
rm zlib-1.3.2.tar.gz
cd zlib-1.3.2
export PATH=${BUILD_DIR}/opt/cross/bin:$PATH
export CHOST=arm-openwrt-linux-muslgnueabi
export CC=${CHOST}-gcc
export AR=${CHOST}-ar
export RANLIB=${CHOST}-ranlib
# install in the toolchain-folder
./configure --prefix=${BUILD_DIR}/opt/cross/arm-openwrt-linux-muslgnueabi --shared
make
make install

# build with musl
export PATH=${BUILD_DIR}/opt/cross/bin:$PATH

cd ${ROOT_DIR}

./compile-noterm.sh

BUILD
