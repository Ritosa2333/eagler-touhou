# TH11MP 预测运行时接入审计与实施提案（2026-10-13）

## 状态、范围与原始证据

本文件先记录只读审计，随后获授权实施 title Runtime；实际新增合同与待测状态见 §9。
全部工作区操作通过 nig。本代理未运行命令；TH10 保持封存，common pin 与稳定 Launcher 未改。
TH11 世界/纹理/driver 由 root 分别验证与接线，本文不宣称整作回滚或性能通过。

审计基线：D:/workspace/eagler/worktrees/th11-rollback-smoothness，起始 HEAD d32390730ae837fb1333bc1f1d03d746f4697936，
common pin 984a8a10834c35fe9ef86c4ffe03daf1fb61f87c。
行号对应本轮读取时文本；root 正在添加 owner 与屏障，应以函数名复核。

路径缩写：

- T = D:/workspace/eagler/worktrees/th11-rollback-smoothness。
- C = T/third_party/eagler-common。
- L = D:/workspace/eagler/eagler-touhou。
- S8 = D:/workspace/eagler/worktrees/th08-network-cadence-100ms。
- S9 = D:/workspace/eagler/worktrees/th09-managed-touch-4x。

已重读原始跨作架构 §§3–6、8–9：
D:/workspace/eagler/worktrees/multiplayer-rollback-architecture/docs/playbooks/multiplayer-rollback-architecture.zh-CN.md:122–429、558–719。
确认成功树源码见 §8；不将 TH10 默认关闭或未通过实验移入默认策略。

## 1. 结论：可以在当前 common pin 上实现，但必须完整拆开五个前沿

当前 common 已有真实输入历史、预测、纠正请求和 RewindSimulationTo，缺的是 TH11 title driver 的完整事务与输出所有权。
最小实现保持 common pin，先使用 LocalCaptured 立即发出本次新输入。尾包合并不是本轮正确性的前置条件。

必须独立保留：

1. next：下一次模拟帧，纠正时可倒退。
2. next_capture：物理采样前沿，只在真正新 forward 输入被接纳后前进，不回滚。
3. correction_end：纠正开始前旧模拟前沿；跨回调/重复纠正保持，未激活用 INVALID_FRAME。
4. confirmed_input：连续实际输入前沿；只表示输入，不表示世界已纠正。
5. committed_output：已完成 Update + 语义 Draw + 封帧、纠正结束、不能再被保留 checkpoint 推翻的单调输出水位。

此外 Host 触控视图与完整显示视图各自保留完整状态及身份，不能读取半恢复世界。
这些前沿、网络队列、ACK、动作 serial、墙钟和已捕获输入不进入 WorldRollback。

## 2. 当前纯锁步接缝及最小修改图

| 接缝 | 当前真实位置 | 必须改变的合同 |
| --- | --- | --- |
| 模式/配置 | T/th11_web/cpp/multiplayer/SessionSetup.hpp:10–17；SessionSetup.cpp:10–29 | 当前 Version5 / 22 words，w14 强制1，无 mode/P limit 字段；需显式模式、预测上限、测得 reserve，并把全部确定性配置纳入 GameplayAbi |
| native core | T/th11_web/cpp/multiplayer/NetplayRuntime.cpp:33–43 | allowPrediction=false、predictableButtons=0；只在 title world-ready 且真实可恢复窗口内允许预测 |
| 采样/发送 | NetplayRuntime.cpp:65–70；T/th11_web/cpp/sdl/Application.cpp:657–718 | 采样从模拟函数分离；新 forward 前沿一次采样 + LocalCaptured，在昂贵 Restore 前完成；恢复 tick 只 Prepare 保留历史 |
| 模拟提交 | NetplayRuntime.cpp:101–108；Application.cpp:220–262 | 不再把 predictedMask 当错误；完整事务成功后重新核对 expected decision，再 MarkSimulated；恢复期间禁止外部提交 |
| 世界恢复 | T/th11_web/cpp/game/WorldRollback.hpp:44–61；WorldRollback.cpp:14–27、127–163 | 稳定 stage owner 是基础；前置 Validate → 完整 shared Undo → 所有 typed restore → 退休所有跨越 frame 的 lease |
| 音频/文件 | Application.cpp:142–151、193–217、241–260、354–380 | 阶段音乐、pause/resume、音效和元数据保存不能从预测世界立即发布；建立逐帧输出命令和确认元数据 |
| 活纹理/语义 Draw | Application.cpp:345–353、382–400、250–255 | capture_pause_frame 是不可逆 GPU 动作；FontDevice 写 live texture；每历史帧保留语义 Draw，纹理由真实 SDL owner 恢复或走确认屏障 |
| Replay | T/th11_web/cpp/multiplayer/ReplayArchive.cpp:10–25、35–38；Application.cpp:257、758–766 | 只 append 经过 title 输出门的 ConfirmedInputs；逐帧 stage/score/completion stamp 必须来自该帧，不能用未来世界的值 |
| 观战 | NetplayRuntime.cpp:117–192 | 原来从 Pump/MarkSimulated 直接 Publish；需经过 corrected + checkpoint 退休边界，暂停纠正时不消耗新观战输入；保留有界 backlog/终态尾包 |
| rAF / pacing | Application.cpp:888–909；T/portable/sdl/FrameCadence.hpp:8–15 | 现 cadence 先扣所有 offered ticks，预算 break 会丢未执行债务；新增 network wait / CPU yield 区分与有界归还，恢复完成仍尝试原应有 forward |
| JS/能力声明 | T/th11_web/sdl-runtime/multiplayer.mjs:13–47、110–114、143–149；T/portable/build.mjs:68、161–162；T/portable/package-eagler.mjs:46–50 | mode/P 校验、diagnostics、build/package attestation 必须同步真实行为；不能只改UI/硬编码0 |
| 房间/relay策略 | L/contracts/src/product-catalog.mts:569–579、723–730；L/launcher/web/src/launcher/multiplayer-runtime-options.mts:119–191；L/services/netplay/netplay-relay.mjs:1260–1303 | 现 TH11 rollbackLimit=0；稳定 Launcher 不改，本轮先把 title 支持做好，再独立测试整个产品契约后发布 |

