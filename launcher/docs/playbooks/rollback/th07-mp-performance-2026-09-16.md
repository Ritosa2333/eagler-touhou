# TH07 MP performance investigation — 2026-09-16

## Scope and acceptance

The change removes avoidable MP overhead without changing fixed 60 Hz game
semantics, protocol, input delay/prediction policy, Replay format, state coverage
or canonical checks. No commit, deployment or real-phone acceptance is implied.
The user's complete mobile-smoothness objective is **not yet accepted**: strong
CPU stress still stalls, and an intermittent 3P disagreement remains unclosed.

Final production WASM: `build-web-th07-netplay/th07.wasm`.
SHA-256: `c7272e6eb2d352d5655d728e1b844091c0d92a74d1408787bb8f0b7a0749ef9e`.
Both production and Replay-audit Web builds completed. `git diff --check`,
Python compilation and the existing rollback source-contract audit passed;
source-contract results are not additional gameplay acceptance.

## Implemented changes

`src/netplay/RollbackJournal.{hpp,cpp}` replaces deque/map history with a bounded
slot ring, reusable uninitialized byte arenas and an index-based AVL tree in
contiguous block storage. The old code reused bytes on age eviction but destroyed
them on rewind/confirmation, immediately reallocating during resimulation.
Per-block map allocations and zero-initialize-then-copy byte growth are removed.
Warm-up/larger scenes may still allocate within the existing bounds; this is
not a claim of allocation-free gameplay or a dense whole-world snapshot.

Duplicate first writes, overlap rejection, address overflow and capacity checks
are retained. Arena growth preserves captured bytes. Iterative insertion reuses
the overlap-search path and stops balancing once subtree height is unchanged.
An earlier recursive version measurably regressed forward work by 8–14%; the
final algorithm was selected only after fixing and remeasuring that regression.

`FrameAdvantageWindow.hpp` and `Th07LanStageProbe.cpp` remove a temporary vector
allocation per input packet while preserving the exact per-peer 64-sample
trimmed-mean policy. `FrameBudget.hpp` and `GameWindow.cpp` add an 8 ms boundary
for starting extra catch-up ticks, retaining the existing six-tick cap. One due
tick is always allowed; remaining debt is retained. This does not interrupt an
atomic rollback or guarantee callbacks shorter than 8 ms.

`GameWindow.cpp` also removes conditional synchronous post-render SDL waits on
Web. The existing requestAnimationFrame callback, nonblocking presentation cap
and fixed-step accumulator own pacing. Native pacing is unchanged. The old
wait depended on imported configuration, so it is a platform hazard, not a
proven explanation for every observed stall. Emscripten's guidance likewise
separates browser callbacks from the native loop's delay:
<https://emscripten.org/docs/porting/emscripten-runtime-environment.html>.

## Measured storage results

Baseline: `5f80a0df8a8434afd2544d4aaa95691ea4d65f89`. Identical Emscripten
6.0.6-git C++17 `-O2` builds, Node execution, three alternating-order runs.
Synthetic workload: 1,536 blocks x 2,048 bytes, 160 measured iterations,
pre-warmed bounded history. Values are medians of each run's mean, not phone FPS.

| Workload | Before ms | After ms | Cost reduction |
| --- | ---: | ---: | ---: |
| Forward, address order | 0.360687 | 0.323014 | 10.4% |
| Forward, scattered | 0.448058 | 0.385896 | 13.9% |
| Six-tick rollback | 4.074920 | 3.087361 | 24.2% |
| Discard and recapture | 0.544655 | 0.393672 | 27.7% |

New warmed journal allocation counts were zero in every workload/run. The old
six-tick workload made 1,476,962 allocations over 160 iterations: cumulative
churn, not allocations per game frame or peak memory. Evidence:
`.codex-tmp/journal-wasm-iterative-comparison.json`; earlier recursive results
remain in `.codex-tmp/journal-wasm-comparison.json`.

## Validation and limitations

Native and WASM SAFE_HEAP/assertion tests all passed for the journal, frame
budget, frame-advantage window and core (eight cases). The standalone core test
holds multiple full histories on its stack and uses a 1 MiB test stack; no
production stack setting changed. Journal coverage includes 4,093-block
insertion orders, overlaps/adjacency, full-capacity duplicates, growth, eviction,
checkpoint extension, repeated undo and 2,000 randomized history operations
against a dense-copy reference. Window comparison covers 10,000 samples.

Browser observations:

- 2P keyboard: 600 frames without injected delay and 900 at 55 ms one-way relay
  delay with 10 ms jitter completed with sampled state agreement, about 60
  logical frames/s.
- 2P analog/one endpoint CPU-throttled 4x: 1,800-frame old/new journal runs
  agreed at all six checkpoints, but did not sustain 60 logical frames/s.
- 3P keyboard/analog: 600-frame reruns and a 900-frame analog rerun passed.
- 2P real Replay: record with rollback, save, menu playback and exact comparison
  of 310/312 playback input frames passed.
- Final Web-wait cleanup build: 2P actual RTC analog run completed 900 frames,
  all three checkpoints agreed, about 60 logical frames/s.
