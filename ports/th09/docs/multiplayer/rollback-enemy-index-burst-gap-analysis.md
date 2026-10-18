# Burst 正文最大新状态间隙：103.660 ms 的时钟拆解

## 结论与范围

本次 release、P1 1× / P2 4× CPU、双端 D0、独立 relay 的 wall-clock 输入实验，P2 正文最大新状态观测间隙为 **103.660 ms**，位于 samples **4276→4277**。对应浏览器 rAF 时间戳差为 **97.185 ms**，两者保留各自口径。它发生在计划 burst 结束 **5.926–6.030 秒以后**，目标起点也晚于最后一份带 burst 标志 packet 的实际 relay send **5.766 秒**。不能把这次最长间隙直接算作计划注入期间的降速。

这一个区间只有一次 native 游戏调用，耗时 **10.210 ms**；correction、resimulation、wait 均为 **0**。P2 的下一次 rAF request 在父 callback 退出后等待 **93.080 ms** 才进入。P1 同期出现两次长 callback 观测间隙，relay 同期两次同步 send API bracket 分别耗时 **47.692 / 44.955 ms**。证据支持下一步调查多个执行端共同出现的调度 / API 长耗时；它不证明某个进程、浏览器、设备、中继 API 或探针是根因。

这里只解释已有不可变证据，没有修改生产代码、重跑浏览器、构建或新增环境采样。数据正确性、输入成功与音频块检查通过，不是“没有卡顿”的验收。本次 wall-clock burst 工作负载也不构成与旧 burst 或 E1 固定输入带的同条件性能对比。

## 1. 冻结来源与提取完整性

仓库根目录：`D:/workspace/eagler/worktrees/th09-rollback-4x`。下列路径均相对该根目录。

| 对象 | 路径 | 字节 | SHA-256 |
|---|---|---:|---|
| 本次完整聚焦证据 | `docs/multiplayer/rollback-enemy-index-burst-gap-context.json` | 333945 | `5b9322700c77751633c9df5f6bb50b98ae2c1d81f59bb8188f5705d503e1dab8` |
| 原始 burst report | `th09_web/artifacts/multiplayer-tests/rollback4x-enemy-index-burst-response-01-report.json` | 25645547 | `76e8d8e6573e28950aa41da0356f88fc25ee628dac6c971fe9f1d47dc70b9f1e` |
| 已冻结 response 证据 | `docs/multiplayer/rollback-enemy-index-response-evidence.json` | 522445 | `92e35a2466f039c373775bc81a161d6aec1789ab0ac63cf32d26280359f9c906` |
| release manifest | `th09_web/artifacts/rollback4x-enemy-index-release/build.json` | 87045 | `ad19bc3a5f0c4e2bb39a9051d7fa8337a5f4b31748f327de85407829f72b61e9` |
| release Wasm | `th09_web/artifacts/rollback4x-enemy-index-release/th09.wasm` | 2827974 | `766dc277b15d86a97d4bc3b38e2a08a9b52a5786d74c817763bc185216673577` |
| 冻结窗口 helper | `th09_web/tests/multiplayer/smoothness-window.mjs` | 9583 | `9c46c45b275b5b57912a62c58dbc309dfe3eec2bff6b788a2370ce6c214c0546` |
| response analyzer | `th09_web/tests/multiplayer/analyze-enemy-index-response.mjs` | 19122 | `5a07b30936471e3272141d404a5ae777c0b553e814f2a9930688a6dd92e10fb9` |

Harness SHA 为 `dde1d2e8f744b33e2eb56337ada81524083f334cd5b897c1dbb57bbf17a74fec`；loader 为 `4e53b06c7e2a99624bc4234ffde1682a9a18ce7c89be3d2a414b4f0cb3620ee8`；relay 为 `56c44ab7520381859b05d21130da40ad605722b42e49ed37c089d0ecf8bbe90c`。其余已记录 helper 身份保存在 context 的 `identity`，不以当前 Git HEAD 代替实测源码身份。

