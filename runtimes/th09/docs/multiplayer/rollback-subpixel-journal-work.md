# TH09：在已验证的亚像素版本上叠加 PlayerShots 回滚日志优化

日期：2026-10-11。本文记录本地候选与实际验证，未发布到测试站。

## 结论与采用建议

用户实测舒服的版本是本轮唯一正式基线。六文件 PlayerShots 优化已在完整保留 managed 触控、0f 输入延迟、亚像素修复的独立源码树中构建和验证，并形成可单独审查的本地提交。

本轮四次双端 CPU4 正式版对照中，候选累计 native 回调墙钟耗时低约 **8.59%**，逻辑推进速率的四端均值高约 **2.43%**。这是有利的观测，但不是严格同输入轨迹下的因果百分比，也不是所有设备的收益承诺。长尾并未全面改善：候选超过 100ms 的新状态间隔为 0 次，基线为 1 次；超过 50ms 的次数反而为 41 对 35。

建议保留用户现有测试站版本作为默认，将此补丁作为独立、可回退的日志优化候选。当前降速、输入预测与显示节奏保持原实现。采用时审查性能提交即可，不需要把本轮所有研究脚本当成产品改动。

来源：[构建核验](rollback-subpixel-journal-build-c1.json)、[四轮正式版汇总](rollback-subpixel-journal-live-c1.json)、[收口数据](rollback-subpixel-journal-closeout-c1.json)。

## 1. 工作目录和精确身份

| 项目 | 值 |
| --- | --- |
| 用户原源码树 | D:/workspace/eagler/worktrees/th09-managed-touch-4x |
| 用户原 HEAD | 63127575d43ea5ef35b317ff8137b00a1ab5b1cc，加用户未提交修复 |
| 独立候选源码树 | D:/workspace/eagler/worktrees/th09-subpixel-journal-4x |
| 候选分支 | experiment/subpixel-journal-4x |
| 用户现有改动的本地基线快照 | a3a59db525d2e76faf9bb683bc624d455d3aa3ae |
| 单独的性能提交 | **cc35222ed5f7fe652821cfec8f44e055da0c3b54** |
| 共享库源码树 | D:/workspace/eagler/worktrees/th09-rollback-input-batch-common |
| 共享库提交 | 512353cab9eaa4f6744f867b0eb07c61ee7f2b0e |

基线正式构建：

D:/workspace/eagler/worktrees/th09-managed-touch-4x/th09_web/artifacts/subpixel-release-20261011-c1/build.json

WASM SHA-256：

cb03c6df0c460360d2d3548e63556a9b74bf18ac7f6613bdf8231a4e03c30480

候选正式构建：

D:/workspace/eagler/worktrees/th09-subpixel-journal-4x/th09_web/artifacts/subpixel-journal-release-c1/build.json

WASM SHA-256：

2ae38afdffaac02d94924af07e5d385535964c8b57154589ce6a93fac3b04ddd

两者均为 -O2、TH09_DEVELOPMENT_HARNESS=0、音频 SIMD 关闭。规范化源码树路径后编译选项完全一致。构建清单的 275 个源码条目中，只有下述六个预期文件发生改变。候选 WASM 为 2,828,795 字节，基线为 2,808,107 字节，增加 20,688 字节。

基线和候选的生成式 JavaScript loader 随 WASM 分别配套，两个 A 的 loader 一致，两个 B 的 loader 一致；不能把它描述为四次 loader 相同。测试驱动、共享联机 shell、触控计划及探针、中继源码、浏览器可执行文件等已启用控制项均相同。Chrome scheduler trace 在四次中均关闭，缺失该身份字段是预定配置，不是文件漂移。

原源码的 275 项构建身份以及以下三个文件在最终检查时仍与冻结基线吻合：

- th09_web/cpp/game/AnmRenderer.cpp
- th09_web/sdl-runtime/managed.mjs
- th09_web/sdl-runtime/shared-netplay.mjs

