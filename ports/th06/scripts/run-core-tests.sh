#!/usr/bin/env bash
set -euo pipefail

python3 tests/cooperative-rules-test.py

CXX="${CXX:-c++}"
OUT="$(mktemp -d "${TMPDIR:-/tmp}/th06-eagler-core-tests.XXXXXX")"
trap 'rm -rf "$OUT"' EXIT

REPLAY_SOURCES=(
  tests/replay-input-test.cpp src/ReplayManager.cpp src/Gui.cpp src/ZunTimer.cpp
  src/Chain.cpp src/LocalizationStub.cpp src/PracticeRuntimeStub.cpp
)
for replay_variant in ordinary multiplayer; do
  replay_flags=()
  replay_sources=("${REPLAY_SOURCES[@]}")
  if [[ "$replay_variant" == multiplayer ]]; then
    replay_flags+=(-DTH_ENABLE_MULTIPLAYER_GAMEPLAY -DTH_ENABLE_NETPLAY)
    replay_sources+=(src/netplay/NetplaySideEffects.cpp
      ../../common/src/netplay/NetplayInput.cpp)
  fi
  # LTO removes unused game entry points; the test links the real Replay and
  # dialogue owners with device/rendering effects stubbed, without SDL libs.
  "$CXX" -std=c++20 -O2 -flto "${replay_flags[@]}" -Isrc \
    -I../../common/include -Ivendored/SDL/include \
    "${replay_sources[@]}" -o "$OUT/replay-input-$replay_variant"
  "$OUT/replay-input-$replay_variant"
done

"$CXX" -std=c++20 -Isrc -I../../common/include \
  tests/netplay-core-test.cpp \
  ../../common/src/netplay/NetplayCore.cpp \
  ../../common/src/netplay/NetplayProtocol.cpp \
  ../../common/src/netplay/NetplaySession.cpp \
  -o "$OUT/netplay-core-test"
"$OUT/netplay-core-test"

"$CXX" -std=c++20 -Isrc -I../../common/include \
  tests/rollback-journal-test.cpp \
  ../../common/src/netplay/RollbackJournal.cpp \
  -o "$OUT/rollback-journal-test"
"$OUT/rollback-journal-test"

"$CXX" -std=c++20 -Isrc \
  tests/gameplay-session-test.cpp src/multiplayer/GameplaySession.cpp \
  -o "$OUT/gameplay-session-test"
"$OUT/gameplay-session-test"

"$CXX" -std=c++20 -DTH_ENABLE_MULTIPLAYER_GAMEPLAY -Isrc \
  -I../../common/include -Ivendored/SDL/include \
  tests/netplay-input-test.cpp ../../common/src/netplay/NetplayInput.cpp \
  -o "$OUT/netplay-input-test"
"$OUT/netplay-input-test"

"$CXX" -std=c++20 -Isrc \
  tests/netplay-side-effects-test.cpp src/netplay/NetplaySideEffects.cpp \
  -o "$OUT/netplay-side-effects-test"
"$OUT/netplay-side-effects-test"
