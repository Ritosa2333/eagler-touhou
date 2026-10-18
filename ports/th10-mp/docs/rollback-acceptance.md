# TH10 rollback acceptance record

Status: **in progress; not a multiplayer capability declaration**.

Worktree: `experiment/th10-multiplayer`, checkpoint `bebb6e3` plus uncommitted
work. A tested substantial item, not a passing unit or build alone, is the next
commit boundary. No canonical integration, publication or deployment occurred.

## Evidence correction

The original 2P, 3P and Bomb gates excluded ANM/visual RNG categories. The old
2P report passed despite visual RNG seed/calls `1234/0` vs `1425/48` and ANM
occupied/cursor/last-id `57/180/180` vs `58/183/183` after correction.

The strict check reproduced this failure and retained it in
`artifacts/multiplayer-tests/browser-rollback-strict-before.json`. Restoring
Startup/Common state, their loading/introduction ANM handles, and the MP HUD
tick marker fixed this reproduced difference.

`browser-rollback-strict-common.json` passed frame-zero correction, a five-frame
late reversal at frame 240 and comparison through frame 359, including **all
currently declared canonical categories 1..27**, ANM and both RNGs. This is not
a claim that the schema inventories every future owner, nor a pointer-
independent cross-device hash.

## Checks and scope

The real Emscripten Runtime/native logical tick runs these checks. Input is
injected through the common core; no WebRTC/WebSocket transport is claimed.

| Script under `portable/multiplayer` | Required observation |
| --- | --- |
| `check-rollback.py` | Exact reference versus delayed input at frame 0 and 240, continuation through 359 |
| `check-rollback-3p.py` | Three pilots, two independent remote lanes |
| `check-rollback-bomb.py` | Non-predicted remote Bomb and native power debit |
| `check-rollback-stall.py` | Prediction ceiling; twenty more outer ticks cannot change logical state/ANM/RNG; repair matches reference |
| `check-rollback-pause.py` | Late P2 Pause, native paused menu, P1 Resume and continuation |
| `check-peer-seats.py` | Actual local roles P1 versus P2 under the same exact input timeline |

All six checks passed on the current source-identified multiplayer build:

- WASM: `9184f78d6213c4d25c0ba85d6b46dc8df07eb33db61ffe63d70cda1018259c7d`.
- Report: `artifacts/multiplayer-tests/suites/85fcfa6b-3d1f-4fc4-810e-0ee2849fb728/suite.json`.

The different-local-role check previously exposed GUI divergence at P2's Bomb
frame 60. The shared seat-zero native GUI fix now passes; local-seat highlighting
remains presentation-only. The 3P and Bomb checks include expanded categories
and genuine HELLO/READY exchanges. Input error enums are rejected, even when
their numeric value is nonzero.

The current canonical schema is version 2 (44 words). Its composite includes
the original gameplay/ANM/RNG categories plus Replay metadata, Common lifetime
fields, dialogue/geometry pools, camera/engine state and score records. Raw
pointer-heavy Enemy/ECL pool diagnostics remain separately identified. These
tests use compatible in-process layouts; they do not certify a portable
pointer-independent network checksum or exhaustive coverage of every owner.

The newer `rollback_testkit.py` records instantiated WASM SHA-256, browser
version, source hashes, failures and unique history reports. Earlier evidence
is retained when a later attempt fails.

## Diagnostic native fixtures

`node portable/build.mjs --multiplayer --multiplayer-fixtures` links the native
source set plus `portable/multiplayer/FixtureExports.cpp` into the separate
`th10_web/artifacts/multiplayer-fixtures` directory. The build is diagnostic.
Normal and production MP source plans exclude fixture exports;
`build-isolation.test.mjs` checks this boundary.

Fixture setup is allowed only at a fully confirmed idle boundary and retires
older checkpoints. It supplies explicit initial conditions; then native
collision, death, resource, rescue, input and rollback owners execute behavior.
There is no separate gameplay simulator or candidate-derived golden.

`check-native-fixtures.py` cases, all passing on the current fixture build:

| Case | Required observation |
| --- | --- |
| Deathbomb | Native hit, predicted death, twelve-frame-late Bomb restores lives/power/world |
| Rescue | Wrongly predicted final Focus tick revives a Spirit; correction restores donor/recipient/progress, then a genuine rescue succeeds |
| Pickup | Late movement changes the collector of the same native power item |
| Wipe cancellation | Late Pause cancels a predicted retry before resource destruction |
| Laser close | Actual session destruction with live pooled lasers, not browser-tab disposal |
| Retry | Native full-wipe/retry restores all pilots' resources |
| Previous stage | Old background retires at its original fade boundary, survives rollback until confirmation, and is not freed twice by a later correction |

Setup conditions are synthetic; resulting gameplay is native. The additional
`check-generation.py` passes with actual P1/P2 roles, old-session wire rejection,
a fresh frame-zero gate which cannot advance without peers, and 30 frames after
the new HELLO/READY exchange. Packets are real common-protocol bytes transferred
by the test; this is not WebRTC/WebSocket transport evidence.

All three current fixture-suite entries passed:

