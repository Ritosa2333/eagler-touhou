# macOS collaboration setup

The source of truth is GitHub. Clone this repository recursively so the SDL
submodules are present:

```sh
git clone --recursive https://github.com/YomotsuHisami/th07.git th07-eagler
cd th07-eagler
git switch eagler
```

The Web build uses the pinned SDL submodule without local source patches. The
Runtime requests its Web audio buffer through SDL's public
`SDL_HINT_AUDIO_DEVICE_SAMPLE_FRAMES` API before opening the Emscripten audio
device. After cloning, refreshing, or building, the vendored SDL submodules are
expected to remain clean.

## What is intentionally not in Git

- Original TH07 game data, music, fonts, executables, or offline packages.
- Emscripten build trees and generated HTML/JavaScript/WebAssembly artifacts.
- Local deployment credentials, TURN secrets, server state, logs, and captures.

Use the separately supplied Mac debug supplement for the current prebuilt Web
Runtime. The supplement contains no original DATA/OGG game resources; import a
complete offline package in the Launcher when gameplay data is required.

## Current Web debugging boundary

The multiplayer Runtime uses input-only RTC/WebSocket transport and local
rollback. The current production baseline restores rollback state every logical
frame. Do not reintroduce the old implementation that simply skipped odd-frame
capture: the first-write undo journal requires every changed frame to belong to
a valid restore checkpoint.

The open performance issue is mobile frame loss as active bullet count rises.
Desktop high-bullet performance and recent two-player desync behavior have
improved, but mobile performance still needs real-device profiling.
