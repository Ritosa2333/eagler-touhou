# TH09 Adonis experiment — implementation and evidence

> Current automatic policy and reproduction: [MEASURED-ADONIS-STARTUP.md](MEASURED-ADONIS-STARTUP.md).
> Both experimental modes now calibrate the actual input channel before frame
> zero. The historical presets, common pin and frozen reports below remain an
> earlier evidence record; do not use an old build with the new measured shell.

> Current user priority: refine TH09 before any more title adaptation. Read
> [TH09-REFINEMENT.md](TH09-REFINEMENT.md) for the held-analog prediction change,
> matched-input comparison, new relay diagnostics and frontend rollback switch.

Date: 2026-10-02. Branch: `experiment/adonis`.
Worktree: `D:/workspace/eagler/worktrees/adonis/th09`.
Base: `5b9305c` from the TH09 multiplayer owner, **not** the ordinary `eagler`
branch. Common pin: `5669eff91e88a4391e144652836bcb0804b1482b`.
Matching Launcher/relay: `../eagler-touhou`, also `experiment/adonis`.
No push, deployment, mutation of an existing MP worktree, or retail asset copy.

## Implemented contract

| Mode | Value | Admission | World history |
| --- | --- | --- | --- |
| Rollback baseline | 0 | Existing prediction policy | Existing full rollback |
| Adonis delay | 1 | Every player's exact contiguous input | No checkpoints, prediction or resimulation |
| Adonis hybrid | 2 | Delayed local capture plus existing prediction | Full rollback corrects remaining late inputs |

Capture physical input exactly once for each forward N, apply it at N+D, and
never recapture on a blocked attempt or rollback. D is fixed for that session,
0..9; neutral prefix inputs are explicit. Mode and D enter the native HELLO
gameplay ABI. Different modes/delays cannot start together. Delayed Replay and
spectator inputs are already authoritative and must not be delayed again.
The simulation remains fixed 60 Hz. Pure delay refuses a speculative world
step and exposes zero snapshot/resimulation counters; it is not merely a
rollback engine with its correction callback disabled.

Adonis-inspired timing is independently written from the supplied static
analysis, without executing or copying the binary package. First actual input
arrival and first forward-frame due timestamps close into 16-frame statistics.
Corrections use a 4 ms lead deadband and at least 60 frames between changes;
local waiting or half the matching peer lead imbalance delays the wall-clock
epoch. This implementation adds an 8 ms correction cap and 250 ms observation
cap. It replaces, rather than stacks with, the old proportional pacer. Queued
phase debt survives high-refresh callbacks and is **not consumed on a blocked
tick retry**, where the scheduler would otherwise ignore it.

These are borrowed policies, not binary/protocol compatibility with Adonis.
No live D-change transaction, VPatch prepare-budget estimator, busy waiting,
scanline polling or higher-frequency simulation was added. The common RTT-tail
recommendation helper is tested but **not wired to lobby probe samples**.
Launcher values 4f (delay) and 2f (hybrid) are labeled experimental starting
values, not measured optimal delays. Existing baseline defaults remain intact.

## Selection and distribution

Use the matching experimental Launcher, open a **TH09 multiplayer** room, and
select the experimental timing mode and D before the host starts. The relay
publishes both values to players/spectators. P2 cannot start a match and a
repeated start cannot change a running mode or D. Old Runtime builds are not
advertised as compatible; this experimental Launcher currently refuses Adonis
on the other titles until those adapters are completed.

Direct development Runtime parameters are `?adonis=delay&inputDelay=4` or
`?adonis=hybrid&inputDelay=2`. Explicit managed `netplayAdonisMode` and
`netplayInputDelay` options take precedence. Both players must agree. The
published `__eaglerNetplayAdonisMode` global is telemetry, **not** saved config.

Exports: `th09_adonis_configure(mode)` (before session only), and
`th09_adonis_info()` returning mode, D, snapshot count, phase correction count,
total requested phase microseconds, and input-wait attempt count. A wait count
is neither a millisecond duration nor a separate physical input sample.

Resource-free MP Runtime directory: `th09_web/build-eagler-multiplayer`.
It retains the multiplayer variant marker and `/savesth09mp` storage identity.

## Frozen builds and verification

