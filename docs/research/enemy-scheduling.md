# Bounded TH128 enemy scheduling evidence

Date: 2026-10-29. Input: read-only Japanese x86
`E:/gee/妖精大战争/th128.exe`, SHA-256
`ba515750ece845b56640fe440592fc39ebb7bdd1ed87c541b67a19b148e767d4`.

## Decision and proof levels

**Local scheduling is recoverable; a complete native frame schedule is not
proved.** The original constructor invokes the per-enemy update **before
linking its own node**. The manager caches each node's successor **before**
calling that update. An update guard, `Enemy.flags & 0x00080000`, prevents a
second motion/ECL execution on a guarded visit; the manager then clears that
guard after a zero update result. Therefore neither “update every listed enemy
every browser frame” nor “all newly appended enemies run motion this pass” is
justified.

The distinctions throughout this report are:

- **Static:** real instructions and direct-call operands decoded from the
  hash-checked PE, with addresses, range hashes and disassembly in private JSON.
  This is local control/data-flow evidence, not a recorded native event trace.
- **Emulated:** bounded execution of those original x86 bytes under Unicorn,
  with declared synthetic object graphs, registers, clock values and stack
  frames. No code is patched and no external return value is synthesized.
- **Not proved:** full native constructor/allocation, a real ECL spawn during a
  manager iteration, complete callback/frame execution, or browser equivalence.

This work changes no `EnemyRules`, StageWorld, Bullet501, ETEX, runtime, build
or site production files. The main worker's existing world-501 stop is not
removed or weakened by this evidence.

## Manager global and callback path

`[0x4b8948]` is the enemy-manager pointer: spawn entry loads it at `0x414ff4`;
the zeroing initializer stores `ESI` there at `0x4152fa`. The latter calls the
original memset at `0x4152ef` to zero `0xcc` manager bytes. These are static
observations, not a fixture running all manager initialization.

| Manager field | Offset | Evidence |
|---|---:|---|
| Head node | `+0xb8` | Read at `0x4158f5`; empty-list store at `0x41520d` |
| Tail node | `+0xbc` | Read at `0x415215`; new tail stored at `0x415238` |
| Node count | `+0xc0` | Increment at `0x415231` |
| Per-pass counters | `+0x24/+0x28` | Zeroed at `0x4158fb/0x415902` |

Manager initialization prepares a callback at `0x41539f..0x4153c0`:
`[callback+8] = 0x415a20`, `[callback+0x20] = manager`, and `EBX = 0x15`
(priority **21**). Direct call `0x4153c0 -> 0x4681d0` inserts it in the tick
chain belonging to `[0x4d2e14]`, rooted at scheduler `+0x14` with head at
`+0x18`. The registration walk compares signed priorities at `0x468226` and
stops on `jge` at `0x468228`: ascending priorities, new equal-priority entries
inserted before old ones. Only an empty-chain insertion is emulated here;
relative ordering of every real installed callback is not inventoried.

The tick dispatcher `0x468310` reads that head at `0x468333`, caches the next
callback at scheduler `+0x48`, prepares `ECX = [callback+0x20]` at `0x468387`,
and calls `[callback+8]` through `EDX` at `0x46838d`. The dispatcher fixture
stops **before** this indirect call, even though its synthetic `EDX` is the
known `0x415a20`. A separate fixture enters that real callback directly; these
are not spliced into a fictitious complete scheduler run.

Callback `0x415a20` returns 1 without entering the manager if `[0x4b8954]` is
null, or if the pointed object's `+0x60` word has any of bits `0x1`, `0x2`,
`0x4`, `0x400` set. Do not assign an unproved gameplay meaning to these bits.
Otherwise the real direct call is `0x415a52 -> 0x4158f0`, with `EDI = manager`.
All four blocking bits, the null pointer, and the active guarded path are
emulated. Blocked callbacks neither advance enemies nor clear their guards.

**Priority 21 alone does not justify one local pass per browser frame.** It is
a chain-sort key, not a frequency, display cadence, or proof that this callback
is active. Registration clears callback flag `0x2` at `0x4153a2`, while the tick
dispatcher tests that bit at `0x46835a`; separate function `0x415630` sets it on
the manager callbacks at `0x415643/0x41564f`. The dispatcher fixture explicitly
seeds an enabled callback and does not prove native activation timing. Gates
can suppress the manager, and multiple/zero scheduler invocations per browser
frame have not been excluded.

