# TH08 rollback smoothness measurements

The previous stationary-bullet, stopped-loop test measured snapshot work. It
did **not** establish end-to-end smoothness. This follow-up implements the
relevant measurement lanes from `eagler-touhou/docs/playbooks/rollback.md`.

## Method

- Freeze the diagnostic WASM/loader, source inventory, smoke HTML/JS, measurement
  hook, runner, DATA, fonts, RTC impairment tool, relay and its dependencies.
  SHA-256 manifests are checked before and after every run. The WASM used here
  is `d56e3866d36c349e891e9c34d5bf184f9d2eb0c16c7ff202965e380c48208487`.
- Each peer has a separate Chromium process. Drive the native SDL rAF loop,
  not a burst of manually stepped simulation frames. Record the actual GPU.
- Use real local RTC. Impair both fast-input and reliable repair-input sends:
  38.5 ms one-way plus seeded +/-5 ms jitter. This approximates requested
  77 ms RTT +/-10 ms at the application-send layer, **not wire RTT**. Reject
  runs without matched/sent packets or with impairment queue errors.
- Boot both peers and confirm frame 179 before starting the measured workload.
  Throttle P2 via CDP at 4x; P1 remains unthrottled. This is a laboratory load,
  not an Android device model.
- The dense lane emits 1,000 native stationary bullets at frame 180; frames
  240–599 are measured. Continuous frame-indexed analog input forces actual
  prediction mismatches. Every mode consumes the same captured inputs.
- Run serially, with no concurrent build/benchmark. Reverse mode order in the
  second round. Retain each raw report and failed run. Later runner revisions
  add a worker watchdog that terminates the browser process tree on timeout.
- Confirm the ending frame and compare complete canonical hashes between
  peers. Compare final hashes between modes too. Measurements here agree at
  the warmup boundary and the final frame; the earlier packet regression also
  checks intermediate canonical checkpoints.

The two production-policy comparisons are `always` (prior checkpoint policy)
and `frontier` (candidate confirmed-prefix elision). Both retain prediction and
rollback. Test-only `exact` waits for remote input with no snapshots;
`exact-snapshots` does the same while retaining checkpoints. These exact
controls are unbuffered lockstep, so their lower logical FPS must **not** be
compared to the full-rollback modes as an optimization result. They establish
that zero-correction/zero-snapshot and snapshot-only lanes really execute.
They are not proposed product settings.

## Hardware GPU, uncapped presentation, two rounds

Chromium's full-browser headless mode (`--browser-channel chromium`) reports
Intel UHD Graphics through ANGLE/D3D11. The default headless-shell experiment
reports SwiftShader and is kept separately. Do not combine those populations.
An unthrottled hardware-GPU pilot reached about 59 logical FPS with 1,000
bullets and no Present-producing callback gap above 50 ms.

P2 below is the 4x-throttled peer. Each row measures 360 logical advances.

| Run order | Policy | Logical FPS | Present-gap p95 ms | p99 ms | Gaps >50 ms | Capture ms |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| Round 1, first | always | 35.05 | 64.0 | 77.5 | 31 | 2341.1 |
| Round 1, second | frontier | 37.81 | 57.5 | 76.0 | 36 | 740.3 |
| Round 2, first | frontier | 28.27 | 72.1 | 86.6 | 36 | 352.1 |
| Round 2, second | always | 28.85 | 60.2 | 108.3 | 38 | 2789.2 |

The capture reduction is real, but a stable logical-throughput or tail-latency
win is **not established**. Round 1 improves average throughput; round 2 does
not. All remain far below 60 logical FPS under this load. Treat frontier as a
correctness-tested candidate, not a completed mobile smoothness fix.

P1 replays 1,683–2,004 historical frames for 360 forward advances, about
7.3–7.5 historical frames per correction. P2's largest correction takes
58.7–98.6 ms across these runs. Requested 38.5 ms send timers actually average
75.0–91.6 ms on P2 during the measured intervals. This exposes both expensive
correction bursts and event-loop delay; it does not establish a wire-network
fault. The next performance investigation should separate peer lead/pacing,
native update cost and correction scheduling, rather than treating capture
time alone as the remaining problem.

