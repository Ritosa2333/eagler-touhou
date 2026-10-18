# E1 两次长停顿与 85.035 ms 指标的解释

## 结论与适用范围

这次 E1 发布版记录仍然包含不可忽视的停顿：P2 正文窗口内，完成新状态并提交渲染后的最大观测间隙是 **98.075 ms**，另一次相邻间隙是 **84.660 ms**。原先关注的 **85.035 ms** 是另一组端点的浏览器 rAF 时间戳差，它对应的实际 post-native 观测间隙只有 23.550 ms。不能把时间戳错位解释为停顿消失，也不能只报 85.035 而漏掉更大的 98.075。

两次长的实际观测间隙都没有 correction 或 resimulation 增量。rAF 请求等待、独立 relay 的 send API elapsed，以及另一浏览器的调度间隙在对应时段出现。证据足以说明“深回滚重演耗时”不能解释这两个具体长段的主要时间组成；尚不足以在 relay、浏览器、OS 调度、运行环境或探针之间确定根因。第一次仍有 **23.985 ms native elapsed**，这项不利成本也不能删掉。

本分析只读已有 E1 raw，未改生产或测试源码，未运行新游戏测试、构建或浏览器。E1 为 release、D=0、输入 tape、P1 CPU1/P2 CPU4、独立本机 relay 注入 39±5 ms、无 burst、无丢包。两端都有生产 rAF 调度及音频探针；没有 packet receive/native acceptance trace 或 Chrome scheduler trace。这是一段有限的受控运行，不能外推为公网达标或所有设备都流畅。

## 1. 三个区间必须分别保留

采用已冻结的 continuous-PredictionFrontier helper。P2 正文为样本 [467, 4862)，F 从 180 跨到 1800；没有按回卷后的 next 筛掉调用。新状态要求 F 前进、next 已恢复到 F，且 renderer submission 计数增长。capture 自身不代表已经完成的新状态。

| 区间 | 样本 | 浏览器 rAF 时间戳差 ms | post-native 新状态观测差 ms | native elapsed 合计 / 最大 ms | correction / resim | capture / submission 增量 |
| --- | --- | ---: | ---: | ---: | --- | --- |
| 正文观测最大值 | 845 → 846 | 78.435 | **98.075** | 23.985 / 23.985 | 0 / 0 | 2 / 2 |
| 后续相邻长段 | 852 → 855 | 18.085 | **84.660** | 9.505 / 9.495 | 0 / 0 | 1 / 1 |
| 正文 rAF 时间戳最大值 | 855 → 857 | **85.035** | 23.550 | 16.970 / 10.660 | 1 / 3 | 1 / 1 |

845→846 只有一次新的 native 调用，F=324→326。852→855 包含三个调用，其中两个 native elapsed 约 0.005 ms；F=332→333。855→857 则有两个调用，其中一次 returnCode=2 的 wait；恢复后的 857 才提交 F=334。856 的 capture 已增加，但 F 与渲染提交没有增加，不能把它提前计作新画面。

### 同一页面时钟可直接分段的观测事实

- 845 的 post-native 采样为 9392.440；其 rAF wrapper 退出为 9392.630；下一请求进入为 9466.340；846 的采样为 9490.515。三个直接观测跨度为 **0.190 + 73.710 + 24.175 = 98.075 ms**。其中 native elapsed=23.985 ms 是最后一段内的原始测量。
- 852 的采样为 9573.060；854 的 rAF wrapper 退出为 9584.955；855 的请求进入为 9647.875；855 的采样为 9657.720。对应 **11.895 + 62.920 + 9.845 = 84.660 ms**。
- 请求 866 的 requestedAt=9584.945，enteredAt=9647.875，原始 request wait=62.930 ms；扣除仍未退出的 parent wrapper 后，eligible wait=62.920 ms。
- 855 的浏览器 rAF timestamp=9587.055，而实际 request 进入为 9647.875，相差 60.820 ms。后续 855→857 的 rAF timestamp 差因此不能当作从 855 新状态完成到 857 新状态完成的持续时间。

