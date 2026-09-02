# TH06MP agent handoff

Copy/use this as the receiving agent's starting brief.

---

Continue work in:

`D:\workspace\eagler`

Target project:

`D:\workspace\eagler\th06-eagler`

Reference implementation / engineering upstream:

`D:\workspace\eagler\th07-eagler`

## Binding direction

We are starting a **new TH06 multiplayer line**.

The old TH06 multiplayer external upstream and **all previous TH06 multiplayer progress are rejected**. Do not continue them. In particular, do not build on the existing `TH_ENABLE_MULTIPLAYER` / `MultiplayerRuntime.*` implementation just because it is already present in the TH06 worktree.

Our current TH07MP implementation is the engineering upstream. It has already gone through real bug fixing, browser tests, rollback/loss stress, Restart stress, transport work and mobile-oriented performance optimization. **Read and reuse the actual current TH07 working-tree code and its focused tests. Do not merely read architecture summaries and then rewrite a similar system from memory.**

The only thing TH07 is not authoritative for is TH06-specific gameplay ownership/semantics. For those, inspect the current TH06 original/portable owners and adapt the proven TH07 architecture to them. Do not copy TH07 Cherry/Border/Spirit/Stage-4 constants or other game-specific rules into TH06.

## Documentation ownership

This TH06MP line has its own document collection and must stay separate from TH07MP documentation:

1. `th06-eagler/docs/th06-mp/README.md`
2. `th06-eagler/docs/th06-mp/TH07-CODE-MAP.md`
3. `th06-eagler/docs/th06-mp/PITFALLS.md`
4. `th06-eagler/docs/th06-mp/PORT-LEDGER.md`
5. this file, `th06-eagler/docs/th06-mp/AGENT-HANDOFF.md`

Keep updating `PORT-LEDGER.md` as the authoritative TH06MP progress ledger. Do not append TH06 progress to the TH07 sbrik ledger.

## First action: do not implement yet - audit and purge the old TH06 MP layer

The current TH06 worktree still physically contains the rejected old MP code, mixed into files that also contain valid Eagler/touch/replay/thprac fixes.

Therefore:

- never `git reset`, `git checkout`, `git restore` or `git clean` the project to remove old MP;
- never replace a mixed TH06 file wholesale from HEAD;
- enumerate every old `TH_ENABLE_MULTIPLAYER` / `MultiplayerRuntime` hunk and remove it surgically;
- preserve unrelated current TH06 Eagler work;
- record each removed owner in Phase A of `PORT-LEDGER.md`.

Known rejected markers include:

- `src/MultiplayerRuntime.cpp/.hpp`
- `scripts/test-multiplayer-source-contract.mjs`
- `TH_ENABLE_MULTIPLAYER`
- old `g_Player2`
- old P2 physical-input button widening
- old P2 ANM slots/offsets
- old `character2` / `shotType2`
- old MP-specific nearest-player / `provokedPlayer` implementation
- old MP-specific Player/Enemy/Bullet/Item/Bomb/GUI/Replay hooks

Old `build-*multiplayer*` directories are not reference implementations.

After the purge, ordinary TH06 must still retain its current touch, ReplayX, thprac, rendering, audio and Web behavior.

## Then read these actual TH07 source files completely

Start with production code, not prose:

### Generic core

- `th07-eagler/src/netplay/NetplayProtocol.cpp/.hpp`
- `th07-eagler/src/netplay/NetplayCore.cpp/.hpp`
- `th07-eagler/src/netplay/NetplaySession.cpp/.hpp`
- `th07-eagler/src/netplay/NetplayInput.cpp/.hpp`
- `th07-eagler/src/netplay/NetplaySideEffects.cpp/.hpp`
- `th07-eagler/src/netplay/RollbackJournal.cpp/.hpp`

These should be copied/reused as directly as practical rather than redesigned.

### Browser transport

- `th07-eagler/src/netplay/BrowserPeerTransport.cpp/.hpp`
- `th07-eagler/src/netplay/WebSocketTransport.cpp/.hpp`
- `th07-eagler/tools/netplay/lan-relay.cjs`

Do not lose fixes for:

- RTC packet buffering while route is undecided;
- reliable ordered control + unordered non-retransmitting input channels;
- whole-room WS fallback;
- signaling reconnect;
- 2.5s progress-aware handling of `disconnected`;
- immediate recovery for `failed`;
- bounded ICE restart attempts;
- O(1) JS receive dequeue via `receivedHead`, not per-packet `Array.shift()`.

