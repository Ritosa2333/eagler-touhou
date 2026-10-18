# TH09 预测回滚响应性：实现与证据记录

更新日期：2026-10-10。本文记录本轮 L、M、N、P 候选，早期 J 作为已经冻结的对照。完整构建身份、逐端汇总、输入与确认哈希交叉校验、原始报告路径和 SHA-256 见 [rollback-responsive-evidence.json](rollback-responsive-evidence.json)。

## 1. 当前结论与验收目标

**当前是已通过所列正确性检查、尚未通过性能验收的实验候选。** 4× CPU 实测仍有低于 60 Hz 的逻辑吞吐和长时间没有新画面的情况，不能宣布“4× 无卡顿”。同时，两轮正式比较存在较大的运行间变化；新一轮已实际记录到其他编译进程，当前证据也不足以直接判定候选造成性能回退。

用户目标按以下顺序验收：

1. 玩家输入应尽快被采样、发送，并影响后续新画面。预测回滚应降低等待实际网络输入的时间。
2. 新游戏画面应连续出现。需要分别观察新逻辑前沿、真正发生的渲染提交和连续无新画面的最长区间；重复提交同一状态不能代替这个目标。
3. 缺少网络信息时，可以在预测余量耗尽前平缓降速，并在连续实际输入恢复后回升。算力不足仍需优化，不能把固定慢速当成网络自适应的成功。
4. 保持 60 Hz 逻辑语义、现有 8 帧预测上限、游戏与 Draw 的历史副作用、完整 BGM/SFX、暂停/结算/Replay 语义。

D=0 原本已经存在。本轮前移输入采样减少的是“先做恢复工作，再采样/发送”的额外等待，没有把某个原先非零的输入延迟设置改成零。现有测试也尚未给出真实硬件输入到屏幕发光的延迟结论。

## 2. 候选内容与冻结身份

下表 SHA 为便于阅读的前缀；完整 64 位 SHA、源清单和文件大小保存在证据 JSON 与各 build.json 中。不同构建的测试结论仅归属于实际被测构建。

| 标记 | 主要内容 | 被测 WASM 身份 | 作用范围 |
| --- | --- | --- | --- |
| J release | 既有渲染批次/集中顶点上传/Lines 重定位、Present 饥饿修复、不可变弹幕模板排除、背景连续区间快照及既有音频路径优化 | b525102519c98ac5…，2,819,778 B | 本轮第一组旧版对照 |
| L profile | 提前采样发送、恢复预算起点修正 | 08cfc57ef71686ba… | 输入与深恢复诊断 |
| M release | L 的生产改动，加文字图像重采样优化 | 535c358231dc21ad… | 第一组新版；生产像素对照；smoke 与网络响应观察 |
| M profile | M 的诊断构建 | 3bb69fa3a5ae7338… | 相邻配对成本诊断的 A 页 |
| N release | M 加连续实际输入不足时的渐进 pacing | 17b20ffcb71be9bb…，2,821,344 B | 带宿主采样正式组的对照 |
| P 初版 profile | N 加分页保存弹幕完整 AnmVm | afe4ff5664e5341f…，3,136,549 B | 1,800 帧世界/round-reset 门槛 |
| P 最终 profile | 分页实现与明确的 DEV 检查范围 | 253adae67aad5882…，3,136,549 B | 最终深 8 帧恢复与配对成本诊断 |
| P 最终 release | 生产构建排除 DEV 范围扫描 | de1760ac76f3aaa8…，2,826,832 B | 带宿主采样正式组、生产 7,200 帧像素对照与网络响应观察 |

### 2.1 提前采样发送与恢复预算

生产 rollback_tick 在收包并确认 Ready 后、开始 Restore 前，检查当前未捕获的前沿输入并采样、Capture、发送。循环内保留同一辅助函数和 once-only 条件；重演不会重复消耗 pulse、auto-fire 或 DirectTouch 输入。跨 callback 恢复时，采样依据当前可捕获的前沿，而不是把正在倒退重演的世界当成新的本地输入帧。

预算从包括 Pump 的恢复入口开始计算，仍允许至少一个不可分割的逻辑 tick。这样可以限制一次 callback 连续恢复的工作量，但单个 tick 本身过慢时仍会越过预算。这个改动没有减少正常前进所需的游戏工作，也不能单独解释或解决约 31 Hz 的普通吞吐。

实现位置：[Application.cpp](../../th09_web/cpp/sdl/Application.cpp)、[RollbackSession.cpp](../../th09_web/cpp/multiplayer/RollbackSession.cpp)。输入合同由 session 测试、900 帧真实输入生产器测试和单调前沿驱动的深 8 帧恢复测试分别覆盖。

### 2.2 M：文字图像重采样与临时存储复用

M 将重采样权重改为共享的不可变对象，缓存命中时不再复制嵌套 vector。一次调用使用的横向和纵向权重各保留有效引用，缓存淘汰不会让正在使用的权重悬空。常用、不超过 4 MiB 的 float 临时缓冲复用；更大的图像使用局部分配 fallback，避免长期保留巨型缓冲。FontDevice 复用 scratch，但每次使用仍完整清零，保留旧的透明背景和字形边缘行为。

该路径已通过 390 项精确组件检查，以及 M release 的 7,200 帧、73 个逐像素对照点。它减少权重复制和分配，不减少文字、画面或音频内容；这些正确性检查本身不能证明整场吞吐提升。

实现位置：[ImageResample.cpp](../../th09_web/cpp/game/ImageResample.cpp)、[FontDevice.cpp](../../th09_web/cpp/sdl/FontDevice.cpp)。组件报告和 M 完整渲染报告分别保存自己的 fixture 与构建身份。

### 2.3 N：只依据连续实际输入缺口制动

设 F=max(next,replayEnd)，A 为已经收到的连续远端实际输入的 exclusive frontier，B=max(0,F−A)。使用 F 可避免恢复时 next 倒退，把实际预测余量误判为已经恢复；使用连续 A 可避免乱序收到较晚帧时忽略前面的输入洞。

收到数据并交给 SessionChannel 后更新 pacing。在 Ready、rollback 模式、expectsInput 都满足时，B 超过 4 开始渐慢，B 达到 7 时目标 intervalScale 为 2。scale 每 50 ms 最多上升 1，每 150 ms 最多下降 1；清理/重置回到 1。重复或倒退时钟不会凭空增加渐变进度。只有网络 scale 大于 1 且处于 Ready/rollback 时，最终间隔取它与原 SessionChannel phase 的较大值；否则完整保留原 phase。因此网络 scale 已回到 1 时，原 phase 仍可以略小于 1。

