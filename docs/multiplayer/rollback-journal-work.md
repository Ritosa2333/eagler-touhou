# TH08MP 回滚日志优化工作记录

状态：2026-10-12，本轮候选实施、原生正确性检查和正式 CPU4 对照已完成。两项生产优化已有独立提交，正式产物已保存，尚未部署。可以确认减少了符合条件的快照复制及记录容器分配；本轮数据不足以承诺固定的整局提速百分比或零掉帧。

机器可读证据：同目录 rollback-journal-evidence.json。它保存构建身份、源文件哈希核对、原始报告 SHA-256、每端统计、全部对照与失败记录索引。统计从原始 rows 重新计算，并逐项对照原 summary；原始数据未改写。

## 1. 范围与来源

- 用户目标：低输入延迟、顺畅呈现，允许网络信息不足导致的平滑降速，不能把大段主线程停顿当作正常降速。仅使用 nig 操作；不使用手机，桌面构造触控移动。
- 实际 MP 基线：eagler-mp 的 05199cdb7402510814480a4d2ef6db429e49b03f；普通 th08-eagler 主树的 b0032f5 不是本次 MP 基线。
- 候选树：D:/workspace/eagler/worktrees/th08-rollback-journal-4x，分支 experiment/th08-rollback-journal-4x。
- 独立对照树：D:/workspace/eagler/worktrees/th08-rollback-journal-baseline。
- 对照树含测试脚本对齐修改；生产源码仍为 05199cd，正式产物的 496 项构建输入已逐项核对。不能把它描述为整棵干净工作树。
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

TH09 正式 ABBA（双独立 Chrome/native GPU、两端 CPU4、Page 目标 120 Hz 触控、中继增加 79±5 ms 输入包转发等待，非实测 RTT）中，累计 native callback 墙钟 45.687635 s → 41.765230 s（-8.59%），逻辑推进率 45.5152 → 46.6233 Hz（+2.43%）。新逻辑状态 >50 ms 的间隔 35 → 41，>100 ms 1 → 0，>250 ms 两版均 0：长尾并非每项改善。触控 tape 不同，不是相同逐帧输入的确定性 CPU 微基准。

既有降速按真实输入缺口工作：0–4f 不增加目标间隔，5f/6f/≥7f 逐步达到约 1.333/1.667/2 倍，并平滑变化。它不是本次新增的、按稳定延迟自适应学习预测余量的算法；对端设备未及时产生输入也可能表现为输入缺口。

已保存的 TH09 报告还记录了 26,077 个符合正文条件的 game rAF：登记到进入最大 30.375 → 29.160 ms，单次回调最大 22.430 → 32.750 ms。这一组没有复现 >50 ms 的登记等待或回调执行，不能反推此前数百毫秒停顿已被 PlayerShots 补丁完全修复。强制持续 8f 的诊断中仍保存了约 589.19/238.84 ms 的最大 native 调用，原因未隔离；既有 Enemy 拥有者增长的恢复边界也未由此补丁解决。

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

三人同样的四场景及初始/+24/+60 tick roundtrip 已通过。双人、三人合计 24 次世界 roundtrip；不要把每次重复执行的 GUI 7 项统计为新的独立场景。live-bullets 的复用、额外变换、附近/全局 ECL 清除、全池清除共 5 项也已通过完整字节 oracle。

| 最终检查 | 结果 | 耗时 | 原始文件，均在 artifacts/rollback-journal-4x 下 |
| --- | --- | ---: | --- |
| Enemy / Screen / Pools / Resources / GUI owner probes | PASS | 34.871 s | candidate-owners-c4/enemy.json |
| 双人四场景，12 次 world roundtrip | PASS | 287.109 s | candidate-owners-c4/world2.json |
| 三人四场景，12 次 world roundtrip | PASS | 296.640 s | candidate-world3-c1/world3.json |
| Live bullets 5 项 | PASS | 13.556 s | candidate-bullets-c1/bullets.json |

