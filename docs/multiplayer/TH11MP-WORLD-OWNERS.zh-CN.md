# TH11MP 完整世界恢复：owner 与寿命边界清单

日期：2026-10-13。审计对象：`D:/workspace/eagler/worktrees/th11-rollback-smoothness`。
本文件记录源码审计与最小实施边界；不是完整世界回滚、浏览器性能或交付通过声明。
开始新阶段时同时重读本树工作记录和跨作架构 §7/8。TH10 已封存；不默认移植其未通过或关闭的实验。

## 1. 建议首个可靠范围

首先实现同一 GameBattle、同一 Stage 和资源身份内的完整事务：
输入历史提交 → GameSession/GameBattle Update → 文本语义处理 → authored Draw → EndFrame。
历史重演也执行有状态 Draw；完整事务之间才允许让出。额外高刷呈现另行证明只读。

首版可将资源图变化、Pause/结果/Ending 菜单和 stage overlap 作为确认模式范围。
这不删除玩法；这些帧仍走原来完整路径，只须有实际连续输入、既有历史已退休才执行不可逆动作。
同一 stage 内普通移动、射击、敌人、Bomb、护盾、Ghost/救援、收点等不能因此排除。
Dialogue 本体可恢复，实际动态文字纹理必须额外处理，不能仅恢复消息队列。

遇到帧内才发现的资源动作，不能先执行动作再看 post-state：
在破坏/外部写入前返回明确 barrier；撤销仍可恢复的部分事务，不消费新设备输入，
待确认条件满足后使用同一帧历史输入重做原始事务。barrier 与游戏错误应分开。
只有下一次完整边界才重新建立预测历史；不可让半帧世界进入 Draw 或触控 getter。

### 必须前置保护的实际入口

| 入口 | 破坏或外部动作 | 最小边界 |
| --- | --- | --- |
| GameSession::reset_state / begin / start_stage / open_title | battle、menu、ending 物理销毁，ANM/compositor reset，资源替换 | 世界世代屏障，所有旧 checkpoint/lease 退休 |
| GameBattle::next_stage | load_stage，Dialogue/Bomb 重建，Enemy program 更换，Stage move/new，玩家 Bomb callback owner 更换 | 在调用前确认；整个 overlap 暂按确认模式 |
| GameBattle::stage_control | start_objects 引起 vector 长度增长；outgoing_stage.reset | 不能只保护 next_stage 后立即恢复预测 |
| GameBattle::update 末尾 | resources.retire_previous → GPU unload + previous_stage.reset | 历史不可再恢复旧资源引用后才能执行 |
| GameSession::update_multiplayer 的 pause_edge 分支 | new PauseMenu、mp_presentation ANM、rate 切换、截屏请求 | pause entry 之前确认；Pause 首版不预测 |
| paused Resume / Title / Restart | pause_menu.reset、session finished、协议世代退休 | 全部确认，保留已有协议重开路径 |
| multiplayer_open_result / open_ending / open_clear_results | new/reset menu；ANM/compositor clear；Ending 资源 prepare/release | 转换前确认；菜单/Ending 首版确认模式 |
| Application::capture_pause_frame | GPU resample 写可见纹理并清 capture_requested | 原始前一画面必须有效，不能由事后 Draw 代替 |
| Application::text_events / FontDevice::text | CPU TextureImage 像素写、revision 更新、VM flags 写 | 纹理恢复 owner 或明确文字写屏障 |
| multiplayer_menu_metadata / menu_save | 文件扫描、时间戳、Replay 写盘/存档 | 确认输出侧，不纳入世界字节撤销 |

证据：game/GameSession.cpp（reset_state/start_stage/open_title），game/GameSessionMultiplayer.cpp:56–128；
game/GameBattle.cpp:115–198、update 末尾；game/GameSessionResources.cpp:27–34；
sdl/Application.cpp:220–262、345–401。

## 2. 固定世界外壳：逐项捕获，不复制对象头

