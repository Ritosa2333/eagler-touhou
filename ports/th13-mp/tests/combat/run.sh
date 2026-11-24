#!/bin/sh
set -eu
cd "$(dirname "$0")/../.."
OUT="${TH13_MP_TEST_CACHE:-${TMPDIR:-/tmp}/th13-mp-combat-suite}"
mkdir -p "$OUT"
newest_hdr=$(ls -t web/cpp/game/*.hpp web/cpp/multiplayer/*.hpp 2>/dev/null | head -1)
for src in web/cpp/game/*.cpp web/cpp/multiplayer/*.cpp; do
    [ -f "$src" ] || continue
    obj="$OUT/$(basename "$src" .cpp).o"
    if [ "$src" -nt "$obj" ] || [ "$newest_hdr" -nt "$obj" ] || [ ! -f "$obj" ]; then
        c++ -std=c++17 -O2 -DTH13_MULTIPLAYER=1 -I web/cpp -I ../../common/include \
            -I web/cpp/multiplayer -c "$src" -o "$obj"
    fi
done
newest_common_hdr=$(ls -t ../../common/include/eagler/netplay/*.hpp | head -1)
for src in ../../common/src/netplay/*.cpp; do
    obj="$OUT/common_$(basename "$src" .cpp).o"
    if [ "$src" -nt "$obj" ] || [ "$newest_common_hdr" -nt "$obj" ] || [ ! -f "$obj" ]; then
        c++ -std=c++17 -O2 -I ../../common/include -I web/cpp/multiplayer -c "$src" -o "$obj"
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
