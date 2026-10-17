# TH09 release 4× 网络与输入响应验收

本页只说明既有配置和指标，不增加性能门槛。目标是低输入延迟和连续的新状态提交；允许因远端信息不足平滑放慢，但不能用重绘旧状态掩盖停止。

核对时 smoothness-browser.mjs SHA256：a0ce03e2e79efaeab2f605c2e14f94415bce543cfdcb57d0e83eee4be4f34fef。后续报告以各自 identity 为准。

## 配置

每份使用唯一 RUN_LABEL、明确冻结 PC_BUILD，入口：

~~~sh
node th09_web/tests/multiplayer/smoothness-browser.mjs --release --relay
~~~

| 环境变量 | 建议值 |
|---|---|
| FRAMES / CPU_RATE / HEADED_CONTROL | 1800 / 4 / 1 |
| IMPAIRMENT_SITE | relay；默认仍是 client，必须显式指定 |
| DELAY_MS / JITTER_MS / DROP_EVERY | 稳态参考 39 / 5 / 0；独立高延迟条件 125 / 5 / 0 |
| INPUT_MODE / RESPONSIVENESS | wall-clock / 1 |
| RAF_DIAGNOSTICS / AUDIO_OUTPUT / HOST_LOAD | 1 / 1 / 1 |
| PROFILE_CPU / LIVE_COST / STARTUP_TRACE | 0 / 0 / 0 |
| NETWORK_TRACE | 主性能口径 0；必要的收包/消费定位另 label 开 1 |
| EXPECT_RTC_PATH | any |
| BURST_AT_MS / BURST_FOR_MS / BURST_DELAY_MS | 稳态 0/0/0；独立 burst 条件 20000/500/120 |

relay 注入自动开启独立 relay trace。fixture 将服务内建 delay/drop/skew 归零；遗留 TH07_* 别名若冲突会启动失败。CPU 实际为两个独立 browser 的 [1,4]，且 openProbe 完成后才配置；不能把 openProbe 耗时称为 4×。

逐包 browser observer 已有可见成本；输入合并减少 observer 工作不能冒充生产收益。NETWORK_TRACE=1 的定位报告应单列，保留全部不利样本。

## 故障含义与正式窗口

39±5 ms 在 relay→目标客户端方向注入，双方各受一次，客户端同步 send。只处理 E9NP v4 kind 1，包含 ACK-only 和冗余输入；控制/hash/信令不延迟。每 sender 的 LCG 按实际转发推进，因此不同 build 的相同 seed 不保证逐包时间线相同。

burst 锚点是 relay 首次收到非空输入。20000/500/120 表示锚点后20秒起，500ms内 schedule 的输入额外延迟120ms，计划单向154–164ms；不是丢包或500ms断网。每包独立 timer，允许重排；burst 后普通包可越过仍排队的旧包。

原主 summary 逐行筛选 frame 180..FRAMES-1，这一定义在 frozen raw 中保留。如果 next 已达到 FRAMES 后又因 correction 回卷，筛选会离开再重入；此时保留下来的两行之间可能存在被排除的真实 callback，不能把两行时间差直接称为 rAF 停顿。现有 burst gate 只要求每 sender 至少一个 burst scheduled 包的 senderFrame 在此范围，不证明整段 burst、接收及其影响都在 formal。必须读实际 events 和前后样本。host 的 formal-poll-start 不是该窗口精确起点。

response 从 first-active 到 ended，全会话分位数不能直接当 formal。用同页时间对齐：

- 游戏观察：measure.started + samples[i][0]。
- 输入阶段：responsiveness.startedAt + intent 的对应 AtMs。
- 先按窗口内的 scheduledAtMs 选意图，再保留完成、未完成及 pendingAge；不要先按成功结果选样本。

旧 burst 离线证据以首/末符合原 frame 筛选的观察时刻组成闭区间，保留该历史定义和文件 SHA。新分析器 schema 2 同时输出这套旧 cohort 与一套连续时间窗：首次观察 next>=180 到首次观察 next>=FRAMES，采用半开区间 `[start,end)`。终点观察作为 elapsed 和最后新状态间隙的边界证据保留，但其 callback 归 tail；后续回卷不能再归入连续 formal。before-formal 只指已经 active 的热身段，不覆盖 openProbe/连接等更早启动；after-formal 为实际仍记录的尾段。

每套 cohort 都按计划时刻分组，再分别记录提交、首次 wire capture、新状态观察发生在哪个窗口。正式计划在 tail 才完成仍保留为正式分母，不能因完成晚而移出。新状态间隙从全部连续 samples 重建，跨窗口间隙与未出现下一个新状态的观察尾段单列；旧 raw summary 原样保留，不能静默替换成修正后的数字。

跨进程不能直接相减 performance.now()；用已有 clock pair/timeOrigin 并保留误差。无 browser clock pair 时，不声称毫秒精确的跨进程传输时长。

## 输入与画面的边界

