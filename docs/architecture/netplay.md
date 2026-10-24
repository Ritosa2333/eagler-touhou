# 联机输入时序与 Adonis

Adonis 在实际游戏输入通道上测量 RTT，协商输入延迟 `D` 与预测预算 `P`，并在运行期保持 60 Hz 逻辑帧。产品入口与适配总览见[适配文档](../adaptation/README.md)，回滚所有权见[回滚运行参考](rollback.md)。

旧 Adonis 操作说明保留为基线 `991e7cdf9060252e022eba25431254c6a0ce73e9` 的历史材料：[Adonis adaptation playbook](https://github.com/YomotsuHisami/eagler-touhou/blob/991e7cdf9060252e022eba25431254c6a0ce73e9/launcher/docs/playbooks/adonis-adaptation.md)。本页按当前源码记录参数和负责模块。

## 谁负责什么

| 层 | 当前实现 | 责任 |
| --- | --- | --- |
| 产品时序声明 | [`common/contracts/product-catalog.mjs`](../../common/contracts/product-catalog.mjs) | 声明 `measuredStartup`、回滚历史窗、可选择的发送预测上限及手动延迟上限。 |
| Launcher 选项 | [`multiplayer-runtime-options.mts`](../../launcher/src/launcher/multiplayer-runtime-options.mts)、`multiplayer-lobby-snapshot.mts` | 校验房间选项并把 mode、手动/自动 D、预测 reserve、产品预测上限传给 Runtime。 |
| 实际测量与协商 | [`AdonisStartup`](../../common/include/eagler/netplay/AdonisStartup.hpp)、[`AdonisConnection`](../../common/include/eagler/netplay/AdonisConnection.hpp) | 通过既有 `PeerTransport` 在逻辑帧 0 前测量、合并各链路结果并提交共同选择。 |
| 运行期网络 | [`SessionChannel`](../../common/include/eagler/netplay/SessionChannel.hpp)、[`AdonisPhase`](../../common/include/eagler/netplay/AdonisTiming.hpp) | 发送输入/维修包；可选地计算帧节奏校正建议。 |
| 标题帧推进 | 标题自己的 `NetplayRuntime`、Session/Driver 和 World journal | 完成资源加载、帧零准入、输入采样、预测或等待、恢复及输出提交。 |

Launcher 房间回报和 UI 状态展示测量结果；Native Runtime 在本地同一输入通道上验证、协商和固定运行参数。Replay/观战流收到的已确认逻辑帧按其协议时序播放，不重复应用玩家输入延迟。

大厅网络检测中的 TURN 延迟是单个客户端到 TURN 服务的往返估计，通过同浏览器双端中转回环的实际 DataChannel 回显折算。游戏右下角延迟读取当前玩家间 ICE 候选对的完整往返时间；双方经同一 TURN 节点中转时，完整往返约为双方到节点的往返时间之和。

## 房间手动丢包测试

在房间中点击自己的玩家卡片，打开个人状态面板后选择「测试丢包率」。一次操作分别测量与房内其他玩家的直连和 TURN 路径，展示「你 → 对方」和「对方 → 你」的本次消息丢失比例，适用于全部多人作品。

[`room-network.mts`](../../launcher/src/launcher/room-network.mts) 复用房间的独立 ICE 探测连接。直连连接只配置 STUN，TURN 连接强制 relay，测试结果按路径分开保留。[`room-packet-loss.mts`](../../launcher/src/launcher/room-packet-loss.mts) 使用可靠通道协调测试、交换收发摘要，使用 `ordered:false, maxRetransmits:0` 的专用通道发送 300 个 96 字节消息，间隔 16 ms。末尾保留 1 秒接收窗，再以唯一序号计数计算两个方向的比例；重复包不增加接收数，接收窗内的乱序正常计入。

测试由按钮启动，两端均完成后发布结果。断开、离房、开局、页面挂起或本地发送拥塞会停止本次测试；路径未连接时显示「连接未就绪」。每次结果对应这次房间探测及其接收窗，游戏开局继续独立选择实际链路。验证入口为 `launcher/tests/test-room-packet-loss.mjs`、`launcher/tests/test-room-network.mjs` 和 `launcher/tests/browser/test-room-packet-loss.mjs`。

专用探测通道兼容旧版房间的延迟回显，房间内不同前端版本可继续交换延迟样本。双向丢包测试由支持该协议的两端完成。

### 优先使用 TURN

同一个人面板提供「优先使用 TURN」开关。选择按本机保存，所有多人作品共用，默认关闭；下一次开局建立连接时生效。每名玩家独立选择，任一方启用即可让与其相连的游戏链路经过 TURN。

Launcher 将本机偏好通过 `netplayPreferTurn` 传给 Runtime。公共 `BrowserPeerTransport` 在启用时使用 `iceTransportPolicy: 'relay'`，让游戏输入与可靠控制通道共同走 TURN；连接恢复继续沿用这项策略。TURN 无法连接时使用既有 WebSocket 服务器中转兜底。关闭后由浏览器在直连与 TURN 候选中选择路径。

丢包测试持续分别测量直连和 TURN，方便按实际结果选择游戏链路。该设置属于本机传输偏好，房间规则与 Replay 玩法身份沿用各自配置。

## 房间目录与成员

公开目录展示至少有一个席位成员或在线旁观者的房间。旧对局保留的 Runtime、信令或旁观传输连接按自身生命周期退休；列表展示与这些连接的保留分开管理。席位与旁观均为空时，房间从公开列表移除；成员重新入席后恢复展示。短暂断线保留的席位和有旁观者的回放房间继续按原有成员状态展示。

服务端 [`room-directory.mjs`](../../services/relay/src/room-directory.mjs) 按成员生成公开快照，启动器 [`lobby-directory.ts`](../../launcher/app/services/lobby-directory.ts) 同时过滤旧服务下发的空房间，并同步修正列表总数。自己的房间恢复入口继续由独立的 membership 信息提供。

标题连接到 Adonis 的入口：

- TH08/10/11/15 通过各自 `NetplayRuntime` 持有 `AdonisConnection`；详见 [TH08](../../ports/th08-mp/web/cpp/multiplayer/NetworkConnection.cpp)、[TH10](../../ports/th10-mp/web/cpp/multiplayer/NetplayRuntime.cpp)、[TH11](../../ports/th11-mp/web/cpp/multiplayer/NetplayRuntime.cpp)、[TH15](../../ports/th15-mp/web/cpp/multiplayer/NetplayRuntime.cpp)。
- TH09 的 [`RollbackSession`](../../ports/th09/web/cpp/multiplayer/RollbackSession.cpp) 直接拥有 `AdonisStartup`，待提交后配置标题输入 core 与 `SessionChannel`。

## 启动时序

Adonis 用于声明 `inputTiming.measuredStartup` 的产品。准备顺序如下：

1. 标题建立会话 ID、玩家席位、seed、游戏 ABI 和传输；加载游戏资源并构造即将运行的标题世界。
2. Runtime 将 `worldReady=true` 交给 `AdonisConnection::Pump()`。世界就绪后开始 Startup；`SessionGate` 与首帧真实输入共同控制帧 0 的开始。
3. 所有玩家在同一会话/ABI/seed/mode/request/reserve 下交换 HELLO。3 人会话按每条必需链路测量，包括 P2–P3。
4. 同一实际游戏输入通道打开且 HELLO 齐全后，等待稳定 1 秒。随后以本地单调高精度时钟测量 RTT：探针和回显走 `SendTo` 输入 lane；汇总和 proposal/accept/commit 走可靠 control lane。
5. 每 16 ms 最多发送一个新探针。序号为 1..129，序号 0 不使用；丢弃暖机槽 0..9，统计槽 10..129，共 120 个候选样本。最后一个探针之后仍等待 16 ms 加 200 ms 尾窗。
6. 每条必需链路至少需要 96 个有效样本。每个席位公布其本地最差链路的 p95、最小、最大、均值、接收和丢失数量；任一链路不够样本则全体重新测量。
7. 三人组所有两两方向链路都参与有效样本检查。Host 汇总各席位报告，按最差 p95 确定相同 `D/P`；参与者依据同一报告复算，并经 proposal/accept/commit 屏障后，Runtime 才把共同参数写入标题并开放逻辑帧 0。

RTT 表示探针往返时间，两端时间戳来自同一本地时钟。假设路径对称时，`RTT/2` 给出单程传输时间估计。

测量尝试的当前边界：HELLO 等待 45 秒；一次测量限时 10 秒；整轮启动限时 90 秒；初次尝试后最多重试 3 次。已发送探针后若 pump 间隙超过 500 ms、页面隐藏或通道恢复/中断，当前尝试重新开始；首个探针发出前的长间隙只重开 1 秒稳定计时。时钟倒退会让启动失败；有效样本不足会重新测量。ADS/4 报文带 attempt、session、席位数、seed、gameplay ABI 和时序请求，旧轮回显不会加入新统计。`AdonisConnection` 交接时暂存最多 16 个当前会话启动控制包，留给后续原生 gate 处理。

回显样本只在本端此前发出同序号探针且该槽尚未采样时记入；有效 RTT 在 0 到 1 秒之间。每链路样本排序后取 `(n-1)*95/100` 位置；偶数样本数且存在下一位置时，对两者取平均。样本摘要发布后固定，迟到回显和重复控制包不改写该轮统计。

资源已经就绪才测量，是为了把真实世界打开后的同一输入通道作为样本对象；加载等待时间不进入 RTT 样本。帧 0 前完成时序协商，让双方模拟从同一逻辑输入合同开始。

## `D` 与 `P` 的计算

当前 [`AdonisStartup::Choose()`](../../common/include/eagler/netplay/AdonisStartup.hpp) 由最差链路的 p95 计算完整 RTT 预算 `B`：

```text
B = max(1, ceil(floor(worst_p95_RTT_us / 2) * 60 / 1_000_000))
```

60 Hz 下每帧 16.67 ms。`D` 是本地采样到逻辑帧之间排入的输入帧数；`P` 是允许的预测领先帧数，二者是不同预算。自动选项的结果：

| 模式 | `P` | `D` | 解释 |
| --- | ---: | ---: | --- |
| 纯延迟 `mode=1` | 0 | `B` | 所有席位需要该帧真实输入后再推进。 |
| 混合 `mode=2` | `min(reserve, B-1)` | `B-P` | `reserve` 为 1 或 2；将最多两帧 RTT 预算用作预测领先，自动 D 至少保留一帧。 |

手动请求 `D` 时，Runtime 保留用户输入的 D（产品当前允许范围内，通常 0–9）。纯延迟的 `P=0`；混合模式的 `P=min(reserve, B)`，reserve 为 1 或 2。手动 D 不因预测 reserve 被改写。

自动纯延迟使用 `D=B`。自动 D 超出产品可用范围时，启动报告所需预算与产品范围。会话期间 D、P 保持固定，运行期校正作用于外层帧的墙钟节奏。

## 运行期节奏校正

[`AdonisPhase`](../../common/include/eagler/netplay/AdonisTiming.hpp) 在每端记录“某帧何时到达”和“该逻辑帧何时到期”，每 16 个逻辑帧汇总一次每个远端的等待和领先时间。统计使用同一台设备的单调时钟，网络重传和重模拟不重写第一次到达/到期时间。

当本端确实等待远端时，校正值以窗口平均等待时间减去已协商的 P 允许量；若无等待，再用双方的领先差做小幅校正。单次建议最多 8 ms，按标题允许方式从新到的墙钟时间中扣除。各逻辑帧按固定 `1/60` 秒步长依次执行。

接线分别位于 [`SessionChannel::Pump()`](../../common/src/netplay/SessionChannel.cpp) 的阶段样本收发和标题外层帧日程：TH08 `NetworkFrameSchedule`、TH09 `RollbackSession::PacedElapsedMs()`、TH10 `NetplayRuntime::PacedElapsedSeconds()`、TH11 `NetplayRuntime::PacedElapsed()`。标题选择何时消费建议值；`SessionChannel` 不模拟游戏帧。

TH08/09/10/11 的混合/纯延迟模式可交换 AdonisPhase 信息；TH15 配置 `adonisPhase=true` 且 prediction allowance 为 0。TH15 按每席位真实输入推进，`maxRollbackFrames=12` 用于保存公共输入与 ACK 历史。

## 输入捕获与帧零

- 设备采样在逻辑层有一个所有者。新采样被安排到 `captureFrame + D`；重传、修复、纠正和 Replay 重放复用已保存的帧值。
- 纯延迟模式使用 `CoreConfig::allowPrediction=false`。`PrepareFrame()` 等待该帧全部席位的实际输入后推进；历史窗负责保存输入和确认记录。
- Hybrid 的 P 是对网络输入前沿的允许距离，不会缩短标题的最大回滚历史。动作边沿仍按标题 `predictableButtons` 策略决定是否预测。
- 帧零的会话/世界启动输入要求真实一致。Adonis 不预测世界初始化数据；每个 Runtime 在测量提交前保持逻辑模拟关闭。
- 网络传输泵、ACK、重传和音频泵独立于逻辑帧推进。输入帧因确认等待而阻塞时，只重试已到期的那个逻辑帧。
- 玩家输入延迟只应用于玩家实时采样。Replay 和观战数据已经是确认后的逻辑帧；播放端以其帧号推进，不再额外排入 D。
- 音频、文件写入和观战发布按帧确认顺序提交，状态恢复由独立模块管理。纯延迟模式也采用这一提交顺序。

## 产品声明与标题差异

产品目录定义可选功能；各作 C++ 分别实现 history、预测算法和存档 owner。两层配置对应如下：

| 产品 | 产品目录输入时序 | 标题帧策略 | 可见实现差异 |
| --- | --- | --- | --- |
| TH08MP | `rollbackLimit=8`、`sendPredictionLimit=8`、实测启动 | `NetplayRuntime` 使用 8 帧回滚 core；`WorldJournal` 捕获游戏状态 | 2–3 人；Adonis 在世界准备后经 `NetworkConnection` 启动。 |
| TH09MP | `rollbackLimit=8`、实测启动、手动延迟到 9；2 人 | `RollbackSession::History=8`；Rollback、Delay、Hybrid 共用标题输入核心 | 自有 `Application.cpp` 恢复/Present 调度和菜单生命周期输入边界。 |
| TH10MP | `rollbackLimit=12`、实测启动；目录未显式设置 `sendPredictionLimit` | `NetplayRuntime.cpp` 设置 `CoreConfig::maxRollbackFrames=12` | 公共策略读取缺省发送预测上限 8；标题 C++ 状态窗为 12，两者字段含义和 owner 不同。 |
| TH11MP | `rollbackLimit=12`、`sendPredictionLimit=12`、实测启动、手动延迟到 9 | `SessionSetup::MaxRollbackFrames=12`；Mode 0/2 可预测，Mode 1 精确输入 | 定时启动支持 2–3 人；直接回滚 Mode 0 可不运行测量，Mode 1/2 使用校准。 |
| TH15MP | `rollbackLimit=0`、实测启动、手动延迟到 9 | 只允许 Adonis Delay；`CoreConfig::allowPrediction=false` | 校准之后严格等待每席位的真实帧输入；保留的 12 帧 core 槽位只用于输入和 ACK。 |

产品时序声明见 [`product-catalog.mjs`](../../common/contracts/product-catalog.mjs)；统一解释函数 `multiplayerInputTimingPolicy()` 的缺省预测上限为 8。TH10 当前目录未显式写 `sendPredictionLimit`，但标题 C++ `CoreConfig.maxRollbackFrames` 设为 12。审阅具体调度上限时要分别追踪 UI/Relay 产品值与 Native Runtime frame admission 字段。

TH06/TH07 使用各自的时序策略。默认 UI 配方读取实际房间配置中的所选值，产品上限用于选项校验。

测量启动只接受 Adonis Delay `mode=1` 或 Hybrid `mode=2`。TH08/09/10 可选择不启用 Adonis 的直接回滚路径；TH11 Mode 0 直接启动回滚，Mode 1/2 走 measured startup；TH15 仅使用 Mode 1 纯延迟。

## 调试启动问题

Adonis 状态依次经历 `WaitingPeer`、`Measuring`、`Negotiating`、`Committed`；失败轮次会进入 `Retrying`，达到重试或总时限则进入 `Unavailable`。标题代码可从 `AdonisConnection::Status()` 读取阶段、探针/回显数、尝试次数、失败原因、D/P 和各席位摘要。

`Choice::fullDelay` 是原始完整 RTT 帧预算 B；`Choice::delay` 是提交的 D；`Choice::prediction` 是 P。自动模式在启动报文里使用 `AdonisStartup::Automatic` sentinel，手动模式则绑定请求的 D。查日志时把这三列分开，才能判断是测量建议、用户参数还是标题运行决策。

启动报文包含 session ID、seat、player count、seed、gameplay ABI、mode、requested delay、prediction reserve 和 attempt。每个参与者从相同输入复算选择；`Committed` 只在该轮协商完毕后成立，标题还需完成 `NeedsApply()` 与 `Applied()` 交接。

传输包装器需要原样转发 `Recovering()`、`Disconnected()` 和 `CalibrationSuspended()`。校准中断时，这些状态决定是否暂停或重测；校准提交后，网络故障恢复由既有 `SessionChannel` 和浏览器传输生命周期接管。标题适配器还要保持输入接收队列在 Startup/SessionChannel 交接时连续，当前会话的原生 HELLO/READY 可以暂存后继续处理。

定位时按顺序查看本地原始报告、每链路有效样本数、最差 `p95Us`、Attempt/RetryReason、双方 Selected `(fullDelay, delay, prediction)`、`NeedsApply()` 和 `SessionGate::CanStart()`。三人房间特别查看 P2↔P3 的两个方向。

公共校准的单元入口包括 [`adonis-startup-test.cpp`](../../common/tests/adonis-startup-test.cpp)、[`adonis-calibration-test.mjs`](../../common/tests/adonis-calibration-test.mjs)、[`adonis-timing-test.cpp`](../../common/tests/adonis-timing-test.cpp)。产品时序策略入口包括 [`test-th11-multiplayer-policy.mjs`](../../launcher/tests/test-th11-multiplayer-policy.mjs) 和 [`test-th15-multiplayer-policy.mjs`](../../launcher/tests/test-th15-multiplayer-policy.mjs)；TH09 另有 [`adonis.cpp`](../../ports/th09/web/tests/multiplayer/adonis.cpp) 与真实世界的 [`output-history-browser.mjs`](../../ports/th09/web/tests/multiplayer/output-history-browser.mjs)。将运行环境、源码提交与结果链接填入本作 `ADAPTATION.md`。

浏览器 UI 状态通过 Wasm 导出读取。公共 [`adonis-calibration.mjs`](../../common/browser/adonis-calibration.mjs) 将 32 个 `u32` 状态词整理成阶段、选项、链路统计和已提交报告；它轮询 Native Runtime，展示结果并上报当前传输路线。

TH09 使用单独的 [`th09_startup_info`](../../ports/th09/web/cpp/sdl/Application.cpp) 导出，27 个 `u32` 词的 schema 为 3，包含其两人本地/对端摘要、Selected D/P 和重试状态。阅读或改动浏览器状态解析时，按标题实际导出布局核对字段数和 schema。

## TH08 与 TH09 的实用经验

### TH08

TH08 已把 `AdonisConnection` 放在现有标题输入 transport 之上，`NetworkConnection` 先完成测量，再配置共同 delay/prediction 并进入 `SessionChannel`。`RollbackDriver` 只在稳定游戏世界允许纠正，独立维护本地捕获前沿；确认输出从该标题自己的 Journal、音频和 Replay 队列排出。

TH08 的历史 Update 基准中，Bullet job 占约 88–91%，活动区域索引据此集中优化该路径。活动取消区域缓存只在 Item 更新后至 Bullet/Laser 更新前有效，正向与重模拟每帧重建；对象顺序、活动检查、命中计数、精确几何和特殊边界回退保持一致。历史候选基准详见固定 SHA 下的 [TH08 复用实验](https://github.com/YomotsuHisami/eagler-touhou/blob/991e7cdf9060252e022eba25431254c6a0ce73e9/launcher/docs/playbooks/rollback/th08-mp-base-update-reuse-2026-09-27.md)。测量构建、输入和设备条件见该记录。

### TH09

TH09 复用公共 `AdonisStartup` 和 `SessionChannel`，由标题 `RollbackSession::BeginMeasured()` 与 `PumpStartup()` 管理提交后切入。普通网络等待记录一个待重试的到期帧；下一显示机会只重试该帧，墙钟欠账不随等待增长。恢复复用已到期的帧，新本地输入保持原采样节奏。

TH09 的恢复与输出调度：恢复放在 Present 后分批执行；`RecoverySchedule` ticket 和 loop epoch 让暂停/停止取消旧回调；输入在 `Capture()` 一次采样并由重模拟复用；确认音频、Replay 和观战帧由独立发布前沿保留。报告中的旧浏览器/网络条件记录保存在 [固定基线回滚 Playbook](https://github.com/YomotsuHisami/eagler-touhou/blob/991e7cdf9060252e022eba25431254c6a0ce73e9/launcher/docs/playbooks/rollback.md#th09-input-scheduling-and-post-present-recovery-2026-10-16)。

## 故障定位

| 现象 | 可能的边界问题 | 检查位置 |
| --- | --- | --- |
| 通道看起来健康但统计样本不足 | 设备回调卡顿、后台隐藏、路径恢复，或多条链路中有一条未回显 | 每端 `AdonisStartup::LinkTo(peer)` 的 received/lost 与 retry reason；三人测试检查完整网状链路。 |
| 房间显示 D 已选，游戏仍不开始 | 某端尚未完成 accept/commit，或世界未就绪 | `AdonisConnection::NeedsApply()`、`Applied()`、`SessionGate::CanStart()` 和首个实际输入前沿。 |
| D 和实际感觉不符 | 把 RTT、D、P、显示采样延迟或产品上限当成同一数值 | 对照原始 RTT p95、fullDelay、Selected delay/prediction 与标题 capture frame。 |
| 会话中节奏变慢，但 D 未变 | AdonisPhase 消费墙钟等待校正，或标题自己的 input-gap pacer | 查 `TakeAdonisDelayMs()` 的调用点、标题墙钟 accumulator 与固定逻辑步长。 |
| Hybrid 仍发生回滚 | P 只是小范围预测允许，后续真实输入仍可能与预测不同 | 查预测位图、最早 RollbackFrame、实际输入前沿和 title journal 覆盖。 |
| 纯延迟模式启动后仍出现推测状态 | Title `CoreConfig.allowPrediction`、模式编码或读取房间字段不一致 | 核对 `PrepareFrame().predictedMask`、TH15 `MarkSimulated()` 精确输入断言和运行时 setup。 |

## 当前源码入口

- [AdonisStartup 协议、探针统计与预算公式](../../common/include/eagler/netplay/AdonisStartup.hpp)
- [AdonisConnection 与标题 transport 交接](../../common/include/eagler/netplay/AdonisConnection.hpp)
- [AdonisPhase 采样与帧节奏校正](../../common/include/eagler/netplay/AdonisTiming.hpp)
- [SessionChannel 输入/确认/维修包](../../common/src/netplay/SessionChannel.cpp)
- [产品时序声明](../../common/contracts/product-catalog.mjs)、[Runtime 选项规范化](../../launcher/src/launcher/multiplayer-runtime-options.mts)
- [TH08 实现](../../ports/th08-mp/web/cpp/multiplayer/NetworkConnection.cpp)、[TH09 实现](../../ports/th09/web/cpp/multiplayer/RollbackSession.cpp)、[TH10 实现](../../ports/th10-mp/web/cpp/multiplayer/NetplayRuntime.cpp)、[TH11 实现](../../ports/th11-mp/web/cpp/multiplayer/NetplayRuntime.cpp)、[TH15 实现](../../ports/th15-mp/web/cpp/multiplayer/NetplayRuntime.cpp)
