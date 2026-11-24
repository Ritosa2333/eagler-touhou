#!/usr/bin/env bash
# th13 一键门槛：在容器里完成构建、回归摘要、全部测试套件、与基线比对和排版检查，
# 任何一步回归即非零退出。
#
# 用法（宿主执行，任意工作目录）：
#   ports/th13/tools/gate.sh [--mp] [--browser]
#
# 选项：
#   --mp       检查多人版 ports/th13-mp（默认 ports/th13）
#   --browser  追加浏览器套件（th13-shot 镜像；需要运行时资源目录与字体）。固定 4 并发：
#              更多会让无头 Chromium 的音频线程饿死，报 AudioContext 错误
#
# 步骤（th13-dev 镜像，于端口目录执行）：
#   1. 排版检查             node tools/format.mjs --check
#   2. WASI harness 构建    node portable/build-harness.mjs
#   3. 回归摘要             node tools/regression-digest.mjs --check（三项，仅普通版）
#   4. 网页运行时构建       node portable/build.mjs
#   5. 全部套件与基线比对   node tools/gate.mjs --group all --compare <基线>
# 多人版的第 5 步另含 M1–M9 累计原生工作流。外层工作区与原版资源的约定同
# tools/container.sh；TH_BUILD_JOBS 等 TH13_/TH_ 开头的环境变量原样传入。
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "$script_dir/../../.." && pwd)"
container="$repo_root/ports/th13/tools/container.sh"
port_flag=()
port=ports/th13
browser=0
while [[ $# -gt 0 ]]; do
  case "$1" in
    --mp) port_flag=(--mp); port=ports/th13-mp; shift ;;
    --browser) browser=1; shift ;;
    -h|--help) sed -n '2,/^set -euo pipefail/p' "${BASH_SOURCE[0]}" | sed '$d;s/^# \{0,1\}//'; exit 0 ;;
    *) echo "gate.sh: 未知参数：$1" >&2; exit 2 ;;
  esac
done

# 基线是动产品代码之前登记的逐套件失败清单；带 --compare 时退出码只反映基线之外的新失败。
"$container" "${port_flag[@]}" bash -lc '
set -euo pipefail
tools=tools
baseline=baseline_th13.json
if [[ "$(basename "$PWD")" == th13-mp ]]; then tools=../th13/tools; baseline=baseline_th13_mp.json; fi
step() { printf "\n==> %s\n" "$1"; }
step "排版检查";           node tools/format.mjs --check
step "WASI harness 构建";  node portable/build-harness.mjs
if [[ -f tools/regression-digest.mjs ]]; then
  step "回归摘要：状态与事件";  node tools/regression-digest.mjs --check
  step "回归摘要：绘制清单";    node tools/regression-digest.mjs --draws --check
  step "回归摘要：逐关绘制";    node tools/regression-digest.mjs --draws --draws-stages all --check
fi
step "网页运行时构建";     node portable/build.mjs
step "全部套件与基线比对"; node "$tools/gate.mjs" --group all \
  --compare "../../archive/th13-refactor-baseline-20261124/$baseline"
'
if [[ "$browser" == 1 ]]; then
  "$container" --shot "${port_flag[@]}" bash -lc 'node --test --test-concurrency=4 web/tests/sdl/*.test.mjs'
fi
