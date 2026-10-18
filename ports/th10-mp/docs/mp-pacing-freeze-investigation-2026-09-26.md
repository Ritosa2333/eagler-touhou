# TH10MP pacing and fatal error investigation

Scope: local candidate only; no commit, push, upload or deployment. Restart was
explicitly removed from this investigation by the user. Earlier dirty gameplay
and retirement-fence changes in this worktree remain intact.

## Findings and changes

- TH10's real browser loop never consumed the frame-advantage information already
  carried by SessionChannel. TH06/TH07 use the shared latency-compensated policy
  to gradually align simulation clocks. A faster-loading endpoint can otherwise
  retain a lead and repeatedly resimulate a longer predicted interval.
- Added `SessionPacing` to canonical `eagler-common`, using its existing per-peer
  trimmed windows and bounded 0.98–1.02 interval policy. The TH10 vendored copy
  integrates it in SessionChannel after packet validation and sequence filtering.
  Fresh session/clear resets the estimate. Retransmissions do not add samples.
- TH10's real rAF host consumes that advice only for active live players. Each
  logical tick remains the original fixed simulation and authored Draw; audio,
  input contents, rollback limits, offline Replay and spectators retain their
  existing ownership. Presentation interpolation uses the calibrated accumulator.
- Added exact failure-site text for the multiplayer `-4` branches and exposed it
  to the shell. `-5` continues to report the transport/session failure. Fatal
  checks remain active. The prior FPS display correction is also still present.
- Stop polling an already failed application. The browser keeps requesting rAF
  callbacks after the shell suspends gameplay; previously a subsequent network
  timeout could replace the original `-4` with `-5` on that same endpoint.

`-4` covers rollback/state/audio/Replay failures, while `-5` is the network pump's
failure path. A peer that has stopped on `-4` can subsequently cause a confirmed
input timeout at the other endpoint; that is a possible relationship, not a
reproduction or a conclusion about the user's incident.

## Browser observations

Old build: `fea2b241873e38741a0483fa9b94084625719d9be11e61afd578390b9832b0c5`.
Candidate: `3e9a190080a2cb94de84a6b8ae34ac108b9c24a5cf09a9614d97c6595ad05b9d`.
Final build, additionally preserving the first fatal error:
`17b5bb65408a94943525287c9a309c9bb8aa29b71966fc3627c79a0fcb686232`.

Real BrowserPeerTransport/WebRTC, two isolated browser contexts, local relay
signaling, injected one-way 38.5 ms ± 5 ms send delay, software GPU. These are
local correctness/profiling observations, not Windows-to-Android device results.

- 150 s old-build keyboard-input run: 4,490/4,491 forward frames, no fatal error;
  5,207/1,088 resimulated frames. This established unequal rollback work.
- Sequential 90 s A/B runs, continuous direct-touch delta input, 500 ms stagger:

| Metric | Old | Candidate |
| --- | ---: | ---: |
| Endpoint 0 forward frames | 2,043 | 2,159 |
| Endpoint 0 resimulated frames | 6,326 | 5,527 |
| Endpoint 0 callback cost p95 | 9.62 ms | 8.80 ms |
| Endpoint 1 resimulated frames | 121 | 351 |
| Fatal errors | none | none |

This single A/B run suggests reduced imbalance/cost, not proof that stuttering
has been eliminated. The software-rendered browser environment has its own
display scheduling limits. Neither A/B run reproduced the reported random
`-4`/`-5`; no root-cause claim for those errors is warranted yet.

Raw observations and temporary drivers are in `artifacts/multiplayer-tests/`:
`stress-before.json`, `stress-analog-before.json`, `stress-analog-after.json`,
`stress-live.py`. The old binary and matching manifest were preserved under
`pacing-baseline/` for repeat comparisons.

## Validation

- Shared SessionPacing test: symmetric latency remains unity; a real lead slows
  the leading clock; duplicates/reordering are ignored; three-peer advice uses
  the greatest lead; reset/outlier behavior; clock convergence.
- TH10 display cadence test: convergence at 60/120/144/165 Hz, bounded interpolation
  and at most one logical tick per display callback.
- Existing shared SessionChannel tests: PASS, including lossy streams, validation,
  timeout, reliable repairs and the pre-existing retirement fence tests.
- Real WebRTC 3-player run dropping every third fast input packet: PASS at
  confirmed frames 59/119/179, including canonical state, ANM/RNG and audio.
  Report: `network-37f9db4b-6303-4f22-b6e0-ba491671619c/suite.json`.
- Full `portable/check-multiplayer-rules.mjs`: PASS, including session/core,
  audio, Replay, cooperation, input ownership and the new pacing cases.
- Relay 2-player canonical checks passed on the first candidate. Its close-peer
  assertion initially failed because the test allowed 10 s while the unchanged
  confirmed-input watchdog allows 15 s. The test now allows 20 s for relay and
  retains 10 s for RTC. This changes the test deadline, not runtime liveness.
- Final-build relay 2-player retest: PASS, including the actual disconnect/
  confirmed-input timeout. Report:
  `network-21403797-881d-4dd5-94bc-5b62b227aabe/suite.json`.
- A planned 7,200-frame/11-frame-delay stopped-loop run was terminated without
  a result because software GPU work dominated elapsed time. It is **not** a
  pass and supplies no evidence that the user's intermittent errors are fixed.
  A smaller 900-frame boundary run uses the same delayed-input path instead.
- Final-build boundary run: 900 frames with 11-frame delayed, continuous direct
  touch input completed without an application error. This is a rollback stress
  check, not a cross-peer canonical-state oracle. Report:
  `stress-rollback-boundary.json`. Cross-peer equality is covered separately by
  the RTC/relay confirmed-frame tests above.

Conclusion: pacing has a tested candidate improvement; fatal errors now retain
their first cause and include a failure-site message. The original intermittent
freeze remains unreproduced and must not be described as resolved. Final build
is local only, without fixture exports. No Restart investigation was performed.
