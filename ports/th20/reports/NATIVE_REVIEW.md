# Independent review and native CPU verification

The recovered RNG and Timer implementations match the inspected machine instructions for their stated isolated-function scope. The supplementary tests found an emulator limitation and verified the affected inputs on the local processor; the C++ core was not changed to imitate that emulator behavior.

## Instruction review

The review inspected the original instructions at `0x422CB0`, `0x423F80`, `0x4530F0`, `0x4533B0`, and the clock helpers at `0x4292E0` and `0x452F50`. The integer RNG multiplies by 48271 and performs one Mersenne fold/subtraction. The timer initializes conditionally, preserves the documented flag bits, uses exclusive float32 comparisons against the actual `0.99f` and `1.01f` constants, and truncates with CVTTSS2SI. The near-one and null clock branches of `timer_tick` increment the integer separately, as the C++ implementation does. These observations come from the supplied executable, not a guessed gameplay model.

The existing primary differential suite covers 39,216 comparisons across seven isolated routines. Its ordinary and boundary-value coverage is useful, but canonical Python `nan` samples do not cover signaling NaNs or distinct payloads, and its host DLL environment was initially assumed to be the default. The new edge suite explicitly handles both issues.

## Edge suite and the observed emulator difference

`tests/validate_native_edges.py` uses uint32 values for the delta adapter and raw rate storage, avoiding Python float conversion. It sets host MXCSR to `0x1F80` before each C++ call and sets the same value in Unicorn. It restores the host value afterward. It compares the full 16-byte Timer object and the `timer_tick` return bits; it does not compare floating-point status flags or exceptions.

The 9,840 comparisons include signed zero, subnormals, extreme finite values, infinities, signaling NaNs, several NaN payloads, null clocks, and 3,000 random raw-bit triples. There are **330 Unicorn/C++ disagreements**, all limited to NaN payload/sign bits. `reports/native_edges_validation.json` intentionally retains `status: failed` and every disputed input instead of suppressing those observations.

## Independent hardware oracle

`tests/native_cpu_compare.cpp` runs in a dedicated 32-bit test executable. It verifies the exact source SHA-256, maps the original PE sections into its own process, and calls only the already inspected isolated Timer/RNG addresses using explicit `__thiscall` or `__cdecl` function pointers. It never calls the original entry point and never resolves or loads the original imports. The source file stays read-only.

The test executable is linked at `0x10000000`. When Windows prevents reserving the original preferred base `0x400000`, the loader applies the verified image's HIGHLOW relocation table and translates the selected addresses to the allocated base. The recorded run applied 29,196 relocations. This changes address fixups exactly as a PE loader would; no function instructions are replaced with behavioral stubs.

The C++ comparison runs in the same 32-bit process by including `native_core.hpp`. Before each side it resets MXCSR to `0x1F80` and x87 control to `0x037F`; saved control settings are restored on exit. The report records CPU vendor, compiler version, source hash, compiled header hash, mapping address, relocation count, counts and Timer input/output records.

On the local Intel processor, **29,856 direct hardware comparisons passed**: 9,840 Timer and 20,016 RNG comparisons. This comprises the full edge grid and independent random samples. `reports/native_cpu_validation.json` contains the results.

`tests/compare_native_oracles.py` joins **all 330** previously disputed inputs to the hardware records. For every one, original hardware machine code = C++ x86 = C++ x64, while Unicorn yields different NaN payload/sign bits. `reports/native_nan_oracle_comparison.json` therefore attributes these observed mismatches to emulator NaN propagation. This attribution covers those exact disputed inputs; it does not establish equality for every possible IEEE754 input or floating-point environment.

For example, with initial float bits `0x00000000`, rate `0x7F800001`, and delta `0x7FA12345`, both native original code and C++ produce `0x7FC00001`; Unicorn produces `0x7FE12345`.

## Reproduction

Build the standard project first, then:

```powershell
python D:\AIWorkspace\Touhou20Reconstruction\tests\validate_native_edges.py
# Exit 1 is expected for this pinned emulator's 330 recorded NaN differences.

cmake -S D:\AIWorkspace\Touhou20Reconstruction\tests\native_cpu -B D:\AIWorkspace\Touhou20Reconstruction\build_cpu -G "Visual Studio 16 2019" -A Win32 -DCMAKE_GENERATOR_INSTANCE=D:/VS2019/IDE
cmake --build D:\AIWorkspace\Touhou20Reconstruction\build_cpu --config Release --parallel 4
& D:\AIWorkspace\Touhou20Reconstruction\tests\validate_native_cpu.ps1
python D:\AIWorkspace\Touhou20Reconstruction\tests\compare_native_oracles.py
```

The PowerShell launcher independently verifies SHA-256 before starting the hardware test, and the executable verifies it again before mapping any code. The hardware report records the header's build-time SHA-256; CMake reconfigures when that header changes.

## Remaining scope limits

Neither the primary nor supplementary suites test complete game execution, ECL VM semantics, long gameplay state sequences, threading/locking in the higher RNG wrappers, clock ownership, nondefault floating-point modes, or floating-point exception/status equivalence. These tests provide concrete evidence for the isolated reconstructed components only. The main and supplementary validators now all require the analyzed executable's exact SHA-256, even when invoked independently.
