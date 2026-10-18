# ReplayInf source recovery

This directory contains C++ implementations of the original 0x360-byte ReplayInf, its 0x30/0x100/0x2a0-byte file/user/stage records, 36,000-frame recording chunks and eight playback cursors. The one production Replay global remains `startup::unrecovered::owner_005c60fc`; no copied executable code is used.

Implemented and compiled: constructor/destructor, factories, callback enable/disable, input reset/repeat derivation, recording/playback/fast-forward/draw callbacks, stage seed resets, session/Player state restoration, file load/save, compressed data and both USER trailers. File payloads use the already recovered shared-dictionary LZSS implementation, followed by the specimen's two encryption passes. User paths remain under the game's configured user-data replay directory; demo mode reads archive resources.

`records_validation.json`: 35,840 original-CPU comparisons passed for three record constructors, playback-cursor constructor, recording chunk construction/append and replay input reset/update. These checks compare all bytes, preserving padding and high flag bits, with only the tested objects' self-pointers normalized. `oracle/build/source_hashes.json` identifies the compiled source inputs.

`demo_validation.json` additionally records a source-only load of all four supplied archive demo replays: four stages and 27,411 input frames, with input and FPS stream offsets validated. The verifier never loads any executable image. This checks decoding and structure, not gameplay execution. `verify_demos.py` reproduces the report.

The Replay owner, save I/O and full frame integration have not yet completed their differential validation. In particular the legacy save trailer's character lookup really uses `character*9 + first_stone` across adjacent static string tables; the source preserves that lookup. Bounds checks reject corrupt files instead of allowing original out-of-bounds memory access. Originally uninitialized padding is not claimed byte-identical across fresh allocations. `extract_strings.py` reproduces the exact static string evidence.

The original specimen hash is `a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897`. Evidence is in `evidence/`. The original indirect virtual508f70 was absent from the decompiler function inventory and was recovered directly from disassembly. The test oracle alone loads selected original CPU functions; the production library never loads original machine code.
