# TH10 Replay Verifier

This is the title-owned TH10 logic regression tester. Its compressed,
content-addressed original-JIT traces are published under `golden/`, so normal
verification does not need the retail executable or the oracle workspace.

- `quick`: the title-owned `demo1 -> demo2 -> demo3 -> demo0` rotation;
- `daily`: complete Marisa B Lunatic and Extra Replays;
- advanced oracle capture: explicit maintenance of expected traces.

Verify the published data:

```powershell
node tools/replay-verifier/verify-golden.mjs
```

The repository does not redistribute `.rpy` files. Fetch the two public long
fixtures from the provenance URLs in `corpus.json` and verify their hashes with:

```powershell
node tools/replay-verifier/fetch-fixtures.mjs
```

After building, packaging and serving the Presentation Lab diagnostic profile,
run the quick lane:

```powershell
python tools/replay-verifier/capture-current-demos.py
node tools/replay-verifier/run-demo-gate.mjs `
  --candidate artifacts\replay-verifier\current-demo\suite.json `
  --report artifacts\replay-verifier\quick-result.json
```

The daily lane consumes `lunatic.json` and `extra.json` plus their sibling row
streams from one capture directory:

```powershell
python tools/replay-verifier/capture-current-replay.py `
  --replay tools\replay-verifier\fixtures\original\th10_ud1aef.rpy `
  --output artifacts\replay-verifier\daily\lunatic.json
python tools/replay-verifier/capture-current-replay.py `
  --replay tools\replay-verifier\fixtures\original\th10_ud1b2a.rpy `
  --output artifacts\replay-verifier\daily\extra.json
node tools/replay-verifier/run-daily-gate.mjs `
  --capture-root artifacts\replay-verifier\daily `
  --report artifacts\replay-verifier\daily-result.json
```

Both gates validate every compressed asset against `golden/manifest.json`
before comparison. Candidate output cannot update expected data.

## Advanced oracle maintenance

This adapter is external diagnostics only. It preserves the title-owned Demo
rotation exactly as `demo1 -> demo2 -> demo3 -> demo0`; numeric sorting would
change observed game behavior and is forbidden.

The existing JIT/Present workflow is the original-provider base. The current
candidate provider uses a `TH_PRESENTATION_AUDIT`-only read-only trace. Run
`capture-current-demos.py` against the Presentation Lab server; it waits for
the title screen to select all four Demos itself and never writes the Demo
index, Replay cursor, input, RNG, scheduler, or gameplay state.

`capture-original-demos.mjs` observes the retail executable at its existing
D3D Present boundary through the historical JIT host. It likewise leaves the
title-owned Demo mechanism untouched.

Pass `--original <suite.json>` to `run-demo-gate.mjs` to use a fresh advanced
capture instead of the published golden. The gate requires
the exact four-entry rotation and all 3000 ticks per Demo; absent providers,
short traces, reordered Demos, or mismatched state all fail closed.

`play-original-replay.mjs --replay <file> --oracle-root <historical-th10_web>`
runs an ordinary clear replay through the original JIT host. It decodes the
file's recorded route and scores, then checks the observed character, shot,
difficulty, stage-entry scores, terminal mode and final score. Early return or
any missing checkpoint fails. This is an original playback checkpoint check,
not a current-Eagler comparison or a certified per-tick golden.

`applicationFramesAfterStartup` is an application-frame budget measurement,
not the number of consumed Replay inputs. The initial menu/entry sequence has
already advanced gameplay when this counter starts.

For an explicit ordinary long Replay oracle capture and one-off comparison:

```powershell
node tools/replay-verifier/play-original-replay.mjs --replay <file> --output <original.json>
python tools/replay-verifier/capture-current-replay.py --replay <file> --output <current.json>
node tools/replay-verifier/compare-replay-captures.mjs --expected <original.json> --actual <current.json> --output <comparison.json>
```

Both collectors append raw rows to a sibling `.rows.jsonl` during execution;
failed or bounded runs therefore retain their verified prefix. The comparator
streams those files, separates contiguous stages, and fails closed on missing
ticks, incomplete lifecycles, field differences, or category-digest changes.
Its `--max-ticks` mode is only a prefix diagnostic and can never return PASS.
Raw rows with a nonzero `sessionFlags` value or a nonpositive stage clock are
retained as stage-lifecycle evidence but are not labeled as completed gameplay ticks. This semantic rule
handles the retail executable's extra Presents during stage loading without
length cropping, frame shifting, or hiding active-play differences.

Presentation Lab imports retail `.rpy` bytes into the ordinary Replay slot.
The `.rpyx` path remains reserved for Replay payloads that actually include a
touch-motion trailer; this diagnostic import does not alter title Demo logic.

Run the completion-check negative tests with:
`node --test tools/replay-verifier/playback-validation.test.mjs`.

Golden changes require review of the Replay hash, original executable/resource
identity, completion evidence, tick count and compressed asset SHA-256. Never
copy candidate output into `golden/`.
