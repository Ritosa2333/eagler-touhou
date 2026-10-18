# TH09 连续输入信息预算：只读提案

状态：**仅设计，未实现控制策略，未运行新测试，未测得性能或流畅度收益。**

日期：2026-10-11。本文读取当前 `th09-rollback-4x` 与 `th09-rollback-input-batch-common` 源码及既有证据。保持 H=8、D=0、networkScale 上限 2、现有输入捕获与恢复规则。本文不改变正在执行的深 8 帧 CPU4 测量，也不承诺覆盖任意断网。

## 1. 判断与建议

可以比“B 达到 5 后才开始提高目标 scale”更早发现**连续输入信息可能耗尽**，最小有用信号是：

1. 本端还可以推进多少个新逻辑帧；
2. 连续实际输入前沿 A 已经多久没有推进；
3. 以往 A 推进事件的间隔及一次推进的帧数；
4. 这段观察是否被本端 Pump 未运行、接收预算未排空、恢复工作或浏览器事件未派发遮蔽。

这里的“输入信息不足”不等于“线路造成”。现有协议没有可用于上述因果区分的远端物理采样、发送或浏览器到达的同源时间戳。即使本端及时 Pump、JS 可见队列为空，也无法仅凭 A 不动区分远端 CPU/调度停顿、尚未送入 JS 的数据、RTC/relay 排队与线路抖动。

**建议先做影子预算预警，保持实际策略不变。** 首先用现有连续 callback 的 F/A 原始序列判断有没有可利用的提前量；只有提前量存在、且时钟和消费边界足够清楚，才值得加入默认关闭的轻量 Pump 观测。下一轮实验只改变“何时开始提升既有 scale 目标”，不同时扩大 H、不引入 cp3、不增加输入延迟帧。这样可以判断是提前制动有帮助，还是额外历史空间或恢复成本才是主要限制。

## 2. 当前源码实际提供了什么

### 2.1 前沿和现有控制

令：

- F = max(next, replayEnd)：本端已到过的最高活动前沿。
- A = 连续实际远端输入的排他前沿，即 Core confirmed + 1；尚无连续输入时为 0。
- B = max(F - A, 0)：目前缺少实际输入的连续历史长度。
- R = F - next：尚未重演完成的距离。R 与 B 不是同一个量。

`RollbackSession::RemoteInputFrontier()` 从 Core 连续确认前沿取得 A；它没有使用公开的 `ConfirmedThrough()`。后者在待纠错或未追上旧前沿时可以为 INVALID，并且受 next 限制，不适合作为输入到达信号。

当前网络目标是：

```text
target = 1 + clamp((B - 4) / 3, 0, 1)

B <= 4 : target 1
B == 5 : target 4/3
B == 6 : target 5/3
B >= 7 : target 2
```

scale 向上变化速率为每 50 ms 增加 1，向下为每 150 ms 减少 1。更新位于 `channel.Pump()` 之后；真实调度应用于未来逻辑时间信用，逻辑步长仍为 1/60 秒。已有待重试 tick 不因为新 scale 再等一个完整帧间隔。

来源：`th09_web/cpp/multiplayer/RollbackSession.cpp:96-153, 241-252`，`RollbackSession.hpp:7-11, 37-46`，`th09_web/cpp/sdl/Application.cpp:612-644`。

### 2.2 几类信号的含义

| 信号 | 可以证实 | 不能证实 |
| --- | --- | --- |
| A 实际增加 | Core 已接纳足以延长连续前缀的实际输入 | 输入在此刻才经过线路、对端刚刚采样 |
| A 不变、B 增加 | 本端在消耗未确认历史空间 | 一定是网络慢 |
| 收到了新的 packet sequence | 接受了更新的协议包序号 | 一定有新输入，或最早缺口已补齐；ACK-only 也可能有新序号 |
| packet.senderFrame | 经校验的新包报告的发送端模拟前沿 | 远端输入产生的 wall-clock 时刻 |
| 连续 A 长时间不动 | 本端尚未获得补齐最早缺口的信息 | 完全没有后续实际输入；乱序包可以已在缺口之后 |
| Pump 返回成功 | 本轮协议处理成功 | 接收队列已完全排空；Receive 有 256 包预算 |
| JS received.length - receivedHead == 0 | 该观测瞬间没有 JS 队列中的待消费包 | 浏览器、操作系统或线路没有尚未派发/送达的数据 |
| RTC peer.lastReceivedAt | 最近一次 input/control DataChannel message 回调执行时的 Date.now | 连续新输入到达时间；relay 也没有同等现成信号 |
| R、native callback 工作时间 | 本端恢复距离、该 callback 的实际工作 | 连续信息缺口由谁产生 |

