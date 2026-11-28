# Bounded bullet collision decisions

## Scope and implementation plan

The user-provided pinned Japanese TH128 executable is read only. No game process,
installer, replacement calls, API stubs, or complete native-frame claim is allowed.
Only the new collision rule, probe, test, fixture, and this document are owned here.
Manager, StageWorld, runtime, site, animation, and shared freeze rules are untouched.

1. RED -> GREEN: add bounded original-byte helper and actual-update-prefix probes.
2. Recover float stores, strict comparisons, gates, and unported call boundaries;
   save byte/hash/trace-backed fixtures under `local/research/bullet-collision*.json`.
3. RED -> GREEN: implement the pure C++ `th128::bullet::collision` API and replay
   original-byte golden cases in WASM, including an undefined-behavior-sanitized build.
4. Review the isolated deliverable, run relevant and broader regressions, and report
   exact commands, unsupported inputs, and pending call obligations.

### Public API design

`rectangleDecision(center, fullSize, player, control)` and
`circleDecision(center, hitboxSize, player, control)` return optional result structs.
Their decision distinguishes complete miss/suppressed/hit/graze helper returns from
an explicit damage-call requirement; a candidate is not a completed player hit.
`updateCollisionPrefix(site, bullet, player, control)` starts after the already
computed half/full position step and preserves unported damage, hit-effects, and
graze calls as intents. No implicit default player snapshot or radius is invented.

The supported numeric domain is IEEE binary32 zero or magnitude `[2^-16, 65536]`,
with nonnegative dimensions/radii/scales and ordered rectangle bounds. Inputs
outside this contract are unsupported (`nullopt`), never silently treated as misses.
The coordinate bound permits exact integer fixed-point reproduction of the x87
64-significand-bit operations and explicit float32 stores, without requiring host
`long double` to have x87 precision. The original probe uses control word `0x037f`;
native builds require ordinary nearest-even IEEE rounding and no fast-math.

### Review focus

- Rectangle full dimensions versus circle size; inclusive versus strict touching.
- Float32 distance/bounds stores before comparisons, and x87 retained precision.
- Positive/zero/negative signed `438dc` and `5a1c`; gates only on the hit path.
- Actual update flags `2`, `4`, `0x10`, `0x40` and half-step signed age gate.
- Unsupported calls never acquire invented return values or premature graze writes.

## Execution ledger

- Original executable SHA-256 verified before probing:
  `ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.
- Ruling: work in the explicitly assigned non-Git checkout, with no agents, worktree,
  commit, or branch; explicit user ownership takes precedence over skill defaults.
- Ruling: use the existing bounded `bullet_probe.emulate` and local Unicorn library;
  do not change the shared emulator or install dependencies.
- Probe tests: missing-module assertions RED (10 failures), then 10/10 GREEN;
  fixture-builder assertion RED (1 failure), then 11/11 GREEN. Two later precision
  and independent-field coverage tests bring the targeted count to 13.
- Production tests: both normal and UBSan cases RED for missing public API, then
  GREEN against all 256 initial original-byte cases. Added original precision,
  independent snapshot, extreme-coordinate, and already-grazed rectangle cases;
  the final golden set contains 270 cases.
- Focused integration request: gate-free geometry API assertions RED in both WASM
  modes and the original-byte geometry entry test RED; then implemented one shared
  exact arithmetic path with no gate/control snapshot construction. Added 141
  geometry-only original cases, paired with every existing circle-path case.

## Safe early circle miss API

The integration can use this API **without any player gate or control snapshot**:

```cpp
enum class Geometry : std::uint8_t { Miss, PotentialHit, PotentialGraze };
std::optional<Geometry> circleGeometry(Vec2 bulletPosition, float hitboxSize,
                                      Vec2 playerPosition, float profileHitRadius) noexcept;
