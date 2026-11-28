# TH128 ECL reader and bounded control VM

Integration update: the later `Program::linkIncludes` and typed reference-provider
API are documented in `ecl-linking.md`. The original component boundaries below
describe its initial standalone behavior; automatic filesystem linking remains
disabled, while hosts may explicitly link prevalidated loaded resources. Unknown
game references still fault unless an explicit provider handles them.

## Implementation plan

This component follows the supplied implementation specification directly, in
the shared checkout, without delegations, commits, game execution, or writes to
the original installation. Only `src/ecl`, `tests/ecl-*`,
`tests/native/ecl-*.cpp`, and this document belong to this task.

- [x] Write failing reader fixtures; implement transactional owned SCPT/1 parsing.
- [x] Write failing VM fixtures; implement checked control, stack, and arithmetic.
- [x] Check native/table differences, especially TH128 opcodes 22, 71, and 85.
- [x] Load all 33 Japanese ECL resources through the catalog-selected manifest;
      verify every available `main` stops explicitly at unhandled opcode 402.
- [x] Run ordinary and undefined-behavior-instrumented tests; document exact API,
      safety bounds, evidence, and unsupported behavior.

The reader owns the complete resource and preserves undecoded instruction words.
The VM snapshots a loaded program and has no engine/global/enemy dependencies.
Unknown instructions require an explicitly registered, accepting host handler;
they never silently succeed. Included resources are reported, not opened by the
reader. No part of this work claims a playable stage.

## Sources and evidence boundaries

- MIT TH12 reference, commit `654717ac6d7b0f9e28239e230d5c965025493434`:
  `ports/th12/src/game/EclProgram.cpp`, `EclInterpreter.cpp`, and `GameTypes.hpp`.
  The original MIT notice is retained in `src/ecl/UPSTREAM_LICENSE.txt`.
  This is a separate C++17 implementation, not a port of the TH12 game dispatcher,
  globals, enemy state, bullets, RNG, or engine scheduling.
- Local `tools/th128_assets/formats.py::parse_ecl` supplies the SCPT/ECLH framing
  and corruption/bounds reference. The reader additionally rejects aliased
  includes, duplicate include sections, nonzero padding, and unsupported ECLH
  data offsets. The subroutine count is the full **32-bit** field at header +16,
  not the low 16 bits used by the referenced TH12 reader.
- `thpatch/thtk`, commit `917a3d3858850f01276b7fb5b9327e2c24d27d5b`,
  `thecl/thecl10.c`: the TH128 lookup falls through the TH125, TH12, TH11, and TH10
  format tables. `th128_fmts` explicitly declares **22 = `Sm`** and **71 = `""`**.
  An empty format means no immediate payload, **not** a no-op. Format declarations
  alone are not proof of execution semantics. No thtk implementation code is
  copied into this component.

### Checked Japanese original-byte evidence

`tests/ecl-native-evidence.py` checks the original file's SHA-256 before and after
inspection:

`ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`

The PE image base is `0x400000`. Core dispatch at `0x46e100` uses the index bytes
at `0x46fdc0` and handler table at `0x46fcc0`.

| Opcode | Observed handler | Evidence and implementation choice |
| --- | --- | --- |
| 22 | `0x46fbe5` | Retail core shares the advance-only slot with opcode 0. Portable VM nevertheless requires explicit host acceptance of decoded `Sm` callback registration; it does not claim retail debug callback triggering is verified. |
| 71 | `0x46eef5` | Typed pop, integer conversion, `sete`: integer logical-not, **not no-op**. Five original-byte cases include float-to-integer conversion. |
| 72 | `0x46f2ae` | Typed pop and comparison against float zero; float logical-not. Static evidence only. |
| 84 | `0x46f5ed` | Integer negation; original-byte `INT_MIN` case wraps. |
| 85 | `0x46f64c` | Native **`fchs`**, not TH12's raw-word negation. Three original-byte cases include integer input and signed zero. |
| 50 | `0x46e524` | Integer addition; original-byte overflow case wraps to `INT_MIN`. |

