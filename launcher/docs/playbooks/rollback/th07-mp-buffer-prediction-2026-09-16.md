# TH07 MP input buffering and prediction investigation

Date: 2026-09-16. Continuation of `th07-mp-performance-2026-09-16.md`.

## Scope

The user requested roughly 50 ms buffering and investigation of more predictable
mobile input. Unlike the previous storage-only work, this phase intentionally
adds local input latency. It does not change simulation frequency, real touch
delta precision, or collision rules. The final implementation extends analog
input semantics while retaining 12-byte input records: live gameplay ABI is 5,
and new Replay streams carry an explicit capability flag (details below).

No physical-phone, public-network, or full dense-stage acceptance is implied.
No commit or deployment was requested for this investigation.

## Implemented policy and ownership

Production LAN defaults to **3 frames of local input delay** (50 ms at 60 Hz).
This delays action buttons as well as movement. Deathbomb/collision rules are
unchanged, but the player's response reaches them later; emergency-input feel
therefore still needs human acceptance. No display trick hides this latency.
Input captured at logical frame F is sent immediately for frame F+D. Simulation
still consumes input for F. The initial neutral prefix is transmitted through
the ordinary reliable/redundant input history; remote prefixes are never assumed.
Peers on the same new ABI may use different local delays. The buffer itself
does not require identical delay values; the new touch semantics DO require
ABI 5 and older live peers are rejected by HELLO before game input is sent.

`RollbackCore::LocalFrameForCapture` owns the mapping and rejects the reserved
INVALID_FRAME sentinel. `HasLocalCapture` checks the future slot, not the due
slot: otherwise the neutral prefix would suppress sampling. Retries retransmit
the scheduled future slot without consuming device input again. Probe tail
keepalives still cover the final simulated frame. Replay and spectators consume
the actual delayed input assigned to each logical simulation frame.

The optional `stable` LEGACY snapshot predictor repeats a delta for at most two
missing frames, and the additional second frame requires two identical,
consecutive, finite actual deltas with the same unlimited-mode flag. From the
third missing frame onward displacement is zero. Bomb remains unpredicted.
Actual input comparison remains exact: no tolerance-based rollback suppression.
This is **not enabled by default**. A sudden stop can overshoot by one more old
delta than the legacy rule, and variable/batched trajectories get no benefit.
It is an experiment for old mode-2 input. The new physical stream uses the
separate remaining-movement prediction described below, not this extrapolator.

Runtime configure options (validated and copied in `resources/shell.html`):

| Option | Default | Allowed values / purpose |
|---|---|---|
| `netplayInputDelayFrames` | LAN: 3; otherwise: 0 | Integer 0..6, fixed for this gameplay session |
| `netplayTouchPrediction` | `legacy` | `legacy`, `stable` |
| `netplayMaxPredictionFrames` | 12 | Integer 1..12; experiment in forward prediction horizon |
| `netplayPerfTelemetry` | false | Read-only cost and queue-residence measurements |
| `netplayTouchWorkload` | `default` | Test harness only: steady, variable, stop, reverse, burst |
| `netplayTestIncrementalTouch` | false | Test-only switch for synthetic mode-3/4 traces; physical touch always uses the new producer |

There is no new Launcher setting UI in this phase. These are Runtime configure
controls used by the focused test host; normal launches get the defaults.

Also removed pointless spectator encoding/transmission when there are no
spectators, and reused the small spectator diagnostic object rather than
allocating a new object twice per driver tick. Start-only spectator ownership
and active spectator streaming remain unchanged.

## Why not immediately change delay dynamically or rename delta to velocity?

GGPO and GGRS combine input delay with prediction. GGRS documents that increasing
delay fills gaps by replicating the last input, while decreasing delay drops
input. Those generic operations are not safe to copy blindly for one-shot touch
displacement or action edges. This implementation fixes delay per gameplay
session. Dynamic adjustment needs explicit, tested no-duplicate/no-loss rules.

Changing units from displacement to velocity does not make continuously varying
floating-point values exactly predictable. Renaming the representation alone
would not eliminate the mismatch-driven correction path. Instead the final
implementation changes ownership: each real displacement is transferred once
and any unapplied remainder belongs to the synchronized simulation.

## Critical physical-touch integration repair

The first buffering implementation exposed an ownership bug that synthetic
FrameInput tests cannot detect. `Touch::GetPlayerDelta` PEEKS accumulated device
movement; the former local Player forward pass consumed it later. Delaying the
Player by three frames would therefore capture the same delta repeatedly, and
an old delayed frame could clear movement from a newer device event. This was
an integration hazard introduced by adding delay, NOT evidence that this exact
bug caused the previously deployed zero-delay stutter.

