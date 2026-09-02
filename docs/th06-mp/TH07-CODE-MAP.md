# TH07MP code map for TH06 sister port

This is a code-reading map, not a design summary. The TH06 agent should open these files and compare implementations line by line before writing replacements.

## A. Generic netplay core - highest reuse priority

### `../th07-eagler/src/netplay/NetplayProtocol.hpp/.cpp`

Preserve unless TH06 wire identity requires a deliberate change:

- transport-neutral `FrameInput`
- buttons + analog mode + x/y + unlimited + touchUsed + touchBomb
- protocol packet separation: Input vs Session
- sequence/ACK/frame fields
- `senderFrame` separate from `latestFrame`
- `frameAdvantage` telemetry separate from resend-window state
- 32-frame application-level input redundancy
- reliable Session packet contract separated from fast input packet

Important: do not infer clock position from `latestFrame`; retransmission may make it old. This exact mistake was already designed out of TH07.

### `../th07-eagler/src/netplay/NetplayCore.hpp/.cpp`

Read and reuse the actual algorithms:

- frame-indexed local/remote histories
- delayed local scheduling
- last-known prediction
- predictable held-button mask vs edge-button neutral prediction
- direction prediction horizon
- correction classification
- earliest rollback request
- confirmed-through bookkeeping
- redundant input-packet construction and ACK window shrink
- rollback budget -> stall instead of unrecoverable speculative advance

Do not replace this with a new queue-based or socket-driven input model merely because TH06 is a new game port.

### `../th07-eagler/src/netplay/NetplaySession.hpp/.cpp`

Reuse the fixed session gate pattern:

- session id
- deterministic room seed
- game id / gameplay ABI fields
- player count and stable local slot
- HELLO then READY ordering
- contract mismatch failure before frame zero

The wire/session structure can remain generic while TH06 supplies its own game id/ABI and room descriptor.

### `../th07-eagler/src/netplay/NetplaySideEffects.*`

Reuse the resimulation-side-effect boundary. Suppress duplicated audio/visual side effects only during rollback replay, not during healthy predicted-forward simulation.

## B. Browser transport - copy the code, including bug fixes

### `../th07-eagler/src/netplay/BrowserPeerTransport.hpp/.cpp`

This is a mature browser-facing upstream. Do not rewrite it from memory.

Behavior that must survive the TH06 port:

- 2/3-peer capable WebRTC full-mesh structure
- two DataChannels per edge:
  - reliable ordered control
  - unordered `maxRetransmits:0` fast input
- emergency whole-room WebSocket relay fallback
- no mid-run route migration
- buffer RTC packets while local route is undecided
- signaling reconnect after RTC route selection
- ICE `disconnected` observes progress for about 2.5s before restart
- ICE `failed` restarts immediately
- max two consecutive ICE restart attempts
- stable lower-slot restart initiator rule
- browser path diagnostics without exposing peer IP addresses
- JS receive queue uses `receivedHead` O(1) dequeue and occasional `slice` compaction

Do not reintroduce per-packet `Array.shift()` for the gameplay receive queue. Under Restart/slow-peer backlog it became a main-thread O(n^2) amplifier and contributed to PC page-unresponsive behavior.

Also preserve the route-barrier race fix in `setupChannel`: an RTC packet arriving before local `route=rtc` processing must be queued, not dropped. The first reliable HELLO can otherwise vanish and deadlock frame zero.

### `../th07-eagler/src/netplay/WebSocketTransport.*`

Keep as transport fallback/test path using the same protocol/core, not a second gameplay implementation.

### `../th07-eagler/tools/netplay/lan-relay.cjs`

Read the live signaling/lobby/emergency-relay implementation when wiring Launcher/rooms. Reuse protocol behavior rather than inventing a TH06-only relay dialect.

Related deployment source:

- `../th07-eagler/tools/netplay/TURN.md`
- `../th07-eagler/tools/netplay/render-coturn-config.cjs`
- `../th07-eagler/docs/th07-public-network-deploy-handoff.md`

The server keeps TURN shared secrets server-side and sends only short-lived credentials to browsers.

## C. Production netplay driver - adapt deeply, do not simplify

### `../th07-eagler/src/netplay/Th07LanStageProbe.cpp/.hpp`

Despite the historical `Probe` name, this file contains the production TH07 browser netplay driver. Read the full file.

Key behavior to port into a TH06-specific driver such as `Th06LanStageDriver`:

### Session lifecycle

`RetireGameplaySession()` is a critical reference:

- clear rollback/session transient state
- reset sim frame, sequence and handshake clocks
- increment `g_SessionGeneration`
- keep the browser transport alive
- derive a distinct session id for the next run so late previous-run packets are ignored

