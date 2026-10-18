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

Service Worker caches and updates the App Shell. Normal Launcher builds enable it by default on their own origin and mount. Only an explicit `EAGLER_REACT_APP_SHELL=disabled` build omits offline document loading. Exact-origin isolated preview builds remain supported. It does not own the Runtime filesystem or hide a package Runtime inside SW state. The Runtime uses same-origin HTML/JS/WASM and the App/Launcher manages DATA.

An App Shell replacement first verifies and caches its complete required Launcher set. An idle Launcher then requests activation and reloads itself; an active game or operation defers that page's reload until it finishes. Other old tabs and Runtime iframes do not block activation. Verify both the reloaded current page and a newly opened page can load the new shell and launch a Runtime while an old page remains open.

Resource delivery has three modes. `hosted` publishes game/shared payloads with the site; `external` keeps Launcher and Runtime local while retaining Release Catalog and Package Descriptors for automatic acquisition through same-origin redirect routes; `import` requires a user-selected local Package. External storage topology does not enter Package Descriptor URLs: descriptors retain safe relative sources, and the web-server/CDN layer owns redirects and CORS.

The static `src/contracts/product-catalog.mts` module is the product registry.
The browser-visible `site-manifest.json` describes the deployed Runtime/content
surface, while `release-catalog.json` points at installable Package
Descriptors. There is no third `games.json` protocol. Local import stays
available even when remote fetch fails. Missing files may be non-fatal during
descriptor inspection, but the start gate must complete required files before
calling the Runtime. Package update replaces the installed state through
descriptor/generation atomicity; a partially updated package must not become
current.

Keep ordinary and MP storage roots separate. Keep DATA separate from Replay/save. `runtimeReady=true` means required Runtime dependencies are prepared; it is not a game-function, browser or device PASS. Record local provenance so an old package cannot silently pose as a new one.

### Installed launches and background updates

Ordinary installed DATA and complete verified Runtime programs launch locally
before any online update check. `prepareRuntimeLaunch` first asks the cache
authority for `localOnly` preparation. Main's `runtime-program-sw.js` is scoped
to `runtime/`; Launcher documents remain outside its control. Isolated App
Shell deployments expose the same Runtime cache protocol. Both use the canonical
whole-generation cache and its byte/hash verification. Neither worker owns
mutable saves, imported DATA, shared fonts or OGG.

After the native launch acknowledgement, compare Runtime Manifest and Package
revision and download changed generations in the background. Announce an update
only after the complete successor has been downloaded, verified and committed.
An identical manifest, failed request, incomplete download or failed verification
produces no update notice. Keep the running document and DATA generation pinned;
the downloaded successor is used on a subsequent launch. A completed update
opens the existing decision dialog with **Restart now** and **Use on next
launch**. Focus defaults to the latter; Escape makes the same choice. Restart
uses native save/close and the existing launch owner, never a page reload. Save
failure retains Retry/Leave/Stay handling. Program and DATA completion share one
prompt for the exact product/version/Runtime epoch; closed or replaced games
cannot be restarted by a stale notification. Existing dialogs and operations
finish before the update prompt appears. Live room matches defer it until the
room is back in its lobby; lobby restart reuses the room launch owner.
Canonical Package Store
serialization checks the expected generation when claiming staging and again in
the IndexedDB transaction that switches current. Web Locks reduce contention;
the durable condition also protects browsers without them. Startup and background
OGG acquisition and required-font upgrades carry the installation generation they
intend to extend, so an older Runtime cannot replace a newer import from another tab.
Only the current launched Runtime epoch may consume an update-notification key;
a retired program check cannot suppress its successor's prompt.

### Required resources before native DATA identity

Complete required Package resources before the Runtime pins its DATA generation.
`prepareGame` and `prepareGameCheck` resolve legacy shared-dependency upgrades
before native preparation. Never replace the Package descriptor or base-file
identity in a native-ready continuation. Ready-time optional resource additions
must remain within the already selected descriptor and preserve loaded base
objects. A different base publication needs a new preparation/session.