## 3. 一次采样、提前发送与按钮/触控

### 3.1 真实旧路径

Application::sample_and_tick 在 :657–673 先 Poll SDL，NeedsCapture 为 false 时直接 multiplayer_tick，避免重复消费 gestures.sample。
:681 gestures.sample 会扣 confirm/bomb/escape ticks；:698 sample_controller 和 keyboard_keys 取设备；
:715–718 Capture 后进入 multiplayer_tick；后者 :222 又 Pump，然后 Update。

这个已有一次采样保护应保留。接入纠正后不能让 sample_and_tick 成为历史 replay 的入口。
推荐拆成 ServiceDeviceEvents、CaptureForwardInput、RunLogicalFrame；名称可调整，所有权不能混合。

恢复前以当前完整 forward 前沿捕获一次；捕获后再开始首次 Restore。
跨回调 correction continuation 不再捕获，不因新 DOM 事件或当次 rAF 频率重复消耗 pulse。
恢复提前碰到生命周期边界时，已捕获旧未来输入仍保留，随后走到对应帧只读取它，
不能把较老帧再次 LocalCaptured：C/src/netplay/SessionChannel.cpp:178–189 明确拒绝倒退 latestCapture。

捕获成功先 ScheduleLocalInput，再 LocalCaptured，发送失败保留明确错误；
不在 half-world 采样，也不把同一 capture frame 的本地内容改写。
C/src/netplay/NetplayCore.cpp:89–130 已提供 F→F+D、中性 lead-in 与 HasLocalCapture；
但没有替 adapter 维护物理采样游标。

### 3.2 TH11 输入合同，不能照搬 TH08 数字

T/th11_web/cpp/multiplayer/InputLanes.cpp:4–21：
DirectTouch 是场地绝对目标 x∈[-184,184], y∈[32,432]；Joystick 是归一原始轴。
T/th11_web/cpp/game/PlayerMotion.cpp:116–121 在模拟帧的权威位置上算 DirectTouch 速度，
Joystick 按当帧 focus speed 解释。保持此输入表示，不改为 sender 预计算速度或 fresh delta。

T/th11_web/cpp/game/GameInput.cpp:3–14：
射击1、Bomb2、C4、低速8、方向0xf0；InputLanes.hpp:13–14 是独立 Pause0x8000、Restart0x4000。
可作为首版验证候选的 held mask 为 1|8|0xf0，方向独立限制；Bomb、C、Enter、Skip、Pause、Restart 不预测。
TH08 的 focus4、TH09 的 focus4 均不能照搬到 TH11。
方向 horizon 是独立可验证策略，不能用它偷偷缩小整个可恢复窗口。

TH11 的射击/低速也参与资源转移：
T/th11_web/cpp/game/MultiplayerBattle.cpp:185–200 使用 low-speed hold 与射击 pressed 五连击；
因此键边沿、power_taps/power_gap/life_hold/ghost 和全部逐席状态必须一起恢复，
验证不能只比较位置或子弹数。迟来的释放仍须完整纠正，不能使用“等价输入”跳过。

### 3.3 不只是 getter：已有浏览器入口直接写模拟字段

Application.cpp:610–632 的 touch_state 直接读 live session / seat；:649 pointer 和 :681 sample 都使用它。
T/portable/input/TouchController.hpp:48–65 会以该坐标建立/恢复 drag anchor；半恢复位置会进入新手势。

更直接的接缝在 Application.cpp：

- :635 clear_inputs；
- :655 cancel_touch；
- :920 th11_touch_options；
- :933 sdl_touch_mode。

以上函数均会把 battle->player_input.movement.touch_mode=0。它们可能由浏览器事件在恢复片之间调用，
从而修改历史世界，绕过已保留的帧输入。MP 下应仅中和 device/gestures，
由下一次获准 capture 把取消带入输入；ordinary 原行为保留。

新增 TH11 专属完整 TouchState 缓存，不能机械照抄 TH10 8-word ABI。
身份至少 session/generation/world/seat/count/selection；另携带 stage instance、context、ready、位置、速度和边界。
首次 Restore 前保留完整逻辑状态；Update、semantic Draw、封帧结束且整段 correction 结束后发布。
开放帧、resim、Correcting 期间均只返回缓存；重开、换座、Replay/观战、stop 失效。
首 tick 前允许明确的冷启动视图，不伪称完整帧。
TouchController::reset 在 :34 保留动作 serial，不能因增加缓存重新归零；fire toggle 合同也保留。

