# TH128 bounded pre-animation bullet update

## Implemented boundary

### Current Integrated Extension

The latest native-byte matrix is84 prefix cases,17 no-effect resumes and6
independent resolved-hit tails (107 total), not the older73/78/95 totals retained
in historical verification notes. Prefix fixture SHA256:
`7cd96410986961d89c4120dba9a2d6d935757604424ad7592e293e6f82d579fb`;
resume fixture SHA256:
`b2d5d6a897e2f7748dddd5d254d81c115f4b7fb244e488cf2936bbd63481f575`;
hit-tail fixture SHA256:
`e9f78265ab3396919c0ca54c9be1d9c54058636d2f6d1de3836fe9cb6a143eb2`.

Motion state3 is the actual post-hit half-speed state, without repeated collision.
`resumeAfterResolvedHit` requires a pending state1/2 collision and genuine completed
hit/cancellation/IRQ obligations; it changes to3 and runs only the native tail,
not an additional movement or reset of already executed timers. The component
alone does not authorize skipping those mandatory effects.

`src/game/BulletTickRules.hpp/.cpp` implements a reusable pure **pre-ANM
decision and next state**, not a complete bullet tick. It follows the actual
per-bullet body at `0x408640`, stopping at `0x408d09` before the first ANM
call, before an unimplemented collision/extension call, or after a proved
reset/removal return. It never executes or substitutes an animation VM,
collision decision, nonempty extension dispatcher, Windows API or allocator.

The existing `BulletVisuals.hpp/.cpp` remain frozen and unchanged. This
component only includes the existing `BulletRules.hpp` for the semantic
`Position` type; it needs neither `BulletRules.cpp` nor `BulletVisuals.cpp`
linked. Manager/world/export/build/site integration belongs to main.
Rectangle/circle collision decisions belong to the independent collision
component and are deliberately not duplicated here.

## Public API and state

All types and the function are in `th128::bullet`:

```cpp
enum class Continuation { NeedsAnimation, NeedsCollision, NeedsExtension, Removed };
struct PreAnimationTick {
    TickInput next;
    Continuation continuation;
    uint32_t boundary;
    int32_t previousLifeClock;
};
std::optional<PreAnimationTick> preAnimationTick(const TickInput&) noexcept;
std::optional<PreAnimationTick> resumeAfterCollisionMiss(const PreAnimationTick&) noexcept;
```

`TickInput` has `Position position, velocity`, `type`, `motionState`, `age`,
`lifeClock`, `lifeTime`, `integer0`, `flags`, `activeExtensions`, `grace`,
`cooldown`, `hasSprite`, `spriteWidth`, `spriteHeight`, `clock`, and
`extensionListEmpty`. `clock` means the time-scale input, not bullet age.
The supported exact time scale is 1.

Relevant original fields are:

| Semantic field | Native bullet offset |
| --- | --- |
| flags / cooldown | `+0 / +4` |
| sprite pointer / control integer0 | `+0x430 / +0x438` |
| position / velocity | `+0x9d8 / +0x9e4` |
| grace / active extension flags / motion state | `+0xa18 / +0xa20 / +0xa2a` |
| manager-owned age | `+0x1188` |
| body life timer's previous integer / current integer / float | `+0x1198 / +0x119c / +0x11a0` |

`previousLifeClock` is the observed `+0x1198` after the prefix. `next.age`
is **not incremented** on a continuing body path. The manager, not this
function, advances age after the body finishes successfully. `Removed`
reflects the original reset, which zeroes age and both current life-timer
values, writes previous life integer -1, sets motion state 0 and clears
flags bit 0; position, velocity, grace and cooldown are retained.

`extensionListEmpty` is an explicit caller declaration, defaulting to false.
For the supported first-emitter case it means the pending extension entry
after consumed slot 0 is a zero sentinel (probe uses index 1). Only when
that declaration is true **and active flags are zero** does the portable
normal-state path pass the proved empty dispatcher. Otherwise it reports
`NeedsExtension` before `0x409a90`, without inventing its return. It is not
a claim that arbitrary zero-active-flags bullets have no pending ETEX.

The native sprite's horizontal size is read at pointer `+0x38`, and vertical
size at `+0x34`, so the probe stores `spriteWidth`/`spriteHeight` in that
order. The static sprite binding slice `0x4592b5..0x4592c1` copies those
fields into VM `+0x60/+0x64`. These are actual unscaled sprite dimensions,
not hitbox dimensions and not dimensions multiplied by VM scale. A missing
sprite pointer is modeled explicitly with `hasSprite=false`.

