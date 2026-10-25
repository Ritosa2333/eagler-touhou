# TH15 ordered triangle batches

`RendererBatch.inc` is the canonical shared packing implementation for normal
and multiplayer TH15. Consecutive authored triangle batches may share a device
submission only when the existing renderer compatibility predicate accepts the
complete effective state. Vertex bytes are copied into renderer-owned storage
before the call returns; caller arenas and interpolation scratch may be reused.
No native update, Draw callback, game geometry, input, or rollback tick is skipped.

First/unacknowledged texture uploads keep the previous immediate path. Prepared
textures retain the existing change/copy/read/release flush boundaries. Primitive
order and strip/fan winding remain unchanged. World instancing still uses the
same separate path. Storage flushes at the existing one-MiB threshold.

`directBytes` counts authored triangle input; `copiedBytes` now also accounts for
those inputs when they join the owned device batch. Neither counter measures
frame rate or actual GPU execution time.

## Evidence lanes

`node tools/th15-cloud/batch-test.mjs --wasm` compiles the production include and
header with an ordered byte/state sink. It checks both ports over 146 scenarios,
25,100 logical draws and at least 2.7 million assertions per lane. The platform
headers in `batch-stubs` provide types only; this lane does not claim GPU pixels.
Clang/GCC and unmodified AddressSanitizer/UBSan configurations also run natively.

The separate frozen-renderer browser lane builds exact production objects from
`b7e7bed8a9c5f1161f8fececdc7c0026300d8a40` and the candidate. The existing five
rollback cases, lifecycle case and presentation-toggle case use the frozen
renderer for their exact-input peer, rather than comparing two copies of the
new renderer. The wrapper checks exact driver anchors and records both fixture
identities, commands, exit codes and report hashes. Fonts and retail DATA remain
in temporary runner storage, and Replay binaries are not published.

The TH15 Cloud performance lane still uses the fixed `79ef177` baseline, twelve
CPU-4x/15-second observations and the existing input/persistence gates. Accept no
performance conclusion unless the new frozen-renderer correctness lane also
passes. SwiftShader observations do not establish Intel UHD or 60 Hz acceptance.