## 4. 最小 title driver：恢复与前置确认屏障

建议独立 RollbackDriver 组合 WorldRollback、common journal、SDL texture owner、FrameOutputs，
Application 提供一次采样、原生事务、semantic Draw 和最终 Present 的服务接口。
避免直接把所有纠正逻辑塞进 rAF 循环或修改每个 Gameplay owner 的网络判断。

### 4.1 完整事务及失败分类

每次新/历史 tick：
Prepare expected → Begin journal/world/texture/output frame → session.update_multiplayer →
收集逻辑音频命令和纹理语义 → session.draw semantic → 解除所有 observer →
End journals → 再验证并 MarkSimulated → 保存完整显示/触控视图资格。

将 return 原因至少分为 Advanced、NetworkWait、RecoveryYield、BoundaryWait、Fatal。
WorldRollback::BoundaryRequested 是可回到前像的边界信号，不是容忍任意 native failure。
journal 容量失败仍是 Fatal，不能用不完整历史继续，也不能将 Failed journal 当正常可撤销帧。

### 4.2 纠正

Pump 收到最早 mismatch r 后，保存旧 next 为 correction_end；重入纠正不能覆盖旧目标。
Validate frame owner/结构，然后 UndoTo(r) 跨全部 journal 记录；保留每份 crossed geometry/owner lease。
所有 typed owner 与纹理恢复完成，才删除被撤销快照与旧 pending outputs。
调用 core.RewindSimulationTo(r) 只撤销 used decisions，保留真实/本地输入与 ACK。

历史 tick 通过 Prepare 读取保留输入，完整执行 Update + authored Draw + Seal。
新包推翻已重演前缀时，再从最早 r 恢复，correction_end 保持旧值。
仅追平或确认的生命周期提前终止时解除 Correcting。
不要直接 ClearRollbackRequest 来掩盖没有恢复的世界。

### 4.3 Pause / next_stage 的具体边界

root 已在 GameSessionMultiplayer.cpp:6–8、60–61、94–95、107–108、123–124
与 GameBattle.cpp:132–134 增加开 observer 时的破坏前屏障。
但这些信号发生前，update_multiplayer 已更新 pause-held / multiplayer_frame，战斗也可能执行了大部分 tick。
所以收到 BoundaryRequested 不能原地解绑 observer 后继续；应回滚本次未提交事务，等待该帧完整实际输入，
确认并纠正所有先前前缀、提交其输出、清除所有可恢复历史，再在同一帧按原路径重做一次。

该帧未 MarkSimulated 时，不要强行 core.RewindSimulationTo(frame)：common 只接受已经模拟区间。
若此前处于 correction，要区分已退休的旧 used tail 与当前未提交 tick，保持 next/capture 的精确关系。
可以显式记录 boundary_frame，随后在无 history、全部实际输入条件下按原路径运行。

GameBattle.cpp:138–175 next_stage 会换资源、销毁 Dialogue/Bomb、重建逐席 Bomb；
:204–215 之后还有 start_objects、activate_stage、outgoing_stage.reset；
:356–360 才 retire_previous。首版整个 overlap 保持 exact-only，
直到 WorldRollback::CanCapture 再次满足 stable stage，不能刚执行 next_stage 就恢复预测。

Pause/Ending/Results/Menu 首版也 exact-only。
capture_pause_frame、GPU readback、文件写、资源 preload/unload 在真实动作之前入门，不能事后检查。
当前 SDL live texture owner若验证完成，可让正常战斗文本参与恢复；否则在 FontDevice::text 前请求同类边界，
不能只恢复 dialogue_text_requests 而保留错误的 texture pixels。

## 5. 确认输出、Replay、观战与终态

### 5.1 统一的可提交边界

NetplayRuntime::ConfirmedInputs 当前 :107 直接透传 core，必须加 title Correcting/open/rollback-request门。
common :381–400 虽核对实际输入和 used input、LastSimulated，却不知道 correction_end，
所以重演到一半的已确认 prefix 可能返回 true；成功两作的 wrapper 正是补这个边界。

driver Commit 顺序：

1. open/Correcting/rollback request 时不发布。
2. 用 min(continuous confirmed, LastSimulated) 限定候选前缀。
3. 逐帧 journal 先 DiscardBefore(safe+1)，然后才释放相应 typed frame；按真实最早 checkpoint 起点再压提交上限。
4. 按 frame 严格顺序发布一次音频/文件/Replay/观战；输出水位不随 Undo 回滚。
5. 未确认尾部丢弃或重演重建，不能在断线/stop 时以未来世界补成“已完成”。

音频命令至少含 Sound(positional x)、StopSound、StageMusic、BossMusic、MusicResume、MusicFade、
phase-driven pause/resume/game-over track，保留原出现顺序。
Application.cpp:241–249 的音乐相位与 :354–380 的 BattleEvent 都必须接管；
只延迟 effects.enqueue 仍会泄漏音乐。
AudioDevice.cpp:75 的 effects.process/淡出逻辑是每个被发布逻辑音频帧的工作；
:98 pump 则是持续音频服务，应在无新 forward/恢复yield回调也运行，二者不要混为一项。

