# Performance and Rendering Playbook

Status: active

## Purpose

Diagnose frame pacing, WebGL/renderer cost, allocation pressure, rollback cost and mobile performance while preserving fixed simulation, audio timing and visual correctness.

Detailed rollback optimization methodology lives in `rollback.md`. This file
continues to own generic renderer/profiling guidance; do not duplicate or
partially reconstruct the TH07 rollback experiment history here.

## Applicability and authority

- TH06/TH07 profiling and VBO/frame-pacing changes are reusable Web adaptation
  evidence.
- TH08/TH10 current directory Runtimes provide the semantic GLES/WebGL2 renderer
  reference.
- Device measurements are evidence for the named device/browser, not universal
  performance guarantees.

<!-- knowledge-id: K-PERF-001 -->
## Normal design

First classify the cost: CPU simulation, GPU/WebGL state/storage, WASM copy, allocation, audio pump, thermal throttling or browser/device behavior. Keep simulation cadence and correctness contracts fixed while testing one layer at a time.

The validated Web-only VBO direction replaces storage lifetime/update behavior with `glBufferData(bytesNeeded, data, STREAM_DRAW)` and `firstVertex=0`. It does not change batching, draw state or simulation. Native paths retain their original update behavior unless a separate native regression proves otherwise.

ImGui state traffic is a separate WebGL cost. On Emscripten, the proven direction skips unnecessary synchronous state save/restore and resets the known state at the blit boundary; it does not silently remove required game state.

Frame pacing and presentation are linked to `interpolation.md`, not duplicated here: fixed 60 Hz logical work remains fixed, and display-side work must not become a second simulation loop. Audio queue/decode rules are linked to `audio.md`.

<!-- knowledge-id: K-PERF-002 -->
## Invariants and pitfalls

- Do not reduce simulation frequency, bullet/enemy counts, correctness checks or rollback state to make a graph look better.
- Do not treat a lower FPS number, a fabricated counter or a screenshot as a performance diagnosis.
- Separate GPU process CPU, game CPU, browser overhead, thermal state and audio starvation before changing code.
- Do not use heavy readback diagnostics in the final hot path. Remove probes after collecting the evidence.
- Rollback uses bounded sparse state/journal ownership; a full dense snapshot for every tick is not a free optimization.
- Presentation smoothing must not write back to logical positions, collision state, Replay or snapshots.
- Repeating a Draw at display refresh must be idempotent within one logical tick. Projection, billboard sizing, fog/color setup and custom geometry may use temporary state, but a second Draw of the same current state must not consume the first Draw's output as new input.
- `AudioContext`/audio pump starvation can look like a frozen game; verify elapsed audio and simulation evidence separately.

<!-- knowledge-id: K-PERF-004 -->
## TH08 WebGL 3D fidelity and repeated-Draw state

TH08 repair history established two renderer rules with direct visual/source
evidence:

1. If authored 3D vertices are transformed to screen coordinates on the CPU before entering WebGL, preserve the original clip-space W in the Web vertex and reconstruct `gl_Position` so interpolated UV/color varyings remain perspective-correct. Sending screen-space XYZ with `gl_Position.w = 1` produces affine interpolation and caused tilted/stretched multicolored 3D materials. Preserving clip W removed that material corruption in human testing.
2. Retail Draw code may use an `AnmVm` field as scratch because it runs once per authored frame. At 120/165 Hz the same Draw can run multiple times for one 60 Hz state. A Background billboard path wrote projected screen scale back into `vm.scale` and then reused that scale on the next presentation, creating recursive drift/flicker. Save/restore or otherwise make such Draw-time scratch state idempotent before repeating it.

These are renderer/presentation fixes only. They do not justify changing depth, blend, fog or gameplay semantics without a focused visual/source comparison.

## Cross-title renderer adaptation failures

Recent repair history adds several implementation-level pitfalls. Evidence
scope is part of each rule below: TH06/TH07 proof remains reusable adaptation
experience, TH08 proof is named explicitly when it exists, and an adopted
precaution is not silently upgraded to a TH08-verified regression. These are
not product-facing requirements by themselves; they explain how an otherwise
correct adaptation can become visually wrong when retail D3D or
one-draw-per-tick assumptions are transplanted into a modern renderer.

