# TH12 Replay verification

This directory is maintainer tooling, outside the player Runtime and Launcher
protocol. The executable pipeline is implemented; **whole-game acceptance has
not been established**. No test, syntax check, candidate run or original capture
was executed as part of this change.

The remaining prerequisite is substantial: this repository's original provider
currently executes only retail RNG and Replay-decryption functions. It does
not produce complete original gameplay state across the real scheduler. Existing
selected-manager/reverse-engineering probes are partial evidence and cannot be
accepted as full-game goldens. A complete original provider, reviewed completion
observations and its immutable full-state traces still have to be supplied.
The suite reports the specific missing prerequisite; it cannot turn these
subsystem observations or candidate output into expected gameplay values.

## Build boundaries

Ordinary `node scripts/build.mjs` produces `site/runtime/core.*` with
`TH12_DIAGNOSTIC=0`. `node scripts/build-eagler.mjs` additionally forces that
setting and the player output directory, irrespective of ambient diagnostic
environment variables. The package step rejects diagnostic and old unclassified
build manifests.

An explicit diagnostic build, in PowerShell:

```powershell
$env:TH12_DIAGNOSTIC='1'
$env:TH12_THPRAC='1'
node scripts/build.mjs
```

This writes `artifacts/diagnostic-runtime/core.mjs`, `core.wasm` and `build.json`.
`TH12_CORE_OUTPUT_DIR` can select another directory; diagnostic output is refused
under `site/runtime`. The dedicated ABI (`diagnostic_build/state`,
`oracle_fixture`, partial-world resource setters, unsupported counters and RNG
test functions) exists only when the C++ diagnostic macro is enabled. The
ordinary state/pose/motion APIs used by the actual game remain available.

The collector requires both the diagnostic ABI and a diagnostic build manifest,
then checks the actual JS/Wasm byte lengths and hashes against that manifest.
It never enables partial-world fixture flags. Old local harnesses which used
diagnostic setters from the player `site/runtime` now import the separate
diagnostic artifact and need that explicit build first; their former dependency
is not a reason to ship those setters to players.

## Corpus and identities

`scripts/build-corpus.mjs` prepares metadata only. It uses explicit paths, with
no historical sibling-repository or deleted-Replay fallback:

```text
node scripts/build-corpus.mjs --original-exe=<owned-th12.exe> --assets=<raw-runtime-assets> --demos=<demo-directory> --replays-source=<explicit-current-replay-directory> --replays=<fixture-copy-directory> --corpus=<corpus.json> --completions=<reviewed-completions.json>
```

`--replays-source` and `--completions` are optional. Missing completion evidence
is written as `unobserved`, and cannot pass a suite. The tool hashes but does
not execute the pinned EXE. It records every raw stage resource it will load,
the Replay hash/size/provenance/loadout, exact route, seeds, input bounds and
initial headers. Existing Replay files with different bytes are not overwritten.
Do not reuse any Replay the owner has removed from the corpus.

The quick rotation is the retail `demo2.rpy`, `demo1.rpy`, `demo0.rpy`. Daily
requires MarisaB on Lunatic stages1..6 and Extra stage7. A shorter route cannot
be relabelled as daily coverage. Ordinary suites validate Replay bytes against
the manifest and verify all loaded ECL/STD/SHT/MSG/ANM hashes; optional midboss
ANM files cannot enter an unpinned resource set.

`original.resources` is a sorted array of `{file,bytes,sha256}`. The resource
identity is SHA256 of the compact JSON representation of that array, sorted by
ASCII filename. `original.executableSha256` pins the retail executable.
`stateSchemaSha256` is SHA256 of the exact `state-schema.json` bytes. These
identities and the Replay SHA256 must all agree with the golden header.

## Complete streaming traces

Formal gameplay traces use JSONL schema `th12-gameplay-trace/2`: one header,
one tick record for **every consumed recorded input**, and one completion footer.
Streaming avoids single-JSON size limits on long daily runs. `compare-traces.mjs`
also retains the older `/1` JSON comparator for independent short fixtures;
those fixtures do not establish formal complete-route acceptance.

The header includes `identity`, `fixture` and `provider`. `fixture` is
`{id,character,shot,difficulty,practice,route}`; each route row contains
`{number,seed,frames,inputTicks,sentinel}`. A sentinel is accepted only at the
declared final input triple. It is not advanced as a simulation tick. All stages
are loaded in the Replay file's recorded order and restored atomically using
the normal `startReplayStage` path, including spell-time metadata and full-game
Replay mode. The collector does not invent continuous/carry inputs between
recorded stage headers or treat independent stage tests as a full route.

Each tick has monotonically contiguous global `frame`, `stageNumber`, stage
`inputFrame`, core `logicFrame`, and all nine required state categories. The
complete snapshot's player/RNG/ECL/enemy/bullet/item/economy fields are copied
directly; stage state and flow are retained; **every other snapshot key** is
preserved under `stage.other`, including animation internals, background camera,
faults and auxiliary state. No sampling interval, field exclusion, epsilon,
reordered array, display-derived clock or frame offset is used.