Diagnostic after the retry-phase fix:
`b6c5cfd2f11cdbd6be821bf28338b64879fa75daa976089c606d51f56e808905`.
Release (no `th09_probe_*` exports):
`480fe94192741332ee5393d5bbe679e42fda2e3b2287096b74c1ac740449d27a`.
The earlier 900-frame peer tests used diagnostic
`45f5cdc75f66993c9f7bd4e36b506cb23fea5b1e7b7b57ed31e9a4421c6dc55d`;
do not present them as verification of the later scheduler fix.

Evidence lives in `th09_web/artifacts/multiplayer-tests/`; the committed
`adonis-evidence.json` is a bounded summary including failed attempts.

- Common Release GNU tests: 29/29 passed, including baseline regressions,
  exact-input admission, forged prediction rejection, 3P phase pairs, codec,
  once-only timestamps, deadband/cooldown and mode/delay ABI mismatch.
- TH09 WASI: baseline session, new Adonis session, frame scheduler, dynamic
  state and bullet snapshot suites passed. The Adonis suite runs 20 pure-delay
  cases (D=0/1/3/9 times five network patterns) and eight baseline/hybrid cases,
  with a per-tick deterministic model oracle, touch/key edges and retirement.
- Browser shell/server tests: 8/8 passed, including actual generated normal/MP
  save and Replay isolation, mode/D propagation, spectator ordering and relay.
  The storage test uses a release-WASM override and the real keyboard helper;
  its previous VM fixture was missing that imported dependency.
- Three real TH09 worlds in one hardware-rendered Chromium process: pure
  delay D=4 for 6,000 frames, and hybrid D=2 for **12,000 frames**, both passed
  complete world audit/hash, final framebuffer equality, byte-identical Replay,
  pause and results coverage against the appropriately delayed exact-input
  reference. Pure: no corrections/resimulations, hash 3197602033, 1,990-byte
  Replay. Hybrid: hash 3681705413, 3,384-byte Replay. The earlier hybrid 6,000
  attempt did not reach results and is retained as a failed coverage attempt;
  the gate was not relaxed to make it pass.
- Local RTC players plus confirmed spectator passed the fault-injection gate.
  The matching experimental lobby/relay also passed pure D=5 RTC and hybrid
  release RTC. Release smoke ends at different frames on different peers and
  therefore does not claim a comparable final hash when their frames differ.

### Observed trade-off, not a phone performance guarantee

One local diagnostic RTC series injected input-send delays of 25..61 ms,
periodic unreliable-lane drops and a short 800 ms additional-delay interval:

| Mode | D | Total replayed frames (both players) | Callback P95 ms (P1/P2) | Time to gate |
| --- | --- | --- | --- | --- |
| Baseline | 0 | 3457 | 3.105 / 3.790 | 12.007 s |
| Hybrid | 2 | 1769 | 2.730 / 2.345 | 11.971 s |
| Delay | 4 | 0 | 0.955 / 0.765 | 13.574 s |
| Delay | 5 | 0 | 1.015 / 0.825 | 12.386 s |

These are single local runs, not ABBA statistics, WAN, TURN, thermally stable
phones or input-to-photon measurements. D changes the applied trace and can
change the game's workload. Callback time is not measured pure CPU/GPU time.
Hybrid spectator presentation also had a long initial backlog in this series;
less player resimulation does not prove all observers or frame tails improved.

Negative evidence is retained: early pure D=3 RTC took about 16.034 s and failed
the unchanged 14 s gate. One pure D=5 WebSocket-relay run completed correct
states but took 23.989 s, including a roughly 12.8 s presentation gap, and also
failed. Its isolated repeat passed at 11.005 s (baseline relay: 10.794 s), with
all three exact final hashes equal. This repeat does not explain away the first
outlier; its cause is unproven. Consult both reports rather than discarding it.
No general “all networks extremely smooth” claim follows from these results.

## Reproduction (PowerShell, from this worktree)

Use local tools/dependencies. Do not fetch all repositories or reinstall online.