## Root/main creation: explicit gap

**Root creation during EnemyManager initialization, and its before/after
relationship to callback registration, are not established by this work.**
The allocation wrapper has real call `0x415882 -> 0x415310` and checks the
initializer result. Decoded initializer `0x415310..0x415475` registers the tick
callback at `0x4153c0` and returns at `0x415472`; it contains **no direct call
to `0x414ff0`**. This local negative observation is not proof about all its
callees: resource initialization via the indirect call at `0x415371`, external
setup, and the stage-control entry graph are not resolved here.

Cheap positive evidence exists in **separate callers**:

| Literal-main request | Original preparation |
|---|---|
| `0x426ae9 -> 0x414ff0` | `0x426ae4` pushes original ASCII `main\0` at `0x4a28b8`; `0x426ad7` calls real memset for the `0x54`-byte parameter structure |
| `0x426c6a -> 0x414ff0` | `0x426c65` pushes the same name; structure is cleared by `0x426c58 -> 0x47bb30` |

The added `literal-main-request-allocation-boundary` fixture starts at
`0x426acf` with explicit synthetic `EDI = 0`, executes original memset over a
dirty synthetic stack structure, pushes the **actual original main string**,
and enters actual spawn function `0x414ff0`. Its stack at the final stop
contains the original name pointer and the now-zero `0x54`-byte structure.
It stops before allocator call `0x415004 -> 0x47296c`: no root is allocated or
updated in this fixture, and the earlier setup/callback graph is not executed.

Thus a literal-main request on these paths is established; creation **inside
the manager initializer**, successful root construction, the selected first-stage
path, its position relative to registration/activation, and browser tick-zero
placement remain explicit gaps. Conditional on any of these real requests
actually completing the ordinary spawn path, the constructor/update-before-link
evidence below also applies; it does not establish that conditional occurred
during initialization. StageWorld's special root handling and entry snapshots
are deliberately not integrated or declared correct by this probe. JSON records
`insideManagerInitializerProven: false`, `initializerToRootOrderProven: false`,
and `browserFrameCadenceProven: false`.

## Constructor and node insertion order

The relevant functions and callsites must not be conflated:

1. Spawn entry **function** `0x414ff0` requests `0x5178` bytes via real call
   `0x415004 -> 0x47296c`. This allocator is unsupported and never executed.
2. On the allocation-success path, `0x415016 -> 0x415a70` invokes the lower
   constructor. Original call `0x415b5f -> 0x47bb30` clears the embedded
   `Enemy+0x11c8` state, size `0x3fb0`, including flags at full-object
   `+0x50ac`. A partial constructor fixture with dirty input executes this
   real clearing path. Full lower construction, including lookup
   `0x415d8e -> 0x470610`, is only statically catalogued, not executed.
3. Real stores `0x415c0e/0x415c14/0x415c1a` initialize the embedded list node:
   owner at `Enemy+0x145c`, next at `+0x1460`, previous at `+0x1464`.
   A separate dirty-node fixture proves these writes rather than seeding the
   expected final values.
4. After spawn parameters have been copied, **callsite** `0x415180` calls the
   update **function** `0x416160`, with `ECX = Enemy+0x11c8`. It is not the
   constructor's entry address. At `0x415185`, `EAX` is immediately overwritten
   from `[Enemy+0x11c0]`; the initial update result is not tested there.
5. Only subsequently, at `0x4151fc..0x415245`, does the spawn entry link its
   own node, increment count, store the new tail, and return the Enemy pointer
   with `ret 8`.

The insertion suffix computes `new = Enemy+0x145c` at `0x4151ff`:

- Empty list: set head to `new` at `0x41520d`.
- Nonempty list: load old tail at `0x415215`; if its next is nonnull, copy that
  successor to `new.next` and update successor.previous at
  `0x415222..0x415228`; set old-tail.next to `new` at `0x41522b` and
  `new.previous` to old tail at `0x41522e`.
- In either case, increment count at `0x415231` and set tail to `new` at
  `0x415238`. For a consistent terminal tail and a constructor-initialized
  node, this is a tail append with `new.next == 0`.

Empty-list and terminal-tail suffix fixtures run the real epilogue/return from
an explicit synthetic saved frame. They do not pretend the allocator or the
preceding initial update succeeded in that fixture.