单次提取命令 `aL5lEqNHrcvurDwdeyATkD8L`，exit 0，657 ms。先读取 response 证据冻结 raw 身份，再验证并读取 raw；所有读取来源的前后 SHA 相同，原文件未改。24 项聚焦检查全部通过，错误数组为空；完整 context 以 create-only 方式保存并完整回读。源检查、全部 unresolved 数组、原始选中行、注入 packet 与时钟资格均保留。完整文件超过 150 KiB，未为满足体积而删除字段；stdout 仅输出小摘要。本说明从已保存 context 的内存副本生成，没有再次读取大 raw。

正文采用冻结 F 窗口：P1 samples **[495,4984)**，P2 **[469,4943)**，两端 F 从 180 跨到 1800。正文成本按原调用连续区间；新状态间隙使用 `formalWithEndBoundary` 闭合到终点。未使用会漏掉回卷片段的旧 next-cursor 过滤。

## 2. P2 目标内的完整调用与 request

所有本节时间为 **P2 Page performance.now()，单位 ms**。`sampledAt` 是 native 调用后观测新状态的时点；rAF 时间戳不是 callback 的实际进入时间，renderer submission 也不是物理显示扫描。

| 字段 | sample 4276 | sample 4277 |
|---|---:|---:|
| sampledAt | 30264.015 | 30367.675 |
| 浏览器 rAF 时间戳 | 30259.390 | 30356.575 |
| rAF request / parent | 4291 / 4290 | 4292 / 4291 |
| request requestedAt | 30260.570 | 30264.025 |
| request enteredAt | 30260.980 | 30357.270 |
| request exitedAt | 30264.190 | 30367.700 |
| native 耗时 | 3.025 | 10.210 |
| next / F | 1557 / 1557 | 1559 / 1559 |
| B / R | 4 / 0 | 6 / 0 |
| network / effective scale，千分值 | 1000 / 1000 | 1140 / 1140 |

4276 是区间起点，之前那次 native 的 3.025 ms 不计入 4276→4277 成本。开区间到闭终点内只有 4277 一次 native，累计 correction +0、resim +0、captures +2、renderer submissions +2、returnCode=1，wait +0。没有被略去的中间 native / frame callback。

request 4292 已知属于游戏 tick，父 request 4291 的退出时间可用。requested→entered 为 **93.245 ms**；资格起点取 requested 和 parent exited 的较晚者，得到 **93.080 ms**。无取消、抛异常、控制暂停或隐藏事件，hook 与活动父样本资格均满足。该“eligible wait”表示这个已识别 request 尚未进入的时间，不等于整页 JS 空闲或线程 CPU 空闲。

同一页面时钟可直接核对：

| 相邻边界 | 时长 ms |
|---|---:|
| 前一 sampledAt 30264.015 → 父 callback 退出 30264.190 | 0.175 |
| 父 callback 退出 → 新 callback 进入 30357.270 | 93.080 |
| 新 callback 进入 → 新 sampledAt 30367.675 | 10.405 |
| 合计 | **103.660** |

10.210 ms native 测量是最后 10.405 ms 内的已测子区间。不能用 97.185 ms rAF 时间戳差减 native 成本来推导线程空闲。

## 3. 注入、relay 与另一端的时钟支持

跨进程换算只使用已记录的 `performance.timeOrigin`。P2 为 **1791686146595.055**，relay 为 **1791686140400.493**；P2 页面时间加 **6194.56201171875 ms** 可数值映射到 relay 时间。未单独校准跨进程时钟误差，不能据此声称精确的收包或系统调度先后。

