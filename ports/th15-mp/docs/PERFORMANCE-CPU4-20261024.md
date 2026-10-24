# TH15 MP CPU 4× 性能、保存边界与固定宽度 hook（2026-10-24）

**CPU 4× 下尚未维持 60 逻辑帧／秒；generic 继续作为默认。** 最新一轮在同一 `792f0290` 源码、同一组编译对象上比较 generic 与 scalar，完成十二场真实生产运行、五个有效配对。scalar 在双人 2 帧延迟回滚中提高约 5.65%–6.37%，双人和三人零延迟场景则全部下降。三十个席窗口均低于 60 Hz，十二场独立计算的 100 ms 回调／画面／前进水位门槛均未通过。

scalar 已通过七组实际游戏 oracle，独立核对 21,946 个条件；1,440 个 hook 微基准样本也完成独立核对。微基准的局部路径省时没有形成整局稳定提升，混合路径的配对耗时中位数反而增加 7.59%。本轮第一场保存证明因约 0.1 ms 的跨时钟事务关联差异失败，保留原 FAIL 并排除该配对。

本文接续[前轮优化报告](PERFORMANCE-OPTIMIZATION-20261024.md)。[机器记录](PERFORMANCE-CPU4-20261024.json)包含新一轮逐席聚合、全部 32 组微基准聚合、完整文件 SHA，以及明确分开的旧 `87d10f5a`／Xeon 转录数据。以下新结果没有与旧数据合并。

## 用户硬件复测所设定的目标


用户提供的 `a0a2e1f9` 复测为 i7-14650HX、Intel UHD 硬件渲染、每席独立 Chrome、CPU 4×、名义附加 RTT 约 100 ms，每组 15 秒。以下是用户报告的结果，本轮没有在该硬件上重新取得测量：

| 场景 | 逻辑帧／秒 | 回调 P95 | 最长画面间隔 |
| --- | ---: | ---: | ---: |
| 双人零延迟回滚 | 28.7–28.8 | 34.1–35.0 ms | 395 ms |
| 双人 2 帧延迟＋回滚 | 43.6–44.0 | 30.6–32.2 ms | 460 ms |
| 三人零延迟回滚 | 31.7–31.8 | 31.0–32.9 ms | 395 ms |
| 三人 2 帧延迟＋回滚 | 33.1–33.7 | 32.9–33.2 ms | 234 ms |

用户同时报告六组含纯延迟对照的输入、Replay 和保存校验全部通过；纯延迟也出现长间隔。后续优化的目标仍是在这个硬件条件下维持 60 逻辑帧／秒。下文的 SwiftShader 结果用于检查本机候选和定位问题，不能与这张表直接相除来宣称性能增减。

## 最新一轮：同源 generic／scalar，EPYC 9V74

