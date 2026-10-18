# TH09 rollback under 4x CPU throttling

## Outcome and acceptance status

The selected J implementation substantially reduces long gaps between rendered
submissions. **The requested good performance under 4x CPU throttling is not yet
fully achieved:** the formal release advances at about 31 logical frames/second
in this fixture, rather than stable 60 Hz. Some presentation gaps still exceed
100 ms. This is a verified improvement in presentation continuity, not a claim
that all rollback performance problems are solved.

The machine-readable record is [rollback-4x-evidence.json](rollback-4x-evidence.json).
It contains all four release runs, build/source identities, confirmed hashes,
input tape identities, renderer counts, correctness results, and hashes of the
raw reports. Its generator validates all 274 recorded source/header inputs of
the selected release against the current source tree before saving the evidence.

Work is isolated in `D:/workspace/eagler/worktrees/th09-rollback-4x`, branch
`experiment/rollback-4x`, based on authored TH09 `6ce1c46`.
Implementation and executable test code are committed locally as
`ea96a16788e81d9937ba95c4fcba7f2dae595844`. The shared dependency
remains `eagler-common` `43add4dc9085c65468f7ab15428c00206cbe3834`, read from
`D:/workspace/eagler/th09-eagler/third_party/eagler-common`.
The canonical title, upstream-tracking checkout, and unrelated launcher changes
were not changed. All environment work used **nig**, as requested. This work
uses authored C++/Web code and private resource fixtures, never an original
executable or reconstruction oracle. There was no public push or deployment.

## Formal release A/B/B/A measurement

A1, B1, B2, A2 ran sequentially with no profiler, concurrent build, or concurrent
performance test. Both peers run in separate Chromium processes with real
WebSocket relay transport. Only P2 has DevTools 4x CPU throttling. Audio output
is enabled. The seeded impairment delays input and repair sends by 39 +/- 5 ms;
actual delivered delays are also recorded because a busy event loop adds delay.
This is relay evidence, not an RTC/TURN or public-Internet measurement.

The browser was Chromium `153.0.8010.12`, using native Intel UHD / ANGLE D3D11.
The frame-indexed hosted-key tape has 1,800 frames; the measured steady interval
is logical frames 180 through 1,799. Every run verified the same captured input
tapes on the wire and the same fifteen confirmed hash points at frames
120, 240, ..., 1800 on both peers and across both builds.

| P2 statistic | A1 baseline | B1 candidate | B2 candidate | A2 baseline |
| --- | ---: | ---: | ---: | ---: |
| Net logical advancement, Hz | 31.94 | 31.26 | 31.14 | 28.55 |
| Callback work p50, ms | 21.52 | 17.64 | 16.86 | 23.51 |
| Callback work p99, ms | 60.01 | 46.36 | 47.56 | 62.80 |
| Present gap p50, ms | 72.57 | 30.79 | 30.45 | 78.67 |
| Present gap p95, ms | 242.54 | 60.74 | 60.65 | 345.24 |
| Present gap p99, ms | 393.79 | 78.73 | 78.74 | 593.91 |
| Maximum Present gap, ms | 666.35 | 109.14 | 163.49 | 1520.96 |
| Sampled Present gaps | 595 | 1452 | 1483 | 510 |
| Corrections in measured interval | 17 | 9 | 16 | 18 |
| Historical ticks replayed in interval | 26 | 11 | 27 | 24 |

A Present gap is the difference between rAF timestamps of callbacks that
observed a new renderer submission. It is **not physical scanout**. Callback
completion gaps are retained separately in the evidence. Net logical
advancement counts new frames, never historical replay ticks.

The candidate's p50, p95, and p99 presentation gaps improve in both repetitions.
The logical-throughput evidence is much weaker: the two-run arithmetic means
are 30.244 Hz and 31.200 Hz, a descriptive difference of about 3.2%, while A1 is
faster than B1. Two runs per variant do not establish a statistically significant
throughput gain. Do not pool the individual percentiles or combine these
percentages with older development-build experiments.

