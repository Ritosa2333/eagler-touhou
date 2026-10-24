#!/usr/bin/env bash
set -euo pipefail

python3 tests/cooperative-rules-test.py

CXX="${CXX:-c++}"
OUT="$(mktemp -d "${TMPDIR:-/tmp}/th07-eagler-core-tests.XXXXXX")"
trap 'rm -rf "$OUT"' EXIT

"$CXX" -std=c++20 -O2 -Isrc tests/lzss-test.cpp src/pbg4/Lzss.cpp -o "$OUT/lzss-test"
"$OUT/lzss-test"

for replay_variant in ordinary multiplayer; do
  replay_flags=()
  replay_sources=(tests/replay-input-test.cpp src/ReplayManager.cpp src/pbg4/Lzss.cpp)
  if [[ "$replay_variant" == multiplayer ]]; then
    replay_flags+=(-DTH_ENABLE_MULTIPLAYER_GAMEPLAY -DTH_ENABLE_NETPLAY)
    replay_sources+=(src/netplay/NetplaySideEffects.cpp
      ../../common/src/netplay/NetplayInput.cpp)
  fi
  "$CXX" -std=c++20 -O2 -flto "${replay_flags[@]}" -Isrc \
    -I../../common/include -Ivendored/SDL/include \
    "${replay_sources[@]}" -o "$OUT/replay-input-$replay_variant"
  "$OUT/replay-input-$replay_variant"
done

python3 audit/source-contracts/netplay-input-repair-contract-test.py
python3 audit/source-contracts/multiplayer-localization-texture-slot-test.py
python3 audit/source-contracts/texture-binding-contract-test.py

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

"$CXX" -std=c++20 -DTH_ENABLE_MULTIPLAYER_GAMEPLAY -Isrc \
  -I../../common/include -Ivendored/SDL/include \
  tests/netplay-input-test.cpp ../../common/src/netplay/NetplayInput.cpp \
  -o "$OUT/netplay-input-test"
"$OUT/netplay-input-test"

"$CXX" -std=c++20 -Isrc -I../../common/include \
  tests/netplay-side-effects-test.cpp src/netplay/NetplaySideEffects.cpp \
  -o "$OUT/netplay-side-effects-test"
"$OUT/netplay-side-effects-test"
