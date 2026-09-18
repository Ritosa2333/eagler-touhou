# TH07MP mobile smoothness: measured development candidate

## Scope and acceptance

The target is dense-bullet, continuous-touch multiplayer with approximately
100 ms RTT, not a particular synchronization architecture. Rollback remains
enabled. No bullet count, collision rule, fixed 60 Hz simulation step, or
canonical-state check is removed. This is not yet Android/iOS device acceptance:
`adb devices -l` returned no attached device.

The working tree contains earlier contributors' changes. Nothing in this session
has been committed or deployed to 129. Do not treat an uncommitted test binary
as a published release.

## Changes and ownership

- Confirmed-prefix snapshot policy (`frontier`), contiguous live bullet capture
  (`runs`), and bounded bulk-copy backend are retained from the preceding work.
- `StableDrawOrder.hpp` replaces per-draw allocation/comparator pointer chasing
  with stable cached-key sorting. Equal-key order is exactly the old order.
- Coalesced journal restore skips a newer write only when an older checkpoint
  being restored fully covers it. Partial overlaps keep the original copy order.
- Bullet interpolation publishes the two potentially visible regular/despawn
  VMs plus the selected spawn VM. All five endpoints are still initialized at
  spawn. Invisible spawn VMs are not redundantly copied on every logical tick.
- Fresh, once-only input is sent before expensive reconciliation. Retransmission
  and resimulation do not consume hardware input again.
- Item appearance (`isOnscreen`, indicator sprite, alpha and authored VM
  position) now advances on the fixed simulation tick. Draw temporarily uses
  interpolated positions and restores authored values. A real checkpoint-900
  divergence was isolated to items; the canonical hash was not weakened.
- Mobile balanced defaults select six input-delay frames (100 ms at 60 Hz),
  frontier/runs/bulk/coalesced, and at most 60 Hz presentation. Ordinary gameplay
  is unchanged. Desktop MP retains its presentation preference. Explicit per-field
  overrides and `netplayPerformanceProfile: "manual"` permit controlled A/B tests.
- Live synchronization ABI is 6, to reject clients with the old Draw-owned item
  state. Replay gameplay/input ABI remains 5; its byte format is unchanged.
- `TH_WEB_SPEED_OPT` is a separate O3/LTO experiment, without fast-math. Retain
  only after measuring and checking state agreement; O3 alone is not a result.

## Validation completed before O3 comparison

`fluid-v8-behavior.json`: 28 native/WASM SAFE_HEAP cases passed, including
300,000 real-AnmVm visible-state comparisons, 40,000 item appearance cases,
6,000 exact stable-sort comparisons, 3,000 overlapping-history byte tests,
and 18 confirmed-prefix model runs of 4,200 frames each.

`netplay-performance-profile-test.cjs`: mobile, desktop, single-player,
explicit/manual overrides and absence of later option overwrite passed.

## Measurement caveats that are part of the result

CPU throttling is not a phone model. The real-stage tests used Windows Edge 153,
640x480 canvas, and Intel UHD D3D11 hardware rendering (not SwiftShader).
RTC application-send impairment is distinct from relay-imposed delay: a busy
sender's timers can add delay beyond the requested 50 +/- 10 ms. Do not call
an un-impaired RTC run a 100 ms RTT run merely because relay delay was configured.

The test-only late-stage entry executes real Stage 6 ECL/STD at Lunatic with
invulnerable probe players, so bot deaths cannot conveniently clear the workload.
It does not prove human Bomb/deathbomb feel or long-term phone thermals.

## Preserved observations and failures

All paths below are relative to `.codex-tmp/`.

