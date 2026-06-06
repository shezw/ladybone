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
#   CXX_STANDARD          主工程 C++ 标准，默认 23；可设为 17 做降级验证
#   JOBS                  并行编译任务数
#   ARMHF_BUILD_TESTING   是否构建 Tests/，默认 OFF；运行产物构建不需要测试可执行文件
#   FORCE_HOST_TOOLS      设为 1 时强制重建宿主机 LagomTools
#   SKIP_STDLIB_CHECK     设为 1 时跳过目标 C++ 标准库能力检查

ARMHF_TARGET_TRIPLE="${ARMHF_TARGET_TRIPLE:-arm-unknown-linux-gnueabihf}"
RUST_TARGET_TRIPLE="${RUST_TARGET_TRIPLE:-armv7-unknown-linux-gnueabihf}"
VCPKG_TARGET_TRIPLET="${VCPKG_TARGET_TRIPLET:-arm-linux-gnueabihf-dynamic}"

ARMHF_TOOLCHAIN_ROOT="${ARMHF_TOOLCHAIN_ROOT:-${HOME}/dev/toolchain/${ARMHF_TARGET_TRIPLE}}"
JINGWEI_SOURCE_DIR="${JINGWEI_SOURCE_DIR:-$(cd "${LADYBIRD_SOURCE_DIR}/.." && pwd)/JingWei}"
BUILD_DIR="${BUILD_DIR:-${LADYBIRD_SOURCE_DIR}/Build/armhf-jw}"
VCPKG_ROOT="${VCPKG_ROOT:-${LADYBIRD_SOURCE_DIR}/Build/vcpkg}"
VCPKG_INSTALLED_DIR="${VCPKG_INSTALLED_DIR:-${BUILD_DIR}/vcpkg_installed}"
VCPKG_BINARY_CACHE="${VCPKG_BINARY_CACHE:-${LADYBIRD_SOURCE_DIR}/Build/caches/vcpkg-binary-cache}"
PROXY_URL="${PROXY_URL:-http://127.0.0.1:15236}"
CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE:-Release}"
CXX_STANDARD="${CXX_STANDARD:-23}"
JOBS="${JOBS:-$(get_number_of_processing_units)}"
ARMHF_BUILD_TESTING="${ARMHF_BUILD_TESTING:-OFF}"
HOST_TOOLS_BUILD_DIR="${HOST_TOOLS_BUILD_DIR:-${LADYBIRD_SOURCE_DIR}/Build/host-tools-build}"
HOST_TOOLS_DIR="${HOST_TOOLS_DIR:-${LADYBIRD_SOURCE_DIR}/Build/host-tools}"
HOST_TOOLS_MANIFEST_DIR="${HOST_TOOLS_MANIFEST_DIR:-${LADYBIRD_SOURCE_DIR}/Build/host-tools-manifest}"
LAGOM_TOOLS_DIR="${LAGOM_TOOLS_DIR:-${HOST_TOOLS_DIR}/share/LagomTools}"
FORCE_HOST_TOOLS="${FORCE_HOST_TOOLS:-0}"
SKIP_STDLIB_CHECK="${SKIP_STDLIB_CHECK:-0}"

# 当前工具链面向 armv7 hard-float。这里显式指定 CPU/FPU，避免 CMake/vcpkg 的子项目
# 回退到不匹配的默认 ABI。
ARMHF_CPU_FLAGS="${ARMHF_CPU_FLAGS:--mcpu=cortex-a9 -mfpu=neon-vfpv4 -mfloat-abi=hard}"

ARMHF_SYSROOT="${ARMHF_TOOLCHAIN_ROOT}/${ARMHF_TARGET_TRIPLE}/sysroot"
TOOLCHAIN_FILE="${BUILD_DIR}/cmake/armhf-toolchain.cmake"
MAIN_TOOLCHAIN_FILE="${BUILD_DIR}/cmake/armhf-main-toolchain.cmake"
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
  HOST_TOOLS_DIR=${HOST_TOOLS_DIR}
  PROXY_URL=${PROXY_URL}
  CXX_STANDARD=${CXX_STANDARD}
  ARMHF_BUILD_TESTING=${ARMHF_BUILD_TESTING}
