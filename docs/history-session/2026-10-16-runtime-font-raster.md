# TH11 / TH15 runtime font resources

The installed-game startup investigation found expanded original glyph tables
inside Runtime generations. Cached launches fully read/hash a generation during
preparation and again during cleanup; individual served resources are checked
again. This work replaces the expanded glyph resources, without changing the
Runtime cache's integrity policy.

TH11 ordinary source is `th11-eagler`, TH11 multiplayer source is the existing
`worktrees/th11-multiplayer`, and both TH15 variants use `th15-eagler`. Launcher
changes are in the existing main checkout `worktrees/launcher-th09-spectator-main`.
The unrelated launcher-refactor checkout is not a source for this change.

Shared implementation: `eagler-common/include/eagler/text/RuntimeFontRaster.hpp`.
Each game routes its existing TextRaster glyph calls to this provider. It reads
the shared Gothic font and the small CP932 map, retains size/weight profiles,
and caps its glyph cache. Existing title-owned outlines, placement, resampling
and point-copy behavior remain. Translated text keeps its language-pack font.
No additional MS Mincho dependency is introduced; the earlier font-generation
recipe's requested family is not evidence that Windows selected that family.

Runtime and Launcher packaging remove TH11 `font0..3.bin`, TH15 `font0..7.bin`,
and each title's old glyph-coverage blend table. The only Runtime font resource
is `fonts/cp932.bin` (131072 bytes). Original Japanese and untranslated fallback
text require shared `msgothic.ttc`, including when a translation is selected.
Gothic bytes are content resources and are not part of Runtime generations.
Older installed packages acquire a newly required shared resource through the
published Package Descriptor. This upgrade accepts only identical installed
DATA/optional file identities and additional required shared files, reuses the
canonical Package Store, and checks the current installation again inside its
mutation queue. Hosted fallback remains available. Import-only installations
without a matching publication retain an explicit missing-resource error.

Local ordinary Runtime inventories, in bytes:

| Title | Previous local build | Validation build |
| --- | ---: | ---: |
| TH11 | 20203448 | 3497532 |
| TH15 | 56061856 | 4045222 |

These are closed Runtime sizes, not total cold installation sizes. A client
without Gothic still needs to acquire the shared font once. Repeated per-file
integrity checks are unchanged; no public startup-time improvement is claimed.

Validation:

- `eagler-common`: `EMSDK=<sdk> node tools/test-runtime-fonts.mjs`, using a
  private Gothic font. Japanese glyphs, size profiles, repeatability, clipping
  and bounded cache passed.
- Both ordinary games compiled with the real Emscripten / SDL3 / SDL3_ttf
  toolchain and produced attested closed Runtime packages without `font*.bin`.
- Main's package preparation / Runtime service tests: 66 passed. Shared-font
  selection, actual TH11 resource installation, product catalog and Runtime
  Release contract tests passed.
- TH11 ordinary / multiplayer package isolation: 7 passed. The MP FontDevice
  and GlyphAtlas translation units compiled. The complete TH11 multiplayer
  diagnostic build also linked successfully against the local shared-library
  work in progress. Diagnostic mode records the actual dependency revision;
  production still rejects an unpinned dependency and diagnostic packages.
- TH15 multiplayer also compiled and produced a closed validation package
  containing only the CP932 font resource. Neither multiplayer result is a
  completed live-room / spectator / Replay acceptance run.
- `EAGLER_WORKSPACE=<workspace> node tests/test-runtime-fonts-browser.mjs`:
  actual packaged ordinary shells launched, Music Room Japanese text was
  visually checked, Stage entry succeeded, and no expanded glyph file was
  requested or installed. Local evidence is under
  `artifacts/validation/runtime-fonts/`.

## Main publication

Published on `https://touhou.vip/` at `2026-10-16T15:56:24Z` after the user's
deployment request. Existing main site settings and unrelated Runtime groups
were retained. TH15 multiplayer is not a published main product, so its local
validation package was not added to main.

Release: `runtime-font-raster-20261016-8847a6937297`.
Release identity: `sha256-35fd97d077ff2a52ea0b5cc7f982154d4df5b302db75fe2422c1ac95da4cbdb6`.

| Runtime | Published bytes | Generation |
| --- | ---: | --- |
| TH11 normal | 3475378 | `377e4881a1970460e1c85a02ac9fb28a657530687746d21ca59690591b0236d4` |
| TH11 multiplayer | 3408226 | `a3710c4587f5b4c7ae9cc4f66ec7083c5fce002d532ff306b549f6c783d50eaf` |
| TH15 normal | 4009256 | `776d78959013e47020d502e237cf5fab9ddde15f33da337e02da2a3b2054a823` |

Publication uses Git archive snapshots of each **previously published** title
revision, with only the listed font edits. The shared text header is separately
identified by SHA-256. TH11 multiplayer uses its published shared protocol pin
`62345858e1a4b101074cdc04720eac44a7c89cf0`. Its exported dependency snapshot is
verified byte-for-byte before compilation, and the normal package validator
accepts the release build. No diagnostic binary or unrelated in-progress
spectator/shared transport change was published.

The existing published frontend was preserved. Its active catalog, shared-font
selection and preparation call were changed with checked AST/string anchors;
the dependency-upgrade helper was compiled from the authored source. Changed
scripts and their importers receive new URLs. The three encoding-table paths
were installed on the existing resource node before cutover. Origin-local
candidate copies were used for generation verification, then removed to retain
the established resource-node-only delivery boundary.

Evidence is under workspace `dist/runtime-font-raster-deploy-20261016/`:

- `provenance.json`, `plan.json`, `published.json`: source closure, release
  contents, atomic cutover and retained rollback location.
- `browser-release.log`: the exact ordinary release packages pass title,
  Music Room and stage-entry checks with shared Gothic.
- `mp-browser/report.json`: the exact TH11 multiplayer release initializes
  native text and renders its Replay menu. This does not establish live-room,
  spectator or Replay playback acceptance.
- `public-browser.json`: real public main frontend loads with no page/request
  errors and advertises the three expected Runtime generations.
- Server receipt: 60 changed non-font paths have verified HTTPS bytes; the
  three encoding paths have verified resource-node bytes, Range and CORS.
  WASM/module MIME types and a real 404 were checked.
- `tests/test-runtime-shared-font-upgrade-browser.mjs`: actual IndexedDB
  upgrades for TH11/TH15 preserve DATA/music objects and local ownership. The
  second game reuses the shared font object, and a second launch reads no
  upgrade metadata. UI TypeScript checking and the 66 preparation/runtime
  service tests pass.

Rollback link:
`/var/www/eagler-touhou/rollback-before-runtime-font-raster-20261016-8847a6937297`.
Relay/TURN processes, upstream ports and network policy were not changed.
No real-device, long-duration netplay or GDI-pixel-parity acceptance is claimed.
