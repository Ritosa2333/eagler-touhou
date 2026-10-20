# TH12 Purple THPrac integration

TH12 uses native ImGui practice, cheat, advanced and information windows.
`src/wasm/PracticeUi.cpp` follows the TH10/11 platform adapters and uses the
shared purple input, speed, key HUD, reaction test and mobile panels from
[`common/docs/thprac.md`](../../../../common/docs/thprac.md). The browser adapter
transports pointer/keyboard input, shared font bytes and cached `ImDrawData`.

`UiCatalog.hpp` imports the three-language labels, selection groups, version
2.2.2.7 and MIT license from `thprac_purple`. `purple-ui-source.json` records
their source digests. The existing native TH12 section catalog, transactional
ECL patches and replay parameter schema own the practice configuration.

## Source and redistribution boundary

`source-provenance.json` records the supplied TH12 source's SHA256, the pinned
official definition revision, 71 section IDs, 43 chapters and the private retail
inputs used to compute 1405 instruction-site digests. `PracticeSites.hpp` stores
only stage/file ordinal, offset, instruction length and the CRC32 of the expected
16-byte header. It contains no original ECL header or argument bytes. Whole-file
hashes identify the local source inputs; they are not used to exclude a translated
file whose required instruction sites still match.

The provenance file's evidence fields retain the source-generation checkpoint
(`isolated-experiment`); current integration and compilation status are recorded
in this document, without promoting that checkpoint to runtime acceptance.

The provided `reference/thprac_th12.cpp` owns THStageWarp, THPatch, parameter
defaults, spell phases and resource conversion. The official JSON at revision
`72f20e99dbf5b74319db7aaec25202d23d584486` supplies enum order, names and
stage/difficulty availability. Public source and `THPRAC-LICENSE.txt` (MIT,
Ack 2022) are retained here. Original game resources are not redistributed with
this source.

Shared dependencies include `PracticeInput`, `PracticeKeyMonitor`, `PracticeSpeed`,
their UI fragments, `PracticePointer`, `PracticeMobilePanels`, common ImGui and
FreeType. `integration/common-dependency.json` pins the shared source hashes.
`scripts/practice-build.mjs` inventories translation units and headers for both
compilation and runtime source fingerprints.

## Rebuild and source generation

`npm run build:eagler` builds the C++ core, Vorbis decoder and code-only Runtime
in sequence, with `TH12_THPRAC=1` unless explicitly overridden. Select the pinned
common checkout with `EAGLER_COMMON_ROOT` when using a local workspace. The core
maps that flag to `TH12_ENABLE_THPRAC=1`. `TH12_THPRAC=0 npm run build` on a POSIX
shell, or setting the equivalent PowerShell environment variable, builds the
ordinary core without the common/UI dependency. Runtime configuration cannot
enable THPrac in a core built without it.

Regenerate or check the purple UI catalog from the supplied source tree:

```sh
node scripts/generate-purple-ui.mjs /path/to/thprac/thprac_purple --write
node scripts/generate-purple-ui.mjs /path/to/thprac/thprac_purple --check
```

Native UI checks use CMake with `TH12_ENABLE_THPRAC=ON` and FreeType (an installed
package or `FREETYPE_SOURCE_DIR`), then `ctest --test-dir BUILD -C Release`.
From the monorepo root, `node tools/ports/th12-practice-browser.mjs` checks the
actual Wasm/ImGui UI with the shared Unifont and writes desktop/mobile screenshots
to `artifacts/th12-purple-ui`. These checks cover UI/input behavior on a synthetic
game surface; game-resource checks use the acceptance paths below.

To regenerate public patch extraction and digest tables from private local
retail input, use:

```sh
node src/practice/generate-source.mjs --assets=/path/to/private/retail/assets
```

The generator's default asset path targets the development workspace's preview
assets, so other layouts should pass `--assets`. Generation reads files; it does
not execute ECL or the game. Patch writes and jump destinations are guarded in an
owned transaction. The pristine files first pass strict structural parsing and
establish routine pointers. Fixed-source patches are then copied into the same
owned buffers without changing file sizes or routine metadata. Native THPrac
jumps intentionally skip overwritten instruction remnants; those dead bytes are
not reinterpreted as a linear instruction stream. Every written byte must be
covered by guarded original instruction sites; jump targets retain exact-site
checks. Publication happens before main-thread birth. Failure leaves the prior
Program intact and produces a visible error. Vanilla mode never applies a
remembered custom section.

## Connected lifecycle and core behavior

The normal entry remains Practice → difficulty → character → shot → the original
Practice stage-select position. `TitleScenes` uses the optional `practiceEnabled`
and `practiceLocale` arguments to place the parameter body there. It preserves the
original background, shot and heading; the pointer-capable body selects actual
parameters. Start and Retry retain `run.practice`.

The Runtime handles fresh-menu reset, stage loading, parameter configuration and
`PracticeBridge.start`. Native file ordinal 0 maps to the loaded stage ECL
(`program.files[1]`), while Extra's ordinal 2 maps to `stage07boss.ecl`.
`GameSession::startGame` calls `practice::applyBirth()` immediately before main's
synchronous birth. Resource initialization therefore uses the original score/10,
nonzero life fragments +1, bomb fragments ×2, point value ×100, selected
power/graze, token side and left-to-right token inventory at the real boundary.
Original Practice (`mode=0`) applies no custom ECL or resource changes.

