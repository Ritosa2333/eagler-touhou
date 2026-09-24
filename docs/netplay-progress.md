# TH08 session admission and rewindable owners

Status (2026-09-24): the native 2P/3P frame-zero gate and exact input handoff
are implemented. Enemy/ECL, screen callbacks, projectile/item/effect pools and
effect geometry ownership journals have focused native acceptance.
**Complete world rollback and real transport are not yet
implemented or accepted.** This continues the cooperation slice at ea2958b.

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

Current production MP WASM:
`c711e1f7d19793f64b6f43029a00e51846bdcd3960c72596eed646ba48943963`.

Current diagnostic MP WASM:
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

## Next implementation boundary

Compose a title-owned WorldJournal covering each player's life, movement,
shots/bombs/regions/options; shared economy/RNG/input edges/cooperation; native
enemy/ECL; active bullet/laser/item/effect pools and allocation writes;
GUI/dialogue/background/STD/ANM mutable data; semantic Draw and supervisor
lifecycle. The screen/ECL/fixed-pool modules above provide lifetime-safe pieces.

Then integrate correction with confirmed external side effects (audio,
persistence and physical presentation), loading/stage/retry fences and a
first-divergent-owner oracle. Only that complete path may set WorldReady=true.
Actual shared BrowserPeerTransport/Relay, MP Replay, spectator, authoritative
touch and Launcher lifecycle remain subsequent work. No performance change,
canonical promotion, push, deployment or human-device acceptance is claimed.