Receive 在 `ApplyInputPacket()` 前后检查相应 peer 的连续确认前沿，只有它变化才表示补齐了前缀；phase pacing 又只使用有效的新序号及递增 senderFrame。所有本轮 Poll 都得到同一个 Pump 入口 nowMs，不能把它当作逐包到达时间。

`ConfirmedInputWatchdog` 已保留“前沿最后一次变化时刻”，但它用于 15 秒存活检查；默认不暴露为 SessionChannel 公共统计，且不解决 Pump 观测空窗。可以复用这个概念，不应把存活 watchdog 直接改成平滑控制器。

来源：common `src/netplay/SessionChannel.cpp:286-376, 413-463`，`include/eagler/netplay/SessionChannel.hpp:17-28, 71-84`，`ConfirmedInputWatchdog.hpp:16-46`，`SessionPacing.hpp:14-31`；common `src/netplay/BrowserPeerTransport.cpp:93-126, 444-450, 704-739`。

## 3. “耗尽时间”可以如何定义

在 B > 0 时，本端还能执行的新逻辑帧数是：

```text
Q = max(0, H - B), H = 8
```

Core 对待执行新帧检查的距离是 B + 1。B=7 可以执行第八个未确认帧，执行后 B=8；B=8 时，下一新帧受限。高于最早缺口的某一帧已收到实际输入也不会绕过这个限制。

如果 A >= F，本端有连续实际输入，不能因为“最近没有新的 A 事件”而提前降速。做通用空间计算时使用有符号差：

```text
Q_full = max(0, A + H - F)
```

它保留 A 超前的储备。控制的缺口分支仍要求 B > 0，不能丢掉已收到的余粮后制造虚假的紧张。

一个粗略的无新信息预算是：

```text
T_room ~= Q * (1000 / 60) * currentSchedulingScale
```

这只是未来信息预算模型。当前 scheduler 的剩余信用、下个 due tick 所处相位、一次 callback 可执行多个 due tick，以及 scale 的 50 ms 上升过程，都会改变精确边界。最小影子模型应至少预留一帧边界余量，并将“保持当前 scale”与“现在开始按原斜率升向 2”分别计算；不要把直接达到 2 的理想时长当成真实制动收益。

例如 B=4 时 Q=4；在恒定理想调度下，这些新状态对应约 66.7 ms 的 60 Hz 进度，scale=2 时对应约 133.3 ms。真实升速过程会消耗其中一部分空间。B=8 时 Q=0，降低有限的逻辑速率也不会凭空产生下一帧所需信息。

**预算的消耗速度使用计划中的 60 / scale，而非实测 F / wall-time。** 否则本端 CPU 慢、R 增加或 rAF 未派发会让模型以为“信息可以支撑更久”，把 CPU 问题揉进网络控制。R 和 native 时间应并排记录，用于解释恢复占用，不加入网络缺口的定义。

来源：common `src/netplay/NetplayCore.cpp:278-314, 362-378`；title `Application.cpp:479-558, 612-644`。

## 4. 到达节奏与缺口年龄：有用，但必须处理几个陷阱

### 4.1 记录连续前沿推进事件，而非包数

每次 A 真正增加时记录本端单调时钟 t、此前的 A、新的 A，以及 deltaA。过去的推进间隔 g = t(i) - t(i-1) 和 deltaA 必须同时保留。

不能把一次推进 6 帧的事件拆成 6 个等间距“输入到达”，也不能让 ACK、重复输入或仍未补齐最早缺口的乱序包刷新这个时钟。否则一次批量释放会伪造稳定 60 Hz 到达。