各 run.json 的 sourceStableBefore/After 均为 true；诊断 WASM 的 505 项源文件输入在收尾时再次全部匹配 build.json。上述耗时是整套检查耗时，包含 oracle、启动及检查工作，不能当作生产帧耗时。

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

### 冻结身份与前置检查

两组依次为 formal-journal-abba-c2 和 formal-journal-abba-c3-recheck，全部使用相同的 A/B 冻结文件集合。第二组是在第一组 A2 出现异常后追加的完整 ABBA，不覆盖或删除第一组。

- 测试源码 HEAD：52af4d28a8227f2ebf66a3e9c33e5fd070e872af。
- A WASM：050312dc6ccb37b7223e808b7d19a79c6a0666f14f84890a022cda73312ea4b3。
- B WASM：8ea9b23fc9ffbdb25ddd1d5bf1a94284fef5e60df00390ec735557ff27e71f55。
- A manifest：3a77b864d1b836ebcf53645bbec024ea438acfa547e974e0b90d3bb88f28698d。
- B manifest：0c5fda8a742e381ef6cf1f2db78f10fb1ac9313cd907e98668ed09482caf908b。
- 每版 68 个冻结文件；仅 build.json、runtime-build.json、th08-sdl.mjs、th08-sdl.wasm 四项不同。
- Chrome 155.0.8059.39，实际游戏 renderer 为 ANGLE / Intel UHD Graphics / D3D11；没有把软件 GPU 标成 native GPU。

首个 formal-journal-abba-c1/A1 因测试输入接线错误未进入测量：hook 提前捕获第 0 帧，却缺少诊断版专用的 capture 发送公告，导致原生 CanCapture 不再进入 capture/send 分支。52af4d2 将直接注入限制在 fixture，正式版从 warmup 起完整使用原生 SDL → CaptureLocal → network.Captured。原生 frame-zero RouteBootstrap 与启动屏障保留；正式 WASM 未因该修正改变。失败记录有 0 个测量行，不能计入性能成绩。

### 八次稳定延迟运行

下表每行合并当次两端：推进率为总实际推进帧数 / 总观测墙钟；callback/tick 为扣除首行 anchor 后的 native callback 墙钟总和 / 实际推进帧数。两个最大值均为该行两端中的最大值。

| 批次 / 顺序 | 版本 | 逻辑 Hz | callback/tick ms | 单次 callback 最大 ms | 新逻辑状态间隔最大 ms | corrections / resim ticks |
| --- | --- | ---: | ---: | ---: | ---: | ---: |
| c2 / A1 | 基线 | 48.906 | 17.854 | 95.2 | 160.3 | 639 / 4023 |
| c2 / B1 | 候选 | 48.765 | 17.949 | 96.8 | 140.4 | 637 / 4016 |
| c2 / B2 | 候选 | 48.303 | 18.176 | 117.0 | 153.2 | 661 / 4124 |
| c2 / A2 | 基线 | 15.267 | 46.158 | 144.5 | 271.2 | 630 / 4908 |
| c3 / A1 | 基线 | 39.330 | 22.675 | 88.0 | 149.9 | 673 / 3877 |
| c3 / B1 | 候选 | 48.495 | 17.952 | 100.1 | 144.5 | 638 / 3954 |
| c3 / B2 | 候选 | 48.289 | 18.251 | 97.3 | 163.5 | 639 / 3911 |
| c3 / A2 | 基线 | 49.303 | 17.872 | 95.7 | 160.1 | 620 / 3902 |

八次全部通过：每次两端 canonical 相同，最终确认到 1199；正式测量行始终 RTC，有真实受损发送增量；触控带动角色；关闭了本次创建的两个浏览器实例、HTTP thread 和已知 relay 子进程。此处是八次运行、合计十六个端的记录，不是十六端同时联机，也不是全局进程不存在审计。

**解释与决策：**

