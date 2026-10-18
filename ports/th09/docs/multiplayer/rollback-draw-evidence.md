# TH09 historical Draw optimization — 2026-09-27

Status: local candidate following the PC storage optimization. No deployment,
404/resource routing change, or physical-phone claim.

## Four requested investigations

1. **Active regions / collision / targets:** TH09 `AttackAreas` already keeps
   an `active` pointer list and stops at its terminator. `cancel` and `damage`
   do not scan the entire 512-slot pool. TH07 `RebuildBombBoxCache` and TH08's
   former 192-slot scan therefore do not identify the same bottleneck here.
   No collision arithmetic, iteration order or target policy was changed.
2. **Intermediate Draw:** chosen after measurement. Historical ticks retain
   the complete title Draw traversal; only four read-only 2D sprite geometry
   entry points omit CPU vertex construction, clipping and batch submission.
3. **Snapshot separation / first write:** already present for enemy and bullet
   visual pools, with dense independent diagnostics. Mutable text textures use
   first-write capture; the snapshot does not store historical full-screen
   pixels. No additional sparse elision was added in this change.
4. **Storage reuse:** the previous PC change retains owning checkpoint storage
   and batches Bullet POD copies. Its separate measured capture reduction is
   recorded in [PC storage evidence](rollback-pc-evidence.md). Do not add
   percentages from these different experiments.

Primary references: shared `rollback.md`, `performance-rendering.md`,
`replay-determinism.md` and `interpolation.md`. TH06/07 lessons guided ownership;
all numbers below come from TH09.

## Phase diagnosis

Diagnostic baseline WASM:
`a73ac6633f3cb2bfe9e11f0b82ebd0a9cb5e52fb19b6efecdbe9e924d25c9d79`.
Chromium 149.0.7827.55, Intel UHD / ANGLE D3D11 hardware renderer, no throttle.
After warmup, the 320-tick no-network/no-snapshot/no-replay control measured
Update/UI 7.670 ms and Draw 22.575 ms. Across 1,600 replayed ticks it measured
Update/UI 58.610 ms and Draw 162.160 ms: Draw was **73.45%** of those two phases.
Capture and restore of 200 checkpoints separately cost 101.510 / 79.825 ms.
These identify this fixture's bottleneck; they do not establish another game's
or scene's cost proportions. Instrumentation is compiled out of release WASM.

## State boundary

`AnmRenderer::draw_no_rotation`, `draw_2d`, `draw_quad` and `draw_strip` do not
write `AnmVm`, consume RNG, advance timers, allocate/release game resources, or
update gameplay. For unpresented rollback ticks the adapter sets
`omit_sprite_geometry`; their validity checks and return codes remain the same.
Normal/final Draw restores the ordinary path. The switch is an execution-phase
flag, cleared after Draw, not saved game state.

Draw-owned player/effect/attack writes, ASCII queue retirement, stage VM
mutations, text texture updates and `world_matrix` animation matrix/flag updates
still execute. 3D matrix/cache paths are retained because their values can
persist across ticks. This does not claim that the entire Draw graph is pure.

## Paired same-checkpoint test

Candidate diagnostic WASM:
`37458dfbe6ff3dfa7731f1ff559932cdfed4d179040486e2d3ebab9f5caea70d`.
The diagnostic-only control selects the previous full sprite geometry path or
the optimized path in **the same WASM and world**. Each 8-tick block restores
the same initial checkpoint between paths; first/second order alternates.
Seven ticks suppress presentation and the eighth draws normally. Full snapshot
inventory fingerprints (including renderer/platform fields) and exact canvas
PNG strings must match after both paths. No oracle/readback work is timed.

All **800 comparisons** passed over four runs and three character pairs.

| Run (200 blocks each) | Draw reduction | Replay block reduction | Capture + restore + replay reduction |
| --- | ---: | ---: | ---: |
| Characters 0/1, A | 14.64% | 6.60% | 3.75% |
| Characters 0/1, B | 19.21% | 11.78% | 6.74% |
| Characters 2/4 | 11.51% | 12.11% | 6.19% |
| Characters 10/12 | 21.09% | 10.63% | 5.14% |

