# TH09 PC rollback cost optimization — 2026-09-27

This follows `rollback.md`: measure base simulation and complete correction
work, retain gameplay coverage, reduce allocation/copy overhead, then verify
real peers and lifecycle. This candidate has not been deployed. The existing
public DATA/OGG routing and its 404 behavior were not changed.

## State ownership and implementation

- `WorldState` retains owning enemy/effect/attack adapter storage when its ring
  record is reused. Save resets valid-prefix counts; Restore visits only that
  checkpoint's prefix. Cached entries outside it cannot restore stale targets.
  ECL context rebinding and deep-copy ownership are unchanged. Attack state
  clones still use the existing concrete-type clone operation.
- The two fixed Bullet POD pools use two contiguous journal touches instead of
  1,074 per-slot touches. Every slot byte remains covered, including dormant
  slots and sentinels. The diagnostic fingerprint retains independent per-slot
  traversal. The shared `eagler-common` journal continues to own copy and undo.
- Gameplay/RNG/input/Replay state, stateful CPU Draw and first-write hooks retain
  their existing ownership. Historical GPU submission is already suppressed.
  This change adds no display-history storage or prediction policy changes.

## Controlled PC evidence

Chromium 149.0.7827.55, **Intel UHD Graphics / ANGLE D3D11 hardware renderer**,
no CPU throttling. Each run uses the same authored versus fixture (characters
0/1, difficulty 3), 180 warmup ticks, 320 base ticks without network,
snapshots or resimulation, then 200 single-checkpoint corrections of 8 ticks.
Full state fingerprints and framebuffer readbacks run outside timing regions.
The bounded fixture is a CPU-cost comparison, not wall-clock input latency.

Frozen baseline `fe0554c85be7d37f7a09fb9dac2f3cc7ff82e8b63f813126476b06b0e59da386`;
candidate `a5c55efe6867af8acf3e4f91dc422dcc56f8de2349f44a643cfef2342418c83f`.
Order A/B/A/B, serial, with no concurrent compiler or benchmark. Raw reports
are in `th09_web/artifacts/multiplayer-tests/pc-*-report.json` and the portable
record is [rollback-pc-evidence.json](rollback-pc-evidence.json).

| Total per 200 corrections | Baseline mean (ms) | Candidate mean (ms) | Reduction |
| --- | ---: | ---: | ---: |
| Capture | 107.880 | 85.255 | 20.97% |
| Restore | 71.455 | 70.792 | 0.93% |
| Replay 8 ticks | 194.450 | 186.833 | 3.92% |
| Capture + restore + replay | **373.785** | **342.880** | **8.27%** |

Both candidate totals (342.340 / 343.420 ms) beat both baseline totals
(377.115 / 370.455 ms). Initial accounted checkpoint bytes are unchanged.
The separate 320-tick base control averages 35.395 ms before and 37.340 ms
after (+5.5%, with individual candidate runs 34.005 / 40.675 ms); no ordinary
Update/Draw speedup is claimed. The source changes are in checkpoint ownership.
All 800 cross-build gameplay audit samples agree. Every in-process restored
full-state fingerprint and framebuffer agrees. Capture p95 drops from
0.965/0.765 ms to 0.575/0.595 ms. Correction p99 varies across runs; this is
not evidence of a universal worst-case stall reduction.

## Actual rollback driver and transport

The real local WebRTC run uses two peers and a confirmed spectator, audio on,
25–61 ms injected send delay, 1/9 fast-lane loss and an 800 ms spike. This runs
the production ring and real repeated capture/restore driver, unlike the
isolated single-checkpoint cost fixture.

| Hardware RTC observation | Baseline | Candidate |
| --- | ---: | ---: |
| Peer / spectator final frames | 600 / 600 / 600 | 600 / 600 / 600 |
| Resimulated frames, peers | 1714 / 1719 | 1699 / 1796 |
| Callback work p95, peers (ms) | 3.835 / 3.960 | 3.470 / 3.415 |
| Callback work p99, peers (ms) | 4.335 / 4.805 | 3.850 / 4.130 |
| Total run (s, including startup/outage) | 11.896 | 11.737 |

Both runs finish with hash 1357358377 on all three worlds, exactly 600 local
captures per peer and running audio clocks. Peer WASM heaps remain 161,087,488
bytes. This live comparison has different packet schedules and is supporting
evidence, not the controlled percentage claim. Each peer still has one >50 ms
presentation gap during the intentional outage; bounded rollback cannot hide
arbitrarily long missing input.

## Reproduction and limits

Candidate correctness gates passed: WASI session and owning-state tests (all
nine attack types), 900-frame peers against an exact-input reference with equal
Replay bytes and framebuffers, 270 restore checks over 1,800 ticks with a round
reset and characters 2/4, pause/resume, retry/world replacement, local Replay
save, reconnect, real hardware WebSocket fallback and six selected server tests.
Lifecycle and independent-world gates use desktop SwiftShader; the PC cost and
transport measurements above use the verified hardware renderer.

The separately compiled release WASM
`b528129f2031029782e8bee414004fb4f3aa9c7208709471dc925fb566da168e`
also passed hardware RTC with audio and spectator (607 peer frames, 601 spectator
frames, 11.551 seconds). No diagnostic exports are present. Different final
frames prevent treating its final hashes as an equal-frame assertion; periodic
confirmed checks remain active. The local packaged multiplayer Runtime contains
this exact release WASM.

Freeze baseline loader, WASM and build.json in `artifacts/pc-baseline` before
building the candidate. Set `NATIVE_GPU=1`; the harness rejects software fallback.
Set `PC_BUILD` to the frozen directory or candidate `artifacts/sdl3`, and
`RUN_LABEL` to the report prefix. Run `node th09_web/tests/multiplayer/pc-cost-browser.mjs`
in A/B/A/B order. The transport harness accepts the same variables.

The evidence summarizer checks binary identities, hardware renderer, all
cross-build audits, complete gate reports and the measured improvement before
writing its record: `node th09_web/tests/multiplayer/summarize-pc-evidence.mjs`.
These are desktop automated tests; there is no physical
phone, human control-feel, public TURN or new online release claim.
