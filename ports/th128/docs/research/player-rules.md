# TH128 player rules: bounded native evidence

## Status and scope

Movement/input is ready for **partial-component** integration. This is not a
complete player, shot emitter, or Freeze system. No StageWorld, ECL VM, enemy,
global RNG, runtime, build configuration, or site files are changed here.
No original game/installer process was launched, no E: files were written,
and no branches or commits were created.

Architecture: independently sampled original x86 instruction slices establish
integer-coordinate rules. A pure C++ API implements those rules at unit
movement/time scale. A bounded Python reader preserves actual shot bytes and
unknown callback IDs without executing them. Composition of independently
verified math is not proof of a whole native frame's scheduling.

Completed plan, using the chat specification and its write allowlist:

- [x] Pin original EXE and asset hashes; identify native consumers.
- [x] Write failing probe tests; implement strict instruction emulation.
- [x] Establish movement, focus/key, bounds, and shot-cycle golden vectors.
- [x] Write failing shot-reader tests; implement confirmed structural layout.
- [x] Write failing C++ edge tests; implement an independent public API.
- [x] Run component tests and record supported versus unknown behavior.

Ruling: keep the supplied shared checkout, without worktree/branch changes or
extra plan files. The user's narrow write allowlist takes precedence.
The work is inline; no subagent/reviewer dispatch tool was available.

## Pinned sources

Japanese `E:/gee/妖精大战争/th128.exe`, read as bytes only:

`ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`

Resolve `local/catalog.json -> variants.ja.manifest`, then that manifest's
parent `raw/pl00a.sht`. **The actual name is `pl00a.sht`, not `pl00.sht`.**
It is 6,736 bytes, SHA-256:

`aed90a45b565510e97cc7d9ab4846d8c93914e57faafe9339e9ab03d47b1511b`

TH12 sources are not used as behavior/formation/damage specifications. No
replacement asset, guessed callback, or new damage value is supplied.

## Native movement evidence

All ends below are exclusive. `local/research/player-native-fixtures.json`
contains instruction bytes, their hashes/disassembly, explicit assumptions,
and 677 observed cases, not manufactured expected outputs.

| Region | Verified operation | Boundary/omission |
| --- | --- | --- |
| `0x43af80..0x43b093` | Direction priority and focus gates | Stops before velocity/ANM work |
| `0x43a669..0x43a6af` | Four SHT float speeds converted to raw integers | Original conversion helper runs |
| `0x43b0eb..0x43b1b7` | Focused/nonfocused velocity selection and truncation | Entry `0x43b0eb` or `0x43b155`; no ANM calls |
| `0x43b230..0x43b33f` | Raw add, bounds, and float position mirrors | Stops before animation-manager lookup |
| `0x48fac0..0x48fb6b` | Original float-to-integer helper | SSE2 selector at `0x4dcd68` explicitly 1 |

The player pointer at `0x4b8a80` addresses both representations:

| Player field | Representation |
| --- | --- |
| `+0xed8`, `+0xedc` | Signed int32 horizontal/vertical coordinates, 128 raw units/pixel |
| `+0xecc`, `+0xed0` | Float32 mirrors written from those integers times `1/128` |
| `+0xee0`, `+0xee4` | Normal/focused cardinal raw speeds |
| `+0xee8`, `+0xeec` | Normal/focused diagonal component raw speeds |
| `+0xf6c` | Direction 0..8 |
| `+0x43a88` | Focus result 0/1 |

The float mirror is the correct interface for consumers that execute native
`fld [player+0xecc/0xed0]`; the integer field is not an interchangeable getter.
No ECL global/getter implementation is duplicated here.

### Speeds, coordinates, and bounds

SHT offsets `0x10,0x14,0x18,0x1c` contain float32 values
`4.5, 2.0, 3.181980609893799, 1.4142135381698608`. The original constructor
multiplies by double 128 and truncates through the original helper:

| Mode | Cardinal raw / pixels | Diagonal component raw / pixels |
| --- | --- | --- |
| Normal | 576 / 4.5 | 407 / 3.1796875 |
| Focused | 256 / 2.0 | 181 / 1.4140625 |

Do not recompute the diagonal by normalizing a floating velocity every tick.
The captured integer components are already truncated in the constructor.