这里的 request wait 是已识别请求的两个观测边界；它不等于“CPU 空闲”，也不说明这段时间里所有 JavaScript 都没运行。上述分段使用同一页面 performance.now 的实际边界，没有用 rAF timestamp 减 native 成本去臆造 idle 时间。

## 2. 原调用、所有者和时钟

下表全部时间单位为 ms，均属于 P2 Page performance.now。request 的 callbackId 均为 0，对应已登记的 game callback `tick`，parent 链保持连续。frameCalls=1，hook 标志有效，无 request/callback throw。837–867 扩展上下文完整保留于 addendum；这里列出 845/846 及要求覆盖的 850–858。

**onGameFrame 进入/退出是 JS 观察回调的边界，不是 native 函数的进入/退出。** native elapsed 使用样本第 2 列；post-native 采样是 onGameFrame 内执行 play.frame 后、后续观察器返回前取得的 performance.now。

| sample | request (parent) | requestedAt | 浏览器 rAF stamp | rAF 进入 | rAF 退出 | onGameFrame 进入 → 退出 | post-native 采样 |
| --- | --- | ---: | ---: | ---: | ---: | --- | ---: |
| 845 | 856 (855) | 9381.890 | 9387.440 | 9387.945 | 9392.630 | 9392.425 → 9392.450 | 9392.440 |
| 846 | 857 (856) | 9392.610 | 9465.875 | 9466.340 | 9490.720 | 9490.500 → 9490.520 | 9490.515 |
| 850 | 861 (860) | 9547.980 | 9544.760 | 9551.535 | 9558.180 | 9557.785 → 9557.990 | 9557.975 |
| 851 | 862 (861) | 9557.990 | 9556.900 | 9561.440 | 9567.560 | 9566.970 → 9567.355 | 9567.180 |
| 852 | 863 (862) | 9567.360 | 9568.970 | 9570.215 | 9573.075 | 9573.050 → 9573.070 | 9573.060 |
| 853 | 864 (863) | 9573.070 | 9574.975 | 9581.340 | 9581.535 | 9581.355 → 9581.530 | 9581.525 |
| 854 | 865 (864) | 9581.530 | 9581.040 | 9584.920 | 9584.955 | 9584.930 → 9584.945 | 9584.940 |
| 855 | 866 (865) | 9584.945 | 9587.055 | 9647.875 | 9657.740 | 9657.540 → 9657.725 | 9657.720 |
| 856 | 867 (866) | 9657.730 | 9653.835 | 9661.505 | 9672.750 | 9672.350 → 9672.555 | 9672.550 |
| 857 | 868 (867) | 9672.725 | 9672.090 | 9674.765 | 9681.295 | 9681.090 → 9681.280 | 9681.270 |
| 858 | 869 (868) | 9681.280 | 9678.070 | 9682.110 | 9682.305 | 9682.120 → 9682.290 | 9682.290 |

两次长等待均处于 start 后、pause(0) 后的同一运行阶段。pause(0) 记录为 3856.685→3856.860，最终 pause(1) 为 34305.080→34305.265；两个事件之间没有记录到 stop/restart/pause。保存的生命周期事件只有 init/pageshow，并标为 visible。该观察不能证明窗口在整个事件中始终没有被遮挡。

| sample | next / F | confirmed / 实际输入前沿 | actual gap | returnCode | native ms | correction / resim / capture / submission 增量 |
| --- | --- | --- | ---: | ---: | ---: | --- |
| 845 | 324 / 324 | 322 / 322 | 2 | 1 | 4.305 | 0 / 0 / 1 / 1 |
| 846 | 326 / 326 | 323 / 323 | 3 | 1 | 23.985 | 0 / 0 / 2 / 2 |
| 850 | 330 / 330 | 328 / 328 | 2 | 1 | 6.240 | 0 / 0 / 1 / 1 |
| 851 | 331 / 331 | 329 / 329 | 2 | 1 | 5.505 | 0 / 0 / 1 / 1 |
| 852 | 332 / 332 | 330 / 330 | 2 | 1 | 2.830 | 0 / 0 / 1 / 1 |
| 853 | 332 / 332 | 330 / 330 | 2 | 1 | 0.005 | 0 / 0 / 0 / 0 |
| 854 | 332 / 332 | 330 / 330 | 2 | 1 | 0.005 | 0 / 0 / 0 / 0 |
| 855 | 333 / 333 | 330 / 330 | 3 | 1 | 9.495 | 0 / 0 / 1 / 1 |
| 856 | 333 / 333 | 331 / 331 | 2 | 2 | 10.660 | 1 / 3 / 1 / 0 |
| 857 | 334 / 334 | 332 / 332 | 2 | 1 | 6.310 | 0 / 0 / 0 / 1 |
| 858 | 334 / 334 | 332 / 332 | 2 | 1 | 0.000 | 0 / 0 / 0 / 0 |

