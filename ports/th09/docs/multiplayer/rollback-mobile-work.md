# TH09 手机移动回滚：当前结果、边界与后续工作

日期：2026-10-11。状态：已完成本轮候选、深回滚正确性、桌面 CPU4 正式链路测试与 managed 队列复现；玩家要求的稳定无大卡目标仍未完成。

当前工作树：D:/workspace/eagler/worktrees/th09-mobile-rollback-4x，分支 experiment/mobile-rollback-4x，来自 b243bb1d24335b1fdab3606e27c6f67e4c636ad9。输入预测代码已作本地提交 a93fb87。common 只读固定为 D:/workspace/eagler/worktrees/th09-rollback-input-batch-common 的 512353cab9eaa4f6744f867b0eb07c61ee7f2b0e。所有环境操作均经 nig；构建、浏览器、测试和大证据计算由 root 串行执行。没有推送、合并或部署。

## 1. 结论先行

本轮得到一个窄而可靠的改进：mode 0 的完整回滚不再把预测的手机模拟移动强制清空。直接拖动的持有绝对目标、free joystick 的持有向量可以延续；晚到的真实输入仍做完整字段比较并触发完整恢复／重演。

相同实采输入的 900 帧三世界实验中，直接拖动的两端 corrections 和 resimulated 总量各下降约 5%。其中每端 60 个“手指停住但没有抬起”的输入帧，按对手实际输入阶段关联的 restore-start 次数由 60 降到 20，重演帧访问由 480 降到 202。持续移动的 216 帧主拖动阶段两版均有 216 次 restore-start，尚未得到同样收益。

这不能写成整体性能提升已经成立。相同输入的深回滚候选实测墙钟反而更差；正式中继 ABBA 的第一份基线约 34.4 Hz，另一次相同基线约 59.7 Hz，两个候选约 59.1／59.6 Hz。所有结果保留，不用第一份差基线宣传大幅加速，也不用一次慢运行判定 CPU4 不可能。

正式 120 Hz 触控刺激的网络突发测试和实际 RTC direct 均完成，但 CPU4 端仍有约 86 ms／53 ms 的新状态空档。候选中继另有约 117 ms 空档，期间没有新增 correction 或 resimulation。低输入延迟、稳定新状态和无明显停顿仍须同时验证。

此外，本轮已经把手机的另一条具体风险从源码怀疑推进到复现：真实 managed 消息监听器会把触控放到资源／文件命令的 Promise 队列之后。已 ready／launched 时，悬置资源读取可阻挡全部随后触控；blur／hidden 的取消之后，旧队列仍能继续调用 native。下一项独立修复优先处理这条实时输入路径。

## 2. 目标与没有改变的输入合同

玩家目标是预测回滚降低本地输入延迟，同时持续呈现新的游戏状态。网络信息不足可以有可控、平滑的降速；不应把大卡、完全停顿，或设备算不过来的持续慢速当成完成目标。

本轮固定 mode 0、D0、H8，保留完整世界快照、逆序恢复、历史重演与实际 FrameInput 的完整比较。每个新的逻辑输入只捕获一次；恢复／重演期间不重采当前手指。协议 v4 的 12 B 输入、Replay、真实按钮边沿、touchUsed、unlimited 和 touchBomb 的规则仍受原校正与验证约束。炸弹、菜单与 touchBomb 不做猜测，方向键仍保留三帧预测界限。

本轮的生产语义改动只有 RollbackSession.cpp：

- directTouchIsAbsolute 对两个有预测的 rollback 模式成立，Delay 模式保持其等待规则。
- Prepare 不再对非 Hybrid 的预测输入把 analogMode、坐标和触控标志清空；合法性校验仍保留。

TH09 direct 手势是相邻指针位移累加、逐事件限幅后的绝对场地目标。停指时目标继续有效。free joystick 是持有向量；digital joystick 主要是方向键。它们不能统称为同一种触控负载。

## 3. 验证覆盖与各自不能证明什么