```

Use `th128::bullet::collision::circleGeometry`. Supply only the known post-motion
world XY, the actual circle-helper argument, current actual player XY, and the
verified SHT profile radius. `Vec2` members are `horizontal` and `vertical`.
For a scaled circle, the caller must supply the actual float32-scaled helper size;
this function neither routes bullet types/flags nor invents scale or profile data.

- **Only an engaged `Geometry::Miss` permits a collision-miss continuation.** It
  is the original geometry path that returns `0` independently of unknown gates.
- `PotentialHit` stops before unknown player/control gates. It is not a hit result
  and must not be changed to Miss based on invented invulnerability or zero fields.
- `PotentialGraze` also requires a stop before unknown caller effects. The API has
  no already-grazed flag input and deliberately does not invent one.
- `nullopt` is unsupported input, never Miss. Only these four explicit arguments
  are validated; no rectangle bounds, player-state snapshot, or control data are
  constructed, read, or defaulted by this path.

The full helper and this gate-free API share `circleGeometryDetails`, including
the integer x87 precision path, original float stores, and strict touch behavior.
Calling this API performs no movement, age increment, flags write, damage, graze,
or other lifecycle work. On Miss, resume **after** the collision check; do not
apply the already-completed motion a second time. Tick-resumption implementation
belongs to its owner, not these files.

`NativeCollision.circle_geometry` proves this without mapping gate values as
permitted data. Only bullet XY, player XY, profile pointer/radius, player global,
and the two original graze constants exist. Potential hits stop at `0x43c608`
**before** the first instruction loading control global `0x4b8950`; player `f78`,
`438ec`, and `438dc` are never readable inputs on this probe. Miss/graze geometry
finishes through actual helper return instructions, without a caller-effects stub.

## Explicit snapshots and results

All functions and types are in `th128::bullet::collision`, declared in
`src/game/BulletCollisionRules.hpp` and implemented in the independent `.cpp`.
There are no Manager, StageWorld, ANM, freeze, runtime, or site dependencies, and
the functions allocate no memory and mutate no caller state. Inputs have no
invented player/profile defaults in the production API.
`Vec2` fields are named `horizontal` and `vertical` (world X and Y respectively).

| Input | Original source |
| --- | --- |
| `PlayerSnapshot.position` | Player from global `0x4b8a80`; XY at `+ecc/+ed0` |
| `PlayerSnapshot.bounds` | min XY at `+f1c/+f20`; max XY at `+f28/+f2c` |
| `PlayerSnapshot.profileHitRadius` | Float at `+4` of profile pointed to by player `+f7c` |
| `stateF78`, `flags438ec`, `counter438dc` | Actual player fields at those offsets; counters are signed int32 |
| `ControlSnapshot.present`, `counter5a1c` | Nullable global `0x4b8950`, then signed dword `+5a1c` |
| `BulletSnapshot.center`, `fullSize` | Already updated XY `+9d8/+9dc`, size floats `+9f8/+9fc` |
| `flags`, `age1188`, `scale1180` | Actual bullet `+0`, signed age `+1188`, float scale `+1180` |

Both helpers return `std::optional<...Decision>`; `nullopt` means **unsupported**,
never a miss. The entire supplied snapshot must satisfy the documented numeric
domain, even if a selected helper does not consume every field. Sizes, radius,
and scale are nonnegative; signed zero is supported. Bounds must be ordered.
Counters and flags support their full int32/uint32 domains.

`Decision.outcome` is one of:

- `Miss`: complete original helper return `0` after geometric miss.
- `Suppressed`: complete original helper return `0` after hit-only gates.
- `HitReturn`: complete original helper return `1` with `counter438dc > 0`.
  This does **not** prove player damage, bullet cancellation, or hit effects ran.
- `GrazeReturn`: complete original helper return `2`; the caller still decides
  whether an unported graze call is needed.
- `DamageCallRequired`: stop before the actual call to `0x43d0c0`;
  `helperReturn` is empty, and `callSite/callTarget` are explicit. Do not skip that
  call or substitute a fabricated helper return.

`RectangleDecision` additionally exposes exact stored `hitBounds` and, only if
tested by the original path, `grazeBounds`. `CircleDecision` exposes the exact
stored float32 `distanceSquared`, and optional stored `dividedGrazeSize/grazeSize`.
The optionals remain empty when the hit path did not evaluate graze geometry.

## Recovered helper behavior

### Rectangle: `0x43c410 .. 0x43c56e`

The original accepts a size pointer in EAX and position pointer in EDX. Width and
height are **full dimensions**: multiply each by the original double `0.5`, then
store each center-minus/plus-half-dimension bound to float32. Overlap against the
player's **precomputed bounds** is inclusive on all four sides. Player actual XY
and profile radius must not replace these bounds.

A geometric miss retries with center +/- the original double `24`, independent
of the supplied size. This fixed graze box also includes touching. Graze returns
`2`, otherwise `0`; neither path consults the hit-only gates. A geometric hit
does not fall through to graze if suppressed. Unported damage call site is
`0x43c4e5`; complete helper returns pop only the return address (`stackDelta=4`).

### Circle: `0x43c570 .. 0x43c658`

EAX points to the bullet position; the raw `hitboxSize` is a stack float. The
original reads actual player XY and the profile radius, not precomputed bounds.
Let `b` be that raw size and `p` the profile radius:

1. Compute squared XY distance with x87 retained intermediates, then **store to
   float32** before comparing.
2. Hit iff that stored distance is strictly less than `p*p + b*b`, with the
   original x87 precision retained in the threshold. It is not `(p+b)^2`, and
   equality is not a hit.
3. On a geometric hit, use the same hit-only gates described below. Suppression
   returns `0` immediately, never retrying graze.
4. Otherwise divide `b` by original double `2.5` and store float32; clamp upward
   to original float `40`. Graze iff stored distance is strictly less than
   `b*b + (p + clampedSize)^2`, again retaining original x87 precision. Equality
   returns `0`. The hit-only gates do not suppress this graze path.

Damage call site is `0x43c648`; complete helper returns also pop the one stack
argument (`stackDelta=8`). Type-8's original `2.4000000953674316` is the raw size
passed to this helper, **not a generic radius contract**. Its bits are
`0x4019999a`; rectangle evaluation of the same size would first halve it.

### Hit-only gates

In original order: if the control pointer exists and `+5a1c != 0`, return `0`;
then player state `2`, `4`, or `3` returns `0`; then low-byte `flags438ec & 2`
returns `0`. Otherwise signed `counter438dc > 0` returns `1` without entering
`0x43d0c0`; zero and negative values stop at the unported call. Unrelated flag
bits and other signed player states are not guessed to suppress collisions.

## Actual update collision prefix

`updateCollisionPrefix(UpdateSite, BulletSnapshot, PlayerSnapshot, ControlSnapshot)`
starts **after** a caller has already executed the appropriate motion prefix:

- `HalfStep`: entry `0x40878d`, following state-2 half-step motion. Signed
  `age1188 < 8` bypasses collision, as does unset flag `2`.
- `FullStep`: entry `0x408a2a`, following state-1 full-step motion. Only flag `2`
  gates collision; there is no age-8 gate at this site.

Flag `0x10` selects circle instead of rectangle. Flag `0x40` scales dimensions
using `scale1180`, with original float32 stores before the helper is called.
Circle uses only the first dimension, without a rectangle-style half-size step.
Scaled dimensions outside the supported helper input domain return `nullopt`.

The result contains `route`, optional helper `Decision`, optional `effectiveSize`,
and an `UpdateIntent`:

| Intent | Meaning and obligations |
| --- | --- |
| `NoHelper` | Original prefix bypassed collision. No helper return is invented. |
| `HelperReturned` | Complete helper decision and no subsequent unported collision call on this prefix. Full bullet update is still not claimed. |
| `DamageCallRequired` | Preserve the helper's exact unported damage-call boundary. |
| `HitEffectsRequired` | Helper really returned `1`; `pendingMotionState=3` belongs before the next call. Stop at the first hit-effects call; later ANM/cancellation/effect work is unproved. |
| `GrazeCallRequired` | Helper returned `2`, flag `4` was unset; call first, only then apply `pendingFlagsSetAfterCall=4`. |

Exact first unported calls after complete helper returns:

| Site | Hit-effects call -> target | Graze call -> target |
| --- | --- | --- |
| Half step | `0x40882b -> 0x40d2a0` | `0x408883 -> 0x43e090` |
| Full step | `0x408abe -> 0x40d2a0` | `0x408b67 -> 0x43e090` |

Already-grazed flag `4` suppresses only the subsequent graze call, not the helper's
geometric return `2`. No pending state/flag is applied by the pure API itself.
Neither entry implements manager iteration, age increment, secondary timer,
movement, emission, animation, damage, or a complete original frame. In particular
the caller supplies the age observed **at this gate**, not a guessed incremented
age or browser-frame age. World XY is not viewport/draw XY; Z is outside this API.

## Precision implementation and discriminating evidence

The production implementation uses Clang/GCC `unsigned __int128`, supported by
the project's Emscripten 4.0.22 toolchain. It deliberately has no binary64-only
fallback or claim of MSVC portability. Within the supported domain every input
binary32 value has an exact signed Q40 representation. Coordinate differences
fit int64; Q80 squared products and sums fit comfortably inside 128 bits. Explicit
nearest-even rounding to a 64-bit significand reproduces each retained x87 multiply
and add; a separate 24-bit rounding step reproduces the original float32 stores.

This matters even when no float32 diagnostic changes. Two executable-byte cases
use center `(65536, 0)`, raw bullet size `65536`, positive counter, and actual
player XY `(0, 0)`. Both store distance-squared bits `0x4f800000`:

| Profile-radius bits | Original helper return |
| --- | --- |
| `0x37800000` (`2^-16`) | `2`, graze |
| `0x37800001` (next float above `2^-16`) | `1`, hit return |

The first sum rounds an exact half-ULP down at x87 precision; the second rounds
up. Binary64 addition discards the small squared term in both cases and cannot
reproduce the second decision. Unlimited-precision comparison without the x87
rounding would also incorrectly turn the first case into a hit. These are
original-byte observations, not a mathematical oracle substituted for the EXE.

Float32-by-float32 scaling products are exact in binary64 before their original
float32 store. The division-by-2.5 result also rounds safely through binary64 to
binary32 for the accepted binary32 inputs: its reduced non-dyadic denominator is
5, leaving ample distance from a binary32 midpoint. Other retained circle
operations use the integer precision path, not ordinary double geometry.

## Probe and golden fixtures

`tools/research/bullet_collision_probe.py` uses the unchanged bounded
`bullet_probe.emulate`, not a game process. Inputs are explicit synthetic snapshots
for the original machine-code slices, not a recorded full original frame.
Unknown call instructions stop **before execution** and never receive fake return
values or Windows API stubs. Diagnostic stores are sampled by rerunning the same
original chunks to exact instruction boundaries, not by patching the binary or
injecting float-store instructions.

The private `local/research/bullet-collision-fixtures.json` contains:

- 91 rectangle cases, 101 circle cases, 78 actual-update-prefix cases.
- Another 141 geometry-only cases reuse every circle helper/update-prefix input
  and confirm the pre-gate stage independently: 48 Miss, 42 PotentialHit, 51
  PotentialGraze. Their 256-instruction budgets have observed maximum 50; no
  player gates or control state are mapped as readable data.
- Original code bytes plus SHA-256 for every slice and constant; exact input float
  bits, explicit registers/state regions, instruction traces/budgets/stops, and
  original float-store measurements.
- 512-instruction main-case budgets; the observed maximum is 108 instructions.
- Separate bounded type-8 branch proof: original profile selector `6` enters the
  branch that ORs flag `0x10` before `0x40988c`. This proof starts with type 8,
  color 10 and flags 0; it does not claim ANM setup or emission was completed.
- Original type-8 size bytes from `0x4b2ed4`, and unchanged EXE hashes before/after.
- `nativeCpuTrace=false`, `windowsGameProcessLaunched=false`,
  `fakeCallReturns=false`, `completedNativeFrame=false`, and `completedTick=false`
  on every main case.

Final fixture SHA-256:
`a034fcc2db9b9ecf07f32d26ed24daaa20c2d241c79b0ad44d19defd1a86cc9b`.
The JS tests pin that digest, validate all byte hashes, and replay every case
through the C++ rules. The Python original-byte test regenerates and compares all
270 full-decision and 141 geometry-only saved cases, not just a few selected fields.
Both normal and UBSan WASM builds replay all 411 records.

## Reproduction commands

Run from the assigned `outputs/th128-web` workspace in PowerShell:

```powershell
$env:PYTHONPATH = (Resolve-Path '..\..\work\reverse-libs').Path
$env:PYTHONDONTWRITEBYTECODE = '1'
$env:TH128_BULLET_EXE = 'E:\gee\妖精大战争\th128.exe'
python -m unittest discover -s tests -p test_bullet_collision_probe.py -v
node --test tests/bullet-collision.test.mjs
```

To regenerate only the owned private fixture (never a public or E: destination):

```powershell
python tools/research/bullet_collision_probe.py $env:TH128_BULLET_EXE --report local/research/bullet-collision-fixtures.json
```

The Python dependencies are the already available pefile/Capstone plus local
Unicorn from `work/reverse-libs`; nothing is installed. C++ tests use the existing
Emscripten SDK, `-std=c++20 -O1 -Wall -Wextra -Werror -fno-exceptions
-fno-fast-math -ffp-contract=off`, and a second build with
`-fsanitize=undefined -fno-sanitize-recover=all`. Test products go only to the
dedicated `work/bullet-collision-tests` directory, never runtime/site.

## Review and verification scope

Final review: self-review (no additional agents permitted). Checked raw-byte
branch directions, signed comparisons, precise unknown-call stops, snapshot
independence, all integer shift/product bounds, signed zero, compiler capability,
and public continuation semantics in a separate pass. No unresolved findings or
deferred minors were identified; this is not an independent reviewer sign-off.

The runtime-publishing full JS suite is intentionally not invoked: its runtime
test rebuilds/publishes `site/runtime`, and the existing enemy harness overwrites
another owner's private research JSON. Neither is permitted by this task's file
ownership. Broader verification instead runs full Python discovery and read-only
draw/inspector/renderer/replay/publication JS regressions alongside this task's
normal and UBSan WASM fixtures. Main remains responsible for its own runtime
integration, scheduling, and publishing validation.

Before the final focused geometry request, broader regressions passed: full Python
discovery ran 210 tests (41 optional original-input tests skipped), and the selected
JS/WASM plus read-only regressions ran 37 tests without failures. After the request
not to read main's files, final verification is restricted to these owned collision
tests and fixtures; no new shared-source inspection or runtime build is performed.

Final scoped verification (2026-10-29):

- `python -m unittest discover -s tests -p test_bullet_collision_probe.py -v`:
  14/14 passed with the actual pinned EXE and Unicorn, no skips. This includes
  byte-for-byte case regeneration for both the 270 and 141 case matrices.
- `node --test tests/bullet-collision.test.mjs`: 3/3 passed: evidence validation,
  normal WASM replay of 411 records, and UBSan WASM replay of the same 411 records.
- Original EXE SHA-256 remains the pinned value. No original process, installer,
  E: output, commit, branch, or additional agent is used. All source deliverables
  are the eight explicitly owned new files; shared integration is untouched.