- c2/A2 必须保留。P0/P1 的重演次数为 33/4875，P0 callback 均值约 45.67 ms，P1 约 6.70 ms。它通过了状态正确性检查，但明显进入了不同的性能与前沿关系。
- 各次浏览器版本、实际 GPU、CPU4 参数、slowSeats=[0,1]、WASM 和冻结输入已核对一致。源码仅对各 page 设置一次绝对 CPU rate，没有按轮次重复叠加的路径。启动带禁后台降级参数，但没有窗口遮挡/OS 前台/CPU self 的观测；异常原因未隔离，不归咎于特定环境因素。
- c3/A1 为约 39.33 Hz，c3/A2 回到约 49.30 Hz；四次候选约 48.29–48.77 Hz。候选在本轮较集中，但不能因此证明它消除了基线异常的根因。
- 不用包含 c2/A2 的合计宣称巨大提速，也不事后丢掉它来重算漂亮结论。全部描述性合计保存在 JSON。与基线正常的约 49 Hz 运行相比，候选并未显示稳定的整局速度提高。
- 四次候选合计 9,659 个正文 callback，其中 >50 ms 为 172 次，>100 ms 为 2 次，>250 ms 为 0 次；新逻辑状态间隔 >100 ms 仍有 63 次，最大 163.5 ms。因此“保持 60fps”“无大卡顿”“固定百分比改善”均未获本轮验证。
- 可以直接承诺的结构性收益仍是：满足条件的 checkpoint 少复制 113568 B，以及 Enemy 记录退休后复用可用容量。没有以关闭预测、加输入延迟或减弱 oracle 换成绩。

### 网络尖峰对照

完成的尖峰对照保存在 c2/spike-A、c2/spike-B，沿用同一冻结 A/B 文件和稳定测试条件。对 seat 1 从第 450 帧起，在 800 ms 窗口中额外施加 800 ms 应用发送延迟，repair/control input 同样受影响；A/B 实际分别延迟 45/44 次匹配发送，其中 7/5 次为 control input。两次均通过双端一致性、最终确认和清理。

| 指标，两端合并 | 基线 A | 候选 B |
| --- | ---: | ---: |
| 正文逻辑推进 Hz | 44.201 | 45.934 |
| callback/tick ms | 19.868 | 18.927 |
| 单次 callback 最大 ms | 94.3 | 101.5 |
| 单次 callback >100 / >250 ms | 0 / 0 | 1 / 0 |
| Present observer 间隔最大 ms | 97.1 | 105.9 |
| 新逻辑状态间隔最大 ms | 770.5 | 772.0 |

候选 P0 最长 772 ms 的逻辑间隔发生于 frame 458→459，期间仍有 83 个游戏 callback、83 次 Present 计数增长，期间单次 callback 最长 60.9 ms，未确认前沿最多 8f。P1 的 764 ms 间隔期间也有 121 个 callback。这支持将该段区分为预测余量耗尽后的缺输入等待，而不是一次占住主线程 772 ms 的函数调用；重复提交画面不代表产生了新逻辑状态。

恢复指标使用“额外延迟注入窗口结束”作为时间零点；A/B 最后一个受尖峰影响的发送分别在其后约 876/885 ms 才发出。候选两端 recoveryToPreSpikeMs 均为 1000，含义是首次满足连续两个 1 秒窗口达到尖峰前速率 95%、confirmed age <200 ms。它不是精确恢复时刻，也不代表后续所有窗口都保持此速度。两版 sustained/settled ≥57 Hz 指标均为 null，不能称恢复后稳定 60fps。

另保留 c3/spike-A 的前置检查失败：脚本在 warmup 之前报 Not RTC，没有尖峰测量数据。源码先等待 netStatus()[2]，随后立即检查 driverStatus()[12]，这次未同时满足；未据此诊断生产链路根因。没有删除失败或降低 RTC 资格要求，同一冻结输入随后在 c2 的两端对照中通过。

## 6. 被撤回的 Replay 缓冲复用