This is a Launcher lifecycle invariant, not a TH11-specific workaround. Current
`requiredShared` products are TH08, TH09, TH10, TH11, TH15 and TH20. TH06/TH07 do
not currently use that upgrade branch. Complete installations retain local
startup without a metadata wait; an incomplete old installation needs its
missing dependency before gameplay. File-only access keeps its separate path.
Runtime program prefetch can overlap acquisition, but must not bind an older
DATA/Package identity before required resources are resolved.

`tests/test-runtime-shared-font-upgrade-browser.mjs` covers actual IndexedDB
upgrade and preparation order for those six products using synthetic Packages
and a substituted native endpoint. It verifies existing DATA/language/OGG object
reuse and no repeated metadata read once complete. Native TH11 old-Package
startup and complete-install offline reload were separately verified; the six
synthetic cases do not establish native startup for every title.

Live Multiplayer is an exception to offline-first program selection. Room
preparation and each ready intent fetch the Runtime Manifest and finish the
exact current MP generation before reporting readiness, including deployments
without an App Shell worker. A failed manifest/current download cannot select
an older complete program. The room advertises program-version support and
records each ready player's generation. Different or missing identities cannot
join a versioned start. The relay pins that generation to the run and verifies
it again on signaling and gameplay connections. The Launcher carries the room
agreement into the initial Runtime plan, before native ready/configure, and
checks that the publication still matches it before opening the game. A release
between readiness and launch therefore blocks startup and requires renewed
room preparation, rather than mixing programs. The short **Check game** path
also verifies the current MP program synchronously. Local MP Replay/file access
retains offline selection. Room popup deferral does not skip these checks.

Deploy the frontend and relay together. A new versioned client refuses readiness
on an old relay without this capability; a legacy client with no program
identity cannot mix with a versioned ready player. Entirely unversioned legacy
or development rooms retain their existing protocol. This is immutable program
agreement between supported clients, not attestation against modified clients.

Missing/corrupt local programs retain whole-generation recovery and verified
HTTP fallback; first installation still needs its required resources. The
runtime-only worker does not provide an offline Launcher document reload.

Program download accounting follows body consumption, not response headers.
Native stream readers use `consumeNetworkBody` to keep the existing document
tracker active through EOF, cancellation or failure, without rebuilding the
browser's stream. The reader retains its inactivity deadline. Worker preparation
can opt into additive `RUNTIME_GENERATION_PROGRESS` messages on its existing
reply port; only the final preparation reply acknowledges a complete program.
Coalesced preparations receive the same byte progress, and a retired Runtime's
callbacks cannot revive its network tasks. This is a shared Launcher invariant
for ordinary and MP products, not a TH10-specific path.

Verified HTTP fallback retains only program bytes until it can transfer them
to a compatible worker through `IMPORT_RUNTIME_GENERATION`. A worker that
became ready during verification rechecks the complete set and atomically
commits the supplied bytes before iframe navigation. This avoids depending on
cross-context HTTP-cache reuse. The temporary buffers are transferred and
released after handoff. No DATA, music or mutable saves enter this channel.

Without a compatible worker or writable storage, ordinary HTTP startup remains
available. The verifier uses `reload` for immutable program files, allowing
best-effort reuse by the browser or worker. Workers still verify each file;
invalid HTTP-cache bytes get one fresh `reload` attempt. Mutable manifests
remain `no-store`. Whole-generation verification, atomic commit and current
MP generation checks remain mandatory. The real browser test
`tests/test-runtime-download-browser.mjs` covers held native streams, iframe
and late-worker reuse, SW byte progress and corrupt HTTP-cache repair, with
`AbortSignal.any` absent. It uses synthetic program files, not native gameplay.