| 验证层 | 已执行内容 | 结果与边界 |
| --- | --- | --- |
| C++ 输入契约 | session、adonis、mobile-prediction；held direct／unlimited／joystick、H8 边界、16 类晚到变化、乱序／重复／非法输入、一次捕获后恢复 | 通过。模型测试不单独等于完整游戏世界正确。 |
| JS 触控探针 | 3 项语法检查、8 项 VM 合同 | 通过。只验证刺激／分母／编码观测规则。 |
| 三世界真实游戏 | 两个 rollback world 加由实采 tape 驱动的第三世界；900 帧、延迟 8 个接收前沿、CPU4 | direct 基线、direct 候选、free joystick 候选均通过。完整 hash、48 项审计、framebuffer 和 Replay 校验通过。 |
| 正式 release 链路 | 两个独立 headed Chrome 进程，CPU [1,4]、native GPU；relay39±5 ms ABBA、120 Hz burst、RTC direct | 六次成功运行全部完成；每次保留实际输入和确认前缀。平均接近 60 Hz 不代表无大卡。 |
| managed 消息链 | 真实模块 source 的 message／blur／visibility listener，受控异步资源、native 调用记录与 ACK | 六项 characterization 通过，四项体验／生命周期目标为 false。没有把 stub 调用说成真实游戏移动。 |
| 真实 Android | 已只读确认现有连接设备 | 本轮尚未做真实 Launcher／设备验收；没有声称设备不可用。 |

正式浏览器为 Chrome 153.0.8010.12，native GPU 为本机 UHD／ANGLE D3D11。CPU4 是该桌面浏览器 renderer 的限速设置，不等于 Android CPU、GPU、热状态、触控事件分发或内存行为。

触控脚本包含持续拖动、反向、停指、抬起、重新接触、cancel，以及 fire／focus 控件。它只有一个实际移动指针，没有测试真实双指接触。正式运行的事件源由 Page performance.now／setTimeout 独立推进，60／120 Hz 不跟随游戏帧、capture 或收包前沿。迟到的 MOVE 仅按已声明的相同 gesture／leg 规则合并；UP、DOWN、cancel 和转向等边界保留。主界面与托管桥仍被 native ABI 刺激绕过。

## 4. 深回滚：相同输入下究竟减少了什么

| 900 帧三世界运行 | P1 corrections／resimulated | P2 corrections／resimulated | P1／P2 全部采样调用墙钟总和 |
| --- | ---: | ---: | ---: |
| direct 基线 B | 785／6273 | 816／6494 | 36.908／36.664 s |
| direct 候选 C | 746／5961 | 775／6174 | 60.376／60.679 s |
| free joystick 候选 | 751／5986 | 776／6173 | 45.004／45.289 s |

direct B／C 的两端 12 B 实采输入前缀相同，最终 Replay 字节相同，三世界最终 hash 均为 3347035085。候选减少了持有目标被清空造成的校正，但没有改变这份真实输入 tape。free joystick 是另一种实际输入，不能和 direct 当作相同工作负载比较。

按对手的实际输入帧关联阶段：

| 阶段 | direct B | direct C | 可以下的判断 |
| --- | ---: | ---: | --- |
| 每端 60 帧 hold 的 restore-start 次数 | 60 | 20 | 持有预测确实减少这一阶段的多余恢复。 |
| 同一 hold 阶段的重演输入帧访问 | 480 | 202 | 重复历史工作下降；访问数不是独立事件响应数。 |
| 每端 216 帧主持续拖动的 restore-start 次数 | 216 | 216 | 连续改变目标的问题仍在。 |
| 输入手势开始前的 restore-start 次数／端 | 132 | 132 | 对话跳过／菜单输入不可预测，另列，未算成移动收益。 |

hold 中 x／y、模式和 flags 没有变化，但仍有 18 个真实按钮变化边沿；自动射击按 capture 形成 3 开／3 关，不能把所有 hold 校正归入坐标变化。多个不符输入还可能合并成一次最早纠错，correction 不是 mismatch 的逐个计数。

所有 manual 调用都保留，包括启动、无推进和分片恢复。三组 P1 的单次最大值都在 frame 0 启动，不冒充正文大卡；正文仍有例如 direct B 约 158.675 ms、C 约 174.290 ms 的深恢复调用。候选的全部调用总和比基线差，不能隐去。

这些数值来自一个 CPU4 renderer 内顺序执行的三个世界；只有两个 rollback world 的单次 game_tick 被采样，第三世界 oracle、准备与检查还有额外工作。128.959／187.706／152.420 s 的整次耗时不能换算成玩家 FPS。

宿主记录也保留了不确定性：direct C 期间，一个不同于已登记 harness／采样器 PID 的 Node 进程，在约 50.983 s 的相邻快照区间累计增加 56 CPU 秒。它证明存在其他已观察活动，不证明是在编译，也不能逐调用解释候选变慢。全机平均忙碌度不能当作游戏主线程的可用算力，更不用于删除不利样本。

