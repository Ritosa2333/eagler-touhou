# TH09 rollback — measured evidence

Release WASM: `868ab04843a65a3e3bbfa9ec7c0db9fe11ee36f9c22e6bf0e2a2f47d7d6aaffd` (2,791,427 bytes).
Development WASM: `fe0554c85be7d37f7a09fb9dac2f3cc7ff82e8b63f813126476b06b0e59da386`.
[Machine-readable record](rollback-evidence.json) contains report fingerprints,
source identities and result details. No deployment was performed.

## Successful optimization

Fixed WASM and identical inputs, serial A/B/A; 1,800 logical frames per run.
Every restore was checked against the original execution's state inventory and
framebuffer. Each run also passed recording chunk and motion rewind checks.

| Run | Capture total ms | Restore total ms | Capture + forward + restore ms | Initial byte counter | State/image comparisons |
| --- | ---: | ---: | ---: | ---: | ---: |
| A: dense | 420.61 | 350.47 | 958.17 | 11,274,252 | 270 / 270 |
| B: sparse | 117.02 | 87.62 | 393.68 | 2,741,824 | 270 / 270 |
| A2: dense | 422.51 | 349.57 | 958.60 | 11,274,252 | 270 / 270 |

Capture plus restore time decreased **73.48%** against
the mean of the two dense runs. Including forward execution and its first-write
capture overhead, measured work decreased **58.92%**.
These totals cover 270 saves/restores and 1,620 forward ticks, after a 180-tick
warmup. Verification hashing, image encoding and the second execution are excluded.
This is not total game FPS. The initial journal/owner byte counter decreased
**75.68%**; it excludes subsequent first writes,
attack-state clones and container/callback metadata, so it is not total snapshot
size or allocated-memory use.

Local scheduling delay is **0 frames**, compared with the former 6-frame queue
(100 ms at 60 Hz). C++ tests check same-frame input use and once-only capture.
The browser test matches independent exact-input gameplay and Replay. This does
not claim zero device-to-display delay.

## Correctness and transport

- Five 600-frame C++ network conditions: steady, jitter, spike, all fast input
  lost, and loss/reordering/duplicates. All converge with once-only captures.
- ECL/effect ownership survives destruction and reuse; all nine attack types
  restore; production adapters compile without RTTI.
- Three additional 1,800-frame character pairs with round reset: (2,4), (6,9),
  (10,12). 810 further full-state/framebuffer comparisons pass.
- Two independent browser worlds plus exact-input reference: 900 frames,
  matching audit, image, hash and 1,312-byte Replay.
- Standalone UI: pause/resume, round reset, whole-world retry, completion, local
  Replay saving, reconnect and disconnect pass.
- Real local RTC and WebSocket fallback: 25–61 ms injected one-way delay and an
  800 ms spike. RTC also drops fast input. An admitted spectator consumes
  confirmed input. Audio contexts run and their clocks advance.

| Transport | Peer frames | Corrections | Resimulated frames | Spectator frame | Elapsed s |
| --- | --- | --- | --- | ---: | ---: |
| transport-rtc | 600 / 600 | 330 / 333 | 1893 / 1992 | 600 | 11.38 |
| transport-relay | 600 / 600 | 355 / 362 | 2282 / 2303 | 600 | 11.61 |
| transport-rtc-release | 611 / 610 | 350 / 350 | 2032 / 1989 | 600 | 11.25 |

Development runs stop at exactly 600 frames and compare all three world hashes.
The release smoke has no test-only frame limiter: endpoints can stop on different
frames, so final hashes are compared only at equal frames. Native periodic
confirmed-hash checks remain active in every run. The transport workload has a
14-second upper bound for 600 ticks, including startup and the injected outage,
to catch recovery accidentally consuming normal simulation time. Historical
pre-fix timing remains in the ignored transport-before-scheduling report.

## Limits and reproduction

Desktop Chromium/SwiftShader results do not establish physical-phone or public
TURN acceptance. Audio clock progress does not establish absence of underruns.
An outage exceeding the eight-frame prediction limit still pauses simulation.

See [migration and commands](rollback-migration.md). After reproducing the named
reports, run `node th09_web/tests/multiplayer/summarize-evidence.mjs`. It fails
if a report fails, a WASM identity differs, or sparse capture fails to beat both
dense runs.