A pause-menu Restart is a **new rollback gameplay session over the existing transport**, not continuation of old rollback history.

### Handshake flood control

`SendSessionControl()` limits HELLO/READY retransmission to 15 driver ticks (about 250ms) and keeps independent HELLO/READY clocks. Do not send reliable ordered control packets every 60 Hz driver tick.

This was discovered by throttling one browser: before the fix a slow endpoint accumulated roughly 206 initial and nearly 386 Restart control packets; after throttling it was around 13/26 in the comparable stress path.

### Input send/retry

Separate:

- capture/schedule one physical logical input for a frame
- resend the already scheduled frame on a stalled boundary

Never re-read keyboard/controller/touch when retrying the same logical frame. Direct-touch displacement is a one-frame delta; resampling/reapplying it changes gameplay.

The current driver resends a stalled scheduled local frame periodically so a lost frame zero or prediction-window boundary packet cannot deadlock merely because no later frame can be scheduled.

### Frame-zero barrier

Frame zero requires every peer's exact real first input before gameplay advances. Later frames may predict within rollback budget.

### Prediction semantics

Reuse TH07 classification rather than generic hold-everything prediction:

- held direction / Shoot / Focus / Skip may predict
- Bomb / Menu / touch-Bomb predict released
- direct-touch displacement may repeat for at most the first missing frame, then displacement must become zero while held flags can remain

### Post-stall catch-up

See `GameWindow.cpp`: a stalled netplay tick does not consume simulation time. After packets return, catch-up is bounded (currently maximum 6 fixed ticks before a draw), preventing both permanent slow-time and unbounded burst work.

### Time synchronization

`RecordTimeSyncSample()` is scheduler/presentation-to-simulation pacing only:

- per-peer rolling sample windows
- reject large outliers
- do not mix unrelated 3P links into one raw sample stream
- derive largest local lead recommendation
- deadband around zero
- gently adjust wall-clock simulation interval scale
- current scale is bounded around `0.98..1.02`
- never modify deterministic 60 Hz game rules, inputs or RNG to 'catch up'

Do not repair clock skew using periodic hard simulation stalls, permanent extra prediction lead, or neutralizing held input after arbitrary timeout.

### Transport failure

A missing required peer must become an explicit room failure. Do not leave a frozen canvas indefinitely.

## D. Rollback storage - reuse journal, replace TH07 owner list with TH06 owner list

### `../th07-eagler/src/netplay/RollbackJournal.hpp/.cpp`

Reuse this implementation first:

- bounded first-write-wins undo journal
- `Touch()` before mutation
- duplicate touches ignored
- partial overlaps rejected
- ordered address map for near O(log n) overlap lookup
- retained history byte-buffer capacity across eviction
- checkpoint can span two consecutive logical frames

TH07 previously suffered dense-scene slowdown because overlap tracking and repeated allocation scaled badly. The current journal is the result of that optimization work.

### `../th07-eagler/src/netplay/Th07RollbackState.hpp/.cpp`

Do **not** copy the TH07 object list blindly. Use it as the implementation pattern for a new TH06 state adapter.

Study:

- fixed/global owner capture
- sparse active object mutation hooks
- Player/Enemy/Bullet/Laser/Item/Effect/PlayerBullet/BombInfo touch helpers
- Supervisor simulation fields vs platform/render fields
- input-history fields included in rollback
- multiplayer sidecars included with all active players
- stage-transition history invalidation
- failure handling for missing old-stage checkpoints vs journal corruption

Build a TH06 mutation inventory from TH06 source. Any mutable state that can affect future simulation must either be journaled/restored or proven reconstructible.

### Performance rule

Do not resurrect whole-manager/full-world snapshots. TH07 already measured this direction as far too expensive for Web/mobile. Sparse first-write storage plus current two-logical-frame checkpoint cadence is the reference baseline.

### Decorative-state rule

History 31 records a late user decision: rollback capture added only to prevent an occasional decorative 3D background flash was removed because it added work without gameplay value. TH06 should likewise not synchronize decorative-only VM/Stage presentation state unless a cheap presentation-only fix exists or the user explicitly asks.

## E. Fixed-tick integration and presentation

### `../th07-eagler/src/GameWindow.cpp`

Read the actual netplay path around fixed 60 Hz simulation:

- network arrival never directly calls game calc chain
- game loop remains simulation owner
- `SimulationIntervalScale()` only changes scheduler interval
- stalled tick does not consume accumulator simulation time
- bounded catch-up after stall
- presentation continues while waiting where safe

Mirror this in TH06's SDL3 callback/fixed-tick owner instead of adding a second simulation loop.

### `../th07-eagler/src/Player.cpp`

Read these specific areas carefully:

- raw-touch local-slot ownership helpers
- per-player synchronized `FrameInput` consumers
- logical `prevPosition` -> current interpolation
- `RemotePresentationState`
- `PresentRemotePlayer()` bounded draw-only smoothing
- `g_RemoteDrawOffsets` consumed by ship attachments
- proximity transparency and `ClampVmAlpha()`
- local-player locator implementation as an example of strictly playfield-only presentation

Presentation smoothing must never write its smoothed coordinate back into simulation state or rollback state.

### `../th07-eagler/src/EffectManager.cpp`

Study how player-attached effects consume the same presentation offset/alpha handling as the owning ship.

## F. Multiplayer session/gameplay layer

### `../th07-eagler/src/multiplayer/GameplaySession.hpp/.cpp`

Reuse the state-ownership shape, but create TH06-specific semantics.

Useful reusable ideas:

- stable local-player slot
- fixed participant array
- active / temporarily absent / permanently departed distinctions
- per-player loadout in a room/session descriptor
- local presentation preferences not part of deterministic compatibility state
- runtime-only content conveniences must not mutate local saves

Do not copy TH07-specific character count, Shot ranges, Spirit/revival, Cherry/Border or Stage 4 rules.

## G. Replay - use current TH07 implementation, not old TH06 MP replay

### `../th07-eagler/src/ReplayExtension.cpp/.hpp`

Current multiplayer format contract:

- EAGX `VERSION = 1`
- `DETERMINISM_ABI = 1` for container semantics in the current file
- multiplayer is a **v1 type/flag**, `FLAG_MULTIPLAYER_INPUT = 32`
- normal v1 header remains 96 bytes
- MP v1 metadata header is extended to 160 bytes
- synchronized per-player `FrameInput` lanes are appended without replacing original/local gesture data

Do not rename multiplayer replay to EAGX v2.

### `../th07-eagler/src/ReplayManager.cpp`

Study when multiplayer frames are committed:

- record synchronized simulation input, not speculative duplicate execution
- rollback resimulation must not append duplicate replay frames
- local touch gesture stream remains recorded in addition to synchronized multiplayer lanes
- playback applies per-player input overrides before simulation consumes them

### `../th07-eagler/src/Touch.cpp/.hpp`

Mandatory replay-touch state machine:

- DOWN may create a replay finger
- MOTION requires an existing active finger
- UP requires an existing active finger
- orphan MOTION/UP ignored
- fresh run/restart clears live touch + recording finger-id mapping + playback touch state
- ordinary stage progression must not clear a legitimately held finger merely because the stage changed

This fixed the 'many ghost + crosses / previous run gesture' family of bugs.

### `../docs/replay-porting-guide.md` and `../docs/replay-determinism-audit.md`

Read both before changing TH06 replay layout/ownership. Original TH06 replay remains the base owner; EAGX only carries what original replay cannot represent.

## H. Results/persistence

### `../th07-eagler/src/ResultScreen.cpp`

Study the separation between:

- showing/saving supported multiplayer replay
- preventing multiplayer session results from contaminating ordinary single-player score persistence

A historical guard skipped the MP Replay save prompt even after MP Replay support existed; it was removed. Do not copy old 'multiplayer means no replay' assumptions into TH06.

## I. ANM/resources - audit real files before allocating sidecar slots

### `../th07-eagler/src/AnmIdx.hpp` and `tests/anm-assets-feature-rebase-test.py`

TH07 had a real menu corruption bug because new multiplayer ANM slots overlapped child slots implicitly occupied by multi-entry original ANM archives:

- title resources occupied a consecutive range
- Music Room dynamic title texture occupied another child slot
- old MP slots overwrote them, producing red blocks and strips of player texture

For TH06, inspect actual DAT/ANM child counts and slot ranges before choosing any new player/face/HUD indices. Never assume one filename consumes one ANM slot.

## J. Focused tests are upstream too

Read and port/adapt these tests, not just production source:

- `../th07-eagler/tests/netplay-core-test.cpp`
- `../th07-eagler/tests/netplay-input-test.cpp`
- `../th07-eagler/tests/netplay-rollback-feature-rebase-test.py`
- `../th07-eagler/tests/netplay-2p-position-sync-smoke.py`
- `../th07-eagler/tests/player-resources-feature-rebase-test.py`
- `../th07-eagler/tests/replay-manager-feature-rebase-test.py`
- `../th07-eagler/tests/result-screen-feature-rebase-test.py`
- `../th07-eagler/tests/anm-assets-feature-rebase-test.py`
- browser/WebRTC smoke harness files referenced from `th07-public-network-deploy-handoff.md`

Tests should be adapted owner-by-owner so TH06 does not regress a bug TH07 already caught.
