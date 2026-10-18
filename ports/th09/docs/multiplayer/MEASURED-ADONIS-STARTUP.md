# TH09: actual-channel startup, with and without rollback

Worktree: `D:/workspace/eagler/worktrees/adonis/th09`, `experiment/adonis`.
Starting title commit `364661a`; common dependency `f7f7889`.
TH09 remains the only new adaptation target. No ordinary tree, other title,
remote push or public deployment is part of this change.

## Meaning of the two policies

Both policies now use the actual Runtime/input transport all the way through:
native world/resource preparation, input-lane probing, two-sided negotiation,
native gameplay HELLO/READY, once-only forward input, repair/confirmation and
bounded running-phase correction. It is a clean implementation in this
project's protocol, not execution or redistribution of the supplied old DLLs.

Let B be the measured full-buffer estimate, P the permitted prediction reserve.
Pure delay uses `D=B, P=0`. Hybrid auto targets D=1, removing at most two frames:
`P=min(B-1,maximumReserve)`, where maximumReserve=2 by default and 1 is supported
by the session contract. B=1/2/3/4 therefore gives D=1/1/1/2. This removes queued input frames;
it does NOT add a new one/two-frame queue. Existing exact input always wins,
prediction is not compulsory, and the original eight-frame rollback/history
window is not reduced to P. Unexpected jitter can still exceed the planned
prediction reserve. Manual D=0..9 stays unchanged even when it differs from B.

An example B=7 therefore yields pure D=7, hybrid P=1/D=6, or P=2/D=5.
These are queue settings, not measured input-to-photon latencies.

## Measurement and transaction

After all required input channels and peer HELLOs are ready, the shared
`AdonisStartup` owner waits one second before sending a probe. A channel closure
before probing restarts this interval. The wait produces no RTT samples; it is
the requested connection-settling step, separate from the Adonis2 schedule.

The owner then sends 129 probes (numbered 1..129, measuring slots 10..129)
with 16 ms relative intervals and a 16+200 ms final reply wait, at most one new probe per pump. Probe and echo both use
`PeerTransport::SendTo`, on the same input DataChannel or selected WebSocket
fallback used by gameplay. There is no second probe connection and no use of
the Launcher's historical minimum RTT or phone-count presets for TH09 auto.

Each peer freezes its P95-style RTT statistic and success/loss counts. The
current policy (updated 2026-10-03 to follow the supplied Adonis2 timing) is:

```
B = max(1, ceil(floor(max(peer RTT-tail estimates) / 2) * 60 / 1,000,000))  // RTT in microseconds
P = mode == hybrid ? min(B - (auto ? 1 : 0), requested reserve of 1 or 2) : 0
D = auto ? B - P : manually requested D
```

The former extra one-frame Runtime margin has been removed; it was
not a measured CPU time. RTT/2 is still a symmetric-path approximation, not an
exact measurement of one-way delivery. Probing observes actual delivery plus
Runtime pump wakeup but does not reproduce a dense boss's CPU/GPU workload.
Do not present this initial estimator as an optimal or guaranteed buffer size.

At least 96/120 valid samples are required. Late/missing replies are counted,
not inserted as zero latency. A >500 ms measuring-pump interruption, hidden
document, route change during calibration, inadequate replies or ten-second
startup timeout fails explicitly. Auto D>9 also fails with retry/manual guidance
instead of silently clipping an insufficient budget. No guessed fallback starts
the game. D stays fixed for the run; live D resizing is not implemented.

The `ADS/2` 64-byte protocol binds session/generation, seed, gameplay/build ABI,
mode, requested D/auto and prediction reserve. Both immutable summaries feed the
same deterministic choice. P1 proposes, P2 accepts, P1 commits, P2 acknowledges.
Retries/duplicates are idempotent. Native gameplay HELLO then binds the chosen
D and reserve again. Up to 16 valid early native session packets can be deferred
across a retried commit; gameplay input cannot pass the pre-frame-zero barrier.

`th09_measured_begin` prepares the native world and creates this pending owner;
`th09_startup_info` exposes phase, attempts/replies, both tails/loss counts, B/D/P
and active phase allowance. `th09_rollback_pump` drives calibration even while
the main game loop is paused. `Ready()` and capture remain false until the
measured transaction and native session gate have both completed.

