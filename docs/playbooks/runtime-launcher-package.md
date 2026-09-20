# Runtime, Launcher and Package Playbook

Status: active

## Purpose

Keep App Shell, immutable Runtime/content, mutable user DATA, Package Descriptor/Store and Launcher product state separate. Route import, update, offline, storage and HTTP migration work without treating a prepared Runtime as a completed game or device product.

## Applicability and authority

Normative ownership rules live in [Architecture](../ARCHITECTURE.md). Storage
validation and browser conformance are documented in
[Runtime storage testing](../RUNTIME_STORAGE_TESTING.md).

The current four-game adaptation authority is split deliberately:

- `src/contracts/adapter-capabilities.mts` answers **whether a
  capability is required at all**;
- `src/contracts/product-catalog.mts` describes real per-product
  differences and must not be used to opt out of required capabilities;
- `lib/content-definition.mjs` owns original-content shape plus
  `hostPreparation`, i.e. the format/build recipe used to materialize declared
  content capabilities without turning that recipe into another product flag;
- `docs/ADAPTING_A_GAME.md` is the step-by-step adapter guide;
- `docs/ADAPTER_CAPABILITIES.md` explains required, inherited,
  profile-required, optional, implementation-detail, format-adapter and
  compatibility classes.

TH06, TH07, TH08 and TH10 all participate in the current Runtime Release,
Launcher/package and workspace contract lanes. That is source/integration
evidence; public deployment, browser and device acceptance remain separate
evidence levels.

<!-- knowledge-id: K-RUNTIME-001 -->
<!-- knowledge-id: K-RUNTIME-002 -->
<!-- knowledge-id: K-RUNTIME-003 -->
## Normal design

Use this ownership model:

```text
App Shell / Launcher
    → selects and prepares a package
immutable Runtime + content
    → runs the game
mutable user DATA / save / Replay
    → stays in explicit storage roots
```

Service Worker caches and updates the App Shell. It does not own the Runtime filesystem or hide a package Runtime inside SW state. The Runtime uses same-origin HTML/JS/WASM and the App/Launcher manages DATA.

Resource delivery has three modes. `hosted` publishes game/shared payloads with the site; `external` keeps Launcher and Runtime local while retaining Release Catalog and Package Descriptors for automatic acquisition through same-origin redirect routes; `import` requires a user-selected local Package. External storage topology does not enter Package Descriptor URLs: descriptors retain safe relative sources, and the web-server/CDN layer owns redirects and CORS.

The static `src/contracts/product-catalog.mts` module is the product registry.
The browser-visible `host-manifest.json` describes the deployed Runtime/content
surface, while `release-catalog.json` points at installable Package
Descriptors. There is no third `games.json` protocol. Local import stays
available even when remote fetch fails. Missing files may be non-fatal during
descriptor inspection, but the start gate must complete required files before
calling the Runtime. Package update replaces the installed state through
descriptor/generation atomicity; a partially updated package must not become
current.

Keep ordinary and MP storage roots separate. Keep DATA separate from Replay/save. `runtimeReady=true` means required Runtime dependencies are prepared; it is not a game-function, browser or device PASS. Record local provenance so an old package cannot silently pose as a new one.

HTTP-to-HTTPS migration must first run on the old Origin. Preserve a precise migration page without HSTS and use nonce/origin-checked `postMessage`; migrate allowed IndexedDB/localStorage/Cache Storage application data, not arbitrary browser cache or raw OGG cache. Ordinary HTTP requests can redirect after migration handling is available.

## Invariants and pitfalls

- Do not replace `FileSystem::GetPrefPath` with a JS-only mount change when the game uses relative paths; the C++ preference path and IDBFS mount must agree.
- Do not mix App Shell/root redirect with a game data directory.
- Keep generated HTML, managed DATA hooks, package generation and public catalog from the same release generation.
- A stale shell must be rejected by packaging/verification rather than silently repaired by Launcher runtime guesses.
- Show network activity for runtime fetch, package import, OGG/DATA download and App Shell update.
- The feasibility shell is evidence for bring-up, not the final Launcher/Runtime product architecture.
- Do not infer a new title's product contract by copying the previous title's
  `features` object or implementation. Required capabilities have no per-game
  `false`; inherited Launcher features should not be reimplemented in a
  Runtime; format/compatibility mechanisms are not product features.
- Before adding a game-name branch to shared orchestration, classify the
  difference. A real product difference belongs in the product catalog; a
  retail-format difference belongs in a format adapter; historical migration
  belongs in compatibility code. Otherwise prefer the shared protocol.
- Host content preparation is declaration-driven. Reuse an existing
  `PRODUCT_CONTENT[game].hostPreparation` kind for shared formats; change
  `host/lib/site-builder.mjs` only when a genuinely new format/build recipe is
  required.
- A Package mutation may outlive the JavaScript document only as durable
  recovery state, never as a long-lived phantom owner. Large `.data` downloads
  stage a per-game pending generation, so reload/crash must not force the next
  same-game launch to wait the full stale timeout. Current browsers use a
  per-game `navigator.locks` exclusive lock in addition to the IndexedDB pending
  record: browser teardown releases the Web Lock automatically, and the next
  holder may immediately reclaim a pending generation that explicitly records
  Web-Lock ownership. Pending state created by older/non-WebLock clients keeps
  the conservative timestamp/heartbeat fallback so another live tab is never
  overwritten merely because a page reloaded.

<!-- knowledge-id: K-RUNTIME-004 -->
## Cross-project generic runtime reference

When a browser runtime hosts legacy software, keep source/game semantics separate from the generic platform layer. Fix the generic API, memory, scheduler or graphics contract instead of adding a game-name, executable-name, hash or magic-offset exception. Derive memory ranges and permissions from live ownership/region state, and keep per-game persistent overlays isolated. This is cross-project reference guidance: TH08's Emscripten/Web lane does not inherit BottleShip's x86 page-table implementation.

## Superseded approaches

Blob HTML as the final carrier, hiding Runtime filesystem state inside a Service Worker, JS-only storage mounts and an unconditional HTTP 301 before migration are superseded approaches. They remain historical evidence for why the current layer boundaries exist.

## Adaptation anchors

- `docs/ADAPTING_A_GAME.md` — adapter
  workflow and Definition of Done.
- `docs/ADAPTER_CAPABILITIES.md` — feature
  classification and verification ownership.
- `src/contracts/adapter-capabilities.mts`
  — machine-readable all-game/profile/optional policy.
- `src/contracts/product-catalog.mts` —
  per-product declarations only.
- `lib/content-definition.mjs` — original
  content and Host format-preparation declarations; this is where current OGG,
  thcrap-language and prepared-content build recipes are selected.
- Canonical title repositories — Runtime-specific implementation authority;
  historical temp/migration worktrees are provenance only.

## Verification

1. Verify catalog, descriptor, installed generation and required-file list independently.
2. Exercise local import, remote fetch failure, update, reload and rollback/old-generation behavior.
3. Check root 308/nested 200, app.js, WASM MIME, managed DATA hook and package generation.
4. Check storage after reload: ordinary save/Replay, MP roots where applicable, and allowed HTTP migration data.
5. Run the affected Runtime/browser smoke; distinguish static contract, browser and device evidence.

## Deliberately omitted claims

Passing the four-game workspace gate does not by itself claim public deployment,
real-device acceptance, long-duration mobile stability or visual correctness.
Those remain browser/device/deployment evidence lanes.
