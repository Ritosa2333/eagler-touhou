#!/usr/bin/env bash
# 本机编译 harness（clang++/g++，-ffp-contract=off 与网页/WASI 构建同口径）并运行 node 测试。
# 用法：bash ports/th16/tests/run.sh   （仓库根目录；原版 th16.dat 在 games/th16/original/）
set -euo pipefail
here="$(cd "$(dirname "$0")" && pwd)"
port="$(cd "$here/.." && pwd)"
build="$here/build"
mkdir -p "$build"
CXX="${CXX:-clang++}"
sources=("$here/harness/harness.cpp")
for f in "$port"/web/cpp/game/*.cpp; do sources+=("$f"); done
"$CXX" -std=c++17 -O2 -fno-exceptions -fno-rtti -ffp-contract=off -Wall -Wextra \
  -I"$port/web/cpp" -o "$build/harness" "${sources[@]}"
export TH16_HARNESS="$build/harness"
cd "$port"
node --test tests/*.test.mjs
