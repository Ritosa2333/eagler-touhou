# Web Runtime Playbook

Status: active

## Purpose

Route browser, Emscripten, SDL3/WebGL2, WebView and Runtime-shell work without
confusing browser/platform compatibility with title semantics or product
completion. TH08 and TH10 directory Runtimes are the current reference lane.

## Applicability and authority

Applies to:

- TH08/TH10 C++ / SDL3 / Emscripten directory Runtimes;
- WebGL2 and browser API ownership;
- same-origin Launcher/Runtime shell protocol;
- first-frame, storage, audio and browser lifecycle.

TH06/TH07 provide older browser adaptation experience, not TH08/TH10
game-semantic authority. One browser bring-up result is evidence for that lane
only, not universal Web/mobile acceptance.

<!-- knowledge-id: K-WEB-001 -->
## Normal design

The current directory Runtime build is produced from `portable/build.mjs` as a
C++ / SDL3 / Emscripten module. The shell instantiates the title's
`*-sdl.wasm`, mounts managed DATA and IDBFS, installs declared Runtime/music
resources, applies the canonical `eagler-touhou/1` configure payload, then
starts the C++ loop.

The formal lane does not depend on pthread proxying, SharedArrayBuffer,
`PROXY_TO_PTHREAD` or an OffscreenCanvas worker architecture. Browser-facing
lifecycle and filesystem preparation stay in the shell; simulation/render/audio
ownership stays in C++.

Keep the layer boundary clear:

```text
title source / recovered game semantics
    ↓
C++ game/platform layer
    ↓
SDL3 + semantic GLES/WebGL2 renderer
    ↓
same-origin Runtime shell + Launcher protocol
```

The shell may provide files, mode selection, input and diagnostics. It must not silently reimplement TH08 gameplay or reinterpret Replay/Score/ANM/ECL formats.

<!-- knowledge-id: K-WEB-002 -->
## Invariants

- Target-game behavior comes from the canonical title source/portable contract.
- Browser-only state (DOM, visibility, same-origin parent bridge, IDBFS,
  `AudioContext`) stays in the Runtime shell/host bridge instead of leaking into
  gameplay owners.
- The formal directory Runtime must not require SharedArrayBuffer, COOP/COEP,
  `PROXY_TO_PTHREAD` or an OffscreenCanvas worker path.
- Original `th08.dat`, `thbgm.dat`, fonts, WAV/OGG and Replay fixtures remain external/private test inputs. Do not commit, publish or package retail data.
- Build/runtime artifacts must carry their own closed file/build identity; never
  substitute a copied directory from another title/revision.
- Local browser success does not prove Android/iOS or public deployment success.
- Draining browser/SDL events is not itself a reason to abandon the current RAF
  callback. Native `PeekMessage`-style loops process queued events and then
  continue update/draw/present unless an event actually changes application
  lifecycle. A Web port that `return`s the whole frame after one ordinary event
  can create input-dependent frame drops and visible stalls. This is an adopted
  Web-loop precaution from recovered/native scheduling semantics and port audit;
  do not label it a TH08 live-browser regression unless that title-specific
  evidence has actually been reproduced.

<!-- knowledge-id: K-WEB-003 -->
## Fixed-address aliases must survive portable/Web builds

The original TH08 image and the non-PIE i386 compatibility build can express semantic aliases by placing two symbol names at the same target address. Emscripten/Web does not use `src/modern/linux/th08-layout.ld`; ordinary non-`DIFFBUILD` `DIFFABLE_STATIC*` declarations therefore become independent C++ storage unless the portable layer explicitly preserves the alias semantics.

TH08 repair history found a concrete gameplay regression from this boundary:

- target `g_GameManager` starts at `0x0160f508`;
- its `youkaiGaugeHumanLimit` field is at aggregate offset `0x3ddf8`, giving target address `0x0164d300`;
- target `g_PlayerGaugeBounds` is also `0x0164d300`.

Retail initialization writes `g_PlayerGaugeBounds[0..5]` and relies on that overlap to initialize the six `GameManager` gauge limit/effect/tint fields. In Web, the separate array left the aggregate thresholds at zero. Human testing then observed the extreme human/youkai frame at gauge 0%; source inspection explains it exactly because both `0 <= humanEffectsThreshold(0)` and `0 >= youkaiEffectsThreshold(0)` become true and spawn fixed-slot Effect 25.

Portable rule:

1. When a target/linker symbol address lies inside a known aggregate's target range, determine whether it is a semantic alias before treating it as independent storage.
2. If the portable/Web lane cannot preserve the physical alias, express the alias semantically at the narrow ownership boundary - for example by updating the aggregate fields when the legacy alias array is initialized.
3. Do not add a symptom-specific gameplay guard such as "do not draw at 0%"; restore the original storage/ownership semantics instead.
4. Audit other simulation-relevant target symbols that fall inside aggregate ranges before declaring Replay/gameplay parity. UI-only or provenance-only overlaps do not justify broad rewrites.

## Superseded approaches

The former TH08 `th08-modern` SDL2/CMake feasibility shell and audited
pthread/OffscreenCanvas experiments are historical compatibility evidence, not
the current directory-Runtime architecture. Do not revive their build flags or
shell ownership as product contracts.

## Code anchors

In the sibling TH08/TH10 Runtime repositories:

- `portable/build.mjs`: canonical C++ / SDL3 / Emscripten build and build
  identity;
- `th08_web/cpp/sdl/GameHost.cpp` /
  `th10_web/cpp/sdl/ApplicationHost.cpp`: loop/browser-facing SDL host;
- `th08_web/cpp/platform/BrowserRuntime.cpp` and corresponding TH10 platform
  owners: title/browser boundary;
- `portable/sdl/Renderer.cpp` plus each title's `cpp/sdl/GraphicsHost.cpp`:
  semantic GLES/WebGL2 rendering;
- `th08_web/sdl-runtime/shell.mjs` /
  `th10_web/sdl-runtime/shell.mjs`: protocol, IDBFS, resource install,
  first-frame and lifecycle;
- each `sdl-runtime/eagler-host.mjs`: shared shell-side input/resource/audio
  bridge.

## Verification

For a Web runtime change, use the smallest affected lane first:

1. Build the target directory Runtime from its canonical repository and retain
   the generated build/file identity.
2. Run that Runtime's focused native/browser contract checks and
   `git diff --check`.
3. Verify Runtime Release packaging sees the expected closed directory file set.
4. Run one real browser page at a time through title → Stage 1 and capture page/runtime errors.
5. Re-run affected audio, storage and Replay checks; a harness or static contract is not a device or deployment result.

## Deliberately omitted claims

This playbook does not treat desktop-browser success as iOS/Android acceptance
or assume every recovered fixed-address/storage alias has already been audited.
