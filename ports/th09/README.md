This repo is uploaded on behalf of [@SteinsGateON](https://space.bilibili.com/34714121).

# th09

[![QQ Group 1124121427](https://img.shields.io/badge/QQ%20Group-1124121427-12B7F5?logo=tencentqq)](https://qm.qq.com/q/eeUrxIltug?from=tim)

A high-fidelity, portable reimplementation of 東方花映塚　～ Phantasmagoria of Flower View ver 1.50a.

## Layout

- `th09_web/`: TH09 game, SDL runtime, documentation, and source tests.
- `th10_web/launcher/`: the shared browser launcher and package subsystem used by TH09.
- `portable/`: shared SDL renderer and input code.
- `tools/`: the pinned Emscripten installer metadata.

## Build

Install the pinned toolchain, then build from `th09_web/`:

```powershell
python tools/download-emscripten.py
npm run web:build
```

Build outputs are written below `th09_web/artifacts/` and are intentionally not tracked.

## Assets and licensing

This repository does not track the original Touhou executable, data, music, replay, save files, extracted retail assets, or bundled development toolchains. A runnable package must be assembled locally from files you are legally allowed to use.

## License

This project is licensed under the MIT License.
