# TH08 session admission and rewindable owners

Status (2026-09-24): the production 2P/3P frame-zero gate, zero-added-delay
input, bounded automatic world correction and confirmed audio/file output are
implemented. Real BrowserPeerTransport WebRTC full mesh and forced WebSocket
Relay both pass 2P and 3P gameplay, native pause/restart and new-generation
handshake tests. The sections below preserve earlier implementation evidence;
the latest production boundary and its exact evidence appear at the end.
This is not a Launcher capability declaration or human-device acceptance.

## Boundaries

The v1 descriptor remains the existing local cooperation fixture. The v2
descriptor adds a nonzero 64-bit session ID and uses eagler-common SessionGate
and RollbackCore. The all-seat HELLO/READY contract includes loadouts, seed,
difficulty and TH08 gameplay ABI; an old generation cannot populate a new run.

BrowserRuntime samples each local frame once, then passes decoded all-seat
inputs through the original GameplayScene. P1 owns menus and every seat may
request the shared pause. A logical frame commits only after native Update
**and semantic Draw**. Missing input or an incomplete barrier cannot repeatedly
execute Draw while waiting. Direct local-fixture input injection is rejected
once a v2 session is configured.

NetplayRuntime defaults to WorldReady=false. RollbackDriver changes it only
after the stable native stage and its world/texture/output owners are bound.
Loading and unregistered scene states remain exact-only. A prediction window
is bounded by the oldest unconfirmed frame, including holes followed by newer
exact input; no needed state is overwritten to make progress.

The dependency is now common 5e14ad8, fast-forwarded from the title's earlier
8316c4f. Atomic input-packet application, simulation-frontier rewind, session
health/retransmission and retirement ACKs use the shared implementation.
Gameplay contract revision 0x08000002 rejects earlier exact-only experimental
peers. Analog/touch samples are rejected until their authoritative simulation
owner is implemented, rather than silently dropped.

## Lifetime-safe owner modules

EnemyJournal wraps the shared RollbackJournal. It captures live Enemy/EclVm
data, contexts, population scalars and mutable ECL bytecode. Native ECL may
write literal arguments, so its loaded buffer is not treated as immutable.
MP asynchronous contexts use shared ownership: snapshots pin an old context
through native self-replacement/return, while pointer ownership words are
never byte-copied. Reused VM slots and newly allocated slots are restored to
their recorded existence. Stage teardown must clear the journal after a
confirmed fence; a needed checkpoint is never silently overwritten.

MP ScreenEffects uses 128 stable slots with embedded native chain callbacks.
Removal and reuse no longer free addresses referenced by checkpoints. The
capture includes ordered callback links and scalar state. Exhaustion fails
closed, and an unowned heap callback is rejected. Ordinary single-player keeps
its original allocation/lifetime implementation. Neither journal by itself
restores the entire game world.

PoolsJournal captures active native bullet/laser/item/effect objects and small
pool metadata, with first-write capture in the real native allocators for
new or reused slots. Linked lists, per-seat item ownership and directed gifts
are included. The independent fixture oracle hashes whole pools, including
inactive slots, so a missed first-write cannot hide behind the sparse capture.

MP effect vertex arrays are owned by their stable effect slots for the stage.
Native deletion detaches the effect's raw pointer without freeing a buffer
referenced by earlier checkpoints. Reuse captures both state and 258-vertex
payload before initialization. Destructive template/item/effect resets reject
live history instead of freeing borrowed state. This is lifetime correctness,
not a claim that the full world adapter or prediction is enabled.

## Evidence

All paths below are under artifacts/multiplayer-tests/.

| Evidence | Actual scope |
| --- | --- |
| admission-963f09c4-9326-4e8b-b1e7-48bf5dbe0add | Production 2P and 3P, 360 native Update/Draw ticks each, frame-zero gate, per-seat movement, missing-input stall. Encoded packets are carried by the test harness, not RTC/Relay. |
| enemy-journal-40dfb09d-a148-40e9-a691-df16ad09a123 | Real ECL execution with async self-replacement/return, literal bytecode writes, slot reuse, undo/re-execution, confirmed pin release, history bound. Real screen callback deletion/reuse, chain order/timer restoration, pool overflow rejection. |
| cooperation-a3eb8688-f087-4540-8c45-30e5fb479eb5 | All seven existing native cooperation/gift/wipe/retry cases remain passing. |