A 推进事件按 Pump 记录时反映的是“Core 已接纳”，不是浏览器 message 到达。当前每 callback 的 `networkPacing` 列则更弱：只是该 callback 末首次观察到 A 变化，真实变化发生在上一次观察与这次观察之间。

### 4.2 两种年龄不能混用

- **前沿停留年龄**：now - lastAAdvanceAt。很适合最小实验，含义清楚。
- **某个具体缺失帧的年龄**：now - firstNeededAt[frame]。如果最早缺口补齐，而下一缺口其实早已存在，就不能把它的年龄重置为 0。

第一版只用前沿停留年龄，并按该名字报告。若以后要引入具体缺口年龄，至少需一个 H+1 有界表，以帧号标识 firstNeededAt，跨 restore 保留，并在实际输入到达或 epoch 结束时退役；“A 每变一次就重设 gapAge”不能声称测到了最老缺失帧年龄。

### 4.3 不要把过期预估变成“马上到达”

`max(p95Gap - age, 0)` 在当前缺口超过以往 p95 时会返回 0；这恰好会在最长缺口时停止预警。不能采用。

更可靠的影子方法是：对前 32 个未被本端观测空窗污染的已完成推进间隔，保留当前 age 之后仍有历史支持的剩余间隔。历史中没有足够长的样本时，输出 **overdue / residual unknown**，继续记录正在经历的未完成间隔，不把它补成 0、删除或宣称能预测恢复时刻。具体分位数应预先固定，并保留样本数；例如 p90 与至少 16 个已完成有效间隔只是可选的首轮实验参数，不是已验证阈值。

第一版不建议直接使用 `scale = 60 / recentArrivalRate`。两端输入捕获与新帧推进耦合：本端降速后远端也可能降速，输入产生频率跟着改变。把较低到达频率直接当成继续降速的目标，可能产生相互维持的慢速状态。平均供应速度也不能表示批量包之间的长空档。

### 4.4 本端观测被遮蔽时暂停学习

以下情况应标成 observed-late / censored / unknown，而非学习成线路抖动：

- Pump 之间有明显的本端服务空窗；
- Receive 因预算退出，或仍看到 JS 队列积压；
- 已知 rAF 待派发、生命周期变化或暂停遮蔽了观察；
- 单次 native 工作覆盖了该推进间隔中不可忽略的一部分。

可以对实际 B 的硬边界继续执行现有策略；这里只暂停新估计器对“正常到达节奏”的学习及网络因果标签。不能丢弃对应原始异常。Pump 间隔正常、队列为空仍只能把部分本地因素分开，不能排除远端 CPU 或尚未进入 JS 的浏览器排队。

## 5. 最小实验：先验证是否存在提前量

### 第一步：使用既有 raw 做影子预警，不改运行时

按完整连续 callback 顺序处理 burst-response 和 relay125 的 `networkPacing`，保留旧回卷后的全部点；不能再按 next 范围过滤出不连续窗口。

每个观察点输出：

- F、A、B、Q、R、实际 scale；
- A 最后一次变化的观察区间、当前停留年龄的界限；
- 未受污染的历史间隔数、条件剩余间隔估计，或 overdue / unknown；
- 保持当前计划速率时的无新信息预算；
- 此刻按现有斜率升至 2 时的无新信息预算；
- 原 B 阈值开始制动的时刻，以及影子预警比它早多少；
- 下一次 A 推进是否落在预测的可支撑区间，或结束时仍未推进。

核心问题是：**有没有一段可识别的时间，原目标尚未制动，而提前制动所增加的预算足以等到实际下一次 A 推进？** 不能只统计预警命中卡顿的次数；必须同时报告健康区间的过早预警，以及已经超出 scale=2 能力的缺口。

这一步可以检查信息量是否足够，但不能据反事实重放给出“预计提升多少 FPS”。更改 scale 会改变两端输入捕获、发送与后续纠错，旧到达时间线不是独立于策略的外生输入。现有采样也不能提供精确的逐包 transport 到达边界。

目前本文未执行该分析。

### 第二步：必要时增加一个默认关闭的 Pump 统计探针

