# Death, item and resource desync measurement

Status: active investigation; real-device desync is not resolved.

## Observed fault and evidence boundary

The report is an ordinary first death with spare lives, followed by different
Power for the same seat on different endpoints and different shared Time.
Spirit/rescue is not a prerequisite. The cancellation-owner patch fixed the
incorrect point-talisman drops but did not establish synchronization correctness.

The earlier `check-desync.py --ordinary-death` run used a fixed frame-180
fixture-input hit, zero bombs, and periodically drained its packet queue before
hash comparison. Its asymmetric mode delayed delivery to endpoint 1 only.
Consequently the P2-death case did not delay the P2 hit input on endpoint 0;
large rollback counts were not proof that the disputed death branch was covered.
The canonical item hash also omitted the exact per-item owner/recipient arrays.

## Observation, not repair

`ResourceTrace.cpp` observes the real RollbackDriver, PlayerLife, resource-value
and ItemSystem call paths. It never copies state between peers, forces shared
resources, changes collision rules, samples a new device input, consumes RNG or
delays local input. The observer is outside rollback ownership deliberately:
it retains the first predicted attempt and the latest resimulated revision.

Each frame has a pre-update and post-semantic-draw snapshot, the exact used
per-seat input sample, prediction mask, revision and correction status. Values
include per-seat Power/lives/bombs/gauge/deathbomb state, shared Time and its
requirement/total, score, RNG seed/calls and the separate time-of-night clock.
Every active item has a slot, type, movement state, age, exact owner/recipient,
position/velocity/target bits and linked-list neighbors. Collection, spawning,
POC claims, homing cancellation, ordinary-death transitions and Time/Power
mutations have before/after events. Slot identities avoid process addresses.

After UndoTo, restored fields are compared with that endpoint's saved start of
the same frame before resimulation. This distinguishes failed undo from a later
different native transition. It audits the recorded subset, not every owner.

Live recording is opt-in. It retains at most 768 frames and approximately 32 MiB
of vector storage; old confirmed records may be evicted earlier under density.
Overflow and eviction are explicit. Export freezes only the observer, not the
simulation, and preserves the confirmation frontier and run at the freeze instant.
No data is uploaded automatically and no Relay/TURN credentials are exported.
Recording has CPU/memory overhead; it is not a performance benchmark. Export
both endpoints promptly while the same game is still open.

## Focused causal harness

`portable/multiplayer/check-death-desync.py` uses three bombs, nonzero shared
Time, active Power/time-orb items and existing stationary projectiles. Only real
movement makes the victim hit a projectile; there is no fixture death input.
The field is authored at an already-confirmed boundary. Keyboard and direct-
touch samples are supported. There is no world reset after that boundary.

Both directions have independently varying deterministic packet delay and
reordering. There is no periodic queue flush. Historical revisions are compared
only through the common confirmed frontier. JSONL is flushed as frames are
compared, so failures do not erase preceding evidence. Inputs and the packet
schedule are retained for reproduction of the test condition.

The gate requires a real spare-life death, a negative Time mutation, collection,
and (for delayed cases) a death-state revision caused by correction. A controlled
run that reached full Power and canceled its lethal projectile was classified
`coverage-incomplete`, not PASS or desync.

Run with a built fixture and the caller's private DATA path:

```text
node portable/build.mjs --th08 --multiplayer-fixtures
python portable/multiplayer/check-death-desync.py --victim 1 --frames 420 --delay 4 --jitter 3 --data <private-DATA-path> --output <evidence-path.json>
```

Use `--delay 0 --jitter 0 --presentations 0` for a reference run. Then use
`--baseline <reference.frames.jsonl>` to compare to that independent trajectory,
not merely another equally speculative world. A different scenario/loadout/input
must not use an unrelated reference as its oracle.

## Actual-runtime capture and pair comparison

```text
node portable/build.mjs --th08 --multiplayer --resource-trace
node portable/package-eagler.mjs --multiplayer
```

The explicit trace build compiles the normal multiplayer product, NOT
`TH_MULTIPLAYER_FIXTURES`. It has no fixture exports or synthetic death input.
The ordinary no-trace build excludes observer implementation and hooks.
After the trace-capable candidate is separately authorized and deployed, open
the Launcher with `?th08Trace=1` on both endpoints. The shell starts recording
when gameplay exists and shows an opt-in export button. The API is also in the
runtime iframe as `__th08Runtime.resourceTrace.export(false)` for a JSON object,
or `export()` for a local download. Do not close the game first.

```text
python portable/multiplayer/compare-resource-traces.py <P1.json> <P2.json> --output <comparison.json>
```

The comparator rejects different builds, sessions, run generations and loadouts.
It intersects confirmed windows and selects the latest recorded revision;
unconfirmed predicted divergence is not labeled desync. Results separately name
the earliest input, item, resource and RNG mismatches. The first mismatching
frame includes item field differences, a differing event and nearby attempts.
RNG is not assumed to be cause or consequence in advance.

`equal-in-retained-window` is deliberately not whole-run acceptance. A mismatch
at the oldest shared frame is left-censored: its origin may predate the capture.
Missing inputs, gaps, event overflow or a failed undo audit are inconclusive.
Two actual failing-device exports remain necessary when controlled runs do not
reproduce the human report.

## Evidence obtained in this investigation

The focused P2 keyboard/POC case with 4–7 frame bidirectional delay and an extra
presentation matched its zero-delay reference at every measured endpoint frame.
Both endpoints rolled back (15/15); the trace includes a corrected death branch,
homing cancellation and shared Time penalties. A P1 direct-touch case covered
ordinary death and correction without a recorded resource/item mismatch. Neither
result reproduces the reported real-device fault.

The attempted WebKit comparison failed native initialization and supplies no
cross-engine gameplay acceptance. Keep that failure separate from desync.

The normal-MP real-shell export integration verifies fixture isolation, opt-in
button availability, non-mutating export and comparison of retained records.
Packet delivery there is harness-controlled; it is not real transport acceptance.
Comparator negative controls alter exported Power, Time and item-owner fields
and verify detection; predicted-only mismatches and stale revisions are excluded.

Evidence is in `artifacts/multiplayer-tests/`: `death-measure-p2-reference-v2.json`,
`death-measure-p2-delayed.json`, `death-measure-p1-touch.json`,
`death-measure-p2-webkit.json`, and `live-resource-trace-integration-v3.json`.
Each report records its own build and scope. Never retroactively relabel an older
artifact as verification of a newer source tree.