| Run | Observation |
| --- | --- |
| `fluid-now-cap60.json` | Persistent 900-bullet relay/continuous-touch test, 2x slow peer: about 60 logic FPS with the existing cap, versus about 49 without it in the earlier same-condition run. |
| `fluid-v4-rtc2-curved/` | Harder 900 turning/multiple-sprite workload at 2x CPU reached only about 44-46 logic FPS. This is not smoothness acceptance. |
| `fluid-v6-earlyinput/r2-d6-frontier.json` | Failed checkpoint 900, items only. Other state owners agreed. Led to the fixed-tick item repair. Do not delete this failure. |
| `fluid-v7-realstage6.json` | Original Stage 6 Lunatic, 3600 frames, all 12 checkpoints matched. Peaks 1024 bullets/403 items. About 57 logic FPS; up to 199 ms presentation gaps under RTC application-delay injection. |
| `fluid-v8-trace-stage6-relay.json` | Same real stage, independent relay 50 +/- 10 ms, slow peer 1.5x: slow presentation p99 25.5 ms but a 13-second incoming-data blackout. Whole-run logic FPS about 49; do not omit the blackout from totals. |
| `fluid-v9-o2-auto/` | Auto profile activated correctly, but the test compared its forced cap with the manual-cap CLI flag and failed that assertion. The expectation was corrected, without changing game behavior. |
| `fluid-v9-o2-diagnostics.json` | All 12 state checkpoints matched; slow presentation p95 20.7 ms/p99 23.2 ms, whole-run logic FPS 55.08. Two incoming-data blackouts (~1.8/3.5 s) remain in raw results. |

The latter blackouts are measured separately from dense-scene computation:
simulation and draw take only a few milliseconds at the stall boundary; the
relay's one-second heartbeat remains normal (maximum interval ~1015 ms), its
socket byte-read counters stop, receivers have zero buffered bytes, sockets are
not paused, and pending writes are zero. This narrows the failure to delivery
before the relay receives bytes, not an expensive 3-second rollback. It does not
identify a specific browser/OS/network defect or prove a fix for it.

Transient instrumentation/compile failures are not performance samples. Real
performance failures are retained even when later runs pass. The test's former
driver-call timeout was replaced by a wall-time deadline; its independent
15-second missing-input watchdog was not weakened.

## Reproducibility

Use `tests/measure-netplay-smoothness.py` to freeze runtime, harness, data and
font identities before serial paired runs. `--auto-profile --bridge-drag`
tests product defaults rather than feeding all tuning knobs explicitly.
`--transport relay --one-way-ms 50 --jitter-ms 10` applies delay in the separate
relay process. `--transport rtc` explicitly instruments real RTC input sends.
Do not compile or run another CPU benchmark concurrently with a measurement.

Raw reports contain bounded per-present long-frame records, CPU costs,
snapshot/restore bytes, heap growth, actual input-impairment timings,
renderer identity and canonical checkpoint values. PASS denotes execution and
state agreement, not automatically smooth presentation.

## Final comparison and closeout

### Compile choice

O3/LTO (`fluid-v10-o3-auto/r1-d6-frontier.json`) passed all twelve checkpoints
but did not show a demonstrated gain over O2. The slower peer's whole-run
presentation p99 was 26.2 ms, versus 23.2 ms in the O2 diagnostic run. Physical
input timelines differ, so this is not proof of a universal O3 regression. The
candidate was returned to O2, `TH_WEB_SPEED_OPT=OFF`.

### Frozen, real-stage A/B

`fluid-final-frozen/` freezes the O2 runtime and harness. The same binary was
tested with manual legacy settings (3-frame delay, always/objects/wasm/sequential,
uncapped presentation) and actual balanced mobile defaults (the host does not
forward individual tuning overrides). This compares the configuration package,
not every individual code change against the old e78d330 binary.

Both use Stage 6 Lunatic, 3600 logical frames, continuous Launcher touch drag,
50 +/- 10 ms independent relay delay, and Edge with peer 1 CPU slowed 1.5x.
The dense phase is predefined as consecutive presentations both having >=500
bullets, after warm-up. All gaps in that phase count, with no outlier filtering.