```powershell
$env:WASI_SDK_BIN='D:/workspace/eagler/toolchains/wasi-sdk-34.0-x86_64-windows/bin'
node th09_web/tests/multiplayer/run-tests.mjs

$env:TH09_EMSDK='D:/workspace/eagler/th08-eagler/tools/emsdk'
$env:TH09_OUTPUT='artifacts/sdl-adonis'
node th09_web/scripts/build-sdl.mjs
$env:TH09_OUTPUT='artifacts/sdl-adonis-release'
node th09_web/scripts/build-sdl.mjs --release
$env:TH09_RUNTIME_ASSETS='D:/workspace/eagler/worktrees/th09-multiplayer/th09_web/assets/sdl-native'
node th09_web/scripts/build-eagler.mjs --multiplayer

$env:TH09_FIXTURE_ROOT='D:/workspace/eagler/worktrees/th09-multiplayer'
$env:PC_BUILD='th09_web/artifacts/sdl-adonis'
$env:NATIVE_GPU='1'
$env:EAGLER_RELAY_SOURCE='D:/workspace/eagler/worktrees/adonis/eagler-touhou/server/netplay-relay.mjs'
$env:ADONIS_MODE='1'; $env:INPUT_DELAY_FRAMES='5'
$env:RUN_LABEL='adonis-local-new'
node th09_web/tests/multiplayer/transport-browser.mjs
# Use --relay for WebSocket fallback; do not run CPU-heavy comparisons in parallel.
# For --release, set PC_BUILD to th09_web/artifacts/sdl-adonis-release.

$env:ADONIS_MODE='2'; $env:INPUT_DELAY_FRAMES='2'
$env:PEER_FRAMES='12000'; $env:PEER_INPUT_TRACE='endurance-tape'; $env:PAUSE_TRACE='1'
$env:RUN_LABEL='adonis-hybrid-lifecycle-new'
node th09_web/tests/multiplayer/peer-browser.mjs
```

## Remaining work / ownership

### Continuation evidence, 2026-10-02

The continuation starts from TH09 `9050e3d`, common `5669eff`, and Launcher
`45f339d`. It adds bounded test-only transport diagnostics and a sequential,
symmetric-order comparison runner; it does not change TH09 gameplay or the
existing WASM in these measurements.

`th09_web/tests/multiplayer/adonis-series.mjs --relay` ran baseline, hybrid,
pure delay, pure delay, hybrid, baseline, sequentially with the same local
fault-injection policy and the diagnostic WASM
`b6c5cfd2f11cdbd6be821bf28338b64879fa75daa976089c606d51f56e808905`.
Every run reached 600 confirmed player frames and 600 spectator frames, with
all three final hashes equal (`1357358377`). This is local desktop Chromium
evidence, not remote phones, input-to-photon latency or isolated phase benefit.

| Order | Policy | D | Total elapsed ms | Both players' resimulated ticks | Original total 14 s gate |
| --- | --- | --- | --- | --- | --- |
| 1 | Baseline rollback | 0 | 11041.620 | 3690 | PASS |
| 2 | Hybrid | 2 | 25490.495 | 2123 | **FAIL** |
| 3 | Pure delay | 5 | 11153.630 | 0 | PASS |
| 4 | Pure delay | 5 | 11066.195 | 0 | PASS |
| 5 | Hybrid | 2 | 10811.525 | 2571 | PASS |
| 6 | Baseline rollback | 0 | 10999.685 | 3714 | PASS |

Both pure-delay runs also recorded zero snapshots for both players. Callback
P95 was 1.240/1.005 ms and 1.080/0.820 ms for the two pure runs, compared with
4.115/4.355 ms and 3.870/3.655 ms for baseline. Hybrid was 3.090/3.815 ms and
4.120/3.925 ms: lower resimulation is repeatable in this small series, but a
uniform callback-time improvement is not established. D changes applied-input
timing, so this comparison cannot attribute benefits to phase correction alone.

New diagnostics locate the order-2 excess primarily **before gameplay**:
players remained at frame zero in the connecting/preparation phase for roughly
15 seconds. Measured gameplay presentation spans were 10.313/10.279 seconds,
maximum player presentation gaps 349.960/268.040 ms and maximum callback work
8.775/11.600 ms. The exact connection/preparation cause remains unproven. This
does **not** explain the earlier pure-delay relay failure with a 12.8-second
in-game presentation gap; that original negative evidence remains open.

Full reports and logs are retained under
`th09_web/artifacts/multiplayer-tests/adonis-resume-relay-20261002-a-*`.
The `*-summary.json` correctly records `complete: true, passed: false`.
The standalone pure D=5 relay run `adonis-resume-relay-telemetry-1` also passed.

