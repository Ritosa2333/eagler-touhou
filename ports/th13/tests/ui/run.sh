#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
port_dir="$(cd "$script_dir/../.." && pwd)"
repo_root="$(cd "$port_dir/../../.." && pwd)"
cd "$repo_root"

flags="-O2 -Wall -Wextra"
[[ "${1:-}" == "--debug" ]] && flags="-O0 -g"

out_dir="$port_dir/tests/ui/build"
mkdir -p "$out_dir"

sources=(eagler-touhou/ports/th13/web/cpp/game/*.cpp)

object_dir="$(mktemp -d "$out_dir/objects.XXXXXX")"
trap 'rm -rf -- "$object_dir"' EXIT
objects=()
status=0
for source in "${sources[@]}"; do
  object="$object_dir/$(basename "${source%.cpp}").o"
  g++ -std=c++17 $flags -fno-exceptions -fno-rtti -ffp-contract=off \
    -Ieagler-touhou/ports/th13/web/cpp -Ieagler-touhou/common/include \
    -c "$source" -o "$object" || status=1
  objects+=("$object")
done
(( status )) && exit 1

for suite in "$script_dir"/*_test.cpp; do
  name="$(basename "$suite" .cpp)"
  if ! g++ -std=c++17 $flags -fno-exceptions -fno-rtti -ffp-contract=off \
      -Ieagler-touhou/ports/th13/web/cpp -Ieagler-touhou/common/include \
      "${objects[@]}" "$suite" \
      -o "$out_dir/$name"; then
    echo "tests/ui/run.sh: 套件 $name 构建失败（其余套件继续）" >&2
    status=1
  fi
done

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
