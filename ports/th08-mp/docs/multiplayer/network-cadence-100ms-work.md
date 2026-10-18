# TH08MP 100 ms 网络条件下的平滑度工程记录

## 当前交付状态（2026-10-12 08:25 UTC）

**v7 本地候选包已生成并完成身份核验，尚未部署或推送。** 当前包的 native 为 47c49ab0、managed 触控 shell 为 a112a6b9，18 个索引文件与 500 个 native 源文件身份已验证；完整路径与 hash 见[第 15 节](#15-v7-本地候选包交付身份)。

[第 14 节](#14-v7-呈现完成门槛实验)已补齐实际结果：原速 100 ms fixture 的端点后保持从 60/55 次降至 12/9 次；正式 CPU4 尖峰样本为 51.349/51.356 Hz，正式原速 100 ms 为 44.148/44.171 Hz。后者 callback/新逻辑 tick 为 17.204/6.391 ms、correction yields 为 513/12，**单端 CPU 成本不对称仍未解释**。这些有限样本不能承诺 CPU4 稳定 60 Hz、无卡顿或整体固定提速比例。

以下保留各阶段当时的状态与失败记录；最新交付与验证以本节及第 14–15 节追加证据为准，旧失败不因新样本而删除。

日期：2026-10-12。状态：**v6 同一冻结包已有一次严重失速 c7（17.593/17.554 logical Hz）与一次通过运行门的部分改善 c7c（49.646/49.664 Hz），仍未达到 CPU4 60 Hz；两次各自最终 canonical 均一致，严重失速根因尚未定位。c7b 与原速 50 ms c6 均为 Not RTC 启动失败、无 body。** v6 owner/固定 worldProbe 与独立 world 八场景已通过；Runtime host 首轮本地服务 404 已修复，当前已保存 2P manual 功能门通过，3P/pure-delay 仍在运行。主测量线程串行执行验证；本文固定来源、失败记录、用户验收边界及下一步。后续结果继续追加，不覆盖旧失败。

工作目录：`D:/workspace/eagler/worktrees/th08-network-cadence-100ms`。本文是该候选树的工作记录，不代表普通主树、用户测试站或线上发布包已经改变。本轮工程操作仅通过 nig，命令、构建、测试与浏览器操作由主线程串行执行；协作者只读源码、证据或编辑指定文件。报告以现有源码、工程记录及已保存实测结果为证据。

## 1. 当前结论与用户目标

用户已经实际体验并认可 TH09：高延迟下会自动降速，大部分时间运动与画面仍然连续，只有很少掉帧。TH08 在用户约 100 ms 的测试中仍出现“FPS 没掉，但子弹和画面运动不顺”的问题。用户已明确本次 TH08 **开启回滚**，无需继续以开关默认值猜测这次体验。

首要目标是同时保留预测回滚带来的低本地输入延迟，以及持续、均匀的新游戏状态呈现。网络信息短缺可以逐渐降速；突然的大卡、长时间完全静止不可接受。优先让降速反映网络信息短缺，不能把设备长期算不过来包装成已完成目标。

已知边界：

- 一端正常暂停、停止产生新输入，另一端用完可预测历史后停下，是用户接受的自然行为；不以持续虚构输入绕过它。
- 正常游戏中的 rAF 长时间未获调度，与回调自身执行很久，是两种不同问题，都应记录。不能把两者统称为回滚计算慢。
- 稳定延迟所需的常态预测空间不应直接触发额外制动。用户提出的“稳定 50 ms 约需 2f”是体验目标的例子，不是无条件的协议恒等式；所需预测帧数还取决于单程/RTT 口径、帧相位、采样发送时机、显式 D 和两端调度。
- TH06MP/TH07MP 已有良好高深度回滚表现，是参照。不能据此假定 TH08 的世界状态、语义 Draw 或资源拥有关系可以原样替换。
- 允许桌面手工或脚本构造触控移动，**不用手机测试**。CPU4 是桌面浏览器的限速设置，不是某款手机的模拟。
- 系统曾有编译等并发活动，不能用一次慢运行过早否定路线；同时不能删掉不利结果，或未经证据就把退步全部归因于环境。本轮重测试与构建由主测量线程串行安排。

当前 v6 必须同时保留两类结果。首轮 c7 两端为 **17.593/17.554 logical Hz，严重未达标**；P1 无 correction/resim，普通 callback 均值 39.383 ms、p99 83.6 ms、最大 147.0 ms，两端 advance 最大 190.8/203.2 ms、>100 ms 为 84/27 次。后续 c7c 复用同一冻结包完成 RTC 测量，两端 **49.646/49.664 Hz**，advance 最大 70.0/101.7 ms、>100 ms 为 0/1 次，Present 最大 60.0/86.8 ms。两次各自最终 canonical 都一致，但 c7 未满足 P1 远端输入回滚覆盖，c7c 则通过运行门；**一次恢复正常不解释或抹去前一次严重失速，仍不能承诺稳定表现**（第 5.8–5.9 节）。

此前通过运行门的 v5 c6c 为 48.759/48.675 Hz，Present 最大 54.4/54.7 ms、advance 最大 61.2/64.7 ms，两类 >100 ms 次数均为零；v4 c5 为 43.866/43.860 Hz，Present 与 advance >100 ms 均各 1/1 次。这些有界改善继续保留。v6 c7c 的推进率略高、累计 callback 墙钟/新逻辑 tick 略低，但长尾并未全面改善，ScreenEffects 独立提速未证明；c7 与旧 v2 c2 的相似严重异常也仍未定位（第 5.2、5.6–5.9 节）。

此前 v3 正式 CPU4 spike 为 39.416/39.450 Hz，同包 cap60 为 41.292/41.257 Hz，均未解决推进不足。v3 原速单程 50 ms 为 59.246/59.272 Hz，单程 100 ms 为 45.191/45.207 Hz；后者没有 >100 ms 新状态空档，但高频 Present 不等于 60 Hz 新状态推进。这些旧样本与失败仍完整保留在第 5.4–5.5 节。

能单独确认的有界结果是 owner probes 全部通过、同一固定真实世界场景中资源 journal 峰值减少 88,784 B（第 7.3 节）、Camera 精确缓存的 24 种算术组合/11,856 项 oracle 检查通过，以及 v5 BackgroundMath 的 12 种模式/310,860 项检查、投影的 24 种组合/43,657 项检查通过（第 7.4、7.7 节）。v6 独立 world 八场景套件现已完整通过，覆盖 2P/3P 的四种模式与 advance=24/60 后续生命周期，仍限于 stable-stage 原生 Update / 语义 Draw 的恢复正确性（第 7.9 节）。数学快路已进入 v5 正式包，组合正式结果见第 5.7 节；v4→v5 有效比较每端只有一个匹配计数窗口，墙钟触控轨迹和恢复工作量不同，未分离每个补丁的独立性能收益。v6 进一步减少 ScreenEffects 的 journal 登记/Touch 粒度，已有 c7 严重失败和同包 c7c 有效测量；不能把 correctness PASS、较少 Touch 块数或单次重测恢复当作独立帧时间收益。2P Runtime host manual 功能门已通过，仍不等于完整 Launcher UI、真实 rAF 性能或公网路由验收。**不能确认 TH08 已达到用户认可 TH09 的体感，也不能承诺 CPU4 稳定 60 Hz、零掉帧或固定总体提速比例。**

## 2. TH09 用户认可版本的真实来源与时间关系

### 2.1 被认可的是 managed-touch 源码树，不是普通主树

用户给出的测试站构建身份：

| 项目 | 身份 |
| --- | --- |
| 源码树 | `D:/workspace/eagler/worktrees/th09-managed-touch-4x` |
| 源码起点 | `63127575d43ea5ef35b317ff8137b00a1ab5b1cc` 加未提交修改 |
| 未提交修改范围 | managed 触控修复、回滚入口固定 D0、亚像素修复 |
| 构建 | 正式 Web Release，`-O2`，`TH09_DEVELOPMENT_HARNESS=0`，音频 SIMD 关闭 |
| common | `D:/workspace/eagler/worktrees/th09-rollback-input-batch-common`，`512353cab9eaa4f6744f867b0eb07c61ee7f2b0e` |
| 产物记录 | `th09_web/artifacts/subpixel-release-20261011-c1/build.json` |
| WASM SHA-256 | `cb03c6df0c460360d2d3548e63556a9b74bf18ac7f6613bdf8231a4e03c30480` |

因此，仅查看 `6312757` 的提交差异，或者仅查看后来 PlayerShots 优化，都无法重建“普通表现到用户满意”的实际变更集合。上述 build.json 位于用户指定 TH09 工作树；不是本 TH08 树中的文件。

### 2.2 在被认可版本之前已经进入的关键变化

以下为已核查历史中的关键顺序。不同阶段的性能样本条件不同，不能把阶段数字连乘成总收益。

| 阶段 / 提交 | 实质变化 | 对目标的意义与限制 |
| --- | --- | --- |
| 2026-10-10，`ea96a16788e81d9937ba95c4fcba7f2dae595844` | 在回调预算退出之前提交实际已经推进到的前进状态；保留每个 tick 的语义 Draw。同步减少可合并渲染提交、复用缓冲、重复投影计算与部分快照工作。 | 修复“计划只画最后一个 due tick，但预算中途退出，前面已经推进的状态一直未呈现”的具体冻结来源。该阶段记录仍有 >100 ms 间隔，不能把它单独描述为完整成功。 |
| 2026-10-10，`0365fb286d872f41e8bf692bbb5b09d8b94d4bf6` | D0 新输入提前一次采样；恢复预算从包括 Pump 的统一起点计；文本重采样工作存储复用；按真实输入缺口渐进制动；完整 VM first-write 页面。 | 提前让对端获得新输入，减少重复记录/分配，并使缺信息时的节奏变化更连续。此阶段报告明确保留不利测量和环境不确定性。 |
| 2026-10-11，`17077dd79291a8c157253b0ea1cf3d11658536e9`，common `512353c` | 把已经 due 的 Pump 普通输入尾包，与紧接着一次性的 frontier 新输入 capture 合并；Restore 或 yield 前必须 flush。控制及可靠修复保留原边界。 | 减少每帧先发旧尾、随后再发新输入的重复工作。既有 900 帧 trace 中两端旧尾/新输入重复发送对从 519→0、321→1；保留的一对符合 repair guard。不是减少协议输入或省略确认。 |
| 2026-10-11，`d6ba14f00e317c6400fee0e7316094cf506d859e` | 固定 Enemy 池 first-write 索引。 | 减少固定拥有者池首次写入判重/定位成本，保持需要恢复的旧字节与 owning-state 语义。不是后来的 PlayerShots 补丁。 |
| 2026-10-11，`a93fb876eb6be6c84c1cf2cf271153ee7fa4763e` | mode0 预测保留持有的触控绝对目标和 free joystick 向量，不再每次预测清零；真实晚到字段仍完整比较与纠错。 | 减少停指但未抬起阶段的无谓回滚。原记录中相同输入的 hold 阶段恢复显著减少，但持续移动每帧改变目标仍会产生真实校正，不能宣称消除触控回滚负载。 |
| `6312757` 起点之后的 managed-touch / D0 / subpixel 修改 | 修复实时触控受异步 managed 命令队列阻塞的路径；在回滚入口使用 D0；保留连续亚像素表现。 | 与前述调度、网络和快照工作共同构成用户认可的版本。用户评价针对整个构建，不足以量化每项独立贡献。 |

上述提交与行为可在 TH09 树中的以下原始记录交叉核查：

- `docs/multiplayer/rollback-4x-performance.md`：`ea96a167` 的选中方案、渲染与预算行为、失败边界。
- `docs/multiplayer/AGENT-HANDOFF.md`：`0365fb`、`17077dd`、common 身份和后续阶段索引。
- `docs/multiplayer/rollback-responsive-work.md`、`rollback-chain-work.md`：早采样、输入发送合并与固定池工作。
- `docs/multiplayer/rollback-mobile-work.md`：`a93fb87` 的完整输入合同、hold 与持续移动的不同负载。
- `docs/multiplayer/rollback-managed-touch-work.md`：managed 实时路径、恢复尾部及其他候选的采用/拒绝记录。

这里没有把文档中曾试验的所有恢复尾部候选都当作被认可构建已采用的代码。特别是不同 worktree 中的补一个 fresh tick、提前一帧网络制动等实验，须按冻结构建身份区分，不能仅因写在同一报告中就归入生产来源。

### 2.3 PlayerShots 是之后的有界优化

后来的 TH09 候选树 `D:/workspace/eagler/worktrees/th09-subpixel-journal-4x` 中，`cc35222ed5f7fe652821cfec8f44e055da0c3b54` 优化 PlayerShots 检查点：活跃连续范围快照与休眠槽位首次写入前保存旧值，替代每检查点的整池复制；`5352dcfa877e30e29b1bf97a72936f69522eda5d` 保存报告。

这是在用户已认可 managed-touch / D0 / subpixel 基线之后的工作，不能作为此前体感跃迁的唯一解释。本 TH08 树的 [rollback-journal-work.md](rollback-journal-work.md) 第 2 节保存了这项后续优化的收益与长尾边界：即使复制工作减少，也没有所有长尾指标一致改善的结论。

### 2.4 TH09 的降速不是“设备问题已经被隔离”的证明

已认可路径使用连续真实远端输入前沿的缺口。现有 TH09 阈值约为缺口 0–4f 不提高目标间隔，5f/6f/≥7f 逐步达到约 1.333/1.667/2 倍，并以既有升/降时间渐变。

这不是按每条网络稳定 RTT 自动学习“应当免费预测几帧”的新算法。缺口还可能来自对端 CPU、输入回调或发送任务迟到。它符合按信息可用性调节节奏的方向，但不能凭缺口制动就断言设备导致的降速已经消失。

## 3. TH08 本轮做了什么，哪些不是性能结论

### 3.1 入口与测量身份

候选 `th08_web/sdl-runtime/multiplayer-host.mjs` 先按原规则验证请求，再把开启回滚的 mode0/mode2 统一为 effective mode0、D0、automatic=false。纯延迟 mode1 保留；配置入口和 shell 经过同一规范化，spectator 要与发送方实际模式一致。没有通过修改共用前端默认值解释用户这次回滚测试。

旧入口实际使用 mode2 自动校准；本轮原速样本最终 D2，CPU4 样本最终 D3。候选样本实际是 mode0/D0。D0 去掉显式本地排队帧数，同时也可能增加需要预测/重演的工作量。因此旧版与候选的正式对照，是用户实际入口策略的端到端比较，**不是只改一行热点代码的相同工作负载微基准**。

真实启动测量使用合法 Lobby admission、每席位 member、资源初始化、实际 configureMultiplayer 与原校准流程。不能继续用硬编码 v3 D0 的简化启动推断旧 mode2/auto 的实际体验。

测量注入器已显式区分真实 Input 包与合法 ADS 包；Probe/Echo 双向接受同样的 one-way/jitter 延迟，使用独立计数与随机序列，不触发 gameplay spike/drop/blackout。其他 ADS 控制消息在当前测量模型中旁路，并在记录中声明；这不等于覆盖了所有启动控制消息的真实网络时延。

### 3.2 v1 / v2 已实施的方向

- 增加与 TH09 对照得到的输入缺口渐进制动、缺输入阻塞期间的调度债务处理。
- 保留新输入在恢复之前一次采样发送的约束；移入 SessionChannel 的普通输入尾包合并。预 rAF Pump 与 Step 用显式一次消费标记衔接，无 capture、错误、等待及回调结束路径必须 flush，不能虚报已发送或提前推进 ACK/repair 计时。
- 把长恢复拆为跨 callback continuation；每块结束先关闭该历史帧 journal，保留完整 Update 与语义 Draw，再允许 yield。恢复尚未完成时不提交半恢复世界；完成后继续遵守预算。
- 统一回调预算起点，涵盖 Pump；同时保留正常 forward 的完成与提交边界。
- 为测量增加独立的 live logical frontier 与历史恢复 cursor，避免将恢复中的 net[3] 当作新状态推进。

这些行为各有具体正确性约束，不能因为 native 单元测试通过，就写成它们组合后的真实游戏平滑度已经改善。

**本轮有效候选记录为 mode0/D0，使用现有 v4 输入语义。** 旧 mode2 的 v6 gate 等假设不能拿来解释当前 mode0 测试中的退步。凡涉及协议或入口的解释都必须以记录的 effective timing/native mode 为准。

## 4. 测量口径：FPS 数字不能替代新状态连续性

正式对照使用两个独立 headed Chrome、实际 native GPU、真实 RTC 输入路径；CPU4 同时设置在两端。桌面触控源按 Page 时间目标 120 Hz 推进，迟到移动合并会记录。未使用手机，未注入 dense bullet 代替普通 authored gameplay。

本文题目所说的“100 ms”主要对应每方向应用层发送等待 50 ms；CPU4 spike 对照另加 ±5 ms jitter 和已配置尖峰。正式运行名中的 `50ms`、`100ms` 则直接表示 `oneWayMs`：原速 `100ms` 样本每方向等待 100 ms，双向配置总量约为 200 ms，不能与前一个场景混为一谈。这些均不是测得的公网 RTT。计时器迟到、回调占用、校准和实际收包等待另有记录，不能把配置值当成实际线路延迟。

主要指标：

| 指标 | 含义与边界 |
| --- | --- |
| logical Hz | live forward frontier 的实际推进率，历史重演不计新逻辑帧。不是物理显示 FPS。 |
| advanceGapMs | 观察到新逻辑前沿变化的间隔。用于识别“呈现仍在发生，但世界长时间没有新状态”。 |
| presentGapMs | 图形 Present 计数发生变化的观察间隔。不是物理扫描，也不保证每次都有新逻辑状态。 |
| callbackMs | 被记录的 native 回调墙钟，不等于 CPU self。短回调可能连续只还恢复债而不产生新状态。 |
| correction / resimulated | 恢复事件及历史重演 tick 数。受两端时序、输入内容和 D 影响，不是固定基准的工作量。 |
| logicalFrame 与历史 cursor | continuation 下必须分开；历史 cursor 可以后退，live frontier 不应因回放被误减。所有中间 partial callback 都计入成本和等待。 |

本轮比较工具按同 scene、可用 generation/stage 边界及共同观察到的 enemyFrame 端点选取 authored 窗口，不跨 reset/loading 拼接。相同 enemyFrame 只保证计数对齐，不能证明子弹、玩家路径、死亡/复活、RNG 完全相同。各次按墙钟触控产生的 tape 不同，跨 run canonical 不相同不能自动当作联机 desync；同一次两端确认状态相同仍是必要门槛。

下表列的是每端整个测量 body 的描述统计；精确 authored 交集数据保存在比较 JSON 的 peerComparisons 中。不得混合不同窗口计算“总体提速百分比”。

## 5. 正式性能结果：截至 v6 复测仍未整体达到目标

### 5.1 已保存的正式 CPU4 结果

| 运行 | WASM | P0 / P1 logical Hz | P0 / P1 advance 最大 ms | P0 / P1 Present 最大 ms | 结论 |
| --- | --- | ---: | ---: | ---: | --- |
| 旧版 CPU4 spike c1 | `8ea9b23f…` | 56.729 / 56.877 | 75.7 / 81.8 | 75.7 / 72.6 | 对照；旧入口 mode2 自动得到 D3。 |
| v1 CPU4 spike c1 | `b3e5eca5…` | 46.704 / 46.593 | 117.5 / 107.1 | 89.5 / 60.0 | 新状态推进退步，不能用 P1 Present 最大值较小宣称成功。 |
| 旧版 CPU4 spike c3 | `8ea9b23f…` | 56.491 / 56.393 | 93.4 / 93.9 | 93.4 / 86.6 | 紧邻复测仍接近旧版 c1，未显示整段环境全面失速。 |
| v2 CPU4 spike c3 | `0241cd71…` | 44.410 / 44.244 | 139.7 / 171.6 | 108.8 / 82.9 | 单 callback 已缩短，但新状态仍明显稀疏，未达到目标。 |

完整 WASM 身份：

- 旧版：`8ea9b23fc9ffbdb25ddd1d5bf1a94284fef5e60df00390ec735557ff27e71f55`。
- v1：`b3e5eca51901a68e44fb75e99842d498f47994c1f8e794758b42fe5a5ce6aaa3`。
- v2：`0241cd719cb6cfdb8ec33c9661fe7afbd6815bcef144685126fe9b6fb9d54ffc`。

汇总证据：

- [comparison-cpu4-spike-c1.json](../../artifacts/network-cadence-100ms/comparison-cpu4-spike-c1.json)
- [comparison-cpu4-spike-c3.json](../../artifacts/network-cadence-100ms/comparison-cpu4-spike-c3.json)

两份比较汇总均记录运行通过自身启动/确认检查，且单次两端 canonical agreement 为 true。这里的 passed 不是平滑度验收通过。

v1 的两端 advance >50 ms 均为 51 次，>100 ms 为 2/1 次；旧 c1 为 16/13 次、没有 >100 ms。v2 c3 两端 advance >50 ms 为 104/124 次，>100 ms 为 14/11 次；旧 c3 为 13/13 次、没有 >100 ms。用户关心的新运动状态连续性没有改善。

v2 c3 的单 callback 最大值仅为 43.6/24.3 ms，仍有 139.7/171.6 ms 的 advance 间隔。这直接说明：**限制单次回调长度与维持均匀的新帧产生，是两个必须同时完成的目标。**

### 5.2 同一 v2 另有更严重的未解释失败，必须保留

此前 `launcher-candidate-cpu4-spike-c2/1-frontier.json` 的同一 v2 正式包曾测得约 15.8/15.9 logical Hz，Present 最大约 389/506 ms。主测量线程已按 forward highwater 口径核对：该异常包含真实长 callback，**不是误把历史 cursor 当逻辑推进造成的假低速**。

该次 P0/P1 correction 为 106/603，resimulated 为 314/4069。细分中 P0 在没有任何 resim 的 forward callback 也显著变慢；P0 后半段可见每 callback 一次 forward、一次 Draw、一次 Present，不能据此编造“重复三次 Draw”原因。P1 则承担更多恢复并经常接近 H8。

同包 c3 重测回到约 44 Hz，并不抹掉这次失败。为什么同一版本出现这一差异尚未定位；不能将 15.8 Hz 当作稳定常态，也不能只挑 44 Hz 报告，更不能把旧包正常的对照证据忽略后直接归咎系统环境。

原始失败保留在 [launcher-candidate-cpu4-spike-c2/1-frontier.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c2/1-frontier.json)。本稿记录主测量线程的 highwater 核对结论，没有重新执行该运行。

### 5.3 原速通过容量检查，不代表 CPU4 或体感已经通过

[comparison-cpu1-50ms-c2.json](../../artifacts/network-cadence-100ms/comparison-cpu1-50ms-c2.json) 记录：

- 旧版两端约 60.021/60.012 logical Hz，实际 mode2/D2。
- v2 两端约 60.057/60.003 logical Hz，实际 mode0/D0。
- v2 Present 最大 27.5/27.9 ms，advance 最大 49.1/35.9 ms。
- 两版该次都保持接近 60 Hz；v2 仍承担更多 resim 工作，不能把 D0 与 D2 对照当成同输入延迟的纯 CPU 加速。

这支持候选在该原速样本具有推进容量，并支持继续验证低输入延迟策略；不支持忽略 CPU4 失败。也没有独立的 input-to-present 响应分布可以把 D0 换算成最终“减少多少毫秒操作延迟”的完整承诺。

### 5.4 v3 CPU4 正式结果：短 callback、高频 Present 仍未换来足够的新状态

[formal-results-through-v3.json](../../artifacts/network-cadence-100ms/formal-results-through-v3.json) 汇总了以下 v3 正式运行。它们使用相同冻结 Release WASM：

`4ea980d640d6662f20cdbc0db8cfe7226f3e7af3f628752a11ab51d8ce0fd6f9`

这四次 v3 运行均记录 requested mode2/D0/automatic=true，host 规范化后 effective mode0/D0/automatic=false，native 也为 mode0/D0；predictionLimit=8、reserve=2。旧 CPU4 c3 的 native 为 mode2/D3。汇总的 `correctnessPassed: true` 表示原运行的网络/确认状态检查通过，**不是平滑度验收通过**，也不替代第 7.2 节独立 world oracle。

下表每格中的两个数依次为 P0 / P1，单位按表头：

| v3 CPU4 运行 | logical Hz | advance p95 / 最大 ms | Present p95 / 最大 ms | callback p95 / 最大 ms |
| --- | ---: | --- | --- | --- |
| spike c4，cap60=false | 39.416 / 39.450 | p95：54.8 / 53.0；最大：103.1 / 109.0 | p95：26.7 / 25.3；最大：81.2 / 55.9 | p95：15.2 / 12.2；最大：28.5 / 17.5 |
| 同包 spike cap60 c1 | 41.292 / 41.257 | p95：54.0 / 52.1；最大：204.0 / 146.0 | p95：34.6 / 36.1；最大：171.7 / 146.0 | p95：14.6 / 11.8；最大：25.7 / 18.7 |

两次均为 CPU rate 4、单程 50 ms、jitter 5 ms 的 spike 条件。对应原始结果为 [launcher-candidate-cpu4-spike-c4/1-frontier.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c4/1-frontier.json) 与 [launcher-candidate-cpu4-spike-cap60-c1/1-frontier.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-cap60-c1/1-frontier.json)。它们是分别执行的墙钟触控轨迹，不是逐输入完全相同的微基准。

v3 c4 两端 advance >50 ms 为 66/59 次，>100 ms 各 1 次；Present >50 ms 为 11/5 次，没有 >100 ms。Present 平均间隔 11.497/10.096 ms，按该观察间隔倒数约为 87/99 Hz，但逻辑只推进约 39.4 Hz。短回调和频繁呈现已经同时出现，世界推进仍显著低于旧版约 56.4 Hz，也低于 v2 c3 约 44.3 Hz。因此，这次不能写成“v3 已解决卡顿”或“只是 FPS 统计方式不同”。

同包 cap60 将每个新逻辑 tick 累计承担的 callback 墙钟从 c4 的 21.030/21.215 ms 降至 16.726/14.759 ms；该数包含途中 partial 回调，不是单个 forward 的纯 CPU self。尽管这一成本口径变小，逻辑仍只有约 41.3 Hz；advance >50 ms 为 60/57 次，>100 ms 各 1 次，Present >100 ms 也各 1 次。新状态与 Present 的最大空档反而更长。**限制为 cap60 不是已经验证有效的解决方案，高刷新率下的额外呈现也不足以解释全部失速。**

v3 c4 的 correction 为 640/722、resimulated 为 1382/2130；跨回调未完成纠正样本为 264/452，budgetYieldCallbacks 为 644/787。cap60 对应 correction 708/658、resimulated 1519/1990、未完成纠正样本 242/385、budgetYieldCallbacks 663/717。不能只比较一个 callback 的上限，遗漏多次让出回调期间没有新 forward 的等待。

本轮数据支持继续检查“完整恢复结束后如何交付当前欠下的 forward”，并减少可证明的重复纯计算；它没有证明 CPU4 的剩余不足已经全归因于某一个分支。v2 c2 的严重单端异常仍按第 5.2 节保留。

### 5.5 v3 原速正式结果：单程 50 ms 接近满速，单程 100 ms 明显降速

以下仍为同一 `4ea980d6…` 正式包、实际 mode0/D0，CPU rate 1、jitter 0、cap60=false。这里的 50/100 ms 是每方向配置等待，区别于标题的约 100 ms 双向条件。

| v3 原速运行 | logical Hz，P0 / P1 | advance p95 / 最大 ms | Present p95 / 最大 ms | callback p95 / 最大 ms |
| --- | ---: | --- | --- | --- |
| 单程 50 ms c3 | 59.246 / 59.272 | p95：24.0 / 23.8；最大：54.9 / 49.5 | p95：13.4 / 12.5；最大：47.4 / 49.5 | p95：7.5 / 6.8；最大：10.5 / 10.7 |
| 单程 100 ms c3 | 45.191 / 45.207 | p95：32.4 / 30.3；最大：64.3 / 54.2 | p95：14.0 / 13.5；最大：55.6 / 43.5 | p95：8.5 / 8.0；最大：11.7 / 10.9 |

证据为同一正式汇总，以及 [launcher-candidate-cpu1-50ms-c3/1-frontier.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu1-50ms-c3/1-frontier.json)、[launcher-candidate-cpu1-100ms-c3/1-frontier.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu1-100ms-c3/1-frontier.json)。

单程 50 ms 这次接近 60 Hz；两端 advance >50 ms 为 2/0 次，没有 >100 ms，Present 没有 >50 ms。它证明这个样本在 D0 下有接近满速的容量，不能表述为稳定 60 Hz 的普遍保证，也不能消去 CPU4 的失败。

单程 100 ms 则约 45.2 Hz；两端 advance >50 ms 为 5/2 次，没有 >100 ms。预测 backlog 均值约 4.843/4.895f，p95 均为 6f，高于单程 50 ms 的均值 3.293/3.585f、p95 均为 4f。该表现与更深的常态预测、渐进降速方向相容；但是此次仍记录 budgetYieldCallbacks 89/52 和未完成纠正样本 28/21，尚未隔离网络缺口与设备/调度成本各自对 45.2 Hz 的贡献，不能直接承诺降速全部由网络信息不足造成。

这两次原速样本没有出现 >100 ms 新状态空档，属于可报告的明确样本边界。它们仍缺少独立的 input-to-present 延迟分布、相同墙钟输入下的重复对照及用户体感验收；高频 Present 也不等于每次有新逻辑帧。不能据此宣称 TH08 已达到用户认可 TH09 的体验。

### 5.6 v4 CPU4 正式结果：推进率回升，但仍有 >100 ms 空档，验收 FAIL

v4 冻结 Release WASM SHA-256：

`6e091598ad469982990b3ef731cd10c172610e644bb3f06f139a01835352258d`

证据为 [launcher-candidate-cpu4-spike-c5/summary.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c5/summary.json)、[同次 1-frontier.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c5/1-frontier.json) 与 [formal-v4-comparison.json](../../artifacts/network-cadence-100ms/formal-v4-comparison.json)。比较文件保留旧版 c3、v3 c4、v4 c5 各自的身份和完整描述统计，没有覆盖先前失败。

本次为两个独立 headed Chrome、实际 native GPU、两端 CPU rate 4、RTC 每方向应用层等待 50 ms、jitter 5 ms 的 spike 条件；cap60=false，两席位桌面墙钟触控目标 120 Hz，没有手机测试或额外 dense bullet。requested mode2/D0/automatic=true 经 host 规范化后为 effective/native mode0/D0，predictionLimit=8、reserve=2。表内统计来自 live logicalFrame 240→1200 的 960 个新逻辑 tick；不计历史重演为新帧。

| v4 c5 指标 | P0 | P1 |
| --- | ---: | ---: |
| logical Hz | 43.866 | 43.860 |
| advance p95 / 最大 ms | 47.3 / 123.0 | 44.1 / 130.3 |
| advance >50 / >100 ms 次数 | 33 / 1 | 25 / 1 |
| Present p95 / 最大 ms | 41.4 / 107.5 | 32.3 / 104.7 |
| Present >50 / >100 ms 次数 | 27 / 1 | 12 / 1 |
| callback p95 / 最大 ms | 24.2 / 35.3 | 15.9 / 28.8 |
| 累计 callback 墙钟 / 新逻辑 tick，ms | 19.663 | 19.302 |
| correction / resimulated tick | 650 / 1,621 | 560 / 2,031 |
| 未完成纠正 callback / budgetYieldCallback | 380 / 380 | 500 / 500 |

原运行 `passed: true`，两端最终 canonical 一致，只表示该运行的网络/确认状态门通过；**平滑度目标仍为 FAIL**，也不能代替独立 world oracle。两端 callback 都没有 >50 ms 样本，仍出现上述 >100 ms Present 与 advance 空档；不能仅凭 callback 上限宣称长停顿已消除。

相较 v3 c4 的 39.416/39.450 Hz，本次约 43.86 Hz，advance p95 及 >50 ms 次数有所改善；但 Present 长尾变差，且仍低于旧版 c3 的约 56.4 Hz。v4 的累计 callback 墙钟/新逻辑 tick 较 v3 的 21.030/21.215 ms 小，而 callback p95 与最大值更大。这符合把部分欠下的 forward 放回完整恢复尾部后的取舍，**不证明全部变化由该分支单独造成**。两次输入按墙钟产生、恢复工作量不同，v4 又同时包含 Camera 缓存，不能从这些数反推每项补丁的独立提速率。

c5 这次正式平滑度运行未开启 CPU sampling（summary 的 `profileAvailable: false`）。第 7.6 节是另一次同 SHA 正式包的采样诊断，不能混入本表算帧时间收益。

### 5.7 v5 CPU4 正式结果：推进与长尾改善，仍未达到 60 Hz

主线程冻结构建记录对应生产提交 `fa3913cb17ea25cd9ebc86373dfc18e49eed8523`；[builds/receipt-c5.json](../../artifacts/network-cadence-100ms/builds/receipt-c5.json) 保存 `diagnostic: false`、multiplayer Release 构建产物与 3,606,811 B 的 WASM。正式 SHA-256：

`8c7636311077626ab91b926fe5f7314533c3b1c1c511b92e764116f9ce667820`

这是包括 BackgroundMath / projection 快路的 v5 正式包，和第 7.7 节两个独立 oracle 的测试 WASM 不同。生产提交身份来自主线程冻结构建记录；receipt 本身未提供 commit 字段。有效测量 c6c 使用 [frozen-source.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c6c/frozen-source.json) 指向的不可变 c6/frozen 目录，[manifest.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c6c/manifest.json) 的 WASM SHA 与上述 receipt 一致；不是重新构建后借用旧身份。

本轮三次尝试分别保留，只有 c6c 产生有效的正式性能样本：

| 尝试 | 原始证据 | 结果与计量边界 |
| --- | --- | --- |
| c6 | [summary.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c6/summary.json)、[1-frontier.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c6/1-frontier.json)、[relay.log](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c6/relay.log) | `passed: false`，`error: Not RTC`；服务器 ROUTE 明确为 `mode=relay`。两端 body rows 为空，impairment matched/sent 均为 0，尚未形成 FPS 或平滑度结果。 |
| c6b | [run.log](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c6b/run.log) | 运行器复用路径传错，尝试读取 c6/frozen/manifest.json 而报 FileNotFoundError；没有浏览器运行结果，不是游戏性能失败。 |
| c6c | [summary.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c6c/summary.json)、[1-frontier.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c6c/1-frontier.json)、[relay.log](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c6c/relay.log) | 同一冻结 v5 成功建立 RTC，ROUTE 为 `mode=rtc`；完整执行正式 body，网络/确认状态门通过，性能结果如下。 |

c6 的路线在客户端与服务器上都按该轮锁定，不是仅仅太早采样造成暂时看见 relay。现有日志不能区分究竟是默认 4.5 秒协商超时、显式 rtc-failed 还是 signal closed 分支触发；**启动失败原因仍未定位**。冻结测量器在 warmup 后才设置 CPU4 throttle，此次 Not RTC 检查发生在该步骤之前，不能把失败归因于 CPU4 限速。应用层 impairment 包装的是输入/校准 DataChannel.send，不是 ICE/SDP；实际 host D0 也没有校准交换，因此没有证据将这次协商失败归因于所设 50 ms 输入延迟。c6c 同包重试成功不抹去 c6 的失败。若复现，再增加有界的启动分支/耗时诊断；当前保留 RTC 必须成立的原门，不接受 relay 冒充 RTC 性能样本。

c6c 沿用 c5 的正式条件：两个独立 headed Chrome、实际 native GPU、两端 CPU rate 4、RTC 每方向应用层等待 50 ms、jitter 5 ms、cap60=false，两席位桌面墙钟触控目标 120 Hz，无额外 dense bullet。requested mode2/D0/automatic=true 经 host 规范化为 effective/native mode0/D0，predictionLimit=8、reserve=2。每端性能 body 为 live logicalFrame 240→1200，共 960 个新逻辑 tick；两端最终 confirmedThrough=1199、logical regressions=0。正式运行未开 CPU sampling（`profileAvailable: false`）。

| v5 c6c 指标 | P0 | P1 |
| --- | ---: | ---: |
| logical Hz | 48.759428 | 48.675371 |
| Present p99 / 最大 ms | 35.2 / 54.4 | 36.5 / 54.7 |
| Present >50 / >100 ms 次数 | 1 / 0 | 1 / 0 |
| advance p99 / 最大 ms | 46.9 / 61.2 | 46.2 / 64.7 |
| advance >50 / >100 ms 次数 | 6 / 0 | 3 / 0 |
| callback p99 / 最大 ms | 18.5 / 20.7 | 18.3 / 21.0 |
| 累计 callback 墙钟 / 新逻辑 tick，ms | 17.166771 | 17.129792 |
| correction / resimulated tick | 690 / 2,238 | 666 / 2,184 |
| 未完成纠正 callback / budgetYieldCallback | 391 / 391 | 412 / 412 |
| predictionBacklogFrames 均值 / p99 / 最大 | 3.170790 / 6 / 8 | 3.198347 / 5 / 6 |

v5 有效运行的 `passed: true` 和最终两端 canonical 一致，表示这一运行的网络/确认状态门通过；**CPU4 60 Hz 性能目标仍为 FAIL**，也不替代独立 world oracle。高频 Present 仍不能证明每次呈现都有新的运动状态；第 6.6 节的端点 hold 风险也没有因此被排除。这里能确认的是该次样本没有 >100 ms Present / advance 空档，并且新状态推进较 v4 增加。

[comparison-v4-v5.json](../../artifacts/network-cadence-100ms/comparison-v4-v5.json) 对 c5 与 c6c 的运行条件进行核对：两端各得到 **1 个匹配计数窗口、0 个拒绝窗口**，scene=2、enemyFrame 209→1169、各 960 个新逻辑帧；unpaired、workloadDifferences 与顶层 notices 均为空。匹配窗口推进率就是 43.866062/43.859850→48.759428/48.675371 Hz，累计 callback 墙钟/新逻辑 tick 由 19.663438/19.302292 ms 下降至 17.166771/17.129792 ms。v4 两端各出现一次的 >100 ms Present 和 advance 空档，在这次 v5 样本中均为零；>50 ms 的次数也减少。

比较的正确性范围是各自运行内两端 canonical 一致；**跨运行最终 canonical 不同**，每端比较均提示墙钟触控 tape 不同。计数窗口匹配不等于玩家位置、子弹/RNG 或死亡轨迹相同，correction 与 resimulated 工作量也已改变。因此这是冻结条件下组合版本的一次有界改进证据，不是每个数学补丁的纯因果提速率，也不能将约 48.7 Hz 的剩余失速全部解释成用户允许的网络降速。下一步继续减少已有证据指向的本端成本，并用正式复测判断结果能否保持。

### 5.8 v6 首次正式 CPU4 c7：普通 callback 异常慢、回滚覆盖门失败，不能报告改进

证据为 [launcher-candidate-cpu4-spike-c7/selected-metrics.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c7/selected-metrics.json)、完整 [1-frontier.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c7/1-frontier.json)、[manifest.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c7/manifest.json) 和 [relay.log](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c7/relay.log)。selected-metrics 是主线程从原运行提炼的指标，不替代原始记录；manifest 与两端运行 identity 的正式 WASM 都是第 7.9 节的 `d405ec608782af4a0bf421d97db77501d46303ef3bdefc004737e215c66f7b5a`。

本次实际建立 RTC，服务器 ROUTE 为 `mode=rtc`；两个 headed Chrome 的游戏 canvas 均确认 Intel UHD / ANGLE D3D11，`nativeGpuVerified: true`。运行条件为两端 CPU rate 4、oneWayMs=50、jitterMs=5、spike、cap60=false，两席位墙钟触控 120 Hz，没有额外 dense bullet；effective/native timing 均为 mode0/D0、predictionLimit=8、reserve=2。每端完成 960 个测量新逻辑帧；未启用 CPU sampling，两端 `profileAvailable: false`。

原运行 **`passed: false`、`error: No measured remote-input rollback`**。两端最终 canonical 数组逐项相同，logical regressions 都为 0；**不是 canonical 校验失败，也不是 c6 那种没有性能 body 的启动失败**。P1 的 correction、resimulated 和 correcting/budgetYield callback 全为 0，没有满足测量所需的远端输入回滚覆盖。尽管如此，已发生的长 callback、Present 与 advance 空档仍是真实失败证据，不能删掉这个样本。

| v6 c7 指标 | P0 | P1 |
| --- | ---: | ---: |
| logical Hz | 17.593147 | 17.553804 |
| 测量新逻辑帧 / elapsed ms | 960 / 54,566.7 | 960 / 54,689.0 |
| callback 样本数 / 均值 ms | 7,414 / 4.908228 | 958 / 39.383403 |
| callback p99 / 最大 ms | 16.9 / 45.0 | 83.6 / 147.0 |
| callback >50 / >100 ms 次数 | 0 / 0 | 163 / 1 |
| Present p99 / 最大 ms | 51.5 / 132.4 | 117.2 / 203.2 |
| Present >50 / >100 ms 次数 | 70 / 3 | 588 / 27 |
| advance p99 / 最大 ms | 158.3 / 190.8 | 117.2 / 203.2 |
| advance >50 / >100 ms 次数 | 544 / 84 | 588 / 27 |
| 累计 callback 墙钟 / 新逻辑 tick，ms | 37.894896 | 39.266146 |
| correction / resimulated tick | 647 / 4,847 | 0 / 0 |
| 未完成纠正 callback / budgetYieldCallback | 1,286 / 1,286 | 0 / 0 |
| predictionBacklogFrames 均值 / p99 / 最大 | 7.461289 / 8 / 8 | 0 / 0 / 0 |

P1 没有历史重演，普通 callback 却已经明显超时，不能把该端的慢直接归因于深回滚重算。P0 则大部分时间接近预测上限，并执行较多重演；这与一端普通处理变慢、另一端受可用输入前沿限制的形态一致，但仍不能从汇总指标确定最初的慢发生在引擎、浏览器调度或其他具体环节。高频 callback/Present 也不能抵消 P0 的新状态长空档。

该失败与第 5.2 节 v2 c2 的约 15.8/15.9 Hz、单端普通 callback 很慢的旧记录有相似之处，**尚未证明两者同根因，也没有定位或修复该异常**。不能未经证据归咎于环境，不能把最终 canonical 一致写成平滑度通过，也不能将 c7 与 v5 做成 v6 已有改善的对照。

后续 c7b 在启动阶段报 Not RTC，没有形成 body；随后同冻结包 c7c 完成测量，结果见第 5.9 节。c7 的原始数据与失败状态完整保留，不因后续恢复正常而被覆盖，也不能单凭复测结果推断每个补丁的独立收益。

### 5.9 v6 同包后续：c7c 有效但仍未达标，两次 Not RTC 不产生性能结果

两次启动失败单独记录：

| 尝试 | 证据 | 结果边界 |
| --- | --- | --- |
| CPU4 spike c7b | [summary.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c7b/summary.json)、[1-frontier.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c7b/1-frontier.json) | `passed: false`、`error: Not RTC`；两端 rows=[]，没有性能 body，不能记为零 FPS 或有效 CPU4 复测。 |
| CPU1 单程 50 ms c6 | [summary.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu1-50ms-c6/summary.json)、[1-frontier.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu1-50ms-c6/1-frontier.json) | 同为 Not RTC、两端 rows=[]；没有新的 v6 原速帧率或平滑度结果。 |

这两次启动失败不能与 c7 已进入活动游戏后的严重失速混为一谈，也不由 c7c 成功消除。现有证据不足以指认各次协商失败的具体分支；继续保留 RTC 必须成立的测量门。

有效 c7c 证据为 [selected-metrics.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c7c/selected-metrics.json)、完整 [1-frontier.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c7c/1-frontier.json)、[frozen-source.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c7c/frozen-source.json)、[manifest.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c7c/manifest.json) 与 [relay.log](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c7c/relay.log)。运行窗口为 2026-10-12 06:57:16.633–06:58:15.635 UTC。frozen-source 指向 c7/frozen，manifest 与 c7 完整一致，WASM 仍为 `d405ec60…`；两次之间没有用新构建替换测量包。

c7c 实际 ROUTE 为 rtc；两个 headed Chrome 的游戏 canvas 均验证为 Intel UHD / ANGLE D3D11 native GPU。两端 CPU rate 4、oneWayMs=50、jitterMs=5、spike、cap60=false、墙钟触控 120 Hz、无额外 dense bullet，effective/native mode0/D0、predictionLimit=8、reserve=2。两端各完成 960 个新逻辑帧，未开 CPU sampling，logical regressions 均为 0。原运行 `passed: true`，两端都有 correction/resim，最终 canonical 数组逐项相同；这是运行门通过，**CPU4 60 Hz 与平滑度目标仍未通过**。

| v6 c7c 指标 | P0 | P1 |
| --- | ---: | ---: |
| logical Hz | 49.646014 | 49.664249 |
| Present p99 / 最大 ms | 36.6 / 60.0 | 39.6 / 86.8 |
| Present >50 / >100 ms 次数 | 7 / 0 | 5 / 0 |
| advance p99 / 最大 ms | 53.4 / 70.0 | 51.2 / 101.7 |
| advance >50 / >100 ms 次数 | 11 / 0 | 11 / 1 |
| callback p99 / 最大 ms | 18.4 / 27.5 | 18.8 / 33.6 |
| 累计 callback 墙钟 / 新逻辑 tick，ms | 16.468229 | 16.554688 |
| correction / resimulated tick | 626 / 2,098 | 643 / 2,114 |
| 未完成纠正 callback / budgetYieldCallback | 335 / 335 | 340 / 340 |
| predictionBacklogFrames 均值 / p99 / 最大 | 3.328691 / 6 / 8 | 3.232270 / 5 / 7 |

与 v5 c6c 的 48.759/48.675 Hz、17.166771/17.129792 ms callback/新逻辑 tick 相比，v6 c7c 的这两个描述统计有所改善；但 Present 最大值、>50 ms 次数及 advance 长尾并未全面改善，P1 仍有一次 101.7 ms 新状态间隔。当前没有隔离 ScreenEffects 单项改动的配对工作负载因果证据，墙钟触控与回滚工作量也不同，不能宣称 ScreenEffects 已独立节省某个比例的 CPU。

因此当前记录是 **同一冻结 v6 包的一次严重失速与一次有效部分改善，根因未定**。c7c 表明该包并非每次都重现 c7 的约 17.6 Hz，但不能证明 c7 已修复，更不能抹去第 5.2 节的相似旧失败。v6 仍未达到稳定 60 Hz，也没有可报告的成功原速 50 ms 新样本。

## 6. 已找到的调度问题与仍未定位的部分

### 6.1 恢复 yield 后的 tick 债务需要区别处理

TH09 的恢复 yield 路径会保留一个待重试 tick，并阻止缺恢复完成期间继续累计大量可追赶债务。TH08 v2 曾把恢复 CPUyield 与普通 forward 的 CPU 超预算相同处理，退还整批未完成 due tick；在连续 partial recovery callback 中会积累多 tick 债务，恢复后形成追赶批次。

源码层面这是明确的节奏差异，有可能形成“还恢复债—追赶—更多晚到输入—再恢复”的反馈。它能解释需要修正的调度条件，但**不能单独解释 v2 严重失败中 P0 无 resim 的 40–60 ms forward 成本**。不能把一个可复现状态问题直接升级为全部退步的唯一根因。

v3 已在 `1430a73` 中区分两类情况：恢复 CPUyield 只留单 pending tick；普通 forward 因 CPU 预算不足而欠下的工作仍保留原语义。第 5.4 节的正式 v3 c4 与 cap60 对照表明，这项修正组合进当前候选后仍未使 CPU4 达标，不能单独给它归因总体收益。

### 6.2 同一预算必须包含完整帧的提交尾部

若仅在 Update/恢复之后检查预算，却把必要的 Draw/Present 留在预算之外，测得的“8 ms 恢复限制”无法约束用户看到的完整回调成本。若由于预算到点又跳过本来必须完成的呈现，则重现 TH09 早期已经修复的“逻辑推进但画面不交付”。

v3 `1430a73` 将完整帧的预算内提交与上述 recovery 单 pending 处理放在一起。每个完成的 forward tick 保持完整语义 Draw，恢复未完成时不暴露半成品，仍是必须保留的正确性边界。v3 对“恢复刚完成但预算已耗尽”的选择是先让出回调，不启动新的 forward；这是调度取舍，不是不可更改的正确性硬门。第 7.5 节的 v4 已试验允许当前欠下的一个 forward 继续；正式结果见第 5.6 节，仍未达标。

一个完整 Update/semantic Draw 本身可以超出目标预算，因此该改动不是“callback 永远不超过 8 ms”的硬保证。

### 6.3 不能跳过历史语义 Draw 来制造快数字

TH08 的 Draw 路径包含会进入游戏状态/恢复契约的 VM、矩阵与效果修改。历史重演可以按既有策略抑制 GPU 几何/提交，但不能泛化为跳过语义 Draw。TH09 和 TH08 的对应边界必须按实际实现对照。

当前仍需区分纯计算/复制成本、恢复重复工作、Present 尾部及浏览器 rAF 排队。此前 P0 的无 resim forward 异常没有被源码审查证明是单一调度分支导致，不能在报告中写成已经定位或修复。

### 6.4 已排除一种归因：TH09 没有在 60 Hz 降速时独立生成额外 alpha 画面

TH08 的额外 readonly 插值 Draw 依赖 high_refresh：`portable/sdl/PresentationCadence.hpp:8–16` 以小于约 15 ms 的回调间隔作为 fast sample，连续 6 次才开启，连续 12 次慢样本才退出。稳定约 16.67 ms 回调不会因此开启高刷新率路径。

`th08_web/cpp/sdl/GameHost.cpp:218–248` 分别计算 `high` 和 `network_scale`；网络降速只影响 `network_schedule.advance(paced_delta/network_scale, ...)`，不会单独启用 high。第 367–405 行的行为是：低刷新率提交完成的 forward/完整纠正端点，额外只读 Draw 则要求 high、无未完成纠正、且本 callback 没有已经提交端点。因此 60 Hz 下逻辑未推进、又没有刚完成纠正时，没有新的中间显示图像。记录中的 alpha 默认值 1 也不能证明实际发生过 Draw/commit。

但这不是漏移植了 TH09 已有的独立插值机制。已核对的 `D:/workspace/eagler/worktrees/th09-managed-touch-4x/th09_web/cpp/sdl/Application.cpp:614–645` 同样先按网络速度计算 ticks，只在 `n < ticks` 的循环里调用 `th09_game_tick(...)`；循环外只有音频、诊断和浏览器回调。ticks 为零时，这条 native rAF 路径不执行 Draw，没有 presentation alpha 或循环外 readonly 重画。

进一步的浏览器路径核查由网络对照线程记录：TH09 的 `th09_web/sdl-runtime/managed.mjs:195` 的 onGameFrame 服务 netplay/统计/生命周期；`shared-netplay.mjs:234–246` 的独立 timer 调用网络 Pump；`Application.cpp:402–410` 的 Pump export 不执行世界 Draw/Present。它们没有补上另一条无 tick 的绘制路径。

结论是：TH08 的 high-only 插值确实限制了“60 Hz 显示、网络降速后逻辑低于 60 Hz”的连续呈现，但所读 TH09 也按逻辑步产生新画面。这一事实本身解释不了 TH09 同样降速却更舒服的差异，不能据此承诺开启额外插值就能解决本轮退步。这里是指定源码树的执行路径核查，没有重新计算该旧源码与 `cb03c6df…` WASM manifest 的全部输入哈希。

### 6.5 TH09 也可能在最后一帧历史重演后因预算到点而 yield

再次直接核对用户认可来源树的 `D:/workspace/eagler/worktrees/th09-managed-touch-4x/th09_web/cpp/sdl/Application.cpp:550–559`：`final` 使用本次执行前的 `f >= rollback_catchup` 判定。最后一个历史帧的 f 仍小于旧前沿，因此其 `final` 为 false；完成该帧后即使已追到旧前沿，仍会执行 `emscripten_get_now()-start >= 8` 的退出检查。第 578 行又用 `rollback.Frame() <= rollback_catchup ? 2 : 1` 返回状态；刚好回到旧前沿但还未运行新 forward 时仍返回 2，外层保留待重试 tick。

因此 TH09 的真实行为是：预算尚有余量时，可以在同一回调继续到当前 owed forward；预算在最后历史帧用尽时，也会让出回调。**不能把“完整恢复完成后无论预算如何都追加新 forward”写成 TH09 已有保证，或将其称为本轮只需照搬的成功秘诀。** v4 放行完整恢复后的新 forward 是本轮的新取舍；第 5.6 节已经同时记录其推进率和长尾，结果仍未达到目标。

### 6.6 已显示逻辑终点后的 hold：源码限制已确认，触发频率未测量

`th08_web/cpp/sdl/GameHost.cpp` 的正常 forward 仍以 `app.draw(1.f,false,false)` 完成语义 Draw。第 367–387 行在低刷新率或预算耗尽时直接提交完整 forward/纠正图像；一旦 `committed_endpoint` 为真，就清 presentation gate，避免下一次无 tick 回调倒插进已经显示过的 prev/current 区间。第 390–406 行的 high-refresh readonly 路径在 gate 尚未重新被完整 forward 激活时使用 alpha=1，因此无新 tick 时可以再次呈现同一个逻辑终点。它不会凭空产生新的运动状态。

这一约束说明“Present 频繁但运动可能停留在旧端点”有明确源码路径。它并不证明每个无 tick callback 都执行了 Draw，也不证明每次重画像素完全相同。当前正式 v4 记录没有逐 callback 的最终子弹中心/呈现 alpha 探针，**本节没有给出端点跳跃或停留的实测频率，也不把它归为已证实的唯一卡顿根因**。

已经真正显示 alpha=1 后，只有旧 prev/current 两个端点，在不外推、不倒退的条件下无法继续向前；简单删除清 gate 会重新允许回跳。若要在网络降速下持续插值，必须在第一次 commit 之前就选择一致的呈现时间，并考虑额外显示滞后。第 6.4 节已排除“TH09 本来就有独立 60 Hz 插值”的归因。

也不能直接把语义 Draw 改为 `draw(alpha,true,false)`：`BulletDrawing.cpp:27–36` 的 source VM 写入、`AnmRenderer.cpp:125–140` 的矩阵与 dirty flags、`BackgroundObjects.cpp` 的 VM/投影/effect 状态均有语义副作用；改成 render_only 又会跳过必要的状态消费。`BrowserRuntime.cpp:215–230` 还会立即复制或将 backbuffer 写入持久捕获纹理。独立的最终几何输出/缓存重放可作为后续方案，但需要对象生命周期、完整帧发布及 capture 边界证明，本轮没有实现 geometry packet。当前优先处理正式 profile 指向且能够逐位验证的纯数学成本。

## 7. 版本身份、正确性、正式 profile、v5 数学结果与 v6 候选

### 7.1 v3 提交与 fixture 身份（保留历史）

v3 阶段已核实的候选提交：

| 提交 | 范围 | 本稿状态 |
| --- | --- | --- |
| `ad8835a` | ANM script first-write | owner probes 与固定场景 worldProbe 通过；保存量收益见 7.3。包含该改动的 v3 CPU4 正式结果未达标，该次 v3 独立 world 未完成；v6 八场景结果另见 7.9，不回填为旧运行通过。 |
| `1430a73` | 完整帧在预算内提交；recovery CPUyield 单 pending，保留普通 forward CPU 欠账 | 已进入 v3 正式包；端到端结果见 5.4–5.5，CPU4 未达标。 |
| `a6291cc` | 分离恢复 cursor 与 live frontier 等进度指标 | 用于正确计量 continuation；不是游戏性能优化收益本身。 |
| `c910a97` | 修复资源 fixture 使用越界槽 70，改为主槽 68 / 后续槽 69 | 修正测试槽位；该版另有私有成员访问导致 c3b 编译失败，不能记为成功构建。 |
| `bc646f9` | fixture 改用公开 `journal.Failed()`；合入 motion 逻辑进度分析修正 | c3c fixture 构建成功；测量分析修正本身不是引擎性能收益。 |

v3 fixture **c3c** WASM SHA-256：

`5f32bf500f745b13ad98d2f05f62f1975492921ebc471a17a1b7ad15d653c925`

大小为 **3,806,619 B**，是诊断 fixture，不是 v3 正式 Release 的性能产物。此前 fixture `67ebcb0d49609bc8aee6f0e268b22cb67a95ce0f1181ef4b56bb95535d77caa8` 的资源 probe 步骤 25 失败记录继续保留；不把它的身份换成新包后宣称原运行已通过。

### 7.2 v3 正确性历史：owner 全通过，该次独立 world 仅部分执行

步骤 25 的测试问题已定位到 fixture 选用了越界槽 70。`c910a97` 将用例改为主槽 68、后续槽 69，但补丁附加访问 `AnmLibrary` 私有成员，使 c3b 构建失败。随后 `bc646f9` 改用公开的 `journal.Failed()` 状态，完成 c3c 构建。该经过是具体测试用例和编译接口的修正，不是删除正确性门。

[owner-probes-c3c.json](../../artifacts/network-cadence-100ms/owner-probes-c3c.json) 保存 `passed: true`；Enemy、Screen、Pools、Resources 及该命令内的固定场景 worldProbe 均为 `[1,1,0]`，原有后续断言也已通过。其 WASM 身份与上面的 c3c 完全一致。

**独立 world-oracle-c3c 的 8 场景套件尚未完成。** 主线程设置的单次 210 秒运行预算到期后终止了进程；日志明确记录 2P shooting、focused-bombs、native-death 三个场景 PASS，未出现断言失败。但该次没有最终 JSON、没有全套 PASS，不能将超时描述为完整 world 正确性通过，也不能把它自动归因于游戏逻辑缺陷。

在上述 v3 阶段，主线程随后串行构建正式版并完成第 5.4–5.5 节的性能运行，独立 world 套件当时仍待补齐。性能结果和网络确认状态通过不代表独立 world 全套已经验收。owner 命令中的一个固定 worldProbe 与独立 8 场景套件必须分开计数。后续 v6 的完整独立套件结果见第 7.9 节，不改变 c3c 超时、仅完成三项且没有最终 JSON 的历史事实。

此前已经通过的 host options、SessionChannel 输入尾包边界、2P/3P continuation 以及 cadence/input-gap 局部检查，是继续工作的基础，不得替代独立 world 套件。现有 [network-cadence-tests/report.json](../../artifacts/network-cadence-tests/report.json) 自身也标明是 WASI 调度/缺口正确性检查，不是帧时间基准。

### 7.3 已核实的真实世界保存量对照

这条结果来自真实固定场景的 worldProbe，不使用人工 resourcesProbe 的 110 字节作为实际游戏收益。对照文件为：

- 旧 accepted：`D:/workspace/eagler/worktrees/th08-rollback-journal-4x/artifacts/rollback-journal-4x/candidate-owners-c4/enemy.json`，`passed: true`，WASM `2eb30712d8be66f2e85d6c8f1d15988b6d6f416d2a412b87fafa7a04a5a7a92c`。
- 新 c3c：[owner-probes-c3c.json](../../artifacts/network-cadence-100ms/owner-probes-c3c.json)，`passed: true`，WASM `5f32bf500f745b13ad98d2f05f62f1975492921ebc471a17a1b7ad15d653c925`。

以下数组下标均从 0 开始：

| 字段 | 含义 | 旧 accepted c4 | 新 c3c | 变化 |
| --- | --- | ---: | ---: | ---: |
| `worldProbe[54]` | Main | 381,998 B | 381,998 B | 0 |
| `worldProbe[55]` | Enemies | 67,211 B | 67,211 B | 0 |
| `worldProbe[56]` | Pools | 37,471 B | 37,471 B | 0 |
| `worldProbe[57]` | Resources | 204,530 B | 115,746 B | −88,784 B，−43.41% |
| `worldProbe[58]` | Replay | 1,172 B | 1,172 B | 0 |
| `worldProbe[3]` | World 保存量峰值 | 692,382 B | 603,598 B | −88,784 B，−12.82% |

两树的 `portable/multiplayer/check-enemy-journal.py` 与 `portable/multiplayer/world-journal-fixture.hpp` 已通过 nig 读取并逐字符比较，完全相同。脚本以 seed 1234、2P `[0,1]` 执行相同动作和 tick 序列，再调用默认 worldJournalProbe；没有传入新增的 runtime timing 参数去改变这条配置路径。

映射依据当前 `th08_web/cpp/multiplayer/WorldJournal.cpp:304–308` 的 `DiagnosticBytesForFrame()` 返回顺序：Main、Enemies、Pools、Resources、Replay。**最后一项是 Replay，不是 TextureJournal。** `portable/multiplayer/world-journal-fixture.hpp:82–89` 只在首次执行 frame 0–5、共 6 个逻辑帧时取保存量最大值。表中是每 owner 峰值及总 world 峰值，**不是六帧累计量、进程内存、WASM heap 峰值或累计分配字节**。

该固定真实场景下，资源 checkpoint 保存量减少 88,784 B，其余四个 owner 数量完全相同，总 world 保存峰值下降 12.82%，且该用例的 Undo/重算断言通过。这是目前可明确报告的有界改善；不能换算成 CPU 耗时下降 43.41%、帧率提升多少，或声称所有关卡都有相同比例。旧 `candidate-owners-c3` 的 Main/Pools 数量不同，不能替换本表 c4 作分母。

截至 v6 同包复测，仍不应发布以下结论：

- “ANM first-write 已证明总体提速”；
- “恢复分片已消除大卡”；
- “CPU4 已达到 TH09 的体验”；
- “v2 的严重单端慢回调已经找到并修复”；
- “fixture 构建成功即可等同正式性能通过”。

### 7.4 Camera 精确结果缓存：oracle 已通过，组合进入 v4，独立收益未分离

v3 正式包冻结之后，本轮增加 Scene 与 Screen 各一个固定大小的 Camera 结果缓存；两个缓存合计由静态断言限制在 512 B 内，没有堆分配。Scene 的键逐位包含实际 13 个浮点输入、viewport width/height，以及 Precision、Rounding、GraphicsArithmetic；Screen 使用实际尺寸和同样三类算术状态。命中时恢复 view/projection，Scene 还恢复 `data.right`，不覆盖 unused 等无关状态。缓存 miss 保持原数学运算顺序；非有限输入/输出、无效尺寸以及两个矩阵输出别名时保守执行原路径。

它只省略相同输入下已经算过的相机矩阵、归一化等纯计算，不跳过 Update、语义 Draw 或呈现。源码为 `th08_web/cpp/game/Camera.cpp/.hpp` 及 `GraphicsMath.cpp/.hpp` 的只读 arithmetic getter。该修改不包含在第 5.4–5.5 节的 `4ea980d6…` 包内，不可拿那些结果归因给 Camera。

主测量线程已串行执行 `node portable/multiplayer/check-camera-cache.mjs`，**24 种算术组合、11,856 项检查 PASS**。[camera-cache-tests/report.json](../../artifacts/camera-cache-tests/report.json) 保存 `passed: true`、`arithmeticCombinations: 24`、`exitCode: 0`，测试 WASM SHA 为：

`306ece04c251e01f70f9f9b2af46200153eb3caa7452d6ec034c3716d19f111f`

`portable/multiplayer/camera-cache-test.cpp` 使用独立保留的缓存前 Camera 公式作为逐位 oracle，底层仍使用原 GraphicsMath helper。覆盖 3 precision × 4 rounding × 2 graphics、重复命中、每项输入变更后恢复旧值、viewport、+0/-0、非有限值/非有限结果、输出别名、right 恢复与其他 SceneCamera 字段不变。测试命中统计仅在测试编译标志下开放，不加入正式导出。

这是相机结果和缓存边界的 WASI 正确性证据，**不是帧时间基准**。Camera 已与第 7.5 节策略一起进入第 5.6 节的 v4 正式包；该组合 CPU4 仍未达标。可以说明命中时省掉了原本重复的计算，但不能从组合结果分离出相机缓存独立节省的整帧成本，或承诺移动设备负载和最差空档已经改善。

### 7.5 v4 完整恢复后继续当前 forward：正式已测，仍未达标

当前 `th08_web/cpp/multiplayer/RollbackDriver.cpp:303–339` 在每个不可分割的历史帧完成、journal 关闭之后才允许 partial yield。预算退出条件仍要求 `NextFrame() < CorrectionEnd()`：若历史已完整恢复，则结束 correction。第 414–420 行的 Step 现在只在 correction 仍未完成时直接返回；完成后经过原 Commit/Admit，再尝试当前已经到期的一个 forward，即使历史恢复刚好用尽 8 ms 预算。普通缺输入、资源边界与失败仍受原 admission 限制，不强行通过。

`th08_web/cpp/sdl/GameHost.cpp:252–350` 仍由外层预算决定能否开始更多 catch-up tick，并为每个完成的 forward 执行完整语义 Draw 和 finish_network_frame。未完成恢复不公开半成品；恢复/forward 端点已经产生的像素按既有提交规则交付。它没有增加可执行 due tick 的数量，也没有把历史重演算成新逻辑进度。

这项尝试针对 v3 大量 callback 用于还恢复债、而没有新的 forward 的节奏问题。其明确代价是完整恢复后的那个 forward 不可拆分，可能把同一个 callback 推得更长。第 6.5 节已说明，TH09 自身也会在最后历史帧预算到点时 yield；本改动不是对 TH09 已验证行为的机械复刻。

**v4 正式结果已完成，见第 5.6 节，平滑度仍未达标。** 两端约 43.86 logical Hz；Present >100 ms 与 advance >100 ms 各 1/1 次。相对 v3 c4 的推进率回升，没有消除长空档，callback p95 与最大值也上升。Camera 纯成本优化与该调度变化共同存在于 `6e091598…` 冻结包，不能从组合结果反推每项独立收益。接下来按正式 profile 的有界热点继续减少可证明的重复计算，不把这项调度实验描述为已复现 TH09 的成功效果。

### 7.6 v4 正式包 CDP 采样：用于定位热点，不是性能验收

证据为 [profile-formal-v4-cpu4-c1/cpu-analysis.json](../../artifacts/network-cadence-100ms/profile-formal-v4-cpu4-c1/cpu-analysis.json) 及其引用的冻结 [1-frontier.json](../../artifacts/network-cadence-100ms/profile-formal-v4-cpu4-c1/1-frontier.json)、WASM、symbols 和 manifest。这是另一次同一 `6e091598…` Release 包、CPU4、单程 50 ms、jitter 5 ms、cap60=false 的运行；`analysisCompleted: true`，`performanceAcceptance: false`，`errors: []`。离线解析核对了冻结 WASM/symbols 哈希配对、真实含 imports 的 `wasm-function[N]` 索引及模块 byte range；显式索引节点均已解析，不能把这一覆盖率误写成所有浏览器/native/GPU 时间均已解释。

**采样范围更正：180 帧 warmup 在采样开始前已经完成。** 随后开启 Profiler，进行活动阶段设置/启动等待，再从已有逻辑前沿继续前进；性能 body 为 logicalFrame 240→1200。因此采样覆盖设置阶段、进入 body 前约 60 个 forward，以及 body，**不是从全新世界再测一段 240 帧 lead-in，也不是已裁剪到 body 的纯 gameplay profile**。`portable/multiplayer/measure-smoothness.py:278–331` 的顺序为 warmup、Profiler.start、随后 start(...,240,...)。冻结原始报告仍保留 `including its 240-frame lead-in` 这个旧 scope 标签；不改写原证据。分析 JSON 的 `inputs.recordedSamplingScope` 保存旧标签，顶层 `scope` 已注明上述正确解释。

请求采样间隔为 1,000 μs，两端实际有效样本分别为 15,217 / 15,279，间隔中位数均约 1.553 ms；所归属的有效 interval 总时长为 23,627.740 / 23,636.109 ms。以下仅摘录固定 top-20 结果中的热点，百分比各自以该端全部有效采样 interval 为分母，包含 idle、GC、program。两端是独立进程，不合并为一个墙钟总量。

| 函数 / 范围 | 计量 | P0 | P1 |
| --- | --- | ---: | ---: |
| `numeric::binary<Extended>` | self 占比 | 9.373% | 9.652% |
| `Extended::from_float` | self 占比 | 3.017% | 2.403% |
| `GraphicsMath::project_composed` | self 占比 | 2.388% | 2.870% |
| `GraphicsMath::multiply` | self 占比 | 2.297% | 2.311% |
| `RollbackJournal::Touch` | self 占比 | 6.555% | 6.644% |
| `eagler_journal_copy_bytes` | self 占比 | 3.386% | 2.637% |
| `GameApplication::draw` | inclusive 占比 | 42.584% | 43.502% |
| `BackgroundObjects::draw` | inclusive 占比 | 17.111% | 17.549% |
| `RollbackDriver::Correct` | inclusive 占比 | 39.251% | 37.174% |

inclusive 范围相互重叠，特别是 Draw、背景和 Correct 包含相同的子调用，不能相加当成总 CPU。采样按 `timeDeltas[i]` 归属到对应样本；这是受采样器开销、优化/内联和较长采样间隔影响的统计墙钟归属，不是逐次调用 CPU self、GPU 耗时或精确 rAF 排队归因。原始时长既不乘也不除 CPU 限速倍数；`(program)` 不能自动算成 idle 或游戏工作。两端记录的 idle 为 9.183%/12.110%，program 未归属为 7.478%/7.918%，GC 为 0.139%/0.157%，也不能从这些类别直接推断设备瓶颈。

这些证据支持优先核查背景中的重复 Extended 转换、点积/坐标复合运算、投影和矩阵组合；journal Touch/复制也仍有实质成本。**self 占比不是可节省比例，inclusive 占比更不是预计提速率**。候选必须先保留原算术结果，再以冻结正式包的相同测量条件验证；不能把某函数约 9.5% 的采样占比写成“整体将加速 9.5%”，也不能把诊断运行当成第 5.6 节的无采样正式结果。

### 7.7 v5 数学快路：两个 oracle 已通过，保留投影首个失败，正式组合结果已测

`th08_web/cpp/game/BackgroundMath.hpp` 与 `BackgroundObjects.cpp` 接入的 compound 快路，只在 Precision::Single / NearestEven 下逐步调用既有 `scalar_binary`。dot、center、translated、coordinate、scaled_add 保留旧二元运算树；每一步的输入、结果及下溢/溢出边界都通过才接纳。任一步失败就从原始参数重算整个旧 Extended 表达式，不沿用已经溢出或下溢的 float 临时值。dot 仍返回 Extended，length 中原有的 to_float→sqrt 边界仍保留；正式编译继续使用 `-ffp-contract=off`。它没有省略 Update、语义 Draw 或必须恢复的状态。

主线程已串行执行 BackgroundMath WASI oracle，输出 **12 种算术模式、310,860 checks PASS，fast 20,580 / fallback 290,280**。[background-math-tests/report.json](../../artifacts/background-math-tests/report.json) 保存 `passed: true`、`arithmeticCombinations: 12`、`exitCode: 0`，测试 WASM SHA-256：

`e49943786051da458342e0d5d5c72c4a9a718b4f356f5331c5e3378785468be1`

`portable/multiplayer/background-math-test.cpp` 保留旧表达式作参考：dot 比较 Extended significand/exponent，其余逐位比较 float；包含正负零、subnormal/正规边界、非有限值、随机原始位型，以及中间扩展指数恢复。其他 11 种模式明确检查不得进入快路。这里验证的是新 compound 实现相对旧 Arithmetic 表达式的数值一致性，底层 Arithmetic 仍共享；不是正式游戏帧时间、全 world oracle 或预期 CPU 收益。

另一路 GraphicsMath 投影快路先经历了 oracle 失败。主测量线程定位到 case 1626 的 NaN payload 不一致；冻结首个失败记录 [projection-fastpath-tests/report-first-failure.json](../../artifacts/projection-fastpath-tests/report-first-failure.json) 保存 `passed: false`、`exitCode: 1`，失败测试 WASM 为 `cb31cc701b32b9851cec95460d444fdbb9c16f4a46d60f5ac157392d01873d35`。该 JSON 的 24 是当时配置的算术组合数，不能把这次失败运行写成全 PASS；case/payload 定位来自主线程失败输出。这份首个失败不会被后续成功覆盖。

主线程随后在失败点追踪确认：NaN payload 差异首先出现在未改动的 original multiply 与独立编译的 frozen clone 之间，旧路径和候选原 fallback 在该点一致；**没有为修复这次失败改变生产算法**。调整的是测试参考的边界：非 NaN lane 仍严格使用冻结旧公式；仅当 frozen 与原 helper 同为 NaN 时，才通过未改动的原 helper 加冻结 viewport 表达式取得原 payload。`projection-fastpath-test.cpp:96–138` 对所有最终候选 lane 仍逐位比较，包括 NaN payload；finite/infinite 差异、NaN 与非 NaN 的类别变化仍失败，不是任意 NaN 均视为相等。

修正参考后，主线程串行运行结果为 **24 种算术组合、43,657 checks PASS**，并记录 matrix 9,840、normal 6,144、raw 3,072、composed 6,984、originalNaNReferences 6,448。[projection-fastpath-tests/report.json](../../artifacts/projection-fastpath-tests/report.json) 保存 `passed: true`、`exitCode: 0` 及上述参考范围，最终测试 WASM SHA-256 为：

`4d94bb8c0f70e39a2814b02e28bcb687284d5747104e807e60fe96346638dfbc`

两个数学 oracle 均已通过，生产提交 `fa3913cb17ea25cd9ebc86373dfc18e49eed8523` 的正式 v5 包与有效 c6c 结果已记录于第 5.7 节。相对 v4，组合版本的推进率和长尾改善，但两端约 48.7 logical Hz 仍未达到 CPU4 60 Hz 目标，尚无各数学补丁独立收益的实测拆分。数值检查不替代独立 world 或真实帧时间；第 7.6 节的采样占比也不作为本次改善的因果百分比。

### 7.8 v6 ScreenEffects 批量保存：实现、owner 验证与保存量

进一步核对的证据是同一 v4 正式 CDP profile 下的 [touch-self-caller-stacks.json](../../artifacts/network-cadence-100ms/profile-formal-v4-cpu4-c1/touch-self-caller-stacks.json)。它只统计叶节点为已验证 WASM index 2459 的 `RollbackJournal::Touch` 样本，并按调用栈归属；其中经过 ScreenEffects::capture_rollback 的 self interval 两端为 **876.484 / 830.009 ms**，对应各端全部 Touch self interval 的 1,548.791 / 1,570.441 ms。forward 与 Correct 路径均有贡献。

这些时长沿用第 7.6 节的采样边界，包含 setup 与进入 body 前约 60 个 forward，并未裁剪成纯性能 body。它们是 Touch 叶采样有 ScreenEffects 祖先的统计归属，不是 capture_rollback 的完整直接计时、实际调用次数、新快路命中率或预计可节省时间；更不能把这个占比直接当成整体加速比例。

当前候选让同一 Chain 的两个 ScreenEffects owner 先配对保存固定 instances 池，再遍历共用链。主线程核对的粒度变化是：**两个池每个新 checkpoint 的 1,024 个细粒度块收拢为 2 个 whole-array 块**；context、allocation_failed 和其他链节点仍须单独覆盖，并非整个 world 只剩 2 个块。完整数组带来额外 **1,792 B** before-image 载荷，换取较少的 journal 登记/Touch 查找。保存内容增加，是否净减成本需要正式验证。

`th08_web/cpp/game/ScreenEffects.cpp:114–146` 的候选先要求 frame open、无 allocation_failed，且 peer 同属一条 Chain；两个池均保存成功后，才跳过已由池覆盖的 calculation/drawing 节点，避免数组与单节点部分重叠。共用链上的其他节点仍 Touch；即使节点已覆盖，链长度/循环边界与动态拥有标志检查仍执行。改变保存粒度不授权省略动态节点生命周期、Undo 恢复或链结构断言。

主线程记录 v6 源码提交为 `9b43f35fe53e6cc5dab79a8e7e2a6372c6ea01ad`；[builds/receipt-c6-fixtures.json](../../artifacts/network-cadence-100ms/builds/receipt-c6-fixtures.json) 保存 `diagnostic: true`、multiplayer-fixtures 与 3,820,570 B 的测试 WASM，SHA-256：

`74a6f37d0ceb03720aaad9822c639e6ffde8a91fa5e01f266baa5375a3b8c17a`

[owner-probes-c6.json](../../artifacts/network-cadence-100ms/owner-probes-c6.json) 的 `passed: true`，实际游戏 canvas 为 Intel UHD / ANGLE D3D11，`nativeGpuVerified: true`。screenProbe 为 `[1,1,0,15957,1,1,1,1,1,1]`，包括主线程新增的 paired owner 步骤 15–32；resourcesProbe 为 `[1,1,0,110,1,1,1,1,1,1]`。后者仍是人工资源 probe，110 B 不能当真实世界收益。固定 worldProbe 的 `worldProbe[3]` 总峰值为 **605,390 B**，`[54..58]` 为 **383,790 / 67,211 / 37,471 / 115,746 / 1,172 B**，按第 7.3 节顺序分别为 Main、Enemies、Pools、Resources、Replay；`[32..35]` 的 roundtrip 标志均为 1。

同一固定场景相对 v3，只有 Main 与总峰值增加 **1,792 B**，与 whole-array 保存的载荷差一致，资源仍为 115,746 B。相对旧固定场景的总峰值 692,382 B，v6 当前总峰值净少 **86,992 B / 12.56%**。第 7.3 节的 88,784 B / 12.82% 属于 v3 身份，继续保留，不移用为 v6 的总保存量收益；资源 owner 自身减少 88,784 B 的比较不变。

本节的 owner-probes-c6 通过范围是 owner 与固定 worldProbe 恢复门，和独立八场景分开计数。后续独立 world-oracle-c6 已全部完成，正式 v6 也已构建，见第 7.9 节；首轮 c7 已发生第 5.8 节的严重失速与覆盖门失败，同包 c7c 的有效部分改善见第 5.9 节，不把这些正确性结果转换成帧率收益。

### 7.9 v6 独立 world 八场景全部通过，正式产物身份

[world-oracle-c6.json](../../artifacts/network-cadence-100ms/world-oracle-c6.json) 保存 **`passed: true`、`completedCases: 8`、`totalCases: 8`、`errors: []`**，不是超时后留下的 partial JSON。全部 case 和八份 identity 均对应第 7.8 节的 fixture WASM `74a6f37d…`；2P 使用 loadouts=[0,1]，3P 使用 [0,1,2]。本次为 headed Chrome、实际游戏 canvas 的 Intel UHD / ANGLE D3D11；每个 identity 的 `nativeGpuVerified` 均为 true。

| 独立 world 模式 | 2P | 3P |
| --- | --- | --- |
| shooting | PASS，含 advance=24 / 60 | PASS，含 advance=24 / 60 |
| focused-bombs | PASS，含 advance=24 / 60 | PASS，含 advance=24 / 60 |
| native-death | PASS，含 advance=24 / 60 | PASS，含 advance=24 / 60 |
| unfocused-bombs | PASS，含 advance=24 / 60 | PASS，含 advance=24 / 60 |

每个 case 的初始 probe 与 `later` 中 advance=24、60 的 probe 均保存 `[0..2]=[1,1,0]`，恢复/重算标志 `[32..35]` 均为 1，共八个场景、每场景三个生命周期取样点。报告保留各自原始数组，没有用 owner 命令中的固定 worldProbe 代替这些独立场景。八场景的内存峰值也不移用为第 7.8 节固定场景的保存量对照。

此 oracle 的明确范围是 **稳定关卡中的原生 Update 与语义 Draw 回滚恢复**。本次 `bombLazy`、`shotSparse`、`recordSparse` 为 true，`optimizationOracle` 为 false；不能将其写成额外完成了优化开关全矩阵。JSON 的 scope 仍明确排除尚未验收的 network correction / output fences；本次通过不等于网络端到端正确性、所有关卡/加载边界或平滑度目标全部完成，也不回填旧 c3c 超时运行的身份。

主线程在 HEAD `9b43f35fe53e6cc5dab79a8e7e2a6372c6ea01ad` 完成正式 v6 构建，记录的构建窗口为 **2026-10-12 06:46:20–06:47:56 UTC**。[builds/receipt-c6.json](../../artifacts/network-cadence-100ms/builds/receipt-c6.json) 保存 `builtAt: 2026-10-12T06:46:20.091Z`、multiplayer、`diagnostic: false` 与 **3,607,029 B** 的 WASM；生产提交与结束时刻来自主线程构建记录。正式 SHA-256：

`d405ec608782af4a0bf421d97db77501d46303ef3bdefc004737e215c66f7b5a`

这个正式包与上述 3,820,570 B 的 fixture 包身份不同。首轮 c7 严重失速并保留为失败；c7b 无 body 的 Not RTC 失败及同冻结包 c7c 的有效测量均已记录于第 5.8–5.9 节。c7c 两端约 49.65 Hz，仍未达到 CPU4 60 Hz，也没有解释 c7 的根因；本节正确性通过不转换成性能通过。

### 7.10 Runtime host 功能门：修复本地服务 404，2P manual 已通过，其余待完成

[host-c6-summary.json](../../artifacts/network-cadence-100ms/host-c6-summary.json) 保留首轮 2P 失败：Runtime 动态导入 `/runtime/startup-branding.mjs` 返回 HTTP 404，`timingVerified: false`。这是本地测试服务未发布依赖造成的启动失败，没有完成 host 功能门；不归为游戏 canonical 或帧时间失败。主线程已修正 `portable/multiplayer/serve.mjs:30` 的发布白名单，以及 `check-network.mjs:48` 的 host 依赖哈希列表，两者现均包含 startup-branding.mjs。原失败保留，后续结果使用新的运行身份。

截至本次读取，[host-c6b-summary.json](../../artifacts/network-cadence-100ms/host-c6b-summary.json) 已保存一项完成记录 `host-c6b-2p-mode2-d2`，07:05:40.448–07:07:29.527 UTC，suite 与 host 均 `passed: true`、host errors=[]，正式 WASM 仍为 `d405ec60…`。详细 [host.json](../../artifacts/multiplayer-tests/network-413107e6-c772-4bf4-ba84-2bd860e817ea/host.json) 核对：

| 2P Runtime host 检查 | 实际结果 |
| --- | --- |
| requested mode2 / D2 → effective timing | mode0 / D0；两端 observed timing 均为 0D0，timingVerified=true |
| 普通输入帧偏移 | configured=0，firstMovementOffset=0 |
| 实际触控帧偏移 | configured=0，firstMovementOffset=0 |
| 保存后重开 / 房间保留 | storageReload=true，roomRetained=true |

`portable/multiplayer/runtime-host.html:15` 使用 `manual=1` 打开 Runtime。此处是 **Runtime host 协议、原生触控、保存/重开与 live relay admission 的功能门**；0f 是该 manual 推进下的逻辑帧偏移，不是已测得 0 ms 输入到画面延迟。详细 JSON 自身的 scope 也排除 public Launcher 产品声明。这不是完整 Launcher UI 操作流程、真实 rAF/CPU4 平滑度或公网直连验收。

3P 与 pure-delay 项目仍在主线程运行，当前摘要只含上述 2P 完成记录；不写成完整 host 矩阵已经通过。后续按各自 report、实际 observed timing 和结果追加，不用 2P 的通过替代其他模式。

## 8. 接下来的验证与验收边界

### 8.1 保留同包两类结果，收敛普通 callback 异常并补齐功能门

步骤 25 和随后 c3b 编译失败的原因、修复及 c3c owner 全 PASS 已记录在第 7 节。v3、v4、v5 正式结果均已保存，CPU4 仍未达标；Camera、BackgroundMath 与投影 oracle 已通过，投影首个失败和参考修正理由也已保留。v5 的 c6 协商失败与 c6b 运行器路径错误分别保留，c6c 才是有效 RTC 性能样本。v6 ScreenEffects 批量保存的 owner、固定 worldProbe 与独立 world 八场景套件已通过；正式 v6 已冻结为 `d405ec60…`，首轮 c7 严重失败，c7b 为无 body 的启动失败，c7c 则完成有效测量但仍约 49.65 Hz。下一步针对两次差异继续核查 c7 中 P1 无 correction/resim 时的普通 callback 异常，保留路线、调度和工作量证据；同时记录正在运行的 3P/pure-delay Runtime host 功能门。继续保留真实 world roundtrip、相同输入与恢复边界的原断言，以及投影最终逐位比较；不通过减少语义 Draw 或放松 oracle 完成验收。

正式测量必须记录独立 Release WASM SHA、构建输入身份、effective/native timing 与 route。构建完成后再串行测试；数值 oracle 和带 CPU sampling 的诊断各自记录，不混成无采样正式帧耗时。若后续重新采样，应保留实际 warmup、Profiler.start 与 body 的边界，避免再次沿用“fresh 240-frame lead-in”的错误解释。

### 8.2 继续两类对照，避免解释混淆

第一类是用户实际入口对照：旧 mode2/auto 的真实校准 D 与候选 normalized mode0/D0，回答“本地输入延迟策略改变后，整局是否仍平滑”。保留两端的实际 D、预测缺口、重演量和新状态间隔。

第二类按需要使用同 D0、同冻结工作负载的对照，隔离实现本身的成本；不能从第一类直接断言某个 journal 或 scheduler 补丁独立节省了多少 CPU。是否需要进一步测试，取决于当前正式结果留下的具体风险，避免无目的扩展矩阵。

原速与 CPU4 都看 Present、advance、callback 的分位数和长尾；同包异常应做相邻旧包/同包复测，并保留顺序、构建活动及所有失败。不以“平均接近 60”掩盖 >100 ms 的活动游戏空档。

### 8.3 触控与网络边界

桌面触控至少保留持续移动、反向、停指未抬起、抬起/取消/重新接触，分别计量。持有目标正确预测带来的收益，不应外推为每帧改变目标的持续移动也无需回滚。

稳定基础延迟与短暂尖峰分别观察；制动应在真实缺口增长时渐变，恢复后不产生多 tick 追赶突发。有限 H 用尽且对端没有新输入时，等待仍然允许；不能为回避正常等待而改变真实输入合同。

RTC direct、独立 relay、公网/NAT/TURN 各自是不同条件。当前本地 RTC 和应用发送延迟证据不能代表所有路线。若继续查路由，须记录选中路径及实际收发，不能仅凭房间选项判断 direct 已工作。

### 8.4 本轮可以怎样对用户承诺

现在应同时报告 v6 的两类结果：c7 为 17.593/17.554 Hz，P1 普通 callback 最大 147.0 ms，advance 最大 190.8/203.2 ms、>100 ms 为 84/27 次；同冻结包 c7c 为 49.646/49.664 Hz，advance 最大 70.0/101.7 ms、>100 ms 为 0/1 次，Present 最大 60.0/86.8 ms。两次各自 canonical 均一致，c7 的失败是缺少 P1 远端输入回滚覆盖并伴随严重失速，c7c 通过运行门。不能用一次有效部分改善抹去严重失败，也不能宣称 ScreenEffects 独立提速已证明。c7b 与原速 c6 的 Not RTC 均没有性能 body，不能据此报告新的原速表现。TH08 仍未达到并验收 TH09 的整体体感，不能承诺 CPU4 稳定 60 Hz 或零长停顿；显式 D0 和 manual host 的 0f 也不等于已测得完整操作延迟收益。

仍可明确报告已实施的 once capture、普通尾包合并、恢复不呈现半成品等约束，以及第 7.3 节 v3 固定真实场景的资源 journal 峰值少 88,784 B / 43.41%、总 world 保存峰值少 12.82%。v6 的固定 worldProbe 加回 1,792 B，当前总峰值相对同一旧场景净少 86,992 B / 12.56%，不能沿用 v3 总量比例。owner probes、Camera、BackgroundMath 与投影 oracle 已通过，v6 独立 world 八场景套件现已完整通过；其 stable-stage 恢复范围不扩写成网络纠正与输出边界也已验收。数学快路已进入有正式实测的 v5，改进属于组合版本的一次对照证据，未分离每项独立收益；CDP 采样占比不能替代收益实测。v6 ScreenEffects 批量保存的对应恢复门已通过、正式包已完成，但有 c7 严重失速与同包 c7c 部分改善两类结果，独立帧时间收益仍未证明。2P Runtime host 的 manual 功能门已通过，3P/pure-delay 结果待补，不能写成完整 Launcher UI/性能验收。

性能承诺应限定到明确产物和实测条件：在重复正式运行中，是否减少相同场景的新状态长空档，是否保持低显式输入延迟，是否避免稳定网络下出现由本端工作量引起的持续失速。没有结果时写“待验证”，不以实现数量代替体验改善。

## 9. 当前结果与余项（交主测量线程继续补齐）

本区已补入 v6 c7 严重失败、c7b 启动失败与同包 c7c 有效测量，以及原速 50 ms c6 的无 body 启动失败；旧正式结果、采样与 oracle 身份均保留。v6 owner/固定 worldProbe 与独立 world 八场景已通过，2P Runtime host manual 功能门在修复本地 404 后通过。待补的是普通 callback 异常根因、正在运行的 3P/pure-delay host 结果、成功的 v6 原速性能样本，以及网络纠正/输出边界的独立验收。继续追加时保留全部失败，不以单次复测恢复完成根因结案。

| 项目 | 当前状态 / 待补内容 |
| --- | --- |
| v3 owner/resource 步骤 25 | 已记录越界槽、c3b 私有访问编译失败、`c910a97` / `bc646f9` 和 c3c 全 PASS；保留原失败证据。 |
| v3 world / 恢复正确性历史 | 210 秒预算终止；2P shooting / focused-bombs / native-death PASS，无最终 JSON。旧记录原样保留；后续 v6 完整八场景另列，不能回填为该次通过。 |
| v3 保存量 | 固定场景资源峰值 −88,784 B / −43.41%，总 world 峰值 −12.82%；不作为 CPU 收益。 |
| v3 正式 Release | 四次新运行均为 `4ea980d640d6662f20cdbc0db8cfe7226f3e7af3f628752a11ab51d8ce0fd6f9`；effective/native mode0/D0。正式汇总已保存；本次报告编辑未重跑构建输入哈希或导出清单校验。 |
| v3 原速正式结果 | 单程 50 ms：59.246/59.272 Hz；单程 100 ms：45.191/45.207 Hz。两次均无 >100 ms advance；完整输入到呈现延迟和用户体感尚未验收。 |
| v3 CPU4 spike | c4：39.416/39.450 Hz，未达标；同包 cap60：41.292/41.257 Hz，仍未解决且出现 204/146 ms advance 最大间隔。 |
| Camera oracle | 24 组合 / 11,856 checks PASS；已组合进入 v4，独立 CPU/FPS 收益未分离。 |
| v4 完整恢复→forward | 已进入正式 `6e091598…`，c5 为 43.866/43.860 Hz，仍 FAIL；Present >100 ms 1/1、advance >100 ms 1/1，完整数据见 5.6。 |
| v4 正式 profile | 同 SHA 的独立 CDP 采样已解析；180 warmup 在采样前，采样含 setup 与到 body 240 前约 60 forward。保留旧 scope 标签并更正解释，热点占比不作收益。 |
| v5 BackgroundMath | 12 模式 / 310,860 checks PASS，fast 20,580 / fallback 290,280；测试 SHA `e4994378…`，仅数值正确性。 |
| v5 projection oracle | 首个 case 1626 NaN payload 失败保留；已核实原 helper 与独立 clone 的参考差异，最终仍逐位比较，24 组合 / 43,657 checks PASS，测试 SHA `4d94bb8c…`。 |
| v5 正式包 / CPU4 | 生产提交 `fa3913cb…`，WASM `8c763631…`，3,606,811 B；c6c 复用 c6/frozen。48.759/48.675 Hz，Present 与 advance >100 ms 均为 0/0；有界改善，60 Hz 目标仍 FAIL。每端一个匹配计数窗口，非独立补丁因果收益。 |
| v5 启动尝试 | c6 Not RTC：服务器已锁定 relay、body 空，原因未定位且发生在 CPU4 throttle 前；c6b 仅复用路径错误、没有浏览器结果。失败证据保留，c6c 才是真正 RTC 性能样本。 |
| v6 ScreenEffects 批量保存 | 提交 `9b43f35f…`；旧 profile 的 ScreenEffects 栈下 Touch self 为 0.876/0.830 秒，两池块数 1,024→2。fixture `74a6f37d…` 已通过 native GPU owner/固定 worldProbe；总峰值 605,390 B，相对 v3 +1,792 B、相对旧固定场景 −86,992 B / −12.56%。独立八场景已通过；formal v6 有 c7 严重失速与 c7c 部分改善，单项性能收益未证明。 |
| v6 独立 world | `world-oracle-c6.json`：passed=true、8/8、errors=[]；2P/3P × 四种模式及 advance=24/60 后续 probe 均通过，实际 native GPU。限定 stable-stage Update / 语义 Draw 恢复，不替代 network correction / output fences 验收。 |
| v6 正式 Release | HEAD `9b43f35f…`；receipt-c6，WASM `d405ec60…`，3,607,029 B，diagnostic=false；构建窗口 06:46:20–06:47:56 UTC。首轮 c7 已记录为失败；两端 canonical 相同，P1 无 correction/resim，报 No measured remote-input rollback。同包 c7b 启动失败，c7c 完成有效测量，全部保留。 |
| v6 首轮 CPU4 c7 | 17.593/17.554 Hz；P1 普通 callback 均值 39.383 ms、p99 83.6 ms、最大 147.0 ms。advance 最大 190.8/203.2 ms、>100 ms 84/27 次；实际 RTC/native GPU/mode0/D0，原始失败与选取指标均保留。 |
| v6 同包 CPU4 c7c | 49.646/49.664 Hz；callback/新逻辑 tick 16.468229/16.554688 ms，advance 最大 70.0/101.7 ms、>100 ms 0/1，Present 最大 60.0/86.8 ms。运行门通过、canonical 一致，仍未达 60 Hz，不覆盖 c7 失败。 |
| v6 无 body 启动失败 | CPU4 c7b、CPU1 单程 50 ms c6 均 Not RTC，两端 rows=[]；没有 FPS 或平滑度结果，RTC 必须成立的门保留。 |
| Runtime host 功能门 | 首轮 startup-branding.mjs 404 与修复保留；host-c6b 当前 2P manual 已核对实际 0D0、普通/触控 0f、保存/重开与房间保留 PASS。3P/pure-delay 仍运行；不是完整 Launcher UI/性能验收。 |
| 呈现端点 hold | 已确认提交 alpha=1 后清 gate、防倒插及无 tick 可重画端点的源码路径；未测正式触发频率，geometry packet 暂未实施。 |
| 独立路线与触控边界 | 本地 RTC、桌面合成触控证据不能覆盖公网 direct/relay/TURN 或手机；后续只针对具体剩余风险扩展。 |
| v2 严重异常后续 | 15.8/15.9 Hz 与真实长 callback 的失败仍未解释；同包重测约 44 Hz 不抹去旧失败。v6 c7 再现相似的单端普通 callback 异常形态，但尚未证明同根因。 |
| 结论与发布状态 | 不能宣称 TH08 已达到 TH09 体验。本报告没有新的部署记录，也没有证明用户当前测试站正在运行本候选。 |

## 10. 证据目录与发布身份注意事项

当前测量证据根目录：

`D:/workspace/eagler/worktrees/th08-network-cadence-100ms/artifacts/network-cadence-100ms`

本报告新建于根目录 `docs/multiplayer/`，遵循既有 `rollback-journal-work.md` 的工程记录位置，未新建 `th08_web/docs` 的平行报告。

只读 public identity 记录 [public-identity/report.json](../../artifacts/network-cadence-100ms/public-identity/report.json) 保存了 2026-10-12 的一次已执行探测：主站入口返回指向 `9289c8acba77472d167fcd7158c141f289c9ff89079c71c3892c0f106c007a3e` 子目录的跳转页；测试站该次 fetch 失败。此记录**没有证明该目录名就是 WASM SHA，也没有核实用户当前测试站运行的是本候选**。当前本地 React 默认值同样不能代替线上部署包身份。

前序 TH08 journal 工作及其独立正确性/性能边界仍见 [rollback-journal-work.md](rollback-journal-work.md)。本报告不覆盖或改写该阶段已经保存的结论。

## 11. 真实 Runtime 触控入口补齐（2026-10-12 后续记录）

### 11.1 从获认可 TH09 中找回的遗漏

再次比对获认可的 `th09-managed-touch-4x` dirty 源码后发现：TH09 的真实 managed receiver 已将 `direct-touch`、`touch-controls`、`touch-cancel` 与键盘一起即时分发；TH08 旧 `th08_web/sdl-runtime/shell.mjs` 仅让 keyboard / keyboard-clear 绕过 Promise 命令队列。文件系统同步或资源下载未完成时，真实 TH08 触控也会跟着等待。这是确定存在的入口差异。此前 `measure-smoothness.py` 直接调用 native 合成触控，绕过 shell，不能用其 FPS 来宣称已覆盖本问题。

当前 shell 改动在原 source/origin/protocol/game/epoch 校验之后即时处理这三类触控。只有当前 document/iframe owner 有效、初始化与 launch 完成、可接受触控且不处于 hidden、closing、Replay reopen 或 spectator 状态时才递交 native。不能接受的消息即时 ACK 并丢弃；不在恢复后回放。letterbox 触控桥采用同样资格检查，取消时清空活动指针。

TH08 原生 `cancel_transient()` 会清 focus、摇杆和短脉冲，但保留 fire toggle；本次没有用伪造 fire=false snapshot 代替它。Bomb/Escape 的 source serial 在资格判断前消费，只有被接受的新变化推进 mapped serial；native open/reopen 的 `begin_session()` 会归零原生序号，所以 shell 同步归零 mapped 而保留 document 内的 source 基线。这保留首次有效按下、uint32 回绕和普通 Replay 菜单触控，同时防止被拒绝的旧序号在新会话中变成迟到的炸弹。

pagehide 视为当前 Runtime document 的终止，pageshow 不重新开放已退休的输入 owner；这不构成同一旧 document 的 BFCache 原地恢复支持声明。

### 11.2 实际旧版失败 / 新版通过

真实 shell 测试位于 `portable/test-th08-shell-touch.mjs`：执行完整 shell ESM 和真实 eagler-host / directory-keyboard，只有 native 与平台依赖使用小型模拟。原始旧 shell 通过 `git show HEAD:th08_web/sdl-runtime/shell.mjs` 按字节留存在 `artifacts/network-cadence-100ms/touch-shell-before.mjs`。

- 第一轮 c1 因本机 Node 的 SyntheticModule.link() 返回 undefined、测试错误地直接调用 .then 而失败。该测试兼容性错误与日志已保留；修改为 Promise.resolve(module.link(...))，未修改生产 shell 来迁就测试。
- c2 新 shell **11/11 PASS**，包括 FS/resource 阻塞、初始化/launch 中间状态、hidden、closing、新会话、Replay reopen、spectator、owner 退休、信封校验、serial 回绕及 letterbox 生命周期。
- 相同测试、相同真实旧 shell，仅选择两个队列场景，**2/2 按预期失败**：分别为 `direct-touch ACK waited behind the filesystem queue` 与 `direct-touch ACK waited behind the resource queue`。这是旧缺陷实际复现，不是用仿写 receiver 证明自身。
- 原有 `portable/test-eagler-host.mjs` helper 测试通过。两文件另经独立静态审查，核对了真实 Launcher 在 launch ACK 后才挂载 TouchOverlay 并发送初始 fire=true snapshot，以及原生 cancel/reset/begin_session 的不同语义。

证据：[touch-shell-validation-c2.json](../../artifacts/network-cadence-100ms/touch-shell-validation-c2.json)、[touch-shell-new-c2.log](../../artifacts/network-cadence-100ms/touch-shell-new-c2.log)、[touch-shell-old-red-c2.log](../../artifacts/network-cadence-100ms/touch-shell-old-red-c2.log)、[touch-host-helpers-c1.log](../../artifacts/network-cadence-100ms/touch-host-helpers-c1.log)。

### 11.3 真实 Runtime host 功能门

旧 shell 的 host-c6b 后续结果现已补齐：2P 和 3P 的 mode2/D2 请求均通过，并观察到 native mode0/D0、键盘/触控首移动 offset=0、storage reload=true、room retained=true。3P 套件为 `network-17d5179f-ef32-4944-ad60-2e7a47e92990`。同批 3P pure-delay 在 outer 180 秒截止时未产出最终 suite，**不记为通过**。

新 shell 的 **host-c6c-2p-mode2-d2** 已由 root 实际完成：07:33:03.002–07:34:54.133 UTC，111 秒，进程 exit=0、suite passed=true、host errors=[]。使用正式 `d405ec608782af4a0bf421d97db77501d46303ef3bdefc004737e215c66f7b5a`，请求 mode2/D2，两端 observed mode0/D0；键盘/触控首移动 offset=0，storage reload=true，room retained=true。日志实际选择 RTC。

证据：[host-c6c-2p-mode2-d2-summary.json](../../artifacts/network-cadence-100ms/host-c6c-2p-mode2-d2-summary.json)、[suite.json](../../artifacts/multiplayer-tests/network-e7cd4b53-1c94-4b83-963f-470ee029d437/suite.json)。

这些是正式 native + 实际 Runtime shell/managed DATA/room/save 合同的 manual 功能门，不是完整 React Launcher UI、持续触控负载或 rAF 运动性能验收。

新 shell 的 **host-c6c-2p-mode1-d2** 也已完成：07:36:12.033–07:38:13.118 UTC，exit=0、suite passed=true、errors=[]；两端保持 mode1/D2，键盘与触控首移动 offset 均为 2。每个稳定 checkpoint 的 driver[3:6]=[0,0,0]、driver[10]=0、net[7]=0 门通过；保存/重开、房间保留通过。见 [host-c6c-2p-mode1-d2-summary.json](../../artifacts/network-cadence-100ms/host-c6c-2p-mode1-d2-summary.json) 和 [suite.json](../../artifacts/multiplayer-tests/network-53f456f8-7ccd-4536-8fb2-3b13624bb8c3/suite.json)。这不追认前一次超时的 3P pure-delay，也不是完整 3P pure-delay 覆盖。

### 11.4 旧 motion 数据用新口径复算

`analyze-motion-cadence.mjs` observationRevision=2 的 10 项测试已由 root 通过。新分析器将 admission wait、correction yield/work 和 pendingUnknown 分开；保留原 aggregate 并注明混合口径。alpha1 时没有有效 previous model 不再让实际几何 hold 消失，连续提交的同 owner/sprite 几何仍可比较；时间是观察下界，不是精确 GPU scanout。

root 对已有 `motion-candidate-cpu1-50ms-c1`、`motion-candidate-cpu4-spike-c1` 原始 JSON 复算并另存 `motion-analysis-v2.json`，未覆盖旧分析。两份使用同一个 04:17 的旧 diagnostic fixture `d4e70bb3…`，不是当前 v6。

四个 peer 的 endpointHolds.runs / heldCommitPairs / gateDisabledReadonlyAlpha1HeldCommitPairs / highRefreshSemanticForwardBudgetCandidates 全为 0。CPU1 稳定 owner 提交对为 1,068/1,088，alpha1 端点样本为 0；CPU4 稳定对为 552/549，alpha1 端点为 567/564，但每次提交的同 owner 几何都有变化。CPU4 的 observed commit gap 中位数为 97.5/74.2 ms，最大 678.2/739.6 ms；0 个 hold 绝不表示没有严重卡顿。

因此，这两份旧数据**没有证明**当前应优先省掉 alpha1 重复 Draw。源码确实存在该路径，也经只读依赖审查确认有窄范围可行方案，但尚未据此修改 native。最新 fixture 的运动测量仍是必要证据；不能把源码可能性写成用户体感的已确认根因。

### 11.5 c6b 原速测量超时与证据限制

新 browser evidence observer 已经 root 检查 Python AST、19 段内嵌 JavaScript，并用 VM 验证 WebSocket constructor/subclass、send receiver/参数/返回值/同一异常对象、二进制跳过、lobby 排除、字段白名单及记录上限；该检查通过，见 [browser-evidence-validation-c1.json](../../artifacts/network-cadence-100ms/browser-evidence-validation-c1.json)。

随后 `launcher-candidate-cpu1-50ms-c6b` fresh freeze 的 manifest 记录正式 WASM `d405ec60…` 与新版 observer，relay 选择 RTC。但 07:23:41.861–07:27:41.884 UTC 的 outer runner 达到 240 秒截止，worker 没有返回原始报告，**没有可报告的 FPS、运动平滑度或 browser evidence 结果**。

已经定位一个确定的测试监督问题：该 job 的 --timeout=180，对应原 supervised_run 的 worker 硬限 180×2+180=540 秒，outer 240 秒会先杀 supervisor，使 worker/relay 的 finally 来不及执行。root 核对进程路径与父进程后，仅清理了这次运行的 worker PID 43236（及其 Playwright/浏览器树）与 relay PID 47092，两次清理均成功；见 [outer-timeout.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu1-50ms-c6b/outer-timeout.json)。没有结束其他用户进程。

这解释了为何原收尾机制没能生效，**尚未解释 worker 最初停在哪个等待点**。该次失败保留，不用“环境波动”替代原因。下一新冻结加入低频阶段记录、长等待 Python 栈和显式 worker 总预算，先让异常可定位、进程可正确回收，再继续最新 fixture 与正式性能测量。


## 12. 当前本地交付与后续验证（2026-10-12 07:41–07:57 UTC）

本节按后续实际证据追加，保留第 1–11 节各自记录时点的结果和失败。**已生成包含真实 managed 触控入口修复的本地 multiplayer Runtime 包；截至 07:49 UTC 的公开文件探测，主站与测试站仍提供旧包，候选未发布。** 最新三次 fixture 运动运行都未通过启动 RTC 门，不能补出新的 FPS、运动连续性或 CPU4 改善结论。第 11 节已完成的 shell 回归测试与 Runtime host manual 功能门，和这些尚未进入测量阶段的运行分别计数。

### 12.1 Shell 已本地提交，Runtime 包按文件身份验证

第 11 节的 shell 触控修复已由主测量线程本地提交为 `c7395abb0587197d995be9c7483c79b3253799a1`。[runtime-managed-touch-v6-c1-receipt.json](../../artifacts/network-cadence-100ms/runtime-managed-touch-v6-c1-receipt.json) 记录 `sourceHead` 为该提交，`packagedAt: 2026-10-12T07:41:40.240Z`、`passed: true`，product/profile/variant 分别为 th08mp/multiplayer/multiplayer。输出目录为：

`D:/workspace/eagler/worktrees/th08-network-cadence-100ms/artifacts/network-cadence-100ms/runtime-managed-touch-v6-c1`

收据的 `verifiedIndexedFiles: 18` 表示主测量线程已逐一核验 [runtime-files.json](../../artifacts/network-cadence-100ms/runtime-managed-touch-v6-c1/runtime-files.json) 所列 18 个文件；目录索引自身另有独立 SHA。核心身份如下：

| 文件 / 身份 | 字节数 | SHA-256 |
| --- | ---: | --- |
| 正式 th08-sdl.wasm | 3,607,029 | `d405ec608782af4a0bf421d97db77501d46303ef3bdefc004737e215c66f7b5a` |
| th08-sdl.mjs loader | 261,952 | `d482728ab977b4eb86034e375f096faf10219e2ec5643931ed8fbe5199363d40` |
| 新 shell.mjs | 31,496 | `a112a6b993c348942bc773fc19c5fed3409667d88cea413f4d1c90cd53553c2e` |
| runtime-files.json 目录索引 | — | `68daf64b9c3d23ae57af4591cfe4b67cccf7523441fcd11b2d2aed85fdf70da6` |

[包内 manifest.json](../../artifacts/network-cadence-100ms/runtime-managed-touch-v6-c1/manifest.json) 的 execution 与上述正式 native/loader 一致，`builtAt` 仍为 native 的 `2026-10-12T06:46:20.091Z`。这次补齐的是 Runtime shell，正式 native 没有为打包而重建；不能只用 native SHA 或旧 builtAt 判断是否已取得新的触控入口。收据同时保存 `javascriptExpressionsValidated: 19`、`retailDataBundled: false`：继续遵守 Runtime 与零售 DATA 分离合同。本次为本地打包与校验，没有候选部署记录。

### 12.2 主站、测试站实际下载身份仍为旧包

[public-identity-c2/report.json](../../artifacts/network-cadence-100ms/public-identity-c2/report.json) 补足了第 10 节初次探测没有完成的实际文件核验。两站入口均返回 200 并指向 `9289c8ac…` 目录；判断部署身份依据是下载到的文件哈希，而非目录名称。两站 manifest 的 `builtAt` 均为 `2026-10-12T00:40:56.797Z`。

| 公开站点 | 实际 WASM 观察时刻（UTC） | 实际 WASM | 实际 shell |
| --- | --- | --- | --- |
| 主站 touhou.vip | 07:49:08.290 | 3,595,502 B，`8ea9b23f…`，200，matchesManifest=true | 29,002 B，`1c1ebe8c…`，200 |
| 测试站 test.touhou.vip | 07:49:08.370 | 3,595,502 B，`8ea9b23f…`，200，matchesManifest=true | 29,002 B，`1c1ebe8c…`，200 |

两站实际文件的完整 SHA-256 相同：

- WASM：`8ea9b23fc9ffbdb25ddd1d5bf1a94284fef5e60df00390ec735557ff27e71f55`。
- shell：`1c1ebe8cce9e4c3a98783476965e044a648e2e544d6b9921d4f1d951b5e46857`。

这些与第 12.1 节的本地 `d405ec60… / a112a6b9…` 不同。因此，在该次探测时，用户访问两站尚未取得本地候选。报告顶层 `actualFilesObservedAt` 为 `2026-10-12T07:49:10.657Z`；只描述该时点，不外推之后站点永远不变。

主测量线程记录，`checks[].error` 中保留的 HTTP 404 来自辅助 `runtime-files.json` 探测；后续实际 WASM、shell、multiplayer-host 文件均成功下载。该辅助索引 404 不构成游戏启动或游戏逻辑错误，也不能抹掉已经保存的实际文件身份。原探测错误继续保留。

### 12.3 WorkerDiagnostics 已补齐超时层次；三次新 fixture 均为启动 Not RTC

针对第 11.5 节 outer 240 秒早于 worker 540 秒的确定监督缺陷，[measure-smoothness.py](../../portable/multiplayer/measure-smoothness.py) 已增加 WorkerDiagnostics、显式 `--worker-timeout`、阶段栈记录与独立收尾预算。同步 CDP 调用期间 Python 循环本身无法执行 deadline 检查，因此 worker 总期限仍由外层进程监督；faulthandler 只输出长阶段的 Python 栈，不将长等待自动定性为浏览器挂起。

最新三次均记录 phase=90 秒、worker=180 秒、worker 回收=15 秒、relay/server 收尾=20 秒、outer margin=10 秒；计入各次启动耗时后的建议 outer 总限均为 **227 秒**。见各自 [c6 timeout-budget](../../artifacts/network-cadence-100ms/motion-candidate-cpu1-100ms-c6/timeout-budget.json)、[c6b timeout-budget](../../artifacts/network-cadence-100ms/motion-candidate-cpu1-100ms-c6b/timeout-budget.json)、[c6c timeout-budget](../../artifacts/network-cadence-100ms/motion-candidate-cpu1-100ms-c6c/timeout-budget.json)。Windows 回收仅指向本次持有的 worker PID 与其子树；relay 由 supervisor 持有的本次进程单独清理，不按 Chrome 进程名批量结束。

这里的 c6/c6b/c6c **完整运行名是 motion-candidate-cpu1-100ms-c6/c6b/c6c**，不要与第 5 节正式 CPU4 样本或第 11.5 节原速 50 ms outer timeout 混用。三次原始报告分别为 [c6 1-frontier.json](../../artifacts/network-cadence-100ms/motion-candidate-cpu1-100ms-c6/1-frontier.json)、[c6b 1-frontier.json](../../artifacts/network-cadence-100ms/motion-candidate-cpu1-100ms-c6b/1-frontier.json)、[c6c 1-frontier.json](../../artifacts/network-cadence-100ms/motion-candidate-cpu1-100ms-c6c/1-frontier.json)。

| Fixture 运行 | worker-run 记录时段（UTC） | 原始结果 | 收尾 |
| --- | --- | --- | --- |
| c6 | 07:42:18.398–07:42:40.060 | passed=false，error=Not RTC | browsersClosed=2，teardown.errors=[] |
| c6b | 07:44:45.908–07:45:02.430 | passed=false，error=Not RTC | browsersClosed=2，teardown.errors=[] |
| c6c | 07:57:27.354–07:57:44.005 | passed=false，error=Not RTC | browsersClosed=2，teardown.errors=[] |

三次均为 `production: false`、multiplayer-fixtures WASM `74a6f37d0ceb03720aaad9822c639e6ffde8a91fa5e01f266baa5375a3b8c17a`，CPU rate 请求值 1、单程输入发送延迟 100 ms、jitter=0、touchHz=120；两端 native mode/delay 为 0/0，实际游戏 canvas 的 nativeGpuVerified 均为 true。可是每端 `rows=[]`、impairment matched/sent 均为 0、`throttleApplications=[]`：在配置与测量 body 之前，实际路由已经选择 relay，原 RTC 门据此失败。**这些是保存了完整诊断的启动失败，不是 FPS、输入延迟或运动连续性的测量结果。**

三次 workerDiagnostics 均保存结束事件、`unfinishedStages=[]`，失败报告和正常浏览器关闭已落盘。它们未触发 worker hard timeout，不能据此宣称强制杀进程的故障注入也已通过；第 11.5 节原始失联等待点也仍待解释。

### 12.4 PC Promise 观测器静态与 VM 验证通过，记录测试脚本自身修正

[pc-evidence-validation-c1.json](../../artifacts/network-cadence-100ms/pc-evidence-validation-c1.json) 记录 `validatedAt: 2026-10-12T07:57:07.127Z`、`passed: true`、`pythonAst: true`、`javascriptExpressions: 19`、`browserInitScripts: 1`、`pcObservationRevision: 1`、`errors: []`。所验证的 measurement 源码 SHA-256 为：

`1ebe9af79ce84daf2edc12cb6902c23b8d8f20dc8c2957e157eb130e9f3e9114`

VM 检查覆盖四个方法的原 Promise 身份、receiver、参数和一次 native 调用，同步异常及异步拒绝对象身份，状态 getter 抛错、settlement/诊断记录失败隔离、无 SDP/地址的状态枚举、128 条记录上限下仍完整执行 140 次 native 调用、8 个 PC 身份上限，以及非 Promise 的旧式返回值原样返回。新 observer 仅位于测试脚本的 BROWSER_EVIDENCE_INIT，browser evidence schema 保持 /2，并以 pcObservationRevision=1 标记本次扩展。

主测量线程最初的验证运行自身有表达式计数与 init 外层括号处理错误，修正的是验证脚本的提取/校验逻辑；没有修改生产 native 或 RTC 实现来迁就它。这些早期校验尝试不记为通过，以上成功 JSON 才是本轮验证依据。19 个表达式与 1 段 init 分项记录，不把它们重复计数为额外运行样本。

### 12.5 c6c 唯一 createOffer 到采集时仍 pending，尚不能定性根因

c6c 原始报告的 `browserEvidence.pages` 中，两端 pcObserverInstalled=true，四个方法均已安装；各端 errors=[]、droppedPcOperations=0、droppedPeerConnectionCalls=0，顶层 browserEvidence.errors=[]。P0 只有一项操作，P1 的 pcOperations=[]：

| P0 原始观测 | 实际字段 |
| --- | --- |
| operationId / pcId / method | 1 / 1 / createOffer |
| call.epochMs | 1791791860164，即 07:57:40.164 UTC |
| call.state | player=1，route=null，signalState=1，connection/ICE/gathering=new，signaling=stable |
| localDescriptionType / remoteDescriptionType | null / null |
| outcome / settled | pending / null |
| P0 收到 route=relay | 1791791860182，即 07:57:40.182 UTC |
| P0 report-collected | 1791791860644，即 07:57:40.644 UTC |

[同次 relay.log](../../artifacts/network-cadence-100ms/motion-candidate-cpu1-100ms-c6c/relay.log) 也保存 `ROUTE ... mode=relay`。createOffer 观测到调用后约 **18 ms** 就收到 relay route，报告采集时距调用约 **480 ms**；只支持“在这个观察窗口内未记录到 settlement”，**不支持 createOffer 已挂满 4.5 秒、GPU/浏览器已被证明为根因，或之前所有 Not RTC 都是同一原因**。该报告未记录 setLocalDescription、setRemoteDescription 或 createAnswer 的调用。常见默认 RTC 超时值 4500 ms 是配置来源记录，不是本次已测得的 createOffer 持续时间或 server timer 实测。

独立静态审查还保留一项 Promise 边界：observer 返回原 native Promise，原 receiver/参数、同步异常和调用方 await/catch 收到的拒绝对象不变；但给原 Promise 注册 rejection handler 会将它标记为 handled，因此无人处理的原 Promise 的 unhandledrejection/rejectionhandled 行为会变化。当前 [BrowserPeerTransport.cpp](../../third_party/eagler-common/src/netplay/BrowserPeerTransport.cpp) 的目标调用均立即 await，原 transport 异常传播仍保留。不能把本观测器描述为完全没有语义或计时影响；它增加有界的状态读取和 Promise reaction，也不是生产性能收益。

### 12.6 当前交付与余项

可以交付的是第 12.1 节身份明确、逐文件核验的本地 Runtime 包，以及第 11 节已复现旧缺陷并通过新测试的 managed 触控队列修复。公开站点尚未取得该候选，不能把用户当前在线体验归因于这次本地改动。

后续仍由主测量线程补齐可进入 RTC 测量 body 的最新 fixture/正式运行，并根据有界操作时间戳收敛启动失败；不放宽 RTC gate，也不将三个 Not RTC 样本换算成 FPS。CPU4 稳定 60 Hz、用户认可 TH09 的整体体感，以及 c7 严重失速根因，继续保留为未达成或未定位。候选部署、后续站点身份变化和新的性能结果均待实际证据另行追加。

## 13. 最新有效原速样本与实际运动信号

### 13.1 启动时间线进一步收敛，默认超时没有被修改

补读 c6c 的 P0 WebSocket 时戳后可见：peer-join 为 1791791855114，createOffer call 为 1791791860164，两者相差 **5,050 ms**。冻结的 ensurePeer 在此之前只有 PC 构造、注册、两次 createDataChannel/setupChannel 等同步工作，没有应用层 await。因此大的未解释区间在 offer 调用之前，尚未细分到某一个 native 调用，不能写成 offer Promise 自身用了 5 秒。

主线程另执行 [rtc-offer-isolation-c1.json](../../artifacts/network-cadence-100ms/rtc-offer-isolation-c1.json)：相同 headed Chrome 155 参数、两个独立进程、不加载游戏，两端都完成 createOffer 与 setLocalDescription。记录的 1.109/1.314 秒从 PC 构造前开始，不是纯 createOffer 延迟；该实验没有建立双端 RTC 路由，也不是游戏性能测试。

随后 c6d fixture 仅将测试环境 EAGLER_NETPLAY_RTC_TIMEOUT_MS 显式设为 15000，复用 c6c 的冻结字节；其 RTC barrier 实际用时约 1.528 秒，offer 自调用至 fulfilled 仅 3 ms。其成功不能证明延长超时是必要原因。再后的两个正式原速样本均以默认 4500 ms 完成 RTC；所有原失败保留。没有修改生产 relay 的默认值或测试的 RTC 必须成立条件，也未继续无目标地扩大启动实验。

### 13.2 三个已经进入 body 的原速样本

三个样本都使用两端独立 headed Chrome 155、实际 Intel UHD / ANGLE D3D11、原生 rAF、双端 120 Hz 墙钟合成触控、0 jitter、没有额外尖峰；所有已执行 CDP throttle 记录均为 rate=1、acknowledged=true。每端都存在真实远端 correction，canonical 两端相等、逻辑 frontier 无倒退，错误与关闭错误为空。测试不使用手机。

| 样本 | 类型 / 延迟 | 实际新逻辑帧率 P0/P1 | callback / 新逻辑 tick | Present 最大间隔 | advance 最大间隔 | correction yields |
| --- | --- | --- | --- | --- | --- | --- |
| launcher-candidate-cpu1-50ms-c6c | 正式 d405ec60，单程 50 ms | 59.808 / 59.813 Hz | 4.487 / 4.315 ms | 49.9 / 51.7 ms | 59.4 / 61.4 ms | 0 / 0 |
| launcher-candidate-cpu1-100ms-c6c | 正式 d405ec60，单程 100 ms | 41.179 / 41.260 Hz | 18.724 / 18.944 ms | 61.8 / 70.7 ms | 75.9 / 83.4 ms | 632 / 659 |
| motion-candidate-cpu1-100ms-c6d-rtc15s | fixture 74a6f37d，单程 100 ms | 45.581 / 45.592 Hz | 6.834 / 6.769 ms | 62.3 / 63.8 ms | 66.4 / 72.6 ms | 8 / 7 |

三个样本的 Present 与 advance 间隔均没有 >100 ms。正式 50 ms 的 measured body 为 720 个新逻辑帧，另外两项为 960 帧；不能把这些短样本提升为“永不大卡”的保证。CPU1 100 ms 正式与 fixture 的计算成本差异明显，fixture 不能代替正式结果；两次墙钟触控轨迹也不逐位相同，不能把差额直接当某项优化百分比。rate=1 ACK 是请求与确认的记录，不是独立有效 CPU 速度标定。

证据：[正式50ms](../../artifacts/network-cadence-100ms/launcher-candidate-cpu1-50ms-c6c/selected-metrics.json)、[正式100ms](../../artifacts/network-cadence-100ms/launcher-candidate-cpu1-100ms-c6c/selected-metrics.json)、[fixture100ms](../../artifacts/network-cadence-100ms/motion-candidate-cpu1-100ms-c6d-rtc15s/selected-metrics.json)、[补充成本与配置汇总](../../artifacts/network-cadence-100ms/latest-cadence-evidence-c1.json)。原始 1-frontier.json、冻结 manifest 和 .job 均保存在各自同目录。

### 13.3 这次确实观察到“提交很多、运动暂时不动”

[当前 fixture 的 motion-analysis-v2.json](../../artifacts/network-cadence-100ms/motion-candidate-cpu1-100ms-c6d-rtc15s/motion-analysis-v2.json) 已由 root 离线执行。与第 11.4 节的旧 fixture 不同，本轮稳定对象片段中：

| 端点后保持信号 | P0 | P1 |
| --- | --- | --- |
| hold runs | 60 | 55 |
| 中心完全不动的连续提交对 | 141 | 134 |
| 其中重新执行 Draw / 同一 calculation epoch | 141 / 141 | 134 / 134 |
| 同时观测 admission/correction 等待 | 0 | 0 |
| hold 总时长观察下界 | 393.3 ms | 386.7 ms |

分母是选中弹幕的稳定对象片段，不是整场游戏时间；探针观察提交几何，不是显示器精确扫描时刻。两端已分析连续片段的 phaseRegression 与 reverseAlongForwardEndpoints 均为 0。

最清楚的 P1 源 row2716–2721 中，同一 slot4/life5/state1/script0/sprite6、alpha255、无 shake：row2716 正常相位为 .85768；row2717 完成 5 帧 resim 和 1 帧 forward，debt 只对应新 pair 的 **.13603**，却提交语义 alpha=1。其 callback 实际为 10.8 ms。之后 row2718–2720 的新 commit 和新 Draw 都保持中心 (243.90924072265625,414.5780334472656)，尽管正常相位已分别推进至 .41053/.68053/.97101。三行 waiting/retry/correcting/yield 均为 false；row2721 下一完整 forward 才恢复插值，中心继续前进。

该窗口的保持观察下界 12.7 ms，端点至下一移动提交间隔 18.7 ms。它符合 GameHost 的完整 forward 预算分支，并已排除该片段内缺输入准入、半成品 correction、对象换代和镜头震动。分析器没有原生 budget_spent reason 位，仍保留 candidate 命名，不伪装为直接读取了该分支。

删除提交端点后的 presentation_gate.reset() 不是修复：若 row2718 立即回到旧 pair 的 .41053，已展示位置会倒退约 .556 px。保留该 guard 与 partial-correction 屏障是必要的。单独省掉 row2718–2720 的重复 Draw 只减少成本，不能消除 row2717 提前展示端点的视觉跳停。

当前正常降速的 alpha 分母已经正确：归一后的新增时间进入 network_schedule，同一个 debt 再除以 1/60 得到 alpha；没有“逻辑降速却继续按固定墙钟16.67ms插值”的分母错误。获认可 TH09 没有这一套高刷插值与 alpha1 端点混用的呈现方式，因此应处理 TH08 自身的交互，而不是宣称漏抄了一个 TH09 alpha 算法。

## 14. v7 呈现完成门槛实验

本地提交 **39421339fa0d660b32527704b9d8f8e1715ebad7** 只修改 GameHost.cpp 的最终呈现开始门槛：历史 correction 切片和继续开始新 tick 的规则仍为 8 ms；完整帧完成后，使用独立 **10 ms** 门槛决定是否开始只读插值 Draw，否则沿原来完整端点提交路径。原 semantic Draw/FinishFrame 顺序、correction 屏障、实际提交端点后的 gate reset 均保留，未叠加端点复用优化。

它是 Draw 的开始门槛，**不是整个 callback 的硬上限，也不是保证只增加2ms成本**。TH09 的外层 10 ms 是在完成 tick/render 后决定是否继续下一 tick，不能把这个 TH08 调整称为等价复制。

[endpoint-budget-cost-c1.json](../../artifacts/network-cadence-100ms/motion-candidate-cpu1-100ms-c6d-rtc15s/endpoint-budget-cost-c1.json) 对当前 fixture 全部新提交的高刷完整 forward 语义端点统计：P0/P1 共 74/65 项，其中总 callback <10 ms 为 59/48 项，中位 9.0/9.2 ms。这使 10 ms 候选值得验证，但新增只读 Draw 的耗时与真实发生比例仍需实测；原 row2717 的 10.8 ms 也表明它不会自动消除所有端点保持。

08:11:17.540–08:11:51.631 UTC 已构建 v7 fixture，diagnostic=true、3,820,570 B，WASM **e685f4099cf56d5d222c97936be2619ba2338031fb3b2e0a0231126e513051d4**；见 [run-c7-multiplayer-fixtures.json](../../artifacts/network-cadence-100ms/builds/run-c7-multiplayer-fixtures.json) 与 [receipt-c7-fixtures.json](../../artifacts/network-cadence-100ms/builds/receipt-c7-fixtures.json)。运动连续性、callback 尾部及正式 CPU4 对照以接下来实际结果决定是否保留，不提前宣称收益。

### 14.1 v7 原速 100 ms 运动对照已完成

08:12:19.932–08:13:48.141 UTC 的 [v7 selected-metrics.json](../../artifacts/network-cadence-100ms/motion-candidate-cpu1-100ms-c7-rtc15s/selected-metrics.json) 和 [原始 1-frontier.json](../../artifacts/network-cadence-100ms/motion-candidate-cpu1-100ms-c7-rtc15s/1-frontier.json) 已落盘，passed=true。两端实际加载上节的 fixture **e685f4099cf56d5d222c97936be2619ba2338031fb3b2e0a0231126e513051d4**，native mode0/D0，独立 headed Chrome 155、实际 Intel UHD / ANGLE D3D11、双端 CDP rate=1 且 acknowledged=true；单程 100 ms、0 jitter、无额外尖峰、双端 120 Hz 墙钟触控，RTC 启动环境仍为 15000 ms。measured body 为逻辑 240–1200 的 960 个新帧，两端 canonical 相等、frontier regressions=0，errors 与 teardown.errors 为空。

[v6/v7 运动对照原始汇总](../../artifacts/network-cadence-100ms/motion-v6-v7-comparison-c1.json) 对应 v6 的 motion-candidate-cpu1-100ms-c6d-rtc15s 与本次 v7。以下 hold 专指 endpointHolds.runs，使用同一稳定对象分段与过滤规则；分母为选中弹幕的 observedStablePairMs，不是全场墙钟时间。

| 运动指标 | v6 P0/P1 | v7 P0/P1 |
| --- | --- | --- |
| 稳定提交对观察时长 | 13.6170 / 13.6685 s | 12.6130 / 12.6307 s |
| 稳定提交对数量 | 2221 / 2222 | 2056 / 2052 |
| 端点后 hold runs | 60 / 55 | **12 / 9** |
| 每稳定观察秒 hold runs | 4.406257 / 4.023850 | **0.951399 / 0.712550** |
| 端点保持中的中心不动提交对 | 141 / 134 | 28 / 24 |
| hold 总时长观察下界 | 393.3 / 386.7 ms | 63.7 / 64.9 ms |
| phaseRegression / reverseAlongForwardEndpoints | 两端均 0 / 0 | 两端均 0 / 0 |

v7 剩余的 28/24 个保持提交对仍全部是 alpha1、gate 关闭、重新 Draw、同一 calculation epoch、没有 forward 或 authoritative 位移；这些行中 admission/correction 等等待信号为 0。改动减少了已识别的端点后保持，没有将它完全消除。两次 P0 都有一个截尾 hold run，时长继续按观察下界报告。

| v7 fixture 运行指标 | P0 | P1 |
| --- | --- | --- |
| 实际新逻辑帧率 | 45.655803 Hz | 45.657323 Hz |
| callback p99 / max | 9.7 / 12.3 ms | 9.5 / 19.0 ms |
| callback 总耗时 / 新逻辑 tick | 6.711250 ms | 7.010417 ms |
| Present 最大间隔 | 64.0 ms | 60.9 ms |
| advance 最大间隔 | 64.0 ms | 60.9 ms |
| Present / advance >100 ms | 0 / 0 | 0 / 0 |
| correction yields | 8 | 5 |

该对照支持“这个高刷、降速场景中已识别的端点保持明显减少”，不支持“整体性能提高 80%”或所有场景同幅改善的承诺。两次由墙钟调度形成的实际触控轨迹与回滚历史不同，不是逐位固定输入的单变量性能实验；稳定片段分母也不同。canonical 相等指每次运行内部两端一致，不要求 v6 与 v7 跨运行 hash 相同。fixture 的诊断开销和本轮有限观察窗口仍保留，不能用它替代正式 CPU4 验收。

### 14.2 第 13 节正式与 fixture 配置的独立只读核对

已对第 13.2 节三个样本的原始 report、.job、manifest、冻结 runner/hook 及 native 默认做交叉核查，没有找到能确定解释正式 100 ms 变慢的 body 配置错误。

正式 50/100 ms 的 manifest 完全相同。fixture 与正式的外层 manifest 仅 build.json、runtime-build.json、native loader、WASM、symbols 五项不同；runner、performance-hook、smoke、multiplayer-host、RTC impairment、relay 与 DATA hash 相同。两份冻结 runtime-build.json 的 **500 个共同 sourceFiles hash 全部一致**，fixture 额外九个 fixture 源文件；这确认共同源码身份，不把两种构建的诊断代码视为相同。证据：[正式 manifest](../../artifacts/network-cadence-100ms/launcher-candidate-cpu1-100ms-c6c/manifest.json)、[fixture manifest](../../artifacts/network-cadence-100ms/motion-candidate-cpu1-100ms-c6d-rtc15s/manifest.json)、[正式 runtime-build](../../artifacts/network-cadence-100ms/launcher-candidate-cpu1-50ms-c6c/frozen/runtime-build.json)、[fixture runtime-build](../../artifacts/network-cadence-100ms/motion-candidate-cpu1-100ms-c6c/frozen/runtime-build.json)。

两个 100 ms 样本的实际 job.args 仅 output、reuse_freeze、production 三项不同，均明确为 frames=1200、cpu_rate=1、one_way_ms=100、jitter_ms=0、spike_ms=0、相同双端触控与 host timing。冻结 runner 的 supervised_run 将本次 args 写入 job，worker 再从本次 job 构造 Namespace；因此复用正式 50 ms 的冻结文件不会继承旧的 50 ms 测量参数。证据：[正式100ms job](../../artifacts/network-cadence-100ms/launcher-candidate-cpu1-100ms-c6c/1-frontier.json.job)、[fixture100ms job](../../artifacts/network-cadence-100ms/motion-candidate-cpu1-100ms-c6d-rtc15s/1-frontier.json.job)、[实际冻结 runner](../../artifacts/network-cadence-100ms/launcher-candidate-cpu1-50ms-c6c/frozen/measure-smoothness.py)。

| body 开关 | 正式编译默认 | fixture 测量配置 |
| --- | --- | --- |
| skipResimVisual / skipResimGeometry | true / true | true / true |
| backMetadataOnly | true | true |
| BombLazy / ShotSparse / RecordSparse / TitleSpellsLazy | 全 true | 全 true |
| world instancing / texture coalesce | 开启 | 开启 |
| early input / live bullet journal / target filter / broadphase / barrier cache | 开启 | 开启 |
| background instance index / checkpoint span | 不启用 / 1 | false / 1 |
| exact-only / always-snapshot | 不启用 | mode0，均 false |

正式默认可核对 [RollbackDriver.hpp](../../th08_web/cpp/multiplayer/RollbackDriver.hpp)、[TextureJournal.hpp](../../th08_web/cpp/multiplayer/TextureJournal.hpp)、[WorldJournal.hpp](../../th08_web/cpp/multiplayer/WorldJournal.hpp)。[FixtureExports.cpp](../../portable/multiplayer/FixtureExports.cpp) 将 setter 应用于现有 driver，并给失败返回错误值；[冻结 hook](../../artifacts/network-cadence-100ms/launcher-candidate-cpu1-50ms-c6c/frozen/performance-hook.mjs) 会据此中止测量。fixture 两端 before/after 的 snapshotModes 实读均为 [1,1,1,1]，不是只读取报告顶层请求值。

**确定存在预热差异：**fixture 的 skipResimVisual、skipResimGeometry 与 backMetadataOnly 初始为 false，直到 180 帧 warmup 停止后的 active-phase-configure 才设为 true；正式从创建即为 true。前两项见 [RollbackDriver.cpp](../../th08_web/cpp/multiplayer/RollbackDriver.cpp) 的 fixture 默认与构造函数，后一项见 [TextureJournal.cpp](../../th08_web/cpp/multiplayer/TextureJournal.cpp) 的 fixture 默认与 Bind。body 之前仍有约 180–240 帧 lead-in；不能把这两种 warmup 路径描述为完全一致，也没有证据把该差异直接定为成本异常的原因。

三份 report 的 throttleApplications 共六次记录均为 rate=1、acknowledged=true、独立 page target，browserEvidence.errors=[]，CDP CPU sampling 均关闭。它证明请求值与成功应答，不是独立有效 CPU 速度标定。另一确定差异为正式默认 RTC 4500 ms、fixture 显式 15000 ms；冻结 relay 只在选路前使用该 timer，chooseRoute 选定后即清 timer，没有用它调整 body cadence。**正式 100 ms 的 41.179/41.260 Hz 与高 callback 成本仍是有效、待解释的观察，不因本次配置核查而注销，也不能归因为漏开优化。**

### 14.3 v7 正式构建与首个 CPU4 实测

[run-c7-multiplayer.json](../../artifacts/network-cadence-100ms/builds/run-c7-multiplayer.json) 记录本地正式构建已完成：08:14:53.925–08:15:25.251 UTC，status=0，源 HEAD **39421339fa0d660b32527704b9d8f8e1715ebad7**，profile=multiplayer、diagnostic=false、3,607,029 B。WASM 为 **47c49ab0da888d17ebbed866f942720338b501a61f7ecf0b3f101fa07ed7c1eb**，loader SHA-256 为 **d482728ab977b4eb86034e375f096faf10219e2ec5643931ed8fbe5199363d40**。

随后 08:15:59.829–08:16:55.047 UTC 的 [launcher-candidate-cpu4-spike-c8/selected-metrics.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c8/selected-metrics.json) 已完成，原始 report 与 [relay.log](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c8/relay.log) 确认相同正式 WASM、native mode0/D0、实际 RTC、默认 4500 ms RTC 启动期限、双端 CDP rate=4 且 acknowledged=true、实际 native GPU。其 [job](../../artifacts/network-cadence-100ms/launcher-candidate-cpu4-spike-c8/1-frontier.json.job) 为单程 50 ms、jitter 5 ms，seat1 从逻辑 600 触发持续 300 ms、额外延迟 120 ms 的尖峰，双端 120 Hz 墙钟触控；measured body 为 960 个新逻辑帧。

| v7 正式 CPU4 指标 | P0 | P1 |
| --- | --- | --- |
| 实际新逻辑帧率 | **51.349255 Hz** | **51.355573 Hz** |
| callback 总耗时 / 新逻辑 tick | 15.987500 ms | 15.793125 ms |
| callback p99 / max | 17.2 / 24.2 ms | 16.8 / 18.6 ms |
| Present 最大间隔 / >50 ms 次数 | 50.6 ms / 1 | 76.0 ms / 2 |
| advance 最大间隔 / >50 ms 次数 | 63.9 ms / 4 | 85.8 ms / 6 |
| callback / Present / advance >100 ms | 0 / 0 / 0 | 0 / 0 / 0 |
| corrections / resimulated | 677 / 2301 | 628 / 2126 |
| correction yields / frontier regressions | 313 / 0 | 300 / 0 |

本轮 passed=true、两端 canonical 完全相同、errors=[]、teardown.errors=[]；这些是测量流程及本轮正确性断言的通过，**CPU4 稳定 60 Hz 仍未达成**。有限样本没有 >100 ms 间隔，不构成永不大卡保证；也不能凭这一次相对 v6 的数值变化宣称稳定因果 FPS 增幅，或覆盖 v6 c7 的严重失速记录。v7 正式 CPU1 单程 100 ms 的 launcher-candidate-cpu1-100ms-c7 尚待主线程补实际结果；新版候选包也须以实际生成和身份核验记录为准。

### 14.4 v7 正式原速 100 ms 已完成，单端 CPU 成本不对称仍未解释

接续第 14.3 节当时的待测项，08:17:51.451–08:19:15.914 UTC 的 [launcher-candidate-cpu1-100ms-c7/selected-metrics.json](../../artifacts/network-cadence-100ms/launcher-candidate-cpu1-100ms-c7/selected-metrics.json) 已成功完成，status=0、passed=true。[原始 report](../../artifacts/network-cadence-100ms/launcher-candidate-cpu1-100ms-c7/1-frontier.json) 确认两端正式 WASM 都为 **47c49ab0da888d17ebbed866f942720338b501a61f7ecf0b3f101fa07ed7c1eb**、fixtureBuild=false、native mode0/D0。[relay.log](../../artifacts/network-cadence-100ms/launcher-candidate-cpu1-100ms-c7/relay.log) 明确记录 mode=rtc，RTC 启动期限为默认 4500 ms。

本次复用 c8 的冻结字节，但 [job](../../artifacts/network-cadence-100ms/launcher-candidate-cpu1-100ms-c7/1-frontier.json.job) 明确使用本次 cpu_rate=1、one_way_ms=100、jitter_ms=0、spike_ms=0；没有沿用 c8 的 CPU4 或尖峰参数。两端 CDP rate=1 且 acknowledged=true，实际 canvas 均为 Intel UHD / ANGLE D3D11、nativeGpuVerified=true，Chrome 155.0.8059.39；仍为独立 headed 进程、原生 rAF、双端 120 Hz 墙钟合成触控、960 个 measured 新逻辑帧，不使用手机。

| v7 正式 CPU1 / 单程 100 ms 指标 | P0 | P1 |
| --- | --- | --- |
| 实际新逻辑帧率 | **44.147673969 Hz** | **44.170830688 Hz** |
| callback 总耗时 / 新逻辑 tick | **17.2040625 ms** | **6.390625 ms** |
| callback p99 / max | 15.1 / 19.7 ms | 9.3 / 12.0 ms |
| Present p99 / max | 31.8 / 66.2 ms | 15.2 / 54.2 ms |
| advance p99 / max | 44.2 / 75.1 ms | 35.6 / 54.2 ms |
| Present / advance >50 ms 次数 | 3 / 5 | 1 / 1 |
| callback / Present / advance >100 ms | 0 / 0 / 0 | 0 / 0 / 0 |
| corrections / resimulated | 804 / 3615 | 729 / 4018 |
| correcting / correction yields | **513 / 513** | **12 / 12** |
| frontier regressions | 0 | 0 |

两端最终 13 项 canonical 数组逐项相同，errors=[]、browserEvidence.errors=[]；teardown.browsersClosed=2、teardown.errors=[]，两端浏览器均成功关闭。以上支持本轮 RTC、完成性与一致性断言通过。

**正式版单端 CPU/回调成本不对称仍然存在，原因未解释。** 两端接近的 44.15 Hz 推进率不能替代对 17.204/6.391 ms 成本与 513/12 次续算差异的解释；P0 本轮 resimulated 反而少于 P1，也不能只用“P0 重算帧数更多”解释其成本。callback/新逻辑 tick 是累计回调耗时除以新逻辑帧数，包含回调中的不同工作，不是纯 Update 时长或系统 CPU 利用率。现有记录没有证明根因是设备环境、JIT 或某个具体补丁。

本轮未观察到 >100 ms 的 Present/advance 间隔，但仍有上表所列 >50 ms 空档，且正式运行没有 fixture 的最终几何运动探针，不能把第 14.1 节的 hold 次数直接套用到这次正式样本。它不覆盖第 13 节正式 100 ms 的慢样本，也不抹去 v6 c7 严重失速；**仍不能承诺 CPU4 稳定 60 Hz、无卡顿或已达到用户认可 TH09 的整体体感。**

## 15. v7 本地候选包交付身份

[完整打包收据](../../artifacts/network-cadence-100ms/runtime-managed-touch-v7-c1-receipt.json) 已保存，passed=true，packagedAt=**2026-10-12T08:25:46.302Z**。本地可测试包目录为：

`D:/workspace/eagler/worktrees/th08-network-cadence-100ms/artifacts/network-cadence-100ms/runtime-managed-touch-v7-c1`

| 项目 | 已核验身份 |
| --- | --- |
| sourceHead | 39421339fa0d660b32527704b9d8f8e1715ebad7 |
| commonHead | 2826a853c5e30451b3f136fdb8aefdc4694fd6e2 |
| product / profile / variant | th08mp / multiplayer / multiplayer |
| diagnostic | false |
| nativeBuiltAt | 2026-10-12T08:14:53.970Z |
| native WASM SHA-256 | 47c49ab0da888d17ebbed866f942720338b501a61f7ecf0b3f101fa07ed7c1eb |
| native loader SHA-256 | d482728ab977b4eb86034e375f096faf10219e2ec5643931ed8fbe5199363d40 |
| managed 触控 shell SHA-256 | a112a6b993c348942bc773fc19c5fed3409667d88cea413f4d1c90cd53553c2e |
| directory index SHA-256 | f19790a97558388509328d8025b27a425022f99daf16c4f5fc77384fdda91021 |
| source manifest SHA-256 | 1f0abca5c82575302c1386b0e842c3569c0d4fc5e934db807e4967323b9c9e8c |

主线程打包核验通过 **18 个索引文件的大小与 SHA-256**，收据记录 verifiedIndexedFiles=18；另验证 **500 个 native 源文件 inventory**，verifiedNativeSourceFiles=500、sourceIdentityValidatedByPackager=true。retailDataBundled=false，包内没有零售 DATA，沿用 Runtime 与用户游戏数据分离的合同。此处 sourceHead 是包所核验的源码身份，不将之后的报告或诊断提交冒充为该次 native 构建来源。

该包包含第 11–12 节已核验的 managed 触控队列修复 shell，以及第 14 节 v7 的呈现完成门槛改动，正式 native 与第 14.3、14.4 节实测身份相同。**本次只生成并核验本地候选，没有部署或推送。** 打包与一致性通过不改变性能验收边界：正式 CPU4 仍约 51.35 Hz，原速 100 ms 正式样本仍约 44.15 Hz且有明显单端 CPU 成本不对称；第 14.1 节的特定运动改善、所有旧失败及未定位问题继续同时保留。

## 16. 当前验收口径与后续优先级

本报告中的 one_way_ms 是**单向应用发送延迟注入**，不是界面 RTT：对称 100 ms 单向注入约增加 200 ms 往返，50 ms 单向约增加 100 ms 往返，另有实际传输与调度成本。用户所说的“100 ms”若为 RTT，应先对照单向 50 ms 条件；不能直接把第 14.4 节的 44.15 Hz 套到该条件。v6 原速单向 50 ms 已有约 59.81 Hz 的正式样本（第 13.2 节），它不是 v7 在该条件下的新实测结果。

当前可交付的是第 15 节的正式本地候选，以及第 14.1 节已测得的特定高刷端点保持改善。10 ms 是最后一轮呈现开始门槛，不是 callback 的硬上限；8 ms 的恢复/新 tick 分片预算、完整恢复后的呈现屏障和端点后防倒退保护继续生效。不同刷新率、完整 Launcher 使用和更长场景中的最终体感仍需继续验证。

下一轮工作按以下顺序推进：

1. **解释正式版单端成本差异。** 使用同一输入轨迹、相同预热与实际模式，比对两端普通 forward、恢复、日志、语义 Draw、readonly Draw 的成本及浏览器采样；只有证据对应时才归因于环境、预热或 JIT。第 14.4 节 P0 重算次数较少却成本更高，单纯统计回滚深度不足以解释。
2. **继续压低导致端点提前呈现的实际工作量。** 以回滚日志、恢复完成后的绘制成本为依据选择下一处改动，保留正确性屏障；不能靠持续放宽 callback 门槛、删除防倒退保护或显示半恢复画面掩盖问题。仅省掉重复 Draw 不会自动消除已经发生的运动保持。
3. **按运动连续性验收。** 分开单向 50/100 ms 与尖峰条件，在桌面 60 Hz 和高刷新率路径检查新状态间隔、稳定对象保持/反向/跳步、callback 长尾以及真实触控输入到首个本地移动的延迟。CPU4 是桌面压力条件，不作为手机测试；一次正常暂停后的自然对端停止继续排除在异常之外。

Native 与 shell 分别已提交为 39421339fa0d660b32527704b9d8f8e1715ebad7、c7395abb0587197d995be9c7483c79b3253799a1；测量器、运动分析与启动证据已本地提交 c6467b86c0d64dc3da7df0183c4a55f808c5b9bd。报告提交不改变第 15 节的包身份。本次对完整 18 文件包、sourceFiles 身份以及 common 工作树已核验；common 为 2826a853c5e30451b3f136fdb8aefdc4694fd6e2，工作树干净。
