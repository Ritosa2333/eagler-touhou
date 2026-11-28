#!/bin/sh
set -eu
cd "$(dirname "$0")/../.."
OUT="${TMPDIR:-/tmp}/th13-combat-suite"
mkdir -p "$OUT"
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
