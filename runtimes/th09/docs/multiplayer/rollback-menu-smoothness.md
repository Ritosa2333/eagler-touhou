# TH09 long-run menu smoothness investigation

Base: `ef772e0`. Resumed on 2026-09-28 at the user's request to continue where
there is a demonstrated improvement. No resource routing, online DATA/OGG,
prediction horizon, local input delay or gameplay math changes are in scope.

## Reproduced problem

The previous 1,800-frame RTC tests did not reach the result menu. A new formal
release run used 7,200 frames, the wire-verified keyboard tape, two separate
Chromium processes with Intel UHD/ANGLE D3D11, 39 +/- 5 ms application-send delay
in each direction, and 2x CPU slowdown on P2. Audio output was enabled.

`endurance-release-baseline-report.json` completed with matching checks but
whole-run logical throughput was about 55.9 Hz. P2 submitted-RAF p99 was about
83.33 ms. Match-complete occupied frame 4285 through 4430 and roughly eleven
seconds instead of about 2.4 seconds. Its callbacks mostly waited for input;
CPU cost there was small. The flaw was the unconditional exact-input gate on
every paused/game-over/match-complete frame, not expensive collision logic.

Timing summaries use frames 180 through 7199, excluding the declared 180-frame
startup warm-up but retaining results, retries and resource transitions within
that interval. Raw reports retain startup samples too. A "whole" summary in
the helper means the complete measured interval, not all browser startup time.

## Change and lifetime invariant

`InGameMenus` now exposes conservative read-only predicates for steps that can
replace the owning world. Normal animation, navigation and pause/resume remain
in the existing snapshot inventory. The rollback driver only waits for exact,
reconciled inputs at resource-retiring steps, and still always gates title
resource transitions. The native menu state machine itself is unchanged.

There is a second fail-closed check before `pending_action` is executed: an
actual speculative record cannot destroy the world even if a future menu rule
forgets to update its preflight. This reports an error before resource teardown.

The diagnostic action-coverage gate calls the actual menu implementation for
11,520 combinations of menu states, boundary timers, flags, continue counts
and buttons. It observed 5,232 retiring actions, all behind a barrier, and 6,208
steps without a barrier; the surrounding full checkpoint was restored exactly.
This is action coverage, not by itself a network or full-game acceptance.

## Test assertion repaired, failure retained

`endurance-menu-candidate-1-failure.json` is retained. Both peers and the old
release agreed at all sixty confirmed checkpoints through frame 7200. The old
test nevertheless compared the final *speculative* state just because both
peers said frame 7205. P1 had only confirmed/published frame 7201; this is not a
valid same-frame exact-state comparison. No production hash was removed.

The corrected test requires every production confirmation frame, ordered at
120-frame intervals, and compares those traces across peers and builds. It
also keeps the final live-state comparison whenever both endpoints have fully
published that same frame. Diagnostic runs must still reach a fully confirmed
common endpoint. Release runtimes do not acquire a test frame-limit export.

Fresh paired measurements use the corrected, identical test harness. The failed
initial candidate is not silently rewritten as a passing performance result.

## Reproduction

From this worktree, use explicit immutable build directories:

```powershell
$env:TH09_EMSDK='D:/workspace/eagler/th08-eagler/tools/emsdk'
$env:TH09_PROFILE='1'
$env:TH09_OUTPUT='artifacts/sdl-menu-frontier'
node th09_web/scripts/build-sdl.mjs
$env:PC_BUILD='th09_web/artifacts/sdl-menu-frontier'
node th09_web/tests/multiplayer/menu-barrier-browser.mjs

$env:TH09_PROFILE='0'
$env:TH09_OUTPUT='artifacts/sdl-menu-release'
node th09_web/scripts/build-sdl.mjs --release
$env:PC_BUILD='th09_web/artifacts/sdl-menu-release'
$env:NATIVE_GPU='1'
$env:DELAY_MS='39'; $env:JITTER_MS='5'; $env:CPU_RATE='2'
$env:FRAMES='7200'; $env:INPUT_MODE='tape'; $env:AUDIO_OUTPUT='1'
$env:RUN_LABEL='your-unique-run'
node th09_web/tests/multiplayer/smoothness-browser.mjs --release
```

`summarize-endurance.mjs` retains the whole run and contiguous scene segments.
It rejects missing/reordered confirmation samples and different wire tapes or
cross-build confirmed state. The new audio probe samples 64 PCM values per SDL
output callback and records callback/playback timing without replacing audio.
It cannot establish acoustic quality, device underruns, or physical scanout.