独立 timer 保留全部计划变化；若长任务吞掉多个变化，只提交当时仍有效的最新状态，未提交计划保留。firstWire 是 send wrapper 看见连续新输入帧，位于原生捕获/序列化后、实际发送调用前，不是远端收到或 ACK。

presentationObserved 同时要求逻辑 frontier 和 renderer submission counter 增加，且 frontier 越过 captureFrame。它证明输入参与了新状态提交，排除仅重绘/仅历史重演；不证明像素运动、物理扫描输出或硬件输入响应。一个 callback 的多个 Present 只有同一观察时刻。

必须同时报告 counts、observations、全部 intents/captures。条件分位数排除了未完成阶段及相同 mask 重提交；notSubmitted、submittedNotCaptured、capturedNotPresented 不能算零延迟或丢弃。另核 inputDelay=0。

这里的 synthetic intent 是完整 hosted-key **保持状态**，不是不可覆盖的输入事件队列。`responsiveness-probe.mjs` 的 submit 逐键调用 `_th09_key(scan,down)`；collectDue 保留全部计划记录，但只提交当前仍有效的最新状态。`Application.cpp` 的 `_th09_key` 只赋 `k.hosted`，sample_keys 在 fresh capture 时读取当前键位；`GameInput.cpp` / `InputFrame.cpp` 再由相邻帧 held 的差异生成 pressed/released。真实 Runtime 的 managed.mjs → directory-keyboard.mjs 也把当前物理 owners 的合并状态立即交给同一 ABI，没有要求每次跨键组合必须独占一个模拟帧。

因此完整 mask 意图未捕获必须计入本探针的未完成项，但还需要看组成按键：中间组合未形成一帧，不必然等于某个键的 DOWN/UP 丢失。要决定是否改变捕获，应先区分“当前 held-state 尽快采样”“同键短 tap 至少保留一次”与“每个跨键组合都占帧”的合同；直接队列化所有组合会改变按住与释放的生效时刻。现有输入序列、分母和 gate 不因这个区别而修改。

passed 仅说明 workload/hash/probe 合同满足，未限制最大延迟或新状态停顿，也不要求每个 intent 都 presented。wall-clock 在各运行中可能落到不同逻辑帧，不能要求跨 run 相同最终 hash 或 CPU 工作量；同 run 两端的120帧已确认 hash 检查必须一致。

## Raw 必读字段

报告：th09_web/artifacts/multiplayer-tests/<RUN_LABEL>-report.json。

| 字段 | 解释 |
|---|---|
| identity / parameters / headedControl | 冻结源、配置与 Chrome；identityVerified 必须单独检查，visible 不证明物理显示 |
| results[].measure.samples | 14项：elapsed、ok、C++ms、frame、confirmed/published、corrections、resim、captures、repairsSent、ready、published、submissions、phase、rAF timestamp |
| measure.frameDetails | 同索引 frame、累计 WorldState Save、可选 last-tick update/draw；capture不是Save，last tick不是callback总时 |
| measure.networkPacing | sampleIndex、version、missingRemote、networkScaleMilli、effectiveScaleMilli、predictionFrontier、actualContiguousExclusive |
| responsiveness | 全部意图、捕获、阶段时刻、未完成数量和截止年龄 |
| rafDiagnostics.requests / frames / controls | requested/entered/exited、onGameFrame/play.frame、pause请求；rAF timestamp不是entry |
| relayTransport.injection / packets / events / timing / pending | 锚点、方向、receive/scheduled/send、dueAt、timer偏离、独立进程停顿 |
| networkTransport（可选） | intent/send/message/poll-copy；copy不等于原生校验接受 |
| lifecycle / headedControl.windows / hostLoad / audioOutput | 环境与其他回调旁证，不证明硬件GPU时间或声学连续性 |

relay 按 columns 解码；packetId 只在本 trace 内有效。跨边界匹配规范化字段与摘要，再用 receiveEventId/scheduledId 区分重复尝试。send.enteredAt-dueAt 是 timer 偏离，不能直接当网络延迟。

首重 effectivePresentationGapMs 及对应 raw 区间，因为它同时要求新 submission 和更高逻辑 frame；presentationGapMs 允许旧状态重复提交。平均Hz/分位数均附最大停顿、区间 callback/Save/replay/输入缺口；startup和收尾异常另列。

actualContiguousExclusive 停止、missingRemote与networkScale上升支持信息不足下的放慢。短C++但已申请rAF长期未派发，不能只归因回滚计算；rAF持续却只有wait/旧帧，也不是新状态连续。旧 confirmed/published 合成字段不能替代连续远端输入 frontier。

准确每次 rollback depth 在此 frozen release raw 中不可观测：没有 restore target 或每次 replay span。相邻样本的 correction/resim/Save 累计差可以报告实际次数与重演工作量，`max(0,predictionFrontier-next)` 可以报告观察时刻尚未重演的历史 ticks；这些都不能直接标成 rollback depth，更不能从 DELAY_MS 换算深度。networkScale 分布按 callback 采样，也不是按 wall time 加权的比例。

