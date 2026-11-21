# TH128 ANM component: bounded implementation and evidence

## Plan and boundary

The user-approved spec requires the MIT TH12 v7 Registry/VM/State/Runtime and
Scene/Node APIs under `th128::anm`, not the TH12 renderer, host, or
PresentationTracking subsystem. State field order is retained. No original game
or installer is launched; the Japanese PE is read-only. Main owns the
Emscripten installation and RNG derivation.

- [x] Establish failing synthetic C++/WASM loader, scheduler, and VM fixtures.
- [x] Import the licensed generic files with preserved MIT attribution.
- [x] Harden structural/aggregate validation and stop unsupported execution.
- [x] Record original-byte evidence for observed TH128 opcodes 111/113/114.
- [x] Verify synthetic and prepared original-data fixtures using Emscripten.

## Evidence hierarchy

The prepared Japanese catalog contains 53 ANM archives, 311 entries, 2,085
sprites, 1,376 scripts, and 26,081 non-sentinel commands: 76 observed opcodes.
Manifest coverage is presence evidence, not semantic proof.

`tools/th128_assets/formats.py` is the bounded M0 binary-format reference.
`work/upstream/thanm_thanm.c` lists v4+ operand forms also used by v7; these are
operand-form hints, not proof of TH128 semantics.

The three logical interpreter files and two Scene files originate in
`work/eagler-source/eagler-touhou-654717ac6d7b0f9e28239e230d5c965025493434/ports/th12/src/game/`.
Their TH12 native-address comments are **not** evidence of TH128 equivalence.
The original MIT license accompanies the adapted files as
`src/animation/UPSTREAM_LICENSE.txt`.

## Verification command

From the workspace root, once main's Emscripten 4.0.22 installation is ready:

```powershell
python outputs/th128-web/tests/animation-run.py --real --sanitize
```

The runner writes its own generated fixture artifacts under
`tests/animation-build/` only. It neither edits shared build scripts nor installs
tools. Native evidence and exact stop conditions are recorded below as verified.

## Original-byte investigation (2026-10-29)

Pinned Japanese executable: SHA-256
`ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`,
752,640 bytes, image base `0x400000`. The original is never executed as a Windows
process. `tests/animation-native-evidence.py` checks its identity before analysis,
checks dispatch-table entries, captures handler bytes/disassembly, and optionally
writes only `local/research/anm-*.json`.

At `0x459e31`, the native interpreter sign-extends the 16-bit opcode, adds one,
bounds-checks against 115, then indexes the table at `0x45d0d4`. This links the
following handlers to actual opcodes, rather than guessing from thtk names:

| Opcode | Handler | Observed uses | Proven operation / remaining boundary |
| --- | --- | ---: | --- |
| 111 | `0x45a4a1..0x45a4d5` | 163 | Reads one integer, resolving mask bit 0 through `0x459b50`; replaces native VM+`0x4b4` bits `0xc0` with `(value << 6) & 0xc0`; advances PC by the command length. |
| 113 | `0x45b3e1..0x45b48d` | 20 | Reads the third operand as a float (mask bit 2), calls `0x46a700` on it and VM+`0x2c`, resolves duration using mask bit 0, stores raw mode, initializes a track at VM+`0x1b4`, and sets a flag. Complete track-update/rotation interaction is not established. |
| 114 | `0x45b7b9..0x45b80f` | 1 | Changes render flags, resolves a masked integer, computes `56 * count`, calls `0x472f6f`, and stores its result at VM+`0x4ac`. Allocation/geometry ownership, update, and rendering semantics are not recovered. |

Opcode 111 is additionally function-emulated in Unicorn: **24/24 cases** cover
literal values 0/1/2/3/4/-1 with three prior flag patterns, masked integer0, and a
masked non-variable literal. Only two read/execute original code pages and three
isolated data/stack pages are mapped; each case must return to `0x459e1a` within
128 instructions with a balanced stack. No allocator, RNG, Windows APIs, game
loop, or renderer are available. This proves the flag write, **not** the entire
opcode's engine-level effects.

The corresponding tick consumer at `0x459dfb..0x459e14` skips work when the
`0xc0` field equals `0x40` and the external object at global `0x4b8954` has bit 1
set at object+`0x60`. Its relationship to host scheduling/pause state is not
verified at that initial checkpoint. **The2026-10-30 continuation now verifies111's
consumer with sixty original gate prefixes and requires an explicit optional
control snapshot; see `midboss-and-anm-control.md`.113/114 remain unsupported**;
recording their partial semantics is not permission to silently ignore their
external effects or substitute TH12 behavior.