The last column adds the same measured capture/restore cost to each paired
replay branch; it is a phase-accounted correction total, not a separate atomic
driver invocation. The change affects Draw only. Update and snapshot timings
are not claimed to improve. Some candidate p99/max samples are higher due to
outliers: no universal worst-case stall reduction is claimed.

## Reproduction

The 900-frame independent peer/reference comparison passed with equal full
gameplay audits, final framebuffers and all 1,312 Replay bytes. Characters 6/9
also passed 270 restore checks over 1,800 frames with forced round reset.
Pause/resume, retry/world replacement, local Replay save and reconnect passed.
Hardware RTC and WebSocket fallback finished at 600/600/600 frames with hash
1357358377 on both peers and the spectator; audio remained running.

Release WASM `8a820bfe3bc9693d7521d13371d98ba6bafdb0c95fad656abecc88a922bcf28a`
is packaged locally and passed hardware RTC with audio (607/607 peer frames,
601 spectator frames, 11.650 s). There are no diagnostic exports. Different
final frames mean this release smoke does not assert an equal-frame final hash;
the periodic native confirmed-hash check remains enabled.

### Live RTC ABBA limitation

An additional full/optimized/optimized/full run uses the same diagnostic WASM
and the same transport harness, with `FULL_SPRITE_GEOMETRY=1` selecting old Draw.
All four runs preserve state and complete the 600-frame workload. Packet arrival
and which endpoint bears more replay work still vary:

| Order | Peer resimulated ticks | Peer callback p95 (ms) |
| --- | --- | --- |
| Full A | 1839 / 1550 | 3.355 / 3.015 |
| Optimized B | 1720 / 1710 | 2.995 / 2.875 |
| Optimized B2 | 2204 / 1253 | 4.150 / 3.050 |
| Full A2 | 1879 / 1520 | 3.030 / 2.595 |

**This live result is mixed.** It does not prove an end-to-end FPS or p95
improvement; the accepted gain is the paired same-state replay CPU reduction.
The deliberate network outage still creates >50 ms presentation gaps. None of
the peer callback-work samples exceeds 50 ms in these four runs. Raw tails and
negative comparisons remain in the machine-readable evidence.

Run `node th09_web/tests/multiplayer/phase-cost-browser.mjs` for diagnostic
phase totals. `PC_BUILD` can select the frozen diagnostic baseline directory.
Run `node th09_web/tests/multiplayer/draw-cost-browser.mjs` for paired comparison;
set `CHARACTERS` (default `0,1`) and `RUN_LABEL` for the report name. Hardware
renderer identity is checked, and software fallback fails the comparison.
`BURST` accepts 2/4/8; reported acceptance here uses 8.
After all named reports exist, run
`node th09_web/tests/multiplayer/summarize-draw-evidence.mjs` to validate and
write the portable record.

Raw reports are in `th09_web/artifacts/multiplayer-tests/`. The portable
[machine-readable evidence](rollback-draw-evidence.json) retains measured
samples, build/source identities and regression results. These fixed logical
inputs measure CPU cost; they do not prove end-to-end human input latency.

## Keyboard prediction follow-up

TH09 already predicts discrete held button bits `0xf0`; the four direction keys
also combine into diagonal input. The current policy holds direction for three
missing frames, independently of the eight-frame rollback limit. Direct touch
uses a separate no-new-pointer-override policy. No continuous-vector prediction
was introduced for keyboards.

The `keyboard-prediction.mjs` probe runs the actual shared `RollbackCore` against
four synthetic 2,400-tick keyboard traces with identical 3–6 tick one-way delay
and reorder, sweeping hold limits 1/3/4/6/8. It measures input mismatches only,
without a game world, reconciliation, browser timing or human trace.

| Synthetic trace | Mismatches at hold 3 | Mismatches at hold 6 |
| --- | ---: | ---: |
| Long holds, eight directions | 1813 | 107 |
| Short correction taps | 600 | 1200 |
| Move then release | 1200 | 240 |
| Rapid left/right reversal | 2400 | 1202 |

Thus blindly extending every keyboard prediction is not accepted: the tap
trace doubles its mismatches and post-release held predictions grow from 200
to 800. A future candidate could gate longer prediction on a confirmed stable
hold, but needs release/reversal and real-input evidence. The shipping default
remains three frames. These synthetic numbers are not game-performance gains.
