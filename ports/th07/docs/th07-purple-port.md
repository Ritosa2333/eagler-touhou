# TH07 purple THPrac integration

## Current integration checkpoint — 2026-10-11

The purple experiment (`510bc39`) is integrated into the mobile-panel worktree
(`0754c2e`, `experiment/mobile-thprac-panels`). Previously the published mobile
branch did not include the purple implementation. This explains why the test
site continued to show the BLUE implementation. This checkpoint is local only;
it does not change canonical checkouts, published branches or deployed sites.

Connected after the original checkpoint:

- Use eagler-common `PracticeInput`, `PracticeKeyMonitor`, `PracticeKeyHud.inc`,
  `PracticeReaction.inc` and `PracticeSpeed.inc`; remove private copies of the
  keyboard/reaction tools. Generate native three-language labels for shared UI.
- Trigger the native 15-tick fast retry at the actual death/life-decrement
  owner. Escape is bit 0x8 and R is bit 0x4000 (not Q). Count logical updates,
  not presentation frames. Disabled THPrac and Replay inputs pass through.
- Connect F12 game speed to the logical simulation interval, not Launcher
  high refresh. Keep LAN rollback on its fixed logical clock.
- Give shared reaction tests raw held-key state independently of vanilla
  Practice/ImGui navigation pulses. Z/X remain excluded from ImGui Activate
  and Cancel. Retain held state on presentation-only frames.
- Restore tracker cherry, cherry-plus and cherry-max alongside the three
  existing miss/bomb/border-break counters.
- Preserve mobile panel placement, pointer routing and original managed DATA
  preloading. Build with the owned `portable/AttachReallyportable.cmake`.
- SDL is pinned to the fetchable official release
  `f87239e71e42da91ca317a12eefb82cfbf3393eb`, replacing the unavailable pin in
  the historical checkpoint below. Shared tools are built from the explicit
  workspace eagler-common root at `52bc86f`.

Current verification:

- Full THPrac-enabled Web build/link: PASS.
- Full THPrac-OFF Web build/link, followed by restoring the ON build: PASS.
- Generator source/hash validation: PASS (103 sections, 43 named hook sites).
- Actual ImGui widget/HUD/reaction rendering and all three locales: PASS.
- 2,097,152 input combinations, fast-retry Escape/R ownership and logical
  speed live/replay/slow/fast/debug selection: PASS.
- Original retail ECL identity, transactional failures and 22,248 section /
  phase / difficulty / shot / dialogue dispatch combinations: PASS.
- Local candidate preserves original DATA hash, byte size and preload layout.
  Desktop/mobile browser startup, launcher-enabled THPrac and opening F12 /
  Backspace / Tab without fatal errors: PASS. Screenshots and results are
  recorded under operator-local `prepared/th07-purple-integration-20261011`.
  These are browser viewport tests, not physical phone gesture evidence.

Physical phone interaction, long replay golden comparisons and all individual
F12 option lifecycles remain separate acceptance gates. Source-matrix tests
are not proof of every full-game sequence. Do not label this checkpoint as
full device acceptance or deploy it without a deployment request.

## Historical isolated checkpoint

The following records the earlier partial experiment before the integration
above. Its missing-dependency and unconnected-tool notes are historical, not
the current build status.

## Source and ownership

- Canonical Eagler base: `dccaf4e` (`eagler`).
- Experiment: `_scratch/th07-thprac-purple`, `experiment/th07-thprac-purple`.
- Purple authority: `thprac/thprac_purple`, commit
  `17780056107a014e88d35fceb3ad86c121d94287`.
- Normalized TH07 native source SHA-256:
  `b5be6dc698f574992ab49696c5c0d8ad756af93bf0760be1504ee9e958bee1b0`.
- Development authority: Eagler `ADAPTING_A_GAME.md`, THPrac playbook and
  adaptation-worktree isolation playbook.
- Generated inventory identifies 103 section names and 43 top-level named
  EHOOK/PATCH sites. It is NOT proof that the sections/hooks are implemented;
  macro-generated hotkey patch sites additionally require their own audit.