## 5. 正式 CPU4 中继与 direct 结果

正文采用冻结汇总器的 native PredictionFrontier 首次跨越 F180 到终点的连续区间。保留所有回调，包括回退游标、恢复和无推进；Hz 和累计量用闭合端点差，新状态空档也闭合到终点。统计的是新的模拟状态及 renderer submission 的 Page sampledAt 观察，不是物理屏幕扫描输出。

| 按执行顺序 | 触控源 | 链路 | P2 前沿 Hz | 新状态间隔 p99 | 最大新状态间隔 | >50 ms／>100 ms 次数 |
| --- | ---: | --- | ---: | ---: | ---: | ---: |
| relay39-b1，基线 | 60 Hz | 单向 39±5 ms relay | 34.422 | 82.805 ms | 175.270 ms | 176／10 |
| relay39-c1，候选 | 60 Hz | 相同声明条件 | 59.109 | 31.110 ms | 117.005 ms | 3／1 |
| relay39-c2，候选 | 60 Hz | 相同声明条件 | 59.552 | 27.945 ms | 67.325 ms | 2／0 |
| relay39-b2，基线 | 60 Hz | 相同声明条件 | 59.740 | 26.720 ms | 78.980 ms | 1／0 |
| burst120-c1，候选 | 120 Hz | relay 加一次突发延迟 | 59.209 | 27.845 ms | 86.270 ms | 4／0 |
| direct120-c2，候选 | 120 Hz | 实际 RTC direct，无注入 | 59.948 | 22.365 ms | 53.270 ms | 1／0 |

前五次终点为 F1800，每次比较 15 个共同已确认 hash 前缀；direct 为 F900，比较 7 个。运行停止时两端可能超出不同终点，因此没有把不可比的末尾 hash 当作同帧状态。每次的确认前缀和完整输入记录通过，不是把 finalComparable=false 隐去。

ABBA 四次声明参数与观察脚本身份一致，但独立墙钟源在不同实际调度下形成不同实采 tape。基线自身从 34.4 漂到 59.7 Hz，因此这不是匹配输入的 CPU 计算 A/B；无法从 B1→C1 计算可信整体加速比，B2 也没有表明候选稳定更快。

避免空回调掩盖成本后，P2 的 fresh-single-step 回调如下。分类要求相邻 next、capture、renderer 各推进 1，correction／resim 为 0，且 next 已到 native frontier。

| 运行 | 样本数 | 中位数 | p99 | 最大值 |
| --- | ---: | ---: | ---: | ---: |
| B1 | 1133 | 15.225 ms | 45.260 ms | 92.725 ms |
| C1 | 566 | 3.250 ms | 9.190 ms | 12.965 ms |
| C2 | 507 | 3.010 ms | 6.570 ms | 10.700 ms |
| B2 | 573 | 2.830 ms | 5.040 ms | 6.480 ms |
| burst120 | 695 | 2.830 ms | 5.675 ms | 6.670 ms |
| direct120 | 718 | 1.885 ms | 5.380 ms | 7.900 ms |

这里仍是整个 C++ rAF 回调的墙钟时间，包含同步发送、WebGL／音频桥、绘制等，也可能包含进入后的抢占；不是纯模拟 CPU 时间。其余恢复、无推进和其他类别在机器证据中互斥守恒。B1 普通 fresh 回调本身已经很重，不能把全部慢速都归于 correction 次数。

最差空档的几个关键事实：

- C1 P2 的 117.005 ms 间隔，F787→789，只有一个记录的回调，回调约 12.245 ms，correction／resim 增量都为 0。
- B2 P2 的 78.980 ms 间隔，F1275→1278，两个回调合计约 11.570 ms，correction／resim 都为 0。
- burst P2 的 86.270 ms 间隔，F848→849，两个回调约 21.075 ms，增加 2 次 correction／7 帧 resim。
- direct P2 的 53.270 ms 间隔，F233→234，两个回调约 10.190 ms，增加 1 次 correction／1 帧 resim。

这些事实要求继续调查回调之外的调度与队列；不能直接把“间隔减回调 ms”叫作 OS、浏览器、relay 或网络的纯耗时。跨页／relay 的有效时钟括号不足时，汇总仅保留对应输入帧的上下文，没有声称两端最大间隔同时发生。

