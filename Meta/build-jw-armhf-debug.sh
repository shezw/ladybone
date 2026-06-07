#!/usr/bin/env bash

set -eo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# shellcheck source=/dev/null
. "${SCRIPT_DIR}/Utils/shell_include.sh"

ensure_ladybird_source_dir

# 本脚本用于编译 armhf JW/fbdev2d 的 Debug 版本。
# 它只设置 Debug 构建需要变化的环境变量，然后复用 build-jw-armhf.sh 中的
# 工具链、vcpkg、Rust target、JW/fbdev 后端等配置，避免两套交叉编译逻辑分叉。
#
# 默认输出目录：
#   Build/armhf-jw-debug
#
# 常用命令：
#   ./Meta/build-jw-armhf-debug.sh configure
#   ./Meta/build-jw-armhf-debug.sh build
#   ./Meta/build-jw-armhf-debug.sh all
#   ./Meta/build-jw-armhf-debug.sh clean
#
# 可覆盖变量示例：
#   BUILD_DIR=/tmp/ladybone-armhf-debug ./Meta/build-jw-armhf-debug.sh all
#   JOBS=4 ./Meta/build-jw-armhf-debug.sh build

export BUILD_DIR="${BUILD_DIR:-${LADYBIRD_SOURCE_DIR}/Build/armhf-jw-debug}"
export CMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE:-Debug}"

# 第三方 vcpkg port 仍沿用 build-jw-armhf.sh 生成的 release triplet。
# 这样 Debug 产物包含 Ladybone/Lagom/JW 的调试信息，同时避免重新构建巨大的
# Debug 版 Skia/ICU 等依赖，适合嵌入式板子上先定位 WebContent/平台层问题。
exec "${SCRIPT_DIR}/build-jw-armhf.sh" "$@"
