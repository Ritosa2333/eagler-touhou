#!/usr/bin/env bash
# th13 统一容器入口：在 th13-dev / th13-shot 镜像内执行命令，宿主不安装依赖。
#
# 用法（宿主执行，任意工作目录）：
#   tools/container.sh [--shot] [--mp] [--] <命令...>    执行一条命令
#   tools/container.sh [--shot] [--mp]                   进入交互 shell
#
# 选项：
#   --shot   使用 th13-shot 镜像（chromium + playwright-core，浏览器验证）
#   --mp     工作目录切到 ports/th13-mp（默认 ports/th13）
#
# 目录约定（与各 tests/*/run.sh、tools/harness-paths.mjs 一致）：
#   /work                 外层工作区：games/th13（原版资源）、artifacts/、.cache/
#   /work/eagler-touhou   monorepo 检出
# 外层工作区默认取 monorepo 的上级目录，可用 TH13_WORKSPACE 覆盖。
# 容器内用户与宿主同 UID；以 TH13_ 或 TH_ 开头的环境变量原样传入。
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd "$script_dir/../../.." && pwd)"
workspace="${TH13_WORKSPACE:-$(cd "$repo_root/.." && pwd)}"

image=th13-dev
port=ports/th13
while [[ $# -gt 0 ]]; do
  case "$1" in
    --shot) image=th13-shot; shift ;;
    --mp) port=ports/th13-mp; shift ;;
    --) shift; break ;;
    -h|--help) sed -n '2,/^set -euo pipefail/p' "${BASH_SOURCE[0]}" | sed '$d;s/^# \{0,1\}//'; exit 0 ;;
    *) break ;;
  esac
done
[[ -d "$workspace" ]] || { echo "container.sh: 外层工作区不存在：$workspace（用 TH13_WORKSPACE 指定）" >&2; exit 2; }

run=(podman run --rm --userns=keep-id -e HOME=/tmp/th13-home
     -v "$workspace:/work:Z" -v "$repo_root:/work/eagler-touhou:Z"
     -w "/work/eagler-touhou/$port")
# TH13_WORKSPACE 只在宿主解析挂载路径，容器内一律以端口目录上三级为工作区。
while IFS= read -r name; do run+=(-e "$name"); done < <(compgen -e | grep -E '^(TH13_|TH_)' | grep -vxF TH13_WORKSPACE || true)
[[ -t 0 ]] && run+=(-it)

if [[ $# -eq 0 ]]; then
  exec "${run[@]}" "$image" bash -l
fi
exec "${run[@]}" "$image" bash -lc "$(printf '%q ' "$@")"
