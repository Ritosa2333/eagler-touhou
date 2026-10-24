# 回滚运行参考

回滚通过输入历史和状态日志恢复游戏世界，再按帧确认顺序提交音频、存档、Replay 与观战输出。维护多人帧循环时，按下方分工接入公共核心与各作状态。

配套阅读：[联机输入时序与 Adonis](netplay.md)；[适配总览](../adaptation/README.md)。旧回滚手册保留为基线 `991e7cdf9060252e022eba25431254c6a0ce73e9` 的历史材料：[回滚 Playbook](https://github.com/YomotsuHisami/eagler-touhou/blob/991e7cdf9060252e022eba25431254c6a0ce73e9/launcher/docs/playbooks/rollback.md)。实验记录列出了当时的构建、输入和设备条件。

## 所有权边界

| 组件 | 当前所有权 | 不变量 |
| --- | --- | --- |
| [`RollbackCore`](../../common/include/eagler/netplay/NetplayCore.hpp) | 按帧保存各席位输入、实际输入前沿、已模拟决策和最早待纠正帧 | `RewindSimulationTo()` 只回退核心的模拟决策记录；它不恢复标题内存。 |
| [`RollbackJournal`](../../common/include/eagler/netplay/RollbackJournal.hpp) | 有界、first-write-wins 的字节撤销存储 | 每个写入点先 `Touch()`；同一块重复触碰安全；部分重叠被拒绝。 |
| `SessionChannel` | 网络时钟、序号、输入重传、确认、维修包和会话退休 | 网络状态留在回滚日志外；重传发送已采样输入，不重新读设备。 |
| 标题帧驱动与各领域日志 | 游戏对象、管理器、池、资源引用及标题级恢复顺序 | 标题层负责完整恢复、重模拟、过期历史退休和生命周期边界。 |
| 输出队列 | 音效、文件写入、Replay、观战帧等不可逆副作用 | 只在对应帧确认后发布；纠正时先撤销尚未提交的目标帧及之后输出。 |

公共核心管理输入与确认账本，各作状态快照负责恢复游戏世界。标题在更改某个值之前记录其原值；恢复时回到目标检查点开始前的状态，再用同一组历史逻辑输入重新执行。

常见标题侧所有者：

- TH08：[`RollbackDriver`](../../ports/th08-mp/web/cpp/multiplayer/RollbackDriver.cpp) 协调帧和输出；[`WorldJournal`](../../ports/th08-mp/web/cpp/multiplayer/WorldJournal.cpp) 组合主状态、敌人池、对象池与资源日志；`PoolsJournal`、`EnemyJournal`、`TextureJournal` 负责各自区域。
- TH09：[`RollbackSession`](../../ports/th09/web/cpp/multiplayer/RollbackSession.cpp) 管理输入帧；[`Application.cpp`](../../ports/th09/web/cpp/sdl/Application.cpp) 的 `checkpoint`、`rollback_tick` 及 SDL 帧循环接管标题世界、恢复和重模拟。
- TH10：[`RollbackState`](../../ports/th10-mp/web/cpp/multiplayer/RollbackState.cpp) 通过稀疏池捕获器和 `RollbackJournal` 接线各领域状态；`NetplayRuntime` 只提供网络和输入决策。
- TH11：`NetplayRuntime` 持有帧前沿和提交水位；[`WorldRollback`](../../ports/th11-mp/web/cpp/game/WorldRollback.cpp) 与 `StageRollback`、`EnemyRollback`、`AnmRollback` 管理标题状态。
- TH15：[`NetplayRuntime`](../../ports/th15-mp/web/cpp/multiplayer/NetplayRuntime.cpp) 设 `allowPrediction=false`，当前多人运行按精确输入推进，不使用标题回滚日志。

## 一帧的输入与恢复路径

1. 物理设备采样由标题当前捕获前沿拥有。一次采样生成不可变 `FrameInput`；重传和历史重模拟复用它。
2. [`RollbackCore::ScheduleLocalInput`](../../common/src/netplay/NetplayCore.cpp) 将采样帧 `N` 安排到 `N + inputDelay`。输入延迟的起始空帧是确定性中性输入。
3. `PrepareFrame(F)` 为每个席位取实际输入，或按标题配置预测最近确认输入。方向预测时限可独立短于总回滚历史；炸弹、菜单等边沿动作通常要求实际输入。
4. 标题用该决策执行逻辑更新，然后调用 `MarkSimulated()`。核心保存该帧用了哪些输入及预测位图。
5. 新远端输入到达时，核心比较它与已用预测输入。相同则确认该帧；不同则保留最早 `RollbackFrame`。
6. 标题先关闭当前检查点，再恢复到目标帧开始之前。`RollbackJournal::UndoTo()` 返回检查点的起始帧；跨帧检查点会从其起始帧重新执行。
7. 标题调用 `RewindSimulationTo()`，让核心丢弃被放弃的未来决策。已捕获的本地输入、已收到的真实远端输入、确认前沿和 ACK 前沿继续保留。
8. 标题从恢复后的逻辑帧逐帧重模拟，读取核心保留的 `UsedInput` 或确定后的实际输入，不读取 DOM、触控或手柄状态。
9. 对应帧在所有远端席位均确认且没有待纠正事务后，标题才退休日志并提交音频、Replay、观战或文件输出。

`RollbackCore` 的 `ConfirmedThroughAllRemotes()` 是所有远端连续确认前沿的最小值；`AcknowledgedLocalThroughAllRemotes()` 是其他席位已确认收到本地输入的前沿。会话退休和新一轮切换分别使用这些水位。分别读取这两个水位，完成各自对应的退休屏障。

## 检查点和日志规则

### 写入捕获

- 用 [`BeginFrame()`、`Touch()`、`EndFrame()`](../../common/include/eagler/netplay/RollbackJournal.hpp) 包住一个逻辑检查点。`Touch(pointer, size)` 必须早于该内存的第一次修改。
- 日志对完全相同的地址和长度只保存第一份原始字节。相邻区域可分别记录；部分重叠被拒绝，避免恢复顺序依赖对象布局。
- 每帧受 `maxBytesPerFrame`、`maxBlocksPerFrame` 限制；环形帧槽及字节 arena 在确认/恢复后可复用。捕获完整时执行纠正；捕获失败时终止该事务。
- `extendPrevious=true` 将相邻逻辑帧并入同一检查点。目标命中该跨度时，恢复点是跨度首帧，历史重放必须覆盖到原有显示前沿。
- 对池对象只捕获确实会参与后续状态变化的槽位时，仍要记录分配器游标、占用位、计数和相关管理器字段；空槽是否可省略取决于其生命周期语义。
- `BeginReadOnlyCapture()` 是有附加约束的批量复制优化：捕获期间源字节保持不变，窗口在任何游戏或输入修改之前关闭。它不改变字节所有权和恢复结果。

### 恢复与确认

- [`UndoTo()`](../../common/src/netplay/RollbackJournal.cpp) 从较新的检查点向较旧检查点恢复，覆盖目标检查点及之后历史；随后这些检查点可供即时重模拟复用其内存容量。
- `coalesceRestore` 只跳过被更旧记录完整覆盖的新块；部分覆盖仍按原顺序完整复制。`fastBulkCopy` 只替换复制路径，不替换恢复范围。
- `DiscardBefore(F)` 只删除结束早于 `F` 的检查点。实际确认值由标题驱动结合输出队列、资源租约、阶段切换和网络 ACK 决定。
- 资源或对象生命周期若会跨越可回滚历史，保留相应句柄/租约或延后析构；不能只恢复裸指针后访问已释放资源。
- TH08 `WorldJournal::UndoTo()` 除了各子日志，还恢复 Replay 记录快照，并调用 `RebasePresentation()` 清理插值缓存、前一位置标记和绘制侧临时状态。

## 物理输入前沿与模拟前沿

多人驱动通常同时维护下列帧位置。把它们拆开可以解释“本地手感慢但模拟能追上”或“网络确认到达但画面还在重放”等现象。

| 前沿 | 含义 | 负责人示例 |
| --- | --- | --- |
| 本地采样前沿 | 下一次可采集的新物理输入槽 | TH08/10/11 的 `next_capture_`；TH09 的 `NeedsCapture()` 与 `next_`。 |
| 模拟前沿 | 下一次正常执行的逻辑帧 | `RollbackCore::LastSimulatedFrame()` 加一，或标题驱动自己的 `next_`。 |
| 远端实际前沿 | 所有远端都已有连续真实输入的第一帧 | `ConfirmedThroughAllRemotes()+1`。 |
| 预测前沿 | 当前模拟或纠正事务已经触及的第一帧 | 纠正过程中保存旧显示前沿，恢复时不随模拟游标倒退而清零。 |
| 输出提交前沿 | 音频、Replay 或观战流下一个可发布帧 | 由标题输出队列分别维护；通常不会与模拟前沿相同。 |

精确输入 UI 阶段可以由 [`ConfirmedInputClock`](../../common/include/eagler/netplay/ConfirmedInputClock.hpp) 允许采样前沿在模拟前沿之前最多领先 8 个槽，以保持按固定节奏发送确认输入。该上限限制的是排队槽数；标题恢复时仍使用这些已采样槽，不重新读取触控事件。切回战斗前要排空/协调队列，保证不会永久增加额外输入延迟。

## 表现状态与逻辑状态

确定性规则只捕获影响后续游戏逻辑、资源所有权或确认结果的状态。可从逻辑世界重建的插值缓存、前一绘制位置、相机平滑标记等可以在恢复后重置或重建。TH08 `WorldJournal::RebasePresentation()` 是当前可读范例。

标题需要逐个审核普通 `Draw` 是否会写入逻辑字段。若绘制插值临时改写逻辑位置，绘制结束应恢复原值；持久的外观动画变化归入固定逻辑更新。对象日志用规范状态或完整对象字节校验，绘制缓存按恢复后的逻辑世界重建。

纠正期间可以压制中间画面提交，让用户只看到完成的恢复端点；浏览器音频泵和网络泵继续由外层循环执行。当前 TH08 的 `RollbackDriver` 在恢复路径重建表现标记，th09 将恢复工作安排在 Present 后执行，减少一次绘制回调内的长时间重算。th09 到达旧显示前沿后，复用最后一帧重模拟中的 Draw 提交完整画面，等待输入期间也能显示已经完成的纠正。

## 音频、文件、Replay 与观战输出

回滚可重复逻辑更新，但浏览器音频、文件系统写入和网络发布本身不可撤销。将副作用变成按帧暂存的事件，随逻辑历史一起作废或确认：

- TH08 [`AudioEvents`](../../ports/th08-mp/web/cpp/multiplayer/AudioEvents.cpp) 在每帧追加音频事件；`CommitThrough(confirmed, simulated)` 只排空两者较小值之前的完整事件。`DiscardFrom(frame)` 删除尚未提交的目标帧及之后事件。
- 文件、Replay 和音频使用各自的队列与提交水位。
- Replay 与观战只消费已纠正、连续确认的输入和状态。重模拟期间暂缓发布，纠正完成后从单调发布游标续传。
- 发生生命周期切换时先完成本阶段历史对账，再关闭阶段资源及日志；网络会话另走输入确认和 ACK 退休屏障。
- 纯延迟模式每帧只在全部真实输入到达后推进，因此不需要预测撤销存储；确认后的音频、存档、Replay 和对象池仍按各自所有权提交。

TH09 输出环按“最旧未发布帧”保留空间，标题驱动同时管理纠正前沿和输出提交游标，供音频、Replay 与观战按序发布。细节见固定基线报告的 [TH09 确认输出保留记录](https://github.com/YomotsuHisami/eagler-touhou/blob/991e7cdf9060252e022eba25431254c6a0ce73e9/launcher/docs/playbooks/rollback.md#th09-confirmed-output-retention-issue-89-2026-10-16)。

## 标题差异

下表是源码配置和实现边界对照；当前验证状态由[适配总览](../adaptation/README.md)维护。

| 标题 | 输入/回滚实现 | 关键差异 |
| --- | --- | --- |
| TH08MP | [`NetplayRuntime`](../../ports/th08-mp/web/cpp/multiplayer/NetplayRuntime.cpp) 的 core 历史为 8 帧；`WorldJournal` 按世界、敌人、池和资源组合捕获；音频/文件单独暂存。 | 物理捕获前沿与纠正模拟前沿分离；帧驱动只在可恢复的世界图内开放预测，重模拟时复用输入。 |
| TH09MP | [`RollbackSession`](../../ports/th09/web/cpp/multiplayer/RollbackSession.hpp) `History=8`，仅 2 人；DirectTouch 表示绝对场地目标。 | `Application.cpp` 在 Present 后安排异步恢复，并由 `RecoverySchedule` ticket/loop epoch 取消过期回调。短缺输入不再通过放慢本地采样节奏解决。 |
| TH10MP | [`core_config`](../../ports/th10-mp/web/cpp/multiplayer/NetplayRuntime.cpp) 设置 `maxRollbackFrames=12`；[`RollbackState`](../../ports/th10-mp/web/cpp/multiplayer/RollbackState.cpp) 包含稀疏对象池捕获和多个 typed slot owner。 | 产品目录 `rollbackLimit=12`；其未设置 `sendPredictionLimit`，公共策略函数回退到 8。C++ 核心历史窗为 12，发送预测上限为 8；修改时分别核对这两个字段。 |
| TH11MP | [`SessionSetup::MaxRollbackFrames=12`](../../ports/th11-mp/web/cpp/multiplayer/SessionSetup.hpp)；世界、阶段、敌人和 ANM 各自有日志。 | 固定的提交水位与阶段租约参与资源退休；`WorldRollback::CanCapture()` 会拒绝暂停状态及暂停菜单拥有期间的捕获。 |
| TH15MP | [`NetplayRuntime`](../../ports/th15-mp/web/cpp/multiplayer/NetplayRuntime.cpp) 配置 `allowPrediction=false`；公共 core 保留 12 帧历史槽用于输入/ACK 管理。 | 产品目录 `rollbackLimit=0`，使用输入延迟模式；core 历史槽用于输入与 ACK。 |

## TH08 与 TH09 的可复用经验

### TH08：先分离普通更新成本，再优化回滚

TH08 的受控分析中，fixture 的 `Update` 大部分成本来自 Bullet job。各作独立定位瓶颈，使用同一测量方法：同世界、同输入、无网络驱动、无快照和重放，交替执行旧/新路径，并逐段比较规范状态；另测 Update 和 Update+Draw。

TH08 MP 的活动索引在 Item 更新后、Bullet/Laser 更新前重建，替换每名玩家每次扫描 192 个取消区域槽的路径。正向执行与重模拟逐帧刷新索引，其生命周期覆盖这些游戏任务，存档保存原始游戏状态。目标筛选只在瞄准 extra 实际需要时运行；远距离碰撞拒绝保留精确几何回退。

历史受控基准采用 4 倍 CPU 限速：2P Update 从 6.40 ms 降至 2.40 ms，3P 从 8.96 ms 降至 2.92 ms。该基准用同世界、同输入比较更新路径；设备与发布验收按适配表覆盖手机、长流程和音频。完整测量记录见 [TH08 Update 复用报告](https://github.com/YomotsuHisami/eagler-touhou/blob/991e7cdf9060252e022eba25431254c6a0ce73e9/launcher/docs/playbooks/rollback/th08-mp-base-update-reuse-2026-09-27.md)。

### TH09：保持已采样输入，恢复安排不制造额外延迟

- `FrameSchedule::blocked(true)` 记录一个已经到期的待推进帧；下一次显示机会重试这一帧，不再累积网络等待时间，也不凭空增加新的预测帧。
- `Application.cpp` 将恢复安排到绘制提交之后；`RecoverySchedule` 为回调分配 ticket 并绑定 loop epoch。暂停、停止或新一轮循环取消旧 ticket，避免过期异步回调操作新会话。
- th09 收包入口可在两次 rAF 之间安排已有历史的纠正并刷新完整端点。重复唤醒合并为一个恢复任务，物理输入和新逻辑帧继续由到期的时钟驱动。
- th09 宣言标志在人物、弹幕更新之后清除。恢复画面验证覆盖随后 8 帧的实际移动，并将全局宣言、场地时停和完整 canvas 对照分别统计，见本作 [宣言回归参数](../../ports/th09/docs/DEVELOPMENT.md)。
- 真实采样沿 `RollbackSession::Capture()` 发生一次；历史 `Prepare/Complete` 和 `Restored()` 只消费缓存输入。`PacedElapsedMs()` 将 Adonis 建议应用到外层经过时间，而非改变固定逻辑 tick。
- 确认输出环的退休点独立于“待纠正帧”。按最旧未提交输出保留空间，恢复工作跨显示回调时，已经确认的音频/Replay/观战帧仍可顺序排空。

输入恢复记录：[TH09 输入调度与 Present 后恢复](https://github.com/YomotsuHisami/eagler-touhou/blob/991e7cdf9060252e022eba25431254c6a0ce73e9/launcher/docs/playbooks/rollback.md#th09-input-scheduling-and-post-present-recovery-2026-10-16)。

## 修改回滚覆盖时的检查顺序

1. 从实际写入点反查状态 owner：游戏逻辑、池分配器、ANM/敌人 VM、资源租约、UI 生命周期、Replay 和输出队列分别列清。
2. 在每个首次修改之前设置日志 hook，并核对分配、销毁、重用槽位的游标与占用状态。
3. 对一个固定输入序列建立普通执行和“执行→恢复→重执行”两条路径；逐帧比较规范状态、每席位输入和确认水位。
4. 延迟远端帧，使纠正分别落在对象生成前、同帧创建/销毁、关卡切换前后、菜单状态改变前后。
5. 单独核对绘制和音频：恢复后的插值状态能重建；音频/文件/Replay/观战事件未提前产生，提交后帧序唯一。
6. 对稀疏池捕获或 coalesced restore 保留密集字节路径作 oracle；比较完整对象字节和最终规范世界。
7. 只有帧日志、输出提交和资源退休三个边界一致后，再测捕获 CPU、恢复字节、重模拟帧数和 Present 间隔。

仓库内可直接定位的验证入口包括公共 [`rollback-journal-test.cpp`](../../common/tests/rollback-journal-test.cpp)、[`netplay-input-test.cpp`](../../common/tests/netplay-input-test.cpp)、[`confirmed-input-clock-test.cpp`](../../common/tests/confirmed-input-clock-test.cpp)，TH08 [`network-frame-schedule-test.cpp`](../../ports/th08-mp/tests/network-frame-schedule-test.cpp)，以及 TH09 [`frame-schedule.cpp`](../../ports/th09/web/tests/multiplayer/frame-schedule.cpp)、[`after-present.cpp`](../../ports/th09/web/tests/multiplayer/after-present.cpp)、[`output-history-browser.mjs`](../../ports/th09/web/tests/multiplayer/output-history-browser.mjs)。运行后将提交、输入、环境和结果链接填入本作 `ADAPTATION.md`。

## 症状、原因与检查点

| 现象 | 常见原因 | 检查 |
| --- | --- | --- |
| 相同输入仍出现状态差异 | 漏记一个可写字段、恢复顺序不完整，或某个更新阶段读取了未恢复的状态 | 从目标帧起比较规范游戏状态；检查每个写入点是否先 `Touch()`，并对照未优化的密集复制结果。 |
| 纠正后本地下一次输入变晚 | 重模拟期间又读设备、旧本地输入未发送，或普通捕获等待模拟帧而不是按物理节奏运行 | 对照采样帧、调度帧、发送时间及 `next_capture_`；确认恢复期间没有第二次设备读取。 |
| 画面回滚后抖动或跳动 | 插值历史和逻辑世界不同步 | 恢复后重置前一绘制位置/相机插值缓存；逐个审核被剔除的字段是否可由逻辑状态重建。 |
| 音效重复或错序 | 在预测更新时立即触发不可逆音频，或确认/丢弃游标未与帧对齐 | 检查事件是否按帧缓存、纠正时是否清除、提交游标是否仅单调前进。 |
| 网络输入确认了，Replay/观战停住 | 确认前沿与输出提交前沿混为一谈；最旧未发布输出被环覆盖 | 检查每个帧槽 owner、`confirmed`、模拟帧和发布游标；输出容量要覆盖异步恢复期间的保留尾部。 |
| 修复越界或偶发损坏 | 历史过早退休、对象先析构后恢复，或部分重叠 Touch 被默默接受 | 检查日志寿命与资源租约；保留 `RollbackJournal` 的范围验证并在生命周期切换处使用提交屏障。 |

## 当前源码入口

- [公共核心与网络通道](../../common/include/eagler/netplay/NetplayCore.hpp)、[SessionChannel](../../common/include/eagler/netplay/SessionChannel.hpp)、[RollbackJournal](../../common/include/eagler/netplay/RollbackJournal.hpp)
- [TH08MP 驱动与世界日志](../../ports/th08-mp/web/cpp/multiplayer/RollbackDriver.cpp)、[WorldJournal](../../ports/th08-mp/web/cpp/multiplayer/WorldJournal.cpp)、[AudioEvents](../../ports/th08-mp/web/cpp/multiplayer/AudioEvents.cpp)
- [TH09MP RollbackSession](../../ports/th09/web/cpp/multiplayer/RollbackSession.cpp)、[恢复调度器](../../ports/th09/web/cpp/multiplayer/RecoverySchedule.hpp)、[SDL 帧循环](../../ports/th09/web/cpp/sdl/Application.cpp)
- [TH10MP RollbackState](../../ports/th10-mp/web/cpp/multiplayer/RollbackState.cpp)、[TH11MP NetplayRuntime](../../ports/th11-mp/web/cpp/multiplayer/NetplayRuntime.cpp)、[TH15MP NetplayRuntime](../../ports/th15-mp/web/cpp/multiplayer/NetplayRuntime.cpp)
