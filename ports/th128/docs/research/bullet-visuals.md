# TH128 type8 bullet visuals

## What this implements

`src/game/BulletVisuals.hpp/.cpp` adds a reusable, caller-scheduled visual
sidecar over the existing C++ `anm::Registry` and `anm::VM`. Each `Visual`
contains **two embedded VMs**, not renderer substitutes. The primary uses the
four sprite-remap inputs supplied by the caller; the optional secondary has
its own script/variables and does not inherit the primary remap or birth IRQ.
There is no dependency on `BulletManager`, no private asset embedded in source,
and no StageWorld, export, renderer, site or shared-animation-file change in
this implementation.

This is a usable animation-birth implementation, **not a complete TH128 port**.
It does not implement bullet movement, ETEX dispatch, collision, pooling,
freezing, sound, whole-game scheduling or a playable stage. Those remain the
responsibility of their owning modules. In particular, successfully creating
a visual is not evidence that the next native bullet update is implemented.

## Public contract

All names below are in `th128::bullet::BulletVisuals`:

```cpp
static constexpr int BankId = 0;
static constexpr uint32_t MaxAdvanceTicks = 4096;
bool load(const void* bytes, size_t size, std::string& error);
std::optional<Visual> create(const CreateInfo& info, std::string& error);
bool interrupt(Visual& visual, int code, std::string& error);
bool advance(Visual& visual, uint32_t ticks, std::string& error);
void setRandomSources(std::function<uint32_t()> game, std::function<uint32_t()> display);
const anm::State* animationState(const Visual& visual, bool secondary = false) const;
const anm::SpriteInfo* spriteInfo(int sprite) const;
const anm::Registry& registry() const;
```

`CreateInfo` contains `type` (only 8), `color` (0..15), the original profile
`script` (43 for the observed type8 profile), `spriteRemap` (exactly four
caller-provided signed integers), `secondaryScript` (`std::optional<int>`),
and `birthInterrupt` (0 means none). The caller must obtain the remap from
the actual profile, not derive a TH12 color map. All four destinations must
exist in this ANM bank. Negative results/white-sprite fallback are deliberately
outside this bounded type8 API.

The native profile's secondary value **0 means absent**: pass `std::nullopt`
for it. An explicitly supplied optional value 0 means bind ANM script index 0;
this distinction is intentional. The class does not parse an EXE profile or
look up color tables itself. The manager can fill `CreateInfo` from its parsed
288-byte profile and precomputed birthtail state without linking this class
to manager internals.

`BankId = 0` is a fixed **host-local identity in this class's own registry**,
not an assertion about the native game's resource slot. The class is neither
copyable nor movable because embedded VMs refer to its registry. Visuals may
be moved into a caller-owned `id -> Visual` sidecar; IDs and pool ownership
stay with the manager. No hidden global allocation/scheduler is installed.

`load` copies/decodes the supplied ANM through the existing bounded Registry
loader. A failed load leaves the current bank and visuals usable. A successful
reload invalidates every older visual using a per-load lifetime token.
Default/unbound, foreign, stale and faulted visuals are rejected before any
explicit advance. The lifetime token also rejects visuals from a destroyed
owner, even if another owner is later allocated at the same address.

`animationState` returns a const pointer, or null for stale/foreign/faulted
state or an absent secondary. `spriteInfo` returns const texture-entry,
rectangle dimensions and UV information, or null for an invalid sprite.
Neither accessor, nor `registry()`, queues an interrupt or runs animation.
Sprite pointers last until the next successful load or owner destruction;
state pointers last as long as their unmoved Visual. Draw code combines the
script transform with the manager's physical birth position, including its
Z value; this class does not reconstruct/override bullet position rules.

## Birth ordering: no third tick

The implemented order is:

1. Default-reset both embedded C++ VMs; construction itself does not tick.
2. Bind the primary with its sprite callback already installed. Existing
   `VM::bind` executes one synchronous tick; do not tick it again here.
3. Bind the optional secondary, also executing its one synchronous tick.
4. Queue the primary `birthInterrupt` supplied by the already evaluated tail.
5. Run one post-tail primary tick, and one secondary tick if present.

Thus a normally active script has **two executed VM ticks at create return**.
`interrupt` subsequently only queues; `advance(visual, n, error)` executes
exactly `n` scheduling steps with `delta = 1` and primary-before-secondary
ordering. Zero steps is read-only. Ended VMs remain ended. Calls above 4096
steps are rejected without advancing. A failing tick stops the call and
does not run further ticks/secondary work after primary failure; already
completed steps are not rolled back.