Signed raw addition wraps modulo 2^32 **before** signed inclusive clamping:

- Horizontal: `[-23552,23552]`, equivalently `[-184,184]` pixels.
- Vertical: `[4096,55296]`, equivalently `[32,432]` pixels.

`advancePosition` reproduces wrap explicitly without C++ signed-overflow UB.
The same bounds apply to idle input as well as movement. No screen-origin
translation or UI coordinates are added. For the main task's separately
confirmed demo start `(0,400)`, pass raw `{0,51200}`; raw structs' zero default
values are not a claim about native spawn position or replay RNG seed.

### Direction and input flags

The native direction table at `0x4a3850` is:

`0=(0,0), 1=(0,-1), 2=(0,1), 3=(-1,0), 4=(1,0),
5=(-1,-1), 6=(1,-1), 7=(-1,1), 8=(1,1)`.

Resolution priority is **up-left, down-left, up-right, down-right, down, up,
left, right, none**. Opposing bits do not cancel: up+down resolves down;
left+right resolves left; all four resolve up-left. Other bits are ignored
only for this operation, not removed from the input record.

`Input` preserves all three uint16 words: holding, pressed, released. Native
holding is read at `0x4db210`, pressed at `0x4db21c`; native replay code also
copies its second uint16 to `0x4db21c` at `0x4413d6`. Released uses
`0x4db220`. The actual replay-decoder flag assignments are retained:

`ShootFreezeKey=1, BombKey=2, FocusKey=8, UpKey=16, DownKey=32,
LeftKey=64, RightKey=128, RapidKey=512`.

These constants alone are not gameplay specifications. Native consumers
independently establish the direction, focus, and shot-cycle semantics below.
Bomb/Freeze activation is not implemented by this component.

### FocusContext: explicit diagnostic contract

Native focus requires all of:

1. `[0x4b8948] != nullptr`.
2. `[[0x4b8948]+0xc0] != 0`.
3. Signed `player+0xf98 >= 4`.
4. Holding Focus bit 8, **or** signed `player+0x43914 >= 12`.

Manager presence/activity is **not proved unconditional**. The meaning of
`+0xf98` beyond this comparison is not renamed to an invented world state.
When gates 1..3 fail, the native slice also resets `player+0x438d4` to 30;
`isFocused` returns only the focus decision and does not mutate that timer.

Public fields are `FocusContext{managerPresent, managerActive, counterF98,
holdCount43914}`. `moveOneTick` requires an explicit context argument. The
default-constructed context represents unconfigured/closed gates, **not** a
claim that the native full game normally lacks its manager.

For the caller-owned no-Freeze diagnostic only:

```cpp
const th128::player::FocusContext diagnostic{true, true, 4, 0};
const auto next = th128::player::moveOneTick({0, 51200}, input, diagnostic);
```

This explicitly assumes the first three native gates are open and allows the
Focus key to select the measured slow speed. Keeping `holdCount43914=0`
disables hold-driven focus. **Label hold-focus unsupported until actual
world manager/timer state is implemented. Do not substitute a synthetic
shoot-held frame counter, and do not treat this diagnostic as full-game
initialization.** The 12 threshold is verified against supplied native raw
state, not a wall-clock schedule or an implementation of that state.

## Native shooting evidence: helpers only, no emitter integration

`0x43f1b0` dispatch gates on player `+0xf78 == 1`; the helpers below assume
that external gate and other frame scheduling have been handled by the caller.

### Start predicate

The sampled `0x43f1c9` prefix stops at the actual branch destinations, before
timer initialization or bullet spawning. When signed shot count
`player+0x43900 < 0`, start is selected if:

- pressed Shoot/Freeze bit 1, regardless of holding word; **or**
- holding Rapid bit 512 **without** holding Shoot/Freeze bit 1.

Holding bit 1 without a new press does not itself retrigger an inactive shot
cycle. Rapid+Shoot together do not take the rapid-only start path. A currently
active shot count bypasses this start predicate. This is not permission to
synthesize pressed bits from the holding word.

### Cycle continuation

`0x43f264..0x43f2e3` executes the real timer setter
`0x403280..0x4032bd` and updater `0x402ac0..0x402b18`, never a fake callout.
For an initialized timer with unit scale, count 0..14, and accumulator=count:

| Condition, checked in this order | Result `(previous,current)` |
| --- | --- |
| Count >= 6 and a new pressed bit 1 | `(-1,0)` |
| Count 14, Rapid held and Shoot/Freeze not held | `(-1,0)` |
| Count 14 otherwise | `(-2,-1)` |
| Count below 14 otherwise | `(count,count+1)` |

The native emitter call at `0x43f25d` occurs **before** this continuation and
only after its own current-versus-previous check. The C++ count helper neither
emits bullets nor models arbitrary accumulator state. Counts outside 0..14
are rejected with `nullopt` instead of extrapolating a gameplay rule.

Holding Shoot/Freeze is separately tested at `0x43f2ff` in the charge/release
dispatcher; its hold counter is `+0x43914`, not shot-cycle `+0x43900`. This
component does not reproduce that charge dispatcher or duplicate Freeze rules.
No claims about seconds, render frames, whole-frame ordering, or full firing
patterns follow from the isolated count transitions.

### Actual shot-record scheduler

At `0x43f180..0x43f19e`, original signed-byte period/phase are compared using
signed int32 `idiv`: `count % period == phase`. The fixture stops before the
emitter at `0x43f190` for due records, or at `0x43f19e` otherwise.
`recordDue` accepts period 1..127 and phase -128..127; zero/negative periods
or out-of-range phase are rejected with `nullopt`, not native divide faults.

## Bounded actual SHT reader

Public Python API: `th128_assets.shot.decode_shot(data, *, max_size,
max_groups, max_records)` returns a dictionary. Defaults/hard ceilings are
4 MiB, 256 groups, 65,536 records; lower positive integer budgets are accepted.
Invalid structures raise the existing `th128_assets.formats.FormatError`.

The native loader `0x43c298..0x43c322` proves:

- Header group count: uint16 at `+2`; actual first uint16 is 3. Its semantic
  meaning is unknown; this reader supports only the observed tag 3 layout.
- Group table: `0x268`, stride 8. First uint32 is a file-relative record offset;
  the second word is preserved as `unknownWord`, not called a formation/power.
- Record stride: `0x34` (52 bytes). Signed byte 0 is period; signed byte 1 is
  phase; a negative byte 0 ends a group, even without a full 52-byte record.
- Four callback-table indices: uint32 at `+0x24,+0x28,+0x2c,+0x30`. The loader
  resolves them through four distinct tables. IDs are retained, not executed
  or interpreted as replacement TH12 callbacks.

The original loader is also emulated on the **pinned actual** `pl00a.sht`.
Only expected group pointers/callback words change in the in-memory copy;
all other bytes remain identical. Callback tables in this structural test use
their **PE initial bytes/declared zero-fill**, all first words zero. This is
not a live runtime-table snapshot and does not establish callback behavior.
The BSS word at `0x4d2e2c` is explicit PE virtual-section zero-fill, not a
fabricated Windows function. Unsupported callback indices are rejected.

The actual file has 16 groups and 114 records, with group counts:

`[2,3,4,4,6,6,6,6,6,8,8,8,10,10,10,17]`.

Reader outputs preserve `raw` (the complete file), `headerRaw`, `tableRaw`,
each record's `raw`, `payloadRaw` (offsets 2..35), all `callbackIds`, and each
group's `tailRaw` starting at the negative terminator. `speedFloats` uses the
four confirmed offsets. Offsets cannot point into header/table or outside the
file; the next distinct group start bounds each record scan. Aliases and
nonmonotonic group order are retained without executing relocation on them.
No other payload fields are assigned speculative mechanics/damage meanings.

## Public C++ API

Namespace `th128::player`; C++20, no platform/renderer/StageWorld dependency:

| API | Contract |
| --- | --- |
| `direction(Input)` | Native priority over held direction bits |
| `isFocused(Input, FocusContext)` | Native raw-state focus predicate only |
| `rawDelta(Direction,bool)` | Unit-scale actual Cirno components; invalid enum -> `nullopt` |
| `advancePosition(RawPosition,RawDelta)` | Signed wrap, then inclusive native bounds |
| `toPixels(RawPosition)` | Float32 horizontal/vertical mirror conversion |
| `moveOneTick(RawPosition,Input,FocusContext)` | Composition of the verified unit-step movement math |
| `startsShotCycle(Input,int32)` | Start predicate; external player-state gate assumed |
| `advanceShotCycle(Input,int32)` | Initialized unit-scale counts only; otherwise `nullopt` |
| `recordDue(int32,int32,int32)` | Native signed period/phase comparison on the supported domain |

