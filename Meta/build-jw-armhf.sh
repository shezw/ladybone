#!/usr/bin/env bash

set -eo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# shellcheck source=/dev/null
. "${SCRIPT_DIR}/Utils/shell_include.sh"

ensure_ladybird_source_dir

set -u

# 本脚本用于交叉编译 JW/fbdev2d 版本的 Ladybone/Ladybird。
# 默认工具链来自用户本地的 arm-unknown-linux-gnueabihf 目录：
#   ~/dev/toolchain/arm-unknown-linux-gnueabihf
#
# 可覆盖的常用环境变量：
#   ARMHF_TOOLCHAIN_ROOT  交叉工具链根目录
#   JINGWEI_SOURCE_DIR    JW/JingWei 源码目录
#   BUILD_DIR             输出构建目录
#   PROXY_URL             下载代理，设为空字符串可禁用
#   CMAKE_BUILD_TYPE      CMake 构建类型
#   JOBS                  并行编译任务数

ARMHF_TARGET_TRIPLE="${ARMHF_TARGET_TRIPLE:-arm-unknown-linux-gnueabihf}"
RUST_TARGET_TRIPLE="${RUST_TARGET_TRIPLE:-armv7-unknown-linux-gnueabihf}"
VCPKG_TARGET_TRIPLET="${VCPKG_TARGET_TRIPLET:-arm-linux-gnueabihf-dynamic}"

ARMHF_TOOLCHAIN_ROOT="${ARMHF_TOOLCHAIN_ROOT:-${HOME}/dev/toolchain/${ARMHF_TARGET_TRIPLE}}"
JINGWEI_SOURCE_DIR="${JINGWEI_SOURCE_DIR:-$(cd "${LADYBIRD_SOURCE_DIR}/.." && pwd)/JingWei}"
BUILD_DIR="${BUILD_DIR:-${LADYBIRD_SOURCE_DIR}/Build/armhf-jw}"
VCPKG_ROOT="${VCPKG_ROOT:-${LADYBIRD_SOURCE_DIR}/Build/vcpkg}"
VCPKG_BINARY_CACHE="${VCPKG_BINARY_CACHE:-${LADYBIRD_SOURCE_DIR}/Build/caches/vcpkg-binary-cache}"
PROXY_URL="${PROXY_URL:-http://127.0.0.1:15236}"
CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE:-Release}"
JOBS="${JOBS:-$(get_number_of_processing_units)}"

# 当前工具链面向 armv7 hard-float。这里显式指定 CPU/FPU，避免 CMake/vcpkg 的子项目
# 回退到不匹配的默认 ABI。
ARMHF_CPU_FLAGS="${ARMHF_CPU_FLAGS:--mcpu=cortex-a9 -mfpu=neon-vfpv4 -mfloat-abi=hard}"

ARMHF_SYSROOT="${ARMHF_TOOLCHAIN_ROOT}/${ARMHF_TARGET_TRIPLE}/sysroot"
TOOLCHAIN_FILE="${BUILD_DIR}/cmake/armhf-toolchain.cmake"
TRIPLET_DIR="${BUILD_DIR}/cmake/vcpkg-triplets"
TRIPLET_FILE="${TRIPLET_DIR}/${VCPKG_TARGET_TRIPLET}.cmake"
LOG_DIR="${BUILD_DIR}/logs"

usage() {
    cat <<EOF
Usage: $(basename "$0") [configure|build|all|clean]

Commands:
  configure  生成交叉工具链文件、vcpkg triplet，并运行 CMake 配置
  build      使用已有配置进行编译
  all        先 configure 再 build，默认命令
  clean      删除当前 BUILD_DIR

Environment:
  ARMHF_TOOLCHAIN_ROOT=${ARMHF_TOOLCHAIN_ROOT}
  JINGWEI_SOURCE_DIR=${JINGWEI_SOURCE_DIR}
  BUILD_DIR=${BUILD_DIR}
  PROXY_URL=${PROXY_URL}
EOF
}

die_if_missing_file() {
    local path
    path="$1"

    if [ ! -e "$path" ]; then
        die "Missing required path: ${path}"
    fi
}

write_toolchain_files() {
    mkdir -p "${TRIPLET_DIR}" "${LOG_DIR}" "${VCPKG_BINARY_CACHE}"

    # CMake 交叉工具链文件只把 sysroot 作为 find root。
    # 不把 vcpkg installed 目录放进 CMAKE_FIND_ROOT_PATH，是为了避免污染 OpenSSL 等
    # vcpkg port 自己的安装过程。需要额外查找目标库的 port 由 overlay port 精确传参。
    cat > "${TOOLCHAIN_FILE}" <<EOF
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(ARMHF_TOOLCHAIN_ROOT "${ARMHF_TOOLCHAIN_ROOT}" CACHE PATH "armhf toolchain root")
set(ARMHF_TARGET_TRIPLE "${ARMHF_TARGET_TRIPLE}" CACHE STRING "armhf target triple")
set(ARMHF_SYSROOT "${ARMHF_SYSROOT}" CACHE PATH "armhf sysroot")

set(CMAKE_C_COMPILER "\${ARMHF_TOOLCHAIN_ROOT}/bin/\${ARMHF_TARGET_TRIPLE}-gcc")
set(CMAKE_CXX_COMPILER "\${ARMHF_TOOLCHAIN_ROOT}/bin/\${ARMHF_TARGET_TRIPLE}-g++")
set(CMAKE_AR "\${ARMHF_TOOLCHAIN_ROOT}/bin/\${ARMHF_TARGET_TRIPLE}-ar")
set(CMAKE_RANLIB "\${ARMHF_TOOLCHAIN_ROOT}/bin/\${ARMHF_TARGET_TRIPLE}-ranlib")
set(CMAKE_STRIP "\${ARMHF_TOOLCHAIN_ROOT}/bin/\${ARMHF_TARGET_TRIPLE}-strip")
set(CMAKE_LINKER "\${ARMHF_TOOLCHAIN_ROOT}/bin/\${ARMHF_TARGET_TRIPLE}-ld")
set(CMAKE_SYSROOT "\${ARMHF_SYSROOT}")

set(CMAKE_FIND_ROOT_PATH "\${ARMHF_SYSROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

set(CMAKE_C_FLAGS_INIT "${ARMHF_CPU_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${ARMHF_CPU_FLAGS}")
EOF

    # vcpkg triplet 负责把上面的交叉工具链转交给所有第三方 port。
    # 这里使用 dynamic linkage，目标是产出 .so 和可执行文件，便于后续做体积分析。
    cat > "${TRIPLET_FILE}" <<EOF
set(VCPKG_TARGET_ARCHITECTURE arm)
set(VCPKG_CMAKE_SYSTEM_NAME Linux)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE dynamic)
set(VCPKG_BUILD_TYPE release)
set(VCPKG_CHAINLOAD_TOOLCHAIN_FILE "${TOOLCHAIN_FILE}")
set(VCPKG_CMAKE_CONFIGURE_OPTIONS
    "-DCMAKE_C_FLAGS=${ARMHF_CPU_FLAGS}"
    "-DCMAKE_CXX_FLAGS=${ARMHF_CPU_FLAGS}"
)
EOF
}

