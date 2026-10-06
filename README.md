# TH11 portable

This repository contains the source-only TH11 1.00a C++/SDL3 portable and Web implementation.

## Layout

- `th11_web/`: TH11 game, SDL runtime, launcher integration, documentation, and source tests.
- `portable/`: shared GLES renderer, input code, SDL support code, and bundled third-party source dependencies.

## Build

The source tree follows the same shared workspace/toolchain layout used by TH08 and TH10. See `th11_web/docs/BUILD.md` for the current build, validation, and packaging commands.

Generated build outputs are written below `th11_web/artifacts/` and are intentionally not tracked.

## Assets and licensing

This repository does not include the original Touhou executable, game data, music, replay, save files, or private development reference material. A runnable package must be assembled locally from files you are legally allowed to use.

Licensing is component-specific. Keep the notices and licenses beside each bundled component; no blanket license is asserted for the original game or its assets.

## Replay verification

`tools/replay-verifier/` adapts the common Replay verifier with all four stored
Demo replays and Reimu/Yukari Lunatic and Extra clear cases. Original-derived
golden traces cover 161,977 declared-state ticks. See the
[commands and scope](tools/replay-verifier/README.md) and
[acceptance record](tools/replay-verifier/RESULTS.md).
