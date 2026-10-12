# TH08MP 回滚日志优化工作记录

状态：候选正确性与正式性能验收进行中；下方待填项将在完成后由真实结果替换。

## 1. 范围与来源

- 用户目标：低输入延迟、顺畅呈现，允许网络信息不足导致的平滑降速，不能把大段主线程停顿当作正常降速。仅使用 nig 操作；不使用手机，桌面构造触控移动。
- 实际 MP 基线：eagler-mp 的 05199cdb7402510814480a4d2ef6db429e49b03f；普通 th08-eagler 主树的 b0032f5 不是本次 MP 基线。
- 候选树：D:/workspace/eagler/worktrees/th08-rollback-journal-4x，分支 experiment/th08-rollback-journal-4x。
- 独立对照树：D:/workspace/eagler/worktrees/th08-rollback-journal-baseline。
- 两树共享库均固定在原 gitlink eae1bdcfa930cc06d564701e0fa2cccacd819fd9。未修改共享库源码。
- 未修改用户普通主树、TH09 已验收树或测试站；本记录不代表线上站点已使用候选。
- 构建命令：node portable/build.mjs --th08 --multiplayer；诊断另用 --multiplayer-fixtures。两者 -O2；正式版无 TH_MULTIPLAYER_FIXTURES profiler/fixture exports。

## 2. TH09 可归因的改动

已交付树为 D:/workspace/eagler/worktrees/th09-subpixel-journal-4x：
- a3a59db525d2e76faf9bb683bc624d455d3aa3ae：记录用户已验证的 managed touch / D0 / subpixel 基线。
- cc35222ed5f7fe652821cfec8f44e055da0c3b54：PlayerShots 检查点薄优化。
- 5352dcfa877e30e29b1bf97a72936f69522eda5d：报告与证据。
- 完整 TH09 报告：docs/multiplayer/rollback-subpixel-journal-work.md。

具体优化将每检查点两位玩家 128 × 2 × 1144 B = 286 KiB 的 PlayerShots 整池复制改为活跃连续区间快照，以及休眠槽位首次写入前保存完整旧值。包含 VM、trail、指针和 padding；完整 hash oracle 未删字段。普通创建、Marisa 预初始化、回合清理均有覆盖。

用户原先已修好的 managed touch、固定 D0、本地亚像素修复，以及已有的网络缺输入降速和恢复调度，均被保留。D0 是本地输入延迟零帧，不等于不预测或不回滚；历史仍为 8。不能把用户对整个版本的良好体感全部归给上述 PlayerShots 补丁。

TH09 正式 ABBA（双独立 Chrome/native GPU、两端 CPU4、Page 目标 120 Hz 触控、79±5 ms 应用发送延迟）中，累计 native callback 墙钟 45.687635 s → 41.765230 s（-8.59%），逻辑推进率 45.5152 → 46.6233 Hz（+2.43%）。新逻辑状态 >50 ms 的间隔 35 → 41，>100 ms 1 → 0，>250 ms 两版均 0：长尾并非每项改善。触控 tape 不同，不是相同逐帧输入的确定性 CPU 微基准。

TH09 产物 WASM：
- 用户基线 cb03c6df0c460360d2d3548e63556a9b74bf18ac7f6613bdf8231a4e03c30480
- 候选 2ae38afdffaac02d94924af07e5d385535964c8b57154589ce6a93fac3b04ddd

## 3. TH08 保留的两项生产改动

### GUI 转场数组条件快照

GuiImplState 大小 143544 B，其中 transition[168] 为 113568 B。WorldJournal 原来每个检查点都保存整个 display。

现在 inventory 按连续前缀、完整 transition 数组、后缀分割。transition_count 在 Always 后缀中；完整 AuditHash 仍按相同顺序包含整个数组，休眠字节也未排除。

- count 非零时保存全部 168 个 VM。count 是执行后剩余数，不能当作有效前缀长度。
- count 为零、区间内也未初始化时，省去 113568 B 的 journal 复制。
- GuiFlow::setup 在初始 memset 和非初始动画初始化之前调用所有者；首次写入保存完整旧值，同一个区间再次 Touch 不重复增加载荷。
- 扩展检查点内才首次写入也恢复到区间起点。
- Bind 成功后安装 callback，Clear 只解绑自己的 callback；拥有关系不进入被恢复的 GuiLoadContext。
- Update 和 semantic Draw 都可能改 VM，关闭几何输出也会修改 world_matrix；活跃数组一律完整保存。
- GuiController::snapshot_presentation 的独立 display 复制仍保留，本补丁不声称省掉那部分复制。

