# Optional same-origin input delivery

The required `eagler-touhou/1` postMessage transport is unchanged. A Runtime may
add `window.__eaglerDirectInputBridge` with schema `eagler-touhou/direct-input/1`,
its `protocol`, `game`, exact `origin`, and synchronous `submit(message): boolean`.
This is an optional transport implementation detail, not a gameplay feature or
a new obligation for every game adapter.

Use the bridge only when same-origin Window access succeeds and all four
identity fields match. Cross-origin, legacy and unsupported sessions keep
postMessage. `true` means consumed once; `false` means no input was consumed.
Exceptions/invalid acknowledgements must never cause retry: movement and Bomb
are non-idempotent inputs.

Supported operations are direct-touch, touch-controls, touch-cancel, keyboard,
and keyboard-clear. Both delivery paths share Runtime validators and raw input
consumers. Keeping all gameplay input on one lane avoids immediate movement
overtaking queued Bomb/cancel input. Request/reply, lifecycle, files, configuration
and thprac mouse commands still use postMessage.

TH07 enables this only for zero-delay full-rollback LAN, not ordinary play,
Replay viewing or buffered modes. No simulation is advanced from the handler,
and no player sprite is predicted separately from its collision position. This
removes one browser task queue hop, not normal 60 Hz sampling and presentation.

Tests: Launcher `test-touch-runtime-protocol.mjs` checks once-only handling,
origin matching, refusal and failure. Runtime `immediate-input-bridge-test.cjs`
checks lifecycle, shared validation and coordinate conversion. The browser
fixture compares both paths using `--wall-clock-drag --input-latency` and
`--direct-input-bridge`; reported latency ends at render submission, not photons.
