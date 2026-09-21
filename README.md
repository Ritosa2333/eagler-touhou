# TH08 portable

This repository contains the source-only TH08 3.4.0 C++/SDL3 portable and Web implementation.

## Layout

- `th08_web/`: TH08 game, SDL runtime, documentation, and source tests.
- `th10_web/launcher/`: the shared Eagler Touhou launcher and browser package subsystem used by TH08.
- `portable/`: shared GLES renderer, numeric compatibility code, input code, build orchestration, and validation tools.
- `tools/`: the pinned Emscripten installer and its metadata.

## Build

Install the pinned toolchain, then build the browser runtime:

```powershell
python tools/download-emscripten.py
node portable/build.mjs --th08
```

Build outputs are written below `th08_web/artifacts/` and are intentionally not tracked.

## Presentation Lab

[Presentation Lab](portable/presentation-lab/README.md) is the supported
diagnostic workbench for TH08 high-refresh presentation. It provides fixed-tick
stepping, draw-only alpha inspection, fast Replay traversal and bounded evidence
reports while keeping its instrumented WASM outside the production package.

Run the normal source gate after presentation changes:

```powershell
node --test portable/presentation-lab/test-analyzer.mjs
node portable/check-high-refresh-contract.mjs
node portable/check-presentation-purity.mjs
```

Build and start the isolated workbench with
`./portable/presentation-lab/start-lab.ps1 -Build`. The script starts only the
loopback server; it does not open a browser.

## Replay logic verification

The [TH08 Replay verifier](tools/replay-verifier/README.md) publishes
content-addressed original traces for a fast four-Demo gate and complete
Lunatic/Extra daily regression. Normal checks do not require the retail
executable; regenerating an oracle is a separate advanced maintenance task.

## thprac

The TH08 Runtime contains a source-level port of thprac's practice parameters and
section patches. Enable thprac in the Eagler Touhou Launcher, enter the game's
Practice menu, then choose a stage section, boss phase, resources, gauge, time,
night value, familiar count, and rank. Practice Replays embed the same
`USER`/`PRAC` parameter block (THPracParam::GetJson()) that upstream thprac
appends for th08, so replays interchange with the desktop tool; playback
restores the same parameters while thprac is enabled. Backspace opens the
in-game assist panel and F1-F7 operate its controls. Matching upstream
`th08_save_replay`, a practice run saves its Replay and `USER`/`PRAC` block
whenever advanced mode is active, including when assists were used.

Generated section tables and bytecode patch adapters are checked against the
local `thprac` repository with:

```powershell
node portable/generate-thprac.mjs <path-to-thprac> --check
node portable/check-th08-practice.mjs
```

The upstream checkout is explicit so worktrees and differently named local
clones cannot silently select the wrong source. Generated files are always
checked or written inside this repository.

## Assets and licensing

This repository does not include original Touhou executable, data, music, replay, or save files. A runnable package must be assembled locally from files you are legally allowed to use.

Licensing is component-specific. Keep the notices and licenses beside each bundled component; no blanket license is asserted for the original game or its assets.