### 5.2 Replay 元数据也是确认输出

ReplayArchive::Append 保留 all-seat exact input 与 stage label。
当前 Application.cpp:257 每 tick立即Append，需改为输出门内对每帧 ConfirmedInputs；
记录当帧 pre-update stage（保持原 :238/:257 的章节语义）、score/completed/terminal等确认stamp。

尤其 Application.cpp:213 与 :758–761 用当前 session.economy / multiplayer_completed 直接编码：
即便输入记录只含已确认帧，仍可能把预测分数/完成状态写入定期或退出保存。
必须用 committed metadata，不只是延迟 Append。
文件名、自动/命名slot隔离及 MaxFrames=250000 合同保留；不能先模拟第250001帧再发现不可存档。

扩展 SessionSetup words 时同步 Replay DescriptionBytes/Version、native Metadata、portable/browser/multiplayer-replay.mjs:16–33；
当前JS硬编码128 bytes、22 words、Version5、mode1、prediction0。
Replay播放依旧 all-seat exact、core D0，不因记录原局mode0就自行预测。
原房间D/模式是metadata，不能给已排好帧的Replay再加一次D。
章节仍是输入位置，Application.cpp:117–119 从0重放；不可冒用 rollback snapshot 当跨局持久章节。

### 5.3 观战与退役

现 NetplayRuntime.cpp:142–192 有 32包/2ms 发布预算、有界8192接收队列、输入环过旧停止观战、
独立 confirmed publication tail，均保留。
Publish 与 FreezePublicationTail 必须使用同一 title-safe上限；P1观战背压不能挡玩家退休。
mode0 的观战不需要 Adonis timing，见 §7。

Finish/BeginNextRun :193–208 已有双向终态 ACK fence、缓存旧局终态回复和 retired config，
这些网络状态不能放进世界快照。新driver退休前还需 no Correcting/no open/no pending rollback、
确认输出已覆盖终态及 history已清理。
新局重置恢复目标、pending画面、touch视图、输出水位与capture身份；旧serial基线按既有Host合同保留。
正常暂停后一端停止输入带来的最终等待是自然现象，不能“修”为单边无限推进。

## 6. bounded recovery 与渐进降速

当前 Application.cpp:894–903 的 cadence.advance 一次先扣全部due，8ms break只停止循环没有退还；
若直接加分片恢复，会把CPU工作让出误算成没有tick欠账。必须单独实现或扩展MP cadence：

- 真缺信息：保留一个已经到期pending tick，重试不再等新1/60信用，网络等待期间不无限积墙钟预测债。
- CPU恢复/追加tick让出：保留有界债务，退还 offered-completed，下一callback仍按明确规则累计有界elapsed。
- 完成恢复后尝试本callback原欠forward；最后历史事务与新增事务都用同一callback起点计算门槛。
- 预算只决定是否开始下一完整事务，不会抢占一个昂贵Update/Restore/Draw；不得承诺每callback<某固定ms。
- 恢复中不Present半世界，不公布中间触控getter；恢复完成后即使无法开始另一个forward，也须允许提交已有完整纠正画面。

C/include/eagler/netplay/PendingFrameSchedule.hpp:13–22 的 retry一律忽略新增elapsed，
只能直接复用“网络pending”语义，不能拿 blocked(true)涵盖所有CPU恢复。
RollbackReplayBudget.hpp:8–20 是TH07明确experimental的4ms/4帧策略，不默认移植。
FrameBudget.hpp:9–23 是追加tick门槛，可借形状，不把TH07阈值当TH11验收证据。

缺口制动用 F=max(next,有效correction_end)，A=continuous remote confirmed+1，B=max(0,F-A)；
先Pump再更新B。max received乱序帧与物理capture计数都不是A/F。
TH09已采用固定4帧死区、5/6/≥7到1⅓/1⅔/2间隔，50ms加、150ms退的平滑，
可作为有来源的初始策略候选，须与TH11实际历史上限对应后验证，不称RTT自适应。
只缩放新增elapsed，保留旧小数debt；最终高刷alpha与同一逻辑phase一致。
不以本地callback成本主动增加D。B也可能由远端CPU/派发迟滞造成，诊断不能假称已经证明纯网络原因。
TH10 rAF补偿、尾部12ms实验均不默认接入。

## 7. 配置模式、当前 common可复用性与产品门

### 7.1 推荐首个完整协议方向

Direct Rollback mode0 / D0 与现有 measured Delay mode1 两条路径都保留。
mode0 通过 common SessionGate直接建连，关闭 AdonisPhase；mode1保留现有实际通道测量。
可以在native显式支持mode2测量Hybrid，但不能把reserve1..2当整个rollback容量。

最终实施选择 Version6 同 22 words，w14 为 mode；不追加字段或改变 Replay description 偏移。
新增 mode0/2 的 GameplayAbi 包含 mode 与固定 prediction_limit=12；mode1 ABI 保持旧值。
measured reserve 沿 common 协商，属于运行时 phase 建议，不另改世界规则 ABI，详见 §9。
12 帧目前是实施与组件测试门槛，不是整作性能交付结论；正式能力开放仍需完整验证。