这里不使用 callback 耗时作为降速目标；CPU 慢但连续实际输入已经齐全时，不会仅因为 CPU 慢而触发这层制动。不过，慢 peer 本身也可能推迟采样、产生和发送输入。native 看到的实际输入不足无法单凭这个数值区分物理网络慢、远端设备慢、浏览器投递慢或 native 队列之前的延迟；没有 CPU 耗时输入，不代表现实中的缺口只来自网络。

8 帧是有限预算。N 能平滑接近边界的过程，不能在没有新实际输入时无限推进；对远超预测余量的长断流，仍可能停在边界等待。最终 P 与 M 的 wall-clock 网络 burst / 输入响应观察已完成并列于第 9 节；P1 在 burst 前后多数时间已处于约 2 倍间隔，不能据此宣布“扰动后恢复正常速率”或“已经消除网络停顿”。用户目标仍未验收通过。

实现与测试：[RollbackSession.hpp](../../th09_web/cpp/multiplayer/RollbackSession.hpp)、[RollbackSession.cpp](../../th09_web/cpp/multiplayer/RollbackSession.cpp)、[session.cpp](../../th09_web/tests/multiplayer/session.cpp)。

### 2.4 P：完整 VM 的首次写入分页保存

P 对弹幕 AnmVm 保存完整 676 字节 wasm32 对象，使用固定 slot×5+part 身份。每页 64 个 VM，即 43,264 字节；物理历史 record 复用已经分配的页。整槽保存和单 part 保存共用首次写入位图，防止相同 VM 在同一 checkpoint 重复捕获。

每个 speculative tick 仍有独立 WorldState，未改为三帧 checkpoint。journal 与 VM 捕获共同受 32 MiB/8,192 项预算约束，保留页面容量另记。恢复顺序是 journal Undo 成功后恢复 VM，再执行 owning undo 和动态 Enemy/Effect/Attack 恢复；没有把整个 WorldState 或 unique_ptr 生命周期视为 POD 来跨 checkpoint 覆盖。

Seal、下一次 Save 和 Restore 都会解除 live observers。Application 在 tick 后 Seal，再处理 Complete。因此已结束 checkpoint 保留的页面容量不等于后续普通帧仍在进行首次写入捕获。

生产构建原先没有定义 NDEBUG。最终版本明确结合 TH09_DEVELOPMENT_HARNESS=0 排除 DEV 范围扫描，避免把诊断检查带入 release。profile 仍保留检查，其时间不能直接当作 release 成本。

实现位置：[WorldState.hpp](../../th09_web/cpp/multiplayer/WorldState.hpp)、[WorldState.cpp](../../th09_web/cpp/multiplayer/WorldState.cpp)。

## 3. 已完成的正确性检查

以下“通过”只对应列出的范围。不同构建、受控输入传输、正式 release 公共 exports 和实际 wall-clock relay 没有混为同一种测试。

| 检查 | 实际构建与范围 | 已观察结果 | 不能据此推出 |
| --- | --- | --- | --- |
| L 输入 / touch | L profile；900 帧；两个实际世界与准确输入参考；原生输入生产器；固定迭代传输 | hash=646215930，48 项 audit、PNG、7,222 B Replay 一致；900 次输入捕获；DirectTouch 事件覆盖 | 真实手机触控或实际网络输入到屏幕延迟 |
| L 深 8 帧恢复 | L profile；1,800 帧；单调 prediction frontier 控制交付；D=0 | 每端 1,799 次纠错，约 14,361/14,362 重演帧；每端 1,620 次实际请求深度 8；跨 callback 恢复 1,072/1,045 次；三世界 hash/audit/PNG 与 Replay 通过 | wall-clock relay 吞吐；本测试未覆盖暂停/结算 |
| 图像重采样 | 独立组件，固定 fixture | 390 项检查通过，涵盖格式/尺寸、边界、复用、随机、原地及无效输入 | 完整游戏时延收益 |
| M 生产完整渲染 | J release→M release；7,200 帧；公共 spectator feed/tick exports | 73 个检查点 network hash/session/PNG 一致，观察 phase 1 与 3，跨暂停和结算 | 有实际网络回滚时的流畅性 |
| N / 最终 session | 最新独立组件与自身源码哈希 | 乱序洞、连续实际输入、深重演、时钟/恢复与 once-only 输入合同通过 | 真网络 burst 已经验收通过 |
| 最终 FrameSchedule | 最新独立组件与自身 fixture 身份 | 组件通过 | 设备在 4× 下有足够算力 |
| P 世界/round-reset | P 初版 profile；稀疏路径；1,800 帧、270 checkpoint | pool、bullet、recording 边界通过；覆盖 VM 跨页、Draw 首写及复用 | 最终 release 的时间结论 |
| P 最终深 8 帧恢复 | P 最终 profile；与 L 相同单调前沿 workload | 每端 1,799 次纠错，14,365/14,362 重演帧；每端 1,620 次请求深度 8；跨 callback 1,320/1,311 次；hash=3545558762，48 项 audit、三世界 PNG、1,985 B Replay 通过；每前沿恰一次捕获 | production 4× 实时表现 |
| P 生产完整渲染 | M release→P 最终 release；7,200 帧；公共 exports | 73 点 hash/session/PNG 以及全部 17 个 SDL 计数逐点相同；final hash=763548183；phase 1 与 3；最终 PNG SHA 为 bb5311f852829c722ad560d11fe242f5bed7220bdae6ca80be6e5fcd9f47f4b9 | 真实输入响应、回滚开销或吞吐收益 |
| P 相邻配对诊断 | M profile→P 最终 profile；40 个普通区块、200 个 checkpoint 区块，各 8 tick | 240 个跨构建 network hash/48 项 audit/完整 PNG gate 与 200 个各自恢复 gate 全部通过；最终前沿 2,100 | release VM 性能回归或改善；真实每帧 checkpoint 的实时吞吐 |
| 同构建页面控制 | M/M；各 40 个普通区块，分别测试原页面条件与显式激活 | 两次所有跨页 hash/audit/PNG 通过；显式激活的 126 条入口观测均 visible/focus | 唯一确定原页面偏差原因，或排除所有宿主竞争 |
| P 显式激活配对 | M/P；40 个普通区块、80 个 checkpoint 区块 | 120 次跨构建 forward、80 次跨构建 resim、两页各 80 次完整 restore gate 通过；286 条入口观测均 visible/focus | 正式 release 改善；该轮仍记录到编译竞争 |
| P / M 网络诊断 | 分别 1,800 帧；D=0；wall-clock；真实 relay；仅 P2 CPU4；全音频和诊断开启 | 各自双端 15 点 confirmed hash 对齐；响应、rAF、网络 trace valid/exercised；实际 burst 覆盖 | 跨 run 同逻辑帧 tape，或因果性能 A/B 与用户流畅性验收 |

