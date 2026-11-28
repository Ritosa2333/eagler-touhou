# Bounded TH128 enemy rules

Date: 2026-10-29. This is a component result, **not full enemy behavior or
first-stage completion**. No ECL VM, StageWorld, runtime, build or site source is
owned or modified by this worker. Global getters/RNG and shooter/rank operations
are other workers' responsibility.

## Evidence and reproduction

- Original read-only input: Japanese x86 `E:/gee/妖精大战争/th128.exe`, SHA-256
  `ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.
- Actual Japanese script is resolved through `local/catalog.json` and its `ja`
  manifest parent, not a guessed generation. Current `raw/st_a1_1.ecl` SHA-256:
  `181aa57ef1581ef19340b283550dd140982ea0b1c12290a5f8957eec2a3f7b35`.
- `tools/research/enemy_probe.py` checks the EXE hash before PE parsing, reads
  dispatch-table entries from that PE, executes only enumerated original-byte
  chunks, and records all registers, explicit data before/after as bytes and
  I32/U32/F32, instruction traces, stack arguments and exact stop reasons.
- The sandbox follows the existing freeze probe's strategy. Its additional
  bounded stack snapshots observe the real prepared spawn structure; they do
  not patch code or synthesize return values. Code is read/execute-only, only
  explicit data spans are readable, stack is bounded, and unlisted direct or
  indirect calls stop **before** execution. Maximum is 200000 instructions per
  probe; these original-byte cases use a 4096-instruction limit.
- Emulator: Unicorn 2.1.4, Capstone 5.0.9, pefile 2024.8.26. Synthetic x87 control
  word `0x037f`, MXCSR `0x1f80`. Motion probes explicitly set time scale to 1 and
  CRT SSE2 availability to 1, and execute the original SSE2 floor routine.
- **Instruction emulation, not Windows/native-CPU validation.** No original
  game/installer, OS API, allocator or executable process is launched. No E:
  destination is written. Zero initial register/stack state and test object
  graphs are explicit synthetic inputs, not inferred original game state.
- TH12 MIT `EclInterpreter.cpp` and `Movement.cpp` were read as search hints
  only. No TH12 address or movement quantum was adopted: this executable's
  quantization constant is **100**, not 128.

PowerShell, from this deliverable:

```powershell
$env:PYTHONUTF8='1'
$env:PYTHONDONTWRITEBYTECODE='1'
$env:PYTHONPATH='../../work/reverse-libs'
$env:TH128_ENEMY_EXE='E:\gee\妖精大战争\th128.exe'
python -B tools/research/enemy_probe.py --exe $env:TH128_ENEMY_EXE --libs ../../work/reverse-libs
python -B -m unittest discover -s tests -p test_enemy_probe.py -v
node --test tests/enemy-rules.test.mjs
```

Private `local/research/enemy-fixtures.json` currently contains **63 cases**:
50 handler/caller boundary stops, 6 returned helpers and 7 intentional unsupported
allocation-call stops. There are no accepted faults, outside-code/data accesses
or exhausted budgets. Python tests enforce each case's expected stop class.
Static-only contexts are explicitly marked `executed: false`.

The C++ test uses the exact SDK/Python/cache configuration of
`tests/native-random.test.mjs`: Emscripten 4.0.22, bundled Python 3.13.3,
forward-slash `EM_CONFIG` and `EM_CACHE`, `EMCC_CORES=2`, `BINARYEN_CORES=2`.
It adds `-ffp-contract=off`, emits a single-file test build into the allowed
`local/research/enemy-native.json`, wraps that JavaScript in JSON, and executes
it with Node. Compiler temporary/cache files are SDK-managed. No runtime or
site output is rebuilt by this test. The native fixture checks synthetic edge
cases and **every one of the 63 original-byte cases**, comparing all selected
integer/float fields bit-for-bit, not with a tolerance. Private evidence absence
is a named skipped test, not a fabricated golden pass.

## Dispatcher and field bases

At `0x418030`, `ECX` is copied into handler `EBX`; the dispatcher follows
`[EBX+0x3f7c] -> runner`, `[runner+4] -> context`, `[context+4] -> instruction`.
Opcode is the signed 16-bit word at instruction `+4`, minus 256. It accepts an
unsigned index through `0x167` (359), selects an 8-bit index at `0x41c7c0`, then
jumps through the 32-bit handler table at `0x41c554`. Common exit: `0x41c52c`.

**Do not add the following state offsets to the full Enemy object directly.**
The dispatched embedded state base is `Enemy + 0x11c8`; the full-object offsets
agree with the original constructor and independently recovered enemy getters.
Global variable-ID binding is deliberately not duplicated here.

| Meaning | Dispatched state offset | Full Enemy offset |
|---|---:|---:|
| Combined XYZ | `+0x34/+0x38/+0x3c` | `+0x11fc/+0x1200/+0x1204` |
| Combined displacement XYZ | `+0x40/+0x44/+0x48` | `+0x1208/+0x120c/+0x1210` |
| Absolute XYZ | `+0x68/+0x6c/+0x70` | `+0x1230/+0x1234/+0x1238` |
| Absolute displacement XYZ | `+0x74/+0x78/+0x7c` | `+0x123c/+0x1240/+0x1244` |
| Absolute speed / angle / mode flags | `+0x80/+0x84/+0x98` | `+0x1248/+0x124c/+0x1260` |
| Relative XYZ | `+0x9c/+0xa0/+0xa4` | `+0x1264/+0x1268/+0x126c` |
| Relative speed / angle / mode flags | `+0xb4/+0xb8/+0xcc` | `+0x127c/+0x1280/+0x1294` |
| Opcode 400 raw extent pair | `+0xd0/+0xd4` | `+0x1298/+0x129c` |
| Opcode 401 raw extent pair | `+0xd8/+0xdc` | `+0x12a0/+0x12a4` |
| 16 ANM handles | `+0xe0 .. +0x11c`, stride 4 | `+0x12a8 .. +0x12e4` |
| Current animation bank | `+0x220` | `+0x13e8` |
| Bound bank / script | `+0x224/+0x228` | `+0x13ec/+0x13f0` |
| Primary base script / selector | `+0x22c/+0x230` | `+0x13f4/+0x13f8` |
| Animation birth layer offset | `+0x238` | `+0x1400` |
| Spawn inherited opaque words | `+0x23c`, 12 words | `+0x1404`, 12 words |
| Absolute polar interpolator | `+0x338`, size `0x3c` | `+0x1500`, size `0x3c` |
| Enemy flags | `+0x3ee4` | `+0x50ac` |

The raw bounds are stored unchanged. Whether a later collision test uses half
sizes, radii or full widths is **not** proved by these setters; the API does not
divide or clamp them and does not implement collision.

## Public C++ contract

The complete signatures and plain value types are in `src/game/EnemyRules.hpp`,
namespace `th128::enemy`. There is no dependency on ECL VM, ANM VM or StageWorld.
All operations are `noexcept`. A false/empty result means an invalid host-domain
input or an explicitly unsupported branch; callers must diagnose it, not no-op
the opcode. Failed mutating operations leave their inputs unchanged.

### 400/401 and 402/403

| Opcode | Handler | Semantics |
|---|---:|---|
| 400 | `0x41a31f` | Resolve floats 0/1; store raw pair at state `+0xd0/+0xd4`. |
| 401 | `0x41a345` | Resolve floats 0/1; store raw pair at state `+0xd8/+0xdc`. |
| 402 | `0x41a36b` | Resolve int 0; OR state flags. If **resulting** bit `0x20` is set, visit all 16 ANM handles through `0x4678b0`. |
| 403 | `0x41a3a4` | Resolve int 0; AND state flags with NOT mask. If **resulting** bit `0x20` is clear, visit all 16 ANM handles through `0x467870`. |

`setBounds(Bounds&, width, height)` stores either pair; the host chooses which
one. `setFlags(flags, mask)` and `clearFlags(flags, mask)` return `FlagsChange`
with the complete unsigned flag word plus a required `VisibilityEffect`.
`Hide` means clearing native ANM flag bit `0x2`; `Show` means setting it. These
are level-triggered effects, **not just changes when enemy bit 0x20 toggles**.
Thus `setFlags(0x20, 0)` requests Hide and `clearFlags(0, 0)` requests Show.
The root's actual `main+20` instruction is `402(0x20)` and reaches the original
common-exit boundary, including real zero-handle lookup paths.

An explicit live synthetic ANM object proves the bit operation through the
original lookup and show/hide helpers. Static code also visits child-list ANMs
when their child-policy field `+0x18` is zero; that descendant graph is not a
golden fixture. Applying these effects to real host animations is host work,
not something the pure flag-word result can claim to have done.

### 258 and 262

- 258, handler `0x4188f9`: integer parameter 0 is stored at state `+0x220`.
  `setAnimationBank(AnimationState&, bank)` preserves that raw signed integer.
- 262, handler `0x418da3`: integer parameters are **slot, script**, not bank,
  script. Existing handle is retired by original `0x4679b0`. Birth arguments at
  the actual `0x418df0 -> 0x41f2a0` call are: `ECX = [enemyManager + 0x80 + 4*bank]`,
  `EAX = layerOffset + 7`, stack arg 0 = output-handle address, stack arg 1 = script.
- `animationSlot(animation, slot, script, flags)` returns an `AnimationRequest`
  with those interpreted fields, Hide if required, and `primary = (slot == 0)`.
  The layer is the **birth argument**; native birth only overwrites the ANM
  `+0x20` field when it is nonnegative. A negative value is not necessarily a
  final negative render layer. Bank/resource/script availability remains a host
  check; the helper cannot validate a resource table it does not own.
- The actual first `GirlA1b` selections are bank 1, slot 0, script 0. Zero old
  handles are retired using real bytes; all three ANM fixtures stop before the
  real allocating birth helper. No ANM return value is invented.
- Static-only post-birth suffix: store returned handle; obtain ANM with
  `0x467780`; slot 0 stores products `ANM[+0x64]*ANM[+0x44]` and
  `ANM[+0x60]*ANM[+0x40]` into state `+0x3e20/+0x3e24`; apply hidden bit; slot 0
  sets flag `0x200000`, stores script into `+0x22c/+0x228`, zeroes `+0x230`, and
  copies bank into `+0x224`. Request preparation is **not** proof of this full
  ANM lifecycle, these metrics, or native script execution.

### 304 and the first reached 305

- 304 handler `0x419467` selects absolute movement at state `+0x68`. Arguments
  are **float angle, float speed**. Strictly greater than `-999999.0f` updates
  that component; equal/below keeps it. After either speed path it clears low
  two mode bits even if both arguments kept their old values. Negative finite
  speed is not clamped. It does **not** cancel a pending 305 interpolator.
- Enemy flag `0x100000` reflects 304's angle using the original float32
  half-pi constant and normalizer. The angle setter `0x41f670` normalizes twice.
  Constants are widened float32 values `3.1415927410125732`,
  `6.2831854820251465`, `1.5707963705062866`, not mathematical pi. Normalization
  keeps both endpoints, stores float32 after each subtraction/addition, and has
  the original bounded-loop behavior. The public rule is `setPolar`.
- The first actual GirlA1b 304 has mask 1 and angle word **float -1**, i.e. a
  typed expression-stack pop, followed by literal speed 3. A separate probe uses
  the actual instruction bytes and a declared synthetic float expression result
  of 1.75. Original `0x46ff70` consumes 8 stack bytes and writes angle 1.75/speed
  3. This does not duplicate or replace the main worker's actual RNG proof.
- 305 handler `0x41954a` arguments are **int duration, int mode, float target
  angle, float target speed**. Actual first command is `(100, 0, 0.3926991f, 1)`.
  It selects polar state `+0x68` and interpolator `+0x338`. A nonpositive duration
  only writes interpolator duration zero; it does not clear movement bits or
  read/apply the mode. Positive mode 0 uses keep sentinels, optional reflection,
  then float32 shortest-angle setup. At absolute delta **greater than or equal
  to** native pi, add tau to the smaller endpoint. Begin does not immediately
  change angle/speed; it clears the low movement-mode bits and resets the timer.
- Interpolator offsets: start angle/speed `+0/+4`, target `+8/+0xc`, zeroed
  auxiliary pairs `+0x10/+0x18`, timer previous/current/float `+0x20/+0x24/+0x28`,
  scale pointer `+0x2c`, timer flags `+0x30`, duration `+0x34`, mode `+0x38`.
  Reset is previous=-1/current=0/float=0 with native scale-pointer/flag setup.
- `beginPolarInterpolation` implements that mode-0 setup and nonpositive early
  stop. Other positive-duration modes fail atomically. `stepPolarInterpolation`
  implements one **time-scale-1** call: increment first, so first result uses
  `1/duration`, not zero. The original evaluator `0x41eb00`/timer `0x402ac0` proves
  f32 ratio, f32 target-start, f32 multiplication, f32 addition, and exact final
  target when the integer timer reaches duration. It then deactivates duration.
  The movement caller's static suffix normalizes angle twice unless mode bits
  0/1 are set; speed is always stored. An inactive portable interpolator is an
  explicit known-state no-op. Scheduling/order relative to VM calls is not
  established by this helper.

### Bounded simple XY advancement

Two additive APIs are available for integration:

```cpp
bool advancePolarPosition(Position& position, Position& velocity, const Polar& polar) noexcept;
bool combinePosition(Position& combined, Position& velocity, const Position& absolute,
                     const Position& relative, std::uint32_t enemyFlags = 0) noexcept;
