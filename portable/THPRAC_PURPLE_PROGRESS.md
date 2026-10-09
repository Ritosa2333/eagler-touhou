# TH15 purple THPrac adaptation

The production THPrac Runtime is enabled through the normal launcher protocol.
The test site was updated on 2026-10-09; main and beta were not changed.
Validation below describes the evidence collected, not full native/device parity.

## Latest fixes and production verification

- Preserve purple's keyboard HUD style and make its key labels readable; include
  arrows and Greek monitor glyphs in the shared Unicode font atlas.
- Serialize the captured Reimu bomb aura handle, not the unrelated live registry
  slot; imported checkpoints remap saved handles without changing the live aura.
  The standalone regression passed without THPrac enabled.
- Keep AB radar side labels inside the panel with an inner margin, without
  changing the font size, chart geometry or score calculation. Source-generation
  consistency and 32 horizontal-bound cases passed.
- Production build `50e147d41e34e3fb44858a7f8a6764070932a6d46ceb4e17507994c94477c8c4`
  passed the local launcher protocol smoke (font mount, keyboard and save RPC).
- Test deployment generation
  `1309bd502e04e012e1da281b746f698370149761a680f4296140394ae411c343`
  passed 22 public-file hash checks. Existing unrelated test files and full
  main/beta file hashes were verified unchanged.

## Authority

- `eagler-touhou/docs/ADAPTING_A_GAME.md` and `docs/playbooks/thprac.md`.
- `docs/playbooks/adaptation-worktrees.md`: isolated topic worktree.
- TH08/TH10/TH11 portable source-generation, expected instruction-site CRC,
  native menu, replay and shared input/render methods.
- Purple checkout `17780056107a014e88d35fceb3ad86c121d94287`;
  normalized `thprac_th15.cpp` SHA256
  `2b412168cd8d71dd7ef8e1f79a1a5d33cf64f983792cbc9d0f4ccbc56e60ce03`.
  Purple version is extracted from its version header, not the blue version.

## Implemented and tested

- Generator extracts all resource patch branches, AB Test's MIT payload,
  enum order, phase counts and three-language section/phase labels.
- Exact patch write ordering and include ordinals retained. Four native process
  hook requests become scene-owned effect data. Chapter set/reward skip/Extra
  bonus and Stars BGM synchronization are now consumed.
- All writes are bounds checked and staged in a transaction. Source-site CRCs
  attest every extracted patch write, not whole files (language ECL may differ).
- Explicit jump source/target checks; newly authored instructions are admitted
  only when every byte of their full instruction span was written.
- Purple Extra S4/S8/S9/S10 intentionally overwrite the following Boss1 header.
  Only this source-specific header span is allowed. The program adapter extends
  Boss to the exact authored entry extent and makes destroyed Boss1 unbindable.
- Purple AB Test installs a complete ECLH routine at file 2, offset 0x694.
  Its authored instructions are length-checked; the original lookup ordinal is
  retained and overwritten subsequent routines become unbindable.
- StageAssets exposes an optional decoded-ECL hook before main/context binding.
  The native Practice caller now supplies the accepted config before binding;
  Original mode and ordinary stage loading do not patch resource bytes.
- USER/PRAC serialization follows purple THPracParam's field names and optional
  fields and the shared ReplaySaveParam alignment. Strict parsing rejects bad
  types, duplicates, overflow, invalid configs and duplicate PRAC blocks.
- Explicit reader correction: purple GetJson writes `bomb_fragment` but ReadJson
  omits it. The portable reader restores it; older missing fields remain zero.

The resource runner exercises **449 cases**, covering 74 sections, seven stages,
chapters, phases/dialogue/angle parameters, program publication, JSON/USER block
round trips, original-mode isolation, damaged sites and transactional failure.
These are automated resource/format checks, **not native gameplay equivalence**.

```powershell
$env:TH15_EMSDK='C:\Users\w3051\Desktop\eagler\th08\tools\emsdk'
node portable/check-th15-thprac.mjs '<private original th15.dat>' '<thprac_purple checkout>'
```

Use `--generate-sites` only after source and original resource validation.
Retail resource payload, executable, fonts and build output are never committed.

