# TH13 feedback closure todo

This file records the current product feedback and its owning repair item. A
case is closed only after its named hypothesis has source/original evidence, a
minimal reproduction, a production-path fix, and browser-visible regression
evidence. Unit-test success alone is not closure.

## Feedback to repair-item mapping

| Case | Feedback | Owner | Classification | Required closure evidence |
|---|---|---|---|---|
| H03-1 | Loading screen's “少女祈祷中” is absent | H03 | Existing feedback; same loading/transition lifecycle gap | Cold start, restart, stage transition, and Replay entry show the original loading ANM with original create, hold, fade, and retire timing |
| H03-2 | Each stage's opening title text is absent | H03 | Existing feedback; same stage-title presentation gap | Every stage in both six-stage Replays shows the title at the original birth frame with matching text, geometry, alpha, and lifetime |
| H01-1 | Reimu disappears after entering any main-menu item and returning | H01 | Existing feedback; title ANM/root-child lifecycle gap | Cold start, enter/leave each available main-menu item, return to title, and repeat; Reimu and all menu rows remain visible and highlighted correctly |
| H08-1 | In-game effects are broadly missing, including spirit absorption, Bomb, spirit-gauge gain, pause, and Power gain | H08 | Existing feedback; expand audio trigger/resource/output matrix | Each named event has original event frame, channel/volume, duplicate/stop rule, decoded PCM assertion, and audible browser playback after pause/resume |
| H11 | Selecting Manual crashes the game | H11 | New feedback; Manual supervisor exists but the host viewer adapter is not implemented | Main-menu Manual entry opens `help.anm` plus `help_01.png` onward without terminating the app; page navigation, end/close, return, and repeated entry work |
| H12 | Music Room text is severely mojibaked | H12 | New feedback; CP932 titles/comments are queued through the ASCII glyph path | CP932 catalog text is rasterized through the production Japanese-text texture path with original line layout, clipping, color, and lifetime |
| H13 | In-game BGM title is absent at the lower right, including stage opening and Boss BGM switches | H13 | Existing user request; no runtime title overlay consumes the current base track | Stage and Boss switches update the title at the original frames and coordinates; `b.wav` variants, stop/fade, pause, Replay, and cross-stage restoration retain the correct displayed title |

## Investigation order

1. H01-1: title ANM root/child and menu-return teardown, before broader menu polish.
2. H11: Manual host adapter and resource ownership, isolated from title-state work.
3. H03-1/H03-2: transition submit/lifetime and stage-title ANM ownership.
4. H12/H13: shared runtime CP932 text path, with separate state/lifetime tests.
5. H08-1: trigger tracing and actual browser PCM/output checks.

The two full Replay comparisons remain the final integration gate for H03,
H08, H12, and H13. H01-1 and H11 additionally require menu and browser
interaction coverage outside Replay input.
