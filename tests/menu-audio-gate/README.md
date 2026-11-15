# Menu audio owner regressions

The tests execute actual MenuAudioGate and, in composition.cjs, actual OriginalPauseController. AudioContexts/native APIs are synthetic and deferred explicitly. They never start a browser or World.

Run the current-checkout tests separately:

```text
node tests/menu-audio-gate/check.cjs
node tests/menu-audio-gate/waiting-entry.cjs
node tests/menu-audio-gate/blocked-resume.cjs
node tests/menu-audio-gate/composition.cjs
```

An optional first argument selects an exact alternative sound.mjs for admitted RED/GREEN cases. The six scenario foundations came from DSH's reviewed packet; MAIN made the source path portable, removed the artificial requirement that cleanup complete while its predecessor's suspend was held, and added waiting-entry, blocked-resume and real-controller composition checks. The original raw evidence remains immutable.

See ../../docs/port-menu-audio609.md for scope. Real device audio, native pause retirement, and already-entered replay/presentation callbacks are not established by this suite.
