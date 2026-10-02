# TH09 Adonis experiment — implementation and evidence

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

TH06/07/08/10 have separate `experiment/adonis` worktrees but no title Adonis
integration yet. They must not be marked complete because common supports the
policy. Their gameplay, 3P, touch, snapshot owners, Replay/spectator ABI and
restart fences require separate integration and real-world verification.

For TH09: investigate the non-reproduced relay long-stall result; obtain real remote phone and
WAN measurements; measure phase-only versus delay-only versus combined policies
with repeated comparable traces; optionally connect startup RTT-tail samples
to an explicitly bounded recommendation. Do not add live D changes without a
transaction. Do not publish an experiment as the ordinary or approved MP build.