随后已对六次运行、两端的最大 sampledAt 新状态间隙，提取同页已保留且关联到 onGameFrame 的 rAF 请求及其父回调。全部 12 项通过来源、hook、样本关联、未取消／异常及时间顺序检查；资格通过不表示页面所有 rAF 或内部事件均被覆盖。等待起点为 max(requestedAt, parent.exitedAt)，终点为 enteredAt；裁剪到各自间隙后合并重叠区间。

| P2 运行 | 最大新状态空档 | 已登记且父已返回的 rAF 等待并集 | 对应整回调报告耗时之和 |
| --- | ---: | ---: | ---: |
| B1 | 175.270 ms | 108.665 ms | 60.015 ms |
| C1 | 117.005 ms | 104.720 ms | 12.245 ms |
| C2 | 67.325 ms | 55.800 ms | 11.335 ms |
| B2 | 78.980 ms | 67.165 ms | 11.570 ms |
| burst120 | 86.270 ms | 65.115 ms | 21.075 ms |
| direct120 | 53.270 ms | 42.145 ms | 10.190 ms |

这是既有请求已登记、父回调已返回却尚未进入的观察；不是承诺该时刻浏览器必须派发，也不能从中细分其他 JS、GC、OS 抢占、GPU 或网络任务的贡献。等待与回调耗时是独立记录，没有用相减残差冒充调度测量。最大空档包含显著的这类等待，后续应同时检查调度与输入队列。本次证据不能量化仅加快历史重演对这些等待的直接或间接影响。

## 6. 中继、直连与手机输入响应的边界

relay 是独立的本机进程，使用已冻结的现代 relay 实现；不是跨机器 WAN／运营商条件。burst 在 relay 首次非空输入收到后 20 s 开始，持续 500 ms，对符合范围的包额外增加 120 ms；实际记录到 60 个 burstScheduled。五次 relay trace 均有效，未见预算失败、同步／异步发送异常。末尾已计划、发送、取消和仍待处理的数量保留；这不证明任意网络、任何队列年龄或 native Apply 接纳都正常。

direct 的 routeEvidence 在两端均报告 selectedPath=direct：选中的候选为 host／host、UDP、nominated、succeeded；input 通道 open、unordered、maxRetransmits=0，control 为 open、ordered。检查时 bufferedAmount=0。它是实际 RTC direct 证据，但不覆盖 WAN NAT 穿透、TURN、弱移动网络或真实手机。

firstWireAt 是同步 send-intent 观察，在实际 send 与注入逻辑之前；不是精确 capture、发送完成、收到或 native 接纳。响应统计也只表示“满足条件的 wire 关联之后，看到了渲染推进”，没有证明具体手势造成了该位置变化，更不是手指到光子的时延。

| P2 运行 | 到期事件 | 实际提交 | 合并 MOVE | 未提交 | 可关联到渲染推进的事件 | 条件计划→渲染 p95 |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| B1 | 2410 | 915 | 1493 | 2 | 530 | 65.450 ms |
| C1 | 1368 | 1336 | 32 | 0 | 975 | 27.443 ms |
| C2 | 1356 | 1338 | 17 | 1 | 894 | 26.833 ms |
| B2 | 1349 | 1331 | 17 | 1 | 933 | 26.077 ms |
| burst120 | 2642 | 2178 | 463 | 1 | 1128 | 21.243 ms |
| direct120 | 1214 | 1162 | 51 | 1 | 590 | 16.245 ms |

这些分母覆盖每次运行到停止的事件，而非只覆盖正式 F180 起的正文。60 Hz 总计划 5638 个事件，120 Hz 总计划 10913 个；未来未到期事件另存，不拿它们充成功率。停止时未提交、无法关联和真实 wire 历史均保留。120 Hz 源高于 60 Hz 逻辑采样，本来就不能要求每个 MOVE 成为独立 FrameInput 或独立呈现。

## 7. 已执行的真实 managed 队列复现

当前普通 Android Launcher 的源码主路径包含宿主覆盖层：host PointerEvent → TouchInputController → RuntimeService.postInput → 既有投递 helper → TH09 managed → native。iOS 使用宿主 TouchEvent；free joystick 还有 host rAF 控件快照。不能根据 runtime 中旧的“Android 直接触及 child”注释，把 Android 都视作绕过 managed。

新测试读取真实 managed.mjs，保留真实 message、blur、visibilitychange 监听器，仅替换外部模块／native／资源等依赖以控制等待。configure、launch、两次 rAF 和启动 timer 都经真实命令路径执行；测试不直接赋值 launched、不调用内部 command 来代替消息入口。其合成时钟不测真实毫秒。

