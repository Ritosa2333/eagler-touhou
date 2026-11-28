#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
port_dir="$(cd "$script_dir/../.." && pwd)"

flags="-O2 -Wall -Wextra"
[[ "${1:-}" == "--debug" ]] && flags="-O0 -g"

out_dir="$port_dir/tests/contracts/build"
mkdir -p "$out_dir"

g++ -std=c++17 $flags -fno-exceptions -fno-rtti -I"$port_dir/web/cpp" \
  "$port_dir/web/cpp/multiplayer/SessionSetup.cpp" "$script_dir/session_setup_wire_test.cpp" \
  -o "$out_dir/session_setup_wire_test"
exec "$out_dir/session_setup_wire_test"