The `prepared-constructor-to-ecl` fixture begins at `0x415021` with explicitly
preallocated/initialized storage and prepared parameters. It executes the real
`0x415180` call, real simple-motion calls, and reaches **before** ECL call
`0x4164e1`; manager head and count are still zero and guard `0x80000` is set.
No ECL runner is executed. This corroborates initial-update-before-link on this
prepared path, not an end-to-end successful native spawn.

**Conditional nesting consequence, static only:** if an initial ECL execution
recursively creates another enemy and both spawn calls return normally, that
inner enemy can finish insertion before the outer enemy inserts itself. Spawn
request order must not be assumed to equal link order. No recursive actual
script trace is claimed here.

## Manager visit and per-enemy guard

The manager's list loop is `0x415910..0x415951`:

| Address | Original operation | Scheduling consequence |
|---:|---|---|
| `0x415910` | `EAX = [ESI]` | `ESI` points to a node; owner is its first word |
| `0x415912` | Test owner flags `0x04000000` | Removal path does not call the update |
| `0x41591c` | `EBX = [ESI+4]` | Cache successor **before** update/destruction |
| `0x415921` | `ECX = owner+0x11c8` | Pass embedded state, not full Enemy |
| `0x415927` | Call `0x416160` | Direct real per-enemy update call |
| `0x41592c..0x41592e` | Test returned `EAX` | Nonzero goes to removal path |
| `0x415930..0x415932` | Reload owner; AND flags with `0xfff7ffff` | Clear guard only after zero result |
| `0x41593e..0x41594b` | Virtual call through owner vtable `+0x14`, arg 1 | Unsupported destructor boundary, no invented deletion |
| `0x41594d..0x415951` | `ESI = EBX`; loop if nonnull | Advance using cached next, not a fresh load from old node |

The real update reads flags at state `+0x3ee4` / full Enemy `+0x50ac`.
`0x416183` tests `0x80000`; `0x416188` branches to `0x41734d` if set. This
return path sets `EAX = 0` at `0x417356` and executes the original security
cookie check `0x417358 -> 0x4728fd`. Its successful original compare/return
bytes are included, not replaced by a stub. The manager then really clears
the guard. Two-node guarded traversal returns through both cookie checks,
preserves unrelated flag `0x20`, and executes neither motion nor ECL.

If guard is clear, `0x41618e/0x416193` set it before the remaining update.
On the supported simple-motion path:

1. Polar interpolation call `0x416200 -> 0x41eb00`, when duration is nonzero.
2. Absolute/relative simple velocity calls `0x416330/0x4163a5 -> 0x46ae00`.
3. Position integrations `0x4163b6/0x4163eb -> 0x46ae80`.
4. Combination `0x4163f0 -> 0x416030`.
5. ANM/bounds checks and then ECL call `0x4164e1 -> 0x46fe20`;
   further direct edge `0x46fe52 -> 0x46e100` is static only in this probe.

The eligible-manager fixture executes this path through real interpolation,
trigonometry, floor and combination bytes, stopping before ECL. Explicit
duration 2, start speed 0, target speed 2, angle 0, time scale 1 produces
elapsed 1, speed 1, and absolute/combined X `-128 -> -127`. It exercises the
real first-offscreen Y branch `0x416530 -> 0x4164c7` with previously-unseen flag
`0x20000` clear; it does not bypass bounds with a fabricated call return.
Other motion modes, animation objects, bounds/removal results and ECL returns
remain unsupported/unexecuted. The existing `EnemyRules` domain restrictions
still apply before any portable helper is used.

## What an appended node can do this pass

These are **conditional local consequences of the decoded list/guard code**,
corroborated by separate prefix, insertion and continuation fixtures. They
are not a continuous emulated spawn-during-ECL or full native frame trace.
Assume consistent links, normal completed construction, no interfering
deletion/reentry/guard mutation, and an active manager callback:

| Append timing | Same manager pass | First later manager visit |
|---|---|---|
| Before the pass reads head | Node can be visited; constructor-set guard suppresses motion/ECL and manager clears it | With guard clear, the next eligible visit can run motion before ECL |
| While updating a node with an existing cached successor, before the old tail is visited | Traversal can reach the new tail via a later fresh `next` read; guarded visit clears guard only | Next eligible visit can run motion before ECL |
| While updating the current terminal tail, after it cached null | Cached null ends this pass even though old-tail.next now points at the new node | New node is still guarded: this visit clears it without motion/ECL; only another eligible visit can advance it |
| After this manager pass has finished | Not visited by that completed pass | Same guarded first-visit behavior |