上述原始报告均在 [证据 JSON](rollback-responsive-evidence.json) 中有完整路径、摘要及 SHA。latest session、FrameSchedule、image 报告按自身 WASM 与 fixture 身份记录，不会把它们写成历史 J/M 冻结构建的测试。

### 保留的两次覆盖门槛失败

rollback4x-responsive-input-deep8-01-failure.json 与 -02-failure.json 原样保留。第一版门槛要求大量“单 callback 内完整恢复 8 帧并前进”，与预算允许跨 callback 的正确行为冲突，一端仅 85 次。第二版改为实际请求恢复深度，但交付时钟仍依赖可倒退的世界帧，一端仅 98 次，仍未达到大于 100 的覆盖要求。

最终 frontier 测试使用单调预测前沿驱动交付，并检查真实请求恢复深度。两次早期失败都在后续 reference gate 前结束；应称“覆盖条件未满足”，不能称“发现的世界不一致已修好”，也不能删掉这两份记录。

## 4. 第一组 J/M 正式 release ABBA

报告：rollback4x-responsive-release-a1/b1/b2/a2-report.json。顺序 J/M/M/J；每次相同 1,800 帧 wire tape、真实 relay、发送注入 39±5 ms、仅 P2 CPU4、音频开启、无 V8 profiler。冻结 harness 为 3dcd9d7f6aad719c6aca3d1388d5655cd24eb36e51aed1e20179d152bd9252a4。

这一组没有宿主 CPU/进程采样，也没有新的 rAF enter/exit 诊断。性能收益结论为 **unconfirmed**；不能单凭 B 较慢认定 M 回退，也没有证据把所有差异归为噪声。

下表都是 P2 原始正式测量窗口的结果，未删除异常值。

| P2 指标 | J A1 | M B1 | M B2 | J A2 |
| --- | ---: | ---: | ---: | ---: |
| 逻辑吞吐 Hz | 40.747 | 26.175 | 22.706 | 25.623 |
| callback p50 / p99 ms | 5.345 / 45.485 | 16.425 / 58.715 | 20.610 / 60.330 | 19.800 / 54.385 |
| callback max ms | 103.555 | 119.160 | 363.995 | 80.205 |
| Present 间隔 p50 / p99 ms | 18.325 / 85.175 | 36.365 / 103.040 | 42.585 / 127.370 | 36.550 / 96.975 |
| Present 间隔 max ms | 1,526.875 | 624.145 | 732.855 | 151.660 |
| Save 增量 | 1,309 | 477 | 306 | 170 |
| 纠错 / 重演帧 | 103 / 355 | 39 / 122 | 21 / 60 | 12 / 34 |

每个 peer 的前 1,800 帧实际 wire 输入值在四次运行中完全相同。每 120 帧一次，共 15 个确认检查点，在四次运行、两个 peer 间完全一致。实时结束时各 peer 停在不同帧，最终 hash 不作错误的跨帧比较。

普通前进的时间也发生变化。在四次运行均满足“同一逻辑帧、仅前进 1 帧、无纠错/重演/Save”的 152 个 P2 匹配帧上，完整 callback 平均耗时分别为 16.545、26.196、24.695、20.291 ms。P1 没有四次都符合条件的共同帧，汇总明确标为不可用。这个分类保留了 Draw、音频与整个 callback 成本，不能理解成单独的游戏逻辑或 Save 时间。

SDL 测量计数接近：calls 约 79,871–80,182，batches 49,181–49,376，buffer replacements 7,252–7,284。计数不支持“新版突然多画了大量内容”这一解释，但也不是 GPU 时间测量。Save 与重演在较慢运行反而更少，说明需要分析普通 callback 和运行环境，而不是只统计恢复次数。

P2 音频产生块数为 562/813/931/825，非零块 538/789/921/817。更长的实际运行时间需要继续产生 PCM，不能把这些块数差异当作音频功能变化或单位块成本。非零探针只证明测量期间有音频数据，不等于已经做了听感或 underrun 验收。

## 5. 第二组 N/P 正式 release ABBA：带宿主采样

报告：rollback4x-responsive-vm-host-a1/b1/b2/a2-report.json。顺序 N/P/P/N；四次冻结 harness 都是 42e6ceae2ed5211e4ea4a979a1996523c2370cd45659c011139193b0a2352e62。HOST_LOAD=1、RAF_DIAGNOSTICS=0，其余正式参数仍为 1,800 帧 tape、39±5 ms 发送延迟、P2 CPU4、release、音频开启、无 profiler；没有启用 burst 或 wall-clock response intents。

新汇总对四次构建身份、参数、harness/relay/shell/audio/host probe、双端 1,800 帧 wire 输入以及 15 次确认 hash 做严格一致性校验，全部通过。**这一组与上一组的 harness、flags 和候选关系不同，不合并为一个性能估计。**

| P2 指标 | N A1 | P B1 | P B2 | N A2 |
| --- | ---: | ---: | ---: | ---: |
| 逻辑吞吐 Hz | 26.133 | 20.434 | 32.045 | 28.669 |
| callback p50 / p99 ms | 18.350 / 54.140 | 24.085 / 58.830 | 7.420 / 51.125 | 15.650 / 46.295 |
| callback max ms | 105.885 | 116.035 | 116.015 | 95.445 |
| 有效新前沿 Present 间隔 p50 / p99 ms | 36.490 / 90.810 | 48.495 / 109.255 | 24.280 / 109.060 | 30.615 / 80.515 |
| 有效间隔 max ms | 157.505 | 133.295 | 3,738.750 | 805.980 |
| 有效间隔 >100 / >250 ms 次数 | 7 / 0 | 40 / 0 | 21 / 3 | 8 / 1 |
| Save 增量 | 214 | 150 | 1,286 | 373 |
| 纠错 / 重演帧 | 18 / 43 | 11 / 22 | 111 / 381 | 26 / 80 |

