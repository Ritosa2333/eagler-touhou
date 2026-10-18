# Runtime allocation, locks, log and entry adapters

This module contains independent C++ implementations for runtime services used
by the recovered entry point. Its production libraries do not load or execute
the original game EXE. The separate CPU oracle uses the original only for
isolated tests; missing graphics behavior remains a required unresolved call.

## Integration

```cpp
#include "runtime_core.hpp"
namespace rt = th20::source::runtime;
auto& locks = rt::shared_locks();
std::lock_guard<std::recursive_mutex> clock_lock(locks.slot(5));
```

All callers share the same registry: scheduler slot 0, allocation slot 1,
file-controller slot 2, log slot 3, clock slot 5. The registry has 22 slots;
ordinary `slot()` locking is unconditional. `enter_tracked`/`leave_tracked`
perform the original enabled-gated lock plus byte-depth operation.

`LockRegistry::scheduler_environment()` returns a scheduler environment bound
to the same slot 0/1 mutexes and the same enabled/depth storage. The scheduler's
standalone default environment remains available for isolated tests.

Targets:

- `th20_runtime_core`: lock registry, PMR resource/controller, allocation, log,
  scheduler factory/lifetime helpers.
- `th20_runtime_entry_adapter`: implements the recovered entry dependency names
  and defines only proven runtime globals: null allocation/function pointers,
  initialized log, shared registry reference and bound scheduler environment.
  Link this target with the real entry/graphics implementations when available.
- `th20_runtime_core_tests`: independent execution tests, no original EXE.
- `th20_runtime_cpu_compare`: explicitly separate original-CPU oracle.

The entry header now aliases its AllocationController/LogBuffer/LockRegistry
types to these real definitions. The old `RuntimeListenerStorage` name remains
at the adapter boundary, but it is aligned raw storage for a four-byte PMR
memory resource, explicitly constructed and destroyed by the entry sequence.

## Corrected types and evidence

All original VAs below refer to the verified 1.00c EXE. Function evidence is in
`analysis/ghidra/pseudocode/`, checked against `analysis/binary/disassembly.asm`.

| Original VA | Actual behavior / source implementation |
|---|---|
| 0x452d80, 0x452e00, 0x452ee0, 0x452e40 | Construct 22 recursive mutexes at 0x30-byte stride; zero 22 depth bytes at +0x420 and enabled byte at +0x436. Mutex flags are 0x102, owner -1, count 0. |
| 0x41ccc0, 0x41ca30 | Set/clear the registry enabled byte. |
| 0x412550, 0x412750 | Conditional tracked acquire/release; lock before increment, decrement before unlock. |
| 0x41f5b0, 0x419320, 0x41f5f0 | Eight-byte allocation-controller construction/destruction: first uint32 zero, embedded four-byte memory-resource object. |
| 0x418db0, 0x418e90, 0x419390 | Memory-resource construction/destruction. Original vtable data at 0x56c920 contains 0x419390, 0x41f6d0, 0x41f760, 0x41c9f0. |
| 0x41f6d0, 0x41f760 | Lock slot 1 around aligned operator new/delete. Deallocation ignores byte count and forwards alignment. |
| 0x41c9f0 | Actual `MOV AL,1; RET 4`: resource `is_equal` always returns true. This constant is recovered behavior, not a placeholder. |
| 0x41e770, 0x541562, 0x541550 | Get/set default PMR memory resource, with standard new/delete resource fallback. |
| 0x41f610, 0x41f670 | Slot-1 protected malloc/free. Original allocation is thiscall with RET 8: size plus an unused second stack argument. Release is thiscall RET 4. New source API omits unused arguments. |
| 0x40d8c0, 0x40d840 | Slot-1 protected new[]/free wrappers; diagnostic allocation label is unused in the observed release code. |
| 0x452dd0, 0x447cc0, 0x446c10, 0x44b2b0 | Log construction: `std::pmr::string` (28 bytes) plus error byte at +0x1c. |
| 0x454150, 0x454230, 0x452d60 | Locked formatting into a 1024-byte **char** buffer, append, optionally set error byte. Ghidra's wide-format names are incorrect. Actual format literals are CP932 bytes, and 0x54873b/0x5488af write byte terminators. |
| 0x4530a0, 0x454310, 0x454340 | Append bytes to existing log. Ghidra's `assign` label is incorrect: length increases by the incoming byte count. |
| 0x419be0, 0x41c0e0, 0x41b8a0 | Clear text length and set first byte zero; preserve capacity and error flag. |
| 0x453220, 0x454450, 0x44baf0 | If nonempty append separator; if error flag set, convert CP932 to UTF-16 and show an actual error MessageBoxW. |
| 0x4187a0, 0x4186c0, 0x418e40 | Allocate/init 56-byte scheduler; teardown invokes renderer flush, recovered scheduler shutdown/removal, then slot-1 protected free. |