最初尝试了 World Record 固定环与 ReplayRecording 可复用深拷贝缓存，独立 WASI -O2 检查共 245 项通过，预热后 capture 分配次数为 0。但最后一轮时间数据并不支持保留：

| 场景 | 旧版两次耗时 ms | 复用版两次耗时 ms |
| --- | ---: | ---: |
| 普通 MP 的空记录形状，depth 1 | 17.0888 / 14.3360 | 19.2645 / 17.3966 |
| 空记录，depth 8 | 14.9867 / 14.6882 | 17.8204 / 18.3094 |
| 填充记录，depth 1 | 48.4289 / 52.1526 | 71.0686 / 60.6914 |
| 填充记录，depth 8 | 54.8183 / 56.3116 | 56.3286 / 59.4522 |

这属于 WASI 组件耗时，不是 Emscripten CPU4 游戏成绩。正常 MP 在 GameplayScene 中 recording_game=false，普通 ReplayRecording 不持续收集输入，MP 的录像来自另一所有者。生产默认诊断中 recording owner 总共约 11 ms，远小于其他所有者，因而该实验优先级也低。

该实现已从生产源码撤回，该项实验实现与组件测试保留在 rollback-journal-rejected-replay-retention.patch；c1/c2/c3/c4/c5 的原始组件记录仍保留在 artifacts/multiplayer-tests。最终产物不包含 ReplayCheckpoint.hpp，不改变原 replay 深拷贝行为。

## 7. 仍需后续单独处理的调度边界

TH08 当前 Correct 同步重演到原前沿，没有按 wall clock 分片。不能只加 while 的 8 ms break：
- 分片中 Pump 新收包可能产生针对已重演区间的更早 request；原 EndCorrection 无条件清 request，会丢掉它。
- Reconcile 的 true 是 confirmed Replay 可以输出的资格，pending 不能冒充完成。
- CanCapture=false 不足以阻止 GameHost 在 pointer/poll 中读取历史中间态，从而影响真实触控取消/移动/释放。
- 高刷尾部可独立 Draw，partial correction 不能只靠最终 Present suppression 遮住。
- 一个完整 undo 或单 tick 仍可能超过预算，分片只限制继续启动下一 tick，不是可抢占的硬上限。

本轮不修改这套输入/恢复/呈现协议；不承诺所有设备无掉帧、不承诺原始 rAF 等待已被量化消除。若继续做，应针对恢复中途收包、跨片输入与输出、60/高刷呈现建立独立验收，再实施 Driver + NetplayRuntime + GameHost 的小范围状态机改动。

## 8. 交付与复现

### 本地提交与正式产物

| 内容 | 提交 |
| --- | --- |
| 两项生产优化，6 个生产文件 | a99c70b8d02091f8d29ad2ab30c0b9236800441a |
| 原生 fixture、串行验收、冻结对照与实验归档 | 765a1fafe2c0baf6434225a73491b0948a5950bb |
| 正式测量使用原生输入采样及发送 | 52af4d28a8227f2ebf66a3e9c33e5fd070e872af |

正式候选：

- D:/workspace/eagler/worktrees/th08-rollback-journal-4x/th08_web/artifacts/multiplayer/build.json
- D:/workspace/eagler/worktrees/th08-rollback-journal-4x/th08_web/artifacts/multiplayer/th08-sdl.wasm
- SHA-256：8ea9b23fc9ffbdb25ddd1d5bf1a94284fef5e60df00390ec735557ff27e71f55。
- 大小 3,595,502 B；基线 3,595,984 B。482 B 文件大小差不等于运行内存收益。
- 正式版本在 765a1fa 上构建；52af4d2 仅改变测试 JS/Python，收尾核实正式 A/B 各 496 项构建源文件、诊断 505 项源文件全部与对应 build.json 匹配，WASM 和 loader SHA 均匹配。
- 工具链版本 6.0.9；正式/诊断构建均使用项目既定 -O2。正式版 diagnostic=false，无 native fixture/profiler 导出。