“有效新前沿 Present”要求渲染提交计数改变且逻辑帧超过上一已计入的前沿；它比只数 Present 更接近玩家持续看到新状态的需求，仍不是屏幕 scanout 时间或逐帧视觉差异判定。B2 的 3.739 s 和 A2 的 805.98 ms 全部保留；本组没有逐 request / frame hook 的 rAF 诊断，不能直接套用后文 M smoke 的定位结论。

四次共有的 157 个普通 P2 前进帧中，完整 callback 均值为 25.099/27.275/17.571/19.522 ms。P1 仍没有共同匹配集合。P 的 B1 中，1,335 个回调满足 Δframe=1 且 Δcorrection=Δresim=ΔSave=0：p50 25.710 ms、p99 58.965 ms，58 个超过 50 ms；全测区超过 50 ms 的回调共 59 个。结合 Seal/Unbind 生命周期，这些普通回调没有遗留 checkpoint VM 首写。这一事实限制了“仅通过优化 VM 捕获就解释全部长帧”的范围，不排除普通 Draw、音频、调度或其他工作，也不归因于某个特定宿主任务。

### 5.1 已记录的宿主竞争

以下是**完整 host 观察窗口**，包括 setup、正式轮询和结果采集，不能称为纯 180–1,799 逻辑帧窗口。JSON 另保留 marks 及相邻 marks 间完全包含的 CPU 采样区间；跨边界区间仍保留在全窗口。

| 宿主指标 | N A1 | P B1 | P B2 | N A2 |
| --- | ---: | ---: | ---: | ---: |
| aggregate busy p50 % | 35.891 | 37.865 | 82.598 | 50.959 |
| aggregate busy p95 % | 49.060 | 83.028 | 97.890 | 73.073 |
| aggregate busy max % | 73.006 | 100.000 | 99.621 | 100.000 |
| CPU 样本数（含首次） | 84 | 114 | 79 | 83 |
| 进程快照数 | 17 | 23 | 16 | 17 |
| 被采到的编译进程 | 未观察到 | wasm-opt | clang、g++、cc1plus | clang、clang++、wasm-opt |

B1 连续快照中相同 PID/name 的 wasm-opt 非负 CPU 计数差累计 43.03125 CPU 秒；A2 对应 wasm-opt 为 67.890625 CPU 秒。B2 的短命编译进程只在首次出现时被采到，没有合格的连续差分，CPU 增量不可量化；JSON 使用 null，而不是把它们记作零 CPU。

这些是实际竞争进程的证据，足以说明运行环境存在混杂。它们不能定位哪一次停顿，也不能证明全部差值来自编译。A1 没有采到编译器同样不能证明没有短任务、单核争抢、频率变化或 GPU 竞争。

host.valid 和 complete 均表示采样完整性。探针约每 1 s 采 aggregate CPU、每 5 s 采候选进程 Id/Name/CPU；首次值是进程生命周期累计值，不计入本次工作量，跨缺失快照和负计数差不累计。5 s 间隔会漏掉已经退出的短任务；没有进程创建时间时也不能彻底排除 PID 复用。aggregate 利用率不能证明浏览器所需的某一 CPU 核、频率、温控或 GPU 状态稳定。所有游戏样本均保留，没有按 host.valid、负载或编译器是否出现筛掉异常。

因此，本组 P 的性能收益仍为 **unconfirmed**。分阶段配对和同构建控制用于增加定位证据，实际响应测试用于检查玩家体验，不能只对四个总分取平均后宣布成功或失败。

## 6. 已定位到的 M smoke 长停顿

报告：[rollback4x-responsive-probe-smoke-01-report.json](../../th09_web/artifacts/multiplayer-tests/rollback4x-responsive-probe-smoke-01-report.json)。这是冻结 M release、CPU1、900 帧、wall-clock 合成输入、真实 relay 的响应性 smoke，harness 为 9114b424bcdac93bb0341519a383e6bfc1a47acb4916b428cc6c5f53d6c571cb，启用 rAF 诊断，未记录宿主负载。不能将其当作 N/P 正式 4× 测试。

| 观察 | P1 | P2 |
| --- | ---: | ---: |
| 两次新提交的 sample 索引 | 1406→1662 | 1420→1668 |
| 逻辑帧 | 424→425 | 425→426 |
| 提交所对应的 rAF timestamp 间隔 | 1,672.815 ms | 1,640.500 ms |
| 区间内 game rAF / frame hook 次数 | 257 / 257 | 249 / 249 |
| 内部稳定等待样本数 | 254 | 246 |
| 稳定等待内 C++ 总耗时 / 单次最大值 | 29.325 / 1.670 ms | 31.020 / 1.540 ms |

稳定等待期间 ok=2，frame、确认边界、Save、correction 和 resim 计数不变；pause=0，输入 timer 持续，实际 rAF entry 间隔最大约 12.4 ms。此前另有 P1 request 1429、P2 request 1434 的 394.765/393.850 ms 请求到进入延迟。这两段是不同现象，不能合并成“一次 1.6 秒 C++ 或 rAF 冻结”。

P1 等待 remote 416，P2 等待 remote 417；对应输入已经在对端 wire hook 捕获，因此不是“双方都等对方，才产生下一输入”的逻辑死锁。发送注入记录的最长完成延迟为 453.580/439.715 ms，记录点是 WS.send 返回之后，不能当作远端实际送达时间。

源码对照显示 SharedNetplay 常规 pump 为 16 ms，等待前仍 Pump；常规 resend 32 ms、TH09 repair 80 ms，其中 repair 仅适用于 RTC，relay 使用可靠 WebSocket，repair=0 属于预期。relay 配置自身 delay/jitter/drop 为 0，正常路径立即 socket.send；现有路径没有 1,500/1,600 ms 的修复等待计时器。

