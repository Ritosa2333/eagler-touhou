# PWA installation, updates and browser boot

## Product boundary

The Launcher has a relative, stable Web App Manifest identity and scope, PNG
192/512 icons, a separately declared maskable icon and an Apple touch icon.
The ET monogram assets in `public/assets/pwa/` are original project artwork,
not extracted game content; they use the repository license. `icon.svg` is the
vector source. All icons and PWA UI resources belong to the frontend delivery
and App Shell manifests, including Import deployments.

The masthead's **Install / offline** entry provides a one-shot, user-initiated
native install prompt where available, otherwise browser installation guidance.
It also explains separate browser/app storage, requests persistent storage only
on an explicit click and reports unavailable/denied permissions without blocking
Launcher initialization. Its Chinese/English copy follows the document locale.
Package Store formats, installed original-game data and saves are unchanged.

Installation is NOT an offline game-readiness signal. The dialog distinguishes
cached Launcher/Runtime components from game, music and font packages. Package
Store remains the authority for game resources and saves. Multiplayer and
resource downloads remain online features. Export saves; neither installation
nor a successful `persist()` request protects against manual data removal.

## Launcher update contract

`app-shell-sw.js` remains the only registered caching worker. The build combines
`src/app-shell-sw.js` and `src/runtime-cache-sw.js` into that artifact.
`app-shell-client.mts` owns registration; optional `pwa.mts` UI does not register
a second worker or await Service Worker/storage promises during boot.

- A Launcher candidate does NOT call `skipWaiting()` or `clients.claim()`.
  Existing windows keep their controlling worker. Closing all site/app windows
  applies the Launcher update. This does NOT delay a new game's Runtime update:
  Runtime selection below runs on every launch independently of SW activation.
- A first visit remains uncontrolled until its next navigation. The client's
  `ready` waits for first activation (bounded, without waiting for control), so
  Runtime preparation does not mistake registration for successful activation.
- A waiting candidate is distinct from an activated replacement. The client
  must not schedule reloads while waiting, and rechecks Launcher activity in
  the scheduled reload task for externally activated/legacy replacements.
- First installation and every replacement cache only the Launcher shell.
  Deferred Runtime entries remain deferred, even for previously used games;
  updates do not download or copy all Runtime bytes. The manifest may describe
  all published groups, but only explicit preparation or launch of a selected
  Runtime prepares its complete set. An unrelated missing/broken Runtime must
  not block a Launcher update. Original game data/music stay in Package Store.
- SHA-256 identifies actual bytes, not just cache labels. An HTTP 200 HTML
  fallback, wrong build, failed fetch or quota failure cannot be committed as a
  successful candidate. Failed installation drains writers before cleanup.
- Activation does not delete old shell caches: it can occur during browser
  shutdown, before the replacement registration is persisted. Cleanup follows
  a live client's update-status request, retains the previous actually used
  shell and leaves newer/waiting candidates alone. It is not a boot dependency.
- Cache ownership is mount-scoped. Other deployments, Package Store and saves
  are never cleanup targets.

## Runtime: latest on this launch, automatic usable rollback

Public HTML, shell, glue and Wasm URLs remain unchanged. Deployers do not manage
version directories, and the user does not choose or enter a version ID.

On every Runtime entry navigation, the worker immediately reads a fresh catalog
from a JSON-only comment in the current `app-shell-sw.js` response. It uses
`cache: no-store`, bounded fetches, schema/path validation and `JSON.parse` only;
it does not evaluate downloaded worker source or add another public endpoint.
The catalog is emitted from the same build's SHA-256 manifest.

The entire SELECTED Runtime set is selected before returning HTML or executing
glue. Opening TH08 does not prepare TH06, TH07 or TH10:

1. Try the just-published set now, reusing only bytes that match its hashes.
   Fetch missing files into a private candidate cache with two concurrent writers.
   Mark it complete only after every declared dependency has been verified.
2. If that attempt fails, find a complete verified local set and launch it.
   Never replace one missing Wasm independently under older executing glue.
   For compatibility, also consider a complete set left in an older App Shell
   cache; do not let an older embedded catalog undo a more recent successful
   launch. This legacy-read path does not schedule any all-game prewarming.
3. With no complete snapshot, try rebuilding the embedded known set from legacy
   cache bytes, checking actual hashes rather than old `runtimeTrusted` labels.
   A fresh network-catalog retry covers publication completing mid-attempt.

A Launcher-only update does not imply that any game's Runtime was updated. If
the user goes offline before opening that game online, its previous complete
snapshot remains the fallback. A never-prepared game is not offline-ready merely
because the Launcher updated. When connectivity returns, the next launch tries
the latest selected Runtime immediately and prepares its new dependencies then.

