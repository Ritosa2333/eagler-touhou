# TH10 purple THPrac source migration

Implementation and automated acceptance completed on 2026-10-09. No remote
push or site deployment is part of this change. Private game DATA/fonts and
browser evidence remain excluded from Git.

## Provenance and ownership

- Eagler base: `8f5138738777e472da3b1ae235fe9b03c97711dc`.
- Experiment: `_scratch/th10-thprac-purple`, branch `experiment/th10-thprac-purple`.
- Purple upstream: `thprac/thprac_purple`, commit
  `17780056107a014e88d35fceb3ad86c121d94287`, version `2.2.2.7`.
- Normalized TH10 source SHA256:
  `5d1691429d58f3005f3c15f68bba66d8fe25a38842cffe356869d48c86ad603a`.

Generated section patches, labels, About/version/license and shared native tools
are extracted from that checkout. The 68 ids and 41 chapter targets retain native
order. Native Win32/D3D patch boundaries are mapped to the actual TH10 C++ owners,
not to launcher-side replacement gameplay. The complete candidate patch and SHA
are exported by `portable/export-purple-candidate.mjs` for review.

## Implemented native TH10 F12 scope

Hint editing uses the ten StageHints ANM ids and native position/scale/ARGB/alpha
fields. Copy exports native hint-file parameters. Dragging retains desktop hover
selection and adds touch-down selection; recycled ANM ids cannot retain a drag.
Clipboard writes use the browser clipboard with internal ImGui fallback.

Native Game Speed and player-speed compensation use the simulation scheduler and
the original final integer movement boundary. Replay fast/slow/debug rates are
separate from presentation. This does not add a second launcher high-refresh
implementation, advertise Windows DLL injection, or change audio pitch.

Gameplay includes Disable X/Z/Shift, simultaneous C filtering, forced Shift,
Fast Retry, native keyboard HUD/APS export, no-continue last-life mapping,
boss-down/range, one-key death, lock timer, white/yellow point counters and the
existing all-clear bonus. Boss-down preserves the original maximum boundary;
one-key death writes lives -1/power 0/player state 4 rather than bypassing the
deathbomb lifecycle. Trainer mutations retain Replay/assisted-run guards.

SSS contains TH10's actual vertical flip and blind-view options. Flip changes
the native final Y step and final presentation orientation; desktop and direct
touch coordinates use the same inverse mapping. Blind view uses the original
embedded PNG, clipping and player-centered placement, and releases its texture
on reload/shutdown. Reaction test and About retain the purple source UI.

The MASTER item remains explanatory help: upstream comments out its runtime
checkbox. Launcher-only MASTER auto-record/history switches and optional extended
Replay pages are not silently added to F12. TH20-only SSS and TH15-specific tools
are not transplanted into TH10.

## Runtime, resource and lifecycle boundary

U uses the actual enemy health-subtraction boundary without disabling collisions,
damage detection, interrupts or scripted HP changes. Backspace uses native green
bracketed state labels; Tab retains the shared mobile shortcut and native F8 also
works. Launcher mobile U visibility is declared per product instead of assuming
only TH15 can implement it.

The original title/difficulty/character/shot/Practice chain is retained. Fresh
launch clears transient retry/keyboard/pointer state. Menu touches are captured
before gameplay gestures and quick down/up edges survive 60 Hz sampling. Cached
high-refresh ImGui draws do not consume another input edge or advance tool clocks.
Native FPS debt is cleared on suspend/load/start/stop. Ordinary OFF builds neither
initialize ImGui nor import missing optional Practice JavaScript modules.

ECL and ANM lengths come from their real resource owners. Section writes are
staged in bounded ECL/STD/ANM copies and committed only after all writers validate;
there is no assumed 0x99999 allocation. No game layout offsets or native instruction
sites are inferred from another title. Existing real-bullet-sprite bypass and
PRAC/USER/motion-trailer ownership remain intact. Blue Replay parameters remain
readable; new files identify purple 2.2.2.7.

## Automated evidence

- Generator checks: native TH10 patch/labels/shared tools match the recorded source.
- Native checks: 15-word config, blue/purple Replay round-trip, candidate lifecycle,
  touch trailer coexistence, input filters/retry, boss range endpoints/OFF/Replay,
  point counters, timer consumption/reset and speed/cadence checks pass.
- Retail browser matrix: **109/109** (68 sections + 41 chapters), including ST4
  infinite-time midboss, chapter 10408, ST6 final spell and Extra, entered via the
  original Practice chain and ran without application/world errors.
- Browser baseline: F12 quick pointer taps, direct mobile pointing/cancellation,
  Tab/U, blind view and flip pass. Flip ArrowUp changes native Y from 400 to 422.5.
- Native Replay save: PRAC block + purple version present in the serialized file.
- Independent OFF compile and ordinary-game browser startup pass.
- Experiment ON WASM: `ee27fab8db087eb574f60f22196161b51ee44a574e2611fc3989b9b74b71c57a`.
- Canonical local ON WASM: `6b1be61565c5d33045a125699d1db3a21b5fd037c2ee6288410a64f8f9bbb67b`.
- OFF WASM: `3552e7fbb17d898cbb5059fda6bfac9b1465e6613370915f687e51ce9b0430eb`.
- Launcher touch protocol/preferences/layout tests pass. Online main/beta/test untouched.

Evidence is under `th10_web/artifacts/purple-check/` (ignored); packaged Runtime
is `build-eagler/` (ignored). `matrix.json`, baseline browser JSON/screenshots and
the candidate identity distinguish runtime tests from source-only assertions.

Run the generators with the purple checkout, then native checks, ON/OFF builds,
`portable/package-eagler.mjs`, `portable/check-purple-browser.mjs` and
`portable/check-purple-matrix.mjs`. Browser tests require `TH10_RETAIL_DATA` and
`TH10_SHARED_FONTS`; packaging requires `EAGLER_FONT_ROOT`. DATA is served locally
through the managed-runtime contract and is never embedded in test source.

## Explicit limits

### Keyboard HUD presentation parity (2026-10-09)

The HUD now uses TH15's default 34x34 KeyRectStyle, dark (32,32,32) text
in pressed/released states, and anchors (1280,0)/(840,0). Native drawing order,
key masks, per-title input recording and APS calculation are unchanged.
The font atlas includes the same explicit arrow/Delta/Sigma symbols as TH15
in all three locales. Production ON WASM:
`ed8c4d50441198728df7c77a0fb02d0f0319c0d8e01cdecfcd46a33cdcfe3f35`.

### Replay advanced-tool parity correction (2026-10-09)

Purple TH10 does not disable boss-down/range or one-key DIE while playing a
Replay. The added UI disabling and clamp replay guard were removed. Other
F1–F7/U cheat Replay restrictions remain unchanged. One-key DIE still writes
lives -1, power 0, player state 4 and the assisted marker; boss-down still marks
an altered run as assisted. Native clamp tests cover Replay range 0.5 and 1,
option OFF and build-disabled behavior; source assertions reject reintroduced
UI Replay disabling. Browser startup/Practice/Replay-save/SSS regression passes.
Corrected canonical ON WASM:
`a9e2a9011934978e002605a7730f92ec83420ea1f1a1d078366ee4325b830229`.

Headless desktop/mobile viewport and native-owner assertions are automated
evidence, not physical Android/tablet or Windows original-game visual oracle
evidence. The matrix verifies section startup and short live execution, not every
attack's full duration or a full-clear Replay playback. Those long-run/device
checks remain manual regression work and are not labeled PASS here.