GameBattle 有多重多态基类、引用和 unique_ptr；GameSession 也有多种资源与外部状态。
不能 memcpy 整个对象。常量绑定与外部宿主服务保留原地址，字段 owner 如下。

### GameSession

在稳定 stage 范围至少保存：

- GameSessionState 全体：phase/frame/stage/character/subtype/difficulty/focus/replay/demo/practice。
- multiplayer_frame、multiplayer_pause_held[3]；不能只恢复 held 而漏 pause 边沿。
- economy 的值、spell_records、clear_records。
- battle_attached、paused_rate、menu_input，以及 error 的 typed 字符串值。
- 若范围包含普通/菜单路径，还需 game_input、last_held/last_pause、completed_run/score/continues、
  initial_stage/next_continues、recording_active/seed、title_idle/demo_index/last_replay_file。
- multiplayer_options、source、resources/effects、Practice/设备配置与世代标识应按生命周期保持身份；
  不能把 source、effects、资源对象、网络 core、设备采样、wall clock 一起恢复。

MP 的 GameEconomy **不是 POD**：score_units 为 i32&。
seat 1/2 owned_economy 绑定 session 共享分，seat 0 又直接引用 session economy。
现有 copy ctor/operator= 可作 typed value snapshot/restore，保留 live 引用绑定；
显式快照时共享 score 只设一个 owner，各席保留 power/point_value/communication/lives/life_fragments/
difficulty/rank/max_power/power_step/graze/max_point_value。

证据：game/GameSession.hpp:40–133；game/GameEconomy.hpp:6–23、GameEconomy.cpp:5–10；
game/MultiplayerBattle.cpp:16–31。

### GameBattle 自身与基类值

- EnemyFrameWorld：practice_time_lock；player_state/special_active/spell_id；
  player_flags/spell_flags/bomb_animation_flags；special_ending/target_locked/target；
  countdown_seconds/countdown_hundredths/spell_elapsed/spell_bonus/shared_spell_state；camera_position。
- LaserWorld::rate/player 与 ShotWorld::enemies；这些是多态基类中的显式字段，不能复制 vptr。
- transition_timer、transitioning/transition_started/stage_active、result_animation。
- player_input、cancellation；spell_backgrounds[2] 和 callback_shared_visual 的完整 AnmVm。
- spell_titles[3]/spell_circle；frame、initialized/player_live/game_over_requested、practice_high、
  replaying/demo、replay_entry、last_error、typed error。
- enemy_environment：random/rate/player_position/camera_delta/rank/difficulty/character/subtype/
  shared_integers/enemy_count/boss。shared_random 和 callback/user 绑定地址保持稳定。
- enemy_commands：practice_time_lock、bosses、manager_flags、health_segments、remaining_phases、
  stage_section/section_frame。其 manager、stage、world 服务指针须与同一资源图匹配。
- EnemyAnimations::resources 为纯资源指针/id 表；稳定 stage 通常不变，但跨 stage 不能假定仍指旧资源。
- events、dialogue_text_requests 为 typed vector 快照/事务输出，不复制容器头。
- fades.effects、shakes.effects 是 typed vector；shakes.offset 也恢复。
- popups.entries/cursor、spell_timing 全字段。
  MP Application::multiplayer_tick 没有普通路径的 SDL wall-clock spell_timing.update；
  不要为接回滚反而引入新实时采样。
- mp_fragments/mp_wipe_frames/mp_tick/mp_effect_seat/mp_graze_mask/mp_rank/mp_enemy_graze，
  mp_paused/mp_finished/mp_failed；mp_damage_remainders 用 typed map，key 依赖 Enemy 地址稳定。

Bullet/Item/Laser/Enemy/ANM/Deformation 各用自己的 owner 接口。
EnemyCallbacks、ItemRewards 等仅含稳定引用的服务对象不需要对象头快照；
其写入目标必须由上列具体 owner 覆盖。

