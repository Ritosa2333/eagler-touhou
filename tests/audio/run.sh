#!/usr/bin/env bash
# Audio PCM diagnostics build/run (container g++ only, no SDL device).
# Auto-discovers tests/audio/*_test.cpp suites and defines
# TH13_AUDIO_MIXER_ONLY so AudioDevice exposes the no-device test entry
# points. Every suite consumes the real original WAV/OGG assets; the tests do
# not substitute synthetic PCM.
# Usage: tests/audio/run.sh [--debug]
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
port_dir="$(cd "$script_dir/../.." && pwd)"
repo_root="$(cd "$port_dir/../../.." && pwd)"
cd "$repo_root"

flags="-O2 -Wall -Wextra"
[[ "${1:-}" == "--debug" ]] && flags="-O0 -g"

out_dir="${TH13_AUDIO_BUILD_DIR:-$port_dir/tests/audio/build}"
mkdir -p "$out_dir"

# Real-asset preconditions. Missing assets are a hard error, never a silent
# fallback to synthesized audio.
for asset in \
  artifacts/reference/raw/se_plst00.wav \
  artifacts/reference/raw/thbgm.fmt \
  games/th13/thbgmogg.dat; do
  [[ -f "$asset" ]] || { echo "tests/audio/run.sh: missing real asset: $asset" >&2; exit 1; }
done

sources=(
  eagler-touhou/ports/th13/web/cpp/game/SoundEffects.cpp
  eagler-touhou/ports/th13/web/cpp/game/MusicLayout.cpp
  eagler-touhou/ports/th13/web/cpp/sdl/AudioDevice.cpp
)

status=0
for suite in "$script_dir"/*_test.cpp; do
  name="$(basename "$suite" .cpp)"
  suite_sources=("${sources[@]}")
  if [[ "$name" == "audio_feedback_production_test" ]]; then
    suite_sources=(eagler-touhou/ports/th13/web/cpp/game/*.cpp eagler-touhou/ports/th13/web/cpp/sdl/AudioDevice.cpp)
  fi
  echo "tests/audio/run.sh: g++ -DTH13_AUDIO_MIXER_ONLY ${sources[*]} $suite"
  g++ -std=c++17 $flags -fno-exceptions -fno-rtti -ffp-contract=off \
    -DTH13_AUDIO_MIXER_ONLY -I eagler-touhou/ports/th13/web/cpp \
    "${suite_sources[@]}" "$suite" -o "$out_dir/$name" -lm || status=1
done
(( status )) && exit 1

for suite in "$script_dir"/*_test.cpp; do
  name="$(basename "$suite" .cpp)"
  echo "tests/audio/run.sh: $out_dir/$name"
  "$out_dir/$name" || status=1
done
exit $status