P1 is not CPU-throttled and performs much more prediction/recovery work than
P2. The run is a closed feedback loop: the same input tape does not imply the
same arrival schedule, number of corrections, or number of replayed ticks.
The evidence preserves both peers' full summaries for that reason.

Release builds have no development frame-limit hook. Their final live endpoints
can have different frame numbers or outstanding predictions. All fifteen
confirmed hashes match, including frame 1800 = `1999583347`; unlike-frame
endpoint hashes were not treated as divergence or compared as equal states.

## Changes selected for J

### Present the state actually reached before the callback budget expires

The old outer loop planned to render only its last due tick. If an earlier
netplay tick exhausted the 10 ms callback budget, the loop could exit before
that planned rendering tick and advance the simulation without presenting its
result. Repeating this produced long visible freezes.

Each reached forward netplay tick can now present before that exit. The inner
rollback frontier still suppresses historical GPU submission. Stateful Draw
traversal remains on every logical/replayed tick; input is captured once per
new logical frame. The prediction window, intentional input delay, 8 ms
recovery budget, 10 ms outer budget, and 60 Hz simulation rules are unchanged.

### Reduce portable renderer submissions and buffer replacement

Owner triangle batches are copied into reusable storage before owner buffers
can be reused. Adjacent compatible batches and independent line segments can
coalesce. World-UV quads use per-instance world matrix, texture matrix, and
texture factor without changing title-specific alpha/depth semantics.

Ordered batches share a vertex upload. Queue limits are 1 MiB for vertices,
1 MiB for instance data, and 1024 commands; an individual oversized owner draw
is kept complete and submitted immediately. Indexed draws, line strips,
clear/copy/read/release/present, and resource writes retain submission barriers.
Text writes flush pending GPU consumers before CPU texture bytes change.

The line path uses its own VAO and rebased attribute offsets. A rejected
nonzero-first variant lost line pixels on the tested native ANGLE path; the
accepted path passed exact pixel and GL-error checks. Uniform uploads are
skipped only for values unused by the current shader branch, retaining cache
state for later draws. Texture-unit ownership remains within the renderer.

### Avoid redundant math and snapshot work

World projection matrices are calculated once for a quad or camera-facing
sprite. The stage translation helper reuses unchanged rows while preserving
the exact original scalar multiplication/addition order. The shared mutable
ribbon projection input is retained. No fast-math or compiler-level
reassociation was enabled.

Sparse snapshots omit immutable bullet appearance templates initialized before
the match/checkpoint lifetime. Live animation instances retain their existing
first-write capture. Background animation VMs are still all captured, but their
stable contiguous storage enters the journal as one range instead of many
individual ranges. The independent dense/hash inventory still visits every VM
and template byte.

### Remove an unused audio pitch stage

TH09 does not request pitch changes or Doppler. Voice creation enables
`MA_SOUND_FLAG_NO_PITCH` alongside the existing no-spatialization flag. Required
sample-rate conversion remains active.

This is not a bit-exact audio claim: at 44.1 kHz, removing the old unity-pitch
filter changes waveform samples. The component fixture verifies direct source
PCM at unity rate, matching cursors, loop/seek/pan/volume/stop/resume behavior,
and unchanged tested output at 22.05/48 kHz. Real browser runs sampled hundreds
of nonzero SDL output blocks. They do not establish uninterrupted whole-stream
output or subjective acoustic quality.

## Correctness and renderer-work evidence