涉及 WorldJournal.hpp/.cpp、GuiFlow.hpp/.cpp。

### Enemy 记录容器复用

EnemyJournal 的 deque<Record> 改为固定 8 槽环形记录。退休时 owners.clear() 立即释放历史 ECL context 引用，同时保留 vector 容量供该槽后续使用；整体 Clear 释放缓存。

历史顺序、第 9 条拒绝、extend、先还原字节再倒序恢复 shared_ptr、再删除 checkpoint 时不存在的 VM，均与原协议一致。未通过 memcpy 还原 C++ owning words，未延迟已退休引用的释放。

源码硬边界是 8 条记录、每条最多 481 个去重 owner。保留容量为各槽 capacity × sizeof(ContextOwners) 的总和，没有新设一个按字节计的缓存上限；不能将特定 ABI/STL 的容量估算写成源码保证。旧有字节 arena 的预算没有变。首次分配和高水位容量增长仍可能发生，平稳复用后的分配模式减少不自动等于总体耗时改善。

涉及 EnemyJournal.hpp/.cpp。

## 4. 正确性检查与旧 fixture 修正

诊断 c3 WASM：2eb30712d8be66f2e85d6c8f1d15988b6d6f416d2a412b87fafa7a04a5a7a92c。

已通过：
- Enemy owner ring 的 8 种 undo 位置、环绕后继续捕获、引用存活/退休、满 8 条拒绝第 9 条。
- Screen callbacks、fixed pools/geometry、Resources owner probes。
- GUI 7 项：非零休眠旧字节、count=1 整数组、首次写入/重复 Touch、extend、active→reset→再激活、两个真实 setup 分支在关闭历史期间先拒绝写入。实际字节差 result[63]=113568。
- 双人射击、focused Bomb、native death、unfocused Bomb，初始及随后 24/60 tick 的 native Update + semantic Draw roundtrip。

待填：三人、live bullets、最终汇总。

边界：GUI 活跃数组的专门字节测试使用合成写入，并未执行真实转场动画；实际 setup 用例验证的是写入前拒绝，没有把成功加载新 stage 后的资源图变化当作可回滚普通帧。现有世界场景 roundtrip 运行真实 Update/Draw，但不能据此声称已覆盖每种关卡过渡。

修正了两个旧 fixture 假设：
1. EnemyPopulation 基线已预分配 481 个 VM，旧测试却要求休眠 slot(1) 为 null。改为校验休眠 VM 的同一对象与完整还原 hash。
2. 离线 world probe 先提交 P1 buttons，随后普通共享输入发布仍从物理键盘采样并覆盖 P1；旧 focused Bomb 因键盘为空在第一次 forward 就失败。原基线也复现同一 step 20。fixture 现在按 smoke.commit 的虚拟键码同步 P1，现有键盘 RAII 仍负责还原。

这些修正只影响诊断输入/断言，没有调整生产输入规则，也没有跳过原 Bomb 场景。诊断 run.json 保存 build/source hash 与 sourceStableBefore/After。

## 5. 正式 ABBA 协议与结果

预设：
- 两个独立的 headed Chrome，实际游戏 WebGL 必须为 native GPU。
- 两端 CPU slowdown=4；不用手机，不把这个限速当手机型号模拟。
- 正式 -O2，无 native profiler/fixture exports；普通 authored gameplay，不注入 dense bullet。
- D0、H8、checkpoint span=1、现有生产 frontier 模式。
- 两端 Page 独立 timer 目标 120 Hz 的触控下按/移动，晚到合并为最新位置；保存 events/coalesced/lateness/tape hash。
- RTC 应用发送统一 79±5 ms 延迟，包括 repair。这不是测出的公网 RTT。正式 rows 与 after 必须始终 route=RTC，正式发送增量必须大于 0。
- 全部原生产优化默认开启：Bomb lazy、shot/record sparse、title spells lazy、skip resim visual/geometry、backbuffer metadata only 等。
- A1→B1→B2→A2；先冻结 A/B WASM、loader、driver、hook、游戏输入、字体、真实 relay 的完整模块图及 ws/acorn 依赖。除四个 build 文件外，manifest 内容必须相同。
- warmup 到 180；统计首次实际观察到 frame≥240 至 1200 的主体，使用每端实际 frames/elapsed，不能硬填 960 推进。
- 同一次双端运行 canonical 相同且确认到 1199；跨次 wall-clock touch tapes 不同，不要求它们的 canonical 相等。
- 正常结束后核实关闭两 browser、HTTP thread 和已知 relay 子进程。没有执行全局浏览器/端口清理。

