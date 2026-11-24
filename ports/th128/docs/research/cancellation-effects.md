# Unified Scene-backed transient/cancellation effects

## Scope and bounded plan

EnemyVisuals remains frozen. This task creates only CancellationEffects,
its native/Node tests, this report, and optional private cancellation evidence.
It uses the existing ANM Scene/Registry/VM, never age-selected sprites.

1. RED: actual bullet.anm scripts83/84/175/357/356, mixed-root phases, caller positions,
   interpolation, RNG-owned state, reload lifetime, cleanup and failure budgets.
2. GREEN: all four root types and attached op88 child356 in one Scene's
   Primary list with tail registration. One caller-selected primary30 pass.
3. Verify normal, UBSan and no-exceptions builds; retain original-byte static
   evidence and state native scheduling/drift limits without inventing calls.

Supported root scripts are **83,84,175,357**. Only357 may create attached
child356. Other root scripts/colors, detached/native callback families,
external geometry, negative/font sprites, unbound display randomness and
unknown instructions fail closed. The standalone TransientEffects class and
tests are unchanged, but the integrating host must route **all** these known
tail births/passes through this single instance, not also tick per-kind
TransientEffects/cancellation groups. Hit-bullet IRQ delivery remains separate.

## Public API contract

`th128::effects::CancellationEffects` exposes:

- `load(bytes, size, error)` for a private Scene registry's bullet bank 0.
- `setGameRandom(callback)` for the required shared caller-owned game source.
- `spawn(script, x, y, z, layer, error)` returning an optional nonzero Handle.
- `tick(error)` for one caller-selected primary phase30 pass over both roots
  and children. `tickPrimary(error)` is an exact alias, not an additional phase.
  Call exactly one of these methods per intended host pass, never both.
- `setRootPositionInterpolation(handle, x, y, z, duration, mode, error)`
  applies exactly VM.interpolatePosition to explicit **script-local** targets.
- `initializeNativeDrift(handle, velocity, clock, globalStart, error)` supports
  only explicit positive-zero start, clock1, finite velocity/target, and a
  fresh **script357** bind-tick1 root at positive-zero script XYZ with no
  existing track. It rejects83/84/175 even if their position is zero.
- `requestNativeDrift(handle, error)` still rejects generic/inferred drift
  without that explicit context; it never invents velocity/clock/global state.
- `remove(handle)`, `clear()`, copied `snapshots()`/`spriteInfo(sprite)`,
  and read-only `rootCount()`/`nodeCount()` for live ownership diagnostics.

Spawn XYZ is physical caller-provided engine position (e.g. 320+X, 16+Y, Z).
The synchronous root bind and any synchronous child binds each execute one
existing VM startup tick. Attached child engine XYZ stays zero; the world
pose adds its ancestor's local/engine/offset XYZ. Snapshot state and optional
sprite metadata are copies. Snapshot world pose is composed separately;
the renderer must not add engine XYZ or ancestor translation a second time.
Snapshots do not tick, consume RNG, mutate Scene, or expose live VM references.

The integrating factory supplies layer23 for175/357 and layer0 for83/84.
The existing layer input range0..30 is unchanged. ANM op68 overrides the
initial layer:83/84/175 set16, child356 sets6, while357 retains23. All authored
layers are retained. None of these draw layers is the scheduling priority.

Successful reload clears active roots/children and makes old handles stale;
malformed/unsupported reload retains the previous bank and active roots.
Handles are instance-local and monotonic across reload/clear, while internal
Scene node IDs are not public handles. Copied snapshots and metadata survive removal/reload and
component destruction. Every owned root entry is reclaimed on retirement.

## Bounds and error policy

The single 4096-unit budget counts all Scene nodes (roots and children)
plus one owned-root record per root. Metadata cannot outlive its root. A
reservation covers each root's maximum linear child count so delayed op88
births cannot exhaust the Scene behind an already accepted root. Root357
reserves three nodes (one root, up to two child356 nodes) plus one owned-root
record: four units, so at most1024 simultaneous actual cancellation trees.
Each83/84/175 root reserves one node plus one root record: two units, so at
most2048 childless roots. Mixed reservations obey `4*C + 2*T <=4096` for the
actual bank, whereC is cancellation roots andT is known childless roots.
Repeating child creation, detached children and child-of-child scripts are not claimed.
Each VM step has a 4096-instruction limit. Scene's own depth/node guards
remain active. Invalid spawn arguments/budget requests do not mutate active
effects. Execution failures latch an error and clear the owned Scene rather
than publishing partial/faulted effect trees; clear/reload is explicit recovery.

