#!/usr/bin/env bash
# 多人版契约测试：会话身份线格式（容器内 g++，无外部依赖与原版资源）。
# JS 契约（选项校验、观战生成块）另经 node --test tests/contracts/ 运行。
# 用法：tests/contracts/run.sh [--debug]
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