只有旧 raw 显示可能的提前量，才值得做以下小接点。每次本来就要执行的 Pump 追加一条固定数字记录，不新增 timer，不每帧 CDP，不增加模拟或物理输入采样：

```text
epoch
pumpEntryMonotonicMs, pumpExitMonotonicMs
F, next, A_before, A_after
receiveCount, receiveExitReason
lastAAdvanceAt, deltaA
B, Q, R, currentNetworkScale, currentEffectiveScale
knownQueueRemaining / unavailable
knownLocalObservationGap / unavailable
```

- 时钟使用本端 emscripten_get_now / performance 的单调域，注明 timeOrigin；不与 RTC Date.now 或 relay 的进程相对时钟直接相减。
- Receive 退出原因至少区分 Poll 为空、达到预算、disconnected/recovering 路径。只读统计不要再 Poll 一个包去“探空”，否则会改变消费所有权。
- 若公共 transport 无法提供队列数，就明确 unavailable；100 ms 外部 queue snapshot 不得伪装成每次 Pump 都已排空。
- 用固定容量，例如 512 条数字记录，保留首个风险前后有界窗口与最终未结束缺口。记录 dropped/truncated、起止时钟及覆盖范围。不要保存完整 packet payload。
- 估计器状态属于 SessionChannel/Session 的网络生命周期，不能放进 WorldState；恢复不得回卷学习时钟。
- default off 时不新增记录和采样开销；启用时的成本需单列，不能宣称完全没有观测成本。

如果需要继续区分“浏览器先收到、后来才入 Core”，应另做严格有界、可关联同一 input 包的 message/enqueue/accept 观测。它是边界归因的独立要求，不能用 relay send 或通用 onReceive 冒充；当前最小影子实验不声称已经具备它。

### 第三步：仅在提前量得到支持后做一次控制 A/B

未来可实验的最小策略改动，是让上述风险预警只提前提升**既有**目标 scale；范围仍为 1..2，仍使用原上升/下降斜率，H=8、D=0、预测规则、捕获次数、ACK/repair、重演预算全部保持。估计未知时明确记录未知；不能把“超过以往最大间隔”当作立即恢复。

是否对 overdue / unknown 也采取保守制动，需要单独预声明，不应在看到同一 burst 结果后调参。先评估健康 direct/relay39 的额外减速与响应代价，再观察固定 burst 的连续新状态间隔、真正等待、R 与 native 工作分布。两个策略必须使用相同带探针 harness、相同控制脚本和连续窗口口径。

D=0 也不意味着此策略没有 wall-clock 输入代价。游戏降速后下一次新帧采样可能更晚；若先前输入已经在被阻塞的前沿捕获，之后的物理变化也仍受原一次采样规则约束。验收必须保留独立 wall-clock intent 的 schedule→capture→first new-state，而不是仅报告配置里的 D=0。

## 6. 与 window 12、cp3 的比较

| 方案 | 直接改变什么 | 对突发信息空档的作用 | 成本与适用边界 |
| --- | --- | --- | --- |
| H=8 下提前预算制动 | 开始降低未来逻辑推进速度的时刻 | 用更均匀的慢速消耗现有空间，可能减少触碰硬边界的 wait | 对未知长缺口没有保证；可能在健康时过早降速；不增加历史容量，也不直接减少每次 replay 工作 |
| window 8→12，保持逐帧保存 | 多 4 帧未确认历史 | 同等 B 下多约 66.7 ms 的理想 60 Hz 空间，scale=2 下约 133.3 ms；不必先预测空档时长 | 更深纠错与更多 retained state；TH09 restore 当前在重演预算检查前逐帧撤销，单次 restore 峰值可能上升 |
| cp3 | 三个逻辑帧共用一个跨帧 first-write checkpoint | 间接降低捕获/恢复开销，为较大 window 的 CPU 成本创造空间 | 不自动增加可预测输入窗口；可能从更早边界重演，额外 0..2 帧；TH09 需要实际生命周期与恢复接口改造 |

### 6.1 window 12 是可测试的容量方向

TH09 的 `History` 同时进入 Core 的 maxRollbackFrames，并决定 `rollback_frames[History+1]` 的大小。由 9 个槽变为 13 个槽是 +4 个槽，不能只改 Core 的限制却忽略 world/audio/hash 历史保留。

