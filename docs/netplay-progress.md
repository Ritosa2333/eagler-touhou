# TH08 session admission and rewindable owners

Status (2026-09-24): the native 2P/3P frame-zero gate and exact input handoff
are implemented. Enemy/ECL, screen callbacks, projectile/item/effect pools and
effect geometry ownership journals have focused native acceptance.
ResourcesJournal and WorldJournal provide tested stable-stage native
Update/semantic-Draw restoration. Diagnostic late-input correction passes
through the actual NetplayRuntime and BrowserRuntime on every 2P/3P local seat.
**The production correction driver, confirmed device output and real transport
are not yet accepted.** This continues the cooperation slice at ea2958b.

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

NetplayRuntime defaults to WorldReady=false. Therefore its native production
bridge currently advances only exact inputs, never unprotected predictions.
The unit-tested correction/frontier machinery is not a claim that TH08 world
undo is already connected. Enabling prediction requires the complete owner
inventory and the actual late-input world comparison, not just these tests.

The pinned common core predates transactional input-packet application. The
title adapter applies a validated packet to a temporary core and commits it
only on success; a conflicting redundant tail cannot partially advance the
live confirmation frontier. Analog/touch samples are rejected until their
authoritative simulation owner is implemented, rather than silently dropped.

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

Only diagnostics currently attach this outbox to the correction loop. The
production driver still needs graphics/capture and persistence protection,
confirmed scene-transition lifecycle and cross-endpoint canonical validation
before enabling WorldReady. No speaker/device listening acceptance is claimed.

## Next implementation boundary

Integrate late-input correction with confirmed external side effects (audio,
persistence and physical presentation), loading/stage/retry fences and a
first-divergent-owner oracle. Only that complete path may set WorldReady=true.
Actual shared BrowserPeerTransport/Relay, MP Replay, spectator, authoritative
touch and Launcher lifecycle remain subsequent work. No performance change,
canonical promotion, push, deployment or human-device acceptance is claimed.