The subsequent pool-owner slice additionally passes:

| Evidence | Actual scope |
| --- | --- |
| enemy-journal-ce3c3145-6d66-4a00-b4c3-1f9ea0073ff0 | Existing ECL/screen probes plus real bullet, laser, item, targeted gift, fixed effect replacement, vertex payload restore and rerun. The pool fixture records 63,223 bytes and compares complete pool/RNG contents. |
| cooperation-2c46d246-c1e4-46f0-bc26-52286de2e402 | All seven native cooperation cases after effect-geometry ownership changes. |
| admission-5811e343-3649-46b6-b345-311ae02853ac | Rebuilt production 2P/3P admission, independent input and missing-input Update/Draw stall. |

Pool-slice production MP WASM:
`c711e1f7d19793f64b6f43029a00e51846bdcd3960c72596eed646ba48943963`.

Pool-slice diagnostic MP WASM:
`59b73aac62e7e92e2fda682af20843867570478ab346f01d3af73420db63fa6d`.

Rebuilt ordinary WASM remains byte-for-byte equal to the pre-slice baseline:
`0d00a84ef6b214d43f2f365a6ffaa8bc6030a89c56b7d53868f3b9295b4cea3f`.
This is a binary identity check, not a new full golden Replay run.

Build logs: build-netplay-production.log, build-netplay-ordinary.log and
build-owners.log for the session/ECL slice; build-pool-owners.log,
build-pools-production.log and build-pools-ordinary.log for the pool slice.
Unit entry points are check-multiplayer-netplay.mjs and
check-multiplayer-cooperation.mjs under portable/. Build-isolation tests check
ordinary exclusion and production/fixture export boundaries.

The browser server requires TH08_MP_DATA to name a local retail th08.dat.
The old games/web-content/th08 path now contains derived music assets only;
do not bundle/copy retail data into source control to work around this.

## Stable-stage world slice (2026-09-24)

WorldJournal composes the native player, shared economy/RNG, input-edge and
cooperation state with EnemyJournal, PoolsJournal and ResourcesJournal. The
inventory also covers GUI/dialogue/STD, mutable ANM scripts and sprite metadata,
semantic Draw state, supervisor fields and the native recording's owned vectors.
Recording snapshots use deep copies, not byte copies of ownership words.
Eight live checkpoints are allowed; a required checkpoint cannot be silently
overwritten. A stable-stage graph and scene fence prevent resource teardown
while its state is still referenced. Presentation sidecars are rebased after
undo instead of being treated as authoritative inputs.

The simultaneous-bomb test exposed a missing SpellFlow attachment in
GameplayScene::load. The MP-only fix attaches its actual resource owner and
calculation/draw jobs before gameplay. It does not fabricate spell state or
silence failed announcements, and does not change the ordinary build.

| Evidence under artifacts/multiplayer-tests/ | Actual scope |
| --- | --- |
| world-journal-1393a209-92b9-4e53-82df-c94bf3cc1fce | Eight fresh 2P/3P scenarios: shooting, focused bombs, unfocused bombs and native death. Each checks six Update/semantic-Draw frames, undo, exact re-execution, partial retirement and a non-destructive scene fence, then repeats at later lifecycle positions. Bomb cases assert actual activation and resource consumption. |
| world-journal-86712b49-6540-46ef-b785-2e5b56ea554a | Twenty focused/unfocused bomb scenarios covering all twelve loadouts in 2P pairs and 3P groups, including two later-lifecycle probes per scenario. All passed. |
| enemy-journal-2dbdbc7e-ecdb-4fbe-b89c-9d374ccf277e | ECL, screen callbacks, pools/geometry, ANM resource ownership and stable-stage world probes all pass after the SpellFlow fix. |
| cooperation-7ae76151-d070-48b1-9b6a-df8702b7d828 | All seven native rescue, gift, pool-capacity and wipe/retry regression cases pass. |
| admission-fcefbb98-96b0-4f73-aa0a-49e3dab6ecd5 | Production 2P/3P admission, per-seat movement and missing-input Update/Draw stall remain passing. |

