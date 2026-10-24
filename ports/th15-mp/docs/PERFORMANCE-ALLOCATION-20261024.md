# TH15 MP 绘制准备分配优化与 CPU 4× 实测（2026-10-24）

**十二场输入、Replay 和保存检查全部通过，六组均可配对；CPU 4× 下，30 个席窗口仍全部低于 60 逻辑帧／秒。** 本轮保留默认 generic journal hook，减少多人绘制准备中的临时容器分配。双人 2 帧延迟回滚的逻辑进度提高 7.62%–8.78%，三人两种回滚场景提高 5.60%–10.22%；双人零延迟结果混合，纯延迟没有改善。

**三人最坏画面间隔同时恶化：零延迟从 635.3 增至 879.7 ms，2 帧延迟从 411.9 增至 512.0 ms。** 本轮不能据此宣称稳定流畅、达到 60 帧，或已经解决长停顿。每个条件只运行一次，百分比描述这次固定配对，不含重复试验的置信区间。

本报告接续 [CPU 4× 与 scalar hook 报告](PERFORMANCE-CPU4-20261024.md)，使用新一轮保留完整原始文件的测量，没有合并旧 scalar 数据或更早丢失原件的转录记录。[机器记录](PERFORMANCE-ALLOCATION-20261024.json)保存全部逐席聚合、原判定、取证统计和完整文件 SHA。本文不发布原始输入或 Replay 内容。

## 改动与比较边界

候选包含两个绘制准备改动：

