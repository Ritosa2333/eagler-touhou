# TH08MP base simulation optimization — 2026-09-27

Local candidate; not committed or deployed. User resumed investigation and
authorized choosing the direction. RMX5080 / Android WebView ~150 remains the
reported device; no physical-device measurement was available.

## Method and finding

Followed `eagler-touhou/docs/playbooks/rollback.md`: separate base Update,
snapshot, resimulation, Draw and presentation tails, then preserve exact world
state. TH07's original-game optimizations are relevant evidence. In TH08 the
bullet draw lists are already layered, presentation snapshots already select
active VMs, and Web callbacks do not contain the TH07 synchronous sleep. These
specific TH07 edits cannot simply be copied into this architecture.

Fixture-only Chain and Bullet timing placed ~88–91% of native Update inside
Bullet priority 14. The bullet updater accounted for ~98% of that flow; sampled
normal-bullet collision/graze occupied ~61–68% of its time. Slow endpoints also
fell below 60 logical FPS with very little resimulation. A 60 Hz presentation
cap did not resolve this. See `smoothness-resume-{chain,bullet,update}-4x-20260927`
under `artifacts/multiplayer-tests/` for raw evidence.

## Retained candidates

1. `BulletUpdate::normal`: select a target only when active extra flag 0x80 uses
   it. Preserve the pre-extra bullet origin and inspect flags after
   `initialize_extra`, because an aiming extra can become active during this
   tick. Straight bullets retain their complete native movement/animation.
2. `PlayerCollision`: after the original cancellation barrier and cancel-item
   setup, reject only boxes clearly outside a player's collision/graze bounds.
   Native float preliminary bounds use a two-unit guard, restricted to finite
   coordinates/sizes within 8192, nonnegative sizes and ordered bounds. Near
   edges and unusual values use the original exact arithmetic. No projectile,
   collision, graze, RNG or gameplay count is reduced.

Within this bounded domain endpoint float error is much smaller than the
two-unit guard (values are below 16384). The preliminary test can retain extra
work but cannot reject a touching/overlapping exact box. The fixture also checks
643,000+ representative position/size/margin combinations against the exact
predicate. Cancellation still runs first, including far-reaching Bomb effects.

Both changes are MP-only. Profiling clocks, switches and oracles are fixture-only.
TH06/TH07, Launcher and the ordinary runtime were not changed by this work.

Rejected and removed: caching player order between graze/hit, and approximate
distance sorting for the roster. Canonical checks passed, but browser benefit
was absent or inconsistent. Their frozen reports remain in
`smoothness-order-canonical-3p-20260927` and `smoothness-roster-canonical-*`.

## Base-game control: isolate code cost from transport

`portable/multiplayer/check-update-performance.py` loads two local native worlds
in same-origin iframes, with no network driver, snapshots or resimulation.
Worlds start from the same seed and 1000 inserted stationary bullets after
native stage initialization. Identical zero inputs, fixed six-tick blocks,
alternating old/new order, ten warmup blocks and forty measured blocks.
Canonical hashes match after every block. Calc timing uses native Chain
counters; Update+Draw is elapsed native tick time. This is a base-cost control,
**not presentation FPS**, and 4x desktop CPU throttling is not this phone.

| Players | Old Update ms/tick | New Update ms/tick | Reduction | Old Update+Draw | New Update+Draw |
|---|---:|---:|---:|---:|---:|
| 2 | 12.634 | 8.425 | 33.3% | 17.969 | 13.999 |
| 3 | 11.797 | 7.025 | 40.5% | 15.356 | 10.585 |

Each within-run pair is the valid comparison; do not compare absolute 2P/3P
times from different runs as a player-count scaling result. Instrumentation is
enabled equally in both policies. Evidence:
`smoothness-base-control-{2p,3p}-20260927.json`.

## Real rAF/RTC control: benefit does not establish mobile smoothness

Frozen same-build runs use separate Chromium processes, native rAF, 1000
starting bullets, fixed frame-index inputs, P2 CPU throttle 4x, cap60, 900 ticks
with measurement from 240. RTC application-send delay is 38.5 +/-5 ms one-way;
actual delivery includes timer overruns, so it is not a measured stable 77 ms
wire RTT. Policies alternate ABBA. Canonical gates pass within and across runs.