证据：game/GameBattle.hpp:47–155；game/EnemyFrame.hpp:5–37；game/EnemyState.hpp:66–76；
game/EnemyCommands.hpp:19–39；game/EnemyAnimations.hpp:6–26；game/LaserState.hpp:40–60；
game/ShotManager.hpp:26–32；sdl/Application.cpp:220–262 与普通路径 :528。

## 3. 逐席 Player / Bomb：稳定地址与完整字段

同一 GameBattle 内 pilots、PlayerFrame、BombController 可以保持地址。
seat 0 的 pilot.player/bomb 只是 world.player/bomb alias；不要重复捕获同一块。

### MultiplayerPilot

- input 与完整 GameInput keys（held/previous/pressed/released/long_repeat/short_repeat/counts/auto_focus_frames）。
- owned_economy 的值与 owned_communication（seat 0 的 live economy/communication 已由 world 捕获）。
- ghost/life_latched/force_attract；
  life_hold/power_taps/power_gap/ghost_clock；ghost_dx/ghost_dy/ghost_random/challenge_misses。
- 继承 ShotWorld::enemies。
- world/seat/economy/communication/rewards 引用、owned_player/owned_bomb 所有权与 raw aliases 不复制对象头。
- mp_stage_reset、game_over、revive 会改 options/body/shots/damage areas/Bomb；
  Ghost 不代表 player/Bomb 的旧状态可以不捕获。

### PlayerFrame / PlayerMotion / ShotManager

- PlayerFrame：state、input、spell、last_error。
- PlayerMotion：state、options[8]、完整 body AnmVm、focus_animation、last_error。
- ShotManager：player、schedule、laser_active[9]、damage_areas.areas[32]、
  locked_target、target_locked、last_error、private shots[256] 完整槽。
- converted_laser_spec 是每个 manager 内的固定 ShotSpec；
  ShotState::spec 可指向该对象或稳定 SHT 资源，保留原 manager/SHT 身份。
- 这些 class 含引用/虚接口；建议显式 capture 接口或纯值 snapshot，不能 sizeof(PlayerFrame) memcpy。
- private shots 的第一版可整池捕获；若之后稀疏化，inactive 初始化前的整槽屏障必须同时补齐。

### BombController

- state（elapsed/secondary、active、animation/auxiliary、position/radius/growth/started_during_spell）；
  selection、last_error。
- mesh 的逻辑指针及 ScreenDeformation owner；不是 unique_ptr 头复制。
- selection 2 的 ANM after_update=ring_noise，reserved_418 指向 **BombController this**。
  Deformation pool 只能解决 mesh 寿命，不能替代 Bomb owner 保活。
- 跨 stage 当前为每席重建 Bomb；因此需先 drain 历史，或以后另外实现 Bomb retirement。
- selection 5 护盾的 secondary、player flags、damage areas、power 退款、共享 invincibility、
  shared_integers[1] 都是恢复范围，不能只保存 active/elapsed。

证据：game/MultiplayerBattle.hpp:13–27、MultiplayerBattle.cpp:18–96；
game/PlayerFrame.hpp:18–54；game/PlayerMotion.hpp:22–53；game/ShotManager.hpp:10–54；
game/BombController.hpp:16–44、BombController.cpp:7–11、43、74、79–104、145–193。

## 4. Stage：资源里也有可变帧状态

StageState 可完整 journal（包含 camera、Timers、11 个内嵌 ANM VM、draw counters）。
额外必须覆盖：

- object_animations vector 中每个完整 AnmVm。只在容量/长度已固定时捕获 data bytes；
  不能恢复 vector 头或在旧 journal 尚存时 resize 导致地址变化。
- resource.objects[].flags：start_objects/update_objects 改 active 位，Draw 又写可见位。
- resource.instances[].flags：draw_layer 写裁剪/可见状态。
- mutable flags 用 typed 数组恢复；无需深拷贝只读 primitives/instructions，
  但恢复时校验相同 Stage/资源结构、数量、VM data 地址。