The exact controls have zero corrections. `exact` has zero checkpoints;
`exact-snapshots` creates 360 measured checkpoints. Their final canonical
hashes match the full-rollback runs. In uncapped mode they redraw repeatedly
while waiting, so callback CPU per logical frame includes a different number
of presentations. A 60 Hz limit lane is used to separate that effect.

## Reused wall-clock native-touch workload

`--wall-clock-touch` adapts TH07's existing `startBridgeDrag` stimulus from
`tests/netplay-performance-browser-host.html`: a 2.5-second sinusoid, 4.5%
canvas amplitude and 0.8-radian seat offset. It sends down/move/up into TH08's
native SDL touch entry point, so the Runtime owns actual once-only capture.
The gesture continues with wall time even when simulation stalls. Event count,
path length and actual player movement must be nonzero. This is a native-touch
adapter test, not physical touchscreen or Launcher bridge latency acceptance.

That fixes a limitation of the earlier frame-indexed cost lane: slow simulation
also slowed its input stimulus. Keep that older lane as a deterministic cost
control; prefer this adopted wall-clock workload for responsiveness stress.
Because the actual captured stream depends on event timing, compare canonical
hashes **between peers in a run**, not between separate policy runs.

Hardware GPU, 4x P2, uncapped presentation, same 1,000 bullets:

| Run order | Policy | P2 logical FPS | Present p99 ms | Gaps >50 ms |
| --- | --- | ---: | ---: | ---: |
| Round 1, first | always | 39.24 | 71.0 | 17 |
| Round 1, second | frontier | 41.0 | 68.2 | 20 |
| Round 2, first | frontier | 43.8 | 66.9 | 17 |
| Round 2, second | always | 35.32 | 71.0 | 35 |

All four runs pass per-peer canonical agreement. The benefit is directional;
neither throughput nor tail latency meets the smoothness target yet.

## Native Hard Stage 1 and 60 Hz controls

Without injected bullets, native Hard Stage 1 through frame 1799, capped to
60 Hz presentation and with P2 throttled 4x:

| Policy | P2 logical FPS | Present p99 ms | Gaps >50 ms | Peak bullets |
| --- | ---: | ---: | ---: | ---: |
| always | 57.25 | 39.2 | 0 | 121 |
| frontier | 58.46 | 35.8 | 3 | 121 |

This is the first part of the real stage, not its clear sequence and not a
1,000-bullet stress scene. Both runs pass canonical agreement. It does not
replace the dense workload.

The separate 60 Hz dense control matrix also passes all state checks. Exact
controls report zero resimulations; snapshot-free mode records zero snapshot
bytes, while snapshot-only mode records 360 checkpoints / 3,089,250,952 bytes.
CPU timings vary enough that subtracting those two runs is not a defensible
precise attribution of the total slowdown. Their unbuffered waiting policy is
also not the TH07 buffered endpoint policy; no buffered-mode benefit is claimed.

## Reused early once-only send

TH07 sends fresh, once-only captured input before expensive reconciliation.
TH08 now follows that order while the current world remains admitted. Loading,
retirement and generation bootstrap retain their existing fences. A local
capture is immutable; a rewind reuses it and never samples physical input twice.

Same-build comparison (`ac96ece7af65b3177bb50cbe3986aa63b645904bb3f7fae7f32d69c17849dd75`),
frontier policy on both sides, hardware GPU, wall-clock native touch, 1,000
bullets, P2 4x slowdown; mode order reversed in round 2:

| Round | Send order | P2 logical FPS | Present p99 ms | Gaps >50 ms | Resimulated ticks |
| --- | --- | ---: | ---: | ---: | ---: |
| 1 | after reconciliation | 40.30 | 77.4 | 21 | 111 |
| 1 | before reconciliation | 43.67 | 64.4 | 18 | 89 |
| 2 | before reconciliation | 42.85 | 64.9 | 14 | 82 |
| 2 | after reconciliation | 41.90 | 71.9 | 19 | 113 |

