# TH13–15 同代引擎 UI 基座移植方案

来源：th15 参考实现（`ports/th15`）逐文件研读 + th13 资源 dump 实证。目标：以最小模块集在 `ports/th13` 落成「Application 状态机 + ANM 句柄驱动 UI + Frame/Draw 数据分离」的同代 UI 基座，本轮交付标题、Stage 1、HUD 基础。

## 一、th15 模式提炼

同代引擎（th10–th15）共用一套 UI 骨架，th15 的移植把它拆成三条不变式。

### 1. Application 状态机：destination 驱动的场景所有权

证据：`ports/th15/web/cpp/sdl/Application.cpp`、`ApplicationState.hpp`。

- `ApplicationState` 是唯一聚合根：设备（`GraphicsDevice`/`FontDevice`/`AudioDevice`/`FileStore`）、共享 ANM 管理（`animations` + 独立时钟的 `loading_animations`）、`FrameScheduler`，以及场景所有权指针 `title`/`ending`/`run`/`session`/`pause`/`flow`（`ApplicationState.hpp:22-26`）。
- 场景不直接切换彼此，只请求目标地：`pending_destination`（int，沿用原版语义：13=进入对局、15=结局、16/2/4/14=回标题变体、3=退出），由 `step()` 尾部的 `apply_destination()` 统一执行（`Application.cpp:178-192`、`237-238`）。切换前先过装载挡板（`draw_loading` → `pending_load`，`Application.cpp:193-199`）。
- `step()` 帧序固定：装载完成 → 活动场景 controls+`scheduler.update()`（或 `session->step()`）→ 音频 update → `display->draw()` → fades/motion 回收 → 装载挡板 → `apply_destination`（`Application.cpp:200-244`）。
- `projection()` 以 16 个 i32 把内部状态投影给 harness/浏览器（title/ending/run、destination、stage、difficulty、score、残机、灵击、pause 屏），是 oracle 对照的固定接口（`Application.cpp:245-250`）。
- 常驻 ANM bank 编号在 `initialize()` 一次登记：text=0、sig=1、ascii=2、effect=8、title=16、title_v=17、help=19（`Application.cpp:119`）。th13 的 dat 内 ANM 文件名与此一一对应（见下文资源对照）。

### 2. ANM 句柄驱动 UI：u32 句柄 + interrupt/pause/retire

证据：`ports/th15/web/cpp/game/AnmManager.hpp`、`GameHud.hpp`、`TitleState.hpp`。

- `AnmManager::create(bank, script, layer, ordering)` 返回 u32 句柄；`registry.find(handle)` 取 `AnmVm`；UI 状态切换靠 `interrupt(handle, label)` / `pause(handle, bool)` / `retire(handle)` 三个原语（`AnmManager.hpp:36-50`）。
- UI 组件只存句柄数组，不持设备对象：GameHud 的 `life_icons[8]`/`bomb_icons[8]`/`boss_icons[2]`（`GameHud.hpp:16`）、TitleState 的 `handles[248]`（`TitleState.hpp:10`）。
- 菜单光标是纯逻辑：`MenuCursor`（cursor/count/history/disabled + push/pop 深度，`MenuCursor.hpp`），动画选择归控制器、禁用项归菜单。
- 残机/灵击图标的标准驱动法：图标脚本内以 interrupt 标签 2=满、3=空、7+n=碎片数（th15 `GameHud.cpp:7-23`）。

### 3. Frame/Draw 数据分离：Context/Services 结构体对

证据：`ports/th15/web/cpp/game/HudFrameData.hpp`、`HudDrawData.hpp`、GameHud 三文件。

