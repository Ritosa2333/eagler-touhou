#!/usr/bin/env bash
# 绿基线防腐烂闸（udSr01 事件窗前 2700 帧双侧冻结哈希对拍）。
#
# 动机（2026-10-22 事故）：74f54c1a 单侧改 StageBattle 弹模型而未动 oracle
# 孪生，udSr01「10662 帧一致」的绿灯静默腐烂——直到下次对拍才发现。本闸
# 每次跑都**双侧重跑** 600 帧并与冻结哈希比对：任何一侧的孪生语义改动都会
# 立即翻红；双侧同步的有意改动则需按下述步骤自觉更新哈希。
#
# 用法（容器内 /work 下任意目录）：
#   bash eagler-touhou/ports/th13/tests/baseline/run.sh
# 环境要求：th13-dev 容器（th13.exe + python3 unicorn 可用）；宿主直跑会
# 以 exit 77 跳过（无 oracle 环境）。
#
# 哈希更新流程（双侧同步改动后，容器内全绿时）：
#   bash tests/baseline/run.sh   # 读失败输出里的新哈希
#   # 将 NEW_PORT/NEW_ORACLE 更新为本文件下方的冻结值，并在提交信息里
#   # 写明两侧改动的对应关系（如 c73f5221↔e5e6348、本次死亡掉功率口径）。
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
port_dir="$(cd "$script_dir/../.." && pwd)"
workspace="$(cd "$port_dir/../../.." && pwd)"

# 冻结基线（2026-10-23：c3cce8cf + 死亡掉功率对拍口径修复后冻结；
# udSr01 st01 前 2700 帧 / 3082 事件，双侧同哈希。窗长选 2700：覆盖
# f~2288 玩家死亡→功率块 9↔8 边界与 f2292 卫星弹相位——600 帧窗抓不到
# 该型漂移（块边界在 kill≥13 处），负测试实证）
FROZEN_PORT=bf7ef00660347f39494457dc7d803efc0f12e10208767fdc1cb8ebe6993d1e7f
FROZEN_ORACLE=bf7ef00660347f39494457dc7d803efc0f12e10208767fdc1cb8ebe6993d1e7f

if [[ ! -f "$workspace/games/th13/th13.exe" || ! -d "$workspace/oracle" ]]; then
  echo "skip: 需要 /work 工作区（th13.exe + oracle/），宿主环境退出 77"
  exit 77
fi

# 临时目录双视界：bash 宿主视界 = $workspace/...（容器内挂载于 /work）；
# wasm 视界 = /...（run-harness 的 WASI 预挂载根 = 工作区）。两侧各自用
# 对应视界的路径。
tmp_rel="artifacts/tmp-baseline.$$"          # 相对工作区根
tmp_host="$workspace/$tmp_rel"               # bash/python 用
mkdir -p "$tmp_host"
trap 'rm -rf "$tmp_host"' EXIT

# 移植侧：harness replay 前 600 帧
# 注意：harness 的 WASI 预挂载根 = 工作区，rpy 传工作区相对路径（挂载内
# 绝对路径 /games/...），资源目录同为 /artifacts/...（挂载内绝对路径）。
(cd "$workspace" && node --experimental-wasi-unstable-preview1 \
  "$port_dir/tools/run-harness.mjs" replay games/replays/th13_udSr01.rpy \
  /artifacts/reference/raw "/$tmp_rel/port.jsonl" 2700 0 >/dev/null 2>&1)
# oracle 侧：stage1_oracle 同窗
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

# 双侧一致性裁决（哈希相同则必然 none，此步为形式自证）
result="$(cd "$workspace" && python3 -m oracle.stage1_compare "$tmp_host/port.jsonl" "$tmp_host/oracle.jsonl" | tail -1)"
[[ "$result" == *"first_divergence_frame=none"* ]] || { echo "比对意外：$result"; exit 1; }
echo "baseline 2700f/3082e 双侧一致（port=oracle=$got_port）"