relay 的 first-nonempty-input-receive anchor 为 **10032.3082 ms**；配置在 anchor 后 20000 ms 起，持续 500 ms，额外 delay 120 ms。计划区间为 **[30032.3082,30532.3082)**；56 份 packet 被标记注入，实际 send 首末为 **30200.9906 / 30692.2876 ms**。目标 sampledAt 区间映射到 relay 为 **[36458.577011704445,36562.23701170087]**。它与计划注入及带 burst 标志的实际 send 时段分离；因 `networkTrace=false`，没有客户端接收 / native 采纳的逐 packet 时间戳，不能额外断言客户端不存在任何未观测队列。

| relay API event / packet | relay API 进入→退出 ms | elapsed ms | 映射 P2 页面进入→退出 ms | burst | due 后晚发 ms |
|---|---|---:|---|---:|---:|
| 13715 / 4568 | 36453.6542→36501.3457 | 47.6915 | 30259.0922→30306.7837 | 0 | 27.3321 |
| 13717 / 4569 | 36501.9540→36546.9089 | 44.9549 | 30307.3920→30352.3469 | 0 | 69.3736 |

两份均为 P1→P2，bufferedBefore / After 均为 0；配置抖动后的 delay 为 36.0856 / 38.2359 ms，scheduled→send-enter 为 63.4177 / 107.6095 ms。packet sequence 2378 / 2379，firstInputFrame 均为 1548，inputCount 5 / 6，senderFrame 1553。API bracket 记录的是 wall elapsed，不是 CPU 执行时间，也不是网络到达耗时；bufferedAmount=0 不能证明底层没有阻塞。

relay 的既有定时探针在 **36547.3124 ms** 执行，相对 due **36524.3875** 晚 **22.9249 ms**，相邻 probe gap **122.9249 ms**。它是 relay 进程里的探针定时器，不是对端游戏心跳或 OS packet 到达事件。这与上述 API elapsed 同段，但不提供唯一因果解释。

P1 页面 origin 为 **1791686142372.125**。以下 P1 相邻 callback 时点已映射到 P2 页面时钟：

| P1 sample 区间 | P2 时钟映射 | sampledAt gap ms | rAF 时间戳差 ms | eligible request wait ms | 终点 native ms | 进度 / wait |
|---|---|---:|---:|---:|---:|---|
| 4317→4318 | 30241.4600→30308.6951 | 67.235 | 18.135 | 65.930 | 1.255 | F +1，wait 0 |
| 4318→4319 | 30308.6951→30359.2551 | 50.560 | 97.170 | 48.205 | 2.300 | F +3，wait 1 |

对应 request / parent 为 **4394 / 4393**、**4395 / 4394**。两段 correction / resim 均为 0，R=0。B 分别 4→5、5→8，network scale 1000→1000、1000→1393。第二段终点 **returnCode=2**，captures=1561、F=1560，是 P1 正文唯一一次零重模拟 wait；它没有否定 P2 目标自身的 0 wait，也不能从 P1 的 wait 返回推算 50.560 ms 都在等待网络。

P1 的**最大新状态间隙**为 **4316→4318 的 72.485 ms**，rAF 差 24.130 ms；67.235 是相邻 callback 指标，两者不可替换。P1 最大 rAF 差 97.170 ms 则对应后一段 sampledAt 50.560 ms。跨端重叠使“只有 P2 重模拟太重”不足以解释这个事件；共同运行环境、调度和 relay API 内的 wall 耗时仍需进一步区分。

## 4. B、R 与注入前后的降速分布

F 是已推进的预测前沿，A 是已采纳的连续远端 actual-input exclusive frontier；**B=max(F−A,0)**，**R=max(F−next,0)**。R 是待完成的重模拟距离。scale 为 callback 结束时观测值，可能不同于调用开始时使用值。以下仅统计 network scale，不把轻微 effective-scale 调节混入网络降速。