The optional Unicorn probes run **10 cases**, each limited to 1,024 instructions
and 100 ms of emulator execution. Original code pages are mapped read/execute,
not writable; synthetic context and stack pages are separate. A synthetic,
read-only zero CRT feature-flag page selects the original x87 conversion helper;
the x87 control word is `0x37f`. This does not run the Windows executable, load
game DLLs, start the game, or write to the original installation. The probe emits
only addresses, input/output words, and hashes, not redistributable game code.

The remaining generic control/numeric operations use compatible documented
formats, the MIT reference, and checked synthetic fixtures. They have **not all
been differentially verified against the original TH128 executable**. The
explicit scheduler/local-frame policies below must not be represented as full
native engine fidelity.

## Exact public API

All types are in namespace `th128::ecl`. Include `ecl/EclProgram.hpp` and
`ecl/EclVm.hpp`; compile `src/ecl/EclProgram.cpp` and `src/ecl/EclVm.cpp`. There are
no engine, runtime, filesystem, graphics, or JavaScript dependencies.

### Program and owned resource

```cpp
bool Program::load(const void* bytes, std::size_t size, std::string& error);
bool Program::loaded() const;
const Resource& Program::resource() const;
const std::vector<Subroutine>& Program::subroutines() const;
const Subroutine* Program::find(std::string_view name) const;
const Instruction* Subroutine::instructionAt(std::uint32_t offset) const;
```

- `load` is transactional. Failure returns `false`, sets a diagnostic, and leaves
  the previous successful resource intact. Success clears the diagnostic.
- A `Resource` contains `bytes` (the **whole** owned resource), `animationIncludes`,
  and `eclIncludes`. All header/reserved/payload bytes remain available in `bytes`.
  Input storage may be freed or changed immediately after `load` returns.
- A `Subroutine` contains `name`, `fileOffset` (the ECLH header), `codeSize`, and
  ordered `instructions`. Name lookup is case-sensitive. `instructionAt` accepts
  only exact instruction starts, not payload addresses or the subroutine end.
- An `Instruction` retains `offset`, signed `time`, `opcode`, `size`,
  `parameterMask`, `rankMask`, `parameterCount`, unsigned `stackOffset`, and
  little-endian `rawWords`. Offsets are relative to **ECLH +16**, not the file.
  Unknown opcode payloads remain lossless; parsing is not execution support.
- `Program` copies share immutable data. References from a particular `Program`
  must not outlive that data; successful replacement of that program can
  invalidate its previously returned references. `Vm` takes its own snapshot.
- Includes are validated identifiers, **not opened or recursively linked**.
  There is no path resolution, file I/O, implicit override, filename-prefix
  stage mapping, or cross-resource call resolver. Calls to unloaded included
  subroutines fail explicitly with `MissingSubroutine`.

### VM lifecycle, host registry, and diagnostics

```cpp
explicit Vm::Vm(const Program& program, Limits limits = {});
bool Vm::registerHostOpcode(std::uint16_t opcode, HostOpcode callback,
                            std::string& error);
bool Vm::start(std::string_view entry, std::uint8_t rankMask = 1);
TickResult Vm::runTick();
VmStatus Vm::status() const;
const Error& Vm::error() const;
std::vector<ThreadSnapshot> Vm::threads() const;
using HostOpcode = std::function<bool(HostCall&)>;
```

- Constructing from a temporary program is safe. Reloading or destroying the
  caller's `Program` cannot change the VM snapshot. `Vm` itself is noncopyable.
- `start` resets thread state and the previous error, but retains registered
  handlers. The main thread has ID 0, time 0, and initially empty locals/stack.
  The rank argument is a nonzero **bit mask**, not a TH12 difficulty ordinal.
- `TickResult` contains `status`, `instructions`, and `skipped`. `instructions`
  counts visited instructions, including rank skips and the faulting opcode;
  budget exhaustion itself does not count an additional instruction.
  `VmStatus` is `Ready`, `Running`, `Finished`, or `Faulted`.
- `Error` contains `code`, `message`, `subroutine`, `offset`, `opcode`, and
  `threadId`. `offset` uses the instruction's code-relative coordinate system.
  The first error is sticky. After a fault, repeated ticks execute zero
  instructions and retain that diagnostic. Ticking before `start` is an explicit
  `InvalidProgram` error. Ticking after normal completion returns `Finished`.