- 逻辑帧：`HudFrameContext`（敌方世界、玩家位置、符卡旗标）+ `HudFrameServices`（纯回调接口），落在 `GameHudFrame.cpp`。
- 文本绘制：`HudDrawContext`/`HudDrawServices`（`hud_text(HudTextDraw)` 一条通道）+ `grouped_number`/`score_number` 格式化，落在 `GameHudDraw.cpp`。
- 生命周期（建句柄、interrupt 布局）在 `GameHud.cpp`。三个文件一个类，按 pass 拆。
- `FrameScheduler`：Update/Draw 双 pass、int 优先级、`FrameCallback` 由外部拥有（帧内零分配），返回 `FrameAction` 控制调度（`FrameScheduler.hpp`）。标题用原版优先级 6（更新）/66（文本）（`TitleFrame.hpp:19-20` 注释），HUD 文本走 caption 通道。

## 二、th13 资源对照（dump 实证）

`artifacts/reference/dumps/anm-list.txt` 中 th13 的 55 个 ANM 与 th15 同名同布局：

| 用途 | th15 bank | th13 文件（dump 行号） | 结论 |
|---|---|---|---|
| 文本/离屏 | text=0 | text.anm（43413） | 同名 |
| 启动装载 | sig=1 | sig.anm（37740） | 同名，装载挡板可复用模式 |
| ASCII 数字 | ascii=2 | ascii.anm（15207） | 同名 |
| 通用特效 | effect=8 | effect.anm（1） | 同名 |
| 标题 | title=16 | title.anm（17132） | 同名 |
| 标题立绘 | title_v=17 | title_v.anm（37812） | 同名 |
| 说明书 | help=19 | help.anm（5541） | 同名 |
| HUD 面板 | （run 装载） | front.anm（5909），244 脚本 | 残机 script30–37、灵击 script38–45，已验证（下节） |
| 灵界/神灵 | — | astral.anm（39718） | th13 特有，本轮不做 |
| Stage1 | — | st01logo/st01wl/st01enm.anm | Stage1 场景装载 |

bank 编号沿 th15 惯例（text=0、sig=1、ascii=2、effect=8、title=16、title_v=17、help=19），front 在 begin_run 时由场景资产登记。

## 三、th13 模块清单与命名

命名空间 `th13`（game 层）/ `th13::sdl`（设备层），目录沿用现有骨架 `web/cpp/{game,platform,rebuild,sdl}`。现有 `sdl/ApplicationHost.*` 由 BrowserRuntime 取代后删除。

### game/（纯逻辑，无设备依赖）

| th13 文件 | 照抄自 th15 | 本轮裁剪 |
|---|---|---|
| `FrameScheduler.hpp/.cpp` | 同名 | 原样照抄（FrameAction/FramePass/FrameCallback 一并） |
| `MenuCursor.hpp/.cpp` | 同名 | 原样照抄 |
| `GameInput.hpp/.cpp` | 同名 | 原样照抄（repeat 参数 8/26 帧照搬，见 §六） |
| `GameKeyboard.hpp/.cpp` | 同名 | bindings 表保留基础位，功能位注释标注 th13 归属 |
| `TitleState.hpp` | 同名 | TitleScreen 枚举仅留 Main/Difficulty/Character/StartGame/Quit |
| `TitleScene.hpp/.cpp` | `TitleScene.hpp` + `TitleFrame.hpp/.cpp` | 不做 Replay/MusicRoom/Manual/Records/PlayerData/Practice/Demo |
| `HudFrameData.hpp` / `HudDrawData.hpp` | 同名 | 去掉 th15 专属字段（pointdevice 指示、chapter 计数） |
| `GameHud.hpp` + `GameHud.cpp` / `GameHudFrame.cpp` / `GameHudDraw.cpp` | 同名三件套 | 残机/灵击/分数/灵力保留；boss 血环、符卡计时、pointdevice、chapter 通知延后；trance 槽仅留成员占位 |
| `PauseState.hpp` | 同名 | PauseScreen 仅留 Inactive/Pause/GameOver |
| `RunPause.hpp/.cpp` | `RunPause.hpp` + `PauseFrame`/`PauseMenu` | 菜单项：继续/重开/回标题；成绩登录与录像保存延后 |
| `SessionState.hpp` | `SessionState.hpp` | 最小字段：stage/starting_stage/difficulty/character/rate/high_score/scene_flags |

### sdl/（设备层）

