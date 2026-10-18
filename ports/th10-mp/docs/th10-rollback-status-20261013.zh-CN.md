# TH10MP 回滚优化：当前交付边界

日期：2026-10-13。本文是当前状态摘要；详细原始记录见 `multiplayer-smoothness-20261012.zh-CN.md` 第 10.55–10.58 节及文末证据索引。保留旧失败，不用新进程的健康结果替旧失败归因。

## 结论

TH10 尚不可宣称达到用户要求的整体交付标准。TH11 尚未开始实施。当前存在一项已经实测的局部收益：在实际 RAF 回调已到达包装器、但被测试器延迟派发 300 ms，且普通定时器仍能运行时，持续有界补偿保持更新和画面提交。它不解决正常回滚恢复的全部停显，也不证明操作系统根因。

源码目录：`D:/workspace/eagler/worktrees/th10-rollback-smoothness`。

分支：`experiment/rollback-smoothness`；基准 HEAD：`2e5d5464b2b28ef3fb936234660b773b604c91ce`；存在未提交修改。common：`77666259b4bb14b965a34a1f38a4e2ec88c289ac`。没有推送、合并或部署本候选。

## 已经验证的局部改进

冻结候选 `candidate-r10p/runtime`，正式 O2、非 diagnostic 构建，无 LTO。

- WASM SHA-256：`551c2c096e63d3fbb7274ccfe6a143f57e53e47613290eba20ffb3a329a7104f`。
- 源码摘要：`bb347d8b8c69d754b177062dc8f9f3a91a0c0ad913f85045b27b6cae1701dfa6`，冻结 563 项构建输入。
- RAF guard 仍默认关闭。其机制是同一 pending RAF 只保留一个拥有唯一序号的 timeout；完整回调退出后才重新计时。首个截止时间为 25 ms，后续为 1000/60 ms，不追赶过期定时器，不重入，不额外发起 RAF。
- stop、pause、hidden、失去 owner/epoch 或资格时不继续补偿；定时器不参与真实屏幕刷新节奏采样。

同一 r10p WASM 的 OFF/ON 对照：600 帧、实际本地 RTC、50±5 ms 单程应用层延迟、P0 CPU 4× / P1 CPU 1×、120 Hz 桌面合成触控、流式 BGM；在 frame 300 后扣住一个实际 RAF 回调约 300 ms。

| P0 故障覆盖区间 | OFF | ON |
| --- | ---: | ---: |
| 实际扣留时间 | 306.8 ms | 301.3 ms |
| 原生回调 / 画面提交 | 0 / 0 | 9 / 9 |
| 完成逻辑高水位增加 | 0 帧 | 16 帧 |
| 实际触控提交 | 28 | 27 |
| 最大画面提交间隔 | 310.1 ms | 44.5 ms |
| 最大逻辑推进间隔 | 324.0 ms | 45.0 ms |

双方完整 44 字状态、确认音频、RTC 路由、原始 RAF 时间戳、一次扣留/释放及清理检查通过。原生 guard 单测为 23 组 / 3445 次检查通过；故障注入组件为 15 组 / 229 次检查通过。这里的画面提交不是物理屏幕扫描或输入到光子证明。ON 的整场 P0 严格 cadence 仍不通过，不能把上表扩展成全场流畅性承诺。

证据根目录：`D:/workspace/eagler/worktrees/th10-rollback-smoothness/artifacts/rollback-smoothness/`。

- `candidate-r10p-build-01/receipt.json`
- `raf-deadline-guard-v2-unit-03/receipt.json`
- `continuous-raf-harness-validation-01/receipt.json`
- `candidate-r10p/rtc-raf-delay300-guardoff-cpu4-reference1/01/performance.json`
- `candidate-r10p/rtc-raf-delay300-guardon-cpu4-reference1/01/performance.json`
- `candidate-r10p/raf-delay300-pair-review-01.json`

## 尚未解决的正常游玩问题

较早冻结 r10o 的正常双端 4×、1200 帧、120 Hz 合成触控与 P0 实际像素采集场次没有注入 RAF 故障。完整状态和音频一致，但 P0 出现 124.2 ms 画面提交缺口，逻辑推进最大缺口 115.1 ms；逻辑为 P0 54.159 / P1 54.289 Hz。双方 timer service 为 0，因此 r10p 缩短补偿链截止时间不能被当作此失败的修复。

这段时间 RAF 和触控继续运行。恢复终点固定于 1076，最后完整高水位为 1075。七个不同历史帧实际执行了十一笔事务；迟到输入使其中四笔需要重做，五次回调在完成部分恢复后让出。当前输入核心已过滤重复确认、预测正确以及废弃未来的输入，未发现可以安全删除这四笔重做的依据。

不能以忽略迟到输入、呈现恢复到一半的世界或取消预算换成单次长回调来消除这个失败。

该场本地玩家 / 远端玩家 / 子弹的有效像素窗口为 3 / 0 / 3。远端缺少足够连续证据，因此运动验收失败；不是已证明远端运动正确，也不是已观察到三类对象都有运动错误。

原始报告：`candidate-r10o/rtc-guardon-normal-motion-cpu4/01/performance.json`，SHA-256 `ecb9379250900f12ee341a7ca97a45e9073a6f021eb11714f6444edacdcfccf1`。聚焦记录为同目录 `normal-largest-present-gaps-01.json` 和 `motion-outcomes-01.json`。

## 为什么没有结束

技术上，TH10 的 authored Draw 承担 VM、变换 dirty bits、相机、雾、fade 和部分状态更新，不能整体当作历史纯输出删除。当前已经省略符合条件的历史 sprite 输出、空闲 Item 调用和已确认前缀的 undo 捕获，仍有必要工作成本。

执行上，调查分支扩展过多，单项正确性通过、受控故障改进和整作体验被分别验证，却没有足够及时收束为明确的交付边界。这是执行方式的问题，不应归咎于用户断网或编译环境。用更多测试次数或越来越长的报告，不能替代尚未关闭的体验问题。

## 后续工作边界

1. 保留 r10p 的局部补偿成果和原始故障 A/B，不再把它算作正常历史恢复的解决方案。
2. 后续生产改动必须直接降低必要历史事务成本，或提供与可变回滚世界隔离的完整呈现数据。先证明状态与资源所有权，再进行一次相同条件的验证；不重复无新假设的参数、LTO、窗口或环境扫描。
3. 如果只能通过独立呈现架构解决，要明确承认那是新增工程，不能称为收尾小修；必须同时检验视觉与碰撞/输入状态的关系，不能只提高 Present 计数。
4. 正常 4× 的长停显、实际对象运动连续性和既有 relay 推进缺口分别关闭后，才能宣称整体可交付。手机测试不在本任务中；完整 managed/WAN 链路尚无验收结论。
5. TH10 未达标前不转去实现 TH11。