以上样本的 networkScaleMilli 均为 1000，effectiveScaleMilli 均为 997。两次实际长段的 actual gap 从 2 到 3，没有发生 networkScale 所指示的降速；不能把它们直接解释为网络信息不足而主动慢放。稍后的 856 确实发生一次 correction 和三帧 resim，应单独记在 855→857。

## 3. 独立 relay 和 P1 的同段观察

P2 timeOrigin=1791684826235.07，relay timeOrigin=1791684819453.011。数值映射为 relay performance.now 减去 **6782.05908203125 ms**，得到对应的 P2 页面时钟位置。原始 relay 时钟和完整 packet/event 关联均保存在 addendum。

这是已有 performance.timeOrigin 的数值对齐，没有独立标定跨进程时钟误差；不能用小于 1 ms 的先后关系证明因果。relay send API 边界不是浏览器收包、native 接纳输入或物理呈现的时间。

| relay event / packet | 方向 | relay send 进入 → 退出 ms | API elapsed ms | 映射到 P2 时钟 ms | 相对 due 的进入迟到 ms |
| --- | --- | --- | ---: | --- | ---: |
| 2785 / 923 | P1 → P2 | 16175.291 → 16189.011 | 13.720 | 9393.232 → 9406.952 | -0.316 |
| 2786 / 924 | P2 → P1 | 16189.092 → 16246.875 | 57.783 | 9407.033 → 9464.816 | 10.993 |
| 2787 / 925 | P2 → P1 | 16246.911 → 16247.054 | 0.143 | 9464.852 → 9464.995 | 63.521 |
| 2842 / 945 | P2 → P1 | 16366.313 → 16429.550 | 63.237 | 9584.254 → 9647.490 | 15.590 |
| 2843 / 944 | P1 → P2 | 16429.654 → 16429.788 | 0.134 | 9647.595 → 9647.729 | 75.261 |
| 2846 / 947 | P2 → P1 | 16432.646 → 16432.708 | 0.062 | 9650.587 → 9650.649 | 47.504 |
| 2847 / 948 | P1 → P2 | 16432.713 → 16432.736 | 0.023 | 9650.654 → 9650.677 | 48.101 |
| 2848 / 946 | P2 → P1 | 16432.747 → 16432.767 | 0.020 | 9650.688 → 9650.708 | 41.833 |

### 第一次：98.075 ms

P2 request 857 的等待为 9392.610→9466.340。relay 先有 event2785 的 13.720 ms send bracket，再有 event2786/packet924 的 **57.783 ms** bracket；后者映射为 P2 9407.033→9464.816。它们处于该 P2 等待之内。

P1 在第一次 P2 长等待的前段仍有回调。P1 自身最大的相关间隙是样本 902→903 的 **33.960 ms**，约映射到 P2 9431.250→9465.210；下一 native elapsed=0.740 ms，correction/resim 增量均为 0，eligible request wait=33.170 ms。不能写成“三个进程同时停止了完整 73.730 ms”。

### 第二次：84.660 ms

P2 request 866 的等待为 9584.945→9647.875。relay event2842/packet945 是 P2→P1 的发送，API bracket 为 **63.237 ms**，数值上映射到 P2 9584.254→9647.490，覆盖几乎同一时段。随后 P1→P2 的 packet944 比 due 晚 **75.261 ms** 才进入 send；从 scheduled 到 send 进入为 **113.887 ms**，其中原计划注入 38.626 ms。