| 情景 | 已观察结果 | 修复目标 |
| --- | --- | --- |
| 合法 active 输入排在悬置资源之后 | 5 个触控消息未 ACK，native 输入调用 0；键盘仍有 3 个调用；资源释放后触控产生 7 个 native 调用，ACK 全清 | 实时合法触控不等无关资源。 |
| queued DOWN／MOVE 后 blur | 真实 cancel 已调用；释放资源后仍有 2 个旧 direct native 调用 | 旧输入不越过取消边界。 |
| queued 非零摇杆后 hidden | 真实 cancel／pause 后仍出现 controls、stick 两次 native 调用，document.hidden 为 true | 隐藏状态不再执行旧的活动摇杆输入。 |
| 对抗性 pre-ready 合法 envelope | createModule 未完成时无 native 输入；初始化后、launch 前出现 3 个输入调用 | 拒绝未具备输入资格时收到的旧事件。 |
| 外来 envelope／无效 epoch | 7 类外来或无效 envelope 与无效 Runtime epoch 均被忽略，合法正例到达 | 保留来源／会话门禁。 |

运行 mobile-managed-queue-characterization-b1：6 项测试通过，来源稳定；4 项修复目标明确为 false。PASS 表示复现与观察合同通过，不是“队列问题已经修好”。测试没有模拟真实手势的 dragging 恢复，不能把旧 native 调用直接说成已经测得实际角色重新移动。

下一项候选应只处理现有同步触控命令，保留资源／配置／启动顺序及初始化、开局连接、停止、隐藏、覆盖态、spectator 和输入 owner 边界。touch-cancel 不能快速执行后又让更早排队的 DOWN 重新到达；普通 sensitivity／controls 快照也不能无条件取消正在拖动。每次 MOVE 的原有限幅顺序和动作 serial 要保留。没有安装新协议或新直达桥。

## 8. 失败、纠正与构建身份

没有把失败样本覆盖成成功：

1. 首次 build source identity 门禁失败，原因是新 checkout 的 PerformanceDiagnostics.hpp 为 CRLF，旧基线为 LF；内容归一化后完全相同。原始文件和失败证据保留，随后恢复旧基线的精确 7003 B 并重新构建 release／diagnostic。最终 275 个 native 输入中 274 个逐字节相同，仅 RollbackSession.cpp 不同。两个候选版本的 WASM 本身与初次构建相同，但没有用这一点跳过严格来源控制。
2. direct120-c1 在浏览器启动前被 harness 拦截：误沿用了 relay 专属 RELAY_TRACE=1。保留 invocation／450 B process log；只改为 RELAY_TRACE=0，新编号 direct120-c2 完成。它不是游戏运行失败或 CPU4 性能样本。
3. 已执行的 deep 汇总 v1 把 stats[5] 误名为 nativeWaitCounter。实际 ABI 是 Channel RepairsSent。保留 v1 源码与原证据，生成 v2 的 corrected evidence；去除语义重命名和方法说明差异后，其余所有数量／数据深比较完全相同。
4. 同时澄清恢复距离来自单次 game_tick 的源码公式，不是独立 native restore hook；在已经分片恢复时，不等于整个 prediction frontier 的全部待重演深度。returnCode=2 还包括未 ready、Prepare 阻塞、资源退出屏障和部分恢复，不能自动叫网络等待。

| 构建 | WASM SHA256 |
| --- | --- |
| 冻结基线 release | 766dc277b15d86a97d4bc3b38e2a08a9b52a5786d74c817763bc185216673577 |
| 匹配来源候选 release | 037c9cd52e70e2c8e8ee516cf15b6aee7f0de5951f88d519b2311cc1c5def373 |
| 冻结基线 diagnostic | 59364fa88a9a23913fe2d808ae6e303790dcb1c1533f5e4d4e5893d14fde4877 |
| 匹配来源候选 diagnostic | f0fe104e03cf4c10617c16c8c08897784a83b02d5e8eeeabf8c2ee1e7e9e58cb |

audio SIMD 仍关闭。旧 enemy first-write index 和上一轮 common 改动保留，旧树 D:/workspace/eagler/worktrees/th09-rollback-4x 的代码、报告与产物不改。当前新树的同名历史文档是继承内容；本报告与新交接优先描述本轮结果。

## 9. 接下来按证据推进

