#!/bin/sh
# 妖梦 Bomb 斩击池 native 定向快测（单 TU，无 wasm/浏览器依赖）。
# 用法：在 th13-dev 容器内于 ports/th13 目录执行 sh tests/bomb/run.sh。
# 退出码：0 = 全部断言通过；1 = 存在失败（stdout 逐条 FAIL 行）。
set -eu
cd "$(dirname "$0")/../.."
OUT="${TMPDIR:-/tmp}/th13-youmu-bomb-test"
c++ -std=c++17 -O2 -Wall -Wextra -I web/cpp \
    web/cpp/game/YoumuBomb.cpp tests/combat/youmu_bomb_test.cpp -o "$OUT"
"$OUT"
