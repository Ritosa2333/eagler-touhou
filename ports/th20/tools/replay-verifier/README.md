# TH20 Replay original/source comparison

Uses the sibling `eagler-common/testkit/replay-verifier` comparison protocol,
as used by TH10 and TH15. Set `EAGLER_COMMON_ROOT` if that checkout is elsewhere.
This is a test tool; the production game does not execute original machine code.

## Requirements

Windows x64, 64-bit Python, Node, sibling `eagler-touhou` Puppeteer dependencies,
Chromium (`TH20_CHROME` can override the executable), and the pinned Japanese
retail 1.00c executable plus adjacent `th20.dat`/`thbgm.dat`.
The executable SHA-256 is
`a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897`.
Supply a real `.rpy`; it is copied into private slot 02. Stages are 1-based;
stage 7 is Extra. The selected stage must exist in the replay.

Run from the repository root, with your own paths:

```powershell
node portable/build.mjs --th20
node th20_web/replay-probe/serve-replay-probe.mjs --port 8791 --game "C:/path/to/retail" --replay "C:/path/to/th20_02.rpy"
```

In another terminal:

```powershell
python tools/replay-verifier/capture-original.py --executable "C:/path/to/retail/th20.exe" --replay "C:/path/to/th20_02.rpy" --stage 6 --max-ticks 3000 --output build-perf-probe/original.json
node tools/replay-verifier/capture-candidate.mjs --url http://127.0.0.1:8791/replay-probe.html --dat "C:/path/to/retail/th20.dat" --replay "C:/path/to/th20_02.rpy" --stage 6 --max-ticks 3000 --output build-perf-probe/candidate.json
node tools/replay-verifier/compare.mjs --expected build-perf-probe/original.json --actual build-perf-probe/candidate.json --output build-perf-probe/comparison.json
node --test tools/replay-verifier/adapter.test.mjs
```

For a complete run, increase `--max-ticks` on both captures and `--timeout`
on the original capture. The original runs at real game speed. The capture
tools terminate only the private oracle/browser they create. They leave an
already running user game alone. Run only one original capture at a time. On a desktop in active use, run the
original and candidate captures sequentially. Both use below-normal process
priority; the browser collector paces batches to at most 60 simulation ticks
per second without altering simulation time.

Start with `--stage 1` on both sides for a whole ordinary run, for example
`--max-ticks 300000` and original `--timeout 7200`. Do not infer completion from
the input stream length: the original cursor can continue with zero input
after recorded inputs are exhausted. Completion is observed from the original
ReplayInf finished flag (`0x25c`, bit 1) or controller teardown. The SDL export
reads the equivalent source flag. Neither collector presses menu confirmation
to manufacture completion.

Asynchronous stage loading can create different numbers of transition frames
and different clock/initialization timing. The strict chronological comparison
retains these differences. For additional diagnosis, `audit-frames.mjs` compares
every shared exact `(stage, replayFrame)` identity, reports missing frames and
all field differences, and labels the first difference outside loading/fade
flags. It never shifts frames or returns acceptance/PASS:

```powershell
node tools/replay-verifier/audit-frames.mjs --expected build-perf-probe/original-complete.rows.jsonl --actual build-perf-probe/candidate-complete.rows.jsonl --output build-perf-probe/frame-audit.json
```

## Isolation and sampling

The original collector copies the executable into the output directory's
`original-private/<receipt-name>/<process-id>`, links resource files, and provides fresh private APPDATA/config
and replay data. Original installation files are never modified. `_NO_DEBUG_HEAP=1` keeps the
private original on the ordinary Windows heap. Debugger-created heap poisoning
otherwise changes freed-region coordinates to `0xfeeefeee`, creating artificial
replay divergences. The receipt records this heap policy. It changes
the private process's host mutex name to permit coexistence and uses debugger
breakpoints at input aggregation (`42010a`) and completed draw dispatch
(`412ccf`). Original bytes are restored for each instruction, single-stepped,
then the breakpoint is reinserted. ASLR is accounted for.

Menu inputs are injected before button derivation; physical input is zero
during Replay. Recorded gameplay input, gameplay clocks and RNG are untouched.
The private process's window active flag is kept set at input aggregation to
match the stopped-loop SDL host and prevent desktop focus from auto-pausing.
The candidate advances its normal SDL update/draw path by fixed ticks. Both
use internal 640x480 coordinates, scale 1, independent of presentation size;
the collectors/adapter reject mismatched internal scale. Oracle display mode
3 is deliberate: mode 8 can select a different internal scale on the desktop.

State samples start at replay input cursor 1, after update and draw. There is
no frame shifting, resynchronization, omitted start, or tolerance for gaps.
The common comparator reports the first difference. Caught original startup
C++ exceptions are recorded separately from fatal exceptions. Runtime errors,
empty captures, invalid words, and missing samples reject the comparison.

`PASS` requires matching states and natural completion on both sides. Matching
bounded prefixes are **INCOMPLETE**, not PASS. A difference in a bounded prefix
is still **DIVERGED**. The comparison command exits nonzero for every non-PASS.
If an incomplete collector stops earlier than its peer, the shared frames are
compared without discarding samples; that coverage boundary is INCOMPLETE.
Different natural completion lengths remain DIVERGED.

## Recorded score checkpoints

Candidate capture receipts include `recordedScoreAudit`. The audit decrypts a
read-only copy of the replay and compares each recorded stage's final input
frame with the following stage's stored starting score, using exact 64-bit
integer scores. Displayed scores are ten times the internal values. A mismatch
blocks parity acceptance even when two captured traces agree with each other.
The stage's restored starting score is not independent verification.

```powershell
python tools/replay-verifier/audit-recorded-score.py --replay build-perf-probe/fixtures/th20_01.rpy --rows build-perf-probe/candidate-complete.rows.jsonl --output build-perf-probe/recorded-score-audit.json
```

Missing endpoints remain incomplete. The header's final score is reported
separately until post-clear/result-screen scoring is observed; the last stage's
last input sample alone does not prove that result. Matching scores are necessary
checkpoints, and do not replace RNG, position, life and bullet-state checks.

## State schema v2

Clock experiments require building with `TH20_REPLAY_CLOCK_EXPERIMENTS=1`.
Only the verifier translation unit receives the macro; the build report is
marked diagnostic. Captures with `--charge-offset` or `--shot-offset` are
rejected as ordinary parity evidence. Rebuild without that environment variable
before delivering a playable artifact.

36 exact unsigned words per frame; floats use raw IEEE-754 bits, not rounded
decimal coordinates. `adapter.mjs` lists each word's name. Captured fields cover
replay stage/cursor, gameplay clocks/flags, derived input, game RNG last/state,
self position/fixed position/state/focus, score, power, lives/fragments,
bombs/fragments, session flags, dialogue state, movement speeds, graze, and
logical scale, point value, special-item count, point ratio and overlay meter. Required categories are RNG, player and economy.

This schema does **not** prove every enemy/bullet/laser/ECL/animation object is
identical. Differences in those objects can precede the first observed scalar
difference; investigate that frame with targeted diagnostics. A recorded run
that also loses lives in the original is not repaired by inventing inputs or
invulnerability in the candidate.
