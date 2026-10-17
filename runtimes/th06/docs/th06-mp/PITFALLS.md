# TH06MP pitfalls inherited from TH07MP and TH06 Eagler history

This file is intentionally opinionated. These are mistakes already paid for in development time or real-device regressions. A TH06 port should begin by preserving the fixes, not rediscovering them.

## 1. Do not build on the rejected old TH06 multiplayer layer

The current TH06 worktree still contains old multiplayer-specific changes. They are not valuable prior progress for the new line.

Wrong:

- keep `MultiplayerRuntime.*` and bolt rollback/networking onto it
- keep old P2 input bit widening because it already compiles
- preserve old nearest-player/provoked-player behavior as gameplay truth
- use old `build-web-multiplayer*` outputs as a baseline
- let old LCConnect behavior decide new product rules

Correct:

- surgically remove old multiplayer-only hunks while preserving unrelated Eagler/touch/replay/thprac changes
- then port our current TH07MP architecture/code into new `src/netplay` + `src/multiplayer` owners
- adapt gameplay to TH06 original source ownership

## 2. Interpolation - the most important rendering warning

### 2.1 Do not reintroduce broad TH06 ANM interpolation

History `docs/history-session/17.md` records a real user-verified failure:

A TH06 experiment added generic high-refresh interpolation across ANM continuous attributes such as scale, rotation, UV scroll and color. It caused severe problems in actual gameplay. The experiment was fully reverted, and the user explicitly confirmed the reverted version was OK.

Do not reintroduce the old experimental concepts such as:

- `presentationPrevValid`
- generic `PresentationLerp`
- generic `PresentationUv`
- generic `PresentationRotation`
- broad common-Draw interpolation of scale/rotation/UV/color

If a specific new multiplayer visual needs smoothing, solve that specific owner with a complete previous/current lifecycle. Do not 'upgrade TH06 ANM interpolation' as part of multiplayer work.

### 2.2 Simulation position and presentation position are different domains

Authoritative gameplay uses logical positions only:

- collision
- targeting
- ownership
- RNG-dependent decisions
- rollback snapshots/journal
- replay simulation

High-refresh drawing may use:

`prevLogical.Lerp(currentLogical, renderAlpha)`

Remote rollback correction smoothing may add a bounded **draw-only** offset after ordinary interpolation.

Never write smoothed/presentation coordinates back into Player/Enemy/Item/Effect simulation state. That creates a second simulation trajectory and can poison determinism.

### 2.3 Publish previous endpoints at the logical tick, not during draw

Each fixed 60 Hz update must establish previous/current logical endpoints before mutation. Render can interpolate them many times. Do not update `prevPosition` per presentation frame.

### 2.4 All attached visuals must share one presentation correction

If the remote ship is smoothed, its dependent presentation must consume the same current-frame draw offset:

- Options
- hitbox indicator
- player labels/prompts
- player-attached effects
- other ship-attached sprites

Otherwise the ship looks smooth while its attachments jitter or separate from it.

### 2.5 Temporary VM transforms need previous-state symmetry

If a draw path temporarily changes a VM's scale/color/etc. and that field participates in interpolation, either:

- change both current and matching previous field, then restore both, or
- draw a local copy

Changing only the current field creates a per-presentation interpolation pulse.

### 2.6 Alpha flicker bug - clamp all ANM color endpoints

TH07 teammate transparency originally clamped only current alpha. `AnmManager::SetRenderStateForVm` interpolates `prevColor -> color` and `prevColor2 -> color2` with render alpha, so each logical frame visually swept from opaque previous alpha back down to the desired transparent alpha.

The correct draw-only pattern is:

- save `color`, `prevColor`, `color2`, `prevColor2`
- clamp alpha on all four
- draw
- restore all four

This is why TH07 `Player.cpp::ClampVmAlpha()` is a mandatory reference. If TH06 later adds teammate fade, do not repeat the current-only-alpha mistake.

## 3. Input/touch ownership

### 3.1 Only the local stable slot samples physical host input

Keyboard/controller/global touch state is endpoint-local. In multiplayer, only the player slot owned by that endpoint may read raw device state.

All gameplay consumers for every slot should read synchronized `FrameInput` lanes once netplay input override is active.

A gameplay-only multiplayer build is a useful regression mode because it exposes accidental global touch leakage that a fully configured network override can hide.

### 3.2 Direct touch is not a held analog axis

Direct-touch movement is a one-logical-frame displacement/sample. For a missing remote frame:

- first missing frame may reuse the last displacement if the accepted prediction model says so
- second and later missing frames must zero the displacement
- held flags/buttons may remain predicted separately

Do not keep dragging a remote player indefinitely during packet loss.

### 3.3 Packet retry must never resample physical input

At frame zero or a prediction-window stall, there may be no future frame to carry the lost sample in the redundant tail. Resend the **already scheduled logical input**.

Never call the device sampler again for the same logical frame. Doing so can duplicate a touch delta, Bomb edge, menu edge or other one-frame action.

## 4. Frame zero and room startup

Frame zero is an exact-input barrier. Every peer must have the real first logical input before gameplay begins.

Do not allow prediction at frame zero to 'make startup faster'. A bad first speculative state contaminates every later correction and also interacts badly with session-route races.

WebRTC route release is asynchronous across browser tasks. If a DataChannel packet arrives while local route is still undecided, buffer it. Dropping the first HELLO because `route` has not been processed yet can permanently deadlock startup.

## 5. Restart / Retry lifecycle

### 5.1 A game Restart is not merely frame = 0

The browser connection may persist, but the rollback gameplay session must be retired and recreated.

Use a generation counter and derive a new logical session id. Reset:

- rollback history
- core/session gate
- sim frame
- packet sequence state
- handshake retry clocks
- transient input/presentation state that belongs to a run

Late packets from the previous run must fail the new session-id contract instead of entering the new run.

### 5.2 Pause-menu R Restart is separate from Continue

TH07 initially had a false sense of Restart coverage because an automated smoke used a shortcut/reset edge rather than the exact user path through the Pause menu state machine.

TH06 originally had a different vanilla UI, but TH06MP now deliberately adopts
TH07's synchronized Pause-menu quick-Restart policy:

- vanilla TH06 Pause only provides Resume/Quit; there is no Pause-menu `R` Restart
- the MP binary still builds with thprac OFF; its Pause-menu `R` support is owned by the multiplayer path, not by thprac
- `R` has no quick-Restart meaning during normal gameplay; it is handled only while the shared Pause menu owns input
- any participant may press `R`; the confirmed combined multiplayer input makes every peer enter the same Restart close state
- after the 20-frame Pause close animation, Story starts a fresh attempt at Stage 1 while Extra/Practice restart the current stage
- Restart retires the old rollback generation and reuses the established transport for a fresh generation from frame zero
- TH06MP deliberately forbids Continue even though vanilla TH06 has a Retry/Continue UI
- only a complete team wipe starts the deterministic 180-logical-frame terminal grace; when it expires, multiplayer requests `SUPERVISOR_STATE_RESULTSCREEN_FROMGAME` directly
- the ordinary Retry menu remains only as a defensive fallback: if any future path sets `isInRetryMenu` during multiplayer, its update immediately clears the flag and goes to Result instead of exposing Yes/No

Current multiplayer contracts (ABI6):

`team wipe -> 180 logical-frame grace -> direct Result handoff -> retire rollback gameplay generation`

`Pause -> R -> 20-frame menu close -> fresh GameManager attempt -> retire old rollback generation -> synchronize new generation from frame zero`

There is no multiplayer Retry-Yes resource-reset/revive path. The old helper and its reset diagnostics were removed so dead Continue code cannot be mistaken for supported behavior.

### 5.3 Do not flood reliable control during slow rebuild

TH07 sent HELLO/READY every driver tick over a reliable ordered DataChannel. A slow browser rebuilding GameManager accumulated hundreds of control messages; combined with JS queue behavior this could make the PC page unresponsive.

Current rule: independent HELLO and READY resend clocks, roughly 15 driver ticks / 250ms cadence. Phase transition may still send immediately.

## 6. Browser receive queues

Do not use `Array.shift()` once per incoming packet on a potentially large JavaScript queue. It repeatedly moves the remaining array and can become O(n^2) during a backlog.

TH07 uses:

- `receivedHead` index
- set consumed slot to undefined
- increment head O(1)
- occasionally compact with `slice`

Port this implementation, not the old queue idiom.

### 6.1 Spectator queues must be finite too

Spectator playback is not part of the player rollback barrier, so do not solve a slow spectator by adding prediction, rollback, snapshots or player-style resync.

There are two distinct backlog owners:

- frame-zero history retained for a spectator admitted at run start but not yet connected;
- live WebSocket / JavaScript receive backlog after that spectator has connected.

Current shared-relay policy:

- keep frame-zero history only while at least one admitted spectator is still unclaimed;
- expire an unclaimed admission after 60 seconds by default (`TH07_SPECTATOR_CONNECT_GRACE_MS`);
- clear retained history as soon as no unclaimed spectator remains;
- cap an already-connected spectator's relay send backlog at about 1 MiB by default;
- cap each spectator Runtime's unconsumed receive queue at 16384 packets;
- if either live backlog cap is exceeded, end that spectator instead of growing memory without bound.

The limits are intentionally much larger than ordinary catch-up needs. They protect abandoned/backgrounded spectators without adding reconnect/snapshot complexity to the first spectator implementation.

## 7. WebRTC recovery

Do not restart ICE immediately for every transient `disconnected` event.

Current TH07 behavior:

- `failed` -> immediate recovery/restart
- `disconnected` -> observe about 2.5 seconds
- if selected-pair traffic or confirmed frame is still progressing, keep observing
- if truly stalled, request ICE restart
- retain/reopen signaling for restart
- no more than two consecutive restart attempts

This avoids false restarts during short mobile network/browser blips.

## 8. Prediction and time synchronization are separate

Prediction answers: what logical input do we use while a remote frame is missing?

Time synchronization answers: is this endpoint's wall-clock simulation schedule drifting ahead/behind its peers?

Do not conflate them.

In particular:

- resend-window `latestFrame` may be intentionally old
- carry sender simulation frame separately
- estimate skew over a rolling window
- reject spikes/outliers
- apply a small scheduler interval correction only
- current TH07 bound is roughly 0.98..1.02 around nominal cadence

Never change gameplay speed, RNG, collision, timers or input semantics to synchronize clocks.

## 9. Network stall recovery must be bounded

If a netplay tick stalls for input, it has not consumed fixed simulation time. Keep only a bounded accumulator backlog and after recovery run a bounded number of catch-up ticks before drawing.

TH07 currently caps this path at 6 fixed ticks per presentation iteration.

Wrong alternatives already rejected:

- permanent large prediction lead
- normal-mode hard frame gates forever after a stall
- unbounded catch-up burst
- forcing held directions neutral after an arbitrary short network timeout

## 10. Rollback memory/performance

### 10.1 No whole-world snapshot ring

TH07/TH08 investigations already showed raw manager/world snapshots are too large and slow for browser/mobile use.

Use the current sparse first-write undo journal architecture and measure actual capture/restore/resimulation cost.

### 10.2 Avoid quadratic first-write tracking

Dense bullet scenes exposed overlap tracking cost. Current TH07 `RollbackJournal` uses ordered address lookup (`std::map` + predecessor/successor checks) rather than scanning every previous touched block.

Keep evicted history slot buffer capacity for reuse instead of repeatedly growing multi-megabyte vectors.

### 10.3 Two-logical-frame checkpoint cadence

Current TH07 performance optimization keeps the first-write set across two consecutive logical frames. If rollback targets the second frame, restore the checkpoint start and replay at most one extra logical tick.

This reduced frequent snapshot work while preserving recoverability. Port this behavior before inventing another 'skip frames' optimization.

### 10.4 Gameplay state vs decoration

Do not pay rollback cost for decorative-only state merely to eliminate a rare harmless flash. The user explicitly chose to remove TH07 Stage decorative VM/object rollback additions because they created extra work and the background desync was not gameplay-relevant.

## 11. Stage transitions

A stage change invalidates old-stage rollback history. Do not restore a snapshot whose pointees/resources belong to a previous stage.

Treat an expected unavailable old-stage correction differently from journal corruption:

- stale impossible correction across a discarded stage boundary may be abandoned/reported without freezing the room
- capture corruption or failed resimulation remains fatal

TH06 stage lifecycle differs from TH07, so map the boundary from TH06 source rather than copying stage ids.

## 12. Replay and touch replay

### 12.1 Multiplayer replay remains EAGX v1

Do not call it EAGX v2.

The current accepted model is one EAGX v1 subtype/flag with additional multiplayer metadata and synchronized input lanes.

### 12.2 Record committed synchronized multiplayer inputs

Do not append speculative execution twice when rollback re-simulates a frame. Replay should represent the committed logical sequence.

### 12.3 Keep the local touch gesture lane

Multiplayer synchronized `FrameInput` lanes do **not** replace the local endpoint's touch event/gesture lane. The user explicitly wants local touch gestures retained in multiplayer replay.

### 12.4 Replay finger lifecycle

