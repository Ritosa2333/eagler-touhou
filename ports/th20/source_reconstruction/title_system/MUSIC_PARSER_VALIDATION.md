# Music comment archive integration

`music_parser_validation.json` records 12,578 passing checks, with zero failures, for the verified v1.00c executable and its original `th20.dat`. The isolated test executable exits normally. Its report includes source hashes captured when CMake configured the build; `music_parser_oracle/run.ps1` rebuilds, runs, then checks these hashes against the current files.

The test compares all 285 archive catalog records and the 5,405 decoded bytes of `musiccmt.txt`. It then uses 4,096 deterministic randomized `TitleInf` states, varying preexisting buffers, cursor state and excluded selections. Each case compares the complete 0x5978-byte object, checks the native callback's zero return, and checks that the native callback actually performed another archive seek and read. The source has 18 music records for this document.

Both sides perform real archive decoding and parsing:

- Original: `51f890` → `410aa0` → `53a3c0`, using the original archive catalog, filename cipher, LZSS and string/stream functions. A test vtable supplies actual Win32 file open/read/seek operations. No original function instructions are patched; the game entry point is never invoked.
- Source: the production `read_music_comments` → `resources::read` → `Archive::read` → `parse_music_comments`. `music_reader.cpp` contains the same resource-wrapper body formerly in `music_environment.cpp`; separating that translation unit allows the complete resource path to be linked without the UI scene.

The original callback uses ECX for `TitleInf` and consumes one unused stack argument (`ret 4` at 51fd1a). The fixture now supplies this argument. The previous zero-argument call drifted the host stack and eventually overwrote its stream vtable; that failure was a test ABI error, not game behavior.

The native CRT runs with a separate, real Win32 heap. Its `GetProcessHeap` IAT binding returns this heap so the host C++ fixture cannot inject its temporary string allocations into native allocation reuse. Other resolved OS imports call Windows normally. Native CRT/FLS state is uninitialized before the mapped code and heap are released.

## Observed original undefined behavior

The original archive member has no trailing NUL. `410aa0` allocates exactly its 5,405-byte size. The parser constructs an unbounded C string through `46a830` → `411180`, whose scan continues until a NUL, including beyond the allocation. The native allocator at `5584c0` calls `HeapAlloc` without zeroing or adding a terminator.

With the fixture and native code sharing the process heap, a recorded run read residual bytes as an extra nineteenth music record on its second case. The original then differed at the record count and additional slot contents. This failure is preserved in `music_parser_shared_heap_probe.json` and `.log`; it is evidence of that observed allocation layout, not a guaranteed failure on every machine or run.

The passing report therefore verifies the real document under the recorded isolated-heap conditions. It does **not** claim equivalence for arbitrary bytes after the file allocation. Source parsing intentionally remains bounded by the decoded resource size. No production change reproduces an undefined heap overread.

This test does not cover asynchronous worker scheduling/lifetime, arbitrary malformed music documents, or rendered GPU frames. Those are separate from the verified archive-to-parser composition.

Run from PowerShell:

```powershell
./source_reconstruction/title_system/music_parser_oracle/run.ps1
```
