#!/bin/bash
set -e
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
