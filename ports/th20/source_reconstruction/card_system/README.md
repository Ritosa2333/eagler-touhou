# CardInf source recovery

The original spell-card owner is 0xc8 bytes, with its real Context+0x10 slot, disabled update priority40/draw priority12 callbacks, and the original three info animations. The C++ constructor, destructor, setup, per-frame update, drawing, spell start/end and post-frame time recording are recovered in this directory. No production file maps, embeds or calls the original executable.

`update.cpp` keeps the bonus decay, two player-position fade thresholds, 0.05 boss-position smoothing and bomb state transitions. `start.cpp`/`finish.cpp` use the actual sprite controller, asynchronous text raster, score accumulator and save profiles. The fallback record is current.profiles[18], not the backup snapshot. The name raster leaf44b020 is `text_renderer/centered.cpp`.

`timing.cpp` keeps the 0.0167-second quantization, signed encoding/checksum, and actual recording/playback stage access. Only the oracle may substitute a deterministic clock sample. The shared Replay owner lifecycle and achievement display remain required source dependencies in other modules.

The library compiles for Win32. Original-CPU differential testing is in progress; compilation does not establish original gameplay equivalence or complete game source. Invalid out-of-table asset indices and the original integer division trap are reported explicitly. Full launch, asynchronous GDI raster and whole-game playback are not yet validated.

The specimen is th20.exe v1.00c SHA256 `a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897`. `evidence/` contains disassembly and read-only constants; `extract_constants.py` verifies this hash before extraction.