Only DOWN creates a playback finger. MOTION/UP must find an already-active finger; orphan events are ignored.

Fresh run / same-stage Restart clears run-level touch recording/playback state. Ordinary stage progression must not arbitrarily break a still-held finger.

This addresses the previous-run gesture leak / too-many-crosses bug family.

## 13. Resource slot collisions

Do not allocate multiplayer ANM slots by looking only at top-level filenames.

TH07 proved that one ANM archive can consume multiple consecutive file slots for child entries/dynamic textures. Multiplayer slots overlapped Replay-title/Music-Room children and caused:

- red block instead of Replay title
- song titles replaced by consecutive strips of player texture

Before TH06 adds P2 resources, inspect actual DAT/PBG/ANM child structure and enumerate occupied ranges. Add a focused contract test for the allocation.

## 14. Served runtime verification

A user can report 'nothing changed' while the local/public server is still serving stale HTML/JS/WASM.

TH07 lost time to exactly this. Before rejecting a fix based on device behavior:

- expose/read a runtime build marker
- hash the current build artifact
- hash the exact file served by the test server
- confirm Launcher/service-worker cache path is serving that artifact

Do not debug source that the device is not actually running.

## 15. Hash boundary

The project may use local diagnostic state hashes to locate desync if already authorized by the existing architecture, but do not add new hash-driven behavior without asking the user.

Forbidden without new approval:

- room admission/rejection by content hash
- compatibility gate by executable/resource hash
- automatic state repair/resync based on hash
- hidden mismatch refusal

## 16. Preserve verified TH06 Eagler fixes

Multiplayer work must not regress stable TH06 platform work, especially:

- float common draw/subpixel presentation
- runtime sprite edge-extrusion atlas against filtering bleed
- streaming VBO replacement-storage path on WebGL/mobile
- high-refresh simulation/presentation separation
- time-stop presentation endpoint fixes
- current audio path
- touch sensitivity/direct-touch behavior
- ReplayX/Replay determinism work
- thprac state-machine isolation
- current touch Replay restart/finger fixes

If a multiplayer solution appears to require rewriting one of these, first prove why the existing owner contract cannot be retained.

## 17. Do not use wall-clock input events as determinism truth

During strict TH06/TH07 rollback testing on 2026-08-29, browser `KeyboardEvent` input driven by wall-clock timers produced convincing but false desync reports:

- TH06 appeared to retain an 8-pixel movement error after rollback
- TH07 appeared to diverge in some 3P dynamic-input runs
- adding diagnostic work changed when the apparent failure occurred

The reason is that a wall-clock event can land on a different logical simulation frame depending on browser scheduling, catch-up cadence, logging cost and render timing. If two endpoints are fed "the same" timer schedule, that does **not** mean they received the same logical-frame input sequence.

Strict determinism tests must instead generate input from logical frame number, for example `LocalScript(player, frame)`, while leaving the real production transport/prediction/rollback path intact.

The corrected scripted tests converged under:

- 2P/3P
- RTC and forced WebSocket fallback
- 120 ms fixed latency
- 55 ms latency + 10 ms jitter
- heavy repeated rollback/resimulation
- deterministic packet loss

Wall-clock input is still useful as a human-like stress/fuzz test, but a failure there must be reproduced with a deterministic logical input sequence before declaring a simulation desync.

## 18. `Chain::RunCalcChain()` does not return `ChainCallbackResult`

This caused a real shared TH06/TH07 driver bug on 2026-08-29.

Individual chain callbacks return `ChainCallbackResult`, where enum values 4/5 mean EXIT_SUCCESS/EXIT_ERROR. But `Chain::RunCalcChain()` consumes those enum values internally and returns a different integer contract:

- `0` -> game exit success
- `-1` -> game exit error
- any positive value -> number of chain jobs updated this tick

Therefore normal frames can legitimately return `4`, `5`, or larger values. Never write:

```cpp
if (result == CHAIN_CALLBACK_RESULT_EXIT_GAME_SUCCESS ||
    result == CHAIN_CALLBACK_RESULT_EXIT_GAME_ERROR)
```

against the result of `RunCalcChain()` or a wrapper that preserves its integer contract.

Doing so made a normal frame with five updated jobs look like a fatal exit; conversely the old netplay driver also returned enum value 5 upward, while `GameWindow` only recognized `-1`, allowing a logged `FAIL` to continue running and even produce a false automated PASS.

Current rule: driver/window boundaries use only `0/-1/positive-count`. Static contracts in both TH06 and TH07 protect this.