| Check | Result and scope |
| --- | --- |
| Final J sparse world, round reset | 1,800 frames; 270 checkpoint/restore/replay comparisons; full state inventory and PNG equal; owning-pool, bullet reset/reuse/overflow, and recording boundaries pass. |
| Final J long rollback | 7,200 input frames; two rollback peers plus an exact-input reference; packet loss/duplication/delay-spike fixture. Both observed pause and match-complete phases; 196 paused calls each. Final hash `763548183`, 48-field audits, framebuffer, and final saved 2,130-byte Replay all equal. P1/P2 replayed 2,391/2,918 historical ticks; peak active bullets 491. |
| Final production release rendering | Baseline/candidate production WASMs, public spectator feed/tick exports, same 7,200-frame tape with pause edges. All 68 PNG, network-hash, and session-state checkpoints equal, including the results lifecycle. No development exports; GL error zero. This is separate from the live-relay benchmark and the rollback-peer test. |
| Renderer-development I, sustained depth eight | 1,800 frames; 12,531/12,467 historical ticks replayed; hundreds of completed eight-deep recoveries, exact three-world hash/audit/framebuffer and 1,985-byte Replay. J's later background-range change has its own world, long-peer, and 200-block eight-tick restore/replay checks. |
| Renderer boundary fixture | Five exact-pixel cases against authored `6ce1c46`: mixed strides and VAOs, normal/instanced offsets, arena/command limits, oversized buffer reuse, resource write/preparation/release, immediate/indexed/line-strip barriers, independent lines, uniforms, resampling, readback and discard. |
| Scalar projection | 20,000 bit-exact matrix/vertex cases, including signed zero, subnormals, random finite inputs and camera matrices. |
| Components | Frame and spectator scheduling, rollback/session/Adonis modes, owning dynamic state, bullet snapshots, audio, and projection pass. Production attack constructors/adapters compile without RTTI. |

The original 4,500-frame paused endurance run stopped at its **coverage**
assertion because it had not reached results. Its failure report is preserved;
the passing 7,200-frame run supplies the missing coverage. The final Replay
comparison is the recording saved at the endpoint after lifecycle transitions,
not a claim that one Replay contains all 7,200 test frames.

The exact 7,200-frame **full-render** production fixture also reports:

| Counter | Baseline | Candidate | Change |
| --- | ---: | ---: | ---: |
| Upper-level draw requests | 338,326 | 338,326 | equal |
| Submitted batches | 337,693 | 206,736 | -38.78% |
| Buffer replacements | 337,693 | 31,881 | -90.56% |
| Vertex upload bytes | 570,219,372 | 573,022,744 | +0.49% |
| Texture binds | 91,694 | 91,694 | equal |
| Framebuffer binds | 14,503 | 14,503 | equal |
| Program compilations | 33 | 33 | equal |

This proves reduced submission/replacement work for that fixed rendered tape.
It does not imply an equivalent reduction in total CPU time or upload bandwidth.
The final image digest is
`bb5311f852829c722ad560d11fe242f5bed7220bdae6ca80be6e5fcd9f47f4b9`.

## Frozen build identities

| Build | WASM SHA-256 |
| --- | --- |
| Baseline production release | `07bb39119789e2f736c8b8055bc360f4f7dc30ac98fb8fbd1409e251f830c73a` |
| Selected J production release | `b525102519c98ac5e3d4cd3bbcd840fa6e77eb1f0bf9196e3ba2cb2a8e94547b` |
| Selected J development/profile build | `bf3d4f465eeb4f0a4fb84d298a6eef2c5130fc59961ee5bf58982ec3ea3760e9` |
| Renderer-development I | `10923542f478f6062d4e01f6dae21c5f2095efa5e9afd80c72069647642e9c2b` |

Selected release directory:
`th09_web/artifacts/rollback4x-candidate-release`.
Selected diagnostic directory:
`th09_web/artifacts/rollback4x-background-span-profile`.
The detached authored baseline is in
`D:/workspace/eagler/worktrees/th09-rollback-4x-baseline`, with release directory
`th09_web/artifacts/rollback4x-baseline-release`.
Raw reports and rejected builds are ignored artifacts; do not overwrite them
or confuse default `artifacts/sdl3` / `artifacts/sdl-release` with these builds.

## Reproduction through nig

Use the task worktree as `workdir`, except when building the baseline. Private
fixtures stay in `D:/workspace/eagler/th09-eagler`; do not commit or upload them.

Common environment:

