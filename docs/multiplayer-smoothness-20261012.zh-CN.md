# TH10MP 流畅度移植记录（2026-10-12）

> 状态：正式 r6 的真实 rAF、900 帧样本在 1× / 4× 都完成 RTC、完整 canonical、确认音频及身份检查；但 4× 逻辑仅 7.53 / 7.60 Hz，频繁出现超过 100 ms 的停顿，性能严重不合格。
> 真实原生菜单 Restart 的完整 canonical 仍失败；仅有有限生命周期观察完成，当前 TH10 候选不可推广。
> 原版异常、r2 / r3 未达标、r4 失步、r5 正常 4× 超时及各轮 Restart 失败均保留。r5 定向 fresh-launch 修复不被扩大；TH10 尚无人体验收，最终像素运动连续性未测。
> 本文记录当前机制、验证边界和待办，不承诺改进百分比。

## 1. 范围与版本身份

| 项目 | 当前记录 |
| --- | --- |
| 工作树 | `D:/workspace/eagler/worktrees/th10-rollback-smoothness` |
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