Current world-slice production MP WASM:
`45379e692454c708d74badeb7a9765675c12aaf6bd2c5289f55add8b6007b298`.
Diagnostic MP WASM:
`b1f69b8109cdfa8cc791dffbdce119d4ff289cc2f0a812cc2deb7527bf59cd71`.
The rebuilt ordinary WASM is still exactly the baseline hash above.
Logs: build-world-matrix.log, build-world-production.log,
build-world-ordinary.log. Commands:
`node portable/multiplayer/check-admission.mjs --world-journal` and
`node portable/multiplayer/check-admission.mjs --world-loadouts`.

Diagnostic fixture headers now participate in both the build-cache fingerprint
and source inventory. This fixes stale fixture code surviving a header edit.
Four build-isolation tests pass, including fixture-header identity separation.

These are in-process restoration comparisons. They deliberately check owned
memory and stable pointer identity; they are not a cross-endpoint canonical
hash schema. Texture pixels/captures, device audio and persisted files are not
restored by WorldJournal. The native frame clock is deterministic in MP;
physical presentation and audio still require their separate output owner.

## Native late-input correction and device boundaries

The diagnostic correction case first records eight exact native frames, then
rewinds and runs eight frames with missing remote input through the real
NetplayRuntime predictor. The ninth frame must stall both Update and semantic
Draw without replacing required history. Reversed and duplicate encoded input
packets then trigger correction; every re-executed native frame must match its
timely-input counterpart, including bombs and shot/focus/direction changes.
Confirmed input publication is blocked until correction finishes. This runs
for both 2P local seats and all three 3P local seats.

The test configures netplay at an already loaded stage. Its fixture drives
WorldJournal Begin/End/Undo and NetplayRuntime Begin/EndCorrection; it does not
yet prove the automatic production driver, startup or transport. Production
WorldReady remains false. Hashes here compare in-process native state and
stable ownership, not a cross-endpoint canonical schema.

Two device-boundary corrections accompany this evidence:

- The live InputController sampler is outside WorldJournal. Its shot-slow
  hold count advances at physical capture and must not rewind when already
  captured FrameInputs are replayed. A native controller-call probe verifies
  its count survives undo. During correction, real pressed keyboard bytes
  (0x80, including Menu) are present but must not be sampled again.
- GameAudioManager MIDI start/reset now uses the same non-rewindable device
  clock as audio_tick, not BrowserRuntime's admitted logical frame clock. A
  probe moves only logical time by ten minutes and verifies audio time stays
  within the device clock interval. Ordinary builds keep the original path.

| Evidence under artifacts/multiplayer-tests/ | Actual scope |
| --- | --- |
| native-correction-1da7c453-5fa2-48f5-a781-ec0ebf9284f2 | All five local-seat native correction cases, eight-frame bound, reverse/duplicate packets, confirmed publication fence, pressed-key non-resampling, physical sampler preservation and independent audio clock pass. |
| world-journal-9366e684-d55d-4785-9c08-d8eb43ed2332 | All eight stable-stage world scenarios and their two later-lifecycle probes pass after sampler separation. |
| enemy-journal-38d47f01-c482-464e-8bdf-118b2fc8a142 | All ECL/screen/pools/geometry/resources/world ownership probes pass. |
| admission-43b8c73f-dd23-45b9-8ab6-770953f70686 | Rebuilt production 2P/3P session admission and input/stall regression passes. |