- WASM: `ab67b19d02d22b86f074bb00ca91fa34b38a4039cc218020feb0a0f873f9aaad`.
- Source digest: `6eae5fafc93567da2654c8428d3194858d7564a884ac0d5249c89b354b4dc0f1`.
- Report: `artifacts/multiplayer-tests/suites/9096f185-9fa1-43b0-80a4-2433a766775d/suite.json`.
- Entries: `check-native-fixtures`, `check-generation`, `check-dense`.

The native fixtures compare committed audio command histories as well as
declared world state. They prove once-only corrected command output in these
scenarios, not physical sound, looping quality or mobile browser audio health.

## Dense rollback measurements

`check-dense.py` starts with 1200 native bullets and corrects three independent
12-frame bursts at frames 60, 80 and 100. All corrected declared owner states
and committed audio histories match the uninterrupted reference. The final
counter is three rollback operations / 36 resimulated frames. Replay allocation
count stays at five; confirmed history is reclaimed after each correction.

On this host's software-rendered headless Chromium run:

- Captured journal bytes per dense frame: 2,840,084 (about 2.71 MiB).
- Live history during each burst: 12 frames; after confirmation: zero.
- WASM linear memory: 200,474,624 bytes initially measured, growing to
  240,582,656 bytes; the last two measured corrections stay at that size.
- Sampled tick time median: 18.85 ms; maximum, including a correction tick:
  300 ms.

This is correctness and bounded-scenario evidence, **not a mobile smoothness
PASS**. Three corrections are not a long-duration memory plateau test. The
current all-at-once 12-frame replay can produce a visible main-thread stall;
performance work must preserve exact state and use the shared rollback playbook.

## Ordinary Runtime / Replay gate bring-up

The ordinary and Presentation Lab C++ builds still match their recorded source
inventories. The ordinary WASM is
`1f5a9557f62f33c1243caa72a8649cd8335944665ceb70da763b8f0fe79f67e1`;
the Lab WASM is
`4396d3bb540edeeae45cd945386ca5bd6cae4edd76a271ac30feb5c0c8660b42`.

Two collector/startup problems were found before completing the quick gate:

1. Non-THPrac packages omit the practice modules, but the shell imported
   `practice.mjs` unconditionally. `practice-loader.mjs` now checks the compiled
   `practice_enable` capability before loading the optional bridge. Packaging
   keeps the loader, preserves the enabled dependency set, and rejects a
   feature declaration which disagrees with the actual WASM export.
2. Returning the full Lab controller from a separate browser evaluation let
   the live game advance before the collector froze it. One rejected capture
   began at Demo frame 2909 and contained only 91 rows of that Demo. Boot and
   freeze now execute within one browser evaluation; no Demo index, Replay
   cursor, RNG, input, missing frame or expected golden is rewritten.

The rejected partial capture is retained at
`artifacts/replay-verifier/rollback-isolation-ed352ac2-d3cf-4c39-8371-700aeeda9deb/suite.json`.
Collectors now persist an explicitly incomplete report from navigation onward,
including failure diagnostics and completed/partial observations. The Lab build
identity includes the packaged Runtime inventory hash so shell-only changes
remain identifiable even when the C++ WASM is unchanged.

The corrected quick golden comparison passed all four Demos in the authored
`demo1 -> demo2 -> demo3 -> demo0` rotation: 3000 compared gameplay ticks each,
12,000 total. Collection used 15,913 application ticks including the unchanged
title/startup lifecycle; these are not additional Replay samples.

- Capture: `artifacts/replay-verifier/rollback-isolation-b0a827f9-3972-4d20-9d0c-107689510564/suite.json`.
- Comparator report: the sibling `quick-result.json`, `passed: true`.
- Report SHA-256: `68331cff40c91b77f847791a404e528ad6c4f758a03f01f83ac9ba913f174453`.
- Packaged Runtime inventory SHA-256: `357c65ac1523917102c77a49f4d7f0dc72fa635dd3440737c7ad694fc7a9412a`.
- The collector source hash in the report matches the current source. There
  are no changes under the published `tools/replay-verifier/golden/` tree.

The local supporting gates also pass: 12 optional-practice/package/isolation
tests, five collector bookkeeping tests, and nine golden-integrity/playback
completion tests. The ordinary package was rebuilt successfully with the same
ordinary WASM. None of these supporting checks substitutes for the actual
12,000-tick comparison above. The long Lunatic/Extra daily lane is still pending.

## Remaining ownership and acceptance work

The rollback frame covers Update plus authored 60 Hz Draw. Input waiting must
not execute another authored Draw. Reconciliation precedes pending-screen
destruction so confirmation cannot discard a required restore.

Dormant native ANM, bullet and item slots retain state. First reuse captures
the whole slot before mutation; restoring only an inactive flag is insufficient.
Rare whole-pool clears preserve every overwritten slot first. Destructors must
return pooled memory to its owner, never `free()` a pool address.

Still required before the rollback item closes:

- complete owner inventory and broader stage/result lifecycle coverage beyond
  the passing focused fixtures;
- a complete confirmed multi-seat Replay writer/player (the preparatory native
  metadata must not be saved as a silently invalid single-player Replay);
- actual transport retirement/ACK fencing beyond synthetic packet exchange;
- the published long-Replay daily regression gate, without changing goldens;
- long-duration and real-device memory/performance validation.

Actual transport, spectator, MP Replay, touch capture/replay and Launcher room
lifecycle are additional mandatory work. A passing build or focused simulation
test does not prove them.