`th12_select_stage` retires the prior ECL generation. Fresh configuration clears
stale replay state; title/menu reset clears the session, and profile OFF also
clears persistent tools. Every Replay explicitly owns its replay state. Custom
practice playback calls `prepareReplay` before the native stage-header/input path
starts, so its ECL patching does not replace recorded initial resources.

| Connected seam | Source-owned behavior |
| --- | --- |
| Player collision, stock decrement and power loss | F1 invulnerability, F2 lives, F3 bombs and F4 power act at the actual game operations. Other miss/bomb/drop effects retain their normal paths. |
| Enemy health subtraction | U preserves enemy health through the native damage path. |
| ECL random movement range | The boss move-down option applies purple's lower-range transform before RNG sampling. |
| Player stock decrement | Infinite-life mapping lets stock reach zero and holds it there without entering Continue. |
| `beforeTick` before Player16 | F6 replaces the deathbomb key requirement only while state4 has stock and its window remains open. |
| Spell bonus branch | F5 skips bonus decay; spell age, capture state and presentation still advance. |
| Enemy update tail | F5 freezes only the phase-age integer at normal game rate. The float clock, ordinary age and slow-rate path retain their source behavior. |
| Dialogue final award | The F12 all-clear setting uses the normal final-award calculation. |
| Music selection and commands | Section BGM/dialogue determines initial music; F7 uses the practice music guard on its connected command paths. |
| Miss, bomb, token pickup/spawn and UFO birth | Tracker counters observe the real events, including events with infinite stock enabled. |

Live tool effects and mutations are disabled during Replay. Speed controls use
the shared logical interval independently of presentation refresh: normal
60–6000 FPS, replay slow 1–60, replay fast in 60-FPS steps, and the source's 9999
rate above 1200. Space follows the optional debug-acceleration setting.

## UI and input boundary

`PracticeOverlay` is constructed with the real `PracticeBridge`, canvas and
`model.practiceAction` callback. The Runtime updates its scene, replay owner,
menu and readiness on the UI cadence. Native ImGui runs once per logical input
tick; presentation renders cached data. Short key and pointer edges survive
between ticks. Numeric fields preserve the purple bounds, including 0–9 lives
and bombs, score/value rounding, native combos and native drag/slider behavior.

F12 opens advanced settings; Backspace opens the cheat window. Its F1–F7 and U
shortcuts require an active game and the open cheat window; Tab controls the
information window. F12 includes the shared speed controls, disable-key/forced
Shift options, fast retry, APS recording/CSV, life mapping, boss range, chromatic
UFO counts, complete capture-history display, lock timer, all-clear bonus,
reaction test and About/license. Tool labels follow `thpracLocale`.

The practice window captures its menu input. During play, an expanded Advanced
window captures gameplay. Shared input filtering precedes the native player
tick; the recorder stores the resolved input returned by `th12_practice_input`.
Replay playback skips live input filters and gameplay cheat mutations.

Launcher `touch-controls` may carry `thpracMobilePanelsEnabled` and
`thpracMobilePanelsVisible`. Reopening the group starts cheat, advanced and tracker
headers collapsed. Visibility does not stop authoritative counters or effects.
`thprac-mouse` uses Runtime viewport CSS coordinates; direct-touch's normalized
coordinates are converted before reaching this pointer path. Ownership is fixed
on down, retained through outside release/cancel, and does not transfer merely
because a game drag crosses a header. Pointer actions invoke the same menu or
core operations as keyboard actions.

The active UI finger owns ImGui's mouse state. Cancelling another game finger
preserves that UI finger. Lifecycle cancellation clears the pointer queue and
native active widget; keyboard clearing preserves active pointer gestures.
Native fields and scrollbars handle touch, while desktop wheel input scrolls
the hovered native window. Shared `/unifont.otf` initializes the FreeType atlas
before the native UI starts. The WebGL transport converts common ImGui's BGRA
vertex colors to the browser framebuffer.

## Replay extension and compatibility limits

Practice metadata is stored in the optional `practice` field of the single
`TH12WEB1` extension. Its payload retains schema `th12-web-motion/1`, coordinate
scale 128, resolved movement rows and optional `damage`. A file needing none of
these Web fields receives no extension. There is no second practice tail block
or Replay sidecar.

`practice` uses `eagler-touhou/th12-practice/1`; `damage` uses the separate
versioned semantic contract described in [DAMAGE-SOURCES.md](DAMAGE-SOURCES.md).
Runtime saves preserve the assisted marker and the ordinary Replay save path.
Playback restores recorded parameters and inputs, while live cheats remain off;
deterministic assisted playback is not guaranteed.

ReimuA full-run stage-entry snapshots and real adjacent-stage carry are wired
before main birth when the THPrac profile is enabled. Playback of an embedded
damage snapshot also requires that profile. Launcher Replay workspace enables
the playback profile automatically while keeping interactive ImGui hidden.
Raw native 0x74-byte damage-source
blobs cannot be imported.
Missing data in older native replays is reported by the compatibility state as
unavailable. The game-resource and device acceptance checklist is in
[WEB_ADAPTATION.md](../../docs/WEB_ADAPTATION.md).
