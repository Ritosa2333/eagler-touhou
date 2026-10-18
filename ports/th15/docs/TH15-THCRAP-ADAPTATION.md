# TH15 THCRAP topic handoff — 2026-10-06

Inter-stage black flash fix (2026-10-07, local/uncommitted): task 2 no longer
calls `draw_loading(false)` after the departing scene composite. That call
cleared the framebuffer to black without signature artwork. Preserve the last
completed composite through resource preparation, then use the existing retained
STD transition; startup, title entry and other loading tasks are unchanged.
Actual browser transition regression PASS with the 60Hz presentation lock both
enabled and disabled: 294,153 / 307,200 pixels retain visible scene content at
the pending resource boundary, with no accumulated gameplay ticks on resume.
Release WASM: `d7285b63267f82d02b46b0ad54d178868f7d8b160ab597fc3d7f483ee1026d22`.

Promotion update (2026-10-07): topic commit `20bd2c8` was fast-forwarded
into `eagler` after fetching origin/eagler `ac75051`. Existing replay diagnostics
were preserved and integrated in a follow-up commit. Private archives, fonts,
language ZIPs and build artifacts are excluded. Earlier isolation/deployment
notes below describe the validation history, not current branch status.

- Upstream worktree: `th15`, `main`, base `fe113b6`.
- Eagler base: `th15-eagler`, `eagler`, `4350363`.
- Experiment: `_scratch/th15-thcrap`, `experiment/th15-thcrap`, uncommitted
  topic changes based on `4350363`; not promoted or deployed.
- Companion Launcher experiment: `_scratch/launcher-th15-thcrap`,
  `experiment/th15-thcrap`, base `f5c2855`, uncommitted topic changes.
- Original Japanese DATA SHA256:
  `f3802e71db30f99b86d61b6e98b8325716c5cbae892547c563dded7837862ae7`.
- Source routing, pack preparation and known limitations are recorded in the
  companion Launcher `docs/TH15_LOCALIZATION.md`.

Owned changes: display-only spell/music/Result localization; official text
bank override; localized MSG/Ruby/bubble presentation; SDL font/layout/image
overrides; pre-launch pack mount; production capability declaration and host
compiler integration. No gameplay timers, RNG, score/replay storage or network
owners are intentionally changed. Canonical worktree edits remain untouched.

## Music Room detail sizing — 2026-10-07

- Only localized comment rows 1–7 request `music_detail`; row 0 retains
  the official note/title format and original size. Track lists, dialogue,
  Player Data and 26px spell titles are unaffected; Japanese measured fonts
  continue through the original raster path.
- Follow-up: Chinese detail prose is fixed at 30px as requested; only English
  starts at 24px instead of 32px. The validated pack manifest language is
  mounted as a Runtime-owned locale marker (not inferred from glyph content).
  Longer authored English lines are
  measured with SDL_ttf, including THCRAP layout runs, and reduced in 1px
  steps to fit the actual sprite region / 1024px scratch buffer with outline
  margins. No translated strings or authored line breaks are rewritten.
- Font size and tab state are restored after each detail render, including
  failed raster writes, so this shared font slot does not leak into menus.
- Release WASM SHA256:
  `3b8bf50a956e1763f2dcb0111af782b810651b63fbe1311ebddea6e24d6d5786`.
- Production protocol browser smoke PASS (ja / zh-hans / en), 83.3s.
  Track-four English and Chinese detail screenshots inspected: both reported
  English long lines now display their complete endings. The test also exits
  Music Room and enters gameplay to exercise the restored shared font.
- Refreshed only the local validation site on port 8146; inventory verification
  PASS, 338 files. Language ZIPs are unchanged: this is runtime typography.
  No commits, pushes or remote deployment.

## Gates

- `tests/test-thcrap-compiler.mjs`, `test-thcrap-static-pack.mjs`,
  `test-th15-thcrap.mjs` in the Launcher: PASS.
- `th15_web/tests/cpp/thcrap-dialogue.test.mjs`, with `TH15_EMSDK` and
  `TH15_LANGUAGE_PACKS`: PASS, 136 scripts / 3489 lines / 51 Ruby. Metrics in
  this lane are synthetic; this is parser/progression evidence only.
- `th15_web/tests/sdl/thcrap-production-browser.test.mjs`, with
  `TH15_ORIGINAL_DAT` and `TH15_LANGUAGE_PACKS`: PASS for all three languages;
  title and Music Room screenshots inspected; no diagnostic production exports.
