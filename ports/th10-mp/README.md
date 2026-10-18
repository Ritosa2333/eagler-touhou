This repo is uploaded on behalf of [@SteinsGateON](https://space.bilibili.com/34714121).

# TH10 portable

This branch contains the source-only TH10 3.5.1 C++/SDL3 portable and Web implementation.

## Layout

- `th10_web/`: TH10 game, SDL runtime, launcher integration, documentation, and source tests.
- `portable/`: shared GLES renderer, numeric compatibility code, input code, build orchestration, and validation tools.
- `tools/`: the pinned Emscripten installer and its metadata.

## Build

Install the pinned toolchain, then build the browser runtime:

```powershell
python tools/download-emscripten.py
node portable/build.mjs --th10
```

Build outputs are written below `th10_web/artifacts/` and are intentionally not tracked.

## Presentation Lab

[Presentation Lab](portable/presentation-lab/README.md) is wired as an isolated
TH10 diagnostic workbench and uses the shared v1 contract from the pinned
`eagler-common` submodule. Fixed-tick control and normalized ANM submission
evidence are available; title-wide owner and state coverage remains incomplete,
so clean sweeps intentionally report purity `unknown` rather than pass.

Run the normal source gate after presentation changes:

```powershell
node portable/check-high-refresh-contract.mjs
node portable/check-presentation-purity.mjs
node --test third_party/eagler-common/testkit/presentation-lab/controller-core.test.mjs third_party/eagler-common/testkit/presentation-lab/release-contract.test.mjs
```

Build and start the isolated workbench with
`./portable/presentation-lab/start-lab.ps1 -Build`. The script starts only the
loopback server; it does not open a browser.

## Replay logic verification

The [TH10 Replay verifier](tools/replay-verifier/README.md) publishes
content-addressed original traces for a fast four-Demo gate and complete
Lunatic/Extra daily regression. Normal checks do not require the retail
executable; regenerating an oracle is a separate advanced maintenance task.

## Assets and licensing

This repository does not include original Touhou executable, data, music, replay, or save files. A runnable package must be assembled locally from files you are legally allowed to use.

## License

This project is licensed under the MIT License.