EOF
}

die_if_missing_file() {
    local path
    path="$1"

    if [ ! -e "$path" ]; then
        die "Missing required path: ${path}"
    fi
}

export_proxy_environment() {
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

write_toolchain_files() {
    mkdir -p "${TRIPLET_DIR}" "${LOG_DIR}" "${VCPKG_BINARY_CACHE}"

    # vcpkg port 构建使用工具链自带 GCC。第三方包通常不要求 C++23，
    # 保持这里不变可以复用已经构建好的 vcpkg binary cache。
    #
    # CMake 交叉工具链文件只把 sysroot 作为 find root。
    # 不把 vcpkg installed 目录放进 CMAKE_FIND_ROOT_PATH，是为了避免污染 OpenSSL 等
    # vcpkg port 自己的安装过程。头文件和库仍然只从 sysroot/root path 查找；
    # package config 使用 BOTH，这样主工程可以找到 vcpkg_installed 里的
    # mimalloc-config.cmake、unofficial-skia-config.cmake 等配置文件。
    cat > "${TOOLCHAIN_FILE}" <<EOF
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(ARMHF_TOOLCHAIN_ROOT "${ARMHF_TOOLCHAIN_ROOT}" CACHE PATH "armhf toolchain root")
set(ARMHF_TARGET_TRIPLE "${ARMHF_TARGET_TRIPLE}" CACHE STRING "armhf target triple")
set(ARMHF_SYSROOT "${ARMHF_SYSROOT}" CACHE PATH "armhf sysroot")

set(CMAKE_C_COMPILER "\${ARMHF_TOOLCHAIN_ROOT}/bin/\${ARMHF_TARGET_TRIPLE}-gcc")
set(CMAKE_CXX_COMPILER "\${ARMHF_TOOLCHAIN_ROOT}/bin/\${ARMHF_TARGET_TRIPLE}-g++")
set(CMAKE_AR "\${ARMHF_TOOLCHAIN_ROOT}/bin/\${ARMHF_TARGET_TRIPLE}-ar" CACHE FILEPATH "armhf archiver" FORCE)
set(CMAKE_RANLIB "\${ARMHF_TOOLCHAIN_ROOT}/bin/\${ARMHF_TARGET_TRIPLE}-ranlib" CACHE FILEPATH "armhf ranlib" FORCE)
set(CMAKE_C_COMPILER_AR "\${CMAKE_AR}" CACHE FILEPATH "armhf C compiler archiver" FORCE)
set(CMAKE_CXX_COMPILER_AR "\${CMAKE_AR}" CACHE FILEPATH "armhf CXX compiler archiver" FORCE)
set(CMAKE_C_COMPILER_RANLIB "\${CMAKE_RANLIB}" CACHE FILEPATH "armhf C compiler ranlib" FORCE)
set(CMAKE_CXX_COMPILER_RANLIB "\${CMAKE_RANLIB}" CACHE FILEPATH "armhf CXX compiler ranlib" FORCE)
set(CMAKE_STRIP "\${ARMHF_TOOLCHAIN_ROOT}/bin/\${ARMHF_TARGET_TRIPLE}-strip")
set(CMAKE_LINKER "\${ARMHF_TOOLCHAIN_ROOT}/bin/\${ARMHF_TARGET_TRIPLE}-ld")
set(CMAKE_SYSROOT "\${ARMHF_SYSROOT}")

set(CMAKE_FIND_ROOT_PATH "\${ARMHF_SYSROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)

set(CMAKE_C_FLAGS_INIT "${ARMHF_CPU_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${ARMHF_CPU_FLAGS}")
EOF

    # 主工程使用宿主机 clang，并通过 --target/--gcc-toolchain/sysroot
    # 指向同一套 arm-unknown-linux-gnueabihf 运行时与链接工具。
    cat > "${MAIN_TOOLCHAIN_FILE}" <<EOF
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(ARMHF_TOOLCHAIN_ROOT "${ARMHF_TOOLCHAIN_ROOT}" CACHE PATH "armhf toolchain root")
set(ARMHF_TARGET_TRIPLE "${ARMHF_TARGET_TRIPLE}" CACHE STRING "armhf target triple")
set(ARMHF_SYSROOT "${ARMHF_SYSROOT}" CACHE PATH "armhf sysroot")

set(CMAKE_C_COMPILER /usr/bin/clang)
set(CMAKE_CXX_COMPILER /usr/bin/clang++)
set(CMAKE_C_COMPILER_TARGET "\${ARMHF_TARGET_TRIPLE}")
set(CMAKE_CXX_COMPILER_TARGET "\${ARMHF_TARGET_TRIPLE}")
set(CMAKE_C_COMPILER_EXTERNAL_TOOLCHAIN "\${ARMHF_TOOLCHAIN_ROOT}")
set(CMAKE_CXX_COMPILER_EXTERNAL_TOOLCHAIN "\${ARMHF_TOOLCHAIN_ROOT}")
set(CMAKE_AR "\${ARMHF_TOOLCHAIN_ROOT}/bin/\${ARMHF_TARGET_TRIPLE}-ar")
set(CMAKE_RANLIB "\${ARMHF_TOOLCHAIN_ROOT}/bin/\${ARMHF_TARGET_TRIPLE}-ranlib")
set(CMAKE_STRIP "\${ARMHF_TOOLCHAIN_ROOT}/bin/\${ARMHF_TARGET_TRIPLE}-strip")
set(CMAKE_LINKER "\${ARMHF_TOOLCHAIN_ROOT}/bin/\${ARMHF_TARGET_TRIPLE}-ld")
set(CMAKE_SYSROOT "\${ARMHF_SYSROOT}")

set(CMAKE_FIND_ROOT_PATH "\${ARMHF_SYSROOT}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)

set(CMAKE_C_FLAGS_INIT "${ARMHF_CPU_FLAGS}")
set(CMAKE_CXX_FLAGS_INIT "${ARMHF_CPU_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_INIT "-fuse-ld=\${ARMHF_TOOLCHAIN_ROOT}/bin/\${ARMHF_TARGET_TRIPLE}-ld")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "-fuse-ld=\${ARMHF_TOOLCHAIN_ROOT}/bin/\${ARMHF_TARGET_TRIPLE}-ld")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "-fuse-ld=\${ARMHF_TOOLCHAIN_ROOT}/bin/\${ARMHF_TARGET_TRIPLE}-ld")
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

write_host_tools_manifest() {
    mkdir -p "${HOST_TOOLS_MANIFEST_DIR}"

    # 宿主机代码生成器只需要 AK/LibCore/LibUnicode/LibIDL/LibMain 这一小段依赖。
    # 不复用源码根目录的完整 vcpkg.json，否则 host-tools 会安装 Skia、ANGLE、FFmpeg、
    # Curl/OpenSSL、图片解码库等浏览器运行时依赖。
    cat > "${HOST_TOOLS_MANIFEST_DIR}/vcpkg.json" <<EOF
{
  "builtin-baseline": "48cfe1e0e928341709d97fc3d2eff10ad6262c96",
  "dependencies": [
    "fast-float",
    "fmt",
    "icu",
    "mimalloc",
    "simdutf"
  ]
}
EOF

    cat > "${HOST_TOOLS_MANIFEST_DIR}/vcpkg-configuration.json" <<EOF
{
  "overlay-ports": [ "${LADYBIRD_SOURCE_DIR}/Meta/CMake/vcpkg/overlay-ports" ],
  "overlay-triplets": [ "${LADYBIRD_SOURCE_DIR}/Meta/CMake/vcpkg/distribution-triplets" ]
}
EOF
}

export_common_environment() {
    export LADYBIRD_SOURCE_DIR
    export VCPKG_ROOT
    export VCPKG_INSTALLED_DIR
    export VCPKG_BINARY_SOURCES="clear;files,${VCPKG_BINARY_CACHE},readwrite"
    export X_VCPKG_ASSET_SOURCES="clear;x-azurl,https://vcpkg-cache.app.ladybird.org/ladybird/source-assets,,read"
    export PKG_CONFIG_LIBDIR="${ARMHF_SYSROOT}/usr/lib/pkgconfig:${ARMHF_SYSROOT}/usr/share/pkgconfig"
    # vcpkg 生成的 .pc 文件使用目标安装目录的绝对路径。若这里设置
    # PKG_CONFIG_SYSROOT_DIR，pkg-config 会把 sysroot 拼到这些绝对路径前，
    # 生成类似 ".../sysroot/Volumes/.../vcpkg_installed/..." 的无效 include 路径。
    export PKG_CONFIG_SYSROOT_DIR=

    export_proxy_environment
}

remove_stale_target_artifacts() {
    # 早期 Skia overlay 会额外安装 libwuffs.a。现在 Skia 已禁用 Wuffs，
    # 该文件不会再被 vcpkg 清单管理；增量构建目录中若残留，直接清掉。
    rm -f "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/libwuffs.a"
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

check_target_cxx_stdlib() {
    if [ "${SKIP_STDLIB_CHECK}" = "1" ]; then
        return
    fi

    local check_source
    check_source="${BUILD_DIR}/cmake/check-cxx${CXX_STANDARD}-stdlib.cpp"
    mkdir -p "$(dirname "${check_source}")"

    if [ "${CXX_STANDARD}" -ge 20 ]; then
        cat > "${check_source}" <<'EOF'
#include <AK/StdLibExtras.h>
#include <compare>

int main()
{
    int storage = 0;
    auto* value = AK::construct_at(&storage, 1);
    auto ordering = (*value <=> storage);
    return ordering == std::strong_ordering::equal ? 0 : 1;
}
EOF
    else
        cat > "${check_source}" <<'EOF'
#include <memory>
#include <vector>

int main()
{
    auto value = std::make_unique<int>(1);
    std::vector<int> values { *value };
    return values[0] == 1 ? 0 : 1;
}
EOF
    fi

    if ! /usr/bin/clang++ \
        --target="${ARMHF_TARGET_TRIPLE}" \
        --gcc-toolchain="${ARMHF_TOOLCHAIN_ROOT}" \
        --sysroot="${ARMHF_SYSROOT}" \
        -I"${LADYBIRD_SOURCE_DIR}" \
        ${ARMHF_CPU_FLAGS} \
        -std=c++${CXX_STANDARD} \
        -c "${check_source}" \
        -o "${BUILD_DIR}/cmake/check-cxx${CXX_STANDARD}-stdlib.o" > "${LOG_DIR}/check-cxx${CXX_STANDARD}-stdlib.log" 2>&1; then
        cat >&2 <<EOF
当前 ARM 目标 C++ 标准库不满足 Ladybird/Ladybone 的 C++${CXX_STANDARD} 构建要求。

检测失败日志：
  ${LOG_DIR}/check-cxx${CXX_STANDARD}-stdlib.log

当前工具链：
  ${ARMHF_TOOLCHAIN_ROOT}

仅换 clang 编译器不够，因为 clang 仍然会使用该 sysroot 里的目标 C++ 标准库。

建议处理方式：
  1. 换成带更新 libstdc++ 的 armv7 hard-float Linux 工具链；或
  2. 为 arm-unknown-linux-gnueabihf 构建并配置目标 libc++/libc++abi。

如需强制跳过此检查，可设置 SKIP_STDLIB_CHECK=1，但当前源码仍可能在 AK 编译阶段失败。
EOF
        return 1
    fi
}

build_host_tools() {
    if [ "${FORCE_HOST_TOOLS}" != "1" ] && [ -f "${LAGOM_TOOLS_DIR}/LagomToolsConfig.cmake" ]; then
        return
    fi

    mkdir -p "${LOG_DIR}"
    write_host_tools_manifest
    export_proxy_environment

    # 交叉编译时 LibWeb/LibJS 等代码生成器必须运行在宿主机上。
    # 因此这里先构建并安装 native LagomTools，再把 LagomTools_DIR 传给 ARM 目标配置。
    (
        unset VCPKG_INSTALLED_DIR
        unset VCPKG_TARGET_TRIPLET
        unset CMAKE_TOOLCHAIN_FILE
        export LADYBIRD_SOURCE_DIR

        cmake -S "${LADYBIRD_SOURCE_DIR}" -B "${HOST_TOOLS_BUILD_DIR}" -G Ninja --preset Host_Tools \
            -DVCPKG_MANIFEST_DIR="${HOST_TOOLS_MANIFEST_DIR}" \
            -DCMAKE_INSTALL_PREFIX="${HOST_TOOLS_DIR}" \
            2>&1 | tee "${LOG_DIR}/host-tools-configure.log"

        cmake --build "${HOST_TOOLS_BUILD_DIR}" --target install --parallel "${JOBS}" \
            2>&1 | tee "${LOG_DIR}/host-tools-build.log"
    )
}

configure() {
    die_if_missing_file "${ARMHF_TOOLCHAIN_ROOT}/bin/${ARMHF_TARGET_TRIPLE}-gcc"
    die_if_missing_file "${ARMHF_SYSROOT}"
    die_if_missing_file "${JINGWEI_SOURCE_DIR}"
    die_if_missing_file "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"

    write_toolchain_files
    build_host_tools
    export_common_environment
    remove_stale_target_artifacts
    ensure_rust_target
    check_target_cxx_stdlib

    # JW 模式会在顶层 CMake 中强制关闭 3D/WebGL、HTTP3、DBus、AVIF/JXL、
    # WebRTC、MathML、SVG 等非当前目标能力。这里仍显式传入关键项，便于阅读日志。
    cmake -S "${LADYBIRD_SOURCE_DIR}" -B "${BUILD_DIR}" -G Ninja \
        -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}" \
        -DCMAKE_TOOLCHAIN_FILE="${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" \
        -DCMAKE_CXX_STANDARD="${CXX_STANDARD}" \
        -DCMAKE_C_COMPILER_AR="${ARMHF_TOOLCHAIN_ROOT}/bin/${ARMHF_TARGET_TRIPLE}-ar" \
        -DCMAKE_CXX_COMPILER_AR="${ARMHF_TOOLCHAIN_ROOT}/bin/${ARMHF_TARGET_TRIPLE}-ar" \
        -DCMAKE_C_COMPILER_RANLIB="${ARMHF_TOOLCHAIN_ROOT}/bin/${ARMHF_TARGET_TRIPLE}-ranlib" \
        -DCMAKE_CXX_COMPILER_RANLIB="${ARMHF_TOOLCHAIN_ROOT}/bin/${ARMHF_TARGET_TRIPLE}-ranlib" \
        -DVCPKG_CHAINLOAD_TOOLCHAIN_FILE="${MAIN_TOOLCHAIN_FILE}" \
        -DVCPKG_TARGET_TRIPLET="${VCPKG_TARGET_TRIPLET}" \
        -DVCPKG_INSTALLED_DIR="${VCPKG_INSTALLED_DIR}" \
        -DVCPKG_OVERLAY_TRIPLETS="${TRIPLET_DIR}" \
        -DVCPKG_INSTALL_OPTIONS=--no-print-usage \
        -DLADYBIRD_GUI_FRAMEWORK=JW \
        -DJINGWEI_SOURCE_DIR="${JINGWEI_SOURCE_DIR}" \
        -DLagomTools_DIR="${LAGOM_TOOLS_DIR}" \
        -DRUST_TARGET_TRIPLE="${RUST_TARGET_TRIPLE}" \
        -DBUILD_SHARED_LIBS=ON \
        -DBUILD_TESTING="${ARMHF_BUILD_TESTING}" \
        -DLADYBIRD_ENABLE_3D_GRAPHICS=OFF \
        -DLADYBIRD_ENABLE_NETWORK_HTTP3=OFF \
        -DLADYBIRD_ENABLE_DESKTOP_DBUS=OFF \
        2>&1 | tee "${LOG_DIR}/configure.log"
}

build() {
    export_common_environment
    remove_stale_target_artifacts

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
