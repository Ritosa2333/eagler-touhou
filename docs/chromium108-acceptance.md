# Chromium 108 compatibility acceptance

Date: 2026-10-12. Source: `542209b` plus the explicit Vite `chrome108`
target. Browser: Playwright Chromium revision 1033, version 108.0.5359.29,
Windows. Origin: `http://127.0.0.1:18736`. Evidence: browser automation,
not public deployment or manual device acceptance.

## Results

- The homepage, English homepage, About, FAQ and lobby render offline and
  survive reload with the HTTP cache disabled and the Service Worker active.
- TH06 installs through Launcher and reaches the native first-frame event.
  After network and HTTP cache are disabled, reload and launch reach another
  native first-frame event using the managed installation.
- A DEFLATE Package ZIP containing the real TH06 DATA and shared fonts passes
  the canonical parser, integrity checks and IndexedDB installation. The
  canonical published-package updater then commits a different descriptor
  revision with linked cancellation signals. This is a package-owner browser
  test; it does not claim coverage of the file-picker UI or update dialog.
- The frontend build and TypeScript gate pass with the explicit Chrome 108
  JavaScript target. The existing cancellation compatibility change avoids
  `AbortSignal.any`; the actual old browser has no such API.

## Test setup and limits

The isolated fixture and scripts are in `.cache/offline-compat-20261012`:
`prepare.mjs`, `prepare-probe.mjs`, `test-shell-offline.mjs`,
`test-offline-game.mjs` and `test-package-108.mjs`. JSON reports and browser
logs are retained there. The native runtime is the published TH06 normal
generation `54e342548d92edceb12bad364ae342e480733566bcd1db83a9fa5572e56762b3`.

Use the contemporaneous Puppeteer 19.3 driver and `headless: 'chrome'` for
Chromium 108. Its legacy headless mode stalls automatic fullscreen. Store
fresh browser profiles on a disk with adequate free space: large response
Blobs failed with the nearly full system-disk profile and succeeded with a
fresh D-drive profile, without changing product code.

The target is explicit because Vite's current default starts at Chrome 111
while bootstrap admits Chrome 108. It bounds generated JavaScript syntax;
it does not polyfill browser APIs. CSS enhancements remain unchanged. This
acceptance does not cover every title, extended gameplay, audio, multiplayer,
the imported-package UI, or public-site deployment.