P1 样本 920→921 的观测差为 **64.185 ms**，下一 native elapsed=1.580 ms，correction/resim 均未增加。其 request eligible wait=62.550 ms。此事件比第一次更接近跨两浏览器与 relay 的同段迟滞。

relay 100 ms timer 在 16429.8048 观测到 late=**69.4734 ms**、相邻 tick 间隙 169.4734 ms；映射约为 P2 9647.746。该 timer 异常属于第二次邻近时段，不能拿来给第一次 98.075 ms 整段作归因。

这一份 relay trace 的覆盖标志正常且无溢出；扩展保留 86 个关联 packet 和 258 个完整 event。这里仍然不能把 send elapsed 称为 ws.send 的 CPU 用量，或从 API bracket 推断 Node 线程始终在执行发送代码。可能的外部调度、同步调用、观察成本与环境原因仍需单变量实验区分。由此也不能保证把 relay 独立成进程就自然消除停顿，更不能据此断言 direct 更好。

P2 的 network receive trace 未开启，rafAnomaly 只保留末段的 32 次 pre-poll，该时段没有 retained poll。实际输入前沿按 native pacing 原样保留；不能把某个 relay send 确定对应到某个 peer receive 或 native acceptance。

## 4. Host 采样是否刚好覆盖停顿

host 有效且完整，CPU 每约 1 s 采样，持续 PowerShell helper 每约 5 s 执行 Get-Process 查询。这不是每 5 s 启动一个新的 PowerShell，也不是该周期在做 CIM 查询。

最近前一次查询记录 sourceUtcMs=1791684835050，sourceDurationMs=30.687，receivedUtcMs=1791684835081。按 source 起点加 duration，查询结束约为 1791684835080.687。下一次 sourceUtcMs=1791684840112，duration=30.6079，receivedUtcMs=1791684840144。

| 长区间 | UTC 起点 ms | UTC 终点 ms | 与前后查询执行区间重叠 | 前查询结束到长区间开始 ms |
| --- | ---: | ---: | --- | ---: |
| P2 request857 等待 | 1791684835627.680 | 1791684835701.410 | 无 | 546.993 |
| P2 request866 等待 | 1791684835820.015 | 1791684835882.945 | 无 | 739.328 |
| relay send2786 | 1791684835642.103 | 1791684835699.886 | 无 | 561.416 |
| relay send2842 | 1791684835819.324 | 1791684835882.561 | 无 | 738.636 |

即使用 sourceUtcMs±sourceDurationMs 的更保守包络，仍没有重叠。下一次查询在这些区间结束后约 4.2–4.4 s 才开始。当前没有直接支持“Get-Process 正在执行，造成这两次停顿”的时间重叠证据。

CPU 样本时间点为 1791684835478 与 1791684836479，四个长区间都位于两点之间。该约 1001.167 ms 累计区间的整机 busyFraction=**31.9042%**，logicalCpus=24。这不是逐核心、逐回调或 GPU 的可用能力读数，不能因为整机没有满载就排除设备、单核争用、抢占或时钟环境。CPU 读取没有保存 before/after 观测边界，不能仅凭采样点错开证明它完全无干扰。

两份 5 s 进程快照之间，登记为 relay 的 node PID46640 累计 CPU 增量 0.578125 s；另一 node PID40148 增量 0.015625 s，其他匹配且计数未倒退的候选为 0。这些粗粒度增量不能分配给 57.783/63.237 ms send 区间，也不能证明编译器、进程调度或发送 CPU 是根因。短命任务与未被候选筛选覆盖的活动仍不可见。

## 5. 窗口环境只列为候选

两端 setup-complete 与 measurement-complete 都记录窗口 bounds=(10,10,1297,809)、viewport=1280×720、报告可用屏幕=1280×720，visibility=visible、focus=true。两窗口的记录坐标相同，外框尺寸也超出报告可用屏幕；因此无法凭该记录证明持续非遮挡呈现。

