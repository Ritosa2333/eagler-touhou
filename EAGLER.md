# TH15 Eagler adapter

This is the canonical `eagler` adapter checkout. The sibling `th15` checkout
tracks upstream `main`; it is not a Runtime source/build owner.
Both branches started at `fe113b6f9cbcb676b15dfd95cfd5916c4d5de95e`.

Read the workspace `WORKSPACE.md` and the shared
`eagler-touhou/docs/playbooks/runtime-launcher-package.md` before changes.
High-refresh, Multiplayer, THPrac and THCRAP require their own experiment lanes
under the shared adaptation-worktrees playbook.

## Build and package

Use the original pinned Emscripten 6.0.9 SDK. `TH15_EMSDK` may point to an external
SDK; `EM_CACHE` may point to a writable cache. Python and Node must be on PATH.

```powershell
node th15_web/scripts/build-sdl-application.mjs --release
$env:EAGLER_FONT_ROOT = '<private measured TH15 fonts directory>'
node portable/package-eagler.mjs
```

The result is `build-eagler/`. The packager verifies source and Wasm identities,
rejects diagnostic exports, and writes the closed Runtime inventory. The SDK,
original content, build results and measured binary font inputs are ignored.

`th15_web/sdl-runtime/shell.mjs` implements the shared Launcher protocol.
`managed.mjs` and `package-release.mjs` remain the upstream standalone delivery
path; they are not the shared Launcher's package entry.

The generated `directory-keyboard.mjs` comes from the MIT-licensed
`eagler-common` browser keyboard owner. Its license is retained at
`portable/browser/LICENSE.eagler-common`. Regenerate/check it with
`eagler-common/tools/sync-directory-keyboard.mjs`, not by editing generated code.

## Shared Launcher

From `eagler-touhou`, prepare content with:

```powershell
node scripts/prepare-th15-content.mjs --original=<Japanese-1.00b-directory> --output=<prepared-content>
$env:EAGLER_DEVELOPMENT_GAMES = 'th15'
$env:EAGLER_TH15_CONTENT_DIR = '<prepared-content>'
node scripts/serve.mjs 18115
```

Open `http://127.0.0.1:18115/?test=th15`. A host declaring `shared.testBuild: true`
shows TH15 without a query parameter, in normal product order. Other hosts
require the `test` parameter. TH20 follows the same test-only visibility policy.
This visibility rule is not an access-control boundary.

DATA is the unmodified Japanese 1.00b `th15.dat`, mounted at `/th15.dat` through
the shared Package Store. Its identity is 77,453,661 bytes,
SHA-256 `f3802e71db30f99b86d61b6e98b8325716c5cbae892547c563dded7837862ae7`.
Ten measured font tables are immutable Runtime resources. The 18 OGG tracks
mount under `/music`; no-music mode needs no OGG and retains game sound effects.
The font and original-content inputs remain private build resources.

IDBFS owns `/savesth15`: `scoreth15.dat`, `th15.cfg`, original-compatible Replay
files, and Pointdevice checkpoint files under `autosave`. `/save` is an alias of
that save root, never of Package content. The shell validates imported game files.

## High-refresh presentation

The canonical `eagler` branch implements fixed-60-Hz logic with display-paced
immutable Draw transaction interpolation. The experiment branch preserves its
implementation and validation history. See
[implementation and evidence](th15_web/docs/EAGLER-HIGH-REFRESH-IMPLEMENTATION.md).
Full Presentation Lab admission and physical-phone acceptance remain incomplete.

## Replay verification

The title adapts the common verifier with all three stored Demo replays and
Marisa Legacy Lunatic and Extra clears. Original-derived golden traces cover
225,582 ticks of the declared gameplay fields in the WASI diagnostic core.
See the [commands and scope](tools/replay-verifier/README.md) and
[acceptance record](tools/replay-verifier/RESULTS.md). Browser presentation and
full Presentation Lab coverage remain separate acceptance gates.

## Scope and verification

This is a basic test adapter, not a claim that all mandatory adapter obligations
are complete. Multiplayer, THPrac and downloadable languages are not advertised. Standardized physical-gamepad semantics, always-hitbox, complete touch
conformance remain follow-up work. The original logic stays at 60 Hz.

A common Replay verifier candidate consistency check is available in
[Replay verification](th15_web/docs/EAGLER-REPLAY-VERIFICATION.md). Its limited
state coverage and completion-menu checks do not replace original golden traces.

The shared repository owns `test:th15-launch:browser` and
`test:test-build-cards:browser`. The former requires a running local host (default
port 18115), real private content and `EAGLER_CHROME_PATH` if Chromium is not
auto-detected. It exercises real title/gameplay, keyboard, simulated touch,
OGG/no-music, pause/return/exit, and persisted configuration after reload.
`EAGLER_TH15_TEST_URL` selects a packaged site. Set
`EAGLER_TH15_TEST_OFFLINE=1` for offline reload, or
`EAGLER_TH15_TEST_IMPORT=<package.zip>` to import while blocking hosted TH15 DATA.
These are desktop Chromium tests, not physical-phone acceptance.

The 2026-10-06 packaged-site acceptance passed desktop OGG and emulated touch
without music, native gameplay/pause/title/exit, persisted configuration after
offline reload, and ZIP import with hosted game DATA blocked. Default-hidden
and test-query-visible card checks also passed. Workspace evidence is retained
in `artifacts/th15-adapter-verification.json` and its linked browser reports.

Loading presentation has a one-second minimum at startup, game entry and stage
transitions. Resource preparation counts toward that minimum; the browser waits
without running input/Replay ticks or accumulating simulation debt. Help-page
resource requests keep their existing behavior. This is a presentation policy,
not a measured claim about the retail executable's exact loading duration.
