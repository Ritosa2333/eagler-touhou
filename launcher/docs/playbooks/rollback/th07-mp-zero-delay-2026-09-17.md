# TH07MP zero-added-delay experiments — 2026-09-17

## Verification checkpoint: local candidate ready, not publicly deployed

This investigation re-read the on-disk source/results instead of assuming an
older pending-work list was authoritative. The following gates were verified:

- `.codex-tmp/zero-final-full-native-wasm.json`: a fresh complete **42/42**
  native/WASM SAFE_HEAP case run, status PASS. This includes the new dormant
  Bullet and input-repair cases; it is not a relabel of the old 38-case result.
- `.codex-tmp/zero-final-launcher-check-serial.log`: `npm run check` with
  `EAGLER_CHECK_JOBS=1` passed the complete repository-core gate. This replaces
  the prior Windows shared-output-lock blocker, not its historical failure log.
- `.codex-tmp/zero-dormant-final-v8/dormant-byte-bomb-audit.json`: Stage 6 with
  OGG, zero local input delay, actual RTC send impairment and both Bombs;
  the optional dormant-elision backend passed **1066 full-byte Bullet restores**
  and all six canonical checkpoints against an unchanged `journal` peer.
  Dormant elision remains **off by default**; correctness is not a performance
  win and this extra experiment is not a reason to postpone the base candidate.

Recovered final audio-on performance evidence is in the immutable
`.codex-tmp/zero-responsive-final-cp3/` fixture (3600 simulation frames,
Stage 6 Lunatic, peak 1024 bullets, continuous wall-clock touch, zero added
input frames, actual RTC sends delayed 50 +/- 10 ms in each direction):

| Report | CPU condition | Result and limits |
| --- | --- | --- |
| `audio-3p-defaults.json` | All three endpoints throttled 2x | All ~59.99 logic Hz; dense Present p99 30.3/31.7/29.8 ms; zero >50 ms Present intervals in the **whole** run. Dense synthetic input-to-render medians 14.4/14.2/14.2 ms and p99 28.1/28.0/26.7 ms. All 12 canonical checkpoints agree. |
| `audio-2p-slow4-defaults.json` | One endpoint throttled 4x | Slow endpoint ~59.83 logic Hz, dense Present p99 38.3 ms; **one** >50 ms interval in dense phases, but **three** in the whole run, whole maximum 70.7 ms. Do not present the dense count as the whole-run count. All 12 canonical checkpoints agree. |

The audio probes observed advancing AudioContext time and nonzero output blocks;
this is not an acoustic-quality or device-speaker acceptance claim. All browser
tests above are Windows Edge, not physical Android/iOS. Synthetic input-to-render
ends at render submission and excludes hardware touch scanning, compositor and
screen scanout. Zero input scheduling frames do not mean zero physical latency.

The latest read-only public manifest check still returned
`runtime/th07/multiplayer/th07.html?hosted=1&v=9b0e491b38e7bb81`.
No deployment or commit was performed in this continuation. The local immediate
mode and selectable old buffered mode have Launcher integration passes, but
public cross-device/thermal/subjective responsiveness acceptance remains open.
The current tested code is a trial candidate, not a guarantee of perfect play on
every phone or a claim that all network outages are eliminated.

## Resumption checkpoint: actual code is beyond the initial summary

The source and raw reports were re-read before continuing. Do not restart the
old snapshot experiments or describe the incremental-reconcile wiring as pending.
Latest adopted local policy is `responsive` = zero input frames/full rollback
for every endpoint, live-part snapshots, 3-tick undo checkpoints, bulk copies,
coalesced restoration, 60 Hz presentation, same-origin immediate input, and
bounded reliable duplicates when the input acknowledgement frontier stalls.
`balanced` remains selectable and retains the previous configuration. Checkpoint
spacing is NOT scheduled input latency. No new public deployment occurred here.

Recovered gates from the work already on disk:

- `.codex-tmp/zero-byte-oracle-v5/live-byte-bomb.json`: real Stage 6, both Bombs
  executed; 1072 full-Bullet-byte restore comparisons on `live` versus an old
  `journal` peer; all six canonical checkpoints agree. This is a correctness
  diagnostic with intentionally expensive full copies, not a speed benchmark.
- `zero-delay-full-native-wasm.json`: 38 passed cases before adding the two
  native/WASM InputRepairBudget cases; those passed separately. Do not silently
  relabel this older JSON as a fresh full 40-case run.