TH09_FIXTURE_ROOT 指向 th09-eagler 仅供测试资源读取；本轮没有从该主树编译游戏源码。

来源：[基线保存记录](rollback-subpixel-journal-baseline-c1.json)、[构建核验](rollback-subpixel-journal-build-c1.json)、[性能提交记录](rollback-subpixel-journal-commit-c1.json)。

## 2. 用户当前版本为什么更舒服

### 0f 是输入延迟，回滚能力仍然存在

共享联机入口将 rollback/hybrid 选择归一到 rollback 模式、输入延迟 0f，并关闭该模式的自动加输入延迟。它仍调用预测回滚会话，最大回滚历史深度为 8f；不是把回滚深度设为零。

### 降速由未收到的真实输入驱动

现有 RollbackSession 使用预测前沿与连续真实远端输入前沿之间的差距：

- 0–4f：目标间隔倍率为 1。
- 5f：目标间隔倍率约 1.333。
- 6f：目标间隔倍率约 1.667。
- 7f 及以上：目标间隔倍率为 2。

倍率上升和释放有平滑过程，原来的固定 1/60 逻辑步长不变。这是固定 4f 的容许区间，尚不是按稳定网络延迟自适应学习出的正常预测深度。

触发量取决于实际输入信息是否到齐。对端设备或浏览器没有及时产生/发送输入，也可能表现为信息缺口，因此不能把每次降速都单独归因为网络链路。当前补丁没有修改这套控制。

### 触控与亚像素修复直接影响手感

managed 输入不再排在无关资源异步任务之后等待，并保留生命周期、取消和所有权检查。AnmRenderer 在最后提交精灵四边形时保留小数坐标，同时保留原来的半像素偏移。后者可以解释小幅移动时量化顿挫减少的一部分，但不能仅靠源码差异把用户全部体感改善归因于它。

来源：[SharedNetplay](../../th09_web/sdl-runtime/shared-netplay.mjs)、[managed runtime](../../th09_web/sdl-runtime/managed.mjs)、[RollbackSession](../../th09_web/cpp/multiplayer/RollbackSession.cpp)、[AnmRenderer](../../th09_web/cpp/game/AnmRenderer.cpp)。

## 3. 这次具体优化了什么

性能提交只包含六个生产文件和三个相关测试文件；六个生产文件合计约 49 行新增、6 行删除。

旧路径在保存 PlayerShots 时，无条件复制双方两个 128 槽位的完整池。每个槽位为 1,144 字节，两个池合计 **292,864 字节，即 286 KiB**。即使大量槽位处于休眠状态，也支付整池复制成本。

新路径先保存活跃槽位的完整连续段。休眠槽位如果随后确实要被写入，则在第一次写入前保存原字节。每个槽位的 VM、轨迹、指针和填充字节仍完整保留。普通新建、Marisa 在初始化前的写入、Lyrica 不生成子弹的分支、回合重置和快照复用均有覆盖。

独立的 dense/hash 路径保留，不能通过减少参与验证的字段来获得“性能提升”。PlayerShots 的逐帧 update、hit、draw 活跃循环没有增加逐槽位 observer 检查。

六个生产文件是：

- cpp/game/Player.cpp
- cpp/game/PlayerShots.cpp
- cpp/game/PlayerShots.hpp
- cpp/game/PoolCheckpoint.hpp
- cpp/multiplayer/WorldState.cpp
- cpp/multiplayer/WorldState.hpp

以上路径均相对 th09_web。

可以承诺的是：该池的稀疏快照不再无条件整池复制，未活跃且本窗口内未写入的槽位无需复制。不能承诺每个场景都更快：新建子弹多一个可空 observer 判断，满池或碎片化活跃池仍有扫描、元数据及 first-write 成本。首次 Save 的字节下降也不等于生命周期总内存、后续首次写入或整个回滚过程都下降相同比例。