export_common_environment() {
    export LADYBIRD_SOURCE_DIR
    export VCPKG_ROOT
    export VCPKG_BINARY_SOURCES="clear;files,${VCPKG_BINARY_CACHE},readwrite"
    export X_VCPKG_ASSET_SOURCES="clear;x-azurl,https://vcpkg-cache.app.ladybird.org/ladybird/source-assets,,read"
    export PKG_CONFIG_LIBDIR="${ARMHF_SYSROOT}/usr/lib/pkgconfig:${ARMHF_SYSROOT}/usr/share/pkgconfig"
    export PKG_CONFIG_SYSROOT_DIR="${ARMHF_SYSROOT}"

    # 网络下载默认走本地代理；如果 PROXY_URL 为空，则保留调用者已有环境。
    if [ -n "${PROXY_URL}" ]; then
        export HTTP_PROXY="${PROXY_URL}"
        export HTTPS_PROXY="${PROXY_URL}"
        export ALL_PROXY="${PROXY_URL}"
        export http_proxy="${PROXY_URL}"
        export https_proxy="${PROXY_URL}"
        export all_proxy="${PROXY_URL}"
    fi
}

ensure_rust_target() {
    if ! command -v rustup > /dev/null; then
        return
    fi

    # Rust 侧使用 armv7 hard-float target。未安装时自动补齐，避免 CMake 配置
    # 到 Rust crate 阶段才失败。
    if ! rustup target list --installed | grep -qx "${RUST_TARGET_TRIPLE}"; then
        rustup target add "${RUST_TARGET_TRIPLE}"
    fi
}

configure() {
    die_if_missing_file "${ARMHF_TOOLCHAIN_ROOT}/bin/${ARMHF_TARGET_TRIPLE}-gcc"
    die_if_missing_file "${ARMHF_SYSROOT}"
    die_if_missing_file "${JINGWEI_SOURCE_DIR}"
    die_if_missing_file "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"

    write_toolchain_files
    export_common_environment
    ensure_rust_target

    # JW 模式会在顶层 CMake 中强制关闭 3D/WebGL、HTTP3、DBus、AVIF/JXL、
    # WebRTC、MathML、SVG 等非当前目标能力。这里仍显式传入关键项，便于阅读日志。
    cmake -S "${LADYBIRD_SOURCE_DIR}" -B "${BUILD_DIR}" -G Ninja \
        -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}" \
        -DCMAKE_TOOLCHAIN_FILE="${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" \
        -DVCPKG_TARGET_TRIPLET="${VCPKG_TARGET_TRIPLET}" \
        -DVCPKG_OVERLAY_TRIPLETS="${TRIPLET_DIR}" \
        -DVCPKG_INSTALL_OPTIONS=--no-print-usage \
        -DLADYBIRD_GUI_FRAMEWORK=JW \
        -DJINGWEI_SOURCE_DIR="${JINGWEI_SOURCE_DIR}" \
        -DRUST_TARGET_TRIPLE="${RUST_TARGET_TRIPLE}" \
        -DBUILD_SHARED_LIBS=ON \
        -DLADYBIRD_ENABLE_3D_GRAPHICS=OFF \
        -DLADYBIRD_ENABLE_NETWORK_HTTP3=OFF \
        -DLADYBIRD_ENABLE_DESKTOP_DBUS=OFF \
        2>&1 | tee "${LOG_DIR}/configure.log"
}

build() {
    export_common_environment

    cmake --build "${BUILD_DIR}" --parallel "${JOBS}" \
        2>&1 | tee "${LOG_DIR}/build.log"
}

clean() {
    if [ -z "${BUILD_DIR}" ] || [ "${BUILD_DIR}" = "/" ]; then
        die "Refusing to remove unsafe BUILD_DIR: ${BUILD_DIR}"
    fi

    rm -rf "${BUILD_DIR}"
}

command="${1:-all}"

case "${command}" in
    configure)
        configure
        ;;
    build)
        build
        ;;
    all)
        configure
        build
        ;;
    clean)
        clean
        ;;
    help|--help|-h)
        usage
        ;;
    *)
        usage >&2
        exit 1
        ;;
esac
