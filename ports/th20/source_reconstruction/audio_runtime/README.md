# Audio runtime recovered as C++

This module implements the actual `SoundInf` object at original VA `0x005ba830`, its effect queue, music command state machine, PCM readers and DirectSound8 backend. The object previously called `ThreadRegistry` is this audio controller. `program_entry::ThreadRegistry` is now a type alias, and `entry_adapter.cpp` defines its one actual global instance.

Production targets `th20_audio_runtime` and `th20_audio_entry_adapter` contain C++ and normal Windows API calls. They do not load, map, launch, patch, or embed instructions from the original executable. The original image is used only by the separate CPU test executable. This module is not a claim that the whole game, every audio failure path or thread timing has been reconstructed identically.

## Evidence and implementation

Evidence is `analysis/ghidra/pseudocode/<VA>.c` and `analysis/binary/disassembly.asm` from the verified original image, SHA-256 `a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897`. Assembly was used where Ghidra omitted return values or misidentified types.

| Original addresses | Source behavior | Verification |
|---|---|---|
| `425ce0`, `425fc0`, `425d20`, `426390`, `425d70`, CRT `4011c0` | Request, command, effect and `SoundInf` construction; 90 effect definitions and 72 filename records | CPU object-byte comparison, definition pointer normalization, exact constant-data comparison |
| `428c90`, `426d70`, `426eb0`, `429090`, `428890` | 31-slot command queue; merged pan requests; float position mapping; queued/immediate effect stopping | CPU state, arbitrary float bit patterns, full queue and signed-count cases |
| `426ef0`, `428810`, `428380`, `428560`, `428fa0`, `428fc0` | Effect playback/stop/release, configuration volume conversion, preload mode, readiness | CPU playback order, volume and configuration fields; backend release lifecycle |
| `4277f0` | Nine command types, staged music switching, busy waits, effect dispatch | CPU null-stream and active-stream stages; allocation/thread teardown stages additionally exercised by actual backend |
| `426840`, `428420`, `428ee0`, `427360`, `426890`, `426be0`, `4286b0`, `428990`, `426170`, `426c70` | Format loading, name lookup, PCM preloading, file/memory music, notification worker, shutdown | CPU lookup/fallback; actual DirectSound backend file/memory playback and thread join |
| `4259a0`, `4598a0`, `45aaa0`, `45b850`, `427050`, `4283b0`, `426760` | DirectSound device/primary buffer, RIFF chunks, effect source/duplicate buffers | Actual Windows DirectSound8: 90 effect buffers, generated silent WAV resources |
| `4594d0`, `4596d0`, `459920`, `459a30`, `45ab40`, `45ac90`, `45af40`, `45aff0`, `45b5f0`, `45bcc0` | PCM file and memory reader | CPU memory reads/resets and real isolated-file open/seek/read/close comparisons |
| `459530`, `459630`, `4597b0`, `459900`, `459a70`, `459d70`, `45b150` | Music stream construction, buffers, notification positions, buffer recreation, destruction | Actual DirectSound file and memory stream creation/recreation/destruction |
| `45a240`, `45a460`, `45a4a0`, `45b410`, `45b490`, `45a580`, `45b780` | Buffer fill, loop/silence, reset, restore, notification ring updates | CPU PCM bytes, reader state, stream state and recorded COM calls |
| `4262f0`, `426330`, `45a050`, `45a0c0`, `45a130`, `45a1a0`, `45b9b0`, `4d9710` | Fade setup/four fade modes/music volume and ordered frame update | CPU fade state, returns, volume calls; update order from `4d9710` |
| `45acf0`, `45add0`, `45af10`, `45bb00`, `45bbe0`, `45bd10`, `45bab0`, `45beb0` | Play/pause/resume/stop, music time/loop, replacement and seek | CPU play/pause/resume and looped time; actual backend seek/recreate |

Trivial array access and PMR vector allocation/destruction helpers are represented by ordinary typed array/vector operations. Their addresses are not counted as independently validated game behavior.

## Layout and recovered details

`SoundInf` preserves all original offsets through `0x57e8`; a required source-service `Context*` follows this region. Its requests start at `+0x1c`, PMR preload vector at `+0x187c`, metadata pointer at `+0x1890`, 90 effect channels at `+0x1994`, 72 source buffers at `+0x2204`, duplicate counts at `+0x2324`, 32 command records at `+0x2544`, 16 track names at `+0x46c4`, music filename at `+0x56c4` and stream pointer at `+0x57c4`. Music and effect levels are `+0x57dc/+0x57e0`. Compile-time assertions enforce these layouts on x86.

