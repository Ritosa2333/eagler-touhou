# Browser keyboard ownership

`keyboard-owners.mjs` owns physical-key identity and release reconciliation.
The title adapter maps events to bits and decides pulse/lifecycle policy.
The Launcher has its own session-scoped protocol owner: it returns the original
DOWN identity when forwarding a release to an older or newer Runtime.

Known modifier sides and Arrow/Numpad aliases remain independent. An UP with no
physical code or location releases every matching logical owner. This can stop
the remaining physically held modifier on a keyboard that loses all identity;
it prevents an arbitrary owner from staying stuck indefinitely. Fresh DOWN
restores input. A repeat with no observed owner cannot restore cancelled input.

Standalone Emscripten shells embed generated source so older game pins do not
need a new external JavaScript dependency. Never edit the generated block:

```powershell
node tools/sync-browser-keyboard.mjs TH06-SHELL.html TH07-SHELL.html
node tools/sync-browser-keyboard.mjs --check TH06-SHELL.html TH07-SHELL.html
node tests/browser-keyboard-owners.test.mjs
```

`--check` is the cross-title drift gate; the behavior test also runs through
CTest when Node is available. Game-specific tests execute the actual shell
handlers, and Launcher browser integration checks confirmed per-seat input.

Directory Runtimes use `directory-keyboard.mjs` with the same physical owner
algorithm. It retains the resolved DOWN code, reconciles an ambiguous UP against
its actual evidence, merges native and hosted owners, and publishes only changed
codes. `clear()` advances a generation and prevents orphan repeats from rearming.
The title still owns its bindings and practice hotkeys. Updated C++ hosts use the
presence of `Module.resetBrowserKeyboard` to choose this browser owner instead of
OR-ing SDL's independent cached keyboard state. C++ lifecycle clears invoke that
hook and also reset SDL; older shells retain their SDL fallback.

The self-contained generated directory module is an explicit source dependency,
listed in the title packager and Launcher product catalog. Generate it with:

```powershell
node tools/sync-directory-keyboard.mjs PATH/TO/sdl-runtime/directory-keyboard.mjs
```

The generator also supports `--check`. TH08/TH09/TH10 follow-up changes on
2026-10-01 were reviewed as source only; builds and tests were not run at the
user's request. TH06/TH07 verification does not cover this directory bridge.
