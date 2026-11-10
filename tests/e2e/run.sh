#!/usr/bin/env bash
# Native 端到端验收构建与运行（容器内 g++，无 SDL/GPU 依赖；同 tests/replay 约定）。
#
# 覆盖面：原版 games/replays/th13_01.rpy 输入流 → 生产标题导航/对局/结算链 →
# 生产 save_replay_slot 导出 → 与原文件整字节比对。全流程 ≈ 11 万帧，
# native -O2 分钟级；run.sh 打印耗时。
#
# 环境变量门槛：
#   TH13_E2E_SKIP=1   只构建不运行（快速回归跳过；退出码仍反映构建结果）。
#
# 资产契约（缺一即硬失败，绝不静默降级）：
#   games/replays/th13_01.rpy                     原版录像（输入流源 + 比对基准）
#   artifacts/reference/raw/                      原版解包资产（ANM/ECL/STD/WAV/thbgm）
#   web/sdl-runtime/assets/msgothic.ttc           对话字体（freetype 真栅格）
#   freetype2 头 + libfreetype                    TTF 桥实现依赖
#
# 用法：tests/e2e/run.sh [--debug]
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
port_dir="$(cd "$script_dir/../.." && pwd)"      # eagler-touhou/ports/th13
outer_dir="$(cd "$port_dir/../../.." && pwd)"    # 外层仓（games/ artifacts/ 所在）
cd "$outer_dir"
[[ "${TH13_E2E_TRACE:-0}" == "1" ]] &&
  echo "trace: script=$script_dir port=$port_dir outer=$outer_dir" >&2

flags="-O2 -Wall -Wextra"
[[ "${1:-}" == "--debug" ]] && flags="-O0 -g"

raw_dir="$outer_dir/artifacts/reference/raw"
font_file="$port_dir/web/sdl-runtime/assets/msgothic.ttc"
replay_file="$outer_dir/games/replays/th13_01.rpy"

for asset in "$replay_file" "$raw_dir/thbgm.fmt" "$outer_dir/games/th13/thbgmogg.dat" \
             "$raw_dir/se_plst00.wav" "$raw_dir/st01.ecl" "$font_file"; do
  [[ -e "$asset" ]] || { echo "tests/e2e/run.sh: missing real asset: $asset" >&2; exit 1; }
done

# freetype2 头：优先系统 pkg-config，回落 emscripten-cache 内的标准头。
ft_cflags="$(pkg-config --cflags freetype2 2>/dev/null || true)"
if [[ -z "$ft_cflags" ]]; then
  ft_header_dir="$outer_dir/artifacts/pr159-fix-20261104/emscripten-cache/sysroot/include/freetype2"
  [[ -f "$ft_header_dir/ft2build.h" ]] || {
    echo "tests/e2e/run.sh: freetype2 headers not found (pkg-config nor $ft_header_dir)" >&2
    exit 1
  }
  ft_cflags="-I$ft_header_dir"
fi
ft_libs="$(pkg-config --libs freetype2 2>/dev/null || true)"
if [[ -z "$ft_libs" ]]; then
  if ldconfig -p 2>/dev/null | grep -q "libfreetype.so "; then
    ft_libs="-lfreetype"
  else
    ft_libs="-l:libfreetype.so.6"
  fi
fi

out_dir="$port_dir/tests/e2e/build"
mkdir -p "$out_dir"

# 游戏源自动发现（game 全量 + sdl 设备层除 BrowserRuntime 外全量）。
sources=()
for source in "$port_dir"/web/cpp/game/*.cpp "$port_dir"/web/cpp/multiplayer/*.cpp; do sources+=("$source"); done
# 共用网络栈（M7）：Session/NetplayRuntime 链接 common/src/netplay。
for source in "$port_dir"/../../common/src/netplay/*.cpp; do sources+=("$source"); done
for name in Application ApplicationTitle ApplicationGame AudioDevice FileStore \
            FontDevice GraphicsDevice SceneDisplay; do
  sources+=("$port_dir/web/cpp/sdl/$name.cpp")
done
sources+=("$script_dir/native/native_stub.cpp")

includes=(
  -I"$port_dir/web/cpp"
  -I"$port_dir/../../common/include"
  -I"$script_dir/native/include"
  -I"$port_dir/web/cpp/multiplayer"
  $ft_cflags
)

status=0
for suite in "$script_dir"/*_test.cpp; do
  name="$(basename "$suite" .cpp)"
  echo "tests/e2e/run.sh: g++ $flags -DTH13_AUDIO_MIXER_ONLY ${#sources[@]} 源 + $name"
  if ! g++ -std=c++17 $flags -fno-exceptions -fno-rtti -ffp-contract=off \
      -DTH13_AUDIO_MIXER_ONLY "${includes[@]}" \
      "${sources[@]}" "$suite" -o "$out_dir/$name" $ft_libs -lm; then
    echo "tests/e2e/run.sh: 套件 $name 构建失败" >&2
    status=1
  fi
done
(( status )) && exit 1

if [[ "${TH13_E2E_SKIP:-0}" == "1" ]]; then
  echo "tests/e2e/run.sh: TH13_E2E_SKIP=1 → 只构建，跳过运行"
  exit 0
fi

for suite in "$script_dir"/*_test.cpp; do
  name="$(basename "$suite" .cpp)"
  bin="$out_dir/$name"
  [[ -x "$bin" ]] || { echo "tests/e2e/run.sh: 缺少 $name 可执行" >&2; status=1; continue; }
  # 运行目录：CWD = 导出落盘点（生产 native 保存路径 fopen("th13_01.rpy","wbx")）。
  run_dir="$out_dir/run"
  rm -rf -- "$run_dir"
  mkdir -p "$run_dir"
  echo "tests/e2e/run.sh: $bin (CWD=$run_dir)"
  start=$SECONDS
  (
    cd "$run_dir"
    TH13_NATIVE_REPO_ROOT="$outer_dir" \
    TH13_NATIVE_ASSET_ROOT="$raw_dir" \
    TH13_NATIVE_FONT="$font_file" \
      "$bin"
  ) || status=1
  echo "tests/e2e/run.sh: $name 耗时 $((SECONDS - start))s"
done
exit $status