Latest production MP WASM:
`c4a231bada816399a4ea498dadd653c2c69db2b08ae00ce63516e9f74a287b3f`.
Latest diagnostic MP WASM:
`75bcc1dc226f7c61c258312701d932d177884d9b2cc47188425d1634b6950307`.
Ordinary WASM is still the exact baseline above. Four build-isolation tests
pass. The runner now also captures/checks its Python/JS harness hashes before
and after each run. Command:
`node portable/multiplayer/check-admission.mjs --native-correction`.
Logs: build-correction-sampler.log, build-correction-sampler-production.log,
build-correction-sampler-ordinary.log.

## Confirmed native audio command boundary

AudioEvents is a title command outbox, not a replacement mixer or MIDI player.
GameAudioManager captures primitive WAV/MIDI commands, one-shot SFX requests,
fade updates, native Process steps and volume settings in authored order.
MusicControl still runs during simulation: music unlocks and the choice of
commands are not delayed until output confirmation. Event arguments/paths and
the applicable audio configuration are captured by value, not read from a
later predicted state. The existing device clock remains non-rewindable.

DiscardFrom replaces only uncommitted frames. CommitThrough requires complete
closed records, executes only through the smaller confirmed/simulated frontier,
and never rewinds or repeats the emitted cursor. BrowserRuntime additionally
rejects a commit while input correction is pending or active, or when a caller
claims a frontier beyond the real NetplayRuntime. Overflow or an external
output failure latches failure rather than dropping events or retrying an
uncertain side effect. The bounds are sixteen pending frames, 4096 events per
frame and 256 bytes per copied path including its terminator.

The native correction diagnostic now compares the complete corrected audio
event sequence against the exact-input branch and commits that surviving
sequence through GameAudioManager once. It verifies that the predicted branch
really produced a different audio sequence and that redundant confirmation
does not emit it again. A separate native routing probe exercises WAV and MIDI
load/play/stop/fade/reset routes and logical music unlocks without changing the
checked native audio queues/device-control state before commitment. The routing
probe's command trace uses a diagnostic output sink; it is not an audible BGM
or MIDI playback test.

| Evidence under artifacts/multiplayer-tests/ | Actual scope |
| --- | --- |
| native-correction-bce5ae12-c6a7-4cf4-9eff-36590eb9cbf0 | All five 2P/3P local-seat cases pass native correction, corrected audio ordering/one-time commit, WAV/MIDI routing, immediate logical unlocks and device-clock/sampler separation. |
| world-journal-26436428-8c1c-4581-8c8e-27d882f67629 | All eight stable-stage world scenarios and later-lifecycle probes pass with the new audio owner present. |
| cooperation-407416d6-4497-4488-b4b6-f7fabda371a2 | All seven native cooperation/gift/wipe/retry regression cases pass. |
| admission-a82fdab0-95ea-4d83-a7c8-6d4fd8b9d205 | Rebuilt production 2P/3P admission and exact input/stall regression pass. |

Unit entry: `node portable/check-multiplayer-audio.mjs`. It covers replacement
of wrong music commands, WAV/MIDI order, partial confirmation, duplicate
confirmation, retained-history/event bounds, malformed commands and partial
external-output failure without retrying already emitted effects.
Audio-slice production MP WASM:
`740707677cb5538aa0bfdc5c03f7b09e26761ea1ee898ec05a8d43b38e8a3fa0`.
Diagnostic MP WASM:
`4a5e02f8c1ba0fc693748c227503ef273800c6a146fe70b9713bf5c47a3ca802`.
Logs: build-audio-routing.log, build-audio-production.log, build-audio-ordinary.log.
The rebuilt ordinary WASM remains exactly
`0d00a84ef6b214d43f2f365a6ffaa8bc6030a89c56b7d53868f3b9295b4cea3f`.
All four build-isolation tests and the native netplay/cooperation/audio unit
entry points pass. This still does not replace a full golden Replay run.

At the audio-slice boundary only diagnostics attached this outbox. The next
section records its production integration. No speaker/device listening
acceptance is claimed.

## Production correction and real network boundary

