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

sources=(eagler-touhou/ports/th13-mp/web/cpp/game/*.cpp)

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
