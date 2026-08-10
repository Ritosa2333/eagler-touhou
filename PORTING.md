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
5. Add IDBFS save mounting, user-gesture audio start, and keyboard/gamepad
   input. Touch controls can follow as a separate platform feature.
6. Accept the first playable milestone only after browser tests reach title,
   menu, character/shot selection, and gameplay at stable logic timing. Audio,
   pause/resume, and save reload remain explicit follow-up acceptance checks.

## High-refresh invariants

TH06 was written around one simulation update and one draw at 60 Hz. Desktop
and web presentation may run faster, but the following rules must remain true:

- Simulation, input edge detection, RNG, timers, collision, audio events, and
  ANM script advancement run only on the fixed 60 Hz calc chain.
- Every presentation frame establishes the viewport, camera, depth clear, and
  other scene state it needs. A draw callback must not rely on the final state
  left by the previous presentation frame.
- Interpolation is limited to state with a complete previous/current lifecycle.
  Creation, object-pool reuse, script replacement, and invisible-to-visible
  transitions must initialize both endpoints to the same authoritative value.
- Layout and clip rectangles are discrete render state. Do not interpolate the
  playfield viewport or other values that determine where subsequent draws are
  clipped.
- Positions derived during drawing from an owning object (for example text on
  a moving result panel) interpolate the owner once, then derive their final
  draw positions. They must not keep an independent history based on temporary
  draw-time coordinates.
- Temporary VM changes made for drawing must be restored before returning, or
  be made on a local VM copy. Draw-only values must not become the next calc
  tick's previous state accidentally.

There are a few deliberate compatibility exceptions inherited by TH07
reallyportable: the pause state changes from its capture state after a draw,
offscreen item indicator sprite selection is render-derived, and the FPS
counter measures presentation time. These paths must remain idempotent on
render-only frames and must never advance RNG, timers, or ANM scripts more than
once per simulation tick.

## Asset contract

For a local bundled test build, place legally obtained files in `assets/`:

- the six `紅魔郷*.DAT` archives;
- `bgm/th06_*.wav` and matching position files when present;
- `msgothic.ttc` (or a compatible user-provided font).

Public deployment must use a user-supplied asset flow; generated `.data` files
contain copyrighted game content and are not distributable with this project.
