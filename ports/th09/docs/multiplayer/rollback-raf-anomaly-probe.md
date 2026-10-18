# Active rAF 异常的轻量有界观察

本次继续追查 headed relay39 中两端约 1.4 秒的已排队 rAF 缺派发。该 episode 的独立中继仍有 receive、schedule、send 和 heartbeat，游戏 callback 本体很短。旧 live 的约 2.177 秒 episode 中浏览器 timer、poll/native pump 也继续。成功的 scheduler-short-03 没有重现旧异常；不能把采集完整或未复现写成生产修复。

本改动提供新的定位证据。**它没有改变生产帧调度、回滚、输入、音频、网络恢复或 fallback，也没有宣称卡顿已修好。** 同一正式性能比较的各组必须使用同一个 harness、同一开关和 helper 身份。

## 选择轻量观察的理由

六类别 Chrome trace 的成功短窗仅约 8.67 秒，每端就有约 249,000 个原始事件、本地保守计费约 71 MB，结束回传约 23 秒。继续反复缩短完整 trace 而未复现，不能回答偶发异常的原因。异常发生后才开始 trace 又缺少异常前史；持续原生录制有实际成本，不能称为无开销。

本轮先复用现有每 100 ms 的 Node 状态轮询，保留小段页面状态前史。它能在相同 pending request 下观察浏览器接收队列、发送缓冲、当前 public stats、游戏 sample 和 callback 计数是否变化，且无需新页面 timer 或逐帧 CDP。它提供的前史是**有限次数的实际轮询观察**，不是线程 trace，也不是覆盖每一毫秒的连续记录。

## 开关与接入

`RAF_ANOMALY=1` 明确启用，默认关闭。启用要求 `--release` 与 `RAF_DIAGNOSTICS=1`；本轮与 `CHROME_SCHED_TRACE`、`PROFILE_CPU`、`LIVE_COST`、`NETWORK_TRACE` 互斥，避免把多个重型观察混在同一性能样本里。独立 relay trace、host、audio、responsiveness 与 actual direct 的原有验证可以继续。

文件：

- `th09_web/tests/multiplayer/raf-anomaly-probe.mjs`
- `th09_web/tests/multiplayer/raf-anomaly-probe.test.mjs`
- `th09_web/tests/multiplayer/smoothness-browser.mjs` 的相关 flag/import、peek、轮询和最终快照接点

页面 API：

```js
// 只在 enabled 时序列化安装到本 harness 新建的页面。
installRafAnomalyProbe({enabled:true,side});
// 原有 status poll 已经读取 stats，传入同一份；probe 不另行调用 WASM。
window.rafAnomalyProbe.observe({stats});
// 成功/失败结果中最终保留一次，不创建另一个轮询循环。
window.rafAnomalyProbe.snapshot({final:true,stats});
```

`observe()` 只返回小型当前状态和首次 episode 的索引/状态。完整有界前后史在 `results[side].rafAnomaly` 或失败的 partial 对应字段里。Node CDP 请求/返回括号保存在独立 `rafAnomalyNodePolls`；原游戏 raw samples、原 rAF 全量诊断和所有失败仍保留。

默认 off 不动态 import helper、不读取其源码 SHA、不安装页面 API、不创建 Node 轮询记录、不执行观察。已有状态轮询的 stats 读取次数保持一次。新的 `peek()` 方法只附加在原有 `RAF_DIAGNOSTICS` 对象上，没有新增 periodic 调用；原 `snapshot()` 的返回与遍历行为保持原值。

## 严格有界的数据访问

原 rAF `snapshot()` 返回 requests/frames/controls/callbacks 的页面内数组引用，没有复制这些数组，但会遍历整个 pending Map。新专用 `peek()` 用 iterator 最多读取 **4 个 pending 项**，另给出完整 `pending.size` 和截断标志。helper 只投影固定字段，因此这些原始数组不会被新的 CDP 状态请求序列化。

每次观察的界限：

| 项目 | 界限与处理 |
|---|---|
| 新 request 检查 | 最多最近 64 项；未扫描数累计，coverage 不完整 |
| pending 候选 | 最多 4 项；总数/截断同时保留 |
| control / lifecycle | 各最多 128 项；超限不猜 epoch/可见性 |
| RTC peer | 最多 4 项；超限显式累计 |
| 前史 | 最多 32 次实际 poll，环形逐出旧观察 |
| 触发后史 | 最多 32 次实际 poll；额外次数累计，最后观察单独保留 |
| 页面结果数据 | retained JSON 预算 256 KiB，预留 32 KiB 元数据；不声称等于 JS heap |
| Node poll 括号 | 最多 2,048 组；另保留最后与进行中组，超限计数 |