The final implementation uses:

- `Touch::TakePlayerDelta`: atomically transfers each producer delta once;
- `AnalogMode::DirectTouchDelta = 3` for fresh displacement and
  `DirectTouchBegin = 4` for a new gesture which resets stale remainder;
- `DirectTouchState` per player, applied once with fixed-tick overrides;
- the original Player focus ratio, speed limit, bomb multiplier and boundary
  clamp, writing remaining movement to this state instead of raw SDL touch;
- rollback capture AND canonical hashing for remainder x/y/active;
- reset on release/mode switch/new gesture and Player-chain construction;
- missing input predicts **zero new displacement**, retaining any previously
  received movement which the speed cap has not yet consumed. No device event
  is repeated and no gesture-begin edge is predicted.

All packets still have the original width. Modes 3/4 are accepted only by new
runtimes, enforced by gameplay ABI 5. Legacy mode 2 retains snapshot semantics.
Replay uses the existing reserved session capability bit 8 for incremental
touch, keeping the container version and ordinary Replay path unchanged. Old
readers reject the unknown capability instead of silently turning modes 3/4
into neutral input. The new reader validates mode/flag/finite values and still
reads legacy mode-2 files without the new bit.

`tests/netplay-touch-pipeline-test.cpp` exercises delayed once-only capture,
speed-limited remainder, clearing/restarting gestures, per-player separation,
codec and prediction, and journal restore/resimulation. The browser test's
`--physical-touch` mode drives actual CDP touch events through the real
Touch/Controller/Player pipeline, not through a fabricated FrameInput hook.

Physical-path evidence (`.codex-tmp/buffer-touch-physical.json`): 2P, 600 frames,
delay 3, relay 50 +/- 10 ms one way. Starting X positions were 160 and 224.
Actual captured displacements were +12.018798828125 and -14.02191162109375;
both peers reached exactly 172.018798828125 and 209.97808837890625 respectively.
No repeated displacement or loss was observed; confirmed hashes at 300/600
matched. This is browser event-path verification, NOT a physical-phone claim.

Additional fresh-stream live tests passed at 600 frames with confirmed hashes:
2P burst (`buffer-touch-delta-2p.json`) and 3P variable
(`buffer-touch-delta-3p.json`). These ran about 60 logical frames/s; presentation
timing must still be evaluated separately from logical throughput.

## Matched physical-intent model

The extended core study compares the SAME 24-unit drag every ten ticks with a
four-unit/tick speed cap. Legacy trace 5 sends the shrinking pending snapshot;
fresh trace 6 sends 24 once and zero new displacement on subsequent ticks.
Both are checked against their exact offline reference, including remainder.

| Representation / delay | Rollback operations | Replayed logical frames |
|---|---:|---:|
| Legacy pending snapshot / 0 | 722 | 3,366 |
| Legacy pending snapshot / 3 | 361 | 1,320 |
| Fresh delta + remainder / 0 | 246 | 990 |
| Fresh delta + remainder / 3 | 124 | 373 |
| Fresh delta + remainder / 4 | 124 | 249 |

This demonstrates about 72% less resimulation from the representation change
at the same 3-frame delay, and about 89% less for the combined change versus
legacy zero delay. It is a deterministic core model, not a mobile FPS promise;
arbitrary continuously varying real input remains inherently less predictable.
The legacy `stable` extrapolator gave no added benefit for these matched traces.

50 ms cannot guarantee smoothness for every connection labelled 100 ms: RTT is
not a guaranteed 50/50 split, and transport/polling/scheduling jitter adds to the
delivery deadline. Buffer depth is evaluated against both correction work and
visible progress gaps, not just an average FPS label.

Primary research:

- https://github.com/pond3r/ggpo/blob/master/doc/DeveloperGuide.md
- https://github.com/pond3r/ggpo/blob/master/src/lib/ggpo/input_queue.cpp
- https://docs.rs/ggrs/latest/ggrs/struct.SessionBuilder.html
- https://docs.rs/ggrs/latest/ggrs/struct.P2PSession.html
- https://www.snapnet.dev/blog/netcode-architectures-part-2-rollback/
- https://emscripten.org/docs/porting/guidelines/api_limitations.html
- https://www.w3.org/TR/pointerevents/
- https://developer.chrome.com/blog/devtools-grounded-real-world

## Tests and interpretation

