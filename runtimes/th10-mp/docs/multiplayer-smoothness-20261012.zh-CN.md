# TH10MP 流畅度移植记录（2026-10-12）

当前简明交付边界见 [2026-10-13 状态摘要](th10-rollback-status-20261013.zh-CN.md)。以下保留完整实验历史；旧阶段计划不代表当前计划。

> **TH10 仍不可交付，TH11 尚未开始实施。** 默认关闭的持续有界 RAF 补偿已在冻结 r10p 的受控 300ms 延迟场证明局部收益：同 WASM OFF / ON 的最大画面提交间隔为 310.1 / 44.5ms，状态与音频通过。但正常双 4× 场仍有 124.2ms 停显和远端运动覆盖不足；没有将其算作修复。最新结果与边界见第 10.57–10.58 节。[E305] [E309] [E310] [E311]
>
> r10i 原 h / i 对照与 562 源冻结身份继续保留。matched RTC 中 i 的 P0 / P1 的 Accept 和 Committed 均为 334 / 388，证明追加 Draw 确实发生；但本地 / 远端 / 子弹像素窗口从 h 的 4 / 1 / 4 变为 i 的 7 / 0 / 4，两场均未通过运动覆盖，不能把补绘次数写成流畅度收益。原始结果见第 10.44 节。[E215] [E219] [E222] [E223]
>
> **真实 WS relay 场取得 7 / 6 / 2 个像素运动窗口、96 / 96 次采集完成，完整 44 字状态、frame1275 音频围栏和 route2 通过；但运行中仍有约 3.145 秒无新逻辑高水位。** 两端仅 48.837 / 48.830 Hz，停顿发生于 f1176→1177 / f1175→1176，早于目标停止帧 1259；每端该段上下文各保留 409 条连续 callback，不能当作正常收尾或三秒 RAF / native 阻塞。它仍是未解释的游戏推进失败。[E227] [E228] [E229] [E230]
>
> 后续同 r10i、无像素且开启 transport trace 的场次为 57.842 / 57.895 Hz，最大 advance 间隔 67.2 / 61.7 ms，trace 完整且 0 dropped；这场没有复现，不能为前场归因或豁免。[E231] [E232]
>
> **1800f 的 WS 像素加 trace 对照已回收，仍为 FAIL。** 双端完成 1859 与 exact1875 状态 / 音频围栏，但 P0 trace 和 metadata 容量耗尽，运动仅 1 / 1 / 0；同时出现 13.03 / 12.2715 秒前沿缺口及 P1 全循环 native 中位 38.8 ms。该存活页在 fence 后、bulk 前的独立固定算术数值正确，P1 耗时仍明显较大；这不是单纯容量问题，也尚未证明 Chrome 限速故障。旧慢端失败不能借此定因。[E233] [E234] [E235] [E236]
>
> **此前三场短启动诊断已全部完成（3/3），原始持续慢端仍未解释。** diagnostic-04 的 native 最大值为 17.3 / 17.5 ms，主线程 CPU / 墙钟比约为 0.208 / 0.204；正常新进程不能校准已经退出的失败进程。r10f spike-01 的约 12.21 Hz / 231.2 ms、live-motion-02 的约 9.87 Hz / 248.9 ms，以及旧 r10b 约 700 ms 长回调全部保留；r10g / h / i 的原始运动 FAIL 也不覆盖。[E189] [E105] [E161] [E176] [E177]
>
> 未进行手机测试。实际 relay 仅覆盖初始本地 WS fallback 与 Input 消息 FIFO 延迟，不是 WAN、TCP 丢包或 RTC 动态迁移验收；完整 managed 链路仍未覆盖。r10g Restart 的既有 D1 / stopped-tick 证据不扩大到 RAF、4× 或 BGM，TH11 仍只读准备。[E199] [E200] [E226]
>
> 当前已完成 r10p 持续有界 RAF 补偿的受控故障对照，收益仅限上述场景。后续核心是正常历史恢复停显；Item 单项及既有无整体收益的开关实验不再重复。旧持续慢端、长回调与网络输入空窗各自保留。像素门槛与完整状态门禁不变，尚无可承诺的整作收益。

## r8 阶段原状态（历史保留）

> **r8 的精确 camera math cache 和 packed occupancy 扫描已完成本轮定向验证，但同一正式 WASM 的紧邻 4× OFF / ON 没有显示明确整作收益。VBO 驻留、camera cache、packed 扫描均继续默认关闭。**
> 2026-10-12 15:04:57–15:05:53 UTC 的串行紧邻对照，OFF 逻辑为 **55.281 / 55.235 Hz**、native work / 新帧为 **15.104 / 12.635 ms**；ON 为 **55.065 / 55.094 Hz**、**14.916 / 12.618 ms**。两组双端 `desktopCadenceTargetMet=false`，仍不能承诺 4× 不掉帧或无大卡顿。[E87] [E88] [E89] [E90] [E91]
> 冻结世界 camera oracle 的 30 次真实 Draw 全部像素零差、44 字 canonical 不变、原生时钟不前进；随后 1× 的 P0 单端开启 camera cache / packed 扫描且逐次核验命中，P1 关闭，初始/最终两端完整 44 字 canonical 与确认音频均一致。capture 的 wasm32 定向测试还覆盖 800 次随机撤销、12 帧 undo、填充、槽复用和失败重试。[E84] [E85] [E86]
> ON 的 camera 命中率约为 look-at **89%**、perspective **99.98%**。这是减少重复计算的工作证据，不能据此推导整作帧率提升；4× ON 未启用逐命中重算校验，其 correctness 依据须连同 1× oracle / 非对称核验一起阅读。[E85] [E87] [E90]
> 三场 runner 的 PASS 覆盖运行、实际 RTC 路由、完整状态、确认音频、身份与自有进程清理；不代表 cadence、最终像素运动连续性或输入到光子验收。当前 RTC 仍为本地 loopback，触控为桌面合成输入，未做手机测试。smoke 仍关闭 BGM，音频边界见 [r7 音频热点审计](th10-audio-hotpath-audit-r7.zh-CN.md)。
> r8 的 unit 回执与 4× profile 已归档。camera 原生单测为 4804 次比较 / 24 种数值模式；全 P0 profile 中仍观测到日志复制 helper 537.272 ms、BeginFrame inclusive 807.251 ms。它们用于定位下一步，不能把采样耗时全部解释为跨 WASM / JS 调用开销，也不构成已获得的日志优化收益。真实原生菜单 Restart 的完整 canonical 历史失败仍未关闭。[E92] [E93] [E94] [E95] [E96]

第 10.25–10.28 节是本轮 r8 的身份、验证与决策；第 10.19–10.24 节保留 r7 时期的事实和当时计划。以下旧状态按阶段保留，不能把后续实验结果倒填为旧实验的收益。

### r7 阶段原状态（历史保留）

本段保留 r7 完成时的判断，其中“r8 尚待运行”是当时状态；后来收回的 r8 证据见第 10.25–10.28 节。

> **r7 减少了符合条件的 Model quad 顶点上传，但紧邻 OFF 重复组反而稍快，尚无整作帧率收益证据，VBO 驻留保持默认关闭。**
> 2026-10-12 14:23 / 14:25 UTC 的同 WASM、桌面双端 4×、50 ± 5 ms 单程延迟、120 Hz 合成触控样本，逻辑约 50–52 Hz，仍未通过 60 Hz cadence 门槛。两组完整 44 字 canonical、确认音频、真实 RTC 和自有进程清理均通过；checker PASS 不能读作流畅度验收。[E75] [E76] [E77] [E78]
> 较早 13:51 UTC 的 r7 OFF 只有约 5.7 Hz，不能与半小时后的 ON 相除得出巨大性能收益，也不将该异常归因于用户此前断网。实际路由为本地 loopback RTC，不是公网/WAN 证明。[E73] [E74]
> r7 的 25 次冻结世界像素 oracle 已通过；最新 profile 将下一步指向精确 camera math 复用和回滚占用扫描等实测热点。r8 的精确 camera cache 与 packed occupancy 是**正在进行、默认关闭、尚待运行验证**的试验，不计为已完成成绩。详情见第 10.19–10.24 节。
> 原生 Restart 完整 canonical 的历史失败仍未关闭；TH10 最终像素运动连续性、输入到光子延迟及用户体验尚未验收。音频边界另见 [r7 音频热点审计](th10-audio-hotpath-audit-r7.zh-CN.md)。

以下第 1–10.18 节保留此前 r6 及更早阶段的机制、运行记录和失败说明，不将后来观察倒填成旧实验的结论。新增 r7 的身份和结果单独列明。

### r6 阶段原状态（历史保留）

> 状态：正式 r6 的真实 rAF、900 帧样本在 1× / 4× 都完成 RTC、完整 canonical、确认音频及身份检查；但 4× 逻辑仅 7.53 / 7.60 Hz，频繁出现超过 100 ms 的停顿，性能严重不合格。
> 真实原生菜单 Restart 的完整 canonical 仍失败；仅有有限生命周期观察完成，当前 TH10 候选不可推广。
> 原版异常、r2 / r3 未达标、r4 失步、r5 正常 4× 超时及各轮 Restart 失败均保留。r5 定向 fresh-launch 修复不被扩大；TH10 尚无人体验收，最终像素运动连续性未测。
> 本文记录当前机制、验证边界和待办，不承诺改进百分比。

## 1. 范围与版本身份

| 项目 | 当前记录 |
| --- | --- |
| 工作树 | `D:/workspace/eagler/worktrees/th10-rollback-smoothness` |
| r8 冻结源码身份 | HEAD `2e5d5464b2b28ef3fb936234660b773b604c91ce` 加 r8 增量；正式清单 553 个源码逐文件核对，common 为 `77666259b4bb14b965a34a1f38a4e2ec88c289ac` [E82] |
| r8 正式 WASM SHA-256 | `30fc5b7eb3c3fb6e9b3c1fd9e5c048f11eeaecb712d5e2971d11fc631727daeb`；`multiplayer`、`diagnostic=false`；VBO / camera / packed 试验默认关闭 [E82] |
| r8 sourceDigest / 运行时 | `46bbe7e49551ad5f3607524259d1b5f0a5069146193d98258929f49241327089`；`artifacts/rollback-smoothness/candidate-r8/runtime` [E82] |
| r7 冻结源码身份 | HEAD `2e5d5464b2b28ef3fb936234660b773b604c91ce` 加 r7 增量；冻结清单逐文件核对 552 个源码，后续 r8 工作区修改不属于该 WASM [E69] |
| r7 正式 WASM SHA-256 | `82232288a0c4f3045c885053c4a34da5faecba87b930e1fccc5392f1016af12f`；`multiplayer`、`diagnostic=false`；VBO 与分段计时默认关闭 [E69] |
| r7 sourceDigest / 运行时 | `802a500599ae44adf85b080a738dd94c8d0c37e4dec9b1b4cc3b7ba79d215c66`；`artifacts/rollback-smoothness/candidate-r7/runtime` [E69] |
| 源码基线 | `aab3f5f`；r6 构建来源为此基线加 title 候选修改，不能将基线提交当作候选全部代码；冻结字节与源码身份见 [E44] / [E48]，后续本地提交回执见 `artifacts/rollback-smoothness/closeout-commits.json` |
| common 基线 | `894474` |
| 前一 common 提交 | `d064941d9902e5ed7c1946e6d4599713bfcab0ad`；此前 4 文件改动 |
| r6 common 本地提交 | `77666259b4bb14b965a34a1f38a4e2ec88c289ac`；在前一提交上修改 AdonisConnection 与既有启动测试两文件，提交回执中工作树 clean；仅本地提交，未 push / merge [E43] |
| 冻结旧运行时 | `D:/workspace/eagler/worktrees/th10-rollback-smoothness/artifacts/rollback-smoothness/baseline-oct10/runtime` |
| 旧构建性质 | `profile=multiplayer`、`variant=multiplayer`、`diagnostic=false` |
| 旧 WASM SHA-256 | `2c5b5a16bb786ccce36a08f34d90e6015abe81d1e78ffdffa9617b5d8ff96d83` |
| 最新正式 r6 WASM SHA-256 | `341d63456fc3b55c4f06451264523171038286e3e48664c3e4665ae685b22caa`；正式 `-O2`、multiplayer、`diagnostic=false`；普通 rAF 一致性完成，4× 性能严重不合格，Restart 完整 canonical 仍失败 [E44] [E48] [E59] [E61] [E63] |
| r6 sourceDigest | `06dfbc9f75d9c2aba3ff15dbd93e55c50dd3c5b9141fc3303186667e32b5797c` [E44] [E48] |
| r6 冻结运行时 | `D:/workspace/eagler/worktrees/th10-rollback-smoothness/artifacts/rollback-smoothness/candidate-r6/runtime`；含 r5 启动门控 / 缓存、恢复尾帧准入及已知退休包校准边界 [E44] |
| 正式 r5 WASM SHA-256 | `e0e6def1341b8d76152554afe597dc2bd3dd4e6a00abc02d0cab9b6a12a9e30b`；正式 `-O2`、缓存开启，启动偏斜定向验证通过，正常 4× 触控 deadline 失败 [E34] [E36] [E38] |
| r5 冻结运行时 | `D:/workspace/eagler/worktrees/th10-rollback-smoothness/artifacts/rollback-smoothness/candidate-r5/runtime`；相对缓存开启 r4 的原生改动为启动门控，尚未加入尾帧预算试验 [E35] |
| 正式 r4（缓存开启）WASM SHA-256 | `4f38b713948b83d3ee1b2c93cfa2fb783dd8f281d93f77753106fa5a65ba66bb` |
| 正式 r4 构建性质 | multiplayer，正式 `-O2`，`diagnostic=false`；首次 1× RTC 失败 [E22]、同字节复测通过 [E27]，后续启动偏斜又复现初始失步 [E29] |
| r4 同字节复测运行时 | `D:/workspace/eagler/worktrees/th10-rollback-smoothness/artifacts/rollback-smoothness/candidate-r4/runtime`；收据确认冻结字节身份 [E28] |
| 正式 r4 缓存关闭对照 | `325b9775b29bb6bf728d28f7d551d2c4737215f71f8184d0b367581644199a48`；multiplayer、`diagnostic=false`，先有单次通过 [E25]，后续启动偏斜同样失败 [E31] |
| 已替代的正式 r3 | `5d1657a2995a74cf7c3e6d9803b08f89ff8945fbfd5e31227504863e2b55c1f8`，3,115,024 bytes；既有 r3 报告仍对应此字节身份 [E8] |
| 已替代的正式 r2 | `9db7600c3d2a8a3349b31cfa1938b7be4c2e76c764912b6cff675cfd026f0c37`，3,114,936 bytes |
| 已替代的首个正式候选 | `fc9ba4a5dcb60f952e4f8eaa0f7c12504f9c8e0e19b67753ad97f184d81cdc52`，3,115,100 bytes |
| 候选性能报告 | r2 的 1× / 4× 见 [E5] / [E7]，r3 的 4× 见 [E8]；r4 三次记录见 [E22] / [E25] / [E27]，不作稳定性能 A/B；r5 正常 4× 超时及短 profile 样本门槛失败见 [E38] / [E45]；r6 普通 rAF 的 1× / 4× 见 [E61] / [E63]。运行 / 一致性通过不代表流畅度通过 |

提交前已逐一核对 r6 构建清单的 552 个源码文件；随后仅清理 `BackgroundProjectionCache.hpp` 及其定向测试各一个文件末尾空行。
冻结 WASM / build.json 保持原字节，原源码副本、前后 SHA 和仅删除最后 1 个 LF 的记录见 `artifacts/rollback-smoothness/closeout-source-normalization.json`；
提交后的源码 SHA 因此不能直接冒充冻结清单的逐文件 SHA，本文全部运行结果仍对应表中的原 r6 字节。

跨作主文档：`D:/workspace/eagler/worktrees/multiplayer-rollback-architecture/docs/playbooks/multiplayer-rollback-architecture.zh-CN.md`。
共享 Launcher checkout 切回 main 后，主执行已从归档恢复该文档及后续补充到独立文档树；`611c97f` 仅是历史初稿提交，不能代表当前全文的最终提交。
本文只记 TH10 的适配与证据，不复制整份架构。

## 2. 目标与迁移原则

首要目标是降低本地输入等待，同时让画面中的运动连续。
网络信息暂时不足时允许平滑降速；应重点消除正常游玩中的长任务和突然整段停顿。
正常暂停后，一端不再生成输入导致另一端到达预测边界而等待，是可接受的自然行为。

TH08 / TH09 已有用户实际游玩的高度正面反馈；这为设计提供参考。
它们的成功不能自动成为 TH10 的体验证明。
下文描述当前候选的组合，包含沿用能力，不把每项机制都算成本轮首次实现。

移植保留原有固定逻辑步长、输入事实、历史事务、状态一致性和输出提交规则。
调度可以决定何时开始下一笔工作，不能跳过逻辑帧或改写输入来制造“追上了”的计数。

## 3. 必须区分的前沿与所有者

| 名称 | 含义与约束 |
| --- | --- |
| `NextFrame()` | 下一待模拟帧；恢复时会向后移动 |
| `CorrectionEnd()` | 开始恢复前保留的排他终点；属于不可回滚的 host 状态 |
| `PredictionFrontier()` | 恢复期间取 next 与原终点中的较大者，避免回退制造虚假的预测余量 |
| `ConfirmedThroughAllRemotes()` | 所有远端输入的连续确认前沿；尚不等于音频、Replay 等输出已提交 |
| 呈现相位 | 当前已完成世界状态之间的只读插值位置，不承担模拟进度 |
| 测量 high-water | 单次 session 内实际观测到的最大 native 前沿，不写回 native |

新的迟到输入可以再次触发恢复；同一仍有效的恢复区间内，保留终点只能维持或扩大，不能随恢复游标一起缩短。
经修正的生命周期边界可以废弃旧未来，但仍须经过转换围栏，不能直接销毁资源。
确认到达后仍必须先完成 reconciliation，再越过相应输出或生命周期围栏。[S1] [S3] [S4]

## 4. 输入先行与可分段恢复

### 4.1 Early capture

native rAF 先泵网络，再处理资源就绪、输入和到期逻辑工作。
可捕获且不处于屏幕转换时，`multiplayer_update()` 先检查本帧是否已有本地输入。
只有尚未捕获的帧才采样、构造 `FrameInput`、存入共同核心并发送，然后开始较昂贵的恢复。
目的在于减少“本端先忙于回滚，另一端只能继续预测”的附加等待。[S1] [S2]

Replay、观战、退休会话和仍处于历史恢复中间的路径，不生成新的玩家输入。
frame 0 仍要求双方实际完成首个输入捕获；HELLO / READY 不能替代世界加载完成。[S3] [S4]

### 4.2 历史事务间 yield

恢复采用原有 native Update 与 authored semantic Draw。
每个历史帧必须完整完成输入提交、Update、Draw、日志及音频命令事务收尾、`MarkSimulated`，
才可在下一历史帧开始前考虑让出浏览器线程。[S1]

冻结 r1–r5 的 live rAF 历史循环在至少完成一笔事务后检查：

- 本回调已重演的历史帧达到共同 `MaxCatchupTicks=6`；
- 或从该 native 回调开始已用时达到 8 ms。

满足任一条件就保留 correction 状态、记录 recovery yield，等待后续回调继续。
**8 ms 是继续开始历史事务的检查点，不是整回调耗时上限。**
一次 Restore、单帧 Update / semantic Draw、收尾或已到期的 forward 工作都可能使回调超过 8 ms。

正式冻结 r6 已将历史准入接到 `RecoveryBudget`：普通追加历史事务仍须在 8 ms 前开始，
仅剩最后一笔历史事务时可在 12 ms 前开始；最多 6 笔、第一笔保证推进及完整事务边界保留。
定向原生检查通过；真实 rAF 的 4× 性能仍严重不合格，尚不能证明尾部预算带来净收益。
Stopped-loop Restart 检查不能验证此预算的性能，详细条件及 rAF 结果见第 10.11 / 10.17 节。[S25] [E41] [E63]

恢复全部完成后，当前回调仍可尝试一个已经到期的 forward 帧。
否则在持续迟到输入下，可能每次只修旧世界、永远无法产生新的前沿。
同步 stopped-loop 正确性入口继续使用同一恢复实现，但不受 live rAF 的分段合同限制。[S1] [S7]

## 5. r3：CPU 欠债、网络 pending 与归一相位

`NetworkFrameSchedule` 将欠下的时间保存在名义 60 Hz 的逻辑单位中。
只对新进入的墙钟时间除以当前 interval scale；改变网络速度时不重新缩放已积累的小数相位。[S2] [S5]

| 情况 | 调度行为 |
| --- | --- |
| 正常 CPU 追赶 | 保留有界欠债，最多提供 6 个 tick，附加 tick 受开始预算限制 |
| CPU correction 未完、尚无新 forward | `recovering(offered-completed)` 返还本回调已提供但未完成的全部 tick，后续回调继续计入新墙钟时间 |
| 真正的网络输入等待 | 保存一个已经到期的 pending forward；等待期间重试这一个义务，不累计等待时间 |
| 网络 pending 重试转入 CPU correction | 清除网络等待标记，将本次提供的一个义务转回 CPU 欠债；此前已丢弃的网络等待时间不补记 |
| 已提供但因 CPU 开始门槛未执行的 tick | 返还 `offered-completed`；同一次退出只能处理一次，不能在网络 pending 上再叠加退款 |
| session / generation 改变 | 重置调度身份、相位及呈现 priming |
| pending 或仍有整帧欠债 | alpha 保持当前端点，不能用小数余量重播旧运动 |

`offered` 是本回调取得的逻辑义务，`completed` 只按实际新 forward 计数；历史重演不冒充新进度。
CPU 退款与随后进入的墙钟时间仍受 6 tick 欠债上限限制。宿主以 `Correcting()` 区分恢复让出与输入阻塞，
已处理的退出路径不会在尾部再退款一次。[S2] [S5]

### 保留 r2 错误历史

r2 曾将“没有完成新 forward”统一交给网络 `blocked()`，其中也包括 CPU correction 的 yield。
下一回调的 pending 重试直接返回一个 tick，忽略该次新墙钟时间；CPU 恢复因此被错误地变成了丢弃时间的网络等待。
r3 对仍在 `Correcting()` 的路径改用上述 CPU 欠债规则，同时保留真正输入等待的单帧 pending 语义。[S2] [S5] [S18]

r3 的定向 native pacing 检查已由主执行运行通过：覆盖 90 / 120 / 144 / 165 / 240 Hz 多次恢复中的时间守恒、
network pending 转 CPU、一次提供多个 tick 后的未完成退款和欠债封顶。[S17]
这些检查证明相应调度合同；第 10 节的 r3 实测仍未达标，不能据此称恢复速度或整体流畅度已经改善。

## 6. 按缺失网络信息逐渐制动

网络泵先接收输入，再以连续确认前沿和保留的 prediction frontier 计算缺口。
缺口单位是逻辑帧，输入为网络信息状态，不使用本端 callback 耗时来加深该制动。[S3] [S4]

当前共用 `InputGapPacing` 的目标为：

`targetScale = 1 + clamp((missingInputs - 4) / 3, 0, 1)`

| 连续缺口 | 目标 interval scale |
| --- | ---: |
| 0–4 f | 1，缺口制动免罚 |
| 5 f | 4/3 |
| 6 f | 5/3 |
| 7 f 及以上 | 2，达到封顶 |

scale 增加速率为每 50 ms 增加 1；降低速率为每 150 ms 降低 1。
因此完整的 1→2 需要 50 ms，2→1 需要 150 ms；较小变化按同一线性速率完成。
这不是“每 50 ms 跳一次档”，也不是新增 4 f 固定输入延迟。[S6]

相同时间戳的重复 Pump 不多赚时间；时钟倒退也不把锚点后移。
会话非适用状态会复位。既有 peer phase 对齐仍独立存在，因此“4 f 免罚”特指缺口制动。
最终 interval scale 组合二者，不通过提高输入 delay 消除回滚。[S3] [S6]

## 7. 高刷呈现与 10 ms 开始门槛

TH10 的 authored Draw 属于世界事务；历史重演不能删掉其状态效果。
历史 Draw 可在适用条件下抑制不需要的 sprite GPU 输出；作者请求的像素捕获仍保留完整路径。[S1]

只读呈现必须等待 correction 完成，不能把恢复到一半的世界显示成最新状态。
当前 live rollback 分支的额外呈现策略是：[S2]

| 当前状态 | 呈现动作 |
| --- | --- |
| correction 尚未结束 | 不开始只读世界重绘 |
| 本回调已完成 forward，非高刷或已用时 ≥10 ms | 提交已产生的 semantic 像素 |
| 高刷且满足额外绘制条件 | 用归一后的相位执行只读 Draw |
| 只有恢复完成的待呈现状态，预算已到 | 保留 pending，后续回调再完成只读呈现 |

**10 ms 是这一分支选择是否开始额外 Draw 的门槛，不是 GPU 或完整 rAF 的耗时上限。**
常规高刷呈现仍按自身条件运行；不能把表中的门槛宣称为所有绘制的统一硬截止。
门槛也不能简单删除：额外重绘本身可能挤占回调预算。

当完整 semantic 端点已直接提交后，清除 interpolation priming。
否则下一次高刷回调可能又沿同一对状态向后插值，造成 Present 很勤、物体运动却重复或不顺。
新 forward 完成后再建立有效的呈现状态对。[S2] [S5]

### r2：停止边界与实际呈现状态

附加 tick 的 `browser_prepare_frame()` 可能停止或替换 loop，并释放原 Application。
r2 在首次及附加 prepare 后检查 running、Application 和 loop epoch；身份失效时安全返回，
不再执行原回调尾部、解引用已经退休的 app。附加路径同时结束当前 defer 状态。[S2]

测量达到目标后的终止移动到 `runtimeFinish`，在真实 callback 的最后呈现之后执行；
追加 prepare 可以拒绝另一个 tick，不能在 C++ 尚未走完本回调时提前停止底层对象。[S13]
新增只读 `multiplayer_presentation_status`，暴露实际 high-refresh、interpolate、alpha、presented 和 primed。
请求 unlocked 只说明配置意图；后续高刷验证必须结合这些实际状态，仍不等同最终物体运动证据。[S2] [S13]

## 8. 尾包、触控与输出围栏

### 8.1 共用尾包合并和 retired ACK

收到输入后，若同一回调将紧接一次合法的新 capture，可推迟普通旧尾包，将它并入新尾包发送。
冗余窗口将满、可靠 repair 到期、背压等路径保持保护；非连续 capture 前必须先 flush，不能覆盖旧环形槽。
未实际 capture、提前退出和回调结束也必须 flush 已欠的尾包。[S1] [S4] [S8]

正常快通道背压不能同时关闭可靠修复通道。
屏幕转换时主动发送终端 input / ACK；没有新输入可生成，也不能把最后一个丢包留给“下一帧顺便修复”。
旧世代 terminal ACK 在下一世代加载和校准期间继续维护，直到各 peer 的新 HELLO 证明其已退休。
这一维护状态不会允许旧会话数据推进新核心。[S4] [S8]

r6 另修复下一轮 Adonis 校准入口：只有与显式已退休描述符匹配的合法旧报文才可忽略，
新会话仍须完成自己的校准，旧输入不透传至新核心。原生同字节 ACK 对照见第 10.12 节；后续真实 RTC 原生菜单 Restart 的新局 canonical 失败见第 10.14 节。[S26] [E42]

### 8.2 触控直达与一次采样

`sdl_touch` 直接进入 native TouchController，避免再把同一事件追加到等待游戏 tick 消费的 SDL 合成队列。
宿主手势状态可以更新，但已捕获帧和历史恢复重试不再次消耗 Bomb / Esc 脉冲，也不覆盖已锁存输入。
真正的新 forward 捕获才将当前按键与模拟量变成该帧的输入事实。[S1] [S9]

managed shell 的文档所有权、epoch / 生命周期、取消和 action serial 保护仍须保留。
直达不会让被主线程阻塞的 DOM 事件提前执行；因此不能据此宣称输入链延迟已全部消除。[S10]

### 8.3 提交围栏

| 输出 / 生命周期 | 可越过围栏的条件 |
| --- | --- |
| 确认音频 | 无待处理 rollback、无 correction，且满足确认与已模拟前沿 |
| Replay 提交 / 保存 | 先完成修正，再提交已确认档案与 checkpoint |
| 观战发布 | 不发布 correction 或待 rollback 中的预测中间态 |
| 资源销毁 / 世代退休 | 修正完成、终端输入确认，并满足双向 ACK 围栏 |

收到真实输入只说明输入前沿前进，不能立即清掉仍需 Restore 的 journal 或销毁其资源。
Update + semantic Draw 事务完整性和确认输出的单次提交，是优化必须保留的约束。[S1] [S4] [S11] [S12]

## 9. 已运行验证与保留的失败

下表来自本轮主执行的实际运行回报；这里只登记已确认结果，不外推体验。
各项对应测试时的源码；后续正式候选构建通过，不等于重新运行了全部前序检查。

| 项目 | 状态 | 可以支持的结论 |
| --- | --- | --- |
| Shell tests | 16/16 通过 | 对应 shell 合同与辅助逻辑 |
| Neighbor tests | 14/14 通过 | 对应相邻回归检查 |
| Summary tests | 8/8 通过 | 连续时间窗、advance 停顿、触控账目等统计约束 |
| WASI 全套 | 33 suites 通过 | 对应 native / common 正确性范围 |
| r3 定向 native pacing | 通过 | CPU / network pending 区分、多次恢复时间守恒及多 tick 退款；范围见第 5 节 |
| r4 投影缓存定向原生检查 | 通过 [E17] | 1,847 次输出 / 数值副作用等价对照，不构成完整游戏一致性验收 |
| r4 正式 multiplayer 构建 | 通过 [E18]，身份见第 1 节 | 编译及正式包生成；后续 Web 一致性实测失败 |
| 纯 CPU 校准 | 完成，报告 `passed=true` [E21] | 两个独立浏览器的算术耗时与限速差异，不是游戏性能 |
| r4 首次 1× RTC | 失败 [E22] [E23] | 最终 canonical 不一致，候选不可采用 |
| r4 缓存关闭对照 1× RTC | 运行 / 一致性通过 [E25] [E26] | 初始及尾端完整 canonical / audio 相同，yield 0 / 0，未覆盖跨回调恢复 |
| r4 同字节初始观察复测 | 运行 / 一致性通过 [E27] [E28] | 窗口 yield 49 / 40，覆盖跨回调恢复；不能消除后续定向复现的启动缺陷 |
| 启动偏斜 skew8 对照 | r4 缓存开启 / 关闭均失败 [E29] [E31] | confirmed 59 的初始观察已失步，无须缓存命中即可出现 |
| r5 启动门控验证 | 正式构建、skew8 初始及最终一致性通过 [E34] [E35] [E36] | 仅支持第 10.9 节的 fresh-launch 启动修复，不代替正常负载验收 |
| r5 正常 4×、900 帧触控 | deadline 失败，双方 done=false [E38]–[E40] | 只保留部分数据；没有完成测量窗口或最终 canonical / 音频验收 |
| r5 短 4× profile、240 帧 | 样本数门槛失败，108 个 measured callbacks [E45]–[E47] | 最终 canonical / 音频一致，但总体失败；只能用于 P0 CPU 归因 |
| r6 恢复预算原生检查 | 既有 netplay-pacing suite 加准入边界用例通过 [E41] | 8 / 12 ms 准入、第一事务和最多 6 笔的组件合同，不证明 Web 流畅 |
| r6 Adonis 退休包检查 | 同一 44 字节旧 ACK 的旧 / 新行为对照及既有启动整套通过 [E42] | 对应原生合同；不覆盖后续真实 Restart 的新局 canonical 失败 |
| r6 正式构建与冻结 | 通过 [E44] [E48]，身份见第 1 节 | 编译、正式包与冻结身份；后续 Restart 完整 canonical 失败，4× 性能不合格 |
| Manual Restart 检查器 | 6 项语法 / AST 检查通过 [E49] | 只证明检查器语法；不证明游戏正确 |
| 首次 Restart runner | relay 依赖缺失，启动前失败 [E50] | 未开浏览器、未运行游戏，无 checker JSON |
| r6 实际 RTC 原生 Restart | 新局完整 canonical 失败 [E52] [E53] | generation 1 与新校准已完成，仍不可采用；stopped-loop 检查不是 rAF 性能验收 |
| r6 迟到 ACK 诊断首轮 | 初始 Last 59 完整 canonical 失败 [E55]–[E57] | 未进菜单、未捕获或重发 ACK；不能记为退休交错覆盖 |
| r6 迟到 ACK 继续观察 | 生命周期观察完成，最终仍 FAIL [E58]–[E60] | 真实 RTC 旧 ACK 收到且新校准完成；完整 canonical 未通过，portable / audio 一致不能替代 |
| r6 普通 rAF 1×、900 帧 | 运行 / 路由 / 一致性 / 身份通过 [E61] [E62] | 约 59.84 / 59.90 Hz；P0 cadence=false，零跨回调 yield，最终运动连续性未测 |
| r6 普通 rAF 4×、900 帧 | 运行 / 一致性通过，但性能严重不合格 [E63] [E64] | 约 7.53 / 7.60 Hz，长停顿频繁；不可用 passed=true 掩盖性能失败 |
| 最后冻结原版 4×、480 帧 | 运行 / 一致性通过，性能仍不合格 [E65] [E66] | 约 19.13 / 19.63 Hz；原版同样慢不能消除候选问题 |
| 原版 / r6 相同窗口离线整理 | 已完成 179→527、348 新帧的连续窗统计 [E67] [E68] | r6 约 8.97 / 9.15 Hz；顺序单个样本，不是有因果控制的 A/B |
| 前期真实 RTC 性能 A/B | 五份报告通过运行 / 一致性门槛；r2 的 4× 明显回退，r3 的 4× 仍未达标 | 双端数值和未通过的 cadence 条件见第 10 节，尚未验收优化 |
| 真人与高刷体验 | 尚未验收 | 尚不能承诺达到 TH08 / TH09 的实际体验 |

WASI 数量以日志中的 `: run wasm32` 条目计数。此前 51 是含重复层级的 PASS 消息数，
不是 suite 数量；主执行已修正 receipt，本文保留这次计数口径修正。

第一次 smoothness runner 尝试在 HTTP 启动阶段失败，**没有运行游戏**。
原因是共用 `createPresentationLabServer` 不允许 HTTP `PORT=0`；失败收据见 [E1]。
修正后 HTTP 使用既有 check-network 方式预选空闲端口，并核验实际 ready IPC；
只有 relay 从实际 `PORT=0` 监听结果取得端口。该失败不构成游戏性能观测。[S15]

r2 标记的冻结 baseline 报告见 [E2]，它仍加载旧 baseline WASM，不是最新 r2 候选的性能数据。
本次使用 headless Chrome `145.0.7632.6`，实际退到 relay；两端 impairment 的 matched / scheduled / sent 全为 0。
检查器以 `Real RTC lane was not impaired` 拒绝该报告；请求 50 ms、jitter 5 ms 不代表已实际施加。
因此不能计为“50 ms 样本”，不能据其游戏推进或内部 peer passed 判断性能通过。退 relay 的根因尚未确定。

headed-r3 baseline 在 Chrome `155.0.8059.39` 也退 relay，报告见 [E3]，属于建链失败证据。
`EAGLER_NETPLAY_RTC_TIMEOUT_MS`、`TH07_RTC_TIMEOUT_MS`、`TH07_DIRECT_TIMEOUT_MS` 均为 null，采用默认 4,500 ms。
相对 P0 connect：peer-join 85.3 ms → createOffer 调用 4,552.7 ms → fulfilled 4,557.4 ms → relay route 4,561.3 ms。
peer-join 到调用前相隔 4,467.4 ms，而 offer Promise 只用 4.7 ms；pre-route PC / ICE 均为 new，通道为 connecting。
现有观测不能进一步拆分 PC constructor 与 dataChannel 创建耗时，也不能把长等待归咎 createOffer Promise；不据此宣称性能提升。

补记环境反馈：用户报告测试期间发生断网并出现大卡。首次 baseline 尖峰运行（11:03:36–11:04:11 UTC）另有环境旁注 [E12]，
原样保留并按“用户报告同期断网，精确重合和原因待复核”处理，不作正式配对验收。后续 repeat 与 r3 尖峰记录另见第 10.3 节。
用户断网与此前 stable r2 / r3 运行的精确时间重合尚未核实，不能据此倒推既有稳定网络样本全部无效。

原有不利结果仍保留；不得将启动失败、超时或未完成的测试计入通过样本。

## 10. 有效性能证据与下一步

后续旧 / 新对照暂以本地 runner 环境 `EAGLER_NETPLAY_RTC_TIMEOUT_MS=15000` 诊断建链，指标必须标注 `rtc15s`。
该条件只影响本地测试子进程；生产与测试站超时配置不改，也不代表直连冷启动问题已解决。

有效报告 [E4]–[E8] 使用 Chrome 155 headed、真实 RTC、Intel UHD / ANGLE D3D11、120 Hz 触控计划、unlocked。
游戏输入发送采用应用层 50±5 ms 计划延迟；计时器迟到另有记录，该参数不是 wire RTT 测量。
每份报告双端均按首次前沿 179→947 的连续时间窗汇总，共 768 个新帧；CPU 条件见表。

| 版本 / 证据 | 双端 CPU | 端点 | 逻辑 Hz | Present Hz | callback 最大 ms | advance 间隔最大 ms | cadence |
| --- | ---: | --- | ---: | ---: | ---: | ---: | --- |
| baseline [E4] | 1× | P0 | 59.657 | 164.413 | 8.0 | 29.7 | true |
| baseline [E4] | 1× | P1 | 59.774 | 160.848 | 23.2 | 77.2 | false |
| candidate-r2 [E5] | 1× | P0 | 59.960 | 153.278 | 9.7 | 39.9 | false |
| candidate-r2 [E5] | 1× | P1 | 59.952 | 158.489 | 7.7 | 49.1 | false |
| baseline [E6] | 4× | P0 | 59.723 | 128.035 | 22.9 | 41.6 | false |
| baseline [E6] | 4× | P1 | 59.691 | 136.900 | 34.1 | 38.1 | false |
| candidate-r2 [E7] | 4× | P0 | 51.813 | 77.109 | 27.8 | 70.8 | false |
| candidate-r2 [E7] | 4× | P1 | 51.956 | 146.709 | 14.6 | 62.2 | false |
| candidate-r3 [E8] | 4× | P0 | 54.141 | 50.702 | 38.9 | 80.2 | false |
| candidate-r3 [E8] | 4× | P1 | 54.276 | 153.756 | 28.4 | 59.9 | false |

以上十个端点窗口 advance >100 ms 均为 0；baseline 1× 的 P1 另有 Present 最大间隔 51.5 ms。
五份报告的真实注入、最终 canonical state、确认音频与 RTC 门槛均通过；runner 收据 passed / integrityVerifiedOnCompletion 均为 true。
报告有效与 cadence 通过是两回事。候选 1× 两端 cadence 均为 false，且 P0 最大 advance 间隔高于本次基线，不能只选较有利的一端。

候选 1× 原生状态实际记录到 P0 / P1 的分数相位高刷呈现 1,964 / 2,031 次，支持额外插值呈现确有运行。[E5]
同一窗口两端 recovery yield 均为 0，因此该组尚未覆盖跨回调分段恢复；它也不能证明最终像素中的物体运动已经连续。

**4× 的 r2 候选在本组观测中明显回退：双端逻辑由约 59.7 Hz 降到约 51.8–52.0 Hz，advance 长间隔也增多。**
P1 的 callback 最大耗时和 Present Hz 较有利，仍不能抵消逻辑推进的退步，更不能把报告 `passed` 当作流畅度验收。[E6] [E7]

| 4× 诊断项 | r2 P0 [E7] | r2 P1 [E7] | r3 P0 [E8] | r3 P1 [E8] |
| --- | ---: | ---: | ---: | ---: |
| callback p99（ms） | 22.8 | 12.3 | 22.9 | 12.0 |
| callback 工作量 / 新帧（ms） | 15.8115 | 12.9680 | 15.5358 | 11.7776 |
| advance 间隔 p99（ms） | 47.3 | 41.7 | 62.9 | 39.5 |
| advance >50 ms 次数 | 5 | 1 | 17 | 2 |
| recovery yield / recovering callback 次数 | 152 / 152 | 77 / 77 | 329 / 329 | 49 / 49 |
| 最大 missing inputs（f） | 6 | 7 | 8 | 7 |
| interval scale 峰值 | 1.300 | 1.907 | 1.733 | 1.827 |
| 分数相位高刷呈现次数 | 627 | 1,675 | 170 | 1,912 |

r2 / r3 的两组 4× 窗口均实际覆盖了跨回调恢复；全部 correcting 样本通过保留 frontier 断言，最终两端 `Correcting=0`，canonical 与确认音频一致。[E7] [E8] [S14]
这支持相应恢复与收尾的一致性，不证明调度已经达成性能目标。r2 的 CPU / 网络等待分类缺陷已在第 5 节留证并由 r3 修正；
r3 仍有明显成本和节奏问题，不能将剩余降速一律归于外部网络。

**r3 仍不可推广：双端逻辑约 54.1–54.3 Hz，仍低于该条件的旧基线，P0 Present 仅 50.702 Hz，advance p99 达 62.9 ms。**
yield 数量和实际分数呈现次数只用于覆盖与时序分析，不是恢复更快或运动连续的证明。[E8]
r3 的独立 CPU 采样诊断见第 10.1 节，和本表未启用采样器的性能窗口分开记录。

4× r2 触控生产者全生命周期计划 2,096 / 2,092 次、实际提交 821 / 1,441 次、合并 1,273 / 648 次，停止时未服务 2 / 3 次。
计时器最大迟到为 51.6 / 25.6 ms；120 Hz 是墙钟计划频率，不能写成已实际交付 120 Hz。[E7]
r3 对应计划 2,023 / 2,020 次、提交 692 / 1,477 次、合并 1,328 / 541 次、停止未服务 3 / 2 次，最大迟到 54.2 / 29.0 ms。[E8]

五次运行使用同参数墙钟触控轨迹，但实际合并、采样时刻和游戏过程会不同，不是完全相同输入的 Replay。
这些是短窗口的观测 A/B，保留双端原值，不据此报告普遍提升百分比。当前候选未验收，继续定位、修正剩余问题。

1. 最后冻结原版 4×、480 帧校准及原版 / r6 的共同 179→527 连续窗整理已完成（第 10.18 节）。原版与候选都不合格，r6 在这对顺序样本中的长停顿更严重；结束本轮扩测并保存全部证据，不归因用户先前断网，也不预断言设备或候选是唯一原因。
2. 保留 r5 startup skew8 的成功及正常 4× 的失败；r6 Restart 已定位部分合法 Hints 自指针差异，但完整 scene / chain 图尚未证明等价。保持 44 词 gate，后续修复另行编号；有限生命周期完成不能代替正确性与性能验收。
3. 短 profile 仅提出三个后续试验：固定 80 B 模型 VBO 驻留、精确 GL 状态缓存、音频独立验证。这些方向在当前记录中尚未实现、没有收益结论，详见第 10.13 节。
4. 桌面每端 120 Hz 墙钟触控计划保持，实际提交 / 合并 / 未就绪跳过另记，不用手机；双端准备后 connect，严格记录实际 RTC、throttle、callback、Present 和真实 advance。
5. 保持初始及最终 canonical、确认音频和完整时间窗门槛；正式产品链路与真人高刷体验验收完成后，再形成用户可感知的结论。

driver 记录原始 native cursor，以及每个 callback 结束时实际观测到的 high-water。
summary 按首次跨越下界与上界选连续时间窗，保留回退期间全部 callback，不能过滤掉较慢的恢复片段。[S13] [S14]

候选可选导出 recovery yield、correcting、correction end、prediction frontier、缺口和 interval scale。
旧 baseline 无该导出时记为 null；不能伪造“0 次恢复”或声称两版都有相同观测。
这些轻量只读计数不把正式 WASM 变成诊断 fixture 构建。

Present 次数只是默认 framebuffer 的实际 blit 证据。
现有 harness 尚未测最终像素中对象运动连续性和 input-to-photon；native 触控刺激也不是完整 Launcher 输入链。
若单个 Restore 或完整历史事务仍出现长耗时，应继续按 owner、日志粒度和实际热点优化，不能用分段阈值掩盖。[S13] [S14]

### 10.1 r3 CPU 采样诊断：独立运行

采样报告 [E9] 使用同一正式 r3 WASM，`fixtureBuild=false`，另行开启 `--profile`；双端 CPU 4×、headed Chrome 155、
真实 RTC、应用层 50±5 ms 计划延迟、120 Hz 触控计划和 `rtc15s` 条件均有记录。采样器只记录 P0 的 `peer-0.cpuprofile`。
收据 passed / integrityVerifiedOnCompletion 为 true，其范围仍是运行与一致性。
**这是带采样开销的独立诊断运行，不能作为未采样性能验收，也不加入上表的 A/B 样本。**[E9]

采样请求间隔为 500 µs；记录跨度约 24.614 s，覆盖完整采集生命周期，不等于上表的 179→947 窗口。
以下为 `hotspots.json` 中 rAF 调用栈的包含子调用估计时间（inclusive），不是单次调用延迟。[E10]

| 调用路径 | 采样估计（s） | 解释 |
| --- | ---: | --- |
| native rAF `frame(double, void*)` | 14.576 | 该采集生命周期内 rAF 栈的总采样时间 |
| `Backgrounds::draw` | 3.743 | 背景绘制及其子调用 |
| `StageRenderer::draw_objects` | 3.290 | 对象绘制及其子调用，与上层背景路径有重叠 |
| `RollbackState::BeginFrame` | 1.497 | 每帧日志准备及其子调用 |
| `RollbackJournal::Touch` | 0.848 | 写前日志触达及其子调用 |
| `eagler_journal_copy_bytes` | 0.758 | 日志字节复制；本次 self 与 inclusive 相同 |

**这些行不能相加得出“可省下的时间”。** 父子调用相互包含，BeginFrame、Touch、复制也可能覆盖同一批采样；
采样器、CPU 限速和观测本身带来开销，当前数字只能帮助选择需要进一步验证的成本来源。[E10]

绘制上下文归因中，历史背景约 0.955 s：`update-and-reconciliation` 中的背景为 0.910017 s，
显式 `historical-semantic-draw` 中为 0.044551 s。后者整类仅 119.035 ms，按可见的 `draw_all` / `AppLoop.update` 栈形状分类；
编译内联会让部分历史路径落在较宽的 update/reconciliation 类，不能用 119 ms 代表全部历史 Draw，更不能据其大小跳过 semantic Draw。[E11]

`bufferData` 的自身采样主要落在正常 forward semantic Draw（272.414 ms）与只读呈现（234.557 ms）。
这使正常 / 高刷提交也成为成本排查对象，不能将上传成本全部归到历史回滚；该归因仍受采样和栈分类边界限制。[E11]

**精确投影缓存候选代码已完成，定向原生 / WASI 检查通过；首次 Web 最终一致性验收失败，r4 不可采用。**[E17] [E22]
检查包含 1,847 次完整输出与 flags / modes / errno 等价对照、199 次真实 flags 转移，顶点逐位一致；
全部 78 个图形输入键字、碰撞与驱逐、非 8 点回退和 aliasing 均有覆盖。此处的测试 WASM 不是正式 r4 游戏产物。
正式 r4 multiplayer 构建已于 11:15:46 UTC 成功完成，`diagnostic=false`，完整字节身份见第 1 节及 [E18]。
首次 1× 真实 RTC 运行已完成测量窗口，但最终 canonical 不一致，详见第 10.6 节；数学等价不能覆盖整个世界的构建与恢复行为。
后续同字节复测曾通过（第 10.8 节）；再用启动偏斜在缓存开启 / 关闭时均复现初始失步，并完成 r5 的定向启动修复验证（第 10.9 节）。
首次自然触发报告缺少初始观察，仍保留其证据边界；这些后续结果不构成整版缓存性能验收。
产品中的缓存命中、内存成本、回滚使用及正确性仍须结合实际运行复核，不能先行宣告收益。
历史 semantic Draw 继续承担完整世界事务，缓存不能替代它的状态效果。
几何 journal 的保存范围也必须覆盖实际写入与消费范围：尚未证明可以只按原请求尺寸截短 geometry 保存，不能把这种截短当作已成立优化。
缓存命中率、额外内存、复制成本和最终未采样的帧间隔收益均待验证；r3 未达标的判断保持不变。

### 10.2 稳定 r3 的恢复尾部重算

对稳定 r3 的 P0 原始窗口（零基 row 153–1201，共 1,049 次 callback）复核：720 次为非 Correcting 且已 presented，
329 次为 Correcting 且未 presented，没有其他组合。因此这 329 次未提交新呈现不能归因于 10 ms 的最终 Draw 开始门槛。[E8]

329 次 yield 中有 149 次只剩最后 1 f，其中 142 次 missing inputs ≤4 f；连续 Correcting 共 247 段。
至少 30 对相邻恢复片的 next 没有增加（6 次倒退、24 次持平），这些相邻片的 CorrectionEnd 均相同。
结合每片至少完成一个完整历史事务的源码约束，这证明存在前缀重新恢复、重算，不能将“仍在恢复”都理解为向终点稳定前进。[E8] [S1]

例如 row 311–314 的保留 frontier 都为 282，next 为 278→281→281→281，missing 为 4→4→3→2，scale 均为 1。
row 315 才恢复 forward 与 presented；前后两次非 Correcting 且 presented 的回调末尾相隔 67.5 ms。[E8]
由此提出的最后一笔历史事务准入窗口，已在 r6 接线实现并通过原生边界检查，见第 10.11 节。
它保留最多 6 笔和第一笔保证推进；后续仅在 remaining=1 且回调用时<12 ms 时放宽普通 8 ms 开始门槛。
这是 TH10 的已实现实验参数，尚无 Web 收益验收；12 ms 不是整回调上限，r3 原始样本仍对应旧 8 ms 策略。[S25] [E41]

### 10.3 网络尖峰重跑与异常样本

两份后续记录为 baseline repeat [E13] 与 r3 [E14]。配置均为双端 CPU 4×、真实本地 RTC、120 Hz 墙钟触控、
基础应用层延迟 50±5 ms，并设 `spike_frame=450`、`spike_ms=600`、`spike_delay_ms=300`、`spike_seat=1`、`rtc15s`。
报告与 runner 的 passed / integrity 均为 true，仅支持相应运行、路由和最终一致性；下表保留原始观测，**不构成已排除干扰的配对性能结论**。[E13] [E14]

| 运行 | 端点 | 逻辑 Hz | Present Hz | callback 最大 ms | advance 最大 ms | advance >100 ms 次数 |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| baseline repeat [E13] | P0 | 55.539 | 106.195 | 51.9 | 192.1 | 1 |
| baseline repeat [E13] | P1 | 55.461 | 86.759 | 35.6 | 66.3 | 0 |
| r3 异常待复测 [E14] | P0 | 6.814 | 7.046 | 223.6 | 656.6 | 502 |
| r3 异常待复测 [E14] | P1 | 6.820 | 6.777 | 211.3 | 656.6 | 509 |

baseline repeat 的 P0 在 frame 461→462 间隔 192.1 ms，其间仍执行 29 次 callback，单次 native 工作最大 21.0 ms，
callback 到达间隔最大 24.8 ms，连续确认前沿仅由 449→450 前进。**这段没有被 rAF 整体阻塞**；
长逻辑推进间隔不能单独证明浏览器回调停摆。上述 21.0 / 24.8 ms 仅描述该段，整轮的其他尖峰仍保留在表中。[E15]

r3 的恶化在计划尖峰之前已经出现：P0 的 frame 203→204 在本端时间 2.8457→3.5023 s 才完成，间隔 656.6 ms。
更直接的同 realm 证据是 P1：它在 5.6038→6.2604 s 的 frame 218→219 已出现 656.6 ms 间隔，
而 P1 的尖峰开始锚点为本端 45.7714 s。因此不能将这次约 6.8 Hz 的异常全部归于 frame 450 注入。[E14] [E15]
这些时间都使用各发送端自身的 realm 锚点；不将两个浏览器的 performance 时钟直接相减，也不假定跨 realm 精确对齐。

唯一已登记的宿主快照为 11:11:57 UTC，晚于 r3 runner 的 11:08:57 结束时间；当时总 CPU 33%，记录中已无 Chrome，仍有其他进程负载。[E16]
**这不是测试期间的同步证据，不能回溯证明异常由宿主干扰造成。** 当前原因待复测；也不能把这组异常当作 clean paired performance。
原始报告、逐 callback 数据、runner、环境旁注及离线检查 [E15] 全部保留，不裁掉恶化区间，也不据此改写此前 stable r2 / r3 的有效性。

### 10.4 原版校准同样异常：未完成的部分样本

随后重新运行冻结原版，仍为第 1 节的 `2c5b5a16…96d83` WASM；双端 CPU 4×、120 Hz 墙钟触控、headed、
基础应用层 50±5 ms、`rtc15s`，本次未启用尖峰注入（`spike_ms=0`）。runner 于 11:16:20→11:18:50 UTC 运行，
检查器在 120 s deadline 后以 `Native performance deadline, [False, False]` 失败，两端 `done=false`。[E19] [E20]

| 原版部分样本 | 记录的 callback 数 | 最后已见逻辑前沿 | 部分跨度逻辑 Hz | native callback p50 ms |
| --- | ---: | ---: | ---: | ---: |
| P0 | 675 | 732 | 5.581 | 121.6 |
| P1 | 720 | 744 | 5.561 | 120.1 |

两端初始 frame 均为 59，目标为 959；上表只描述失败前保留下来的原始数据，**不是完成 179→947 连续窗口的成绩**。
没有最终 canonical / 确认音频验收，runner 的 `passed=false`、`integrityVerifiedOnCompletion=false`；
不能把这些部分统计加入第 10 节的有效 A/B 表。[E19] [E20]

这份记录支持“严重异常也出现在未包含新缓存的原版，问题并非新缓存独有”；共同测试条件的具体原因仍未确定。
不能回溯断言设备或网络就是原因，也不能据此推翻此前稳定样本。当前先查共同条件，暂停用异常时段的数据评价优化幅度；
r4 随后的 1× RTC 曾出现最终一致性失败（第 10.6 节），原失败保留；r5 启动修复和正常负载失败另见第 10.9–10.10 节。
r6 已实现最后一笔历史事务的 12 ms 准入窗口并通过原生检查，但尚未 Web / 真人高刷验收。
原始失败报告、离线检查和完整日志保留；runner 收据中自己启动的 relay、HTTP server、checker 均标为 `closed=true`。[E19] [E20]

### 10.5 纯 CPU 校准：只描述本次浏览器算术耗时

11:21:45→11:21:51 UTC 运行两个独立 headed Chrome 155 进程，仅执行 JS / WASM 算术，没有游戏、网络或 GPU 绘制。
每端按 1×→4×→1× 切换，每档保留 7 次样本；报告 `passed=true`。以下是各档的中位耗时，单位 ms。[E21]

| 端点 / 算术 | 首轮 1× | 4× | 恢复 1× |
| --- | ---: | ---: | ---: |
| P0 WASM | 7.0 | 54.3 | 7.7 |
| P0 JS | 6.5 | 52.2 | 7.5 |
| P1 WASM | 6.9 | 31.3 | 6.6 |
| P1 JS | 6.6 | 29.0 | 6.3 |

同样声明 4× 的两个进程没有得到统一的四倍耗时；恢复 1× 后又回到约 6–8 ms 的中位区间。
这提供了限速表现存在端点差异的独立证据，但不等于游戏帧率、回滚成本或此前异常原因已经查明。
也不能将后续最终 canonical 分歧解释为这次纯计算校准或先前 4× 异常造成。[E21]

### 10.6 r4 首次 1× RTC：测量完成，最终 canonical 失败

11:24:13→11:24:59 UTC 的首次 r4 实测使用第 1 节正式 WASM，双端 CPU 1×、headed Chrome 155、硬件图形、
真实本地 RTC、应用层 50±5 ms、120 Hz 墙钟触控、解锁呈现与 `rtc15s`；未启用尖峰，未开启 CPU profile。[E22] [E23]
两端都完成 179→947、768 新帧的窗口，单端 `done / passed=true`；尾端已完成 / 确认帧对齐至 975（`next=976`）后，检查器却报告
`Final canonical composite differs`。**总体报告与 runner 均失败，r4 不可采用，不能把单端测量完成当作最终验收。**[E22]

| r4 失败运行中的端点观测 | 逻辑 Hz | callback 最大 ms | advance 最大 ms | timing cadence |
| --- | ---: | ---: | ---: | --- |
| P0 | 59.715 | 16.0 | 58.5 | false |
| P1 | 59.662 | 6.9 | 49.1 | true |

此表仅保留失败运行的诊断信息，不加入第 10 节的有效 A/B，也不支持优化收益或最终像素运动连续性的结论。[E22]
最终 composite 为 P0 `4206334120`、P1 `3140173708`。visual RNG 调用次数为 1,714 / 1,698，P0 多 16 次；
ANM `cursor` 与 `last_id` 均为 1,642 / 1,641，P0 多 1。对应的 RNG、ANM 和 scene hash 也不同。[E22] [S19]

捕获的 scene owner 字节对照仅列出 GUI 的 3 个差异位置；按结构定义和创建逻辑，字段含义如下。[E22] [S20] [S21]

| GUI 内字节偏移 | 所属字段 | 含义 |
| --- | --- | --- |
| `0x9e50` | `difficulty_badge` | ANM 动画句柄 |
| `0x9e54` | `difficulty_label` | ANM 动画句柄 |
| `0x9e58` | `border` | ANM 动画句柄 |

**这些是动画句柄，不是计时器。** 捕获的 Stage / StageVM 字节对照相同；这不消除 GUI、ANM 与 RNG 的分歧。
尾端报告尚不能单独确定分歧首次产生于初始激活、正常前进还是历史恢复，也未证明新缓存是唯一原因；必须继续定位，不能用此前性能异常解释掉一致性失败。[E22] [S14]
确认音频与网络尾端数组在本次捕获中相同，但不抵消 canonical 失败；1,847 次数学等价检查的通过范围仍保留，不能扩展成完整游戏验收。[E17] [E22]
runner 的 `integrityVerifiedOnCompletion=false`、checker exit code 1，自己启动的 relay / HTTP server / checker 均已 `closed=true`；原始报告与日志全部保留。[E23]

### 10.7 同对象布局的缓存关闭对照：单次一致性通过

主执行保持对象布局，构建关闭缓存的独立正式对照；完整 WASM 身份见第 1 节，构建 `code=0`、`diagnostic=false`。[E24]
其 11:34:14→11:34:49 UTC 的 1× RTC 运行及收据均通过，`integrityVerifiedOnCompletion=true`。[E25] [E26]
初始已完成 / 确认帧 59（`next=60`）和尾端 975（`next=976`），两端分别具有完全相同的 44-word canonical 与确认音频数组。
窗口 recovery yields / recovering callbacks 均为 0 / 0；逻辑为 59.617 / 59.639 Hz，两个 timing cadence 都为 false。[E25]
**这次没有覆盖跨回调恢复，不能凭它判定缓存有问题，也不能把运行通过写成流畅度验收。**

该场次起，检查器在既有 confirmed 59 围栏后、CPU throttle 与 `performanceStart` 前，增加只读 `initialComparison`；
若起始 composite 不同则另存 scene owner。它增加初始状态证据，未把呈现次数当进度，也未放宽最终 canonical 门槛。[S14]
原始 r4 失败场次没有这项初始观察，因此仍不能凭其尾端差异确定首次分歧阶段。上述对照与原失败记录分别保留。

### 10.8 原 r4 同字节复测：跨回调恢复通过，当时原失败未解

11:35:41→11:36:24 UTC 使用冻结的原 r4 WASM（仍为 `4f38b713…a66bb`），带相同初始观察再次运行 1× RTC。
报告与 runner 均通过，收据的 integrity 为 true；这是原字节的重跑，**不是一次已完成修复的证明**。[E27] [E28]
初始已完成 / 确认帧 59 的两端 44-word canonical 全等，composite 均为 `3770734673`；
尾端 975 的全数组也相同，composite 均为 `634562467`。两个观察点的确认音频均相同。[E27]

| 同字节复测端点 | 逻辑 Hz | Present Hz | work / 新帧 ms | callback 最大 ms | advance 最大 ms |
| --- | ---: | ---: | ---: | ---: | ---: |
| P0 | 59.132 | 132.340 | 12.783 | 19.9 | 79.4 |
| P1 | 59.129 | 129.877 | 12.868 | 50.9 | 78.3 |

179→947 窗口内 recovery yields 为 49 / 40，全生命周期计数为 72 / 58；这次确实覆盖了跨回调恢复，并在最终门槛上通过。
名义 CPU 1× 下仍有上述成本与长间隔，两个 timing cadence 都为 false；该运行不构成稳定性能 A/B，也不验证最终像素运动。[E27]
本场初始 ANM `cursor / last_id` 均为 59，而缓存关闭场次为 62；每场内部双端一致，跨场次初始状态却不同。
因此不能用一次关闭通过或一次开启通过确定缓存因果，也不能用重跑 PASS 覆盖原失败；后续定向启动证据见第 10.9 节。[E25] [E27]
两次后续运行的原始报告及收据保留，各 runner 启动的 relay / HTTP server / checker 均已关闭。[E26] [E28]

### 10.9 启动偏斜复现与 r5 启动门控验证

检查器新增 `--startup-poll-multiplier=8`，只对 seat 0 在 `multiplayer_active` 前多调用已有的 stopped-loop 加载推进；
每次调用都检查 active，一旦进入活动联机状态便停止倍增。它改变加载期间的调用次数，不伪造逻辑前沿，也不在正常游戏阶段倍增 tick。[S14]
本组为双端 CPU 1×、headed 真实本地 RTC、应用层 50±5 ms、`rtc15s`、240 帧请求；没有尖峰或 CPU profile，
`touch_motion_hz=0`。这是初始化正确性复现，不是此前的 120 Hz 触控负载测试或 768 新帧性能 A/B。[E29] [E31] [E36]

在已有 confirmed 59 围栏（`next=60`）、开始性能测量前读取初始状态，得到以下结果：

| 版本 | startupPollCalls P0 / P1 | ANM cursor / last_id P0 / P1 | visual RNG calls P0 / P1 | 初始 canonical |
| --- | ---: | ---: | ---: | --- |
| 冻结 r4，缓存开启 | 410 / 71 | 234 / 64 | 16 / 0 | 不同 [E29] [E30] |
| 冻结 r4，缓存关闭 | 274 / 49 | 166 / 53 | 16 / 0 | 不同 [E31] |
| r5，缓存开启 | 354 / 61 | 31 / 31 | 0 / 0 | 44 词全部相同 [E34] [E36] |

r4 开启缓存时，初始 GUI 的 `difficulty_badge / difficulty_label / border` 三个 ANM 句柄，P0 均比 P1 大 170，
对应差异在尾端仍保留。初始缓存 hits 为 0 / 0，但 misses 已为 2,856 / 2,856、evictions 为 2,600 / 2,600。
准确结论是**首次缓存命中前已经失步**，不是缓存代码尚未执行；关闭缓存也复现，说明这条初始化分歧不依赖缓存命中。[E30] [E31]
两份 r4 总体报告及 runner 都失败，不能把其中的帧率当作性能验收；原始数据和清理收据保留。[E29] [E31] [E32] [E33]
这已把一条可复现的分歧定位到初始观察点之前；首次自然触发的 r4 报告没有该观察，不能为它补造当时的初始状态。

**r5 的最小改动位于 `Application::step`，在网络服务与 `advance_loading()` 之后检查以下条件：**[S1]

```cpp
if (session.configured && !session.started && title && title->loading &&
    startup_view && startup_view->elapsed >= 2) return 0;
```

这针对首次 tick 前已经配置联机、尚未 started、title 仍在加载的新启动。加载器仍被服务，网络仍由回调入口或 step 前半段 pump；
满足条件时停止额外的 ANM / Common 推进，避免本地等待期间的调用次数继续改变开场状态。检查必须保留在 `advance_loading()` 之后，
使实际加载和等待仍能完成。[S1]
`startup_view->elapsed >= 2` 保留两次开场 authored Draw：第一次创建开场 VM，下一次呈现后才允许资源退场；
另由 Title 的 `steady_clock` 保留真实 2 秒等待。这两个“2”属于不同条件，不能把等待删掉或把 Draw 次数解释成秒数。[S22] [S23]
现有 shell launch 与 smoke `startNet` 都在首 tick 前配置联机，满足此次前提；启动后才 configure 的入口不在本次保证范围内。[S10] [S24]

r5 保留开启的精确投影缓存，相对原 r4 的原生改动只有上述启动门控；正式字节已冻结，身份见第 1 节及 [E35]。
其 11:46:33→11:46:57 UTC 的 skew8 运行及 runner 均通过，`integrityVerifiedOnCompletion=true`。[E36] [E37]
即使两端启动调用数仍是 354 / 61，初始已完成 / 确认帧 59 的完整 44-word canonical 相同，composite 均为 `2098270709`；
尾端已完成 / 确认帧 315（`next=316`）的全数组也相同，composite 均为 `1539085983`。两个观察点的确认音频均完全相同。[E34] [E36]
**可确认的是：r5 消除了这项受控 fresh-launch 启动偏斜复现中的双端分歧。** 这项 PASS 不代替正常 4× 触控、帧间隔、最终像素运动或真人高刷验收。
后续正常 4× 触控测试已登记为 deadline 失败，见第 10.10 节；不撤销本节的定向启动结果，也不能据启动 PASS 称整版合格。
冻结 r5 未加入 12 ms 恢复尾帧方案；该实验属于另行冻结的 r6，见第 10.11 节。[E35] [E44]

### 10.10 r5 正常 4× 触控超时：只有部分数据

11:49:17→11:51:56 UTC 的运行使用冻结 r5、双端 CPU 4×、headed 本地真实 RTC、应用层发送注入 50±5 ms、
`rtc15s` 和每端 120 Hz 墙钟触控计划；请求 900 帧，未注入尖峰、未开 profile，startup 倍增为 1。
检查器在 120 s performance deadline 后以 `Native performance deadline, [False, False]` 失败。[E38]–[E40]
测量前 confirmed 59 的完整 44-word canonical 与确认音频均相同，ANM cursor / last_id 均为 31，visual calls 均为 0；
正常启动调用数为 67 / 67。但这次双方 `done=false`，没有最终 canonical / 音频栅栏结果。[E38] [E39]

| r5 部分记录 | callbacks | 首末 callback 跨度 s | 观察到的新帧数 | 部分逻辑 Hz | native p50 / max ms | advance max ms |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| P0 | 1,078 | 120.112 | 685 | 5.703 | 65.8 / 268.5 | 671.9 |
| P1 | 1,092 | 122.135 | 696 | 5.699 | 66.5 / 256.1 | 652.3 |

上表按失败时保留的首末 callback 计算，**不是完成 179→947 测量窗口的成绩，不加入有效 A/B 表**。
首条 native 回调就分别耗时 114.8 / 121.6 ms，当时 recovery yields、Correcting 和 missing inputs 均为 0，scale=1。
P0 全部 1,078 条记录的 scale 都为 1；该端严重慢速不能归因于缺口渐进制动，也不能把全部普通帧成本归为恢复尾部。[E38]

失败收集并非同时停止两端，P1 的尾部约晚 2 秒读取；这段存在 P0 停止产出后的自然 producer-stop 影响。
不能把 P1 尾部 missing=12、scale=2 等值当作正常双端持续游玩的纯网络证据，也不将两个 realm 的时间戳直接相减作精确对齐。[E38]
触控实际提交为 897 / 917 次，合并 13,540 / 13,759 次；计时器最大迟到 422.8 / 383.5 ms。
120 Hz 仍只是墙钟计划，未被改成随逻辑降速的刺激，但主线程无法实际逐次交付。[E38]

事后复核回执所列运行时、测量器、relay 入口等 12 项哈希，全与开跑记录相同；这不等于完整 relay 导入模块树均已逐项证明。
runner 仍为 `passed=false`、`integrityVerifiedOnCompletion=false`，事后核对不能补造运行完成门槛。
自己启动的 relay / HTTP server / checker 均已关闭，原始记录和日志全部保留。[E38] [E40]

### 10.11 r6：最后一笔历史事务的有限准入窗口

`RecoveryBudget::CanStartTick` 已接到 `Application::multiplayer_update`，沿用同一 native callback 的墙钟起点。
冻结 r1–r5 的普通历史继续门槛是 8 ms；r6 只增加最后一笔历史事务的完成机会。[S1] [S25]

| 条件 | r6 是否允许开始历史事务 |
| --- | --- |
| 没有 remaining，或本回调已完成 ≥6 笔 | 否 |
| 尚未完成第一笔，且仍有历史 | 是，即使回调已超时；保证至少完成一笔事务 |
| 已完成 1–5 笔，remaining≥2 | 仅 elapsed<8 ms |
| 已完成 1–5 笔，remaining=1 | 仅 elapsed<12 ms |

精确到 8 / 12 ms 的追加门槛拒绝新工作；准入后仍必须完成整笔 Update、semantic Draw、journal / 音频事务和 MarkSimulated。
**12 ms 是最后一笔历史事务的开始门槛，不是其执行时长、整回调耗时或 GPU 的上限。**
该窗口不额外授权 forward tick，不允许呈现半恢复世界，也不改变新输入一次采样和 r3 的 CPU / network 时间记账。[S25]

既有 `netplay-pacing` WASI suite 加入上述边界用例后已通过：覆盖空历史、第一笔超时仍进、8 / 12 ms 精确边界、
最多 6 笔，以及最后一笔可在 9 ms 开始、完整执行到 14 ms 的事务例子，避免误把准入阈值当硬截止。[S17] [E41]
该收据证明组件合同，不能证明真实 RTC 下的尾部重算已经减少。

r6 已完成正式 multiplayer 构建（12:15:21 UTC），并于 12:21:29 UTC 冻结；WASM 与 sourceDigest 见第 1 节。
它保留 r5 启动门控和精确投影缓存，并包含下一节的退休包校准接线。
r6 后续的真实 RTC 原生 Restart 已执行，但新局完整 canonical 失败（第 10.14–10.16 节），没有通过整版生命周期正确性验收。
该检查使用 stopped-loop 推进；另行运行的真实 rAF 1× / 4× 结果见第 10.17 节，其中 4× 性能严重不合格。
既有 r3 / r5 数据仍归其原始字节身份，不改名为 r6；现有计数没有单独证明 12 ms 宽限的命中次数或收益。[E44] [E48] [E59] [E61] [E63]

### 10.12 r6：退休 ACK 不应使下一轮校准失败

`RetireRun()` 成功后，可靠通道上的旧 ACK 等报文仍可能迟到。旧 Adonis 校准入口会把不属于当前会话的这类合法报文判为失败。
新入口由 TH10 `BeginNextRun()` 显式传入刚退休世代的描述符，按报文类型验证旧 session、玩家 mesh 与相关字段；
仅匹配的合法旧报文被忽略，未登记的世代、错误字段、畸形包及过早的当前 gameplay 输入继续拒绝。[S4] [S26] [S27]

原生对照使用旧 `d064941d` 头文件与新头文件，输入完全相同的 **44 字节合法旧 ACK**，日志中的报文 hex 逐字相同。[E42]
这份独立 common 对照选择测试 fixture 的 `MagicGame='T'`，包头为 ETNP；正式 TH10 选择标题配置 `MagicGame='A'`，包头为 EANP。
实际正式 WASM 的 native terminal send 已观察到 `45414e50`，与 TH10 读取器一致。fixture 报文不应被称为标题线上包，也不能将两者包头差异记为识别器缺陷。[S31] [S32] [E54]

| 版本 | Pump | failed | Waiting | Ready | deferred pending |
| --- | ---: | ---: | ---: | ---: | ---: |
| 旧头文件 | false | true | true | false | 0 |
| 新头文件，显式已退休描述符 | true | false | true | false | 0 |

新版只是继续等待本轮真实校准，没有因旧 ACK 提前 ready，也没有把旧输入透传至新核心。
既有 `adonis-startup` 整套及新正反用例通过，覆盖 2 / 3 人、模式与手动 D、丢包、慢加载，
旧尾包 / HELLO / READY / phase 的边界、当前 HELLO 字节顺序、错误与未退休包拒绝，以及 Clear 后不能继承旧例外。[S27] [E42]
旧版重现用例按预期以 17 退出，收据把它记为已成功观察旧缺陷；这不等于旧实现已通过新行为要求。

common 已独立本地提交 `77666259b4bb14b965a34a1f38a4e2ec88c289ac`，仅涉及头文件与既有测试两文件，回执状态为空。
没有 push / merge。随后真实 RTC 原生菜单 Restart 已运行，却在新局完整 canonical 检查失败；
因此不能把这份原生对照扩大成菜单流程已修复通过，具体记录见第 10.14 节。[E43] [E52]

### 10.13 r5 短 4× profile：普通绘制与音频仍需独立优化

12:01:42→12:02:19 UTC 另跑冻结 r5 的 240 帧、双端 4×、120 Hz 墙钟触控、headed RTC 诊断。
仅 P0 开启 500 µs CPU 采样；profile 跨度约 12.473 s，覆盖采集生命周期，不是完整性能测量窗口。[E45]–[E47]
两端完成采集，最终 confirmed 315 的完整 44-word canonical 和确认音频相同；但 summary 窗口只有 108 个 callback，
未满足既有 `len(rows)>120` 门槛，报告以 `Insufficient measured callbacks` 失败。没有降低门槛，runner 仍失败。[S14] [E46] [E47]

| P0 采样路径 | 估计 ms | 口径 |
| --- | ---: | --- |
| native `frame(double, void*)` | 6,968.290 | inclusive，采集期间总量 |
| `AppLoop::draw / AnimationEngine::draw_all` | 4,480.667 | 相同嵌套路径，占 frame 采样约 64.3% |
| `bufferData` | 606.245 | self；绘制子调用，不是单次上传耗时 |
| `sdl_audio_pump` | 590.918 | inclusive，含混音 / 重采样子调用 |

上述父子调用重叠，**不能相加作为“可省下的时间”**；64.3% 的分母是 frame 栈采样量，不能解释成整机 CPU 百分比。
P0 全采集记录的 recovery yields 为 0，普通 Draw 已有明显成本；另一端为 186，不能由 P0 的热点代表所有恢复场景。
这是带采样开销的短诊断，不作未采样性能验收，也不和此前 900 帧失败拼成完整 A/B。[E45]

接下来仅列三个独立试验方向：固定 80 B 模型 VBO 驻留、保留完整状态正确性的精确 GL 状态缓存、音频泵与重采样的独立验证。
这些方向在当前记录中尚未实现，也没有 Web 收益结论；须继续保留 semantic Draw、资源寿命和确认音频合同。
原始 profile、失败报告与日志保留，runner 自己启动的子进程均已关闭。[E45] [E47]

### 10.14 r6 真实 RTC 原生 Restart：新局 canonical 失败

Manual Restart 检查器、smoke / runner 及嵌入 JS 的 6 项语法 / AST 检查通过，这只是测量工具的语法证据。[E49]
首次 runner 于 12:27:17 UTC 因共享 Launcher 缺少 `ws/index.js`，relay 在 ready 前退出；
当时尚未开浏览器、未进游戏，没有 checker JSON。失败收据及 relay stderr 保留。[E50]

主执行随后在 `artifacts/rollback-smoothness/relay-r6-locked` 建立独立测试 relay 快照，来源为稳定提交
`f92b1595a10b14b2fe0c6e3dce398e81b45be082` 的 26 个源码文件；按来源 package-lock 固定
ws 8.21.3、TypeScript 7.0.2、esbuild 0.28.2，原 contracts-build 成功，未修改共享 Launcher 树。
来源、源码和 contract manifest 哈希见快照；这是本地测试条件，不是 WAN 性能证据。[E51]

12:31:42→12:32:32 UTC 的独立 relay basic 运行使用正式 r6、两个独立 Chrome 155 桌面进程，
请求并选定纯 Delay D1、prediction=0，由 P1 原生 SDL 键盘输入经过实际菜单 Pause → Restart → Yes。
**检查器调用 stopped-loop native tick；它验证实际 RTC 和原生生命周期，不运行 rAF 性能窗口，也不覆盖 Launcher UI 或物理设备体验。**[S28] [E52]

| 观察点 | generation | LastSimulatedFrame | confirmedThrough P0 / P1 | 完整 44-word canonical | 确认音频 |
| --- | ---: | ---: | ---: | --- | --- |
| 初始共同边界 | 0 | 59 | 60 / 60 | 全部一致 | 一致 |
| 新校准后共同边界 | 1 | 119 | 120 / 120 | 零基 words 1 / 10 / 11 不同 | 一致 |

旧局的 RTC 证据记录双端 input / control 通道 open、连接 connected，并选中 UDP host pair。
菜单操作后，两端进入 generation 1，新校准各有 129 probes / 120 replies，最后观察仍为 `route=rtc`。
它们确实继续到相同的 LastSimulatedFrame 119；但完整 canonical 的 composite word 1 为
`3764867713 / 979422204`，words 10 / 11 也不同，故报告与 runner 均为失败。[E52] [E53]
不能用音频一致、其他 41 词一致或新校准完成替代完整 canonical 门槛；后续仅定位了已导出 owner 的部分差异，完整图等价尚未证明，见第 10.16 节。

本轮 `late_terminal_ack=false`，没有执行可选的受控迟到旧 ACK 交错，因此也不能宣称该真实网络交错已通过。
这项结果不撤销 r5 的受控 fresh-launch 启动修复，但明确说明那项保证不能直接扩展到 Restart 的新世代。
r6 整版不可采用，后续修复须另行编号保留旧失败；runner 的 `integrityVerifiedOnCompletion=false` 及全部原始数据保留，
自己启动的 relay / HTTP server / checker 均已关闭。[E52] [E53]

### 10.15 迟到旧 ACK 诊断首轮：初始边界就失败

诊断增加了 scene owner 原始字节与 `portableCanonical` 附加观察；本轮没有更改正式 r6 的原生源码。
`manual-restart-late-terminal-ack-diagnostic.json` 用时 36.701 s，在初始 Last 59、confirmed 59 / 60 的完整 44 词检查就失败。
零基 words 1 / 10 / 11 / 28 不同，分别涉及 composite、scene、chain 和 raw EnemyManager；完整 owner 字节及 changed offsets 均保留，不能概括为全是无害指针。[S19] [E55] [E56]

这轮只有一个初始 checkpoint，`nativeTerminalSends=0`，没有 held / released ACK 或匹配接收记录。
因此既未进入 Restart 菜单，也未捕获、重发退休 ACK；它不能提供所请求网络交错的覆盖。
原始报告 SHA-256 为 `9ebf13bdffa9618f16c433013fadfcaa0f94a317178ff0259ba9c07f8e6d3373`；
runner 为失败、`integrityVerifiedOnCompletion=false`，自己启动的三个子进程已关闭。[E55]–[E57]

### 10.16 继续观察：真实旧 ACK 生命周期完成，完整正确性仍失败

后续 checker 显式启用 `--observe-after-canonical-mismatch`，允许记录正常完整帧检查点失配后继续诊断。
它仍累计这些完整 44 词失配，并在结束时强制失败；没有改用 portable canonical，也没有缩小原正确性 gate。
新世代刚建立的原始 scene 是额外只读观察，不能混称为已完成逻辑帧的常规 checkpoint。[S28]

本轮用时 56.855 s，`lifecycleCompleted=true`、`canonicalPassed=false`、`passed=false`。
原始 SHA-256 为 `9e818f99e395617899efb694d8eda08458e2a1b310063acb1a007091b7bbc42e`。[E58] [E59]

| 观察点 | 原生进度 | 完整 canonical | portable canonical / 确认音频 |
| --- | --- | --- | --- |
| 初始共同边界 | Last 59，confirmed 60 / 60 | 44 词全部一致 | 均一致 |
| 新 generation 刚建立 | Next 0，Last -1，尚未 ready | words 1 / 10 / 11 已不同 | 此处为额外 scene 观察，不据此声称音频 gate |
| 最终共同边界 | Last 119，confirmed 120 / 120 | words 1 / 10 / 11 仍不同 | 均一致 |

最终 **已导出的 11 类 scene owner 字节差异全部为 Hints 的 16 个合法 self pointer**。
Hints 长 424 B，两端地址为 86294592 / 78775616；从 `0x18` 起每 12 B 的 16 个值分别等于本端 Hints 地址。
这些字段是 `pending[8]` / `retained[8]` 中 `ListNode<StageHint>::value` 的哨兵 owner；
`StageHints::initialize` 明确用 `reinterpret_cast<StageHint*>(this)` 初始化。它们不是 NativeCallback context。[S29] [S30] [E58]

这一定位仍不等于完整状态图已证明等价：11 类导出 owner **不含 word 10 还会哈希的 `rollback_hints` pool**；
word 11 的 chain / entry / effect pool 也没有本轮分项原始字节证明。portable 相同只能作附加观察，
不能将全部 words 10 / 11 判为仅重定位，更不能消除最终完整 canonical 失败。[S19] [S24] [E58] [E59]

本轮确实从原 RTC control 通道重发已退休 frame 154 的原始 **44 字节 EANP 缓存 ACK**，
字节 SHA-256 为 `8329546eccba835c886ab1179b7dcf45a10f8a50d78d74f4d58c1c63915e616c`。
新校准状态 2–4 中实际收到相同字节，原 receive 调用正常返回，随后完成 129 probes / 120 replies 并继续新局。
记录有 160 次匹配接收；正常原生重传和受控重发的字节相同，不能把这 160 次匹配写成 160 次手动注入。
退休 poll-only 窗口只覆盖 9.512 ms / 1 poll，不外推为任意退休持续时间或网络组合均通过。[E58] [E59]

这支持有限的“旧 ACK 没有阻止这次新校准及继续运行”观察，不支持整条 Restart 正确性 PASS。
事后核对运行时、测量器与 relay 的 12 项哈希均相同；该核对不覆盖失配，runner 仍为 FAIL，自己启动的子进程全已关闭。
两轮都使用 stopped-loop 正确性入口，不作为 rAF、WAN、Launcher 完整输入链或物理设备流畅验收。[E58]–[E60]

### 10.17 正式 r6 的真实 rAF：1× 完成，4× 性能严重不合格

两次都使用冻结 r6、独立锁定 relay、两个 Chrome 155 headed 进程、硬件 GPU、实际 RTC 和已确认的双端 CPU throttle。
请求 900 帧、input delay 0、unlocked presentation、每端 120 Hz 墙钟触控计划；
应用层发送计划为 50±5 ms，`rtc15s` 只用于建链等待，没有注入 spike、没有启用 profile，startup multiplier=1。[E61]–[E64]

两轮初始 Last 59 / confirmed 59 的完整 44 词和音频均一致，composite 均为 1092101675。
最终都到 Last 975 / confirmed 975，完整 44 词和确认音频在各自两端一致；
1× 最终 composite 为 2464875763，4× 为 1305346354。不同运行的触控实际采样和游戏过程不同，不要求两场最终 hash 相同。
两份 runner 均 `passed=true`、`integrityVerifiedOnCompletion=true`，RTC barrier / final 均为 rtc，所启动子进程全已关闭。
这些 PASS 的范围是运行、路由、状态与确认音频；**4× 的流畅度明确失败，不能用总 passed 字段掩盖。**[E61]–[E64]

各端都取实际 high-water 首次跨越 179→947 的连续窗口，共 768 个新帧；恢复期间的全部 callback 保留。

| 测量项 | 1× P0 | 1× P1 | 4× P0 | 4× P1 |
| --- | ---: | ---: | ---: | ---: |
| 逻辑 Hz | 59.839 | 59.898 | 7.534 | 7.602 |
| Present Hz | 153.783 | 163.457 | 14.176 | 7.546 |
| native callback p99 / max，ms | 5.3 / 10.2 | 4.6 / 6.1 | 160.7 / 259.9 | 190.7 / 339.7 |
| native work / 新帧，ms | 3.378 | 2.898 | 87.011 | 85.273 |
| 实际 advance 间隔 p99 / max，ms | 27.8 / 60.3 | 21.6 / 49.2 | 636.1 / 834.3 | 493.2 / 780.9 |
| advance >50 / >100 ms 次数 | 2 / 0 | 0 / 0 | 605 / 418 | 601 / 424 |
| 恢复 yield 次数 | 0 | 0 | 554 | 247 |
| 最大缺失输入帧 | 6 | 6 | 12 | 9 |
| interval scale 峰值 | 1.333 | 1.333 | 2.000 | 2.000 |
| 分数 alpha 的实际提交次数 | 1,974 | 2,096 | 300 | 0 |
| desktopCadenceTargetMet | false | true | false | false |

1× 有高刷分数呈现，但 P0 仍有两次 >50 ms 的 advance 间隔；不能表述为双端全无卡顿。
其累计 rollback 为 711 / 667、重演 2,920 / 2,357 帧，所以零 yield 不等于没有回滚；
它只说明测量窗没有跨回调 yield 覆盖，也没有专门记录 12 ms 追加准入是否命中。
4× 确实覆盖 554 / 247 次跨回调 yield 且最终收敛，但其 native 和实际 advance 都出现长停顿；
P1 完全未观察到高刷分数提交，尚不能承诺实现用户要求的性能。[E61] [E63]

触控按墙钟持续计划，实际交付单独记账，以下均为**整段触控生产者生命周期**，不是上述 768 帧窗口分母：

| 触控账目 | 1× P0 / P1 | 4× P0 / P1 |
| --- | ---: | ---: |
| 计划事件 | 1,807 / 1,804 | 13,082 / 13,048 |
| 实际提交 | 1,604 / 1,804 | 1,855 / 888 |
| 合并的过期目标 | 4 / 0 | 10,022 / 12,143 |
| 未 ready 时跳过 | 199 / 0 | 1,201 / 14 |
| 停止时未服务 | 0 / 0 | 4 / 3 |
| 实际提交 Hz | 106.507 / 119.994 | 17.013 / 8.174 |
| timer 最大迟到，ms | 9.333 / 6.433 | 451.967 / 524.067 |

4× 的输入刺激没有主动随逻辑降速，但主线程无法逐次交付 120 Hz 计划。应用层发送计时器也迟到：
最大实际延迟为 513.6 / 568.3 ms、timer overrun 为 463.801 / 522.002 ms。
因此这里的 50±5 ms 只能称计划注入参数，不能写成实际网络 RTT 或已稳定交付的单向延迟。
这些记录不证明用户先前断网是本轮原因，也不自动证明全部成本来自宿主外部干扰。[E63]

最终像素对象运动与 input-to-photon 仍未测；native 触控 API 也不是完整 Launcher / managed 输入链。
最后的冻结原版 4×、480 帧校准和共同 179→527 帧窗口整理已完成，见第 10.18 节；原 768 帧全窗不被改写或替代。
不公布因果改进比例；Restart 的完整 canonical 问题和本次 4× 性能失败均仍阻止推广。

### 10.18 最后一次冻结原版校准：相同配置仍慢，不能消除候选问题

最后的冻结原版运行使用同一独立锁定 relay、同一检查器与 driver、Chrome 155 headed、硬件 GPU、
双端 4× throttle、120 Hz 墙钟触控计划、应用层 50±5 ms 和 rtc15s；请求 480 帧，无 spike、无 profile。
WASM 仍是已冻结的原版 `2c5b5a16bb786ccce36a08f34d90e6015abe81d1e78ffdffa9617b5d8ff96d83`，
未将当前候选源码重建成“原版”。runner 的执行区间为 12:50:34→12:51:23 UTC。[E65] [E66]

初始 Last 59 / confirmed 59 的两端完整 44 词与音频一致，composite 为 3502027289；
最终到 Last 555 / confirmed 555，完整 44 词和确认音频仍一致，composite 为 1320125848。
实际 RTC、硬件与已确认的 throttle gate 完成，runner `passed=true`、`integrityVerifiedOnCompletion=true`，
自己启动的 relay / server / checker 全已关闭。其通过范围依然不包含流畅度。[E65] [E66]

原版按 high-water 首次跨越 179→527 取连续窗口，共 348 个新帧，两端 cadence 均为 false。
P0 的 Present 虽为 128.142 Hz，逻辑只有 19.127 Hz；P1 的累计 rollback / resim 均为 0，
native callback 仍最大 185.6 ms。它们说明这轮慢不能全部由 r6 新增的跨回调恢复解释，
但没有隔离出全部成本来自何处，也不能用旧版同样慢来免除候选的性能问题。[E65]

两场是顺序执行的单个样本，实际触控交付与游戏轨迹不同，调度及宿主负载未被固定控制。
以下仅列相同逻辑区间的观察绝对值，不称为具有因果控制的 A/B，不报告改进百分比：

| 相同 348 新帧窗口 | 原版 P0 | 原版 P1 | r6 P0 | r6 P1 |
| --- | ---: | ---: | ---: | ---: |
| 逻辑 Hz | 19.127 | 19.630 | 8.975 | 9.154 |
| Present Hz | 128.142 | 19.651 | 26.481 | 9.033 |
| native callback p50 / p99 / max，ms | 2.7 / 32.6 / 51.2 | 29.8 / 106.6 / 185.6 | 6.6 / 145.3 / 193.2 | 46.3 / 200.4 / 284.4 |
| native work / 新帧，ms | 33.403 | 34.494 | 72.082 | 70.815 |
| 实际 advance 间隔 p99 / max，ms | 198.9 / 220.1 | 155.5 / 232.7 | 744.2 / 834.3 | 478.0 / 780.9 |
| advance >100 ms 次数 | 38 | 23 | 142 | 140 |
| 测量窗口 callback 数 | 2332 | 348 | 1378 | 410 |
| 窗口恢复 yield / 最大缺失帧 | 未导出 | 未导出 | 351 / 12 | 66 / 6 |
| desktopCadenceTargetMet | false | false | false | false |

原版样本见 [E65]；相同窗口的全部双端统计见 [E67]。原版没有新 recovery 导出，因此“未导出”不能读作零 yield。
原版在本轮也慢，r6 在相同帧区间的逻辑和长停顿指标则更差；这支持“当前候选不合格”，没有提供可采用的收益证据。
此前原版 4× 约 59.7 Hz 的较稳记录 [E6] 和后续严重异常 / deadline 记录都保留，不挑选其中一段来证明全部原因。[E6] [E19] [E38] [E67]

离线脚本只通过 AST 加载现有 `distribution` / `summarize` 函数；不启动浏览器、不改原始报告。
它保留各端 `initialFrame=59` 及全部 raw rows，仅在内存中的副本把 `targetFrame` 设为 539，
让已有上界规则选到 527；下界仍为 179，按首次 high-water 跨越保留中间每个 callback，排除下界锚点已结束的工作。
它还根据两份原始报告与收据，重新断言初始 / 最终完整 44 词、确认音频、RTC、runner、身份完整性和自有子进程关闭状态。[E67] [E68]

**匹配窗口只改变基于 rows 重算的窗口统计。** Sidecar 中的 `rollbackCount`、`resimulatedFrames`，
以及 `touchMotion` 大部分字段仍来自各原始报告的完整生命周期，不能拿它们除以 348 或当作窗口内事件数。
触控只有明确命名为 `recordedWindowSubmittedEvents` / `recordedWindowSubmittedSpanMs` 的字段随窗口重算；
上表没有混入那些全程账目。最终 canonical gate 也来自各场原始尾端，而非凭离线裁窗生成了一个新的 frame 527 状态快照。[E67] [E68]

本轮执行到此结束扩测，保留原始报告、runner、日志、冻结字节和离线方法。
后续工作仍是第 10.13 节列出的普通帧成本方向、Restart 完整状态图核对，以及能区分共同条件的验证设计；
固定 80 B VBO 驻留、精确 GL 状态缓存和音频独立验证没有被写成已实现或已获得性能收益。

### 10.19 r7：固定 80 B Model VBO、分段计时与冻结身份

本节起是 r6 结束之后的新增工作，不修改第 10.18 节当时“固定 VBO 尚未实现”的历史含义。

r7 正式运行时于 13:43:19 UTC 冻结，fixture 运行时于 13:45:16 UTC 冻结。两者 common 均为 `77666259b4bb14b965a34a1f38a4e2ec88c289ac`；正式清单 552 个源码、fixture 清单 553 个源码均在冻结时逐文件核对。fixture 只用于需要测试导出的验证，不能冒充正式包。[E69] [E70]

| 用途 | WASM SHA-256 | 身份边界 |
| --- | --- | --- |
| 正式 4× 对照与 profile | `82232288a0c4f3045c885053c4a34da5faecba87b930e1fccc5392f1016af12f` | `diagnostic=false`；同一 WASM 内切换 VBO |
| 1× 冻结世界像素 oracle | `ff4c3b123513af14058726b3dd1dfc03ffc9ee16da8473a2c707320392da19ce` | `diagnostic=true`；包含 fixture Draw 入口 |

实际改动位于 [portable/sdl/Renderer.cpp](../portable/sdl/Renderer.cpp) 和 [Renderer.hpp](../portable/sdl/Renderer.hpp)。TH10 multiplayer 的 WorldUv、20 字节 stride、无索引、两三角 strip、80 字节顶点形状可以使用 renderer 自有 VBO。首次或全部 80 字节比较不同才上传；一致则复用同一份 GPU 数据。它按已发出的批次计数，不把模型实例数当上传次数。

原有矩阵、雾、深度、混合、UV、实例矩阵、顺序与 draw 调用保留。VAO key 纳入实际 vertex-buffer ID；命中路径仍绑定正确 ARRAY_BUFFER，避免实例缓冲绑定后新 VAO 取错数据。开关切换先 flush，默认 `modelQuadResidency=false`。它是严格条件下减少重复上传的实现，不是删掉 Draw 或改变世界逻辑。

[ApplicationHost.cpp][S2] 增加默认关闭的分段计时：网络准备、原生输入与应用 step、flush/presentation、audio pump、loading、Replay seek，以及 callback-local offered/completed、单步最大耗时和累计 rollback/resimulation。测量行在 runtimeFinish 前发布，默认关闭时不因这些观测额外调用时钟。已有帧准入和事务边界没有因计时而改变。

这些分段是经过时间观测，不是可抢占的硬上限；单笔世界事务仍可能超过预算。新增 renderer/audio counter delta 覆盖完整 live-loop，包括 warmup 和 tail，和裁剪后的 callback timing 窗口不是同一分母。[E75] [E77]

### 10.20 r7 的 1× 像素 oracle：上传复用与状态正确性成立

13:47:49–13:48:25 UTC 的 fixture 运行先在确认的冻结世界执行 alpha 0、0.25、0.5、0.75、1；每个 alpha 按 OFF / ON / ON / OFF / ON 顺序 Draw，共 25 次。[E71] [E72]

逐项读回原始报告确认：

- 25 次 `counterInvariant=true`；
- 25 次完整 canonical 差异列表为空；
- 25 次实际 native presentation 增量为 1，排除反复读取旧 framebuffer 冒充相等；
- 25 次 RGBA8 全缓冲 `differingBytes=0`；
- GL errors、wrapper errors 与 failures 均为空，开关和 wrappers 已还原；
- oracle 前、恢复后和返回时的完整 44 字 canonical 一致，native time 前后相同。

启用组共遇到 135 个 eligible batch，只上传 1 次、命中 134 次，上传 80 B、复用 10720 B。实际观测到普通 model draw 形状 OFF / ON 为 10 / 15 次，实例化形状为 80 / 120 次；两条原生形状均覆盖。[E71]

该 oracle 使用同步 Draw、立即 readPixels 和短期 GL 包装，证明的是给定冻结世界的像素与状态等价。它的 Draw 时间不是原生 rAF 性能证据，不能用来承诺 4× 改善。

后续同场 1× live loop 的逻辑为 59.864 / 59.953 Hz，P0 ON、P1 OFF 的 native work / 新帧分别为 10.821 / 3.051 ms。P0 先执行了带观测的 oracle，两端成本明显不对称；这不是能证明开关因果收益的对照。初始/最终两端完整 canonical 与确认音频均一致，runner 完成并关闭自有子进程。[E71] [E72]

### 10.21 r7 的 4×：必须使用紧邻重复组，不能跨半小时计算收益

三场均使用冻结正式 r7 WASM、两个独立 headed Chrome、硬件渲染、480 帧、50 ± 5 ms 单程延迟注入、双端 CDP 4×、桌面合成 120 Hz 触控目标、原生高刷展示与相同分段计时。运行由根代理串行完成，没有手机测试。ON / OFF 仅改变相同 WASM 内的 Model quad residency 开关。[E73] [E75] [E77]

| 测量窗指标，P0 / P1 | 较早 OFF，13:51:30–13:55:01 UTC | ON，14:23:31–14:24:00 UTC | 紧邻 OFF 重复，14:25:20–14:25:59 UTC |
| --- | ---: | ---: | ---: |
| 逻辑 Hz | 5.738 / 5.736 | 50.341 / 51.099 | 52.303 / 52.475 |
| Present Hz | 5.664 / 15.611 | 49.876 / 140.721 | 51.074 / 140.930 |
| native work / 新帧，ms | 102.769 / 103.449 | 16.738 / 13.668 | 16.069 / 13.424 |
| native callback p99，ms | 307.7 / 273.3 | 28.5 / 15.2 | 26.4 / 14.7 |
| native callback max，ms | 682.3 / 925.6 | 31.2 / 17.8 | 28.3 / 19.3 |
| 新前沿推进间隔 max，ms | 4749.0 / 4781.2 | 97.5 / 48.0 | 65.2 / 53.4 |
| 推进间隔 >100 ms 次数 | 178 / 172 | 0 / 0 | 0 / 0 |
| AudioPump 窗口总时间，ms | 4954.7 / 5443.2 | 142.1 / 72.5 | 112.3 / 67.3 |
| desktopCadenceTargetMet | false / false | false / false | false / false |

窗口沿用原 checker 的首次 high-water 越界规则，保留中间全部回调。ON P0 的下界首次观测是 frame 180，因此计 347 个新帧；其余上表各端是 frame 179→527、348 个新帧。这些是真实运行窗口，输入交付时机及游戏轨迹并未逐帧锁死，不能当作严格相同世界状态微基准。

较早 OFF 的极端尾部没有在半小时后的紧邻 ON / OFF 重复对照中复现。不能把 5.7 Hz 与 50 Hz 相除来宣称约九倍提升，也不能因为两次发生时间相隔较远就替具体异常指定环境原因。本组异常不归因于用户此前的断网反馈，旧报告完整保留。

紧邻 OFF 的逻辑和每新帧工作量稍好于 ON，尾部各有波动。**目前可以承诺实现减少了符合条件的上传，不能承诺提升整作帧率、降低回滚总成本或已经解决 4× 流畅度。VBO 继续默认关闭。**

#### 上传减少的独立证据

以下 renderer counter delta 来自完整 live-loop，不与上表裁剪后的计时窗口混算：

| Model quad 计数，P0 / P1 | ON | 紧邻 OFF 重复 |
| --- | ---: | ---: |
| eligible batch | 5378 / 15151 | 5732 / 15056 |
| 实际上传次数 | 1 / 1 | 5732 / 15056 |
| 复用命中次数 | 5377 / 15150 | 0 / 0 |
| 上传字节 | 80 / 80 | 458560 / 1204480 |
| 复用字节 | 430160 / 1212000 | 0 / 0 |
| 驻留字节 / 端 | 80 / 80 | 0 / 0 |

batch 数量不同不影响“每端仅上传一次”的事实，但不能把跨运行计数差异解释为同一轨迹下的精确收益百分比。[E75] [E77]

触控目标是 120 Hz，实际 timer 交付仍受主线程影响。ON 的完整 producer-lifetime 提交约 35.19 / 80.90 Hz，OFF 重复约 36.52 / 81.17 Hz；其余位置目标按现有规则合并到最新位置。不能把目标 120 Hz 读作每端实际持续提交了 120 个输入事件/秒，也不能把它推广成 managed Launcher 或手机触控验收。

#### PASS 的具体范围

本次文档核对重新读取三份报告的初始/最终 comparison：各端均为完整 44 字 canonical，双端逐字相同；确认音频均为 9 字，双端相同。各 runner 的 checker exit code 为 0、身份完整性完成、自有 relay/server/checker 均记录 closed，运行已经结束。[E73] [E74] [E75] [E76] [E77] [E78]

checker 的 PASS 是运行、实际 RTC 路由、状态和确认音频门槛，不是 cadence 通过。当前 fresh 4× 仍只有约 50–52 Hz；P1 的高 Present Hz 也不能证明最终对象运动连续或输入到光子足够快。这里的实际 RTC 是本地 loopback 两个浏览器，未建立公网直连、独立远端 relay、WAN 抖动或真实移动设备结论。

### 10.22 r7 新 profile：先校正符号，再选择热点

14:27:59–14:28:27 UTC 对相同正式 r7 WASM、VBO OFF 另开串行 CPU profile。其测量窗逻辑为 51.367 / 51.521 Hz，native work / 新帧为 16.532 / 13.409 ms；初始/最终完整 44 字、确认音频、RTC、身份和退出清理通过，但双端 cadence 仍 false。[E79] [E80]

[符号化热点报告][E81]的 scope 是整个 P0 profile，包含 warmup/tail 等采样区间，不是只对第 10.21 节裁剪窗口求和。以下为 inclusive，已经包含子调用，**父子项目不能相加，也不能把整张表求和当作总 CPU 时间**：

| 路径 | P0 全 profile inclusive，ms |
| --- | ---: |
| Application::step | 6805.025 |
| UpdateChain::run | 5672.443 |
| AppLoop::update | 4754.629 |
| AnimationEngine::draw_all | 2468.578 |
| AppLoop::draw | 1975.085 |
| Backgrounds::draw | 1904.048 |
| StageRenderer::draw_objects | 1659.270 |
| RollbackState::BeginFrame | 927.896 |
| Camera::configure_world | 352.126 |
| GraphicsCamera::look_at | 226.760 |
| GraphicsCamera::perspective | 69.773 |
| sdl_audio_pump | 181.553 |

例如 Backgrounds → StageRenderer，以及 Camera → look_at/perspective 都存在调用包含关系。上表用于定位应检查的路径，不能将多层 inclusive 相加，或直接据此承诺缓存后的理论加速。

独占 self 样本另显示：`eagler_journal_copy_bytes` 556.414 ms、Extended binary 486.890 ms、`RollbackState::BeginFrame` 369.047 ms、`bufferData` 314.334 ms。日志与帧前捕获仍是实测成本；不能说已经只剩渲染问题。相反，固定 Model quad 只是部分 bufferData，不能把全部 bufferData 都当作该 VBO 可以节省的时间。[E81]

#### 必须保留的 World lambda 符号纠正

[World.cpp](../th10_web/cpp/platform/World.cpp) 的 multiplayer 构造器在所有 callback bind 之前先定义了 `engine.player_view_alpha` lambda。因此该构建的 `$_0` 是 player alpha，之后的 callback 序号相对忽略这段代码的推算多一位。

| 当前构建符号 | 源码对应 | self，ms | inclusive，ms |
| --- | --- | ---: | ---: |
| World 构造器 `$_5` | BulletsUpdate → update_bullets | 186.970 | 186.970 |
| World 构造器 `$_6` | BulletsDraw → draw_bullets | 2.072 | 4.124 |
| World 构造器 `$_9` | ItemsUpdate → update_items | 149.019 | 301.612 |

不能把 `$_5` 的更新成本误写成子弹绘制热点，也不能把 `$_9` 当作 ItemsDraw。该样本的 BulletsDraw inclusive 只有 4.124 ms；这是本场样本的事实，不代表所有高密度弹幕场景的绘制永远便宜。后续构建若增加/删除 lambda，必须重新从相应源码核对，不能跨版本沿用序号。[E81]

### 10.23 r7 音频与 Restart 诊断边界

已补充 [TH10 r7 音频热点只读审计](th10-audio-hotpath-audit-r7.zh-CN.md)，保留源码行锚点、严格的证据范围和两个尚未实施的候选：

- smoke 显式 `sdl_music_enabled(0)`，本组不含真实 OGG 解码性能；
- pump 一次最多 6 × 1024 帧，未发现按墙钟无限补混音；
- 原来的源消费位置和块间音乐通知是正确性修复，不应先删掉；
- SDL 另有主线程 ScriptProcessor 输出回调，其设备重采样和 JS 声道复制不计入 rAF 尾部 AudioPump；
- 按属性更新 controls、保留全部相位/缓存状态的同速重采样快路径只是候选，没有写成已获得的收益。

较早音频长尾不能直接解释成可重复固有成本，也不能因 fresh 样本好转就宣称修好了音频。确认音频摘要相同并不是 PCM 输出相同；真正修改 mixer 时仍须旧/新冻结 WASM 逐样本比较及输出连续性验证。

r7 新增图存储观测导出和 [graph-storage.mjs](../portable/multiplayer/graph-storage.mjs)，用于把 UpdateChain 和相关固定池的原始占用/活动块读取出来，核对完整 canonical 的相关摘要。它没有修改既有 44 字 gate，也没有把指针字简单屏蔽成相等。第 10.14–10.16 节的真实菜单 Restart 完整状态失败仍保留；本节没有新的成功 Restart 运行证据，诊断工具存在不能算问题已解决。

### 10.24 r7 阶段拟定的 r8：精确 camera cache 与 packed occupancy（历史计划）

以下保留 r7 结束时的主线工作安排；当时为**默认关闭、尚待运行验证**，后续验证记录另见第 10.25–10.28 节。r7 的冻结 WASM 和全部以上成绩不包含这些修改，不能将源代码已经出现当作 r8 正确性或性能通过。

| 试验 | 针对的已观测路径 | 需要保留的边界 |
| --- | --- | --- |
| 精确 camera math cache | configure_world、look_at、perspective 中重复的精确数值计算 | 保留输入位模式、原运算结果、数值环境及必要副作用；不改变矩阵提交、绘制顺序或世界状态；开关默认关闭 |
| packed occupancy scan | BeginFrame / pool capture 的占用扫描 | 仅优化定位活动槽的方式；保留槽顺序、实际活动集合、完整 journal Touch 字节和新增/释放/复用覆盖；开关默认关闭 |

当前工作区可见 `CameraMathCache` 的 `enabled_=false`，以及 `RollbackState.cpp` 的 `packed_occupancy_scan=false`。这只是候选状态；在默认关闭等价、逐项正确性、同冻结 WASM 的紧邻开启/关闭运行及最终关闭观测复核之前，不作整作性能承诺。

接下来仍先由根代理串行完成当前 profile 指向的热点实现和运行验证。即使新候选改善每帧成本，真实菜单 Restart 的完整状态差异仍须解决，高刷最终运动连续性也须独立验证。当前桌面合成触控不构成 managed Launcher 或手机实机验收。正常暂停引发的另一端等待不是要消除的错误；正常游玩中的主线程长停顿仍是目标问题。

### 10.25 r8：精确 camera math 与 packed 扫描的冻结身份

本轮把第 10.24 节两项候选实现收敛到可切换、默认关闭的构建：camera math 复用精确矩阵计算，packed occupancy 改变活动槽的扫描方式。它们与 r7 的 Model quad VBO 是不同开关。冻结清单中的相关源码包括 `Graphics.hpp`、`CameraMathCache.hpp`、`RollbackState.cpp/.hpp`、`RollbackPoolCapture.hpp` 及 common 的 `SparsePoolCapture.hpp`；不能把这次改动描述成删掉完整 journal 捕获。[E82]

| 项目 | 正式 4× 运行时 | 定向 oracle fixture |
| --- | --- | --- |
| 冻结时间，UTC | 2026-10-12 14:57:59 | 2026-10-12 14:58:23 |
| profile / diagnostic | `multiplayer / false` | `multiplayer-fixtures / true` |
| WASM SHA-256 | `30fc5b7eb3c3fb6e9b3c1fd9e5c048f11eeaecb712d5e2971d11fc631727daeb` | `edcf8baf508dd523b2d78466020be10687a9c84a8840373c120998ac86846f42` |
| sourceDigest | `46bbe7e49551ad5f3607524259d1b5f0a5069146193d98258929f49241327089` | `b2ab120a992d8541942a0376de6353bf319ccdc2ab95aca8bc6cf828378a1c0d` |
| 冻结源码数 | 553 | 554 |
| 清单逐文件核对 | true | true |

两份冻结清单都记录同一个 HEAD / common HEAD，列出的 10 项 changed-source copy 的哈希逐项相同。fixture 多一个诊断源码，包含测试所需入口；其浏览器运行不能标成正式 release 性能。冻结清单列出的默认值均为：Model quad residency=false、camera math cache=false、camera verifyEvery=0、packed occupancy=false、callback timing=false。[E82] [E83]

本轮没有把这些实验开关改成面向玩家的默认开启；也没有因出现高命中率就绕过完整状态或像素核对。构建清单记录的范围仍为本地隔离实验，未 push、merge 或部署。[E82] [E83]

### 10.26 r8 定向正确性：字节、像素、非对称 live loop 分别核对

#### Camera 数值单测与 capture 原生单测

此前待归档的 unit 证据已在 **2026-10-12 15:18:23 UTC** 落盘，本文现已完整读回 [native-validation.json][E92] 与 [camera stdout][E93]。两条命令由根代理通过 nig 执行，回执中 exitCode 均为 0；不再仅依赖执行口头反馈。

| 原生单测命令 | nig command ID | 结果 |
| --- | --- | --- |
| `--suite camera-math-cache` | `hXoLylLAGpq436muctrLrPIp` | 4804 次比较；2166 次 flags 转换；0 次 errno 转换；24 种 arithmetic modes；每函数 4 个 cache slot |
| `--suite common-sparse-pool --suite rollback-pool-capture` | `_y-qb2DD1tzxtOnzZqjXrs1x` | 两组 PASS；common 包括 800 次随机 rewinds，具体覆盖见下表 |

camera stdout 的这些数值与回执逐项一致。0 次 errno 转换是该样本的观察，不等于可以忽略 errno 语义。回执保存三份 test WASM、相关源码 / runner 与日志的 SHA-256；测试 WASM 是单测产物，不是浏览器正式运行时：

| test WASM | SHA-256 |
| --- | --- |
| `camera-math-cache.wasm` | `026d41cea986f6b6ca91c8544d995608cdf76e8cc30ebd90e4d5fd3913ff18ed` |
| `common-sparse-pool.wasm` | `9732053fe77cfddaada4517d6f4e976d1560ce07fb60f063279fd6c757a65142` |
| `rollback-pool-capture.wasm` | `624fb8fb90e7595d1708c216b31d20a8cd6ec1b17aeaa9d6ffda987a18a8488c` |

归档注明源码在执行至归档期间保持不变，源码哈希是在归档时核对。本文另核对了其中 `CameraMathCache.hpp`、`SparsePoolCapture.hpp`、`RollbackPoolCapture.hpp` 三个相关源码哈希，均与正式 r8 冻结清单相同；回执关联的正式 WASM SHA 也一致。其证据范围是 WASI 原生单测，仍不是整作性能或 PCM 验证。[E82] [E92] [E93]

已完整读回 `capture-native-tests.stdout.log`，其中两组 wasm32 编译 / 运行均记录 PASS：[E84]

| 单测 | stdout 明确记录的覆盖 |
| --- | --- |
| common sparse pool | scalar / packed 有序 run 等价；所有字节对齐和非零占用值；稠密 / 碎片化字节；800 次随机 rewinds；槽复用；失败 / 重试 |
| TH10 rollback pool capture | scalar / packed 有序 run 和重试；payload=37、stride=48；稠密 / 稀疏填充；休眠槽首次写入；复用；12 帧 undo；ring / continuation；elision 与失败 |

这些测试约束“扫描快路径仍捕获相同对象与字节，并能正确撤销”的语义。它们不能单独证明所有整作状态、性能或真实菜单 Restart 已通过，也没有把不同 payload / stride 的填充区域当成可任意省略的字节。

#### 冻结世界 30 次真实 Draw 的 camera oracle

15:01:41–15:02:08 UTC 的 fixture 运行在确认且停止推进的世界上，对 alpha **0、0.25、0.5、0.75、1** 各执行以下六步，总共 **30 次 Draw**：[E85] [E86]

| phase | cache | verifyEvery | 目的 |
| --- | --- | ---: | --- |
| 0 | OFF | 0 | 取得当前 alpha 的原路径像素 |
| 1 | ON | 0 | 实际无重算校验的缓存路径 |
| 2 | ON | 1 | 命中后重算并逐项比较 |
| 3 | ON | 0 | 核验后再走实际快路径 |
| 4 | OFF | 0 | 同一 alpha 再次复核原路径 |
| 5 | ON | 0 | 再次复用缓存 |

本次文档核对读全 oracle 子对象，并逐个检查全部 30 条 sample：

- 每条都实际提交 1 次 native presentation；
- 每条完整 44 字 canonical 前后相等；
- 每条 640 × 480 RGBA8 framebuffer 的 `pixelComparison.equal=true`、`differingBytes=0`；
- oracle 总 failures 为空；verify1 的 look-at / perspective 分别验证 **110 / 110** 次命中，未发现 mismatch；
- verify0 下仍分别产生 **316 / 326** 次有效命中，因此像素相等不是仅覆盖每次重新计算的验证分支；
- oracle 前后完整 canonical 相等，原生时钟都是 **2.1333333333333306**，结束后 cache / verify 开关恢复原值。

该结果证明给定冻结世界和五个 alpha 下的原路径、核验路径、实际快路径像素与世界状态等价。它没有推进逻辑时钟，不是每秒 Draw 吞吐实验，也没有覆盖所有关卡、摄像机状态或最终运动连续性。

#### 单端开启、另一端关闭的真实 1× 回滚

同场随后进入实际 native rAF / RTC live loop：P0 开启 camera cache 与 packed 扫描，`verifyEvery=1`；P1 两项关闭。初始 comparison 在本轮开关设置前取样，最终 comparison 在 live loop 后取样；已分别重新读回两端的 **44 字 canonical 和 9 字确认音频**，初始与最终均逐字相等。[E85] [E86]

完整 live-loop counter delta（含 warmup / tail）中，P0 的 look-at **64092** 次命中全部核验，perspective **72517** 次命中全部核验，mismatch 均为 0；P1 全部走关闭的 bypass 路径。其非对称开关比两端同时开启更能暴露“双方以同一种错误方式前进”的部分风险；它仍不是所有状态的穷举证明。

本场裁剪计时窗逻辑 **60.029 / 60.066 Hz**，但 `desktopCadenceTargetMet` 仍为 **false / false**。不能只因平均逻辑接近 60 Hz 就把这场写成全部 cadence 门槛通过；逐命中验证和 fixture 性质也使它不适合估计正式 4× 的收益。

### 10.27 r8 紧邻正式 4×：高命中率尚未转化成整作收益

正式 OFF 于 **15:04:57.739–15:05:25.780 UTC** 运行，ON 紧接于 **15:05:25.977–15:05:53.252 UTC** 运行，前一场退出至后一场启动相隔 **197 ms**。两者使用同一个正式 WASM、两个独立 headed Chrome、硬件渲染、480 帧、双端已确认的 CDP 4×、50 ± 5 ms 单程应用 RTC 发送延迟注入、120 Hz 桌面合成触控目标、原生高刷展示及 0 帧额外输入延迟。[E88] [E89] [E90] [E91]

两组 VBO 都 OFF、callback timing 都 OFF、camera verifyEvery 都为 0。OFF 将 camera / packed 同时关闭；ON 将两项同时开启。因而这是两项合起来的整体候选对照，**不能拆成某一项单独造成收益或回退的因果证据**。三场精简结果及各原始 JSON 的 SHA 集中记录在 [r8 紧邻汇总][E87]；本文的数值已另与两份 raw summary 逐项核对。

| 裁剪计时窗指标，P0 / P1 | OFF | ON |
| --- | ---: | ---: |
| 逻辑 Hz | 55.281 / 55.235 | 55.065 / 55.094 |
| Present Hz | 67.150 / 145.291 | 68.085 / 141.762 |
| native work / 新帧，ms | 15.104 / 12.635 | 14.916 / 12.618 |
| native callback p99，ms | 25.8 / 13.9 | 23.5 / 14.4 |
| native callback max，ms | 57.0 / 54.3 | 30.9 / 18.2 |
| 新前沿推进间隔 p99，ms | 53.4 / 39.8 | 56.1 / 43.4 |
| 新前沿推进间隔 max，ms | 82.6 / 67.8 | 79.6 / 66.7 |
| present gap max，ms | 72.4 / 54.9 | 59.9 / 53.8 |
| 推进间隔 >100 ms 次数 | 0 / 0 | 0 / 0 |
| desktopCadenceTargetMet | false / false | false / false |

计时窗依旧按首次 high-water 越界取样并保留中间全部回调。OFF P0 实际取 frame **180→528**；其余三端为 **179→527**，每端都是 **348 个新增前沿帧**。native work / 新帧是该窗口原生 callback 工作累计时间除以新增前沿帧，包含窗口中回滚、恢复和展示回调，不能读成一次无回滚逻辑 step 的固定成本。[E88] [E90]

ON 的每新帧累计工作略低，逻辑 Hz 也略低。native callback max 在这次 ON 降低，但推进间隔 p99 两端都略高；尾部和调度并非所有方向一致改善。**单次 max 降低不能承诺没有大卡顿；两组没有 >100 ms 推进间隔，也只能描述这两个短窗口。** 两者约 55 Hz 且 cadence 均未过，因此当前不能承诺整作帧率收益、4× 达标或玩家体验已经等同 TH08 / TH09。

#### 缓存做了真实工作，但计数分母与性能窗口不同

以下为完整 live-loop 的 camera counter delta，包含 warmup / tail；不是上表裁剪窗口内的计数。关闭组 look-at / perspective 全部 bypass。[E88] [E90]

| ON 计数 | P0 | P1 |
| --- | ---: | ---: |
| look-at calls / hits | 48603 / 43247 | 59811 / 53182 |
| look-at 命中率 | 88.980% | 88.917% |
| perspective calls / hits | 48603 / 48595 | 59811 / 59803 |
| perspective 命中率 | 99.984% | 99.987% |
| verifyEvery / 实际核验次数 | 0 / 0 | 0 / 0 |
| packed occupancy | ON | ON |

4× 中的 `mismatches=0` 不能单独充当相机正确性证明，因为本场核验次数为 0；必须结合上一节 verify1 的 110 / 110 oracle 和 64092 / 72517 live-loop 核验。高命中率说明缓存被真实使用，不说明查找 / 键比较 / 矩阵提交等剩余成本足够小，也不说明 camera 是支配全帧的唯一热点。

#### 触控负载与正确性边界

120 Hz 是位置目标产生频率。实际完整 producer lifetime 的输入提交频率，OFF 为 **42.761 / 76.501 Hz**，ON 为 **47.403 / 79.190 Hz**；部分中间位置按已有规则合并。该 counter 的生命周期也不同于裁剪计时窗。两场受墙钟调度影响的触控交付时机并不相同，不能当作逐帧完全相同输入的确定性重放；ON 提交频率的单次变化也不能直接承诺输入延迟改善。[E88] [E90]

两组 raw 初始/最终完整 44 字 canonical 与 9 字确认音频均已重新逐端读回并比较相等。这里比较的是**同一场的两个 peer**，不是宣称 OFF 场与 ON 场的最终世界或音频摘要彼此相同。两份 runner 都 `passed=true`、checker exit 0、结束时身份完整性 true，自有 relay / server / checker 全部 closed。[E89] [E91]

### 10.28 r8 profile 已归档：日志热点与下一步

**当前可确认的是候选实现、定向等价性和真实回滚下的非对称状态一致性；整作流畅度收益仍没有充分证据。** 因此 VBO 驻留、camera cache、packed 扫描全部保持默认关闭，camera verifyEvery 为 0，分段计时默认关闭。验证入口继续保留，方便把之后的新候选与本次冻结字节作有边界的对照。[E82] [E87]

#### 本场 profile 的身份、通过项与测量口径

r8 正式 WASM 的 profile 于 **15:10:04.322–15:10:31.067 UTC** 串行运行。使用双端 4×、480 帧、50 ± 5 ms 单程应用 RTC 延迟、120 Hz 合成触控目标；camera / packed ON、VBO OFF、verifyEvery=0。此场**额外开启 callback timing 和 CPU profiling**，与第 10.27 节未开这些观测的紧邻 OFF / ON 是不同运行。[E94] [E95]

已另读回此场 raw 初始 / 最终 comparison：两端完整 44 字 canonical 和 9 字确认音频均相同；runner 的 checker exit 0、结束时身份完整性 true，自有 relay / server / checker 全部 closed。正式 WASM、loader 与 sourceDigest 均匹配第 10.25 节冻结身份。[E94] [E95]

| profile 场裁剪计时窗，P0 / P1 | 结果 |
| --- | ---: |
| 新增前沿帧 | 348 / 348 |
| 逻辑 Hz | 56.499 / 56.596 |
| native work / 新帧，ms | 14.547 / 12.168 |
| native callback max，ms | 27.3 / 15.9 |
| 新前沿推进间隔 max，ms | 58.0 / 47.9 |
| desktopCadenceTargetMet | false / false |

这场仍未通过 cadence，不能因数值略高于前一对照场就宣布进一步加速。它的触控交付和回滚轨迹来自另一次墙钟运行，且增加了观测开销，不能与 r7 或前一场直接相减得出候选因果收益。

#### 全 P0 采样仍指向日志复制和捕获

[符号化热点][E96]已完整读回，记录 **9484 个 samples**、按 `timeDeltas` 加权 **9993.640 ms**。范围是**整个 P0 CDP profile**，包含 profiler 启动 / 尾段，也包含 idle / program 样本，和上表只覆盖 frame 179→527 的裁剪 timing 窗口不同。该文件关联原始 performance JSON、CPU profile、符号表与冻结 WASM 的 SHA。

| 全 P0 profile 路径 | self，ms | inclusive，ms |
| --- | ---: | ---: |
| native `frame` callback | 22.412 | 7120.041 |
| `RollbackState::BeginFrame` | 233.843 | 807.251 |
| `RollbackJournal::Touch` | 176.345 | 574.448 |
| `eagler_journal_copy_bytes` | 537.272 | 537.272 |
| camera wrapper `GraphicsCamera::look_at` | 20.605 | 66.947 |
| 原运算 `GraphicsMath::look_at` | 4.363 | 44.269 |
| camera wrapper `GraphicsCamera::perspective` | 13.474 | 13.474 |

inclusive 已含子调用，表中的 `frame`、BeginFrame / Touch / 复制，以及 look-at wrapper / 原运算存在调用包含关系。**不得把父子行相加，更不能把 inclusive 总和当成总 CPU 时间。** self 与 inclusive 是采样归属，不是逐调用精密计时或所有未来场景的固定成本。

相机 wrapper 的采样成本可见，但不能拿 r7 异场 profile 的 look-at 数值与本场相减来声称缓存节省了某个确定百分比。第 10.27 节同 WASM 紧邻 OFF / ON 没有整作收益证据的判断不因此改变。World 构造器 lambda 的来源核对和序号风险继续见第 10.22 节；不能重新把 `$_5` 误标成 BulletsDraw。[E81] [E96]

#### 复制 helper 537.272 ms 不是已经隔离出的 crossing 成本

当前 [RollbackJournal.cpp](../third_party/eagler-common/src/netplay/RollbackJournal.cpp#L7) 的 `CopyBytes` 在 Emscripten wasm32、fastBulkCopy 开启且 size ≥ 1024 时调用该 EM_JS helper。helper 内部检查 WASM 内存范围，并执行 `HEAPU8.copyWithin`，因此包含真正的数据搬运。每次重新读取 HEAPU8 也服务于内存增长后的正确视图；它不是一个只做空调用的桥接函数。

**537.272 ms 是归于整个 helper 的采样耗时，不能全部标作 WASM / JS crossing 开销，也不能认定删掉 crossing 就能全部收回。** 本 profile 没有把边界检查、调用边界、实际复制、调用规模和复制次数分别隔离；其他 `wasm-to-js` 采样又可能包含调用树后代，不能把它与 helper 数值相加来计算额外收益。[E96]

这份 profile 证明日志复制 / 捕获仍值得继续分析，没有证明新复制方案已经更快。下一步由根代理继续核查实际复制路径和负载，保留完整 Touch 字节、活动槽顺序、恢复语义、失败重试及内存增长边界，再以可核验候选验证是否减少了实际工作。现有归档不包含已经完成并通过性能验证的新 journal copy 优化。

这三场均为本地 loopback 的真实 RTC 路由；发送层注入的 50 ± 5 ms 延迟包含可靠输入修复，但不是实测公网 wire RTT。它们未覆盖公网 direct、独立远端中继、WAN 断网恢复或 managed Launcher。这里不以用户此前断网解释单次旧性能样本，也不把正常暂停导致另一端自然等待当成错误。[E85] [E86] [E88] [E89] [E90] [E91]

[smoke.mjs](../portable/multiplayer/smoke.mjs) 仍显式 `sdl_music_enabled(0)`。44 / audio PASS 证明本次确认事件摘要一致，未测真实 BGM 解码、PCM 输出等价和设备输出连续性；具体泵上限、SDL 独立回调及候选改动风险继续以 [r7 音频审计](th10-audio-hotpath-audit-r7.zh-CN.md) 为准。本轮没有修改音频策略并取得改善的证据。

最后仍须独立解决真实原生菜单 Restart 的完整 canonical 历史失败，并测量最终像素运动连续性与输入到光子。现有高刷提交标志、较高的 Present Hz、短窗口零 >100 ms 事件，都不能替代这些验收；TH10 尚不能宣称已达到 TH08 / TH09 的成功体验。

### 10.29 r9：初始捕获批次与稳定 hint owner 的实现、冻结身份

r9 接续第 10.28 节的日志热点分析，新增两项性质不同的修改：**初始日志捕获批次是默认关闭的性能试验；稳定 hint manager owner 是没有开关的生命周期修复。** 本节只记录已读回的实现和构建证据，不将 r8 的 537.272 ms helper self 采样全部当成批次可收回的成本，也不预设整作加速幅度。[E96] [E100]

#### 初始捕获批次：保留全部日志字节，只合并同步只读窗口中的大块复制

[RollbackState::BeginFrame](../th10_web/cpp/multiplayer/RollbackState.cpp) 把原有初始枚举包在同步 lambda 中，开启试验时调用 common 的 `BeginReadOnlyCapture()` / `EndReadOnlyCapture()`。该窗口只读取游戏源存储；在它关闭前不得推进 Update、semantic Draw 或其他会改写源字节的工作。开关仅在 Emscripten wasm32 上接受开启，默认值为 false。[E100]

[RollbackJournal](../third_party/eagler-common/src/netplay/RollbackJournal.cpp) 继续按原 Touch 顺序登记地址、大小和目标 arena offset，保留去重、重叠拒绝、byte / block 预算及 undo 顺序。窗口内小于 1024 B 的块立即复制；大块先登记，关闭窗口或 arena 扩容前统一 drain。正式 Emscripten 快路径复用既有 Block 数组，一次 import 遍历本段待复制记录，检查当前 WASM 内存范围后执行 `HEAPU8.copyWithin`；不另造一份全世界快照，也不省略 accepted payload。窗口外的首写 Touch 仍立即保存。[E97] [E98]

所有普通枚举失败均先离开 lambda，再独立计算 `drained = !batched || journal_.EndReadOnlyCapture()`，最后返回 `captured && drained`。因此 captured=false 不会短路 drain。common 的 EndReadOnlyCapture 也先复制已接受前缀、关闭 scope，再返回 failed 状态；后续 Touch 的拒绝和 arena relocation 都不能把此前接受的字节丢掉。定向测试检查了这一合同，实际浏览器 import 的运行结果仍待单独收回。[E97] [E98]

[SessionExports](../th10_web/cpp/multiplayer/SessionExports.cpp) 新增 14 个 double 状态字：schema、enabled、supported、scopeOpen、pendingBlocks、pendingBytes，以及 scopes、deferredBlocks / Bytes、copiedBlocks / Bytes、flushes、batchImports、maxBlocksPerFlush。[initial-capture-batch.mjs](../portable/multiplayer/initial-capture-batch.mjs) 检查安全整数、字段布局、窗口关闭、pending 清零和 accepted / copied 相等。performance-driver 仅在 initial / final 读取这些计数，未把新导出放进每个 native callback 的采样热循环。[S13] [S19]

#### 稳定 hint owner：同一 World 内固定地址，完整记录指针与槽位

[HintOwnerPool.hpp](../th10_web/cpp/multiplayer/HintOwnerPool.hpp) 定义 `RollbackPool<sizeof(StageHints), 1>`，由 World 持有。[WorldHints.cpp](../th10_web/cpp/platform/WorldHints.cpp) 在多人模式下把 StageHints manager 的分配 / 释放接入这一槽位；StageHints 的 initialize / start / shutdown 和实际链表、callback 绑定继续执行。复用 retired manager 前先 Touch 完整 Storage，StageHint node 初始化 / 复用也保留其首写捕获。

RollbackState 同时保存 `world.actors.hints` 指针、owner 池 occupied / cursor 和 active slot。`hint_owner_capture_` 以真实 Storage stride 跟踪覆盖；后续 `Touch(manager, sizeof(StageHints))` 能识别 payload / stride 并复用已捕获槽，避免把一个已保存的对齐块又当作部分重叠的新记录。测试环境的 manager payload 为 **424 B**、Storage stride 为 **432 B**，不是只保存 424 B 而忽略尾部填充。[E97] [E98]

**16 个 self sentinel、callback owner 与 NativeCallback context 必须分开解释。** `StageHints::initialize` 给 `pending[8]` 和 `retained[8]` 的 `ListNode<StageHint>::value` 写入 `reinterpret_cast<StageHint*>(this)`：这 16 个值是 manager 的合法自指针，不是 callback context。`StageHints::start` 另外把同一个 manager 作为两个 `UpdateChainEntry::owner` 传入；NativeCallback 的 context 则由 callback resolve / bind 路径提供，调用时与 owner 分别传参。真实 UpdateChain 单测明确检查 owner 指向 manager、context 指向测试的 UpdateChainEnvironment 且不等于 owner，并实际调用函数。其 **callback_stride=64 B** 是 callback 池的步长，与 manager 的 432 B 不同。[S29] [S30] [E98]；结构定义见 [UpdateChain.hpp](../th10_web/cpp/game/UpdateChain.hpp)。

这一修改针对同一 World 中销毁 / 重建后 manager 地址漂移的来源；它不把自指针抹零，不屏蔽 raw canonical 的指针字段。单测里的稳定地址与正确链表不能代替跨浏览器完整状态门禁，历史 Restart 的 word 10 / 11 失败仍须用新的正式运行验证。

#### 已冻结的正式身份

根代理串行运行 `portable/build.mjs --multiplayer`，回执时间为 **15:45:49.810–15:47:18.745 UTC**，exitCode=0；随后在 **15:47:19.258 UTC** 冻结正式产物。以下值来自已读回的构建回执、冻结清单和 build.json：[E100] [E101] [E102]

| 项目 | r9 正式 multiplayer |
| --- | --- |
| profile / diagnostic | `multiplayer / false` |
| HEAD / common HEAD | `2e5d5464b2b28ef3fb936234660b773b604c91ce` / `77666259b4bb14b965a34a1f38a4e2ec88c289ac`，均须连同当前增量和冻结文件哈希识别 |
| WASM SHA-256 | `7a8bc51bd01c2608cd44555b164a6226c348bf265cc1d9bd3f0000b54b8f49bb` |
| WASM 大小 | 3,131,249 B |
| sourceDigest | `c4cab25c2132dbbbd63b45ee1691ed8c3c94759e201e050a173a4610721973fe` |
| 冻结源码数 / 核验标记 | 554 / `currentSourceInventoryVerified=true` |
| 运行时目录 | `artifacts/rollback-smoothness/candidate-r9/runtime` |
| 默认批次 / 稳定 owner 池 / graph schema | OFF / true / 2 |
| 既有试验默认值 | VBO、camera cache、packed scan、callback timing 均 OFF；camera verifyEvery=0 |

冻结清单中的 common journal 头 / 实现、HintOwnerPool 和 RollbackPoolCapture 哈希与本轮 native-validation 的对应源码项相同。这使定向测试与该正式候选的相关源码能够对上，但单测 WASM 仍不是浏览器运行时。清单 scope 继续为本地隔离实验，没有 push、merge 或部署证明；正式编译成功也不等于 EM_JS 已在真实回滚中运行通过。[E97] [E100] [E101]

### 10.30 r9 WASI 定向验证：五组通过，保留负对照与首写边界

已完整读回 [native-validation.json][E97]、[stdout][E98] 和 [stderr][E99]。根代理本次串行执行从 **2026-10-12 15:44:43.483 UTC** 到 **15:44:59.785 UTC**，exitCode=0、signal=null、error=null；回执记录 `sourcesUnchanged=true`。stdout 逐组记录 compile wasm32、run wasm32 与 PASS；stderr 只有 Node 的 WASI ExperimentalWarning，没有测试失败记录。

| suite | 本次已收回的结果与范围 |
| --- | --- |
| `common-journal-capture-batch` | **4048 checks**。涵盖 1023 / 1024 / 1025 B 边界、非对齐地址、重复 Touch、窗口外立即首写、空 / small-only 窗口、嵌套拒绝、EndFrame / Clear / Reset、扩展 checkpoint、arena 增长、ring / discard / 重用及完整字节 undo；fastBulkCopy、coalesce 和 batch 的适用组合均运行。拒绝后 drain 已接受前缀覆盖 null、零长度、两侧重叠、byte / block 预算和整数地址溢出。 |
| `hint-owner-lifetime` | 真实 StageHints / UpdateChain，manager payload=424、stride=432、callback stride=64；稳定重建地址、16 个 self sentinel、node 前后链接、native context / owner 区分、完整 12 帧 undo、history / generation 重置、容量及错误释放、scalar / packed 与负对照均 PASS。 |
| `common-sparse-pool` | scalar / packed 有序 run、所有字节对齐和非零占用值、稠密 / 碎片化字节、**800 次随机 rewinds**、槽复用及失败 / 重试 PASS。 |
| `rollback-pool-capture` | payload=37、stride=48；scalar / packed 有序 run 和重试、稠密 / 稀疏填充、休眠槽首写、复用、12 帧 undo、ring / continuation、elision 与失败 PASS。 |
| `cooperative-rollback` | TH10 cooperative owner 与 common 迟到输入回滚联动 PASS。 |

batch 的案例与独立完整字节对照见 [rollback-journal-capture-batch-test.cpp](../tests/rollback-journal-capture-batch-test.cpp)；owner 的案例和负对照见 [hint-owner-lifetime-test.cpp](../tests/hint-owner-lifetime-test.cpp)。以上覆盖说明按源码和本次 stdout 共同核对，不将某个通用 pool 测试的 PASS 扩大为所有生产分配路径已有首写 hook。[E97] [E98]

#### manager 首写已验证，dormant callback 首写没有在本轮填补

owner 单测的生命周期循环从两项 callback slot 都已 active 的 checkpoint 开始，完整捕获后再执行销毁、同槽重建与撤销。测试 allocator 保持当前生产 callback 行为，**没有额外发明 dormant callback 首写 hook 来让测试通过**。

独立的 dormant manager 案例只分配并 initialize StageHints，不新建休眠 callback slot：有 manager 首写捕获时完整恢复；故意省略时 owner 字节恢复失败。另一个负对照故意漏掉 `current` / `actors.hints` 指针捕获，即使 manager、nodes、callbacks 字节都恢复，current 仍为 null，整体检查仍失败。这两项分别证明 manager 首写与 actor 指针捕获有必要，不能拿来宣布 dormant callback 首写已经修好。通用 `rollback-pool-capture` 的休眠槽首写结果也不改变这一生产边界。[E98]；具体分支见 [hint-owner-lifetime-test.cpp](../tests/hint-owner-lifetime-test.cpp)。

回执保存五份单测 WASM、相关源码和两份日志的 SHA-256，便于后续固定同一证据。单测 WASM 身份如下，不能混用为正式 Web WASM：[E97]

| test WASM | SHA-256 |
| --- | --- |
| `common-journal-capture-batch.wasm` | `d4176fa13e7e03d6e3babc511c731af17b587284802b5dc1f0b423568a59e96c` |
| `hint-owner-lifetime.wasm` | `22ba55ca79cf9c48de9904fb96e3e2ce2873e76237172a40e486cf5e1cae849c` |
| `common-sparse-pool.wasm` | `c29c709ec2d9b1511ccca5834ffd551f4d7fe14c80dca0f81a24b0f3f343cd21` |
| `rollback-pool-capture.wasm` | `93000e2a30c884ff57bf1e8432f5756ef32ba8b3f0fd9effbf7ceb07e24368a1` |
| `cooperative-rollback.wasm` | `d6d4c22f46600b7d7f3985e7ef9604b35d75335dc16b8cbd0805e6c5cc5e02e2` |

### 10.31 r9 验证边界与下一步：正式 batch、完整重开、同字节 4× 分开验收

截至本节记录点，通过项是上述五组 **wasm32-wasip1 定向测试**及正式构建冻结。WASI suite 不运行 Emscripten 的 EM_JS import 分支；回执 scope 中提到的 host boundary 属于另一路验证，不能由这五组 PASS 推定已经在正式游戏中通过。当前没有收回 r9 的 1× 非对称、4× cadence / 长任务或真实菜单 Restart 成功结果，也没有新增最终像素运动、输入到光子、PCM 或手机验收。[E97] [E100] [E101]

#### graph v2 和完整 canonical 保持严格

[SessionExports](../th10_web/cpp/multiplayer/SessionExports.cpp) 的当前 word 10 保留 scene owner / stage 摘要，并纳入 `rollback_hints` 与 `rollback_hint_owners` 的 occupied、cursor 和 active payload。graph descriptor v2 新增第四个 owner 池，分别报告 payload 大小与 Storage stride。[graph-storage.mjs](../portable/multiplayer/graph-storage.mjs) 仍接受 v1 的三个旧池；按真实 stride 寻址，只按 BlockSize 复制参与 native hash 的 payload，保留原始 occupied 字节和原始指针。

word 11 继续按 chain、callback pool、effect pool 的 native 字节顺序重算并校对；word 10 的图读取只提供部分诊断材料，明确标记为 incomplete，不冒充完整 scene oracle。性能 checker 和 manual-restart 仍比较全部 **44 个 schema-2 canonical words**，没有跳过 word 10 / 11、屏蔽 self pointer，或用 portable hash 取代完整门禁。[S14] [S19] [S28]

已核对 [check-manual-restart.py](../portable/multiplayer/check-manual-restart.py) 的 `boundary()`：在等待两端到同一 confirmed frame 后，**无条件调用 `multiplayerSmoke.graphStorage()`**，再执行完整 canonical gate；new-generation observation 也会读取 graphStorage。因此下一轮运行 r9 manualRestart 时，正常 boundary 就会执行 v2 解码，无需另扩性能 checker 只为触发该诊断。此处是现有源码的调用路径，尚不是新一轮 Restart 已通过的运行证明。

旧 r8 的 batch exports 缺失时，当前 helper 在默认选项下返回 null，现有 runner 可用当前 fixture 服务冻结旧 WASM。显式 `--initial-capture-batch off` 也会要求新 exports，故旧 r8 对照应省略这个选项或使用 `default`；`on` / `seat0-on` 仅用于支持该接口的新候选。serve、runner 和 checker 均已把新 helper 纳入服务映射或源文件身份校验。[S13] [S14] [S15]；开关条件见 [initial-capture-batch.mjs](../portable/multiplayer/initial-capture-batch.mjs)。

#### 接下来收回的证据

1. **正式 1× 非对称回滚**：P0 开批次、P1 关闭，确认 options 确实应用、scope / pending 在观测边界清零、accepted 与 copied 一致、开启端确有 batchImports，并保留初始 / 最终完整 canonical 和确认音频门禁。当前根代理已开始该运行，本节不提前记录结果。
2. **真实原生菜单 Restart**：复用当前 stopped-loop / 实际 RTC 检查，同时执行 graph v2 解码。要用同一正式 r9 WASM 的完整 canonical、确认音频和 generation 围栏判断稳定 owner 是否解决旧失败；有限生命周期单测不能替代这项结论。
3. **串行紧邻的同字节 4× OFF / ON**：继续使用 50 ± 5 ms 单程注入和桌面合成触控，独立报告逻辑 Hz、native work / 新帧、callback 长尾、前沿停顿和 batch 计数。复制批次即使减少 import，也要实际观察整作收益；不把采样热点百分比或构建 PASS 直接换算为帧率改善。

本轮保持第 10.27–10.28 节 r8“约 55 Hz、同字节 ON / OFF 无明确整作收益”的原结论，直到新的正式记录给出独立证据。网络信息不足时允许平滑降速、正常暂停后另一端自然停下仍是既定行为；待解决的是正常游玩中的长任务和突然停顿。桌面合成触控不等于手机实机结果，本节也没有把本地 RTC 检查扩展成 WAN、公网 direct 或独立远端中继验收。

### 10.32 r9 正式结果、真实 EM_JS 边界与 r10 的可靠性修复

本节更新第 10.31 节当时尚待收回的结果。正式运行时仍是冻结的 r9，WASM SHA-256 为 `7a8bc51bd01c2608cd44555b164a6226c348bf265cc1d9bd3f0000b54b8f49bb`；之后的 r10 源码修改不倒填为该构建的行为。

#### 非对称批次与真实 WASM 行为

正式 r9 的 1×、50 ± 5 ms 单程注入、120 Hz 桌面合成触控运行中，P0 开启 initial capture batch，P1 关闭，其他实验开关均关闭。初始/最终完整 44 字 canonical、确认音频、实际 RTC、身份和自有进程清理均通过。P0/P1 逻辑速度为 59.963 / 59.822 Hz；单次 native 回调均无超过 50 ms 的记录。此处 PASS 仍不包含最终像素运动连续性，严格 cadence 标志亦未全部通过。[非对称原始报告](../artifacts/rollback-smoothness/candidate-r9/cpu1-touch120-batch-asymmetric.json)

开启端实际接受并复制 46,504 个延迟大块、764,734,272 字节，使用 1,703 次 batch import；关闭端批次计数全零。接受/复制数量与字节完全相同，观测边界无未关闭 scope 或 pending block。这说明大块捕获跨边界调用数减少约 96.34%，**不是 CPU 时间减少 96.34%**，更不能把它直接换成帧率收益。

真实 Emscripten 探针已串行编译执行通过：原有 4,048 项定向断言；1023/1024/1025 字节边界；pending capture 中实际增长 WASM memory 后使用新 HEAP 视图；日志 arena 扩容前排空与后续复用；1/1024/1025 字节越界在原来的 Touch 调用内失败，且已接受的有效前缀完整保留。故意 trap 的实例在失败后只读取原始内存，不再进入 C++。这是 Node 中的真实 EM_JS 正确性证据，观测器转发原生 copyWithin；其耗时不作为游戏性能对照。[完整探针收据](../artifacts/rollback-smoothness/candidate-r9/journal-capture-batch-wasm/receipt.json)

#### 原生菜单 Restart 的剩余失步已定位到六个 owner 字段

r9 实際 Pause → Restart Yes、重新校准/HELLO 和迟到的旧 generation ACK 路径已经跑完。旧局 frame 59 的完整状态相等；新 generation 的 frame 119 仍失败，仅 canonical word 11 及包含它的 composite word 1 不同。word 10 已相等。graph v2 对两端原始 word 11 均重算一致，不能通过删掉指针或降低哈希覆盖把该失败记为成功。[Restart 原始报告](../artifacts/rollback-smoothness/candidate-r9/manual-restart-late-ack.json)

回调链、pool occupancy/cursor、回调 context/function/parameter 等其余已捕获字节相同；差异是六个回调槽中偏移 32 的 owner 指针：87/88 为 LasersUpdate/LasersDraw，97/98 和 99/100 为两名玩家的 BombUpdate/BombDraw。稳定 hint owner 的完整载荷也相同。[逐槽字节差异](../artifacts/rollback-smoothness/candidate-r9/restart-callback-owner-diff.json)

沿原生释放路径还发现 Bomb 泄漏：shutdown 经资源环境把 pilot.bomb 清成空指针，随后旧实现 free(pilot.bomb) 实际释放空指针。r10 保存 old 指针后 shutdown，再释放 old；Laser/Bomb owner 改为 World 持有的 pool，保持原生工厂、回调顺序、初始化和释放语义。Laser 单槽；Bomb 为三槽，允许两人局在新 generation 时按确定性分配顺序轮转，不能声称每个 seat 永远绑定同一地址。当前指针、pool metadata、完整 Storage 的首次写入捕获和 active payload 均纳入回滚；word 11 在保留旧字节前缀的基础上增加两个 owner pool。graph v3 描述六个 pool，同时保留 v1/v2 解码兼容。真实 Restart 的最终关闭结论要以 r10 新正式构建复验为准。

另一个 ABI 陷阱：WASI 定向测试中 hint owner 的 payload 为 424、Storage stride 为 432 字节；正式 Emscripten 运行时 graph v2 实测其 stride 为 424。StageHint 正式 payload/stride 为 136/136，ScreenEffect 为 68/72，callback entry 为 60/64。存储捕获必须使用当前目标的 sizeof(Storage)，有效 payload 哈希使用实际 BlockSize；不能把一条测试 ABI 的对齐数写死成另一个运行时的布局。

#### 4× OFF / ON / OFF 尚不支持性能收益承诺

以下三次均为同一冻结 r9 WASM、相同 50 ± 5 ms 单程注入和 120 Hz 桌面合成触控。前两次串行紧邻；第三次用于调查开启组明显异常，不是挑选更好数据代替失败。

| 测量 | P0 / P1 逻辑 Hz | native work / 新帧 ms | native 回调 p99 ms | native 回调 max ms | native >50 ms 次数 |
| --- | ---: | ---: | ---: | ---: | ---: |
| OFF | 56.870 / 56.783 | 14.390 / 13.998 | 20.7 / 16.9 | 27.5 / 19.2 | 0 / 0 |
| ON | 15.034 / 15.022 | 48.342 / 48.888 | 86.8 / 90.9 | 125.0 / 130.6 | 84 / 86 |
| 再次 OFF | 32.734 / 32.803 | 24.444 / 21.039 | 28.7 / 15.8 | 38.9 / 21.5 | 0 / 0 |

三场初始/最终 44 字 canonical、确认音频、RTC、正式身份与子进程清理均通过；smoothness 并未通过。[完整三组统计](../artifacts/rollback-smoothness/candidate-r9/r9-batch-off-on-off-summary.json)

再次 OFF 的低平均逻辑 Hz 包含一次长输入缺口：P0 前沿 411、确认 398；P1 前沿 409、确认 396，双方缺少 12 帧确认输入，随后分别约 3485 / 3316 ms 没有新前沿。期间 rAF/native 回调持续运行，并非同样长度的主线程任务。应用发送注入没有 spike/drop/overflow/error，实际发送计时器最长延迟为 114 / 86 ms。现有数据能把它与 CPU 长回调区分，却不足以判断 RTC、补包、确认或宿主网络的具体责任；正在按 transport/ACK 实现继续定位。idle 期间重复呈现的 native 工作也计入 work/新帧，不能把该整段平均值直接当作每个逻辑 tick 的纯计算成本。[停顿时序与 runner 收据](../artifacts/rollback-smoothness/candidate-r9/r9-off-repeat-stall-analysis.json)

因此 batch 继续默认关闭，ON 大幅变慢的原始证据保留。没有把用户断网、环境负载或某一个缓存当成已证明原因，也没有把低平均 Hz 与单次长主线程任务混写。

#### r10 触控完整前沿

在分块 Correcting 期间，宿主事件可能于两个回调之间进入。旧 application_touch_state 直接读取已回退的 Player：例如历史位置 x60、最近完整位置 x100，新的 +16 px 手势可能被锚定成目标 76，而不是 116。两端 canonical 可以一致地执行错误物理意图，故完整状态一致仍不能替代输入语义检查。

r10 用 Application 持有的完整 8-word TouchState 缓存，发布条件覆盖 Correcting、Application 开放事务、engine.netplay_frame、resimulating 与可用性。只在完整逻辑端点发布；恢复结束而尚不能 forward 时同样更新。首次开局无完整 tick 时可读取原有初始状态，但不冒充已完成缓存，回退至 frame 0 不进入该例外。World/session/generation/seat/Replay/spectator 身份和显式生命周期失效共同处理地址复用；宿主 epoch 变化只 cancel_transient，保留动作 serial 与原有开火策略。

专用单测使用真实 TouchController，包含历史位置/ready/context、cold、nested recovery、同前沿更新、同地址 Restart、blur/cancel 和动作 serial 的正负对照。实际 GameHost 中的回调间触控验证仍需补齐；这一修复也不被用来解释前述 3.4 秒输入确认停顿。

### 10.33 r10 / r10b：实际 Restart 闭环、触控边界与新的数值实验失败

#### 正式候选身份

r10 的正式 WASM 为 `77f2c800cffd3ff28eb2c243e9529093749cf4520db4a4dfdc5b2a35d3eea6e8`，3,136,524 B；r10b 为 `9f2868d3fc0724074150c195bd36893fc6c82777ac1848b345bf80c803e7fd4b`，3,138,801 B。两者都为正式 `-O2` Multiplayer、diagnostic=false，未改生产算术；VBO、camera math cache、packed occupancy、initial capture batch 继续默认关闭。r10b 冻结于 2026-10-12 16:42:04.618 UTC，556 个源码输入完整核验，source digest 为 `ef2a0e66142dc71f804ecb908a1f6fce2356de7e21a79c3a5208c80677f8f36f`。[r10 身份](../artifacts/rollback-smoothness/candidate-r10/runtime/frozen.json)；[r10b 身份](../artifacts/rollback-smoothness/candidate-r10b/runtime/frozen.json)；[r10b 构建收据](../artifacts/rollback-smoothness/candidate-r10b/formal-build.json)。

候选目录一经冻结不再覆盖。后续原生诊断钩子若改变源码，会另建候选，不能将当前工作树反称为这份 r10b 的源码。

#### 原始 owner 差异如何闭环

r10 将 LaserManager 与 Bomb 本体放入 World 所有的稳定池，并在真实 native initializer 首次写入前保存完整 Storage。Bomb 的清理修复为先保存旧指针、真实 shutdown、再释放旧指针；原先 shutdown 清空 current 后再释放，会落到释放空指针。真实 Laser/Bomb/UpdateChain 单元测试完成 47,668 项检查，覆盖两人/三人、逆序创建、保留对象、12 帧全部字节撤销和失败回收。[r10 单测](../artifacts/rollback-smoothness/candidate-r10/native-validation-attempt1.json)。

随后 r10 的实际 Restart 仍然失败，旧六个 Laser/Bomb 回调 owner 差异已经消失；新的差异只在四个回调条目的 owner 字段：SessionUpdate/Draw 与 EffectsUpdate/Draw。链顺序、回调 context/function/parameter、原有六类池的有效负载和元数据一致。这个失败没有被改成通过或忽略 owner 地址。[原始 r10 Restart](../artifacts/rollback-smoothness/candidate-r10/manual-restart-late-ack.json)；[四个 owner 解码](../artifacts/rollback-smoothness/candidate-r10/restart-owner-decoded.json)。

r10b 将 GameSession / GameEffects 各放入容量 1 的 World 稳定池，统一实际分配、失败回收和正常销毁出口。初始捕获保存 actor pointers、池元数据及活跃完整 Storage；新槽在 native memset 前走首次写入捕获；同一负载不重复捕获。完整 canonical 的 word 11 在原有链和池后继续增加这两个 owner 池，原始字段没有缩减。graph descriptor 升为版本 4、8 类池，JS 仍可读取版本 1–3 的旧证据。

r10b 的真实 Session / Effects 单测通过 26,174 项检查，并重跑 Laser/Bomb 与触控前沿测试，560 个记录输入测试前后未变化。新单测包含全部 loader phases、真实 retained shutdown、12 帧 Undo、8 次 history/generation reset、分配/资源失败与遗漏首次写入/actor pointer 的负对照。其 fixture 为 new_game=0、mode=0、资源 barrier 已就绪；其他 SessionObject 工厂是显式外部依赖，不能扩大为完整游戏加载或 ScoreData 新局统计测试。[单测收据](../artifacts/rollback-smoothness/candidate-r10b/native-validation-attempt1.json)。

实际菜单验收随后通过：P1 原生按键打开 Pause，选择 Restart、确认 Yes；观察原生 BeginNextRun、新 generation 校准/HELLO，并在新校准期间通过实际 RTC 送入上一代 ACK。两端初始 frame 59 与新代 frame 119 的完整 44 字 canonical 和确认音频均相等，lifecycleCompleted / canonicalPassed / runner cleanup 均通过。新代 word 11 为两端同一 `3143150277`。这是 stopped-loop 原生菜单与实际 RTC 生命周期验证，不是 rAF 性能或公网证明。[完整 Restart](../artifacts/rollback-smoothness/candidate-r10b/manual-restart-late-ack.json)；[紧凑摘要](../artifacts/rollback-smoothness/candidate-r10b/manual-restart-summary.json)；[runner](../artifacts/rollback-smoothness/candidate-r10b/manual-restart-late-ack.json.runner.json)。

WASI 与正式 Emscripten 的 Storage 对齐仍需区分。例如 GameEffects payload=24 B，WASI stride=32 B，正式布局须从 descriptor 读取；不能把一条工具链的 stride 硬套到另一条，或只比较 payload 而遗漏被捕获的 padding。

#### r10 的 4× 持续触控诊断

2026-10-12 16:28:42–16:29:20 UTC，冻结 r10、实际 loopback RTC、50±5 ms 应用单程延迟、两端 4×、原 120 Hz wall-clock 触控轨迹、unlimited、超期只合并至最新目标；所有可选实验 OFF。这一轮增加了有界收发追踪，属于带诊断的数据。[原始报告](../artifacts/rollback-smoothness/candidate-r10/cpu4-touch120-transport-diagnostic.json)；[摘要](../artifacts/rollback-smoothness/candidate-r10/cpu4-touch120-transport-diagnostic-summary.json)。

| 指标 | P0 | P1 |
| --- | ---: | ---: |
| 逻辑速度 | 57.459 Hz | 57.348 Hz |
| 原生 work / 新帧 | 14.076 ms | 14.176 ms |
| 原生 callback p99 / max | 19.0 / 25.3 ms | 19.4 / 27.0 ms |
| 原生 callback >50 ms | 0 | 0 |
| present gap p99 / max | 32.6 / 42.4 ms | 33.9 / 47.8 ms |
| 逻辑高水位 advance gap max | 49.7 ms | 47.6 ms |
| 触控首个超期 deadline 的最大延误 | 45.27 ms | 49.13 ms |
| touch skipped-not-ready / trace dropped | 0 / 0 | 0 / 0 |

完整状态、确认音频、实际 RTC 与清理通过；本轮没有复现数秒无进展。严格 cadence 两端仍为 false，最终对象运动仍未测。不能用这次顺利运行解释或抹去 r9 的 3.4 秒确认停顿，也不能把它与旧的不同 fixture 运行直接相除得出优化百分比。

这次同时修正了测试 producer 的就绪来源：实际使用 `application_touch_state` 的完整 8-word 视图，原 `multiplayer_status.playerState` 只作诊断。旧 producer 在恢复期间看到历史 player state 时可能自行取消手势。轨迹、幅度、120 Hz 计划和超期合并策略均未减负。

收发追踪最短 20 ms 取样、上限 2048 次，没有额外收包轮询或定时器；P0/P1 为 387/385 个样本，错误与丢弃均为 0。它仍有跨 WASM/JS 状态读取和小对象分配，不能视作无开销。必须区分：Correcting 是历史重演，RTC recovering 是连接恢复；`lastReceivedAt` 是执行任一 lane 的 JS onmessage 时记录的 Date.now，含初始化值，不能单独证明在线到达时刻。不同 peer 的相对 atMs 起点不同，应按各自 performance.timeOrigin 和实际采样时间投影。原生 ready<=0 或 suspended 不进入 runtimeFinish，因此 trace 空白本身不能证明 rAF 没执行。

#### 真实回调之间的触控验证

新的 `touch-frontier-oracle.mjs` 只在 native callback 已返回后的 microtask 里，通过真实 `sdl_touch` 执行两例：new-down/move，以及 cancel/re-down/move。它比较完整 8 words，以实际旧 capture frontier 标定随后第一次新增 Capture，并用 const `multiplayer_packet_build` 读取已有本地输入；不自行 capture/submit，不发送探测包，不写世界状态。正确目标必须与完整端点相符，而且明显区别于历史位置产生的错误目标。

首轮两端 4×、900 帧、50±5 ms 单程延迟，并在 frame 450 注入持续 250 ms / 额外 120 ms 的双 lane 尖峰。P0 两例均直接通过：frame 460 的实际 targetX=-1050.00146484375，而历史错误目标=-1250.00146484375；frame 461 的实际 targetX=-850.0015258789062，而历史错误目标=-1250.00146484375。P1 的首次 down/move 也通过，实际 targetX=6699.99853515625，而历史错误目标=7099.99853515625。这里坐标均为协议的百分之一像素单位，不能写成屏幕像素。

**整轮结果仍为 not-covered / FAIL**：P1 只出现一个合格 recovery yield，未完成第二例；没有发生已观测触控目标错误。两端初始/最终完整 canonical 与确认音频一致。P0 完成观察后停止移动，使另一端后半程远端输入不再持续改变，故下一轮采用逐 seat 检查、对端继续原 120 Hz 触控的 fixture 补覆盖。没有把缺失窗口变为 PASS，也没有为制造窗口改原生恢复预算。[原始未覆盖报告](../artifacts/rollback-smoothness/candidate-r10b/cpu4-touch-frontier-spike-oracle.json)；[详细分析](../artifacts/rollback-smoothness/candidate-r10b/cpu4-touch-frontier-spike-oracle-analysis.json)。

#### 四个算术 wrapper 的独立实验

冻结原版 Arithmetic.cpp 的完整 4965 B 原文，隔离生成只插入四处 flatten 属性的候选。两个独立 Emscripten WASM 共用同一 bridge.o 和生产 SoftFloat.o，分别有自己的线性内存；保留编译命令、IR、code section 与四份二进制 trace。共 140,928 cases / 169,728 records，每批原 A、候选 A、候选 B、原 B 使用两种合法栈历史。[原始收据](../artifacts/rollback-smoothness/arithmetic-flatten-experiment/run-01/receipt.json)；[完整表示差异分析](../artifacts/rollback-smoothness/arithmetic-flatten-experiment/run-01/full16-repeat-analysis.json)。

同一栈历史下两轮原版/候选比较的有效数值、模式、sticky flags、tininess、errno 以及完整 16 B 都是零差异。但原版与候选各自 A/B 重复各出现 151,620 条完整表示差异，全部仅在二元运算结果偏移 10–15；四类转换均未出现这种差异。303,240 是两项重复差异之和，不是候选新增的数值错误。

原因是 C++ Extended 的尾 6 B 为显式 reserved 成员，而 SoftFloat C extFloat80M 在同一位置是 padding；四个回退路径把结构返回值整体写入后，旧栈字节覆盖了原先已经清零的 reserved。实际 IR 和 prime_stack 首差异指纹支持该归因。四处 flatten 的确消除了通用 binary dispatch，但没有整作性能结论。

本轮保留 `FAIL_BASELINE_STABILITY`，生产 Arithmetic.cpp 不变。若以后修复表示，必须独立说明旧 raw16 会改变，再用稳定基准验证；不能通过过滤尾字节、规范化生产 hash 或重命名失败来推进此次优化。

#### 交付前仍需完成的事情

当前实际 Restart 的已知失败路径已闭环，不等于全部动态所有者都有跨生命周期稳定地址保证。只读审计还识别出 Spell、EnemyManager、Results、ScorePopups、Gui、Player、EnemyBulletManager、ItemManager、Replay、Stage 的普通分配路径；其中大 manager 必须保持现有稀疏捕获，Stage 的 current/previous/退休物理槽不能按普通池盲目撤销占用。此项作为明确架构风险记录，不用单次同址重开掩盖。

继续完成逐 seat 实际触控覆盖、独立纯性能 4× 120 Hz 与尖峰长尾验证、最终真实提交的玩家/普通子弹运动证据。TH10 达到交付条件之后进入 TH11mp 的实现；TH11 只读准备不算已经完成回滚移植。

### 10.34 逐 seat 触控闭环、纯 4× 长尾与真实运动观察（r10b / r10c-build02）

本节承接第 10.33 节，分别记录补齐的正确性证据、未带诊断的性能结果、带诊断的定位运行，以及新增观察器的验证范围。**TH10 尚未达到交付条件，纯 4× 的节奏目标与长停顿问题均未关闭。** 后续顺利样本不替代此前失败，也不把离线分析器通过当作真实游戏运动通过。

#### 逐 seat 的真实回调间触控已补齐

新的两场独立检查分别观察一个 seat，对端始终继续原有 120 Hz 桌面触控。两场都直接覆盖 new-down/move 与 cancel/re-down/move，完整 8-word 状态、不同 recovery window、实际第一次新增 Capture 全部通过；双方初始 / 最终完整 44 字 canonical、确认音频和自有进程清理也通过。全程未使用手机。[E103]

| 观察端 | new-down/move 首个 Capture | cancel/re-down/move 首个 Capture | 两例直接结果 |
| --- | --- | --- | --- |
| seat 0 | frame 459 | frame 460 | PASS / PASS |
| seat 1 | frame 460 | frame 461 | PASS / PASS |

这补齐了第 10.33 节所述的缺失覆盖；旧的双端同时观察场仍保留 not-covered / FAIL。各例的协议输入值与历史位置负对照见汇总，不把百分之一像素的协议单位误写成屏幕像素。r10b 运行身份为 WASM `9f2868d3fc0724074150c195bd36893fc6c82777ac1848b345bf80c803e7fd4b`，sourceDigest `ef2a0e66142dc71f804ecb908a1f6fce2356de7e21a79c3a5208c80677f8f36f`；这些结果不属于随后构建的 r10c。[E103]

#### 纯 4× 稳定场接近 58 Hz，强尖峰场的后段长回调仍未解决

同一 r10b 冻结运行时的两场未开启 profiler、原生分段计时与 transport trace。两端 4×、120 Hz 桌面合成触控、50±5 ms 单程应用发送延迟；强尖峰仅作用于 seat 1，持续约 300 ms、额外延迟 600 ms。此处仍为受控本地 RTC，应用发送延迟不是实际公网线缆时延。两场运行、路由、初始 / 最终完整状态、确认音频及清理门禁均通过，节奏门禁是独立项目。[E104]

| 纯性能样本 | 逻辑 Hz（P0 / P1） | native callback 最大值（ms） | 新前沿最大间隔（ms） | 默认 blit 最大间隔（ms） | strict cadence |
| --- | --- | --- | --- | --- | --- |
| steady | 57.989 / 57.998 | 24.1 / 22.7 | 58.0 / 55.7 | 46.7 / 46.1 | 双端 false |
| spike300x600-seat1 | 47.300 / 47.307 | 692.4 / 741.1 | 693.8 / 881.3 | 518.3 / 880.8 | 双端 false |

表中使用统一 high-water 测量窗：从首个达到 frame 179 的实际回调，到首个达到 frame 1247 的实际回调，包含所有中间样本；P1 steady 尾端实际达到 1248。blit 间隔只是提交节奏，仍不能替代对象像素运动。稳定场没有超过 25 ms 的 native callback，但严格 cadence 未过，不能据平均约 58 Hz 宣称 4× 不掉帧。[E104]

强尖峰在 P1 自己的 performance 时间原点下对应 runtime 约 6.766 s，最后一笔被尖峰延迟的发送约在 7.714 s；异常 native callback 的开始时间却集中在各端 runtime 约 16.8–22.2 s，最晚结束约 22.53 s。P0 最大回调为 17.233→17.926 s 的 692.4 ms；P1 最大回调为 19.930→20.671 s 的 741.1 ms。不能直接相减不同页面的绝对 performance 时间，也不能把这些后段长回调统称为当时仍在等待同一网络尖峰。[E104] [E105]

这些异常回调及前后相邻记录的线性内存均为 **200,474,624 B**，没有同次 heap growth；但整场测量窗的 heapMin / heapMax 为 167,051,264 / 200,474,624 B，不能写成整场从未扩容。记录证明 native 回调计时区间内存在长墙钟时间，尚不能单靠总时长断言具体函数计算量、主线程调度或某个传输环节。纯性能场未启用分段计时，原始长尾仍待定位。[E104] [E105]

#### 两场相位诊断不能用作纯性能验收

| 诊断场 | 条件与结果 | 可以保留的结论 |
| --- | --- | --- |
| diagnostic-01 | P0 开 CPU profiler，P1 不开；运行行为发生明显变化，最终 input-settle 失败，整轮 FAIL | 保留相位及追踪数据；不进入性能接受样本 |
| diagnostic-02 | 双端相同分段计时与 trace，不开 profiler；运行门禁 PASS，逻辑 58.988 / 59.028 Hz，native 最大 24.8 / 18.1 ms | 本场未复现约 700 ms native 长尾，仍不能关闭原失败 |

diagnostic-01 的终止错误是 `Confirmed peer input stopped advancing`，位于 `performanceSettle`；自有进程已清理。非对称 profiler 改变了观测条件，不能拿这一场的恢复频率或时长代表未采样运行，也不能据此断言 profiler 是原长尾的根因。[E106] [E117]

diagnostic-02 的 P0 新前沿仍有 **138 ms** 最大间隔，而同场 native callback 最大仅 24.8 ms；P1 新前沿最大间隔为 46 ms。这个 138 ms 应继续按实际记录区分网络输入缺口、CPU 预算让步与回调到达间隔，不能混写成一个 138 ms 原生长任务。两端 strict cadence 仍为 false；带计时 / trace 的约 59 Hz 不替代纯性能结果，也不抹去先前 692.4 / 741.1 ms 的异常。[E107]

#### r10c-build02 的完整冻结身份

r10c-build02 增加真实 ANM 提交与默认帧缓冲观察边界，观察器默认 OFF；稳定所有者和触控前沿保留。正式构建 exit 0，冻结清单完成 558 个源码文件逐项核对。[E108] [E109]

| 项目 | 冻结值 |
| --- | --- |
| 源码树 | `D:/workspace/eagler/worktrees/th10-rollback-smoothness` |
| 冻结运行时 | `D:/workspace/eagler/worktrees/th10-rollback-smoothness/artifacts/rollback-smoothness/candidate-r10c-build02/runtime` |
| HEAD / common HEAD | `2e5d5464b2b28ef3fb936234660b773b604c91ce` / `77666259b4bb14b965a34a1f38a4e2ec88c289ac`；具体增量以逐文件清单为准 |
| profile / diagnostic | `multiplayer` / `false` |
| WASM SHA-256 / 大小 | `fb1834d18f5e888ed0d57c247717cc925e8cba40a03873cbfd44258c7e68e085` / 3,158,471 B |
| loader SHA-256 | `e1147f6a876f60690aef8b56646918a9cd582d97afd6890444549754b26131f6` |
| sourceDigest / 清单数 | `15ec90c5ef43642e23db5133957bf45da84a1444030c9117e63aaf016d85e785` / 558 |
| 冻结时间 | `2026-10-12T17:20:45.878Z` |
| 默认关闭 | live motion observer、callback timing、Model quad residency、camera cache / verify、packed occupancy、initial capture batch |
| 观察 / graph ABI | live motion schema 1；graph storage schema 4 |

HEAD 与 common HEAD 只表示源码基点，不能代替该候选的完整 sourceDigest、`runtime/build.json` 和 `runtime/frozen.json`；原生变化副本及逐文件 SHA 也保存在冻结目录。后续 JS 分析器与测试 runner 的修改不自动成为这个 WASM 的新原生代码。[E108] [E109]

#### 离线运动分析 24 项通过，首轮真实运行仍为未覆盖

新观察器记录自然提交的四顶点、身份、资源、端点与真实 world_alpha，并在实际默认 blit 后匹配完整提交记录；JS 通过非阻塞 PBO 读取预设窗口中的 ROI。离线分析只在同一身份 / 端点、连续真实 commit、形状与资源稳定时检查几何位移，并要求至少 1 px 可分辨、优于原地且候选唯一的局部纹理共移动。present 计数和 ROI hash 变化都不是运动通过条件。采样、分析开销只属于诊断运行。现有 ABI 没有独立绝对端点 quad 锚点，故检查的是相对 alpha 位移；共同的未知固定偏移不在此门禁内，RGB 相关也不是完整透明度 / 遮挡 oracle，歧义必须留为 not-covered。[S33] [S34] [S35] [S36]

2026-10-12 17:31:24 UTC 的独立离线正负对照 **24 / 24 PASS**：包括旧 ROI 配新计数、旧 geometry 配新 alpha、背景变色但对象停住、重复纹理、身份替换、掉样、合法静止、未采像素区间和窗口边界不能漏报等。它只验证分析器判定，不证明真实游戏已流畅。分析器 SHA-256 为 `b0dc97cc2f155676bbf0c5ae359205210b26a438d1544e1d7c900e5add447d6e`，测试源码为 `9c53f1751abef451419e578bb1d755036e4c20f27597d857adba7933dcf2b540`，独立回执保留这两项身份。[E110] [E111] [S37]

随后 r10c-build02 的 `live-motion-cpu1-01` 已结束：仅 P0 开启观察，P1 关闭观察作为同 WASM 对照。P0 共 2561 个真实默认 blit，其中 metadata-only 2465 个，三段预设窗口各 32 次、合计 96 次像素读取全部完成；错误、dropped、binding mismatch 和完整性拒绝均为 0。**最终运动门禁为 NOT-COVERED / FAIL**：本地玩家 15 个合格像素窗口，远端玩家 0，普通子弹 0，明确 violations 为 0；要求仍为两名玩家各至少 2 个窗口、子弹至少 1 个，不能合并玩家数量凑通过。[E116]

远端 0 覆盖不能解释成远端没有移动：紧凑摘要的六个 completeExamples 中，远端每次记录的 previous / current 相等，但跨样本位置发生变化，须继续核对其端点与重演边界。相同示例中的子弹尚未被观察到，说明预设采样时段也需要检查；不能挑出成功图片替换失败尝试。局部玩家的 15 个窗口只是有限自然窗口证据，不证明全画面永不掉帧；首轮真实结果也不是纯 4× 性能证据。原始大报告保留 SHA 和路径，日常复核使用 `compact-summary.json`，不重新展开庞大嵌套摘要。[E116]

#### 默认关闭时的原生检查与 runner 失败边界

既有 ordinary 与 multiplayer 的真实 ANM submit helper / body 检查均通过，包含实际顶点、回调 / pipeline、VM / manager 字节、返回值、errno 与算术状态比较。dense 这一轮的原生输出也完成 PASS：579 个 subpixel cases，1200 / 4096 ANM 实例的 12 帧 undo / replay / 顶点比较，以及溢出池 packed / fragmented、per-slot / runs 边界均保留原结果。[E112] [E113] [E114]

**整份 `disabled-observer-validation/receipt.json` 仍为 FAIL**：dense runner 在所有原生检查完成后写依赖元数据时，错误地删除 Windows `.d` 目录反斜杠，读取 `ExactFloat.hpp` 的失真路径而 ENOENT，exit 1。已只修 `portable/check-anm-dense-engine.mjs`，复用既有 Make 依赖解析器，保留转义空格 / 盘符与工作树越界拒绝，并将解析器自身纳入源码哈希闭包；原生实现、编译参数和通过标准未改。原失败回执及 stdout / stderr 保留，修复后的完整复测收据尚待收回，不提前改写为 runner PASS。[E112] [E114] [E115] [S38] [S39]

#### 下一步与交付边界

继续定位纯尖峰场的长 native 回调，并单独解释 diagnostic-02 的 138 ms 前沿间隔；补齐远端玩家与自然子弹的真实运动覆盖，保持预设采样预算与严格的身份 / 提交 / 像素门禁；收回 dense runner 的完整复测。以上完成后还须重新核验不带诊断的 4× 节奏与强尖峰体验。当前不宣称 TH10 可交付，不把 TH08 / TH09 的用户好评移作 TH10 证明；TH11mp 的实现继续排在 TH10 交付条件闭环之后。

### 10.35 运动门禁修正、远端显示根因与 138 ms 间隔分类

本节补充第 10.34 节之后的结果，不改变原始运行或旧报告的结论。TH10 仍待真实运动覆盖、纯 4× 节奏和此前长 native 回调问题闭环。

#### 原生 disabled-observer 检查已完整收回

`disabled-observer-validation-02` 于 17:37:15 UTC 完成，exit 0、完整回执 PASS。Windows Make 依赖解析的两项既有测试，以及真实 subpixel / dense ANM 的原生执行和最终元数据写入均通过；普通 / multiplayer 的 submit helper 与完整 body 成功证据仍引用上一轮，不重复冒充新增测试量。修复只涉及测试 runner 对 Windows `.d` 路径的读取，不改变渲染实现或通过标准。[E121] [E112] [E113]

#### 子弹：分开处理分析器假阴性与未采到像素

真实子弹将插值后的世界平移和锚点写入 `vm.script_position`，而玩家主要使用 `vm.position`。旧分析器错误要求两类对象的 `script_position` 都不变，因此把正常子弹运动判为外观改变。现在只对玩家保留这个不变量；对子弹明确校验该字段的变化必须与逻辑位移乘以实际 `world_alpha` 增量一致，其余形状 / UV / 色彩 / 资源条件保持。新负对照可识别不匹配的 script 平移，25 项测试先通过，随后显示前沿 ABI 对照扩充为 29 项并全部通过。[S36] [S37] [E122] [E118]

使用修正分析器重读原始、SHA 未变的 `live-motion-cpu1-01/performance.json`，得到子弹 392 个合格**几何**窗口；这不是重新运行游戏，也不是新增像素证据。旧预设窗口在 180、600、1000 帧开始，各自前 32 次实际提交很快耗尽预算，而该场自然子弹较晚出现，六个保留示例均未采到子弹。后续固定窗口改为 180–260、700–780、1100–1180，各仍 32 次，总额仍 96；实际尝试和失败全部保留，不按成功结果筛选图片。旧场像素门禁仍是本地 15、远端 0、子弹 0、violations 0，NOT-COVERED / FAIL。[E120] [E119] [S35]

原始 95,153,523 B 报告 SHA-256 为 `be7dac382a0ef265fd60de8925b461b64c0c805feff2ecf2f51a9183aead1653`。补充的 `compact-summary-v2.json` 从真实 `comparison` 字段核实最终两端各 44 字 canonical 相等、确认音频相等，不能把漏读旧字段误报成缺少一致性结果。整体失败仍由运动覆盖不足触发，不将最终一致性通过转写成总门禁通过。[E119]

#### 远端：最后一次历史 tick 不是完整显示前沿

真实记录显示远端坐标跨已完成逻辑前沿发生变化，但最终 `previous/current` 连续相等。原因是绝对触控输入的预测继续保持最后一个真实目标：回滚较早的历史帧重新抵达目标后，最后几笔历史 / 前进 tick 已不再移动；逐 tick 保存的 presentation 端点最终都是同一点，无法让两次完整预测前沿之间的更新产生高刷子步。零 violations 不能证明跨回滚边界没有跳动，因为原分析器在该边界明确结束旧窗口。[E119] [S36]

修复方向是在 rollback / canonical 状态以外保存远端玩家的固定值显示前沿：首次 Restore 前保留完整目标，历史恢复期间不替换；在 Application 的最终完整步边界发布新目标，仅渲染副本使用该端点对。没有新前进帧的纠正、死亡 / 对象复用 / stage / generation / seat 改变、已真实展示的终点以及暂停停止都需要明确断开或消费，避免显示回退。原始逻辑端点和 DirectTouch 预测规则不变。此段描述进行中的生产修复，尚未将它计入已冻结的 r10c-build02 WASM 或真实通过成绩。

观察 ABI 使用保留的第 4 记录槽，显式 magic `0x52465031` / version 1，并由远端主记录 flag 256 指定显示端点；主记录的原始逻辑 previous/current 字段不改语义。分析器要求目标坐标逐位等于同帧原生 current，检查 frame / revision / incarnation 与有效 snap，任何缺失、伪造、错误 seat 或无标记的扩展都拒绝。独立 29 项正负对照于 17:54:53 UTC PASS；它证明这些拒绝规则有效，仍不能代替生产修复的真实运动运行。[S36] [S37] [E118]

#### 138 ms 是网络信息等待，692 / 741 ms 尚未解释

对 r10b `spike-stall-diagnostic-02` 的 P0 最长前沿间隔重新定位：callback 行 849→866，页面自身时钟 6852.2→6990.2 ms，间隔 138 ms。其间前沿 461、连续确认 449、缺口 12、scale 2；已接收包累计数持续为 711，队列和 buffered amount 为 0，RTC 仍 open / connected。连续约 15 个回调只需 1.6–3.3 ms 且均提交画面。6980.1 ms 新包到达后累计 712、确认升至 467；随后以约 7.1 与 9.3 ms 的有界恢复 / 前进回调继续运行。这个间隔由预测边界上的真实信息不足主导，不能称作 rAF 被阻断 138 ms。[E123]

上面的分类不解释旧纯尖峰场的 692.4 / 741.1 ms 长 native 回调。下一次诊断保留同一冻结 r10b、同一双端 4× / 120 Hz 触控 / 50±5 ms / 300×600 ms 尖峰，增加对称、有界的 Chrome timeline，关闭 CPU sampling profiler；大 trace 流必须在最终 canonical / 音频核验完成后再读取，避免再次改变停止后的对账过程。未采到长回调不能据此宣布修复。[E105] [E106] [E117]

### 10.36 r10d 远端显示桥已接入；真实几何通过，透明前景取证与长尾仍待关闭

#### 冻结身份与原生边界

r10d 于 18:01:55.366 UTC 冻结为正式 multiplayer O2 构建，`diagnostic=false`，559 个源码文件清单逐一验证。WASM SHA-256 为 `581b821210c1f0c77d482857507e4c47e8bf2295b9ca10dcdf4ff287e8889012`；loader SHA-256 为 `e1147f6a876f60690aef8b56646918a9cd582d97afd6890444549754b26131f6`。观察器及 VBO / camera / packed / batch 试验均默认关闭；冻结目录不再覆盖。[E125]

远端显示桥使用固定容量的独立值缓存，首次 Restore 之前冻结旧完整目标，仅在 Application 最终完整步发布新目标。历史重演不反复改写显示端点；真实完成的 endpoint Present 才消费旧端点。无新前进的纠正、身份变化、死亡状态、超大位置跳跃和停止各自有明确 snap / invalidate 行为。取值必须精确匹配 world / session / generation / seat / frame / revision / 原生当前位置，渲染只改变临时 Player 副本，原始逻辑位置和输入不变。[S40] [S1] [S2] [S43]

缓存没有加入 rollback snapshot 或完整 44 字 canonical。实际 live Host 在真实 Present / commit 后调用消费边界；停止后的手工 `presentation_lab_draw` 不执行同一消费路径，不能把手动 Draw 误称为 live 消费验证。单独 helper 在 WASI O2 与 trapping UBSan 下各通过 11 组、1334 次检查，并保留源码字节不变性守卫；这不是浏览器覆盖的替代。[E124]

#### 实际 1×、原 120 Hz 合成触控轨迹的结果

18:05:16.867–18:06:26.313 UTC，双方独立硬件 Chrome、1× CPU、50±5 ms 单程延迟、1200 帧，固定窗口 180–260 / 700–780 / 1100–1180。使用原 120 Hz 桌面触控轨迹，未使用手机、未开启 CPU profiler 或 Chrome trace。完整原始报告 42,564,610 B，SHA-256 为 `a8b4800a4873dcc8be28d113150e6358ead31e0135c7f19459a00e7750f6ffd6`。[E126]

| 结果 | P0 | P1 |
| --- | ---: | ---: |
| 逻辑 Hz | 59.864 | 59.840 |
| native p99 / 最大，ms | 6.6 / 7.8 | 5.8 / 7.4 |
| present p99 / 最大，ms | 13.6 / 52.7 | 13.3 / 46.6 |
| 新逻辑前沿间隔 p99 / 最大，ms | 26.6 / 63.8 | 26.1 / 50.2 |
| 现有严格 cadence 门禁 | 未通过 | 未通过 |

双方 native run、最终完整 44 字 canonical、确认音频均通过，实际 RTC 路由和自有进程清理完成。观察 P0 的 2568 次 default blit 均绑定到真实提交，96 次像素尝试全部完成，错误、丢弃、binding mismatch 与队列溢出均为 0。由于自然对象越界等原因，22 个 ROI 不受支持，按原记录保留。整体 checker 仍为 FAIL：像素覆盖本地 14、远端 0、自然子弹 15，violations 0；两端 cadence 也未过。[E126]

远端有 981 个合格几何窗口，2567 个新显示扩展记录中 2108 个端点对实际非零。例如 frame 180，原逻辑 previous / current 都约为 `[-92.31,333.60]`，显示 previous 为约 `[-94.66,333.59]`；同一端点对两次真实提交的 alpha 从 0.13771 到 0.61771，实际 quad 位移约 1.128 px，与显示端点插值相符。这证明旧的“最后历史 tick 已停在绝对触控目标，所以高刷端点相同”问题已被实际路径修复，不能据此扩大为完整画面门禁通过。[E126]

#### 半透明前景为何仍不能通过整块 RGB 匹配

三个固定窗口各取最先完成的六张远端 ROI，原字节保留，并生成仅翻转行顺序 / 2× 最近邻放大的查看图；没有按匹配成功筛选图片。肉眼可见精灵与滚动地面混合，后两个窗口还有绿色弹幕遮挡上半身。原生 `player_proximity_alpha` 对远端平时最多约 128/255，接近时可降到 51/255；实际顶点颜色记录也分别出现 `0x33ffffff` 和 `0x80ffffff`。[E127] [S41]

因此，quad 内的最大 RGB 梯度不必来自角色，旧匹配器可能选中背景或遮挡弹幕。实际失败包含 expected 相关系数约 0.675、竞争位移约 0.737，不能只把 0.08 竞争裕量调低来“过测”。当前保留原相关、竞争、零位移和背景反证门槛，新增默认 OFF 的 CPU 纹理取证接口：对真实提交的 owner / surface / version 保存印记，读取已有 CPU Image 并核对 GPU 版本未变、未被 GPU 写入，以原始纹理 alpha / 梯度决定前景支持点，再核对真实默认 framebuffer 像素。它不修改游戏不透明度，不额外 Draw，不调用同步 GPU readback；接口仍在下一版实现和验证中，未计入 r10d。[E126] [E127] [S35] [S36]

#### Chrome 追踪：兼容性修正已证实，完整运行缓冲容量尚待复测

diagnostic-03 在游戏测量前因类别枚举为空停止。独立同配置 about:blank API probe 实测：Tracing.start 前枚举为空，ACK 之后有 288 类，真实 stream / 两个 marker / dataLoss=false / cleanup 全部通过。helper 因而将支持检查移到 start ACK 之后，未取消支持门禁；启动失败的记录仍保留。[E128] [E129]

diagnostic-04 使用同一冻结 r10b、双端 4×、120 Hz 触控、50±5 ms，以及 frame 450 的 seat1 300 ms 持续 / 600 ms 附加延迟。双方实际 native、完整 44 字 canonical 和确认音频通过，native 最大 16.7 / 16.9 ms，没有复现旧 692.4 / 741.1 ms。但双方 32 MiB recordUntilFull 均溢出：各约 870k 个事件，cc / gpu 详细绘图事件占主要部分，dataLoss=true，结束 marker 缺失，所以整体诊断 FAIL。两个完整取出的 gzip 和 SHA 保留；流关闭、session detach 完成不能弥补丢失的时间区间。[E130]

下一轮只保留 toplevel / devtools.timeline / blink.user_timing / v8，排除 cc / gpu 详细事件，保持 32 MiB、同一 1200f 场景、双端对称及完整首尾 / dataLoss=false 门禁。真实 probe 已证明 RunTask / FireAnimationFrame / FunctionCall 存在带 tdur 的实例；仅对实际同时含 dur / tdur 的目标事件比较 wall 与线程时间，不能把差值直接称作 GPU 等待，也不能将无 GC 事件写成排除 GC。语法、helper import 及 disabled JS capture 不访问 native / GL 的验证已通过；完整四类追踪仍在运行。[E128] [E131] [S42]

当前仍不宣称 TH10 可交付，也不将新一轮未复现等同于旧长停顿已修复。TH11 只读接线审计已形成，实施继续等待 TH10 交付条件关闭。

### 10.37 r10e 实际像素运动已通过；4× 诊断与 rAF 等待边界，最终验证继续

#### 本轮冻结身份与默认开关

r10e 于 18:36:42.388 UTC 构建、18:38:00.226 UTC 冻结，仍为本工作树的正式 multiplayer 构建，`diagnostic=false`。本节实际运行使用冻结产物，不以后来继续修改的工作树替代其身份；559 个源码文件的清单已逐项核验，HEAD 仅作版本定位，完整源码摘要与产物摘要如下。[E132] [E133]

| 身份项 | r10e 冻结值 |
| --- | --- |
| 源码树 | `D:/workspace/eagler/worktrees/th10-rollback-smoothness` |
| 冻结目录 | `artifacts/rollback-smoothness/candidate-r10e/runtime` |
| HEAD / 分支 | `2e5d5464b2b28ef3fb936234660b773b604c91ce` / `experiment/rollback-smoothness` |
| common HEAD | `77666259b4bb14b965a34a1f38a4e2ec88c289ac` |
| WASM 大小 | 3,173,368 B |
| WASM SHA-256 | `d68bc4962e96938e87e0c126deb49a2161af9e4caee0245779d10a95a705b647` |
| loader SHA-256 | `dd7f866a35ed69c51c3eaaa6a9c24021ac3eb1954542d553f7b34620d21dcdd4` |
| sourceDigest | `3423437dd872aef51fbc904ef3ef76ff9d82d152eab7cd6815c61c2ba608bc2d` |
| build.json SHA-256 | `19d128db9abd4ee85ae007579d5ed1045bf4766297a42a751a4d0894855ff9ff` |

完整显示前沿及 Hint / Laser / Bomb / Session / GameEffect 稳定 owner 池继续启用，graph storage 为 v4。主运动观察器、LMT1 CPU 纹理观察器、callback 计时及 VBO / camera / packed / initial-capture-batch 试验均默认 OFF；LMT1 的最大纹理边长为 128。只有本节明确请求的诊断运行开启相应观察项。语法与默认 OFF 不访问 native / GL 的 harness 检查已经完成，不能把这些检查扩大为整作性能结论。[E132] [E136]

#### 半透明角色的实际运动证据，独立正负对照与原生守卫

LMT1 将当前真实提交的 draw tuple、slot、owner、surface、CPU / GPU version 与已有 CPU 纹理相绑定，再用纹理自身的 alpha / 梯度选择前景支持点，核对默认 framebuffer 的实际位移。它用于解决第 10.36 节“半透明角色与背景混合，强 RGB 特征可能来自背景”的证据歧义；没有改变角色透明度、确定性输入或碰撞，也没有降低原有 0.92 相关门槛和 0.08 竞争裕量。[S33] [S34] [S35] [S36]

18:56:39.031–18:56:40.149 UTC 的独立离线 suite 为 **49 / 49 PASS，0 failed / skipped / todo**。原有 29 项保留，新增 20 项使用独立公式生成 50% 半透明精灵与纹理背景，覆盖 Nearest、真正四 texel Linear、CPU 顶行与 GL 底行方向、非对称 mask、20% 不透明度不覆盖、全透明 intrinsic mask、旧像素配新 tuple、缺失或损坏的 LMT1、版本边界和普通数组序列化。合法 native non-OK 空 payload 只能记 not-covered；损坏包及同版本字节改变会阻断证据，不回退到 RGB 路径凑覆盖。收据绑定 analyzer SHA `fe4def49f3586919383cdc862898b5886fd52f05b5eb2239d5318a7653449bde` 与 test SHA `9a4df753eb10900765d5e569e63814c182c61ecdbc00acdbe49eeec4c55b5191`。[E134] [E135] [S37]

实际 1× 场于 18:59:48.843–19:01:02.769 UTC 执行：两个独立硬件 Chrome，本地 Launcher 建链后的实际 RTC，50±5 ms 单程注入，1200 帧，目标 120 Hz 桌面合成触控。观察器只开 P0，其世界中分别观察本地 seat0、远端 seat1 与一颗自然子弹；P1 观察器关闭，使用同一冻结 WASM 作为 canonical 对照。双方开启 callback phase 与 transport 诊断，没有 CPU profiler / Chrome trace。本轮是运动与正确性取证，不能当作纯 1× 性能样本或 WAN 测试。[E137] [E138]

| 实际运动门禁 | 结果 |
| --- | --- |
| 本地角色像素运动窗口 | 19，达到该 seat 独立要求的至少 2 个 |
| 远端角色像素运动窗口 | 4，达到该 seat 独立要求的至少 2 个 |
| 自然子弹像素运动窗口 | 9，达到至少 1 个 |
| 固定像素计划 | frame 180–260 / 700–780 / 1100–1180，各前 32 次尝试 |
| default blit / metadata | 2586 / 2586 |
| 像素尝试 / 完成 | 96 / 96 |
| CPU 纹理包尝试 / 接受 | 168 / 168，拒绝与错误均为 0 |
| readback errors / binding mismatches / queue overflow / metadata overflow | 均为 0，停止时无 pending |
| 不支持 ROI | 24，原样保留，不算运动通过 |
| analysis errors / violations | 0 / 0，coverage 与 captureComplete 均为 true |
| 最终状态 / 确认音频 | 双方完整 44 字 canonical 一致，确认音频一致 |

同一次真实运行还通过 **25 项 native 观察边界守卫**：默认关闭、非法 enable、错误 tuple / owner / surface、合法两 seat 包、toggle 失效、下一次真实 Draw 重建印记、callback 关闭后旧 pending 拒绝、结束关闭及状态守卫均通过。共 19 次纹理 getter、21 次状态快照，在两个既有提交边界观察；守卫不另行 Draw、查询 GL 或提交资源。这些结果与错误明细保留在实际 motion summary，不能以离线 fixture 代替它们。[E137] [S44]

原始报告为 44,190,678 B，SHA-256 `1a185ee1c85cc0aa12b95e69a5129a4a35140b420469df08e39d588ef341fc55`；runner 的运行后身份核验、实际 route / state / audio 门禁与自有进程清理通过。本轮逻辑为 59.918 / 59.948 Hz，native 最大 8.6 / 8.8 ms；present 最大仍有 24.9 / 51.7 ms，两端严格 cadence 均为 false。**因此已关闭的是三个对象种类在预设有限自然窗口内的实际运动覆盖，未证明整个画面、全部时间或 4× 永不保持、永不掉帧。** 第 10.36 节远端 0 的旧结果继续保留，本轮没有挑走失败样本、静止状态或不支持 ROI。[E137] [E138]

#### r10e 新 4× 尖峰诊断：native 长回调未复现，严格节奏仍未通过

19:04:58.768–19:05:45.939 UTC，两个独立硬件 Chrome 均 4× CPU，目标 120 Hz 桌面触控、50±5 ms 单程注入、1200 帧，frame 450 对 seat1 注入 300 ms 持续、额外 600 ms 延迟。VBO / camera / packed / batch 继续 OFF；双方开启 callback timing 与 transport trace，未开启 CPU profiler、Chrome trace 或实际运动观察器。报告的 PASS 指实际运行、route、完整状态与确认音频；它不包含严格 cadence 或本轮像素运动通过。[E139] [E140]

| 同一连续测量窗口 | P0 | P1 |
| --- | ---: | ---: |
| 高水位前沿范围 | 179→1247 | 179→1247 |
| callback 数 / 新前沿帧数 | 2271 / 1068 | 2249 / 1068 |
| 逻辑 Hz | 58.982 | 58.987 |
| native callback p99 / 最大，ms | 13.9 / 18.2 | 13.7 / 17.6 |
| native callback 超过 25 ms | 0 | 0 |
| callback 结束间隔最大，ms | 56.3 | 54.9 |
| 扣除当前 native 后的间隔最大，ms | 50.3 | 45.7 |
| present p99 / 最大，ms | 18.7 / 55.3 | 18.5 / 53.1 |
| 新逻辑前沿间隔 p99 / 最大，ms | 31.1 / 96.5 | 31.6 / 60.3 |
| rollback / resimulated frames | 798 / 3219 | 936 / 3938 |
| 严格 cadence / 本轮像素运动 | false / not measured | false / not measured |

这里保留首次高水位跨越之间的全部 callback，没有按快慢重选样本。双方完整 44 字 canonical、确认音频与清理均通过；原始报告 6,559,998 B，SHA-256 `974ba04d1276a6a6d243643671145f21d99050bbd0c173acc9cac2d4b9d3492b`。本轮没有超过 25 ms 的 native 回调，是可复核的新结果；**phase / transport 诊断已开启，所以不能把它与旧纯运行合并成“4× 已无长停顿”的保证。** 未复现旧约 700 ms 样本，亦未解释该样本。[E139] [E140] [E105]

#### diagnostic-05 的完整 trace 与 request→fire 能确认什么

第 10.36 节尚在运行的四类追踪已经完成：同一冻结 r10b、双端 4× 原尖峰负载，保留 `toplevel / devtools.timeline / blink.user_timing / v8`，仍用 32 MiB 上限，未启用 CPU sampling。双方完整追踪与收据通过，实际 native / route / 完整状态 / 确认音频亦通过；六类溢出的 diagnostic-04 仍为原失败记录。四类版本不具有 GPU 详细事件覆盖，不能据此宣称排除 GPU 等待。[E141] [E143] [E151] [S42]

离线分析按每页自己的首尾 marker 映射到对应 `CrRendererMain` 的 pid / tid，绑定原报告及两个 gzip 的大小和 SHA，保留原始 marker / 时钟漂移 / 附近 rows / transport 与有限事件，不用墙钟拼接两个页面。P0 / P1 原始事件数为 354310 / 346159；首尾 offset 漂移为 −23 / +18 µs。两端的最大结束间隔与最大 native 四个目标均分析通过，event overflow 为 0。原报告 7,021,550 B，SHA-256 `3b87e66bfc2500f07add298c42072e8057a38fc3ac797129a8bf3709878a13bb`；完整离线结果 1,809,204 B，SHA-256 `1c7e3eb72f41f5849a81b1cb6b98f690ec8535bf2149a1a204413ed7e48d6eaa`。[E141] [E142] [E143] [S45]

最大结束间隔附近的关键时序如下；“native 之外”使用 `previous callbackEnd → current callbackEnd − current nativeMs`，属于由 row 推回的入口近似，仍含 JS finish-hook 与采样误差，不能把结束间隔全算作 native 耗时。[E141] [E143]

| 各端最大结束间隔目标 | P0 | P1 |
| --- | ---: | ---: |
| row index / 当前结束时间，ms | 2129 / 17331.9 | 1646 / 13527.8 |
| 结束间隔 / 当前 native，ms | 41.1 / 2.8 | 70.0 / 12.3 |
| native 之外的间隔，ms | 38.3 | 57.7 |
| 该区间同一主线程完整 RunTask wall 并集，ms | 4.289 | 4.397 |
| 未被上述 RunTask 覆盖，ms | 34.011 | 53.303 |
| 匹配的 rAF request id | 2145 | 1665 |
| request 相对上一 callbackEnd，ms | +0.125 | −0.110 |
| 对应 FireAnimationFrame 起点相对上一 callbackEnd，ms | +37.940 | +57.392 |
| 同 id request→fire，ms | 37.815 | 57.502 |
| 该次 Fire 的完整 wall / 实际 tdur，ms | 3.421 / 0.775 | 12.752 / 2.935 |

这确认了上述有限样本存在“rAF 已申请，随后较晚才 fire”的间隔；P1 的 70 ms 并非 70 ms 都花在当前 native 回滚调用中。相邻 row 的 missing inputs 分别从 4→1、4→2，interval scale 都为 1，没有在这些边界看到硬预测上限耗尽。完整 RunTask 的 wall 区间只求并集，不能嵌套相加；保留的 tdur 仅适用于各条真实完整 span，未按 wall 比例估算截取区间 CPU。[E141] [E142] [E143]

剩余 34.011 / 53.303 ms 只能称为“未被本次捕获的同线程 RunTask 覆盖”，不能称作 CPU 空闲、GPU 阻塞或 OS 抢占。附近没有被归类为 GC 的事件也不能排除 GC；任务参数里的 `cc/trees/proxy_impl.cc` 来源不足以证明 GPU 根因。旧纯 r10b 的 692.4 / 741.1 ms 回调没有这份 trace，发生于旧运行约 16.8–22.2 秒、明显晚于约 6.77 秒的注入时点；它的原因仍然开放，不能套用本轮 41 / 70 ms 的时序或用本轮未复现擦除。[E142] [E143] [E105]

#### 新首写保护与余下门禁：不计入 r10e 已冻运行

本节更新期间，callback 首写保护的 WASI O2 与 trapping UBSan 两份独立收据均已完成：每份 **16 cases / 141 checks、candidate 0 failures**，使用真实 `Callbacks`、`UpdateChain`、pool capture 与 common journal，同一 owner 的完整字节快照作独立 oracle。每份 candidate 的 26 次 full-pool 比较全部一致、6 次清零前观察无遗漏、4 次 byte / block budget 失败均被 journal 拒绝。[E144] [E145] [E147] [E148]

固定旧 `Callbacks.hpp/.cpp` 的 legacy 负对照在同样的 141 次检查中各出现 38 处失败：18 次 full-pool mismatch、6 次 preclear miss、4 次预算失败未拒绝；两份正对照均为 0 失败。候选和旧版使用相同共享 UpdateChain / Journal 与 oracle，原始 JSONL 均保留。此处“不足预算拒绝”指 journal 失败锁存和 EndFrame 拒绝：已分配指针仍可能非空、payload 仍可能被初始化，不能写成“分配返回空且完全无写入”。[E144] [E145] [E146] [E147] [E148] [E149]

这些单测的 receiver 仍是 fixture，**没有执行真实 AnimationEngine 构造 / forwarding、真实 World 的 BeginFrame / EndFrame 或 Application 的 finalize / MarkSimulated 提交门禁**，也不验证渲染、RTC、性能或旧 700 ms 原因。因此单独通过 callback 单元不等于这条实际接线全部通过；该新修复不倒填进 r10e 的冻结身份和浏览器结果。[E144] [E147]

后续仍需完成几何池约 64 KiB 清零前首写遗漏的修复、ANM / geometry 的真实单元与整条提交门禁、下一正式 runtime 的构建冻结，以及新版本的完整状态 / 音频 / Restart / 运动和纯 4× 复验。真实 BGM harness 的语法、源码绑定和默认关闭检查已有收据 PASS；它没有执行实际音乐播放，BGM 浏览器运行仍待 r10f，不能用确认音频一致代替实际 BGM 覆盖。[E150]

网络尾部更强渐进制动目前只完成只读方案评估，本轮暂不实施，不把建议参数写成 r10e 已有收益。当前可承诺的是已列出收据范围内的正确性与有限运动覆盖；TH10 的严格交付及纯 4× 节奏目标仍未通过，TH11 继续保留只读方案，等待 TH10 门禁关闭后再实施。

### 10.38 r10f 首写保护、真实 BGM 纯运行与收尾次序（2026-10-12）

本节记录 r10f 已冻结的正式字节、已完成的有限正确性证据，以及真实 BGM 运行暴露的性能失败。第 10.37 节“ANM / geometry / BGM 待验证”是此前状态；以下新收据补充其进展，不倒改旧章节。**当前没有 TH10 交付通过结论，也没有把测试报告收尾的改动计为游戏流畅度收益。**

#### 冻结身份与默认边界

正式 multiplayer 构建时间为 **2026-10-12 19:29:48.463 UTC**，于 **19:31:08.812 UTC** 冻结，`diagnostic=false`。runtime 归档同时记录并核验 559 个源文件；HEAD / common HEAD 用于定位源码基线，未提交修改的实际身份由完整 source digest 与逐文件 SHA 约束。[E152] [E153]

| 身份项 | r10f 冻结值 |
| --- | --- |
| 工作树 | `D:/workspace/eagler/worktrees/th10-rollback-smoothness` |
| HEAD | `2e5d5464b2b28ef3fb936234660b773b604c91ce` |
| common HEAD | `77666259b4bb14b965a34a1f38a4e2ec88c289ac` |
| WASM SHA-256 | `812a04bbd6d77e6c19f95e7250564af88160fc58229c11c7b4c200928a3d3a04` |
| loader SHA-256 | `e1e68a2d66d999a2b9dae56c238f5ba5aff4aac66d0efc94ad6c4c73de732efe` |
| source digest / 文件数 | `22e5b16f4bf215772b0ccc4d6bc7f97d90f44fa92192fc8882ceeb8ea211454c` / 559 |
| build.json SHA-256 | `28e7c29c4b574bfb89d2bad8fb3fa6ffe451d4dec0ea6ea8b16912d4ef8400f8` |

冻结 manifest 的八项 first-write 标志均为 true，保留已有稳定 Hint / LaserManager / Bomb / Session / GameEffects owner、触控输入前沿及远端完成显示前沿。VBO 驻留、camera cache、packed occupancy、initial-capture batch、callback timing、真实提交与 LMT1 纹理观察均默认 OFF；graph storage 保持 schema 4。新 `sdl_music_observation` 是只读导出，不把 getter 本身当作“实际音乐已运行”的证明。[E152]

#### 8 类对象、9 个分配入口：先保存旧槽，再保留原清零语义

问题是帧开始时处于 dormant 状态的槽没有 active-payload 快照；如果分配器在通知 journal 之前已经清零，随后 Touch 保存到的只是新零字节，Undo 不能恢复分配前的内容。本轮将这些入口统一为“无清零 allocate → 保留原空指针分支 → 首写 Touch / preserve → 原完整 BlockSize 清零”，不缩成请求的较小 size，也不改变普通非 MP 分配分支。[S46] [S47] [S48] [S49] [S50] [S51]

| 类型 | 生产入口 | 本轮验证边界 |
| --- | --- | --- |
| callback entry | `Callbacks::allocate_entry` | 真实 Callbacks / UpdateChain 单元；receiver 为 fixture |
| ANM overflow VM | `AnimationEngine::allocate_animation` | 真实 engine、AnmManager 创建 / 保留 / 释放单元 |
| geometry 槽 | `AnimationEngine::allocate_geometry`、`AnimationEngine::allocate`，共 2 入口 | 真实 ring opcode 84 与 AnmDistortion 单元；两入口都保留完整 64 KiB |
| Dialogue | `HudMessages::allocate` | 当前源码与清零前旧源码复核 |
| ScreenEffect | `ScreenEffects::allocate` | 当前源码与清零前旧源码复核 |
| ECL 槽 | `WorldEnemies.cpp` 的 `Script::allocate` | 当前源码复核；小节点 / scratch 请求仍保存并清零 `sizeof(EclContext)` |
| Enemy | `Enemies::allocate_enemy` | 当前源码与清零前旧源码复核 |
| Laser 槽 | `WorldLasers.cpp` 的 `Lasers::allocate` | 当前源码复核；Straight / Timed 共用 `sizeof(TimedLaser)` 的完整原槽 |

最后五个入口分布在 Hud、ScreenEffects、WorldEnemies、WorldLasers 四个文件，修改前的完整原字节与 SHA 已冻结。**没有为这五个入口分别新增真实调用单测，不能把后述三组单元的 PASS 扩写成九入口全部实测通过。** 它们的现有空池返回、无效 size 与 `w.fail()` 分支，以及完整类型 include，均按源码复核范围记录。[E154]

World / RollbackState 的帧开始仍捕获 pool metadata 与 active slots；新槽经 `TouchSlot` 扩成对齐后的完整 Storage，避免只保存 payload 或已清零片段。已有记录由 capture 路径处理，不能再随意重复添加部分重叠块。首写保存失败会锁存在 journal，common EndFrame 必须拒绝，实际 Application 也必须在成功 EndFrame 后才 MarkSimulated；**失败时已分配指针可以仍非空，初始化也可以继续写，保护点是禁止提交这个不完整帧**。这一生产接线经源码复核，后述定向单元没有执行真实 World / Application 的整条提交门。[S52] [S53] [S1]

#### Callback、ANM、geometry：已执行的 O2 / UBSan 收据

六份收据中的 candidate 都实际退出 0、无 trap、0 failures；每格列出单次运行的 checks，不能把 O2 与 UBSan 相加冒充更多独立场景。

| 专项 | WASI O2 | trapping UBSan | 独立旧行为负对照 |
| --- | --- | --- | --- |
| callback 首写 | 141 checks / 16 cases，PASS | 141 checks / 16 cases，PASS | 固定旧 Callbacks 文件：38 failures，含 18 次 full-pool mismatch |
| ANM overflow 首写 | 288 checks，PASS | 288 checks，PASS | 当前完整 engine 文件仅换回旧分配体：O2 34 failures，含 14 次 full-pool mismatch |
| geometry 两入口 | 144 checks，PASS | 144 checks，PASS | 当前完整 engine 文件仅换回两处旧分配体：O2 26 failures，含 6 次 full-pool mismatch |

callback 两份收据与原始正 / 负对照沿用第 10.37 节的证据，26 次完整 pool 比较均一致、6 次 preclear 观察无遗漏、4 次预算失败均被拒绝。[E144] [E147]

ANM 的 O2 使用 `anm-overflow-first-write-validation-02`，不能把早先 `-01` 失败尝试替换掉。O2 / UBSan 各有 20 次 full-pool 和 20 次 full-manager 比较全部一致、6 次 preclear 无遗漏、4 次预算拒绝；真实 engine 构造 / 析构各 16 次，实际 AnmManager create / create_at、脚本写入、overflow 保留与释放均执行。[E155] [E156]

geometry 的 O2 / UBSan 各有 10 次 full-pool 与 10 次 full-owner 比较全部一致，6 次 preclear、4 次预算拒绝；真实 engine 构造 / 析构各 14 次，4 次 ring-script 与 4 次 distortion 检查、20 次 full-clear 与 8 次 tail-zero 检查。两个入口都核验原来的 65,536-byte 清零范围，不能用请求 size 的首部相等代替整槽相等。[E157] [E158]

四份 ANM / geometry 收据绑定的生产 `AnimationEngine.cpp` SHA 均为 `4a1acdf3b646bdf9e5bc5f2aec10eee7fe0f61a8771729d5de1cd39ff508d22e`，与 r10f 冻结文件一致。它们没有抽取函数重写 oracle；负对照复制的是**当前完整源文件、仅反转目标分配体**，不能称为历史整文件版本。测试中的 `RollbackState::Touch` 由显式 delegate 接入真实 pool capture / common journal；生产 RollbackState dispatcher、真实 World BeginFrame / EndFrame、Application finalize / MarkSimulated、GPU、RTC、整机性能均未在这些单元中执行。UBSan 覆盖测试及所链接的 C++，vendored SoftFloat 仍为未插桩的 O2。原始 JSONL、stderr、编译参数与源文件摘要均随收据保留。[S54] [S55]

#### 真实 streaming BGM：steady 的正确性通过，cadence 仍未通过

`music-stream-cpu4-touch120-steady-01` 在两独立桌面浏览器均 4× CPU 限速、计划 120 Hz 的 native TouchController 输入、50±5 ms 单程应用发送延迟下运行；共 1200 新逻辑帧，统计窗为 frame 179→1247。phase、transport trace、Chrome trace、Profiler、像素 / 纹理观察均未开启，属于本轮的纯运行。18 个正式 OGG 在 Application 启动前加载并绑定文件 SHA；实际 native streaming decoder / mixer 在运行中使用这些音乐资源。[E159] [E160]

| steady，P0 / P1 | P0 | P1 |
| --- | ---: | ---: |
| logic Hz | 59.637 | 59.628 |
| 默认 framebuffer present Hz | 121.529 | 119.783 |
| native callback p99 / max，ms | 14.4 / 19.4 | 15.2 / 49.2 |
| present gap p99 / max，ms | 21.9 / 58.1 | 21.7 / 67.7 |
| 新逻辑前沿 gap p99 / max，ms | 33.0 / 62.6 | 32.3 / 74.5 |
| 严格 desktopCadenceTargetMet | false | false |

本次 runner / 完整 44 字 canonical / 确认音频门禁通过；这不是 cadence PASS。音乐起止观察均为 `enabled=1, fullDecode=0, streams=1, waiting=0, validDecoders=1, fullPCMBytes=0`，所选 trackMask 不变且 decoderFrames / cursorBytes 前进。例如 P0 decoderFrames 从 176,400 到 1,069,425，cursorBytes 从 15,580,752 到 19,152,852。这证明了有限运行中的实际 streaming 音乐路径和只读起止观察；不证明 PCM 逐样本相同、设备可听输出、全程无音频中断或 managed 加载路径。双方 heap 在本轮从 167,051,264 到 200,474,624 bytes，也不能仅凭两个端点把任何长 gap 归因于扩堆。[E159]

steady 原报告为 4,746,413 bytes，SHA-256 `116e83adfad285fee97346b3a8b5fa65f366339c64e2a5d33af9b7e0eb9fa9da`；runner 另行绑定 r10f WASM / loader / source digest 与音乐资源身份。后续比较必须使用这一实际报告，不把第 10.37 节的 r10e 诊断结果并入本次纯性能统计。[E159] [E160]

#### BGM spike-01：明确失败，P0 在尖峰与收尾之前已经持续偏慢

`music-stream-cpu4-touch120-spike-01` 的总报告和 runner 都为 **FAIL**。两端 native loop 均走完目标帧，随后在 `performanceSettle` 报 `Confirmed peer input stopped advancing`，最终 exact fence / 完整 44 字和确认音频门禁未完成；单端 performanceResult 的 `passed=true` 只代表 native loop 完成，不能覆盖整场失败。[E161] [E163]

| spike，frame 179→1247 | P0 | P1 |
| --- | ---: | ---: |
| logic Hz | 12.212 | 12.224 |
| 默认 framebuffer present Hz | 12.171 | 116.522 |
| native callback p50 / p99 / max，ms | 49.9 / 168.8 / 231.2 | 2.2 / 11.2 / 23.1 |
| present gap p99 / max，ms | 248.3 / 315.4 | 33.6 / 51.1 |
| 新逻辑前沿 gap p99 / max，ms | 240.8 / 309.5 | 282.9 / 394.8 |
| 严格 desktopCadenceTargetMet | false | false |

这些统计由保留的原始 rows 按同一连续前沿窗口离线提取，没有删去坏区间。P0 第 0 行在 callbackEnd 129.9 ms 时 native 已花 67.6 ms、逻辑仍为初始 frame 59；随后两行 native 分别为 39.9、76.5 ms，对应逻辑 frame 59、60。P0 全 1197 行 `highRefresh=0`、无切换，起止行的 interpolate / recovery yields / missing inputs 均为 0，运行中远端确认前沿经常领先自己的逻辑前沿。计划注入在 P1 自身时钟约 45.145 s 才开始，不能用这次有限网络尖峰解释 P0 从 row 0 就存在的持续慢回调。[E161] [E162]

P0 起止 `multiplayer_memory_status` 的 SnapshotFrames 都为 57、SnapshotBytes 都为 23,673,342，**整段新增 journal 快照数 / 保存字节数的增量为 0**；rollback / resimulated counters 都为 0→0，LiveFrames 为 0→0、LastBytes 为 0→0，heap 始终为 167,051,264 bytes。57 个累计快照是运行前已经存在的值，不能误写成“从来没有建过快照”。这排除了“这段 P0 的巨大新增快照或回滚重演量”作为已观察负载的解释；仍不能仅凭这些计数推断普通 Update / Draw、音频、rAF 调度或 GPU 的实际分段耗时。[E162] [S56]

`highRefresh=0` 在这里是分支结果和诊断线索，**不是已经证明的最初原因**；P1 的 native 仍短而逻辑被慢 P0 的信息前沿限制，也不能倒推为真实 WAN 故障。新 231.2 ms 是 native 调用的 wall duration，缺少该次完整 phase / trace，不等同物理 CPU 执行时间。旧 r10b 的 692.4 / 741.1 ms 仍是另一场未归因证据，不能用本轮 231.2 ms 或之后未复现来替它定性。[E105] [E161] [E162]

spike 原报告完整保留，14,236,244 bytes，SHA-256 `cd9258d1a411b4f33220e2ece8ccede4e2d17cbcf1013e6dfe578debe30b5c90`。错误、原始 rows、两端起止状态与实际网络注入证据均在摘要指向的原报告中；focus 只投影必要字段，没有替换原始失败。[E161]

#### Post-run 顺序修复：只处理测试收尾，尚不能证明消除运行中大卡

旧 checker 在停止两端 native loop 后、进入协作 exact fence 前，先跨 CDP 搬运两份完整 performanceResult。native 停止不等于 SessionChannel watchdog 停止：下一次 poll 仍按原 wall clock 检查确认前沿是否前进，大对象序列化 / 搬运会占用这段时间。源码能证明这种收尾风险，但旧失败报告没有精确的首次 settle marker 和 watchdog 最后进展时点，不能据此独断这次错误全部由序列化引起；更不能解释此前约 100 秒运行中的持续慢回调。[S13] [S14] [S24] [S8]

本轮只调整 `check-performance.py` 的顺序：先在每页内投影少量 completion 字段（done / passed / error、native / network error、finalNet、firstForward 与原 native 时钟），计算原来的“最大结束帧 +16”目标并协作 settle；完整 44 字 / 确认音频 exact fence 后才搬运完整 performanceResult。trace End 仍在 fence 前发送，大流读取仍在最终 fence 之后或 failure finally；Profiler.stop 的原位置未挪动。冷启动、实际 RTC、impairment、音乐、观察器、源文件身份等既有门禁保留；失败路径继续保存两端原报告和主错误，不延长 stall timeout、不重新 arm watchdog、不虚构继续输入。[S14]

新增 postRun 时点区分 Python 进程内 monotonic clock 与每页独立 performance timeOrigin，用于记录 native 完成、首次 settle、fence 完成、完整报告取回的阶段；这些计时属于测试收尾诊断。实际语法 / 源码稳定性收据在 **19:52:19.573 UTC** 完成，Python AST parse exit 0，执行前后 checker SHA 均为 `7c02faa7cb348f16c71c672a2c7fadde0e78be656863a7da809a3d207b792e54`。这里 before / after 是语法检查前后字节相同，不是旧版与新版 diff；**收据只证明语法和源码稳定，不能当作真实浏览器收尾已通过**。[E164]

#### 本节截止时的待收口边界

新安排的同冻结 r10f、360f、双端 phase / transport 与完整 Chrome trace 诊断由 ROOT 串行执行，其实际分段、trace 完整性与 postRun 浏览器结果留待后续追加，**本节不预填通过**。即使新样本没有再出现长回调，也必须保留旧 700 ms 与新 231.2 ms 的未归因状态；诊断开销存在时，结果不能替代纯 4× 验收。

下一步限于已识别的交付缺口：收回该次早期慢回调与收尾阶段证据，必要时作对应的最小运行时修复；在最终冻结字节上完成所需 Restart / 迟到 ACK、完整状态与确认音频、触控 / 实际运动，以及真实 BGM 的纯 4× 稳态与有限尖峰复验。已有 r10b Restart / 逐 seat 触控、r10e 1× 19 / 4 / 9 像素窗口继续作为原版本证据，不跨版本自动颁发 r10f 全项通过。

用户允许正常暂停传播、也允许网络信息不足时平滑降速；这不构成接受普通游玩持续慢回调、突然停顿或设备性能造成明显降速的授权。本轮没有手机测试，合成触控仍来自桌面 native API；没有 WAN、完整 Launcher / managed 触控消息或真实输入到光子覆盖。**TH10 尚未交付，TH11 尚未实施，成功的 TH08 / TH09 不因本轮实验改变。**


### 10.39 r10f 分段追踪与首次真实 BGM、4× 像素运动（2026-10-12）

本节三场运行继续使用同一冻结 r10f，未改变原生代码或打开默认关闭的优化。两端实际硬件 Chrome、4× CPU、50±5 ms 单程应用发送延迟、计划 120 Hz 合成触控和正式 streaming BGM 均保留。下面都是诊断运行，不能替代纯性能对照；逐行原始数据与失败场次继续保留。[E165] [E167] [E168]

| 诊断，P0 / P1 | 逻辑 Hz | Present Hz | native p99 / max，ms | Present max，ms | 总体结果 |
| --- | --- | --- | --- | --- | --- |
| 360f 开局，phase / transport / Chrome trace | 60.003 / 59.367 | 114.308 / 119.342 | 14.9 / 16.6；14.1 / 16.1 | 27.7 / 20.0 | 运行、完整状态与确认音频 PASS，cadence false |
| 1200f 原尖峰条件，phase / transport / Chrome trace | 58.587 / 58.630 | 115.824 / 119.916 | 15.3 / 21.0；14.1 / 18.7 | 38.8 / 61.3 | 运行、完整状态与确认音频 PASS，cadence false |
| 1200f steady，P0 实际像素 / LMT1，双方 phase / transport | 59.296 / 59.311 | 113.034 / 122.931 | 15.4 / 43.4；14.2 / 55.5 | 70.7 / 75.5 | 像素运动覆盖不足，整体 FAIL；状态 / 确认音频 PASS |

短场原报告为 2,352,827 bytes，SHA `70876bd5298bd05ebe91b547752bf0e1af9438f2f524af4155a924a140cdefed`；尖峰诊断为 6,985,659 bytes，SHA `593f8bb80b4af27dd64ef0adcfd4e0337d407d9552a56dd149dbac5b84edd570`；实际运动原报告为 33,771,140 bytes，SHA `1113f2e51ab75b0717569944e1eb3b210e54fc0867b67042f29d878c6bc84165`。摘要只是这些报告的离线投影，没有改变窗口或门槛。[E165] [E167] [E168] [E169]

#### 首回调与 trace 的实际边界

360f 的两端首条回调 native 为 5.9 / 5.7 ms，Offered / Completed / StepCalls 均为 0。NetworkPrepare 为 1.0 / 0.7 ms、Flush 为 0.2 / 0.1 ms、末尾 AudioPump 为 4.4 / 4.9 ms，未细分余量约 0.3 / 0 ms。已初始化的 mixer / SDL 队列首次补给确实可以在没有逻辑 step 的回调中发生；这次没有重现旧 67.6 ms，不能从该健康样本反推旧异常就是音频。[E165] [S2]

两个有 Chrome trace 的运行都完整保留双方起止标记、gzip、零 data loss 与收尾核验。360f 的离线 gap 分析却有一个独立失败：P0 最大回调间隔目标内存在非完整 RunTask 事件，分析器拒绝自行拼成完整区间。因此原始 trace 的完整 PASS 和该项派生 RunTask 并集分析的 FAIL 必须分开记录，不能默默略去错误事件。其余完整事件只说明各自观察范围，不解释旧约 700 ms 或新 231.2 ms。[E166]

1200f 尖峰诊断中，P0 最长新前沿间隔为 119.7 ms、P1 为 67.6 ms；最长 native 为 21.0 / 18.7 ms。信息不足期间的停止、回调内部工作和获调度前的等待仍需按同段数据分流，不能把最长新前沿间隔直接叫作深回滚 CPU 开销。[E167]

#### 收尾顺序有了实际验证，但运动覆盖仍失败

三场的 postRun 都记录：共同帧 / 非纠正状态 / 完整 44 字 canonical / 确认音频核验先完成，大报告随后才搬运。像素场次的报告搬运实际约 **21.65 秒**，而完整核验已在其之前完成；这证明先处理有限同步边界的必要性。它仍不构成此前旧 watchdog 错误的唯一因果证明，也不消除原生慢回调。[E165] [E167] [E168]

4× 像素场次实际完成 96 / 96 次 PBO 取回、189 / 189 个 LMT1 texture packets；25 项原生边界 guards 通过，无丢样、完整性错误或已定义的运动违规。元数据几何中，本地 / 远端 / 子弹分别有 **510 / 355 / 205** 个合格窗口。但最终像素只有子弹 1 个合格窗口；本地 28 个候选中 27 个只有一个样本，远端 32 个中 30 个只有一个样本，其余因静止或像素关联不确定而未覆盖。**两位玩家 0 / 0 的结果是覆盖失败，不能写成顺畅通过，也不能单凭未覆盖判定所有玩家画面错误。** [E168]

该场 P0 最大 native 43.4 ms 的同一行 Flush 为 32.6 ms，包含 opt-in 实际像素观察；P1 的 55.5 ms 位于 InputAndStep，恢复仍未结束，heap 未增长。不能把观测场次的所有差异计为运行时回归，也不能用另一端的短回调解释其具体内部长段。[E168]

后续工作先检查采集窗口是否与原有元数据资格一致，并保留全部元数据和已发生的取回；不降低像素相关性、位移或完整窗口要求。相机精确数学缓存将以同冻结字节、只切一个开关的纯运行隔离验证；此前 camera / packed 组合结果不当作单项结论。信息不足尾部制动继续按时间债务、相位和硬预测上限审查，尚未改变 TH08 / TH09 的共享默认策略。TH10 仍未交付，TH11 仍未实施。

### 10.40 单项缓存对照、rAF 同笔请求，以及保留的再次慢端失败

#### 相机缓存没有显示稳定的整作收益，继续关闭

20:07:21–20:08:04 UTC 的 OFF 与 20:09:24–20:10:09 UTC 的 ON 使用同一冻结 r10f、同一 harness 源身份，都是 1200f、两端 4×、50±5 ms、120 Hz 计划触控及真实 streaming BGM。只切 camera cache；VBO / packed / initial batch 与所有重观察均关闭，verify-every 为 0。两个完整状态 / 确认音频门禁通过；一个紧邻配对只支持这次差异，不支持独立于环境的优化百分比。[E170]

| 单项对照，P0 / P1 | OFF | ON |
| --- | --- | --- |
| logic Hz | 59.677 / 59.642 | 59.446 / 59.454 |
| native work / 新帧，ms | 11.918 / 11.551 | 11.561 / 11.851 |
| native p99，ms | 14.4 / 14.7 | 14.5 / 14.5 |
| native max，ms | 19.2 / 18.6 | 18.7 / 18.3 |
| Present max，ms | 26.3 / 57.7 | 48.0 / 93.1 |
| advance max，ms | 42.7 / 75.4 | 55.5 / 94.0 |

ON 的 look-at 命中约 88.87% / 88.89%，perspective 约 99.9945%；工作量一端下降、另一端上升，合起来没有稳定整体收益，双方 cadence 仍 false。因此 camera 继续 OFF。早先 batch 的同冻结 r9 OFF→ON→OFF 已有明显原生长回调负证据，共享 batch 实现未换；本轮不重开同一广泛实验，更不把跨界调用次数减少折算为等量 CPU 收益。[E170] [r9 三组统计](../artifacts/rollback-smoothness/candidate-r9/r9-batch-off-on-off-summary.json)

#### 网络硬窗口等待与 rAF 获调度前等待已有同段区分

尖峰诊断的 P0 最长 advance 119.7 ms 中，frame 461 / confirmed 449 已达到 B12。row 800–803 仍完成自然 alpha 0.147→0.825；row 804–813 的 callback 只约 1.8–2.5 ms、没有新 forward，alpha 已为 1，仍有 Present。补包后才开始纠正并发布新完整前沿。这段实际证明的是信息窗口耗尽后的终点停留；不能把它叫成 119.7 ms 原生重演。[E171]

同场 P1 row 2088→2089 的 61.3 ms Present / callback-end 间隔中，后一次 native 为 10.1 ms，约 51.2 ms 在 callback 之前。实际同 renderer / thread / frame 的 RAF id 2127 请求时间为 394136389307 µs，执行为 394136440100 µs，即 **50.793 ms**；等待内仍记录到 pumpTouch 与 RTC onmessage。记录允许检验“到期 timer 是否能先获得一次服务机会”，但 timer 自己也会迟到，且提交不等于显示器 scanout。[E172]

双方 Chrome trace 本身完整且零 data loss；派生 gap 分析的该段却因非完整 RunTask（ph I）保持 FAIL，不能擅自把约 46 ms 解释成确定空闲，也不能倒推出 GPU / OS / 其他脚本是唯一原因。相机 ON 的约 94 ms callback-end 间隔同样主要在 callback 外（约 82.4 ms），但这场没有 Chrome trace，证据止于 native 边界。[E173] [E174]

#### 采样规则已修正并通过单元检查，实战复测仍明确失败

旧 4× 像素场次 96 次尝试中，58 次 frame 元数据本就被既有 frozen/open 规则排除。新采集器仍保留 180–260、700–780、1100–1180 三窗口和 96 次总上限，固定分为 12 个子段，各 8 次；先按原 analyzer 能在 pending 时验证的结构条件和非零显示 XY 端点取样。RFP1 使用 display pair；不读取未来 Presented、像素或相关分数。所有真实 blit 元数据保留，ROI / getter / queue 失败先扣额度、不会补采，最终提交绑定与分析门槛没有改动。[S35] [S36] [S58]

11 项新选择测试与 49 项原 analyzer 正负对照在同一串行命令下全部通过。收据还记录 JS 语法、Python AST 和前后源码 SHA。新 capture SHA 为 `90ce2ce9ed602453606fabe1b6e1e896962e3a2bfad1c55932c4860da1374b39`，analyzer 保持 `fe4def49f3586919383cdc862898b5886fd52f05b5eb2239d5318a7653449bde`。[E175]

随后 `music-stream-live-motion-cpu4-02` 保留同一 r10f WASM 和旧原生默认，但 **P1 从开局起持续慢，整场不能通过**。原报告 43,627,932 bytes、SHA `a598c86e06f1cbc7cc29f7101ae3208404a6cd6b85697208a4b5dfdd4595e91f`。由于 checker 在 transport 完整性检查处停止，离线摘要只提取同 SHA 原 checker 的 distribution / summarize 函数；没有改写原始失败或提高通过标准。[E176] [E177]

| 再次失败场次 | P0 | P1 |
| --- | ---: | ---: |
| logic Hz | 9.870 | 9.887 |
| Present Hz | 116.851 | 9.827 |
| native p50 / p99 / max，ms | 2.1 / 11.1 / 23.3 | 61.7 / 166.9 / 248.9 |
| native >50 ms 次数（原连续测量窗） | 0 | 739 |
| Present max，ms | 163.6 | 299.6 |
| advance max，ms | 433.4 | 339.8 |

P1 首行 frame 59、Offered / Completed / StepCalls 均为 0，native **228.4 ms**：NetworkPrepare 32.3 ms、AudioPump 150.7 ms、Flush 11.0 ms，余量约 34.4 ms。第 2 行（index 2）才完成首个 forward，native 264.4 ms，其中 InputAndStep 146.2、AudioPump 99.8 ms。P1 没有开启像素 / LMT 观察，highRefresh 保持 0。分阶段数据排除了“首个慢 callback 全部来自一次游戏重演”的解释，但尚不证明音频、限速器或环境中的任何一项是唯一根因。[E176]

P0 主要随慢端等待：transport 记录达到原 2048 上限，明确 dropped 2962、complete=false；实际运动的 4096 元数据容量也耗尽，明确 overflow 9919。只有 8 次实际 PBO / 14 个 LMT1 包完成，本地像素窗口 1、远端 0、子弹 0，captureComplete=false，不能作为修复后的覆盖通过。所有这些失败保持原样，不增大容量后覆盖原记录。[E176]

共同 frame 1275、完整 44 字 canonical 和确认音频已在大报告搬运前通过；之后搬运约 40.10 秒，没有触发先前的收尾次序错误。**这项正确性与 harness 改善没有抵消约 10 Hz 的实际失败。** [E176]

当前生产候选仍默认关闭：TH10 专属网络尾部制动在首份真实输入前选择并锁定，OFF 直接走原 common InputGapPacing；ON 正在做定向验证，尚未作为 r10f 性能结果。小缺口规则与旧策略的逐值等价只适用于此前未进入深制动的序列，补包后的快速恢复本身是有意差异。另一个有限 rAF 补偿候选正在实施，两者都不能声称解释或修好首回调内的长耗时。TH10 尚未交付；TH11 尚未实施。[S57] [S3] [S4]

### 10.41 r10g：有限调度候选已构建，原始慢端失败仍未关闭

**2026-10-12 21:07 UTC：r10g 正式构建并冻结；后续浏览器实测收口见第 10.42 节。** WASM 为 `965f0b733f05c786359e952aefef3e7cd15580641994ac0e3e46d86b855a5f61`，源码摘要为 `1fe06d589aef09d1546bc31e00c04f5bdff50587848ee181ce470ba58d39f14c`；561 个输入源文件在构建前后及冻结复制时逐一核对。正式 O2、非 diagnostic，原首写保护、稳定 owner、触控输入前沿和远端完成画面前沿继续保留。新网络尾部制动与 RAF 补偿均默认关闭，可在同一个 WASM 上选择后锁定，不能把“构建成功”当作性能改善。[E186] [E187]

#### 再次启动诊断：没有复现持续慢端，仍记录到长呈现间隔

前述 live-motion-02 的 P1 持续慢端出现后，约定最多做三场短启动诊断。diagnostic-02、03、04 现已全部完成（3/3），均为冻结 r10f、360f、双端桌面 4×、真实流式 BGM、50 ± 5 ms 单程延迟和 120 Hz 合成触控；开启原生分段与 transport，未使用像素观察器。02 / 03 开启 Chrome trace；04 采用固定工作探针与 CDP ThreadTime，未开启 Chrome trace。[E181] [E182] [E189]

| 短诊断 | 逻辑 Hz（P0 / P1） | native P99 / 最大值，ms（P0；P1） | 最大呈现间隔，ms（P0 / P1） |
| --- | --- | --- | --- |
| startup diagnostic-02 | 59.581 / 59.578 | 14.6 / 17.1；15.2 / 16.8 | 54.9 / 28.9 |
| startup diagnostic-03 | 59.063 / 59.258 | 15.1 / 17.5；15.1 / 18.5 | 66.0 / 62.6 |
| startup diagnostic-04 | 59.124 / 59.224 | 14.2 / 17.3；15.2 / 17.5 | 23.5 / 46.1 |

三场完整 44 字状态、确认音频、实际 RTC 和清理均通过，双方 cadence 仍为 false。它们没有重现首回调后持续出现的 50–250 ms 原生耗时，因此不能用来解释或抹去 live-motion-02、spike-01 和更早 r10b 的约 700 ms 失败。

diagnostic-03 额外在最终状态 / 音频围栏之后、bulk 搬运之前，于原页面和原 4× 配置执行私有固定工作探针。它不导入游戏 memory、table 或 global，默认关闭时不读取时钟或构造 WASM；只执行固定次数的 JS / WASM 无符号整数运算，不调整 rAF、输入、模拟或限速。两端各 5 次正式 WASM 计时为 14.0–16.4 ms 和 14.3–18.7 ms，运算结果全部一致。此前 Node 的 8 组验证还使用独立 BigInt 参考和故障负对照。[S61] [E180] [E182]

diagnostic-04 已在原 CDP 限速会话、timeTicks 域取得有效 ThreadTime 差分。整个 native-window 的 P0 墙钟为 6.277096 秒、主线程 CPU 为 1.303779 秒，比例 0.207704；P1 分别为 6.221754 秒、1.269814 秒、0.204093。两端 native-window 与 post-fence-probe 共 4/4 个区间均可用。这是包括暖场与尾段的粗粒度区间，不是裁剪后的 179→407 帧计时窗，也不是某个长 callback 的 CPU self time。[E189]

ThreadTime 采集实现有独立的 10 组 / 72 项验证收据，检查缺失或重复字段、非有限数、计数器回退、读数失败及会话和边界归属；验证运行的是实际采集函数与 fake CDP，不构成浏览器性能实测。[E188] 现在可以区分这些正常新页面的主线程 CPU 累计与墙钟，**仍不能校准已经退出的失败进程**，也不能把固定工作探针或约 0.208 / 0.204 的比例解释为原慢端的原因。约定的三场诊断到此完成，原始坏场继续保留。

#### 网络尾部只观察缺失输入，不观察设备耗时

TH10 专属候选继续保留 B≤4 的免费预测区；B=5、6、7 的目标表达式与 common 策略逐值相同，B=8、9、10、≥11 的目标间隔倍率分别为 2.375、3.5、5.375、8。升制动仍使用原来的时间限制；从深制动恢复时先按速度 `1 / scale` 恢复，8→2 需要 225 ms，2→1 再沿用 150 ms，避免在补包后花 1,050 ms 缓慢走回原速。[S57]

B 来自保留的预测前沿与连续确认输入之间的差距，不使用已经回退的模拟 cursor、CPU 欠账或 callback 耗时。预测硬上限仍为 12；很长的断网仍可能自然等待，这个候选不能在没有输入时无限前进。开关须在首份有效真实输入前选择，回滚、retire 和 BeginNextRun 都不解锁。四组实际 WASI O2 验证——netplay-pacing、common-session-pacing、netplay-runtime、netplay-generation——均通过，源码前后不变；未在这一结果中声称浏览器性能或像素通过。[E178]

#### RAF 补偿的范围、时钟和生命周期

候选仅在已由真实 RAF 确认高刷、最新 raw RAF 样本仍快、页面可见、正常 ready、非暂停且实时 rollback 已有初始输入时允许补偿。每个 pending 真实 RAF 最多一次 timer 服务；deadline 从上一完整原生 callback 返回后再加 25 ms，包含同步 runtimeFinish 和析构。Timer 不重排原 RAF、不申请新 RAF、不重新安排自己。被覆盖或补偿完成后不足 4 ms 到达的 RAF 跳过原生工作，仍保留真实刷新率样本。[S59] [S2]

定点审查处理了三个具体边界：

- 剩余 timeout 向上取整，避免 24.5 ms 被浏览器整数延迟量化成提前到达、消耗唯一补偿。
- 旧 runtimeStopped / runtimePrepare / runtimeFinish 在同步重启后不能清理新循环；完成操作同时核对 epoch、请求 serial 和 callback sequence。
- 模拟 / 音频采用实际 dispatch arrival；raw RAF 时间戳只参与刷新率采样和覆盖判断。否则 timer 内第二个 tick 已读取到 106 ms 后，raw=105、arrival=115 的 RAF 会让原宿主时钟回退。现在 getter 使用同一个 arrival 锚与已观察时间下界，且该下界不回写 audio、elapsed 或模拟欠账；暂停和 readiness 拒绝不把等待墙钟变成补跑负担。

原生策略测试实际通过 16 组、2,850 个断言，源码前后不变；测试包含生产 cadence / debt helper，**不编译实际 ApplicationHost**。独立只读复核与之后完整游戏构建分别补上接线、SDK 和编译检查；后续真实浏览器 ON / OFF 的完整状态与确认音频结果见第 10.42 节，物体运动及整作收益仍未通过。这个候选不能抢占已经开始的长原生回调，也不能保证 compositor / 屏幕及时显示已提交像素。[S60] [E179] [E187]

#### 不把 10 ms 决策门槛误读成整次 callback 耗时

第 7 节的门槛位于 flush_network_inputs 之后、最终 presentation Draw / commit 和 audio pump 之前。`completed_after_correction` 在每次成功 forward 都会置真，并非只在发生过 rollback 的 callback 才会触发；因此源码确实可能在完整 forward 用时达到门槛时直接提交端点并取消 priming。不过，仅凭整次 native 为 15 ms，无法推出这一门槛前已经用了 10 ms。已有稳态仍存在大量 fractional presented，当前没有据此删除或提高门槛；4× 实际运动将按当时 gate 前成本和呈现状态解释。[S2]

匹配本机 Chrome 155.0.8059.39 的 Chromium 源码已单独归档：Windows 限速器使用真实主线程 handle，4× 的 helper 通过暂停 / 恢复主线程实现限速，重复设置 4 不等于 16；这只说明“callback 壁钟不等于主线程实际计算”具有实现依据，尚不能据此给某场失败归因。Performance 的 timeTicks 模式下 Timestamp 是墙钟、ThreadTime 独立来自线程 CPU 累计；其他 Task / Script Duration 仍是墙钟并可能包含嵌套工作，ProcessTime 包含其他线程，不能拿来当游戏 CPU。[E183] [E184] [E185]

三场有界启动诊断、r10g 同字节调度对照与本版菜单 Restart / 迟到旧 ACK 已完成，其结果和覆盖限制见第 10.42 节。TH10 尚未交付，TH11 仍只读准备；接下来收口远端真实运动覆盖与网络深降速恢复时的最终呈现，不能用新的正常场次关闭原始持续慢端。

### 10.42 r10g 实测收口：状态通过，流畅度仍有明确未通过项

本节对应冻结 r10g 的实际报告，WASM 与源码身份沿用第 10.41 节；诊断摘要均绑定未改动原报告的 SHA-256。RAF 与网络尾部各做一组 OFF / ON 对照，实际运动和菜单 Restart 另行记录。**完整状态、确认音频通过只说明相应门禁通过；平均 Present、native 总耗时或单场更小的最大值都不能替代物体运动与持续端点检查。** 两个调度候选继续默认关闭。[E186] [E190] [E191] [E193] [E194]

#### RAF OFF / ON：有限补偿确实运行，尚无稳定整作收益

两场均为双端桌面 4×、流式 BGM、120 Hz 合成触控和 50 ± 5 ms 单程延迟；网络尾部候选关闭，没有像素运动验收。两场完整 44 字状态、确认音频、实际 RTC 路由及清理均通过。[E190] [E191]

| RAF 模式 | 逻辑 Hz（P0 / P1） | native 最大值，ms（P0 / P1） | 最大呈现间隔，ms（P0 / P1） | 最大逻辑推进间隔，ms（P0 / P1） | 实际 timer 服务（P0 / P1） |
| --- | --- | --- | --- | --- | --- |
| OFF | 59.540 / 59.570 | 18.2 / 60.1 | 65.6 / 62.5 | 65.8 / 67.0 | 0 / 0 |
| ON | 59.138 / 59.211 | 44.4 / 42.8 | 58.9 / 46.4 | 62.0 / 55.7 | 0 / 2 |

OFF 保留原 RAF loop，guard 的 RAF / timer dispatch 计数均为 0；ON 只有 P1 得到 2 次 timer 补偿，不能据此承诺稳定收益。补偿不抢占长 native callback，也不保证浏览器及时 raster、合成或扫描输出。这一对照支持保留有界候选供定位，尚不支持默认开启。[E190] [E191]

为避免把两页相近的相对 `endMs` 误当作同时发生，另用各页 `timeOrigin + startedAtPerformanceMs + callbackEndMs` 对齐绝对时间，减去 native 总耗时估算回调起点。原行、页时钟和源报告 SHA 已归档；以下是 2026-10-12 UTC 的近似墙钟窗口，不是线程 CPU self time。[E192]

| 场次与原始位置 | P0 窗口 | P1 窗口 | 对齐后观察 |
| --- | --- | --- | --- |
| ON：P0 row 867 / P1 row 849 | 21:17:38.5948–38.6392；native 44.4 ms，其中 InputStep 44.1 ms，处于 correcting | 21:17:38.5963–38.6391；native 42.8 ms，其中 Flush 42.6 ms，没有 step | 两个不同阶段的长回调重叠约 42.8 ms，结束相差约 0.1 ms |
| OFF：P0 presents[446→447] / P1 row 451 | 21:15:10.3288–10.3944；提交间隔 65.6 ms | 21:15:10.3283–10.3884；native 60.1 ms，其中 InputStep 59.7 ms | 提交间隔与另一页长 native 重叠约 59.6 ms |

OFF 的 P0 前一 callback 结束至下一 callback 开始还有约 53.6 ms 的 native 外间隔，处于 P1 的上述长回调窗口内。这些跨进程、不同阶段的重叠只支持“共同外部因素值得继续核对”这一候选解释，不能归因于系统、GPU 或 CPU 限速；native 外间隔也不能全部写成 RAF 等待。提交时刻不等于屏幕实际显示时刻。[E192]

#### 网络尾部 OFF / ON：慢速有缓冲作用，恢复阶段仍会出现连续端点

两场保持 RAF guard 关闭，均为 4× / BGM / touch120、50 ± 5 ms 单程延迟；在 seat 1 的 frame 450 起持续 300 ms，额外注入 600 ms 应用层延迟。两场完整 44 字状态、确认音频与实际 RTC 通过。注入参数相同不代表两场每个输入包的实际到达序列相同；后续未延迟包也可能提前修复缺口，不能把配置直接解释成固定 600 ms 无输入窗口。[E193] [E194] [E195]

| 网络尾部模式 | 逻辑 Hz（P0 / P1） | native 最大值，ms（P0 / P1） | 最大逻辑推进间隔，ms（P0 / P1） |
| --- | --- | --- | --- |
| OFF | 59.041 / 59.027 | 18.2 / 18.5 | 104.0 / 45.4 |
| ON | 58.047 / 58.040 | 60.8 / 61.4 | 115.0 / 70.4 |

ON 的 60.8 / 61.4 ms native 最大值发生在约 frame 227，早于 frame 450 的网络尖峰；不能算作尖峰制动导致的长回调。判断网络制动是否缓和硬停，改看实际输入缺口 B、输入连续确认前沿、倍率、同一逻辑帧内的分数 alpha 和连续端点，而不是只比较上表最大值。[E194] [E195]

**OFF 的 H12 是可定位的输入等待。** P0 在 frame 461 达到 B=12 后，先有 alpha 从约 0.151 增至 0.823 的分数呈现；并非看到 B=12 就已经整帧静止。随后原 row 812–819 连续 8 个样本 `offered=1 / completed=0`、`correcting=0`、B=12、alpha=1，首末样本跨度 52.1 ms。row 820 的连续确认输入才由 449 前进到 466，开始纠正；row 821 进入 frame 462 并重新出现分数 alpha。因此 frame 461→462 的 104 ms 推进间隔包含分数插值、硬上限等待和恢复，不能整体记为 104 ms 画面冻结。[E195]

**ON 在对应尖峰中没有到达 H12，但更慢的逻辑推进本身不是失败。** P0 最大 B=11、间隔倍率峰值 6.535；frame 460 的 row 740–752 保持 primed / interpolate / presented，13 个样本的 alpha 从约 0.0239 增至 0.9889，跨度 90.8 ms，样本最大间隔 11.4 ms。frame 460→461 虽为 115 ms，仍包含持续变化的插值相位。这说明该场慢速确实分摊了等待，不说明已通过最终玩家或子弹像素验收。[E195]

恢复后的问题也能落到实际门槛前的成本，而不是只猜整个 callback 超时：

| ON 恢复位置 | native / InputStep，ms | 本次完成 forward 后的状态 | 同端点连续样本的首末跨度 |
| --- | --- | --- | --- |
| P0 row 754，frame 461 | 12.5 / 12.1 | high=1、completed=1、correcting=0，随后 interpolate=0、primed=0、alpha=1 | 7 个样本，41.0 ms |
| P1 row 811，frame 471 | 12.4 / 12.0 | 同上 | 6 个样本，32.8 ms |
| P1 row 818，frame 472 | 12.3 / 11.9 | 同上 | 5 个样本，26.5 ms |
| P1 row 824，frame 473 | 15.2 / 11.6 | 同上 | 5 个样本，23.5 ms |

这里 InputStep 单独已超过 10 ms，所以最终 Flush 之前必定越过原预算门槛；与冻结 r10g 的 `CommitDisplayedEndpoint / Unprime` 分支一致。P0 后续样本 B=0、没有新 forward，倍率仍由约 4.807 降至 3.731，于是保持已经提交的端点，直至下次 forward 建立新插值对。**直接把 alpha 变成 1 的原因是预算分支提交端点；倍率恢复延长了下一次 forward 之前的等待，不是把旧分数相位重置为 1。** 本段保留的预测前沿也没有随纠正 cursor 回退而虚增余量；RFP1 不是全局 priming 的直接控制者。这不等于已经排除所有对象呈现问题，仍需要真实运动验收。[S2] [S4] [S5] [E195]

连续端点的首末样本跨度只是观察下界，不等于精确像素冻结时长。固定 frame 450–480 的窗口内，OFF / ON 的 P0 端点段观察下界合计为 52.1 / 41.0 ms，但 P1 为 37.0 / 129.9 ms；两场该窗口墙钟长度和包到达过程不同，不能把单端改善写成全局收益。现有证据支持继续处理深降速恢复时的端点呈现，网络尾部开关仍默认 OFF。[E195]

硬上限仍有必要的等待边界：连续确认输入长时间不前进，有限预测深度和最高 8 倍间隔只能延后耗尽，无法保证永不等待。已经收到更后面的某帧，也不能绕过更早的输入缺口任意向前推进。正常暂停时一端停止产生新输入，另一端最终停下属于预期行为；这里要改善的是正常游玩网络波动时，从免费预测区到缓慢推进再到必要等待的过渡。[S4] [S5] [S57]

#### 真实 4× 运动：采集完整、零已检出违例，但远端覆盖仍为 FAIL

`music-stream-live-motion-cpu4/01` 使用冻结 r10g、双端桌面 4×、流式 BGM 与 120 Hz 合成触控，两个新调度开关均关闭。P0 页面同时观察本地和远端玩家。它取得了完整的有限采集，但分析器明确返回 `passed=false / coverage.met=false`；不能因为没有违例就改成通过。[E196] [E197] [E198]

| 项目 | 实际结果 | 结论范围 |
| --- | --- | --- |
| 合格像素窗口 | 本地玩家 9、远端玩家 0、子弹 11 | 本地与子弹已有证据；远端覆盖未达标 |
| 像素 / 纹理采集 | PBO 96/96、纹理 181/181；captureComplete=true | 没有排队溢出、残留或读取错误 |
| 分析违例 | 0 | 只覆盖实际合格窗口，不能补足远端 0 窗口 |
| native 最大值 | P0 20.1 ms / P1 19.4 ms | 本场没有复现持续慢端，不解释旧坏场 |
| 逻辑 Hz | P0 59.529 / P1 59.521 | 双端 cadence 仍 false；平均提交频率不证明连续物体运动 |
| 完整状态与确认音频 | frame 1275 围栏通过 | 与运动覆盖门禁分别记录 |

postRun 时钟记录显示：native 循环完成后先做完整状态 / 确认音频围栏，随后才进行约 20.422 秒的 bulk 数据搬运。它属于离线报告采集的成本，不能算成游玩期间一次 20 秒停顿；也不能反过来抹去此前原生循环中的任何长间隔。原始 31,523,206 字节报告完整保留，SHA-256 为 `8aee00245298f3a833975ef548f3e44ac10944ae47b0d305312813a1fa2b6986`。[E198]

远端采样份额修正正在实施，尚未构建或验证；原 motion-01 的 FAIL 保留，后续必须在新证据中取得远端合格窗口，才能收口这一项。

#### 本版菜单 Restart / 迟到旧 ACK：生命周期通过，性能范围不扩大

`manual-restart-late-ack-01` 的 `passed`、`lifecycleCompleted`、`canonicalPassed` 均为 true，错误列表为空；两端身份均为冻结 r10g。该场通过实际 RTC 和两个独立桌面浏览器进程，使用原生暂停、菜单选择 Restart、确认操作，并核对迟到旧 ACK 与新一轮生命周期。canonical 比较覆盖相同 LastSimulatedFrame、且输入已确认到该帧时的全部 44 个 schema-2 字。[E199] [E200]

这份 checker 明确使用 stopped native ticks，输入配置为固定 D1、reserve=2、automatic=false；它不是 RAF 调度、4× 性能、BGM、手机、WAN 或完整 Launcher / managed 流程验收。只把本版这一条有限 Restart 生命周期证据记为通过，不转移旧版本其它测试的覆盖范围。原报告为 20,461,165 字节，SHA-256 为 `284a1159789a43f75b3b45102d7848d3d9e27d480cf080ce528abc86651428af`。[E199] [E200]

#### 当前决定与下一步

RAF 补偿与网络尾部制动继续默认关闭；现有同字节对照尚不能承诺稳定的整体改善。网络深降速恢复时的窄呈现例外已进入实施，仍由 tail OFF 保持默认关闭，不提高全局 10 ms 门槛、不延期已显示画面、不跳过有语义的 GPU 工作；尚未构建或验证，不能写成修复完成。远端运动采样份额修正同样只记录为实施中。

下一步以新候选的实际端点段和本地 / 远端玩家、子弹运动证据验收，再核对状态、确认音频与生命周期。原始 r10b、r10f 的长回调和持续慢端失败原样保留；三场正常启动诊断及本次正常 native 运动场均不替它们归因。[E105] [E161] [E176] [E177] TH10 仍未交付，TH11 维持只读准备。

### 10.43 r10g 运动失败复核与 r10h / r10i 完整帧末呈现候选

本节记录第 10.42 节之后的增量，验收状态收口到 2026-10-12 22:23 UTC 的 r10i 正式冻结与 Stage 组件收据。前节“实施中、未构建”是当时状态，历史数据不倒填；此时尚无 r10i 浏览器性能或运动结果。

#### r10g：透明度改善后，远端仍缺少足够的连续运动窗口

两场均为桌面双端 4×、120 Hz 合成触控、实际 RTC 和流式 BGM，RAF 与网络尾部候选均关闭。native loop 及最终 state / audio fence 完成，但整场 motion gate 因远端窗口不足而 FAIL；没有已检出运动违例不等于通过。[E201] [E203] [E204] [E207]

| 原始场次 | 本地 / 远端 / 子弹通过窗口 | PBO 完成 / 尝试 | 固定窗远端 fractional moving | 其中透明度支持 / 已采 |
| --- | --- | --- | --- | --- |
| `music-stream-live-motion-cpu4/02`，orbit | 4 / 0 / 6 | 52 / 52 | 77 | 4 / 4 |
| `separated-lanes-live-motion-cpu4/01` | 5 / 1 / 7 | 66 / 66 | 21 | 21 / 17 |

原始 orbit 报告为 29,181,248 字节，SHA-256 `dc4c9e05978bb54451debef23b1b3709155157590f2eb8c0e9f2e3064eb5a64c`；lane 为 30,117,343 字节，SHA-256 `2a81ec93ef0c3f872d7d52c3d32de094854586dec009ac165fef7b352cef584e`。紧凑 metadata 与像素结果都绑定这些未改动的原报告。lane 的远端像素窗口分解为 19 个单样本、14 个合法静止、2 个不足 1px、1 个通过；不能通过拼接新 identity / revision、静止画面或背景变化补足覆盖。[E202] [E205] [E206]

在 lane 的 430 条固定窗 metadata 中，仅用相邻 callback 的 rollback / resim 增量判定，114 次纠正加完成 forward 有 101 次提交 authored endpoint，其中 97 次 InputStep 本身已达 10ms；剩余 13 次留下远端 moving 的只读插值提交。另有 50 次对象可接受的普通无重演 forward，49 次远端端点完全静止，另一次只移动约 0.01px。源码的实际 endpoint Present 会消费 RFP1 pair，下一 forward 的相同目标自然成为静止 pair；不能删除消费或在已显示终点后重放旧位移。metadata 的 `FrameWasOpen` 是 authored Draw 当时的标志，不代表回调返回仍未完成。[E205] [S1] [S2] [S40]

按固定窗紧凑 metadata 中的同 identity / 同端点对、连续 commit、递增 fractional alpha 与透明度支持条件，远端潜在配对只有 5 组，其中达到 1px 的仅 2 组：frame211 的 1.529px 已通过；frame213 的 index298→299 对应约 3.437px，但第二张因 section 配额耗尽未采，只能保留为未覆盖机会。第三窗还包含 53 条 owner 未见、16 条 NoQuad、68 条视口裁剪，它们不能算作预算 gate 丢失的可显示运动。下一步不降低 1px、相关性或唯一性门槛，也不覆盖这两份原 FAIL。[E205] [E206]

#### r10h：真实 Stage 边界通过，首场未命中追加补绘

r10h 于 22:06:57 UTC 正式冻结，`profile=multiplayer`、`diagnostic=false`；562 个输入源文件逐项验证。WASM SHA-256 为 `c24d528aa3072e7d3b1bca0b4028869d40a3e92c575197f938bdf5c4593e2444`，sourceDigest 为 `76bfe2cbc908e3736264d52207d326d8097ca1b009d6abd06562138da2d8a209`。完整语义 Draw 及其 GPU 工作继续执行；例外只决定完整 forward 之后是否追加一次只读末呈现，不修改输入、两前沿、End / Mark 或网络 debt。[E208]

资格在本 tick 开始、真实 authored Draw 前、submit 消费请求前后累计。current / previous Stage 的 fade、visibility、owner / 资源变化，以及 capture、screenshot、暂停、加载、错误等边界形成 sticky veto；这些瞬时资格留在 host Application，不进入 journal / 协议。真实 WASI32 O2 StageRenderer、Timer、Camera、GraphicsMath、AnmRenderer 组件在 ordinary / multiplayer 两个 variant 各通过 6 case、1,446 检查，包含“只在 Draw 后检查会漏掉最后 fade”的负向反例。它记录设备边界，未执行完整 Application 生命周期、RAF、最终像素、音频或性能验收。[S62] [S63] [S64] [E209] [E210]

`network-tail-diagnostic-pair-01/on` 的完整 44 字状态、确认音频、实际 RTC 及清理通过，逻辑为 58.132 / 58.193 Hz，native 最大值 49.1 / 51.0ms；两端严格 `desktopCadenceTargetMet=false`。它启用了 phase / transport / ThreadTime 等诊断，不是纯性能验收。schema 2 的真实 `Accept=0、Committed=0`（两端皆同）；满足较早资格的深缺口 forward 有 4 / 3 次，均被 `GateOutsideWindow` 拒绝，不能把该场运行通过归功于未发生的追加 Draw。[E211] [E212] [E214]

h 原报告为 7,023,545 字节，SHA-256 `662f963c53a09d042e3b15785760f9798b58f625edee0f55c6b6e3084fd0c794`。同源离线提取进一步要求 decision7（此前资格已通过）、`scheduledInputGapScale∈[1,3]`、double / float alpha 均在内部、真实 gate 在 `[10,14)`、boundaryFlags=0，得到下表；这是直接读取 gate 时钟，不是用整个 callback 或 InputStep 代替 gate。[E213]

| h 的浅缺口潜在机会 | 数量 | gate p50 / p90 / p99 / max，ms | 原 flush p50 / max，ms |
| --- | --- | --- | --- |
| P0 | 379 | 11.1 / 12.9 / 13.8 / 13.9 | 0.0 / 0.7 |
| P1 | 353 | 11.2 / 13.0 / 13.7 / 13.9 | 0.1 / 0.5 |

这些回调尚未承担追加只读 Draw，不能据其时间或次数预判 i 的费用、像素结果或提升幅度。

#### r10i：只扩大完整帧呈现资格，正式构建不等于浏览器改善

r10i 仅取消呈现资格中的 `scale>3`，改为校验 finite 且 `scale≥1`；保留真实最新 fast RAF、高刷、恰好一个完成 forward、完整且非纠正 / 非 open 前沿、double / float 内部 alpha、`[10,14)` 和全部 sticky veto。decision7 保留为 r10h 的历史含义，不重新分配；仍绑定既有 `NetworkGapTailEnabled()`，默认 OFF，没有新产品开关。它不改变 Runtime、输入、RFP1 `Consume`、语义 Draw / GPU 工作，也不把旧 pending picture 延到下一 RAF。[S62] [S2] [S40]

正式构建于 22:22:41 UTC exit0，562 个输入源文件前后不变并完成冻结。WASM SHA-256 为 `35a3d60abf2691163c96233d93bd4012cae67861e5b6175c2251c17177a1663a`，sourceDigest 为 `e5ec2e58a1c957e476446514b9fbf1efa3f1e965e859486febe9153d12e81c2e`。Stage 复测两个 variant 各 6 case、1,489 检查通过，562 源与 i 身份匹配；新增覆盖正常 gap 与准入边界，组件覆盖范围仍与 h 相同。[E215] [E216] [E217] [E218]

**本节收口时尚无 i 浏览器效果。** 补绘前的 `[10,14)` 是准入区间，不是最终 callback 耗时保证；追加 Draw 可能增加主线程工作和下一 RAF 的等待。必须用 h / i 同 tail ON 负载比较实际 Accept / Committed、远端连续像素运动及回调 / advance / Present 尾部，再按完整状态与音频门禁判定。h 的潜在机会数、Stage PASS 和正式构建均不能先写成收益。旧约 700ms、231.2ms、248.9ms 失败继续保留；未测试手机，无 WAN / 完整 managed 验收，TH10 未交付，TH11 仍只读准备。

### 10.44 r10i 实际补绘与 WS relay：有限运动证据通过不等于游戏无停顿

本节承接第 10.43 节“i 尚无浏览器效果”的历史状态。以下 h / i matched RTC、两场 WS 对照及随后 1800f 的像素加 transport trace 场均已回收，原报告保持原字节；不能用后来的正常或失败结果替前场定因。

#### matched RTC：追加 Draw 已发生，远端连续像素覆盖仍不足

两场均使用双端桌面 4×、120 Hz separated-lanes 合成触控、50±5 ms 单程应用层延迟、流式 BGM、tail ON、RAF guard OFF。h / i 是相同设置下的相邻场次，各自绑定自己的正式 WASM；包含计时、像素与主线程诊断，不作纯性能或完全相同包到达序列的对照。[E219]

| 指标，P0 / P1 | r10h | r10i |
| --- | --- | --- |
| 逻辑 Hz | 59.037 / 59.001 | 58.739 / 58.683 |
| Present Hz | 111.248 / 112.228 | 100.616 / 106.572 |
| 实际 Accept = Committed | 0 / 0 | 334 / 388 |
| Accept 回调 flush 中位数，ms | 无实际命中 | 2.0 / 1.8 |
| 本地 / 远端 / 子弹通过窗口 | 4 / 1 / 4 | 7 / 0 / 4 |
| 完整 44 字状态、确认音频、RTC | 通过 | 通过 |
| 整场运动覆盖 / 严格 cadence | 未通过 / false | 未通过 / false |

i 的 Accept 回调 native 中位数为 13.7 / 13.3 ms，gate 中位数为 11.5 / 11.4 ms；准入窗口仍是追加工作之前的 `[10,14)`，不保证最终 callback 小于 14 ms。当前结果只证明原来被 gate 阻止的补绘确实执行，不能根据次数宣布整体收益，也不能用一次诊断场的差值推导纯性能退化比例。h 原报告 30,978,465 字节、SHA-256 `7f9e5a4b933148fa4123b14f2ba078eade48745126f9b7a5fc12cf2d1b798134`；i 为 33,060,288 字节、`712baf8fea0e486f41cc9656a8d466928df3adffaa731605cd40002da7d0371c`。[E224] [E225]

三段固定窗 metadata 的远端分解如下。连续对要求相邻真实提交、同观察身份 / revision / 端点、时间间隔不超过 50 ms；这是像素采集机会，不替代完整资源、形状及最终纹理门禁。[E220] [E221]

| 远端机会 | h | i |
| --- | --- | --- |
| 固定窗 metadata 数 | 453 | 423 |
| 合格 fractional moving；均满足透明度要求 | 90 | 118 |
| 其中已完成像素采集 / 因 section 配额耗尽未采 | 31 / 59 | 35 / 83 |
| 同 pair 连续移动相邻对 | 35 | 40 |
| 其中不足 1 px / 达到 1 px | 28 / 7 | 33 / 7 |
| ≥1 px 且两张像素 complete | 3 | 1 |
| ≥1 px 但只采首张 / 两张均未采 | 0 / 4 | 1 / 5 |

i 的 7 个 ≥1 px 相邻对只属于 6 个端点 frame，f757 有两对，不能重复计算独立覆盖。i 固定窗中，以实际 Accept 起始的 46 组远端 moving 窗口有 33 组只留下一个同 pair 合格提交，13 组形成多样本。原 analyzer 的远端窗口为：h 共 40 个，其中单样本 25、合法静止 8、亚像素 4、纹理歧义 2、通过 1；i 共 54 个，其中单样本 44、合法静止 7、亚像素 2、纹理歧义 1、通过 0。新 frame、身份或 revision 必须结束原窗口；`FrameWasOpen` 仍是 authored Draw 时刻的观察边界，不能称为玩家暂停。[E220] [E221] [E222] [E223] [S34] [S36]

i 唯一完整 ≥1 px 对是 `pixels-44-1`，f244、index306→307、alpha 0.060554→0.942554。真实 quad 位移约 1.07694 px，CPU 纹理 96 支持点、opacity 0.501961；期望位移相关性 0.898754，高于原地 0.572290 和其它候选 0.610255，却未达 0.92，因此保持 `pixel-match-ambiguous-or-occluded`。不能降低 1 px、0.92 或 0.08 唯一性门槛补 PASS。f735 的 1.26439 px 对只采到首张；f712 / 737 / 757 / 758 的其余 ≥1 px 机会均缺完整采样。本轮 fractional 透明度淘汰为 0，未覆盖不等于像素静止。[E221] [E223] [S35] [S36]

#### 完整前沿与已经显示的位置不同，不实施无界 lastDisplayed 重锚

RFP1 的 `Freeze` / `Publish` 继续使用旧完整 `to`，而非上一张 fractional picture 的显示位置；只有实际 endpoint Present 后才 `Consume`。在 metadata 的相邻提交、稳定观察身份、相邻逻辑 frame 且 `new.from==old.to` 子集中，h / i 有 47 / 51 次旧 moving pair 尚在内部 alpha 时进入下一 pair，其中旧剩余位移 ≥1 px 为 11 / 18 次；下一 pair 本身静止的为 5 / 14 次。i 的 f237→238、index296→297，旧 alpha0.214554 后进入新静止 pair，实际提交 quad 一次补上约 3.13739 px；该例未采像素，只证明几何行为。[E220] [E221] [S40] [S1]

正常连续插值不必恰好显示 alpha1，不能把新 frame 换 pair 本身判成运动违例，也不能删除 `Consume` 后重放已经显示的 endpoint。本轮不实施“每帧任意取 lastDisplayed 作新 from”：若没有累计误差、最大滞后和明确收敛边界，反复重置显示起点可能不断拖延跟上完整目标，产生额外显示迟滞。下一项显示方案必须先覆盖匀速跨 pair、纠正后新静止 pair、真实方向反转、已显示 endpoint、identity / 资源变化及 capture / fade 边界；当前未构建或验收这种方案，不承诺它更流畅。

#### 初始本地 WS fallback：协议与有限运动通过，运行中推进停顿仍阻断交付

测试 shim 的 16 组离线用例、三项 Node 语法检查及 checker AST 均真实 exit0，收据 `passed=true、sourceUnchanged=true`；覆盖 Input 分类、每个当前 relay socket 的 FIFO、消息字节、生命周期与计数。生产输入协议未改：延迟发生在 WebSocket.send 之前，匹配的 Input 消息保持 FIFO，非 Input 控制消息绕过测试队列。本轮使用 50±5 ms、无 drop / spike；实际路由从初始握手即为本地 WS fallback，未覆盖公网、TCP 包丢失 / 重传或 RTC→WS 动态迁移。[E226] [S65] [S66]

`websocket-relay-live-motion-cpu4/01` 的握手、native loop 与最终围栏均观测到 route2。observer 只在 P0 世界中观察两名玩家及一颗自然子弹，所得本地 / 远端 / 子弹窗口为 **7 / 6 / 2**，PBO **96 / 96** 完成，纹理包 192 / 192 接受、无 dropped / binding / integrity 失败；这是有限自然窗口的最终像素证据。完整 44 字 canonical、frame1275 的确认状态和音频围栏通过，bulk 报告搬运仍在 fence 之后。[E227] [E230]

| 该 WS 场的 measured window | P0 | P1 |
| --- | --- | --- |
| 逻辑 Hz / Present Hz | 48.837 / 104.959 | 48.830 / 111.309 |
| native 最大值，ms | 44.5 | 41.0 |
| Present 最大间隔，ms | 60.7 | 55.6 |
| 最大逻辑高水位间隔，ms | **3145.6** | **3143.6** |
| 缺口端点 | f1176→1177 | f1175→1176 |
| 本页 native 运行相对时间，ms | 19614.2→22759.8 | 19516.4→22660.0 |

**像素门通过没有解除这次运行中的大停顿。** 两端 native 目标停止帧均为 1259，以上缺口显然更早，不能归为一端完成测试后正常停采输入。每端最长缺口的上下文各保留 409 条连续 callback；取缺口起止 row 的 405 条，最大 callback-end 间隔为 34.2 / 29.6 ms，native 最大值为 16.3 / 14.7 ms，没有三秒 RAF 或 native 空白。本场没有 transport trace，持续 callback / Present 只能划清这段长停顿的时间边界，尚不能定位输入 hole、ACK、队列或修复路径的具体原因，更不能推给设备 / OS / 环境。[E228] [E229]

原始 WS 报告 **39,149,508 字节**，SHA-256 `47061c94f6b69f8ec85d47ab5bd3d460d5868190cc31184c0c1f52f0d1415e26`。checker 的运行、状态和有限运动门可返回 PASS，但严格 cadence 为 false，这场仍不满足“正常游戏不能突然整段停住”的交付目标；原报告不改写为性能通过。[E230]

#### 无像素的 transport trace 场未复现，不为前场定因

随后同冻结 r10i 的 `websocket-relay-transport-diagnostic-cpu4/01` 关闭像素采集、启用有界 transport trace。两端逻辑为 **57.842 / 57.895 Hz**，最大 advance 间隔 **67.2 / 61.7 ms**；transport trace 为 enabled / valid / complete，0 dropped、0 readErrors。这里的 trace 是约 20 ms 的原生与队列 metadata 观察，不是 Chrome CPU 采样或逐包到达证明。完整状态 / 音频通过，严格 cadence 仍为 false；该场没有像素运动验收。[E231]

其原报告 **5,957,158 字节**，SHA-256 `b0a915e8695542eb71c56d73d45352bbef98b684a72284a745a3e9a12911f784`。没有复现不能证明前场由 PBO、观察器或某项网络修复计时器造成；两组观测配置不同，原失败进程也已退出，不作因果消元。[E232]

#### 1800f 像素加 trace：容量失败与真实慢端同时存在

同冻结 WASM 的 `websocket-relay-live-motion-cpu4/02` 两端正常完成 native 目标 1859，随后 exact1875 的完整 44 字状态、确认音频与 route2 通过。checker exit1 的首个失败为 P0 transport evidence 不完整：2048 条上限后 dropped 3307；P0 像素 metadata 4096 条上限后溢出 11081 条，有限运动窗口仅 1 / 1 / 0、captureComplete=false。已经记录的 32 次 PBO 读取全部完成不等于整场观察完整；可用的局部 trace 仍保留，不能声称覆盖末段或整场。[E233] [E235]

这次也不能只称作容量问题。两端 native loop 总时长为 132.2 / 130.2 秒；下表是**全循环**回调与计数，包含开头和尾部，不是 checker 裁剪后的 measured window。[E235]

| 全 native loop | P0 | P1 |
| --- | --- | --- |
| native p50 / p99 / max，ms | 2.4 / 13.6 / 45.1 | **38.8 / 88.0 / 104.0** |
| rollback / resimulated frames | 946 / 9109 | 32 / 84 |

P0 最大前沿缺口为 f581→582、13.030 秒，P1 为 f590→591、12.2715 秒；远早于 1859 目标停止，期间仍有大量 native 回调。P1 只有 32 次 rollback、84 帧重演，不能将它整个慢端概括成回滚重演负担。P0 trace 容量已失，局部记录可继续定位，但不扩大成完整网络因果证明。[E234] [E235]

同一存活页面在 state / audio fence 之后、bulk 之前执行独立固定 JS / WASM 算术，五组测量结果均数值一致：P0 WASM 为 14.7–14.9 ms，P1 WASM 为 68.6–126.5 ms，P1 JS 五组为 143 / 148.6 / 111 / 91.6 / 117.5 ms，完整范围 91.6–148.6 ms。探针不读取游戏状态，也不测某个 callback 的线程 CPU；它证明本次 P1 连独立固定工作也出现墙钟耗时异常，尚不能单凭时间认定 Chrome CPU 限速实现故障，更不能用于追认旧 231.2 / 248.9 ms 失败的原因。[E233] [S61]

02 原报告 **72,194,314 字节**，SHA-256 `d97136a28441e29479ad5bf9deddc03e2995565681f425c9ccbd936fc45c5322`。下一步拟在同一个存活的异常页面、最终围栏之后做 4→1→4 固定工作校准，并保留每段原始耗时、限速会话与页面身份；本节没有该校准结果，不把新进程结果当旧进程校准。[E236]

此前约 700 / 231.2 / 248.9 ms 原生慢回调、WS01 约 3.1 秒前沿停顿、WS02 容量及慢端失败，以及 h / i 原始运动覆盖 FAIL 均保留。候选默认关闭，不改 TH08 / TH09 的成功默认；无手机测试，无 WAN / 完整 managed 验收，TH10 未交付，TH11 尚未实施。

### 10.45 relay 收包空窗的定位边界、独立尾部开关与 r10j 冻结

本节更新当前工作状态，不改写第 10.44 节的原始失败。**TH10 仍不可交付，TH11 尚未开始实施。** 本轮缩小了一类网络推进停顿的观察边界，并拆开信息降速与额外补绘；没有取得足以解除旧慢端、长回调或运动覆盖失败的性能结论。

#### r10i hop 诊断：收发 API 之间存在空窗，native 与 callback 仍连续

命令 `fmiVCC6m5KZyNdYqkJRqxur9` 实际 exit0，归档 `candidate-r10i/websocket-relay-hop-diagnostic-cpu4/01`。双端本地 WS route2、frame1275 的完整 44 字状态与确认音频围栏通过，浏览器 transport 和 Node hop 证据完整；本场没有像素运动验收，也不是无观察器的纯性能场。[hop-focus-01.json](../artifacts/rollback-smoothness/candidate-r10i/websocket-relay-hop-diagnostic-cpu4/01/hop-focus-01.json)

| measured window，P0 / P1 | 实际结果 |
| --- | --- |
| native 最大值，ms | 22.9 / 19.4 |
| callback 到达最大间隔，ms | 33.4 / 24.9 |
| 逻辑高水位最大推进间隔，ms | **1538.2 / 1556.8** |
| 对应缺口 | f271→272 / f270→271 |
| 严格 cadence | 两端 false |

Node 同一 `performance.now()` 时钟上，上一条 `ws.message` 完成于 **17212.5454 ms**，下一条开始于 **19040.8435 ms**，中间 **1828.2981 ms** 没有新的已观察游戏消息。期间四次 500 ms heartbeat 仍执行，实测间隔约 510.2 / 511.9 / 515.2 / 513.7 ms；这不支持“Node 整个事件循环同步阻塞约 1.8 秒”的解释。浏览器时钟仅通过各自记录的 `timeOrigin` 映射，不能直接比较两页原始 `performance.now()`。[node-gap-detail-01.json](../artifacts/rollback-smoothness/candidate-r10i/websocket-relay-hop-diagnostic-cpu4/01/node-gap-detail-01.json)

限定在上述 Node 空窗内部的浏览器样本，P0 底层 `send` 返回计数为 **408→459（+51）**，P1 为 **415→464（+49）**；接收计数却分别固定在 **412 / 404**。这将本场的待查范围缩小到 **浏览器底层 send 已返回 → Node 触发 ws.message** 之间；它没有证明字节何时进入内核、网络栈何时交付、WebSocket 何时完成解码，也未定位根因。不能把本次空窗直接归为 ACK 相互等待、Chrome 限速、TCP 或 OS 故障。[stall-table-01.json](../artifacts/rollback-smoothness/candidate-r10i/websocket-relay-hop-diagnostic-cpu4/01/stall-table-01.json)

本场 native 耗时正常，不能豁免 WS02 中同页私有固定算术持续变慢与长 native 回调，也不能解释旧约 700 / 231.2 / 248.9 ms 失败。API 空窗和持续慢端仍是分别保留证据、分别定位的问题；新健康进程不能回头校准已经退出的异常进程。[E105] [E161] [E233] [E235] [E236]

#### 深缺口 pacing 与额外只读 Draw 已分离

当前源码的成功 live Rollback `Reset` 默认采用 **深缺口 pacing ON、额外只读 Draw OFF**。保留原 4f 免费预测区间、B≤7 的原有渐进规则及 H12 上限；网络信息不足仍可逐步降速，不能为了关闭尚无稳定收益的补绘而一并关闭深缺口 pacing。额外 Draw 仅控制原先的窄补绘例外，不跳过每个逻辑帧必需的 semantic Draw。两个选项共享首次输入后的锁定边界，跨 generation 保留已选值；`Clear`、播放与观战采用中性状态。[NetplayRuntime.cpp](../th10_web/cpp/multiplayer/NetplayRuntime.cpp) [ApplicationHost.cpp](../th10_web/cpp/sdl/ApplicationHost.cpp)

原 pacing 的 6-double ABI、callback timing 的 25-double ABI 与 Decision 枚举不改；新增独立 3-double status。JS 默认 `null` 只读当前值，兼容缺少新导出的旧冻结 i；显式要求某项能力而导出缺失、选项已锁或返回不合法时，必须拒绝，不能静默当作已配置。[NetworkExports.cpp](../th10_web/cpp/multiplayer/NetworkExports.cpp) [network-gap-tail.mjs](../portable/multiplayer/network-gap-tail.mjs)

两次验证的实际范围分开保留：

- `aybbPLvq4e5sIxgOnsqXOw9X`：`netplay-runtime / netplay-pacing / common-session-pacing` rules 子检查 exit0；随后 Stage 命令漏传必需的 `--output-dir`，整体 **exit1 / passed=false**。这是实际 CLI 失败，不能抹去，也不能称 Stage 已执行通过。[首轮 receipt](../artifacts/rollback-smoothness/network-tail-split-native-validation-01/receipt.json) [Stage stderr](../artifacts/rollback-smoothness/network-tail-split-native-validation-01/stage-presentation.stderr.log)
- `ah_mCZPWBZP4_fpNpb1h59Ay`：补正参数后，`network-tail-split-validation-02` 实际 exit0；真实 Stage / Timer / Camera / ANM 组件的 ordinary、multiplayer 各 **1489 checks** 通过，真实 JS helper 的 **8 组 ABI** 检查通过，所列源码前后未变。02 的实际 checks 只有 Stage 与 JS ABI，未重新执行上一条 rules；组件结果不覆盖 Application 全生命周期、浏览器像素、音频或性能。[第二轮 receipt](../artifacts/rollback-smoothness/network-tail-split-validation-02/receipt.json) [Stage stdout](../artifacts/rollback-smoothness/network-tail-split-validation-02/stage-presentation.stdout.log) [ABI stdout](../artifacts/rollback-smoothness/network-tail-split-validation-02/split-options-abi.stdout.log)

#### raw TCP 观察器的离线证明与真实路径证明分开

`oNJ90hnpWHiL7_0gNCI4KndZ` 实际 exit0，`relay-hop-validation-04` 的 server-semantics 为 **35 组 / 433 checks**。本轮实际执行项只有 `server-syntax / server-semantics / runner-syntax / checker-syntax`；receipt 的宽泛 scope 或源码清单不代表 browser 观察器、校准、summary 用例在这一轮也重新执行。离线覆盖的是关闭时不读取 raw socket、真实 EventEmitter 嵌套顺序、异常透传、固定容量、所有权恢复和不可用状态；不是已经拿到真实 TCP 数据路径的根因证据。[relay-hop-validation-04/receipt.json](../artifacts/rollback-smoothness/relay-hop-validation-04/receipt.json)

#### r10j 已构建，真实 socket 场仍出现 13 秒推进停顿

正式构建 `w6XNb4-Ci1XwiZxISbbsSQxA` 已 exit0，562 项源码前后未变；r10j 冻结 WASM SHA-256 为 `bce8491fb8d0a9c187ff1ee862677ab73575d915b4b4da3d6a7d2c142d301e5b`，sourceDigest 为 `25d88156ed1b2dc933f0d053461ad937f33001a6efa4a5cad4a5ba83fb2b2567`。这是新独立选项的正式构建身份，不把旧 i 的诊断结果转记到 j。[r10j frozen.json](../artifacts/rollback-smoothness/candidate-r10j/runtime/frozen.json)

`hlKqBiixq_68Z1qNqOkK6N_9` 的 `candidate-r10j/websocket-relay-socket-diagnostic-cpu4/01` 已确认 exit0，但**流畅度目标仍为 FAIL**：P0 / P1 最大逻辑推进间隔为 **13650.6 / 13508.6 ms**，native 最大值仅 **38.4 / 21.4 ms**，callback 到达最大间隔 **42.8 / 65.8 ms**。Node raw data 与 ws.message 均有约 13.75 秒双向空窗，Chromium 已绑定实际 gameplay socket 的写日志仍持续，最大发送间隔约 84 / 81 ms；待查边界进一步缩小到 **Chromium socket write 观测 → Node data dispatch**，仍不是线上 ACK 或根因证明。这里只记录结果与边界，详细时序关联另行补充。[socket-brief-01.json](../artifacts/rollback-smoothness/candidate-r10j/websocket-relay-socket-diagnostic-cpu4/01/socket-brief-01.json)

该场 performance SHA-256 为 `fd832ff5b6e854e088ea61b612b56f1a0db8392940189f210e76ce461720eb84`，relay-hop 原始报告 SHA-256 为 `9d658623c9f2a0b733a65532a6e1f3205cfb9723eb80717925e75ac557872f47`。nig 轮询曾出现 FRP404，后来已确认原命令完成；没有据此重启同场或把文件存在当作通过。下一步沿实际 socket 的写入、raw data、ws.message 边界核对缺失时段，同时保留旧持续慢端的独立调查。状态 / 音频、真实运动和 native 耗时各自验收，不以命令 exit0 代替连续流畅游戏的交付判断。

### 10.46 r10j 后续：socket 空窗、完整回调分类与紧邻 4× 校准

截至本节记录，**TH10 仍不可交付，TH11 未开始实施**。本节所有游戏样本均为桌面 Chrome、双端 4×、BGM stream、120 Hz 合成触控；没有使用手机。socket 与 CPU 两条调查分别保留，不能用一条的新证据消除另一条旧失败。

#### socket 的 13.697 秒共同空窗已核到同一条实际连接

以 Node 最后一次 raw dispatch 结束到下一次 raw dispatch 开始为共同边界，区间为 **27472.6679 → 41170.0881 ms，长 13697.4202 ms**。映射的实际连接为 P0 Chromium request370 / job372 / socket374（本地 57309）和 P1 request296 / job298 / socket302（本地 55330），都连到 relay50068；HTTP50069 是另一条连接。[E237]

在该共同空窗内，P0 / P1 的 gameplay socket 分别仍有 **387 / 379 次 SocketBytesSent，102840 / 83562 B**，最大写间隔 **61 / 59 ms**；两端 SocketBytesReceived 均为 0。Node 没有 raw / message / relay send，但有 26 次约 500–515 ms 的心跳，整场心跳最大间隔约 562 ms。游戏仍有 callback，H12 / scale8 / input-wait，也没有连续 13 秒 native 计算。因此边界是 Chromium socket write 观测至 Node JS raw dispatch，而不能写作“WASM 卡死 13 秒”或“Node 主线程完全阻塞 13 秒”。[E237]

这里的 SocketBytesSent 不是线上 ACK 证据，NetLog 的跨时钟 offset 也有精度限制。Windows 网络栈、过滤层与 Node I/O 的根因尚未获得；没有据此修改代理、安全软件或系统配置。该场也不能倒推旧 i 场缺少 raw / NetLog 的停顿原因。

#### 恢复后的单次 raw dispatch 本身也有可处理的成本

两个实际 64 KiB raw dispatch 中，P0 raw6082 耗时 **43.7602 ms**，包含 242 次 message / send；P1 raw6824 耗时 **144.7251 ms**，包含 295 次 message / send。P1 的 message inclusive 为 136.1125 ms，内嵌 send inclusive 为 118.3889 ms；这些时间互相包含，不能相加。[E238]

消息头显示是普通 Input 尾部重发，不能因 latest/count 相同就认定完整 payload、ACK 或控制语义相同。当前不做“只留最后一包”：sequence 半环比较存在具体反例，旧序号包也可能填补缺失帧。研究中的候选改为**保留所有帧和字节，只在同一次同步 raw dispatch 内，对同一目标 socket 做有上限的底层写合并**；首条发送立即执行，每批最多 8 包 / 64 KiB（含 WS 头），退出时归还本模块自己的 cork 层，未知、异步积压和外部 cork 情况旁路。

此候选尚未接入正式 relay，也没有实际收益结论；原锁定 relay 保持不变。必须统计最终 uncork 和真实 write/writev，不能只把原 send 时间搬到清理阶段，再声称优化成功。即使它消除了 145 ms 恢复 burst，也不能据此声称修好了前面的 13.697 秒输入空窗。

#### r10j RTC 原像素场的失败：不能把 933 个历史标志当成 933 次未完成恢复

`8An_kO2ZHxphQWdIANEYxo7M` 的 1200f RTC 像素场为 **exit1**，完整 44 字 canonical 和 frame1275 确认音频先通过，随后本地 / 远端 / 子弹自然运动覆盖仅 **0 / 0 / 1**。P0 / P1 逻辑为 **49.834 / 49.708 Hz**，实际呈现为 **46.364 / 124.710 Hz**，native 中位为 **14.0 / 3.3 ms**，最大为 **57.2 / 41.9 ms**。这是当前真实运动失败，不能用后来正常的新进程覆盖。[E239]

原 metadata 的 `frozen-replay-or-open-frame` 原因有 933 次，其中 `FrameWasOpen` 是在 BeginDraw 记录后保留的历史位，并不表示 runtimeFinish 时事务仍未结束。按真实 correcting、completed、累计 rollback / replay 增量重算连续测量窗口：[E240]

| 回调类别 | P0 | P1 |
| --- | ---: | ---: |
| 部分恢复，未呈现 | 216 | 147 |
| 完成恢复并推进新帧 | 615 | 509 |
| 普通 forward | 322 | 538 |
| 无新帧、无恢复 | 60 | 1633 |

P0 完成恢复并推进的 615 次 callback 中，真正 fractional 呈现仅 3 次；P1 为 152 次。普通 forward 的 fractional 次数为 68 / 512。P0 的普通 forward native 中位也有 12.8 ms，不能把它的慢完全解释为回滚次数较多：窗口中 P0 重演 2141 帧，P1 重演 2477 帧。[E240]

没有因此放宽像素 oracle。Authored Draw 的 alpha1 终点不是同一完整前沿对上的 fractional 插值，也不能从前一张图继承 RFP1 pair。修正原因文字本身不会把 0 / 0 覆盖变成已测成功。

该原像素场没有启用 ThreadTime 和 4→1→4 同页面校准，页面已经退出；其 post-fence 固定 WASM 算术 P0 在 20.3–40 ms 间波动，不能补测过去的进程。后续独立运行只能说明后续运行。

#### 三场紧邻 600f：packed 单开与恢复关闭均没有取得可承诺收益

三场均使用同一 r10j WASM，RTC、50 ± 5 ms 单程延迟、separated-lanes 触控、无像素 / 无 NetLog / 无 transport trace。均开启旧分段计时、ThreadTime、独立固定算术，以及状态 / 音频 fence 后同页面 4→1→4 校准。三场完整状态、frame675 音频、路由和身份检查通过；严格 cadence 全部未通过。诊断带观察开销，不是纯性能验收。[E241] [E242] [E243]

| 场次 | P0 / P1 逻辑 Hz | P0 / P1 呈现 Hz | native p50 ms | native p99 ms | 最大逻辑推进间隔 ms |
| --- | --- | --- | --- | --- | --- |
| OFF 初始校准 | 58.650 / 58.641 | 106.879 / 103.421 | 6.1 / 6.3 | 16.5 / 17.5 | 60.2 / 59.7 |
| 仅 packed ON | 45.394 / 45.491 | 41.902 / 119.216 | 17.3 / 3.8 | 33.6 / 16.4 | 62.7 / 60.8 |
| 紧邻 packed OFF | 45.301 / 45.651 | 41.883 / 113.793 | 16.5 / 4.0 | 36.4 / 16.4 | 95.7 / 56.9 |

初始 OFF 的独立 WASM 算术在原 4× 下大致 14.7–19.2 ms，1× 为 3.2–3.3 ms，恢复 4× 后约 14.9–16.1 ms。仅 packed ON 的 P0 原 4× 为 **27.1–31.8 ms**，1× 为 3.3–3.4 ms，恢复 4× 仍为 **17.8–34.2 ms**；紧邻 OFF 的 P0 原 4× 为 **17.2–30.6 ms**，恢复 4× 为 **20.3–29.5 ms**。[E241] [E242] [E243]

关闭 packed 没有恢复本次 P0 的推进性能，所以没有证据把 ON 时变慢单独归因于该算法，也没有收益证据支持默认打开。**四个旧可选优化（VBO 驻留、camera math cache、packed 扫描、初始捕获 batch）继续默认关闭。** 固定算术证明同页面执行速率会波动，尚未证明具体系统成因，更不能消除旧长 callback 失败。

### 10.47 新增独立 callback 工作分项：实现与验证范围

为避免继续只看宽泛的 InputAndStep，本轮接入独立、默认 OFF 的 **32-double / schema1** 诊断；旧 25-double / schema2 保持原布局。新行按 callback、epoch、Application owner、generation 和前后 native frame 绑定，在 runtimeFinish 之前一次性发布完整结果；getter 不暴露正在组装的行。[S67] [S68] [S69]

八类互不嵌套的墙钟分项为：forward capture、history capture、forward Update、history Update、forward semantic Draw、history semantic Draw、Restore、ConfirmedCommit。只记录 Host 原生 input 与 Application::step 的作用域，包括实际 spectator extra step；只读高刷呈现、ReplaySeek 整批、加载和停止后的完整 hash / 固定算术不混入。OFF 不读时钟、不分配堆内存、不扫描世界。

计量口径必须保留：Capture 包括捕获遍历、journal 元数据和复制；Update / Draw 仍各自包含执行期间的首写成本；semantic Draw 仅覆盖 `engine.draw_all()`，外围 begin_scene / flush 留在 residual。ConfirmedCommit 包括 Application 显式外层音频、Replay 和确认退休调用，不能再给 World 内层重复计时。此前确认音频提交确实位于 InputAndStep 内，Host 的 AudioPump 并没有覆盖它。

原 InputAndStep 减八类时间之和得到 residual，原始负值不截断；若超过 1e-6 ms 容差则标为无效。计时嵌套、未关闭作用域、clock 倒退 / 非有限数、过期 callback token、owner 改变都有明确诊断标志。诊断 flag 和 invalid 行原样保留，不把它们改成零时长正常样本。

`Y5Zw3XJ3rfrUoTsGaGeJI43m` 已完成 WASI -O2 fake-clock 组件验证：**16 cases / 160 checks / 0 failures**，源文件前后身份一致，测试 WASM 为 `c2d4d25aedb15d9149215e496f7bca4761594dea5c5611a91ada18cbdc00508a`。覆盖关闭 / 父开关关闭、提前退出、八类槽位、嵌套负控、未发布回调、stale 析构、owner 变化、时钟异常和 residual 负控。[E244]

这项组件测试不编译游戏调用点，不是已经获得的性能改善。后续独立 r10k 构建和真实 JS / Python / 双端分项结果见下一节；不改变历史预算、输入预测、降速或像素门禁来使诊断通过。

### 10.48 r10k 正式诊断：实际 Draw 成本、启动失败与采样边界

#### 构建和诊断验证已经完成

`KInFtRp3IxIVGPZjk9RLsebD` 正式 -O2 multiplayer 构建完成并冻结，diagnostic=false、THPRAC 关闭；563 个源码文件在构建前后逐一一致。WASM 为 `93888dcfb07e1971426796637e1df0274de2115c2f8ba8640a7f5640d83a9658`，sourceDigest 为 `b1e9d41055bd462925c219ee59ca0ef8afefd6b6dbc0ad305c49cad6b592d486`。八类分项仍默认 OFF，旧 25-double ABI 和原游戏默认开关均保持；这份构建编译了实际四处 native 调用点。[E245] [E246]

JS / Python 验证分两次完成：初次 JS 19 组 / 416 断言、Python 18 组 / 879 断言及原 summary 8 项 unittest；之后补上观察器失败时仍停止并拆除诊断、首个错误保留、不可打印异常和恢复后不伪造零样本，JS 最终为 **23 组 / 449 断言**。第二次只重跑实际改变的 JS 范围，Python 结果仍绑定第一次的相同源码哈希，没有写成重测。[E247] [E248]

两个启动失败原样保留。`rtc-work-phases-cpu4/01` 没有进入游戏，原因是新增 helper 未加入 smoke HTTP 的显式路由表；Node 导入测试不足以覆盖真实页面服务。该场在 `window.multiplayerSmoke` 初始化处超时，未设置实际 4×，不计为 BGM / 触控 / 游戏性能样本。随后一个 HTTP 定向检查又因使用 PORT=0 触发既有 1024 下限；未改变 server-core 门禁。修正检查为分配实际端口后，smoke、performance driver 和新 helper 的 HTTP 200、JavaScript MIME 与源码字节身份全部验证通过，自有服务正常关闭。[E249] [E250] [E251]

#### 第一场 600f：Draw 约占一半 InputAndStep

`HHNVxSrvxQc7xqVZs5zWatM8` 的 `rtc-work-phases-cpu4/02` 使用同一冻结 k WASM，双端桌面 Chrome、4×、50±5 ms 单程消息延迟、BGM stream、120 Hz 分离轨迹合成触控，启用八类分项、ThreadTime 和同页固定算术校准；未启用像素、NetLog 或传输 trace。完整 44 字状态、675 音频围栏及实际 RTC 路由通过，新增诊断所有观测行有效、两端正常关闭，但 **cadence 均未通过**。[E252] [E253]

| 指标 / 累计墙钟 | P0 | P1 |
| --- | ---: | ---: |
| 同一 468 新帧窗口逻辑 Hz | 48.8875 | 49.2875 |
| 呈现 Hz | 43.2348 | 134.3265 |
| native p99 / 最大 ms | 31.0 / 40.3 | 15.1 / 22.3 |
| 呈现最大间隔 ms | 91.2 | 55.4 |
| InputAndStep 累计 ms | 7436.4 | 3829.4 |
| 前进 / 历史 Capture ms | 482.6 / 561.5 | 257.5 / 377.0 |
| 前进 / 历史 Update ms | 529.9 / 1229.7 | 308.0 / 659.8 |
| 前进 / 历史语义 Draw ms | 2158.7 / 1729.8 | 970.9 / 894.2 |
| Restore / ConfirmedCommit ms | 224.1 / 30.4 | 81.6 / 14.3 |
| residual ms | 489.7 | 266.1 |

Draw 合计占 InputAndStep 的 **52.29% / 48.70%**，Capture 为 14.04% / 16.57%，确认提交不足 1%。这给出本场优先级：先找 Draw 中可以精确保留语义的重复计算和输出成本。它没有证明 Draw 全是 GPU 时间，也不能据此删除历史 Draw。捕获中的 first-write、inactive 复用和 owner 生命周期仍须保留。

同页固定 WASM 算术中，P0 原 4× 的五次为 40.4 / 19.6 / 23.0 / 22.8 / 20.9 ms，1× 为 3.2–3.6 ms，恢复 4× 为 21.1–35.4 ms；P1 原 4× 为 15.5–18.8 ms，1× 为 3.0–3.3 ms，恢复 4× 为 16.2–18.2 ms。页面与限速会话身份、校验数值均通过。这些事实保留了当页速率不对称，尚未证明系统原因，不能给旧慢端归因。[E253]

#### 第二场 CPU profile：Stage / 投影值得继续拆查，但记录间隔仍有长尾

`0eSMg0WOGNNB9QU_pBxI6ua3` 在相同冻结 WASM 与 600f 条件上给 P0 增加 500 µs 请求间隔的 CPU 采样；完整路由 / 状态 / 音频门禁通过。逻辑为 52.6233 / 52.5671 Hz，native 最大 27.5 / 25.0 ms，**固定 row 记录点的最大间隔仍为 282.6 / 287.9 ms，并非直接 rAF 入口测量**，呈现最大 277.7 / 283.9 ms。两端 cadence=false，没有像素运动验收。不能把采样场较快于前场写成代码优化收益，因为游戏代码相同且观察器不同。[E254] [E255]

P0 profile 共 11,003 个样本，累计 delta 11.687688 秒，无负 delta。精确绑定同一 k 的 symbols SHA `674d15e34edb9fb378b91b738886b7bc55d03dc56d0c9074f1c12b49a9393d34`；按 `wasm-function[N]` 的 N 查表，不能混用别的构建或偏移索引。全程采样 inclusive 中，`Backgrounds::draw` 为 2149.543 ms，`StageRenderer::draw_objects` 为 1892.239 ms，`BackgroundDraw::project_points` 为 376.800 ms；这些范围互有包含，**禁止相加作为独立开销**。[E254]

采样权重是 profiler 的墙钟间隔，不等于线程 CPU；还含启动、driver 和只读高刷呈现。历史 Draw 在该 -O2 构建中部分内联到 AppLoop::update，共用的 draw_all / AnmRenderer / Camera 叶名称不足以判定前进、历史或呈现。下一步结合祖先栈和源码边界，优先处理具体 Stage 投影 / 绘制热点，并另行保留上述记录间隔长尾。relay 候选保持独立验证，不声称它能修复此前 13 秒的输入接收空窗。


### 10.49 r10l 的有据修改、启动失败与本轮收口

#### Item 调用前判断：已构建，尚无性能收益结论

旧 k 的冻结 WASM 字节证明，`Item::draw`（function 657）先执行 `stack -= 1344`，随后才读取 `state` 并让 inactive Item 返回；实际 World 绘制调用者（function 1632）的两个循环直接调用它，没有外层 `state` 判断。这是存在无用入口成本的证据，不能把 1344 字节栈预留说成同等字节的清零、复制或堆分配。r10l 仅在 `ItemManager::draw` 的 regular / faith 两个循环逐槽检查 `item.state`；保留遍历顺序、后续槽的即时状态读取、内部 guard 和完整 active Draw，没有删历史语义 Draw。[E256] [E258]

正式 -O2 multiplayer 构建已 exit 0，diagnostic=false，563 个源码前后一致。相对 k 的 native 修改只有 `th10_web/cpp/game/ItemDraw.cpp`；冻结 WASM SHA-256 为 `bf891c7ca82ce80c2e4c97d697ac422074214811bb227f198ac8030ee2f15b20`，sourceDigest 为 `5f25ca18ac687b18111b06e0c0a25c630341a3f325e5d6a684536c94704ed1e5`。冻结清单与构建收据分别绑定完整身份；此前 fake-clock 组件不覆盖这次真实 Item 调用点的游戏验收。[E257] [E258]

#### 首场完整 44 字失败仍是失败

`rtc-item-gate-profile-cpu4/01` 在双方 nextFrame=60、lastSimulatedFrame=59、confirmedThrough=59 时报 `Initial full 44-word canonical differs`，差异为 word 1 / 9 / 10 / 11 / 18 / 28 / 29。报告的 `cpuThrottle=[]`；检查发生在设置限速和开始性能测量之前，不能称为一场“4× 优化无效”或“4× 性能通过”。原报告为 3,821,907 B，SHA-256 `0f66144007c01838d919c1d72bd609271067f0317fdc9222617fa277946ff934`，原始 FAIL 与差异投影均保留。[E259] [E260]

已保存的局部证据中，session / spell / results payload 字节相同但 owner 地址不同；Hints 的 16 个差异词对应各端自身 owner 哨兵，Stage 的文件内部偏移 144 / 964 / 1876 / 2052 一致，object_animations 指向各端已保存的 stage-vms。它们能解释这些具体字段的合法重定位，**不能证明完整对象图等价**。该场仅保存 initialComparison / initialSceneOwners，没有 initialGraphStorage；GUI / ANM 等关联资源身份、完整链与所有者关系仍有证据缺口。现有 raw canonical 含地址，不能把“看起来像指针”的差异直接掩掉，或以局部 payload 相同改判整场成功。[E259] [E260] [S19] [S29]

#### relay 组件的失败、修正与通过范围

`relay-bounded-writes-validation-01` 的第 16 组 needDrain 用例失败：夹具过早降低 highWaterMark，使首个立即发送就触发 needDrain，正确旁路后自然没有该用例预期的 borrowed batch。第二轮只修夹具时序，在第一帧正常返回后降低阈值，由真实第二次 write 触发 needDrain；没有伪造 flag。两份收据的生产模块 SHA 均为 `28df819ce385f6e3e7e617258e7606b5e38120e512979b864b9af1e81542ce51`，测试源码 SHA 改变，第一轮 exit 1 证据保留。[E261] [E262]

`validation-02` 在 Node 24.13.0、锁定 ws 8.21.3 下为 **36 组 / 1125 项 PASS**，语法和实际组件进程均 exit 0，运行前后源码身份一致。范围是实际 Node Writable / Socket、合成 raw 来源与 loopback 目标的字节、顺序、背压及清理边界；它不是浏览器、游戏、WAN 或此前 13 秒接收空窗的性能修复证明。[E262]

#### 计时名称与尚未验证的候选

第 10.48 节 k profile 的 282.6 / 287.9 ms 来自固定 row 记录点的间隔。`callbackEndMs` 在 `runtimeFinish` 中、native 返回及部分诊断读取之后记录，**不是直接采到的真实 rAF 入口时间**；这些长间隔仍需保留，但不能仅据该列断言浏览器 rAF 入口整段被阻断，或拆出主线程空闲 / CPU 耗时。[E255] [S13]

[BackgroundProjectionCache.hpp](../th10_web/cpp/platform/BackgroundProjectionCache.hpp) 与[既有定向测试](../tests/background-projection-cache-test.cpp) 已补两级 exact integer hash prefix memo：容量仍 256，完整 82-word key / 最终比较、数值 miss 路径和 FP flags 均保留。**截至本节收口，该新前缀候选尚未运行测试，也不属于 r10l 的冻结字节，不能列为验证通过或已有游戏收益。** 本轮没有获得可承诺的整作流畅度改善；后续已安排的有限复测不得覆盖本节启动 FAIL、旧持续慢端、长回调或网络接收空窗。

### 10.50 前缀验证完成，但交付候选仍保留 r10l

#### 已验证的内容与正式身份

第 10.49 节“前缀尚未运行”是当时状态。随后实际 WASI -O2 组件已 exit 0：**2073 次数值比较、204 次 post-flag 转换、216 个前缀边界、10 个观察开关 / fallback 边界全部 PASS**，包含原输出字节、FP mode / flags、errno、碰撞、alias 与 count fallback；源码前后哈希一致。它证明定向等价及整数循环边界，不证明整作性能收益。[E263] [E264]

r10m 正式 -O2 multiplayer 构建 exit 0，diagnostic=false，563 源前后一致；相对 l 的 native 修改仅 `BackgroundProjectionCache.hpp`。冻结 WASM SHA-256 为 `5c97dcc8b92cb431b24aca31d8d2893900400da84b1ed94242bd6893b6a3f31b`，sourceDigest 为 `5f80b3ae5e6386930d3e575997fd0683604b4a60f66cc2980ee195aa350b8871`。l 的原冻结字节继续独立保留。[E265] [E266]

#### 实际 l / m 对照没有支持接受前缀优化

l 的 `rtc-item-gate-profile-cpu4/02` 已通过完整 44 字与确认音频，但 P0 逻辑只有 **25.18 Hz**、native 最大 **72.5 ms**，同页私有固定算术核也出现慢态。这场不能计为 Item 优化收益，更不能改写首场启动 FAIL。[E267]

随后紧邻运行冻结 l / m：双端桌面 4×、RTC、600f、BGM 与 120 Hz 合成触控；**CPU sampling OFF**，分项计时和 ThreadTime 诊断 ON，未测最终像素，因此也不标成纯性能验收。两场运行、完整状态和确认音频门禁通过，结果如下。[E268]

| 同一测量窗口指标 | l：P0 / P1 | m：P0 / P1 |
| --- | ---: | ---: |
| 逻辑 Hz | 59.64 / 59.60 | 57.99 / 57.82 |
| native 最大 ms | 16.5 / 18.6 | 26.9 / 27.3 |
| Present 最大间隔 ms | 26.8 / 27.5 | 74.8 / 47.4 |
| advance p99 ms | 28.7 / 28.0 | 40.1 / 39.4 |
| strict cadence | false / false | false / false |

m 在没有 Profiler 的该场，post-fence 私有 WASM 核仍分别耗时约 81.2–109.9 / 139.8–183.6 ms，而 l 为约 14.3–16.2 / 14.0–17.9 ms。这说明慢态不能仅归因于 Profiler，也不能把两场全部差异直接归因于前缀实现；**现有证据没有给出接受 m 的净收益理由**。因此 m 保留为隔离实验，交付候选继续冻结 l，不将定向等价 PASS 当作采用性能优化的理由。[E268]

#### k 的长间隔得到更窄的事实边界，仍未归因

k 既有 282.6 / 287.9 ms 记录间隔已按同一页面时钟核对：两端 native 序号分别连续为 **410→411 / 429→430**，间隔内分别有 **32 / 14 条触控采样**，已记录的重叠 LongTask 为空。连续序号不支持“中间许多 native callback 被漏记”，触控记录也不支持整段所有 JS 均停止；但 LongTask 未记录并不证明空闲，row 时间仍非直接 rAF 入口，不能据此确指 OS、GPU、Profiler 或其他外因。[E269]

本轮后续验收固定为冻结 l 的 `rtc-delivery-motion-cpu4/01`，1200f 真实像素场已启动，结果尚未在本节回收。当前可陈述的是 l 已出现较好的有限计时样本；严格 cadence、像素运动和旧异常的适用边界仍须分别报告，**不宣称 TH10 已可交付**。

### 10.51 两场 1200f 已回收：像素覆盖不足，慢端仍在

第 10.50 节等待中的 `rtc-delivery-motion-cpu4/01` 已结束，**exit 1 的原因是像素覆盖不足，不是发现运动违规**：本地 / 远端 / 子弹通过窗口为 **0 / 0 / 0**，violationCount=0；28 次像素读取全部完成，errors / dropped 均为 0。两端最终完整 44 字相等、确认至 frame 1275 的音频围栏通过。单样本窗口占主要部分，另有 geometry-not-covered 与不足 1px 的位移，仍不足以证明运动连续，不能降低门槛改判成功。[E270] [E271]

随后 `rtc-delivery-timing-cpu4/01` 在同一冻结 l 上关闭像素、CPU profiler、ThreadTime、八类分项和算术 probe，仅保留原 25-double callback 计时；**exit 0 覆盖运行 / 状态 / 音频，strict cadence 两端仍为 false**，最终像素未测。两个 1200f 场均为桌面双端 4×、RTC、BGM stream 与 120 Hz 合成触控。[E272] [E273]

| 指标 | 像素场 P0 / P1 | 轻量计时场 P0 / P1 |
| --- | ---: | ---: |
| 逻辑 Hz | 52.66 / 52.61 | 54.58 / 54.61 |
| native 中位 ms | 12.4 / 2.8 | 12.3 / 2.7 |
| ordinary-forward 中位 ms | 11.6 / 5.6 | 11.6 / 5.6 |
| native 最大 ms | 37.6 / 22.4 | 30.9 / 28.0 |

**去掉像素 observer 后，ordinary-forward 的两端差距仍相同，不能用像素诊断解释整个慢端。** 这不排除观察器自身有成本，也不把两次耗时差异自动归到某个外因。原报告身份分别为 15,957,782 B / SHA `8a309679eaa87757dfd9fe2c13265ddeee8a8f05af11d42ebe92814efe936528`，以及 5,094,133 B / SHA `586fae16ba0ebdb90f586d0cc86089f435c10c233e45703d15c8a38f50ab5922`；两场原始结果独立保留。[E270] [E271] [E272] [E273]

窗口证据暴露了一个尚未控制的配置条件：两端 DOM 均记录位置 `(10,10)`、outer `817×689`，inner `800×600`。同一机器实际显示器边界是 `1463×914`、工作区是 `1463×866`；DOM 的 screen / avail `800×600` 属于浏览器仿真值，不能用来当作监视器尺寸。**这只能确认配置缺口，不能证明实际遮挡状态或它是慢端根因**，更不能据此给旧长回调及网络空窗归因。[E274] [E275]

现已加入受控 `overlap / tiled` fixture：两组共同采用 HTML viewport `672×520`，游戏 canvas 属性与绘制缓冲严格保持 `640×480`，两组之间只改变窗口位置，并以原生工作区及边界采样检查放置结果。它与旧 `800×600` 场不构成只改变位置的直接对照。[S14]

首两次 `rtc-layout-overlap-cpu4/01`、`/02` 均在测量前退出，`cpuThrottle=[]`：01 的 CDP 外框在只请求位置后由 `689×609` 变为 `690×610`；02 的 CSS rect 为约 `639.998413×479.996857`，canvas 属性与 drawingBuffer 仍为 `640×480`。这些原因没有被断言为 DPI 或取整故障。fixture 随后仅改为记录 setup 每轴不超过 1px 的尺寸变化，并固定后续 applied 外框；CSS rect 目标与前后差允许 `1/64px`，canvas 属性和绘制缓冲仍严格相等。这不是游戏、性能或像素运动门槛放宽；两次原始失败保留，**目前仍无有效 layout A/B 结果**。[E276] [E277] [S14]

### 10.52 位置对照已回收，并排仍未解决慢端

同一冻结 l、双端 4×、RTC 单程 50±5 ms、BGM 与 120 Hz 合成触控的 1200f 位置对照已完成。overlap03 的 checker / runner 虽 exit 0，性能仍严重不合格；tiled02 两次边界均 `nonoverlapping=true`，也未达到 strict cadence。两场均未测最终像素运动。[E278]

| 指标 | overlap03 P0 / P1 | tiled02 P0 / P1 |
| --- | ---: | ---: |
| 逻辑 Hz | 10.653 / 10.535 | 51.137 / 51.352 |
| Present Hz | 9.978 / 19.446 | 47.596 / 137.343 |
| native 最大 ms | 332.0 / 329.1 | 36.5 / 21.3 |
| Present 间隔最大 ms | 1116.9 / 1932.8 | 93.2 / 36.5 |

两组内 viewport 均为 `672×520`，canvas 属性 / drawingBuffer 严格为 `640×480`。tiled02 外框 P0 `690×610`、P1 `691×610`，1px 边框差被明确记录。此前 tiled01 因 P1 位置请求后宽度 `689→691` 而在测量前拒绝；fixture 现记录 setup 每轴 ≤2px、peer 外框差 ≤1px，后续仍严格核对 applied 尺寸，CSS rect 容差仍为 `1/64px`。这些只修正放置检查，未改变游戏、性能或像素门槛，也未证明 DPI / 取整原因。[E278] [E280] [S14]

tiled02 中限定 `completed==1` 且累计 resim 未增加，P0 / P1 的 InputStep 中位仍为 **9.1 / 3.6 ms**（113 / 551 个样本）。并排没有消除慢端；边界位置也不能证明全程 OS 遮挡状态。两场不构成根因证明，更不能给旧大卡免责。[E278]

m 的 header 与测试已撤回，**563 项生产源码逐项匹配冻结 l**；测试恢复单独记录了归档末尾去掉一个 LF 的规范化。恢复收据 sourceDigest 为 `5f25ca18ac687b18111b06e0c0a25c630341a3f325e5d6a684536c94704ed1e5`，冻结 WASM 为 `bf891c7ca82ce80c2e4c97d697ac422074214811bb227f198ac8030ee2f15b20`。这是源码恢复核验，不是新的构建、部署或性能通过；m 与历史失败证据保留。[E279]

补记：双 4× 的 foreground-seat0 请求场已回收，P0 / P1 逻辑 **51.945 / 51.796 Hz**、Present 52.147 / 46.687 Hz、native 最大 41.1 / 34.1 ms、Present 间隔最大 82.1 / 86.2 ms，同类单前进 InputStep 中位 **6.9 / 7.4 ms**，慢端没有简单交换；窗口布局 / 前台分支至此收住，不据此归因或改游戏，下一场单玩家端 4× / 对端 1× 将与旧双 4× 失败分开记录，结果待测。[E281]

### 10.53 玩家端 4× / 参考端 1× 仍未达标，四开关未见整体收益

两场均使用冻结 l，P0 CPU 4× / P1 CPU 1×、foreground-seat0 请求、tiled 布局、RTC 单程 50±5 ms、1200f、计划 120 Hz 桌面触控及 stream BGM。这是独立的使用条件，**不替代此前双 4× 的失败**。checker / runner 的 passed 均为 true，各场两端完整 44 字和最终确认音频一致；strict cadence 均为 false，最终像素运动未测。[E282] [E283] [E284] [E285]

对照场仅在 P0 将 `model_quad_residency / camera_math_cache / packed_occupancy_scan / initial_capture_batch` 设为 `seat0-on`，P1 仍关闭；原生状态确认四项实际启用。[E284] [E285]

| 指标（P0 / P1） | 四项 OFF | 四项仅 P0 ON |
| --- | ---: | ---: |
| 逻辑 Hz | 56.872 / 56.901 | 54.272 / 54.319 |
| Present Hz | 58.084 / 146.823 | 60.286 / 159.840 |
| native 最大 ms | 24.7 / 5.7 | 37.9 / 8.7 |
| Present 间隔最大 ms | 69.0 / 49.5 | 104.4 / 32.0 |
| 无重演单前进 InputStep 中位 ms | 5.7 / 0.8 | 7.0 / 0.7 |

四项 ON 虽增加部分 Present 计数，P0 的逻辑速度、长尾与同类前进耗时未显示整体收益，**四项继续默认 OFF**；一次组合对照也不能逐项归因。120 Hz 是输入计划频率，P0 实际提交仅约 37.92 / 39.88 Hz，存在合并，不能描述为实际每秒收到 120 次触控。两个原报告分别为 5,185,054 B / SHA `fb2f68f866a32fb6e8458e418ca5cbd2596304a6d5a6372fae0dba124a402969`，以及 5,718,355 B / SHA `b0d18336e2b2559a40c56da827b5f6bdc1b71dafff36bc9c3fccaaacd58901e1`，各自完整保留。[E283] [E285]

下一项仅为默认关闭、TH10 专用的 full LTO 构建候选：`--th10-lto` 使用独立 `multiplayer-lto` 产物 / 对象目录，保留 O2 与原浮点设置；当前构建仍在进行，尚无构建结果、正确性或性能收益结论。[构建入口](../portable/build.mjs)

### 10.54 LTO 未获整体收益证据，已撤回至冻结 r10l

r10n-build-01 的编译 exit 0，但 `free` 导出缺失，冻结检查失败；导出差异及 `rejected-runtime` 均保留，不能把编译成功写成接口通过。build-02 显式保留 `_malloc / _free` 后，构建与接口检查通过，563 项源码前后稳定，冻结 WASM 为 `6340295e2c035b066e6250134ff7e1287bce51048a17de022f69b9877b960977`。[E286] [E287] [E288]

首场 `rtc-foreground0-cpu4-reference1/01` 在结束边界发现 P1 窗口从要求的 `(710,10)` 变为 `(668,122)`，几何门禁失败，原场保留且不得用作性能对照。[E289]

有效 02 场仍为 1200f、P0 4× / P1 1×、RTC 单程 50±5 ms、foreground0 请求、计划 120 Hz 触控与 stream BGM。完整 44 字和最终确认音频一致，但 P0 逻辑仅 **53.963 Hz**、native 最大 **57.6 ms**、Present 间隔最大 **106.3 ms**，strict cadence 失败，像素运动未测。没有整体收益证据，故不采用 LTO；这不是“单次对照已证明 LTO 导致退化”，运行检查通过也不等于性能验收通过。[E290] [E291]

现已恢复原构建脚本，**全部 563 项生产输入精确匹配冻结 r10l**，sourceDigest 为 `5f25ca18ac687b18111b06e0c0a25c630341a3f325e5d6a684536c94704ed1e5`。LTO 源码、失败接口产物和全部运行证据继续留档；此次撤回不抹去此前双 4× 或 4× / 1× 的失败，也不构成交付通过。[E292]

### 10.55 停止扩散诊断，收敛到持续有界 RAF 补偿

用户指出任务已跨越 12 小时以上。不能把“允许花任意长时间”当成持续扩展调查范围的理由；当前没有达到可承诺稳定交付的证据，主要工作失误是诊断分支过多、修复闭环太慢。正常暂停导致另一端等待不是本轮缺陷；仍要处理正常游玩中长时间不执行更新的情况。TH11 保持未实施。

对已留存数据的最后两项核对没有给出新修复依据：

- 按同一完成帧配对、排除回滚次数增长、重演和前后 correcting 后，l / n 仍分别有 42 / 58 个普通单步配对；InputAndStep 的每对耗时比中位数约 6.71 / 12.6。它们是墙钟时间，不能据此断言浏览器 CPU 限速错误，也没有证明两端预测世界内容完全相同。[E293]
- l / m 既有 ThreadTime 数据中，native 窗口主线程 CPU / 墙钟比例均约 0.22；m 的测后独立探针虽慢，其探针阶段 CPU 时间也约为 l 的 4.6 倍。测后探针不能反推整个 native 测量窗口的 CPU 状态。[E294]

只追加了一场带 Chrome trace / ThreadTime 的 1200f、4× / 1×、RTC 50±5ms、120Hz 计划触控、stream BGM 诊断。它没有复现原坏态：P0 / P1 逻辑 59.938 / 59.975Hz，present 最大间隔 24.2 / 20.3ms，advance 最大间隔 34.7 / 24.7ms；P0 advance p99 29.7ms 仍未过原严格门槛。full44 / 音频 / 布局通过不能代替交付，健康新进程也不能解释旧失败。本分支停止重测。[E295] [E296]

浮点模式静态核对没有发现明确错误：已有模式写入来自创建或显式设置接口，高刷与历史绘制路径未发现新 setter。未添加模式探针，也不据此声称所有内存均无问题。

下一项具体修复是 **同一个真实 RAF 尚未返回时，允许有界连续 timeout 补偿**。原实现只补一次，面对约 300ms 的 RAF 派发延迟仍会留下较长空白。新实现保留一个真实 RAF、最多一个 timeout，每次完整回调结束后重新等待 25ms；每个 timer arm 有独立序号，旧 ticket、暂停、停止、owner / epoch 更换均不得重挂。timeout 不冒充真实刷新率样本，不修改回滚预算、音频债务或网络预测上限。默认 OFF。

验证仅使用桌面：测试钩子在第 300 帧后扣住一次实际到达的 RAF 回调 300ms，保留原 timestamp，触控和定时器继续运行。同一新 WASM 分别 OFF / ON，先证明该类延迟中的推进与取消行为，再检查正常场景。此故障是可控机制验证，**不是历史 RAF 阻塞根因证明，也不能抢占单次过慢 native 工作**。当前尚无本修复实测收益结论。

### 10.56 持续补偿有明确局部收益，但正常场仍未通过

组件首轮因三个测试局部变量未使用而在 -Werror 下编译失败；修正测试变量后，23 组 / 3365 断言通过。延迟注入 helper 的 15 组 / 229 断言、原 JS 23 组 / 449 断言、Python 18 组 / 879 断言及原 8 项统计回归通过。实际 HTTP 模块校验通过。[E297] [E298] [E299] [E300]

正式候选 **r10o** 在 r10l 的 563 项生产输入中只改 RAF 策略和 Host 两个文件，构建前后源码稳定；WASM SHA 为 `f5e144dd1377de306c5501cc9c4a99b1f201233a51c6df643c4923f1ccd4a82a`，sourceDigest 为 `12f2ef17375f6068a867a95516cafa745b3c705423ebb1ddf4a7b201e32b212f`。默认 OFF，未合并或部署。[E301]

同一 r10o、600f、P0 4× / P1 1×、RTC 单向 50±5ms、stream BGM、120Hz 计划触控，第 300 帧后延迟一次真实 RAF 派发 300ms：

| 受测 P0 | 保护 OFF | 保护 ON |
|---|---:|---:|
| 实际 hold 时长 | 314.2ms | 308.5ms |
| hold 内 native callback / 画面提交次数 | 0 / 0 | 7 / 7 |
| hold 内已观察完成帧增量 | 0 | 12 |
| hold 内实际触控提交 | 24 | 26 |
| 与 hold 重叠的最大画面提交间隔 | 337.7ms | 52.6ms |
| 与 hold 重叠的最大推进间隔 | 337.7ms | 46.4ms |

两场 full44、确认音频、RTC route、布局及原 timestamp / 一次 hold-release / 停止清理门均通过。这是“RAF 派发延迟但 timer 仍运行”这一类故障的具体改善。ON 的 52.6ms 仍超过原 50ms 上限，不能宣称严格通过；没有把两场整体平均帧率差当成独立因果收益。[E302] [E303] [E304]

随后无故障注入、双 4×、双端 guard ON 的 1200f 正常场，full44 和音频在 1275 帧 fence 一致，但 **整体失败**：P0 / P1 逻辑 54.159 / 54.289Hz；P0 present 最大 124.2ms、advance 最大 115.1ms，P1 对应 37.1 / 53.2ms。两端 timer service 均为 0，说明本轮正常场的失败不能被写成已由新补偿修好。[E305] [E306]

P0 实际像素采集完整，无 overflow / readback error / 运动违例；本地玩家 / 远端玩家 / 子弹窗口覆盖为 **3 / 0 / 3**，远端不足导致验收失败。未覆盖不等于已观察到错误运动，也绝不等于通过。保留原门槛，TH10 继续不可交付；当前可承诺的范围只到上面的受控 RAF 长等待改善。[E306]

### 10.57 r10p 受控 RAF 长等待缓解通过有限场景验证

在首次 25ms 期限不变的前提下，连续补偿改为每次完整结束后等待一个逻辑帧周期，即 `1000/60ms`；整毫秒 timeout 向上取整。仍只有一个真实 RAF 和一个独立身份 timer，没有 catch-up、重入或重复时间计账。更新后的 23 组 / 3445 断言通过。[E307]

正式候选 r10p 相对 r10o 只修改该续挂间隔及 Host 注释，563 项生产输入冻结一致，WASM SHA `551c2c096e63d3fbb7274ccfe6a143f57e53e47613290eba20ffb3a329a7104f`，sourceDigest `bb347d8b8c69d754b177062dc8f9f3a91a0c0ad913f85045b27b6cae1701dfa6`。默认 OFF，未合并或部署。[E308]

同一 r10p 再做同配置、同目标帧的 OFF / ON 对照：

| 受测 P0 | OFF | ON |
|---|---:|---:|
| 实际 hold 时长 | 306.8ms | 301.3ms |
| hold 内 callback / present | 0 / 0 | 9 / 9 |
| hold 内完成帧增量 | 0 | 16 |
| hold 内实际触控提交 | 28 | 27 |
| 与 hold 重叠的最大 present gap | 310.1ms | 44.5ms |
| 与 hold 重叠的最大 advance gap | 324.0ms | 45.0ms |

两场的 full44、音频、route、布局、原 timestamp、单次 hold-release 及清理均通过；ON 故障窗口的两类最大间隔均小于原 50ms 上限。ON 全测量窗 P0 / P1 逻辑 59.685 / 59.696Hz，但 P0 advance p99 为 31.2ms，仍不能写成原严格整体通过。[E309] [E310] [E311]

本组 native 耗时分布比 r10o 的旧组健康；因此**不能把 o 的 52.6ms 到 p 的 44.5ms 全部归因于 25→16.667ms 参数变化**。可支持的因果结论来自 p 同一构建的 OFF / ON 故障响应和 ON 窗口内实际补偿服务，而不是跨进程平均耗时比较。既有正常双 4× 样本中 timer service 为零；该续挂间隔改动不会自动修好那一场，未重复跑一场健康结果来抵消失败。

### 10.58 正常 124.2ms 停显属于必要历史恢复，未发现可直接删除的伪回退

原 r10o 正常场 P0 最大 present gap 为 18820.4→18944.6ms。rAF 和触控均继续执行；sequence 1445–1449 五次回调都处于 correcting，没有完整世界可以呈现。结合逐回调游标、计数与实际循环，历史事务依次为：

| sequence | 推导历史重演帧 |
|---|---|
| 1445 | 1069、1070 |
| 1446 | 1070 |
| 1447 | 1071、1072 |
| 1448 | 1073 |
| 1449 | 1072、1073 |
| 1450 | 1073、1074、1075，随后 forward 1076 并呈现 |

共 7 个不同历史帧、11 次重演，额外重做 4 次。CorrectionEnd 一直固定在 1076；行尾游标不是回滚起点，尤其不能把 1449 写成从 1073 恢复。[E312]

common 的 `NetplayCore.cpp` 已去重相同已确认输入、过滤与使用预测一致的输入、清除回退点之后全部 used 槽，并且对废弃未来的迟到输入只 Accepted；`simulation-frontier-test.cpp` 已覆盖这条合同。TH10 只对不晚于当前已模拟游标的真实差异恢复，不会无条件回到最早初始点。当前证据不能安全省掉上述 4 次重做。禁止忽略 correction 内真实差异、呈现未完成世界，或取消事务预算换成单次长阻塞。

每次接收先批处理至多 256 包再恢复；没有预算耗尽的证据，不能凭猜测修改接收策略。初次恢复前已提前捕获并发送下一帧本地输入，恢复中的旧 NextFrame 已有对应捕获，这不是简单漏发。剩余目标是降低必要历史事务成本；当前未承诺已完成该优化。[回滚输入核心](../third_party/eagler-common/src/netplay/NetplayCore.cpp) [TH10 恢复循环](../th10_web/cpp/platform/Application.cpp)

## 11. 源码索引

| 引用 | 相对本源码树的文件 |
| --- | --- |
| [S1] | `th10_web/cpp/platform/Application.cpp` |
| [S2] | `th10_web/cpp/sdl/ApplicationHost.cpp` |
| [S3] / [S4] | `th10_web/cpp/multiplayer/NetplayRuntime.hpp / .cpp` |
| [S5] | `th10_web/cpp/multiplayer/NetworkFrameSchedule.hpp` |
| [S6] / [S7] | `third_party/eagler-common/include/eagler/netplay/InputGapPacing.hpp / FrameBudget.hpp` |
| [S8] | `third_party/eagler-common/src/netplay/SessionChannel.cpp` |
| [S9] / [S10] | `th10_web/cpp/sdl/GameHost.cpp`；`th10_web/sdl-runtime/shell.mjs` |
| [S11] / [S12] | `th10_web/cpp/platform/World.cpp / WorldReplay.cpp` |
| [S13] / [S14] | `portable/multiplayer/performance-driver.mjs / check-performance.py` |
| [S15] / [S16] | `portable/multiplayer/run-smoothness.mjs / test-performance-summary.py` |
| [S17] / [S18] | `tests/netplay-pacing-test.cpp`；`third_party/eagler-common/include/eagler/netplay/PendingFrameSchedule.hpp` |
| [S19] | `th10_web/cpp/multiplayer/SessionExports.cpp`；canonical 字段与 ANM / RNG 计数 |
| [S20] / [S21] | `th10_web/cpp/game/Gui.hpp / GuiResources.cpp`；GUI 字段布局与动画句柄创建 |
| [S22] / [S23] | `th10_web/cpp/platform/Title.hpp`；`th10_web/cpp/game/StartupScreen.cpp`；开场 Draw 边界与真实 2 秒等待 |
| [S24] | `portable/multiplayer/smoke.mjs`；首 tick 前联机配置及已有加载推进入口 |
| [S25] | `th10_web/cpp/multiplayer/RecoveryBudget.hpp`；r6 历史事务准入 |
| [S26] / [S27] | `third_party/eagler-common/include/eagler/netplay/AdonisConnection.hpp`；`third_party/eagler-common/tests/adonis-startup-test.cpp`；退休包校准边界 |
| [S28] | `portable/multiplayer/check-manual-restart.py`；实际 RTC、原生菜单输入与 stopped-loop 生命周期检查 |
| [S29] / [S30] | `th10_web/cpp/game/StageHints.hpp / .cpp`；pending / retained 哨兵 owner 自指针 |
| [S31] / [S32] | `th10_web/cpp/multiplayer/netplay/NetplayProtocolConfig.hpp`；`third_party/eagler-common/tests/fixtures/include/netplay/NetplayProtocolConfig.hpp`；正式标题与测试 fixture 协议 |
| [S33] / [S34] | `th10_web/cpp/multiplayer/LiveMotionObservation.hpp / .cpp`；真实提交观察 ABI 与生命周期 |
| [S35] / [S36] | `portable/multiplayer/live-motion-capture.mjs / live-motion-analysis.mjs`；非阻塞 ROI 与有限运动证据 |
| [S37] | `tests/live-motion-analysis.test.mjs`；独立离线正负对照 |
| [S38] / [S39] | `portable/check-anm-dense-engine.mjs / make-dependencies.mjs`；原生 dense runner 与 Make 依赖解析 |
| [S44] | `portable/multiplayer/live-motion-texture-guards.mjs`；真实 pending / callback 生命周期与 LMT1 只读守卫 |
| [S45] | `portable/multiplayer/analyze-chrome-gaps.py`；SHA 绑定的离线 rAF / native 间隔分析 |
| [S46] / [S47] | `th10_web/cpp/platform/Callbacks.cpp / AnimationEngine.cpp`；callback、ANM overflow 与 geometry 清零前首写 |
| [S48] / [S49] | `th10_web/cpp/platform/Hud.cpp / ScreenEffects.cpp`；Dialogue / ScreenEffect 原完整槽清零边界 |
| [S50] / [S51] | `th10_web/cpp/platform/WorldEnemies.cpp / WorldLasers.cpp`；ECL / Enemy / Laser 首写入口 |
| [S52] / [S53] | `th10_web/cpp/multiplayer/RollbackState.cpp / RollbackPoolCapture.hpp`；World capture 接线与 slot-aware Storage 保存 |
| [S54] / [S55] | `portable/check-anm-overflow-rollback.mjs / check-geometry-rollback.mjs`；真实 native 单元与负对照覆盖边界 |
| [S56] | `th10_web/cpp/multiplayer/DiagnosticsExports.cpp`；journal / 回滚 / 重演计数的 12-word ABI |
| [S57] | `th10_web/cpp/multiplayer/NetworkInputGapPacing.hpp`；TH10 专属、默认关闭的深缺口制动候选 |
| [S58] | `portable/multiplayer/check-live-motion-capture-selection.mjs`；pending 资格 / 固定配额 / 原分析器对照 |
| [S62] | `th10_web/cpp/sdl/NetworkTailPresentation.hpp`；完整帧末呈现准入、Stage 边界与历史 decision 编码 |
| [S63] | `th10_web/cpp/platform/ApplicationFrames.cpp`；真实 authored Draw 前及 submit 前后的 sticky 边界 |
| [S64] | `portable/check-network-tail-presentation.cpp`；真实 Stage / Timer / 图形边界组件与最后 fade 反例 |
| [S65] | `portable/multiplayer/websocket-input-impairment.cjs`；测试专用、按 Input 消息 FIFO 的 WS 发送前延迟 |
| [S66] | `portable/multiplayer/check-websocket-input-impairment.mjs`；WS 字节 / 顺序 / 生命周期的独立离线验证 |
| [E1] | `artifacts/rollback-smoothness/baseline-oct10/rtc-touch120-cpu1-delay50-jitter5-unlocked.json.runner.json`；HTTP 启动失败 |
| [E2] | `artifacts/rollback-smoothness/baseline-oct10/rtc-touch120-cpu1-delay50-jitter5-unlocked-r2.json`；无有效 RTC 注入的 baseline 报告 |
| [E3] | `artifacts/rollback-smoothness/baseline-oct10/rtc-touch120-cpu1-delay50-jitter5-headed-r3.json`；headed 建链失败与前置等待证据 |
| [E4] | `artifacts/rollback-smoothness/baseline-oct10/rtc-touch120-cpu1-delay50-jitter5-headed-rtc15s.json`；首个有效 1× 基线，收据为同路径加 `.runner.json` |
| [E5] | `artifacts/rollback-smoothness/candidate-r2/rtc-touch120-cpu1-delay50-jitter5-headed-rtc15s.json`；有效 1× 候选，收据为同路径加 `.runner.json` |
| [E6] | `artifacts/rollback-smoothness/baseline-oct10/rtc-touch120-cpu4-delay50-jitter5-headed-rtc15s.json`；有效 4× 基线，收据为同路径加 `.runner.json` |
| [E7] | `artifacts/rollback-smoothness/candidate-r2/rtc-touch120-cpu4-delay50-jitter5-headed-rtc15s.json`；有效 4× 候选回退证据，收据为同路径加 `.runner.json` |
| [E8] | `artifacts/rollback-smoothness/candidate-r3/rtc-touch120-cpu4-delay50-jitter5-headed-rtc15s.json`；r3 的有效 4× 报告，仍未达标，收据为同路径加 `.runner.json` |
| [E9] | `artifacts/rollback-smoothness/candidate-r3-profile/rtc-touch120-cpu4-delay50-jitter5-headed-rtc15s.json`；独立采样运行，不作未采样性能验收，收据为同路径加 `.runner.json` |
| [E10] | `artifacts/rollback-smoothness/candidate-r3-profile/hotspots.json`；P0 采样热点与原 profile / symbols SHA-256 |
| [E11] | `artifacts/rollback-smoothness/candidate-r3-profile/draw-contexts.json`；采样栈的绘制上下文分类与限制 |
| [E12] | `artifacts/rollback-smoothness/baseline-oct10/rtc-touch120-cpu4-delay50-jitter5-spike300x600-seat1-headed-rtc15s.json.environment-note.json`；首次尖峰的用户断网旁注，原始报告保留 |
| [E13] | `artifacts/rollback-smoothness/baseline-oct10/rtc-touch120-cpu4-delay50-jitter5-spike300x600-seat1-headed-rtc15s-repeat.json`；尖峰重跑基线，收据为同路径加 `.runner.json` |
| [E14] | `artifacts/rollback-smoothness/candidate-r3/rtc-touch120-cpu4-delay50-jitter5-spike300x600-seat1-headed-rtc15s.json`；异常待复测的 r3 尖峰记录，收据为同路径加 `.runner.json` |
| [E15] | `artifacts/rollback-smoothness/spike-inspection-r3.json`；长逻辑间隔离线检查与同 realm 尖峰锚点 |
| [E16] | `artifacts/rollback-smoothness/host-after-r3-spike.json`；11:11:57 UTC 的事后宿主快照 |
| [E17] | `artifacts/rollback-smoothness/component-checks/background-projection-cache-r4.json`；缓存定向原生检查收据，非 Web 性能验收 |
| [E18] | `artifacts/rollback-smoothness/formal-build-r4.json`；r4 正式构建成功、完整 WASM / source digest 身份，非 Web 性能验收 |
| [E19] | `artifacts/rollback-smoothness/baseline-after-anomaly-inspection.json`；原版无尖峰校准失败后的部分数据检查 |
| [E20] | `artifacts/rollback-smoothness/baseline-oct10/rtc-touch120-cpu4-delay50-jitter5-headed-rtc15s-after-anomaly.json`；未完成的原始校准记录，失败与子进程清理收据为同路径加 `.runner.json` |
| [E21] | `artifacts/rollback-smoothness/cpu-calibration/browser-cpu-calibration.json`；独立浏览器的纯 JS / WASM 算术校准，不是游戏性能 |
| [E22] | `artifacts/rollback-smoothness/candidate-r4/rtc-touch120-cpu1-delay50-jitter5-headed-rtc15s.json`；首次 r4 1× RTC 的最终 canonical 失败及完整诊断数据 |
| [E23] | `artifacts/rollback-smoothness/candidate-r4/rtc-touch120-cpu1-delay50-jitter5-headed-rtc15s.json.runner.json`；r4 失败状态、运行时身份与子进程清理收据 |
| [E24] | `artifacts/rollback-smoothness/formal-build-r4-cache-off.json`；缓存关闭对照的正式构建与字节身份 |
| [E25] | `artifacts/rollback-smoothness/candidate-r4-cache-off/rtc-touch120-cpu1-delay50-jitter5-headed-rtc15s.json`；关闭缓存、初始与尾端一致的单次 1× 运行 |
| [E26] | `artifacts/rollback-smoothness/candidate-r4-cache-off/rtc-touch120-cpu1-delay50-jitter5-headed-rtc15s.json.runner.json`；关闭缓存对照的通过与清理收据 |
| [E27] | `artifacts/rollback-smoothness/candidate-r4/rtc-touch120-cpu1-delay50-jitter5-headed-rtc15s-initial-probe.json`；原 r4 同字节带初始观察复测，覆盖跨回调恢复，原失败未解 |
| [E28] | `artifacts/rollback-smoothness/candidate-r4/rtc-touch120-cpu1-delay50-jitter5-headed-rtc15s-initial-probe.json.runner.json`；冻结原 r4 的身份、通过与清理收据 |
| [E29] | `artifacts/rollback-smoothness/candidate-r4/rtc-startup-skew8-cpu1-delay50-jitter5-headed-rtc15s.json`；缓存开启 r4 的初始失步与最终失败 |
| [E30] | `artifacts/rollback-smoothness/startup-skew-r4-inspection.json`；初始 / 最终 ANM、RNG、GUI 差异和首次命中前的状态 |
| [E31] | `artifacts/rollback-smoothness/candidate-r4-cache-off/rtc-startup-skew8-cpu1-delay50-jitter5-headed-rtc15s.json`；关闭缓存也复现启动失步 |
| [E32] / [E33] | [E29] / [E31] 原始路径各加 `.runner.json`；冻结字节身份、失败及子进程清理收据 |
| [E34] | `artifacts/rollback-smoothness/startup-skew-r5-inspection.json`；r5 初始 / 最终完整 canonical 与音频一致 |
| [E35] | `artifacts/rollback-smoothness/candidate-r5/runtime/frozen.json`；r5 正式运行时冻结身份及尾帧试验之前的范围 |
| [E36] | `artifacts/rollback-smoothness/candidate-r5/rtc-startup-skew8-cpu1-delay50-jitter5-headed-rtc15s.json`；r5 启动偏斜定向验证原始报告 |
| [E37] | `artifacts/rollback-smoothness/candidate-r5/rtc-startup-skew8-cpu1-delay50-jitter5-headed-rtc15s.json.runner.json`；r5 运行通过、完整性与清理收据 |
| [E38] | `artifacts/rollback-smoothness/r5-cpu4-deadline-inspection.json`；r5 正常 4× 超时的部分数据、初始状态和事后身份核对 |
| [E39] | `artifacts/rollback-smoothness/candidate-r5/rtc-touch120-cpu4-delay50-jitter5-headed-rtc15s.json`；r5 900 帧请求 deadline 失败原始报告 |
| [E40] | `artifacts/rollback-smoothness/candidate-r5/rtc-touch120-cpu4-delay50-jitter5-headed-rtc15s.json.runner.json`；r5 失败、运行时身份与子进程清理收据 |
| [E41] | `artifacts/rollback-smoothness/component-checks/netplay-pacing-r6.json`；既有 pacing suite 与 r6 RecoveryBudget 边界通过 |
| [E42] | `artifacts/rollback-smoothness/component-checks/adonis-retirement-r6/checks.json`；同一 44 字节旧 ACK 的旧 / 新行为与既有 Adonis 启动整套 |
| [E43] | `artifacts/rollback-smoothness/component-checks/adonis-retirement-r6/common-commit.json`；common 本地提交 7766625、两文件及干净状态回执 |
| [E44] | `artifacts/rollback-smoothness/candidate-r6/runtime/frozen.json`；r6 正式冻结身份与未验收范围 |
| [E45] | `artifacts/rollback-smoothness/candidate-r5-profile-cpu4/hotspots.json`；短 profile 归因；108 callback 门槛失败，非性能验收 |
| [E46] | `artifacts/rollback-smoothness/candidate-r5-profile-cpu4/rtc-touch120-cpu4-delay50-jitter5-headed-rtc15s.json`；r5 240 帧 profile 原始报告，最终一致但总体失败 |
| [E47] | `artifacts/rollback-smoothness/candidate-r5-profile-cpu4/rtc-touch120-cpu4-delay50-jitter5-headed-rtc15s.json.runner.json`；短 profile 的失败与清理收据 |
| [E48] | `artifacts/rollback-smoothness/formal-build-r6.json`；r6 正式 multiplayer 构建成功、diagnostic=false |
| [E49] | `artifacts/rollback-smoothness/component-checks/manual-restart-syntax-r6.json`；6 项语法 / AST 检查，不是游戏验收 |
| [E50] | `artifacts/rollback-smoothness/candidate-r6/manual-restart-basic.json.runner.json`；relay 依赖失败，未开浏览器；stderr 位于同名 .json.logs/relay.stderr.log |
| [E51] | `artifacts/rollback-smoothness/relay-r6-locked/snapshot.json`；独立 relay 源码、固定依赖和 contracts-build 身份 |
| [E52] | `artifacts/rollback-smoothness/candidate-r6/manual-restart-basic-isolated-relay.json`；r6 实际 RTC 原生 Restart，新局完整 canonical 失败 |
| [E53] | `artifacts/rollback-smoothness/candidate-r6/manual-restart-basic-isolated-relay.json.runner.json`；Restart 失败、正式字节身份与清理收据 |
| [E54] | `artifacts/rollback-smoothness/candidate-r6/manual-restart-basic-inspection.json`；basic 原始报告身份与正式 EANP terminal send |
| [E55] | `artifacts/rollback-smoothness/candidate-r6/manual-restart-late-inspection.json`；首轮 late 诊断初始失配，未进入 Restart 或捕获 ACK |
| [E56] | `artifacts/rollback-smoothness/candidate-r6/manual-restart-late-terminal-ack-diagnostic.json`；初始失败与完整 owner 原始字节 |
| [E57] | `artifacts/rollback-smoothness/candidate-r6/manual-restart-late-terminal-ack-diagnostic.json.runner.json`；首轮诊断失败、身份与子进程清理 |
| [E58] | `artifacts/rollback-smoothness/candidate-r6/manual-restart-late-observe-inspection.json`；继续观察的 scene 范围、真实 ACK 收发和最终 FAIL |
| [E59] | `artifacts/rollback-smoothness/candidate-r6/manual-restart-late-terminal-ack-observe.json`；完整 owner 字节与有限生命周期观察；canonical 仍失败 |
| [E60] | `artifacts/rollback-smoothness/candidate-r6/manual-restart-late-terminal-ack-observe.json.runner.json`；继续观察最终失败及子进程清理 |
| [E61] | `artifacts/rollback-smoothness/candidate-r6/rtc-touch120-cpu1-delay50-jitter5-isolated-relay.json`；r6 真正 rAF 1× 900 帧；完整一致性通过，P0 cadence 未达标 |
| [E62] | `artifacts/rollback-smoothness/candidate-r6/rtc-touch120-cpu1-delay50-jitter5-isolated-relay.json.runner.json`；r6 1× 路由 / 一致性 / 运行时身份与清理通过 |
| [E63] | `artifacts/rollback-smoothness/candidate-r6/rtc-touch120-cpu4-delay50-jitter5-isolated-relay.json`；r6 真正 rAF 4× 900 帧；一致性完成但性能严重不合格 |
| [E64] | `artifacts/rollback-smoothness/candidate-r6/rtc-touch120-cpu4-delay50-jitter5-isolated-relay.json.runner.json`；r6 4× 路由 / 一致性 / 身份与清理；passed 非流畅度通过 |
| [E65] | `artifacts/rollback-smoothness/baseline-oct10/rtc-touch120-cpu4-frames480-isolated-relay-control.json`；最后冻结原版同独立 relay 的 4×、480 帧；一致性完成但性能不合格 |
| [E66] | `artifacts/rollback-smoothness/baseline-oct10/rtc-touch120-cpu4-frames480-isolated-relay-control.json.runner.json`；最后原版身份、通过与自有子进程清理；日志见同名 .json.logs |
| [E67] | `artifacts/rollback-smoothness/r6-baseline-matched-window.json`；179→527 相同连续窗；顺序单样本、全程字段口径与原始报告 SHA |
| [E68] | `artifacts/rollback-smoothness/analyze-r6-baseline-matched-window.py`；只读离线 AST 复用现有统计函数及完整性断言 |

[S1]: ../th10_web/cpp/platform/Application.cpp
[S2]: ../th10_web/cpp/sdl/ApplicationHost.cpp
[S3]: ../th10_web/cpp/multiplayer/NetplayRuntime.hpp
[S4]: ../th10_web/cpp/multiplayer/NetplayRuntime.cpp
[S5]: ../th10_web/cpp/multiplayer/NetworkFrameSchedule.hpp
[S6]: ../third_party/eagler-common/include/eagler/netplay/InputGapPacing.hpp
[S7]: ../third_party/eagler-common/include/eagler/netplay/FrameBudget.hpp
[S8]: ../third_party/eagler-common/src/netplay/SessionChannel.cpp
[S9]: ../th10_web/cpp/sdl/GameHost.cpp
[S10]: ../th10_web/sdl-runtime/shell.mjs
[S11]: ../th10_web/cpp/platform/World.cpp
[S12]: ../th10_web/cpp/platform/WorldReplay.cpp
[S13]: ../portable/multiplayer/performance-driver.mjs
[S14]: ../portable/multiplayer/check-performance.py
[S15]: ../portable/multiplayer/run-smoothness.mjs
[S16]: ../portable/multiplayer/test-performance-summary.py
[S17]: ../tests/netplay-pacing-test.cpp
[S18]: ../third_party/eagler-common/include/eagler/netplay/PendingFrameSchedule.hpp
[S19]: ../th10_web/cpp/multiplayer/SessionExports.cpp
[S20]: ../th10_web/cpp/game/Gui.hpp
[S21]: ../th10_web/cpp/game/GuiResources.cpp
[S22]: ../th10_web/cpp/platform/Title.hpp
[S23]: ../th10_web/cpp/game/StartupScreen.cpp
[S24]: ../portable/multiplayer/smoke.mjs
[S25]: ../th10_web/cpp/multiplayer/RecoveryBudget.hpp
[S26]: ../third_party/eagler-common/include/eagler/netplay/AdonisConnection.hpp
[S27]: ../third_party/eagler-common/tests/adonis-startup-test.cpp
[S28]: ../portable/multiplayer/check-manual-restart.py
[S29]: ../th10_web/cpp/game/StageHints.hpp
[S30]: ../th10_web/cpp/game/StageHints.cpp
[S31]: ../th10_web/cpp/multiplayer/netplay/NetplayProtocolConfig.hpp
[S32]: ../third_party/eagler-common/tests/fixtures/include/netplay/NetplayProtocolConfig.hpp
[E1]: ../artifacts/rollback-smoothness/baseline-oct10/rtc-touch120-cpu1-delay50-jitter5-unlocked.json.runner.json
[E2]: ../artifacts/rollback-smoothness/baseline-oct10/rtc-touch120-cpu1-delay50-jitter5-unlocked-r2.json
[E3]: ../artifacts/rollback-smoothness/baseline-oct10/rtc-touch120-cpu1-delay50-jitter5-headed-r3.json
[E4]: ../artifacts/rollback-smoothness/baseline-oct10/rtc-touch120-cpu1-delay50-jitter5-headed-rtc15s.json
[E5]: ../artifacts/rollback-smoothness/candidate-r2/rtc-touch120-cpu1-delay50-jitter5-headed-rtc15s.json
[E6]: ../artifacts/rollback-smoothness/baseline-oct10/rtc-touch120-cpu4-delay50-jitter5-headed-rtc15s.json
[E7]: ../artifacts/rollback-smoothness/candidate-r2/rtc-touch120-cpu4-delay50-jitter5-headed-rtc15s.json
[E8]: ../artifacts/rollback-smoothness/candidate-r3/rtc-touch120-cpu4-delay50-jitter5-headed-rtc15s.json
[E9]: ../artifacts/rollback-smoothness/candidate-r3-profile/rtc-touch120-cpu4-delay50-jitter5-headed-rtc15s.json
[E10]: ../artifacts/rollback-smoothness/candidate-r3-profile/hotspots.json
[E11]: ../artifacts/rollback-smoothness/candidate-r3-profile/draw-contexts.json
[E12]: ../artifacts/rollback-smoothness/baseline-oct10/rtc-touch120-cpu4-delay50-jitter5-spike300x600-seat1-headed-rtc15s.json.environment-note.json
[E13]: ../artifacts/rollback-smoothness/baseline-oct10/rtc-touch120-cpu4-delay50-jitter5-spike300x600-seat1-headed-rtc15s-repeat.json
[E14]: ../artifacts/rollback-smoothness/candidate-r3/rtc-touch120-cpu4-delay50-jitter5-spike300x600-seat1-headed-rtc15s.json
[E15]: ../artifacts/rollback-smoothness/spike-inspection-r3.json
[E16]: ../artifacts/rollback-smoothness/host-after-r3-spike.json
[E17]: ../artifacts/rollback-smoothness/component-checks/background-projection-cache-r4.json
[E18]: ../artifacts/rollback-smoothness/formal-build-r4.json
[E19]: ../artifacts/rollback-smoothness/baseline-after-anomaly-inspection.json
[E20]: ../artifacts/rollback-smoothness/baseline-oct10/rtc-touch120-cpu4-delay50-jitter5-headed-rtc15s-after-anomaly.json
[E21]: ../artifacts/rollback-smoothness/cpu-calibration/browser-cpu-calibration.json
[E22]: ../artifacts/rollback-smoothness/candidate-r4/rtc-touch120-cpu1-delay50-jitter5-headed-rtc15s.json
[E23]: ../artifacts/rollback-smoothness/candidate-r4/rtc-touch120-cpu1-delay50-jitter5-headed-rtc15s.json.runner.json
[E24]: ../artifacts/rollback-smoothness/formal-build-r4-cache-off.json
[E25]: ../artifacts/rollback-smoothness/candidate-r4-cache-off/rtc-touch120-cpu1-delay50-jitter5-headed-rtc15s.json
[E26]: ../artifacts/rollback-smoothness/candidate-r4-cache-off/rtc-touch120-cpu1-delay50-jitter5-headed-rtc15s.json.runner.json
[E27]: ../artifacts/rollback-smoothness/candidate-r4/rtc-touch120-cpu1-delay50-jitter5-headed-rtc15s-initial-probe.json
[E28]: ../artifacts/rollback-smoothness/candidate-r4/rtc-touch120-cpu1-delay50-jitter5-headed-rtc15s-initial-probe.json.runner.json
[E29]: ../artifacts/rollback-smoothness/candidate-r4/rtc-startup-skew8-cpu1-delay50-jitter5-headed-rtc15s.json
[E30]: ../artifacts/rollback-smoothness/startup-skew-r4-inspection.json
[E31]: ../artifacts/rollback-smoothness/candidate-r4-cache-off/rtc-startup-skew8-cpu1-delay50-jitter5-headed-rtc15s.json
[E32]: ../artifacts/rollback-smoothness/candidate-r4/rtc-startup-skew8-cpu1-delay50-jitter5-headed-rtc15s.json.runner.json
[E33]: ../artifacts/rollback-smoothness/candidate-r4-cache-off/rtc-startup-skew8-cpu1-delay50-jitter5-headed-rtc15s.json.runner.json
[E34]: ../artifacts/rollback-smoothness/startup-skew-r5-inspection.json
[E35]: ../artifacts/rollback-smoothness/candidate-r5/runtime/frozen.json
[E36]: ../artifacts/rollback-smoothness/candidate-r5/rtc-startup-skew8-cpu1-delay50-jitter5-headed-rtc15s.json
[E37]: ../artifacts/rollback-smoothness/candidate-r5/rtc-startup-skew8-cpu1-delay50-jitter5-headed-rtc15s.json.runner.json
[E38]: ../artifacts/rollback-smoothness/r5-cpu4-deadline-inspection.json
[E39]: ../artifacts/rollback-smoothness/candidate-r5/rtc-touch120-cpu4-delay50-jitter5-headed-rtc15s.json
[E40]: ../artifacts/rollback-smoothness/candidate-r5/rtc-touch120-cpu4-delay50-jitter5-headed-rtc15s.json.runner.json
[E41]: ../artifacts/rollback-smoothness/component-checks/netplay-pacing-r6.json
[E42]: ../artifacts/rollback-smoothness/component-checks/adonis-retirement-r6/checks.json
[E43]: ../artifacts/rollback-smoothness/component-checks/adonis-retirement-r6/common-commit.json
[E44]: ../artifacts/rollback-smoothness/candidate-r6/runtime/frozen.json
[E45]: ../artifacts/rollback-smoothness/candidate-r5-profile-cpu4/hotspots.json
[E46]: ../artifacts/rollback-smoothness/candidate-r5-profile-cpu4/rtc-touch120-cpu4-delay50-jitter5-headed-rtc15s.json
[E47]: ../artifacts/rollback-smoothness/candidate-r5-profile-cpu4/rtc-touch120-cpu4-delay50-jitter5-headed-rtc15s.json.runner.json
[E48]: ../artifacts/rollback-smoothness/formal-build-r6.json
[E49]: ../artifacts/rollback-smoothness/component-checks/manual-restart-syntax-r6.json
[E50]: ../artifacts/rollback-smoothness/candidate-r6/manual-restart-basic.json.runner.json
[E51]: ../artifacts/rollback-smoothness/relay-r6-locked/snapshot.json
[E52]: ../artifacts/rollback-smoothness/candidate-r6/manual-restart-basic-isolated-relay.json
[E53]: ../artifacts/rollback-smoothness/candidate-r6/manual-restart-basic-isolated-relay.json.runner.json
[E54]: ../artifacts/rollback-smoothness/candidate-r6/manual-restart-basic-inspection.json
[E55]: ../artifacts/rollback-smoothness/candidate-r6/manual-restart-late-inspection.json
[E56]: ../artifacts/rollback-smoothness/candidate-r6/manual-restart-late-terminal-ack-diagnostic.json
[E57]: ../artifacts/rollback-smoothness/candidate-r6/manual-restart-late-terminal-ack-diagnostic.json.runner.json
[E58]: ../artifacts/rollback-smoothness/candidate-r6/manual-restart-late-observe-inspection.json
[E59]: ../artifacts/rollback-smoothness/candidate-r6/manual-restart-late-terminal-ack-observe.json
[E60]: ../artifacts/rollback-smoothness/candidate-r6/manual-restart-late-terminal-ack-observe.json.runner.json
[E61]: ../artifacts/rollback-smoothness/candidate-r6/rtc-touch120-cpu1-delay50-jitter5-isolated-relay.json
[E62]: ../artifacts/rollback-smoothness/candidate-r6/rtc-touch120-cpu1-delay50-jitter5-isolated-relay.json.runner.json
[E63]: ../artifacts/rollback-smoothness/candidate-r6/rtc-touch120-cpu4-delay50-jitter5-isolated-relay.json
[E64]: ../artifacts/rollback-smoothness/candidate-r6/rtc-touch120-cpu4-delay50-jitter5-isolated-relay.json.runner.json
[E65]: ../artifacts/rollback-smoothness/baseline-oct10/rtc-touch120-cpu4-frames480-isolated-relay-control.json
[E66]: ../artifacts/rollback-smoothness/baseline-oct10/rtc-touch120-cpu4-frames480-isolated-relay-control.json.runner.json
[E67]: ../artifacts/rollback-smoothness/r6-baseline-matched-window.json
[E68]: ../artifacts/rollback-smoothness/analyze-r6-baseline-matched-window.py

[E69]: ../artifacts/rollback-smoothness/candidate-r7/runtime/frozen.json
[E70]: ../artifacts/rollback-smoothness/candidate-r7/fixtures/frozen.json
[E71]: ../artifacts/rollback-smoothness/candidate-r7/quad-oracle-cpu1-touch120.json
[E72]: ../artifacts/rollback-smoothness/candidate-r7/quad-oracle-cpu1-touch120.json.runner.json
[E73]: ../artifacts/rollback-smoothness/candidate-r7/cpu4-touch120-quad-off-phases.json
[E74]: ../artifacts/rollback-smoothness/candidate-r7/cpu4-touch120-quad-off-phases.json.runner.json
[E75]: ../artifacts/rollback-smoothness/candidate-r7/cpu4-touch120-quad-on-phases.json
[E76]: ../artifacts/rollback-smoothness/candidate-r7/cpu4-touch120-quad-on-phases.json.runner.json
[E77]: ../artifacts/rollback-smoothness/candidate-r7/cpu4-touch120-quad-off-phases-repeat1.json
[E78]: ../artifacts/rollback-smoothness/candidate-r7/cpu4-touch120-quad-off-phases-repeat1.json.runner.json
[E79]: ../artifacts/rollback-smoothness/candidate-r7/profile-cpu4-touch120-off/performance.json
[E80]: ../artifacts/rollback-smoothness/candidate-r7/profile-cpu4-touch120-off/performance.json.runner.json
[E81]: ../artifacts/rollback-smoothness/candidate-r7/profile-cpu4-touch120-off/hotspots-symbolized.json

[E82]: ../artifacts/rollback-smoothness/candidate-r8/runtime/frozen.json
[E83]: ../artifacts/rollback-smoothness/candidate-r8/fixtures/frozen.json
[E84]: ../artifacts/rollback-smoothness/candidate-r8/capture-native-tests.stdout.log
[E85]: ../artifacts/rollback-smoothness/candidate-r8/cpu1-touch120-camera-oracle-packed-asymmetric.json
[E86]: ../artifacts/rollback-smoothness/candidate-r8/cpu1-touch120-camera-oracle-packed-asymmetric.json.runner.json
[E87]: ../artifacts/rollback-smoothness/candidate-r8/r8-adjacent-pair.json
[E88]: ../artifacts/rollback-smoothness/candidate-r8/cpu4-touch120-off.json
[E89]: ../artifacts/rollback-smoothness/candidate-r8/cpu4-touch120-off.json.runner.json
[E90]: ../artifacts/rollback-smoothness/candidate-r8/cpu4-touch120-on.json
[E91]: ../artifacts/rollback-smoothness/candidate-r8/cpu4-touch120-on.json.runner.json

[E92]: ../artifacts/rollback-smoothness/candidate-r8/native-validation.json
[E93]: ../artifacts/rollback-smoothness/candidate-r8/camera-native-tests.stdout.log
[E94]: ../artifacts/rollback-smoothness/candidate-r8/profile-cpu4-touch120-on/performance.json
[E95]: ../artifacts/rollback-smoothness/candidate-r8/profile-cpu4-touch120-on/performance.json.runner.json
[E96]: ../artifacts/rollback-smoothness/candidate-r8/profile-cpu4-touch120-on/hotspots-symbolized.json

[E97]: ../artifacts/rollback-smoothness/candidate-r9/native-validation.json
[E98]: ../artifacts/rollback-smoothness/candidate-r9/native-validation.stdout.log
[E99]: ../artifacts/rollback-smoothness/candidate-r9/native-validation.stderr.log
[E100]: ../artifacts/rollback-smoothness/candidate-r9/runtime/frozen.json
[E101]: ../artifacts/rollback-smoothness/candidate-r9/formal-build.json
[E102]: ../artifacts/rollback-smoothness/candidate-r9/runtime/build.json

[E103]: ../artifacts/rollback-smoothness/candidate-r10b/touch-frontier-seat-matrix-summary.json
[E104]: ../artifacts/rollback-smoothness/candidate-r10b/pure-cpu4-touch120-matrix-summary.json
[E105]: ../artifacts/rollback-smoothness/candidate-r10b/cpu4-touch120-clean-spike300x600-seat1-stall-analysis.json
[E106]: ../artifacts/rollback-smoothness/candidate-r10b/spike-stall-diagnostic-01/phase-analysis.json
[E107]: ../artifacts/rollback-smoothness/candidate-r10b/spike-stall-diagnostic-02/phase-analysis.json
[E108]: ../artifacts/rollback-smoothness/candidate-r10c-build02/runtime/frozen.json
[E109]: ../artifacts/rollback-smoothness/candidate-r10c-build02/formal-build.json
[E110]: ../artifacts/rollback-smoothness/candidate-r10c-build02/live-motion-analysis-validation-01/receipt.json
[E111]: ../artifacts/rollback-smoothness/candidate-r10c-build02/live-motion-analysis-validation-01/stdout.log
[E112]: ../artifacts/rollback-smoothness/candidate-r10c-build02/disabled-observer-validation/receipt.json
[E113]: ../artifacts/rollback-smoothness/candidate-r10c-build02/disabled-observer-validation/anm-submit/stdout.log
[E114]: ../artifacts/rollback-smoothness/candidate-r10c-build02/disabled-observer-validation/anm-dense-engine/stdout.log
[E115]: ../artifacts/rollback-smoothness/candidate-r10c-build02/disabled-observer-validation/anm-dense-engine/stderr.log
[E116]: ../artifacts/rollback-smoothness/candidate-r10c-build02/live-motion-cpu1-01/compact-summary.json
[E117]: ../artifacts/rollback-smoothness/candidate-r10b/spike-stall-diagnostic-01/performance.json.logs/checker.stderr.log

[S33]: ../th10_web/cpp/multiplayer/LiveMotionObservation.hpp
[S34]: ../th10_web/cpp/multiplayer/LiveMotionObservation.cpp
[S35]: ../portable/multiplayer/live-motion-capture.mjs
[S36]: ../portable/multiplayer/live-motion-analysis.mjs
[S37]: ../tests/live-motion-analysis.test.mjs
[S38]: ../portable/check-anm-dense-engine.mjs
[S39]: ../portable/make-dependencies.mjs

[E118]: ../artifacts/rollback-smoothness/candidate-r10c-build02/live-motion-analysis-validation-03/receipt.json
[E119]: ../artifacts/rollback-smoothness/candidate-r10c-build02/live-motion-cpu1-01/compact-summary-v2.json
[E120]: ../artifacts/rollback-smoothness/candidate-r10c-build02/live-motion-cpu1-01/offline-analysis02-summary.json
[E121]: ../artifacts/rollback-smoothness/candidate-r10c-build02/disabled-observer-validation-02/receipt.json
[E122]: ../artifacts/rollback-smoothness/candidate-r10c-build02/live-motion-analysis-validation-02/receipt.json
[E123]: ../artifacts/rollback-smoothness/candidate-r10b/spike-stall-diagnostic-02/advance-gap138-analysis.json

[S40]: ../th10_web/cpp/multiplayer/RemotePlayerPresentation.hpp
[S41]: ../th10_web/cpp/multiplayer/PlayerPresentation.hpp
[S42]: ../portable/multiplayer/chrome-trace.py
[S43]: ../th10_web/cpp/multiplayer/WorldPlayerMultiplayer.cpp
[E124]: ../artifacts/rollback-smoothness/candidate-r10d/native-display-validation-01/receipt.json
[E125]: ../artifacts/rollback-smoothness/candidate-r10d/runtime/frozen.json
[E126]: ../artifacts/rollback-smoothness/candidate-r10d/live-motion-cpu1-01/motion-summary-01.json
[E127]: ../artifacts/rollback-smoothness/candidate-r10d/live-motion-cpu1-01/roi-inspection-01/summary.json
[E128]: ../artifacts/rollback-smoothness/chrome-trace-api-probe-01/probe.json
[E129]: ../artifacts/rollback-smoothness/candidate-r10b/spike-stall-diagnostic-03-chrome/performance.json
[E130]: ../artifacts/rollback-smoothness/candidate-r10b/spike-stall-diagnostic-04-chrome/diagnostic-summary-01.json
[E131]: ../artifacts/rollback-smoothness/chrome-trace-validation-03/receipt.json

[S44]: ../portable/multiplayer/live-motion-texture-guards.mjs
[S45]: ../portable/multiplayer/analyze-chrome-gaps.py
[E132]: ../artifacts/rollback-smoothness/candidate-r10e/runtime/frozen.json
[E133]: ../artifacts/rollback-smoothness/candidate-r10e/runtime/build.json
[E134]: ../artifacts/rollback-smoothness/candidate-r10e/live-motion-analysis-validation-01/receipt.json
[E135]: ../artifacts/rollback-smoothness/candidate-r10e/live-motion-analysis-validation-01/stdout.log
[E136]: ../artifacts/rollback-smoothness/candidate-r10e/harness-validation-01/receipt.json
[E137]: ../artifacts/rollback-smoothness/candidate-r10e/live-motion-cpu1-01/motion-summary-01.json
[E138]: ../artifacts/rollback-smoothness/candidate-r10e/live-motion-cpu1-01/performance.json.runner.json
[E139]: ../artifacts/rollback-smoothness/candidate-r10e/spike-stall-diagnostic-01/diagnostic-summary-01.json
[E140]: ../artifacts/rollback-smoothness/candidate-r10e/spike-stall-diagnostic-01/performance.json.runner.json
[E141]: ../artifacts/rollback-smoothness/candidate-r10b/spike-stall-diagnostic-05-chrome/gap-analysis-01.summary.json
[E142]: ../artifacts/rollback-smoothness/candidate-r10b/spike-stall-diagnostic-05-chrome/gap-focus-01.json
[E143]: ../artifacts/rollback-smoothness/candidate-r10b/spike-stall-diagnostic-05-chrome/gap-analysis-01.json
[E144]: ../artifacts/rollback-smoothness/callback-first-write-validation-01/receipt.json
[E145]: ../artifacts/rollback-smoothness/callback-first-write-validation-01/candidate.jsonl
[E146]: ../artifacts/rollback-smoothness/callback-first-write-validation-01/legacy.jsonl
[E147]: ../artifacts/rollback-smoothness/callback-first-write-ubsan-validation-01/receipt.json
[E148]: ../artifacts/rollback-smoothness/callback-first-write-ubsan-validation-01/candidate.jsonl
[E149]: ../artifacts/rollback-smoothness/callback-first-write-ubsan-validation-01/legacy.jsonl
[E150]: ../artifacts/rollback-smoothness/real-music-harness-validation-01/receipt.json
[E151]: ../artifacts/rollback-smoothness/candidate-r10b/spike-stall-diagnostic-05-chrome/performance.json.runner.json

[S46]: ../th10_web/cpp/platform/Callbacks.cpp
[S47]: ../th10_web/cpp/platform/AnimationEngine.cpp
[S48]: ../th10_web/cpp/platform/Hud.cpp
[S49]: ../th10_web/cpp/platform/ScreenEffects.cpp
[S50]: ../th10_web/cpp/platform/WorldEnemies.cpp
[S51]: ../th10_web/cpp/platform/WorldLasers.cpp
[S52]: ../th10_web/cpp/multiplayer/RollbackState.cpp
[S53]: ../th10_web/cpp/multiplayer/RollbackPoolCapture.hpp
[S54]: ../portable/check-anm-overflow-rollback.mjs
[S55]: ../portable/check-geometry-rollback.mjs
[S56]: ../th10_web/cpp/multiplayer/DiagnosticsExports.cpp
[E152]: ../artifacts/rollback-smoothness/candidate-r10f/runtime/frozen.json
[E153]: ../artifacts/rollback-smoothness/candidate-r10f/runtime/build.json
[E154]: ../artifacts/rollback-smoothness/pooled-owner-first-write-source-01/receipt.json
[E155]: ../artifacts/rollback-smoothness/anm-overflow-first-write-validation-02/receipt.json
[E156]: ../artifacts/rollback-smoothness/anm-overflow-first-write-ubsan-validation-01/receipt.json
[E157]: ../artifacts/rollback-smoothness/geometry-first-write-validation-01/receipt.json
[E158]: ../artifacts/rollback-smoothness/geometry-first-write-ubsan-validation-01/receipt.json
[E159]: ../artifacts/rollback-smoothness/candidate-r10f/music-stream-cpu4-touch120-steady-01/pure-summary-01.json
[E160]: ../artifacts/rollback-smoothness/candidate-r10f/music-stream-cpu4-touch120-steady-01/performance.json.runner.json
[E161]: ../artifacts/rollback-smoothness/candidate-r10f/music-stream-cpu4-touch120-spike-01/pure-summary-01.json
[E162]: ../artifacts/rollback-smoothness/candidate-r10f/music-stream-cpu4-touch120-spike-01/spike-runtime-focus-01.json
[E163]: ../artifacts/rollback-smoothness/candidate-r10f/music-stream-cpu4-touch120-spike-01/performance.json.runner.json
[E164]: ../artifacts/rollback-smoothness/post-run-fence-harness-validation-01/receipt.json
[E165]: ../artifacts/rollback-smoothness/candidate-r10f/startup-slow-callback-diagnostic-01/startup-phase-focus-01.json
[E166]: ../artifacts/rollback-smoothness/candidate-r10f/startup-slow-callback-diagnostic-01/gap-analysis-01.summary.json
[E167]: ../artifacts/rollback-smoothness/candidate-r10f/music-stream-cpu4-touch120-spike-diagnostic-02/diagnostic-phase-focus-01.json
[E168]: ../artifacts/rollback-smoothness/candidate-r10f/music-stream-live-motion-cpu4-01/motion-focus-summary-01.json
[E169]: ../artifacts/rollback-smoothness/candidate-r10f/music-stream-live-motion-cpu4-01/performance.json
[S57]: ../th10_web/cpp/multiplayer/NetworkInputGapPacing.hpp
[S58]: ../portable/multiplayer/check-live-motion-capture-selection.mjs
[E170]: ../artifacts/rollback-smoothness/candidate-r10f/camera-only-pair-01/comparison-01.json
[E171]: ../artifacts/rollback-smoothness/candidate-r10f/music-stream-cpu4-touch120-spike-diagnostic-02/gap-context-01.json
[E172]: ../artifacts/rollback-smoothness/candidate-r10f/music-stream-cpu4-touch120-spike-diagnostic-02/raf-dispatch-focus-01.json
[E173]: ../artifacts/rollback-smoothness/candidate-r10f/music-stream-cpu4-touch120-spike-diagnostic-02/gap-analysis-01.summary.json
[E174]: ../artifacts/rollback-smoothness/candidate-r10f/camera-only-pair-01/on/gap-context-01.json
[E175]: ../artifacts/rollback-smoothness/capture-selection-and-tail-harness-validation-01/receipt.json
[E176]: ../artifacts/rollback-smoothness/candidate-r10f/music-stream-live-motion-cpu4-02/failure-focus-01.json
[E177]: ../artifacts/rollback-smoothness/candidate-r10f/music-stream-live-motion-cpu4-02/performance.json
[S59]: ../th10_web/cpp/sdl/RafDeadlineGuard.hpp
[S60]: ../portable/check-raf-deadline-guard.cpp
[S61]: ../portable/multiplayer/fixed-work-probe.mjs
[E178]: ../artifacts/rollback-smoothness/network-gap-tail-native-validation-01/receipt.json
[E179]: ../artifacts/rollback-smoothness/raf-deadline-guard-native-validation-01/receipt.json
[E180]: ../artifacts/rollback-smoothness/probe-and-raf-harness-validation-01/receipt.json
[E181]: ../artifacts/rollback-smoothness/candidate-r10f/startup-slow-callback-diagnostic-02/startup-phase-focus-01.json
[E182]: ../artifacts/rollback-smoothness/candidate-r10f/startup-slow-callback-diagnostic-03/startup-phase-focus-01.json
[E183]: ../artifacts/rollback-smoothness/cpu-throttler-source-155/receipt.json
[E184]: ../artifacts/rollback-smoothness/cpu-throttler-source-155/receipt-02.json
[E185]: ../artifacts/rollback-smoothness/cpu-throttler-source-155/receipt-03.json
[E186]: ../artifacts/rollback-smoothness/candidate-r10g/runtime/frozen.json
[E187]: ../artifacts/rollback-smoothness/candidate-r10g-build-01/receipt.json
[E188]: ../artifacts/rollback-smoothness/thread-time-metrics-harness-validation-01/receipt.json
[E189]: ../artifacts/rollback-smoothness/candidate-r10f/startup-slow-callback-diagnostic-04/startup-phase-focus-01.json
[E190]: ../artifacts/rollback-smoothness/candidate-r10g/raf-only-diagnostic-pair-01/off/diagnostic-focus-01.json
[E191]: ../artifacts/rollback-smoothness/candidate-r10g/raf-only-diagnostic-pair-01/on/diagnostic-focus-01.json
[E192]: ../artifacts/rollback-smoothness/candidate-r10g/raf-only-diagnostic-pair-01/cross-seat-time-alignment-01.json
[E193]: ../artifacts/rollback-smoothness/candidate-r10g/network-tail-diagnostic-pair-01/off/diagnostic-focus-01.json
[E194]: ../artifacts/rollback-smoothness/candidate-r10g/network-tail-diagnostic-pair-01/on/diagnostic-focus-01.json
[E195]: ../artifacts/rollback-smoothness/candidate-r10g/network-tail-diagnostic-pair-01/network-tail-comparison-01.json
[E196]: ../artifacts/rollback-smoothness/candidate-r10g/music-stream-live-motion-cpu4/01/diagnostic-focus-01.json
[E197]: ../artifacts/rollback-smoothness/candidate-r10g/music-stream-live-motion-cpu4/01/motion-review-01.json
[E198]: ../artifacts/rollback-smoothness/candidate-r10g/music-stream-live-motion-cpu4/01/performance.json
[E199]: ../artifacts/rollback-smoothness/candidate-r10g/manual-restart-late-ack-01/restart-focus-01.json
[E200]: ../artifacts/rollback-smoothness/candidate-r10g/manual-restart-late-ack-01/report.json

[S62]: ../th10_web/cpp/sdl/NetworkTailPresentation.hpp
[S63]: ../th10_web/cpp/platform/ApplicationFrames.cpp
[S64]: ../portable/check-network-tail-presentation.cpp
[E201]: ../artifacts/rollback-smoothness/candidate-r10g/music-stream-live-motion-cpu4/02/diagnostic-focus-01.json
[E202]: ../artifacts/rollback-smoothness/candidate-r10g/music-stream-live-motion-cpu4/02/metadata-window-focus-01.json
[E203]: ../artifacts/rollback-smoothness/candidate-r10g/music-stream-live-motion-cpu4/02/performance.json
[E204]: ../artifacts/rollback-smoothness/candidate-r10g/separated-lanes-live-motion-cpu4/01/diagnostic-focus-01.json
[E205]: ../artifacts/rollback-smoothness/candidate-r10g/separated-lanes-live-motion-cpu4/01/metadata-window-focus-01.json
[E206]: ../artifacts/rollback-smoothness/candidate-r10g/separated-lanes-live-motion-cpu4/01/pixel-window-outcomes-01.json
[E207]: ../artifacts/rollback-smoothness/candidate-r10g/separated-lanes-live-motion-cpu4/01/performance.json
[E208]: ../artifacts/rollback-smoothness/candidate-r10h/runtime/frozen.json
[E209]: ../artifacts/rollback-smoothness/network-tail-presentation-stage-validation-01/receipt.json
[E210]: ../artifacts/rollback-smoothness/network-tail-presentation-stage-validation-01/test.stdout.log
[E211]: ../artifacts/rollback-smoothness/candidate-r10h/network-tail-diagnostic-pair-01/on/diagnostic-focus-01.json
[E212]: ../artifacts/rollback-smoothness/candidate-r10h/network-tail-diagnostic-pair-01/on/tail-qualification-review-01.json
[E213]: ../artifacts/rollback-smoothness/candidate-r10h/network-tail-diagnostic-pair-01/on/shallow-forward-opportunities-01.json
[E214]: ../artifacts/rollback-smoothness/candidate-r10h/network-tail-diagnostic-pair-01/on/performance.json
[E215]: ../artifacts/rollback-smoothness/candidate-r10i/runtime/frozen.json
[E216]: ../artifacts/rollback-smoothness/candidate-r10i-build-01/receipt.json
[E217]: ../artifacts/rollback-smoothness/network-tail-presentation-stage-validation-02/receipt.json
[E218]: ../artifacts/rollback-smoothness/network-tail-presentation-stage-validation-02/test.stdout.log

[S65]: ../portable/multiplayer/websocket-input-impairment.cjs
[S66]: ../portable/multiplayer/check-websocket-input-impairment.mjs
[E219]: ../artifacts/rollback-smoothness/candidate-r10i/bounded-forward-live-motion-cpu4/pair-review-01.json
[E220]: ../artifacts/rollback-smoothness/candidate-r10h/bounded-forward-live-motion-cpu4/01/metadata-window-focus-01.json
[E221]: ../artifacts/rollback-smoothness/candidate-r10i/bounded-forward-live-motion-cpu4/01/metadata-window-focus-01.json
[E222]: ../artifacts/rollback-smoothness/candidate-r10h/bounded-forward-live-motion-cpu4/01/pixel-window-outcomes-01.json
[E223]: ../artifacts/rollback-smoothness/candidate-r10i/bounded-forward-live-motion-cpu4/01/pixel-window-outcomes-01.json
[E224]: ../artifacts/rollback-smoothness/candidate-r10h/bounded-forward-live-motion-cpu4/01/performance.json
[E225]: ../artifacts/rollback-smoothness/candidate-r10i/bounded-forward-live-motion-cpu4/01/performance.json
[E226]: ../artifacts/rollback-smoothness/websocket-relay-impairment-validation-01/receipt.json
[E227]: ../artifacts/rollback-smoothness/candidate-r10i/websocket-relay-live-motion-cpu4/01/route-and-motion-review-01.json
[E228]: ../artifacts/rollback-smoothness/candidate-r10i/websocket-relay-live-motion-cpu4/01/long-gap-focus-01.json
[E229]: ../artifacts/rollback-smoothness/candidate-r10i/websocket-relay-live-motion-cpu4/01/network-progress-focus-01.json
[E230]: ../artifacts/rollback-smoothness/candidate-r10i/websocket-relay-live-motion-cpu4/01/performance.json
[E231]: ../artifacts/rollback-smoothness/candidate-r10i/websocket-relay-transport-diagnostic-cpu4/01/network-progress-focus-01.json
[E232]: ../artifacts/rollback-smoothness/candidate-r10i/websocket-relay-transport-diagnostic-cpu4/01/performance.json
[E233]: ../artifacts/rollback-smoothness/candidate-r10i/websocket-relay-live-motion-cpu4/02/diagnostic-focus-01.json
[E234]: ../artifacts/rollback-smoothness/candidate-r10i/websocket-relay-live-motion-cpu4/02/network-progress-focus-01.json
[E235]: ../artifacts/rollback-smoothness/candidate-r10i/websocket-relay-live-motion-cpu4/02/longer-run-focus-01.json
[E236]: ../artifacts/rollback-smoothness/candidate-r10i/websocket-relay-live-motion-cpu4/02/performance.json
[E237]: ../artifacts/rollback-smoothness/candidate-r10j/websocket-relay-socket-diagnostic-cpu4/01/joint-socket-gap-01.json
[E238]: ../artifacts/rollback-smoothness/candidate-r10j/websocket-relay-socket-diagnostic-cpu4/01/raw-burst-focus-01.json
[E239]: ../artifacts/rollback-smoothness/candidate-r10j/rtc-live-motion-cpu4/01/performance.json
[E240]: ../artifacts/rollback-smoothness/candidate-r10j/rtc-live-motion-cpu4/01/callback-categories-01.json
[E241]: ../artifacts/rollback-smoothness/candidate-r10j/rtc-rate-calibration-cpu4/01/calibration-focus-01.json
[E242]: ../artifacts/rollback-smoothness/candidate-r10j/rtc-packed-occupancy-cpu4/on01/calibration-focus-01.json
[E243]: ../artifacts/rollback-smoothness/candidate-r10j/rtc-packed-occupancy-cpu4/off02/calibration-focus-01.json
[E244]: ../artifacts/rollback-smoothness/callback-work-native-validation-01/receipt.json
[S67]: ../th10_web/cpp/multiplayer/CallbackWorkTiming.hpp
[S68]: ../portable/multiplayer/callback-work-timing.mjs
[S69]: ../tests/callback-work-timing-test.cpp
[E245]: ../artifacts/rollback-smoothness/candidate-r10k/runtime/frozen.json
[E246]: ../artifacts/rollback-smoothness/candidate-r10k-build-01/receipt.json
[E247]: ../artifacts/rollback-smoothness/callback-work-harness-validation-01/receipt.json
[E248]: ../artifacts/rollback-smoothness/callback-work-harness-validation-02/receipt.json
[E249]: ../artifacts/rollback-smoothness/candidate-r10k/rtc-work-phases-cpu4/01/performance.json
[E250]: ../artifacts/rollback-smoothness/callback-work-serving-validation-01/receipt.json
[E251]: ../artifacts/rollback-smoothness/callback-work-serving-validation-02/receipt.json
[E252]: ../artifacts/rollback-smoothness/candidate-r10k/rtc-work-phases-cpu4/02/performance.json
[E253]: ../artifacts/rollback-smoothness/candidate-r10k/rtc-work-phases-cpu4/02/work-phase-focus-01.json
[E254]: ../artifacts/rollback-smoothness/candidate-r10k/rtc-draw-profile-cpu4/01/profile-inventory-01.json
[E255]: ../artifacts/rollback-smoothness/candidate-r10k/rtc-draw-profile-cpu4/01/performance.json
[E256]: ../artifacts/rollback-smoothness/item-draw-entry-review-01/raw-body-01.json
[E257]: ../artifacts/rollback-smoothness/candidate-r10l/runtime/frozen.json
[E258]: ../artifacts/rollback-smoothness/candidate-r10l-build-01/receipt.json
[E259]: ../artifacts/rollback-smoothness/candidate-r10l/rtc-item-gate-profile-cpu4/01/startup-canonical-diff-01.json
[E260]: ../artifacts/rollback-smoothness/candidate-r10l/rtc-item-gate-profile-cpu4/01/performance.json
[E261]: ../artifacts/rollback-smoothness/relay-bounded-writes-validation-01/receipt.json
[E262]: ../artifacts/rollback-smoothness/relay-bounded-writes-validation-02/receipt.json
[E263]: ../artifacts/rollback-smoothness/background-projection-prefix-validation-01/receipt.json
[E264]: ../artifacts/rollback-smoothness/background-projection-prefix-validation-01/rules.stdout.log
[E265]: ../artifacts/rollback-smoothness/candidate-r10m/runtime/frozen.json
[E266]: ../artifacts/rollback-smoothness/candidate-r10m-build-01/receipt.json
[E267]: ../artifacts/rollback-smoothness/candidate-r10l/rtc-item-gate-profile-cpu4/02/delivery-focus-01.json
[E268]: ../artifacts/rollback-smoothness/candidate-r10m/rtc-draw-noprofile-cpu4/l-m-review-01.json
[E269]: ../artifacts/rollback-smoothness/candidate-r10k/rtc-draw-profile-cpu4/01/delivery-focus-01.json
[E270]: ../artifacts/rollback-smoothness/candidate-r10l/rtc-delivery-motion-cpu4/01/performance.json
[E271]: ../artifacts/rollback-smoothness/candidate-r10l/rtc-delivery-motion-cpu4/01/motion-outcomes-01.json
[E272]: ../artifacts/rollback-smoothness/candidate-r10l/rtc-delivery-timing-cpu4/01/performance.json
[E273]: ../artifacts/rollback-smoothness/candidate-r10l/rtc-delivery-timing-cpu4/01/timing-focus-01.json
[E274]: ../artifacts/rollback-smoothness/candidate-r10l/rtc-delivery-timing-cpu4/01/window-geometry-01.json
[E275]: ../artifacts/rollback-smoothness/workspace-concurrency-review-01/desktop-monitor-bounds-01.json
[E276]: ../artifacts/rollback-smoothness/candidate-r10l/rtc-layout-overlap-cpu4/01/performance.json
[E277]: ../artifacts/rollback-smoothness/candidate-r10l/rtc-layout-overlap-cpu4/02/performance.json
[E278]: ../artifacts/rollback-smoothness/candidate-r10l/layout-pair-review-01.json
[E279]: ../artifacts/rollback-smoothness/restore-r10l-source-01/receipt.json
[E280]: ../artifacts/rollback-smoothness/candidate-r10l/rtc-layout-tiled-cpu4/01/performance.json
[E281]: ../artifacts/rollback-smoothness/candidate-r10l/foreground0-review-01.json
[E282]: ../artifacts/rollback-smoothness/candidate-r10l/rtc-foreground0-cpu4-reference1/01/layout-focus-01.json
[E283]: ../artifacts/rollback-smoothness/candidate-r10l/foreground0-reference1-review-01.json
[E284]: ../artifacts/rollback-smoothness/candidate-r10l/rtc-foreground0-cpu4-reference1-options/01/layout-focus-01.json
[E285]: ../artifacts/rollback-smoothness/candidate-r10l/foreground0-reference1-options-review-01.json
[E286]: ../artifacts/rollback-smoothness/candidate-r10n-build-01/receipt.json
[E287]: ../artifacts/rollback-smoothness/candidate-r10n-build-01/export-diff-01.json
[E288]: ../artifacts/rollback-smoothness/candidate-r10n-build-02/receipt.json
[E289]: ../artifacts/rollback-smoothness/candidate-r10n/rtc-foreground0-cpu4-reference1/01/invalid-layout-focus-01.json
[E290]: ../artifacts/rollback-smoothness/candidate-r10n/rtc-foreground0-cpu4-reference1/02/layout-focus-01.json
[E291]: ../artifacts/rollback-smoothness/candidate-r10n/foreground0-reference1-review-01.json
[E292]: ../artifacts/rollback-smoothness/restore-r10l-build-01/receipt.json
[E293]: ../artifacts/rollback-smoothness/matched-forward-frame-review-02/matched-forward-frames-01.json
[E294]: ../artifacts/rollback-smoothness/threadtime-retained-l-m-01/retained-threadtime-01.json
[E295]: ../artifacts/rollback-smoothness/candidate-r10l/rtc-cpu-wall-span-cpu4-reference1/01/performance.json
[E296]: ../artifacts/rollback-smoothness/candidate-r10l/cpu-wall-span-review-01.json
[E297]: ../artifacts/rollback-smoothness/raf-deadline-guard-v2-unit-01/receipt.json
[E298]: ../artifacts/rollback-smoothness/raf-deadline-guard-v2-unit-02/receipt.json
[E299]: ../artifacts/rollback-smoothness/continuous-raf-harness-validation-01/receipt.json
[E300]: ../artifacts/rollback-smoothness/continuous-raf-serving-validation-01/receipt.json
[E301]: ../artifacts/rollback-smoothness/candidate-r10o-build-01/receipt.json
[E302]: ../artifacts/rollback-smoothness/candidate-r10o/rtc-raf-delay300-guardoff-cpu4-reference1/01/performance.json
[E303]: ../artifacts/rollback-smoothness/candidate-r10o/rtc-raf-delay300-guardon-cpu4-reference1/01/performance.json
[E304]: ../artifacts/rollback-smoothness/candidate-r10o/raf-delay300-pair-review-01.json
[E305]: ../artifacts/rollback-smoothness/candidate-r10o/rtc-guardon-normal-motion-cpu4/01/performance.json
[E306]: ../artifacts/rollback-smoothness/candidate-r10o/rtc-guardon-normal-motion-cpu4/01/motion-outcomes-01.json
[E307]: ../artifacts/rollback-smoothness/raf-deadline-guard-v2-unit-03/receipt.json
[E308]: ../artifacts/rollback-smoothness/candidate-r10p-build-01/receipt.json
[E309]: ../artifacts/rollback-smoothness/candidate-r10p/rtc-raf-delay300-guardoff-cpu4-reference1/01/performance.json
[E310]: ../artifacts/rollback-smoothness/candidate-r10p/rtc-raf-delay300-guardon-cpu4-reference1/01/performance.json
[E311]: ../artifacts/rollback-smoothness/candidate-r10p/raf-delay300-pair-review-01.json
[E312]: ../artifacts/rollback-smoothness/candidate-r10o/rtc-guardon-normal-motion-cpu4/01/normal-largest-present-gaps-01.json