- [`edf56864`](https://github.com/Ritosa2333/eagler-touhou/commit/edf56864f42c48e6688309914b8eef4a3c949841)增加只读 `AnmRegistry::visit_children`，在多人 owner、opacity tag 和 Reisen shield 的遍历中直接访问现有 children，保持原顺序，避免每次生成临时 vector。回调不得修改 registry；需要边遍历边修改的调用仍使用原快照接口。
- [`eb2485a6`](https://github.com/Ritosa2333/eagler-touhou/commit/eb2485a6ec77ccdf83e2de3a20c3cf3f4fa7578a)复用活跃 `multiplayer_player_opacity` map 节点。准备开始时用第 8 位标记旧项，重新访问的项写入本帧 opacity，结束时清除未访问项；值类型扩大为 `u16`，完成准备后仍只暴露 0–255。这样可保留活跃节点，同时移除已分离 VM 或切换视角留下的旧覆盖。

实现见 [AnmRegistry.hpp](../web/cpp/game/AnmRegistry.hpp)、[AnmRenderer.hpp](../web/cpp/game/AnmRenderer.hpp)及 [MultiplayerPilot.cpp](../web/cpp/game/MultiplayerPilot.cpp)。这些改动减少了代码路径中的容器构造／重建；正式窗口没有记录逐次 allocation 数量，不能把整局百分比直接解释成分配器省下的 CPU 时间。

| 身份 | 固定源码／Wasm SHA-256 | 字节数 |
| --- | --- | ---: |
| 基线生产源码 | `792f029084a405d4ec8ec3c927c64ecf534f6e85` | — |
| 候选生产源码 | `eb2485a6ec77ccdf83e2de3a20c3cf3f4fa7578a` | — |
| 两组测量 harness | `d34957521c5d8f987a9d2645df9b59c353824973` | — |
| 基线生产 Wasm | `622f1e9392b98875d461a3c7da0fbee3c471041c4225f9e50e2b31782e9cafa2` | 6,025,407 |
| 候选生产 Wasm | `f15653a9032efb28860eedf4feb79062a553692287d1b9ea1f4764a1f7542911` | 6,024,982 |
| 候选游戏 oracle fixture | `969a8e7e52a9f2105408b4a39e47e8dbdc05a8771832499ae8b548b74ef3eed1` | 见机器记录 |

生产 manifest 的改动范围是 **1 个 cpp、2 个 header**：`MultiplayerPilot.cpp`、`AnmRegistry.hpp`、`AnmRenderer.hpp`。每包 266 个对象，4 个对象字节改变，262 个不变；变化对象是 `AnmRenderer.cpp`、`MultiplayerPilot.cpp`、`AudioDevice.cpp`、`GraphicsImages.cpp`。编译 flags、loader、资源和实际 generic profile 逐项相同。源码版本及相应架构 revision 字段按各自版本核验，未宣称两包整个目录逐字相同。

两包都没有启用 scalar hooks 或 inline dirty blocks；generic 继续作为默认。此前 scalar 的局部收益、微基准及失败场次仍留在[原报告](PERFORMANCE-CPU4-20261024.md)，不作为本轮 allocation 改动的成绩。

## 固定的十二场性能观察

本轮在 **AMD EPYC 9V74 80-Core Processor** 的同一当前环境中执行，容器可见 9 个逻辑 CPU，`cpu.max=800000 100000`，即 8 核配额。CPU 型号来源为同环境的实际 micro receipt；各性能 receipt 另行核验真实 Chrome、Runtime context、SwiftShader 和 CPU throttle。它与用户 i7-14650HX／Intel UHD 硬件渲染属于不同条件，不能直接比较绝对 FPS 或跨环境计算收益。

正式运行区间为 **18:01:55.174–18:11:32.601 UTC**。每席独立 Chrome，CPU 4×、SwiftShader、headless、render scale 1、15 秒固定窗口、未开启 profiler。relay 每方向发送延迟为 50 ms、jitter ±5 ms，名义附加 RTT 100 ms；真实 RTT 会受调度影响。CPU 4× 是 CDP renderer slowdown，不等于物理 CPU 降频，也没有把软件 GPU 进程按相同比例降速。

顺序固定为 `(mode 1,2P)、(0,2P)、(2,2P)、(1,3P)、(0,3P)、(2,3P)`，每条件先基线、后候选，各一次，没有重跑或替换。mode 1 为 2 帧纯延迟，mode 0 为零延迟回滚，mode 2 为 2 帧延迟加回滚。表内 Hz 和 P95 区间为同场各席范围；变化按 seat 0、1、2 顺序排列，最长画面间隔取该场所有席最大值。[完整逐席数据](PERFORMANCE-ALLOCATION-20261024.json)保留未舍入值。

| 场景 | 基线逻辑帧／秒 | 候选逻辑帧／秒 | 各席变化 | 回调 P95：基线 → 候选（ms） | 最长画面间隔：基线 → 候选（ms） |
| --- | ---: | ---: | --- | --- | --- |
| 双人纯延迟，mode 1 | 33.40–33.47 | 32.67–32.73 | −2.00%、−2.39% | 12.7–12.9 → 12.5–13.9 | 131.6 → 181.6 |
| 双人零延迟回滚，mode 0 | 26.20–27.27 | 26.33–26.53 | +0.51%、−2.69% | 33.5–36.7 → 31.6–32.1 | 468.3 → 367.9 |
| 双人 2 帧延迟＋回滚，mode 2 | 27.33–28.00 | 29.73–30.13 | +8.78%、+7.62% | 34.3–34.6 → 29.3–31.3 | 393.2 → 303.3 |
| 三人纯延迟，mode 1 | 22.93–23.00 | 23.00–23.07 | +0.58%、0.00%、0.00% | 20.4–26.7 → 21.6–23.0 | 207.7 → 224.9 |
| 三人零延迟回滚，mode 0 | 11.87–12.53 | 12.87–13.67 | +10.22%、+8.43%、+7.98% | 43.2–48.4 → 37.6–41.2 | **635.3 → 879.7** |
| 三人 2 帧延迟＋回滚，mode 2 | 15.00–15.47 | 16.07–16.33 | +7.11%、+5.60%、+8.89% | 40.5–46.8 → 37.5–43.9 | **411.9 → 512.0** |

独立 Python 审核首次运行得到 **549,041 项检查、0 差异**，复算全部 30 个席窗口并保留原执行判定。十二场原 `performanceMeasured`、`observationPassed`、`saveValidationPassed` 都为 true；原 `thresholdsMet` 全为 false。六组均具备完整来源、输入与保存证明，可以配对。**0／30 席达到 60 逻辑 Hz，0／12 场通过 100 ms 回调／画面／前进水位门槛。** 审核 PASS 表示证据及计算一致，不等于性能通过。

逻辑进度沿用最大 `NextFrame` 水位，回滚重演或倒退不重复计帧。回调 P95 统计窗口内开始的完整原生回调；逻辑水位只读取在窗口内完成的回调。画面和前进水位间隔按固定窗口边界截断。画面提交指默认 framebuffer 的颜色 blit，不等于 GPU 完成或显示器扫描输出。指标定义和计时 probe 未随保存关联修复改变，见[指标实现](../portable/multiplayer/performance-browser-metrics.mjs)及[测量驱动](../portable/multiplayer/performance-browser.mjs)。

## 三人长间隔与观察器边界

最大画面间隔的墙钟分解如下。前三项互斥分区；保存重叠另列，不能与它们相加，也不能直接认定因果。

| 场景／席 | 总间隔 | 原生回调重叠 | 回调后观察器重叠 | 已观察回调之外 | 已记录保存区间重叠 |
| --- | ---: | ---: | ---: | ---: | ---: |
| 3P mode 0 基线，seat 0 | 635.3 ms | 220.1 ms | 12.6 ms | 402.6 ms | 0 |
| 3P mode 0 候选，seat 0 | 879.7 ms | 50.0 ms | 4.1 ms | 825.6 ms | 0 |
| 3P mode 2 基线，seat 2 | 411.9 ms | 321.7 ms | 15.0 ms | 75.2 ms | 0 |
| 3P mode 2 候选，seat 2 | 512.0 ms | 173.7 ms | 1.4 ms | 336.9 ms | 199.8 ms |

候选三人零延迟 seat 0 的 879.7 ms 画面间隔没有与已记录保存区间重叠；同席最长前进水位间隔为 907.8 ms。候选三人 2 帧延迟 seat 2 的长间隔与自然保存 token 1／FS 1 重叠 199.8 ms，重叠本身不能证明保存造成整段停顿。纯延迟对照也出现超过 100 ms 的间隔，其历史、纠正、恢复计数为零。

三十席观察器 P95 为 0.6–3.6 ms。三人零延迟的基线为 3.2／3.0／2.3 ms，候选为 2.6／2.8／3.6 ms；这些已计入原始观察，不通过删减观察器换取候选的表面收益。

实际运行的历史工作量也会变化。双人 2 帧延迟基线前进 410／420 帧、重演 157／145 帧；候选前进 446／452 帧、重演 141／123 帧。三人零延迟基线前进 186／178／188 帧、重演 161／193／173 帧；候选前进 205／193／203 帧、重演 171／131／167 帧。正式观察器不能把回调墙钟进一步拆成 simulation、restore、render、软件 GPU 或 OS 调度的独立 CPU 成本，也不能把回调之外的剩余时间指定给其中某一项。此次没有新 CPU／GL profiler 窗口，不套用上一包的 profile 分摊新包耗时。

## 输入、Replay 与真实保存

十二场共提交 **2,172 个 Host 输入，全部 ACK，调用错误为零**。30 个席的完整保存证明全部建立；30 个 attempt 均在第一次取得 stable 结果，没有被替代 attempt 或运行重试。审核重建并核对 **120 条 native token → completion → FS.syncfs completion 链**。

Replay 核对涉及 **210 份观察、90 个不同磁盘文件、371,118 个含重复快照的 frame-seat samples**；另外的 MEMFS／IndexedDB 内存观察按实际来源分别验证，没有把重复快照当独立游戏帧。固定输入结束帧、session／seat／ABI、已有输入前缀、native 校验结果，以及 evidence copy／MEMFS／IndexedDB 字节一致性全部保留。原生 validator 的记录与实际字节关联由独立审核检查，离线审核没有重新执行原生 validator。

矩阵所有 exclusive `receipt.final.json`、case final、生产输入和最终文件身份在运行结束及独立审核结束保持一致。原 `pid:[4026532200]` 命名空间中的 **120 个记录浏览器 PID 全部退出**，无活动匹配进程；清理仅只读检查，没有杀进程。

### d349：用 Runtime 自己的时钟关联保存

此前 same-source scalar 矩阵首场因约 0.1 ms 的 Host／Runtime 跨时钟边界差异失败，该原 FAIL 仍保留在[原报告](PERFORMANCE-CPU4-20261024.md)。新 [`d3495752`](https://github.com/Ritosa2333/eagler-touhou/commit/d34957521c5d8f987a9d2645df9b59c353824973)在计时窗口结束后、Host sync 请求前及 ACK 后，同步读取同源 Runtime 的 `performance.now()`、`timeOrigin` 和保存事件序号。新事务必须在真实 start／ACK marker 的序号区间内完成，并匹配 request、label、时钟源与保留 marker；ACK、copy、MEMFS 和 IndexedDB 读取的顺序也检查。

该修复没有扩大旧时间容差、补写历史 marker 或修改旧判定；旧格式仍走原关联规则。native／FS 唯一链、完整字节及输入前缀、固定 proof horizon、最多三次取证及固定总期限均保留。计时 probe、普通 Host RPC、并发输入事务和 metrics 源字节保持不变。

新协议实际通过 **136／136 Node 子测试**；另有一场 **2P mode 1、CPU 4×、5 秒、save-only** 的真实浏览器补充回归，使用基线 `792f0290` 包及 d349 harness，输入／保存通过，独立审核 **12,674 项、0 差异**。该场 `performanceMeasured=false`、`performanceEligible=false`，不计入十二场性能观察，也不替代 eb 游戏正确性测试。

## 正确性与性能分开计数

| 验证 | 实际数量与结果 | 证明范围 |
| --- | --- | --- |
| eb 最终候选游戏 oracle | 7／7 PASS；独立 21,978 条件、0 差异 | 五组常规＋生命周期＋高刷新 presentation 的真实游戏／纹理／Replay 正确性 |
| 707 入口的 targeted visual 脚本 | 3／3 PASS | 原生 headless geometry、VM、RNG、擦弹、护盾及本地视角；不等于 GPU 像素或 FPS |
| Runtime clock Node 套件 | 136／136 PASS | 新 marker／旧格式、错误身份／时间／序号、字节和输入边界 |
| Runtime clock 真实 save-only 补充 | 1 场 PASS；独立 12,674 条件、0 差异 | 真实 token、FS、MEMFS／IndexedDB 与新 marker 的关联 |
| 正式性能矩阵 | 12 场输入／保存 PASS；6 有效配对；30 席未达到 60 Hz | 本报告固定 CPU 4× 条件下的性能结果 |

edf 的早期 visit-only 包另有七组实际游戏 oracle PASS，以及 targeted-03 三项 PASS，没有单独的独立 oracle 审核，不能写成 eb 的新测试。最终 eb 使用新 fixture 和独立 execution flow；原 build flow 仍明确为 `BUILT_ONLY`、零 oracle case。

[`70760824`](https://github.com/Ritosa2333/eagler-touhou/commit/707608247d1ceaf76760acfcda5e0c5cac74f1ba)修正三个 visual 脚本的正式构建入口，使其从实际 build manifest 选取并链接游戏及 WriteJournal／SimulationHeap 依赖。旧构建入口失败保留在证据中。最终三脚本使用相同冻结 eb 对象，运行并验证新增的三项 opacity 断言：分离 focus 不留旧覆盖、新本地玩家去除旧队友淡化、前本地玩家获得队友淡化。没有通过修改游戏断言来规避失败。

## 文件身份与复测

以下均为本轮实际完成文件的 SHA-256；完整清单及每个 case 的 final／receipt 身份见[机器记录](PERFORMANCE-ALLOCATION-20261024.json)。文件 hash 用于核对具体证据，不把审核 PASS 改写为 60 帧达标。

| 文件 | SHA-256 |
| --- | --- |
| `opacity-reuse-01-paired-matrix.final.json` | `5f69303f8e4fc862003a0c29540a3765ee0ba2a418fe545f3de1155b07c2c9fe` |
| `opacity-reuse-01-independent-audit-01.json` | `69af70cb658de14de177176f570faca3342b045afe35312f616b9a22327cc816` |
| `opacity-reuse-01-cleanup-original-namespace.final.json` | `975443f292fe851b04b4f10adafea9fc4ec579802a5eba63ffa68f439dae3561` |
| `run-opacity-paired-matrix-01.py` | `d932f63c08ca9155ede0a94fe5b4896401b5d785f03d5521b83407d78fdc6fd6` |
| `audit-opacity-paired-matrix-v1.py` | `2143e939d8ddd373576d91be2eafa096bf5863cd6d4016ba0a1f75ccda27c521` |
| `opacity-reuse-01-oracle-execution-flow.final.json` | `e2cbd4bec6012401327ee5c8fe96cfb4109885c24219baa020e173a0b0497e45` |
| `opacity-reuse-01-independent-oracle-audit-01.json` | `505cc58fd6091f68852bbd21c85296c1f337ba6983a5a28c9d433f7e77ffc7dc` |
| `opacity-targeted-01/receipt.final.json` | `a96de843b4fdce46eb674df176777d82d90cb950158530ae2c65e53d4f7e84ef` |
| `runtime-clock-save-evidence-tests-01.final.json` | `084e71bf28aa635afa203232aa4ea875dfb1f94912f2db6d46d30108b25e9782` |
| `runtime-clock-save-only-01/receipt.final.json` | `23d9b6466c8ac8022f651e1a71d75d08f2accb8adb46b3c2410c2b57ee9c5382` |
| `runtime-clock-save-only-independent-audit-01.json` | `b85718bf63ff53f9a38c4693f36e9afb1a6186d19c2ade233e93446369931bdf` |

本地复测包 `th15-cpu4-runtime-pair-20261024.zip` 为 **3,536,760 字节、32 个成员**，SHA-256 为 `9495e42392c82b41bf669e0f8b76ae9217b505e11346ddb73aba05d82233bed0`。打包时逐成员字节与 ZIP CRC 核对通过，内容为两套冻结 runtime、README 和 identities；保留原 cp932 表，不含零售 DATA、两份字体正文、输入日志或 Replay。该包和原始测量证据未外部上传。

硬件复测使用实际的 `--headed` 选项，例如在固定 d349 harness、分别指向两套 runtime 时运行：

```sh
node ports/th15-mp/portable/multiplayer/performance-browser.mjs \
  --runtime=/absolute/path/to/frozen-runtime \
  --mode=0 --players=2 --duration=15 \
  --cpu-throttle=4 --renderer=hardware --headed \
  --diagnostic=none --output=/absolute/path/to/new-result
```

按六种条件分别运行、保留每场全部原判定，核对实际 Runtime WebGL renderer 和 CPU throttle；hardware 模式不能静默退回 SwiftShader。将用户合法持有的数据和所需字体按复测包 README 放置，继续检查输入、Replay、真实保存及最长间隔。用户 i7／Intel UHD 的硬件实测将决定是否接近原定 60 帧目标，本轮软件渲染结果没有完成这一验收。
