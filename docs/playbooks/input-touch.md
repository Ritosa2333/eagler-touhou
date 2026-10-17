# Input and Touch Playbook

Status: active

## Purpose

Keep keyboard, mouse, touch, gesture, focus and mobile layout behavior tied to the correct owner and logical tick. Preserve Replay determinism while allowing presentation-only controls and helpers.

## Applicability and authority

- Touch is now an all-game adapter capability, not a TH06/TH07 precedent that a
  later title may silently omit.
- The machine-readable parent contract is
  `src/contracts/adapter-capabilities.mts`:
  `touch-controls` is required and `REQUIRED_TOUCH_BEHAVIORS` enumerates the
  shared behavior profile.
- TH08/TH10 share the portable `TouchController.hpp` policy and are covered by
  cross-repository deathbomb/movement/sensitivity gates.
- Device and orientation behavior must still be verified on the named device;
  source/workspace PASS is not a real-device claim.

<!-- knowledge-id: K-INPUT-001 -->
<!-- knowledge-id: K-INPUT-002 -->
## Normal design

Separate the layers:

```text
DOM/device event
    → input bridge / touch manager
    → one logical-frame sample
    → game input owner or Replay owner
    → presentation-only control visuals
```

Touch manager availability and the enable macro are separate concerns. The
shared sensitivity protocol is **100–300%** with 150 as the Launcher default; a Runtime
must not retain an older hidden 50% (or wider native) range when the Launcher no
longer exposes it. Preset/preview and four-tap copy are different operations.

Required touch behavior currently includes direct rate-limited and unlimited
drag, digital and free-direction joystick modes, hold/toggle/two-finger focus,
Fire/Bomb/Escape, original deathbomb timing, optional double-tap Bomb,
menu/dialogue navigation, mixed-input isolation and lifecycle cancellation.
The layout editor, magnifier and touch Restart button are Launcher-owned; a new
Runtime reuses them rather than inventing per-game UI/protocol commands.

Unset Launcher preferences default to unlimited direct touch. Preserve explicit
saved movement choices, including legacy `unlimitedTouch: false` as rate-limited
direct touch. Enabling touch and changing movement modes apply immediately,
without touch/Replay/unlimited warning confirmations. Starting with touch disabled
also has no input warning. The magnifier/two-finger conflict warning is not
displayed; the ordinary accessible control description remains. This shared
frontend policy applies to all products;
room movement restrictions and native Replay/result semantics still apply.

Bind layout state to orientation: direction, portrait/landscape, viewport editor settings, runtime apply and window-position memory must use the same orientation owner. Focus controls should remain visible; magnifier is opt-in. Dialogue, StageClear, menus, help and fullscreen each need an explicit touch path rather than assuming gameplay controls cover them.

For Replay, record the logical run-state consumed by simulation, not a picture of touch buttons or an unbounded stream of raw browser events. Pause, restart and focus loss must have explicit lifecycle behavior.

## Invariants and pitfalls

- Physical gamepads must be normalized at the platform boundary before the
  title's KeyConfig or gameplay code sees button numbers. Never persist or
  compare raw `SDL_Joystick` button indices as stable game bindings: device,
  driver and browser mappings can differ across DualSense/Xbox/Steam
  controllers. KeyConfig capture and gameplay sampling must consume the same
  normalized button slots; D-pad directions must use semantic direction input,
  not assumed raw button numbers. TH06/TH07 provide the proven SDL Gamepad
  precedent; TH08/TH10 must preserve the same invariant.
- Launcher controller occurrence IDs must remain stable for every still-connected
  browser index. Removing the lower-index member of an identical pair must not
  rename the survivor, change its saved mapping or transfer it to another seat.
  Reconnection still requires neutral input before rearming. Across a browser
  restart, indistinguishable devices may still require reassignment.
- Analog button/trigger activation uses its finite `value` and the configured
  threshold. Browser `pressed` may already be true below that threshold and
  must not bypass it; it is only a fallback when no finite value is available.