The last observed server release is preferred on later offline starts, including
an operator rollback that reuses an older snapshot. Every subsequent online
launch still immediately tries the currently published release.

Private CacheStorage metadata pins each browser client to its selected complete
snapshot before delivering its entry document. Subsequent ESM imports, Wasm
requests and dedicated workers retain that pin, even when another tab starts a
newer Runtime. Pins survive worker-process termination. Delivered Runtime
responses use `Cache-Control: no-store` so normal HTTP/memory caches cannot
alias different clients' resources merely because the public URL is identical.

Garbage collection retains the two latest complete Runtime snapshots plus live
and in-flight client pins. An uncommitted candidate is never allowed to delete
the usable fallback. This does use extra storage for recovery; it never clears
game packages or saves to make room for an update.

A complete set means verified build coherence, not proof that a new game engine
has no gameplay bugs. If all usable local copies have been manually removed or
evicted and the server is also unavailable, no missing program can be restored
from nothing. This is not treated as a successful offline-readiness state.
The first transition from a previously deployed legacy worker requires the new
worker code to become active; code already executing in an old client cannot be
retroactively repaired. No website-data clearing is part of this update flow.

Keep release publication atomic and retain the existing deployment verifier.
Integrity validation and rollback do not replace a coherent server release.

## Reproducible browser lane

`.github/workflows/pwa-browsers.yml` runs Chromium, WebKit and Firefox with the
locked browser requirements. Both browser suites and all assertions are required:

```sh
npm ci --ignore-scripts
python -m pip install -r tests/requirements-browser.txt
python -m playwright install --with-deps chromium webkit firefox
npm run build:launcher
npm run build:content
node tests/runtime-cache-cases.mjs
node tests/shell-handoff-cases.mjs
python tests/browser/runtime-recovery-cases.py --browser=webkit
python tests/browser/test-pwa-boot.py --browser=webkit
```

Repeat with `chromium` and `firefox`. The real published Launcher and production
SW surround small original-resource-free Runtimes. Their actual Wasm binaries
call different EM_ASM-like addresses, and mismatched glue/Wasm really throws.
Coverage requires successful execution after latest-first update, corrupt-new
fallback, offline fallback, partial-new-cache rollback, network restoration,
multi-tab/late-import/module-worker isolation and fresh-process offline startup.
Node cases additionally cover timeouts, quota failure, persisted pins, poisoned
legacy cache migration, server rollback ordering and shutdown cache retention.

The existing suite also covers root/nested and localized/query entries, optional
APIs being restricted, icons, multi-window Launcher waiting, HTTP 503, a corrupt
shell candidate and scoped-cache isolation. Server-side request recording proves
that shell install/update/activation fetch no Runtime bytes, even with a broken
unselected Runtime, and that preparing/launching one group never fetches another.
It executes the old complete Runtime offline after a shell-only update, then
updates only the selected Runtime online and executes its new dependencies
offline. Corrupt-Runtime fallback remains in the separate recovery suite.
Passing these tests does NOT prove real TH06/TH07/TH08/TH10 gameplay, physical
iOS installation, touch or audio resume.

### Offline injection and the WebKit driver

An independent literal-response Service Worker probes offline emulation before
application code loads. Only when it reproduces the specific internal WebKit
error reported at <https://github.com/microsoft/playwright/issues/42775> does the
lane replace `setOffline(true)` with origin TCP connection drops. An application
failure never triggers this fallback. No application assertions are removed.

Both suites additionally drop origin connections on every engine and do so
before starting a cold browser process. Page-only offline flags must not
accidentally permit the SW's own network process to reach the server.
A non-cached negative-control request must also fail. Logs record fault injection.
Origin unavailability does not prove physical airplane-mode behavior.

## Release acceptance beyond this lane

Use a complete verified deployment and legally held game packages. On actual
Android Chrome/Edge and iOS/iPadOS Safari Home Screen, test installing each of
the four games through the normal UI, first launch offline, selected music and
fonts, process termination/relaunch, background/audio resume and interrupted
installation/update. Also test macOS Safari, Windows Chromium and Firefox.
Do not call Playwright WebKit an iPhone/iPad test or mark these checks complete
solely because a synthetic Runtime executes successfully.

Useful platform references:
- https://web.dev/articles/service-worker-lifecycle
- https://developer.mozilla.org/en-US/docs/Web/Progressive_web_apps/Guides/Making_PWAs_installable
- https://webkit.org/blog/14403/updates-to-storage-policy/
- https://webkit.org/blog/14445/webkit-features-in-safari-17-0/