- error typed 字符串；MP tint_owner/tint_source 及 frozen_draw 状态（或明确呈现侧不参与权威资格）。
- mesh 逻辑身份 + deformation pool typed 数据。
- manager/animations/text/background_color 引用、file_id、active_camera、practice_game_frame 绑定保持原身份。

Stage::initialize 会复制资源并 memset state；start_objects 会 resize object_animations。
有活跃或保留中的历史时必须在这些写入前拒绝结构变化，不能仅检查当前 journal 是否 open。
最小组件可要求初次初始化/延迟 start_objects 完成于初始确认 frontier；
以后跨 stage overlap 同样有确认屏障。

Stage Draw 的实际写入不止 fade：
draw_background 写 camera、flags/tint、fades、draw counters/effects_enabled；
draw_layer 写 resource flags、VM position/scale/flags、renderer.draw 的 VM 缓存；
draw_foreground 推进 fade_timer、设置 outgoing 隐藏位。
现有 multiplayer_frozen_draw 只封住部分分支，不是一般只读 Draw。

证据：game/Stage.hpp；Stage.cpp:15–54；StageDraw.cpp:10–76、103–124；
MultiplayerPresentation.cpp:47–49。

## 5. Deformation owner 合同

ScreenDeformation callback 通过 VM.reserved_418 解引用稳定 owner，然后读取该 owner 的 vectors；
不保留 vector.data() 指针。因此 vectors 可 typed 复制恢复，owner 地址必须固定。

- columns/rows、vertices/positions/animations 的 typed 值、active/逻辑分配 frontier 同时恢复。
- logical release 执行原 EnemyAnimations::erase 副作用；erase 只是给 VM 置删除位。
- 本帧旧 strip 仍可能 Draw，release 不得当场清空旧 vectors/尺寸；
  至少等旧 animations 中所有非零 id 不再 manager.find 后才能复用 payload。
- initialize 部分失败也可能已有 strip + geometry，失败槽须同样逻辑退休，不能直接销毁 owner。
- 物理池析构不能对已逻辑释放的 owner 重复 erase。
- ANM journal Undo 完成 → typed pool metadata 恢复 → 未来 checkpoint 退休。
  中间不运行 callback/Draw；跨越的所有 geometry leases 在 Undo 完成前保留。
- Stage、Bomb、EnemyState 中的 raw deformation 指针由各自世界 owner 恢复；
  pool 本身不解决世界/Stage/Bomb 物理销毁边界。

证据：game/ScreenDeformation.cpp:6–26；EnemyAnimations.cpp:12；
Stage.cpp:35–38；BombController.cpp:43、74、104、146。

## 6. Dialogue / HUD / 文本 / Compositor

### 语义 owner

- DialogueState、active/character/stage/last_opcode、error、private end 指针。
  state.instruction/end 引用 StageResources.messages，必须保留消息 vector 原地址。
  Dialogue::begin 可原位复用；同一 stage 不必重建 owner。
- HUD：lives[9]/digits[2]/communication[4]/indicator 完整 VM；
  elapsed/previous_seconds/score/displayed_health/target_health/health；
  frame_animation/boss_name/stars/difficulty_intro/difficulty_label/bonus_digits/spell_notice/item_notice/
  spell_time_animation/show_spell_time/practice_skip_logo/multiplayer。
- SpellController：timer/initial_bonus/timeout/frame_count/circle_position/name/selection/stage/
  translation_rank/replay/bomb_active/practice_time_lock；SpellRecords 在 begin/end 被改，必须保存。
- StageCompletion：mode/state；ClearRecords 在 complete 被改；稳定多人 economies views 保留绑定。
- AsciiText：spacing 与 requests typed vector，其中每条含 std::string。
- SceneCompositor：copies[3] 完整 VM、full/play/world_camera、region、clear_color；
  private resource 指针固定 stage 范围不变，reset/initialize 在屏障侧。