`WaveReader` is `0xa0` bytes. `TrackFormat` is the original packed 52-byte `thbgm.fmt` record. The stream retains original fields through `0xa8`, with its source-owner pointer following them. Its 4-byte packing preserves the original vtable/data layout, including the double fields at `+0x38/+0x40/+0x48/+0x50`.

Preserved observed behavior includes:

- Command slot 31 is a sentinel; additional commands are silently dropped after 31 occupied slots. The preload branch retains the advanced queue pointer after shifting records, matching the original jump.
- Effect-definition records are not sorted by ID. Binding searches the ID, while the request cooldown path indexes the table by ID; this asymmetry is retained.
- Configuration starts at `0x5c4f08`. Audio enable bytes `0x5c4f7d/7e` are configuration `+0x75/+0x76`; signed volume bytes `0x5c4f86/87` are `+0x7e/+0x7f`. Preload is bit 4 of configuration `flags` at `+0x84`.
- The effect volume uses a cubic curve; music volume a quadratic curve; the stored configuration attenuation uses a fourth-power curve. Operation order and truncating SSE conversion are preserved.
- Name lookup prefers the final `/`; only when absent does it use the final `\`. An unknown track returns index 0. This return fallback is present in assembly but omitted by the decompiler.
- The memory reader ignores seek position when reset, applies loop start independently, and `open_memory(...,0)` initializes its fields then returns `E_NOTIMPL`; the original caller ignores that result.
- RIFF traversal advances chunk size plus eight without an odd-byte padding adjustment. A missing resource returns `-1`; failures after resource loading return zero.
- The original starts its window timer with ID 0 and kills ID 1. Original staged shutdown clears a joined thread handle before the later close; that observed field behavior is retained.

## Integration

`audio::bind_game_services()` binds the existing configuration, log, clock and `resources::read` manager to `program_entry::thread_registry`; it does not initialize audio or change startup order. The owner of original loading-thread function `4d8350` should call it before these recovered operations:

```cpp
audio::bind_game_services();
auto& sound = program_entry::thread_registry;
sound.load_formats("../../bgm/thbgm.fmt");
sound.initialize(program_entry::window_state.window, *sound.context);
sound.apply_configuration();
// The loading-thread's original file-existence/preload branch controls whether
// to call start_stream("thbgm.dat") or only store the music filename.
```

The graphics update already calls `audio::update_stream(sound)` in original order `45a1a0 → 45a0c0 → 45a130 → 45a050`. Window shutdown adapters bind `poll_background_jobs`, `stop_audio` and entry dependency `fn_00426170` to actual methods. All locks come from `runtime::shared_locks()`: command/effect processing slot 11, stream refill/seek slot 12, file preload slot 2 and allocation slot 1.

## Build and verification

```powershell
& D:\cmake\bin\cmake.exe -S source_reconstruction/audio_runtime -B source_reconstruction/audio_runtime/build -G 'Visual Studio 16 2019' -A Win32
& D:\cmake\bin\cmake.exe --build source_reconstruction/audio_runtime/build --config Release
& source_reconstruction/audio_runtime/build/Release/th20_audio_cpu_compare.exe '<verified th20.exe path>' source_reconstruction/audio_runtime/cpu_validation.json
& source_reconstruction/audio_runtime/build/Release/th20_audio_backend_smoke.exe source_reconstruction/audio_runtime/build/backend_smoke
```

MSVC uses static CRT, C++20, `/fp:strict` and `/arch:SSE2`. The CPU report binds every production audio source/header, the adapter, and the oracle source to SHA-256 values. The separate backend report is `build/backend_smoke/backend_validation.json`. The backend test uses a hidden owned window and generated silent PCM; it creates all 90 effect buffers, file and memory streams, pauses/resumes/seeks, joins the notification worker, and releases resources without the original executable.

The CPU report covers selected well-defined inputs and controlled host boundaries. Hardware/device-loss timing, arbitrary corrupt metadata, failed/truncated file I/O byte counts, resource exhaustion, and all inter-thread schedules are outside its equivalence claim. Source-side bounds checks reject several original overread/non-progress cases; failed factory cleanup avoids some original leaks. These are explicitly defined error behavior, not evidence of identity to undefined original behavior. The original notification architecture and lock regions are retained; whole-game scheduling and final audible output still require game integration and longer runtime comparison.
