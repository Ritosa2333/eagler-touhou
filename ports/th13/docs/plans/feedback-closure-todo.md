# TH13 feedback closure todo

This file records the current product feedback and its owning repair item. A
case is closed only after its named hypothesis has source/original evidence, a
minimal reproduction, a production-path fix, and browser-visible regression
evidence. Unit-test success alone is not closure.

## Feedback to repair-item mapping

Browser tests live in `web/tests/sdl/` and run against the production page.

| Case | Feedback | Owner | Status | Closure evidence |
|---|---|---|---|---|
| H01-1 | Reimu disappears after entering any main-menu item and returning | H01 | Closed | `menu-closure.test.mjs` enters Manual and Music Room, returns to the title and repeats, with audio output; native `title_start_retires_restore_logo_root_after_menu_return`, `title_option_entry_preserves_logo_and_version_bar` |
| H11 | Selecting Manual crashes the game | H11 | Closed | `menu-closure.test.mjs` opens the production Manual repeatedly without terminating the page |
| H12 | Music Room text is severely mojibaked | H12 | Closed | `music-room-text-identity.test.mjs` rasterizes the production catalog through the Japanese-text texture path |
| H08-1 | In-game effects are broadly missing (spirit absorption, Bomb, spirit-gauge gain, pause, Power gain) | H08 | Closed | `audio-events.test.mjs`: pause voice 14 at the frozen replay frame; stage-1 real replay spirit, big-power and spell matrix with PCM output |
| H03-1 | Loading screen's “少女祈祷中” is absent | H03 | Fixed; browser-visible evidence pending | Native `anm_loading_caption_rng_test.cpp`, `loading_prayer_test.cpp`, `stage_transition_lifecycle_test.cpp`, `b_menu_prayer_rng_test.cpp`; the draw-manifest digests lock the caption's draws |
| H03-2 | Each stage's opening title text is absent | H03 | Fixed; browser-visible evidence pending | Native `hud_test.cpp`: `stage_logo_uses_real_script_and_original_gates`, `stage_logo_fails_when_bank_is_missing` (`0x425170`) |
| H13 | In-game BGM title is absent at the lower right | H13 | Fixed; native and browser-visible evidence pending | `GameHud::stage_music_logo`（`0x429a24`），原生测试 `tests/ui/hud_test.cpp`` |

## Remaining work

1. H03-1 and H03-2: assert on the production page that the caption and the stage title appear at their original birth frames, then move both to Closed.
2. H13: add a native test for `stage_music_logo` and a production-page assertion for stage and Boss BGM switches.