记录中的启动参数包含 disable-background-timer-throttling、disable-backgrounding-occluded-windows、disable-renderer-backgrounding，并完成 identity 验证。参数与 visible/focus 快照不等于实际窗口合成、物理 scanout 或 OS 调度的证据。这里未移动窗口、修改尺寸或更换任何浏览器控制参数。

## 6. 后续计划与边界

1. 继续沿冻结的发布版、F 连续窗口和双时钟口径分析既定 burst/response/shadow；实际新状态间隙、rAF 调度等待、native elapsed、输入缺口和 relay API 时钟分别保留。不能凭这两次事件宣布敌人去重优化已经消除卡顿。
2. 这两次长段优先用于核查跨端调度和 relay 观察边界，不能把它们当作深回滚 restore 优化的收益证明。深8/CPU4 中真正的重演 CPU 成本仍需按原计划优化和验证。
3. HOST_LOAD 关闭可作为后续低成本的单变量受控检查，前提是保持源码、浏览器参数、CPU、路由、输入序列和验收窗口一致，并保留必要的 rAF/relay 观察。当前查询不重叠使“直接由 Get-Process 引发”的假设缺少支持；关闭探针一次没有复现也不是因果证明。
4. 窗口控制或 direct/relay 拓扑变化应分别成为后续受控变量；不能把它们和代码改动一起混入同一归因比较。当前不实施新的参数改变，也不以加输入延迟、削减状态或改 checkpoint 常数替代问题分析。

## 7. 证据与提取记录

所有路径以 D:/workspace/eagler/worktrees/th09-rollback-4x 为根：

- raw：th09_web/artifacts/multiplayer-tests/rollback4x-enemy-index-relay39-e1-report.json，24,324,549 B，SHA256 `31009adb066dcb4e754ec0a9c57453f17f1bd1d8f703554db31a1217df16d0b2`。
- 首份聚焦证据：docs/multiplayer/rollback-enemy-index-relay39-e1-gap-evidence.json，197,010 B，SHA256 `76806795b3856070b5dfca9cb7d7a48d59439aab9022fbcbd6a372f0e4de888d`。
- 两次事件扩展：docs/multiplayer/rollback-enemy-index-relay39-e1-gap-addendum.json，234,938 B，SHA256 `26edb102fa616fa2467cfe9dade39f15db1b598162c66c3a0a0f018d86c1f65c`。首份聚焦文件保持原 SHA。
- helper：th09_web/tests/multiplayer/smoothness-window.mjs，SHA256 `9c46c45b275b5b57912a62c58dbc309dfe3eec2bff6b788a2370ce6c214c0546`，与预声明计划相同。
- native callback 采样位置：th09_web/tests/multiplayer/smoothness-browser.mjs 第 441–462 行附近；该文件 SHA256 `dde1d2e8f744b33e2eb56337ada81524083f334cd5b897c1dbb57bbf17a74fec`。
- release Wasm SHA256 `766dc277b15d86a97d4bc3b38e2a08a9b52a5786d74c817763bc185216673577`；loader SHA256 `4e53b06c7e2a99624bc4234ffde1682a9a18ce7c89be3d2a414b4f0cb3620ee8`。

首轮提取命令 DSNm2TodXlt8IyHVCcWhl0Tf 已 exit0，但 stdout 保留缓冲截掉前 237,029 B；不把它当完整 JSON 证据。之后 x5YC9m3wabYd3rD5QgOWsN2A（862 ms）用 flag wx 保存首份聚焦文件；2GN5enMU-uDePFxLLcigE31z（637 ms）同样保存扩展。两次保存均 exit0，只读原始结果，没有重跑游戏。raw 前后 SHA 一致。

元数据勘误：addendum 的 sampleDefinitions 中第 8–10 列为未用于本分析的占位名称，正确原始对应为 rollback-info 的索引 5、6、7（不是占位文字中的 4、5、6）。原始数值行完整保留且没有改变；本文只使用已核实的 sample 列 0–7、11–13 和 pacing 列 0–6。原两份证据维持不可变，勘误在此明确记录。

