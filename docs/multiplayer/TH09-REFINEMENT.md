# TH09-first Adonis refinement

Current continuation: [actual-channel startup and prediction reserve](MEASURED-ADONIS-STARTUP.md).
The historical fixed-preset evidence below remains unchanged; it is not the
new automatic calibration policy.

Date: 2026-10-02. Worktree `D:/workspace/eagler/worktrees/adonis/th09`, branch
`experiment/adonis`. Starting commit `e55afe0`; common stays at `5669eff`.
The user narrowed work to TH09 before further title adaptation. Do not resume
TH06/07/08/10 integration from this task. No ordinary tree, push or deployment.

## Held analog prediction

The old TH09 adapter replaced every predicted analog input with None. That
avoided treating absolute field targets as repeated displacements, but it also
made an unchanged remote finger target disagree on every missing-input frame.

The shared core already supports held absolute targets. Hybrid mode now sets
`directTouchIsAbsolute=true` and preserves its last-known absolute target or
joystick vector. Mode 0 retains the previous prediction behavior for comparison;
pure mode 1 still requires exact contiguous input and owns no world checkpoints.
Button masks, direction horizon, rollback window, physical sampling, D and 60 Hz
simulation are unchanged. Actual movement/release/mode changes still trigger
full exact restoration, and Bomb/Menu edges remain unpredicted.

No shared-library change was needed. Comparing the old and candidate build
source inventories identifies exactly one compiled change:
`th09_web/cpp/multiplayer/RollbackSession.cpp`.

## Component and real-world evidence

`node th09_web/tests/multiplayer/run-tests.mjs` passes all existing suites plus
held-target/joystick checks and twelve new Hybrid D=0/2/9 impaired-network model
runs. Each model compares every confirmed tick with its exact-input reference,
including releases, target changes, one-shot actions and retirement. These are
component tests, not performance measurements.

The new `touch-peer-browser.mjs` uses the actual native gesture producer: drag,
hold, reverse, release, new gesture and cancel, with one sample per forward
frame. Two real C++ worlds run with a three-iteration input-delivery delay and
the SAME Hybrid mode / D=2. A third real world consumes the confirmed all-seat
tape without prediction. All 900 frames complete, with 490 applied touch samples.
It checks full final world audit, canonical hash, framebuffer and byte-identical
Replay against that exact reference. No gameplay or invincibility shortcut was
introduced in this harness.

The control/candidate/candidate/control sequence is retained under
`th09_web/artifacts/multiplayer-tests/adonis-held-abba-*-report.json`:

| Run | P1/P2 resimulated ticks | P1/P2 measured work ms, frames 150..899 | P1/P2 callback P95 ms |
| --- | --- | --- | --- |
| Control A | 627 / 149 | 426.995 / 327.990 | 0.950 / 0.720 |
| Candidate A | 153 / 149 | 472.260 / 455.425 | 1.255 / 1.210 |
| Candidate B | 153 / 149 | 438.075 / 415.125 | 1.080 / 0.975 |
| Control B | 627 / 149 | 462.670 / 351.745 | 1.090 / 0.770 |

Every run has the same confirmed tape hash
`f233bb49d00139e61cbb6defdb230cf77871ff467585484861cff9a389319899`,
the same Replay hash
`1c1a82cfe818b4b99b2427901d4ec012578e2644d5f5c3e4faa44535d310aaeb`
(7,223 bytes), and all three final world hashes `3509093080`.
This isolates the prediction change without increasing D or changing input.
Total resimulation is 776 -> 302 (61.1% less); only the peer predicting the remote
touch stream changes its count. **The absolute work/P95 measurements do not
establish a CPU or frame-rate improvement**: both candidate runs measured slower
than the corresponding controls, including the unchanged P2 workload. Preserve
the raw values, and repeat under controlled host load before claiming speedups.
The test is desktop Chromium/Intel hardware rendering, not phone, WAN, display
frame pacing, input-to-photon latency or long-session thermal acceptance.

Frozen diagnostic control:
`th09_web/artifacts/sdl-adonis`, SHA-256
`b6c5cfd2f11cdbd6be821bf28338b64879fa75daa976089c606d51f56e808905`.
Candidate:
`th09_web/artifacts/sdl-adonis-held`, SHA-256
`5b2dff86555a891d2d0e64cf9fd1263d678d391d61c8dbe7496706a456853aae`.
Do not overwrite the frozen builds.

## Delivery-gap investigation

`relay-timing-observer.mjs` is an optional local-test preloader. It records
bounded per-socket binary receive/send/send-completion counters, queued bytes,
timestamps and 100 ms relay-loop samples. It refuses non-loopback use, records
no payloads or credentials, and is not shipped in the relay or Runtime. Browser
and relay timelines carry comparable epoch timestamps. It observes rather than
changes simulation or network admission; instrumented timings are not production
CPU measurements.