Validate with `tests/test-runtime-offline-first-browser.mjs`,
`tests/test-runtime-launch.mjs`, `tests/runtime-cache-cases.mjs` and the
preparation/runtime service tests. `tests/test-game-update-dialog-browser.mjs`
checks the real shared dialog in Chromium, Firefox and desktop WebKit. The
offline-first browser fixture accepts `--engine=chromium|firefox|webkit`; it uses
an independent literal-response worker probe before permitting origin connection
drops for WebKit's known offline-emulation issue
([upstream #42775](https://github.com/microsoft/playwright/issues/42775)).
Synthetic browser fixtures establish cache
and network behavior; native title and device acceptance remain separate.
Use `--multiplayer` on the offline-first browser fixture for the separate MP
root and foreground failure cases. `tests/test-relay-runtime-version.mjs`
verifies real local relay readiness/start and signaling/gameplay gates without
running spectator scenarios.

Source diagnostics such as Presentation Lab use an isolated build/package
profile. Instrumented WASM, `audit_*` exports, diagnostic workbench files and
evidence reports are not Runtime Release or Package Store inputs and never add
commands to `eagler-touhou/1`.

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

## TH09 independent Replay browsing

TH09's Replay workspace lists and downloads persisted files through
`app/services/replay-storage.ts`, a read-only compatibility adapter for the
existing IDBFS v21 `FILE_DATA` store. It derives the ordinary root from the
Product Catalog and preserves the Runtime's separate MP root. Missing databases
are not created; unsupported schemas fail without migration or native fallback.

Opening the workspace starts the persistent viewer and independently reads the
file list in parallel. Browsing and downloading must not wait for Runtime, DATA,
language or music readiness, and remain available after a loading failure.
Watch joins an in-flight preparation; subsequent files and backward seeks reuse
that viewer. Native writes requested during preparation wait for the same owner.
A matching already-owned Runtime must finish its normal
save/sync before an independent read. This handshake concerns the current
document's Runtime, not unsaved memory held by another browser tab.

Native Runtime commands remain the only writer for import, rename, move and
delete. The reader preserves Replay bytes, THMOTION export extensions and
validated MP sidecars. ZIP code is available with the page so an offline folder
download does not depend on fetching a module at the moment of download.

Verify file listing/download while game readiness is withheld, automatic player
preparation, ordinary/MP root isolation, offline download and byte equality,
save ACK before read, queued Watch,
hot file switch, and one-click return. Source gates are
`tests/react-main/replay-storage.test.mjs`, `replays-model.test.mjs`, and the
Replay cases in `browser-session-mounted.test.mjs`.

Replay fullscreen uses the permanent `#player` workspace owner, so wide screens
retain the file browser and its menus beside the game. Portrait fullscreen uses
the game and transport only. Keep the native iframe in place and preserve a
visible upper-right fullscreen exit control in either layout.

The Replay workspace mounts playback controls without the normal gameplay input
owner, touch HUD/utilities or runtime diagnostics. Keep that boundary through
preparation, launch ACK and file playback. Its iframe does not receive pointer
input, so the native canvas cannot consume vertical page swipes. Verify phone
scrolling with Touch enabled after the game is ready, as well as file actions
and the workspace's own fullscreen controls. Ordinary play and live spectating
retain their existing input owners.

## Verification

### Shared local Replay workspace

Verify source ownership before building or testing Replay changes. TH06/TH07
ordinary and MP builds are separate artifacts: inspect each CMake multiplayer
flag and test both. For TH08 and later, inspect the actual `eagler` and
`eagler-mp` owners and propagate shared changes to both. An ordinary build made
with MP disabled in the MP checkout does not establish ordinary-branch coverage.
The native browser checks bind each result to its branch, WASM identity and
declared Replay source hashes; MP recording checks accept an explicit `--repo`
and reject an ordinary checkout. Keep prior named-artifact evidence separate
from a new validation of the latest branch HEAD.

The non-versus workspace uses the title's native decoder for name, date,
loadout, difficulty, score units, slowdown, special modes and recorded stage
starts. Selecting a file expands its real stage/score list. Only selecting an
available native stage starts playback. TH09 keeps its matchup/round flow.
Keep values in aligned columns without repeated NAME/PLAYER labels. Check the
actual desktop and portrait pages: a file-row `small` rule previously overrode
the summary layout and concatenated otherwise separate React spans.

The shared browser controller owns pacing, pause and seek command ordering.
Native title adapters own input consumption, stage bootstrap and EOF. Backward
seeks reopen the selected native stage in the same viewer/IDBFS owner. Wait for
native menu readiness, rather than a startup loading picture's first-frame
event, before retiring the original loop. A paused seek to zero must finish
native initialization before clearing its seek target. MP stages may have a
nonzero native input offset; keep transport time relative to that start.

Seek reconstruction uses bounded message tasks instead of waiting for another
display refresh after every batch. Keep each native fixed tick and authored
Draw, including offscreen rendering and capture requests. TH06/TH07 optionally
coalesce only backend window swaps through `eagler_replay_present`; do not skip
their native `GameWindow::Present`, which also owns capture and game state.
Present the completed target before clearing the seek status, preserving the
user's pause/rate. Hidden viewers yield and closing cancels queued work.
Measure the actual browser/device: software GPU rendering can dominate even
when JavaScript/native execution no longer waits for RAF.

TH06/TH07 external native file reads require the full absolute preference path.
Their workspace adapter must also stop at the native recorded-input boundary:
a partial run can exhaust its recording while gameplay still remains active.
Waiting only for a return to the menu can keep advancing beyond saved input.
TH08 ordinary caches Replay resources at boot, so direct playback must replace
the cached bytes from the current FS file after import/overwrite. TH08 ordinary
does not declare a total input length: display an unknown duration and disable
arbitrary position seeking rather than inventing a duration. Stage selection,
pause, rate and restart remain available. Preserve stage branches and Spell
Practice metadata through the native decoder, including CP932 spell names.

The common bridge source is `eagler-common/browser/local-replay*.mjs`. Inline
carriers are generated by `tools/sync-local-replay.mjs`; `--check` rejects stale
blocks. Use zero-filled byte buffers for C-string terminators because HTML
optimization may materialize a string's NUL escape as an invalid HTML byte.

See `docs/history-session/2026-10-17-local-replay-workspace.md` for per-title
browser evidence and its corpus/coverage limits.
These control/storage checks do not establish retail playback determinism.

1. Verify catalog, descriptor, installed generation and required-file list independently.
2. Exercise local import, remote fetch failure, update, reload and rollback/old-generation behavior.
3. Check root 308/nested 200, app.js, WASM MIME, managed DATA hook and package generation.
4. Check storage after reload: ordinary save/Replay, MP roots where applicable, and allowed HTTP migration data.
5. Run the affected Runtime/browser smoke; distinguish static contract, browser and device evidence.

## Deliberately omitted claims

Passing the four-game workspace gate does not by itself claim public deployment,
real-device acceptance, long-duration mobile stability or visual correctness.
Those remain browser/device/deployment evidence lanes.

## Windows 本地发布检查的边界

`lib/static-deployment.mjs` 的原子发布依赖 POSIX 目录符号链接替换。Windows 在创建目录或修改 current 之前返回 `EAGLER_ATOMIC_DEPLOYMENT_UNSUPPORTED_PLATFORM`，不把它降级为逐文件复制。对应 Windows 门禁验证提前拒绝及无写入，POSIX 门禁仍验证真正的原子替换；Windows 的 PASS 不能表示验证过 Linux 发布。`refresh-deployment-app-shell.mjs` 的预备目录刷新另有有界共享锁重试、整目录候选验证和失败回滚，它不解除 live publication 的平台限制。
