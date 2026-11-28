# TH128 first-enemy emitter: bounded research ledger

## Scope and plan

This is the bullet-worker portion of the approved first-stage plan. Only assigned
BulletRules, bullet probe, bullet tests and bullet research files are written.
The Japanese executable is read-only: no game process, installer, E: write or Git.

1. Recover actual TH128 dispatch and first-emitter instructions.
2. Write failing probe tests and original-byte configuration cases.
3. Write failing portable C++ golden tests, then verified scalar rules.
4. Run Python, original-byte and WASM checks; document unsupported boundaries.

## Coordination findings

- Wrapper `0x417fe0` adds `0x11c8` to ECX and enters `0x417ff0`.
  Dispatch at `0x41803e` uses `opcode - 256`, byte table `0x41c7c0`,
  jump table `0x41c554`, maximum index `0x167`.
- Dispatcher EBX is the enemy context after the wrapper's `+0x11c8`.
  Emitter zero is EBX `+0x4a0`; emitter stride is `0x380`.
- Actual GirlA1b opcode 435 writes local **8**, selecting **3, 3, 5, 7**.
  Opcode 43 previously initializes local **4** to **1**. Opcode 522 reads
  local **4**, not local 8. The first volley has **one bullet on every
  difficulty**, not 3/3/5/7. Local 8 is the later firing-loop bound.
  First-volley speed is 1/1/2/2, second speed 1.
- The app-tool update to the parent was rejected because this is a native worker;
  this shared ledger publishes the dispatch without touching another worker's files.

## Completed bounded deliverables

All four plan steps are complete for this configuration/prefix phase. This is
not a complete emitter, bullet manager or playable bullet-engine implementation.
The runtime may stop at 509, or consume the verified slot-zero configuration and
then stop at 501. It must not silently ignore either unsupported execution path.

The public files are `src/game/BulletRules.hpp` and `.cpp`, namespace
`th128::bullet`. No StageWorld, ECL VM, runtime, browser or other worker source is
edited. The native C++ fixture is compiled to WASM with exactly the SDK/Python/
EM_CONFIG/EM_CACHE selection used by `tests/native-random.test.mjs`.

### Stable public contract

```cpp
std::optional<std::int32_t> chooseDifficulty(
    std::int32_t difficulty, const std::array<std::int32_t, 4>& values) noexcept;
Emitter resetEmitter() noexcept;
void setTypeColor(Emitter&, std::int32_t type, std::int32_t color) noexcept;
void setAimMode(Emitter&, std::int32_t mode) noexcept;
void setAngles(Emitter&, float angle, float spacing) noexcept;
void setDifficultySpeeds(Emitter&, std::int32_t difficulty,
    const std::array<float, 4>& speeds, const std::array<float, 4>& endSpeeds) noexcept;
void setDifficultyCounts(Emitter&, std::int32_t difficulty,
    const std::array<std::int32_t, 4>& counts,
    const std::array<std::int32_t, 4>& layers) noexcept;
bool setFirstExtensionRaw(Emitter&, std::int32_t extensionIndex,
    const ExtensionRaw&) noexcept;
std::optional<std::uint32_t> prefixCount(const Emitter&, std::uint32_t budget = 4096) noexcept;
std::optional<EmissionPrefix> emissionPrefix(const Emitter&,
    std::int32_t bulletIndex, std::int32_t layerIndex, float aimAngle) noexcept;
```

`Emitter` contains `type`, `color`, `positionX`, `positionY`, `positionZ`,
`angle`, `spacing`, `speed`, `endSpeed`, signed-16-bit `count` and `layers`,
unsigned-16-bit `aimMode`, `flags`, `shotSound`, `transformSound`, and `extension0`.
It is a portable typed subset, **not** the original binary memory layout. Plain
`Emitter{}` zero-initializes; opcode 500 must use `resetEmitter()`.

`ExtensionRaw` has these final full field names:

| Field | Opcode 509 input | Native offset from emitter zero |
|---|---|---|
| `parameterR` (float) | F(6) | `+0x28` |
| `parameterS` (float) | F(7) | `+0x2c` |
| `parameterA` (int32) | I(4) | `+0x38` |
| `parameterB` (int32) | I(5) | `+0x3c` |
| `type` (int32) | I(3) | `+0x48` |
| `control` (int32) | I(2) | `+0x4c` |

