# TH09 resumed smoothness work — 2026-09-28

## Current local candidate

Base commit: `bf60dbc`, branch `experiment/th09-multiplayer`. The user resumed
the previously paused task. Work is local; no push, deployment, DATA/OGG or
404-route change is authorized or performed. The canonical `th09-eagler` tree
is not the optimization owner.

Current diagnostic candidate: `artifacts/sdl-smooth-retry` under `th09_web`,
WASM SHA-256 `a7e5e500f942da06f8cd750a044b713ac0b22458b5e9e9439d09706ec2ee24b0`.
Implementation is on the experiment branch; use its log/status to identify the
local commit rather than assuming the old handoff is its HEAD. This is a locally
packaged release candidate, not a published version.

The candidate has passed the 1,800-frame round-reset/full-state/pixel gate and
per-frame four/eight-frame sustained rollback against a no-rollback reference.
The unsampled final RTC A/B/B/A comparison is
`retry-tape-{a1,b1,b2,a2}`; all four final worlds had hash `1999583347` and the
same transmitted inputs and 48-field audited state. This supersedes the
earlier `smooth-tape-*` comparison, whose candidate lacked the pending retry.
The final sparse/dense/round/Eiki-Yuuka world gates, default packet-fault peer
gate, RTC/Relay plus spectator gates, and production-build RTC/Relay smoke gates
also passed. `rollback-smoothness-evidence.json` keeps their individual scopes
and report hashes. Never treat an intermediate artifact as the current build.

## Changes and invariants

1. Bullet POD snapshots retain complete active contiguous runs. Before-spawn
   and full-pool-reset hooks retain dormant bytes on first write. Retained
   collision-hazard pointers also protect dormant slots. Shared
   `SparsePoolCapture` deduplicates slot identity so a slot write cannot partly
   overlap an already captured run. Capacity, overflow, order and RNG are intact.
2. Each Bullet's five animation VMs have independent first-write ownership.
   Update and stateful Draw save the whole VM they actually mutate; creation,
   type changes and round reset share the same five-slot bitmap. This does not
   drop VM fields, skip Draw state changes or rebuild animation approximately.
   The dense full-state/hash oracle still traverses every VM independently.
3. The 10 ms outer-loop budget no longer clears the fractional clock remainder
   when all due ticks were completed. Budget limits, actual-wait behavior and
   the bounded debt policy are unchanged. Physical input is still captured
   exactly once per forward frame and never sampled while replaying history.
4. `FrameSchedule` keeps one already-due tick pending across network or recovery
   waits. The next RAF can retry it without first earning another 1/60 second.
   Only fractional phase survives a wait; no elapsed stall time accumulates as
   extra forward-tick debt. Pause/restart resets the retry. Ordinary high-refresh
   and non-rollback cadence remains unchanged. A dedicated production-helper
   test covers 30/59.94/60/90/120/144/165/240 Hz, long stalls, budget completion,
   mode exit and pause/restart.

All original logical timing, collision, enemy scripts, resources, local input
delay (zero), direction prediction (three missing frames), rollback window
(eight frames), confirmed side effects and Replay ownership are unchanged.

## Frozen builds and intermediate evidence

All paths here are relative to `th09_web`. Intermediate builds are retained;
do not overwrite them or add percentages from different experiments.

| Build | WASM SHA-256 | Meaning |
| --- | --- | --- |
| `artifacts/sdl-resume-baseline` | `faaa5e05d2b53d1b8cc28c4d137d9a353e14ad4345ac06c5b38eb9c047f115b8` | Original implementation, read-only probes, opt-in function names |
| `artifacts/sdl3` | `bdd46451e03ecee5dbc52d2428280d282021e0781beb14704b04e4a4b66d2a54` | Sparse Bullet POD only; not current source |
| `artifacts/sdl-bullet-runs` | `af22ef6a9e478a66fadc5ee78bf0c94242df35770e5c158da498ef37a37676e2` | Intermediate full five-VM contiguous runs |
| `artifacts/sdl-vm-first-write` | `99cbda63a8a73bf4738ec05264a7199244cba6357fd8c4162eba408463ae823e` | Per-VM first writes before clock-remainder fix |
| `artifacts/sdl-smooth-candidate` | `62d8a64da91cff45ecdc5ed0e57b691469a712fbd841e52a7e80e172f702cdf8` | Per-VM first writes plus clock-remainder fix |
| `artifacts/sdl-smooth-retry` | `a7e5e500f942da06f8cd750a044b713ac0b22458b5e9e9439d09706ec2ee24b0` | Current: one pending retry without stall debt |
| `artifacts/sdl-smooth-release` | `d1dafe25e34d8deb5811ac3504a77750c40316952b004da9319ae3ef12c5eebc` | Current production-flag build, no diagnostic entry points |