- Mobile/external keyboards need two browser paths: ordinary keys travel over
  the hosted keyboard bridge, and Runtime-side decoding must preserve the
  `code -> key -> legacy keyCode` fallback used by TH06/TH07 because mobile
  keyboards may emit `Unidentified` on one half of a key lifecycle. Devices
  that Chromium exposes only through the Gamepad API additionally need a
  D-pad-only fallback. Restrict that fallback to keyboard-like device IDs and
  buttons 12-15 so it cannot become a second owner for real gamepads.
- Sample touch delta at most once per logical frame. A permitted first-frame repeat is not a license to repeat later input; later missing samples are zero.
- Network retry, browser redraw and display refresh must never resample the device or feed the same input twice.
- Keyboard, touch and mouse must have one clear owner at each tick.
- Treat pointer/touch lifecycle recovery as implementation robustness, not as a
  product gesture. If an UP/CANCEL is lost or a browser reuses an ID, a later
  DOWN must be able to retire stale ownership before starting the new gesture.
- Ordinary live-control/config snapshots must update their own fields without
  clearing an unrelated active gesture. Only an owner-changing mode transition,
  disabling Touch, or a real lifecycle/context reset should retire it.
- Gameplay/menu/dialogue/replay context changes must invalidate ownership that
  is no longer legal in the new context. Delayed browser events or old snapshots
  must not re-arm a cancelled gesture after blur, visibility, pagehide, pause or
  another lifecycle boundary.
- Recompute the screen-to-game touch mapping after resize, orientation,
  fullscreen or viewport-transform changes. A stale rectangle is a coordinate
  mapping bug, not a new touch behavior.
- Rate-limited direct-touch implementations must clamp the stored reachable
  target at the actual movement bounds. Do not accumulate unreachable
  out-of-bounds displacement that later behaves like movement debt.
- A multi-touch gesture must not depend on one particular participating finger
  receiving the final UP. End/release ownership from the actual surviving
  gesture state, so browser event ordering cannot strand the menu gesture.
- One-shot Bomb/Pause/Confirm pulses expire in the state/context where they were
  produced. A rejected or missed pulse must not queue across death/respawn,
  menu/dialogue changes or another run.
- Dialogue, StageClear and other modal touch-confirm owners must suppress the
  preceding gameplay Fire hold before emitting a fresh confirm edge. In
  multiplayer, do not OR another seat's held Shoot back into that modal owner;
  otherwise two touch clients with auto-fire can make tapping unable to
  advance. TH06/TH07 use the P1 logical lane for the shared message owner;
  TH08/TH10 adapters must preserve the same edge invariant even when their
  state owners differ.
- Restart/fresh-attempt boundaries invalidate the previous raw-touch lifecycle
  for Replay capture. Until a new DOWN creates a new point, orphaned MOVE/UP
  from the old attempt must be ignored.
- Pause-R, Continue, restart and focus loss are not interchangeable state transitions; use the game-specific owner and contract.
- The shared Restart control emits the ordinary R-key path. Every formal
  adapter must map that path to restart-current-run while the pause UI owns
  input; this is a required semantic action, not a per-game feature toggle.
- Touch Bomb must preserve the title's original deathbomb window and must not
  drop a held movement gesture merely because movement output is temporarily
  blocked during death/respawn state.
- Unlimited direct touch is a cheat only after a non-zero unlimited movement is
  actually consumed by gameplay. Merely enabling the option must not mark the
  run. Once used, the run-level marker survives stage transitions and forces the
  title's Result/high-score and saved-Replay processing-drop rate to `100%`.
- Do not use CSS alone to fix an orientation-memory bug. Verify the saved window/layout owner.
- `less motion` reduces mobile presentation motion; it does not delete functionality or gameplay.
- Do not add unrequested D-pad, free remap or save-state features because a feedback item mentions them.

## Bounded keyboard edge retention