| Slower peer | Legacy settings | Balanced run 1 | Balanced run 2 |
| --- | ---: | ---: | ---: |
| Dense logical FPS | 39.42 | 60.01 | 60.02 |
| Dense Present p95 (ms) | 93.9 | 21.2 | 20.8 |
| Dense Present p99 (ms) | 135.8 | 23.5 | 22.5 |
| Dense maximum gap (ms) | 169.0 | 35.3 | 36.6 |
| Dense gaps >50ms | 134/1080 | 0/1090 | 0/1089 |
| Mean dense bullet count | 666 | 757 | 758 |
| Whole-run logical FPS | 52.20 | 56.78 | 58.41 |

Raw reports: `fluid-final-stage6-legacy-settings.json`,
`fluid-final-stage6-auto-r1.json`, `fluid-final-stage6-auto-r2.json`.
All three passed twelve cross-peer canonical checkpoints. Balanced runs still
contain 3.4 s and 1.7 s low-bullet delivery blackouts respectively; these are
included in whole-run timings. The legacy FAST peer's ordinary Present array
hit its 4096-sample limit (6815 total presentations), so that truncated array
must not be called a whole-run distribution. The dense arrays in this table
did not hit their limit.

### Blackout control without the game

`websocket-loopback-health.py` runs blank same-origin pages against a minimal
echo server, with no game, WASM, rendering or rollback. In
`loopback-health-origin-default.json`, both pages kept sending (max send gap
29/45 ms) while receiving stopped for 1.65 s. All messages eventually arrived.
Explicit browser direct mode also ended with 450/438 unanswered messages in
`loopback-health-origin-direct.json`; its small completed-reply p99 is NOT a
fix, because the end blackout is right-censored. The probe now reports pending
count and receive age, and fails an incomplete run. No system proxy setting or
production relay behavior was changed. This proves game computation is not
necessary to reproduce this particular local delivery failure, but does not
identify the browser/OS/network root cause.

### Functional regressions

Initial bundled-Chromium Replay runs hit page-load timeout/WebGL context errors;
the initial spectator run timed out. Their logs remain at
`fluid-final-replay-2p.log`, `fluid-final-replay-3p.log`, and
`fluid-final-spectator.log`. The Replay HTTP fixture was changed from a
single-connection server to ThreadingHTTPServer, and the regressions gained
an explicit browser-channel option. No exact-input or state assertion changed.

On the same Edge channel used for performance:
- `fluid-final-replay-2p-edge.log`: PASS, 306/306 exact input frame comparisons;
  recorded runs exercised 326/191 rollbacks before saving and playback.
- `fluid-final-replay-3p-edge.log`: PASS, 64/64/92 exact input comparisons;
  recorded runs exercised 117/115/113 rollbacks.
- `fluid-final-spectator-edge.log`: PASS, player/spectator checkpoint 300
  agrees; spectator remains read-only.
- `fluid-final-spectator-rtc.log`: PASS with actual player RTC transport,
  player/spectator frames 301/302/301 and the same agreed checkpoint-300 hash.

### Ordinary build and 3P density

`fluid-final-ordinary-build.log`: non-MP `build-web-eagler-thprac` compiled
successfully (netplay/gameplay multiplayer both OFF). This is a compile gate,
not full human single-player acceptance.

`fluid-final-3p-dense.json`: all six 3P checkpoints passed over 1800 frames,
900 persistent moving test bullets, real touch bridge, 50 +/- 10 ms relay,
slow player 2 at 1.5x CPU slowdown, production balanced defaults.

| Peer | Whole-run logic FPS | Dense logic FPS | Dense p99 ms | Dense maximum ms | Dense >50ms |
| --- | ---: | ---: | ---: | ---: | ---: |
| 0 | 59.99 | 59.99 | 24.5 | 26.7 | 0/1499 |
| 1 | 59.99 | 60.01 | 24.3 | 33.4 | 0/1499 |
| 2, slowed | 60.02 | 60.01 | 27.9 | 38.3 | 0/1498 |

This 3P engine fixture is distinct from the real Stage 6 comparison. Combined,
these results justify a mobile trial candidate while retaining rollback.