RollbackDriver now owns the actual BrowserRuntime path, not a diagnostic
replacement loop. It opens a native world snapshot, texture before-images and
audio/file records together. Forward native Update is followed by exactly one
semantic Draw. Correcting reuses captured local input and common's corrected
remote input, executes those same native Update/Draw jobs, and never samples a
device or physically presents intermediate frames. The original journals are
retained as independent low-level oracles rather than nested over this driver.

TextureJournal pins released texture records, copies pixels lazily before
native text/name-atlas/capture writes, and restores the surface map, capture
request and renderer state. The stage backbuffer is captured before a
speculative draw because a later pause can copy it. Undo invalidates device
caches instead of copying GPU handles. Render-only writes are associated with
the last frame's output history without creating a simulation tick. The
isolated texture fixture proves write, release, replacement and capture undo.

FileEvents owns copied, bounded pending file writes and read-your-writes
semantics. Only corrected confirmed frames reach ResourceManager and the
physical FileDevice. Failed external output latches failure; it cannot retry
an uncertain earlier write. External sync does not attach the result-screen
owner during network gameplay, and shutdown does not serialize predicted
state. Multiplayer Replay remains a separate product task; ordinary Replay
exports are not passed off as all-seat recordings.

The new driver defers scene destruction until the selecting frame is
reconciled and confirmed. Native Restart/SpellRestart additionally waits for
common's ACK retirement, creates a new session identity and repeats HELLO/READY
on the same transport. Corrected scene branches discard the abandoned future.
Local physical sampler state is not rewound. MIDI service uses the device
clock and runs even when a cadenced callback cannot advance the game; only
confirmed commands may select/start music.

NetworkConnection is a small title seam over shared BrowserPeerTransport and
SessionChannel. It validates TH08's supported input fields/window; signaling,
ICE, mesh, Relay fallback, ACK/retransmission, health and epoch assistance stay
in common. No title-specific Relay or second simulator was added.

### Latest evidence

Production WASM:
`ec963f8bfa65502b25d7858f3b2d8dbdbf42843b96cb494345cd83e8cc781425`.
Diagnostic WASM:
`ac35403b255f3f8bc5db8e72ae1210038fb7d4f14bd931173c410e351315bbb4`.
Ordinary WASM, rebuilt after the MIDI servicing fix, is byte-identical to:
`0d00a84ef6b214d43f2f365a6ffaa8bc6030a89c56b7d53868f3b9295b4cea3f`.

| Evidence under artifacts/multiplayer-tests/ | Actual scope |
| --- | --- |
| network-4099b5f5-f3b3-47e4-bc07-bbf224679bc9 | Production 2P/3P x real RTC/forced Relay: native frames through 379, pointer-independent gameplay/resource canonical comparison, committed audio comparison, pause/resume, native Restart, generation-1 frame 119, stale old HELLO rejection. All four cases pass. |
| network-4ba553eb-f9ae-4de0-9d78-bbed513020b7 | Production automatic driver in the diagnostic binary: four-frame late, reordered and duplicate encoded packets for 2P/3P; independent zero-delay reference; shooting, focused/unfocused motion, bombs and pause/resume; full gameplay-owner hash and cropped framebuffer equality; extra render-only draws; eight-frame bound with frozen game/image and eight MIDI service calls while stalled. Both cases pass. |
| native-correction-f38197c0-10e0-4bc9-81e4-61c88c98c8f5 | All five manual owner/correction local-seat oracles plus native texture/capture restore, unchanged physical sampler, independent audio clock and corrected audio routing pass. |
| admission-b6e98a8d-7dae-4fac-a308-d65ba0a02328 | Production 2P/3P barrier, per-seat native input, bounded prediction then Update/Draw stall pass. |
| cooperation-3ef13500-2370-4ebb-a13c-14812973e35f | All seven native cooperation, directed gift, final-death rescue and team-wipe/retry cases pass. |
| enemy-journal-85366e11-47db-4dea-9897-ec0f7ac2b957 | Independent ECL/screen/pool/resource/world ownership oracles remain passing after production-driver integration. |

