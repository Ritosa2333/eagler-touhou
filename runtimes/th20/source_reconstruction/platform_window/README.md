# Recovered Windows platform and display source

This is new C++ source. It neither starts the original EXE nor embeds or executes
its instructions. It compiles as `th20_platform_window.lib`; it is one component
of the still-incomplete source reconstruction, not a playable standalone game.

| Original VA | Source implementation | Current evidence |
|---|---|---|
| 0041e050 | `calculate_layout` | 6,912 actual CPU cases, all object bytes, all four SSE rounding modes, separate/global `this` |
| 00419c00 / 0041a280 | `pressed` / `repeated_or_pressed` | 20,000 actual CPU return comparisons |
| 0041c020 | `acquire_single_instance` | assembly and compilable Win32 calls |
| 0041ccf0 / 0041d350 | `create_game_window` / `window_proc` | assembly, corrected omitted return values |
| 0041d0c0 | `is_japanese_user_locale` | `GetUserDefaultLCID()==0x411` |
| 0041ae70 / 0041abe0 / 0041ab30 | startup dialog / callback / apply | assembly, native Win32 APIs, real input dependency |
| 0041a1b0 | `register_dialog_raw_input` | original joystick RAWINPUTDEVICE fields |
| 0041c320 / 0041c3e0 | Direct3D object / presentation setup | actual D3D9 calls, source build only |
| 0041c1a0 / 0041c730 | device creation / backbuffer creation and reset | actual HAL/REF fallback and both reset attempts |
| 0041a2c0 | `initialize_render_state` | ordered 24 render, 8 texture, 6 sampler writes; five sprite cache fields |
| 0041d0f0 / 004117a0 | entry adapters | display-mode predicate / first-field assignment |
| 0040b780 | shared `runtime::Worker` constructor | 256 original-CPU object/EAX comparisons; real `std::jthread` |
| 004ddb20 / 004da1f0 | render viewport offset and camera | 2,000 original-CPU full graphics-object byte comparisons, actual D3DX SDK matrix calls |
| 004dbce0 | center render viewports | compiled composition of the recovered offset operation |
| 004dd840 / 004dbd70 | release/acquire render surfaces | actual COM methods; ANM script binding remains external |
| 00401090 / 00418ac0 | window global initialization | actual 0x2138-byte zeroing and display-mode 2 |
| 0040aa10 / 004d8990 | graphics global initialization | actual 0xde8-byte zeroing, config ctor, event flags 0x880, real worker construction |
| 0056b3f0 / 004d8da0 | graphics static destruction | reverse worker destruction, shared runtime locks |
| 004d9e30 | graphics worker closure | original detach-before-join order; three source thread behavior tests |
| 004dd490 | graphics shutdown | compiled real control flow with explicit scene/audio/archive/ANM dependencies |
| 004daba0 | `initialize_render_viewports` | 1,000 original CPU comparisons of all six viewport records and untouched graphics bytes |
| 0041dce0 / 004da120 | `select_viewport` / `apply_camera` | 512 original CPU comparisons, original self pointer, sprite cache bits, transform and viewport COM call traces |
| 00416d20 / 00414820 | `initialize_fonts` / enumeration callback | 184 original CPU checks with actual Windows GDI, availability flags and logical font parameters/names |
| 004dda60 | `disable_fog` | flush, cache write and actual D3D state call; source build |
| 004193e0 / 004199a0 / 00419a50 | three `finish_*_frame` paths | original timing arithmetic, raster polling, Present and distinct Reset-success policies; source build |
| 00419760 / 004193c0 | `before_present` / `after_present` | screenshot selection, statistics update and explicit game dependency |
| 0041bfc0 / 0044bb10 | surface copy queue and copy | actual texture/surface methods and SDK D3DX surface copy |
| 004de040 / 004d9210 | `capture_snapshot` / screenshot worker | actual lockable backbuffer, shared Worker, BGRA crop to bottom-up BGR24 |
| 004d9c40 / 004d97f0 / 004d8740 | PNG save / encoder lookup / bitmap allocation | actual GDI+, CP932 path conversion and shared file lock |
| 004abb80 / 004abc90 / 004ac1a0 / 004abad0 / 004ac080 | FrameStatistics construction, destruction, factory and callback registration | actual 0x108 layout and shared CallbackOwner; rendering dependency explicit |
| 004abd50 / 004ac0e0 | FrameStatistics update / local wall clock | source arithmetic and actual system/steady clocks; not original CPU validated |
| 004d9ea0 / 004dd600 / 004de1f0 | archive/version load, graphics initialization and registration | source-owned archive and shared RNG/input/statistics objects, pending ANM asset initialization |
| 004dc510 / 004da540 | graphics update / scene transition table | original scene-dependent calls and EAX 1/4/5 returns; explicit missing game owners |
| 004dc5f0 / 004dc8b0 / 004d9120 / 004dd1c0 / 004d8e90 / 004dd2e0 / 004d8f80 / 004dd400 / 004d9040 / 004dc7c0 | ten ordered draw callbacks | actual D3D clear/state/target operations; explicit Animation draw and layer dependencies |
| 004d9db0 / 004ddf80 | state setter / disable depth writes | actual flush, cached flag and COM operations |