RNG callbacks forward to the stored callable, preserving mutable closure
state across root and child births/ticks. There is no seed or display source
fallback. Exception-enabled builds turn callback throws into a bounded,
latched error and suppress later source calls in that step. No-exceptions
builds compile no catch syntax and require nonthrowing caller callbacks.
Mutation from a callback is rejected; snapshots during execution return no
partially constructed tree. Shared RNG/script side effects are not rolled
back on failure.

## Native evidence boundary

The original request's 357-to-360 relationship does not match the actual
hash-pinned ANM bytes. Script357 (source ID and ordinal both357) contains two
op88 operands `64010000`, i.e.356. Script361 contains `68010000`, i.e.360.
Native type8/color10 cancellation still selects root357. Child356 is green
(op52 RGB 0/255/0); child360 is yellow (192/192/0). Both select sprites
580..587. The production component follows357-to-356 without changing ANM
operands. A separate real Scene regression verifies361-to-360, including
its different tint and complete cleanup; it is not exposed as color10.

The fixed original EXE is read only and never launched. Primary-tail root
and child membership is now directly supported by TH128 bytes, not inferred
from the different registration function addresses. Existing Scene saved-
successor traversal details, geometry ancestry and parent-retirement semantics
still include inherited TH12 reconstruction. This component exercises that
implementation without claiming full native-frame or cross-component global
ordering equivalence.

Native hit cancellation's 0x40d2a0 queues the short IRQ and may call a VM
custom IRQ callback; it does not by itself execute an immediate ANM tick.
That bullet IRQ remains the main worker's responsibility. Cancellation357
is a separate Scene-root effect, not the hit bullet's primary VM.

Native 0x408aec spawns the cancellation and 0x408af3 resolves its handle.
0x408b4b calls 0x40d1a0 after separate velocity/clock float stores, then a
multiply by native double0x4a6738 = **10.0**, not0.5. New bounded fixtures
execute that original scalar prefix and, separately, the original initializer
with declared zero/start/target/VM memory. The initializer takes EDX From,
ECX To, duration30/mode6, and copies the declared global zero vector into
curve-slope fields. These facts support only the guarded initializer below,
not an inferred camera meaning, generic runtime-global state, or full native
mode6 curve/FPU/frame equivalence.

## Retained original bytes and interpretation