The extra bundled-Chromium Replay retest exceeded its outer 180-second command
deadline without producing a result (`fluid-final-replay-2p-chromium-retest.log`).
Do not silently promote the passing Edge gates to all-browser acceptance.

### Remaining device acceptance

Actual Android/iOS, audio-on Boss sequences, thermal steady-state and subjective
deathbomb response are not device-verified. The balanced 6-frame scheduling
delay is approximately 100 ms, NOT 50 ms; that responsiveness tradeoff must be
stated to users. Desktop CPU throttling alone does not certify a phone model.

## Superseding mixed-device policy: endpoint rollback ownership

The fixed six-frame mobile profile above remains a conservative fallback, but
it is no longer the preferred policy for a room containing exactly one mobile
endpoint and one or two standard/desktop endpoints. Public-device feedback
confirmed that six frames (~100 ms) fixed local scheduling delay removes the
high-bullet slowdown but is too damaging to immediate-control feel.

The accepted mixed-device policy moves rollback ownership to the stronger
endpoint(s) instead of making the phone pay the snapshot tax:

- the sole mobile endpoint schedules local input one frame ahead (~16.7 ms),
  uses `netplayRollbackPolicy="buffered"`, creates no predicted forward state,
  captures no rollback snapshots and waits only when an exact remote input has
  not arrived by its simulation deadline;
- each standard endpoint schedules four frames ahead (~66.7 ms) and retains
  the ordinary full prediction/rollback path, absorbing late low-latency mobile
  input corrections;
- rooms with zero or multiple mobile endpoints deliberately retain the prior
  conservative Runtime profile. The single-mobile evidence must not be
  generalized to mobile-vs-mobile without a separate acceptance run.

This is endpoint-local rollback policy, not global lockstep and not a gameplay
semantic change. Every endpoint still executes the same authoritative logical
frames and canonical state must agree.

### Formal production-policy acceptance

The following reports use the production `netplayRollbackPolicy` field rather
than the old test-only no-rollback switch. Both use real Stage 6 Lunatic,
continuous Launcher bridge drag, a 1.5x CPU-slowed mobile-designated endpoint,
actual RTC input sends delayed 50 +/- 10 ms one-way, 60 Hz presentation cap,
and 1800 logical frames. The Stage 6 workload peaks at 1024 active bullets.

`2p-production-owned-1800.json`:

| Endpoint | Policy / local delay | Logic FPS | Dense FPS | Dense p99 | Dense max | Rollback / resim | Snapshot ticks |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| standard | full / 4 frames | 60.00 | 60.01 | 28.7 ms | 35.9 ms | 1087 / 5368 | 5744 |
| mobile, slowed | buffered / 1 frame | 59.98 | 59.97 | 23.5 ms | 24.9 ms | 0 / 0 | 0 |

All six 300-frame canonical checkpoints match; there were no reported stalls.

`3p-production-owned-1800.json`:

| Endpoint | Policy / local delay | Logic FPS | Dense FPS | Dense p99 | Dense max | Rollback / resim | Snapshot ticks |
| --- | --- | ---: | ---: | ---: | ---: | ---: | ---: |
| standard P1 | full / 4 frames | 59.98 | 59.93 | 27.2 ms | 29.6 ms | 1076 / 5200 | 5577 |
| mobile P2, slowed | buffered / 1 frame | 59.98 | 59.97 | 23.0 ms | 28.2 ms | 0 / 0 | 0 |
| standard P3 | full / 4 frames | 59.98 | 60.00 | 26.9 ms | 28.0 ms | 1086 / 5275 | 5652 |

Again, all six canonical checkpoints match and no stalls were reported. The
mobile endpoint's rollback snapshot/resimulation cost is therefore eliminated
without deleting rollback from the room; the desktop endpoints deliberately
carry that work instead.

These browser/CPU-throttling results are strong enough to call this a mobile
trial candidate at roughly 100 ms RTT-class conditions with ~17 ms of local
logical input buffering. They still do not replace real Android/iOS thermal,
audio-on and subjective deathbomb acceptance.