来源：[WorldState](../../th09_web/cpp/multiplayer/WorldState.cpp)、[PlayerShots](../../th09_web/cpp/game/PlayerShots.cpp)、[PlayerShot 精确还原测试](../../th09_web/tests/multiplayer/shot-capture.cpp)。

## 4. 验证结果与范围

| 验证 | 结果 |
| --- | --- |
| PlayerShot 精确字节、生成、绘制、重置、历史与复用 | 99 个用例通过 |
| 敌人 first-write 与拥有状态检查 | 118 个用例通过 |
| 子弹池还原、重演、重置、复用、溢出 | 600 个用例通过 |
| 亚像素精灵 | 40 个位置、锚点、镜像及重复绘制用例通过 |
| C++ 触控控制器 | 9 组、520 个断言通过 |
| managed 触控与共享联机入口 | 31 项 Node 测试通过 |
| 持续强制 8f 回滚 | 600 个逻辑帧通过；两端重演 4,748 / 4,747 帧 |
| 8f 参考世界比较 | 三个世界哈希均为 750422264；48 项审计状态、画面和 replay 文件字节一致 |
| 四轮正式版双端测试 | 4/4 运行、8/8 peer 的工作负载资格通过；每轮 10 个连续已确认哈希点匹配 |
| 运行中源码身份 | 10 份受保护调用记录均 sourceStable=true |
| 正式版运行收尾 | 四轮 teardownClean=true |

8f 检查运行三个顺序世界、在同一个 CPU4 Chromium renderer 内注入前沿延迟，用于状态与输入正确性。其诊断构建仍是 -O2，但 TH09_DEVELOPMENT_HARNESS=1，且开启了诊断配置。它不是双端正式版 rAF 流畅性测试。

该强制 8f 诊断场景仍记录到长调用：两端最大受控 native 调用约 **589.19 / 238.84ms**。尚未隔离其原因，不能因为它是诊断场景就直接认定全由仪器开销造成。因此本轮并不承诺任意持续极端 8f 纠正都能守住单帧预算。该不利结果完整保留，未混入正式版对照或筛除。

敌人组件日志也继续保留原有的边界：265 个拥有者发生重定位增长时，旧线性路径和已有位图路径都不能正确 Restore。这不是 PlayerShots 补丁新引入的变化，也不应被“118 个通过”掩盖。

来源：[组件日志](../../th09_web/artifacts/multiplayer-tests/subpixel-journal-components-c1-process.log)、[运行时测试日志](../../th09_web/artifacts/multiplayer-tests/subpixel-journal-runtime-c1-process.log)、[8f 原始报告](../../th09_web/artifacts/multiplayer-tests/subpixel-journal-deep8-c1-report.json)、[收口数据](rollback-subpixel-journal-closeout-c1.json)。

## 5. 正式版双端 CPU4 的 A/B/B/A

预先固定的条件：两个独立有窗口的桌面 Chrome 进程、原生 Intel GPU、双端 4× CPU、0f 输入延迟、每轮 1,200 逻辑帧、Page 时钟连续 120Hz 摇杆事件计划。中继为输入包人为增加 79±5ms 转发等待，无额外 burst 或丢包。79ms 不是实测 RTT。音频输出关闭；不启用 CPU profile 或 live-cost 重型剖析。

这是桌面通过 native ABI 手工构造的连续触控移动负载。按照用户要求，没有操作或测试手机。它也不等同完整 Launcher 输入桥或物理触摸到屏幕响应测量。

正文窗口沿用既有 driver：从首次前沿到达 180，到首次到达 1,200；结束 callback 不计入正文。结束边界和跨界间隔单独核验并保留，本轮没有因此藏掉更大的新状态极值。

以下每格为 P1 / P2。间隔使用 Page performance.now()，单位 ms。

