#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
port_dir="$(cd "$script_dir/../.." && pwd)"
repo_root="$(cd "$port_dir/../../.." && pwd)"
cd "$repo_root"

flags="-O2 -Wall -Wextra"
[[ "${1:-}" == "--debug" ]] && flags="-O0 -g"

out_dir="$port_dir/tests/ui/build/portrait_identity"
mkdir -p "$out_dir"
object_dir="$out_dir/objects"
rm -rf "$object_dir"
mkdir -p "$object_dir"

sources=(eagler-touhou/ports/th13/web/cpp/game/*.cpp)
objects=()
for source in "${sources[@]}"; do
  object="$object_dir/$(basename "${source%.cpp}").o"
  g++ -std=c++17 $flags -fno-exceptions -fno-rtti -ffp-contract=off \
    -Ieagler-touhou/ports/th13/web/cpp \
    -c "$source" -o "$object"
  objects+=("$object")
done

g++ -std=c++17 $flags -fno-exceptions -fno-rtti -ffp-contract=off \
  -Ieagler-touhou/ports/th13/web/cpp \
  "${objects[@]}" "$script_dir/portrait_identity_test.cpp" \
  -o "$out_dir/portrait_identity_test"

"$out_dir/portrait_identity_test"
