#!/usr/bin/env bash
# scoreth13.dat 格式孪生与 FileStore 得分面单测（容器内 g++；仿 tests/replay/run.sh）。
# 自动发现 tests/score/*_test.cpp 套件；用 --debug 切 -O0 -g。
# 资产依赖（gitignored，本地/容器须先就位，缺位时相应用例如实失败）：
#   artifacts/score_golden/*.bin       黄金档（Unicorn 原版代码产出）
#   artifacts/score_golden/*.image.bin 黄金未压缩镜像（外层 oracle/scorefmt.py 解出）
# 用法：tests/score/run.sh [--debug]
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
port_dir="$(cd "$script_dir/../.." && pwd)"
repo_root="$(cd "$port_dir/../../.." && pwd)"
cd "$repo_root"

flags="-O2 -Wall -Wextra"
[[ "${1:-}" == "--debug" ]] && flags="-O0 -g"

out_dir="$port_dir/tests/score/build"
mkdir -p "$out_dir"

# 游戏源自动发现 + FileStore/ScoreDat（sdl 层仅 FileStore 依赖，无 SDL/EM 依赖时
# 原生可编）。
sources=(eagler-touhou/ports/th13/web/cpp/game/*.cpp
         eagler-touhou/ports/th13/web/cpp/sdl/FileStore.cpp)

status=0
for suite in "$script_dir"/*_test.cpp; do
  name="$(basename "$suite" .cpp)"
  g++ -std=c++17 $flags -fno-exceptions -fno-rtti -ffp-contract=off \
    -Ieagler-touhou/ports/th13/web/cpp \
    "${sources[@]}" "$suite" \
    -o "$out_dir/$name" || status=1
done
(( status )) && exit 1

for suite in "$script_dir"/*_test.cpp; do
  name="$(basename "$suite" .cpp)"
  "$out_dir/$name" || status=1
done
exit $status
