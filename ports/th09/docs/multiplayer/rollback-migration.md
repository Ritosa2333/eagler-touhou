# TH09 rollback migration — 2026-09-27

Status: implemented in `experiment/th09-multiplayer`. Both the Launcher and
standalone browser entry use the shared C++ rollback session. The distributable
Runtime is `th09_web/build-eagler-multiplayer`. The initial rollback release was
deployed; the subsequent PC cost optimization is local and has separate evidence
in [PC optimization](rollback-pc-evidence.md).
The next local candidate suppresses pure sprite geometry during historical
ticks; see [Draw optimization](rollback-draw-evidence.md) for its separate proof.

See [measured evidence](rollback-evidence.md) and its [machine-readable record](rollback-evidence.json).

## Ownership and references

- Base: `3b52630`, canonical `D:/workspace/eagler/th09-eagler` remains untouched.
- Implementation: `D:/workspace/eagler/worktrees/th09-multiplayer`.
- `third_party/eagler-common` is pinned to `ac82fa2` (SessionChannel and SessionPacing).
- Primary playbook: `eagler-touhou/docs/playbooks/rollback.md`; also multiplayer,
  replay-determinism, performance-rendering and adaptation-worktrees.
- TH06/07 is the proven design reference. Measurements here are TH09 measurements;
  no TH08 or phone result is substituted for TH09 evidence.

## What changed

`cpp/multiplayer/RollbackSession` owns the shared RollbackCore, SessionGate and
SessionChannel. Local scheduling delay is zero (previously six frames, about
100 ms at 60 Hz). Prediction is bounded to eight frames. Holding fire/charge and
focus is predictable; direction prediction expires after three frames. Bomb and
menu edges are not invented. TH09 absolute touch targets predict no pointer
movement when missing; they cannot use a displacement predictor's zero target.

Captured inputs, received inputs, acknowledgements and network clocks survive a
world rewind. Devices are sampled once per logical frame. Correction reuses that
input history. Confirmed output cannot advance past an unfinished correction.
Fast input, reliable repair, session identity and pacing come from eagler-common.
The previous lockstep APIs remain only for compatibility; neither JS gameplay
entry sends the old lockstep input protocol.

`WorldState` is a named inventory over shared RollbackJournal. Plain fields are
journalled; owning objects have explicit adapters. ECL asynchronous contexts are
deep-copied with internal pointers rebound. Effects retain owned animation,
burst and vectors. All nine polymorphic attack states clone their concrete type
without RTTI. Input edge history, RNG, players, bullet/laser/shot/item pools,
background, dialogue, HUD, menus, scene services and session state are included.
Replay and motion recording rewind append positions, including chunk reallocation.

The optimized capture copies active enemy/visual slots and intercepts first
writes to dormant slots on spawn, reuse, clear and round reset. Other pools retain
the explicit full inventory. A dense mode remains in the development harness as
the A/B/A reference. The initial byte counter covers the journal and accounted owners; it excludes
subsequent first writes, attack clones and marker metadata. It is not total
snapshot size or allocated memory. The timing comparison includes forward
execution to account for first-write capture overhead.

Every logical tick executes CPU Draw semantics: TH09 Draw changes animation and
attack state. Intermediate recovery ticks suppress GPU submission and present.
Mutable text textures are journalled on first write and only changed textures
are re-uploaded after restoration. Recovery admits at most eight historical
steps before the final frontier, with an 8 ms check between recovery ticks and
10 ms check between outer ticks. These are start budgets, not preemption of a
single expensive tick. Replaying history does not consume the current tick: the
driver also advances one new frame after reaching the old frontier. The real
transport test checks wall time as well as state equality to catch slow simulation.

Sounds, music changes, pause audio and record/unlock events are released once,
from confirmed history. Wall clocks and already-committed records are outside
snapshots. Spectators receive confirmed input only. Destructive menus (retry,
world replacement, return to title) wait for exact, reconciled input. Pausing
itself can be corrected. Retry retains the transport epoch; completed sessions
continue servicing acknowledgements until close. Reopening a standalone room
clears the retired local session without destroying the local replay-save UI.

Both browser entries show actual correction and resimulation counters. Every
120 confirmed frames the existing compact gameplay hash is checked. That hash
is an online diagnostic, not the complete snapshot correctness oracle. The
in-process restoration oracle includes the full inventory and owned state;
separate tests compare framebuffers, independent worlds and serialized Replay.

## Validation and scope

The evidence record identifies the final harness/release WASM, source inputs,
reports and test scripts. Tests cover faulty input lanes, all fast input lost,
real local RTC, relay fallback, spectator convergence, once-only capture,
restoration with framebuffer equality, round reset, Replay chunk/motion rewind,
pause/resume, retry, match completion, local Replay save and reconnect.

The browser tests use desktop Chromium with SwiftShader and local retail assets.
Injected network delays are controlled test conditions; the RTC channels and
relay connections themselves are real. The work/capture measurements are not
physical-phone, Internet TURN, or audio-underflow acceptance. Audio clock progress
is recorded but does not establish subjective music quality. Long outages still
stall when the eight-frame prediction limit is reached.

No commit, push, canonical integration, upload or deployment was performed.

## Reproduce

From the experiment root, initialize the pinned submodule, provide local retail
assets/fonts/music using the usual TH09 setup, then run:

```powershell
$env:TH09_EMSDK='D:/workspace/eagler/th08-eagler/tools/emsdk'
$env:WASI_SDK_BIN='D:/workspace/eagler/toolchains/wasi-sdk-34.0-x86_64-windows/bin'
node th09_web/tests/multiplayer/run-tests.mjs
node th09_web/scripts/build-sdl.mjs
node th09_web/tests/multiplayer/world-browser.mjs
node th09_web/tests/multiplayer/world-browser.mjs --round-reset
node th09_web/tests/multiplayer/peer-browser.mjs
node th09_web/tests/multiplayer/transport-browser.mjs
node th09_web/tests/multiplayer/transport-browser.mjs --relay
node th09_web/tests/multiplayer/transport-browser.mjs --release
node th09_web/tests/browser/netplay-check.mjs
node th09_web/scripts/build-sdl.mjs --release
node th09_web/scripts/build-eagler.mjs --multiplayer
```

For the serial performance comparison, use `RUN_LABEL=dense-a-final` with
`world-browser.mjs --dense`, `RUN_LABEL=sparse-b-final` without `--dense`, then
`RUN_LABEL=dense-a2-final` with `--dense`. Keep WASM and harness unchanged across
those three runs. `CHARACTERS=2,4` selects another pair for a correctness run.
Set `EAGLER_RELAY_SOURCE` if the local Launcher relay has a different path.
