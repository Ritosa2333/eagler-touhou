#!/usr/bin/env bash
# ANM 管线单测构建与运行（容器内 g++，无外部依赖）。
# 自动发现 tests/anm/anm_*_test.cpp 套件；用 --debug 切 -O0 -g。
# 用法：tests/anm/run.sh [--debug]
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
port_dir="$(cd "$script_dir/../.." && pwd)"
repo_root="$(cd "$port_dir/../../.." && pwd)"
cd "$repo_root"

flags="-O2 -Wall -Wextra"
[[ "${1:-}" == "--debug" ]] && flags="-O0 -g"

out_dir="$port_dir/tests/anm/build"
mkdir -p "$out_dir"

# 游戏源与套件均自动发现，使任意历史提交点可独立构建当时的测试。
sources=(eagler-touhou/ports/th13/web/cpp/game/*.cpp)

object_dir="$(mktemp -d "$out_dir/objects.XXXXXX")"
trap 'rm -rf -- "$object_dir"' EXIT
objects=()
pids=()
status=0
# Compile fresh product objects once per invocation; no old object cache is
# accepted. Independent translation units use up to eight available cores.
for source in "${sources[@]}"; do
  object="$object_dir/$(basename "${source%.cpp}").o"
  g++ -std=c++17 $flags -fno-exceptions -fno-rtti -ffp-contract=off \
    -Ieagler-touhou/ports/th13/web/cpp -c "$source" -o "$object" &
  pids+=("$!")
  objects+=("$object")
  if (( ${#pids[@]} == 8 )); then
    for pid in "${pids[@]}"; do wait "$pid" || status=1; done
    pids=()
  fi
done
for pid in "${pids[@]}"; do wait "$pid" || status=1; done
(( status )) && exit 1

for suite in "$script_dir"/anm_*_test.cpp; do
  name="$(basename "$suite" .cpp)"
  g++ -std=c++17 $flags -fno-exceptions -fno-rtti -ffp-contract=off \
    -Ieagler-touhou/ports/th13/web/cpp \
    "${objects[@]}" "$suite" \
    -o "$out_dir/$name" || status=1
done
(( status )) && exit 1

for suite in "$script_dir"/anm_*_test.cpp; do
  name="$(basename "$suite" .cpp)"
  "$out_dir/$name" || status=1
done
exit $status