目前只能把缺口限定在 **对端 wire 捕获至本端应用输入之间**。旧报告没有逐包 sequence/ACK/fingerprint 关联，无法为具体缺帧包继续细分；WS.send 返回只是已有发送记录的边界，不证明对应缺帧包已经发送或送达。全局 send-delay 最大值也不能替代这个逐包关联。缺少 relay 收发、浏览器 onmessage 与 JS/native 队列时间戳，不能进一步区分发送之前、relay event loop、浏览器网络服务、消息投递或 native 队列。独立 host 探针自测的 5.511 s 窗口不与本 smoke 对齐，不能用来解释或排除这个停顿。后续新 NETWORK_TRACE 属于新增诊断条件，可以分析新运行，不能回填旧运行的具体包路径。

该 smoke 两端各有 80 次计划/提交的合成输入，其中实际捕获并呈现 70/71 次，submitted-not-captured 为 10/9，captured-not-presented 为 0。报告同时保留这些未完成/被后续状态取代的计数，不能只用条件延迟百分位掩盖未捕获意图。它们也不是硬件按键或手机触摸的 input-to-photon 测量。

## 7. 时间、音频与性能计数的解释边界

旧 14 字段 samples 继续保留。sample[0]+started 的采样发生在 play.frame 返回之后、输入更新与统计采集之前。历史名称 callbackCompletionGapMs 实际是这个观察点之间的间隔，并非 onGameFrame 的真正退出时间。rAF timestamp 则由浏览器给出，两者不能按同一 sample 索引直接归因给 C++。

例如旧 J A1 的最大 rAF gap 在 samples[1789→1790]，为 1,526.875 ms；最大旧观察点 gap 在 [1788→1789]，为 1,533.120 ms。对应 frame 和每次 C++ 时长都保留在 JSON，不能据时间戳归属差异把整段停顿算给后一帧。

新的 opt-in rAF 记录区分 request、实际 enter/exit、onGameFrame enter/exit、play enter/exit、sampleAt 与 afterSampleAt。只有启用该诊断的报告具有这些证据。正式 HOST 四组明确未启用；M smoke 与新 P/M 网络 burst 已启用。网络 burst 的新 harness、probe 身份和 wall-clock 条件独立保存，不能回填旧正式组。

渲染 calls/batches/buffer replacements、Save 次数、VM 页面容量是工作量或资源计数，不是 CPU 提速比例。计数减少可以说明提交结构改变；是否让普通前进或纠错尾延迟变好，需要实际分阶段时间和有效新帧间隔支持。

音频保持开启，OGG/PCM 与 BGM/SFX 功能检查按原测试保留。当前的块数、非零块与 cursor/PCM 对照不能替代输出连续性和设备 underrun 观测；不把关闭音频或减少音频功能作为流畅性验收方式。

## 8. 配对成本诊断与页面控制

### 8.1 原 M/P 与同构建 M/M：先识别测量偏差

报告：[原 M/P 配对](../../th09_web/artifacts/multiplayer-tests/rollback4x-responsive-vm-paired-4x-01-report.json)。使用同一浏览器的两个页面，M profile 与 P 最终 profile，AB/BA 相邻配对，两页均请求 CPU4，附宿主采样。冻结 harness 为 7fbdc9e3a231c51ac0b957c9d972e99e035ac57b0d10dbbb8d1e59ac5bd64aec。40 个普通区块和 200 个 checkpoint 区块全部保留，跨构建与各自恢复 oracle 通过，performanceGate 为 null。

以下均为每 8 tick 区块的毫秒数。配对差值为逐对计算 B−A 后求 p50，不是两列 p50 相减。

| 阶段 | M A p50 | P B p50 | 配对 B−A p50 | 范围 |
| --- | ---: | ---: | ---: | --- |
| baseEightTicks | 21.900 | 42.725 | 23.030 | 40 个无 checkpoint 普通区块 |
| capture | 4.005 | 6.595 | 2.600 | 200 次 Save |
| forwardEightTicks | 20.255 | 40.100 | 18.605 | Save 后、checkpoint 有效的原始前进 |
| restore | 1.115 | 4.150 | 2.730 | 200 次恢复 |
| resimEightTicks | 14.900 | 40.295 | 26.380 | 恢复后、checkpoint 已解除的重演 |
| correction | 22.840 | 51.480 | 31.840 | capture+restore+resim |
| totalMeasured | 47.245 | 92.800 | 51.730 | capture+forward+restore+resim |

**P 的 VM 首次写入发生在 Save 后的原始 forward 中，restore 后的 resim 不再持有该 checkpoint。** correction 本身没有包含这部分主要工作，所以必须同时报告 forward 和 totalMeasured。本轮 P 在 base 与 resim 也较慢，原结果没有隔离 VM 捕获成本。

随后完成的 [原 M/M 控制](../../th09_web/artifacts/multiplayer-tests/rollback4x-responsive-paired-control-mm-01-report.json) 两页加载完全相同的 M WASM、loader、manifest，PC_CHECKPOINT_BLOCKS=0，40 个普通区块全部通过跨页 hash/audit/PNG。冻结 harness 为 70678067bca5f7ab98611351a4623c273f8db9318469d662b74aa0b6e15f9c93。A/B base p50 为 18.860/43.580 ms，合计 916.305/1,774.965 ms，配对差值 p50 为 +21.590 ms。相同代码也出现近似的页面差异，证明原方法存在具体混杂，不能把原 M/P 的约 2 倍差异直接归于分页实现。两份原始数据均保留。

### 8.2 显式激活页面后的控制与 M/P80

后两次使用新的冻结 harness a95f3fcc32ebd5cdeb8f3d103f67311cd88aaaa400a493aac29dc4f6d4e89457，PC_ACTIVATE_PAGE=1，每个区块记录 visibility/focus。它们与前两次是不同条件，不覆盖旧数据，也不按前台状态过滤样本。

[激活页面的 M/M40 控制](../../th09_web/artifacts/multiplayer-tests/rollback4x-responsive-paired-control-mm-active-01-report.json) 的 23 个预热区块与 40 个测量区块、两页合计 126 条入口观测全部 visible 且 hasFocus。A/B base p50 为 32.345/34.510 ms，总计 1,388.515/1,408.330 ms，合计差 +1.43%，配对差值 p50 +2.065 ms。AB 总差 +55.470 ms，BA −35.655 ms。原先接近 2 倍的偏差显著缩小，但单块波动仍大，不能确定页面激活是唯一原因。

