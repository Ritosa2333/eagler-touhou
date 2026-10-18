This repo is uploaded on behalf of [@SteinsGateON](https://space.bilibili.com/34714121).

# th15

[![QQ Group 1124121427](https://img.shields.io/badge/QQ%20Group-1124121427-12B7F5?logo=tencentqq)](https://qm.qq.com/q/eeUrxIltug?from=tim)

A high-fidelity, portable reimplementation of 東方紺珠伝　～ Legacy of Lunatic Kingdom ver 1.00b.

## Layout

- `th15_web/`: TH15 game, SDL runtime, launcher, documentation, source tests, and original-game oracle comparison tools.
- `portable/`: shared GLES renderer and input code.
- `th08_web/`, `th09_web/`, `th10_web/`, `th11_web/`: dependency metadata and shared validation, packaging, and server helpers used by TH15.
- `tools/`: the pinned Emscripten installer and local development input importer.

## Build

Install Python and Node.js 22 or newer, then install the pinned toolchain:

```powershell
python tools/download-emscripten.py
```

Restore local assets and test dependencies from the matching development package:

```powershell
python tools/import-local-inputs.py "D:/path/to/TH15-1.0.3-development-20261005"
node th15_web/scripts/build-sdl-application.mjs --release
node th15_web/scripts/package-release.mjs
```

Run the original-game oracle comparisons:

```powershell
node th15_web/scripts/cpp/build.mjs
node th15_web/scripts/cpp/test-suite.mjs local-oracle
```

Build outputs are written below `th15_web/artifacts/` and are intentionally not tracked. See [build and test instructions](handoff/BUILD-AND-TEST.md) for the development package workflow.

The font raster oracle requires the Windows font environment recorded in `th15_web/assets/sdl-native/fonts/manifest.json`.

## Assets and licensing

This repository does not include original Touhou executable, data, music, replay, save, or generated font files. A runnable package and original-game oracle tests must be assembled locally from files you are legally allowed to use. The importer verifies the development package's file hashes before copying local inputs.

## License

This project is licensed under the MIT License. Bundled third-party components retain their own licenses, including the launcher's [GPL-3.0 license](th15_web/launcher/LICENSE).