### 7.2 mode0 不需要更换 common 或伪造校准

C/include/eagler/netplay/AdonisStartup.hpp:36–43 只接受Delay/Hybrid；
mode0 不调用Prepare/Begin，而是 channel.BeginSession(gate.Config(),now,policy)。
policy.adonisPhase=false且adonisPredictionFrames=0；disabled AdonisConnection仍直接转发wire。
T/NetplayRuntime::Connect/BeginNextRun 分别按mode分支，Clear不可误删正常退休ACK缓存。
参考成功S8/th08_web/cpp/multiplayer/NetworkConnection.cpp:25–26、170–176。

Direct spectator：setup已经给出session/seed/loadouts/build/D0/mode/limit，连接时建立此ABI并timing_ready=true；
现SpectatorFramePacket每帧含session、gameplayAbi、人数、frame，照常严格核验即可。
mode0 Publish/FreezeTail不生成MakeAdonisSpectatorTiming。
mode1/2仍先收严格测量timing，然后以有效D/reserve重建ABI。
C/include/eagler/netplay/AdonisSpectatorTiming.hpp:10 只接受mode1/2，不要放宽或伪造rtt/sample。
成功S8 NetplayRuntime.cpp:84–99、NetworkConnection.cpp:114–138已经采用此分流。
L/services/netplay/spectator-frame.mjs:96–109已允许TH11 confirmed frame packet，无需为mode0新增wire格式。

### 7.3 现有 common API可直接复用/不能照搬

| API | 本轮结论 |
| --- | --- |
| ScheduleLocalInput / HasLocalCapture / LocalFrameForCapture | 直接复用；title另管一次采样游标 |
| PrepareFrame / MarkSimulated / UsedInput | 直接复用；prediction模式MarkSimulated本身不验证严格next/expected，title wrapper必须补，见Core.cpp:317–334 |
| HasRollbackRequest / RewindSimulationTo | 直接复用；只处理used decisions，真实world必须先按有效checkpoint恢复 |
| ConfirmedInputs | 作为最后逐座实际/used校验；不能替title Correcting/输出寿命门 |
| SessionGate / SessionChannel BeginSession / Pump / LocalCaptured | 直接复用mode0 + repair/ACK/退休；LocalCaptured可满足早发 |
| SessionChannel deferred ordinary batching | 当前pin没有；hpp:49–56只有Pump/LocalCaptured。保持pin先不用，不能调用成功树更新common中的不存在API |
| AdonisConnection / AdonisStartup / SpectatorTiming | mode1/2可复用，manual D包括0保持权威；自动Hybrid减reserve不是加D。mode0绕开测量 |
| PendingFrameSchedule / RollbackReplayBudget | 语义与阈值不能整包移植，见§6 |
| SubmitEquivalentRemoteInput / DirectTouchEquivalence | 不采用；TH11资源转移/机体/触控完整等价未证明 |
| Journal extendPrevious | 首版禁止，逐帧checkpoint；资源退休与输出边界尚未证明跨帧span |
| InputReplay / packet codecs | 继续使用；不要另造网络协议/录像输入编码 |

### 7.4 Launcher与relay必须后续完整验证，当前稳定树不改

L/contracts/src/product-catalog.mts:573当前 rollbackLimit=0。
L/launcher/web/src/launcher/multiplayer-runtime-options.mts:119–122、159–166会将回滚产品的请求mode0/2归一为mode0+D0；
L/services/netplay/netplay-relay.mjs:1297–1300也这样做。
因此只把TH11 catalog设为可回滚而native仍走Adonis Delay/Hybrid必失败。
上述Direct方案与现产品方向一致，但正式开启仍需独立Launcher/relay/Runtime同身份测试。

T/th11_web/sdl-runtime/multiplayer.mjs 的options校验、setup words、runtime globals、generation处理同步mode；
common browser/adonis-calibration.mjs:12已经跳过mode0，不能拿未测量状态发ready测量报告。
T/portable/build.mjs、package-eagler.mjs的能力attestation应在整个接线验证后真实更新；
保留ordinary/MP隔离以及same-build Replay要求。
launcher现有TH11 policy测试（tests/test-th11-multiplayer-policy.mjs、tests/ui/multiplayer-services.test.mjs:1175起）
是明确锁步断言，需要增加新能力/旧版本拒绝测试，不能删掉断言只求绿灯。

## 8. 成功两作的实际合同与本作最小下一步

### 8.1 已核对原始源码

- S8/th08_web/cpp/multiplayer/NetplayRuntime.hpp:27–37、68–95：
  next、next_capture、correction_end分离；Correcting时不新采样；F使用旧目标。
- 同cpp:151–160：ConfirmedInputs先拒Correcting/rollback request，再逐座核对used与actual。
- 同cpp:170–198：Begin保存end，Restart保留end，End仅完整追平或显式生命周期边界。
- S8/th08_web/cpp/multiplayer/RollbackDriver.cpp:90–108：
  Commit先拒纠正、退休安全journal前缀，再以FirstCheckpoint截断不可逆输出。