[激活页面的 M/P80](../../th09_web/artifacts/multiplayer-tests/rollback4x-responsive-vm-paired-active-4x-01-report.json) 使用 40 个普通区块、80 个 checkpoint 区块。120 次跨构建 forward、80 次跨构建 resim、两页各 80 次完整 restore gate 均通过；286 条入口观测全部 visible/focus。两页 WASM heap 始终为 128 MiB、Intel UHD / D3D11 renderer 相同，80 次初始 captureBytes 逐块相等。

| 阶段 | M A 总计 ms | P B 总计 ms | M A p50 ms | P B p50 ms | 配对 B−A p50 ms |
| --- | ---: | ---: | ---: | ---: | ---: |
| baseEightTicks，40 对 | 973.195 | 1,067.020 | 17.355 | 18.555 | +0.115 |
| capture，80 对 | 299.040 | 326.510 | 1.960 | 2.485 | +0.070 |
| forwardEightTicks | 1,596.245 | 1,743.640 | 16.605 | 19.825 | +1.995 |
| restore | 256.855 | 228.675 | 2.510 | 2.295 | +0.085 |
| resimEightTicks | 1,597.225 | 1,756.040 | 18.050 | 18.910 | +1.440 |
| correction | 2,153.120 | 2,311.225 | 22.520 | 26.455 | +2.980 |
| totalMeasured | 3,749.365 | 4,054.865 | 43.485 | 46.235 | +6.650 |

totalMeasured 总量 +8.15%，AB 与 BA 的总差分别为 +145.935/+159.565 ms，**本轮没有证实正向总体成本收益**。同时，总量不能读成稳定的逐块效果：base 总差 +93.825 ms 中，block 7 / frame 236 单块贡献 +97.585 ms，base 配对 p50 仅 +0.115 ms；restore 总差 −28.180 ms 中，block 69 / frame 732 贡献 −26.975 ms（A 29.440、B 2.465 ms），restore 配对 p50 实际为 +0.085 ms。不能称普通每块都慢约 10%，也不能称恢复已稳定提速。所有这些区块仍参与完整分布，未删异常值。

宿主采样同样保留。按 paired-base-start、paired-checkpoint-start、paired-workload-complete 标记分段，CPU 只统计完整落在阶段内的采样区间；进程只使用 sourceUtcMs 落在阶段内、原始相邻且成功的同 PID/name 非负差分，不计首见 lifetime 值和跨界差分：

| 宿主阶段 | 完整 CPU 区间数 / 加权 busy | 已观察编译进程的 CPU 差分 |
| --- | --- | --- |
| base，约 48.019 s | 47 / 42.485%，24 逻辑 CPU | wasm-opt PID 60284：7 点、6 差分，+128.09375 CPU s；wasm-ld PID 31880：+4.84375 CPU s |
| checkpoint，约 179.245 s | 176 / 41.403%，24 逻辑 CPU | 同一 wasm-opt PID 60284：35 点、34 差分，155.96875→882.28125，即 +726.3125 CPU s；相邻观测覆盖 175.180 s |

CPU 加权采用完整区间的累计 total/idle 差；阶段包含 oracle、页面激活和 harness 等计时外工作，不能把整阶段 CPU 竞争精确分配到某一个 native timer。上述事实说明 active 控制仍有真实编译竞争，不能把 +8.15% 判为 VM 代码的因果回退，也不能将它包装成优化成功。没有按宿主负载排除任何游戏区块，也没有用 base 比值“校正”其他阶段。

### 8.3 配对诊断的适用范围

每 8 帧一个 checkpoint 与实时每 speculative tick 独立保存不同，profile 还保留 DEV 重叠检查。_th09_probe_rollback_step 临时设置 recording_frame，抑制 tick_inputs 的 audio.update/pump，也不走实际 rollback_tick 末尾的确认音频发布。因此 base/resim **不包含真实 AudioDevice::pump**，不能用这组结果排除 live 普通前进的音频成本。

世界和 PNG 检查位于计时外，截图读回仍可能影响随后 GPU 状态；performance.now 是 elapsed 时间，不是进程 CPU 秒。证据 JSON 保存全部 phase、AB/BA、极端区块、内存/入口状态与阶段宿主摘要，验证每个 timer 的起止、差值、顺序和 oracle。正确性通过不等于性能目标通过。

## 9. 真实网络 burst、输入响应与启动段

### 9.1 完成的 P/M 观察与正式区间

最终 P 和 M 的两份报告均已完成：[P 网络 burst](../../th09_web/artifacts/multiplayer-tests/rollback4x-responsive-vm-network-burst-01-report.json)、[M 网络 burst](../../th09_web/artifacts/multiplayer-tests/rollback4x-responsive-text-network-burst-01-report.json)。每次 1,800 帧，wall-clock 合成输入，真实 relay fallback、仅 P2 CPU4、音频、response/rAF/HOST/NETWORK_TRACE 全部开启。客户端发送注入 39±5 ms，各页 active age=20,000–20,500 ms 额外延迟 120 ms，无配置丢包。

共同 harness 为 6137632e9b273512a529f2f027a00fa02e1a880f7d274db6bad68c7996df6398，network probe 为 090fe224bf5642b3daf4aaf354b67287ba5ad0b39a9d4f336cc1abe6603578c3。两次各自双端 15 次 confirmed hash 对齐，response/rAF/trace valid，trace exercised 且无 overflow。实际 burst 包数为 P 两端 23/23，M 31/32。

wall-clock 输入依赖运行时钟，实际落入的逻辑帧和墙钟长度不同。这是相同配置的两次诊断观察，**不是同一逻辑帧 tape 的因果 A/B**，不能跨 run 比较最终 hash 或合并成同一性能估计。

| P2 正式 frame 180–1,799 | P | M |
| --- | ---: | ---: |
| logic Hz | 20.253 | 35.824 |
| callback p99 / max ms | 54.510 / 95.870 | 44.045 / 60.095 |
| 有效新前沿 Present 间隔 p99 / max ms | 109.015 / 224.270 | 84.800 / 145.575 |
| 有效间隔 >100 ms / >250 ms 次数 | 34 / 0 | 4 / 0 |
| P1 正式最大有效间隔 ms | 175.605 | 127.390 |

两次正式窗口均没有复现旧的秒级停顿，但这不证明旧问题已修复，也不代表整个启动过程没有大间隔。当前最终 P 4× 的新画面连续性和响应仍未达到用户目标；复杂宿主环境不能把这个现状改写为已流畅，也不能单凭这些非匹配运行确定代码回退。