## Exact sequencing retained

1. The body saves the old `lifeClock`, increments it, and separately adds
   1 to float `lifeTime`, before removal checks and movement. It does not
   derive one timer representation from the other.
2. Flags bit 8 takes the original immediate reset/removal path.
3. Active flag `0x400000` encounters an unimplemented scale-extension helper
   before motion and stops at call `0x4086d9`.
4. Motion state 2 moves at **half velocity**. Each component is stored to
   float32 before later operations.
5. State 2 checks `age >= 8` before its collision call when flags bit 2 is
   set. This precedes reading the control integer gate. In particular,
   nonzero int0 cannot skip an age-8 collision call.
6. Nonzero `integer0` changes state 2 to 1. The normal path can then perform
   empty dispatch and **another full-velocity movement in the same tick**.
   State 1 input enters this normal path directly.
7. State 1's collision call has no age-8 gate. Collision results are not
   supplied by this component.
8. With a sprite pointer, active bit `0x1000` encounters another unsupported
   helper at `0x408caa`. Active bit `0x100` skips bounds checking. Otherwise
   bounds are checked when grace is already <= 0, using the moved position
   and actual sprite size.
9. A nonzero cooldown decrements, even when negative. Grace decrements only
   while positive. Checking grace happens **before** this decrement: grace
   1 skips bounds on that body step, becomes 0, and is checked next time.
10. A surviving prefix stops at `0x408d09`, before its LEA and the ANM call
    at `0x408d0c`.

Half movement followed by full movement is not folded into `1.5 * velocity`.
The golden case starting X=16777216, velocityX=1 demonstrates the distinction:
the two actual float stores leave X unchanged, whereas one combined move
would produce 16777218. Signed zero, unequal sprite dimensions, exact bounds
edges and neighboring representable positions are also exercised.

Bounds helper `0x408560..0x4085ca` returns outside for:

```text
x + width/2 <= -192  or  x - width/2 >= 192
y + height/2 <= -64  or  y - height/2 >= 448
```

Thus touching the outer edge is already outside. Z does not participate.
This is offscreen lifetime/bounds behavior, not collision with the player.
Outside invokes actual reset helper `0x407fb0..0x408055`; those cases return
from the body with EAX=-1 without executing the cooldown/grace tail or ANM.

## Continuation contract

| Continuation | Boundary and meaning |
| --- | --- |
| NeedsAnimation | `0x408d09`: normal body remains incomplete; no ANM tick or body return is invented. |
| NeedsCollision | Actual call site, before `0x43c410` or `0x43c570`; next position/timers are prefix state only. |
| NeedsExtension | Actual scale, dispatch or late extension call site, before that unsupported call. |
| Removed | `0x4086c6`: actual early/bounds reset path has returned EAX=-1. |

State-2 rectangle sites are `0x4087b4` / scaled `0x4087f0`; the circle site
is `0x40880f`. State-1 counterparts are `0x408a44` / `0x408a80` / `0x408a9f`.
These classifications expose the handoff without reproducing collision
geometry or collision side effects. Continuation enum numbers are **not**
native EAX return values.

Do not feed a collision/extension `next` back into `preAnimationTick` as a
way to resume the same body: that would restart its timer and motion. Use
the bounded miss continuation below only after a verified zero/no-effect
collision result; extension and side-effect paths remain stopped.
For `NeedsAnimation`, main may invoke the actual
visual host in native primary/secondary order, but must still account for
native animation-ended removal before declaring body success and advancing
manager age. `BulletVisuals::advance` returning true only means no VM error;
it does not itself assert that the original body returned zero.

## Bounded collision-miss resume

`resumeAfterCollisionMiss(pending)` resumes only a validated
`NeedsCollision` value at one of the six original call sites. It does not
call collision code or accept/invent its return value. **The host must first
obtain a verified no-effect helper result**, for example the collision
component's `UpdateIntent::HelperReturned` with helper return 0 and outcome
Miss or Suppressed. HitReturn, GrazeReturn, DamageCallRequired,
HitEffectsRequired and GrazeCallRequired do not authorize this helper.
A positive player cooldown alone is not a no-effect decision: an original
helper can still return 1 and require hit effects. This function's input
shape cannot authenticate an external collision result; the check belongs
to the host and is an explicit API precondition.

- A half-step collision miss resumes at the integer0 gate, without repeating
  half motion, the life timer, or age. Nonzero int0 can then enter the normal
  path, execute the proved empty-dispatch semantics, perform full movement
  once, and return **a second NeedsCollision** at the full-step site.