General keyboard preferences default to the original Arrow-key layout, with no
added aliases. WASD is an explicit selectable preset; preserve saved presets and
custom layouts across reloads. TH09 local two-player input keeps its independent
original per-player layouts rather than inheriting the general preset.

TH09 `a2d7652` (2026-10-16) retains a bounded complete keyboard stroke when
DOWN and UP both arrive between fresh logical captures. The product contract
is [short-press protection without extending holds](../ADAPTER_BEHAVIOR_INVARIANTS.md#短按保护不得延长长按或制造重复操作).

- Retention belongs to the physical input owner, outside world snapshots.
  Capture consumes it once; retries and historical replay reuse the captured
  word. OS repeat is a held-state update, not a new DOWN edge.
- Bound retained edges and coalesce excess complete strokes. A long stall must
  not produce a long tail of old movement or shots after physical release.
  An ordinary hold already observed by capture must release on the next fresh
  capture; the short-stroke path must not add another held frame.
- Map protection through the selected device's logical actions. TH09's Z and
  right-keyboard Down are both Fire bindings; a generic movement tap may retain
  only mapped direction bits. Do not accidentally synthesize Bomb or Focus
  because the same physical key has another layout's meaning.
- Scope retention to the intended gameplay context and clear it at lifecycle
  cancellation. Preserve the title's menu, cooldown and deathbomb rules.
- Keep active device-event delivery independent of unrelated resource/file
  command queues. TH09's managed shell has a synchronous device-command path;
  this does not authorize taking another logical sample or advancing a tick.

For a late release, distinguish owner UP, first fresh capture with the released
bit, the frame that applies it, and the first submitted picture containing the
result; see [rollback input-latency decomposition](rollback.md#separate-input-latency-components).
An already-sent held word can remain pending while UP has correctly reached the
owner. Changing that old word would violate the shared input history.

Reference anchors in `th09-eagler/th09_web/`:

- `cpp/game/KeyboardInput.{cpp,hpp}` and `cpp/sdl/Application.cpp::sample_keys`;
- `sdl-runtime/managed.mjs` and `directory-keyboard.mjs`;
- `tests/multiplayer/keyboard-shot.cpp` and `keyboard-shot-browser.mjs`.

The component gate covers hold/release, sub-capture strokes, re-press, layout
mapping, repeat and clear. A short-shot or direction-release browser result is
not an end-to-end long-held Z charge-release test. Check the actual control
mode, keyboard versus touch/autofire ownership, and charge/focus state before
attributing a late charge release to a lost key event.

## Superseded approaches

Recording only raw touch graphics, fixing orientation-memory defects with CSS alone, or treating preset/preview as the four-tap copy action is superseded. The logical input owner remains the source of Replay truth.

## Code anchors

- `src/contracts/adapter-capabilities.mts` — required touch and
  option classification.
- `tests/test-portable-touch-controller-contract.mjs` — TH08/TH10
  shared controller, deathbomb and sensitivity boundary.
- `tests/test-required-gameplay-actions.mjs` — shared Restart/R
  semantics across TH06/TH07/TH08/TH10.
- `tests/test-gamepad-contract.mjs` — all-game physical controller
  normalization and KeyConfig/gameplay ownership contract.
- `tests/test-always-hitbox-contract.mjs` — all-game hitbox
  presentation contract.
- sibling TH08/TH10 `portable/input/TouchController.hpp` — canonical
  directory-Runtime policy for those titles.

## Verification

1. Test one input source and one logical frame at a time; log whether the source was keyboard, mouse, touch or Replay.
2. Check focus, blur, pause, restart, menu, dialogue, StageClear and fullscreen transitions.
3. For touch, test portrait and landscape independently, including orientation change after saved layout state.
4. For Replay, compare the input stream and first divergent logical frame; do not rely on a button screenshot.
5. Label desktop browser, emulator/WebView and real-device manual evidence separately.

Controller regression lanes:

- `node --test tests/react-main/gamepad-matrix.test.mjs` covers seven synthetic
  layouts, action learning/profile restoration, analog thresholds, identical
  devices, sparse browser indices and four-device assignment isolation.
- `python tests/test-gamepad-matrix-browser.py --workspace <workspace> --output <report.json>`
  covers the same seven layout families in Chromium, Firefox and WebKit through
  all eight actual JS input boundaries (168 basic combinations), plus hotplug,
  lifecycle and saved-profile checks. These are input-boundary fixtures, not
  full game or physical-controller acceptance.
- `python tests/test-gamepad-runtime-browser.py --url <local-th09-preview> --output <report-directory>`
  uses a current Launcher, Release TH09 and local retail DATA. Virtual standard,
  raw-stick/axis-trigger, identical-controller and keyboard/controller pairs use
  the real settings UI and Gamepad API path through menu navigation, independent
  character selection, stage selection, gameplay, pause/resume and identical
  hotplug. Its traces observe the C++ sampler callbacks without adding captures.
  USB/Bluetooth behavior, browser device-remapping databases and physical latency
  require separate hardware evidence.

## Deliberately omitted claims

The static/workspace contracts do not claim iOS/Android real-device orientation,
long-duration mobile stability, or visual feel. Those require the corresponding
browser/device acceptance lane.

## 常显判定点的原版视觉验收

`options.alwaysHitbox` 是全作品必需显示能力。配置必须在启动前生效，并在场景重建后保留，不能只检查 shell 出现了开关。它不应改变原生低速输入、速度或碰撞范围。

沿用每作原版判定圈的素材、尺寸、混合、父子变换、动画曲线和绘制层。重复绘制不能多推进动画，暂停和半速应跟随该作逻辑时间。原生低速判定圈（含退场动画）仍存在时，不能再叠画一份。仅加载原版图片不能替代完整动画核对。

| 作品 | 视觉来源与本次检查范围 |
| --- | --- |
| TH06 | 原版没有可见判定圈。保留现有 TH07 etama2 素材的扩展显示，不能宣称原版 TH06 同款。 |
| TH07 | 原版效果 24 / ANM 0x2c2，独立显示 VM，运行动画时保存和恢复 RNG。 |
| TH08 | 原版 fixed effect 22，原生动画更新路径。 |
| TH09 | 原版 etama.anm 脚本 63，两个场地分别拥有私有 VM/RNG，在原生特效层绘制。 |
| TH10 | 原版 effect 脚本 0x160，修复配置先于 World 创建时丢失的问题，normal/mp 分别覆盖。 |
| TH11 | 原版 bullet 脚本 74，独立 presentation manager，绘制副本。 |
| TH15 | 原版 effect.anm 25/26，由私有 VM/子对象宿主执行，在原生层 14/18 的时机绘制。 |
| TH20 | 原版 effect.anm 18/19 的模板及原生插值/运动函数，原生层 18 绘制。原始脚本在时间 18 设置角速度，需保留该时序。 |

跨作品接线和 TH10 创建时序：`tests/test-always-hitbox-contract.mjs`。TH09/15/20 的原版资源和原生动画组件对照：`npm run test:hitbox-visuals:workspace`，显式提供 `EAGLER_TH09_HITBOX_ANM`、`EAGLER_TH15_HITBOX_ANM`、`EAGLER_TH20_HITBOX_ANM`。这些私有素材不可提交。组件对照包括普通/半速、倍率、旋转、缩放、透明度、顶点（TH15）、重复绘制和共享 RNG/注册表边界，不等于完整浏览器逐像素或整局 Replay 验收。

`npm run test:always-hitbox:browser` 用显式私有 Runtime/数据路径运行正式包，留下未低速、原生低速和松开后的截图。必须检查进入实际游戏，不能用标题计时器改变作为进入关卡的证明。增加新作品时同时更新覆盖检查和本表，不能通过产品开关退出必需能力。