## Running phase, Replay and spectator

`SessionChannelConfig.adonisPredictionFrames` reserves the intended one/two
frames in the wall-clock phase controller as well. Only mean waiting beyond
that allowance becomes an extra bounded delay. Otherwise the phase controller
would slowly put back the latency removed from D at startup. Raw due/arrival
statistics are retained; deadband/cooldown/correction cap and 60 Hz remain.
The field defaults to zero, preserving all existing common-library consumers.

Pure mode still forbids world snapshots, speculative commits and resimulation.
Hybrid retains the complete existing rollback owner. Neither mode resamples
physical input during retries or historical replay. Calibration probes never
enter gameplay input history or Replay. There is no presentation-only player
prediction or gameplay reduction.

P1 sends a versioned `T9TM/1` timing description before the confirmed spectator
frame stream. The relay validates that envelope for TH09 only and preserves it
in the admitted backlog. Spectators check build/mode/request and chosen policy,
then consume already-applied frames with their own queue delay zero. They do
not independently calibrate, vote on D, or apply the player's delay twice.

## Launcher and relay

TH09's existing rollback switch remains to the left of the delay capsule. The
automatic label is now `自动 · 开局实测`, then `正在实测…`, then `实测 · Nf`.
The Launcher sends an unresolved `netplayInputDelayAuto` request and default
`netplayPredictionReserve=2`, not a numeric recommendation based on device type.
Manual D and host-only control remain. Matching experimental Runtime is required;
an old Runtime missing the measured-start export fails rather than ignoring auto.

The existing epoch-fenced `runtime-info` event carries `netplayTiming`. It does
not erase renderer information when carrying only timing. A validated result is
mirrored by P1 with the current room serial; relay publication is display-only,
not an additional source of native simulation authority. P2, stale serials and
changed live results are rejected. The native peers already agreed before play.
TH08's earlier experiment and other titles' policies are not enabled or changed.

## Verified evidence

### 2026-10-03 automatic hybrid target refinement

Automatic hybrid now keeps at least one queued input frame and saves at most
two. The existing rollback history remains intact; manual D stays authoritative.
Common CTest passes 30/30, including a 32 ms two-sided hybrid startup that agrees
on B=1/D=1/P=0 after dropped control/retry. TH09 WASI passes a complete 600-frame
low-latency measured hybrid session with the same choice, exact reference state,
once-only capture, and zero phase prediction allowance. Launcher and real Relay
gates cover B=1/2/3/4/5, reject the former automatic D=0 result, and preserve
host authority and immutable timing publication.

Release `sdl-adonis-one-frame-release-20261003` has SHA-256
`74a449fc40a4cc0fdb3f8b32290c321af7b5b06301b008a6f80d5baa218d5e0b`.
Both RTC and Relay browser runs pass with a confirmed spectator under injected
startup/gameplay impairment (B=5/D=3/P=2). Reports are
`one-frame-release-hybrid-rtc-02-report.json` and
`one-frame-release-hybrid-relay-report.json`, setup 3389.660/3082.685 ms and
gameplay 11455.090/12653.380 ms. These Release runs have no forced frame limit.

Public test acceptance is recorded under
`D:/workspace/eagler/dist/test-adonis-one-frame-20261003/`:
`public-hybrid-low-report.json` agrees on B=1/D=1/P=0 across both players and
the spectator. `public-hybrid-b2-report.json` adds 20 ms only to each startup
RTC probe/echo send, then uses the normal gameplay channel; both players agree
on B=2/D=1/P=1 at P95=61.900 ms. Players/spectator finish at 1115/1118/1110
frames with 106 equal common-frame hashes. This is a controlled startup
measurement case, not a 61.9 ms WAN performance claim. Main remains unchanged.
The separate `public-hybrid-low-progress-report.json` repeats low-latency
acceptance with an explicit final progress gate: B=1/D=1/P=0, P95=2.300 ms,
players/spectator at 1137/1140/1133 frames and 142 equal common-frame hashes.

