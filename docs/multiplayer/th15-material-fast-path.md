# TH15 animation material state

The ordinary and multiplayer renderers share an unchanged-state fast path in
`AnmMaterial.hpp`. It tests the exact fields written by `material()` before
allocating its full local pipeline copy. A matching pipeline keeps the current
batch; any mismatch executes the original state update and flush logic.

Texture lookup, validation and binding still happen first. Draw order, geometry,
blend normalization, integer opacity arithmetic, VM mutations and RNG calls are
unchanged. Unknown blend/address modes preserve the former untouched fields.
NaN fog fields deliberately reject the fast path because the original
`PipelineState::compatible()` comparison rejects those values too.

## Verification

`node tools/th15-cloud/material-test.mjs --wasm` executes the independent former
pipeline update against both port headers, natively and with the selected
Emscripten SDK. Each lane checks 266,896 cases, including all encoded blend and
address modes, individual combiner changes, non-finite fog and seeded random
states. It records commands, actual exit codes and source hashes under
`artifacts/th15-material/`; these are contract checks, not GPU/FPS acceptance.

Before submission, an independent native renderer from `aa90188` matched the
candidate across 175 opacity/draw scenarios with Clang and GCC. The sanitizer
lane disables ASan fake-stack allocation because the legacy identity oracle
requires temporary-address reuse; it is not a default-ASan claim. Those local
logs and the separate native microbenchmark remain in the conversation evidence.

The TH15 Cloud workflow rebuilds the production candidate and executes its
seven real-Wasm correctness groups before the frozen twelve-run comparison.
The baseline is `79ef177`, with CPU 4x, 15-second windows and the existing network
conditions. Software-rendered observations are never Intel UHD or 60 Hz
acceptance. A native material/draw microbenchmark cannot establish whole-game
speedup; the paired runtime receipts determine that separately.