Prepared data details: all 111 operands are unmasked, 130 use value 0 and 33 use
value 1. Opcode 113 occurs in title scripts 120..124, with duration 10, mode 9,
and targets 0, pi/2, pi, -pi/2. Opcode 114 occurs in `front.anm` global script 162
at command byte offset 56, mask 1, integer variable identifier 10000. The JSON
evidence contains every occurrence and original payload, not a compatibility
claim.

Reproduce the optional private report:

```powershell
python -B -X utf8 outputs/th128-web/tests/animation-native-evidence.py --exe "E:/gee/妖精大战争/th128.exe" --report outputs/th128-web/local/research/anm-native-evidence.json
```

## Title integration evidence and limits

Main's independent native title investigation identifies `title.anm` as bank 24
and `title_v.anm` as bank 25. Native `0x4670b0`, called from `0x445eb9` and
`0x445ed2`, binds global scripts 97 and 101. At title clock 60 the menu root is
global script 0. Script 97 spawns 99 and 98; 99 spawns 100; 101 spawns 103/104.
These are global table-order indices, not the per-entry source IDs.

The real-data fixture loads the catalog-selected raw `title.anm`, binds 97/101,
ticks both scheduler domains for 60 frames, verifies all seven boot scripts are
present without unsupported callbacks, and binds menu root 0 plus bank25/script0
from `title_v.anm`. At clock 71 it delivers immediate IRQ3 to the root, queued
IRQ17, and immediate IRQ31 to nonselected scripts 4..9. It then runs through
clock 120, ending with 23 retained nodes and no unsupported callback. Main's
native investigation identifies these calls as `0x4674f0` (immediate),
`0x467480` (queued/propagated), and `0x444e80` (nonselected items). The second
nonselected item set (item+10), IRQ30 path, and every selection transition are
not comprehensively tested here. This is a bounded logical integration check,
not proof of draw fidelity, final menu behavior, input handling, or full title
compatibility.

Scene's synchronous birth ticks, birth-before-registration ordering,
primary/secondary head/tail insertion, saved-successor traversal, intrusive child
chain, interrupt propagation, detached ownership, and collection behavior are
**inherited from the MIT TH12 implementation, not verified TH128 scheduling**.
The only removed game dependency is PresentationTracking instrumentation;
presentation discontinuity/reset calls are not animation scheduling operations.

## Host contract

- `Registry::loadBank(id, bytes, length, error)` is transactional. Failed loads
  leave an existing bank untouched. Scripts and sprites use global table-order
  ordinals across entries. `Script::sourceId` is metadata, not the bind index.
- `VM::bind` still executes a synchronous birth tick and returns whether the
  binding exists. A successful bind can fault during that tick: check
  `VM::faulted()`, `diagnostics().faulted`, or the `Runtime::unsupported` callback.
  `tick` returns true for either a normal end or a fault. Rendering must not tick.
- `State` fields, ordering, and defaults remain unchanged from the imported
  interface. Additional persistent diagnostics (`faulted`, `faultOpcode`,
  `reason`) are appended to `Diagnostic`, not State. Diagnostics retain command
  PC and script-relative byte offset after a halt even without a host callback.
- Runtime RNG callbacks and child construction are host-owned. No RNG seed or
  TH128 RNG algorithm is introduced here. Scene supplies its child scheduler.
- `ChildRequest::failure` is an appended internal scheduler-to-VM failure
  channel. It propagates failed child bindings and birth/node limits into a
  parent stop; it does not change the original State layout or spawn callback
  signature.
- Primary and secondary ticks, collection, and embedded scheduling remain
  explicit host calls. Scene has no renderer or platform dependency.

## Validation budgets and stop conditions

The loader reads little-endian words explicitly. Each entry must be a forward,
aligned v7 region with bounded tables, a terminated name, non-aliased disjoint
sprite/script/name/texture offsets, finite sprite/UV geometry, and (when present)
a valid THTX header and bounded pixel payload. Every script is confined by the
next physical resource boundary and requires an eight-byte sentinel header.
Commands require aligned payloads, exact known operand counts, and no variable
mask bits outside their payload. Unknown opcode bytes are retained for a later
explicit execution fault, not rejected merely for being unknown.

