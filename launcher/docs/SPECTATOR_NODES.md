# 换面与 Boss 旁观节点可行性

首次调查：2026-10-16。实施核对：2026-10-17。适用 TH06 / TH07 / TH08 / TH10 / TH11 / TH15 / TH20。
按用户要求排除 TH09 等对战游戏，TH09 仍沿用自己的 match/round 模型。
本文是 [旁观协议](SPECTATOR_PROTOCOL.md) 的节点调查与实施提案，证据为当前
源码，不把现有 Replay、练习跳关或 Boss 标志视为已经可公网中途旁观。

## 结论

**所有上述作品都具备换面和 Boss 注册事件的源码入口，可以建立这两类定位节点。**
原有分关 Replay 也提供了面首状态恢复的基础。因此建议用“面首初始化器 +
本面确认输入 + Boss 标记”作为第一版，优先补齐多人状态。Boss 当时的完整
world 快照可作为后续加速项，需单独测量体积、生成成本和恢复正确性。

六个合作 MP 的原生下放正在实施，已接入的作品与逐作验证状态见
[协议实施记录](SPECTATOR_PROTOCOL.md#9-公共前端接入与实施状态)。TH20 有单人
Replay 与事件入口，当前不提供 MP。Boss 刚入场节点按用户补充为可选。
所以“节点可定位”“面首可独立恢复”“Boss 可独立恢复”“进行中可加入”必须
分别验收，不能合成一个已经通过的结论。

## 1. 进入的节点与重建的节点

| 名称 | 含义 |
| --- | --- |
| 显示标记 | 时间线上的面首或 Boss 入场位置，只有事件与确认帧坐标 |
| 重建起点 | 含完整初始化状态、可以在新 Runtime 独立恢复的 checkpoint |
| 入场播放点 | 新观众开始看画面的位置，可晚于重建起点 |
| 直播边缘 | 当前确认历史允许播放的最新位置，保留播放缓冲 |

例如玩家已在四面 Boss 战：面首 checkpoint 在 S，Boss 入场标记在 B，
新观众加入时最新确认前沿为 L。只要保存 `checkpoint(S) + inputs[S,L)`，
观众就可以后台从 S 重演，到 B 后展示该段，或继续重演到当次固定直播目标。
这不要求观众看完四面道中，也不要求第一版把 Boss 世界完整序列化。

**入口策略提案**：非对战作品以加入时最近的已确认换面/Boss 标记作为当前段
起点，提供“回到直播”。若最近标记没有独立快照，后台从其依赖的面首重建。
入场目标在加入时固定，不能因后来出现下一个 Boss 而不断移动。该段保留首次
调查的提案；当前合作流已接入面首 checkpoint，Boss 节点为可选，逐作实测范围
见协议第 9 节。TH09 保持已有默认播放方式。

## 2. 逐作源码依据

下表中的路径相对各作品仓库。TH10 读取 `eagler-mp` 分支，TH11 读取现有
`worktrees/th11-multiplayer` 的 `eagler-mp`，没有切换普通分支或新建工作树。

| 作品 | 面首基础 | Boss 事件入口 | 还需补齐的状态与边界 |
| --- | --- | --- | --- |
| TH06 | `src/ReplayManager.cpp` 的分面录制/恢复，保存 seed、rank、残机、Bomb、火力、点数等；`ReplayExtension` 已保存多人分面资源和贡献 | `src/EclManager.cpp` 的 `ECL_OPCODE_BOSSSET` 设置 boss slot | 面首 score 的现有恢复会读取上一面结束值，新初始化器必须自包含；全部席位、共享规则和输入边沿需一起恢复 |
| TH07 | `src/ReplayManager.hpp` 的 `StageReplayData`、`ReplayManager.cpp` 与 `ReplayExtension.cpp`，含 Cherry、graze、extend 和多人资源/贡献 | `src/EclManager.cpp` 的 Boss 注册分支 | Cherry/Border、四面 Boss 连战规则和多 slot 要覆盖；Phantasm 的现有 Replay 复用 Extra 槽，协议节点必须保存真实路线/关卡身份 |
| TH08 | `th08_web/cpp/game/ReplayPlayback.cpp::begin` / `ReplayRecording.cpp::begin` 已读写分面状态；多人 `ReplayArchive` 有 stage stamp | `cpp/game/EclLifecycle.cpp::execute_lifecycle` 的 opcode 127，设置 `boss_slots` | 队伍与全部玩家状态、刻符/人妖槽、时刻、extend、路线及 Final A/B 身份；当前多人 `prepare_replay` 仍从整局起点重演到 StageFrame，stage bookmark 尚不是独立 checkpoint |
| TH10 | `th10_web/cpp/game/Replay.cpp` 的 `ReplayStage::capture_game/restore_game` 与 `Replay::activate_stage`，含信仰计时、位置历史和 Option 状态 | `cpp/game/EnemyStateCommands.cpp` 的 opcode 0x14c，设置 `boss_slots` | 多人逐席位经济/位置/Option、共同进度，不能只恢复 P1 的原版 ReplayStage；Option 恢复有机体特例，须沿用作品语义 |
| TH11 | `th11_web/cpp/game/Replay.hpp` 的 `ReplayStageState`、`Replay.cpp`、`ReplayRecorder.cpp` 与 `GameSession.cpp` 的分面路径 | `cpp/game/EnemyStateCommands.cpp` 的 opcode 0x14c，设置 `w.bosses` | 全部席位、支援配置、经济、救援/转移状态；`GameSessionMultiplayer.cpp` 的 next_stage 路径及前后背景重叠不能被普通 open_stage 简单替换 |
| TH15 | `th15_web/cpp/game/ReplayStageSnapshot.cpp` / `SessionReplay.cpp` 有 0x238 字节分面快照和恢复路径 | `cpp/game/EnemyCommands.cpp` 的 opcode 512，设置 `boss_ids`；524 另有原生章节事件 | 所有席位、碎片、章奖励阈值、死亡/贡献与关卡经济；原生章节 retry 会扣火力并增加重试计数，不能直接用于无副作用的观众定位 |
| TH20 | `source_reconstruction/replay_system/replay.hpp` 的 `StageRecord` 为 0x2a0，`initialize.cpp` 恢复 seed、player_table 和石头选择等 | 已编入游戏的 `source_reconstruction/gameplay/enemy_opcode_state.cpp`，opcode 512 注册 Boss identifier | 当前无 MP 数据流/确认前沿；可先复用为单人 Replay 节点。以后接 MP 还需完整输入与身份契约，不能仅开放 Launcher 入口 |

这些是恢复基础，**原版单人分面 Replay 状态不自动覆盖多人面首状态**。需要
逐字段对照作品的多人权威状态清单，验证恢复后同帧一致，而不是直接复制这些
原版结构作为新的公共网络格式。

## 3. 如何认定节点

### 换面

新面的资源与 world 已按原作路径准备好、第一份 gameplay 输入尚未消费时，
在作品定义的边界捕获初始化器，记下一帧输入序号 S。仅观察 `stage++` 或
加载画面出现不够：它们可能早于完整资源建立，也可能仍有上面的背景/动画 owner。
有跨面状态的作品应携带必要状态，或把可恢复点放到另一个已经证明的边界。

节点身份至少含 room run、simulation generation、真实 stage/route key 和 S。
新 run、原生 Restart、Extra/Phantasm、分支路线和重复进入同一面不能共用身份。
stage-local frame 归零也不能使网络历史序号被误认为倒退。

### Boss

2026-10-16 用户补充：这里指刚进入 Boss 的节点，难实现或不能证明正确的作品
可以不做。本轮必须先完成中途旁观、换面恢复、播放控制和满员大厅入口的实测。
只有具备真实注册事件及确认帧证据的作品才发布 Boss 节点，不制造占位标记。

优先观察原生逻辑中的 Boss 注册/注销与实体生命周期。一次新遭遇的确认注册
产生一个 Boss 入场节点，节点可以包含道中 Boss 和关底 Boss。多 Boss 同场、
同一 slot 换实体、实体池复用和同一实体重复 set-boss 需要作品适配层去重和
分组，不能仅凭 slot 0、血条显隐、BGM 或 wall clock 判断。

本调查定位到的是**注册入场**，可能包含随后的对话或入场动画。若还要提供
“对话结束后开战”，应从原生战斗解锁事件增加另一种标记，不能把两者混称。
每张符卡/非符不是本轮必需节点，不把每次生命条变化自动当作新 Boss。

标记与确认输出同源。预测帧中观察到的事件保存在该帧记录，回滚时撤销，
reconcile 后只发布一次。事件发生于 tick F 时，若标记表示事件执行后的状态，
其定位坐标定义为下一帧待输入点 F+1；面首 S 则表示第一份面内输入之前。
作品需要证明这两个边界在 native 调度中的实际位置，避免定位差一帧。

## 4. 推荐实现与历史保留

1. 公共元数据使用 `kind: stage | boss`、稳定 node id、frame、stage/route、
   所依赖的 checkpoint id，以及必要的 Boss encounter/slot 身份。该语义模型
   的公共候选已实现为 `E?SV/1` 与 `spectator-playback`。详见旁观协议 4.1a/9；
   各作原生初始化器与控制桥已接入，六作本地旁观专项验收已完成。公共能力与接口接入
   不等于该作品已经通过 native 节点恢复，当前证据以协议第 9 节为准。
2. 面首记录完整、版本化、无指针的初始化器，再发布从 S 起连续的全部玩家
   确认输入。Boss 节点先引用本面起点，不要求它自己能造完整 world。
3. Relay 始终保留可重建的依赖链。最新标记从面首变成 Boss 时，**不能删除
   本面道中输入**，否则缺少 Boss 快照的新观众无法到达该点。
4. 换面后也要等新 checkpoint 已完整确认并可独立恢复，才切换新观众 bootstrap。
   边界尚未完成时使用上一有效起点重演，不能让进行中加入只能等下一个 Boss。
5. 老观众本地继续保留已收到的旧段。新观众得到最新有效重建链，不承诺获得
   服务器此前所有面的历史。用于后台重建的前缀和可见时间线范围分别声明。
6. 后台重建仍执行固定 tick 和相同输入，不重新采样设备，不改原作冷却/RNG，
   不触发玩家成绩、自动录像、文件保存或重复声音。限制每次补算工作预算并报告
   进度。目标很远也不能通过跳过逻辑帧缩短等待。
7. 如实测面内重演仍太慢，再增加最近 Boss 的独立 checkpoint。它需包含
   敌人/ECL VM、弹幕、激光、道具、玩家/Option、RNG、规则、时钟、生命周期及
   作品要求的呈现/音频状态，连同内容/ABI、资源重绑和容量预算独立验证。
   现有本机 rollback snapshot 不自动成为可跨进程序列化的网络快照。

首次调查的通用准入历史 8192 包约对应 136 秒的单帧流，TH09 时间线 32401 包
也不代表其他作品完整一面的容量。当前合作协议的面内历史上限为 108000 输入帧
且最多 16 MiB，接收队列也有独立的包数/字节限制，详见协议第 4.1a 节。
这些是有界预算，不能替代最长段实测。长道中、Extra、耐久 Boss、暂停和慢观众
都可能超过它。必须测量真实最长段的输入与初始化器字节量，再设计公共限额、
背压和明确失败策略；不能通过无限加配额或无提示丢前缀解决。

## 5. 最小实施与验收路线

| 阶段 | 交付与通过条件 |
| --- | --- |
| A 只读标记 | 各作只采集换面/Boss 事件，绑定确认帧；验证正常与 rollback 重演不重复、不漏事件，玩家状态与无标记版本一致 |
| B 面首恢复 | 从实际进行的第二面或更后面恢复全部席位，使用此前真实输入到相同逻辑帧；对比顺播与重建的 canonical state 并定位首个分歧 |
| C Boss 定位 | 从面首后台重演到道中/关底 Boss，验证固定目标、对话/动画与 F/F+1 边界，多 Boss、重注册及关卡分支不误标 |
| D 中途加入 | 真实 Launcher + Relay + Release Runtime，在道中、Boss、换面瞬间分别加入；已在房间的玩家持续前进，观众进入当前段并可回直播 |
| E 压力与优化 | 测量重建时长、主线程阻塞、初始化器/输入/档案峰值；覆盖长局、旧观众暂停、换面时仍在 bootstrap、重连与队列溢出，再决定 Boss 快照 |

优先验证 TH06/TH07 的既有多人分面资源路径，再验证 TH08/TH10/TH11/TH15
各自的多人初始化 owner。该顺序是降低未知量的实现建议，不降低后四作的
必须验收范围。TH20 暂列单人节点/未来 MP 接缝，不宣称支持联机旁观。

## 6. 首次调查的证据范围

以下保留首次调查的源码边界，迁移后的真实状态以 [实施状态](SPECTATOR_PROTOCOL.md#9-公共前端接入与实施状态) 为准。
调查逐作阅读了实际阶段 Replay、Boss 注册和当时的 MP 接收/启动代码。
TH06/TH07 旁观输入期望 frame 从 0 连续递增；TH08 `BeginSpectator` 要求 next
为 0；TH10、TH11、TH15 的旁观接收计数同样从 0 起。这证明迁移前的接收边界，
不否定它们借助分面状态实现更晚节点的可行性。

首次调查未更改游戏逻辑，也不构成跨作阶段恢复、Boss 快照、真实中途加入或性能
PASS。普通 Replay 的已有函数是工程入口，独立多人恢复和实际入场体感仍需
上述门禁。TH20 的 Boss/面首结论来自实际游戏 owner，未使用其带 TH10 key
的练习区段表作为 TH20 节点证据。