### 9.2 响应覆盖和条件延迟

全部原始 intents 都参与覆盖重算。条件延迟只包含实际到达对应观察点、且不是 same-mask resubmission 的意图；未提交、提交后被下一状态取代、未呈现等计数单独保留，不用条件分位数掩盖它们。

| 运行 / 端点 | 计划 / 提交 / 捕获 / 呈现 | 未提交 | 已提交未捕获 | same-mask |
| --- | --- | ---: | ---: | ---: |
| P / P1 | 356 / 356 / 344 / 344 | 0 | 12 | 0 |
| P / P2 | 356 / 326 / 326 / 326 | 30 | 0 | 1 |
| M / P1 | 209 / 209 / 207 / 207 | 0 | 2 | 0 |
| M / P2 | 209 / 201 / 201 / 201 | 8 | 0 | 0 |

四端均无“已捕获但未呈现”意图，captureGap、inputMismatch、changedHistory、observationErrors 均为 0。P/P2 的 1 次 same-mask 保留在覆盖计数中，仅从条件分布排除。各项的全部 p50/p95/p99/max、样本数、初始意图和未完成意图在证据 JSON 中。

| 条件延迟 ms，p50 / p99 / max | P / P1 | P / P2 | M / P1 | M / P2 |
| --- | --- | --- | --- | --- |
| schedule→提交 | 9.405 / 20.595 / 25.215 | 61.810 / 147.790 / 162.270 | 8.680 / 20.985 / 26.770 | 42.930 / 126.495 / 149.790 |
| 提交→首次 wire | 23.835 / 102.435 / 145.240 | 16.390 / 63.145 / 170.955 | 15.275 / 102.710 / 205.435 | 10.400 / 44.165 / 77.370 |
| 首次 wire→新前沿呈现观察 | 13.720 / 91.610 / 147.065 | 23.135 / 65.315 / 317.515 | 7.960 / 91.315 / 103.630 | 16.060 / 55.315 / 269.490 |
| schedule→新前沿呈现观察 | 50.535 / 173.910 / 193.960 | 101.205 / 235.340 / 499.070 | 42.795 / 169.295 / 243.595 | 71.730 / 200.330 / 358.200 |

schedule→提交包含合成输入 timer 延迟；wire 是捕获/序列化之后的 hook，并非精确按键采样时刻。呈现观察依据 callback 内出现新逻辑前沿与提交，不是硬件 input-to-photon、屏幕扫描输出或远端 ACK。P/P2 和 M/P2 的最大 schedule→呈现分别为 499.070/358.200 ms，均来自初始 intent 0 / frame 0；启动意图保留在延迟分布。

### 9.3 启动长回调与逐包边界

新诊断将 rAF timestamp 与真实 onGameFrame entry 分开，二者不能混用。以下启动段均保留：

| 运行 / 端点 / frame | rAF timestamp 差 ms | 旧 now 差 ms | 实际 onGameFrame entry 差 ms |
| --- | ---: | ---: | ---: |
| P / P1 / 8→9 | 569.425 | 590.270 | 590.140 |
| P / P2 / 0→1 | 145.415 | 502.125 | 515.460 |
| P / P2 / 1→2 | 496.930 | 180.360 | 181.205 |
| M / P1 / 8→9 | 315.025 | 334.600 | 334.615 |
| M / P2 / 0→1 | 102.975 | 363.965 | 377.150 |
| M / P2 / 1→2 | 345.350 | 138.055 | 137.600 |

P2 的初始 sample 0 没有已显示提交，所以 0→1 是启动响应，不能称两个已经显示画面之间的物理间隔。P/P2 首次 C++ 回调为 **433.130 ms**，M/P2 为 **311.310 ms**，M 下一次另有 120.580 ms。这些长回调真实存在，尚未分解到具体 native 子系统。P1 等待 8→9 的期间仍持续有短回调，不能一概称为 rAF 冻结。

按完整 packet 元数据及 occurrence 关联后，首个 P2→P1 输入包的链条如下。时间统一到各次运行的 P1 active 起点，原报告保留本页时钟与跨页对齐信息：

| 运行 | intent ms | 实际 WS.send entry ms | 对端 message entry ms | 正数 poll-copy entry ms |
| --- | ---: | ---: | ---: | ---: |
| P | 233.625 | 768.685 | 771.600 | 772.350 |
| M | 109.055 | 524.880 | 535.190 | 538.295 |

相应发送前间隔为 **535.060/415.825 ms**。当前 39±5 ms 和 burst 都由客户端主线程 setTimeout 注入，所以这些间隔包含 timer 被同一 433/311 ms 长回调推迟的影响；不能直接外推为没有客户端 timer 注入的生产 WS 路径也必然这么晚。只把 APP DELAY 设为 0 仍然走 setTimeout(0)，不等于同步发送控制。长回调、启动画面和响应指标仍是实际观察，不因这个测量边界而删除。

反向 P1→P2 首包从 WS.send 到 message entry 分别为 640.855/299.135 ms。P 全程该段最大 675.425 ms，所有 20 个 >250 ms 包属于最初 seq 1–20；M 相同方向的 >250 ms 包也集中在启动。没有 relay 收发和浏览器网络服务的逐包时间，不能把这段分配给 relay、网络或单一 native 任务。

两个报告都没有中途无法关联的完整键事件，但结束边界存在未完成记录：P 每向各 3 个 intent 未见 send、2 个 send 未见对端 message；M 每向各 6 个 intent 未见 send，P1→P2 另有 2 个、P2→P1 另有 4 个 send 未见 message。它们保留为停止窗口尾部 unresolved，不当作已知丢包，也不删除。message entry→正数 poll-copy 全程最大值为 P 在 P1/P2 的 23.215/51.185 ms，M 为 18.525/40.580 ms；这两次未复现入站 handler 已进入后再等待数秒的情况，不能据此排除旧运行。

完整包键、sequence/ACK、正式最大缺口、burst 各方向链条和时钟限制见 [网络诊断记录](rollback-network-trace-plan.md)。native dequeue 不是 SessionChannel 已接受 ApplyInputPacket；P 的 raw actual 仅在既有 pump 返回或 onGameFrame 末尾观察，M 没有相应 export。新逐包证据不回填第 6 节旧 smoke 缺失的关联。

