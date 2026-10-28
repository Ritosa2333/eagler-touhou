#!/usr/bin/env bash
# 道具/神霊/灵界单测构建与运行（容器内 g++，无外部依赖；与 tests/ecl 同法）。
# 自动发现 tests/items/items_*_test.cpp 套件；--debug 切 -O0 -g。
# 用法：tests/items/run.sh [--debug]（在容器 /work 内执行）
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
port_dir="$(cd "$script_dir/../.." && pwd)"
cd "${TH13_TEST_WORKSPACE:-$port_dir}"

flags="-O2 -Wall -Wextra"
[[ "${1:-}" == "--debug" ]] && flags="-O0 -g"

out_dir="$port_dir/tests/items/build"
mkdir -p "$out_dir"

# 游戏源自动发现（独立套件，不依赖 harness.cpp）。
sources=("$port_dir"/web/cpp/game/*.cpp)

status=0
for suite in "$script_dir"/items_*_test.cpp; do
  name="$(basename "$suite" .cpp)"
  g++ -std=c++17 $flags -fno-exceptions -fno-rtti -ffp-contract=off \
    -I"$port_dir/web/cpp" -DTH13_TEST_PORT_DIR=\""$port_dir"\" \
    "${sources[@]}" "$suite" \
    -o "$out_dir/$name" || status=1
done
(( status )) && exit 1

for suite in "$script_dir"/items_*_test.cpp; do
  name="$(basename "$suite" .cpp)"
  "$out_dir/$name" || status=1
done
exit $status
