# core/func.cpp recovered scheduling module

This directory contains independently compilable C++ source for the TH20
function-chain scheduler. `scheduler.cpp` calls no original executable address
and contains no original machine-code bytes. This is an engine module, not a
complete game. The original EXE is used only by the separate hardware test.

## Build and validation

```powershell
cmake -S source_reconstruction/core_scheduler -B source_reconstruction/core_scheduler/build -G 'Visual Studio 16 2019' -A Win32
cmake --build source_reconstruction/core_scheduler/build --config Release
& '.\source_reconstruction\core_scheduler\build\Release\th20_scheduler_cpu_compare.exe' `
  'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe' `
  '.\source_reconstruction\core_scheduler\cpu_validation.json'
```

The static library is `build/Release/th20_core_scheduler.lib`. Set
`-DTH20_BUILD_ORACLE=OFF` to build only source code without any PE-reading test
target. The library uses C++17, the original x86 object widths, and the ordinary
MSVC static runtime.

The recorded run passes **13,768 checks**: **13,767 comparisons against original
CPU code**, plus one source-only heap ownership/removal execution. The hardware
test verifies the original EXE hash, maps its sections/relocations, and resolves
only `GetCurrentThreadId` for the original recursive mutex. It never enters the
original game entry point and does not patch/substitute original code. Complete
State and Node words are compared after normalizing corresponding object
pointers, together with callback order and dispatcher return values.

Coverage includes signed priority extremes and ties, before-insert callbacks,
all update/draw control codes, enabled/disabled/null callback combinations,
callback insertion, removal of self/next/other nodes during dispatch, shutdown
callbacks, arbitrary flag words, and eight setter/destructor-field operations.
The report records exact scheduler source hashes. Test-only helpers reuse the
existing hash-gated PE mapper; no such dependency exists in the static library.

## Recovered layouts and source evidence

The retained string at VA `0x56c728` identifies `core/func.cpp:317 funcChainInf`.
`0x4127b0` calls the allocator wrapper at `0x411910`, which allocates `0x2c`
bytes; the constructor `0x411a70` writes all 44 bytes. The manager allocates
`0x38` bytes at `0x4187a0`, with constructor `0x418a80`.

| Structure | Original offsets |
|---|---|
| Node, 44 bytes | priority +0; flags +4; main callback +8; before-insert callback +12; shutdown callback +16; embedded Link +20; userdata +40 |
| Link, 20 bytes | payload +0; next +4; previous +8; owner list +12; active iterator +16 |
| List, 24 bytes | Link sentinel +0; tail pointer +20 |
| Iterator, 8 bytes | current +0; cached next +4 |
| State, 56 bytes | current-link pointer +0; update list +4; draw list +28; shutdown flag +52 |

`scheduler.hpp` enforces these sizes and critical offsets at compile time.
Pointer and field evidence comes from the functions below in
`analysis/ghidra/pseudocode/`, checked against
`analysis/binary/disassembly.asm`. Compiler-generated exception scaffolding
and incorrect Ghidra library names are not treated as source semantics.

| Original VA(s) | Recovered implementation | Validation |
|---|---|---|
| 0x411970, 0x411a70 | Link/Node initialization | complete constructor object comparison |
| 0x418a10, 0x418a50, 0x418a80 | sentinel/List/State initialization | complete constructor object comparison |
| 0x4119b0, 0x411b00, 0x411c30 | Iterator begin/destruction/advance | priority insertion and in-callback deletion comparisons |
| 0x411d50, 0x411ce0, 0x411d20 | unlink, observer repair, owner-tail repair | 4,800 removals plus 50 in-callback mutation cases |
| 0x411e30, 0x411e70 | linked-list lookup | removal comparisons |
| 0x411ee0, 0x411f30, 0x411ea0 | insert after/before/append | 4,800 priority insert comparisons |
| 0x411f80, 0x412100 | ordered update/draw insertion | signed extreme/tie order, before-insert side effects |
| 0x412400 | scheduler removal | complete state/trace comparison; owned heap deletion source-only |
| 0x4124b0 | locked removal | same body; integration uses real C++ mutex, not exact original CRT layout |
| 0x412810, 0x412aa0 | update/draw dispatch | 400 default and 240 control-code cases each, mutation/shutdown cases |
| 0x412d30, 0x412d80, 0x4127f0 | owned/enabled/disabled bits | 300 arbitrary flag words each |
| 0x412d50, 0x412d10, 0x412dc0, 0x412da0, 0x411b80 | callback/userdata setters and callback clearing | 300 object comparisons each |
| 0x4127b0, 0x411910 | create owned Node | allocation wrapper composed from validated initialization; original allocator not compared |
| 0x4122c0, 0x412310, 0x412360, 0x4123b0 | register_callback variants | composed from create/setters/ordered insertion; global manager becomes explicit State argument |
| 0x4125b0 | shutdown_chains scheduling portion | dispatch shutdown path compared; leading renderer flush remains external |

## Behavior recovered beyond the pseudocode

New nodes with equal signed priority go **before** existing nodes. Iterators
register observers on both the current and cached-next links. Unlinking updates
those observers before clearing links; caching a next pointer alone would fail
when a callback removes the next node.

Dispatchers return a count/control value even though their Ghidra signatures
say `void`. Instructions at `0x412a57` and `0x412ccf` load that value into EAX.
The update jump table is at `0x412a74`, draw at `0x412cec`:

| Callback result | Update dispatch | Draw dispatch |
|---|---|---|
| 0 | remove node, then count it | same |
| 1 / unrecognized | count it and continue | same |
| 2 | repeat current enabled callback | same |
| 3 | stop and return 1 | same |
| 4 | stop and return 0 | same |
| 5 | stop and return -1 | same |
| 6 | restart from first node, resetting count | count and continue |
| 7 | invoke shutdown callback if present, then count | count and continue |
| 8 | stop and return 0 | count and continue |

Disabled nodes with a non-null main callback still contribute to the count.
Nodes with a null main callback do not. During update shutdown, enabled nodes
invoke the shutdown callback instead of the main callback. Draw's final pass
at `0x412c4c` clears **Link+0x10 iterator observers**, not Node+0x10 callbacks;
the reused field setter caused misleading inferred types in the pseudocode.

## Explicit integration dependencies and unverified domains

- Original global lock/debug structures at `0x5c0240` are replaced by an explicit
  shared `Environment` with real C++ recursive mutexes and matching callback
  unlock/relock boundaries. `runtime_core::LockRegistry::scheduler_environment`
  now binds that Environment to registry slot 0/1 and its enabled/depth bytes;
  the entry adapter uses this shared binding. Original cross-thread scheduling, CRT lock
  failure behavior and debug instrumentation have not been proved equivalent.
- Original allocation metadata, global allocator ownership, out-of-memory
  behavior and free tracking are not recovered. `create`/owned deletion use
  standard C++ allocation; successful Node state semantics are implemented.
- Original `0x4125b0` begins with renderer call `0x4d9e30`. `shutdown_chains`
  explicitly implements only the scheduler portion; its owner must perform the
  renderer operation when that renderer module is recovered. It is not silently
  replaced by an empty function.
- Nested simultaneous iterators, corrupted graphs, callback exceptions and
  callbacks violating node lifetime assumptions are not validated. The source
  keeps the original single-observer link design rather than promising stronger
  mutation guarantees than the evidence supports.
- This module contains no rendering, ECL execution, game entities, input loop,
  or complete-game executable. Compilation and local scheduler comparison do
  not establish whole-game 1:1 behavior.