`resume-named-rtc77-slow2` retained V8 CPU profiles. On the throttled endpoint,
`eagler_journal_copy_bytes` accounted for 2,214 ms self time, `WorldState::Save`
about 2,619 ms inclusive and `WorldState::Restore` about 1,062 ms inclusive.
Sampling is diagnostic and is disabled for performance acceptance.

`pod-phase-{a1,b1,b2,a2}` used the same fixed-input PC phase fixture, Intel UHD
ANGLE/D3D11, with full-state checks outside timing. Means over the two runs:
capture 91.295 -> 70.435 ms, restore 70.600 -> 48.1175 ms, isolated
capture+restore+eight-tick replay 362.570 -> 317.675 ms. The latter is 12.38%
less work in that fixture, but p99/max outliers were worse. It is not a
sustained per-frame history-ring or frame-pacing result. First-write work can
move into forward simulation, so capture-only numbers are never the verdict.

`pod-rtc-*` wall-clock-input runs varied in captured input and bullet density.
They are retained exploratory measurements, not a controlled cross-build
performance verdict. `pod-tape-*` subsequently verified every transmitted
input frame against the same tape; all four final hashes were `1999583347`.
POD copying cost improved but submitted-frame pacing did not clearly improve.
The full-VM-run intermediate passed sparse, dense, round-reset, Eiki/Yuuka,
and exact cold-slot/reset boundary checks (`runs-world*`). Its two RTC tape
runs also ended at `1999583347`, but are not a fresh four-run ABBA experiment.

## Final diagnostic RTC and sustained-history results

`retry-tape-comparison.json` validates the actual current candidate, frozen
baseline, browser/GPU, harness/relay/shell hashes, complete actual input tapes,
terminal world inventories, and chronological A/B/B/A order. Each endpoint has
3,238 measured forward frames across its two runs (warmup excluded). Conditions:
single-direction send delay 77 +/- 10 ms, P2 CPU throttling 2x, native GPU,
two independent Chromium processes, no active CPU sampler.

| Metric | P1 before -> after | P2 throttled before -> after |
| --- | --- | --- |
| Logic rate | 58.51 -> 59.79 Hz | 58.46 -> 59.80 Hz |
| Submitted-RAF gap p99 | 33.33 -> 16.67 ms | 33.33 -> 16.67 ms |
| Submitted-RAF gaps >25 ms | 71 -> 12 | 135 -> 17 |
| Longest submitted-RAF gap | 50.00 -> 33.33 ms | 66.67 -> 33.33 ms |
| Waiting callbacks | 39 -> 8 | 41 -> 16 |
| Work per forward frame | 1.610 -> 1.454 ms | 4.096 -> 3.809 ms |

Both endpoints replayed slightly MORE frames in the final candidate; work per
forward frame still fell 9.65% / 6.98%. P2's **maximum work** did not improve
(18.97 -> 20.69 ms). Do not hide that outlier or substitute the earlier 18.27%
CPU result from the different `smooth-tape-*` experiment. The demonstrated
improvement is especially in scheduling/submission tails, not a promise that
every individual callback is cheaper.

At 39 +/- 5 ms per direction with the same P2 throttle, both
`retry-rtc39-{1,2}` completed the same 1,800-frame input trace and terminal
world hash. Logic rate was 59.96–60.00 Hz. Each P2 run had two >25 ms submitted
RAF gaps, maximum 33.33 ms, none over 50 ms. The single extra pending-recovery
callback in each run did not force a needless additional idle callback.

`retry-depth4` and `retry-depth8` exercise the actual per-frame checkpoint
ring, not a single snapshot around an eight-tick block. A deterministic hosted
charge/strafe trace with alternating focus causes one correction every frame;
the real battle reaches 383 active bullets. Each run has 1,800 corrections per
peer, 7,194 / 14,372 historical ticks replayed, and 1,620 steady calls per peer
completing the requested four/eight-frame correction. Eight-frame work p99 was
8.53 / 8.96 ms and maxima 10.15 / 10.56 ms, on the unthrottled native-GPU PC.
All three worlds (two peers plus exact-input reference) ended at `3545558762`,
with identical audited state, pixels and 1,986-byte Replay. This manual
iteration-delay gate is sequential in one browser and is **not** an RTC pacing
or CPU-throttled benchmark; its role is sustained ring/correctness coverage.