The transport harness now retains a bounded 600-sample frame/confirmation,
callback-gap, route, send/receive, pending-timer and buffered-byte timeline even
on failure. A subsequent instrumentation-only change explicitly splits
`setupMs` and `gameplayMs` and records timestamped status transitions. It keeps
the original combined 14-second gate; a slow setup is not silently reclassified
as a passing session. Do not run the timing comparisons alongside heavy builds.

Reproduce with the existing environment below/above, plus:

```powershell
$env:ADONIS_SERIES_LABEL='NEW-UNIQUE-LABEL'
node th09_web/tests/multiplayer/adonis-series.mjs --relay
```

The runner refuses existing result labels, saves every failure, uses fresh
browser instances, and checks that all reports refer to one WASM build.

### Second sequential series and remaining stalls

`adonis-resume-relay-20261002-b-summary.json` is complete and all six runs pass
the unchanged combined 14-second gate. It uses the same diagnostic WASM as
series a and the new explicit setup/gameplay split:

| Order | Policy | D | Setup ms | Gameplay ms | Resimulated ticks, both players | Largest player presentation gap ms |
| --- | --- | --- | --- | --- | --- | --- |
| 1 | Baseline | 0 | 407.080 | 10489.250 | 3808 | 401.660 |
| 2 | Hybrid | 2 | 348.070 | 10495.610 | 1990 | 351.600 |
| 3 | Pure delay | 5 | 301.785 | 10568.755 | 0 | 417.925 |
| 4 | Pure delay | 5 | 323.965 | 13291.550 | 0 | **1886.965** |
| 5 | Hybrid | 2 | 328.595 | 10467.055 | 1942 | 349.655 |
| 6 | Baseline | 0 | 324.230 | 10477.165 | 3734 | 400.795 |

Each run reached 600 confirmed frames and equal final state hashes for both
players and its spectator. Do not claim identical hashes across all runs:
order 5 ended at `2181465441`, the others at `1357358377`. Physical input timing
and D can change the executed trace. Both pure runs still have zero snapshots
and zero resimulation. Baseline resimulation averages 3771 ticks versus 1966
for hybrid in this series (about 47.9% less); this is an observation of these
local diagnostic runs, not a phase-only causal result or a phone FPS claim.

**Passing the total-duration gate does not mean stutter-free.** In order 4,
around diagnostic time 8.77..10.39 seconds, players remain at frames 114/113.
Both relay receive counters remain at 251/249 while sends continue; receive
age grows from 163 to 1780 ms. Callbacks continue with sampled ages around
0..16 ms and timer lateness remains about 31 ms or less. The later receive
burst permits gameplay to resume. Total phase delay is only 21.468/24 ms.
This narrows this particular long pause to a delivery gap observed at the
browser, rather than seconds of phase debt or expensive rollback (pure mode
has none). It does not establish whether the cause is relay scheduling,
browser/network delivery, or another host condition. An earlier approximately
0.8-second callback pause in the same run is a separate observed event.

Retain this negative frame-tail evidence even though the report's original
aggregate gate passes. Instrument relay receive/forward times and event-loop
lag next; do not reduce gameplay, relax correctness gates, or declare the old
12.8-second outlier fixed based on this shorter pause.

### Remaining title integration

TH08 now has both modes, component/Replay gates, a real multiplayer WASM build,
and 2P/3P browser game gates. Read its own `docs/multiplayer/ADONIS-EXPERIMENT.md`
for the exact evidence and remaining phone/WAN/lifecycle scope. Launcher/relay
exposure is deliberately limited to matching TH08/TH09 experimental Runtimes.
TH06/07 have partial, unvalidated edits; TH10 has its separate tree and common
dependency but no title implementation. Their per-title experiment documents
state the remaining work. Do not enable them merely because common supports it.

For TH09: investigate the retained relay delivery/presentation stalls; obtain real remote phone and
WAN measurements; measure phase-only versus delay-only versus combined policies
with repeated comparable traces; optionally connect startup RTT-tail samples
to an explicitly bounded recommendation. Do not add live D changes without a
transaction. Do not publish an experiment as the ordinary or approved MP build.