- Registration rejects null handlers, core overrides, duplicate registrations,
  and exhausted registry bounds. An absent handler or a callback returning
  `false` produces `UnsupportedOpcode`; execution does not continue afterward.
  A helper failure remains a VM fault even if the callback returns `true`.
- With C++ exceptions enabled, thrown callbacks become `HostFailure`. With
  exceptions disabled, callbacks **must not throw**. The core builds and runs in
  both modes. Use `-fexceptions` if the integration promises exception-to-error
  conversion; no flag change is needed for a nonthrowing diagnostic callback.
- `start`, `runTick`, or registry mutation during a callback fails explicitly
  with `ReentrantCall`. A VM is single-threaded, not a concurrent host object.

`ThreadSnapshot` contains `id`, signed `asyncId`, `active`, `subroutine`, next
`offset`, `time`, `callDepth`, raw-word `locals`, and typed `stack`. It is an owned
copy for diagnostics. Inactive slots may later be reused with a new ID.

`ErrorCode` values are `None`, `InvalidProgram`, `InvalidLimits`,
`MissingSubroutine`, `InvalidInstruction`, `UnsupportedOpcode`,
`UnsupportedReference`, `StackUnderflow`, `StackLimit`, `CallLimit`, `ThreadLimit`,
`InstructionBudget`, `InvalidBranch`, `LocalBounds`, `NumericError`, `HostFailure`,
and `ReentrantCall`.

### HostCall and opcode 22 registration

```cpp
const Instruction& HostCall::instruction() const;
std::uint64_t HostCall::threadId() const;
float HostCall::time() const;
const CallbackRegistration* HostCall::callbackRegistration() const;
bool HostCall::readInteger(std::size_t word, std::int32_t& value,
                           std::size_t parameter = automaticParameter);
bool HostCall::readFloat(std::size_t word, float& value,
                         std::size_t parameter = automaticParameter);
bool HostCall::readString(std::size_t word, std::string& value,
                          std::size_t parameter = automaticParameter);
bool HostCall::storeInteger(std::int32_t byteOffset, std::int32_t value);
bool HostCall::storeFloat(std::int32_t byteOffset, float value);
bool HostCall::push(Value value);
```

`automaticParameter` defaults the logical parameter index to the raw word index.
For variable-sized strings/typed descriptors, pass the correct logical parameter
index explicitly: mask bits address **parameters**, not raw payload words.
Helpers check payload/local/stack bounds and set sticky diagnostics on failure.
Masked `-1` reads normally consume one typed operand; do not repeatedly resolve
the same stack argument. Game/global negative references are unsupported.

Opcode **22 is not a call**: payload is signed slot `S`, then an `m` string
(`u32` padded byte length followed by NUL-terminated ASCII and zero padding).
The VM validates exactly two logical parameters and the entire payload before
dispatch. `callbackRegistration()` exposes `{slot, subroutine}` for the host to
record; its slot has already been resolved. `readInteger(0, ...)` returns that
cached slot rather than popping a masked `-1` twice. There is no implicit
subroutine invocation, enemy debug event system, or silently successful no-op.
Without a registered accepting handler, **22 explicitly faults**.

```cpp
std::string error;
vm.registerHostOpcode(22, [&](th128::ecl::HostCall& call) {
    const auto* registration = call.callbackRegistration();
    if (!registration)
        return false;
    return recordHostCallback(registration->slot, registration->subroutine);
}, error);
```

The example's `recordHostCallback` belongs to the caller, not this component.
`HostCall` and the registration pointer are valid only during the callback; copy
data that must survive it. Hosts are trusted code: the VM cannot meter or roll
back arbitrary host-side time, allocation, or side effects.

`Value` has `bits`, `type` (`ValueType::Integer`/`Float`), static constructors
`integer(int32_t)` and `floating(float)`, and conversions `asInteger()` and
`asFloat()`. Float-to-integer conversion truncates toward zero; out-of-range or
nonfinite values produce the x86 `INT_MIN` sentinel. VM float boundaries reject
nonfinite operands/results. Compilation requires 32-bit IEEE-754 `float`.

## Implemented control and numeric semantics

