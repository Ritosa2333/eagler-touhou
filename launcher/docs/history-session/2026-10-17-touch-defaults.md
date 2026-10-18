# Shared touch defaults and warning policy

The existing main frontend now defaults unset movement preferences to
`touch-unlimited`. Explicit saved modes and legacy `unlimitedTouch: false`
remain unchanged. Shared preferences and fallback readers use the same default.
The separate touch-enabled switch keeps its existing preference behavior.

Enabling/disabling touch and selecting a movement mode no longer ask for a
touch/Replay/unlimited warning confirmation. Starting with touch disabled also
has no touch-input warning. The visible magnifier/two-finger conflict warning
is removed. Room movement restrictions, native Replay/result behavior,
save/discard confirmations and operational errors remain with their owners.

This is an all-product frontend policy, recorded in the Input/Touch playbook.
No native Runtime or title-specific input implementation was changed.

Validation: preference migration/persistence checks passed. The related settings,
input and mounted-editor matrix passed 81 tests. A mounted Launcher test also
confirmed that a touch device can start with touch disabled without an input
warning, while the separate music-mode confirmation remains. Reports:
`artifacts/validation/touch-defaults.log` and `touch-start-warning.log`.

Included in the user's subsequent request to commit all main-workspace changes.
Not deployed or pushed in this session.
