# TH11 THPrac adaptation — in progress

## Keyboard HUD presentation parity (2026-10-09)

TH11 uses TH15's default 34x34 KeyRectStyle, dark (32,32,32) text and
(1280,0)/(840,0) anchors. Per-title key masks and APS input recording are
unchanged; the existing font atlas already includes arrow/Delta/Sigma symbols.
Production ON WASM:
`a3ef22b2108c565a23920b6f16b8e5907595db069d3a46776455e9118a7b3d16`.

## Replay Miss observation fix (2026-10-10)

Miss counting now belongs to `PlayerFrame::die`, not the live-only
`GameBattle::record_death` callback. Replay observes the same committed death
transition while retaining the original prohibition on live Death score events.
The counter is incremented once; deathbomb recovery is not counted as a Miss.
The production build excludes the development-only death probe.

Browser regression passes for all six shots in zh-CN/en-US/ja-JP: after loading
an actual saved PRAC Replay, a native death increases Tracker by one and emits
zero live Death events. Existing restart, touch, C, stage-six/Extra and PRAC
save/restore regressions also pass. Physical-device/full-run oracle checks remain
manual. Production WASM SHA256:
`d62d294d0e30522e342bee2efafc4e70da73b7fd4a81b421757d85544501a15d`.

## Purple replacement (2026-10-09)

This section supersedes the blue-source identity and phase counts below. Older
sections remain historical evidence, not current purple completion claims.

### Recorded boundary

- Authoritative source: `thprac/thprac_purple`, commit
  `17780056107a014e88d35fceb3ad86c121d94287`, version `2.2.2.7`.
- Normalized TH11 source SHA256:
  `6236d13b9c62ac981fc5bb625a81d0e93553b67bf64a2095fd4d6bd1c7a4abc0`.
- Canonical Eagler branch/base: `th11`, `eagler`, `0a582e6`.
- Experiment: `_scratch/th11-thprac-purple`,
  `experiment/th11-thprac-purple`, based on `0a582e6`.
- Companion Launcher experiment: `_scratch/launcher-th11-purple`,
  `experiment/th11-purple-launcher`, base
  `70eb01f33b0726094fd102a850b53e76fd47c266`.
- Implementation references: TH15's source extraction/shared-tools/native ImGui
  boundary, plus the existing TH11 gameplay and Practice lifecycle owners.
- Integration status: isolated working implementation; not merged into canonical
  `th11`, not pushed and not deployed. No Launcher Runtime path is redirected to
  this experiment. Promotion still follows the worktree playbook's reviewable
  commit and canonical re-verification requirements.
- Handoff boundary: generated `th11_web/artifacts/th11-purple.patch` against the
  recorded base, with all new generated source headers included, plus companion
  `th11-purple-launcher.patch`. These are candidate patch artifacts, not accepted
  canonical commits. The experiment build directory is `build-eagler`.

### Changed owners

- Purple's 136 sections and exact patch order replace blue extraction. Added
  boss coordinates, stage-five wave controls/injected wave ECL, eight final-boss
  phases and the purple Extra phase catalog. Writes remain bounded,
  transactional and protected by the retail patch-site CRC inventory.
- The native wave array is 12024 bytes although the Windows code copies 12040;
  the portable adapter appends the real array only, avoiding the native
  out-of-bounds read. This ownership adaptation is explicit, not new game logic.
- F12 uses the purple TH11 group membership: speed, key filtering/fast retry,
  native keyboard HUD/APS CSV, infinite-life mapping, Hint, boss move-down,
  MASTER display routing, Marisa B formation lock, lock timer, All Clear Bonus,
  reaction test and About/license. Speed is authoritative simulation pace,
  not the Launcher's high-refresh presentation option.
- U enemy invincibility reaches the enemy damage owner. Backspace uses native
  green/bracket rows; Tab/F7 retain in-game information. All three source locales
  share the original font and keyboard glyph ranges.