32 次 poll 不能直接换算成固定 3.2 秒。CDP、页面、Node 或宿主忙碌时轮询间隔会变化，实际 before/observed/after 都被记录。前史逐出属于显式的环形策略；request/pending/control/lifecycle 截断是覆盖限制，不能把这些缺项解释成“没有异常”。

## 哪些 request 可以触发

阈值为 **250 ms 的 eligible dispatch wait**，只用于定位秒级异常这一类别，绝不是新的“低于 250 ms 就流畅”的验收线。正式新状态间隔 p95/p99/max 及原帧目标继续独立计算。

候选必须属于已有游戏 rAF chain，前一回调的 game frame 已有 active sample、尚未结束，前一 wrapper 已返回，且请求/进入区间没有暂停、stop、重新 start 或隐藏生命周期。游戏启动时已排队但 pause=1 的 rAF、无 active sample 的启动任务和正常关闭尾部不会作为 active 异常触发。

- request 身份使用原 `request.index`、`requestedAt` 与从 `_th09_loop_start` 观察到的 epoch。
- `nativeId` 仅作为辅助字段，取消后的 native ID 复用不能视为旧 request 恢复。
- 等待使用 `requestedAt` 和真实 `enteredAt`。浏览器给出的 rAF timestamp 不用于等待计算。
- `eligibleAt = max(requestedAt, parent.exitedAt)`，避免把在父 callback 内先发出的请求所覆盖的父工作量算成派发饥饿。
- 只 latch 第一个 eligible episode；后续保留同一 request 的 pending/entered/cancelled/paused/stopped/restarted/hidden/owner-changed 结果。
- 长等待可能在两次 poll 之间已结束。helper 检查有限新 request，并单独追踪上次 poll 的 pending 索引，保留 `completed-before-poll`；不会要求“恰好轮询到正在挂起”才承认异常。
- 暂停发生在真实 entry **之后**，不取消已完成等待的证据；暂停/stop/重启发生在 entry **之前**，自然恢复时长被删失，不能伪造已恢复。
- 最终仍未进入的 request 保留在 final observation；没有 entry 的 `waitMs` 为 null，不填零，也不把 final 样本时刻说成恢复时刻。

这里的 observed loop epoch 是 harness 观察到的 start control 身份，不冒充未直接读取的 C++ 内部 epoch。页面的 diagnostic/play/measure/core 对象被替换时，旧 owner 证据结束并标记，不把旧 pending 接到新会话上。

## 保留的数据及解释边界

每条页面观察包含：

- 当前页面 `performance.now()` 的 before/observedAt/after、`performance.timeOrigin`，以及 RTC lastReceived/lastInputSent 所使用的独立 `Date.now()`。
- active/connected/recovering/finished、measure 起止、关闭次数、可见性、pause 请求、observed loop epoch。
- sample 数、最后 sample 时间、next/confirmed/corrections/resim/captures/Present/session phase，以及原状态轮询取得的 public stats。
- 原 rAF 的 requested/entered/exited/cancelled/gameFrames 计数与有界 pending request。
- JS transport 的 received array length、head、remaining；relay bufferedAmount；RTC 每端 input/control bufferedAmount、连接状态和已有 lastReceived/lastInputSent wall-clock 值。

不保存包内容、键码历史、URL/query、SDP、candidate/IP、栈或任意 peer 文本。所有动态状态字符串只能通过固定枚举；没有创建网络监听器、重发、消息队列、timer、rAF 或新 frame pump。

`receivedHead` 和 array length 会在既有队列压缩时归零，**不是累计收/取包数**。单次 `queuedPackets=0` 不证明这个区间没有流量，也不证明 native 已接受全部实际输入。RTC lastReceived 的变化能说明其 message handler 已执行；它不是网络栈到达的精确时间。`bufferedAmount` 是发送队列观察，不能直接代替对端到达、确认或接收队列。

Node 与每个 Page 的时钟名称和 timeOrigin 分开记录。Node 的 CDP 请求/返回括号可表明观察到达迟滞，不把不同原点的数字直接相减成传输延迟。丢失一次 poll、一次 evaluate 很晚返回或全局 CPU busy 上升，都不能单独证明某个 rAF 没派发；只有已识别 request 的原始字段提供这一证据。

## 对 relay/direct 下一步的实际判别力