- ANM manager 只捕获自己管理的独立 VM；这里所有内嵌 VM 必须由所属 owner 捕获。
- 另外捕获 battle.mp_presentation / hitbox_presentation 两个独立 ANM manager；
  mp_life_icons、mp_communication_icons、modes/hidden/display_lives/display_fragments、hitbox_marker。
  它们虽是呈现用途，但 update 消费自己的时钟/RNG/动画状态，不能在纠正时任意遗留未来状态。

### 动态文字不是仅一个 queue

Application::text_events 在 Draw 前调用 FontDevice::text；
后者写 GraphicsDevice::Texture::image.pixels、增加 revision，并写 vm.flags|=1。
源 AnmResource.textures[].pixels 不是这个 live CPU 图像的 owner。

最小可交付方案二选一：

1. 先把实际文字写入作为前置确认屏障，保护首次符卡/对白/菜单文字帧；
   对应帧确认执行后，新的 checkpoint 从完整 Draw 之后开始。
2. 实现 live 文字纹理区域/整纹理前像，并在恢复后正确 dirty/upload/invalidate；
   VM flag 与消息队列也要恢复。不能仅重放某一帧的新文字请求，因为旧文字可能未重写。

GPU render targets 与 Pause 截屏另有原始帧依赖。
初版保留 authored GPU 路径，不因 CPU 世界可恢复就宣称历史 GPU submission 均可省略。

证据：game/Dialogue.hpp、Dialogue.cpp:10–21；Hud.hpp；SpellController.cpp:11–26、53–62；
StageCompletion.cpp:12–88；AsciiText.hpp；SceneCompositor.cpp:17–35；
sdl/Application.cpp:384–390；sdl/FontDevice.cpp:115–138；sdl/GraphicsDevice.cpp:103–114。

## 7. Pause/menu 的准确范围

MP pause 使用 battle.mp_presentation，combat animations.rate=0；
PauseMenu::begin 仍创建 text script75 截屏背景并 capture_requested=true。
Resume 会销毁 PauseMenu，Restart/Title 属协议世代操作；
Pause 的 Replay Save 仍进入原 25 槽/name-entry 流程。

若以后让稳定菜单动画参与预测，需要 typed PauseMenu snapshot：
state/forced_exit/timer/elapsed、cursor/name_cursor、
menu_animation/background_animation、replay/capture_requested、
replay_files shared_ptr 数组、entered_name/name_length、completed/stage/unranked/selection/difficulty/
result_score/continues/slowdown/timestamp/practice、
scan_requested/save_requested/recording_metadata_requested/practice_save_disabled、
sounds/action/error、multiplayer_result/multiplayer_pause。
其动画/文本/分数引用、对象寿命与外部扫描结果须分别处理；
捕获字段不等于允许预测文件保存或截屏。

首版确认模式覆盖整个 Pause/result/Ending/Title 范围，可保留原交互语义并避开菜单资源新 owner 扩张。
不将 pause 强制延长或改为本地暂停；不同端仍使用同帧全席输入。

证据：game/GameSessionMultiplayer.cpp:56–128；PauseMenu.hpp:17–44；
PauseMenu.cpp:16–21；MultiplayerMenus.cpp:4–43。

## 8. 现有 helper 的复用与验证边界

### ANM 脚本参数本身也可能被写入

AnmInterpreter.cpp 的 int_dest/float_dest 对未设置 destination references 位的参数，
直接返回当前 AnmInstruction 内参数地址；算术/RNG/三角函数等指令随后可写 AnmScript.bytes。
不能因为资源对象在同一 stage 不替换，就假定 script bytes 恒定只读。
这需要真实 interpreter 写前捕获、稳定脚本存储寿命及字节恢复；具体资产是否使用该形式另行实测。
Stage 只拥有其内嵌 VM 和 STD mutable flags，引用的 core/stage ANM 脚本写入由共享 ANM/世界资源 owner 处理。

### 已实施 Stage 接口（第三轮组件通过，两轮失败保留）

