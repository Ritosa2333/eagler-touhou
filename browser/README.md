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
