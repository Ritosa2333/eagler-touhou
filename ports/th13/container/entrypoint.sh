#!/bin/bash
# 容器入口：环境摘要 + 缓存落盘 + 执行用户命令
set -e
# emscripten 端口/系统库缓存（SDL3 等）落到工作区卷：
# EM_CACHE 是 emcc 认可的缓存目录覆盖（优先于 .emscripten 的 CACHE 行），
# 避免每次容器运行重新下载并编译 SDL3 端口。
if [ -w /work ]; then
    export EM_CACHE="${EM_CACHE:-/work/.cache/emscripten}"
    mkdir -p "$EM_CACHE"
fi
if [ "$1" = "bash" ] || [ -z "$1" ]; then
    echo "== th13 神灵庙移植开发环境 =="
    echo "node    $(node --version 2>/dev/null || echo n/a)"
    echo "python  $(python3 --version 2>/dev/null || echo n/a)"
    echo "emcc    $(emcc --version 2>/dev/null | head -1 || echo n/a)"
    echo "wasi    ${WASI_SDK_PATH:-n/a}"
    echo "ghidra  $( [ -d /opt/ghidra ] && echo /opt/ghidra || echo n/a )"
    echo "workdir /work"
fi
exec "$@"
