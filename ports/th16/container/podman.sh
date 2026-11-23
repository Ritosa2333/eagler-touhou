#!/usr/bin/env bash
# 在临时容器里执行命令：仓库根挂载为 /work，容器退出即销毁，本机不安装任何依赖。
# 镜像 th16-dev 缺失时先按同目录 Dockerfile 的 dev 目标构建。
# 用法：ports/th16/container/podman.sh bash -lc 'bash ports/th16/tests/run.sh'
#       ports/th16/container/podman.sh node --version
set -euo pipefail
here="$(cd "$(dirname "$0")" && pwd)"
repo="$(cd "$here/../../.." && pwd)"
image="${TH16_IMAGE:-th16-dev}"
if ! podman image exists "$image" 2>/dev/null; then
    podman build --target dev \
        --build-arg DEV_UID="$(id -u)" --build-arg DEV_GID="$(id -g)" \
        -t "$image" "$here"
fi
exec podman run --rm --userns=keep-id -e HOME=/home/dev \
    -v "$repo:/work:Z" -w /work "$image" "$@"