Per-bank limits: 128 MiB input, 4,096 entries, 65,536 sprites, 4,096 scripts per
entry / 16,384 total, 200,000 commands including sentinels, 1,000,000 operand
words, and 4,096 bytes per name. Registry limits: 256 resident banks, 262,144
sprites, 65,536 scripts, 1,000,000 commands, and 4,000,000 words. Replacing a bank
does not count its prior registry allocation twice; external VMs retaining old
bank references remain the host's responsibility.

Scene bounds synchronous birth depth at 64 and retained nodes at 4,096. A VM's
default per-tick instruction budget is 4,096, configurable through Runtime.
Those safety limits are adaptations, not claims about native game limits.

Execution **halts** on an unknown/unrestored opcode (including observed
113/114 or a missing required111 control snapshot), missing required RNG or child callbacks, failed child construction,
missing created-child state for op96/97, invalid command operands/mask, an
invalid bank/script or jump target, return without an interrupt continuation,
integer divide exception, random-sprite division by zero, invalid/non-finite
time delta, unsupported interpolation mode outside 0..17, or an
instruction/birth/node budget breach. No later instruction,
velocity/interpolation tail, or timer advance executes after a fault. The current
command's State changes are rolled back, preserving earlier successful commands;
host callback side effects are not transactionally undone. A fault is reported
once per VM and cannot be resumed by an interrupt; an explicit rebind is required.

Normal sentinel/op2 completion and op1 deletion are not unsupported faults.
Known implemented opcodes and interpolation/timing/variable behavior are
**inherited TH12 semantics**, exercised by synthetic and bounded real-data tests,
not automatically verified against the TH128 executable. Font-bank defaults and
special rendering modes also remain host integration assumptions unless stated
otherwise.

## Verification record

The initial licensed baseline failed alias/overlap rejection, malformed-resource
validation, aggregate limits, and unknown-op stop fixtures. A second red run
also demonstrated missing Scene birth-depth and invalid-delta stops. These were
fixed only after observed WASM fixture failures.

An Emscripten 4.0.22 / bundled Python 3.13.3 run passed 12 synthetic fixture groups,
all 53 original Japanese bank loads, and the title boot/menu-root fixture. A
subsequent undefined-behavior-sanitized run passed the same set with no UBSan
diagnostics. Further adversarial checks and the final rerun are recorded below.

The existing Python suite also completed: 93 tests, 30 expected skips, no
failures (`python -B -m unittest discover -s outputs/th128-web/tests -p 'test_*.py'`).
This does not stand in for the host's browser/renderer or complete gameplay tests.

The fixture runner invokes bundled Python 3.13.3 with forward-slash-normalized
`EM_CONFIG=<sdk>/.emscripten`, `EMSDK_PYTHON=<sdk>/python/3.13.3_64bit/python.exe`,
an isolated `EM_CACHE=tests/animation-build/cache`, and two compiler workers.
It does not edit the SDK configuration or use its default cache. Build artifacts
and temporary files stay under its own `tests/animation-build/` directory.

### Final component result

`python -B outputs/th128-web/tests/animation-run.py --real --sanitize` completed
with exit code 0: 12 synthetic groups plus a separate bounded-angle-normalization
probe, all 53 Japanese bank loads, and the 120-frame title/overlay/IRQ fixture
with 23 retained nodes. There were no UBSan diagnostics. Synthetic coverage
includes script/operand aggregate limits, persistent fault diagnostics, no
post-fault State writes or time advance, invalid runtime arguments, and Scene
birth-depth limits. No full renderer or native screenshot parity is claimed.

Additional red tests demonstrated unsupported interpolation modes continuing and
the inherited angle helper hanging on maximal finite float inputs. The former
now stops explicitly. The latter is bounded to 34 adjustments, matching original
TH128 helper `0x46a700`'s shared iteration limit and exact float32 pi/tau constants.
**11/11 original-byte Unicorn cases**, including both signs of maximal float32,
match that bounded normalization bit-for-bit within a 1,024-instruction cap.
Those cases are included in the private evidence JSON. This is the only
additional native-function adaptation; the broader interpreter/scheduler remains
inherited unless explicitly identified above.

No SDK configuration, shared runtime build script, renderer, server, host,
engine RNG, or other worker source was edited. No commits were created. The
component is ready for main's integration review; at that historical checkpoint111/113/114 were deliberate
fault boundaries rather than silently approximated features.
