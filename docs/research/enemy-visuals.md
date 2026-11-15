# Caller-scheduled enemy ANM sidecar

## Design and bounded plan

Implement only the new EnemyVisuals component, its native/Node regressions,
and this report. Reuse the existing ANM Registry and VM without editing them.
The caller supplies every world pose and chooses every global tick phase.
There is no age-to-sprite selection, ECL simulation, native-process launch,
or claim of whole-frame equivalence.

1. RED: synthetic cases for bind/reset, explicit poses, slots, visibility,
   missing RNG, unknown operations, failed reloads, and resource budgets;
   actual-bank cases for the 78-script first include and its steady loop.
2. GREEN: bounded registry/VM sidecar with 16 slots per entity, 4096 live
   roots, and 4096 instructions per VM tick. No child scheduler is fabricated.
3. Verify actual resources and any cheap native static evidence. Report
   remaining native scheduling/geometry gaps explicitly.

## Resource identities

| Bank | Resource | Sprite 0 dimensions |
|---|---|---|
| 0 | Original `bullet.anm`, also used by real secondary enemy259 | Program-dependent |
| 1 | First ANIM include, `enemy.anm` | 32 x 32 |
| 2 | `enemy_ll.anm` | 64 x 64 |
| 3 | `boss01.anm` | 96 x 96 |

Bank numbers identify caller-loaded bytes; the API does not infer filenames.
The host must load these exact include identities, not map bank 1 to the
second include. First `GirlA1b` uses ECL262 slot 0 / script 0 / bank 1.

## API contract

`th128::enemy::EnemyVisuals` loads copied/parsed ANM bytes with
`load(bank, bytes, size, error)`. `create(entity, slot, bank, script, layer,
[VisibilityEffect,] error)` binds a fresh VM with engine position zero and
performs exactly the existing VM's single synchronous startup tick. A
previously caller-supplied owner pose is copied in only after that tick.
ANM commands may override the requested initial layer.

`position(entity, x, y, z)` applies only an explicit finite caller pose to
existing roots. `visibility(entity, effect)` applies EnemyRules' existing
Hide/Show effect to draw-enable flag 2; it does not tick or replace ANM
visibility flag 1. `tickAll(error)` advances surviving roots once in native
newest-first head order across all owned entities, slots and banks. Synchronous
startup still occurs in the caller's creation order; successful replacement
becomes newest and failed replacement retains the old root's list position.
`removeOwner(entity)` releases all that owner's roots.

`snapshots() const` returns copied entity/slot/state/sprite records.
`spriteInfo(bank, sprite) const` returns optional copied metadata. Neither
can execute instructions, draw, consume RNG, or expose mutable live VM state;
retained copies survive removal, reload, and sidecar destruction.

Successful reload removes only roots referencing that bank; malformed reload
keeps the previous bank and roots. Failed create leaves the previous slot
intact. Execution is not transactional: a failed bind/tick can already have
consumed explicit caller RNG or written shared script operands. On tick
failure the failed root is hidden from snapshots and subsequent tickAll calls
fail until it is removed/replaced/reloaded. Earlier roots may have advanced.

There are no default RNG seeds. `setRandomSources(game, display)` is required
only when executed instructions read those domains; opcode102 always uses
the game source. Sprite validation returns the original index unchanged,
never applies a bullet-color/native enemy remap. Negative/font fallback,
missing sprites, camera-dependent operands, child scheduling, required but absent
control snapshots and unsupported VM instructions fail closed. Explicit native
control snapshots now implement111's real pre-dispatch gate; see
`midboss-and-anm-control.md`. This is not a
complete Enemy ANM implementation or an IRQ/child API.

## Evidence boundary

The existing interpreter/Scene comments contain inherited TH12 addresses
and scheduler semantics. This sidecar uses VM command execution, not Scene
priorities, and does not silently promote those comments into TH128 evidence.
In particular, the native constructor binds/resets synchronously and
registers an ANM list node, while the caller still owns cross-system phase.

The main worker's static appearance-bound analysis for constant programs
3/4/68 remains authoritative for its supported first-primary path. Animated
sprites here do not repair or replace ECL262 metrics capture or its native
root-cause analysis. No collision/appearance metric is recomputed from age
or from later animation frames.

## TH128 original-byte static evidence