| Opcodes | Implemented behavior |
| --- | --- |
| 0 | Intentional verified no-op. |
| 1 | Terminates only the current VM thread; does not remove an enemy or trigger game rules. |
| 10 | Checked return, or terminates the current top-level thread. |
| 11 | Synchronous named call with bounded typed `ii`, `if`, `fi`, `ff` descriptors and logical parameter masks. |
| 12 / 13 / 14 | Relative jump / integer-zero / integer-nonzero branch; condition is popped; target must be an exact instruction boundary in the same subroutine; sets the signed target time. |
| 15 / 16 | Spawn untagged / signed-tagged asynchronous context. |
| 17 / 21 | Stop matching tagged async contexts / all nonmain contexts. |
| 40 / 41 | Allocate / free checked aligned local bytes. Allocation preserves supplied argument words; repeated allocation and unmatched free fault. |
| 42 / 43 | Push integer operand / pop and store integer into a reference-masked local destination. |
| 44 / 45 | Float equivalents; a float local address encodes a numeric byte offset. |
| 50–58 | Alternating integer/float add, subtract, multiply, divide; 58 is integer remainder. |
| 59–70 | Integer/float pairs: equal, unequal, less, less-or-equal, greater, greater-or-equal; integer boolean result. |
| 71 / 72 | Integer / float logical-not; typed integer result. |
| 73 / 74 | Integer logical-or / logical-and. |
| 75 / 76 / 77 | Integer bitwise XOR / OR / AND. |
| 78 | Local post-decrement: pushes old integer, stores wrapped decrement. |
| 83 | Subtract resolved integer wait duration from the current float32 time. |
| 84 / 85 | Wrapped integer negation / native TH128 float negation. |

Integers add/subtract/multiply/negate using unsigned words, so wrapping has no
C++ signed-overflow UB. Division/remainder by zero and `INT_MIN / -1` fault,
rather than copying TH12's zero/default fallbacks. Float division by zero,
nonfinite results, and a clock that can no longer advance one tick also fault.

Time starts at zero. A tick executes due instructions, then advances each
surviving scheduled context's current frame by one. Integer instruction times
are compared without first rounding them to float32. Rank skips still consume
the aggregate budget. Waits may take multiple ticks; branches replace time.

Calls preserve the caller's locals, operand stack, and time. Callees get their
own checked local and operand frames; callee residue is discarded on return.
`maxCallDepth` counts the main frame as one; snapshots report only nested depth.
Local-byte and operand-value limits are aggregate **across a thread's frames**,
not a fresh allowance for every recursive call.

The bounded scheduler snapshots active thread IDs in stable slot order at tick
entry. Children begin **next tick**, not during their spawning tick. Cancelled
IDs are skipped; inactive slots can be reused without growing retained context
count. Main completion does not apply native enemy-deletion rules: the VM is
`Finished` when every context is inactive. These are explicit component policies,
not claims of verified native enemy/coroutine scheduling.

## Bounds and unsupported behavior

| Bound | Default | Accepted maximum / policy |
| --- | --- | --- |
| Resource bytes | Fixed | 32 MiB before any dereference/allocation |
| Subroutines | Fixed | 4,096 per resource |
| Instructions | Fixed | 200,000 aggregate per resource |
| Includes | Fixed | 4,096 aggregate across sections |
| Identifiers | Fixed | 4,096 bytes including NUL; safe archive filenames at most 200 characters |
| Inline padded strings | Fixed | 4,096 bytes |
| `maxStackValues` | 512 | 4,096 aggregate per thread |
| `maxLocalBytes` | 4,096 | 4,096 aggregate per thread; positive aligned configured bound |
| `maxCallDepth` | 64 | 256 frames including the root |
| `maxThreads` | 16 | 64 retained slots, including main/inactive slots |
| `maxInstructionsPerTick` | 10,000 | 1,000,000 aggregate visits across all scheduled threads |
| `maxHostOpcodes` | 256 | 1,024 unique registered handlers |

Zero/out-of-range configured limits and zero rank masks are rejected. Stack and
call overflow, invalid branch boundaries, fall-through beyond a routine, missing
targets, unaligned/out-of-range locals, bad string padding, invalid descriptors,
and invalid opcode shapes all stop the VM. Underflow never synthesizes zero.

