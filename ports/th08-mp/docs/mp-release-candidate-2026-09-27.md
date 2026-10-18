# TH08MP performance release candidate — 2026-09-27

This record supersedes the readiness section of `mp-smoothness-measurement-2026-09-27.md` for this isolated candidate. It does not certify physical RMX5080 performance or claim a deployment.

## Source and scope

Candidate: `worktrees/th08-mp-release-20260927`, branch `codex/th08mp-performance-20260927`, based on `80d555554d79dba8bca382f0cff4c23bbd4dc9a8`. Canonical adaptation base: `cb1bad6e84efbc2148da17b3906dd5cacb8a19ab`; upstream: `fa94b0d43b525cef99c43eebb7e53dbb8f9fb588`; pinned common: `5e14ad8f18f55566e81aa41a5c913e4de9113b5b`.

`artifacts/release-candidate/source-selection.json` records selected changes; `source-working.diff` preserves the original experimental diff. The interval snapshot experiment was excluded, including its Replay/output retention changes. This candidate retains ordinary per-frame correction boundaries. Original experiment worktree and Launcher parallel changes remain untouched.

Retained changes include frontier-only capture, once-only early input send, partitioned live Bullet snapshots, Extra route alignment, richer native failure diagnostics, target-selection filtering, conservative collision broadphase and the new cancellation-region batch below. Fixture profilers and policy switches are excluded from production.

Formal WASM: `ee8fb394df5bba293c6b61a533680e286e5336b09870bbd77fdc4bdc9c5a1165`.
Fixture WASM: `b03a761dc1a5673bc33baab3373ab85431b92872a4e75e8f227f00e810ca0078`.

## Reused TH07 base-game optimization

TH07 `Player::RebuildBombBoxCache` / `CalcBombCollision` enumerate active Bomb boxes once instead of scanning the pool for every bullet. TH08 previously scanned all 192 cancellation-region slots on every barrier query, including when no region was active. This is ordinary gameplay cost, multiplied by rollback; changing prediction policy does not remove it.

TH08 now builds an ascending active-slot list for each pilot after Item update and synchronization, immediately before Bullet/Laser update. RAII scopes end before the next scene job, including on early return. Every forward/resimulated tick rebuilds it. The cache is derived state outside `PlayerSimulationState` and never persists across checkpoint/undo. Geometry, active flags, exact scalar arithmetic, hit counters, first-hit order and cancellation item ownership retain their original owners and calculations.

Audited writers: Player/Bomb, Enemy/ECL and Item jobs create regions before this scope. Bullet/laser hit and graze create effects/items or set life state, but do not allocate cancellation regions inside it. The old/new gameplay oracle enables an additional full membership check on every cached barrier query; this audit is absent from timing runs and production.

Rejected this round: hoisting numeric-mode selection for an exact distance helper. Bitwise comparisons passed, but the two-player benchmark did not improve; the implementation was removed. Do not mistake its retained experimental reports for accepted code.

## Base Update cost

Same renderer, two local worlds, identical input, 1000 starting bullets, CPU throttle 4x, 240 measured ticks. No networking/snapshot/resimulation. Baseline already includes the preceding target/broadphase changes. Canonical state is checked after each block.

| Players | Previous Update | Cached Update | Reduction | Previous Update+Draw | Cached Update+Draw |
|---|---:|---:|---:|---:|---:|
| 2 | 6.397 ms | 2.404 ms | 62.4% | 9.640 ms | 5.535 ms |
| 3 | 8.959 ms | 2.924 ms | 67.4% | 12.592 ms | 6.380 ms |

Two-player evidence is in the original experiment worktree at `artifacts/multiplayer-tests/barrier-cache-base-2p-20260927.json`; three-player evidence is in this candidate at `artifacts/multiplayer-tests/barrier-cache-base-3p-20260927.json`. The former fixture includes disabled interval experiment code; the candidate excludes that code. Compare each old/new pair internally, not absolute times between runs.

## Native rAF and real RTC

Frozen candidate fixture, separate Chromium processes, one-way application-send impairment 38.5 +/- 5 ms, slow peer CPU 4x, presentation cap60, 900 ticks, measurement from240. Delay also affects repairs; actual timer overruns are recorded. This is not a measured stable wire RTT or an emulation of the physical phone.

| Order | Cache | Slow-peer logical FPS | Present p99 | Gaps >50 ms |
|---|---|---:|---:|---:|
| 1 | off | 51.75 | 50.5 ms | 7 |
| 2 | on | 59.51 | 28.1 ms | 0 |
| 3 | on | 59.53 | 30.4 ms | 0 |
| 4 | off | 48.62 | 53.2 ms | 10 |

All four runs passed endpoint and cross-policy canonical comparisons. Both optimized endpoints had zero gaps over50ms; the fast endpoint still resimulated about2900 ticks. Peak live bullets were1007; the inserted field cleared naturally before the end, so this is not a sustained1000-bullet whole-run claim. Frozen assets, source identity, raw rows, callback costs, snapshot bytes and transport timers are under `artifacts/multiplayer-tests/release-cache-rtc-abba-20260927/`.

Wall-clock native touch stimulus with the same starting field passed (`release-touch-dense-20260927/`): slow peer58.99 logical FPS, Present p99=31.5ms, max41.4ms, zero gaps over50ms. This is actual native touch production, but scripted browser input rather than a physical touchscreen.

The first authored Lunatic2400-tick run (`release-authored-lunatic-20260927/`) passed canonical checks but had56.44 whole-run logical FPS and a181.5ms maximum Present gap on the slow peer. The game loop had retired its gameplay jobs nearframe2375: the84.4ms Update there was dominated by Supervisor priority0 (76.8ms); later low-cost frames waited for confirmation. Beforeframe2300 both peers were about58.9 FPS. These are observations, not a reason to discard the transition stalls. A subsequent run records explicit scene and loading fields to distinguish gameplay from retirement/menu time.