`tests/netplay-delay-prediction-test.cpp` uses the real core, codec, ACK/tail
logic and prediction. It exercises 1,200 logical frames, ring wrap, loss,
reordering, capture retries, a frame-zero barrier, mixed local delays 0/3/6,
2P/3P, shortened prediction windows, and correction against an independent
offline world. Each endpoint must capture exactly once per new logical tick,
and all final worlds must equal the exact delayed-input reference.

The native core study (not a phone benchmark) used 3..5 ticks one-way delivery,
i.e. about 50..83 ms plus a poll boundary, not a claimed exact 100 ms RTT.
Across two endpoints, variable-delta resimulation fell from 3,996 frames at
delay 0 to 1,595 at delay 3 and 1,195 at delay 4. Stable prediction did not help
that variable trace. For the steady trace at delay 3, resimulation fell from
1,211 to 848 with the predictor, while the number of rollback operations was
unchanged. Those are different metrics and must not be conflated.

The first browser sweep was INVALID: the Shell whitelist dropped the new
options. The test correctly rejected the requested delay. Those files are
`.codex-tmp/buffer-variable-d*.json`; do not use them as A/B evidence.

After fixing the whitelist, `.codex-tmp/buffer-validated-variable-d*.json` used
one build, two browsers, 900 frames, scripted variable direct-touch input,
50 +/- 10 ms one-way relay injection, and no CPU throttling. All checked the
actual configured delay, exact canonical checkpoints, and active telemetry:

| Delay | Total resimulated frames (both peers) | Logical frames/s (P1 / P2) | Largest visible-advance gap |
|---|---:|---:|---:|
| 0 | 7,169 | 59.62 / 60.08 | 50 ms |
| 3 | 4,049 | 38.17 / 38.47 | 7,433 ms |
| 4 | 3,026 | 59.93 / 60.08 | 50 ms |

These are single exploratory trials, not established performance guarantees.
The delay-3 trial reduced replayed work but had a large stall; its largest
measured reconciliation was only 20 ms. It must not be reported as a smoothness
PASS simply because canonical checkpoints matched. Further instrumentation
records incoming-event gaps, JS queue residence, send progress and stalled
simulation snapshots to distinguish these costs.

Telemetry queue residence starts at the JS handler, NOT the network interface.
It excludes any time spent waiting before that handler executes. Core timing is
read-only wall time; reconcile time includes resimulation time, so do not add
them together. Browser RAF observations are not necessarily actual game draws.
CPU throttling is a stress experiment, not a named Snapdragon emulation.

## Follow-up findings

The 3-frame variable-touch repeat completed at 59.85 / 60.08 logical frames/s
and replayed 3,872 frames across both peers, versus 7,169 in the zero-delay
trial (about 46% less resimulation). File:
`.codex-tmp/buffer-variable-d3-repeat.json`. This does not erase the earlier
7.4-second stall.

All ten native/WASM SAFE_HEAP behavior cases passed, including the new delayed
capture/mixed-delay/loss/reorder tests. Report:
`.codex-tmp/buffer-core-validation.json`.

With only P2 CPU-throttled 4x, 900-frame variable-touch runs gave:

| Delay / prediction ceiling | P1 logical frames/s | P2 logical frames/s | Total resimulation |
|---|---:|---:|---:|
| 0 / 12 | 39.78 | 40.23 | 7,150 |
| 3 / 12 | 51.33 | 51.31 | 5,806 |
| 3 / 6 | 47.43 | 25.46 | 3,317 |

Files: `.codex-tmp/buffer-cpu4-d{0,3}-w12-v2.json` and
`.codex-tmp/buffer-cpu4-d3-w6-v2.json`. The shorter-ceiling run contained an
approximately 14-second transport gap: client send attempts continued, JS
queues were empty, and relay 1-second timers continued normally while its
received/forwarded counters stayed at 269 for about 14 seconds, then jumped
to 756. That observed stall is not an atomic rollback taking 14 seconds.
It narrows the problem to delivery before relay receipt; it does not identify
a specific browser/OS/proxy cause. Prediction ceiling remains 12 by default;
we cannot infer that shortening it improves the overall experience.

The optional `--relay-diagnostics` imports only a test-process observer. It
does not alter the shared/public relay implementation. An initial observer
launch failed because Node on Windows requires a file URI for `--import`;
the fixed runner uses `Path.as_uri()`. The non-v2 CPU-report files are startup
failures, not valid performance measurements.

The browser test now also records actual Present-call intervals and separates
outer driver work from draw/submission/swap time. Arrays are bounded at 4,096
samples, disabled outside opt-in telemetry, and do not read back the GPU.
Presented-call timing is still not proof of physical display scanout.

## Reproduce

