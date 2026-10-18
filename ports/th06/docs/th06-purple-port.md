# TH06 purple migration — integrated into eagler

Implementation base: `fa4d48f`, `experiment/mobile-thprac-panels` worktree. Existing mobile
panel changes are preserved. Purple source authority is
`thprac/src/thprac/thprac_th06.cpp`; the generator checks the normalized,
BOM-stripped SHA256 `114a55f6c321a83acf2dea28c29893dd2821a25b1088bdb8683bbc5918d9bb81`.

Implemented first tranche:

- Native green/bracket Backspace rows; mobile checkbox actions, U enemy
  invulnerability; F12/U browser keyboard carrier fallback.
- Shared purple input restrictions (merged keyboard/controller/touch input),
  Replay and advanced-window guards.
- Exact two Player rank-drop sites (bomb/death), never generic item rank loss.
- Native infinite-stock/no-continue mapping, and fast retry at the confirmed
  miss owner with the shared 15-tick Escape/R sequence.
- Native boss movement bounds formula, applied only at ECL bounds assignment.
- Fixed seed at game initialization with native stage/section gates; Replay
  remains authoritative and exposes the last loaded seed.
- Shared key HUD/APS/CSV (browser download), one authoritative Player callback
  sample, native drawing order with 1280-to-640 coordinate scaling.
- Extracted native bullet/laser/enemy collision drawing, Replay marker, Rank
  in the existing information window, shared in-game reaction test.

Second tranche:

- Source-owned portable extension replaces the archive's blue section owner.
  Native purple THPatch and ECLJump/SetHealth/SetTime/Stall are extracted with
  an explicit source pin. All 106 section identities remain stable; native
  THStageWarp is checked equivalent to the previous implementation.
- Candidate ECL bytes and derived context are committed only after every
  bounded write succeeds, retaining the retail stage CRC gate.
- Book phases, six fixed-position flags and X/Y controls, mirror/flip/rotate
  and Y reset; Sakuya doll phases; final 56-way variants and 0..600-frame delay.
  Extended native fields travel through the host session and PRAC trailer.
  Fresh accepts versus restarts preserve the native conditional field writes.
- MSVC mt19937/range-reduction compatibility for the doll random phase was
  compared against the locally installed MSVC STL over all 65,536 seeds;
  the same Web test digest is `2316479866`. This is not a proof against every
  historical compiler used to build THPrac. A generated index of 640 wraps to
  zero at the typed ring-buffer owner, avoiding an out-of-range dereference.
- Old trailers without purple fields retain blue section patch semantics;
  new trailers emit the reviewed purple source version `2.2.2.7`, not the
  old blue version. Archive-only license UI dependency removed.

Third tranche:

- Wall-practice hooks at the typed ECL movement / ExIns owners, including
  native eligibility tests, branch thresholds, two-draw RNG sequence and
  near/far snipe percentages. Purple replay parameters run the same hooks;
  unrelated live cheats remain replay-guarded.
- Native book random X, copied/pasted six-pair positions, book failure,
  timing and capture counters; the browser paste adapter is asynchronous.
- TH06Save's exact version-1 data layout, spell/session/time tables, 65 native
  spell labels in all locales, legacy `spell_capture.dat` import, and current /
  total counts in Tab. Only `/savesth06/score06.dat` is written, not vanilla
  `score.dat`; the existing IDBFS autoPersist owner flushes it.
- Lock-time counter at the confirmed boss callback; pink life-threshold bar
  rendered through a temporary VM, without mutating the authoritative ANM.
- Extracted native rotating player-hitbox animation / PNG and SSS blind PNG;
  optional external `hitbox.png` / `blind.png`, reload and graphics shutdown
  have explicit GL texture ownership and preserve pixel-upload state.
- Native background-fix equivalents: STD fog assignment (0x4039e5), whole
  frame clear (0x42073b), HUD redraw conditions (0x419f4b / 0x419f81).
- Shared logical Game Speed and replay Ctrl/Shift/Space controls, cadence
  reset on period changes, authoritative rendering at non-base speeds. This
  is not Launcher high-refresh. Shift+C rank chord is active inside F12 only.
- Native default infinite-stock mapping is enabled. Keyboard HUD uses native
  GameGuiInit colors / spacing; recording controls appear only when enabled.