| 轮次 | 逻辑推进 Hz | 新状态间隔 p99 | 新状态最长间隔 | >50ms 次数 |
| --- | ---: | ---: | ---: | ---: |
| A1 用户基线 | 45.01 / 44.99 | 51.98 / 44.02 | 103.57 / 68.66 | 15 / 3 |
| B1 候选 | 46.40 / 46.48 | 57.91 / 43.35 | 93.84 / 81.75 | 20 / 5 |
| B2 候选 | 46.77 / 46.84 | 51.78 / 41.94 | 71.10 / 51.56 | 15 / 1 |
| A2 用户基线 | 46.09 / 45.97 | 54.86 / 42.69 | 77.69 / 53.85 | 16 / 1 |

两轮、四端正文的描述性合计：

| 指标 | 用户基线 | 候选 |
| --- | ---: | ---: |
| 逻辑推进 Hz 的四端均值 | 45.515 | 46.623 |
| native 回调累计墙钟耗时 | 45.688 秒 | 41.765 秒 |
| 重演帧数 | 13,760 | 13,927 |
| 实际 WorldState Save 次数 | 13,962 | 14,194 |
| 新状态间隔样本数 | 4,075 | 4,074 |
| 新状态间隔 >50ms | 35 | 41 |
| 新状态间隔 >100ms | 1 | 0 |
| 新状态间隔 >250ms | 0 | 0 |

不能将 native 墙钟耗时称为 CPU 自耗时。各轮输入计划相同，但合并、实际捕获 tape、世界轨迹、包到达和重演次数不同，未构成严格同轨迹实验。上述约 8.59% 的成本下降和约 2.43% 的推进速率上升是观测值；不能直接外推到手机或其他网络条件。

实际信息缺口的中位数为 P1 4f、P2 5f。P2 的网络间隔倍率中位数约 1.12–1.20，说明降速在工作。低于 60 的逻辑推进率不能单独拿来证明 CPU 无力维持游戏。

触控正文的完整到期事件分母也保留：

| 事件口径 | 用户基线 | 候选 |
| --- | ---: | ---: |
| 到期事件 | 10,943 | 10,687 |
| 已提交 | 7,183 | 7,259 |
| 被更新的移动合并 | 3,760 | 3,428 |
| 正文内未提交且未合并 | 0 | 0 |

120Hz 是计划频率，不是每秒必然送入 120 个独立 native 输入。原始报告的条件响应延迟只统计能关联到 changed-wire 与后续新状态的子集；不能把该子集当全部触控事件，也不能当物理显示延迟。整轮末尾的未提交事件与正文分母分开保留。

来源：[预定协议](rollback-subpixel-journal-protocol-c1.json)、[正式版汇总](rollback-subpixel-journal-live-c1.json)、[收口数据](rollback-subpixel-journal-closeout-c1.json)。

## 6. rAF 是否还在正常游玩时等待数百毫秒

本轮可明确回答的范围是：**已经进入活跃正文、且被识别为 game callback 的 26,077 个 rAF 请求**。这些请求全部通过 active、暂停、异常、hook 和可见性检查，没有因上述条件剔除样本。正文进入时间范围内也没有登记后始终未进入的请求；结束后的 pending 请求另有 inventory，不能拿正常收尾等待当游玩卡死。

| 指标 | 用户基线 | 候选 |
| --- | ---: | ---: |
| 正文 game rAF 数 | 13,044 | 13,033 |
| 登记到进入的最大等待 | 30.375ms | 29.160ms |
| 扣除父回调剩余执行后的最大等待 | 30.365ms | 29.155ms |
| 单次 rAF 回调执行最大墙钟时间 | 22.430ms | 32.750ms |
| 登记等待 >50ms | 0 | 0 |
| 回调执行 >50ms | 0 | 0 |

所以，这组正式版测试没有复现此前数百毫秒的 rAF 调度停顿。它不是对整个浏览器、未进入的全部请求、启动阶段或所有运行条件的全局上限承诺。扣除父回调的等待也不能证明期间 CPU 或浏览器空闲。

