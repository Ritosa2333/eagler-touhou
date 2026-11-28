#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
port_dir="$(cd "$script_dir/../.." && pwd)"
workspace="$(cd "$port_dir/../../.." && pwd)"

FROZEN_PORT=c361a5e5d9da3ff015f8a34b4c1452c1dd2c173fe6a9f7d0faf239fc8975147f
FROZEN_ORACLE=c361a5e5d9da3ff015f8a34b4c1452c1dd2c173fe6a9f7d0faf239fc8975147f

if [[ ! -f "$workspace/games/th13/th13.exe" || ! -d "$workspace/oracle" ]]; then
  echo "skip: 需要 /work 工作区（th13.exe + oracle/），宿主环境退出 77"
  exit 77
fi

tmp_rel="artifacts/tmp-baseline.$$"
tmp_host="$workspace/$tmp_rel"
mkdir -p "$tmp_host"
trap 'rm -rf "$tmp_host"' EXIT

(cd "$workspace" && node --experimental-wasi-unstable-preview1 \
  "$port_dir/tools/run-harness.mjs" replay games/replays/th13_udSr01.rpy \
  /artifacts/reference/raw "/$tmp_rel/port.jsonl" 2700 0 >/dev/null 2>&1)
(cd "$workspace" && python3 -m oracle.stage1_oracle \
  games/th13/th13.exe artifacts/reference/raw "$tmp_host/oracle.jsonl" \
  --replay games/replays/th13_udSr01.rpy --replay-stage 0 --frames 2700 \
  >/dev/null 2>&1)

got_port="$(sha256sum "$tmp_host/port.jsonl" | awk '{print $1}')"
got_oracle="$(sha256sum "$tmp_host/oracle.jsonl" | awk '{print $1}')"
fail=0
[[ "$got_port" == "$FROZEN_PORT" ]] || { echo "基线腐烂（port）：$got_port != $FROZEN_PORT"; fail=1; }
[[ "$got_oracle" == "$FROZEN_ORACLE" ]] || { echo "基线腐烂（oracle）：$got_oracle != $FROZEN_ORACLE"; fail=1; }
if (( fail )); then
  echo "若两侧哈希相同则是一次双侧同步的有意改动——按文件头流程更新冻结值。"
  exit 1
fi

result="$(cd "$workspace" && python3 -m oracle.stage1_compare "$tmp_host/port.jsonl" "$tmp_host/oracle.jsonl" | tail -1 || true)"
[[ "$result" == *"first_divergence_frame=none"* ]] || { echo "比对意外：$result"; exit 1; }
echo "baseline 2700f/3195e 双侧一致（port=oracle=$got_port）"
