#!/usr/bin/env bash
set -euo pipefail

CXX="${CXX:-c++}"
OUT="$(mktemp -d "${TMPDIR:-/tmp}/th06-eagler-core-tests.XXXXXX")"
trap 'rm -rf "$OUT"' EXIT

"$CXX" -std=c++20 -Isrc \
  tests/netplay-core-test.cpp \
  src/netplay/NetplayCore.cpp src/netplay/NetplayProtocol.cpp src/netplay/NetplaySession.cpp \
  -o "$OUT/netplay-core-test"
"$OUT/netplay-core-test"

"$CXX" -std=c++20 -Isrc \
  tests/rollback-journal-test.cpp src/netplay/RollbackJournal.cpp \
  -o "$OUT/rollback-journal-test"
"$OUT/rollback-journal-test"

"$CXX" -std=c++20 -Isrc \
  tests/gameplay-session-test.cpp src/multiplayer/GameplaySession.cpp \
  -o "$OUT/gameplay-session-test"
"$OUT/gameplay-session-test"

"$CXX" -std=c++20 -DTH_ENABLE_MULTIPLAYER_GAMEPLAY -Isrc -Ivendored/SDL/include \
  tests/netplay-input-test.cpp src/netplay/NetplayInput.cpp \
  -o "$OUT/netplay-input-test"
"$OUT/netplay-input-test"
