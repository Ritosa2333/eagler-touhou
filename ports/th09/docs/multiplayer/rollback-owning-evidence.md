# TH09 owning-pool rollback optimization — 2026-09-27

Local candidate; no deployment or 404/resource-route changes. Primary guide:
`D:/workspace/eagler/eagler-touhou/docs/playbooks/rollback.md`.

## Changes

- Effects and attacks now capture active/owning slots at the checkpoint boundary,
  plus reserved slots and overflow sentinels. Cold slots are captured before
  initialization or destructive clear. Fixed-size bitmaps deduplicate first
  writes; the three effect-manager ranges preserve fixed pool identity.
- Reuse the same concrete attack-state allocation during save/restore. Exact
  type tokens work without RTTI; a type change still creates the correct clone.
- Preserve the byte representation of named trivial blocks in owning copies.
  An added pre-resimulation oracle exposed stale tail padding in EnemyAnimation
  after cold-slot activation; no gameplay-field divergence was observed. Copying
  these blocks completely fixes the exact-byte discrepancy. Owning children
  retain their explicit deep-copy and ECL pointer-rebinding paths.
- World tests now compare the restored state before resimulation, as well as
  the resimulated state and final pixels. An adversarial diagnostic exercises
  cold slots, repeated writes, reserved slots, pool exhaustion and clear/reuse
  under both sparse and dense snapshots.

## PC evidence

Chronological hardware runs: old A, new B, new B2, old A2. Chromium
149.0.7827.55; Intel UHD / ANGLE D3D11. Each run uses the same real-game inputs,
200 correction blocks with one snapshot and eight replay ticks. Hashing and
pixel readback are outside timing; no build ran concurrently.

Mean total milliseconds per run:

| Work | Before | After | Reduction |
| --- | ---: | ---: | ---: |
| Capture | 95.757 | 83.298 | 13.01% |
| Restore | 81.260 | 62.745 | 22.78% |
| Eight-tick resimulation, 200 blocks | 194.892 | 184.530 | 5.32% |
| Capture + restore + resimulation | 371.910 | 330.573 | 11.11% |

All 800 correction blocks pass full-state/pixel checks; all cross-build
48-word gameplay audit arrays match. The reported initial maximum checkpoint
bytes fall from 2,741,824 to 2,452,192. This byte counter excludes subsequent
first writes and attack clone allocations; it is not total memory usage.

Limitations: the no-snapshot 320-tick control was 26.370 -> 28.443 ms (7.86%
slower), so this does not establish an ordinary-frame speedup. Candidate
correction max outliers reached 12.465 ms versus baseline 7.090 ms; there is no
universal tail-latency improvement claim. This fixture is one checkpoint per
eight-tick block, not the entire live per-frame network driver. Do not add its
percentage to earlier storage/Draw improvements.

## Correctness and release gates

- Sparse and dense world tests plus characters 6/9 round reset: 810 checkpoints,
  1,800 frames per run, exact initial restore, replayed state/pixels, adversarial
  owning-pool boundary and recording-chunk boundary pass.
- All nine polymorphic attack types: 128 repeated restores each preserve the
  allocation, exact type, content and parent link; delete/type-change and
  no-RTTI compilation pass. Cold animation byte restoration passes.
- Three independent worlds, 900 frames: canonical state, framebuffer and all
  1,312 Replay bytes match the exact-input reference under packet faults.
- Actual RTC and WebSocket relay with admitted spectator: all three worlds
  reach frame 600 and hash 1357358377; audio runs. Deliberate outages produce
  visible presentation gaps, so this is not a claim of imperceptible correction.
- Release build and packaged runtime pass; release RTC passes periodic confirmed
  checks with audio and spectator. Its final frames differ, so final hash equality
  is not used as a same-frame assertion. No diagnostic exports in release.

Diagnostic WASM: `fbc7d7c30b2ff490ab983a60cea1af86602be823fa8965e5640e59b5532ac7d6`.
Release WASM: `64c19a08eb80a7165820325bffa6510ce8269948e00939592c5556f2dedfe490`.

The [machine-readable evidence](rollback-owning-evidence.json) includes complete
reports and build source hashes. Reproduce its checks with
`node th09_web/tests/multiplayer/summarize-owning-evidence.mjs`.

## Remaining suggestions and keyboard decision

Active AttackAreas already use an active pointer list; enemy targeting shares
its existing active-enemy update traversal. No second cache or changed collision
math was added without evidence of a bottleneck. The previously measured pure
sprite Draw omission remains enabled; stateful effect geometry remains intact.

A new experimental keyboard selector uses the real common core at three- and
six-frame horizons, extending only after six consecutive received actual inputs
hold the same nonzero direction. Synthetic results per 2,400 samples:

| Trace | Default 3-frame mismatches | Adaptive mismatches |
| --- | ---: | ---: |
| Long hold | 1,813 | 734 |
| Short tap | 600 | 600 |
| Move then release | 1,200 | 640 |
| Rapid reversal | 2,400 | 2,400 |

But long-hold transition wrong-direction predictions rise 13 -> 78; release
over-hold rises 0 -> 80. Fewer total mismatches does not establish safer dodging.
The production policy remains unchanged: zero intentional local input delay,
three-frame direction hold, eight-frame rollback limit. The selector is a test
candidate only, not shipped prediction behavior. Actual human-trace/competitive
acceptance remains unproven.