- 同cpp:274–350、410–438：
  完整历史事务之间让出；完成纠正后仍尝试欠的forward；无法前进仍PresentCorrection。
- S9/th09_web/cpp/multiplayer/RollbackSession.cpp:193–195、207–249：
  旧frontier早采样，Restored用max(next,replayEnd)，ConfirmedThrough在next<replayEnd时无效，
  ConfirmedInputs还调用common实际/used校验。
- 同cpp:34–47：绝对DirectTouch保留target，但TH09按钮位与本作不同。

不能照搬S8的纹理/几何visual抑制、TH09两人限制、各作defaultbudget、各自common新API；
TH11首版完整语义Draw与当前pin保持。

### 8.2 建议下一独立实施包及验收

第一包：TH11 title协议/前沿层，限定SessionSetup + NetplayRuntime及其真实native协议测试，
同步必要的Replay配置兼容测试；暂不启用UI能力，不改Launcher稳定树。
实现mode0直连gate、mode1保留、world-ready、独立capture/next/correction_end、
Begin/Restart/EndCorrection、strict MarkSimulated、title ConfirmedInputs和safe publish接口。
为未接driver情况默认 world-ready=false，不能有预测悄悄跑进旧Application。

最低行为验证应覆盖：
2P/3P，D0及Delay D0/2/9，首次前像、乱序洞、真实晚释放、重复纠正、
恢复中confirmed前进却不输出、捕获前沿不倒退、stage boundary缩短旧future、
wrong decision拒绝、Replay/spectator exact、mode0不发测量timing、
终态尾包与generation旧ACK。
由root串行运行并留精确源码/命令收据，不与world/纹理编译重叠。

第二包：独立driver + FrameOutputs + SDL Application接缝 + MP调度 + TouchState frontier；
需要已通过World/texture oracle。生产真正开始预测前，上述完整事务/屏障/输出/触控泄漏同时关闭。

第三包：正式Release身份、JS/packaging与独立Launcher/relay产品能力联调；
桌面生成触控移动，真实RTC与WS fallback、稳定网络与尖峰、4× CPU与原版对照。
指标同时包含逻辑净推进、完整显示间隔、恢复完成间隔、rAF入队/进入、输入早发时间、
B/D/mode、普通Update/semanticDraw/捕获/Undo/输出耗时。未通过不称交付。

## 9. 已实施的 title Runtime 合同（native 组件已通过）

本节覆盖前文只读提案；原始审计锚点留作旧路径证据。生产改动限
T/th11_web/cpp/multiplayer/{SessionSetup,NetplayRuntime}.{hpp,cpp}，
以及获得单独授权的 ReplayArchive.cpp::Metadata 有效配置复检。
未修改 common pin、成功作品、TH10、稳定 Launcher；Application/JS/driver 由 root 接线。
本代理没有运行命令、构建、测试或浏览器。root 已串行完成 native 验证和随后完整 JS↔native
交叉验证，证据见 §9.5–9.6；模块通过不外推为真实浏览器联机、整场音频或性能通过。

### 9.1 模式、旧 ABI 与录像格式

- setup Version6 保持原 22 words 与 128-byte Replay description，w14 为 mode。
- Version5 只接受 mode1；默认构造仍 Version5/mode1。Version6/mode1 的 GameplayAbi
  保持旧字段流完全相同。mode0/2 才追加固定标签、mode 与 prediction_limit=12。
- mode0 是 D0、manual、最大恢复窗口 P12，绕开测量但保留 SessionGate/repair/ACK/代次退休。
- mode1 原纯锁步，自动/手动 D 均走原 common。mode2 真实 Hybrid：
  common 选择有效 D 与 0–2 的 phase reserve，物理恢复上限仍为 12。
- measured_prediction 是已协商的 phase 建议，未占额外 words，也不冒充新的世界上限。
  有效 D 和既有 requested reserve 已参与 ABI；readonly 用记录模式做身份检查，
  内部 Core.inputDelay=0、allowPrediction=false。
- Replay Metadata 覆盖 effective input_delay 后重新 valid()。定向反例为
  mode0 记录伪造 D1，同时重算 GameplayAbi 和 checksum，仍必须被拒绝。

### 9.2 可供 Application 接线的确切方法