I(0) selects the emitter and I(1) the extension slot. The original handler uses
slot stride `0x2c` without bounds checks. The portable setter accepts **only slot
zero** and finite R/S; rejection leaves the emitter unchanged. These are explicit
portable guards, not recovered native validation. No meaning such as TH12's
"async" is asserted for `control`. All six distinct argument writes, including
negative integers and nontrivial floats, have a separate original-byte case.

Successful `setFirstExtensionRaw` means only "raw fields stored". It does **not**
mean the extension is understood, executed, or safe to fire. In particular, the
actual first script's type 2 is not a supported bullet update/lifecycle routine.

### Opcode evidence

| Opcode | Original handler | Proven configuration behavior |
|---|---|---|
| 435 | `0x41a55f` | Resolve four int choices, then write selected destination |
| 500 | `0x41a67f` | Zero selected `0x380` block and set the TH128 defaults |
| 501 | `0x41a7bd` | Prepare emitter origin and manager field; stop at `0x40a8f0` call |
| 502 | `0x41a8c3` | Store type/color as full int32 |
| 504 | `0x41aa39` | Store angle/spacing as float32 |
| 507 | `0x41af99` | Store low 16 bits of aim mode |
| 509 | `0x41b009` | Store the six raw extension parameters above |
| 521 | `0x41aafb` | Select primary and secondary speeds independently |
| 522 | `0x41ab92` | Select counts/layers; store low 16 bits, used as signed counts |

435 is **not** an unrestricted clamp. Difficulties 0,1,2,3,4 choose indices
0,1,2,3,3. Other int32 difficulties do not write: `chooseDifficulty` returns
`nullopt`. The original resolves **all four input choices before testing the
difficulty**. The caller must preserve those argument evaluations and use the
VM's actual reference-store operation for the destination, rather than reading
the destination as I(0).

521/522 have different invalid-rank behavior: difficulty exactly 0/1/2 selects
index 0/1/2, and **every other int32 value selects index 3**. They resolve only
the selected primary argument and then the selected secondary argument. Golden
cases cover -1,0,1,2,3,4,5, signed-word wrapping, and mode low-word wrapping.

Native reset defaults are type/color/position/angle/spacing zero, primary speed
2, secondary speed 0, count/layers 1/1, mode 0, flags **35**, shot sound **22**,
transform sound **40**, extension index zero and zeroed extension storage.
The complete `0x380` block is captured. Resets at slots 0,1,15 verify adjacent
emitter blocks are untouched. The native reset also clears the X/Y words of
auxiliary position arrays at context `+0x3ca0/+0x3d60` and a word at `+0x3d68`;
these auxiliary native buffers are outside the portable subset and not a license
to implement the other origin-setting opcodes.

### Actual first configuration

Pinned `st_a1_1.ecl` SHA256:
`181aa57ef1581ef19340b283550dd140982ea0b1c12290a5f8957eec2a3f7b35`.
The probe parses the original prepared **raw ECL**, not a hand-edited JSON export.

At GirlA1b offsets 484..816: 435 selects local 8; 500 resets emitter 0; 507
sets mode 0; 502 sets type/color 8/10; 522 uses immediate count 1 on Easy and
local 4 on the other difficulties; 504 sets angle 0, spacing float bits
`0x3d20d97c`; 521 sets speeds 1/1/2/2 and secondary speed 1; 509 writes slot-zero
`control=1,type=2,parameterA=1,parameterB=-999999,parameterR/S=-999999.0`.
83 then requests a wait argument of 14 before 501. This is script/configuration
evidence, not a claim to have completed the native timed firing loop.

The five difficulty cases (including the native value 4) verify first count and
layers **1/1**. The loop limit in local 8 is **3/3/5/7/7**. Do not substitute
the loop bound for the current count local.

### Geometry is explicitly pre-ETEX

18 original-byte geometry cases enter actual call target `0x4091f0` and stop at
`0x409717`, before animation setup, extension copying, type-2 spawn treatment,
manager advancement, return, or update. Original helpers `0x46a660` and `0x40d110`
execute their own normalization and x87 `fsincos` instructions; they are not
synthetic math-return stubs. Every case says `completedEmission=false`.