依据：当前 smoothness/responsiveness/relay-transport/network-transport probes 和 Application.cpp 只读导出；旧 M burst 的20000/500/120来自 rollback4x-responsive-text-network-burst-01-report.json，但旧版使用client注入，不是相同relay时钟条件。

## relay125-01：连续窗口与未完成组合状态

原始 `rollback4x-chain-release-input-batch-headed-relay125-01-report.json` 为 28,866,220 B，SHA-256 `d03833c48742160afa7956a70b4aa82f135069005e3b4b38422a6b3c08bca770`。独立离线证据为 `docs/multiplayer/rollback-chain-release-relay125-response-evidence.json`，保留旧/new 两套 cohort、全部意图、被覆盖记录、warmup/formal/tail、实际 relay 分布、完整 audioOutput 和 host 记录；旧 burst evidence 未覆盖。

两端连续半开 sample 区间为 P1 `[573,5745)`、P2 `[561,5729)`，以终点行闭合计算得到 40.60846 / 40.61585 Hz。P2 旧 frame 筛选把 samples 5729..5738 的 frame 1800..1802 排除，再在 sample 5739 的 frame 1799 重入，形成原 summary 的 83.735 ms 拼接值；这十条真实 callback 一直存在。连续窗内 rAF timestamp 间隙最大 25.795 / 25.105 ms；用全部 samples、并包含首次到达终点的边界，新状态间隙最大 41.135 / 44.695 ms。这些仍是 callback/renderer 观察，不是物理扫描输出。

两套正式意图 cohort 在此样本恰好均为每端 161 条。**P1 为 161/161 捕获并提交新状态；P2 为 160/161，完整意图覆盖没有达到全部成功。** P2 缺失 id40 确在 formal，不能作为 tail 或短保持而删除。原 harness 的 `passed=true` 未要求全部意图完成，不能转述为 161/161 输入成功。

| P2 意图/采样 | 按键状态 | response 时钟 |
|---|---|---:|
| id39，mask 5 | Z(1) + Shift 专注/低速(4) | 计划 9,360 ms |
| capture frame394 | mask 5 | firstWire 9,737.185 ms |
| id40，mask 69 | Z + Shift + Left(64) | 计划 9,720；提交 9,747.240 ms |
| id41，mask 65 | Z + Left，释放 Shift | 计划 9,750；提交并覆盖 id40 于 9,752.080 ms |
| capture frame395 | mask 65 | firstWire 9,767.505 ms |

id40 的实际组合保持 4.840 ms，未形成独立的 wire capture 或新状态。Left DOWN 在 id41 仍保持，下一次采样从 5→65 会产生 Left 的 pressed=64 与 Shift 的 released=4；不能描述成整个 Left 按下丢失。本次没有修改输入语义或重跑以抹去这条未完成记录。键位依据为 `portable/input/KeyboardMap.inc`、`KeyboardInput.cpp`；bit4 的专注/低速用途由 `PlayerMotion.cpp` 明确使用。

正式条件延迟如下，单位 ms。P2 的三项分布都只来自已完成的 160 条，旁边必须保留 1 条未完成；不能将其计为零。

| 指标 | P1 n=161：p50 / p95 / p99 / max | P2 n=160/161：p50 / p95 / p99 / max |
|---|---|---|
| schedule → 首次 capture 观察 | 20.220 / 33.435 / 42.715 / 45.280 | 24.395 / 41.215 / 49.585 / 50.775 |
| schedule → 新状态观察 | 20.845 / 34.530 / 43.225 / 45.785 | 27.795 / 49.205 / 54.325 / 59.435 |
| submit → 新状态观察 | 12.140 / 24.815 / 30.525 / 30.585 | 19.255 / 33.155 / 38.190 / 40.155 |

热身每端 20 条均完成，P2 热身 schedule→新状态最大 84.735 ms，未藏入正式分位数。tail 仍有每端 20 个 callback 样本，但没有新计划意图，不能用空分布宣称 tail 响应为零。两端 D 均为 0。实际 missingRemote 正式观察范围均为 4..8，networkScaleMilli 中位为 1473 / 1340；连续正式段记录 correction 292 / 299、resimulated ticks 1764 / 1740，属于观察数据，不是由 125 ms 推算。

音频每端 521 个输出抽样块，非零块为 519 / 518，原输出门槛通过。抽样为零只发生在最初 2 / 3 块，对应 AudioContext 从 measure.audioStart 推进约 90.667..170.667 / 90.667..232.000 ms；随后没有近零抽样块。每块只抽样第 0 声道最多 64 个值，未记录队列欠载标志，不能据此证明无 underrun 或声学完全连续。最大抽样幅值为 1.019997 / 1.048200，也如实保留。host 全部 61 次 CPU 样本（60 个有效区间）、14 次进程样本与 402 行候选进程记录均保留，无错误/overflow；整体 busyFraction 的均值 21.699%、最大 47.835% 不能证明单核/GPU余量或排除短暂外部任务。