Only zero instruction `stackOffset` is executable; nonzero values are preserved
by the reader but explicitly rejected by the VM until semantics are verified.
Only local references and the typed `-1` operand pop are supported. Other
negative references, RNG/global variables, cross-resource linking, interpolation,
trigonometry/vector instructions 79–82 and 86–92, game-specific opcodes, enemies,
bullets, bosses, cameras, rendering, and debug-event triggering are not supplied.
Noncore opcodes can be accepted explicitly by the host, otherwise they halt.

Archive includes reject path separators, traversal, Windows device names,
trailing dots, duplicate names, and ASCII case aliases, including aliases across
include sections. Duplicate routine names/offsets and overlapping/out-of-bounds
framing are rejected. Complete instruction-boundary prefixes cannot be
distinguished from intentionally shorter routines by the format itself; an
empty routine is structurally readable but is not a valid VM entry/call target.

## Verification and integration

From `outputs/th128-web`:

```powershell
node --test tests/ecl-runtime.test.mjs
$env:TH128_ECL_SANITIZE='1'
node --test tests/ecl-runtime.test.mjs
Remove-Item Env:TH128_ECL_SANITIZE
python -B -X utf8 tests/ecl-native-evidence.py --exe "E:/gee/妖精大战争/th128.exe"
```

The runner uses the bundled Emscripten **4.0.22** Python **3.13.3**, sets
`EM_CONFIG` and `EM_CACHE` to forward-slash paths, uses
`work/emscripten-cache-4.0.22`, and limits `EMCC_CORES` and `BINARYEN_CORES` to 2.
No SDK/default cache configuration or sanity check is changed. Generated build
files stay under `tests/ecl-build`. Missing toolchain/catalog inputs are failures,
not skipped tests. The optional original-byte probe uses installed `pefile` and
`capstone`, plus Unicorn from `work/reverse-libs`.

The three C++/WASM test groups cover:

1. Owned/transactional parsing, exact metadata/branch boundaries, every partial
   truncation of the small fixture except its structurally valid empty tail,
   invalid offsets/sizes/includes/names, aggregate limits, and **2,000 deterministic
   byte mutations**. Actual files are selected only by `local/catalog.json` →
   Japanese manifest → `manifest.parent/raw`, not by a hard-coded generation.
2. Numeric conversions/wrap/TH128 differences, stack/local/call/thread/budget
   faults, rank/mask/timing/branch behavior, async cancellation, typed descriptor
   masks, host rejection/helper faults/exceptions/reentry, and snapshot ownership.
3. Explicit **`-fno-exceptions`** consumer with a nonthrowing host callback,
   structured fault fields, repeated-fault idempotency, and destroyed input owner.

Corpus verification loads **33 files / 1,459 subroutines / 49,097 instructions**.
Exactly **16 files contain `main`**; each starts, allocates locals with opcode 40,
and stops on the second visited instruction: **`UnsupportedOpcode`, opcode 402,
`main` offset 20**, before opcode 22 where present. Files without `main` are
library/boss resources, not silently invented stage entries. No stage/STD mapping
is inferred from ECL filename prefixes.

Integration can leave 402 unregistered (same explicit diagnostic) or register a
nonthrowing diagnostic host that declines it. Check every boolean result and
`TickResult.status`; serialize the structured `Error` fields for the frontend.
The parent owns `src/runtime`, `src/engine`, frontend, and build tooling; this
component does not modify them or claim that a stage is playable.

### Final recorded results — October 29, 2026

| Check | Result |
| --- | --- |
| Ordinary Emscripten suite | **3/3 passed**, no skips/failures; 18.68 s |
| UBSan suite, nonrecovering instrumentation | **3/3 passed**, no skips/failures; 26.14 s |
| Original-byte Unicorn probes | **10/10 passed**, original SHA-256 unchanged |
| Actual resource corpus in both suites | **33/33 loaded**, 1,459 subroutines, 49,097 instructions |
| Actual `main` diagnostics in both suites | **16/16** explicit unsupported opcode 402, offset 20, before 22 |
| Exception-disabled host consumer | Passed in ordinary and UBSan suites |
| Upstream MIT license | Byte-for-byte SHA-256 match to the referenced TH12 license |

The final regression pass includes both positive and negative float-clock
precision boundaries, aggregate cross-thread instruction budgeting, reentrant
registry protection, and a masked opcode-22 slot resolved exactly once. The
native probing script is optional; the ordinary component tests require no
original EXE access. No production integration file was changed by this task.