## Required next work before capability promotion

### 2026-10-08 integration progress (not semantic closure)

- Native Practice state callbacks, generated purple locale/phase labels,
  desktop numeric fields and shared mobile mouse/keyboard carrier are wired.
  Tab replaces purple F8 at the user's request; Backspace opens the overlay.
- ImGui input advances at fixed simulation ticks, with cached draw data after
  high-refresh presentation. The game retains ownership of vanilla Pause.
- Source stock units are applied before the initial replay snapshot; decoded
  ECL patches and one-shot chapter selection are reached by live Practice.
- Player hooks implement invincibility, life/bomb preservation, power-loss
  suppression and death-window autobomb; enemy damage suppression is bound.
  Purple's default no-continue mapping preserves life only at zero; the
  advanced checkbox can restore preservation of every life stock.
- Initial Reisen shield construction skips stock/SFX/aura and retires excess
  child layers. Actual animation and shield exhaustion need device/oracle tests.
- PRAC is appended to real replay exports and selected before ECL loading on
  playback, including validation and preservation of Practice mode flags.
  This wiring has not yet passed full live record/replay equivalence tests.
- F12 currently exposes implemented information/life-mapping preferences and
  source version. It explicitly labels the port incomplete; it is not a claim
  that the full purple advanced window works. No fake enabled advanced toggles.
- F5 skips the enemy age timer call entirely (including previous/fractional
  fields), and bypasses spell bonus decay. Lifetime/ECL/animation, spell age
  and active frames continue. The original patched relative call lands at
  Timer's RET 4, not a tick with rate zero.
- Chapter skip consumes each original reward branch, including chapter zero;
  the snapshot branch remains intact. Chapter override follows the original
  run-clock comparison. Extra's one-shot total=1 runs after checkpoint copies
  and file requests, so retry still restores saved total=0.
- Initial scene music selects the source section's stage/Boss slot. This
  entrance-only hook does not reroute ordinary MSG/retry music.
- Tab now displays source-faithful Legacy/Pointdevice headings, column order,
  stage/current/total retries and optional shooting-down percentage/product.
  AB Test uses the original manager's globals (float +1c, integer +0c), not
  their different serialized checkpoint offsets.
- AB Test scoring, ten overall ranks, per-axis ranks, radar geometry and its
  90-frame reveal/interpolation are extracted from purple ABTestRender.
  The shared extracted scoring helper passes zero/unit/selective-phase checks.
  GUI generation advances only on simulation frames, not extra presentations.
- Browser blur/stop resets ImGui held keys/pointers and mobile shortcut bits;
  replacing the application destroys the previous context. Attract-mode demos
  explicitly clear live Practice configuration before constructing resources.
- At this earlier milestone, speed/replay controls, keyboard monitor/blind mask
  and SSS were missing. The later integration below supersedes this inventory.
  SSS means Super Secret Settings (flip/blind), not autosave; purple TH15's F12
  does not contain an autosave feature.
- The ON build uses `--thprac`; no launcher capability was enabled, published,
  committed, pushed or deployed. Automated resource tests are not device proof.

### Additional 2026-10-08 verification

- Read-only assertions on 27 original executable sites establish the precise
  time-lock, life, chapter, movement, history and BGM ownership; executable SHA256 is
  `67a642357c8777089f468aab9c7a0ae346ebdb62849d842a7b7b18d1e6910364`.
- `check-th15-thprac-gameplay.cpp` exercises real SpellCard, SessionRuntime and
  ChapterCheckpoint implementations with fake platform services. It covers
  bonus decay vs advancing animations, no-continue mapping/replay gating,
  reward-only skips, dead-player guard, one-shot ordering and retry restoration.
- The same runner retains all 449 retail-resource cases and source/site
  extraction checks. These typed tests are not native execution parity or
  browser/mobile/rendering proof.

### Further 2026-10-08 source-owner integration