`cache-next-tail/nontail` execute real manager prefixes through the cached-next
load, stopping before update. `cached-null-after-append` and
`cached-next-after-append` deliberately start at `0x41594d` with **synthetic
cached EBX** and already-linked graphs. The first ignores a now-nonnull next
link and leaves the new guard set; the second visits the old successor and
the appended guarded node, clearing guards without motion. No fake update
return, allocator, interrupted call or complete birth event is inserted.

This is why a blanket per-frame guard reset or an array snapshot alone would
not reproduce the recovered local code. “Later visit” here is not an asserted
native frame number: callback suppression, repeat calls and the unexecuted
manager suffix must still be accounted for by any future world scheduler.

## Probe, reproduction and validation

`tools/research/enemy_schedule_probe.py` reuses the unchanged safe emulator from
`enemy_probe.py`. Code is original and RX; data accesses are confined to
explicit regions; stack is bounded; all unknown direct calls and **every**
indirect call stop before execution. Maximum per fixture is 16384 instructions
(hard emulator ceiling 200000); the largest actual fixture executes 8472.
Synthetic x87 CW is `0x037f`, MXCSR `0x1f80`, clock scale 1, SSE2 floor enabled,
CRT accelerated memset disabled, and scheduler threading flag zero. There are
no OS/API/allocation/trig/floor/security-cookie call-return stubs.

Private `local/research/enemy-scheduling.json` has **22 cases**: 10 explicit
boundaries, 7 returned partial functions/gated callbacks and 5 unsupported-call
stops (initial allocator, literal-main allocator, later registration allocator,
destructor, scheduler callback). No accepted fault, outside-code/data access or
exhausted budget.
Every case records actual trace addresses, stop reason, registers, initial
stack, bounded data before/after and named scalar observations. Code catalogs
are static evidence even when a separate fixture executes some of those bytes.
Distribution metadata: pefile 2024.8.26, Capstone 5.0.9, Unicorn 2.1.4.

PowerShell from `outputs/th128-web`:

```powershell
$env:PYTHONUTF8='1'
$env:PYTHONDONTWRITEBYTECODE='1'
$env:PYTHONPATH=(Resolve-Path ../../work/reverse-libs).Path
$env:TH128_ENEMY_SCHEDULE_EXE='E:\gee\妖精大战争\th128.exe'
python -B tools/research/enemy_schedule_probe.py --exe $env:TH128_ENEMY_SCHEDULE_EXE --libs ../../work/reverse-libs
python -B -m unittest discover -s tests -p test_enemy_schedule_probe.py -v
```

All **22 targeted tests pass with the pinned EXE enabled**, not as skipped
original-byte tests. Initial contract regressions failed because the module
did not exist; implementation followed. Further red/green checks pinned
constructor continuation evidence and dirty-node writes. Original-byte tests
also exposed and corrected an explicit stack-frame omission and an omitted
real pre-ECL bounds branch; neither was “fixed” by inventing an external return.

Independent review found no Critical/Important issue in these bounded claims.
It identified an inherited helper-catalog limitation: `normalize` ends at
`0x46a78e`, inside the `ret 4` beginning at `0x46a78d`. No scheduling fixture
reaches that return. The report now records decoded/undecoded byte counts and
marks this range `partial-static-decoding` with one undecoded byte; it must
not be used as a complete-function proof. A metadata regression first failed
before this disclosure was added. Original helper ranges were not silently
expanded and unknown execution remains fail-stop.
The bounded root follow-up also started with a failing regression before its
static catalogs and literal-main request fixture were added.

Remaining gaps are deliberate: initialization-to-root reachability/order,
browser-frame cadence, real allocator/entire construction and lookup,
actual ECL-driven nested birth, indirect scheduler/destructor execution,
post-`0x415954` manager effects, all callback ordering/reentry, deletion and
other update branches, and complete native frame behavior. No original EXE or
installer was launched; no E: file was written; no commit/branch/agent or new
original-source archive was created. This evidence is sufficient to constrain
local helper invocation order, **not** to declare StageWorld scheduling or
emission complete.
