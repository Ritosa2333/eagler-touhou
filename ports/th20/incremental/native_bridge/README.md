# TH20 x86 C++ replacement DLL

This DLL supplies seven recovered C++ functions to the incrementally rebuilt
game executable. It does not launch the original executable. The remaining
game functions are outside this DLL's scope. The implementation includes the
existing `native_recovered/native_core.hpp` unchanged.

Build from the project root with MSVC 2019 and the Windows SDK:

```powershell
cmake -S incremental/native_bridge -B incremental/native_bridge/build -G 'Visual Studio 16 2019' -A Win32
cmake --build incremental/native_bridge/build --config Release
```

Output: `incremental/native_bridge/build/Release/th20_native_bridge.dll`.
The target uses `/MT`, `/fp:strict`, `/arch:SSE2`, DEP and ASLR. Its import table
contains only `KERNEL32.dll`; it requires no separately installed VC runtime.
The bridge does not change the floating-point control environment or create
threads. `DllMain` obtains the actual main executable base with
`GetModuleHandleW(nullptr)`.

## Stable PE exports and original ABI

| Original VA | Export | Original ABI and return |
|---|---|---|
| `0x422cb0` | `th20_lcg_step` | `__cdecl(uint32_t) -> uint32_t` |
| `0x4235f0` | `th20_lcg_next` | `__thiscall(uint32_t* self) -> uint32_t` |
| `0x423f50` | `th20_timer_reset` | `__thiscall(Timer* self) -> Timer* self` |
| `0x423fe0` | `th20_timer_mode` | `__thiscall(Timer* self, uint32_t) -> Timer* self` |
| `0x423f80` | `th20_timer_set` | `__thiscall(Timer* self, int32_t) -> Timer* self` |
| `0x4530f0` | `th20_timer_add` | `__thiscall(Timer* self, float) -> Timer* self` |
| `0x4533b0` | `th20_timer_tick` | `__thiscall(Timer* self) -> int32_t` |

The six member functions use `__fastcall(self, unused_edx, ...)` in C++.
This preserves ECX as `self`, ignores EDX, and pops the original number of stack
arguments. The `.def` file provides the undecorated names above. All four timer
mutators preserve the original `EAX=self` return in addition to object bytes.
The clock rate is read from the pointer stored at `main_module_base+0x1aefe0`.

`g_th20_bridge_stats` is a writable data export, with the following layout:

| Byte offset | uint32 field | Value |
|---|---|---|
| 0 | magic | `0x42303254` (`T20B`) |
| 4 | version | 1 |
| 8 | size | 52 |
| 12 | source_status | 1 = compiled recovered C++ implementation |
| 16 | main_module_base | actual main executable base |
| 20 | clock_pointer_rva | `0x1aefe0` |
| 24 | counts[7] | calls in the export order shown in the table above |

Counters use inline interlocked increments and wrap modulo 2^32. They are
diagnostics, not game state. An external verifier can find the data export by
parsing the loaded DLL's PE export directory and read it with
`ReadProcessMemory`. A hardware oracle that maps an EXE as test data may
temporarily set `main_module_base` to that mapping; production initialization
always uses the actual process EXE.

## Actual DLL ABI and CPU validation

```powershell
& '.\incremental\native_bridge\build\Release\th20_bridge_abi_check.exe' `
  'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe' `
  '.\incremental\native_bridge\build\Release\th20_native_bridge.dll' `
  '.\incremental\native_bridge\bridge_abi_validation.json'
```

The harness hash-checks and maps the original PE without running its entry
point. It dynamically loads the shipped DLL and invokes both versions using
the original calling conventions. Its reserved BSS provides the production
clock pointer RVA; no test-only build or mock replacement is involved.

The recorded local run passed **100,116 comparisons** with zero differences:

- RNG return values and full state; complete 16-byte timer objects.
- Mutator `EAX=self` and tick return values.
- ESP stack balance and EBX/ESI/EDI/EBP preservation.
- MXCSR/x87 control bits unchanged from the tested default environment.
- All seven diagnostic counters exactly match exported function call counts.

Cases include timer initialization flags, signed integer boundaries, null and
fractional clock rates, exact near-one threshold neighbors, arbitrary float
bit patterns, infinities, signaling/quiet NaNs and their payloads. Floating
status flags are not compared. The report records both input and DLL hashes.
These results verify the seven replacements on this CPU; they do not establish
whole-game playback or frame equivalence.