For mode 0, the externally supplied aim angle is added; mode 1 omits it. Odd
counts enumerate offsets `0,-spacing,+spacing,-2*spacing,+2*spacing,...`.
Even counts enumerate `+spacing/2,-spacing/2,+3*spacing/2,-3*spacing/2,...`.
Layer speed is primary minus `(primary-secondary)*layer/(layers-1)`, or primary
alone for one layer. Intermediate float32 stores are preserved. Velocity uses
the **raw** angle, not the separately normalized angle stored on the bullet.

The original normalization uses promoted-float constants pi
`3.1415927410125732421875`, tau `6.283185482025146484375`, not mathematical
double pi/tau; the golden wrap case caught a one-ULP error from using the latter.
Native normalization also has a shared loop guard allowing at most 34 repeated
adds/subtracts before returning its current value; it is not unrestricted fmod.

`emissionPrefix` does not generate a bullet or run any extension. It accepts
only modes 0/1, valid signed-count indices, finite inputs and raw angles within
2048 radians (an explicit portable guard). It returns raw/stored angle, speed
and velocity. `prefixCount` computes the bounded product of positive counts,
zero for an empty dimension, and `nullopt` on exceeding the caller's budget.
Aim is a fixture/caller input: the native player-to-emitter atan2 path is **not**
claimed proven. Portable libm is bit-identical for the 18 saved geometry vectors;
no claim is made for every possible x87 transcendental input.

## Provenance, safety and verification

`local/research/bullet-fixtures.json` has schema
`th128-original-byte-bullet-fixtures/1`, **49 cases**, and pinned SHA256
`7411892ba21f9f35bb88d53717e08beda3b3887ec602641d49eaf3b8b4cd3009`.
Both before/after EXE hashes are
`ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.
These are Unicorn-emulated original bytes, not native Windows CPU traces.
No Windows game process is launched.

Each case uses explicit instruction-boundary chunks and explicit state spans:
maximum 32 chunks / 65536 code bytes / 64 regions / 131072 data bytes / 65536
stack bytes, hard instruction ceiling 20000 and timeout 500000 microseconds.
Current cases request 2048 instructions each. Page padding is not usable state;
readonly jump tables can share a code page without becoming executable code.
Unknown or indirect calls stop without changing return registers or inventing
a return. All saved cases end at a declared boundary or an unsupported call;
none is accepted on a fault, timeout, undeclared memory or exhausted budget.
Dispatch wrapper/selector/table evidence is separately labelled static.

The C++/WASM test compares **38** native-byte golden vectors: seven rank choices,
seven ranked speed/count cases, six raw-extension configurations and eighteen
geometry prefixes. All returned geometry float bits match exactly.

Commands, from the deliverable root with `work/reverse-libs` on PYTHONPATH:

```powershell
$env:TH128_BULLET_EXE='E:\gee\妖精大战争\th128.exe'
python -B -m unittest discover -s tests -p test_bullet_probe.py -v
python -B tools/research/bullet_probe.py --exe $env:TH128_BULLET_EXE
node --test tests/bullet-rules.test.mjs
```

Observed targeted results: 13 Python tests pass with original EXE enabled;
2 Node tests pass, including the WASM build and 38 golden vectors. Full Python
discovery passes 152 tests with 22 unrelated optional-native cases skipped.
Full Node discovery with `--test-concurrency=1` passes **54 tests**, zero skips
and failures. This includes the shared actual StageWorld test, which still
correctly stops GirlA1b at opcode 509 / offset 748 in that snapshot.

That is the earlier configuration-only snapshot. The current world stores raw
509 fields and stops at **501 / offset 816 / kernel tick 114**, as recorded in
`first-world.md`. The subsequent 35-case aim/spawn-tail/motion investigation is
in `bullet-lifecycle.md`; those prefixes do not complete emission or ETEX.

## Remaining hard boundary

It is safe to retain a 509 runtime stop. If the caller wires the proven raw
setter, it may retain those parameters and advance to the wait, but **must stop
at unsupported 501** rather than firing a pseudo-emission. The native manager,
player-relative aim computation, type-2 spawn/lifecycle behavior, later ETEX
updates, animation bindings, collisions, lasers and freezing are not implemented
or validated by this phase. Nothing here marks the complete first stage playable.