Position/delta members are `horizontal` and `vertical`. `Movement` returns
`direction`, `focused`, `delta`, `position`; `ShotCycleCounts` returns
`previous`, `current`. Every function is `noexcept`. Constants expose the
measured raw speeds/bounds/128 scale and the original key bits.

## Verification and boundaries

TDD was observed: missing probe/reader/C++ files failed first; implementations
then passed. Additional sandbox tests first demonstrated unchecked mapping
counts and a jump-to-return masquerading as a return, then passed after strict
checks. A C++ test-output precision mismatch was corrected in serialization,
not hidden with approximate movement comparisons or a production math change.

Fresh component verification is saved in `local/research/player-test-results.json`:

- Python player probe: **20 tests**, including pinned original bytes and loader.
- Python shot reader: **11 tests**, including the actual local resource.
- Node/C++: **3 tests**, with all **677 native vectors** matched both normally
  and under **UBSan**; strict warning flags and fixed C++ edge assertions.
- The original executable hash remains the pinned SHA-256 after verification.

The 677 cases cover 160 direction/focus inputs, 18 velocities, 56 position
edges, 128 shot-start inputs, 120 cycle transitions, and 195 signed record
schedules. They cover contradictory keys, irrelevant bits, gate thresholds,
inclusive bounds, signed overflow, negative remainder, and rapid+hold conflicts.

Probe sandbox: at most 200,000 instructions/call (ordinary slices use 2,048;
loader 20,000), 64 code chunks / 1 MiB code, 256 data regions / 2 MiB data,
2,048 mapped pages, a 64 KiB stack, explicit span checks and read-only guards.
Direct calls require supplied original code; indirect/unimplemented calls,
interrupts, undeclared memory/code, budget exhaustion, and fake returns fail.
No Windows/IAT/service emulation or game-process launch is provided.

Integration review update: aliased SHT groups now share their immutable
terminator-to-boundary tail bytes rather than copying a large tail per alias.
A 256-alias regression fails the old implementation and confirms shared storage
after the fix. Unknown bytes and logical group ordering are still preserved;
this does not change formation or callback semantics.

Commands from the deliverable root, with `PYTHONUTF8=1`,
`PYTHONDONTWRITEBYTECODE=1`, and `PYTHONPATH` including the shared
`../../work/reverse-libs` for Unicorn:

```text
python -B tools/research/player_probe.py --exe E:/gee/妖精大战争/th128.exe
python -B -m unittest discover -s tests -p test_player_probe.py -v
python -B -m unittest discover -s tests -p test_shot.py -v
node --test tests/player-rules.test.mjs
```

The Node harness uses the installed SDK 4.0.22/bundled Python 3.13.3, normalized
SDK `EM_CONFIG`, shared `work/emscripten-cache-4.0.22` with **frozen cache**,
and `EMCC_CORES=BINARYEN_CORES=2`. Single-file test executables are transient
in a uniquely named OS temporary directory and removed; no deliverable build
or site files are changed. Only these component suites are run, not an
integration/full-game suite.

**Unknown/unsupported:** real world-manager lifecycle and hold-counter wiring;
non-unit time/movement scaling; animation side effects and full-frame ordering;
Freeze/bomb activation; shot-group selection, actual bullet spawning/formations,
callback effects, collision/damage application, and rendering. Do not advertise
this as full player/shooting support. Main owns StageWorld integration and
ECL/global RNG; this component does not alter those responsibilities.

## Changed paths

- `src/game/PlayerRules.hpp`
- `src/game/PlayerRules.cpp`
- `tools/research/player_probe.py`
- `tools/th128_assets/shot.py`
- `tests/test_player_probe.py`
- `tests/test_shot.py`
- `tests/player-rules.test.mjs`
- `tests/native/player-rules.cpp`
- `docs/research/player-rules.md`
- `local/research/player-native-fixtures.json`
- `local/research/player-test-results.json`