- Practice config/replay metadata includes position/wave parameters. Old blue
  `2.3.0.3` Extra timeout recordings use a source-extracted replay-only legacy
  case; the removed blue phases are not reintroduced into purple's menu.
- Touch forwarding preserves pointer cancellation, persistent held-fire,
  Escape and the existing C functionality. Launcher declares extra U-key support
  as product capability rather than another game-name branch.

### Evidence

- Both generators' `--check` pass against the recorded source.
- All 366 section/phase/dialogue/chapter and old-blue replay cases pass using
  the Japanese retail archive, including actual decoded-stage loading,
  original-mode isolation, CRC checks and failed-transaction rollback.
- Shared input filtering, 15-frame retry, keyboard masks/APS/CSV and simulation
  cadence checks pass in the C++ resource harness.
- Browser regression passes zh-CN/en-US/ja-JP: original Practice entry/cancel,
  six shots, boss/chapter warps, Pause-R, PRAC save/restore, all special phases,
  portrait/landscape/tablet touch sliders/popups/cancel, held-fire continuity,
  existing C gap/formation behavior, F12 touch input-owner toggling, live Marisa B
  formation lock and U carrier.
- Default OFF compiles/links, advertises `thprac: false`, and excludes Practice
  configuration and test-only exports. OFF WASM SHA256:
  `567537b82011dd9548df53ab54fa97bc06544e59f1b0db9d21ac0e59c79309ea`.
- Packaged ON WASM SHA256:
  `4d4a9224c947efd20286f8e96cfbb1f89c5f50a0d197bd2d7785bdf7652b6654`.
- Launcher product-catalog tests and launcher TypeScript compilation pass.
  The full Launcher check still fails unrelated existing ownership registration
  and TH15 support-inventory expectations; these are not silently fixed here.

Unknown/not-run: physical Android/iOS/tablet validation, comprehensive native
oracle comparisons of every F12 feature, full-run stage-six replay clocks and
broad ordinary-game/localized-resource regression. Passing source/lifecycle
checks must not be described as complete device/native-oracle certification.

```powershell
$env:EMSDK = '<existing emsdk>'
$env:EAGLER_WORKSPACE = '<maintainer workspace>'
node portable/generate-thprac.mjs '<thprac/thprac_purple>' --check
node portable/generate-thprac-tools.mjs '<thprac/thprac_purple>' --check
node portable/check-th11-thprac.mjs '<Japanese th11.dat>' '<thprac/thprac_purple>'
node portable/build.mjs --thprac
node th11_web/tests/browser/thprac.mjs
$env:EAGLER_FONT_ROOT = '<private SDL-native fonts>'
node portable/package-eagler.mjs
```

The generator also reads sibling `thprac_blue` for historical replay-only
compatibility. Runtime packaging is distinct from a private game import ZIP;
shared `/unifont.otf` still must be supplied by the ordinary site's resource
provider. No private retail resources or baked fonts are committed.

The user requested the ordinary launcher option on 2026-10-03, with device
testing after their own deployment. TH11 now uses the same production entry and
prelaunch enable/locale options as TH08/TH10; no separate test product is added.
The remaining verification gaps below stay open, not relabeled as PASS.

Build the normal thprac-capable runtime with `node portable/build.mjs --thprac`,
then package it with `node portable/package-eagler.mjs` and EAGLER_FONT_ROOT set
to the baked font directory. The launcher publication must include shared
`/unifont.otf`. Build/Runtime manifests attest the actual compile flag; a build
without the flag remains vanilla and does not advertise thprac. The user-facing
launcher option still decides whether the compiled practice owner is enabled.

## Source authority

- TH11 base: `97019e9702eb6f99c523f3b2bde21fe499c3b6f7`.
- THPrac upstream: `585fae1aad4b720f350655a44e3f2ec7fc0dfa25`.
- `thprac_th11.cpp` LF SHA256:
  `af2c0a29b2d92bb41fca2d595614a5ea4aa08b0b5b9c0f0c3c5f3789663f6ea7`.