## Release and transport validation

The release WASM is 2,797,027 bytes and has no `th09_probe_*` or
`th09_title_open` exports. The evidence exporter verified every compiled source
and header against the current tree, identical diagnostic/release source
inventories, and every packaged Runtime file's SHA-256. The resource-free
local Runtime is `th09_web/build-eagler-multiplayer`; its version build is
`d1dafe25e34d8deb5811ac35`. No push, upload, deployment or online-version
claim is implied by successful local packaging.

The diagnostic RTC and Relay transport gates ran two players and a confirmed
spectator to exactly frame 600, all with hash `1357358377`, with packet loss,
delay and a deliberate outage. All three audio clocks progressed. The release
RTC and Relay gates also passed, but cannot use diagnostic forced frame limits:
RTC ended at player frames 608/608 and spectator 601; Relay at 609/608 and
spectator 601. Thus their final values are NOT same-frame world-hash evidence;
the release smoke relies on the production confirmed-check protocol instead.

The transport fixture deliberately adds an 800 ms outage window and still has
visible long gaps, including roughly 1.7 seconds in the release Relay run.
Those are not hidden or presented as smoothness acceptance. Its legacy
`presentationGapMs` measures callback completion, unlike the corrected
smoothness harness's submitted-RAF measurement. Audio-clock progress does not
prove absence of audible discontinuities. Public Internet/TURN, physical
scanout, handheld hardware and subjective dodging remain separate acceptance.

## Measurement contract and reproduction

Reports are in `th09_web/artifacts/multiplayer-tests`. The smoothness harness
uses two separate desktop Chromium processes and native Intel UHD / D3D11.
`CPU_RATE=2` throttles only P2. `DELAY_MS=77 JITTER_MS=10` is **per-direction
application-send delay**, not 77 ms RTT. Timers add overhead: the delivered
delay distribution is retained. Input and reliable repair lanes are impaired.
Queue and sample budgets are bounded.

`INPUT_MODE=tape` uses the real hosted keyboard sampler. A synchronous outgoing
input packet verifies the actual captured frame and installs the next frame's
keys. No runtime test input override, prediction change, invulnerability or
workload reduction is introduced. Raw input tapes and SHA-256 are retained.
The default `wall-clock` mode remains useful for input intent while simulation
falls behind, but is not assumed to be the same world across timing variants.

The original harness's end-of-callback gaps included differences in callback
work time. They must **not** be called missed display frames. The current
`presentationGapMs` measures RAF timestamps at new renderer submissions;
`callbackCompletionGapMs` retains completion gaps separately, and `rafGapMs`
shows callback opportunity gaps. None measures physical scanout. Audio checks
only establish a running audio clock, not absence of audible underruns.

```powershell
$env:TH09_EMSDK='D:/workspace/eagler/th08-eagler/tools/emsdk'
$env:TH09_PROFILE='1'
$env:TH09_OUTPUT='artifacts/sdl-your-new-label'
node th09_web/scripts/build-sdl.mjs
$env:WASI_SDK_BIN='D:/workspace/eagler/toolchains/wasi-sdk-34.0-x86_64-windows/bin'
node th09_web/tests/multiplayer/run-tests.mjs
$env:PC_BUILD='th09_web/artifacts/sdl-smooth-retry'
$env:RUN_LABEL='your-unique-label'
node th09_web/tests/multiplayer/world-browser.mjs
node th09_web/tests/multiplayer/peer-browser.mjs
$env:DELAY_MS='77'; $env:JITTER_MS='10'; $env:CPU_RATE='2'
$env:FRAMES='1800'; $env:PROFILE_CPU='0'; $env:INPUT_MODE='tape'
node th09_web/tests/multiplayer/smoothness-browser.mjs
node th09_web/tests/multiplayer/summarize-smoothness-comparison.mjs retry-tape retry-tape-a1 retry-tape-b1 retry-tape-b2 retry-tape-a2
# Revalidate retained reports, source identities and the local release package:
node th09_web/tests/multiplayer/export-smoothness-evidence.mjs
```

Use a distinct report label per command. Build serially, then compare frozen
binaries serially with no compilation or other heavy test running concurrently.
`TH09_PROFILE=1` is diagnostic only and deliberately rejected for release;
clear it before release builds. No physical-phone, public-Internet/TURN,
subjective dodging or acoustic acceptance is claimed.