`local/research/cancellation-static.json` retains the 0x4b2dd0 profile bytes,
original EXE code ranges and per-range SHA-256, raw ANM commands for scripts
356/357/360/361, and the original double at 0x4a6738. EXE SHA-256 before and
after was `ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.
Japanese bullet.anm SHA-256 is
`9860822687f33e740f2a5e1fb25a4b05cf27b0fc6d15ed9626fb903986f7784c`.
These are file/static observations, not a native CPU/event trace.

| Observation | Original bytes / location |
|---|---|
| Type8/color10 cancellation script357 | Profile +0x10+10*16 = +0xb0, `65010000` |
| Actual357's two child operands are356 | Script357 byte offsets44/88: op88 payload `64010000` |
| Actual361's two child operands are360 | Script361 byte offsets44/88: payload `68010000` |
| Child356 is green | op52 payload `00000000ff00000000000000` |
| Child360 is yellow | op52 payload `c0000000c000000000000000` |
| Resolve spawned cancellation handle | 0x408af3: `e888ec0500`, call0x467780 |
| Explicit native interpolation call | 0x408b4b: `e850460000`, call0x40d1a0 |
| Queue VM short IRQ | 0x40d2b1: `6689bef8030000`, then `c3` |

For zero-valued game RNG, actual357's single bind tick creates both356
children: ten game-source reads, root clock -39, child clock1/ticks1. Each
child selects actual sprites580..587 at times0/5/10/15/20/25/30/35 and deletes
at40. Two additional scale RNG reads occur at child time20. With each caller
iteration doing one primary pass, roots and children retire at the40th pass
and are collected through existing Scene.collect.
The regression checks every primary pass, actual tint/mode/layer/metadata,
explicit root-local mode6 interpolation and root/child physical translation.

Interpolation requests accept finite local XYZ, duration0..4096, and scalar
curve modes0..6. Duration0 uses VM's existing disable-without-target-write
semantics. Preflight validates selected-program sprites/colors/modes and
rejects literal-destination script mutation, including incorrectly typed
opcode5 destinations. Unsupported reload is rejected before replacing Scene
registry/ownership. Unsupported execution is bounded by the VM budget and
clears/latches the effect service instead of drawing a partial tree.

## Primary-domain correction

The original task and the first design assumed root357 belonged to secondary8.
That assumption is withdrawn: **head versus tail insertion is not a list
domain distinction**. Direct disassembly establishes:

- Root factory0x407ec0 binds at0x407f65 (call0x4593b0), then calls0x466d60
  at0x407f70. The latter appends to primary manager +0x8b9704 head /
  +0x8b9708 tail. 0x466de0 inserts at the head of that same primary list.
- Op88 handler0x45a32f pushes registration flags0 at0x45a33f, then calls
  child factory0x467220 at0x45a351. That factory chooses0x466d60 for flags0
  at0x4672d9. This is also primary-tail, not secondary.
- The primary manager pass is priority30. Neither cancellation roots nor
  these op88 children are advanced by a secondary8 call in this service.

The new actual-bank phase regression was observed RED with the old secondary
root, before the membership/pass-only repair. For a root/children all bound
before the host's primary30 pass, startup is tick1 and that same-cycle pass
is tick2 for each. A separate delayed-birth case exercises Scene's saved end
successor: a lone tail root's children created inside its pass receive only
their synchronous tick1 there, then tick2 on the next primary pass.

`tick` and `tickPrimary` each execute exactly one primary pass. They are
alternative names, not two scheduler registrations; without a frame/pass
identifier the component does not invent deduplication of two caller calls.
No secondary compatibility method was needed. Native head/tail membership
is proved here. Known83/84/175/357 births and356 children are now deliberately
unified in one primary-tail traversal: there are no per-script-group passes.
Interleaving with external Enemy-head roots or additional unsupported resource
families, and whole native cross-system frame equivalence, remain unproved.

## Guarded native drift initializer

Call `initializeNativeDrift(handle, velocity, 1.f, {0.f,0.f,0.f}, error)`
immediately after a successful spawn, before its first host primary pass.
It requires explicit f32 velocity, exact clock1, positive-zero start words,
positive-zero root script XYZ, root tick count1 and no existing interpolation.
Each axis stores `float(velocity*clock)` separately, then
`float(double(stepped)*10.0)`. Only after all axes validate does it call the
existing `VM.interpolatePosition(targetXYZ,30,6)`. It does not tick, consume
RNG, change engine/world position, or create nodes. Reinitialization, stale
handles, nonzero/-zero global starts, non-fresh/nonzero root positions,
unknown clocks, non-finite velocity and target overflow are rejected without
changing the current track. Arbitrary-context drift remains unsupported.

`local/research/cancellation-drift-fixtures.json` records four input/golden
cases, including float-rounding-sensitive velocities and signed zero. Each
case is **two separate** original-byte Unicorn executions with FPCW0x037f:
0x408af8..0x408b4b stops before the native helper call; 0x40d1a0..0x40d286
executes through its real ret8 with declared synthetic VM/global/target data.
No external call is given a fake return and no Windows game process runs.
The original initializer's stores and untouched bytes outside its interpolator
are checked. This proves scalar-prefix/initializer goldens in that context,
not generic curve stepping or a composed hit/frame trace.

The C++ regression consumes those native prefix target words, verifies all
guard failures and the no-extra-tick/RNG contract, and checks each of30
logical primary passes against a separately initialized existing-VM track
using those original-byte targets. That comparison proves delegation to the
shared logical VM, **not** native curve/FPU equivalence.

Regenerate and verify with the existing local reverse dependencies on
PYTHONPATH (`../../work/reverse-libs`):

```powershell
python tools/research/cancellation_effect_probe.py --exe "E:\gee\妖精大战争\th128.exe" --output local/research/cancellation-drift-fixtures.json
python -m unittest discover -s tests -p test_cancellation_effect_probe.py -v
```

## Pre-extension verification, 2026-10-29

- Feature-absent, typed-destination/negative-mode, combined-node/owner-budget,
  primary-domain and guarded-drift API regressions were observed RED before
  their corresponding repairs; the current focused set is GREEN.
- `node --test tests/cancellation-effects.test.mjs tests/bullet-visuals.test.mjs`:
  **8 passed**, zero failures/skips. This is focused/adjacent verification,
  not a full runtime/gameplay integration or native-frame comparison.
- `TH128_CANCEL_UBSAN=1` with the cancellation test file: **5 passed**, zero
  failures/skips, using undefined-behavior sanitization and exception catching.
- `TH128_CANCEL_NOEXCEPT=1` with the same file: **4 passed**, zero failures,
  **1 intentionally skipped callback-throw case**; production code compiles
  with `-fno-exceptions` and requires nonthrowing callbacks in that mode.
- `python -m unittest discover -s tests -p test_cancellation_effect_probe.py -v`
  with existing reverse dependencies: **2 passed**, exercising four original-
  byte scalar-prefix/initializer cases without running the Windows game.
- Manual author review checked callback lifetimes/stateful source forwarding,
  failure publication, ownership reservations/cleanup, copied metadata/state,
  primary-tail scheduling, fresh-root drift guards and separate-f32 targets.
  No independent review agent was launched; the main worker retains its
  independent review/integration responsibility.

This was the verified pre-extension freeze. The following unified-tail change
supersedes its root-script allowlist and cross-tail grouping limitation, but
does not change public method signatures, primary30 membership or drift bounds.

Only nine allowlisted new source/test/report/probe/fixture files were written.
EnemyVisuals stayed frozen and untouched; StageWorld/runtime/site/shared ANM/
BulletVisuals/TransientEffects/player-hit files were not edited. No agents,
branch, commit, original-game process or writes to driveE were performed.

## Unified known-tail extension

Keeping separate175/83/84 and357/356 services would change traversal when a
delayed357 root creates a child before other live tail roots. This is fixed at
ownership/scheduling level: `spawn` accepts83/84/175/357 in the **same Scene**,
and one `tick` traverses its Primary list once. The shared game RNG callable
and Scene's synchronous birth/saved-successor behavior are retained, not
approximated by per-kind age or sprite logic. Root reservations are selected
by script; child creation is still forbidden for83/84/175. Load preflight checks
all five involved programs transactionally, including typed sine/cosine
destinations now needed by the actual175/84 scripts.

The regression's controlled mixed-script programs make ordering observable:
four root births consume game32 results0/1/2/3; the next pass creates a delayed
child with result4, advances the interleaved83/84/175 roots with5/6/7, then
reaches that appended child and gives it8 on its same-pass tick. The next
single primary pass produces9/10/11 for those roots and12 for the child. A
grouped transient-then-cancellation or cancellation-then-transient traversal
fails these literal expected values even when the total RNG count matches.

The actual-bank mixed test spawns357,84,175,83,357,84 in creation order with a
shared deterministic source that defers357's first child. After each birth and
64 whole primary passes it compares node ownership, counts, script/sprite,
ticks, clocks, integer state, colors, layers and selected f32 pose fields to
one direct existing Scene with the same births/source. This is a scheduler-
adapter regression against the existing logical Scene, not a native full-frame
claim. No test draws a sprite by age or substitutes a fake callback result.

Another actual-bank regression creates one83 root plus32 actual84 particles,
alongside175. With zero callback outputs175 consumes one game32 callback
(do not confuse the RNG engine's underlying16-bit draw counter with this
callback count),83 consumes none and each84 consumes four at bind:129 total.
The graze root retires at primary pass12; the32 particles at28 for their
duration20-plus-wait8 path;83 at30. All33 hit roots and their owner records
are gone, retained snapshots remain readable, and later passes consume no
additional RNG. Physical screen coordinates are accepted without another
320/16 translation. Guarded native drift remains357-only.

## Unified final verification and freeze, 2026-10-29

- Fresh pre-extension focused/adjacent baseline: **8/8 passed**. The added
  unified-root/global-tail regressions were then observed RED because83/175
  were rejected by the old root allowlist, before the bounded extension.
- Current `node --test tests/cancellation-effects.test.mjs tests/bullet-visuals.test.mjs`:
  **9 passed**, zero failures/skips. The cancellation file contains6 tests,
  including literal mixed-tail RNG order, actual mixed Scene state, per-root
  reservation capacity,357-only drift and33-hit-root retirement coverage.
- Current `TH128_CANCEL_UBSAN=1` cancellation file: **6 passed**, zero
  failures/skips. Current `TH128_CANCEL_NOEXCEPT=1`: **5 passed**, zero
  failures, **1 intentional callback-throw skip**.
- Original-byte drift probe tests were rerun after the extension:
  **2 passed**, unchanged prefix/initializer goldens and original EXE hash.
- No public method signatures changed. The existing class name is retained
  as requested; all known83/84/175/357 roots and356 children now share its
  single Primary-tail Scene, one host pass and the stored game RNG callable.
- This extension changes only CancellationEffects.hpp/.cpp, its two test
  files and this report. No EnemyVisuals or parent/shared runtime files were
  edited. No native game process, agents, branch, commit or driveE writes.

**Frozen for the main worker's pinned-source build and Jason's independent
review.** No further source edits or builds are planned from this task.
Parent integration must replace both per-kind runtime services with this one
instance; do not tick the standalone TransientEffects class alongside it for
these roots. The standalone class/unit tests remain unchanged. Whole native
frame equivalence and interleaving with unsupported external families are
still not claimed; the requested known-tail ordering problem is implemented,
not left as an unsupported grouping disclaimer.
