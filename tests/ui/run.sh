#!/usr/bin/env bash
# UI 基座单测构建与运行（容器内 g++，无外部依赖；同 tests/ecl 约定）。
# 自动发现 tests/ui/*_test.cpp 套件；用 --debug 切 -O0 -g。
# 用法：tests/ui/run.sh [--debug]
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
port_dir="$(cd "$script_dir/../.." && pwd)"
cd "${TH13_TEST_WORKSPACE:-$port_dir}"

flags="-O2 -Wall -Wextra"
[[ "${1:-}" == "--debug" ]] && flags="-O0 -g"

out_dir="$port_dir/tests/ui/build"
mkdir -p "$out_dir"

# 游戏源与套件均自动发现，使任意历史提交点可独立构建当时的测试。
sources=("$port_dir"/web/cpp/game/*.cpp)

# 本轮新编译一组游戏对象供所有套件复用，不读取旧对象缓存。
object_dir="$(mktemp -d "$out_dir/objects.XXXXXX")"
trap 'rm -rf -- "$object_dir"' EXIT
objects=()
status=0
for source in "${sources[@]}"; do
  object="$object_dir/$(basename "${source%.cpp}").o"
  g++ -std=c++17 $flags -fno-exceptions -fno-rtti -ffp-contract=off \
    -I"$port_dir/web/cpp" -I"$port_dir/../../common/include" \
    -DTH13_TEST_PORT_DIR=\""$port_dir"\" \
    -c "$source" -o "$object" || status=1
  objects+=("$object")
done
(( status )) && exit 1

for suite in "$script_dir"/*_test.cpp; do
  name="$(basename "$suite" .cpp)"
  if ! g++ -std=c++17 $flags -fno-exceptions -fno-rtti -ffp-contract=off \
      -I"$port_dir/web/cpp" -I"$port_dir/../../common/include" \
      -DTH13_TEST_PORT_DIR=\""$port_dir"\" \
      "${objects[@]}" "$suite" \
      -o "$out_dir/$name"; then
    echo "tests/ui/run.sh: 套件 $name 构建失败（其余套件继续）" >&2
    status=1
  fi
done

# 所有 suite 均运行（构建失败的缺可执行计为失败），退出码聚合。
for suite in "$script_dir"/*_test.cpp; do
  name="$(basename "$suite" .cpp)"
  bin="$out_dir/$name"
  if [[ -x "$bin" ]]; then
    "$bin" || status=1
  else
    echo "tests/ui/run.sh: 缺少 $name 可执行（构建失败），计为失败" >&2
    status=1
  fi
done
exit $status