指标口径：
- callbackMs：进入 native rAF 回调至 audio pump 后的墙钟。不是 CPU self，也不包含 rAF 排队或 runtimeFinish observer 尾部。
- presentGapMs：observer 看到 graphics Present 计数变化的间隔。不是物理扫描，也不保证都是新逻辑画面。
- advanceGapMs：observer 看到 net logical frontier 变化的间隔。允许一次 callback 多 tick。
- 首次变化仅建立间隔 anchor；保留原始 rows，不称这些分布覆盖首段。
- 1200 后的最终确认/reconcile 尾部不入这些时序行；不将主体指标称为全程等待时间。
- audioClock 来自普通 sdl_loop_time，不是 WebAudio 时钟。music off，本轮不验收真实声音/BGM。
- 不可用的正式 profiler 字段为 null，不能以 0 冒充未发生 capture/restore。
- 未覆盖远端独立 relay、公网路径、完整 managed Launcher、移动设备和全部关卡。

待填：冻结身份、ABBA 主表、异常/长尾、可承诺范围。

## 6. 被撤回的 Replay 缓冲复用

最初尝试了 World Record 固定环与 ReplayRecording 可复用深拷贝缓存，独立 WASI -O2 检查共 245 项通过，预热后 capture 分配次数为 0。但最后一轮时间数据并不支持保留：

| 场景 | 旧版两次耗时 ms | 复用版两次耗时 ms |
| --- | ---: | ---: |
| 普通 MP 的空记录形状，depth 1 | 17.0888 / 14.3360 | 19.2645 / 17.3966 |
| 空记录，depth 8 | 14.9867 / 14.6882 | 17.8204 / 18.3094 |
| 填充记录，depth 1 | 48.4289 / 52.1526 | 71.0686 / 60.6914 |
| 填充记录，depth 8 | 54.8183 / 56.3116 | 56.3286 / 59.4522 |

这属于 WASI 组件耗时，不是 Emscripten CPU4 游戏成绩。正常 MP 在 GameplayScene 中 recording_game=false，普通 ReplayRecording 不持续收集输入，MP 的录像来自另一所有者。生产默认诊断中 recording owner 总共约 11 ms，远小于其他所有者，因而该实验优先级也低。

该实现已从生产源码撤回，完整候选与组件测试保留在 rollback-journal-rejected-replay-retention.patch；c1/c2/c3/c4/c5 的原始组件记录仍保留在 artifacts/multiplayer-tests。最终产物不包含 ReplayCheckpoint.hpp，不改变原 replay 深拷贝行为。

## 7. 仍需后续单独处理的调度边界

TH08 当前 Correct 同步重演到原前沿，没有按 wall clock 分片。不能只加 while 的 8 ms break：
- 分片中 Pump 新收包可能产生针对已重演区间的更早 request；原 EndCorrection 无条件清 request，会丢掉它。
- Reconcile 的 true 是 confirmed Replay 可以输出的资格，pending 不能冒充完成。
- CanCapture=false 不足以阻止 GameHost 在 pointer/poll 中读取历史中间态，从而影响真实触控取消/移动/释放。
- 高刷尾部可独立 Draw，partial correction 不能只靠最终 Present suppression 遮住。
- 一个完整 undo 或单 tick 仍可能超过预算，分片只限制继续启动下一 tick，不是可抢占的硬上限。

本轮不修改这套输入/恢复/呈现协议；不承诺所有设备无掉帧、不承诺原始 rAF 等待已被量化消除。若继续做，应针对恢复中途收包、跨片输入与输出、60/高刷呈现建立独立验收，再实施 Driver + NetplayRuntime + GameHost 的小范围状态机改动。

## 8. 交付与复现

待填：本地提交、正式候选 WASM、完整证据索引、复现命令。

生产改动和诊断证据均保留在独立 git worktree。没有 push、merge 或部署动作；使用候选时必须以 build.json 与 WASM SHA 识别来源，不能从普通主树重新构建冒充该版本。