## Connected in this checkpoint

- Native `GuiHotKey::OnWidgetUpdate`: text, green active color, brackets,
  measured invisible-button bounds, clickable toggle, balanced color stack.
  The generator only substitutes formatting and removes the external locale
  lookup; original implementation supplies widget behavior.
- Backspace rows F1–F8 and U. F8 information shortcut retains the existing Tab
  alias. Browser code/key/keyCode forms reach the C++ F12/U/F8 bit owner.
- U follows native `mEnemyMuteki` patch `0x421447`: suppress only the enemy
  life subtraction. Retain computed damage, lastDamage, score/cherry/collision.
- Native disable-X/Z/Shift and force-Shift filter statements, including C's
  dependency on disabled X and force-Shift precedence. Filter the merged live
  gameplay sample, after keyboard/controller/touch merging and before recording.
  Preserve Replay and THPrac-OFF input. ImGui navigation owns separate raw input.
- Native no-continue infinite-life mapping (`th07_inf_lives`, `0x441171`)
  applies the pre-decrement `< 1` gate. Preserve the native F2 game-over bypass
  when the stock lives are zero; do not write a synthetic positive life count.
- Extracted purple MIT license is independent of the external GUI extension.
  Practice configuration and Replay provenance still retain the BLUE identity
  until the complete purple profile can be promoted.
- User-supplied `thprac-reallyportable.zip` SHA-256:
  `ef423e0560bcb79ad26b592cd04b4458ce600f3748ccf8a936646866bfa60820`.
  `scripts/import-portable-th07.mjs` imports source only, with per-file hashes
  in `portable/IMPORT.json`; no .git, binary, game DATA or build output.
  The imported adapter is now owned in this experiment and is the default
  enabled-build extension; ordinary THPrac-OFF builds remain unhooked.
- Purple and recovered BLUE section definitions, chapter and THPatch bodies
  are identical after whitespace removal. Keep the legacy Replay IDs and
  instruction semantics; generate all 103 section patches from purple source.
- Adapter applies patches against a private byte copy and commits bytes and
  context only after all bounded writes succeed. Preserve mandatory retail
  CRC checks; no new permissive translated-ECL fallback.
- Source-equivalent projectile/laser hitbox rendering, lock timer reset hooks
  (GUI creation / ECL 99 / ECL 114) and logic-only countdown-setter observation.
- One-key death uses actual lives/bombs and PLAYER_STATE_DEAD; retains other
  hotkey switches, suppresses Replay mutation and never manufactures RNG.
- Native key HUD shape, colors, alignment, TH07 key masks and 60-tick APS.
  The player update owns sampling (one P1 sample, not presentation frames).
  Record/stop/export APS.csv and plot integer samples, avoiding the native
  vector<int> to vector<uint8_t> reinterpretation error.
- Native reaction-test class and state machine extracted from the purple shared
  tools; SDL nanosecond counter replaces QueryPerformanceCounter, a private GUI
  PRNG replaces GetRndGenerator (never gameplay RNG), and merged raw button edges
  replace Win32 keyboard queries. Reset on collapsing the F12 section.
- Always-save-score/spell-history option uses the native state-18 temporary
  ResultScreen AddedCallback / WriteScore / ReleaseScoreDat sequence at the two
  actual pause-menu quit/retry transitions. Skip Replay, disabled THPrac and
  existing persistent-write exclusions. Free owned score-list nodes without
  invoking DeletedCallback's unrelated shared texture/chain teardown.

## Checks actually run

- Generator `--write`, then `--check`: PASS.
- Actual bundled ImGui regression: cold-open, measured click/release, on/off,
  twelve frames, draw-data and item/color-stack balance: PASS. This is NOT
  physical touch or browser-gameplay evidence.
- 2,097,152 input samples, OFF/Replay passthrough, unrelated-bit preservation,
  C dependency and Shift precedence: PASS.
