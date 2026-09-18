#!/usr/bin/env bash
set -euo pipefail

CXX="${CXX:-c++}"
OUT="$(mktemp -d "${TMPDIR:-/tmp}/th07-eagler-core-tests.XXXXXX")"
trap 'rm -rf "$OUT"' EXIT

"$CXX" -std=c++20 -Isrc -Ithird_party/eagler-common/include \
  tests/netplay-core-test.cpp \
  third_party/eagler-common/src/netplay/NetplayCore.cpp \
  third_party/eagler-common/src/netplay/NetplayProtocol.cpp \
  third_party/eagler-common/src/netplay/NetplaySession.cpp \
  -o "$OUT/netplay-core-test"
"$OUT/netplay-core-test"

"$CXX" -std=c++20 -Isrc -Ithird_party/eagler-common/include \
  tests/rollback-journal-test.cpp \
  third_party/eagler-common/src/netplay/RollbackJournal.cpp \
  -o "$OUT/rollback-journal-test"
"$OUT/rollback-journal-test"

"$CXX" -std=c++20 -DTH_ENABLE_MULTIPLAYER_GAMEPLAY -Isrc \
  -Ithird_party/eagler-common/include -Ivendored/SDL/include \
  tests/netplay-input-test.cpp third_party/eagler-common/src/netplay/NetplayInput.cpp \
  -o "$OUT/netplay-input-test"
"$OUT/netplay-input-test"
