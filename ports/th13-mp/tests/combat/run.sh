#!/bin/sh
# 战斗语义定向快测（多 TU：全部 game 源 + 单测试文件）。
# 2026-11-01 补建：本套件此前无 runner，从未进常规门（假绿教训的一部分）。
# 用法：容器内 ports/th13 目录执行 sh tests/combat/run.sh。退出 0=全过。
set -eu
cd "$(dirname "$0")/../.."
# th13-mp：对象缓存与 SP 隔离（同一 /tmp 下不同目录），避免两侧对象互踩。
OUT="${TH13_MP_TEST_CACHE:-${TMPDIR:-/tmp}/th13-mp-combat-suite}"
mkdir -p "$OUT"
# 游戏源一次性编译成对象缓存（源或任一头更新才重编：mtime 只看 .cpp 会
# 在改 hpp 后复用旧 ABI 对象，测试 TU 与库结构大小错位 → 运行期崩溃。
# 2026-11-09 实证：DamageRegistration/PlayerShot 加 owner_seat 后全套
# combat 段错误，rm 缓存后全过）。
newest_hdr=$(ls -t web/cpp/game/*.hpp web/cpp/multiplayer/*.hpp 2>/dev/null | head -1)
for src in web/cpp/game/*.cpp web/cpp/multiplayer/*.cpp; do
    [ -f "$src" ] || continue
    obj="$OUT/$(basename "$src" .cpp).o"
    if [ "$src" -nt "$obj" ] || [ "$newest_hdr" -nt "$obj" ] || [ ! -f "$obj" ]; then
        c++ -std=c++17 -O2 -DTH13_MULTIPLAYER=1 -I web/cpp -I ../../common/include \
            -I web/cpp/multiplayer -c "$src" -o "$obj"
    fi
done
# 共用网络栈（M7）：Session/NetplayRuntime 链接 common/src/netplay（非
# Emscripten 下 BrowserPeerTransport 的 EM_JS 为空实现——纯逻辑面）。
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
