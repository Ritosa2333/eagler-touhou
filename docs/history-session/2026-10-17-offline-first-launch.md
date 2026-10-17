# Installed launches use local resources first

Target: existing main checkout, `worktrees/launcher-th09-spectator-main`.

- Installed launches no longer ask about an update before downloading it.
- Complete Runtime programs are prepared from CacheStorage before requesting an
  online manifest. Main publishes a runtime-only worker scoped to `runtime/`.
- Native launch acknowledgement starts the background checks. Only a changed,
  fully downloaded and verified program/Package generation produces a notice.
- Active programs and DATA keep their pinned generation. The next launch may
  use the completed successor; failures leave the old installation usable.
- Completion uses the shared two-button dialog: Restart now / Use on next
  launch. Restart follows native save/close and the normal launch owner. Save
  failure can stay in the old game; stale product/version/epoch actions cancel.
- Existing dialogs, busy file/launch operations and live multiplayer matches
  defer the update prompt. Code and DATA completion coalesce per running game.
- Package background mutations fence their expected generation inside the
  canonical queue so later imports are preserved.

This is a shared Launcher rule for all registered products, recorded in the
Runtime/Launcher/Package playbook. No per-title cache/update implementation was
added. Runtime-only caching does not cache the Launcher document, shared fonts,
DATA, OGG, saves or Replays.

Validation artifacts are under `artifacts/validation/offline-first-tests.log`,
`offline-first-build.log`, `offline-first-package-atomicity.log` and
`runtime-offline-first/report.json`. Type checks, 73 preparation/runtime tests,
eight production-build/self-host contracts and Runtime generation/cache tests
passed. The real IndexedDB atomicity test also verified that a delayed update
cannot overwrite a later local import. The browser
fixture exercises actual Service Worker + CacheStorage with immutable synthetic
programs, including offline use, identical manifests, pending download, integrity
failure, successful successor and persistence across Launcher reload. It does
not establish native game/device acceptance.

The user subsequently requested committing all main-workspace changes together.
This frontend change has not been deployed or pushed in this session.

Follow-up validation: `update-dialog-services.log`, `update-dialog-mounted.log`,
`update-dialog-build.log`, `game-update-dialog/report.json` and
`runtime-offline-first/{chromium,firefox,webkit}/report.json`. The popup is tested
in all three desktop engines, including Chinese/English labels, default focus,
keyboard confirmation and Escape. Mounted Launcher tests cover a deferred
update, immediate restart in the same iframe, quiet download failure, and a save
failure that stays in the current game. WebKit cache tests use origin connection
drops only after reproducing the upstream offline-emulation failure with an
independent literal-response worker. These are not real iOS/Android device tests.