- F7 is now wired to desktop and the shared mobile shortcut carrier. The
  everlasting state machine is extracted from purple `thprac_games.h`; TH15's
  four native caller tags are extracted from its template instantiation.
  Play/Stop/Pause/Resume and Other requests remain distinct. The source's
  bit-1 test and Stop's retained status are preserved, not replaced with the
  Practice-entry bit or a blanket music/retry prohibition. Music preparation,
  fade requests, MSG and session calls now share the filter.
- Stars's normal phase alone arms the one-shot offset. Source `0x8fc768` is
  extracted as PCM **bytes** and converted with the selected original track's
  block alignment before audio starts. General seeks and wave-switch requests
  do not consume it. The original `th15_12.wav` layout, one-shot lifetime,
  phase exclusion and native non-intro caller/argument are checked. This is
  not yet live audio timing equivalence.
- Boss movement uses the extracted exact four-step range formula after ECL
  opcode 504 stores its four arguments. Default strength is source 0.5, with
  a source-language advanced checkbox, 0..1 adjustment, help and active
  warning. As in purple it can affect ordinary games and replay playback;
  new live runs with this preference are marked assisted for save safety.
- Complete spell history bypasses both native >=100 branches and sets the
  original caption coordinate mode to zero. Production caption tests cover
  2/3, 99/100, 100/123 and 999/1234 with the preference ON/OFF. GPU text layout
  still requires live evidence.
- Lock-time counting is observed at real Boss countdown publication, reset
  on native GUI preparation/Boss-mode/next-pattern owners and consumed once
  per generated GUI frame. Multiple observations and cached high-refresh
  redraws do not multiply the count. The F5 + advanced display uses the
  source two-decimal seconds and fixed white/black box. Real EnemyInterrupts
  tests cover Boss/non-Boss, no-time-limit, disabled adapter and pending-reset
  semantics. High-refresh browser/device behavior remains unverified.
- Source/resource regression retains all 449 cases; generated shared-hook
  source and the 27 original executable sites are checked without copying
  executable/assets to public artifacts. ON/OFF release builds are checked
  separately. No live menu/replay/device parity is claimed by these tests.

### Assisted replay menu lifecycle

- Pause and completed-run replay saving query the application-owned assisted
  state. Unassisted Practice recordings retain the normal PRAC save path;
  assisted runs disable the save entry instead of reaching a fatal write.
- Rechecking while paused avoids duplicate disabled entries in the original
  bounded cursor. Enabling assistance inside replay slot/name entry unwinds
  the pushed menu frame, clears replay UI flags and resumes the result menu.
  Completed runs use the existing cancel/cleanup path without writing.
- `check-th15-thprac-menus.cpp` links real game/menu implementations with
  fake storage/audio and asset-free animation state. It checks normal save
  confirmation, 100 repeated restriction checks, slot/name history unwinding
  and completed-run cleanup. It does not establish rendered disabled labels,
  device behavior or native execution equivalence. The shared regression
  runner now includes this menu test.

Next: close the explicitly missing purple features, then exercise native menu
cancellation/confirmation, mobile fields/popups, shield exhaustion, assisted
save rejection and full PRAC recording/playback in the actual Runtime. Promote
the launcher switch only with those lifecycle/capability attestations.

There has been no commit, push, deployment or capability promotion for this work.

### Shared TH15 F12 integration and the narrowed user scope

- The user explicitly limited this adaptation to purple TH15 F12 membership,
  excluding extra native-launcher preferences and another high-refresh port.
  Source generation now asserts ContentUpdate membership and the shared
  GameFPSOpt default (`replay = true`). Game speed is a simulation-period owner,
  not a new display-refresh option; existing 60 Hz/high-refresh defaults stay.
- Native launcher auto-shoot binding, faster-BGM and pitch preferences were
  removed from the browser launch boundary. No new eaglerOptions fields were
  added. Conditional launcher-only sound/pitch controls remain absent, as with
  the native default configuration. Windows DLL/API selection diagnostics are
  not emulated or presented as working browser APIs.
- F12 includes the source speed/preset/free/replay/debug controls, gameplay
  filters, fast retry, keyboard monitor/APS CSV/plot, life mapping, shooting-down
  rate, Boss range, full history, lock-time display, All Clear Bonus, Secret
  flip/blind/reload, reaction test and About/license groups. Shared drawing and
  reaction algorithms are generated from purple, not newly designed tools.