当前恢复会先从 next 逐帧 Undo 到 correction，之后才进入最多 9 次循环、8 ms 恢复启动预算。因此扩大 window 时应保留当前 callback 工作预算，不应顺手把循环上限增至 13 来试图一次追完。实际 retained bytes、restore 峰值、跨 callback 重演与输入响应仍要测量。不能把槽数增长直接换算成精确内存用量，因为每个 WorldState 的保存内容及复用容量随 workload 变化。

来源：`RollbackSession.hpp:11`、`RollbackSession.cpp:34`、`Application.cpp:74, 512-558`。

### 6.2 cp3 不是 TH09 现有旋钮

当前 TH09 每个 speculative tick 保存独立 WorldState，journal 以单帧配置创建，Save 重开 frame 0，Restore 执行 UndoTo(0)。driver 从 next 到 correction 对每一帧逐个恢复；`RollbackSession::Restored(frame)` 又要求 frame 精确等于 Core 的 rollback request。

TH07 的参考源码则有 `checkpointLogicalFrames`。后续逻辑帧通过 `BeginFrame(frame, true)` 延长已有 journal，`RestoreTo(frame, &replayFrom)` 返回较早的真实 checkpoint 起点。移植这个方法涉及共享 checkpoint 的 first-write 去重、owning 生命周期、画面语义状态、输入历史与音效/Replay 发布边界，不能仅将 TH09 每三帧的另外两次 Save 删除。

来源：TH09 `WorldState.cpp:4, 141-142, 212-214`、`Application.cpp:512-558`、`RollbackSession.cpp:242-248`；TH07 `D:/workspace/eagler/th07-eagler/src/netplay/Th07RollbackState.cpp:564-643` 与 `Th07RollbackState.hpp:18-38`。

TH07 历史 cp3/window12 的良好结果仍是应追求的目标。它支持继续研究较深窗口与更低 capture 税，并不是当前 TH09 实现已经具备 cp3 的证据。

## 7. 与已观察到的 91.840 ms 缺口如何连接

现有 burst-response-01 记录的是 39±5 ms 基础单向注入，500 ms 内再额外增加 120 ms。P2 最长新状态间隔 91.840 ms 内包含 9 次 callback、4 次 wait、2 次纠错和 12 帧重演，累计 native 工作 55.230 ms，单次最大 15.725 ms。P1 的对应最长缺口则为 9 次 callback、4 次 wait、累计 native 1.870 ms、零纠错。两端 scale 都曾升至 2 后恢复。

所以提前预算制动首先应回答：**四次 wait 前是否还有可支配的信息空间，预警是否比旧 B 阈值更早？** 即使答案是有，也不能声称能消除 P2 的全部 91.840 ms；已有新状态缺口同时包含信息等待与拆分恢复。

已记录的秒级 active rAF 待派发是另一个问题。网络预算控制无法在游戏 callback 根本没有被派发时提交新的游戏状态，不能拿上述策略替代 rAF/浏览器调度诊断。

证据：`docs/multiplayer/rollback-chain-work.md:108-120`，`rollback-chain-release-burst-response-evidence.json`，SHA-256 `b254a712a15f79205c96fc0e3517e7e9e5dc9a52db7cc4ba21478c95e554bac9`。本文引用该既有结果，没有重新运行、重新归因或宣称修复。

## 8. 本次交付范围

仅新增本提案文档。未修改生产代码、harness 或控制参数；未启动 Node 测试、构建或浏览器；未执行影子统计。建议的下一项工作是只读分析现有连续 F/A 序列中的实际提前量，然后据结果决定是否值得增加小型默认关闭观测接点。

## 9. 后续交付：影子实验已执行，当前预警规则不直接上线

在根代理继续授权后，新增了独立的 `th09_web/tests/multiplayer/analyze-information-pacing-shadow.mjs` 和 `docs/multiplayer/rollback-information-pacing-shadow-plan.json`。第 8 节记录最初只读提案的交付；此节记录随后新增的离线实现。生产、冻结 harness/helper 和实际网络策略仍未修改。