- Conditional auto-shoot uses the shared native toggle/cancel algorithm.
  Optional Runtime `thpracAutoShootKey` is a validated VK slot (-1 disables).
  No default trigger or extra Launcher UI was invented; the F12 option only
  appears when a trigger is configured, matching native DisableKeyOpt.

Scope boundaries: F12 switches remain process-local as in native ContentUpdate;
the native LauncherSettingGet defaults, pause-BGM/auto-name launcher preferences,
Win32 window controls and injected input-backend diagnostics are not additional
F12 features and were not copied into the web menu. Persistent statistics and
recorded practice parameters have separate native-compatible owners.

The source-owned extension is `portable/AttachReallyportable.cmake`. The archive
is a source import reference, no longer a required workstation runtime path.
Legacy blue ECL statements are retained solely for old replay dispatch.
Generated labels are not proof that every corresponding option is supported.

Build dependency: the previous SDL pin is not fetchable from its official
remote. This experiment uses official SDL 3.4.12 `f87239e71e42da91ca317a12eefb82cfbf3393eb`,
as in the TH07 build repair. Only C++ sources force-include cstdint (the title
also contains stb_vorbis C). Current SDK requires Safari minimum 15.

Verification lanes: generator/source checks, shared executable input/APS/CSV
test, full THPrac ON and OFF builds, and headless browser lifecycle / visual
checks. Retail Replay golden and actual phone checks remain pending lanes.
The dated verification records below describe the isolated migration before
promotion; they are historical evidence, not the current branch status.

Recorded 2026-10-10: full ON and OFF Web builds passed; source gates and
executable shared input/APS/CSV test passed; common Release suite passed 34/34.
Purple label and full-width REP glyphs are included in the locale font ranges.
Device visual verification remains pending. The final ON artifact is under
`build-purple-thprac/th06.{html,js,wasm}`, with external game assets/font mounting.

Regeneration uses an explicit development common root:

```sh
node scripts/generate-purple-thprac.mjs <purple-checkout> --common=<common-checkout> --check
node scripts/check-purple-thprac.mjs
node scripts/import-portable-th06.mjs <reallyportable-reference> <purple-checkout> --check
```

Second-tranche checks: executable native book/final-wave byte assertions,
636 section/phase bounded patches, extended-field roundtrip and legacy dispatch
decode; native and Web random-seed compatibility. Retail playback, rendering
and actual phone interaction remain pending, not implied by these tests.

Recorded 2026-10-10 second tranche: full THPrac OFF then ON builds passed with
the local extension; generated-source checks, extended section test, legacy
session/ECL unit tests and JS/WASM load validation passed. The final artifact
is again the ON build under `build-purple-thprac`. No promotion or deployment.

Recorded 2026-10-11 third tranche: full OFF and ON Web builds passed, with the
final artifact restored to ON. Source/import gates, JS parsing and WASM
compilation passed; common Release suite passed 34/34. Executable checks cover
the save layout and counter bounds, wall branches and RNG consumption, shared
input/auto-shoot/speed/APS/CSV, 636 section/phase patches and replay-field
roundtrips, plus the MSVC-compatible doll-seed digest above.

Headless Chrome checks opened Backspace, F12 and Tab before gameplay without
an exception, scrolled the full advanced list, opened native statistics and
SSS, then entered actual stage-1 gameplay. Screenshots were inspected for the
blind mask, Tab counters, native labels, mobile collapsed headings and a
clickable F1 checkbox. No browser page error was recorded. The browser lane
uses mouse-driven mobile protocol settings, not a physical phone; it does not
establish native retail Replay equivalence or exhaustive option behavior.
Artifacts and `report.json` are under `build-purple-thprac/browser-check`.

The reproducible browser lane mounts private assets without copying them into
source control:

```sh
python scripts/check-purple-browser.py --pack <th06-offline-game-directory> --font <shared-unifont.otf>
```

Final whitespace checks passed. Changes remain in this experiment worktree;
no commit, push, canonical promotion or deployment was performed.

## Eagler integration, 2026-10-12

The canonical `eagler` branch now includes the complete source-owned migration
through `3397c35`, including shared purple practice and mobile panel owners.
The integration preserves the SDL repair and pins eagler-common to `52bc86f`.
The historical experiment and browser verification records above remain valid
only within their stated coverage. Promotion does not establish exhaustive
retail Replay equivalence or physical-phone verification. No deployment is
part of this branch-integration task.