### Production driver / rollback integration

Read the full:

- `th07-eagler/src/netplay/Th07LanStageProbe.cpp/.hpp`
- `th07-eagler/src/netplay/Th07RollbackState.cpp/.hpp`
- `th07-eagler/src/GameWindow.cpp`

The historical name `Th07LanStageProbe` is misleading: it contains the production browser netplay driver as well as test/probe paths.

When making TH06's version, preserve the actual behaviors:

- one physical input sample per logical frame;
- resend already-scheduled logical input while stalled, never resample device input;
- exact frame-zero input barrier;
- 32-frame redundancy;
- bounded prediction/rollback;
- edge actions not predicted held;
- direct-touch displacement only on the first missing frame;
- fresh session generation on Pause-menu Restart while keeping browser transport;
- session id changes across generations so stale packets are ignored;
- HELLO/READY reliable-control resend approximately every 15 driver ticks, not every frame;
- bounded post-stall catch-up;
- separate sender-frame time-sync telemetry and small scheduler-only pacing correction;
- stage-transition rollback-history invalidation.

### Gameplay/presentation/replay integration

Read:

- `th07-eagler/src/multiplayer/GameplaySession.cpp/.hpp`
- `th07-eagler/src/Player.cpp/.hpp`
- `th07-eagler/src/EffectManager.cpp`
- `th07-eagler/src/Controller.cpp/.hpp`
- `th07-eagler/src/ReplayManager.cpp`
- `th07-eagler/src/ReplayExtension.cpp/.hpp`
- `th07-eagler/src/Touch.cpp/.hpp`
- `th07-eagler/src/ResultScreen.cpp`
- `th07-eagler/src/AnmIdx.hpp`

Then map those contracts onto the corresponding TH06 owners.

## Read the tests as source contracts

At minimum:

- `th07-eagler/tests/netplay-core-test.cpp`
- `th07-eagler/tests/netplay-input-test.cpp`
- `th07-eagler/tests/netplay-rollback-feature-rebase-test.py`
- `th07-eagler/tests/netplay-2p-position-sync-smoke.py`
- `th07-eagler/tests/player-resources-feature-rebase-test.py`
- `th07-eagler/tests/replay-manager-feature-rebase-test.py`
- `th07-eagler/tests/result-screen-feature-rebase-test.py`
- `th07-eagler/tests/anm-assets-feature-rebase-test.py`

Port/adapt the tests together with production behavior. A TH07 bug that already has a contract should not be rediscovered manually in TH06.

## Critical pitfalls - do not repeat these

### 1. TH06 broad ANM interpolation already failed badly

Read `docs/history-session/17.md`.

A previous TH06 experiment generically interpolated ANM scale/rotation/UV/color for high-refresh presentation. It caused severe actual-game problems, was fully reverted, and the user confirmed the reverted state was OK.

Do not reintroduce broad generic ANM interpolation, including the old experimental `presentationPrevValid` / `PresentationLerp` / `PresentationUv` / `PresentationRotation` approach.

For multiplayer smoothing, keep simulation logical and make remote correction presentation-only.

### 2. Never make presentation a second simulation

TH07 remote smoothing is draw-only. Collision, targeting, ownership, RNG, rollback and Replay use authoritative logical coordinates.

Do not write a smoothed remote position back to `Player.position` or other deterministic state.

All attached visuals should consume the same draw-only correction as the ship.

### 3. Alpha interpolation flicker

TH07 teammate transparency flickered because only current ANM alpha was clamped while the renderer interpolated from `prevColor` / `prevColor2`.

If TH06 implements teammate fade, clamp and restore all four endpoints at draw time:

- `color`
- `prevColor`
- `color2`
- `prevColor2`

Do not solve this with gameplay-state hacks or a second position model.

### 4. Pause-menu Restart is a new rollback session

Do not simply set frame to zero. Retire current gameplay rollback state, increment generation, recreate core/session state and use a new session id over the existing WebRTC transport.

Test the **actual Pause menu Restart path**, not only a debug reset shortcut.

### 5. Reliable control flood can freeze the page

TH07 originally sent HELLO/READY every driver tick over reliable ordered control. A slow endpoint built hundreds of queued packets and Restart could make the PC page unresponsive.

