#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
port_dir="$(cd "$script_dir/../.." && pwd)"
repo_root="$(cd "$port_dir/../../.." && pwd)"
cd "$repo_root"

flags="-O2 -Wall -Wextra"
[[ "${1:-}" == "--debug" ]] && flags="-O0 -g"

out_dir="$port_dir/tests/ecl/build"
mkdir -p "$out_dir"

sources=(eagler-touhou/ports/th13/web/cpp/game/*.cpp)

object_dir="$(mktemp -d "$out_dir/objects.XXXXXX")"
trap 'rm -rf -- "$object_dir"' EXIT
objects=()
status=0
for source in "${sources[@]}"; do
  object="$object_dir/$(basename "${source%.cpp}").o"
  g++ -std=c++17 $flags -fno-exceptions -fno-rtti -ffp-contract=off \
    -Ieagler-touhou/ports/th13/web/cpp -c "$source" -o "$object" || status=1
  objects+=("$object")
done
(( status )) && exit 1

for suite in "$script_dir"/ecl_*_test.cpp; do
  name="$(basename "$suite" .cpp)"
  g++ -std=c++17 $flags -fno-exceptions -fno-rtti -ffp-contract=off \
    -Ieagler-touhou/ports/th13/web/cpp \
    "${objects[@]}" "$suite" \
    -o "$out_dir/$name" || status=1
done
(( status )) && exit 1

for suite in "$script_dir"/ecl_*_test.cpp; do
  name="$(basename "$suite" .cpp)"
  "$out_dir/$name" || status=1
done
exit $status
