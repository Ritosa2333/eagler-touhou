# Native UI diagnostic capture

This development-only GPU fixture replaces the Application translation unit in a separate diagnostic link. It never adds exports to production, packages a Runtime, runs the original executable, or provides transport/full-playthrough acceptance. Captures use the actual GameSession, SDL/GLES renderer, original resources and Chromium SwiftShader.

Build the current requested production variants first. Their `build.json` source/header signatures must match the checkout; the diagnostic refuses stale caches, including headers that change class layouts. For ordinary builds the diagnostic reads `features.thprac` and requests matching `--thprac` flags; it does not change or overwrite that cache. For the focused MP audit use `--mp-only`, retaining the captured ordinary reference.

```sh
node portable/build.mjs --multiplayer
node portable/multiplayer/native-ui-build.mjs --phase after --mp-only
node portable/multiplayer/native-ui-capture.mjs --phase after --mp-only
```

`EMSDK` selects the pinned SDK. `EAGLER_LAUNCHER_ROOT` selects the Launcher checkout that provides Playwright. `TH11_TEST_ARCHIVE`, `TH11_TEST_FONTS` and `TH11_TEST_MUSIC_ROOT` can override the local retail archive, original baked font directory and decoded original music directory. Ordinary references load `th11_00.ogg` and `th11_01.ogg`; audio is muted and not part of screenshot acceptance. `--mp-only` builds/captures only MP and needs no music or ordinary binary; `--ordinary-only` captures just the ordinary references. Ordinary THPrac capture additionally requires the UI font used by its production build. All new diagnostic exports use the rejected-from-package `mp_fixture_ui_` prefix.

Outputs go to ignored `artifacts/multiplayer-ui/20261009/<phase>/`: diagnostic binaries, build signatures, screenshot evidence and PNGs. The initial `before` capture used commit `2bd03f0` and verified matching production cache signatures. Its captured source copies remain ignored artifacts, not checked-in test fixtures.

The MP fixture supplies exact native input arrays directly, then consumes native capture/text/audio events and draws/presents the real frame. Initial neutral ticks are 125. Life/Power fixtures set positions to x=0/10/100, y=400 and protect players from incidental bullets. Ghost rescue pins only ghost drift in this diagnostic, as recorded in evidence. Transfers themselves use native timing, input edges and items. Terminal screenshots enter existing StageExit seams; Ending runs the real Ending/Staff interpreter. They do not prove a full clear or the 180-frame wipe. Game Over/Extra/TitleResults settle for 30 ticks so native 20-tick title transitions complete.

The suite captures ordinary pause and Replay list, MP pause, three local-seat views, life gift hold 30/60/89/90 plus delivery, ghost rescue 45/89/90, Power taps 3/4/5, Game Over, Extra Result and post-Ending Results. Evidence records state/hash and the fixture entry alongside each PNG. Visual comparisons should preserve original layout/resources while excluding ordinary score/name/ReplaySave/Continue side effects from MP.