- Method references: TH08 and TH10 portable generators, config/runtime owners,
  native Dear ImGui menus, thin host bridges and replay adapters.
- Governance: launcher `docs/playbooks/thprac.md` and
  `docs/playbooks/adaptation-worktrees.md`.

## Implemented resource layer

`generate-thprac.mjs` extracts the upstream 136 sections, chapter/section
patch logic and three-language labels. Executable-address accesses are replaced
with owned buffers and the real stage-section owner, not new gameplay rules.
MIT provenance is retained. `--check` detects extraction drift.

Writes are bounded and transactional. Original ECLs are decoded strictly;
directory/name/ECLH bytes cannot change; inserted jumps must target original
instruction boundaries. On successful patching, verified directory metadata is
kept and instruction bytes installed before ECL pointer binding. Short native
replacement instructions intentionally leave unreachable operand bytes; those
bytes must not be interpreted as a new linear instruction stream.

`load_practice_stage` preserves native include ordinals, patches ECL/STD/ANM
before loading, and clears transient caches on every exit. Stage 6's inserted
STD sentinel retains byte-identical original tail instructions for the native
late jump; normal STD parsing remains unchanged.

The test harness checks all 327 section/phase/dialogue/chapter cases on the
retail archive, actual decoded stage loading, Original-mode isolation, invalid
configuration, auxiliary-buffer truncation, and transactional rollback.

Local evidence (2026-10-03): extraction check and all 327 resource cases pass.
Ordinary `portable/build.mjs` compiles/links all 106 translation units; WASM
SHA256 `916738779f5411395826f3ce7712c6dd9ad6991575d031ca0d8af7e3888d4124`.
Its manifest still correctly reports `features.thprac: false`. These checks
are source/resource/build evidence only, not gameplay or browser evidence.

```powershell
$env:EMSDK = '<path to existing emsdk>'
node portable/check-th11-thprac.mjs '<path to th11.data>' '<path to thprac repo>'
```

## Native integration and additional evidence (2026-10-03)

- Original Practice/difficulty/character/partner entry opens native Dear ImGui,
  with the upstream TH11 locale sizes and section/category/phase catalogs.
- Backspace, Tab and F12 are native windows; the host relays keys and pointer
  coordinates only. ImGui targets TH11 screen handle 1 without a depth surface;
  TH10's depth sentinel is not a valid TH11 handle.
- Separate pending config and live run owners; native Pause-R preserves the
  run, cancel/exit clear it. Vanilla and custom replay owners reject cheat keys.
- USER/PRAC metadata is appended after native/touch metadata and restored only
  for the matching selected stage. Assisted recordings can also be saved;
  the live assisted marker is retained, but playback does not reapply cheats
  and deterministic reproduction is not guaranteed (matching TH06–TH08).
- F1..F5 reach the native player/enemy/spell owners; F6 BGM uses upstream
  ElBgmTest state transitions. Initial boss music, stage-six intro skipping and
  stage-logo conditions follow THBGMTest and the original source hooks.
- All Clear Bonus follows 41eb9a/41eca6/41ed4a. Local retail disassembly confirms
  the early Practice bypass and post-award Practice exits. Stage-six/Extra
  live and replay tests verify awards without incrementing full-clear records.
- 405 CRC32 patch-site ranges are regenerated from all source patch cases.
  Changed instruction-site bytes are rejected transactionally; this is not a
  whole-file hash gate. CRC inventory drift is checked by the resource harness.
- Browser automation passes zh-CN/en-US/ja-JP: original entry/cancel, six shot
  types, chapter/boss warps, real Pause-R retry, PRAC save/restore, exit/re-entry,
  Backspace/Tab/F12, stage-six and Extra special phases (120 live ticks each).
  Screenshots were inspected: the native menu renders over the game correctly.