- Browser hotkey fallback mapping and production-owner source boundaries:
  PASS (source evidence, not full-game execution).
- Emscripten syntax checks of modified production PracticeRuntime,
  ThpracImGui, EnemyManager, Controller and Player with THPrac enabled: PASS,
  without the missing portable extension. Existing format-security warning in
  BuildUpstreamReplayJson remains unchanged.
- Ordinary THPrac-OFF stub, Controller, EnemyManager and Player syntax checks:
  PASS. No generated game content is included.
- Full link/build, original demos, long Replay golden checks, browser/phone
  interaction and public deployment: NOT RUN.
- Recovered portable core Replay decode and CRC / expected-byte / transactional
  failure tests: PASS.
- Production adapter tested against the original local TH07 DATA (all eight
  retail ECL CRC identities matched): 103 sections x 3 phases x 6 difficulties
  x 6 shots x 2 dialogue states = 22,248 cases PASS. This tests patch dispatch,
  bounded native patch execution, context and fail-without-mutation behavior;
  game-start/stage-background and long Replay playback remain separate gates.
- Actual recovered ImGui source, hotkey and key HUD rendering / masks / APS /
  CSV regression: PASS. Existing 2,097,152 input cases remain PASS.
- Reaction-test cold-open/reset/render in all three locales: PASS. This is NOT
  a complete press/release timing, physical input or high-refresh latency test.
- Enabled syntax checks of ResultScreen, AsciiManager and PracticeRuntime with
  the native pause-exit save owner: PASS. Persistent score-file lifecycle and
  actual browser quit/retry remain unverified.
- Production syntax checks WITH the formerly missing portable extension:
  PracticeRuntime, EclManager, Gui, Player, EnemyManager, ThpracImGui and the
  recovered adapter: PASS (Emscripten SDL3 ports used for this compile lane).

## Remaining before promotion

- Source recovery is closed. Older source audits still reference missing
  `th07-eagler` paths; migrate their configured reference paths without weakening
  assertions before claiming full acceptance.
- Verify all native practice fields, 103 sections, timeline/health/time/dialogue
  patch paths, Extra/Phantasm lifecycle, and blue replay compatibility.
- Connect and verify the remaining native F12 owners: fast retry and game speed
  (logical cadence, not Launcher high refresh). Run full lifecycle/device
  verification for the connected reaction test, pause-exit score saving,
  HUD, lock timer, hitbox and one-key death. Do not import
  Launcher-only tools.
- Audit information/tracker counters and native save/read/checksum/MIDI hooks.
- Materialize pinned build dependencies, build a source-only Runtime and run
  lifecycle, replay determinism, enabled/OFF and mobile acceptance gates.
  eagler-common `3d54af473a9b8b900bf4aadecc6a318cf6a49e35`, SDL_image
  `bec9134a26c7d0f31b36d6083c25296e04cabff5`, SDL_ttf
  `a1ce3670aec736ecbf0936c43f2f0cc53aa61e5b` and their dependencies are
  materialized. SDL `0a61b59af1eec0c89b06b13bd4fd0f2ecdc3aff6` cannot be fetched
  from the configured `libsdl-org/SDL` origin ("not our ref"). The incidental
  checkout created by submodule initialization is NOT accepted as a replacement
  pin and must not enter a commit/release. Full build remains NOT RUN.
- Review and promote the complete experiment diff into canonical Eagler only
  when its owners and evidence are closed. No deployment or push is authorized
  merely by this checkpoint.

Run the current isolated gates with explicit source/tool roots:

```text
node scripts/generate-purple-thprac.mjs <purple-checkout> --check
EMSDK=<local-sdk> THPRAC_IMGUI_ROOT=<reviewed-imgui> node tests/test-purple-menu.mjs
node scripts/import-portable-th07.mjs <recovered-reference> <purple-checkout> --check
EMSDK=<local-sdk> TH07_RETAIL_DAT=<original-DATA> node tests/test-purple-portable.mjs
```