- `scripts/verify-server-build.mjs <validation-site>`: PASS with a warning
  about no operator fallback-download link. Site has no OGG; it is not a
  complete game resource package.
- `th15_web/tests/sdl/thcrap-launcher-browser.test.mjs`, with `TH15_HOST_SITE`:
  PASS for ja/zh-hans/en through the actual assembled Launcher, catalog
  selection, validated pack mount and production Runtime startup.
- Full ending/physical-phone/GDI parity and all ASCII-site coverage: NOT RUN
  or incomplete. Do not relabel these as PASS.

## 2026-10-06 visual regression corrections

- PNG replacement now includes the ANM entry's source X/Y offset. The original
  signature is split into 1024-wide and 256-wide entries, and title/front banks
  also share image names across entries. Using entry-local coordinates as PNG
  coordinates caused the Japanese signature tail, English logo rectangle and
  incorrect gameplay BGM sprites. Zero/one-sprite entries patch their entire
  bitmap, following thcrap `thcrap_tsa/src/anm.cpp`; opacity decisions use the
  unmodified original bitmap. Full wrapped multi-sprite/jdiff parity remains
  outside this regression proof.
- Music comment row zero is the thcrap `@` title placeholder, not a separator
  to delete. Preserve the patch's eight slots and expand it with the official
  `Music Room Note Title` format and the localized track title.
- Loading completion no longer has an invented one-second deadline. Read-only
  disassembly of the Japanese `th15.exe`, SHA256
  `67a642357c8777089f468aab9c7a0ae346ebdb62849d842a7b7b18d1e6910364`,
  identifies owner creation at 44bc00 (flag 2), registration at 44be50 (update
  initially disabled), worker 44bd00 (loads signature, title, text, comments,
  audio and shared assets; enables update at 44be38), and completion callback
  44c310. This was incomplete timing evidence: the separate title worker
  460500 waits for loading owner +644 to reach 180 frames (46052a), incremented
  by draw callback 44c210. That minimum has NOT yet been ported to the Web
  presentation clock; the current fast loading is a known unresolved issue.
  Game worker 43bff0 also waits for graphics-owner state 51e0a4 to clear.
  `tests/native/loading-xrefs.py` is a read-only investigation helper.
  Resource completion alone is not proof of original loading timing parity.
- Production browser regression captures actual startup canvas, title,
  Music Room title-track comments and stage-one BGM banner for Japanese,
  Simplified Chinese and English. Music is disabled in this fixture: it proves
  banner rendering, not audio acceptance. Local validation uses port 8146;
  no remote site is changed by this work.

Validation: production three-language regression PASS; inspected both patched
startup canvases (no Japanese disclaimer tail), English title (logo restored),
Chinese/English stage-one BGM banners and English title-track comments (no @).
`loading-buffer-browser.test.mjs` PASS for startup, entry and stage transition:
no invented deadline, resource completion and no accumulated simulation debt.
Final release WASM SHA256:
`e2da8d36b7829fc82edcc68095e8045122db05c2b8c8e78f1bf524b318f2b9de`.
Local Runtime generation:
`cce6f150bd48b2c8b147ee7dcbb227cd02e0a58ddd234f8a0441fc2c7fab8a2a`.
Local server manifest/integrity verification PASS (fallback-download warning;
no OGG, as above). Language ZIP content is unchanged: these are runtime fixes.
Real assembled Launcher selection/install/startup regression also PASS for
Japanese, Simplified Chinese and English on this final Runtime generation.

## Scene-bridge regression fix — 2026-10-07

The outer SDL platform provided font measurement, but StageGameplay::Services
forgot to forward dialogue_text_extent. Its inherited default returned -1:
ordinary dialogue used UTF-8 byte-count balloon widths, and THCRAP ruby stopped
the game with "THCRAP ruby font measurement unavailable". Forward the request
through the actual scene bridge; do not suppress the error or drop ruby rows.

Localized font sizes now follow TextRaster::font_index and measured native
CreateFontA slots (32/32/15/15/40/40/15/15/28), not scratch-row dimensions.
The English language leaf now overrides font-provider dependency artwork and
tables; the font-options preference is unchanged. Its Player Data image is
the complete English result00.png, not script_latin's partial image.