All four runs pass per-peer canonical agreement. This is a directional tail
improvement in two rounds, not a 60 Hz dense-mobile acceptance result. The real
RTC 2P/3P generation/restart regression and 3P shortened Stage Clear correction
also pass. An initial network-test failure was a stale test helper omitting the
mandatory P1 frame-zero route bootstrap, before gameplay; its failed report is
retained as `early-input-network.json`, and the repaired helper passes in
`early-input-network-v2.json`.

## Reuse map

| Guide component | TH08 adapter status |
| --- | --- |
| RTC send impairment, including reliable repair | Uses shared `eagler-common/testkit/rtc-input-impairment.cjs` |
| Wall-clock continuous touch | Adapts TH07 `startBridgeDrag` trajectory to TH08 native SDL input |
| Frontier checkpoints | Implemented in the TH08 driver; confirmed prefix creates no world/texture checkpoint |
| Early once-only input send | Adapted from TH07 driver ordering, with TH08 lifecycle fences |
| Generic bulk copy / coalesced restore | Already enabled in TH08's journals before this follow-up |
| Fixed Bullet slot/part ownership | Uses shared `PartitionedPoolJournal` directly; 1537 TH08 slots, exhaustive six-part layout |
| Batched live gather/scatter | Adapts TH07's single-transition `HEAPU8.copyWithin` callback |
| Exact whole-byte restore oracle | Adapts TH07's standalone test and dense-copy comparison around native UndoTo |
| Three-tick checkpoint span | Not implemented in this candidate; do not confuse frontier elision with interval checkpoints |

The title-specific seams are the object layout, write hooks, resource lifecycle
and native input entry. The journal algorithm is shared, rather than rewritten.
TH08's extra opcode `0x4000` overwrites all five animation VMs on a live Bullet;
it must touch every part before mutation, as do spawn/reuse and destructive
clear paths from both the Bullet and enemy owners.

## Live Bullet correctness gates

The standalone WASI test restores all 6,578,360 Bullet bytes (1537 slots),
including padding and pointer bytes, after hot writes, cold whole-slot writes,
extended checkpoints and later overwrites. Captured hot bytes are 4,084,596 for
its mixed-state fixture. The generic fixed-pool implementation is unchanged.

Native writer tests exercise slot creation/clear, extra-driven template change,
ECL radius clear, ECL global clear and Bullet-owner clear. Each UndoTo compares
the entire Bullet pool with a separate dense byte copy, then checks every pool
owner hash. The diagnostic dense copy is disabled for performance measurements
and entirely absent from production.

Cross-backend delayed-input tests retain the old complete-slot journal on one
endpoint while another uses live parts. A 900-frame 2P death/visual-purity case
passes 349/348 complete-byte restore checks; a 600-frame 3P reverse assignment
with 1,000 bullets passes 203 checks per endpoint. All intermediate canonical
checkpoints agree. In the 3P case both backends create 1,008 snapshots; cumulative
world/texture storage falls from 9,580,798,896 to 7,534,530,672 bytes (21.4%).
These audited, manually stepped runs are correctness evidence, not smoothness
timing measurements.

The native full-world suite also passes all 20 team/solo loadout and focused /
unfocused Bomb cases, plus 40 later lifetime probes. Each world probe performs
three complete Bullet-byte restore checks alongside the existing whole-world
checks, including native resource/lifecycle ownership.
The live backend also passes the real RTC 3P shortened-Stage-Clear regression
through entry into the next stage (`live-bullets-stage-3p.json`).

The first browser attempt (`live-bullets-mixed.json`) failed before boot because
the local server retained an earlier build identity. The identity guard was
not bypassed; the service was restarted for the actual matching build before
the passing `live-bullets-mixed-v2.json` and `live-bullets-mixed-3p.json` runs.

## Live Bullet end-to-end comparison

