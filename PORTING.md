# TH06 reallyportable architecture

This branch ports EoSD using the proven `th07` reallyportable platform model.
It is intentionally not an extension of the old SDL2 `WebGL` experiment.

## Non-negotiable boundaries

- `src/graphics/Gles.*` is the only accelerated renderer used by web builds.
- SDL3 callback lifecycle (`SDL_AppInit`, `SDL_AppIterate`, `SDL_AppEvent`,
  `SDL_AppQuit`) owns the application loop. No blocking loop or browser-only
  sleep workaround is allowed.
- Audio is callback/engine driven and must not require a producer thread on the
  browser main thread.
- Save/config data lives under `/savesth06` and is persisted with IDBFS.
- Original game data, music, and fonts stay in the ignored `assets/` directory.
  They must never be committed or shipped by this source repository.
- Web support is a platform of the portable fork, not a separate copy of the
  game sources or a pile of `__EMSCRIPTEN__` patches.

## Migration order

1. Establish the same CMake, vendored SDL3, shell, and asset boundary as th07.
2. Move window/event ownership to SDL3 callbacks and make one `Render()` call
   advance a bounded amount of work.
3. Port the th07 GLES3 backend contract to TH06's draw/state semantics, then
   remove `WebGL`, `FixedFunctionGL`, `GLFunc`, and the software fallback from
   the web target.
4. Replace SDL2 audio/threading with the th07-style non-blocking audio path.
5. Add IDBFS save mounting, user-gesture audio start, keyboard/gamepad input,
   and touch controls.
6. Accept only after browser tests reach title, menu, gameplay, pause/resume,
   audio, and save reload at stable logic timing.

## Asset contract

For a local bundled test build, place legally obtained files in `assets/`:

- the six `紅魔郷*.DAT` archives;
- `bgm/th06_*.wav` and matching position files when present;
- `msgothic.ttc` (or a compatible user-provided font).

Public deployment must use a user-supplied asset flow; generated `.data` files
contain copyrighted game content and are not distributable with this project.