- A verified miss at that second site (or a state-1 input's first site)
  resumes only the tail: bounds, cooldown and grace. It does not repeat full
  movement or any timer increment.
- Bounds removal and extension handoffs remain possible on resume. No hit,
  graze, damage, ETEX or ANM side effects are silently supplied.

The resume guard checks continuation kind, call site against state/route,
half-step age, collision-enable flags, supported counters/fields, and known
normal-path empty-extension preconditions. Default, removed, animation,
unknown-site and malformed pending inputs return null. Counter provenance
requires `next.lifeClock == previousLifeClock + 1`, with the previous value
in 0..4096; pending current value 4097 is therefore supported. Arithmetic
guards reject malformed INT_MAX counters before addition. Moved float32
positions may be smaller than the input minimum or reach three times the
input magnitude budget; resume accepts those finite reachable outputs,
instead of applying fresh-entry validation to already executed movement.

Native evidence is separate in
`local/research/bullet-tick-resume-fixtures.json`, schema
`th128-original-byte-bullet-tick-resume-fixtures/1`, **17 resume cases**,
SHA256 `b2d5d6a897e2f7748dddd5d254d81c115f4b7fb244e488cf2936bbd63481f575`.
These execute the **actual rectangle/circle helper bytes** with explicit
miss/suppression player inputs, reusing the collision worker's existing
probe data builder/constants without editing its files or reproducing its
geometry. Measurements stop at the real helper return sites and verify
EAX=0 from execution. There is no synthetic return stub or register override.
Suppressed cases use the actual native state=2 suppression gate.

For a half-to-full transition, one native run stops at the second unexecuted
collision call; another executes that helper too, verifies its real zero
return, and reaches the tail. The C++ resume results are compared bit for
bit with those native-byte prefixes. All six collision call sites,
second-collision handoff, counter edge 4097, removal, extension stop,
suppression and final tail counters are covered. A later integration correction
expands the original73-case prefix matrix to78; see the motion-domain note below.

## Scheduler priority and age evidence

These are **static original-byte facts**, not a trace of 2000 executing
bullets:

- `0x408241` loads EBX=24; `0x408246` stores callback `0x4091a0`; the
  registration call at `0x408253` targets `0x4681d0`.
- The known enemy registration at `0x4153ae` loads EBX=21 and calls the same
  registration helper at `0x4153c0`. Thus bullet priority is 24 versus the
  enemy's 21; no guessed priority is introduced by this component.
- Callback `0x4091a0` calls manager `0x408d50` at `0x4091c1`, after its real
  pause/control gate. This probe does not stub that gate.
- The manager sets a 2000-slot counter at `0x408d8d`, starts at manager
  `+0x64`, calls body `0x408640` at `0x408dbf`, and advances ascending slot
  addresses by `0x11b8` at `0x408e48`.
- A nonzero body result branches to the next slot before age handling.
  The near-one age increment is at `0x408e22`, followed by the store to
  bullet `+0x1188`. It occurs after successful body completion, including
  animation, not at entry to this pre-ANM prefix.

The Node evidence test verifies immediates and relative call targets from
the saved bytes, including the two registration calls, callback-to-manager,
manager-to-body, slot count and stride.

## Probe, provenance and numerical limits

`tools/research/bullet_tick_probe.py` reads only the pinned Japanese EXE,
SHA256 `ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.
It executes copied original instructions in bounded Unicorn memory, not a
Windows game process or native Windows CPU session. No E-drive output,
host API stubs or invented call results are used. The report preserves
`nativeCpuTrace=false`, `windowsGameProcessLaunched=false`, and matching
before/after EXE hashes.

Private report `local/research/bullet-tick-fixtures.json` has **78 cases**:
58 NeedsAnimation, 8 NeedsCollision, 5 NeedsExtension and 7 Removed. Its
SHA256 is `24e25984dc661593dc420f6471beab2064b3257a6264bf4c3c574d5e269652bd`.
The body slice `0x408640..0x408d09` hashes to
`a7839c89c7b54cb9f2644f2caeccbbad5e62f0ffc94920337cae530330aac760`.
The report contains hashed helper/static spans and constants, declared code
spans per case, bounded instruction traces, input/output memory digests,
float-bit vectors and extracted actual prefix state.

The empty-dispatch proof executes the actual dispatcher entry, zero-sentinel
exit and security-cookie check `0x4728fd`; it is **not** a fabricated zero
return. The cookie is a declared read-only input copied from the EXE, not a
claim that native process initialization ran. Cases without the explicit
empty-list declaration stop at the call rather than include that helper.
Unknown calls outside the supported classification fail the probe.

Input buffers are explicitly initialized synthetic semantic states, not
captured whole-game snapshots. The probe verifies declared state against
its packed native input before executing. A regression caught an initial
control-integer packing error; the final probe stores int0 at `+0x438` and
tests its preservation. That erroneous intermediate fixture is not retained.

Bounds are enforced before EXE access: exact three-component vectors;
type8 and motion states1/2; age/life integer 0..4096; signed grace/cooldown
-4096..4096; uint32 flags; int32 int0; exact booleans; exact float clock1;
nonnegative life float/sprite dimensions. All inputs are finite and have magnitude
at most2^24. Scalar lifetime/sprite fields must additionally be zero or have
magnitude at least2^-16. Tiny extents outside that
domain are rejected rather than claim equivalence for x87-vs-double edge
comparisons. Float32 stores and separate movement stages are retained.
Position/velocity components now accept tiny normal/subnormal floats: real
motion produced a cancellation residue near2^-22, and a vertical aim can produce
velocityX near-4.37e-8. Rejecting those values stopped the next valid update.
Five original-byte tiny-motion/subnormal vectors and a failing-then-passing
integration regression prove the bounded relaxation without clamping or changing
the separate half/full float32 stores. The stricter collision numeric domain is
unchanged; callers must still explicitly stop if collision inputs exceed it.
Only the existing emulator's declared x87 environment is exercised; this
is not exhaustive proof for every float/FPU mode or later bullet state.

The report is capped at 256 cases, each at 2048 instructions and the existing
500000-microsecond timeout. Existing code/data/stack span budgets remain
enforced. No ANM call at `0x408d0c` or `0x459d90` appears in a case trace.

## Verification and scope

```powershell
$env:PYTHONPATH = '../../work/reverse-libs'
$env:TH128_BULLET_EXE = 'E:\gee\妖精大战争\th128.exe'
python -B tools/research/bullet_tick_probe.py --exe $env:TH128_BULLET_EXE
python -B -m unittest discover -s tests -p test_bullet_tick_probe.py -v
node --test --test-concurrency=1 tests/bullet-tick.test.mjs tests/bullet-visuals.test.mjs
python -B -m unittest discover -s tests -v
```

TDD RED was observed for missing rules/probe, missing tiny-float rejection,
and incorrect control-integer fixture packing before their fixes. Final
configured probe tests pass **4/4**. Combined current tick plus unchanged
visual tests pass **7/7**, including all 73 prefix and 17 resume vectors in
ordinary WASM and UBSan builds with exceptions disabled, plus actual
script43 startup. Broader Python discovery runs **214 tests: 137 passed,
77 skipped** in its
default opt-in configuration. Native-game-CPU execution is never claimed.

Generated binaries/logs stay in `work/bullet-tick-test`; only the requested
new source/test/probe/doc files and two private reports are deliverables. No frozen
BulletVisuals, collision-worker, manager, world, build, site, Git or E-drive
file is edited by this component. This is an implemented, tested handoff
for real pre-collision movement and animation boundaries, not a claim that
the complete bullet update, collisions, ETEX or full TH128 port is finished.

Review-ready source identities (no further source edits pending):

- `src/game/BulletTickRules.hpp` SHA256:
  `e69069f799faa476df71c945007ebca2c93a85623912fc9f778eec22448af0eb`.
- `src/game/BulletTickRules.cpp` SHA256:
  `44da3966846c9f95fc35f5df3b31008d4aec87ba50b4435f0b8f27d1e59cb674`.

The previous frozen BulletVisuals header/implementation remain respectively
`add61feb628c04f567031be7e4f72d0b881201ddaa0a82968b7d8e8856d6f368`
and `1a0f3980ad1d91e0a491ca8d5286ee39b9680b2ced087c0fc66e06e425f77e25`.

Independent focused review after integration corrections: no openCritical/Important
finding. Actual measured helper centers replace the earlier pre-motion metadata;
all17 now match pending positions. Both output paths/distinctness are validated
before probing/writing and both reports finish generation before either write.
Scheduler metadata is qualified: when body executes, only its successful return
reaches age advancement; control bit0x400 can bypass the body and still handle age.
Fresh review replayed all95 vectors in normal/UBSan builds and all6 probe tests.
