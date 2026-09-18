# Runtime Storage verification

Storage ownership is defined by [`ARCHITECTURE.md`](ARCHITECTURE.md). Game
parameters reuse the `storage` declaration in the authoritative Product
Catalog; do not maintain another game registry in tests or documentation.

## Source/protocol gate

`npm run test:storage` (also run by `npm run check`) executes every formal
`emscripten-preload` Runtime shell in isolated VMs with mocked DOM and FS
surfaces (currently TH06 and TH07). It covers the restore barrier, failure,
late success after timeout, read/write/remove operations, synchronization error
propagation, and unsafe paths. Directory/retail-memory Runtimes use their own
source/runtime gates and are covered by the browser conformance lane below.
This is module evidence and does not prove browser IndexedDB behavior.

## Browser conformance

Start the current workspace server on port 8130 with `npm start`, then explicitly provide owned fixtures at `<fixture-root>/<game>/score.dat`:

```powershell
npm run test:storage:browser -- --fixture-root D:\fixtures\saves --package th08=D:\fixtures\th08-content.zip
```

`--package GAME=PATH` is repeatable and imports an explicit content package through the existing UI. The runner never selects resources automatically from `artifacts/`. Without this option, the site must already provide game DATA. Tests use isolated browser contexts and never access the user's everyday browser saves.

The default matrix is **every currently registered Product Catalog game**
(currently TH06/TH07/TH08/TH10) × Chromium/WebKit ×
`score-only`/`commands`. The runner derives that game set from the Product
Catalog instead of carrying a second hard-coded registry. The latter case adds
an independent flat probe covering list/read/write/remove/sync behavior, path
rejection, and persistent deletion. Both cases cover orderly shutdown,
full-page reload, destruction of temporary Runtime exports, restoration, and
exact byte comparison. They do not require a cold start to create a score or
configuration file automatically.

`--cases nested-write` is an independent extended case that creates `probe/nested.dat`; its report does not replace `score-only`. `--cases restore-failure` uses a Playwright new-document initialization script to install a `Module` setter in the expected Runtime document and make the first `FS.syncfs(populate)` call fail, without changing product source. The Runtime must emit an error and must not reach ready/first-frame. Neither extended case runs by default. Existing game-specific scripts remain only as historical investigation entrypoints; future generic storage verification uses this runner.

Output defaults to `artifacts/validation/runtime-storage/<timestamp>/` and includes per-case JSON, exported files, and failure screenshots. A failed case exits nonzero. Console stderr is retained separately; only `pageerror` or Runtime error/fatal events constitute runtime-failure evidence. Rerun after browser or build updates. Playwright WebKit is not evidence from a real iOS device.

Each release candidate must rerun its required matrix and retain the report in that candidate's external acceptance evidence. Repository documentation does not retain dated PASS claims from a particular machine, and it never presents Playwright WebKit as proof from real Safari hardware.