- THPrac ON and default OFF compile and link. OFF omits the configuration
  export; ON now attests the optional production capability, requested by the
  user for post-deployment testing through the ordinary launcher toggle.

Latest ON WASM SHA256:
`aba813872a5b0703e5cec606879c4617d656b18759f3002f756c8ea2d8ceebda`.
Latest default OFF WASM SHA256:
`85c1d3277764903fc4c6dd20416dc966adede8ed87960b5c4501fa21f11b1e6b`.
Retail archive used for CRC generation (not included in Git):
`3cb521c5d420d8cbad6494d53bcdb048bcc396abf95c18fac68e38fd2d19f8e2`.

The stage-six replay STD fix is ported after opcode 3. Its clock mapping uses
native priority-10 current = portable end-of-tick frame + 1. Full-run replay
transition/fast-forward comparison against the native game remains required.

### Mobile Practice input closure

TH08/TH10 already provide the shared launcher Tab, Backspace and F12/function
buttons, native key-bit bridge and scaled `thprac-mouse` carrier. TH11 reuses
them; no duplicate HTML Practice UI or extra mobile button layout is added.
The missing normalized `sdl_touch`/SDL-finger -> native ImGui route is now
connected at TH11's common touch-event owner (640x480 logical coordinates).
Gesture-derived keys are merged before ImGui's input tick, matching TH10.
Cancellation releases the native pointer as well as the gameplay gesture.
ImGui window/popup hit regions consume pointer gestures, preventing a second
synthetic Z activation alongside native widget input. TH08/TH10's existing
popup-open guard on SetWindowFocus is also preserved: returning focus to the
main Practice window while a combo is open otherwise closes that popup.
Outside the
native window, ordinary tap-confirm/two-finger-cancel gestures remain owned by
the existing shared TouchController.
Browser tests exercise the real shared direct-touch carrier at portrait,
landscape and tablet sizes in all three locales; sliders change pending native
parameters, stage combos open/select, cancellation stops dragging, and widget
taps do not accept runs. Outside-window tap-confirm and two-finger-cancel pass.
Native Backspace/Tab/F12 window visibility is asserted against the shared
key-bit carrier, in addition to the original desktop/lifecycle regression.
This is browser-emulated mobile evidence, not physical Android/iOS certification.
Launcher capability is published for user-requested deployment testing; this
does not claim physical-device or native-oracle certification.

```powershell
$env:EAGLER_WORKSPACE = '<maintainer workspace>'
$env:EMSDK = '<existing emsdk>'
node portable/build.mjs --thprac
node th11_web/tests/browser/thprac.mjs
```

## Required before promotion

### Mobile held-fire regression, 2026-10-04

After Practice acceptance, the previous render owner waited for Z/X/Enter/Esc
release before clearing menu capture. A mobile fire toggle continuously supplies
Z in gameplay, so it could keep capture enabled indefinitely until toggled off.
Capture now ends when the native Practice menu closes, independently of the
gameplay fire owner. The browser regression fails before this fix and passes
after it: fire remains ON through selection, entry, keyboard/lifecycle clear and
Pause-R retry, and turning fire OFF stops it. All three locales and the portrait,
landscape and tablet viewports pass alongside the existing lifecycle cases.
This bounded fix is based on Eagler commit 60cfe04 in the isolated THPrac
worktree; native-oracle/physical-device gaps above are unchanged.

- Native-vs-portable live cheat/time-lock/auto-bomb/BGM oracle comparisons.
- Full-run stage-six replay transition/fast-forward clock proof.
- All Clear Bonus stage-one through stage-five transition lifecycle proof.
- Launcher packaging/capability integration and real phone/tablet verification.
- Broad ordinary-game regression testing, including translated resources.
- Default-OFF build gating and truthful build/launcher capability attestation.
- Automated lifecycle and browser/device verification; no UI-only proof.

Do not describe the still-open native-oracle/device gates as complete THPrac proof.