The pure D=5 diagnostic run `adonis-held-relay-observed-02` passes exact state
and the original combined 14-second budget: 10,834.750 ms total, 339.080 ms setup,
10,495.670 ms gameplay. Three final hashes agree (`1357358377`), no snapshots or
resimulation. The observer produced 167 samples; its largest 100 ms timer interval
was 112.465 ms. Largest player presentation gap was 416.660 ms with the existing
25..61 ms input delay and short additional-800 ms injection.

The earlier unexplained 1.9 s/12.8 s pauses did NOT reproduce in this single run;
they are not declared fixed. New instrumentation can distinguish relay-loop
starvation, upstream arrival gaps and downstream/browser receive gaps when it
recurs. The first preloader attempt failed before gameplay because Windows
`--import` needed a file URL; it was corrected and the failure log is retained.

## Longer regression, Release and local packaging

`adonis-held-lifecycle-12000` passes with the candidate diagnostic WASM: 12,000
confirmed frames per player, injected delay/drop/duplicate/spike handling, pause
on both peers and the real results path. All three worlds have canonical hash
`3681705413`, equal full audit and framebuffer, and identical 3,384-byte Replay.
Observed peak bullet count was 497. This uses the pre-existing **keyboard**
endurance tape, not a 12,000-frame touch or WAN test; touch evidence is the
separate 900-frame matched-input test above.

Actual Release build also passes, with no `th09_probe_*` exports:
`th09_web/artifacts/sdl-adonis-held-release`, 2,799,676-byte WASM,
SHA-256 `d1ea10968ee2a942a820bea4a5ff9cd336ad9f66fe6c5c93511be8ea6ee2961c`.
`adonis-held-release-rtc` passes the real RTC/confirmed-spectator smoke and
original total-duration gate (12,135.500 ms). It ends at player frames 605/606
and spectator frame 601, so it does **not** claim a comparable final-state hash
even though the observed hashes happen to match.

The resource-free local multiplayer Runtime was rebuilt from that exact Release
directory. `th09_web/build-eagler-multiplayer/version.json` is
`d1ea10968ee2a942a820bea4`; the multiplayer variant remains distinct from ordinary
TH09. This local packaging is not a public push/deploy or main-site replacement.

## Reproduction

```powershell
$env:WASI_SDK_BIN='D:/workspace/eagler/toolchains/wasi-sdk-34.0-x86_64-windows/bin'
node th09_web/tests/multiplayer/run-tests.mjs
$env:TH09_EMSDK='D:/workspace/eagler/th08-eagler/tools/emsdk'
$env:EM_FROZEN_CACHE='1'; $env:TH09_BUILD_JOBS='1'
$env:TH09_OUTPUT='artifacts/NEW-DIAGNOSTIC-DIRECTORY'
node th09_web/scripts/build-sdl.mjs
$env:TH09_FIXTURE_ROOT='D:/workspace/eagler/worktrees/th09-multiplayer'
$env:NATIVE_GPU='1'; $env:ADONIS_MODE='2'; $env:INPUT_DELAY_FRAMES='2'
$env:PC_BUILD='th09_web/artifacts/sdl-adonis-held'
$env:RUN_LABEL='NEW-UNIQUE-TOUCH-RUN'
node th09_web/tests/multiplayer/touch-peer-browser.mjs
# Run sequentially with the frozen control to compare; never alongside builds.
$env:EAGLER_RELAY_SOURCE='D:/workspace/eagler/worktrees/adonis/eagler-touhou/server/netplay-relay.mjs'
$env:TH09_RELAY_TIMING='1'; $env:ADONIS_MODE='1'; $env:INPUT_DELAY_FRAMES='5'
$env:RUN_LABEL='NEW-UNIQUE-RELAY-RUN'
node th09_web/tests/multiplayer/transport-browser.mjs --relay
# For a new Release build, use a new TH09_OUTPUT directory and --release.
# Local multiplayer packaging from the validated candidate:
$env:TH09_OUTPUT='artifacts/sdl-adonis-held-release'
$env:TH09_RUNTIME_ASSETS='D:/workspace/eagler/worktrees/th09-multiplayer/th09_web/assets/sdl-native'
node th09_web/scripts/build-eagler.mjs --multiplayer
```

Build concurrency is now explicitly configurable with `TH09_BUILD_JOBS=1..4`;
default remains four. Local SDK/cache only; no download or broad fetch.

## Frontend ownership and remaining gates

The matching experimental Launcher now puts a default-on rollback switch left
of the input-delay capsule for TH09. Manual D is independent. See its own
`docs/ADONIS-EXPERIMENT.md` for UI and role/room evidence.

Continue TH09: repeat stable-host matched-input timing, retain negative tail
events, verify remote phones and long real transport sessions. Avoid broad title
adaptation and do not substitute a total-duration PASS for smooth presentation.