### 证据索引

仓库内可审查报告：

- docs/multiplayer/rollback-journal-work.md：本文。
- docs/multiplayer/rollback-journal-evidence.json：完整元数据、逐端统计和原始文件 SHA。
- docs/multiplayer/rollback-journal-rejected-replay-retention.patch：未保留的实验。

原始记录与冻结资产在 artifacts/rollback-journal-4x 下保存：

- candidate-owners-c4、candidate-world3-c1、candidate-bullets-c1：最终原生正确性。
- candidate-release-c1、candidate-fixtures-c3、baseline-release：产物快照。
- formal-journal-abba-c2：第一组四次对照及通过的 spike-A/B。
- formal-journal-abba-c3-recheck：追加四次对照及未进入测量的 spike-A。
- formal-journal-abba-c1：已定位并修复的第 0 帧测试输入接线失败。
- collect-evidence.cjs、collected-summary.json：本次原始数据重算与汇总。

游戏数据、字体和冻结二进制保留在已有本地资产目录，不随源码提交重复收录。

### 复现入口

在候选工作目录、已有工具链与游戏数据可用时，先构建所需 lane；正式性能对照要求独立 baseline 树也已构建相同 lane。每次 RUN_LABEL 使用新名字，runner 拒绝覆盖已有运行。

~~~powershell
$env:EMSDK = 'D:/workspace/eagler/th08-eagler/tools/emsdk'
$env:EM_CONFIG = 'D:/workspace/eagler/th08-eagler/tools/emsdk/.emscripten'
$env:EAGLER_COMMON_ROOT = 'D:/workspace/eagler/worktrees/th08-rollback-journal-4x/third_party/eagler-common'
$env:TH_BUILD_JOBS = '4'
$env:EMCC_CORES = '4'
$env:TH08_MP_DATA = 'D:/workspace/eagler/th08-eagler/artifacts/presentation-lab/input/th08.dat'
$env:TH_PYTHON = 'C:/Python314/python.exe'
$env:EAGLER_WORKSPACE = 'D:/workspace/eagler'

node portable/build.mjs --th08 --multiplayer-fixtures
if ($LASTEXITCODE -ne 0) { throw 'Fixture build failed' }
$env:RUN_LABEL = 'review-native-unique'
node portable/multiplayer/journal-verify.mjs enemy world2 world3 bullets
if ($LASTEXITCODE -ne 0) { throw 'Native checks failed' }

node portable/build.mjs --th08 --multiplayer
if ($LASTEXITCODE -ne 0) { throw 'Formal build failed' }
$env:BASELINE_ROOT = 'D:/workspace/eagler/worktrees/th08-rollback-journal-baseline'
$env:BASELINE_COMMIT = '05199cdb7402510814480a4d2ef6db429e49b03f'
$env:RUN_LABEL = 'review-abba-unique'
node portable/multiplayer/journal-performance.mjs freeze
if ($LASTEXITCODE -ne 0) { throw 'Freeze failed' }
foreach ($phase in @('A1', 'B1', 'B2', 'A2', 'spike-A', 'spike-B')) {
    node portable/multiplayer/journal-performance.mjs $phase
    if ($LASTEXITCODE -ne 0) { throw "Phase failed: $phase" }
}
~~~

下一轮性能工作的优先级应是量化并拆分长 callback 内的恢复、重演、Draw/同步浏览器工作，同时记录 rAF 登记等待及窗口状态。对最坏时延采用可中断恢复状态机之前，先满足第 7 节的中途收包、输入所有权和稳定呈现边界。当前数据没有支持把同步 Correct 简单加一个 8 ms break 就称为安全优化。

生产改动和诊断证据均保留在独立 git worktree。没有 push、merge 或部署动作；使用候选时必须以 build.json 与 WASM SHA 识别来源，不能从普通主树重新构建冒充该版本。

