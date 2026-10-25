# TH15 rollback draw scratch

Multiplayer animation output uses one host-owned scratch object per renderer.
Both vector metadata and vertex storage stay outside the simulation heap. The
renderer itself, its material binding state, VM quad/color/transform mutations,
parent rotation normalization, error strings and RNG still use ordinary undo.
The host scopes cover only synchronous buffer construction/submission and never
skip original Draw callbacks. No simulation frame or GPU submission is removed.

After restoring a simulation frame, the driver discards any unfinished future
vertex batch before renderer invalidation. The immutable presentation transaction
continues to restore GPU output. Host capacity survives a rewind, so a growing
scratch vector cannot resurrect a freed old backing allocation.

This change belongs to `ports/th15-mp`: it changes the journal ownership boundary
and the multiplayer restore driver, not the ordinary game's renderer policy.

`node tools/th15-cloud/draw-scratch-test.mjs` links production game/journal objects,
instruments the linked Wasm and executes parent-rotation/quad undo, unfinished
batch discard, repeated screen/world/shape buffer growth and error-string undo.
Its synthetic CPU sink is not a GPU or performance test. The separate TH15 Cloud
workflow retains the real-archive oracle, seven browser correctness groups and
twelve same-runner baseline/candidate observations, including failed cases.
Software-rendered cloud timings cannot establish Intel UHD hardware acceptance.
