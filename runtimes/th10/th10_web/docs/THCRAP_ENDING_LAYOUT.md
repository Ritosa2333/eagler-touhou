# TH10 ending layout repair

## Source and ownership

- Canonical Eagler base: `ec0a7d5829ff516886e1669faac2c6b71a6eb07b`.
- Upstream-tracking ref at review: `upstream/eagler`, same commit.
- Experiment: `_scratch/th10-ending-layout`, branch
  `experiment/th10-ending-layout-20261004`.
- Authority: local thcrap `thcrap_tsa/src/layout.cpp`,
  `layout_tokenize`, `layout_parse_tabs`, `layout_process`.
- Adopted port: TH11 `th11_web/cpp/sdl/ThcrapLayout.hpp` at `5652582`.

TH10 previously flattened markup, suppressed hidden tab definitions and
applied the last `r/l/c` command to the entire string. Ending speaker names
therefore displaced the body and continuation lines lost their tab position.

Markup now reaches the SDL font host unchanged. It measures and draws each
run with the selected font, keeps tab definitions across TextOut calls, and
resets the local x/tab cursor each call. `ts` defines a hidden tab without
advancing narration; `r` aligns only the speaker inside that tab; `l` resumes
the body at the established tab. Font modifiers participate in measurement
and raster cache identity. Shutdown clears tab state. Original CP932/CP936
decoding, fallback faces, A4R4G4B4 blending, bitmap upload and MSG page timing
remain unchanged. No language text or retail resources are replaced.

## Gates

- `node portable/check-thcrap-layout.mjs`: PASS, Chinese/English ending
  speaker/body/continuation, hidden tabs, reference widths, full-bitmap
  alignment, font commands and literal fallback.
- `EAGLER_WORKSPACE=<workspace> node portable/check-thcrap-layout.mjs --th11-proof`:
  PASS against the existing TH11 parser; no identical flattening bug found
  in its ending-to-FontDevice path. This is not a claim that every TH11 ending
  has been visually inspected.
- `EAGLER_WORKSPACE=<workspace> node portable/check-thcrap-font.mjs`: PASS
  for both prepared TH10 language subset fonts. Actual SDL_ttf raster pixels
  match separately positioned speaker/body and continuation reference runs.
- Full `portable/build.mjs --thprac`: PASS.
- FontHost compilation without `TH_ENABLE_THCRAP`: PASS.
- `git diff --check`: PASS.

Set `EMSDK` to the pinned SDK when it is outside this repository. The font
regression requires private `prepared/th10-auto-dialogue` packs and shared
fonts from the canonical `th10/build-eagler/fonts`; these are not committed.
One initial test compilation hit a temporary-directory permission failure;
rerunning with a task-local TEMP/TMP passed.

Not run: full ending playthrough, physical-phone visual QA, original Windows
GDI pixel parity, deployment. The tests prove source-derived positioning and
the named SDL_ttf raster path, not complete visual parity. No beta/main site
or private resource ZIP is changed by this repair.
