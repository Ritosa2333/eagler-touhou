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
No new persistent data store or migration format is introduced.

Installation is NOT an offline game-readiness signal. The dialog distinguishes
cached Launcher/Runtime components from game, music and font packages. Package
Store remains the authority for game resources and saves. Multiplayer and
resource downloads remain online features. Export saves; neither installation
nor a successful `persist()` request protects against manual data removal.

## Update contract

`src/app-shell-sw.js` remains the only Launcher/App Runtime caching authority.
`app-shell-client.mts` owns registration; optional `pwa.mts` UI does not register
a second worker or await Service Worker/storage promises during boot.

- A candidate does NOT call `skipWaiting()` or `clients.claim()`. Existing
  windows keep their controlling worker. Close all site/app windows to apply
  an update; refreshing one window is not a safe update action. A first visit
  remains network-controlled until its next navigation. The client's `ready`
  waits for first activation (bounded, without waiting for control), so normal
  Runtime preparation does not treat registration alone as successful activation.
- A waiting candidate is distinct from an activated replacement. The client
  must not schedule reloads while waiting, and rechecks Launcher activity in
  the scheduled reload task for externally activated/legacy replacements.
- Initial visits defer Runtime downloads. A replacement prepares all published
  App-owned Runtime entries before completing installation. Preparing only
  the previously used games is insufficient: a user can first install another
  game while the candidate is waiting. Unchanged verified files are reused;
  original game data/music packages are never pulled into the App Shell.
- Workbox's manifest revisions use SHA-256 for both mapped and globbed files.
  Network responses must match before caching. A 200 HTML fallback, wrong build,
  failed fetch or quota failure cannot be committed as a successful candidate.
  The initial transition refetches unverified legacy cache bytes.
- Cache names include the mount path. Legacy cache ownership is checked by the
  exact scoped metadata URL. Cleanup retains the current and previous scoped
  generations, not other same-origin deployments, Package Store or saves.
- Failed installation drains its concurrent writers before removing only its
  candidate cache. A failed update leaves the existing worker available.

Keep release publication atomic and retain the existing deployment verifier.
Integrity validation detects bad publication; it is not a substitute for it.

## Reproducible browser lane

The default repository gate still owns the Node behavior tests. The explicit
`.github/workflows/pwa-browsers.yml` lane runs Playwright Chromium, WebKit and
Firefox with the locked browser requirements:

```sh
npm ci --ignore-scripts
python -m pip install -r tests/requirements-browser.txt
python -m playwright install --with-deps chromium webkit firefox
npm run build:launcher
npm run build:content
python tests/browser/test-pwa-boot.py --browser=webkit
```

Repeat with `chromium` and `firefox`. The fixture builds the real published
Launcher and production worker. It covers root/nested and localized/query
entries, restricted optional APIs, offline reload, HTTP-503 server outage,
fresh-process offline startup, multi-window waiting, a corrupt candidate,
scoped-cache isolation and an iframe -> ESM -> WASM dependency added by an
update. Tests must observe behavior, not merely find strings in source.

The small WASM module is explicitly synthetic. Passing this lane does NOT prove
TH06/TH07/TH08/TH10 gameplay, physical iOS Home Screen installation, platform
storage sharing, touch, audio resume or storage-pressure behavior. Do not label
Playwright WebKit as an iPhone/iPad test.

### Offline injection and the WebKit driver

An independent literal-response Service Worker probes offline emulation before
any application code is loaded. The currently reported Playwright WebKit issue
<https://github.com/microsoft/playwright/issues/42775> rejects even that response
with an internal error under `setOffline(true)`. Only if this exact failure is
reproduced does the lane switch to dropping origin TCP connections without an
HTTP response. It keeps every application boot/update assertion and verifies
that a non-cached negative-control fetch really fails. The JSON log records the
chosen fault-injection method; no app failure triggers this fallback.

Origin-connection failure is not identical to airplane mode or
`navigator.onLine === false`. A green WebKit lane using this mode proves cached
startup with the origin unreachable, not WebKit device-offline emulation or a
physical iPhone's offline behavior. Chromium/Firefox keep native emulation when
the independent probe succeeds. Physical-device acceptance remains required.

## Release acceptance beyond this lane

Use a complete verified deployment and legally held game packages. On actual
Android Chrome/Edge and iOS/iPadOS Safari Home Screen, test installing each of
the four games through the normal UI, first launch offline, selected music and
fonts, process termination/relaunch, background/audio resume and interrupted
installation/update. Also test macOS Safari, Windows Chromium and Firefox.
The generic repository does not contain the originals needed for that gate.

Useful platform references:
- https://web.dev/articles/service-worker-lifecycle
- https://developer.mozilla.org/en-US/docs/Web/Progressive_web_apps/Guides/Making_PWAs_installable
- https://webkit.org/blog/14403/updates-to-storage-policy/
- https://webkit.org/blog/14445/webkit-features-in-safari-17-0/