Compile-time assertions check MemoryResource=4, AllocationController=8,
pmr::string=28 and Log=32 on the original x86 target. The test additionally
checks LockRegistry=0x438 and mutex=0x30.

## Validation

```powershell
cmake -S source_reconstruction/runtime_core -B source_reconstruction/runtime_core/build -G 'Visual Studio 16 2019' -A Win32
cmake --build source_reconstruction/runtime_core/build --config Release
ctest --test-dir source_reconstruction/runtime_core/build -C Release --output-on-failure
& '.\source_reconstruction\runtime_core\build\Release\th20_runtime_cpu_compare.exe' `
  'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe' `
  '.\source_reconstruction\runtime_core\cpu_validation.json'
```

The recorded hardware run passed **2,541 comparisons**: mutex initialized
fields and real recursive lock/unlock across all 22 slots; log constructors,
SSO and heap growth/append, clear while preserving error flag; successful raw
and aligned allocation/free; resource equality and controller destruction with
delete flag clear. Original instructions are unchanged. The oracle resolves
only real Win32 thread/lock/heap imports and supplies valid heap/PMR state.
The report records the original and recovered source hashes.

Worker tests add 1,088 comparisons. The unchanged original `40b780` constructor
and `40b980` destructor are compared over all 16 bytes for every fill byte
0..255, including the three untouched tail-padding bytes. The user-provided
source constructor preserves that padding even with value initialization.
Idle `40bcf0` join and `4ba8b0` detach are compared byte-for-byte. Another 128
running source-created, ABI-compatible jthreads are closed and joined or
detached by the unchanged original methods versus source methods. Join must
wait for actual thread completion; detach and detach-then-join must return
while the worker body remains gated, and the detached body must later observe
the close flag. Each pair has separate live OS threads. No game entry or fake
thread endpoint is invoked. Join error paths, self-join and competing callers
are outside this test domain. The report freezes both Worker sources and its
test case hashes at build time.

Independent source tests verify real cross-thread exclusion, aligned allocation
at 4..4096-byte alignments, PMR default installation, actual formatted narrow
log text, oversized raw-allocation rejection, unified scheduler lock ownership,
and renderer-flush-before-shutdown ordering. They do not display a message box.
After scheduler binding changes, its **13,768 checks** were rerun successfully.

## Remaining domains

- Original variadic formatting was not CPU-compared; valid narrow formatting
  is implemented with the current MSVC secure CRT and exercised by source tests.
  Locale, invalid-parameter handlers and extreme format behavior are not proved
  identical across CRT versions.
- Allocation failure/new-handler callbacks, invalid alignments and global CRT
  lifetime interactions are not CPU-compared. Successful byte/alignment and
  lifetime behavior is covered. The original resource destructor does not
  unregister itself; the source entry follows that observed sequence.
- Error-log MessageBox display is implemented but deliberately not opened by
  automated tests. Conversion failure now raises an exception rather than
  displaying an uninitialized buffer; invalid/oversized log behavior is outside
  the equivalence claim.
- `free_function_controller` still calls the real unresolved renderer operation
  `fn_004d9e30(graphics_state)`. It is not replaced by a no-op. The generic
  `destroy_scheduler` requires a renderer callback and rejects an absent one.
- ThreadRegistry, SpriteController, graphics/device state and the full game
  runtime are not provided here. The adapter defines no guessed substitutes.