| th13 文件 | 照抄自 th15 | 说明 |
|---|---|---|
| `Application.hpp/.cpp` | 同名 | PIMPL 外壳（initialize/step/error/projection） |
| `ApplicationState.hpp` | 同名 | 裁剪：无 practice/checkpoint/thprac；ANM bank 表照 §二 |
| `ApplicationTitle.cpp` | 同名 | 标题期平台服务（音乐请求先落桩） |
| `ApplicationGame.cpp` | 同名 | 对局期平台服务（life_hud/bomb_hud/game_over/notice 保留） |
| `GraphicsDevice.hpp/.cpp` | 同名 + `GraphicsImages.cpp` | ZunGraphics 适配 `portable/sdl/Renderer`；离屏/捕获延后 |
| `SceneDisplay.hpp/.cpp` | 同名 | 跨场景显示拥有者；`ScreenTargets` 离屏捕获延后 |
| `FontDevice.hpp/.cpp` | 同名 | 复用 `common/include/eagler/text/RuntimeFontRaster.hpp`；字体槽集按 th13 校准 |
| `BrowserRuntime.cpp` | 同名 | EM 导出 + RAF 循环 + FrameCadence；触控/thprac/practice lab 全部裁掉 |
| `AudioDevice`、`FileStore` | th15 接口签名 | 本轮桩实现（静音 + 内存档），接口先立，后续整件接入 |

### portable/（从 th15 拷入）

`portable/sdl/`：`Renderer.cpp/.hpp`、`FrameCadence.hpp`、`PresentationCadence.hpp`、`Shaders.hpp`、`GraphicsState.hpp`、`RenderCommands.hpp`、`LegacyGraphics.hpp`、`AssetPixelFormat.hpp`、`ResampleShader.hpp`、`StartupBranding.hpp`、`third_party/{stb_image.h,stb_vorbis.h,miniaudio.h}`；`portable/input/KeyboardMap.inc`（两代物理布局一致，原样拷入）。`build.mjs` 的源码收集规则（按目录 glob）无需改动即吃到新文件。

### 前置大件（UI 基座的依赖，非本方案范围但同期推进）

`AnmResource`/`AnmFile`/`AnmVm`/`AnmRegistry`/`AnmManager`/`AnmRenderer`（ANM8 装载 + VM，锚点 0x462450，`web/cpp/rebuild/AddressMap.hpp:23` 的 anm-vm 槽，当前值 0 待回填）。UI 基座只需它跑得动 front/title/ascii 的 UI 脚本子集；未实现指令显式停、不静默跳过（th15 `AnmVm.hpp:25` 的原则照搬）。

## 四、状态机（标题 → 游戏 → 暂停）

```
Boot    prepare_platform（dat 装载/直读 /assets）
        prepare_loading（sig+ascii 挡板，启动 180 帧，独立 ANM 时钟）
        initialize（七 bank 常驻装载 + FontDevice + SceneDisplay + 动画回调注册）
        begin_title(0)                                          // current_destination=4
Title   TitleScene: TitleScreen 枚举 + MenuCursor + handles[248]
        主菜单 → 难度 → 角色 → start_game(stage) ⇒ pending_destination=13
        step 尾部：draw_loading 挡板 → apply_destination → begin_run
Run     begin_run：RunGameplay/Session/Flow/Pause 构造 + display->attach
        step：pause->controls → session->step(input) → display->draw()
        Esc（0x100）⇒ RunPause::open(Pause)   // 覆盖层，不动 destination
        暂停菜单：继续 / 重开(13) / 回标题(2/4)
        Game Over ⇒ PauseScreen::GameOver → 结算幕 → 回标题
```

要点：