| 端 | 正文调用数 | B p50 / p95 / max | R>0 次数 / max | network scale p50 / p95 / max | scale>1000 次数 | wait 总数 / 有 resim 进度 |
|---|---:|---|---|---|---|---|
| P1 1× | 4489 | 4 / 4 / 8 | 0 / 0 | 1000 / 1000 / 2000 | 165，3.676% | 1 / 0 |
| P2 4× | 4474 | 3 / 4 / 8 | 5 / 6 | 1000 / 1000 / 2000 | 110，2.459% | 14 / 14 |

按每次 callback 的 post-native 时刻换算到计划注入的半开区间，正文每条样本都归入 before / during / after，无 missing phase。after 包括延后投递与制动衰减，不能当作完全无扰动区间。这里是**每 callback 的未加权分布**，不是降速占墙钟时间，也不等于实际 FPS 或速度比。

| 端与时期 | 调用数 | B 均值 / p95 / max | scale>1000 次数 | network scale 均值 / p95 / max | wait 次数 |
|---|---:|---|---:|---|---:|
| P1 before | 2767 | 3.397 / 4 / 7 | 25 | 1003.351 / 1000 / 1667 | 0 |
| P1 during | 82 | 6.195 / 8 / 8 | 70 | 1634.159 / 2000 / 2000 | 0 |
| P1 after | 1640 | 3.639 / 4 / 8 | 70 | 1013.129 / 1000 / 1667 | 1 |
| P2 before | 2755 | 2.772 / 4 / 5 | 5 | 1000.146 / 1000 / 1100 | 5 |
| P2 during | 79 | 5.709 / 8 / 8 | 65 | 1522.658 / 2000 / 2000 | 2 |
| P2 after | 1640 | 2.584 / 3 / 7 | 40 | 1007.921 / 1000 / 1667 | 7 |

注入期 B 和网络制动显著增多；after 两端 network scale 的 p95 与最后样本均为 1000，表明大多数回调已回到基准。没有从聚焦文件推造“第一次完全恢复”的精确时刻。P2 正文最后一条 **network=1000、effective=998**；effective 全正文最小 998，不能写成两种 scale 都一直不低于 1000。P1 两者最小均为 1000。P2 R>0 分布为 during 2 次、after 3 次；全正文 14 次 wait 均有 resim 计数前进，不是 14 次完全停顿。

正文 logicHz 为 P1 59.188、P2 59.126；这以 F 进度 / 墙钟计算，不是物理显示 FPS。即使进度接近 60，103.660 ms 新状态间隙仍保留为不利事实。

## 5. Host、anomaly、音频与输入证据边界

最近 Get-Process 源查询从 UTC **1791686176243** 开始，duration **29.8109 ms**，结束 **1791686176272.8109**，received **1791686176275**；下一次源查询开始 **1791686181303**，duration **44.118 ms**，received **1791686181349**。这些是持续 PowerShell helper 内的查询，不是每 5 秒重启进程，也不是 CIM 采样。

| 目标 bracket | 与最近查询是否重叠 | 查询结束到 bracket 起点 ms |
|---|---|---:|
| P2 request 4292 requested→entered | 无 | 586.269 |
| relay API 13715 | 无 | 581.336 |
| relay API 13717 | 无 | 629.636 |

所以本次没有直接的 Get-Process 查询区间重叠证据。它不证明整个探针对调度完全无干扰：CPU 观察每 1 秒、process 查询每 5 秒，都有观察成本。覆盖事件的 CPU 计数区间约 UTC **1791686176478→1791686177483**，24 logical CPU 的整机 busy 为 **27.1008%**。该粗区间不说明单线程 / 单核可用性、主频、GPU 或 OS 抢占，也不排除短任务。Host 记录 valid / complete，47 次 CPU attempts、10 次 process messages、250 行，overflow 与 errors 均为空。

两端 headedControl 都记录 normal bounds 10,10,1297,809、viewport 1280×720、available screen 1280×720、visible=true、focus=true。几何重叠及超出可用区域只是环境候选；该实验没有改变窗口参数或证明窗口几何导致长等待。