计划已固定旧 burst-response 与 relay125 两份输入的路径、字节数和 SHA，以及 32 个完成 gap、至少 16 个历史样本、4 个条件存活样本、nearest-rank p90、1 帧预算余量和本地观察遮蔽规则。样本点完整保留 F/A/B/Q/Qfull/R，A 变化用本地读取上下界表示。误报代理只在已闭合且没有已观察遮蔽的 gap 中计数；间隔重叠、预测支持不足和最终无下一次 A 的 gap 均保留为未知或删失。已经在同一 gap 内观察到原制动的点不会作为“更早预警”。

无新信息预算分别解析计算“保持末端观测 effective scale”和“网络 scale 按原 50 ms/单位上升至 2、保留当前 phase 底值”。它们是连续速率模型，不复制未来离散 Pump、scheduler 剩余信用、CPU 工作或后续 phase 变化；scale 也只有按千分位导出的观测精度。输出不会提供反事实 FPS、输入延迟收益或网络根因。

2026-10-11 按根代理串行排期完成了一次正式执行：先核对不可变参数、记录 plan/analyzer 身份并通过 12 个数学与区间分类契约，再验证每份 raw 的尺寸和 SHA，之后才解析 raw。语法检查和正式执行均 exit 0，正式 Node elapsed 838 ms。四端源形状/读取区间/上下文校验均通过；plan/analyzer 前后 SHA 未漂移。完整输出以 create-only 方式写入 `docs/multiplayer/rollback-information-pacing-shadow-evidence.json`，包含原始计划。计划仍保留“声明时尚未获准读 raw”的历史状态，没有事后回写参数。

随后只对生成的 evidence 做了一次派生摘要，exit 0、Node elapsed 141 ms；没有重读原始 raw 或重算预警。完整结果与边界见 `docs/multiplayer/rollback-information-pacing-shadow-results.md`，紧凑数值见 `rollback-information-pacing-shadow-summary.json`。实际已执行命令为：

`node th09_web/tests/multiplayer/analyze-information-pacing-shadow.mjs`

**当前规则不应直接上线。** 旧 burst 两端完整报告的首预警为 36/63 个：闭合且未标观察污染的 28/54 个全是保持当前速率预算内就得到下一 A 的无必要预警 proxy；其余 8/9 个有后续污染，保留未知。首预警没有可确认的严格模型机会。正文对应首预警为 36/61、proxy 为 28/53、污染为 8/8，不能把正文和完整报告混用。

这不等于全部机会为零：对所有点与未预警 gap 独立回看，burst 两端各有 1 个严格模型机会，且均漏报。P1 sample 3603、P2 sample 3032 都已有 32 个历史 gap，survivor 分别为 32/13，预测剩余 p90 为 33.3475/21.5875 ms，小于 hold 安全预算 50.100/49.850 ms，因而未警告。实际下一 A 区间分别为 [51.590,58.205]/[50.260,64.365] ms，落在 ramp 安全预算约 80.685 ms 内；相对原 B 制动的领先区间下界仅 5.045/2.040 ms。这是尾部低估的模型漏报，不是未暖机，也不是已经证明的平滑收益。

relay125 两端首预警与严格提前机会均为 0；正文已全部暖机，主要是现有制动已经发生，余下少数候选受到观察污染，不能归纳为无任何改善空间。四端各保留初始左删失和最终右删失 gap，未知与污染不作真阴性。全报告诊断完整性是事后证据资格门，并非当时在线已知的网络信号。

下一步先利用 producer/relay/browser/native 接纳边界判断输入缺口来源，必要时只增默认关闭、O(1) 且有界的队列/Pump 信息观测，再预声明另一项独立实验；不在本次 raw 上调阈值后覆盖证据。每 Pump 服务、剩余队列、逐输入浏览器到达和远端 producer 的实际推进仍未测，A 静止不能归因线路。生产、harness 和真实控制策略均未更改，没有反事实 FPS 或输入延迟收益结论。

这两份旧报告的总体现象此前已经知晓，因此该计划是执行前固定方法的探索性再分析，不能称为盲测或独立复现。若首次执行发现实现缺陷，应记录缺陷和修正；不能在看到结果后调整阈值并覆盖同一份证据。


