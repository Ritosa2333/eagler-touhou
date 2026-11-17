#!/usr/bin/env bash
# Build the independent production-Renderer WebGL fixture from this directory.
# Run through `make exec CMD='bash eagler-touhou/ports/th13/tests/render/
# color01-fixture/build.sh'`; this does not touch the game build or artifacts.
set -euo pipefail

here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
port_root="$(cd "$here/../../.." && pwd)"
out="$here/build"
rm -rf "$out"
mkdir -p "$out"

common_include="$port_root/../../common/include"
include_flags=()
if [[ -d "$common_include" ]]; then
  include_flags+=(-I"$common_include")
fi

emcc \
  "$here/fixture.cpp" \
  "$port_root/portable/sdl/Renderer.cpp" \
  -std=c++17 -O2 -g0 -fno-exceptions -fno-rtti -fno-strict-aliasing \
  -ffp-contract=off \
  -DTH_SDL3=1 -DTH_NATIVE_PLATFORM=1 \
  --use-port=sdl3 --use-port=sdl3_ttf \
  -I"$port_root/web/cpp" \
  -I"$port_root/portable/sdl" \
  "${include_flags[@]}" \
  --no-entry -sDEFAULT_TO_CXX=1 \
  -sMODULARIZE=1 -sEXPORT_ES6=1 -sEXPORT_NAME=createColor01Fixture \
  -sENVIRONMENT=web -sINVOKE_RUN=0 -sEXIT_RUNTIME=0 \
  -sALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=1048576 \
  -sINITIAL_MEMORY=67108864 -sMAXIMUM_MEMORY=1073741824 \
  -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 \
  -sGL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=0 \
  -sEXPORTED_FUNCTIONS=_color01_run \
  -sEXPORTED_RUNTIME_METHODS=UTF8ToString \
  -o "$out/fixture.mjs"

cp "$here/fixture.html" "$out/fixture.html"
printf 'fixture: %s\n' "$out/fixture.html"