| Variable | Value |
| --- | --- |
| `TH09_FIXTURE_ROOT` | `D:/workspace/eagler/th09-eagler` |
| `EAGLER_COMMON_ROOT` | `D:/workspace/eagler/th09-eagler/third_party/eagler-common` |
| `TH09_EMSDK` | `D:/workspace/eagler/th08-eagler/tools/emsdk` |
| `WASI_SDK_BIN` | `D:/workspace/eagler/toolchains/wasi-sdk-34.0-x86_64-windows/bin` |
| `TH09_BUILD_JOBS` | `4` |

Rebuild production with `node th09_web/scripts/build-sdl.mjs --release`,
`TH09_PROFILE=0`, and a **new** `TH09_OUTPUT=artifacts/rollback4x-release-repro`.
The same command at authored baseline `6ce1c46` creates the control.
Diagnostic builds omit `--release`, set `TH09_PROFILE=1`, and use another new
output directory. `TH09_OUTPUT` is relative to `th09_web`; `PC_BUILD` is relative
to the command's workdir. Default optimization remains O2, C++17,
`-ffp-contract=off`, and the existing Safari-14.1-compatible lowering.

Run `node th09_web/tests/multiplayer/smoothness-browser.mjs --relay --release`
with `PC_BUILD` pointing at the explicit release directory, `CPU_RATE=4`,
`FRAMES=1800`, `DELAY_MS=39`, `JITTER_MS=5`, `NATIVE_GPU=1`, `PROFILE_CPU=0`,
`INPUT_MODE=tape`, `AUDIO_OUTPUT=1`, and a unique `RUN_LABEL`.
Run baseline, candidate, candidate, baseline sequentially. Do not benchmark
while compiling, running another browser fixture, or collecting V8 profiles.

For correctness:

- `node th09_web/tests/multiplayer/run-tests.mjs` runs the component gates.
- `node th09_web/tests/multiplayer/world-browser.mjs --round-reset` uses the
  explicit diagnostic `PC_BUILD` and a unique label.
- `node th09_web/tests/multiplayer/peer-browser.mjs` with
  `PEER_FRAMES=7200`, `PEER_INPUT_TRACE=endurance-tape`, `PAUSE_TRACE=1`,
  `NATIVE_GPU=1`, `FIXED_DELAY_FRAMES=0`, `FORCE_CORRECTION=0`,
  `ADONIS_MODE=0`, and `INPUT_DELAY_FRAMES=0` repeats the long peer fixture.
- The same peer script with `PEER_FRAMES=1800`, `FIXED_DELAY_FRAMES=8`,
  `FORCE_CORRECTION=1`, `PAUSE_TRACE=0`, and
  `PEER_INPUT_TRACE=charged-strafe-alternating-focus` exercises depth eight.
- `node th09_web/tests/multiplayer/release-render-browser.mjs` uses
  `PC_REFERENCE` and `PC_BUILD` for the two explicit production releases,
  `FRAMES=7200`, and a unique label.
- `node th09_web/tests/multiplayer/renderer-queue-browser.mjs` builds the
  five synthetic cases from authored `REFERENCE_REVISION=6ce1c46` and current
  portable source; use a new `RUN_LABEL`.
- `node th09_web/tests/multiplayer/summarize-rollback4x.mjs` validates and
  condenses the **original frozen report labels/directories** into the evidence
  JSON. It intentionally refuses source or identity mismatches.

## Rejected trials and remaining work

The common-journal TouchMany/bulk-copy prototype, O3/LTO, nonzero-first line
queue, pre-resampled sound-effect cache, and audio refill-quantum experiment
were not integrated. Their respective failures were throughput regression,
lack of measured improvement, missing line pixels, audio sample/cursor
differences, and worse live pacing. The separate common prototype worktree is
preserved, but the accepted common pin stays at `43add4d`.

Diagnostic profiles still show costs in snapshot capture/copying, audio
decoding/conversion, and renderer/browser submission. They justify continued
investigation, not a numerical promise about the remaining gain. One noisy
development profile also slowed the unthrottled peer and is not acceptance
evidence.

Stable 60 Hz at 4x, physical display timing, sustained audio quality, phones,
Safari, RTC/TURN and other titles remain outside the achieved acceptance.
The source retains the existing portability target, but this task did not
perform runtime acceptance on those other platforms.