PASS: actual renderer stage-one Sanae dialogue, eight boxes in both Chinese
and English including the Eagle Rabbit ruby; real production three-language
startup/Player Data/Music Room/BGM smoke; real Launcher selection/install;
local site manifest and content identity verification. This does not establish
all Extra-dialogue/spell-title layout or long Music Room comment acceptance.

Current release WASM: d2b1f8bf912d763c01616ec6c3a3fae3cc4ca6a1c72963d0dd983014b23d3906.
Local Runtime generation: 4ce83e3dd33d39c51b552c6399d8dd3793be3800929b08a804996ff532da64e0.
English language ZIP: 13,758,224 bytes, SHA256
9a081d4f52e4f7a80f7fc0635cd2647dc92c1354855dadeed6834d1bd1441814.
Chinese ZIP is unchanged. Local 8146 only; no commit/push/remote publication.

Still open: original 180-frame startup/fade and game-entry timing, proportional
official font rules for long English Music Room descriptions, and rendered
Extra long spell-title/dialogue acceptance. Do not advertise these as fixed.

## Review / integration boundary

### 2026-10-07: native startup wait and loading ANM clock

Read-only native disassembly reconfirms title worker 46052a compares loading
owner +644 with 0xb4 (180), with increments at draw callback 44c2f8. Startup now
retains 180 isolated 60Hz loading animation ticks before enabling gameplay RAF.
The old unconditional 11-tick loading ANM fast-forward is removed; signature
and prayer animate using their original scripts. Resources are prepared before
resuming the loop; loading debt is independent of game cadence and input is
cleared at exit. No gameplay/replay tick or RNG is consumed by loading animation.
Game entry still uses resource completion and existing native entry/intro
controllers. No fixed 90/120-frame game-loading minimum has been established;
this update must not be advertised as full restoration of its observed 1.5-2s
duration or native asynchronous resource-worker behavior.
Browser loading regression passed: startup 4070.71ms including preparation,
entry resource hold 1871.53ms, next-stage resource hold 6850.10ms in this desktop
Chromium fixture (music disabled). These resource costs are observations, not
hardcoded timing guarantees. Startup retains all 180 frames, screenshots prove
loading animation changes, and resume has no accumulated simulation debt.
Release WASM SHA256:
c7b24223209ea5e12120f6cd44928ee44bc2f9941ff4939d0c40f5f4addbc4e1.

### 2026-10-07: final requested localized spell-title size

User requested 26px instead of 24px. Only localized font slot 8 is changed to
26px. Other font slots, measured Japanese font assets, language ZIPs and the
corrected high-refresh background interpolation remain unchanged.

### 2026-10-07: actual ANM texture-coordinate column correction

The user confirms Extra twitch disappears with STD background interpolation
disabled. Source tracing found an independent concrete defect in the interpolator:
anm_texture_transform writes UV translation in m[8]/m[9] and the shared shader
multiplies textureMatrix by vec4(uv,1,0), but presentation treated m[12]/m[13]
as UV translation. The actual scrolling entries therefore took a direct linear
path across Repeat boundaries, producing a spurious midpoint near half a tile.
presentation_texture_matrix now applies sampler-aware wrap handling to m[8]/m[9]
and ordinary linear interpolation to other entries. Tests cover both UV axes,
Clamp, sampler changes, irrelevant m[12] and exact alpha=1 matrix equality.
The Extra-only 60Hz isolation has been removed from both live RAF and private
probe callers; background high-refresh interpolation is enabled again.
Keep the optional bypass API for explicit diagnostic use, not automatic stage
selection. This correction changes presentation only; language ZIPs and 24px
localized spell-title settings are unchanged. Final visual acceptance is still
the user's high-refresh Extra retest, not a claim based solely on static code.
Actual original-data Extra regression passed across 2400 logical ticks with
background interpolation enabled. Repeat UV wraps were captured at frames
240, 480, 720, 960, 1200, 1440, 1680, 1920, 2160 and 2400, affecting 11-14
camera samples per boundary. The corrected midpoint remains near its previous
periodic endpoint. Repeated midpoint draws are pixel-idempotent, alpha=1 matches
the authored current framebuffer, and the 16-field gameplay/RNG probe is unchanged
by presentation. These checks do not replace complete replay-state coverage.
Release WASM SHA256:
6dc28fb7ca0bae1984ad42e02ca5c67810f09754940299a80da0a33979024ab4.
Refreshed local Runtime generation:
76438621c6d2d05f89a6e7bdf7baee23737127a28cb467e5340ff30b5863aabb.
Local production package and deployment inventory validation passed.