This ordering is grounded in TH128 EXE bytes:

- `0x40974b` calls reset `0x402ea0`; `0x409750` installs callback `0x40ccb0`
  at bullet `+0x4e8` (primary VM `+0x4e0`) and the owner at bullet `+0x4ec`.
- `0x409771` reads the primary script from the type profile, and
  `0x409777` calls bind `0x4595f0`.
- `0x409785` resets the secondary at bullet `+0x4f0`; `0x40979f` tests the
  profile secondary slot. Zero skips its bind.
- Inside bind, `0x45969f` calls tick `0x459d90` synchronously.
- The birthtail's primary pending-IRQ field is VM `+0x3f8`. After the
  extension dispatch boundary, `0x4099e9` calls primary tick, and the optional
  secondary tick call is at `0x4099fd`.

The supplied native offset relationship bullet `+0x438` = primary VM
`+0x430` identifies the script integer used by later bullet control. Portable
code retains that semantic variable as `primary.state().integers[0]`, not as
an alias of position, flags or float0. No native packed-memory layout is
claimed for the C++ State structure.

## Actual-byte callback and profile evidence

`tools/research/bullet_visual_probe.py` reuses the existing bounded Unicorn
helper. It reads the pinned Japanese EXE, never launches or modifies it:

- EXE SHA256: `ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.
- Actual `bullet.anm`, resolved through `local/catalog.json` -> Japanese
  manifest parent -> `raw/bullet.anm`, SHA256:
  `9860822687f33e740f2a5e1fb25a4b05cf27b0fc6d15ed9626fb903986f7784c`.
- Private report: `local/research/bullet-visual-fixtures.json`, schema
  `th128-original-byte-bullet-visual-fixtures/1`, **64 returned cases**.
- Report SHA256:
  `fe5e11ed7c2719d0bc8a76a8b1c1982cd757037c5cd284bd27b19d257eb043ad`.
- Callback slice `0x40ccb0..0x40cce8` SHA256:
  `51489aa56eab22f5876440255b4edee6cb4c2416da1ce1da88ba08eeb844b566`.
- Profile span `0x4b2dd0`, 288 bytes, SHA256:
  `7427ad4aa6459742ffaa628c7a8e303029a9692b7b3ffbdd5b7f9a3207f0d80e`.

The helper receives primary VM in ECX and source sprite in EDX. It reads the
owner at VM `+0x4e4`, type/color signed words at owner `+0x11b0/+0x11b2`,
and, after the per-type negative-sentinel check, loads:

```text
0x4b24d4 + type * 0x120 + color * 16 + sprite * 4
```

All 16 colors and all four input slots are executed through those original
bytes, with a 64-instruction per-case budget, explicit read-only data and
stack spans, and the existing 500000-microsecond emulator timeout. Source
slot 3 can overlap the next color's conceptual fields; it is **not** inferred
from a tidy independent-color layout. Color 10's actual four slots are
`[138, 328, 346, 357]`. Primary script is 43 and secondary profile slot is 0.

The report also retains hashed static selection/bind/tail call-site slices.
An undeclared reset call at `0x40974b` is separately shown stopping with
`unsupported_call` before the call; no return value or host API is fabricated.
This is original-byte **emulation**, not native Windows CPU execution, a
Windows game-process run, or a recorded complete-game trace.

## Script43 regression and inherited semantics

The native/WASM test loads the actual raw ANM rather than a fabricated
script43 fixture. The ANM itself proves the instruction operands: initial
integer10000 = 0, source sprite1, scale3, alpha/scale interpolation duration8;
label IRQ8 at authored time8 replaces that startup with duration15. Both
paths jump to the steady block at authored time48, which writes integer10000
= 1, alpha255, scale1, selects source sprite0, and holds. IRQ1 later selects
source sprite2, reads a random rotation operand and shrinks before deletion.

These portable-state expectations are tested for all 16 actual remap vectors:

| Path | State at create return | Explicit steps to steady block |
| --- | --- | --- |
| No birth IRQ | ticks2, clock2, int0=0, alpha63, scale1.90625 | 7 |
| Birth IRQ8 | ticks2, clock9, int0=0, alpha45 | 15 |

After steady entry both have int0=1, held=true, authored clock48 and source0
mapped through the supplied color table. Secondary script43 is separately
tested with raw sprite1, clock2 and two birth ticks, independent of primary
IRQ8/remapping; optional script0 is also distinguished from absence.

**Important evidence boundary:** instruction dispatch, numeric-variable
semantics, interpolation arithmetic/rounding, interrupt saved timers, held
clock handling, RNG domains, and error budgets are provided by the existing
TH12-derived `AnmLogic` implementation. This task does not independently
prove every one of those semantics against the complete TH128 native ANM
interpreter. The numeric snapshots above are portable regressions through
the actual TH128 script, not claimed TH128 native-CPU snapshots. TH128-proven
here means the asset bytes, profile selection, callback mapping, and static
birth call ordering, with bounded emulation specifically of the callback.

## Fail-closed behavior and remaining limits

Missing profile slots, out-of-range color/type/script/sprite inputs, unknown
ANM opcodes, missing RNG and unsupported ANM child creation fail with a
non-empty error. Unknown ANM calls are not registered as no-ops. Interpreter
errors retain script/opcode/byte-offset diagnostics; visual errors are sticky
and `animationState` refuses to expose a failed visual. Sprite callback
failure invalidates the entire visual even though its current VM command may
finish internally; its partially executed state is not a valid draw snapshot.

RNG is **not seeded or invented here**. Default hooks are absent; script43
startup and IRQ8 need none. For later branches such as IRQ1, callers may set
game/display 32-bit sources. The stored callable, including mutable closure
state, is invoked rather than copied/reset per tick. Clearing hooks restores
fail-closed behavior. Tests supply declared deterministic RNG input for the
IRQ1 rotation/deletion regression; this is not an original RNG-output trace.
No child scheduler is installed. Camera values and broader inherited runtime
facilities are not a newly verified TH128 camera implementation.

The caller owns ANM/model lifetimes and must not mutate embedded VMs directly
to bypass scheduling or fault checks. The class is not thread-safe, does not
manage IDs/releases, and does not guarantee simulation equivalence beyond
its explicitly documented visual-birth boundary.

## Verification

```powershell
node --test tests/bullet-visuals.test.mjs
$env:PYTHONPATH = '../../work/reverse-libs'
$env:TH128_BULLET_EXE = 'E:\gee\妖精大战争\th128.exe'
python -B -m unittest discover -s tools/research -p test_bullet_visual_probe.py -v
python -B tools/research/bullet_visual_probe.py --exe $env:TH128_BULLET_EXE
```

The Node test uses Emscripten 4.0.22 with warnings-as-errors and the actual
asset. Its generated CJS/WASM files stay under `work/bullet-visuals-test`,
not the shared build/site. RED was observed before implementation; a second
RED regression demonstrated stateful RNG closure reset, then verified the
fix. The probe tests validate destination bounds and input rejection before
any EXE read, and execute all 64 callback cases when the EXE is configured.

Final focused verification on October 29, 2026: Node **3/3 passed**; configured
original-byte probe tests **3/3 passed**, covering all 64 callback returns;
the same actual-script C++ regression also passes with **UBSan and
`-fno-exceptions`**. The EXE digest remains unchanged. Broader Python discovery
in a private scratch snapshot ran **195 tests: 133 passed, 62 skipped** (the
default suite's opt-in native-evidence cases were not enabled in that run).
Broader JS/WASM verification uses a scratch copy so existing suites cannot
publish/overwrite the shared site or build outputs.

The broader scratch Node sweep completed **63 tests: 62 passed, 1 failed**.
The sole failing check was `bounded production bullet manager allocates real
births and preserves explicit unsupported paths`: its hardcoded relative
compiler path did not exist in the relocated scratch project, so spawning
the compiler returned null, not a product assertion failure. A concurrent
recheck then hit a scratch Emscripten cache-lock reset (`WinError 32`). Neither
condition justifies claiming a clean all-tests run. The sweep did pass the
actual first-stage birth, WASM 501-to-real-ANM construction, existing ECL,
animation, player/rules, publication, renderer and replay checks. No shared
production/test files were changed to work around the scratch environment.

After the broad run released its scratch cache, a serialized manager recheck
passed **1/1**, including that test's regular and UBSan builds. Thus the sole
initial failure was cleared by fixing the relocated toolchain environment,
not by a production-code change. This is a successful follow-up check, not
a rewritten claim that the initial 63-test invocation was entirely green.

## Frozen source identity

The component API and implementation are frozen for the main owner's fresh
review/runtime build. No further feature or source edit follows this freeze.

- `src/game/BulletVisuals.hpp` SHA256:
  `add61feb628c04f567031be7e4f72d0b881201ddaa0a82968b7d8e8856d6f368`.
- `src/game/BulletVisuals.cpp` SHA256:
  `1a0f3980ad1d91e0a491ca8d5286ee39b9680b2ced087c0fc66e06e425f77e25`.