- destination 数值沿用原版 supervisor 语义（th15 对照：13 开局、2/4/14 回标题变体、3 退出、15 结局，`Application.cpp:178-192`）。th13 的具体数值与菜单树在 Ghidra 定位 th13 supervisor（th15 0x44e600 对应物）后登记 `AddressMap.hpp`；本轮代码内以枚举 `Destination::{Title,Run,Ending,Quit}` 表达，数值不外露到 game 层。
- 装载挡板复用 th15 双路径：启动保留 180 帧原生装载动画，场景切换只等资源完成（`Application.cpp:83-103`、`150` 注释语义）。
- pause 是 run 内的覆盖层：`RunPause` 组合 `PauseState`/`PauseActivation`/`PauseFrame`/`PauseMenu`，由 `display->attach` 时绑定（`Application.cpp:151-156`），Esc 经 `PauseFrameInput{pressed,repeated,lost_focus}` 驱动（`PauseFrame.hpp:6`）。
- `projection()` 16 槽投影照 th15 布局（`Application.cpp:245-250`），wasm 侧导出 `th13_probe_state` 对应，oracle 与单测共用。

## 五、HUD 基础

### 句柄驱动部分（front.anm，244 脚本，dump 已验证）

- 残机图标：script30–37（sprite12–17，x=527..，y=100；interrupt 2=满/3=空/7+n=碎片，dump `anm-list.txt:6643` 起 script30）。
- 灵击图标：script38–45（sprite18–23，y=138，同构 interrupt 布局，dump `anm-list.txt:7003` 起 script38）。
- `GameHud::initialize` 建句柄（life/bomb 各 8 个，ordering=4，清 `render_flags&0xc000`）、`life()/bombs()` 用 `direct_interrupt` 刷新 —— 与 th15 `GameHud.cpp:27-43`、`7-23` 逐行同构。
- 难度标签/通知类句柄（th15 为 front+80/86）脚本号在实现时对 dump 定位，接口先按 th15 形状留。

### 文本部分（hud_text 通道）

- hi-score / score：`score_number(value, last_digit)` 千分位格式，`displayed_score` 每帧逼近（th15 `GameHudDraw.cpp:7-9` 的增量算法照搬）。
- 灵力（Power）：`x.xx /4.00` 两段字号绘制（th15 `GameHudDraw.cpp:20` 的 power 格式照搬，`power_step` 常量按 th13 原版校准）。
- 文本队列走 `AsciiText`→`AsciiFrame`（FrameScheduler 注册的 caption pass），HUD 文本服务是 `ApplicationState::hud_text` 一条通道（th15 `ApplicationTitle.cpp:21`）。

### th13 特有（本轮不做，仅留占位）

- **trance 槽（灵界槽）**：GameHud 留 `trance_gauge`/`trance_frame` 句柄成员与 `update_trance()` 空位；其 ANM 来源（front 面板脚本号）与 astral.anm 灵界特效、Overdrive 一并在「神灵/灵界」分期实现（对应 `AddressMap.hpp:29` spirit 条目）。
- boss 血环/倒计时、符卡横幅、chapter 奖励幕：Stage1 尾 Boss 战分期。

## 六、键盘输入映射表

浏览器侧 `KeyboardMap.inc`（JS `code` → 原生 scan/vk）原样拷入；游戏位掩码由 `GameKeyboard.cpp` 的 bindings 生成（th15 原表 `GameKeyboard.cpp:5`），`GameInput::calculate()` 负责 pressed/released 边沿与 repeated（首重复 26 帧、此后每 8 帧，`GameInput.cpp:4`）。

| 位 | 键（vk） | th15 语义 | th13 本轮 |
|---|---|---|---|
| 0x01 | Z (90) | 射击/确认 | 接入 |
| 0x02 | X (88) | 炸弹/取消 | 接入 |
| 0x08 | Left/Right Shift (16/160/161) | 低速移动 | 接入（KeyboardMap 层归一双 Shift） |
| 0x10/0x20/0x40/0x80 | ↑↓←→ (38/40/37/39) + 小键盘 8/2/4/6 (104/98/100/102) | 移动 | 接入 |
| 0x60/0xA0/0x50/0x90 | 小键盘 7/9/1/3 (97/105/103/99) | 斜向组合位 | 接入 |
| 0x100 | Esc (27) | 暂停菜单 | 接入 |
| 0x200 | Left/Right Ctrl (17/163) | 对话快进/加速 | 接入（Stage1 对话为桩时仅留位） |
| 0x40000 | Home/P (36/80) | 截图 | 不接 |
| 0x80000 | Enter (13) | 确认（菜单） | 接入 |
| 0x10000 | Q (81) | th15 中途退出确认 | 不接 |
| 0x20000 | S (83) | 帧跳过 | 不接 |
| 0xA00 | C (67) | 回放快进/慢放 | 不接（回放延后） |
| 0x100000 | D (68) | 回放调试 | 不接 |
| 0x200000 | R (82) | 快速重试 | 不接 |
| 0x800000 | F10 (121) | thprac 帧率显示 | 不接 |