| 方法 | 合同 |
| --- | --- |
| NeedsCapture / CaptureFrame / Capture | 独立单调物理游标；新 forward 采样后立即 LocalCaptured，昂贵 Restore 前可采一次；纠正中不采样 |
| NextFrame / LastFrame | 下一逻辑帧与最后已 Mark 完整事务；世界尚未封帧时不能推进 |
| AllowsRollback / PredictionLimit / SetWorldReady | mode0/2 的预测资格和真实世界可恢复门；默认 false。关闭前必须无纠正/request 且旧 next 前缀连续 actual |
| BeginCorrection(first) | 必须先由 driver 恢复对应世界；first 等于最早请求、在未提交历史内且深度≤12；保存旧 next 为 correction_end |
| RestartCorrection(first) | 重复更早纠正仍保留原 correction_end 和 next_capture，绝不回滚设备/收包/ACK |
| EndCorrection(false) | 已完整追平旧目标且无新请求 |
| EndCorrection(true) | 显式生命周期缩短；next 前缀必须已实际对齐，废弃旧模拟 future，但保留已经采到的输入 |
| PredictionFrontier / MissingRemoteInputs | F=max(next,correction_end)，B=max(0,F−A)，A 为连续实际远端输入的下一帧 |
| InputsReconciledBeforeNext | 只表示 next 前缀实际输入就绪且无请求；并非 owner 退休或输出许可 |
| SetCommitLimit / CommittedThrough | driver 先退休对应 checkpoint/leases，随后单调授权 ≤LastFrame 且 common ConfirmedInputs 成功的帧 |
| ConfirmedInputs | 拒绝整个纠正区间、任何未处理 rollback request、超过 commit_limit；再由 common 核对 actual/used |
| Finish / BeginNextRun | 完整实际前缀已授权、无纠正/request 后才允许退休；保留 pinned common 旧代 ACK 生命周期 |

mode1/readonly 没有 title 预测历史，MarkSimulated 自动授权该 exact 帧以保留旧调用者。
mode0/2 永远不在 Mark 自动授权。SetWorldReady(false) 不是握手初始化 API，
握手未完成时返回 false 不能误报为游戏故障。

### 9.3 预测控制语义与网络降速

采用真实 TH11 mask **0xf9**：Shot1、Focus8、方向0xf0。方向仅短持 3 帧，
超过后方向中和，完整恢复窗口仍 P12；绝对触控目标与归一摇杆按 common 现有语义保持。
Bomb2、C4、菜单、Pause、Restart 不预测。

Shot/Focus 是 held 控制。持续预测同一 held 不制造新的 pressed；
真实释放/开始会请求完整纠正。TH11 资源赠送或救援可成为预测世界的可撤销结果，
不能因此把 Shot/Focus 每帧预测为零，更不能另设 transfer gating 改变玩法。
必须以 World 捕获逐席 held/pressed、power_taps/power_gap/life_hold/ghost 等后撤销结果；
原网络测试只证明输入/边沿，不替代 World oracle。

mode0 降速使用 TH09 同类固定策略：B≤4 保持 1×，4–7 帧缺口逐步到 2× interval，
增加时间尺度 50ms、回落 150ms，与 channel 的网络相位建议取较大值。
这是固定网络缺口死区，**不是自适应 RTT**，不以设备耗时增加输入 D。
rAF/CPU 恢复预算与实际 Present 连续性仍由 root 的调度层另行验证。

### 9.4 验证门及其局限

root 已执行的 native 串行命令（本树 cwd）：

```text
node portable/multiplayer/network-test.mjs --native-only --output artifacts/rollback-smoothness/network-runtime-01
```

runner 使用 O2/WASI32，记录 exact compiler/common/tree 身份、源文件 SHA-256、wasm/Replay SHA-256，
生成 fresh receipt 与完整 stdout/stderr；拒绝覆盖旧 receipt，结束时复核源文件未变。
原锁步 2P/3P、D0/2/9、challenge、乱序洞、captured ownership、跨代、全座 Replay
保留；pinned common calibration-retirement/session-channel/adonis-startup 和观战尾包回归保留。

新 network-rollback.cpp 包含 8 组真实 Runtime/Core/InputLanes/GameInput 行为：
模式/ABI、2P/3P P12（即便更远 exact 到达也不能越过整个缺口上界）、
held 无伪边沿/晚 release/start、绝对目标与摇杆、最早纠正重启和最高前沿、
early capture 的恢复后复用、生命周期缩短、v6 direct/hybrid Replay 只读 D0。
生成 replay-direct.rpy 与 replay-hybrid.rpy 供后续真实 JS 元数据检查。

这些是协议与 title 前沿组件门，不含完整 world Restore、真实 WS/RTC、浏览器 rAF、
4× CPU、零售资源或整场交付证明。--native-only 明确不执行当时尚在接线的 JS 能力断言。

### 9.5 实际执行证据与后续 JS 门

root 执行 nig command **HLMBLWT8oi7utcONj3dTVR3s**，exit 0。
已独立读取并核对 artifacts/rollback-smoothness/network-runtime-01/receipt.json 与 suite.stdout.log：

- startedAt 2026-10-13T08:32:24.816Z，completedAt 08:32:51.078Z。
- passed=true、sourceStable=true；common 为 984a8a10834c35fe9ef86c4ffe03daf1fb61f87c 且 clean。
- 新预测组件 **8 groups / 1202 checks**；旧 exact 2P/3P、D0/2/9、Replay/代次全部通过。
- pinned common asymmetric final ACK/旧代 phase/尾包、session channel、11组 startup 测量结果、观战尾包全部通过。
- network-protocol.wasm SHA-256：98b6be632c4fb1f0449199914d7de04d70f0c82d1ea73339934023d1b5e1259a。
- sourceDigest：75844a5ca486aa8065c217997ffba5def7e6d1722dd76d330ea8ec763b11bb31。
- NetplayRuntime.cpp SHA-256：3e8ac42190c2c39cb51829105fc797e2c449a97cdb88083f13365127cfca5e9c。