- **Re-establish per-frame render state.** A presentation-only draw must not
  depend on the viewport, clip region, camera, depth/fog state or other mutable
  renderer state left by the tail of the previous presentation frame. TH06 and
  TH07 both produced pause/retry viewport regressions when the next high-refresh
  draw inherited the previous frame's final full-window/playfield state. This
  is proven TH06/TH07 evidence; treat it as an adopted precaution elsewhere
  until reproduced or source-proven for that title.
- **Preserve capture timing.** Pause/transition screenshots are one-shot render
  requests whose authored draw-stage timing matters. Flush before the capture at
  the original stage; do not move all captures to post-Present, and do not let a
  presentation-only frame consume a request intended for the next authored
  draw. Direct repair evidence is TH07; other titles inherit the precaution,
  not a claimed identical bug.
- **Synchronize renderer caches after out-of-band drawing.** Ending paths,
  screenshot surfaces, special backgrounds or other code that bypasses the
  normal renderer can directly change texture/blend/viewport/program state. It
  must restore that state or invalidate the normal renderer cache before normal
  draws resume. TH07 Ending/screenshot repair provides the direct evidence.
- **Do not port persistent-backbuffer dirty redraw mechanically.** Retail code
  may skip redrawing lives/bombs/HUD regions because the old backbuffer retained
  them. A modern renderer that clears the whole frame must still redraw every
  visible element without consuming gameplay dirty state merely to keep pixels
  on screen. Current TH08 repair history provides direct evidence for this
  class.
- **Preserve pretransformed-vertex perspective semantics.** D3D transformed
  vertices can carry reciprocal-W/clip-W information even after CPU-side screen
  projection. Replacing that with `w = 1` changes interpolation from perspective
  to affine and can visibly skew distant textures/materials. TH08 has direct
  source and human visual evidence for this failure.
- **Make filtering safe once subpixel presentation is possible.** Fractional
  sprite placement plus linear filtering can expose atlas-neighbor texels or RGB
  hidden under fully transparent pixels. TH06/TH07 use edge extrusion for static
  ANM sprites; other backends may use different UV/gutter/filtering solutions.
  The implementation requirement is to prevent sampling bleed, not to mandate a
  particular atlas design. This is proven TH06/TH07 evidence; no identical
  TH08/TH10 regression is claimed here.

## Superseded approaches

Reducing simulation fidelity, disabling correctness checks, changing native update behavior for a Web-only issue, or using a dense full-world snapshot as a per-tick optimization is superseded. The Web-only VBO storage fix does not imply a new renderer for every platform.

<!-- knowledge-id: K-PERF-003 -->
## Faithful generic-layer performance

Performance is part of behavioral fidelity when a runtime's CPU, GPU, WASM or audio work starves the frame or audio pump. Profile the actual hot layer and improve the generic implementation with bounded allocations, cache-friendly data and batching; do not hide a slow path with a per-game exception or by reducing simulation/correctness.

## Target-runtime anchors

In the sibling TH08/TH10 Runtime repositories:

- `portable/sdl/Renderer.cpp`, `Renderer.hpp`, `GraphicsState.hpp` and
  `Shaders.hpp`: shared semantic GLES/WebGL2 renderer;
- each title's `cpp/sdl/GraphicsHost.cpp`: browser graphics host;
- each title's player/enemy/bullet/item/background draw owners: owner-specific
  presentation and upload cost;
- `portable/build.mjs`: current Runtime build identity and optimization flags;
- focused `portable/check-*.mjs` / cadence tests: browser/runtime performance
  probes. Treat each probe as evidence only for the workload it actually runs.

## Verification

1. Reproduce on a fresh or correctly isolated build and name the thread/shell lane.
2. Capture the smallest relevant profile: CPU/GPU, WebGL calls/state, WASM copy/allocation, audio callback or device thermal data.
3. Compare normal and changed paths with the same resolution/DPI and game state.
4. Run title → Stage 1, OGG/MIDI, storage and direct-entry Replay regression.
5. For mobile, record device, OS, browser/WebView, DPI/orientation and whether the action was automatic or human.
6. Remove diagnostic readbacks and report only the evidence actually captured.

## Deliberately omitted claims

One browser/device profile does not represent all devices, and a Web renderer
optimization does not justify changing native rendering or simulation
semantics.