```

The native caller prefix `0x4162d3..0x4163f5` is emulated through original
`0x46ae00`, original x87 `fsincos` helper `0x46c1f0`, integrator `0x46ae80`,
combiner `0x416030`, and original SSE2 floor routine `0x48fdf0`. All 14 caller
cases reach the declared boundary after combining; there are no trig/floor
stubs. Absolute and relative position interpolators are explicitly zero.

At time scale 1 and mode bits 0/1 clear, velocity XY is f32 cosine/sine times
speed, Z velocity is zero. Position adds f32 velocity, rounds that sum to f32,
scales each XY by the original **double 100**, stores f32, floors downward,
stores f32, divides by double 100, then stores f32. Z is not quantized.
Negative -0.001 becomes -0.01, not zero. The combiner first stores f32
absolute+relative, f32 desired-old combined displacement, then uses the same
add-and-quantize integrator. It is not merely assignment of the sum; both the
intermediate roundings and additional combined-coordinate quantization matter.

The portable trig adapter is C++ double `std::cos/std::sin` with explicit f32
stores; it matches all selected original-byte fixtures bit-for-bit, including
signed pi endpoints, negative speed, near-zero motion and mixed relative motion.
That is **not universal libm/x87/native-CPU equivalence**. The helper restricts
angle to the canonical [-nativePi, nativePi] interval, rejects nonfinite values,
overflow and circular/ellipse mode bits, and does not implement acceleration,
Hermite/position interpolation, alternate time scales or transforms.
`combinePosition` rejects enemy clamp flag `0x40000` and transform flag
`0x08000000`. Callers must also reject those unsupported world paths before
calling the component helpers; it cannot see other world state. Position and
velocity output objects must be distinct. No host frame scheduler is implied.

### Static local update order, not a complete frame schedule

A bounded follow-up for the main worker records original call operands and
hashed disassembly in `localCallOrder` and `staticContexts`. It adds **no new
runtime scheduling function** and is marked `executed: false`,
`fullFrameScheduleProven: false`.

- Manager list traversal is `0x4158f0`, list head `[manager+0xb8]`. Call
  `0x415927 -> 0x416160` passes `Enemy+0x11c8`. It skips flag `0x04000000` enemies,
  saves next-node pointer before the call, and clears flag `0x00080000` after a
  zero update result at `0x415932`; other outcomes have deletion/skip paths.
- Entry `0x416160` tests state flag `0x00080000` at `0x416183`, branches to
  `0x41734d` when already set, otherwise ORs the flag at `0x41618e/0x416193`.
- Inside the ordinary entered path, absolute polar interpolator call is
  `0x416200 -> 0x41eb00`; relative polar interpolator call is
  `0x416273 -> 0x41eb00`, with additional interpolation paths between them.
- The already-probed XY prefix follows: velocity calls
  `0x416330/0x4163a5 -> 0x46ae00`, position integrator calls
  `0x4163b6/0x4163eb -> 0x46ae80`, combined position
  `0x4163f0 -> 0x416030`. ANM lookup, bounds and offscreen/removal branches follow.
- Paths reaching script execution then call `0x4164e1 -> 0x46fe20` with runner
  from state `+0x3f7c` and time step through state `+0x278`. That runner walks
  contexts at runner `+0x11b0`; call `0x46fe52 -> 0x46e100` runs the ECL core.
- Out-of-core opcodes use `0x46fbd4..0x46fbe5`: load context `+0x1014` host,
  load its vtable first slot, indirect call at `0x46fbde`. Actual vtable first
  slot `0x4a2704` contains **`0x417fe0`**. The thunk adds `0x11c8` to host ECX and
  jumps at `0x417fe6` to function `0x417ff0`; its body reaches dispatcher
  `0x418030`. Adjacent vtable slots agree with the main worker's native getters.
- The spawn constructor itself calls `0x415180 -> 0x416160` after assigning
  initial state. Thus it is unsafe to model birth merely as appending an entity
  for a future generic VM-first tick.

**Static conclusion:** on this per-enemy path, the interpolation/motion calls
precede that call's ECL execution. This does not establish the application-wide
frame phase, new-node insertion/visit timing, constructor guard state, callback
reentry, deletion ordering, all branches or which event should advance a host
tick counter. The current world's earlier unsupported 509/ETEX stop remains
valid; these addresses do not justify silently enabling a guessed frame loop.

The current world has since wired raw slot-zero 509 stores and stops at
**501 / offset 816**. `enemy-scheduling.md` adds bounded original-byte evidence
for constructor update-before-link, the update guard and saved-successor list
visits; it still does not establish a complete global frame schedule. In the
edge `0x415180 -> 0x416160`, the first address is a **callsite**, not the
constructor's function entry.

### 256/257/260 spawn requests

| Opcode | Handler | Prepared position | Native mode word |
|---|---:|---|---:|
| 256 | `0x41807d` | parent **combined** XY + arguments, Z zero | 0 |
| 257 | `0x418366` | argument XY, Z zero | 0 |
| 260 | `0x418720` | parent **combined** XY + arguments, Z zero | 1 |

String length at instruction `+0x10` is the padded byte count. String starts at
`+0x14`; subsequent x/y/life/score/drop words follow the padded string. Logical
parameter indexes remain name=0, x=1, y=2, life=3, score=4, drop=5 even though raw
word indexes vary. Native x/y use the original float resolver; integer fields
have the corresponding mask/typed-stack/global paths. The probes use literal
spawn parameters. The public API accepts **already resolved typed arguments**.

The real caller clears a `0x54`-byte structure and prepares:

| Byte offset | Content |
|---:|---|
| `0x00/04/08` | float32 XYZ |
| `0x0c` | int32 score |
| `0x10` | int32 drop |
| `0x14` | int32 life |
| `0x18` | uint32 mode |
| `0x1c` | zeroed reserved word |
| `0x20..0x4f` | 12 opaque words copied from parent state `+0x23c` |
| `0x50` | opaque word copied from runner `+0x11c0` |

At the stopped call to `0x414ff0`, stack arg 0 points to the original name,
stack arg 1 points to this structure. Static constructor code copies mode bit
0 into enemy flag `0x100000`; mode 1 must not be renamed to an invented enemy
class/type. Other constructor side effects are not implemented here.

`prepareSpawn(opcode, name, parent, x, y, life, score, drop)` returns this
interpreted `SpawnRequest`. Name is a **borrowed string_view**: a host retaining
it past the call must copy/own the name. Inherited words and runner word are
preserved uninterpreted, not guessed TH12 fields. Relative position uses
explicit f32-after-add stores. Flags are not indiscriminately inherited.
Parent flag `0x08000000` selects a transform callout in 256; that branch returns
empty, not a guessed ordinary-relative spawn. 257/260 do not take that branch.
Unsupported spawn opcodes return empty. Allocation, entity caps, callback/VM
creation, resource availability and constructor/lifecycle rules remain host work.

The actual first gameplay birth is in **MainSub00**, called from main after
`ins_83(100)`, not the first syntactic 257 later in the main subroutine. Its
unchanged actual bytes prepare GirlA1b at (-128,-32), life 20, score 1000, drop 2,
mode 0. A separate exact MainSub00 260 instruction prepares GirlA1 with x=128,
y=-32 and mode 1. Two opcode-modified synthetic spawn fixtures distinguish
relative/absolute semantics without pretending to be additional actual script
events. All four stop at the real allocating constructor, never after a fake
successful return.

## Host guards and remaining boundary

Finite numbers, distinct output objects, 16 slots, nonnegative resource/script
indexes, checked layer addition, nonempty/NUL-free names of at most 255 bytes,
canonical simple-motion angle range and supported mode/opcode checks are
**portable host-domain restrictions**, not recovered native clamps. Native raw
bounds and signed speed/life/score/drop are not silently corrected.

Do not treat these scalar/request functions as completed native enemy opcodes
when required host effects are absent. Full ANM birth/metrics/children, spawn
construction and flags/lifecycle, world update order, global callbacks,
shooters/ETEX, collision, damage, freezing, drops, score, offscreen behavior and
every unlisted movement mode remain outside this bounded component. Unknown
executed operations must still stop explicitly. Private JSON is derived game
analysis, not redistributable original source or proof of a native game run.

## Validation result

On 2026-10-29, component Python tests pass with the pinned EXE/Unicorn enabled,
and both Node tests pass under Emscripten 4.0.22, including all 63 differential
cases. The broader Python discovery also passes; optional original probes for
other workers remain named skips when their opt-in environment variables are
absent. Main-owned world/runtime/browser integration builds are not run or
modified by this bounded worker. Use standard floating-point semantics, keep
the explicit f32 stores and disable FP contraction; do not add fast-math and
then assume the fixture result still applies.
