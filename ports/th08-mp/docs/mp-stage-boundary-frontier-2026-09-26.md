# TH08MP Stage Clear capture reuse and frontier snapshots

This follow-up addresses the production Stage 1 Clear screenshot showing
`captured input send failed`, and the report of mobile dense-bullet stalls.
It follows `eagler-touhou/docs/playbooks/rollback.md`. TH06/TH07 experience is
the source of the frontier idea; the acceptance evidence below is TH08-specific.

## Reproduced failure and fix

Late Shoot input accelerates the native Stage Clear clock. A correction can
therefore reach a stage/resource boundary before the predicted timeline did.
`EndCorrection(true)` shortens the timeline, but previously captured local
inputs remain immutable and available. During loading, `Step` reuses them and
calls `NetworkConnection::Captured` again. The shared channel correctly rejects
an announcement older than its highest capture, producing the reported error.

The TH08 adapter now recognizes an already captured older frame and leaves
retransmission to the channel pump. It neither resamples physical input nor
rewinds the send frontier. Missing captures and existing channel failures still
fail. No shared protocol or transport implementation changed.

Evidence:

- The WASI regression fails with HEAD's original `NetworkConnection.cpp` at
  `connection.Captured(frame)` after correction, and passes with the candidate
  for both 2P and 3P. It checks immutable capture reuse, no extra old-frame send,
  subsequent fresh input, and genuine error propagation.
- The browser regression uses real local RTC and delays binary input packets
  to P2. A fixture initializes the native Stage Clear clock near its end;
  native Update then selects the next stage. It asserts that P2's correction
  actually shortens the predicted timeline, then advances through frame 300.
  Both 2P and 3P reach stage index 1, have no driver/channel failure and match
  all canonical world hashes. This is a targeted boundary test, not an entire
  played Stage 1 or a remote mobile run.

## Safe first performance step

Forward ticks and resimulation ticks in the contiguous confirmed input prefix
cannot be rewound by later valid input. They now skip world/texture checkpoints
and discard obsolete history before native first-write hooks run. Unconfirmed
frames retain the existing complete checkpoint. Native Update, authored Draw,
input sampling, output confirmation and replay handling retain their semantics.
There is no added local input delay or reduced bullet/collision simulation.

The diagnostic build has an `always` policy for comparison and capture,
restore, Update, Draw, checkpoint count and byte counters. These controls and
counters are excluded from the production build. The dense fixture emits
1,000 stationary native bullets and uses continuously changing analog input
with four-frame delayed delivery. Measurements cover frames 180–599, after
startup. Endpoints share a Chromium process using SwiftShader; runs are serial.

| Run | Policy / seat | Checkpoints | Skipped | Capture ms | Captured bytes |
| --- | --- | ---: | ---: | ---: | ---: |
| 2P | always / P1 | 1,232 | 0 | 1,407.2 | 10,514,782,864 |
| 2P | frontier / P2 | 1,008 | 224 | 1,169.5 | 8,603,056,960 |
| 2P reversed | frontier / P1 | 1,008 | 224 | 1,177.2 | 8,603,056,960 |
| 2P reversed | always / P2 | 1,232 | 0 | 1,402.9 | 10,514,782,864 |
| 3P | always / P1 | 1,232 | 0 | 1,532.2 | 11,709,859,964 |
| 3P | frontier / P2 | 1,008 | 224 | 1,276.6 | 9,580,798,896 |
| 3P | frontier / P3 | 1,008 | 224 | 1,303.4 | 9,580,798,896 |

All three runs matched canonical checkpoints, performed 203 corrections per
endpoint and retained 1,000 active bullets. The 2P comparisons remove 18.2% of
checkpoint count/bytes and measure 16.1–16.9% less capture time. Captured bytes
are cumulative copied bytes, not simultaneous memory use. These results do
not establish phone FPS, frame-time tails or elimination of the reported lag.

Full bullet records remain costly. Contiguous-run storage was considered but
not adopted: the current journal rejects partially overlapping touches, and
native slot reuse must still preserve first-write ownership. That requires a
separate storage change and exact whole-pool validation, not just combining
the capture calls. Live-part storage and multi-tick checkpoints are not part
of this candidate.

## Validation and reproduction

- 900-frame 2P ordinary-death regression, four-frame delay, analog input,
  different visual settings and two extra presentations: PASS, no divergence.
- Exact GPU image and CPU texture restoration/reuse: PASS in 2P and 3P.
- Existing native frame-zero, corrected-frontier, atomic packet and generation
  tests: PASS.
- Production build: PASS; all 482 manifest source hashes match the working
  tree, diagnostic flag is false and no `mp_fixture_` exports are present.
  WASM SHA-256: `30c5e0ecafc920972958348dc70c900b7b7db5dc15a6f5eefec8bf1283e2e930`.
- The production artifact also passes a 600-frame 2P, four-frame delayed analog
  input test, with 203 corrections per endpoint and no canonical divergence
  (`artifacts/multiplayer-tests/frontier-production.json`).
- No TH06, TH07, TH10, shared runtime or launcher source changes.
- At initial validation this follow-up was local and uncommitted. The user
  subsequently requested deploying this exact production build; see
  `mp-stage-boundary-deployment-2026-09-26.md`. Later live-pool/early-send
  experiments are excluded from that deployment.

From the TH08 multiplayer worktree, with the workspace toolchains configured:

```powershell
node portable/check-multiplayer-capture-boundary.mjs --baseline # expected failure
node portable/check-multiplayer-capture-boundary.mjs
node portable/check-multiplayer-netplay.mjs
node portable/build.mjs --th08 --multiplayer-fixtures
python portable/multiplayer/check-stage-boundary.py --url <fixture-origin> --relay <relay-url> --players 2 --output artifacts/multiplayer-tests/stage-boundary-2p.json
python portable/multiplayer/check-stage-boundary.py --url <fixture-origin> --relay <relay-url> --players 3 --output artifacts/multiplayer-tests/stage-boundary-3p.json
python portable/multiplayer/check-desync.py --url <fixture-origin> --frames 600 --delay 4 --analog --dense-bullets 1000 --snapshot-policy mixed --output artifacts/multiplayer-tests/frontier-dense-mixed.json
python portable/multiplayer/check-desync.py --url <fixture-origin> --frames 600 --delay 4 --analog --dense-bullets 1000 --snapshot-policy mixed-reverse --output artifacts/multiplayer-tests/frontier-dense-reverse.json
python portable/multiplayer/check-desync.py --url <fixture-origin> --players 3 --frames 600 --delay 4 --analog --dense-bullets 1000 --snapshot-policy mixed --output artifacts/multiplayer-tests/frontier-dense-mixed-3p.json
python portable/multiplayer/check-desync.py --url <fixture-origin> --frames 900 --delay 4 --ordinary-death 1 --presentations 2 --guest-config visual --analog --output artifacts/multiplayer-tests/frontier-death-visual.json
python portable/multiplayer/check-texture-history.py --url <fixture-origin> --output artifacts/multiplayer-tests/frontier-texture-history.json
node portable/build.mjs --th08 --multiplayer --resource-trace
```

Use the existing smoke server with `TH08_MP_FIXTURES=1`, local DATA and fonts,
and the launcher's local relay. Resource tracing remains opt-in. JSON evidence
and build logs are local under `artifacts/multiplayer-tests/`.
