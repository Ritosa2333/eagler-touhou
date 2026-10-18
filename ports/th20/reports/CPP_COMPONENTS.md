# Independently implemented C++ binary components

These components inspect the actual supplied executable and extracted ECL data. They are not a playable recreation or a native implementation of the ECL virtual machine.

## Build

Tested with Visual Studio 2019, MSVC 19.29, Windows SDK 10.0.19041, x64:

```powershell
cmake -S D:\AIWorkspace\Touhou20Reconstruction -B D:\AIWorkspace\Touhou20Reconstruction\build_component -G "Visual Studio 16 2019" -A x64 -DCMAKE_GENERATOR_INSTANCE=D:/VS2019/IDE
cmake --build D:\AIWorkspace\Touhou20Reconstruction\build_component --config Release --parallel 4
ctest --test-dir D:\AIWorkspace\Touhou20Reconstruction\build_component -C Release --output-on-failure
```

The `th20_binary` static library has a public C++17 API in `include/th20/binary.hpp`. It uses standard C++ only and does not link thtk. `th20_inspect` is the command line interface; Windows uses `wmain` for Unicode filesystem paths.

## Commands

```text
th20_inspect pe INPUT.exe [OUTPUT.json]
th20_inspect ecl INPUT.ecl [OUTPUT.json]
th20_inspect ecl-summary INPUT.ecl [OUTPUT.json]
th20_inspect ecl-scan DIRECTORY [OUTPUT.json]
th20_inspect ecl-roundtrip INPUT.ecl OUTPUT.ecl
```

Input files are opened read-only; writing over the input through the same path or a hard/symbolic link is rejected. The archive must be extracted before using ECL commands. A `SCPT` container does not encode its game version, so the tool explicitly identifies the TH10+ container family and does not assign unverified TH20 opcode semantics. Unknown opcodes remain available, with all parameter bytes intact. Unsupported signatures, header layouts, invalid ranges, zero-length instructions and out-of-range instruction bodies are rejected with a file offset. The scanner returns status 2 if any file is invalid; other inspection errors return 1.

## Recovered structures

ECL parsing reads the SCPT header, the aligned ANIM and ECLI string lists, the subroutine offset/name tables, ECLH headers and all instruction records. Each JSON instruction has its file offset, hexadecimal address, subroutine-relative offset, unsigned time bits, signed time view, opcode, size, parameter mask, rank mask, parameter count, stack-reference field and exact hexadecimal parameter payload. No integer/float interpretation of arbitrary operands is invented. Name strings escape each original byte as U+0000 through U+00FF when necessary, rather than guessing Shift-JIS or another encoding.

The layout was checked against [thtk thecl10.c at commit 892114a0fcaa0bbdaaecf3cb4ad56f758683fb40](https://github.com/thpatch/thtk/blob/892114a0fcaa0bbdaaecf3cb4ad56f758683fb40/thecl/thecl10.c), including both reader and writer: the writer confirms `include_length` excludes `include_offset`, despite the imprecise field comment. The implementation here is separately written C++, with explicit little-endian reads and bounds checks instead of casting input to packed structs.

PE parsing supports PE32 and PE32+, COFF/optional headers, sections, file-backed RVA mapping, named imports and ordinal imports. Virtual-only section bytes are not treated as on-disk bytes. This component does not disassemble native x86 machine code, inspect delay-load imports or recover high-level game functions.

## Validation on the supplied game

* All 21 extracted ECL files parse: **730 subroutines and 23,760 instructions**.
* Structural serialization re-encodes every instruction header and parameter payload; every resulting file is byte-identical to its input. Unmodelled container and padding bytes remain preserved. This is not evidence that gameplay has been reproduced, and it is not a general ECL source compiler.
* `reports/ecl_binary_scan.json` records each file and the opcode histogram; `reports/ecl_json/*.json` contains the full per-instruction exports.
* Independent thtk raw decompilation agrees on the ordered subroutine names and opcode sequences for every file. Re-run `tests/check_export_consistency.ps1`; results are in `reports/cpp_thtk_crosscheck.json`.
* The original executable parses as PE32/i386 with image base `0x400000`, entry VA `0x5435E0`, 6 sections and 211 imports. These numbers independently match the pefile-based analysis in `analysis/binary/pe_summary.json`. C++ results are in `reports/pe_cpp_inspection.json`.
* CTest passes valid and malformed PE32/PE32+ fixtures, named and ordinal imports, RVA boundary checks, ECL truncation and corrupted-count tests, parameter/header edits, and all 928 single-bit changes to a compact ECL fixture. Accepted changes must round-trip; invalid ones must throw a bounded format error.

The zero-byte `st01.ecl` in the original game directory is not the stage script. The actual stage data inspected here is `assets/raw/st01.ecl`, extracted from `th20.dat`.

## Additional native review

`reports/NATIVE_REVIEW.md` documents the separate review of the recovered native RNG/Timer components and its reproducible tests. The hardware oracle passes 29,856 comparisons. The extended Unicorn edge test records 330 NaN payload differences; all 330 are independently attributed to the emulator by comparing exact disputed inputs against the actual original machine instructions running on the local processor. The core C++ was not altered to mimic the emulator's NaN selection.
