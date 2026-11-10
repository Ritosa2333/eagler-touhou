#!/usr/bin/env bash
# Host-side CI entry for the independent production-Renderer WebGL fixture.
# It uses only the repository's sanctioned container targets:
#   dev container: build the fixture with the real Renderer.cpp
#   shot container: run it in Chromium/SwiftShader
set -euo pipefail

here="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
port_root="$(cd "$here/../../.." && pwd)"
outer_root="$(cd "$port_root/../../.." && pwd)"
fixture_rel="eagler-touhou/ports/th13/tests/render/color01-fixture"

if [[ -f /run/.containerenv ]]; then
  echo "run-ci.sh is a host entry; use 'make exec' / 'make shot' inside containers" >&2
  exit 2
fi

make -C "$outer_root" exec CMD="bash $fixture_rel/build.sh"
make -C "$outer_root" shot CMD="node $fixture_rel/run-shot.mjs"
