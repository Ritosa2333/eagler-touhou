#!/usr/bin/env bash
set -euo pipefail
script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
port_dir="$(cd "$script_dir/../../.." && pwd)"
repo_root="$(cd "$port_dir/../../.." && pwd)"
cd "$repo_root"
out_dir="$(mktemp -d)"
trap 'rm -rf "$out_dir"' EXIT
out="$out_dir/audio-immediate-test"
g++ -std=c++17 -O2 -Wall -Wextra -fno-exceptions -fno-rtti -ffp-contract=off \
  -DTH13_AUDIO_MIXER_ONLY -DTH13_AUDIO_IMMEDIATE_TEST \
  -I "$script_dir/include" -I "$port_dir/web/cpp" \
  "$port_dir/web/cpp/game/SoundEffects.cpp" "$port_dir/web/cpp/game/MusicLayout.cpp" \
  "$port_dir/web/cpp/sdl/AudioDevice.cpp" "$script_dir/route_test.cpp" -o "$out" -lm
"$out"