From the `th07-eagler` repository root, build the existing Web target after code
changes, then run sequentially (do not overlap CPU performance measurements):

```powershell
python tests/run-netplay-performance-tests.py --output .codex-tmp/buffer-core-validation.json
python tests/netplay-performance-browser.py --frames 900 --delay-ms 50 --jitter-ms 10 --touch-workload variable --input-delay 0 --output .codex-tmp/buffer-variable-d0-new.json
python tests/netplay-performance-browser.py --frames 900 --delay-ms 50 --jitter-ms 10 --touch-workload variable --input-delay 3 --output .codex-tmp/buffer-variable-d3-new.json
python tests/netplay-performance-browser.py --frames 900 --delay-ms 50 --jitter-ms 10 --touch-workload steady --input-delay 3 --touch-prediction stable
python tests/netplay-performance-browser.py --frames 900 --delay-ms 50 --jitter-ms 10 --touch-workload variable --input-delay 3 --cpu-rate 4 --prediction-window 6
python tests/netplay-performance-browser.py --frames 600 --delay-ms 50 --jitter-ms 10 --physical-touch --input-delay 3
python tests/netplay-performance-browser.py --players 3 --frames 600 --delay-ms 50 --jitter-ms 10 --touch-workload burst --touch-stream delta --input-delay 3
```

`--require-rollback` is available for correction-specific tests. It is not the
default for a buffering experiment: correctly eliminating all rollbacks should
not itself fail a performance run. The independent core suite forces real
corrections, and browser completion/confirmed-state checks remain mandatory.

## Compatibility and remaining acceptance

ABI-5 Replay lifecycle tests passed with new modes: 2P saved and replayed exact
input frames 301/304; 3P replayed 78/80/82, after actual rollback in the recording
phase. This verifies input transport/recording/playback, not every gameplay
owner over a full run. A keyboard-stream spectator run passed at confirmed
checkpoint 300; a separate incremental spectator regression is required below.

Closeout: the incremental `burst` spectator regression also passed (frames
302/305/301; confirmed player frames 300/303; all hash300
`9442ee3f20826a84`). The ReplayExtension in-browser round-trip self-test passed
both old mode-2 files and new modes 3/4, including rejection when the new
capability flag is removed. All 12 native/WASM SAFE_HEAP cases passed again
in `.codex-tmp/buffer-final-behavior-tests.json`, as did the source-only rebase
audit. The ordinary `build-web-eagler-thprac` target (NETPLAY=OFF) compiled and
linked successfully; this is build coverage, not an ordinary gameplay run.

Final production Web rebuild and real browser touch-event rerun also passed:
`.codex-tmp/buffer-closeout-physical.json`. At 3-frame delay and 50 +/- 10 ms
one-way relay injection, each endpoint captured and applied the exact once-only
displacements again. Logical throughput was 59.88 / 60.13 frames/s, resimulation
15 / 14 frames. Actual Present-call intervals averaged 21.00 / 20.89 ms, with
maxima 44.5 / 45.6 ms and no interval over 50 ms in that fixture. Thus the run
is NOT claimed to be stable physical-display 60 FPS despite 60 Hz logical work.

Final production artifact: `build-web-th07-netplay/th07.wasm`, SHA-256
`87fc2c2826f0df919646287651c3d294ca642b7cc6b3f955c8cbfaa63eecaf62`.
Runtime diagnostic identity: `th07mp-20260916-buffer-touch-abi5`.
`git diff --check` passed (line-ending conversion notices are not code errors).

Actual RTC event-path closeout passed too:
`.codex-tmp/buffer-closeout-physical-rtc.json`, both routes asserted `rtc`, same
exact physical-stimulus displacement and confirmed checkpoints. Over 600 frames,
P1 replayed one frame and P2 none; logical rate was 59.88 / 60.13. This was local
RTC with no artificial transport delay, NOT a public 100 ms RTT measurement.

The last 12-case native/WASM SAFE_HEAP suite passed in
`.codex-tmp/buffer-touch-validation.json`; the final updated 7-trace core suite
is also run as part of closeout. Retain any earlier timeout reports. Tests that
failed to start due to a dropped option or an import URL are explicitly invalid
performance evidence, not hidden successes.

Not completed: physical mobile/thermal/OGG/MIDI acceptance, full dense-stage
and pause/restart touch-lifecycle coverage, adaptive per-session/within-session
delay negotiation, deterministic root cause of sporadic multi-second transport
gaps, or proof that this configuration is globally optimal. Default input
buffer is fixed 3 frames; optional narrower windows/legacy extrapolation are
not enabled simply because one metric looks better. No commit/deploy was made.