### 2026-10-07: temporary Extra background isolation (not a complete fix)

User retesting confirms background-only twitch remains after the camera-loop
guard. Do not label that guard as resolution of the reported symptom.
This local experiment now bypasses all interpolation of captured camera/STD
background commands in Extra (stage 7), submitting their authored current
packet unchanged. Player, bullets and non-background presentation keep high
refresh. Other stages retain their existing interpolation behavior.
This trades Extra background smoothness for an A/B isolation check; it is not
approved for remote deployment or promotion as a finished fix. Spell-background
ANM rendered outside the STD camera ownership is not covered by this bypass.
The manual question is whether the remaining twitch persists with STD camera
packet interpolation removed. Simulation and replay timing remain unchanged.
Isolation release WASM SHA256:
e2ee7efbc9e86058e4e57d213a4d312a6b0ace7650aed71d028961bf0d497145.
Local Runtime generation:
6a61a7ebb4b3f551427738a888e9bccceb8516133101502b1504b167d1bc6c8d.
Production package and local deployment inventory validation passed.

### 2026-10-07: Extra camera-loop boundary and 24px localized spell titles

Localized spell-title slot 8 now uses 24px (other localized slots unchanged).
The measured Japanese slot remains 28px; no retail font asset is regenerated.
Private original-data Extra diagnostics ran 2400 actual ticks with midpoint
presentation draws. Camera discontinuities appeared at frames 2, 1031 and 2061;
the recurring boundaries have a maximum matrix delta of 361.70279 and a world
matrix delta of 512. No multi-sample camera batches occurred in this run.
Previously the 128-unit guard skipped only large camera matrix elements while
interpolating small elements and other geometry, creating a mixed reset view.
The guard now rejects interpolation for the complete matched background sample
at camera discontinuities, retaining the authored current draw transaction.
It does not change STD scripts, camera simulation, RNG or replay state.
Regression checks cover continuous matrices and positive/negative cut bounds.
Diagnostics are not a complete visual oracle; the user's Extra high-refresh
acceptance remains necessary. Test-only Extra entrance and diagnostics exports
are gated by TH15_DEVELOPMENT_HARNESS and must not be published.
Current local release WASM:
040c24ee4a38b6b79879dfe29d365eab69ce45c7a1f8aa5b8ef408033a53b721.
Current local Runtime generation:
9e325c41fe8f9ed2d96cd05a9a25d658023118ed27a1ad925013327d9baca038.
Production packaging rejects diagnostic exports and passed for this release;
refreshed local deployment inventory validation passed. Language ZIPs unchanged.

### 2026-10-07: spell-title size and presentation UV boundaries

Spell titles now use native font slot 8 (28px) instead of slot 0 (32px).
Dialogue, Music Room and Player Data font selection is unchanged.
Presentation interpolation now respects each axis's sampler address mode:
Repeat has period 1, Mirror has period 2, and Clamp uses direct interpolation.
Sampler changes retain current coordinates instead of mixing incompatible
states. The same rule applies to vertex UVs and texture-matrix translations.
The standalone C++ regression covers wrap boundaries, half-period ties,
non-scrolling coordinates and exact current-frame endpoints and passes.
This corrects a presentation defect but does not establish that the user's
probabilistic background twitch is fully resolved; manual high-refresh
acceptance remains required. Simulation timing and RNG are not modified.
Local release WASM SHA256:
caa39d8651501efbfc4f81423821a15b221ee97cc1a10a1c968b0a1d9a1770f6.
Local Runtime generation:
6aca3502f6a1d15b6053f473281d581b38de5da2da633b2defec1c693145c047.
Production package and refreshed deployment inventory validation pass.
Language ZIP contents are unchanged for this presentation-only update.

The complete tracked and new-file topic diff must be reviewed against the
recorded bases before promotion. Shared compiler branches are scoped to TH15;
previous compiler/static-pack regressions pass. Ordinary JP overrides remain
absent when no pack is mounted. Runtime overrides are bounded and installed
only before launch; ZIP identity is still validated by the Launcher.

Uncommitted source and the labeled local artifacts are an intermediate state,
not a completed promotion boundary. Generated artifacts, retail DATA, private
fonts and language output ZIPs must not be committed with the source.