`local/research/enemy-visual-static.json` retains complete original byte
ranges, SHA-256 per range, selected complete Capstone instruction listings,
and bank resource hashes/first-script commands. These are static decodes,
not emulated/native CPU traces. The executable SHA-256 before and after was
`ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.
The original was only read; no original process, Windows game call, or fake
external return was used.

| Address | Original bytes | Static observation |
|---|---|---|
| `0x415325` | `898f80000000` | Store resource pointer at EnemyManager +0x80, slot 0 |
| `0x4154dd` | `c745fce0904e00` | First include resource base is 0x4e90e0 |
| `0x415554` | `89bc10a46fb1ff` | Include store displacement -0x4e905c; first destination +0x84 |
| `0x418dd6` | `8b8c8a80000000` | ECL262 lookup is manager +0x80 + bank*4 |
| `0x41f310` | `e89ba00300` | Synchronous call to VM binding at 0x4593b0 |
| `0x41f31b` | `e8c07a0400` | Followed by primary-list registration at 0x466de0 |
| `0x4638f6` | `bb1e000000` | Registration priority 30, not TH12's 27 |
| `0x4638fd` | `c7400890664600` | Primary callback pointer 0x466690 |
| `0x463907` | `e8c4480000` | Register callback via 0x4681d0 |
| `0x46393c` | `bb08000000` | Secondary priority 8 |
| `0x463943` | `c74008c0664600` | Secondary callback pointer 0x4666c0 |

Primary callback 0x466690 conditionally jumps to manager pass 0x466af0;
secondary 0x4666c0 jumps to 0x466be0. The primary pass reads the same
manager +0x8b9704 list head populated by 0x466de0, saves the successor,
checks a possible VM +0x4bc custom callback, and calls VM tick 0x459d90.
Registration0x466de0 inserts at the list head. The sidecar follows this policy
for all of its roots, while rejecting child/native custom callbacks. Other
engine-wide ANM roots not owned here are still outside this component's scope.

The draw-time Enemy world-pose callback and full cross-system scheduling
remain unproved here. Native priority is a sort key, not a browser frequency
or proof of activation. The main host now orders enemies21, bullets24 and primary
ANMs30, based on these native priority observations. Enemy roots register at
the primary head, while effect roots created via0x466d60 append to the same
primary list; they are not secondary8 objects. These registration operations
are separately emulated in the integration's pool probe;
a newly created enemy VM receives its synchronous birth tick and the later
priority30 pass in the same local cycle. An integration regression caught the
former before-world placement as1tick instead of2. Global bootstrap, activation
and draw-time explicit poses still are not a complete native-frame proof.

Independent review found and corrected two Important issues and one Minor:
newest-first RNG/shared-operand order, op102 signed overflow before sprite
validation, and an integer operand falsely identified as a float camera read.
RNG-order regression/mutation checking fails with the old append policy and
passes with native head insertion, including interleaved owners/slots/banks,
successful/failed replacement and retirement. Original-style unsigned wrapping
plus bit-preserving signed reinterpretation removes op102 UB; a malformed
INT_MAX+1 replacement first triggered UBSan, then rejected safely without losing
the previous root. Camera context is now checked only when the VM actually reads
a float camera operand; integer bit patterns are not preclassified as floats.
All four updated EnemyVisuals tests pass under UBSan.

## Integration and limits

The frozen public header is `src/game/EnemyVisuals.hpp`. Use
`setRandomSources(sharedGameRandom32, {})` when only a game-domain source is
known; an executed display-domain RNG instruction then remains an error.
The component rejects reentrant mutation from its callbacks (boolean false,
and an error for load/create/tickAll), and is single-threaded. Per-VM runtime
callbacks forward into the stored callable, rather than copying its target;
mutable closure-owned game/display RNG state survives both births and ticks.
With C++ exceptions enabled, RNG callback throws return an explicit failure,
suppress subsequent RNG source calls in that VM step, and hide/latch the
failed root just like unsupported instructions. Failed births publish nothing;
the RAII execution guard is restored on all paths. Shared script/RNG side
effects preceding failure are not rolled back. With `-fno-exceptions`, no
catch syntax is compiled and the caller's RNG callbacks **must not throw**.
Layer inputs are bounded to 0..255; slots are 0..15.

Read snapshots' copied `state` and optional copied `sprite`. Script-space
XYZ/offsets and caller engine XYZ remain separate; drawing must compose them
without ticking or using enemy age to choose a sprite. A slot with no current
sprite can legitimately have an empty `sprite`; it is not a lookup fallback.

For the actual first bank, script0 selects sprites 0/1/2/3 at times
0/5/10/15, then jumps to byte offset 12 with clock 0 at time 20. Bind executes
time 0 once. The regression checks every one of 1024 caller ticks, including
the fifth-tick change and repeated loops, without providing any RNG source.
Programs 3/4 begin on sprite 7; program 68 begins on sprite 156. The last
script, 77, binds successfully; script 78 is rejected.

## Final verification, 2026-10-29

- Initial feature tests were RED with the component absent, then GREEN with
  real VM binding and scheduling. Stateful closure-owned RNG and callback
  exception regressions were separately observed RED before the forwarding
  and guarded-catch repair; both are now GREEN.
- `node --test tests/enemy-visuals.test.mjs tests/bullet-visuals.test.mjs
  tests/enemy-rules.test.mjs tests/appearance-bounds.test.mjs`: 10 passed,
  zero failures/skips. This is the focused/adjacent set, not a whole-game test.
- With `TH128_ENEMY_UBSAN=1`, the enemy-visuals test file: 4 passed, zero
  failures/skips, compiled with undefined-behavior sanitization and exception
  catching enabled.
- With `TH128_ENEMY_NOEXCEPT=1`, the same file: 3 passed, zero failures,
  one intentionally skipped callback-throw case; production compiled with
  `-fno-exceptions`.
- Manual author review checked callback capture lifetimes, owner/slot and
  reload removal, the 4096-node cap, read-only snapshot ownership, and failure
  publication. No independent review agent was launched; the main worker
  retains its separately requested review/integration responsibility.

Production public API is frozen. No StageWorld, runtime, site, EnemyRules,
shared ANM implementation, or BulletVisuals edits were made by this task.
No branch, commit, native game process, or writes to the original drive were
performed. Only the six allowlisted source/test/report/fixture files are new.
