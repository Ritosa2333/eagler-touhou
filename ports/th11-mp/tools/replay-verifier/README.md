# TH11 Replay Verifier

TH11 adapts `eagler-common/testkit/replay-verifier`. Common owns streaming
records, lifecycle validation, comparison, stage joining and immutable golden
files. This directory owns TH11 input selection and state observation.
Requires common revision `223d06c` or later for the file-backed adapter.


- `quick`: all four stored Demo replays in the title order `3 → 2 → 1 → 0`.
- `daily`: Reimu/Yukari Lunatic full campaign and Extra clear.
- `oracle`: explicit original-executable capture. It never replaces golden data.

The candidate is the **current WASI diagnostic game core**, not a Browser Runtime
or Presentation Lab build. The Demo lane consumes each stored Demo to its native
terminal record. Title idle, the interactive 3600-frame Demo timeout, title
return, browser rendering, audio and persistence are separate acceptance gates.

Build the diagnostic core with the existing local WASI SDK:

```powershell
$env:WASI_SDK_ROOT = '<wasi-sdk-root>'
node th11_web/scripts/cpp/build.mjs
```

Original capture requires the version/hash in `th11_web/target.json`, the matching
`th11.dat`, decoded `reference/assets`, public long replays from `corpus.json`,
the title's bounded Unicorn dependency and native math resources. These are
ignored local inputs. A local development delivery can restore them through
the workspace's `tools/maintainer/restore-oracle-inputs.py`; it validates every
input against `FILE-SHA256.json` before copying.

```powershell
node tools/replay-verifier/run-oracle.mjs --lane all --output artifacts/replay-verifier/oracle
node tools/replay-verifier/run-gate.mjs --capture-root artifacts/replay-verifier/oracle --report artifacts/replay-verifier/oracle-result.json
```

Individual capture: add `--case demo3`, `--case reimu-a-lunatic`, or
`--case reimu-a-extra`. `EAGLER_COMMON_ROOT` selects a different common checkout.
The original tool runs original manager/ECL/gameplay code and native stage
transitions. File, GPU, audio and glyph-output boundaries are the existing
title oracle's diagnostic replacements. Only the instruction-budget hook is
disabled for speed; original calculations and every gameplay comparison remain.

After reviewing a complete passing oracle suite, explicitly create a new golden
directory. An existing manifest cannot be overwritten:

```powershell
node tools/replay-verifier/run-gate.mjs --capture-root artifacts/replay-verifier/oracle --accept-golden tools/replay-verifier/golden
```

Ordinary checks do not run the original executable:

```powershell
node tools/replay-verifier/capture-candidate.mjs --lane quick --output artifacts/replay-verifier/candidate
node tools/replay-verifier/run-gate.mjs --lane quick --capture-root artifacts/replay-verifier/candidate --golden-root tools/replay-verifier/golden --report artifacts/replay-verifier/quick-result.json
```

Use `--lane daily` for both clears. For an advanced comparison against freshly
captured original files, use `--original-root <oracle-directory>` instead of
`--golden-root`.

Coverage: effective held/pressed input, 12 economy fields, byte-exact player
motion, gameplay RNG seed/calls, enemy count and active bullet count. The explicit
oracle run also retains the existing deeper player lifecycle, enemy, item and
stage-clock assertions. This is selected-field proof, not an all-owner fingerprint.

The all-FFFF Replay termination marker is lifecycle evidence, not a completed
simulation tick. It is handled before either side's partial shutdown can enter
the common trace. Missing/extra/reordered effective ticks fail without cropping,
frame shifts or resynchronization. Failed captures retain their observed prefix.

The common adapter API is exported by `adapter.mjs`. Fetch public daily fixtures
with common's `fetch-fixtures.mjs --corpus tools/replay-verifier/corpus.json`.
The TH11 advanced original session retains the native/candidate cross-checks
from the existing title world oracle. Ordinary candidate sessions run alone.