| 同一个 pending request 期间的观察 | 可支持的判断 | 仍需补充的证据 |
|---|---|---|
| public stats 或 RTC lastReceived 推进、rAF game/sample 不推进 | 页面部分网络/控制路径仍在执行，和 rAF chain 可分离 | 具体 Chrome frame eligibility、compositor、GPU 或任务类型 |
| received remaining 增长、消费 head 不推进 | 已进入 JS receive 队列但尚未排空的线索 | 逐包入队/取出时刻、native 256-message budget 命中情况 |
| 接收 queue/head 周期变化但 game sample 不变 | 接收与队列消费可能继续；不能仅从单点 head 算吞吐 | 连续累计或逐包诊断，确认 native 接受而非只复制 |
| relay API send 继续，浏览器没有新的接收观察 | service 发出到浏览器观察这一段尚未闭合 | socket/browser message/消费边界；API send 不是远端交付 |
| input/control bufferedAmount 增长 | 浏览器发送队列拥塞或对端未消费的候选 | 实际路径、repair 频率、可靠控制 HOL、网络条件 |
| 完成后的 request 显示长等待，但中间没有任何成功 poll | 存在该 request 的长 dispatch wait | 不能区分 renderer 整体任务阻塞、CDP/Node 拥塞或 frame 专用派发；仍需 trace |

现有源码已有分层：common `BrowserPeerTransport.cpp` 的 receiveBinary 入 received[]，poll_copy 按 head O(1) 消费并偶尔 compact；TH09 `SharedNetplay.pump()` 另有 16 ms timer；native SessionChannel 有每次有限 DrainPackets。独立中继继续运行不意味着浏览器消费链也健康。actual local direct 使用 unordered/no-retransmit input 与 ordered control，但一次 localhost host/host UDP 成功仍不覆盖公网 NAT/TURN 或深度回滚下的恢复。

旧 RTC constructor 的秒级同步开销属于连接准备阶段，与 active rAF episode 分开。此补丁没有调整约 4.5 秒协商期限，也没有因偶发 constructor 慢就扩大重试或改变 fallback。

## 如仍需要原生环形 trace

本轮**没有**开启或实现 Chrome 环形 recorder。若轻量记录确认仍是相同 rAF episode，应另行设计明确 opt-in 的 native ring，保留异常前的线程证据。之后才 start trace 无法补回前因；一直录制则必须披露成本。

候选最小范围是 owned Chrome 内较小原生 buffer、相同已审查类别及一次性触发/截止。原本 `recordUntilFull`、buffer-full 判错、armed/first-active/capture-stop 三标记完整性规则不能直接套到会覆盖早期数据的 ring。应重新证明最近窗口的保留、两个近期时钟标记、episode 请求开始与 end 覆盖，并单独记录 intentional overwrite 与真实数据丢失。现有 32 MiB 在短窗约使用 30% 的数据只够估计更小 buffer 的时长，不能保证任意 workload 的秒数。

`Tracing.end` 后现有 ReportEvents 的大量回传/清洗会影响之后的工作；不能在自然恢复时直接 flush 后仍把后续游戏帧当成无额外干扰的正式性能样本。若选择诊断提前结束，必须保留未完成 workload、暂停引入的删失和全部原始失败。也不能靠新增 timer 游戏循环、提前请求 rAF、关闭 vsync、安全软件或代理修改来制造一个不同条件的“修复”。

## 已执行的验证

本次只运行了纯 Node 和 syntax 检查，没有启动 browser/build/performance 命令。

```text
node --check th09_web/tests/multiplayer/raf-anomaly-probe.mjs
node --check th09_web/tests/multiplayer/raf-anomaly-probe.test.mjs
node --check th09_web/tests/multiplayer/smoothness-browser.mjs
node --test th09_web/tests/multiplayer/raf-anomaly-probe.test.mjs
node --test th09_web/tests/multiplayer/chrome-scheduler-finish.test.mjs
```

13 个 anomaly 合同通过，包含从实际 harness 源码提取的 peek 最多读取 4 项/原 snapshot 仍返回全部 pending、default off、无新 timer/rAF、pending 身份、间隔中已完成等待、native ID 复用、pause/hidden/startup 排除、重启/owner 删失、父 wrapper 与派发等待分离、前后史/JSON 上界和隐私。原 scheduler finalizer 3 个合同也通过；其代码未改。该次串行命令总耗时约 0.865 秒。

提交给 root review 的 harness SHA 为 `dde1d2e8f744b33e2eb56337ada81524083f334cd5b897c1dbb57bbf17a74fec`。Chrome collector 本体未改变，不重跑不相关的采集校准。真实新性能运行由 root 串行安排；本页不把 mock 通过称为已复現真实卡顿或生产收益。