- Key recording preserves source transition counting and 60-frame window.
  The source's invalid vector<int> to vector<uint8_t> cast is deliberately
  corrected while preserving last-600 sampling. Blur/stop clears the input
  latch. Extra presentation redraws do not advance UI timers or key records.
- The native flip subtracts the Y step and flips the final presentation. The
  portable pointer boundary inversely maps display Y for usable menus; touch
  targets account for the flipped movement sign. Default OFF is unchanged.
- About uses the actual source version, glossary text and redistributed MIT
  license. The non-source incomplete-status sentence was removed from the UI;
  verification limitations remain recorded here, not disguised as native text.
- Actual browser Runtime testing exposed and fixed an old-ImGui mismatch:
  EndDisabled requires the same condition as BeginDisabled in this vendored
  version. Modern unconditional EndDisabled aborted the assist overlay.
- `check-th15-thprac-browser.mjs` mounts private original DAT and measured fonts,
  initializes the real Application/THPrac owners, and exercises original menu
  cancellation, re-entry, pointer popup selection of Boss1 and ECL-backed launch.
  It checks F12 open/close, Tab, Backspace/F1 assistance, gameplay checkbox
  mutation, native flip movement and mirrored pointer restoration, blind image
  fallback/reload, production PRAC storage, assisted-save rejection, and playback
  selected through the original Replay stage chooser (substate 4, same screen).
- Development probes are gated out of release builds and do not replace game
  menu/session/serialization owners. Private assets and screenshots live only
  under ignored artifacts. Browser music is disabled in this lifecycle test;
  it does not prove audio timing, exhaustive section execution, human mobile
  ergonomics, or Windows-native execution equivalence. Launcher capability
  remains unpromoted pending the remaining proof/deployment workflow.

### Extended AB Test execution proof

- Actual original-DAT browser execution exposed missing ECL opcode 526 in the
  authored AB Test's later phases. Retail 0x42b275 evaluates a float, squares it
  into enemy +0x4074, and 0x42bd23 passes it to the bullet emission distance gate.
  The existing typed field and emission owner now execute that operation.
- Two read-only executable assertions bring the native inventory to 33 sites.
  Typed regression covers literal/negative/reference operands and truncated
  input without mutation. No unsupported instruction was silently skipped.
- The browser regression completes 11,640 actual AB simulation frames,
  including natural hits through the authored phases, global state 6 and the
  180-frame result/score reveal. The source expands the window during frames
  1..90, then animates scores during frames 90..180. The old frame-90 screenshot
  showed six F(0.0) labels by design; it was not proof of final scores. At frame
  180 the actual result displays A(92.4), D(51.9), C(57.6), C(67.2), D(37.2),
  C(64.4), with a filled radar polygon. The screenshot was visually inspected. This
  is Runtime execution evidence, not Windows-native or device equivalence.
- Separate `--tools-only` browser regression exercises both source reaction
  modes for five trials each. Signals are detected from the presented canvas
  PNG (the game render target excludes the ImGui overlay), followed by actual
  raw-arrow press/release and result screenshots. No clock, random delay or
  source reaction state is overwritten by the test.

### All Clear Bonus completion details

- The advanced checkbox/help are generated from purple's shared glossary,
  not newly authored translations. Default remains OFF. Assisted live runs
  cannot save a replay whose extra completion preference is absent from PRAC.
- Native patch `43d99d` falls through only after original practice records;
  replay practice still exits earlier. Hooks `43daac` and `43dcb5` synchronize
  HUD score and return to practice only for mode bit `0x10`. The latter is
  the non-final stage branch, NOT Extra's final bonus branch. Extra retains
  the source's unhooked completion/ending path. No symmetric hook was invented.
- Source generation rejects changed hook topology. Four added original
  instruction assertions bring the read-only native inventory to 31 sites.
- The real SessionCompletion regression matrix covers 96 combinations of
  ON/OFF, live/replay, Legacy/Pointdevice, all four mode-bit values and stage
  1/6/Extra, including record/ending/HUD/return order and exact score units.
  These tests do not establish native execution parity or live ending UI.
