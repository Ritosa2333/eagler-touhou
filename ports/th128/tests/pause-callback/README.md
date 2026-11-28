# Pause callback ownership tests

check.cjs extracts the complete production originalPauseConsumer and runs its callbacks through the real OriginalPauseController and MenuAudioGate. Deferred operations expose an entered barrier and a separate held promise; assertions confirm the call is still pending before retirement/replacement. It also executes the complete actual help-page loader with synthetic fetch/bitmap/GL services.

The41 cases cover replay/name observe/save, help loading, audio-owner lookup and restart awaits; positive paths, current errors, retirement/replacement, stale errors, selection and store replacement. Existing old-store completion is allowed; later native ACK, store calls, binding and successor-state writes are rejected.

controller-owner-actions.cjs adds8 actualcontroller checks around actions1/2/3/6 after a deferred audio consumer. Both active and replaced ownership are covered.

```text
node tests/pause-callback/check.cjs
node tests/pause-callback/controller-owner-actions.cjs
```

Optional check.cjs arguments: play.mjs, sound.mjs, draw.mjs in that order. MAIN618 observed25 failures and16 passes against the old sources;619 passed the same41;620 ran49 new plus74 existing regression cases on the integrated source.

Native, storage, DOM and graphics are synthetic boundaries. These tests do not start a World/browser, read GPU pixels, prove durable storage, or retire a real native pause owner. Never overwrite the immutable accepted case directories.