The provider samples after `_th12_tick` has returned from the complete
authoritative update, before the browser loads the next stage's resources.
Stage-clear teardown performed inside that authoritative tick is included;
the recorded clear-interval ticks are retained. `ended1` is not treated as an
early game-over while the full-game flow is still clearing the stage. It does
not call the renderer, advance a display sampler,
inject wall-clock time or consume audio while collecting. The existing
`captureAnimationDraw`, legacy draw and custom-mesh getters only build output
buffers from core state; they do not advance authority. The collector uses
dedicated read-only snapshots rather than relying on a presentation call to
make state current. This code-path assessment has not been executed as an
observational-equivalence test in this change.

Completion rules are reviewed original observations, never candidate-derived.
`--completions` maps fixture ids to this exact footer shape:

```json
{
  "record": "completion",
  "complete": true,
  "ticks": 123,
  "stages": [{"number": 1, "inputTicks": 123, "ended": 0, "flowStatus": "playing"}],
  "outcome": "reviewed-original-outcome-label"
}
```

The numbers above illustrate the schema, not an accepted observation. Tick count,
stage route and each actual terminal state must agree with the Replay and
original evidence. An early game-over/transition fails rather than cropping the
tail. A failed/diverged run may leave an explicitly incomplete candidate file;
that file cannot pass validation or be accepted as a golden.

## Reviewed original acceptance

Full-state original proposals must use the same schema and provide
`provider.kind: "original-full-state"`, the documented after-tick sampling
point, and `provider.coverage` listing input, player, RNG, ECL, enemies, bullets,
items, economy, stage and animations. These declarations must be backed by a
maintainer's review of the real original provider; string labels alone are not
evidence that a partial provider has become complete.

Once that missing provider and reviewed original traces exist, acceptance is a
separate explicit operation:

```text
node tools/replay-verifier/accept-gameplay-golden.mjs --reviewed-original=1 --proposal=<original-full-state.jsonl> --corpus=<corpus.json> --assets=<assets> --replays=<replays> --goldens=<gameplay-golden-index.json>
```

It validates the entire proposal and reviewed completion, copies it exclusively
to `golden/<sha256>.jsonl`, rechecks the stored bytes, and writes an index binding
fixture id, SHA256 and byte length. Existing object bytes are immutable. Changing
an index entry requires `--replace=<previous-sha256>`; old objects are retained.
The command never converts candidate traces or subsystem proposals into full
original evidence. Private golden storage is supported through `--goldens`;
keeping owned original traces outside Git is not itself a conformance failure.

## Ordinary quick/daily gates

```text
node tools/replay-verifier/check-gameplay.mjs quick --corpus=<corpus.json> --assets=<assets> --replays=<replays> --runtime=<diagnostic-core.mjs> --goldens=<gameplay-golden-index.json> --output=<reports-directory>
node tools/replay-verifier/check-gameplay.mjs daily --corpus=<corpus.json> --assets=<assets> --replays=<replays> --runtime=<diagnostic-core.mjs> --goldens=<gameplay-golden-index.json> --output=<reports-directory>
```

The existing `npm run verify:quick` / `verify:daily` call those same entry points.
Defaults remain repository-relative; all asset/Replay/build/golden/report paths
can be supplied separately. Environment alternatives are `TH12_ASSET_DIR`,
`TH12_REPLAY_DIR`, `TH12_DIAGNOSTIC_RUNTIME`, `TH12_REPLAY_CORPUS`,
`TH12_GAMEPLAY_GOLDENS` and `TH12_VERIFIER_OUTPUT`.

The gate checks immutable expected identity, full route, shape and completion
before running a candidate. Candidate state is compared exactly as it arrives,
so the report records the earliest observed field divergence. Only a complete
run with all terminal rules matched can pass. Exit0 means pass,1 means observed
divergence,2 means missing prerequisites or candidate/format failure. The gate
never runs an original process, accepts a proposal or overwrites expected data.
`suiteDeterminism` describes only the declared complete Replay routes; a pass
does not assert every loadout, all original menus or pixel/audio equivalence.

## Existing subsystem lane

The advanced original-only command remains deliberately limited:

```text
python tools/replay-verifier/capture-original.py --exe=<owned-th12.exe> --replay-dir=<explicit-fixtures> --output=<proposal-directory>
```

It runs pinned retail RNG16/RNG32 and Replay decryption in Unicorn. Optional
`--python-packages` locates locally installed dependencies. It does not sample
full gameplay and prints that limitation in its result. Subsystem acceptance is
`accept-subsystem-golden.mjs`; `check-subsystems.mjs` uses the separate diagnostic
Runtime and supports the shared path arguments plus `--subsystem-index`.

No full-game golden, completion observation or passing acceptance was fabricated
or regenerated during this implementation.
