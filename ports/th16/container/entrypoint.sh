#!/bin/bash
# 容器入口：Emscripten 缓存落工作区卷 + 执行用户命令（容器由 podman.sh 以 --rm 启动，退出即销毁）
set -e
# SDL3 / sdl3_ttf 端口首次使用时由 emcc 下载并编译。EM_CACHE 是 emcc 认可的缓存目录覆盖，
# 指到挂载卷里的忽略目录（.cache/emscripten），跨容器运行复用，不必每次重新下载并编译。
if [ -w /work ]; then
    export EM_CACHE="${EM_CACHE:-/work/.cache/emscripten}"
    mkdir -p "$EM_CACHE"
fi
if [ "$1" = "bash" ] || [ -z "$1" ]; then
    echo "== th16 天空璋移植开发环境 =="
    echo "node    $(node --version 2>/dev/null || echo n/a)"
    echo "python  $(python3 --version 2>/dev/null || echo n/a)  oracle: ${TH16_PYTHON}"
    echo "emcc    $(emcc --version 2>/dev/null | head -1 || echo n/a)"
    echo "wasi    ${WASI_SDK_PATH:-n/a}"
    echo "clang   $(clang++ --version 2>/dev/null | head -1 || echo n/a)"
    echo "ghidra  $( [ -d /opt/ghidra ] && echo /opt/ghidra || echo n/a )"
    echo "workdir /work"
fi
exec "$@"