Common native CTest: 30/30 PASS, including startup, phase, core and channel.
Title WASI component suite: PASS (`measured-component-02.log`), including ten
measured startup/600-frame cases, pure/hybrid, auto/manual 0/1/9, P=1/2, lost
commit/ack handoff, no input before readiness, exact reference state and capture
counts. An input-lane black hole fails before any gameplay capture. Existing
dynamic-state, Bullet restore and ordinary scheduling gates remain passing.

Initial real-browser reports use diagnostic WASM
`5ffa4c74eb99d92fbc173ef15d44ea4ef1516819e0a407f25f09e18d4511cf9c`:

| Report prefix | Actual route | Request | B / D / P | Setup ms | Gameplay ms |
| --- | --- | --- | --- | --- | --- |
| measured-hybrid-rtc-01 | RTC | auto, hybrid | 7 / 5 / 2 | 4556.180 | 10323.210 |
| measured-pure-rtc-01 | RTC | auto, pure | 7 / 7 / 0 | 4790.660 | 10485.080 |
| measured-hybrid1-relay-01 | relay | auto, hybrid | 7 / 6 / 1 | 3248.170 | 10312.285 |
| measured-manual9-relay-01 | relay | manual 9, pure | 7 / 9 / 0 | 3212.905 | 10448.385 |

These tests install delay/loss BEFORE connecting, on the actual transport
prototypes, and assert that >200 calibration probe/echo sends hit the expected
input lane. They do not calibrate a pristine connection then only impair the
game. Players and spectator complete exactly 600 confirmed frames and equal
same-frame canonical hashes. The host timing result is also observed in the
relay lobby snapshot. Pure cases assert no snapshots/corrections/resimulation.

The startup is now mandatory extra work. Reports retain the old combined 14s
boolean (false for the first two cases above); new acceptance explicitly uses
<=15s connection/calibration setup and the unchanged <=14s gameplay budget.
This is a new lifecycle gate, NOT a claim that old total latency improved or
that aggregate PASS means stutter-free. All raw counters/tails remain in the
reports. Earlier 1.9s/12.8s delivery/presentation stalls are not declared fixed.

The final native handoff refinement additionally defers early session traffic
across a lost commit; its frozen V2 builds/reports are recorded below after
validation. Initial builds remain retained, not overwritten as new evidence.

Launcher: TypeScript build (55 sources), measured payload/options/snapshot,
spectator envelope, original protocol and real relay authority/immutability
tests PASS. Real DOM + relay UI gate `artifacts/adonis-measured-ui-02` passes
1280/960/390/320px, English 320px, keyboard toggle, host rights, manual D and
new-room reset. An initial locale-label assertion failed because the static
translation overwrote the dynamic auto caption; removing that stale binding
and rerendering room timing on locale changes fixed it. First failure retained.

The UI fixture has no playable assets; real WASM/transport checks are the
separate browser reports, not a claim that that UI fixture ran the game.
No phone/WAN/thermal or dense-boss performance improvement is claimed.

## Reproduce

### Final V2 builds and acceptance

The final handoff refinement builds successfully in both configurations:

* Diagnostic `artifacts/sdl-adonis-measured-v2`, 2,835,700 bytes,
  SHA-256 `77d788d6d0dc1e5c0ebfeecbba9b3a65ca80d2a36a7fbe43f2900e3a638f0eba`.
* Release `artifacts/sdl-adonis-measured-v2-release`, 2,812,004 bytes,
  SHA-256 `aaf0fab5a2a67ec12a07ec076b91d1fbc5a9ba17ec395aca638950b51ed32115`.
  No development probe exports. Local MP packaging now uses this exact WASM;
  `build-eagler-multiplayer/version.json` is `aaf0fab5a2a67ec12a07ec07`.

The four V2 diagnostic cases below all PASS exact 600-frame confirmation,
equal player/spectator same-frame hashes (`1357358377`) and **byte-identical
1,003-byte Replay exports across all three worlds**. This adds a real Replay
check to calibration/transport ownership rather than inferring it from a join.