| Order | Policy | P2 logic FPS | Present p99 ms | Gaps >50 ms | Native Update ms/executed tick |
|---|---|---:|---:|---:|---:|
| 1 | old | 45.68 | 60.5 | 18 | 8.225 |
| 2 | new | 52.57 | 48.1 | 3 | 5.839 |
| 3 | new | 43.69 | 58.4 | 18 | 9.028 |
| 4 | old | 45.82 | 59.7 | 18 | 8.277 |

Evidence: `smoothness-retained-ab-20260927/`. Do not cherry-pick the first pair.
Whole-session FPS remains variable and below 60 despite a clear base-cost
reduction. Earlier `smoothness-candidate-ab-4x-20260927` has similar order/host
variation. Exact-only modes are unbuffered lockstep, not base-capacity controls.

## Correctness and identities

Performance RTC build: `ca50e7d1f8b374a22a7b6b0f73376e05d62b354ed10452c9ea6254a93ebc7b7c`.
Current fixture with added old/new replay oracle:
`1e063bafae2f8f8b9dea8bceeb09db6dd0dbbc2f23958424d7ee27d64ae7b0ff`.
The latter adds diagnostic comparison only; production candidate logic is the
same. Build logs: `smoothness-retained-build-20260927.log` and
`smoothness-update-oracle-build-20260927.log`.

`check-world-journal.py --optimization-oracle` compares legacy forward execution
with optimized execution after exact undo. All six per-tick world group and
block hashes must match, with full Bullet-byte restore audits. It seeds an extra
0x80 that becomes active inside normal Update. This compares old/new gameplay,
not merely two peers both running the same optimized implementation.

Completed: 20 focused/unfocused Bomb cases across all twelve loadouts and 2P/3P
combinations, plus 40 later-lifetime probes (`update-oracle-loadouts-20260927.json`).
Another eight shooting/death/Bomb cases plus sixteen later probes pass
(`update-oracle-actions-20260927.json`), including aiming extra activation in
the shooting case without a Bomb cancelling it first.
Real RTC 2P/3P analog, pause/restart-generation checks pass
(`update-network-20260927.json`); real RTC 3P delayed StageClear reaches the next
stage (`update-stage-3p-20260927.json`). Native complete Bullet-byte restore
checks pass (`update-live-bullets-20260927.json`). That older native fixture does
not force reuse of a particular previous slot; do not overstate its coverage.

The first death trace run (`update-death-20260927.json`) is a failed experiment:
the harness injects up to twelve ticks of latency but assumes exactly one game
tick always advances per iteration. It hits the bounded prediction window at
188; no confirmed divergence was observed before that. This is not a PASS or
evidence of a gameplay regression. The bounded-delay rerun
(`update-death-window-20260927.json`) compared all confirmed native values
successfully but failed its final coverage assertion: the script still assumed
15 global trace fields whereas native trace had expanded to 30. Updated the
field map, derived pilot offsets from its length and added a schema-length
guard. Default injection is now 3+0..3 ticks, within the prediction window.
The complete rerun `update-death-schema-20260927.json` passes ordinary collision
death, correction of that death, item collection, time penalty and restored
state checks through frame 419. Failed reports are retained, not overwritten.

All seven build-isolation tests pass. Current fixture source inventory has zero
stale hashes. The retained production WASM was independently rehashed and is
still `30c5e0ecafc920972958348dc70c900b7b7db5dc15a6f5eefec8bf1283e2e930`.

## Remaining work and working-tree ownership

- Physical RMX5080 acceptance remains open; base-cost gains do not certify
  whole-session smoothness. Next useful step is a real phone trace separating
  forward work, resimulation and actual input delivery stalls.
- Previous early-input send, live Bullet journal, Extra routing and native
  failure diagnostics remain dirty. The unfinished interval adapter is still
  default span **1**; span 3 is not accepted/enabled by these tests.
- New dirty work is the two Update optimizations, fixture profilers/oracles,
  benchmark harnesses and this record. Preserve Launcher parallel UI/room work.
- No commit, push, test deployment or production upload was performed here.
  `th08_web/artifacts/multiplayer/` was not rebuilt; production generation and
  rollback details remain in the previous handoff. Only `multiplayer-fixtures`
  was rebuilt.