The deterministic packet case produces actual correction, not just a joined
room: 2P endpoints correct 64/52 times and resimulate 255/207 frames; 3P
endpoints correct 64/66/66 times and resimulate 255/263/263 frames. Independent
timely-input references perform zero corrections. At frame 379 canonical
composites match 2861016556 for 2P and 1430406737 for 3P. Cropped GPU framebuffer
SHA-256 matches each reference:
2P `8bd131550f91a4fbb29941b72224565fa0295b69577383abb0fa792fc4af36af`;
3P `12e3734538c4143c7af2fb1aaf95e29c0df3bad2713b430f29fa59561f87ba4c`.

The cross-endpoint canonical export maps pointer identities to native pool
slots, resource indices and byte offsets in diagnostic copies. It is never
used to restore state or gate gameplay. The exact in-process WorldJournal
oracle remains unchanged. Personal high score is omitted; authoritative
economy, input edges, native player/shot/bomb state, ECL, pools, RNG, resources,
spell/camera state and scene lifecycle remain compared.

Reproduction entries: check-network.mjs (production RTC/Relay or --fixtures
--only=packets), check-admission.mjs --native-correction, and
check-multiplayer-files.mjs. The network runner records native build identity
and harness hashes before and after execution. Source-build logs are
build-driver-verified-production.log, build-driver-verified-fixtures.log and
build-driver-verified-ordinary.log. Native netplay/cooperation/audio/files units,
four build-isolation tests and diff whitespace checks pass.

## Authoritative analog input and Runtime host boundary

The room input ABI is revision 3. Validated joystick vectors and direct-touch
displacements enter each native Player through commit_frame_inputs; they are
recorded and corrected by the same common input history as keyboard buttons.
Physical touch sampling runs once per uncaptured local frame, and local
coordinates refer to that seat's player rather than implicitly to P1. Missing
samples use common's bounded prediction, not a second device read. Unlimited
movement marks the run only when a nonzero movement is actually consumed; the
marker survives stage transitions. Practice mutation is disabled in the native
network entry points as well as the host UI.

multiplayer-host.mjs maps the existing Launcher room/run options to the native
session gate and BrowserPeerTransport. It does not own a simulation loop or
the parent lobby. The actual shell validates the role/configuration before
launch, initializes native resources, connects the room, and exposes read-only
network diagnostics. Storage uses savesth08-multiplayer in both native and
IDBFS owners; live save import/removal is rejected. Packaging includes this
Runtime module without declaring an unfinished public TH08MP product.

Evidence under artifacts/multiplayer-tests/:

- network-599523af-7bda-4fc9-a6e2-dbe1e4f4c8a1: production 2P/3P RTC and
  forced Relay, with native restart/generation and stale-session rejection.
- network-a99c699f-114d-4b04-b6f7-e9384c424f71: current diagnostic build,
  2P/3P analog input with delayed/reordered/duplicate packets, independent
  timely-input reference, canonical owner and framebuffer equality, and the
  bounded prediction stall. Direct rate-limited, joystick and unlimited
  displacement are exercised in one short run, not long Replay screenings.
- network-e29d00dd-8c25-42b1-ba23-110d6628b76a: PRODUCTION Runtime shell and
  real room/run admission; P2 direct-touch movement and bomb arrive identically
  on both endpoints, P1 does not move with P2's gesture, live save import is
  rejected, one exit notification is emitted, the parent lobby survives, and
  isolated score data survives a newly created iframe/IDBFS mount.

Production WASM:
`0e7e89691f56b1556f862bc51b06ae46b48ed46eb9265e383e2df81d01118ee5`.
Diagnostic WASM:
`547250831e0d19eb4f62405b762cbdcbca9c72fc5cbcbf029233f5e0287aa313`.
The ordinary build was rebuilt and remains byte-for-byte
`0d00a84ef6b214d43f2f365a6ffaa8bc6030a89c56b7d53868f3b9295b4cea3f`;
log: build-input-host-ordinary.log. Host-option/native input units, the four
build-isolation tests and diff whitespace checks pass. These are automated
local-browser checks, not human mobile/device or public deployment acceptance.