### Default presentation and formal production checks

The uncapped diagnostic1000-bullet run (`release-default-presentation-20260927/`) passed at55.14 slow-peer logical FPS, below the capped59.5 result. It must not be omitted when reporting the stress workload. There is no new production presentation cap in this change.

Explicit scene instrumentation (`release-authored-scenes-20260927/`) confirmed the authored run remains in Game(scene2) fromframe240 to2374, then enters GameResults(scene6). With cap60 the gameplay segment was59.80/59.77 FPS; GameResults advanced13.39/14.67 FPS. Whole-run57.48/57.61 FPS includes that wait and remains part of the result.

The harness now has a `--production` lane: only the actual formal WASM, no native fixture or profiling exports, no injected bullets, and unchanged runtime policy. Unsupported switches are rejected. Unavailable cost counters are reported asnull rather than zero. It still uses the standalone test host and disables music; the separate formal-shell test above covers the managed shell and storage, not their combined long-run performance.

`release-formal-authored-20260927/`: production SHA verified in both browsers, no fixture exports, default uncapped rendering, Lunatic,2400 ticks, same RTC impairment and slow-peer CPU4x. Per-peer and final canonical gates passed.

| Segment | P1 logical FPS | P2 logical FPS | P1/P2 maximum Present gap |
|---|---:|---:|---:|
| Game, frames240–2374 | 59.30 | 59.29 | 36.4 / 32.4 ms |
| GameResults, frames2375–2400 | 14.24 | 15.38 | included below |
| Whole measured run | 57.20 | 57.25 | 185.7 / 195.3 ms |

Neither endpoint had a gameplay Present gap over50ms. Results-scene loading/wait remains visible and unresolved; it must not be advertised as uniformly60FPS through all scene transitions. The formal authored run does not replace the separate diagnostic1000-bullet stress comparison.

Reproduce this final lane:

```powershell
python portable/multiplayer/measure-smoothness.py --output artifacts/multiplayer-tests/new-formal-run --data D:/workspace/eagler/th08-eagler/artifacts/presentation-lab/input/th08.dat --production --modes frontier --rounds 1 --frames 2400 --cpu-rate 4 --bullets 0 --difficulty 3
```

## Validation status

The original experiment's new barrier cache passed all20 focused/unfocused Bomb loadout cases and40 later-lifetime probes, with old/new per-tick world hashes and complete Bullet-byte undo audits (`barrier-cache-oracle-loadouts-20260927.json`).

Candidate-specific completed gates:

- Eight shooting/death/focused/unfocused Bomb old/new world comparisons and16 later probes: `release-world-actions-20260927.json`.
- Native complete Bullet-byte undo, five writer scenarios: `release-live-bullets-20260927.json`. This older fixture does not force reuse of the exact same prior slot.
- WASI complete-byte partitioned Bullet regression and corrected-boundary capture reuse for2P/3P: PASS.
- Formal shell, native P2 touch/Bomb bridge, exit, isolated IDBFS reload, room retention: `release-host-manifest-20260927.json`.
- Formal production RTC2P/3P analog inputs and lifecycle: `release-network-20260927.json`.
- Formal production Replay2P/3P,500 recorded frames each including generation restart,500 per-tick canonical/state/audio comparisons each in fresh offline native Replay menu: `release-replay-20260927.json`.
- Three-player real RTC with delayed input across native Stage Clear reaches the next stage: `release-stage-20260927.json`.
- Ordinary/production/fixture separation: all7 tests, `release-isolation-20260927.log`. Ordinary build SHA: `935bd22627863e29fc645706c010fad0268e65fd3a39a5d319e01c77412908ff`.
- All three build source inventories and WASM digests revalidated with zero stale source entries.
- Packaged formal Runtime (15files) is at `build-eagler-multiplayer/`; no publication.

The first formal-host attempt, `release-host-20260927.json`, failed before gameplay because the local test server lacked `/runtime/manifest.json`. The server now uses the canonical Runtime manifest generator and the actual build identity; the rerun above passes. No admission check was bypassed. This test-tool fix and the scene-level timing fields were also preserved in the original experiment worktree.

Physical RMX5080 / Android WebView acceptance remains necessary. Desktop CPU throttling cannot validate that device's GPU, thermal behavior, WebView scheduling or real TURN delivery. The user authorized committing this candidate after the measurements; no push or deployment is included.

## Next bounded work

1. Run this exact packaged candidate on RMX5080/WebView with the remote peer and real TURN, including high bullets, death/Bomb and stage transitions; record build identity and actual frame gaps.
2. Investigate GameResults loading and confirmed-input cadence separately. The world journal intentionally retires before resource destruction; do not allow prediction across that boundary without proving state/resource ownership.
3. If the phone still misses gameplay deadlines, distinguish high-refresh presentation work from Update using the retained uncapped/capped controls. Do not remove bullets, soften determinism checks or turn the measured average into an unqualified60FPS claim.

The commit includes the scoped barrier cache, its old/new membership oracle, fixture profiling/benchmark changes, the local manifest-serving fix and this record, along with earlier live-Bullet, early-send, Extra and diagnostics changes. Generated Runtime packages and raw measurements stay local. The original experiment worktree retains its dirty changes, including the excluded interval experiment; it is not the release candidate. Launcher parallel edits and TH06/TH07 sources were not modified. The original worktree's production WASM remains the previously deployed `30c5e0ecafc920972958348dc70c900b7b7db5dc15a6f5eefec8bf1283e2e930`.