| 指标 | P1 | P2 |
|---|---|---|
| anomaly valid / coverageComplete | true / **false** | true / true |
| requestsScanned / skipped | 5088 / **2** | 4984 / 0 |
| polls / 最大 poll 间隔 ms | 287 / 434.180 | 287 / 434.505 |
| episode | null | null |
| 音频 blocks / nonzero | 359 / 357 | 359 / 358 |
| 零块索引 | **[0,1]** | **[0]** |
| 音频最大观测幅度 | 1.0062623 | 1.0149671 |

P1 coverage 缺口不能用 episode=null 掩盖。250 ms anomaly 触发阈值本来就不会拒绝本次 93.080 ms eligible wait；两端各 255 次 pre-history eviction 是有界历史记录，完整计数保留。停止后的 pending request 分别年龄 6.320 / 10.185 ms，均因没有活动父样本、控制中断、loop 停止、harness inactive 而不具备活动卡顿资格；不得计成新游戏冻结。

音频 overflow 均为 false，每端观测 22976 个采样值，但它只是输出块观察，不是硬件可听性或连续播放验收；不擅自剔除零块为热身。音频相对块时间也不能无已记录 page anchor 硬对齐到 103 ms 事件。

正文意图 P1 110 / 110 / 110、P2 111 / 111 / 111 均已计划 / capture / presentation-observed；两侧 unresolved 完整数组为空。按计划时点归属正文的意图仍保留跨到 tail 的观察，不丢慢输入。正文 schedule→presentation-observed p95 / max 为 P1 **31.040 / 36.565 ms**、P2 **38.500 / 49.840 ms**。这不能使没有新输入的长新状态间隙消失，也不是物理输入到像素延迟。

15 个相同 confirmed 前缀检查点两侧一致。原记录 `sameFrame=false`、`finalComparable=false` 保留：wall-clock 输入下终态不在相同可比较帧，不能写成 final-world-hash 完全相同。原始错误、响应观察失败计数与来源检查完整保存在 context，不以成功子集替代全量。

## 6. 下一步候选与停止条件

先把这种“rAF 长等待 + 跨端同段等待 + 独立 relay send wall elapsed 长”与深回滚 CPU 路径分开验证。当前 103 ms 区间没有 resim；单独减少 Restore 次数、调整 checkpoint 或降低设备端模拟速度，不能从本证据推出会修复它。深回滚 CPU 优化仍需独立等价性与 4× 实际调度验证。

一个小范围候选是在现有 relay send wall bracket 内同时记录 **当前线程 CPU delta**。本 trace 没有该字段，也尚未在 relay 植入或做新实测。根代理随后已单独完成 [接口可行性校准](rollback-relay-thread-cpu-feasibility.json)：受控等待报告 CPU 为零，但约 50 ms 忙循环报告 62 / 63 ms CPU，高于墙钟区间。这些短窗原值必须保留，不能据此计算精确 CPU 占比或 wall-minus-CPU 等候；完整开销与精度边界见 [执行记录](rollback-relay-thread-cpu-feasibility-execution.json)。后续若植入，还需保留 API 不支持 / 失败路径和现有 wall 字段，并做同条件观察开销对照。若长区间 thread CPU 很低，可减少“线程持续计算”这一解释的支持；仍不能单独区分同步阻塞、外部抢占、GC / 本机运行时或其它调度原因。若 CPU 也高，再定位具体同步工作。不能以 process 总 CPU 或接口校准代替该事件本身的线程证据。

HOST_LOAD 开关受控实验仍可作为将来的排除项，但本次 Get-Process 查询无直接重叠，不应优先定罪探针。后续若做配对实验，需冻结相同 release、窗口、输入与网络注入，保留全部失败和主机背景；一次改善不足以宣称因果。现阶段不改生产行为，不用 250 ms 阈值、接近 60 的 logicHz 或成功音频检查覆盖 **103.660 ms** 这个尚未解决的体验缺口。
