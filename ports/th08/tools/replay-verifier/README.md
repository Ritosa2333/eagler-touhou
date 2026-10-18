# TH08 Replay Verifier

This is the title-owned TH08 logic regression tester. Published, content-addressed
original-JIT traces live in `golden/`; ordinary checks run only the current
diagnostic Browser Runtime and do not require the retail executable or the
historical oracle workspace.

The supported lanes are:

- `quick`: all four title Demos, intended for frequent development checks;
- `daily`: the complete Magic Team Lunatic Final B and Extra Replays;
- advanced oracle capture: explicitly regenerate source traces from the retail
  JIT environment. It never updates the published golden set automatically.

Verify that the checked-in golden set is intact:

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
node tools/replay-verifier/run-current-candidate-gate.mjs `
  --capture-root artifacts\replay-verifier\current-demo `
  --report artifacts\replay-verifier\quick-result.json
```

The daily lane consumes `lunatic.json` and `extra.json` plus their sibling row
streams from one capture directory:

```powershell
python tools/replay-verifier/capture-current-replay.py `
  --replay tools\replay-verifier\fixtures\original\th8_ud2bec.rpy `
  --start-stage 0 --output artifacts\replay-verifier\daily\lunatic.json
python tools/replay-verifier/capture-current-replay.py `
  --replay tools\replay-verifier\fixtures\original\th8_ud2bf9.rpy `
  --start-stage 8 --output artifacts\replay-verifier\daily\extra.json
node tools/replay-verifier/run-daily-gate.mjs `
  --capture-root artifacts\replay-verifier\daily `
  --report artifacts\replay-verifier\daily-result.json
```

Both gates verify every compressed golden asset against `golden/manifest.json`
before comparison. A candidate build cannot write or bless expected data.

## Advanced oracle maintenance

Pass `--oracle-root` to the quick gate to compare directly with the historical
source artifacts. Ordinary long Replay capture uses:

Run `portable/presentation-lab/prepare.py`, build/package the TH08 Presentation
Lab profile, and start `portable/presentation-lab/serve.mjs` first. The capture
emits a Replay tick only when calculation finishes with gameplay still owning
both the active and requested scene. This excludes the browser shell's
draw/present after a calculation-chain `Break` requests a scene transition;
the original executable has no such half-transaction because its logic and
Present boundary are integrated.

The gate does not trim the candidate to the oracle length or Replay input
bounds. Extra, missing, or incomplete candidate ticks remain comparison
failures.

```powershell
node tools/replay-verifier/capture-original-replay.mjs --replay <file> --start-stage <0-or-8> --output <original.json>
```

Golden changes require review of the Replay hash, original executable/resource
identity, completion evidence, tick count and compressed asset SHA-256. Do not
copy candidate output into `golden/`.

Each provider appends raw rows to a sibling `.rows.jsonl` while it runs, so a
timeout or crash retains the completed prefix. `--max-ticks` on the comparator
is a diagnostic prefix check and deliberately ends as `INCOMPLETE`; only two
complete captures can produce `PASS`. Contiguous stages become separate trace
segments. Raw Present observations are retained, but the common trace contains
only rows where the gameplay clock advances. Retail and the fixed-tick browser
driver have different Present cadence while that clock is frozen; any RNG
consumed there remains observable at the next gameplay tick. The adapter also
removes only `game_flags & 0x800`, because that bit records the dialogue route
already represented by the observed Replay stage and imported stages bypass
the original dialogue selection. All other flags stay authoritative.

The browser collector validates the complete ordered stage list and the final
score stored in the Replay before accepting a capture as complete. It runs only
the isolated diagnostic build and does not modify normal gameplay or the
built-in Demo rotation.