## Start-time read-only spectator

The TH08 adapter now uses the shared SpectatorFramePacket and
BrowserPeerTransport::ConnectSpectator path. P1 publishes only already
simulated, reconciled all-seat confirmed input, including the authoritative
analog/touch payload. Observers have no gameplay peer/seat and no input
prediction. They consume the exact rows through the same RollbackDriver,
GameApplication and native Update/semantic Draw path as live players.

The receiver requires frame zero and a contiguous session/ABI-matched stream;
malformed samples, gaps, duplicates and a backlog above 4096 frames fail
closed. A delayed already-admitted observer catches up with at most four
native logical frames per callback, without resampling devices or physically
presenting intermediate catch-up frames. The MIDI clock remains outside that
logical frame count. There is no mid-run admission or reconnect shortcut.

Native capture, analog capture, HELLO/READY, input-packet construction, direct
input submission, local cooperation input and THPrac mutation are rejected for
the spectator role. The shell also discards spectator keyboard/touch commands
and rejects resource replacement after multiplayer launch. Read-only native
file policy is active BEFORE native initialize(), so config/score writes at
startup are not an exception. Existing records remain byte-identical through
startup, gameplay and exit.

The observed players' confirmed Restart ends the current observation. The
observer does not create the players' next generation: its Runtime exits once,
while the parent lobby remains owned/open in Launcher. The players continue
their own ACK-retired generation and handshake normally. Spectator failure or
publication backlog exhaustion never rewrites or stalls a player's world.

Production evidence under artifacts/multiplayer-tests/:

- network-28cd501d-4c28-4005-854d-3c268353330c: all four combinations of
  2P/3P and real RTC/forced Relay, using the actual Runtime shell and room/run
  admission. Players reach frame 89 before the observer Runtime launches.
  All endpoints agree at frame 239 on every exposed canonical owner and
  committed audio; deliberate local observer input has no effect. Native
  Restart ends the observer at the old generation while players reach
  generation one/frame 119. Imported record bytes and the parent lobby survive.
- network-e6bf4c4f-9a18-4959-885c-3c9db65d31fe: final host-only resource-write
  hardening, actual player touch/storage host regression and 2P RTC spectator
  regression, including live resource-replacement rejection in both roles.
- network-00146889-8317-496a-8651-1f5948b4a4db: isolated 3P analog delayed /
  duplicate / reordered packet correction, independent timely-input reference,
  canonical and framebuffer equality and bounded prediction stall all pass.
  The preceding combined run network-5e8b2e23-9419-4fe0-8680-60c5ff798eda
  passed 2P but stalled during 3P resource bootstrap; it was terminated and its
  incomplete report is preserved, not counted as a 3P result. The harness now
  yields every twelve preload operations, matching the real Runtime shell,
  and records the boot endpoint/step. No gameplay tick or input was added.

Production WASM:
`47b88a6088485452f45232aa8d30be72d81c974095d11472e32f8802a105b0d7`.
Diagnostic WASM:
`4955b0712e8ec07d7f1cd40abbd1c4e6e0b1fb0953c768450af8f7ed9a83609a`.
Rebuilt ordinary WASM remains
`0d00a84ef6b214d43f2f365a6ffaa8bc6030a89c56b7d53868f3b9295b4cea3f`.
The shared Relay whitelist additionally recognizes TH08's existing '8' magic;
its product-policy regression remains passing. This does not itself declare
or publish a TH08MP product. These are automated local-browser results, not
human-device or public-network acceptance.

## Next implementation boundary

Production networking, automatic correction, authoritative analog/touch,
start-time spectators and the Runtime room/start/exit/storage contract are
connected. MP Replay and public Launcher product integration remain before declaring the complete TH08MP
profile. Full-stage coverage, real-device acceptance and public deployment are
not implied by these focused local-browser tests. No performance change,
canonical promotion, push or deployment is claimed.