该收据 browserOptionChecks=false、browserSession=false、worldRestore=false、performance=false，
保留原始边界。Runtime native 文件随后冻结给正式构建，未因更新 JS 测试改变。

runner 后续新增真实 JS options 与 native Replay decoder 的交叉验证：
mode0 D0/P12、mode1 P0、mode2 P12；manual D0 和 auto D0 的区别；观战身份；
错误 mode/P/D/auto 拒绝；旧 v5 文件保留；新 v6 direct/hybrid 元数据；
把 JS 真实发出的 22 words 写入相同 native 录像、重算 checksum 后由 native decoder 验证。
这段仍只是 JS 模块行为，不是浏览器联机与性能。root 随后以 fresh output 执行：

```text
node portable/multiplayer/network-test.mjs --output artifacts/rollback-smoothness/network-runtime-02
```

### 9.6 network-runtime-02 实际完整结果

root 的 nig command **qiHfWIodLg3wOamIR9byDefB** exit 0。本代理独立读取并核对
`artifacts/rollback-smoothness/network-runtime-02/receipt.json` 与 `suite.stdout.log`：

- startedAt 2026-10-13T08:41:59.745Z，completedAt 08:42:28.217Z。
- passed=true、sourceStable=true；browserOptionChecks=true。
- 真实 Runtime/Core/InputLanes/GameInput **8 groups / 1202 checks**；原 exact 协议与 pinned common 回归全通过。
- JS configure/identity/URL admission、v5/v6 Replay 元数据、recorded P3、namespace isolation 全通过；
  JS 22-word setup 经真实 native Replay decoder 验证通过。
- sourceDigest：bc6fbe7b641d3451d4cf7bafb1d595f72aaabfd0a74ed89feba943734414e2fb。
- network-protocol.wasm：98b6be632c4fb1f0449199914d7de04d70f0c82d1ea73339934023d1b5e1259a，
  与 network-runtime-01 相同；本轮更新 JS runner 没有改变冻结 native 二进制。
- network-test.mjs：a159830845d1d06c289406e22e1c7a48c8bb2826858289aa24a52772f568f0da。
- multiplayer-replay.mjs：9391f95badb73d1f17fe2ec37e72bd64107c67ef6484125cacadb28a3a4f5376。
- sdl-runtime/multiplayer.mjs：025fb0fdc83402e2a3bdd8eccd03c73f21a5cd19d8611b17f4e7ccca2b5c2bc0。
- replay-direct.rpy：1916d9be616efa7ea015c188e1a06db275d53a744cd631784109292818aab2c2；
  replay-hybrid.rpy：18f982d9c47297c9207be7fd981a8242598e71a2af86a5a5f87dbbff146d33b6。

收据仍明确 browserSession=false、worldRestore=false、performance=false、retailAssets=false。
这是完整协议组件结果，不能替代正式 Release 真实 3P、Challenge、Lunatic 整场、4× CPU、
音频压力及网络路由验证。network-runtime-01 原始记录保持不动。

## 10. Application 四项接线修复的只读复审

2026-10-13，root 修改生产接线，本代理只读复核，没有在正式构建冻结期修改 C++/hpp/inc。
锚点基于此次读取版本，后续改动应以函数名复核。

| 原具体问题 | 已落盘修复与复审结论 |
| --- | --- |
| 手动导出或保存 Replay 从正在预测/恢复的 live world 读取 score/completed | `MultiplayerRollback.inc:3–8,18,125,174` 封帧记录 post-tick 元数据，实际确认并成功 Append 后更新 committed 缓存；`Application.cpp:87–93` 统一读取缓存，save 与 replay_size 使用该路径。exact 帧成功后同步缓存；restart 强制 incomplete。未发现本修复额外遗漏。 |
| 高刷 ticks=0 回调可先 Restore，再一直恢复而未采到旧完整前沿输入 | `Application.cpp:692–753,945–949` 增加 capture-only；首次 Restore 前调用 `sample_and_tick(false)` 再 `multiplayer_tick(false)`，NeedsCapture 拒绝纠正期间重采；已捕获输入保留在原 F。未发现二次消费脉冲。 |
| 预测到 Replay 容量边缘后过早 fatal/Finish，妨碍末帧纠正确认 | `MultiplayerRollback.inc:231–238` 容量门位于 correction 和 commit 之后，仍收包、恢复、发布末帧，完整确认后自然 Finish；`Application.cpp:710` 不采超容量输入。未删除旧边界检查或忽略末帧确认。 |
| 新 generation 继承旧 cadence，或只在 advanced 时检查而漏掉 Pump 内换代 | `Application.cpp:47,167–170,611,953` 前置声明并在 multiplayer_begin 实际 reset；sample 返回后先比较 generation，再消费 cadence/判断 advanced。包括 terminal Pump 导致 advanced=false 的换代。 |

上述是源代码路径复核，**不是 Application 行为测试或正式浏览器通过证据**。没有发现阻止 root
开始正式 build03 的具体新增遗漏。root 后续验证须继续记录确切构建身份；不得以旧 build02
结果代替这些修复后的构建。

音频确认链与高密度 3P 风险另见 `TH11MP-AUDIO-ROLLBACK-AUDIT.zh-CN.md`。