Same fixture WASM `39ebe48179db69af0372de78698513713e4a77e17bf93330cd4a44475264f2d5`,
frontier + early send on both variants, hardware GPU, 1,000 native bullets,
wall-clock native touch, 4x P2, uncapped presentation, no byte-audit overhead:

| Round | Bullet backend | P2 logical FPS | Present p99 ms | Gaps >50 ms | Capture ms / checkpoint |
| --- | --- | ---: | ---: | ---: | ---: |
| 1 | complete-slot journal | 45.39 | 56.5 | 7 | 5.35 |
| 1 | shared live-part journal | 46.52 | 52.4 | 5 | 3.94 |
| 2 | shared live-part journal | 36.41 | 65.3 | 9 | 4.85 |
| 2 | complete-slot journal | 40.95 | 71.9 | 19 | 5.68 |

All four runs pass canonical agreement. Both rounds lower capture cost per
checkpoint and Present p99, but logical FPS does not improve consistently.
Round 2 live also spends more time per native update than the other runs;
the evidence does not establish why. Do not discard that run or infer an
overall throughput win from storage/copy savings. The endpoint is still below
60 logical Hz at this load. This remains a local candidate requiring further
profiling and phone acceptance; it was NOT included in the separately requested
production Stage Clear hotfix.

## Measurement boundaries

- Present gaps mean intervals between callbacks in which the native renderer's
  completed-Present counter advances. They are CPU-side observations, not a
  display scanout or GPU-fence latency measurement. Logical advance gaps are
  recorded separately; a high redraw rate does not imply smooth world motion.
- Native counters split capture, restore, Update and resimulation Draw; the
  callback timer includes the normal draw and other host work. Max correction
  is the restore/resimulation burst. None of these is GPU execution time.
- Receive age refers to the last packet consumed by the native channel, not
  an independent wire capture. Delivered impairment delay is measured by the
  shared RTC test tool and includes event-loop timer overrun.
- Heap page growth is recorded. Per-arena allocation/growth and restore-copy
  elision byte counters are not exposed by this TH08 adapter. No claim is made
  that those unavailable dimensions were measured.
- Music is disabled, trace recording is disabled, no framebuffer readback is
  performed during the sample, and canonical hashing occurs outside it.
- This is not physical touch-to-photon latency, a full played stage, or phone
  acceptance. Fixed stationary bullets also do not cover every curved/multi-
  sprite workload. No TH06/TH07 or launcher source was changed.

## Reproduction and evidence

From `worktrees/th08-multiplayer`, build fixtures, then:

```powershell
python portable/multiplayer/measure-smoothness.py --data <local-th08.dat> --output <new-experiment-directory> --frames 600 --cpu-rate 4 --modes always,frontier,exact,exact-snapshots --rounds 2 --browser-channel chromium
```

Use `--cap60` to resolve the Runtime's presentation limit, `--bullets 0
--difficulty 2` for native Hard Stage 1 rather than synthetic density, and
`--players 3` for three peers. The runner checks resolved native mode bits,
transport and presentation limit. Diagnostic controls are compiled only in
the fixture variant.

Local evidence roots under `artifacts/multiplayer-tests/`:

- `smoothness-pilot/`: headless-shell/SwiftShader pilot.
- `smoothness-dense-4x/`: two serial software-GPU rounds, separate evidence.
- `smoothness-browser-pilot/`: unthrottled hardware-GPU pilot.
- `smoothness-hardware-4x/`: two hardware-GPU rounds and control groups.
- `smoothness-hardware-cap60/`: capped presentation control matrix.
- `smoothness-hard-stage1/`: native Hard Stage 1, first 1,800 frames.
- `smoothness-wall-touch/`: adopted wall-clock path through native TH08 touch.
- `smoothness-early-input/`: same-build early/late send comparison, two rounds.
- `smoothness-live-bullets/`: old/live Bullet backend comparison, two rounds.

Each root contains frozen inputs, SHA-256 manifest, raw per-run JSON and a
summary. Hardware runs also retain worker logs. These assets remain local;
retail DATA is not committed or uploaded. No deployment is part of this test.