第一优先是已复现的 managed 输入队列问题，独立实现并用目标门槛重新验证；将 native ABI 性能测试与真实 Launcher／设备链路分开。现有 Android 设备可作为实际验收入口，需记录实际浏览器、orientation、输入 owner、代码／资源 generation 与本地调试路由。

持续 MOVE 的回滚量仍要继续降低，但不能靠丢弃真实变化、只回滚玩家状态或给完整比较加 epsilon。一个较小的研究入口是对已实采输入做“保持目标”和“目标速度外推”的离线精确预测命中比较：只使用当时已到的实际输入，float 运算顺序对应候选实现，分别保留持续移动、转向、停指和释放的得失。没有真实接收／预测轨迹时，它只是准确性 shadow，不是已经测得的 correction 或性能反事实。

free joystick 停指段只发现一个微小数值变化实例，现有位置加减链值得做 float32 fixture；它还不足以证明主要负载来源。不能为消除浮点变化而承诺旧新 actual tape 必然相同。

TH06MP／TH07MP 继续作为高性能参考。Begin／Delta 有成熟的可回滚余量合同可借鉴，但默认零 delta 预测仍会对持续非零 MOVE 产生不符；TH09 的目标起点、逐事件边界、focus／scale／rate、Replay 和 spectator 编码也不同。先做独立运动语义证明，不能直接把该接入当成免费优化。既有 information-pacing shadow 的误报结果继续保留且离线，不自动上线调速。

## 10. 证据入口与可追溯路径

以下文件在当前工作树，执行 raw／invocation／process.log 在 th09_web/artifacts/multiplayer-tests，均按唯一编号保留。

| 入口 | 用途 |
| --- | --- |
| [rollback-mobile-validation-plan.json](rollback-mobile-validation-plan.json) 与 [addendum](rollback-mobile-validation-addendum.json) | 浏览器运行前声明的阶段、顺序和参数。 |
| [rollback-mobile-build-matched-evidence.json](rollback-mobile-build-matched-evidence.json) | 最终 275 输入的匹配构建身份。 |
| [rollback-mobile-components-evidence.json](rollback-mobile-components-evidence.json) 与 [JS evidence](rollback-mobile-js-evidence.json) | 本轮实际组件与探针门禁。 |
| [rollback-mobile-deep-corrected-evidence.json](rollback-mobile-deep-corrected-evidence.json) | 三个完整 raw 的 v2 汇总；相同 tape、阶段关联、成本与正确性证据。 |
| [rollback-mobile-release-evidence.json](rollback-mobile-release-evidence.json) | 六次 release、闭合连续窗口、所有成本类别、触控分母、链路与宿主上下文。 |
| [rollback-mobile-gap-dispatch-evidence.json](rollback-mobile-gap-dispatch-evidence.json) | 全部六次／两端最大新状态空档的既有 rAF 请求、父回调及等待并集。 |
| [rollback-mobile-managed-queue-evidence.json](rollback-mobile-managed-queue-evidence.json) | 真实监听器复现、native 调用、ACK 与四项未满足目标。 |
| [rollback-mobile-input-path-audit.md](rollback-mobile-input-path-audit.md) | 当前 Launcher、managed、传输、native 与手机验收入口的源码审计。 |
| [rollback-mobile-timing-boundaries.md](rollback-mobile-timing-boundaries.md) | rAF／C++／JS／wire 时钟范围及 auto-fire／移动负载差异。 |
| [rollback-mobile-analysis-corrections.json](rollback-mobile-analysis-corrections.json)、[run corrections](rollback-mobile-run-corrections.json)、[line ending control](rollback-mobile-line-ending-control.json) | 原失败／误标与更正的可追溯记录。 |
| [rollback-mobile-execution-index.json](rollback-mobile-execution-index.json) 与 [final verification](rollback-mobile-final-verification.json) | 22 份串行调用、独立 JS 门禁、保留失败、原始日志与九份游戏 raw 的身份核验；另核对四个构建的当前 native 来源及旧树保护项。 |

深回滚 raw 编号：mobile-rollback-touch-deep-b1、mobile-rollback-touch-deep-c1、mobile-rollback-stick-deep-c1。正式链路 raw 编号见第 5 节，均加 mobile-rollback- 前缀；direct 成功为 c2，c1 仅是 preflight。实际游戏运行时的 Git HEAD 尚为 b243bb1，候选工作区改动由构建 manifest 与执行前后文件 SHA 固定；不能只看 HEAD 就把候选误称为未改基线。