- `zero-live-replay-2.log` / `zero-live-replay-3.log`: save/playback passes with
  zero-delay live snapshots and exact input comparison (306/306 frames for 2P,
  66/66/65 for 3P). This is the actual recorded playback range, not a full run.
- `launcher-responsive-e2e-r6/result.json`, `launcher-responsive-3mobile/result.json`:
  real Launcher, RTC and room-selected zero/full for mixed 2P and three
  mobile-designated endpoints. Browser identity simulation is not a real phone.
- `launcher-balanced-mixed-r2/result.json`: optional old mode launches as
  standard=4/full, mobile=1/buffered. The preceding failed report is retained;
  its dialog click raced a disappearing/closing startup dialog. The test now
  scopes the real click to the active dialog and tolerates only observed dialog
  closure; a persistently non-clickable dialog is still a failure. No force click.

New extended evidence uses the immutable `zero-responsive-v7/frozen` Runtime,
not a moving build directory: `healthy-3p-cp3-3600-r2.json`, Stage 6 Lunatic,
three CPU-2x endpoints, continuous wall-clock touch, 50 +/- 10 ms application
send delay on real RTC input, and zero input buffering. The three endpoints
completed 3600 frames at 59.987/59.988/59.988 logic Hz. Dense present p99 was
29.4/31.0/30.2 ms, maxima 39.3/39.2/34.4 ms, with zero >50 ms intervals.
Synthetic event-to-render p99 was 26.4/27.9/27.3 ms; this excludes physical
touch scanning, browser compositor and screen scanout. All 12 canonical samples
agree. The earlier cp3 1800-frame run also passed; the worse cp1/cp2 reports
remain available and are not discarded.

An approximately 4-second interruption exists in an earlier immediate-input
run. It was NOT deleted or declared a game-compute problem. Separate fault
injection in `zero-input-repair-v6` / `zero-responsive-v7` establishes a limited
claim: reliable duplicates help when the fast input lane stops but the control
lane still works. They do not cure total network outages. The repair payloads
receive the same injected 50 +/- 10 ms delay as ordinary input, avoiding a free
zero-latency repair channel in the test.

`zero-responsive-final-cp3/frozen` now contains the rebuilt candidate, copied OGG
resources and a dedicated `--responsive-profile` test lane. That lane passes only
the room-level zero/full choice and lets Runtime choose the real snapshot,
cadence and repair defaults; assertions compare the resolved configuration.
Audio-on tests and final full checks were the active tasks at this checkpoint.
The first parallel Launcher `npm run check` hit a Windows shared-output file
lock in esbuild; retain that log and use the supported `EAGLER_CHECK_JOBS=1`
repeat rather than calling the failed check a pass.

### Method used to decide acceptance

Keep old and new runtimes/fixtures immutable with hashes. Independently verify
the actual transport impairment, rendering intervals, input-to-render timeline,
audio output and confirmed simulation state. Compare optimized history against
an unchanged backend, then exercise Bomb/clear, replay and real Launcher modes.
Never trade away bullet counts, collision correctness or input sampling for a
better FPS number. Do not confuse logic Hz with unique presented frames, a
successful room join with gameplay correctness, or CPU throttling with a phone.

This file supersedes the **goal**, not the retained results, of the mobile-owned
delay policy. The user rejected both 100 ms mobile delay and 67 ms desktop delay.
The principal target is zero scheduled input frames on **every** endpoint, while
retaining original 60 Hz gameplay and exact corrections. Buffered policies remain
an explicit optional fallback. No experiment below has been deployed yet.

## Current source and production boundary

- Repositories: `th07-eagler` and `eagler-touhou` in `D:/workspace/eagler`.
- Production last checked in the preceding task: runtime `9b0e491b38e7bb81`,
  identity `th07mp-20260917-mobile-owned-rollback`. Preserve that release/backup.
- Existing full journal, frontier, bulk-copy, coalesced-restore, incremental
  reconciliation and failed compact-matrix experiments already existed. Do not
  reimplement them from an old conversation summary.
- Original/optional buffered mode retains `journal`. The explicit zero/full
  responsive mode now selects the verified `live` backend.
- Launcher/relay local source now has a room-level `responsive`/`balanced`
  setting, declared for TH07 only. Responsive means every endpoint 0/full;
  balanced retains the previous one-mobile 1/4-frame allocation and fallback.
  Missing field from an older relay does NOT imply zero-delay support.

## Frozen zero-delay baseline: a significant correction