StageRollbackFrame + Begin/End/Validate/RestoreRollbackFrame 已落源码；
Validate 必须在全局 UndoTo 前调用，核对 owner、frame、固定 VM data/长度、resource flags 容器 data/长度。
frame 持有历史 token，Stage 仅 weak 观察；有任何保留的 frame 时，initialize/start_objects 在写入前拒绝。
retained frame 的退休必须晚于其 journal 记录退休，不能先删除 token 再保留指向旧 VM data 的 journal。
MP Stage 使用构造时绑定 manager 的 ScreenDeformationPool 和 raw mesh；ordinary unique_ptr 路径保持。
组件专属入口 portable/multiplayer/stage-rollback.mjs，真实 authored STD/ANM/Draw，设备边界为记录器；
root 已执行首轮组件验证：`stage-component-01`，nig command `rqqKyB2tGuSpmClU25JjaP5-`，exit 1。
ordinary 链接缺 `SceneFog::set` 与 `sample(FogInterpolator&, const float*)`，未进入行为测试；
真实实现位于 `th11_web/cpp/game/FogInterpolation.cpp`，已补入专属 runner 的编译源清单，未使用 stub。
原失败收据和 stderr 保留在 `artifacts/rollback-smoothness/stage-component-01/`，不得覆盖。
root 第二轮 command `mVCYt1b7FBdmGaZ7DxfHaFTc`，exit 1，证据目录 `artifacts/rollback-smoothness/stage-component-02/`。
ordinary 通过 1 group / 19 checks；MP 进入 group 5，在 check 160 `open fresh shared checkpoint` 失败。
定位为专属测试在主动破坏容器 identity 后调用 `RollbackJournal::Clear()`，却未在下一次 capture 前 Reset；
common 的 Clear 同时使 `configured_=false`，因此拒绝后续 BeginFrame，尚无 Stage 生产缺陷证据。
已在原负面测试与有效后续恢复测试之间显式 `Reset(config())` 并断言成功，保留所有原用例和恢复比较。
root 已串行运行 `node portable/multiplayer/stage-rollback.mjs --output artifacts/rollback-smoothness/stage-component-03`，
command `XW9BlZCQ2xdzkZTlrWeZwmf_`，exit 0；ordinary **1 group / 19 checks**，MP **6 groups / 194 checks**。
新收据 `artifacts/rollback-smoothness/stage-component-03/receipt.json`，01/02 失败收据均保留。
实际覆盖真实 STD/ANM/语义 Draw rewind/replay、外部 geometry、直接跨两帧 Undo、deformation 回调 owner 同址重演、
保留历史时的结构增长门、foreign/expired/relocated preflight、容量不足拒绝封帧。
这是 Stage 组件正确性通过，不是完整世界、浏览器或 4× 性能通过。

### 可复用与不可替代

可复用：

- 本轮新增 ANM / Bullet / Item / Laser owner 接口，按照各接口范围接入；
  不能对它们所属 VM 子范围另做部分重叠 Touch。
- GameEconomy 的 value copy/assignment，可保留共享分引用绑定。
- 各 POD state / fixed arrays（先 static_assert trivially_copyable）与明确 typed containers。

不可当 world checkpoint 使用：

- GameSession::save_replay 的 recording snapshot 仅复制 recorder。
- MP Replay chapter 仅是 confirmed 输入位置；Application::multiplayer_seek 重新建 world 并从 frame0 重放。
- multiplayer_hash / mp_hash 是已有抽样 canonical，并未覆盖全部恢复状态：
  完整 ANM/HUD/text资源、Stage flags、Player/Bomb/shot 所有字节等不能只凭该 hash 相同判定恢复正确。

源码审计未发现本轮新增接口之外的既有 TH11 完整世界 checkpoint helper。
组件通过后，世界 oracle 仍须比较参考、预测纠正、再次重演的完整 owner 值/拓扑与副作用，
涵盖 stage Draw、逐席触控/射击、六机体 Bomb/护盾、Ghost/救援、对白与资源屏障。
这份清单只列实际源码 owner 与已确定边界，不宣称上述 world oracle 已实现或通过。