两个最大的画面更新间隔有具体记录：

- A1 P1 的 103.57ms 跨越 sample 2168→2177 的 9 次后续回调。最初 F=756、A=748，缺口达到 8f，间隔倍率为 2；实际输入推进后进行多次纠正，最后呈现前沿从 756 到 757。期间没有一次 rAF 长时间阻断。
- B1 P1 的 93.84ms 跨越 sample 2725→2733 的 8 次后续回调。初始 F=930、A=924，随后输入推进与多次恢复/重演交织，直到前沿达到 931 才出现新状态。它仍然是玩家可感知的较长更新间隔，需要如实保留。

这里 F/A 取自 native networkPacing 记录，未把旧 stats 水位字段误当真实远端输入前沿。

若后续继续优化偶发顿挫，这类“信息不足后连续纠正，尚未超过旧显示前沿”的区间比单看平均 FPS 更值得针对性分析。本轮没有据此再改用户已满意的降速参数。

来源：[rAF 配对汇总 c2](rollback-subpixel-journal-raf-c2.json)、[离线分析实现](../../th09_web/tests/multiplayer/journal-raf-summary.mjs)。c1 分析文件保留历史，c2 修正了字段说明并增加未进入请求 inventory；等待数值和正文结果不变。

## 7. 本轮可以承诺什么，哪些仍未覆盖

可以承诺：

1. 性能提交相对用户修复快照仅包含六个生产文件、三个测试文件。
2. 用户的 managed 触控、0f 输入延迟、亚像素修复和现有降速参数被保留；构建输入和最终源码有哈希核验。
3. PlayerShots 稀疏快照取消无条件整池复制，状态仍完整保存；已覆盖的还原、输入和回放检查通过。
4. 所列四轮正式版、双端 CPU4、高延迟、连续桌面模拟触控场景，没有观察到 >50ms 的已进入正文 game rAF 登记等待或执行。
5. 所有不利间隔和诊断深回滚长调用保留在报告中。

不能承诺零掉帧、所有设备都有固定百分比收益、任意高密度世界/持续 8f 纠正都在 16.7ms 内，或所有 direct/中继/浏览器调度问题已修复。

本轮没有重新验收 direct 连接、真实远程独立中继负载、物理手机或完整 Launcher 桥。共享运输源码没有改动；该事实本身不能替代这些端到端验收。后续若需要继续，可优先处理可复现的连续纠正推进成本，再评估 TH06MP/TH07MP 中可复用的类型化日志/容器保留策略，避免将大规模日志重构和当前已舒服的节奏调整混成一次改动。

## 8. 交付与记录

性能提交：cc35222ed5f7fe652821cfec8f44e055da0c3b54。

本地可直接审查：

~~~powershell
git -C D:/workspace/eagler/worktrees/th09-subpixel-journal-4x show --stat cc35222
git -C D:/workspace/eagler/worktrees/th09-subpixel-journal-4x show cc35222
~~~

全部操作通过 nig 进行。构建与浏览器运行串行。等待 8f 检查时出现一次短暂 nig 404，连接恢复后读取原进程的完整成功记录，没有重复启动测试，也未将连接错误当作游戏失败。

本轮未推送、合并、部署或修改测试站。正式产物、原始报告和完整运行环境位于候选树 th09_web/artifacts 内；测试前后源码身份、完整参数、确认哈希点、输入分母、窗口边界和不利结果均保留在相应 invocation/report JSON 中。

测试完成后，为通过差异空白检查，只清除了 journal-run.mjs 文件末尾的一个空行。实测脚本原字节保存在 th09_web/artifacts/multiplayer-tests/source-snapshots/journal-run-measured.mjs，原始调用继续使用当时的哈希；未改写协议或实测身份来掩盖这项格式变化。历史 rAF c1 分析源字节也保存在同目录 journal-raf-summary-c1.mjs。生产文件未因此改变，无需重新运行游戏测试。
