# TH20 replay input-isolation probe

Deterministic browser probe for the question "can live input change replay
playback?". It boots the SDL3/WebAssembly game directly (no launcher shell),
steps it with the fixed-step `sdl_loop_tick` entry point instead of
`requestAnimationFrame`, drives the title menu by key injection and plays a
saved replay while injecting live keyboard/touch input.

Playback is a simulation: the recorded input stream is the only input the game
may consume. Two runs of the same replay therefore have to agree frame by frame.
The probe hashes the player position, player state and scene every frame and
reports one FNV-1a hash per 300 frames plus the dialogue windows, so a
divergence can be located without transferring a per-frame trace.

## Run

```powershell
# from the repository root
node th20_web/replay-probe/serve-replay-probe.mjs --port 8791 `
  --game 'C:\Program Files (x86)\上海アリス幻樂団\東方錦上京' `
  --replay '..\th20_02.rpy'
```

Then open `http://127.0.0.1:8791/replay-probe.html?build=new` and drive
`window.H.longRun(mode, frames, referenceBlocks)` from the console (or from a
browser-automation session). `build=base` loads `baseline/th20-sdl.mjs`, which
is how a pre-fix generation is compared against the current artifact.

Supported injectors: `none`, `key-shoot`, `key-mash`, `key-slow`, `arrow-left`,
`arrow-cycle`, `touch-drag`, `touch-fire`, `touch-all`.

`key-*` drives the hosted-keyboard bridge (`sdl_key`); that is the same path the
launcher's on-screen controls and a physical keyboard use. `touch-*` drives the
gesture/direct-touch exports.

The server serves `th20.dat` and a system font from the retail install and falls
back to `th20_web/artifacts/sdl3` for `th20-sdl.mjs` / `th20-sdl.wasm`, so no
build output needs to be copied. `baseline/th20-sdl.{mjs,wasm}` is not committed
either; drop a generation that should be compared against into that directory.

See `reports/th20_replay_input_isolation.md` for the recorded results.
