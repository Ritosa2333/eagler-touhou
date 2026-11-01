#!/bin/sh
# 战斗语义定向快测（多 TU：全部 game 源 + 单测试文件）。
# 2026-11-01 补建：本套件此前无 runner，从未进常规门（假绿教训的一部分）。
# 用法：容器内 ports/th13 目录执行 sh tests/combat/run.sh。退出 0=全过。
set -eu
cd "$(dirname "$0")/../.."
OUT="${TMPDIR:-/tmp}/th13-combat-suite"
mkdir -p "$OUT"
# 游戏源一次性编译成对象缓存（源更新才重编）。
for src in web/cpp/game/*.cpp; do
    obj="$OUT/$(basename "$src" .cpp).o"
    if [ "$src" -nt "$obj" ] || [ ! -f "$obj" ]; then
        c++ -std=c++17 -O2 -I web/cpp -c "$src" -o "$obj"
    fi
done
rc=0
for t in tests/combat/*_test.cpp; do
    name=$(basename "$t" .cpp)
    if ! c++ -std=c++17 -O2 -I web/cpp "$OUT"/*.o "$t" -o "$OUT/$name" 2>"$OUT/$name.buildlog"; then
        echo "not ok - $name (build)"
        rc=1
        continue
    fi
    if ! (cd "${TH13_TEST_CWD:-/work}" && "$OUT/$name") >"$OUT/$name.log" 2>&1; then
        rc=1
        echo "== $name"
        grep -E "^not ok|失败" "$OUT/$name.log" || true
    fi
done
[ "$rc" = 0 ] && echo "combat: ALL PASS"
exit $rc