`tests/measure-netplay-smoothness.py --freeze-only` created
`.codex-tmp/zero-latency-baseline-20260917/frozen/` with runtime and fixture hashes.
The **pre-existing code**, before this task's optimization, already passed the
following zero-delay tests. Do not attribute this baseline success to new code.

Every run below uses Edge, Stage 6 Lunatic, continuous Launcher direct touch,
actual RTC input send impairment 50 +/- 10 ms one-way, full rollback, frontier
snapshots, runs/bulk/coalesced, 2-frame checkpoints and capped 60 Hz presentation.
CPU throttling is not an Android/iOS device, and send impairment is not measured
public RTT. PASS means completion, requested configuration/route and canonical
checkpoint agreement, not a subjective smoothness acceptance.

| Report | Frames | Slow CPU | Slow dense logic Hz | Dense present mean / p99 / max ms | >50 ms |
| --- | ---: | ---: | ---: | --- | ---: |
| baseline `zero-stage6.json` | 1800 | 1.5x | 60.094 | 16.64 / 27.2 / 32.0 | 0 |
| baseline `zero-stage6-cpu4.json` | 3600 | 4x | 58.81 | 24.28 / 47.0 / 102.6 | 6 |
| live-v1 `zero-live-vs-journal.json` | 3600 | 4x | 59.301 | 26.63 / 46.4 / 52.4 | 4 |
| live-v2 `zero-live-vs-journal.json` | 3600 | 4x | 59.88 | 22.75 / 38.7 / 49.4 | 0 |

Live reports are under `.codex-tmp/zero-live-bullet-v1/` and `...-v2/`.
Both test the new backend on the slow peer **against the old journal backend on
the other peer**, with all 12 canonical checkpoints equal and peak 1024 bullets.
The baseline and candidates are separate runs, not identical input recordings;
repeat and use wall-clock input before making a strong relative-speed claim.
Dense logic Hz is not unique rendered frames: mean 22.75 ms still means missed
presentation opportunities despite near-60 Hz simulation. Continue improving.

## Implemented live-parts journal

`PartitionedPoolJournal.hpp` keeps exhaustive disjoint first-write parts of a
fixed object pool, using slot/part identity instead of an address AVL index.
`LiveBulletSnapshot.hpp` divides each Bullet into its five complete AnmVms and
one complete tail. It does not omit matrices, padding or pointer bytes.

At a checkpoint, capture normal VM + despawn VM + tail, plus the selected spawn
VM when spawning. Unchanged dormant spawn VMs stay live. Every spawn, slot reuse,
and whole-Bullet clear records all remaining parts **before** writing them.
The two destructive memset sites in BulletManager now have explicit touch hooks.
Restore takes the oldest recorded value for each part, including parts first
written only in later checkpoints. It does not ignore input mismatches.

V1 cut peak snapshots from ~5.2 MB to ~3.1 MB but did not reduce capture CPU.
V2 builds merged gather/scatter regions and uses one JS copyWithin batch callback
per capture/restore batch, retaining a native memcpy fallback. It copies before
returning to game code and reacquires heap views after memory growth.

`live-bullet-unit-v1.json` and `...-v2.json`: 4/4 native/WASM SAFE_HEAP cases pass.
The generic oracle runs 6000 checkpoints / 2000 exact-byte restores with extensions,
ring reuse and cold writes. The Bullet oracle includes dormant state and complete
slot clears. This is not a substitute for real-game Bomb/Replay/phase regression.

## Input responsiveness instrumentation added, not yet accepted

- `--wall-clock-drag`: gesture phase advances by real elapsed time, so slow
  simulation cannot make the input stimulus quietly slow down too.
- `--all-slow`: apply the declared CPU rate to all endpoints for phone/phone.
- `--input-latency`: diagnostic synthetic Launcher event -> receive -> physical
  capture -> original forward simulation -> first Present containing it.
- `tests/netplay-input-latency-test.cjs` validates decomposition and no duplicate
  accounting on replay. Actual device sampling/compositor/scanout are excluded.

## Initial remaining gates (superseded by the resumption checkpoint above)

Build latest timestamp hooks; paired wall-clock/latency tests at zero vs buffered
delay; multi-slow 2P/3P; Bomb/clear and ordinary replay/phase correctness; Launcher
mode selection/readiness behavior and full check; decide whether measured `live`
backend earns default status. Do not erase old experiments or claim that phones
have been physically tested. Keep production unchanged until an accepted candidate.