Keep the current rate-limited control retry and the current O(1) browser receive queue.

### 6. Packet retry cannot reread touch

If frame zero/current boundary input is lost while simulation is stalled, resend the already scheduled logical sample. Do not call keyboard/controller/touch capture a second time for that same frame.

### 7. ANM slot collision is real

TH07 MP once overwrote hidden child slots of original ANM archives. Result: Replay title became a red block and Music Room song-title text became sequential player-texture strips.

Before allocating TH06 P2 resources, inspect actual TH06 archive/ANM child counts and all occupied slots. One top-level ANM filename can consume multiple slots.

### 8. Replay MP format is EAGX v1, not v2

Current rule:

- EAGX version remains 1;
- multiplayer is a v1 subtype/flag with extra metadata and synchronized player input lanes;
- local machine touch gesture events remain stored in addition to synchronized multiplayer lanes;
- rollback resimulation must not append duplicate Replay frames.

### 9. Touch Replay finger lifecycle

Only DOWN may create a playback finger. Orphan MOTION/UP must be ignored. Fresh run/Restart resets run-level touch recording/playback state.

Do not regress the current TH06 thprac Replay fix that stopped previous-run gestures and ghost cross indicators from leaking into the next attempt.

### 10. Do not rollback decorative state just to make it visually identical

The user explicitly chose to remove extra TH07 rollback coverage that only prevented occasional decorative 3D-background flashing because it added work and did not affect gameplay.

Keep rollback focused on deterministic gameplay state. Harmless decoration may briefly disagree if synchronizing it creates measurable burden.

### 11. Verify the device is running the build you think it is

TH07 debugging once appeared to show every fix ineffective because local 8136 still served stale Runtime HTML/JS/WASM.

Use a runtime build marker and compare actual served artifact hashes before rejecting a code fix from a device report.

## Supporting documents/history

After reading code, use these for context:

- `th07-eagler/docs/th06-th07-multiplayer-feature-rebase-notes.md`
- `th07-eagler/docs/th07-sbrik-feature-rebase-handoff.md`
- `th07-eagler/docs/th07-sbrik-feature-rebase-ledger.md`
- `th07-eagler/docs/th07-public-network-deploy-handoff.md`
- `docs/replay-porting-guide.md`
- `docs/replay-determinism-audit.md`
- `th06-eagler/PORTING.md`
- `docs/history-session/31.md`
- `docs/history-session/17.md`
- history 29/30 when public-network details are needed

Important history ordering caveat: `history-session/33.md` contains a Stage decorative rollback state from an earlier timestamp; a later checkpoint near the tail of `history-session/31.md` records the user's decision to remove that decorative rollback overhead. Use timestamp/turn ordering, not numeric archive filename alone, when records conflict.

## Recommended implementation order

1. Audit and surgically purge rejected old TH06 MP only.
2. Establish ordinary TH06 build/regression baseline.
3. Copy generic TH07 netplay core + tests.
4. Copy current browser transport + transport contracts.
5. Build TH06-specific multiplayer session/player slot foundation.
6. Build TH06 rollback-state owner inventory and sparse mutation hooks.
7. Integrate synchronized input into TH06's real fixed 60 Hz calc owner.
8. Make two local runtimes run identical logical inputs before adding difficult gameplay behavior.
9. Adapt Player/resources/Enemy/Bullet/Laser/Item/Bomb/GUI owner classes one by one.
10. Add real Restart/stage lifecycle handling.
11. Port EAGX v1 MP Replay while preserving local touch gestures.
12. Port presentation smoothing/attachments only after logical simulation is correct.
13. Audit TH06 ANM slots from actual assets before loading extra MP resources.
14. Wire Launcher/room integration using the existing TH07 browser/session infrastructure.
15. Run loss/jitter/reorder/slow-peer/Restart/WebRTC/fallback tests.
16. Only then ask for real PC+phone human acceptance; never claim human PASS from automated tests.

## Working-tree safety

The repositories are dirty by design. Do not commit, push or deploy unless the user explicitly asks. Do not clean unrelated files.

Record every TH06MP implementation decision and evidence in:

`th06-eagler/docs/th06-mp/PORT-LEDGER.md`

---

The key mindset is: **TH07MP is not merely an example. Its current code and tests are the practical upstream. TH06 is a careful game-owner adaptation of that tested system, after completely discarding the old TH06 MP line.**
