#!/usr/bin/env bash
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
port_dir="$(cd "$script_dir/../.." && pwd)"
workspace="$(cd "$port_dir/../../.." && pwd)"

exe="$workspace/games/th13/th13.exe"
[[ -f "$exe" ]] || { echo "export.sh: 找不到原版 th13.exe：$exe" >&2; exit 2; }
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

MAXMEM=4G /opt/ghidra/support/analyzeHeadless "$project" th13 "${target[@]}" \
  -analysisTimeoutPerFile 2400 -max-cpu 8 \
  -scriptPath "$script_dir" -postScript ExportTh13.java "$output" "$@" \
  -log "$decomp/analysis.log" -scriptlog "$decomp/scripts.log" > "$decomp/headless.log" 2>&1 || {
    tail -40 "$decomp/headless.log" >&2; exit 1; }
grep -E "TH13 (functions exported|export complete)" "$decomp/headless.log" | tail -2
