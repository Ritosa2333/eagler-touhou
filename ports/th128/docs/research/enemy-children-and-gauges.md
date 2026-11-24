# Actual MBoss189 children and427 gauge state

Continuation of `midboss-and-anm-control.md`, main thread only,2026-10-30.
Neither a whole boss nor a stage/game is complete. Original installation files
are read-only; native allocations/APIs are never assigned synthetic returns.

## Native evidence and supported topology

`enemy_child_probe.py` checks actual88 dispatch45a32f, scripts186/187/188,
parent and registrationFlags0. Each request stops at real467220 before allocation.
A separate original post-bind suffix46729a starts with declared prebound storage,
executes real466d60 tail registration, and returns through parent-link stores.
This is not a continuous original allocation/bind trace. Existing real reset/bind
evidence shows factory's pre-reset display bit2 is cleared; these authored child
programs consume the existing shared game RNG, not a made-up display seed.

The original primary-manager prefix466b45 caches each successor before VM update.
Continuation466ba6 uses that saved pointer rather than rereading the newly
appended link. The existing single global EnemyVisuals list now follows it:
ECL roots enter at the head; supported88 children enter at the tail. New children
may be visited later in the same pass only if a previously cached successor
eventually reaches them. A per-parent Scene would change ordering and RNG.

Only actual bank0/189 →186/187/188, one-level attached88 topology is accepted.
Other opcodes/topologies, detached children, immediate child retirement and
missing game RNG fail explicitly. Source raw resources are not rewritten.

## Ownership and presentation

Each root/child has a stable nonzero node identity, parent identity and actual
entity/slot. Child startup executes synchronously. The child's local engine pose
stays separate from the owner's post-bind pose; world XY/Z, scale and applicable
rotation compose through its live parent once. Render records use unique child
IDs; they do not reuse the root's16-slot key or translate the parent twice.

Successful replacement retires the old attached children; rejected startup keeps
the original root/slot/identity and leaks no staged child. Already consumed RNG
stays consumed. Owner removal retires all its attached generations without
touching other owners. A failed live parent hides its descendants from snapshots.
Read-only snapshots never advance RNG or animation. Derived nonfinite geometry
is handed to the explicit presentation guard, never silently dropped.

All roots/children share the explicit111 control snapshot. Actual primary files
`enemy.anm`, `enemy_ll.anm` and `boss01.anm` are loaded as native banks1/2/3 in
WASM and WebGL. Every appearance script still requires separate constant-metric
proof when used; loading an atlas is not proof that all its programs work.
Bank0 remains actual `bullet.anm`, also used by enemy secondary animations.

## Tests and fixes found during review

- Actual189/99 plus an unrelated enemy root compare with established Scene for
 100 ticks: authored script order, child births/expiry, exact shared Random
 seed/count, paused clocks, colors, transforms and composed geometry.
- Tests cover owner removal, successful root replacement, failed replacement
 from missing RNG/throwing callbacks, distinct identities and no leaked children.
- A RED test found the node budget reserved an extra parent even during a live
 update. Startup reserves one pending root; a registered parent can fill4096
 exactly; the4097th node is explicitly refused.
- A RED test found composed overflow could silently hide a snapshot. The real
 rendering error path now receives the nonfinite result instead of drawing less.
- A RED test found nested host calls rejected reentry but an ignoring RNG callback
 could still publish the parent and consume later random requests. Operation
 failure is now latched into that node, later RNG is suppressed, startup rolls
 back staged children, and the next explicit attempt can recover after rejection.
- Existing ordinary and exception-disabled root tests remain required. These
 focused tests do not stand in for fresh whole-suite or browser verification.
- Dedicated desktop/mobile integration found score-digit IDs sharing the charge
 HUD's7000000 range. A genuine RED now loads the same actual font as the browser
 and checks live draw identities every30ticks; score glyphs use9000000 separately
 from HUD7000000 and child8000000. No scoring/timing/resource semantics changed.

##427 stores, not invented health UI

The original427 handler41a238 divides authored float health by the native int32
maximum at enemy-data3e40 and calls the actual414db0 setter. Twelve complete
original handler/setter paths prove a float32 ratio/color pair at HUD+5918,
stride54 per boss and8 per segment. No other HUD bytes change in these samples.
Portable state accepts only bounded boss0..7/segment0..1, finite health and
positive maximum; invalid inputs are rejected before mutation.

Actual Normal MBoss sets life10900, then ratios6300/10900 and2000/10900, with
colorsffff8080 andff800000. State is retained and exported, not ignored or replaced
by a generic full-health bar. Native gauge animation/draw/live consumption is
still missing; retaining these writes is not complete Boss HUD implementation.

## Current executed boundary and next dependency

The real Normal/seed34772 path remains at world frame3170 but now has actual
bank2/script0 64x64 body, bank0/99 aura,189/186 child and both427 gauge writes.
It stops at `MBoss offset516 opcode414`, the first original phase registration.
Eight deaths/eight respawns have already executed. No later MBoss script or
phase switch is simulated by guessing a return.

Read-only next observations:414 writes one of eight16-byte records at enemy
50c4, storing a health threshold, time limit and two subroutine pointers. Helper
41dea0 is called after damage at41665a and handles health/time phase selection,
timer state, visible countdown and additional global spell/control writes.
Those are not yet implemented; the next work must cover real registration,
selection and continuation ordering before connecting414.113/114, Z/freezing,
complete initialization, gauges, lasers, backgrounds and all routes remain open.
