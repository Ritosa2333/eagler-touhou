# TH15 Replay Verifier

This title adapter uses `eagler-common/testkit/replay-verifier` for trace
validation, streaming comparison, stage joining and immutable golden files.
Requires common revision `223d06c` or later for the file-backed adapter.


- `quick`: all three complete stored Demo replay fixtures, in `0 → 1 → 2` order.
- `daily`: Marisa Legacy Lunatic and Extra clear replays.
- `oracle`: explicit original-executable capture, separate from golden acceptance.

The candidate is the **current WASI diagnostic game core**. This does not admit
TH15 to Presentation Lab or certify browser presentation. The Demo lane covers
the full stored input fixtures, not the title's 3900-frame display timeout and
idle/return lifecycle. Daily runs restore each recorded stage from its original
Replay header, execute original and candidate gameplay, and require native and
candidate next-stage or ending completion before joining stages. The isolated
WASI driver binds the production RunCompletion controller, including natural
clear bonuses and player-option retirement. Cross-stage frontend
loading, the ending scene, browser audio/storage and real devices are separate.

Build:

```powershell
$env:WASI_SDK_ROOT = '<wasi-sdk-root>'
node th15_web/scripts/cpp/build.mjs
```

Original inputs remain ignored: the original executable/hash from `target.json`,
the decoded game resources, bounded Unicorn and original vector math library,
and exact replays from `corpus.json`. The workspace's
`tools/maintainer/restore-oracle-inputs.py` restores hash-verified development
delivery inputs. Both public daily fixtures are downloadable from their corpus URLs;
the gate binds SHA-256, character, difficulty, full-clear metadata and route.
The supplied Sanae Extra fixture is not a clear and is not the standard daily case.

```powershell
node tools/replay-verifier/run-oracle.mjs --lane all --output artifacts/replay-verifier/oracle
node tools/replay-verifier/run-gate.mjs --capture-root artifacts/replay-verifier/oracle --report artifacts/replay-verifier/oracle-result.json
```

Use `--case demo0`, `--case marisa-lunatic`, or `--case marisa-extra` for a focused
run. `EAGLER_COMMON_ROOT` overrides the common checkout. Oracle acceleration
omits only the instruction-budget hook. Original ECL/gameplay, natural life and
Replay input remain unchanged. The viewport is initialized to the same logical
geometry on both sides; glyph raster/upload and host resource/GPU/audio outputs
are diagnostic boundaries, following the existing native component oracles.

After reviewing all complete original/candidate cases, accept a new golden set
explicitly. Existing manifests cannot be overwritten:

```powershell
node tools/replay-verifier/run-gate.mjs --capture-root artifacts/replay-verifier/oracle --accept-golden tools/replay-verifier/golden
```

Ordinary candidate checks do not execute the original:

```powershell
node tools/replay-verifier/capture-candidate.mjs --lane quick --output artifacts/replay-verifier/candidate
node tools/replay-verifier/run-gate.mjs --lane quick --capture-root artifacts/replay-verifier/candidate --golden-root tools/replay-verifier/golden --report artifacts/replay-verifier/quick-result.json
```

Use `--lane daily` for the two clears. Advanced fresh-original comparison uses
`--original-root <oracle-directory>` instead of `--golden-root`.

The versioned state schema covers 13 native gameplay words: stage clock, effective Replay input,
exact player position/state, score, deaths, life/Bomb stocks, power, game RNG
seed/calls and enemy count. Visible bullet count, visual RNG and raw pool
allocation slots remain diagnostics. The existing whole-memory world tools are
retained separately: their Reimu/Sanae pool/ANM comparisons currently fail and
are not promoted to an all-owner or full-memory PASS. The new original provider
executes retail gameplay independently of candidate simulation and those
comparison assertions. No provider may drop effective ticks or shift/resynchronize traces to
hide a mismatch. Short runs retain an incomplete receipt and cannot pass.

Fetch the public daily inputs through common:

```powershell
node ../eagler-common/testkit/replay-verifier/fetch-fixtures.mjs --corpus tools/replay-verifier/corpus.json
```

The common adapter API is exported by `adapter.mjs`. Original and candidate
providers can also be run separately with `capture-original.mjs` and
`capture-candidate.mjs`. Original collection supports `--stage N` for an isolated
stage. `join-original.mjs` refuses incomplete/missing stages and preserves every
tick of the complete recorded route. Observation backtraces are optional;
disabling those read-only hooks was independently checked against the full
6269-tick Demo 2 trace without changing retail gameplay.