`evidence/` contains documentary disassembly for these functions and their
inlined field accessors. It is not an input to the C++ build. The addresses
in `evidence_manifest.json` are an evidence list, not independently validated
full functions. The current test has **30,864 original-CPU cases plus 3 source
thread behavior tests**, all passing. Font checks compare all scalar LOGFONT
fields and the terminated family name; GDI's unspecified trailing name-buffer
bytes and opaque HFONT values are excluded. COM trace tests validate the calls,
not GPU-rendered pixels. No full rendering or gameplay equivalence is claimed.

`dialogs.rc` is editable textual resource source. Resource IDs 203 and 204,
language 1041, compile to byte-identical 914-byte and 1,012-byte original dialog
templates (`dialog_validation.json`). This intentionally preserves the original
English dialog's `TH19` caption. `data_constants.hpp` contains only recovered
text, numeric resolutions and floats, never executable instruction arrays.

Build and compare:

```powershell
cmake -S source_reconstruction/platform_window -B source_reconstruction/platform_window/build -G "Visual Studio 16 2019" -A Win32
cmake --build source_reconstruction/platform_window/build --config Release
source_reconstruction/platform_window/build/Release/th20_window_cpu_compare.exe '<original th20.exe>' source_reconstruction/platform_window/cpu_validation.json
```

The test maps the verified local original only as an isolated oracle. It does
not invoke its entry point, initialize its runtime or run the game. It resolves
two matrix APIs from the ordinary `d3dx9_43.dll` SDK for viewport checks and
four User32/GDI imports for font checks.
Those mapping facilities are absent from the library source. The production
module also calls the same documented SDK math APIs, never original game code.

Ongoing integration: the input sampler and actual storage are owned by the
`input` module, logs/locks by `runtime_core`, config by `platform_services`, and
the sprite allocation by `sprite_renderer`. Window/graphics global storage and
constructors and 0x108-byte FrameStatistics owner are now real source definitions. References for event flags, reset
countdown, render value and update duration all refer to their one actual field
inside GraphicsState. Remaining external domains are ANM script binding and VM
destruction, scene shutdown, background jobs, scheduler object retirement, audio
stop dispatch and archive-manager closure. Graphics initialization now uses the
source archive manager and shared RNG directly. `graphics_callbacks.hpp` names
the remaining ANM asset/update/draw/layer and startup/menu/game/replay owner
dependencies. `frame_statistics.hpp` names the game-flag and FPS text-renderer
dependencies. These have declarations, not stubs. Compiled control flow is not
equivalent to an independently linked executable.
