# Progress and score storage

This is independent C++ for the original score manager at `0x005c6108`. The
production library contains no original instructions and never loads the game
executable. Startup now creates this manager; gameplay selects its actual
profiles and commits its actual save worker.

`SaveManager` is `0x1242d8` bytes: current and backup `Snapshot` objects of
`0x92140` bytes, one word and sixteen further words, then the shared 16-byte
`runtime::Worker` at `+0x1242c4`. Each snapshot has 18 selectable `Profile`
records and one fallback record, each `0x7ae8` bytes; `Metadata` is `0x1f8`
bytes at `+0x91f48`. `gameplay::Profile` aliases this single source type.
Unknown record fields remain bytes with their original offsets. Constructors
preserve uninitialized padding rather than pretending the entire records were
zeroed by those constructors. The original factory's separate zero-fill is
also retained.

| Original VA | Source implementation and scope |
| --- | --- |
| `50af20`, `50e4a0`, `50e500`, `50e6a0` | `construct_profile`, including the original constructor ranges and padding |
| `50e540`, `50e6e0` | `Snapshot::Snapshot`, `construct_metadata` |
| `50ef00`, `486e10` | `initialize_profile`; seven score tables and 113 spell defaults |
| `50f090`, `4beb60`, `463f20` | Metadata defaults, checksum and exact stream 1 RNG sequence |
| `50fc90`, `4bd460`, `464100` | Record checksum, profile lookup, selected profile branch |
| `4640a0`, `4640c0`, `4640e0`, `463fb0`, `463fe0`, `464530` | Actual shared session mode, metadata accessor, checksum verification and `quit_requested = -2` |
| `50eb70` | `parse_snapshot`; header, decrypt, LZSS, record checksum and copy |
| `50eeb0`, `50f5f0` | Snapshot record copy and backup/current merge order |
| `50f6b0` | `serialize_snapshot` and OS write half in `manager.cpp` |
| `539550`, `5399a0`, `5399d0`, `539060`, `539180`, `5394c0`, `539960`, `539a10` | Full binary-search-tree LZSS encoder, exact bit output and final dictionary |
| `4103c0` | `encrypt`; complement of the already recovered archive decryptor |
| `50f3b0`, `50fb60` | `SaveManager::load` and `save`, actual backup/main file ordering |
| `50e5d0`, `50e990`, `50adc0`, `50fce0`, `50fc10`, `50f660` | Factory, lifetime, unique global, load/save worker launch and join |
| `50ab10`, `50ad50`, `50ae50`, `50eb30` | Composition through real C++ thread and destruction operations; original CRT allocation ABI is not exported |

Compression clears the original process dictionary before encoding, builds the
8193-node tree and leaves its final dictionary intact. `resources` supplies the
same dictionary used by archive reads and score decoding, protected by lock 2.
Save operations use lock 20 and the existing shared worker lock 6. The file
writer retains the two original writes and CP932 path conversion. `50f3b0`
and `50fb60` have an unused stack argument in the original ABI; oracle calls
include it. `50eb70` and `50f6b0` pop their stack arguments despite inaccurate
automatic calling-convention labels.

The oracle verifies constructors, defaults, all RNG state bytes, checksums,
2,048 compression streams (including the whole final dictionary and tree),
4,096 encryption cases and 80 complete file serialize/parse cases. The manager
tests compare original load/save bodies with real source worker execution,
missing/invalid files, backup restoration, selected profiles and damaged
metadata checksums. Original file write boundaries are recorded in memory;
source threads perform real Win32 writes solely in `oracle/isolated_files`.
The isolated original mapping uses the current CRT locale query because its
entry point and original CRT TLS initialization are never run.

The original OS thread-launch ABI, arbitrary interleavings, full original
constructor/destructor thread timing and disk-full behavior are not claimed
equivalent. Record parsing bounds formerly undefined truncated/oversized
inputs instead of reproducing overreads or infinite loops. Allocation failure
is reported instead of following a null dereference. These differences do not
change the validated valid-record domain; they remain explicit limits of the
current evidence. This module is not a claim that the whole game is complete.

Build and test from the project root:

```powershell
D:\cmake\bin\cmake.exe -S source_reconstruction/progress_state/oracle -B source_reconstruction/progress_state/oracle/build -G "Visual Studio 16 2019" -A Win32
D:\cmake\bin\cmake.exe --build source_reconstruction/progress_state/oracle/build --config Release
source_reconstruction/progress_state/oracle/build/Release/th20_progress_cpu_compare.exe ORIGINAL.exe source_reconstruction/progress_state/cpu_validation.json
python source_reconstruction/progress_state/record_evidence.py
```

Additional original-code comparisons cover current/fallback profile access, unlock and stone grants, and hundredth-second playtime accumulation (49,152 checks). The fallback getter returns profile slot 18 in the current snapshot, not the backup snapshot.
