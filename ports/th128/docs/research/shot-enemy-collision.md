# Shot/enemy collision: bounded native contract

## Scope and implementation plan

This worker owns only `ShotEnemyCollisionRules.hpp/.cpp`, the matching collision
probe/tests, this document, and `local/research/shot-enemy-collision*.json`.
No live actor hook, SHT motion binding, shared build changes, EXE launch, external
drive writes, commits, agents, or item/death implementation belong to this work.

1. Recover original query/contact/caller bytes and pin executable/resource hashes.
2. Demonstrate missing probe/API RED, publish the header, then implement the pure
   ordinary kind-0 geometry and the first mandatory native-call mutation boundary.
3. Compare native returns and positive call prefixes with portable C++ in WASM
   and UBSan; cover float-bit edges and wrapping life/stat accounting.

## Frozen public API

Namespace `th128::shotenemy` is self-contained. The public header is the integration
contract; no existing shot or enemy API is modified.

- `planOrdinaryShotEnemyContact`: immutable 0x78-byte slot plus actual enemy
  combined position/full width and height; a runtime record binding is separate
  from the SHT file's callback IDs. An absent binding is never a zero callback.
- `planShotEnemyQuery`: previous/current player time, all 256 slot images, runtime
  bindings, optional explicit 2000-element secondary damage-pool flag snapshot.
  Stops at the first unsupported record/kind/callback/ANM continuation or active
  secondary source. An absent secondary snapshot is not an empty pool.
- `planEnemyDamageAfterCompletedQuery`: accepts only a `Complete` query plan;
  calculates bounded life/stat mutations from explicit caller snapshots. It does
  not make a positive contact prefix into completed damage.

## Recovered ABI

`0x41659b` invokes `0x43fbf0` **after Enemy ECL**. Its three stack arguments are
`enemy combined XYZ*`, `embedded enemy state + 0xd0 full-size XY*`, and optional
`uint32_t* damageType`. EAX is signed damage; `ret 0xc` consumes all arguments.
The out flag is a **four-byte word**, not a bool or a pointer return.
The equal player `+0xf80/+0xf84` time gate returns zero without touching it.
Otherwise the non-null out word is initialized to zero at `0x43fc6b`.

The ordinary pool starts at player `+0xfa8`, has 256 slots with stride 0x78,
and skips active values 0 and 2. Position is +0x14, velocity +0x20, active +0x48,
ANM handles +0x4c/+0x50, contact marker +0x58, damage +0x60, full size +0x68/+0x6c,
runtime record pointer +0x74. Record kind is byte +0x1d and collision callback is
runtime pointer +0x30 (not the file callback index).

`0x43eb20` forms float32 rectangle edges using x87 intermediate arithmetic.
Ordinary edges are inclusive and enemy maximum Y must be >= 0. There is no circle,
TH12 hitbox, velocity sweep, or substitute collision radius. Kind 2 has distinct
geometry and kinds 2/4 have damage gates; all nonzero kinds are outside this API.

On ordinary overlap, nonzero/unknown callback stops before marker mutation at
`0x43ec10`. Confirmed-zero callback writes marker +0x58 = 1 and selects the signed
slot damage into a local, then **must** call `0x467780` at `0x43ec8a`, followed by
cleanup `0x4679b0`, replacement ANM creation `0x4670b0`, another lookup and position
sync. Only marker +0x58 is in the implemented prefix. Active = 2, Z replacement,
speed scaling, new handle, and final damage return cannot be claimed yet.

After actual shot-call completion positive accumulated damage requires impact
call `0x427260` at `0x43fcb3`. Subsequent power adds 100 or 10 according to explicit
manager `0x4b8938 -> +0x6b218` state. This is not a fabricated freeze implementation.
The second pool starts at player +0x8edc, has 2000 slots at stride 0x78 and active
flags +0x74; any active source is an unresolved native query continuation here.

## HP accounting boundary

Caller scales completed query damage by signed /5 for player states 0 and 2.
Control +0x5a1c nonzero bypasses all stats and HP. Positive damage updates manager
+0x24 for a nonzero type word (overkill quartered) or +0x28 otherwise. Counters
wrap as native 32-bit words. Native `0x414e50` reads explicit spell flags at
`0x4b8934 -> +0x7c`; bits 0 and 6 together reduce subsequent HP damage by /30,
without reducing the earlier stats delta.

Enemy flag 0x10 or positive protection +0x3ea4 suppresses `0x4147d0` HP subtraction
but not earlier stats. Its six-word life object can have a flagged /7 accumulator
mode, not just a scalar HP. Arithmetic wraps before signed division.
Post-damage script lookup `0x41dea0` remains mandatory. A life<=0 / flag-0x80-clear
state is only a death *candidate*: `0x417370` at `0x416715` is still unknown and no
item release, actor removal, script return, or enemy-host completion is invented.

## Evidence

The current report has183 cases and SHA256
`790b24331bee0df10c6a22c96a091b70cf21b2b01512fa387c733ca5b9fa6463`.
Normal and UBSan WASM checks compare ordinary contact slot mutations, exact
enemy-edge bits, complete miss returns, source-pool stops and explicit HP/stat
prefixes. The remaining native death predicates are recorded but not treated
as completed deaths. The main-thread host applies the actual marker on first
contact and stops at `0x43ec8a`, without decrementing enemy HP or allowing the
shot to continue through the target.

An opt-in contact-animation host now actually unbinds the old primary VM,
binds the SHT-authored replacement script (6/8/10), restores its actual old
rotation, performs physical-to-display sync and applies the independently
executed native tail. Five tail vectors prove active2, depth0.1 and speed/8.
The marker never becomes completed damage before real animation operations.
Fourteen impact vectors prove motivation+2, flag gating, native100000 cap,
and the mandatory sound18/notification boundary on each10000 crossing.
The host requires an explicit positive-query impact callback; none is supplied
by default and full HP/death integration remains incomplete.

Six after-ECL gate cases execute0x416500: native clears flag0x400000 first,
then flags1/0x20 bypass the ordinary query. StageWorld now follows that gate
and passes the actual post-ECL position/extents, including synchronous birth
updates and the original manager guard against a duplicate first-pass update.
