#!/usr/bin/env bash
# UI 基座单测构建与运行（容器内 g++，无外部依赖；同 tests/ecl 约定）。
# 自动发现 tests/ui/*_test.cpp 套件；用 --debug 切 -O0 -g。
# 用法：tests/ui/run.sh [--debug]
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
port_dir="$(cd "$script_dir/../.." && pwd)"
repo_root="$(cd "$port_dir/../../.." && pwd)"
cd "$repo_root"

flags="-O2 -Wall -Wextra"
[[ "${1:-}" == "--debug" ]] && flags="-O0 -g"

out_dir="$port_dir/tests/ui/build"
mkdir -p "$out_dir"

# 游戏源与套件均自动发现，使任意历史提交点可独立构建当时的测试。
sources=(eagler-touhou/ports/th13-mp/web/cpp/game/*.cpp)

# 每次运行使用独立目录，新编译对象与套件，不读取旧产物。
run_dir="$(mktemp -d "$out_dir/run.XXXXXX")"
object_dir="$run_dir/objects"
mkdir -p "$object_dir"
echo "tests/ui/run.sh: 本次产物 $run_dir"
objects=()
status=0
for source in "${sources[@]}"; do
  object="$object_dir/$(basename "${source%.cpp}").o"
  g++ -std=c++17 $flags -fno-exceptions -fno-rtti -ffp-contract=off \
    -Ieagler-touhou/ports/th13-mp/web/cpp -Ieagler-touhou/common/include \
    -c "$source" -o "$object" || status=1
  objects+=("$object")
done
(( status )) && exit 1

# 仅记录本轮链接成功的套件；失败不能回退执行旧二进制。
built=()
for suite in "$script_dir"/*_test.cpp; do
  name="$(basename "$suite" .cpp)"
  if g++ -std=c++17 $flags -fno-exceptions -fno-rtti -ffp-contract=off \
      -Ieagler-touhou/ports/th13-mp/web/cpp -Ieagler-touhou/common/include \
      "${objects[@]}" "$suite" \
      -o "$run_dir/$name"; then
    built+=("$run_dir/$name")
  else
    echo "tests/ui/run.sh: 套件 $name 构建失败（其余套件继续）" >&2
    status=1
  fi
done

for bin in "${built[@]}"; do
  "$bin" || status=1
done
exit $status