## Current verification state

The action-coverage gate and release build passed. The independent-world
pause/resume test passed 1,800 frames with 1,800 corrections and 14,372 replayed
ticks on each peer; its exact-input reference, framebuffer and 1,835-byte Replay
agreed. Both peers visited pause for 192 calls, with 384 peak active bullets.

The 7,200-frame independent-world results/retry test also passed. It reached
both match and match-complete, peaked at 431 active bullets, and matched the
exact-input reference, framebuffer and 2,704-byte Replay. This harness injects
eight *iteration* transport frames; it is not a real-RTC wall-clock measurement.

The four formal-release A/B/B/A runs also passed, using the same unchanged
runner and audio probe. Their labels are `menu-long-a1`, `menu-long-b1`,
`menu-long-b2`, `menu-long-a2`. All sixty confirmed checkpoints and both wire
input tapes agree across both peers and all four runs, including the retry.

| Metric, P2 throttled 2x | Previous release, two runs | Menu-boundary candidate, two runs |
| --- | --- | --- |
| Measured interval logic Hz, aggregate frames / elapsed | 55.950 | 59.958 |
| Submitted-RAF p99, each run | 83.325 / 83.325 ms | 16.670 / 16.670 ms |
| Submitted-RAF intervals >25 ms, sum | 276 | 31 |
| Submitted-RAF intervals >50 ms, sum | 177 | 1 |
| Result-menu duration, 145 net frames | 10,829.915 / 10,830.665 ms | 2,449.695 / 2,465.255 ms |
| Result-menu waiting callbacks | 505 / 504 | 2 / 3 |

Battle-only intervals remained about 59.97-60.00 Hz across the four runs. This
is a result/menu scheduling improvement, not a new claim that battle Update
became faster. The candidate still had occasional long submissions: P1 maxima
99.995 / 83.325 ms and P2 maxima 49.995 / 66.665 ms. Do not claim zero stalls.
Runs were serial, without concurrent compilation or heavy benchmarks; focused
correctness checks took place between the A/B and B/A pairs.

Each candidate endpoint delivered 1,408 observed SDL audio blocks per run.
After the first second, every sampled block had nonzero PCM. Callback/queued
playback timing still has variation, including up to 24 ms extra scheduled gap
in one candidate endpoint. This is output observation, not evidence of improved
acoustic quality, perfect continuity or a device-level underrun guarantee.
The recorded WASM memory size was 193,331,200 bytes for every endpoint/build;
this is heap size, not browser-wide or peak total memory.

The standalone UI lifecycle gate passed on the new diagnostic build:
pause/resume, same-confirmed-frame synchronization, retry, returning to the
local score/Replay menus, saving Replay on only one peer, reconnecting and
closing the room. Source-level action coverage and live lifecycle evidence
therefore remain separate, and both are retained.

Final sparse/dense/round-reset, four component suites and release
RTC/relay/spectator regression gates passed. The latter are transport/lifecycle
smokes with injected outages, not smoothness acceptance under total outages.
Production release contains no `th09_probe_*` exports. Both builds' source
attestations match the current source and each other.

The verified local package is `th09_web/build-eagler-multiplayer`, build ID
`63b2d839e75a8dfc076b8cd5`, release WASM SHA-256
`63b2d839e75a8dfc076b8cd5d3c27cab56eb9bd4ce3f1792e1717b5b703fc950`.
Diagnostic WASM is in `th09_web/artifacts/sdl-menu-frontier`, SHA-256
`a3d9f7bd769ce7233ccf96b852e7abb3ae11f47f022b2cf83be91c0f5d46d987`.

`node th09_web/tests/multiplayer/export-menu-evidence.mjs` verifies and exports
the portable record `docs/multiplayer/rollback-menu-evidence.json`: A/B/B/A
identities, complete confirmed traces, current source attestation, separate
action/peer/restore/UI/transport checks, audio observations and the retained
invalid first-run assertion. Do not run an older stage-specific exporter as a
gate for this source; its historical build hashes are intentionally different.

No push or deployment was performed. Source DATA/OGG/resource routing and its
404 behavior were not edited. Remaining acceptance is physical remote-PC play,
subjective response and audio quality, rather than a reason to add input delay
or disable rollback. Isolated long frames remain in the measured reports.
