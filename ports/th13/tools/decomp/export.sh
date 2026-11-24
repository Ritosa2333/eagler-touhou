#!/usr/bin/env bash
# 原版 th13.exe 反编译导出（容器内执行，使用 th13-dev 镜像的 Ghidra 与 JDK）。
# 产物落 <外层工作区>/artifacts/decomp/jp/：functions/ 下逐函数的 .c 与 .asm，
# 以及 functions.json、blocks.json、imports.json、strings.json 四份索引。
# 首次运行分析原版二进制并保存工程，之后复用工程只重跑导出。
# 用法：tools/decomp/export.sh [--reanalyze] [入口地址 ...]
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
port_dir="$(cd "$script_dir/../.." && pwd)"
workspace="$(cd "$port_dir/../../.." && pwd)"

exe="$workspace/games/th13/th13.exe"
[[ -f "$exe" ]] || { echo "export.sh: 找不到原版 th13.exe：$exe" >&2; exit 2; }
# docs/DECOMP.md 登记的 v1.00c 身份；版本不符时函数地址无法与锚点表对照。
expected=f12760ca142725d6cb772d4a3658c2e05f7d1656bbae7e84801be9272dadf80d
actual="$(sha256sum "$exe" | cut -d' ' -f1)"
[[ "$actual" == "$expected" ]] || { echo "export.sh: th13.exe 版本不符：$actual" >&2; exit 2; }

decomp="$workspace/artifacts/decomp"
project="$decomp/project"
output="$decomp/jp"
mkdir -p "$project" "$output"

reanalyze=false
if [[ "${1:-}" == "--reanalyze" ]]; then reanalyze=true; shift; fi
$reanalyze && rm -rf "$project"
rm -rf "$output/functions"

if compgen -G "$project/th13.rep" > /dev/null; then
  target=(-process th13.exe -noanalysis)
else
  target=(-import "$exe")
fi

# 反编译逐函数串行，分析阶段并行；堆上限经 MAXMEM 传给 Ghidra 启动器。
MAXMEM=4G /opt/ghidra/support/analyzeHeadless "$project" th13 "${target[@]}" \
  -analysisTimeoutPerFile 2400 -max-cpu 8 \
  -scriptPath "$script_dir" -postScript ExportTh13.java "$output" "$@" \
  -log "$decomp/analysis.log" -scriptlog "$decomp/scripts.log" > "$decomp/headless.log" 2>&1 || {
    tail -40 "$decomp/headless.log" >&2; exit 1; }
grep -E "TH13 (functions exported|export complete)" "$decomp/headless.log" | tail -2