- Final 3P relay/analog run: 900 frames and all three checkpoints agreed;
  logical throughput was about 53 frames/s, not 60. Raw report:
  `.codex-tmp/netplay-final-3p-relay.json`.
- Final spectator check: players reached frames 316/317 and spectator 309,
  with the same frame-300 canonical hash and a read-only spectator transport.
- Final Replay rerun: 3P exact input comparison passed for 71/69/73 playback
  frames; 2P passed for 305/307. This does not establish full-game Replay coverage.

The controlled storage A/B changed only the journal and its direct owner TU,
retaining other candidate code. One pair improved the slow peer's RAF mean from
48.23 to 38.30 ms, but logical throughput went from 36.91 to 35.39 frames/s with
multi-second stalls. This mixed observation is **not** an established whole-game
speedup. RAF callbacks are not necessarily completed game draws, and multiple
desktop browsers with CPU throttling are not actual mobile devices.

One earlier 3P 900-frame analog run disagreed with P3 beginning at sampled frame
300 (`.codex-tmp/netplay-perf-after-3p-analog.json`). Later passes do not prove a
repair. Final metadata/item hashes differed while positions and several owners
agreed. `ItemManager::OnDraw` writes `isOnscreen`, which is included in the hash;
this is an owner-boundary lead, not proof of the metadata divergence's cause.
Checks were not weakened. Detailed later passing evidence is
`.codex-tmp/netplay-after-3p-900-diagnostic.json`.

One delayed 2P and one old-journal 3P run timed out; later runs completed.
A 3P Replay attempt also timed out in recording at frame 65 (confirmed 52),
before the later successful Replay rerun. That timeout is not a proven repair.
Repeated remote-tool HTTP 404 failures occurred too, but runtime failures must
not all be reclassified as infrastructure problems without evidence.

The initial browser fixture missed the shared font and failed before gameplay;
the performance, Replay and spectator fixtures now load it. Startup failure is
checked before interpreting timing. No physical Android/iOS, thermal, OGG/MIDI,
public-network or long dense-bullet gameplay acceptance was performed.
`adb devices -l` returned no attached devices. The spectator fixture's old
take-seat-with-ready assumption was also corrected: wait for the final roster,
then send explicit `set-ready` messages. Its response waits are now bounded.

## Reproduce from the TH07 repository

Build `build-web-th07-netplay` and `build-web-th07-netplay-replay-audit` first.
The analog performance fixture requires production `TH_DEV_TOOLS=OFF`; Replay
lifecycle deliberately uses the separate developer-tools audit build.

```powershell
python tests/run-netplay-performance-tests.py --output .codex-tmp/netplay-unit-validation.json
python tests/benchmark-rollback-journal.py --toolchain wasm --before-ref 5f80a0df8a8434afd2544d4aaa95691ea4d65f89 --repeats 3 --output .codex-tmp/journal-wasm-iterative-comparison.json
python tests/build-netplay-journal-baseline.py --revision 5f80a0df8a8434afd2544d4aaa95691ea4d65f89
python tests/netplay-performance-browser.py --frames 900
python tests/netplay-performance-browser.py --frames 1800 --analog --cpu-rate 4
python tests/netplay-performance-browser.py --players 3 --frames 900 --analog
python tests/netplay-performance-browser.py --frames 900 --analog --rtc --delay-ms 0 --jitter-ms 0
python tests/netplay-replay-browser-smoke.py 2
python tests/netplay-spectator-browser-smoke.py th07
```

The baseline builder reads exact Git blobs, compiles two TUs through stdin and
relinks into a distinct build. It does not check out or overwrite source, scan
the workspace, or replace the candidate build. Regenerate it after changing
other candidate objects to keep the A/B controlled.

## Methodology

1. Investigate the incremental cost of enabling MP, not the game's age. Split
   simulation, rollback multiplicity, storage/copy/allocation, rendering and
   network waiting; do not assume every low-FPS symptom is GPU load.
2. Preserve ownership and invariants before optimizing. Keep input sampling,
   RNG/collision order, state coverage and confirmation semantics unchanged.
3. Use three separate evidence levels: model-based behavior tests, same-compiler
   cost A/B, then end-to-end browser/device acceptance. None replaces the next.
4. Fix lifetime before micro-operations. Rewind must preserve capacity for the
   immediate replay; then remove duplicate initialization, searches and nodes.
   Benchmark the normal forward path as well as rollback to expose regressions.
5. Respect the event loop. Bound additional catch-up work and yield to browser
   input/network/audio instead of sleeping; never drop game work to fake FPS.
6. Report tails and logical progress as well as averages. Separate confirmed
   state from predictions and real drawing from mere callback activity.
7. Retain failures and provenance. A later green run does not close an earlier
   unexplained desync. Record first differing owners and real device/browser,
   workload, compiler, transport, timing and build identity.

The next acceptance boundary is a reproducible phone MP capture of the same
dense gameplay/input sequence, recording driver/render cost and confirmation
progress together. Long stalls and intermittent 3P mismatch remain explicit
work items, not hidden behind microbenchmark gains.
