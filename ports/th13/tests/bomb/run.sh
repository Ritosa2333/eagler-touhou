#!/bin/sh
set -eu
cd "$(dirname "$0")/../.."
OUT="${TMPDIR:-/tmp}/th13-youmu-bomb-test"
c++ -std=c++17 -O2 -Wall -Wextra -I web/cpp \
    web/cpp/game/YoumuBomb.cpp tests/combat/youmu_bomb_test.cpp -o "$OUT"
"$OUT"