### 9.4 Pacing 的已观察行为

P1 在 burst 前 19–20 s 已有 outstanding=6–8、network scale=1.827–2.000；burst 期间多为 2.000。20.635–20.765 s 收到实际输入后，actual 从 474 增至 479、scale 短暂从 1.887 降到 1.333，20.800 s 又回到 2.000。P2 的 network scale 在同窗一直为 1.000；20.682 s actual=480、prediction=478、outstanding=0，effective scale 0.983/0.984 来自既有 channel phase。两端 raw actual 全程无回退。

这支持“控制器会响应连续实际输入缺口”的有限结论，**没有证明 burst 结束后恢复正常速率，更没有证明所有减速只由网络造成**。慢 peer 的输入生产、发送前调度与投递仍会反映到 actual 缺口。应同时检查原始前沿、输入响应和新画面间隔，不用重复 Present 或固定慢速证明流畅。

## 10. 下一阶段优先级与停止条件

本轮实现和证据在此冻结，不再追加 benchmark。后续按下面顺序推进，每步保留 D=0、60 Hz 逻辑、Draw 历史副作用、完整 BGM/SFX、暂停/结算和 Replay 合同。

1. **先定位真实普通回调与首帧准备。** 在没有 Save、correction 或 resim 的生产回调中，分别量出 Update、语义 Draw、GPU 提交、确认音频发布、PCM 补充；启动段单列首次资源/音频/渲染准备。产物应把总 callback 与这些阶段对齐，并保留宿主、音频队列/连续性和实际新画面间隔。第 5 节已说明多数长回调可以出现在没有分页首写的普通前进中，配对探针又没有真实 AudioDevice::pump，因此下一项应由 live 占比决定。没有阶段证据之前，不继续扩大快照表示重构；相邻小批次 append 的兼容性比较仅是有限潜力备选，须先证明确有占比。接受改动需要实际减少该成本或尾延迟，不能仅依据 Save/GL 计数。
2. **按已发现的网络边界补足独立条件。** 计划新增“客户端直接同步发送、独立 relay 进程注入 delay/jitter/burst、relay recv/send 逐包时钟”的条件，利用已有 EAGLER_NETPLAY_RELAY_DELAY_MS/JITTER_MS 与 sendBounded 能力，保留当前客户端注入结果。先消除 client timer 与长回调耦合的解释歧义；只有 message→poll-copy 或安全 actual 行显示具体异常时，再补相应 native 边界。不能用 APP DELAY=0 冒充同步发送，也不把缺口归因给尚未观察的环节。这一条件尚未实施或执行。
3. **联合验收玩家体验。** 在记录宿主竞争的 CPU4 生产会话中，同时报告逻辑吞吐、有效新前沿提交的 p50/p95/p99/max、最长无新画面区间、>100/250 ms 次数、全部响应 coverage 与条件延迟、全音频连续性。信息齐全时 N 不应主动制动；短期缺口允许平滑渐慢，但恢复到正常节奏必须有连续 actual 与 scale 回升/回落的实际记录。秒级硬停顿属于失败项，启动段必须独立列出。不能通过固定降低正常速度、重复同一画面、少运行逻辑或关闭音频换取分位数改善。正确性继续使用匹配输入的 hash/audit/PNG/Replay、深恢复和 round-reset 边界，性能收益必须与对应冻结 release 和条件绑定。

同项目成功经验应继续作为方法和目标参照。TH07 历史原始报告 D:/workspace/eagler/th07-eagler/.codex-tmp/zero-responsive-final-cp3/audio-2p-slow4-defaults.json，SHA-256 8467bb164fcc7c5c40c95a0f52a09816ecfaa610339280e08015a93bf6e755c3，记录 3,600 帧 Stage 6 Lunatic、OGG、RTC、P2 CPU4、D=0、3 帧 checkpoint、12 帧窗口：P2 simulation 59.830603 Hz、dense 59.180617 Hz、resim 6,879、Present p99 37.200 ms、max 70.700 ms。它说明无需靠增加输入延迟或关闭慢端回滚才能接近目标，值得比较完整 VM 首写、捕获频率、连续输入前沿和工作分配。TH09 每 tick 的语义 Draw、owning 状态生命周期及双侧战斗工作不同；异游戏、异时环境不是 matched benchmark，不能将 TH07 FPS 或某个快照优化比例直接移给 TH09。该历史参考单独注明来源，不计入本轮 31-source 同条件验收。

## 11. 复现证据汇总

汇总脚本只读取既有 JSON、校验身份和合同并生成证据文件，不构建、不启动浏览器、不跑 benchmark：

~~~sh
node th09_web/tests/multiplayer/summarize-responsive.mjs --include-progress --include-host-abba --include-paired --include-paired-controls --include-paired-active --include-network-burst
~~~

入口：[summarize-responsive.mjs](../../th09_web/tests/multiplayer/summarize-responsive.mjs)。

原始 11 份 required 报告始终必须存在，包含保留的两次 coverage failure。--include-progress 显式要求本轮 10 份构建/正确性/诊断来源；--include-host-abba 要求 N/P/P/N 四份完整报告；--include-paired 要求原 M/P 配对；--include-paired-controls 要求原 M/M 控制；--include-paired-active 要求已完成的激活页面 M/M40 与 M/P80；--include-network-burst 要求 P/M 两份完整网络响应观察。最终证据共 **31 个来源**。

缺文件、改变的 fixture、构建身份/参数不符、失败报告、输入或确认哈希不一致都会报错，不静默跳过。历史 J/M 与 HOST N/P 各自验证双端、每端 1,800 帧 tape 和 15 个 confirmed 检查点；网络 wall-clock 只在各自运行内验证 confirmed hash，不虚构跨 run 同输入关系。完整 phase 和极端区块从所有原始配对重算，host.valid 只代表采样完整性，不用于删除游戏数据。

证据 JSON 保存完整双端 summary、生命周期、分组派生指标、响应覆盖、启动/正式时间端点、阶段宿主与 source manifest。巨量 raw callback/tape/PCM/逐包数组仍在原报告，不复制进 Git。所有异常、结束窗口未配对事件和两次覆盖失败保留；旧 rollback4x evidence 与旧 summarizer 没有覆盖。最终结论保持：所列正确性已通过，性能收益待确认，当前最终 P 的 4× 玩家体验尚未验收。