位值本身源自原版 32 位输入状态过滤器，接入位经 oracle 录像逐帧对照确认；「不接」位保留在表中，接线时机见分期。

## 七、本轮分期边界

### 必须（本轮交付）

1. portable 共享件拷入（Renderer/KeyboardMap/FrameCadence/PresentationCadence），`build.mjs` 一次编过。
2. ANM 装载与 VM 的 UI 脚本子集可用（front/title/ascii/sig/text/help/effect 七 bank；create/interrupt/pause/retire/update 原语）。
3. Application 状态机：装载挡板、begin_title/begin_run/apply_destination、projection 导出。
4. 标题：主菜单 → 难度 → 角色 → 开始链路可走通（th13 角色 pl00–pl03，Reimu/Marisa/Sanae/妖梦，选人即定 bank）。
5. Stage 1：st01logo/st01wl/st01enm + st01.ecl 装载，玩家移动/低速/命中框，ECL 帧推进接敌机生成接口（敌机完整行为按 `AddressMap.hpp` 优先级同期推进，不阻塞 UI 验收）。
6. HUD 基础：分数/hi-score、残机+碎片、灵击+碎片、灵力。
7. 暂停：Esc 菜单（继续/重开/回标题），Game Over 幕。
8. 键盘基础位全接入，`node --test` 输入单测（repeat 参数、掩码表）。

### 不做（后续分期，接口先留）

- 神灵/灵界/Trance/Overdrive（`AddressMap.hpp:29` spirit）。
- Boss HUD、符卡系统、Stage 结算幕、Extra（st07）。
- Stage 2–6。
- Replay 录制/回放、成绩/全局存档（FileStore 整件）、音乐室、说明书、thprac/thcrap。
- 音频实装（AudioDevice 桩接口化）、触控、netplay。
- 离屏捕获/背景保留过渡（SceneDisplay 高级路径）。

### 验收口径

- 网页构建 `node portable/build.mjs` 通过；WASI harness `build-harness.mjs` 同源编译通过。
- 标题→Stage1→暂停→回标题全链路无 `fail()`；projection 状态曲线与 oracle 逐帧对照（对齐已有 rng 探针方法）。
- HUD 句柄脚本号与 dump 一致（残机 30–37、灵击 38–45 已锁定）。

## 八、风险与待定位

1. **ANM VM 是单点依赖**：0x462450（13.6KB，functions.json 实测 84 调用方/320 被调）的指令覆盖率直接决定 UI 还原度；ANM8 版 thanm 反汇编已干净（`docs/ASSETS.md`），未映射指令按 dump 对照补齐。
2. **th13 supervisor 待锚定**：destination 数值与标题菜单树需要 Ghidra 定位 th13 侧对应物后回填；本轮以枚举隔离，数值不进 game 层。
3. **字体槽集**：th15 FontDevice 的 23 个槽位是 th15 度量；th13 的字号/字重集在 FontDevice 实装时按原版逐槽校准。
4. **HUD 面板脚本号**：残机/灵击已验证；power 面板、难度标签等面板脚本号实现时对 dump 定位。
5. **音频桩的时序回补**：标题/对局音乐切换在 AudioDevice 实装后需回头校准装载挡板与 destination 的先后关系。
6. **Stage1 敌机深度**：ECL 主分发 0x41B260 的还原进度决定 Stage1「有敌人可打」的完整度；UI 基座验收不以此为门槛，但对局可玩性以此为门槛。