| Report prefix | B / D / P | Setup ms | Gameplay ms |
| --- | --- | --- | --- |
| measured-v2-hybrid-rtc-02 | 7 / 5 / 2 | 4633.780 | 10338.515 |
| measured-v2-pure-relay | 7 / 7 / 0 | 3204.330 | 11562.150 |
| measured-v2-hybrid1-rtc | 7 / 6 / 1 | 5075.690 | 11208.450 |
| measured-v2-manual9-relay | 7 / 9 / 0 | 3238.650 | 11245.245 |

Release `measured-v2-release-pure-relay` PASS: B7/D7/P0, setup 3238.465 ms,
gameplay 10799.370 ms, player frames 603/604, viewer 601, no prediction/world
snapshots/resimulation. Release `measured-v2-release-hybrid-rtc-03` PASS:
B7/D5/P2, setup 4636.365 ms, gameplay 10364.675 ms, player frames 605/607,
viewer 601. The largest player presentation gaps in that hybrid run are
268.980/234.630 ms; the viewer gap is 349.490 ms under injected impairment.
Both use the final Release WASM. They intentionally lack forced frame limits,
so **no equal-final-frame hash or Replay equivalence claim** is made from them.

### Retained negative evidence and limits

The first `measured-v2-hybrid-rtc` attempt failed fetching a resource before
calibration/gameplay; its exact failing resource was not captured. The following
run passed. This does not identify the first failure's cause. The harness now
retains bounded failed-request/HTTP-error details on failure.

More importantly, `measured-v2-release-hybrid-rtc` **FAILS the gameplay budget**:
setup 4494.550 ms, 24013.445 ms until players AND viewer passed 600 frames.
Players progressed to 1401/1403 (confirmed 1401), while the viewer was at 606.
Player callback maxima were 7.925/3.985 ms and presentation gaps 376.800/377.615
ms; the viewer had a **13,952.110 ms presentation gap**. Player phase correction
totals were only 2.500/10.916 ms. This is evidence of a spectator delivery/catch-up
problem while the players kept advancing, not a 14-second atomic player rollback.
The three final hashes are at different logical frames and cannot establish a
desync. The cause remains open; the later passing run does not erase it.

The instrumented `...hybrid-rtc-02` repeat hit an erroneous observer gate:
it required one relay socket to both receive AND send binary data. RTC gameplay
does not require that: P1 uploads spectator data and a different viewer socket
receives it. The gate now checks those directions across the participating
sockets, and always saves the completed game's report before checking observer
success. The failed repeat/log is retained, not retroactively marked PASS.
The corrected `...-03` contains 206 relay samples; pure-relay has 197.

The feature has functional and build gates, but **not all-run smoothness or
production performance acceptance**. Local packaging does not authorize release.
Do not claim mobile/WAN results, eliminate the retained stalls, or equate fewer
queued frames with measured end-to-end latency or CPU improvement.

### Commands

From this title tree, use existing local SDK/cache and unique output labels:

```powershell
$env:WASI_SDK_BIN='D:/workspace/eagler/toolchains/wasi-sdk-34.0-x86_64-windows/bin'
node th09_web/tests/multiplayer/run-tests.mjs
$env:TH09_FIXTURE_ROOT='D:/workspace/eagler/worktrees/th09-multiplayer'
$env:PC_BUILD='th09_web/artifacts/sdl-adonis-measured-v2'
$env:NATIVE_GPU='1'
$env:EAGLER_RELAY_SOURCE='D:/workspace/eagler/worktrees/adonis/eagler-touhou/server/netplay-relay.mjs'
$env:ADONIS_MODE='2'; $env:INPUT_DELAY_AUTO='1'; $env:PREDICTION_RESERVE='2'
$env:RUN_LABEL='NEW-UNIQUE-LABEL'
node th09_web/tests/multiplayer/transport-browser.mjs
# Add --relay for the real WebSocket fallback; --release requires its Release build.
# Manual D: INPUT_DELAY_AUTO=0 plus INPUT_DELAY_FRAMES=0..9; calibration still runs.
```

Run timing cases sequentially without compilers or other timing comparisons.
Continue with controlled phase/reserve estimator comparisons, full Launcher +
Runtime product acceptance and real remote phones. Keep D transactions separate
from this fixed-run design; do not resume broad title adaptation prematurely.
