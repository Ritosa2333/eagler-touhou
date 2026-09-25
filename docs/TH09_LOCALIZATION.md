# TH09 THCRAP localization adaptation

This title uses the same Host → static THCRAP ZIP → Launcher → Runtime boundary
as TH06/07/08/10. The Runtime never downloads patch repositories itself.
The unmodified Japanese files remain the fallback when a patch file or entry
is absent.

## Source ownership

- TH09 v1.50a archive extraction uses `thdat -x 9`; dialogue compilation uses
  `thmsg -d/-c 9` and the TH09 opcode/crypt table from `thcrap_tsa`.
- Endings (`endNN.end`) use the base_tsa `.end.jdiff` patch format; TH09 story
  and match dialogues use `plNN.msg.jdiff` and `plNN_match.msg.jdiff`.
- `spells.js`, `themes.js`, `musiccmt.js`, TH09 stringlocs and `data/**/*.png`
  are compiled or copied by the shared THCRAP pack preparation path.
- A local, licensed Unicode font is subset to the codepoints in each selected
  patch. The selected `options.json` names that subset. Do not publish retail
  `th09.dat`, `th09.data`, or `msgothic.ttc` as part of the language ZIP.

## Build and publish boundary

Run the existing `scripts/prepare-th06-language-pack.mjs` with `--game th09`,
`--language lang_zh-hans` or `lang_en`, and explicit `--archive`, `--thdat`,
`--thmsg`, `--font-file`, `--output`, and (on Windows) `--temporary-root`
arguments. The output has `catalog.json` and `language/lang_*.zip` files.
Use that directory as `--th09-language-packs` when calling
`scripts/package-server.mjs`; the Host Manifest's `languageOptions` and the
Package Descriptor's language component are generated there, not handwritten.
To make a single web-importable resource-and-language ZIP, run
`node scripts/package-offline-game.mjs <assembled-site> th09 <output.zip>`
and verify it with `node scripts/verify-offline-game-package.mjs <output.zip> th09`.
This is the same Package Descriptor/STORE ZIP producer used for TH06/07/08/10;
it includes the TH09 data, required fonts, OGG music, and all published
language packs. Never commit this ZIP or publish it without the game's asset
distribution rights.

`th09_web/scripts/build-sdl.mjs --release` enables `TH_ENABLE_THCRAP`.
`th09_web/scripts/build-eagler.mjs` advertises the Runtime language capability.
The Launcher passes its selected language pack into the Runtime's `configure`
command before launch. The Runtime mounts validated files under `/thcrap/th09/`
and routes image, dialogue, ending, music, spell, help-string, and font lookups
through those files. Its original-data paths are retained as fallback.

The Launcher already derives `thpracLocale` from the same selected game
language (`ja` → `ja-JP`, `lang_zh-hans` → `zh-CN`, others → `en-US`). TH09 does
not yet expose a thprac adapter, so this establishes the shared language
selection contract for a future implementation; it does not claim that TH09
thprac localization is active now.

## Verification scope

Run `node tests/test-thcrap-compiler.mjs`,
`node tests/test-thcrap-static-pack.mjs`,
`node tests/audit-th09-generated-pack.mjs <pack-directory>`,
`node scripts/verify-server-build.mjs <site-directory>`, and the local browser
smoke `EAGLER_TEST_URL=<local-site> node tests/smoke-th09-localization.mjs`.
`EAGLER_TEST_MUSIC_ROOM=1` additionally opens the Music Room for both packs;
`EAGLER_TEST_STORY=1` launches Chinese Story Mode. The browser test verifies
Chinese and English startup plus required Runtime mounts. Screenshots confirm
visible Chinese Music Room titles/comments and an actual translated story
dialogue line. This is functional evidence, not proof of pixel-perfect parity
or of every ending/dialogue branch; those require further visual regression.
For the single-ZIP import path, set `EAGLER_TEST_PACKAGE=<output.zip>` and
`EAGLER_TEST_LANGUAGE=lang_zh-hans` or `lang_en` when running
`node tests/smoke-th09-package-import.mjs`. That check asserts the selected
language mounts from the imported package without requesting a hosted pack.
