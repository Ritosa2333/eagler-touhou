#!/usr/bin/env bash
# 用 thtk 生成对照基准（只写入忽略目录 artifacts/th16/）：
#   dat/         thdat -x16 解包：tests/archive.test.mjs 对 215 个条目逐字节对照（已存在则跳过）
#   dumps/anm/   thanm -l16 的 ANM 列表文本：entry / 精灵 / 脚本计数对照
# 在容器里运行：ports/th16/container/podman.sh bash ports/th16/tools/thtk-dumps.sh
set -euo pipefail
port="$(cd "$(dirname "$0")/.." && pwd)"
root="$(cd "$port/../.." && pwd)"
dat="$root/artifacts/th16/dat"
dumps="$root/artifacts/th16/dumps/anm"
original="${TH16_ORIGINAL_DIR:-$root/games/th16/original}"
mkdir -p "$dat" "$dumps"
if [ -z "$(ls -A "$dat")" ]; then
    (cd "$dat" && thdat -x16 "$original/th16.dat")
fi
for anm in "$dat"/*.anm; do
    thanm -l16 "$anm" > "$dumps/$(basename "${anm%.anm}").txt"
done
echo "thtk: $(ls -A "$dat" | wc -l) entries in $dat, $(ls "$dumps" | wc -l) ANM listings in $dumps"