两包均来自 [`792f0290`](https://github.com/Ritosa2333/eagler-touhou/commit/792f029084a405d4ec8ec3c927c64ecf534f6e85)，266 个编译对象及缓存键逐项相同，loader 字节相同。切换项是固定宽度 scalar hook 插桩；生成的 manifest、runtime-files、shell 和 Wasm 随 profile 改变，不能宣称整个目录逐字相同。

| 对象 | 实际 Wasm | 字节数 | 用途 |
| --- | --- | ---: | --- |
| generic 控制包 | `622f1e9392b98875d461a3c7da0fbee3c471041c4225f9e50e2b31782e9cafa2` | 6,025,407 | 默认 profile，矩阵基线 |
| scalar 包 | `64fef944913233436c088297eb9ddd743d88b5fb164d6dbe7137eb306c0b0579` | 5,871,787 | opt-in profile，矩阵候选 |
| scalar 游戏 oracle fixture | `078e7f6b2c2da559e84d98b222e90607a837c7542f2b07e4bf00fb39c82a7737` | 5,931,209 | 原生游戏／纹理／Replay 正确性 |

scalar 生产包减少 153,620 字节，约 2.55%。它与 generic 的两个构建 flow 都保持 `BUILT_ONLY`；七组游戏 oracle 有另外的真实 execution flow。generic 控制包本次没有另外创建游戏 oracle fixture，不把 scalar 的结果写成 generic 的新 oracle 记录。

当前主机为 **AMD EPYC 9V74 80-Core Processor**，容器可见 9 个逻辑 CPU，`cpu.max=800000 100000`，即 8 核配额。CPU 身份来自同一当前环境的实际 micro receipt。正式矩阵使用 SwiftShader，每席独立 Chrome、CPU 4×、15 秒窗口、名义附加 RTT 100 ms，未开启 profiler；实际执行区间为 17:17:48–17:27:31 UTC。它与此前 Xeon 一轮和用户 Intel UHD 硬件复测分属不同环境，不能跨表计算优化收益。

### 十二场与五个有效配对

顺序固定为 `(mode 1,2P)、(0,2P)、(2,2P)、(1,3P)、(0,3P)、(2,3P)`，每条件先 generic、后 scalar，各一次，不重跑或替换失败。表内区间是同场各席范围；百分比按 seat 0、1、2 顺序列出。单次观察没有重复试验置信区间。

| 场景 | generic 逻辑帧／秒 | scalar 逻辑帧／秒 | 各席变化 | 回调 P95：generic → scalar（ms） | 最长画面间隔：generic → scalar（ms） |
| --- | ---: | ---: | --- | --- | --- |
| 双人纯延迟，mode 1 | 33.33 | 33.33–33.40 | **排除：基线保存关联失败** | 12.1–13.0 → 11.3–12.2 | 124.2 → 136.2 |
| 双人零延迟回滚，mode 0 | 25.07–25.53 | 22.60–22.73 | −11.49%、−9.31% | 33.6–35.0 → 31.7–33.4 | 424.0 → 362.5 |
| 双人 2 帧延迟＋回滚，mode 2 | 28.27–28.33 | 29.93–30.07 | +6.37%、+5.65% | 34.8–38.4 → 33.5–34.9 | 319.7 → 314.0 |
| 三人纯延迟，mode 1 | 23.53–23.67 | 23.33–23.40 | −1.41%、−0.85%、−1.13% | 22.4–24.8 → 22.8–25.6 | 219.7 → 245.5 |
| 三人零延迟回滚，mode 0 | 12.40–13.00 | 11.67–12.33 | −3.65%、−5.91%、−6.67% | 41.0–44.2 → 39.9–50.4 | 631.9 → 745.5 |
| 三人 2 帧延迟＋回滚，mode 2 | 16.00–16.73 | 16.60–16.73 | +2.87%、+4.58%、−0.80% | 40.1–48.8 → 40.6–49.7 | 505.0 → 560.5 |

固定指标继续使用最大 `NextFrame` 水位统计新逻辑进度，重演和倒退不重复计帧。回调 P95 使用在窗口内开始的完整原生回调；画面／前进水位间隔在窗口边界截断。画面提交指默认 framebuffer 的颜色 blit，不是 GPU 完成或显示器刷新。定义见[指标实现](../portable/multiplayer/performance-browser-metrics.mjs)。

原执行为 scalar 6／6、generic 5／6 完成完整输入与保存检查。独立审核从原 raw 全量复算 **528,192 项、0 差异**，包括 30 个席窗口、28 个完整输入／保存席证明、26 个可配对席窗口；2,136 个 Host 输入全部 ACK，输入调用错误为零。Replay 核对涉及 201 份观察、352,118 个含重复快照的 frame-seat samples、87 个不同磁盘文件。30 个 attempt 中 29 个 stable、1 个 error；error 所在原运行仍为 FAIL。

十一场原 `thresholdsMet=false`，失败首场没有生成这个键。独立数值计算的十二场 100 ms 门槛都为 false；审核 PASS 只表明证据与计算一致，不代表游戏执行或性能通过。原 PID 命名空间 `pid:[4026532200]` 中 120 个记录浏览器 PID 全部退出，无活动匹配进程，文件身份在审核结束时保持不变。

### 新一轮长间隔与工作量

scalar 三人零延迟 seat 2 的两个最大间隔如下；均没有与已记录保存区间重叠：

| 间隔 | 总长 | 原生回调重叠 | 回调后观察器重叠 | 已观察回调之外 |
| --- | ---: | ---: | ---: | ---: |
| 画面提交 | 745.5 ms | 102.8 ms | 0.4 ms | 642.3 ms |
| 前进水位 | 1313.6 ms | 402.5 ms | 9.6 ms | 901.5 ms |

同场观察器 P95 为 3.6／5.1／2.8 ms。scalar 三人纯延迟的最大画面间隔为 245.5 ms，其中 23.3 ms 在回调、0.7 ms 在观察器、221.5 ms 在二者之外，和保存区间重叠 132.6 ms；纯延迟的纠正、恢复、历史帧计数为零。墙钟重叠不能把全部间隔归因于保存，也不能拆出模拟、restore、render、软件 GPU 或系统调度的 CPU 耗时。

两套实际运行也不保证相同历史工作量。例如双人零延迟 generic 的前进量为 383／376 帧、历史帧为 187／181；scalar 为 339／341 和 220／216。它说明整局工作量存在变化，不能据此单独认定下降原因。新正式窗口未开启 CPU／GL 诊断，不借用旧 `877e` profile 给新包分摊耗时。

### 首场失败：约 0.1 ms 的跨时钟关联边界

`scalar-profile-01-baseline-mode1-2p-01` 在计时后报 `Stable post-window persistence evidence failed`。seat 0 的 attempt 1 stable；seat 1 的错误为 `No completed native request and filesystem transaction matches the Host sync interval`。

这次中立区间 `[771,773)` 已被两席 776／775 帧 Replay 覆盖，fire 的固定结束帧 789 也被 789／790 帧副本覆盖。每席 evidence copy、MEMFS 与 IndexedDB 字节完全一致，native 校验通过，原输入前缀保持；两席共同 789 帧的 18,936 字节输入一致。取证期间没有新保存替代、native／FS 错误。因此本次失败不能沿用此前“中立 Replay 太短”的解释。

seat 1 将 Host 时间换算为 Runtime 时间后，记录如下：

| 事件 | Runtime 时钟（ms） |
| --- | ---: |
| Host sync 开始 | 34190.9 |
| 新 native token 4 请求 | 34193.1 |
| native 完成 | 34194.2 |
| FS transaction 4 开始 | 34194.3 |
| 换算后的 Host ACK | 34212.9 |
| FS transaction 4 完成 | 34213.0 |
| ACK 后实际 Runtime 快照 | 34213.1 |
| evidence copy | 34225.9 |

原关联规则要求 FS 完成不晚于 Host ACK，容差 `0.001 ms`；实际换算差为 `0.0999999996 ms`，所以没有候选事务。真实控制流先记录 FS completion、调用回调、完成 `save()`，再发 Host ACK；这些跨 realm 时间戳出现了与该因果顺序不一致的边界。时间量化或 timeOrigin 对齐是可能解释，本轮没有独立校准证明其来源。

原 attempt、执行 FAIL、缺失的阈值键和配对排除全部保留，没有扩大容差或事后升级。后续新协议可在发 RPC 前和收到 ACK 后都取得同一 Runtime 时钟的边界，再关联真实 token／FS 链；字节、generation、固定输入区间与唯一事务检查仍需保留。

## 固定宽度 hook 微基准：32 组完整聚合

微基准链接真实 journal／heap C++，先执行生产插桩，再只恢复 18 个明确命名计时循环的原始函数体，以避免显式 hook 与自动 hook 双计。逐函数白名单、索引／签名、实际 callee、volatile store 和冷 capture 路径均有字节审核；这是测试专用模块。实现及测量契约见 [rollback README](../../../tools/rollback/README.md)和[驱动](../../../tools/rollback/scalar-hook-micro.mjs)。

Node 24.19.0／V8 13.6 使用 `--no-liftoff --no-wasm-lazy-compilation`；同一 EPYC 主机上保留全部 **1,440** 个样本，其中 288 个暖机、1,152 个正式样本。32 个 case 各含 plain／generic／scalar、3 轮暖机、12 轮正式计时，全部六种执行顺序各重复两次。每次只给直接 Wasm 循环计时；准备、完整 8 MiB 堆／host／guard／checksum 检查和 Restore 校验在计时外。plain 预先记录目标堆块以便同样验证恢复，其预记录成本也在计时外。

下表是每组 12 个配对 `scalar/generic` 耗时比的中位数减一；**负值为省时，正值为增加耗时**。它不是两组独立中位数相除，也不是整游戏 FPS 变化。

| case family／宽度 | 1 字节 | 2 字节 | 4 字节 | 8 字节 | 16 字节 |
| --- | ---: | ---: | ---: | ---: | ---: |
| `heap-dirty` | −52.24% | −5.42% | −7.58% | −1.30% | −34.07% |
| `heap-first` | −9.02% | −1.52% | +4.84% | +5.12% | −3.11% |
| `outside` | −54.36% | −5.93% | −5.61% | −1.25% | −39.83% |
| `cross-dirty` | 无该 case | −1.13% | −3.91% | +7.83% | −27.71% |
| `cross-first` | 无该 case | −3.26% | −0.17% | +3.72% | −2.31% |
| `cross-first-dirty` | 无该 case | +6.26% | −2.10% | +1.66% | −6.74% |
| `cross-last-dirty` | 无该 case | +0.91% | +1.70% | −2.36% | −0.17% |

第 32 组 `mixed-widths-and-paths` 为 **+7.59%**，12 个正式配对中 10 个 scalar 更慢、2 个更快。它的 generic／scalar 独立中位数分别为 14.55／15.88 ns 每次迭代；这些值不能替代配对比中位数。

独立审核 **32,386 项通过**，重新检查全部样本、工作计划、hash、精确捕获／块计数、checksum、Restore 标记和跨 variant 一致性，没有丢弃慢样本。瞬态完整堆字节由原 trial 内 C++ oracle 核对，未全部持久化，离线审核不声称重新读过这些瞬态字节。所有 32 组数值和实际文件身份见 JSON 的 `scalarMicrobenchmark`。

16 字节在此 fixture 中是一个 16 字节 hook 后接两次 i64 store；生产 scalar 包的静态 16 字节 store 数是零，不能把这一行收益代入生产帧成本。其余静态 store 数为 1／2／4／8 字节各 9,051／2,488／44,800／20,619，它们不是动态命中频率。Node 微基准没有 CPU 4×、浏览器渲染和网络调度，plain 差值也不是可相加的游戏耗时分解。

## 当前 scalar 实现与实际正确性


[`792f0290`](https://github.com/Ritosa2333/eagler-touhou/commit/792f029084a405d4ec8ec3c927c64ecf534f6e85)增加 1／2／4／8／16 字节的单地址参数 hook，共用原 clipping、first-write tag 和 capture 逻辑。Wasm 插桩先保留原 pointer／value operand 的求值顺序和私有 locals，再按 store 宽度选择 hook；bulk 操作继续使用双参数 generic hook。原生 ABI 为 1，所有已声明的 scalar hook 与 ABI 导出在现有各 guard profile 中都验证并排除递归插桩。默认构建不启用 scalar，且与 inline dirty guard 互斥。

两个具体检查已纳入当前源码：打包必须比较实际 Wasm 的 scalar profile 与 build metadata 的布尔身份；只有 bulk 写入的合法 scalar profile 允许 `scalarHookStores=0`，同时要求该计数等于 stores、各宽度计数总和一致。实现见[插桩器](../../../tools/rollback/instrument.mjs)、[生产构建](../web/scripts/build-sdl-application.mjs)和[打包检查](../portable/package-eagler.mjs)。

| 已完成验证 | 实际结果 | 证明范围 |
| --- | --- | --- |
| `instrument.test.mjs` | 33／33 PASS，0 skip | 宽度、operand 副作用／嵌套、未对齐、边界和 offset trap、ABI／签名／别名拒绝、hook 排除、bulk-only profile |
| 真实 C++ Wasm memory suite | 4 profiles ×（15 scenarios＋2 traps）＋1 raw rejection＝69 个实例 PASS | allocator／journal 恢复、root／heap 边界、失败语义与真实 Wasm trap；scalar profile 路由 1286 个静态 store 位置 |
| 关闭 journal 的对象隔离 | 4 个实际 Wasm 对象、0 个定义符号 | 未定义宏和宏为 0 时均不引入 allocator／journal 符号 |

上述 33／69 测试的真实日志与 final 保留原身份。新 scalar 生产 fixture 随后实际运行了五组常规、一组生命周期、一组 presentation oracle，七组全部 PASS；独立审核 **21,946 个条件、3,600 个后续语义帧、15,480 个 Replay frame-seat samples**。它沿用完整语义、全分辨率配对像素、实际纹理修改／恢复与 Replay 检查，并增加实际 scalar metadata、ABI 1、导出类型与函数体身份核验；1,240 个读取文件在审核结束时身份未变。

这些 fixture 直接调用真实 native `mp_tick`，不替代生产 rAF 调度的矩阵观察。presentation 在选定模拟帧扫真实 alpha 0／0.5／1；600 个未来比较为语义比较，不是 600 次 framebuffer readback。生命周期覆盖 native stage／ending／results owners，不扩大成完整剧情脚本和在线 ACK retirement 的验收。

generic 仍为默认，scalar 仍需显式启用。下一候选已以 [`edf56864`](https://github.com/Ritosa2333/eagler-touhou/commit/edf56864f42c48e6688309914b8eef4a3c949841) 推送，将 presentation 的 owner／tag／shield 遍历改用只读 `visit_children`。它等待单独构建与验证，不计入本文任何 oracle 或性能结果。

## 新一轮原始文件身份

本节列出的是当前实际存在并已独立读取的原文件，全部 SHA-256 由完整字节计算。机器记录另列十二份原 `receipt.final.json`、每场 final、两套生产 identity、脚本与派生分析的完整身份；这里只公布身份和聚合，不公布 raw 输入／Replay 内容。

| 原文件 | SHA-256 |
| --- | --- |
| `scalar-01-oracle-execution-flow.final.json` | `36cd2199ca01d5d4dc04c79d9e8278e79c8387b29ea8b2da20e1606c50b5cd80` |
| `scalar-01-independent-oracle-audit-01.json` | `53aff8ab51cb7919039a5a22f15e8c19f8a75d10386a689ec4077411531b52af` |
| `scalar-hook-micro-05/measurement.final.json` | `df5f533515c4ac799dfb830920b49a12889b738652010b8cc728d272f3e85417` |
| `scalar-hook-micro-05/measurements.jsonl` | `f48d11600b5092b73e85cecefab9bd1ed65e076f9fec09c12cc2c75d3e200d17` |
| `scalar-hook-micro-05-independent-audit-01.final.json` | `252a744a7732abcace66d46f8eff802fc75f4f97a0a1f8653c853d1431cb50d5` |
| `scalar-profile-01-paired-matrix.final.json` | `8d448e00da6437f4fb6ac98ec490741ba3332cd879e8be3faefd3dbd7500ffcd` |
| `scalar-profile-01-independent-audit-01.json` | `267734561e2bd92663def058cd5cb2316d8f2416d99fce0b0d4954304462f8d0` |
| `scalar-profile-01-save-association-analysis-01.json` | `9aaaf40b330ec79fbd36aeae66ab60aee5cc36a79c0d58a48ddab4d0b46e65cf` |

## 此前一轮：精确堆计数，旧 87／Xeon

本节保留此前 `496b0c30 → 87d10f5a` 一轮的实际工具转录。其丢失的矩阵／诊断 raw 没有恢复，也没有用本文 EPYC 新文件或新 SHA 替代。此前一轮的保存边界、CPU／GL 归因和局部算法理由仍有参考价值，但与上面同源 scalar 比较各自独立。

### 版本与证据

| 对象 | 源码 | 实际产物／范围 |
| --- | --- | --- |
| 此前矩阵基线 | `496b0c30` 重新构建 | Wasm `9b079349…`，6,023,458 字节；堆修改后使精确用量缓存失效 |
| 此前矩阵候选 | `87d10f5a`，包含 `55bffb97` | Wasm `877e5039…`，6,023,838 字节；按真实 chunk 大小维护精确用量 |
| 两场附加 CPU／GL 诊断 | 同一 `87d10f5a`／`877e5039…` | 显式开启采样和 GL 包装，不参与正式性能配对 |

此前矩阵的 harness 固定为 `87d10f5a`。两套官方构建清单中的 266 个编译源文件和 357 个头文件，经核对仅 `SimulationHeap.cpp` 实现与 `WriteJournal.hpp` 说明发生变化；feature 和 netplay 配置相同。重新构建的基线 `9b079349…` 与更早的历史 `9d71c176…` 不同，差异原因未建立，不能把它们当作同一二进制。

**证据边界：** checkpoint 02 真实归档保留了两套冻结生产包、七组 oracle 和前矩阵脚本；703 个 ZIP 成员 CRC、清单中 702 个成员 SHA 均通过。归档阶段为 `pre-performance`，包含 **0 个正式性能结果、0 个 save-only 结果**。后续矩阵、保存定向回归及 CPU／GL 诊断的原始逐帧文件、最终回执和 profiles 未存入该归档，本节性能与诊断数值来自此前实际工具输出的转录。本文没有重建这些原件，也没有宣称可以从现存 raw 重新审核它们。上述转录与本文新一轮的真实原件分别标注，旧 SHA 不作为新文件的身份。

### 此前五个有效配对

每种模式／人数按基线、候选顺序各测一次，条件顺序为 `(1,2)、(0,2)、(2,2)、(1,3)、(0,3)、(2,3)`。暖机至少 180 个原生帧，随后固定观察 15 秒。表内范围是同场各席最小值至最大值；百分比按 seat 0、1、2 顺序列出，不是重复试验的置信区间。

| 场景 | 基线逻辑帧／秒 | 候选逻辑帧／秒 | 各席变化 | 回调 P95：基线 → 候选（ms） | 最长画面间隔：基线 → 候选（ms） |
| --- | ---: | ---: | --- | --- | --- |
| 双人纯延迟，mode 1 | 20.13–20.27 | 12.00–12.27 | **排除：基线保存证明失败** | 31.8–37.3 → 22.3–107.8 | 657.5 → 775.6 |
| 双人零延迟回滚，mode 0 | 16.20–16.53 | 15.27–15.53 | −4.12%、−7.66% | 41.9–45.2 → 41.6–43.1 | 617.3 → 431.7 |
| 双人 2 帧延迟＋回滚，mode 2 | 18.87 | 19.87 | +5.30%、+5.30% | 47.8–49.0 → 41.4–45.6 | 434.0 → 407.3 |
| 三人纯延迟，mode 1 | 15.00–15.07 | 15.40–15.60 | +2.67%、+3.54%、+3.54% | 34.8–47.4 → 34.4–38.0 | 305.4 → 310.5 |
| 三人零延迟回滚，mode 0 | 7.40–8.07 | 5.40–5.93 | −21.62%、−25.21%、−33.06% | 52.9–62.4 → 69.5–160.0 | 873.1 → 1231.2 |
| 三人 2 帧延迟＋回滚，mode 2 | 11.47–11.60 | 11.27–11.80 | +2.91%、0%、−1.74% | 59.8–70.3 → 43.8–58.3 | 484.1 → 438.2 |

双人纯延迟基线确实完成了计时窗口，所以保留其已观察的数值；它在后置保存证明中失败，不能进入配对比较，也不能记为 0 FPS。候选六场、基线五场完成完整输入／保存检查，合计 28 个席证明、26 个可配对席窗口。此前独立审核 v2 报告 402,339 项检查、0 差异；这里的审核一致性 PASS 不改变原执行的 1 场 FAIL，也不代表性能通过。

#### 此前计时与环境

新逻辑进度使用最大 `NextFrame` 水位，只统计固定窗口结束前完成的回调，重演和倒退不重复计入进度。回调 P95 包含在窗口内开始的完整原生回调耗时，包括越过结束边界的部分；画面和前进水位的最后间隔在窗口边界截断。画面提交是向默认 framebuffer 的颜色 blit，不是 GPU 完成或显示器扫描频率。实现见[指标计算](../portable/multiplayer/performance-browser-metrics.mjs)。

此前一轮主机为 Xeon Platinum 8573C，容器可见 9 个逻辑 CPU，配额为 8 核、内存上限 8 GiB；浏览器后端为 SwiftShader。每席独立 Chrome，通过真实 relay 发送输入，发送端 FIFO 增加单向 50±5 ms 延迟，名义附加 RTT 为 100 ms。实际定时器迟到另行观察，不能把名义值当成实测 RTT。

CPU 4× 使用 CDP `Emulation.setCPUThrottlingRate`，绑定实际 Runtime iframe 的执行上下文和所属 target，并在窗口后核对同一上下文。它是 renderer slowdown 设置，不是物理 CPU 频率测量，也不限制 SwiftShader GPU 进程的 CPU 使用。此环境与用户的 i7-14650HX／Intel UHD 硬件渲染不同；两组数字不能直接互相替代。代码见[环境核验](../portable/multiplayer/performance-browser-environment.mjs)。

#### 此前长间隔与观察器

三人零延迟候选最大的画面间隔发生于 seat 1：

| 间隔 | 与完整原生回调重叠 | 与回调后观察器重叠 | 在已观察回调之外 |
| ---: | ---: | ---: | ---: |
| 1231.2 ms | 466.9 ms | 2.5 ms | 761.8 ms |

同场 seat 0 的完整回调最大值为 1207.3 ms，三个席的观察器 P95 为 9.9／9.3／5.3 ms。三人纯延迟候选也有 310.5 ms 的最大画面间隔，其中 26.9 ms 在回调内、283.6 ms 在其外；该间隔与真实保存事务重叠约 256.3 ms。时间重叠不能证明整个停顿由保存造成，纯延迟的纠正、恢复和历史帧计数均为零，也不能把全部停顿归给回滚。

这些分解是墙钟区间求交，不能进一步拆成模拟、restore、render、GPU 或操作系统调度 CPU 时间。每个条件只有一个实时窗口，游戏前进量、实际输入到达、历史工作量和周期保存重叠均可能不同，因此整局结果没有形成各场景稳定提升的证据。

### 精确 chunk 增量计数为什么成立

生产外壳的 `onGameFrame` 会调用 [multiplayer.frame](../web/sdl-runtime/multiplayer.mjs)，读取 `_th15_mp_rollback_status`；[状态导出](../web/cpp/sdl/MultiplayerRollback.inc)调用 `journal::Stats()`，再读取模拟堆用量。性能观察器也使用同一生产导出。`496b0c30` 的缓存可以复用两次查询之间未修改的堆，但一次分配或释放仍会使下一次查询重新遍历 allocator chunks。

[`55bffb97` 的改动](https://github.com/Ritosa2333/eagler-touhou/commit/55bffb97ee0693ff055a83e4dfaf7449e1e13cdd)保留精确 `mallinfo.uordblks` 口径。首次查询建立缓存；成功的 `malloc`／`memalign` 加上分配完成后的 `chunksize(mem2chunk(pointer))`；`free` 在合并前取得原 chunk 大小，再减去该值。这里使用 allocator 实际块大小，包含对齐、舍入及元数据影响，不能改为累加调用方请求的字节数。

该等式依赖当前固定外部 segment、单线程、关闭 morecore／mmap／mremap／footers／locks 的 allocator 配置；源码以静态断言固定这些条件。用量缓存属于 host，不进入回滚历史。恢复、清空、失败、切换 mspace 或算术边界异常会使缓存失效，下一次查询重新读取真实 allocator 状态。若 allocator 内部的 journal 写入触发失败，分配函数返回后也不能重新激活已失效的缓存；更新前后都检查有效位和空间身份。实现与反例见 [SimulationHeap.cpp](../../../common/src/netplay/SimulationHeap.cpp)、[缓存测试](../../../common/tests/simulation-heap-cache-test.cpp)和[真实 journal 测试](../../../common/tests/write-journal-test.cpp)。

优化减少的是普通堆修改后统计查询的扫描工作，没有改变被测逻辑帧定义、输入工作量或计时窗口。它仍不能消除首次查询／restore 后重建、逐写入 hook、历史 Update／Draw、软件 GPU 及调度竞争。五对整局结果显示的混合变化必须与这一局部算法改进分别判断。

### 此前保存证明与旧快照边界

完整完成的 28 个席证明均在第一次稳定取证通过；2,061 个实际 Host 输入全部获得 ACK。此前审核解析了 196 份 Replay 观察、269,773 个 frame-seat samples，包含重复快照；对应 84 个不同磁盘 Replay 路径。当时在原 PID 命名空间核实 117 个记录浏览器 PID 均已消失。

被排除的双人纯延迟基线报错为 `Confirmed Replay does not cover the required proof interval`。固定中立区间是 `[537,539)`，seat 0 的保存 Replay 只有 537 帧，seat 1 为 539 帧；两份文件共同的 537 帧、12,888 字节输入前缀一致。缺失的两帧不能用于证明输入不一致。

seat 0 周期保存 token 2 在 iframe 时钟 60193.2 ms 发起，真实 `FS.syncfs` 区间为 60390.3–60467.2 ms；Host `baseline-sync` 在同一时钟 60455.7–60467.9 ms 加入这个尚未完成的事务。[保存控制器](../web/sdl-runtime/score-save.mjs)对已有请求返回 `pending.promise`，所以 Host 正确等到了旧事务持久化完成，却取得一个早于固定中立屏障的快照。该场在并发 fire 和稳定取证 ledger 之前失败；原 FAIL、固定区间和配对排除均保留。

`87d10f5a` 的[稳定取证收集器](../portable/multiplayer/performance-persistence-proof.mjs)处理后续 evidence copy 被新保存替代的另一处边界：固定总期限 35 秒、每席最多三次，完整保留每个 attempt；只有明确的新保存替代证据才允许重新取得完整 Host sync → copy → MEMFS／IDB 事务。原 native 15 秒超时、字节相等、generation、原确认输入前缀和 Replay 检查不变，也不关闭自然保存。单独 save-only 回归曾观察到第三席 `superseded → stable`，它不替换矩阵中的失败。中立基线要在将来的新运行中证明保存快照覆盖既定屏障，不能缩短证明区间来接受旧快照。

原失败回执的 `thresholdsMet` 键没有生成，并非显式赋值 `null`。审核 v1 曾把它误判为应等于 `false`；v2 限定在有真实提前异常、尚未进入阈值汇总的路径保留缺键，10 个离线正反例通过。原审核失败身份与修订说明均保留在机器记录，未升级原执行结果。

### 此前两场 CPU／GL 诊断

两场诊断均使用旧候选 `877e5039…`，开启 4,000 μs CPU 采样和 GL API 包装；`performanceEligible=false`、`thresholdsMet=null`。五席的输入、Replay、native／FS／MEMFS／IDB 检查当时通过，333 个 Host 输入均获得 ACK，五次保存都是首次 stable。诊断审核 v2 报告 73,903 项检查、0 差异，不参与上述五对正式比较。

#### CPU 初步样本

下表来自已经实际打印的初步分析，完整 CPU 综合报告没有成功完成。比例使用非 idle 的样本权重作为分母；权重是采样时间间距归给采样栈，不是测得的函数 CPU 执行时间。`inclusive` 包含子调用，列之间会重叠，不能相加。

| 诊断／席 | touch self | touch inclusive | capture self | status self | AnmDraw inclusive |
| --- | ---: | ---: | ---: | ---: | ---: |
| 2 人 mode 2／0 | 5.50% | 9.36% | 4.02% | 1.41% | 35.27% |
| 2 人 mode 2／1 | 8.50% | 13.80% | 5.46% | 1.12% | 36.89% |
| 3 人 mode 0／0 | 11.28% | 17.60% | 6.36% | 2.19% | 39.25% |
| 3 人 mode 0／1 | 8.45% | 12.98% | 4.63% | 1.48% | 40.84% |
| 3 人 mode 0／2 | 10.25% | 14.68% | 4.51% | 1.30% | 40.76% |

这些样本支持同时检查写入 hook 热路径和绘制路径，不能推出各写入宽度、外部地址命中或 dirty 命中的实际频率。三人 seat 1 原 profile 有一项 −2 μs 的负 delta，位于所用保守窗口之外；记录保留该异常，不据初步表宣称完成 allocator／GC／JIT 的完整排名。

#### GL、进程 CPU 与 cgroup

以下是两场此前 `877e5039…` 诊断曾实际完成的 GL／资源计算转录。CDP 进程计数区间约 17.1／17.7 秒，包含 profiler 启止，不能写成精确 15 秒窗口的 CPU 占用。

| 场景 | GPU 进程平均 CPU 核数 | Renderer 平均 CPU 核数 | GPU 占匹配浏览器 CPU |
| --- | ---: | ---: | ---: |
| 2 人 mode 2 | 6.044 | 1.772 | 76.59% |
| 3 人 mode 0 | 5.496 | 2.347 | 69.20% |

GPU 进程在这里运行 SwiftShader；表中为它消耗的 CPU，不是硬件 GPU 时间。Host Node 平均约 0.022–0.028 核。原 cgroup `cpu.max=800000 100000`，资源观察的内外区间如下：

| 场景 | 观察区间 | 平均 CPU 核数 | 节流周期／总周期 |
| --- | --- | ---: | ---: |
| 2 人 mode 2 | 内括约 14 秒 | 7.9978 | 140／140 |
| 2 人 mode 2 | 外括约 16 秒 | 7.9954 | 157／160 |
| 3 人 mode 0 | 内括约 14 秒 | 7.9991 | 139／140 |
| 3 人 mode 0 | 外括约 16 秒 | 7.9955 | 157／160 |

这些区间显示接近配额的 CPU 负载和频繁节流，但没有把某次节流与某一个游戏停顿建立因果对应。

完整原生回调内，被包装 GL 调用的累计墙钟比例为双人 9.33%／7.96%，三人 7.37%／11.64%／10.48%。五席测量阶段最大的单次 GL 调用分别为 13.3／15.7／12.5／13.9／17.0 ms，没有达到 25 ms。GL API 的同步墙钟不包括全部异步软件 GPU 工作，不能用这组小比例否认 GPU 进程的 CPU 消耗。

五席 measure 阶段纹理调用与估算上传字节均为零，所有阶段 `readPixels` 为零；每席 warmup 有 165 次 `texImage2D`，估算上传 138,875,392 字节。这一结论限定于这些已暖机窗口，不覆盖新字体、资源切换或整个流程。

原两场控制台另记录 4／6 条未带 URL 的资源 404，均在测量开始前。诊断审核 v1 额外要求 `consoleErrors=[]` 而失败；v2 按实际 observer／input／save 检查范围保留全部错误，没有声称它们无害或所有网络资源已成功加载。

## 复现与恢复


### 硬件 GPU CPU 4× 观察

先按[开发指南](DEVELOPMENT.md)准备 Node、Playwright、Chrome、launcher／relay 依赖和输入文件。使用已冻结且具有 `runtime-files.json` 的生产目录，并让每场输出使用一个全新的目录。下载 [touhou.vip 的 TH15 数据](https://touhou.vip/games/th15/th15.dat)，将 `TH15_TEST_ARCHIVE` 指向本地文件并记录其实际 SHA；本轮 DATA SHA 为 `f3802e71db30f99b86d61b6e98b8325716c5cbae892547c563dded7837862ae7`。

```bash
export EAGLER_CHROME_PATH=/path/to/chrome
export TH15_TEST_ARCHIVE=/path/to/th15.dat
export TH15_TEST_FONT=/path/to/msgothic.ttc
export TH15_TEST_UNICODE_FONT=/path/to/unifont.otf

node ports/th15-mp/portable/multiplayer/performance-browser.mjs \
  --runtime=/path/to/frozen/runtime \
  --mode=0 --players=2 --duration=15 \
  --cpu-throttle=4 --renderer=hardware --headed \
  --diagnostic=none \
  --output=/path/to/new/result
```

实际有窗口参数是 **`--headed`**，输出参数是 **`--output`**。hardware 路径关闭软件回退许可，并核验当前 `Module.ctx` 所持有的游戏 WebGL2 上下文与浏览器活动后端；不匹配就失败，不重建上下文或静默换用 SwiftShader。CPU throttle 覆盖同源 iframe 所在 page target；若 Runtime 属于独立 target，则覆盖实际 Runtime target 并记录对应身份。

本报告的软件渲染条件使用 `--renderer=swiftshader`，默认无窗口。正式配对必须使用相同 driver、参数和输入，依次运行上述六种条件的基线／候选，保存每一次成功或失败，并在计时期间暂停其它构建、浏览器和重型审核。Chrome／Runtime／输入文件身份、逻辑帧水位及保存结果都必须保留，不能用显示回调频率替代逻辑帧率。

CPU／GL 诊断另开目录并使用 `--diagnostic=gl-cpu`；自然保存替代回归另开目录，使用 `--save-validation-only --save-evidence-supersession-test`。这两类输出都不能进入正式性能配对。

### 已归档原件

[checkpoint 02](https://chatgpt.com/library?file_id=libfile_71d32a5d02d88191b921310af57891ec&account_id=personal)文件名为 `th15-cpu4-validation-checkpoint-02-20261024.zip`，SHA-256 为 `9dd54a5e5cfbf04c0eac10a0de1ede36c685f2e2347d1034b39aa9e855536a4b`。恢复时先核对 ZIP 身份、全部成员 CRC 和 `archive/member-index.json` 的逐成员哈希，再按安全相对路径提取；保留版本、失败标记和存档阶段。输入 DATA、字体正文、SDK 与浏览器不在归档内，按输入清单另行准备。

该存档可以恢复此前两套构建与旧七组 oracle 原件，不能恢复旧矩阵和旧诊断 raw。本文新一轮 scalar 原件来自之后实际重新执行的构建、oracle、微基准与矩阵，使用各自新身份；没有给旧转录补造回执。

后续 [checkpoint 03](https://chatgpt.com/library?file_id=libfile_920e2926ca5c819182ad9cd4fcb3c462&account_id=personal)保留当前 33／69 测试的真实日志、final 和结果，以及明确标注的性能／诊断转录。文件名为 `th15-scalar-validation-checkpoint-03-20261024.zip`，251,026 字节，SHA-256 为 `2faf03b0162d6c0e4fa739a41fca46b5baf21488b39beac532b21917f27269ac`。它不包含丢失的旧矩阵／诊断原始文件，也不包含本文新 scalar 七组 oracle、微基准和矩阵的原件。

当前新一轮原始文件及完整审核结果保留在既有运行目录，本文公布聚合及完整 SHA，不发布原始输入／Replay。checkpoint 04 的外部归档被自动审批拒绝，需用户明确授权；因此本文不提供其链接，也不把本地原件写成已完成外部归档。
