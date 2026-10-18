# TH09 managed 触控接收与持续移动：阶段情况报告

日期：2026-10-11。最新快照已包含 12 个正式正文合格的桌面双端样本，以及 GPU 三组的双时钟、输入分母和联合时间线。下方各分节保留完整历史、失败样本、勘误与审计索引。

## 1. 当前快照与玩家目标

**玩家目标仍未完成：保留 D0 预测回滚降低输入延迟，在双方 CPU4、持续变化触控下避免突然的大卡顿或完全停顿；网络信息不足时允许平滑降速。** 平均接近 60 Hz、确认状态一致或测量资格通过，都不能代替这一目标。当前只做桌面人工触控，分别验证独立 relay 与实际 RTC direct；不进行手机测试或设备操作。Android 只有历史连接/自有空白页诊断，没有实际游戏测试，未执行准备和结果未知的纠正写入不算验证。

**入口修复已经落实到代码与对照证据。** managed 同步触控入口、完整 ready/launch、document owner 和生命周期门禁已有原生 9 组/520 断言及真实 listener 57/57 目标支持，旧版本同门有 40/57 目标失败；AppShell 开关修复有 c12/c13 对照及本地提交 c84cc3941c6637fcdf975667db9cf52ad307a43d。Launcher c20 单机输入功能及三段运动资格通过，但 HUD 的严格整体资格仍为 false，不能外推多人性能。范围见第 2–5 节及[组件证据](rollback-managed-touch-component-evidence.json)。

**恢复补尾 B、提前制动 C 和 GPU 单开关均不作为修复采用。** B 正确性通过但收益未成立；C 保留约 31 Hz 的严重不利样本；GPU 三组是 A 坏→B 好→A 也好，参数可逆关联未成立。不能因此排除 GPU/呈现链路，也不能把坏样本一概归于编译或 CPU。没有 push、merge 或部署。

### 1.1 最新 12 个合格正文样本

下表来自[九组比较](rollback-desktop-continuous-touch-comparison-c1.json)和[GPU 三组比较](rollback-desktop-gpu-vsync-comparison-c1.json)。每组正式 frontier 窗口为 180→1200，双端 CPU4、D0/H8、独立 Page 时钟持续人工触控；这 12 组使用 native ABI 刺激，不证明真实 Launcher 多人整条输入路径或物理触控；各组内部的 10 个确认 hash 至 1200 相同。它们不是跨组相同实际输入的严格性能 A/B。Hz 四舍五入至两位，间隔均为 **rAF 时钟**；B/C 指 C++ 候选，GPU B1 仅指浏览器 flag=1。

| 样本 | 新状态 Hz（P1 / P2） | rAF 新状态 max（ms，P1 / P2） | 当前解释 |
|---|---:|---:|---|
| 原版 A，direct graphics c2 | 60.01 / 59.98 | 24.150 / 24.070 | P1 几乎无回滚；drain 晚尾不 clean |
| 原版 A，普通 relay39 c1 | 58.53 / 58.58 | 38.160 / 31.025 | 双方有回滚负载的有利样本 |
| 原版 A，relay39 burst160 c1 | 56.16 / 56.22 | 60.505 / 90.335 | 仍有明显停顿 |
| 补尾 B，普通 relay39 c1 | 59.95 / 59.87 | 25.095 / 30.295 | 正确性通过，不能独立证明收益 |
| 补尾 B，relay39 burst160 c1 | 56.36 / 56.43 | 66.525 / 96.750 | P2 Page 同端点达 100.335 ms |
| 原版 A，burst160 回切 c2 | 57.63 / 57.67 | 66.530 / 66.560 | 原版回切也改善，不能归功 B |
| 制动 C，普通 relay39 c1 | 54.42 / 54.37 | 48.695 / 42.420 | 平均推进下降，未建立单因果 |
| 制动 C，relay39 burst160 c1 | 30.93 / 30.90 | 181.775 / 224.050 | 严重不达标，完整保留 |
| 原版 A，普通 relay39 回切 c2 | 55.25 / 55.15 | 424.230 / 478.705 | 单端供给断档传播对端 |
| GPU A1，flag=0 | 33.98 / 34.15 | 254.510 / 290.525 | 正式资格通过，性能不达标 |
| GPU B1，flag=1 | 56.55 / 56.53 | 45.060 / 45.660 | rAF 无 >50 ms，P1 Page max=53.795 ms |
| GPU A2，回切 flag=0 | 58.03 / 57.93 | 42.425 / 34.440 | 回切也好，开关不作为修复采用 |

“合格正文”不表示性能达标或收尾干净。direct graphics c2 与 GPU 三组都保留 teardownClean=false；略高于 60 的窗口 Hz 是端点采样口径。rAF 和 Page 最大值可能来自不同端点，Page 是 harness 观察时间而非物理呈现。旧 UI/连接失败、截断运行、缺资格或 trace 不完整样本仍在第 4–5 节；这 12 组只是上述两个比较 artifact 的范围，没有删除或免责其他坏结果。

### 1.2 已经分开的两类关键证据

**一端约半秒未产出新输入，会把另一端拖入信息不足。** 原版普通 relay 回切中 P2 有 477.630 ms 合格已登记 rAF 等待，frame 与 relay 记录支持其新输入供给断档；中继已收到的旧包正常发送，P1 继续运行后用尽预测余量。P2 同期 57 个到期人工事件中 56 个合并、1 个后交，低成功条件代理不能代表这段输入及时。跨进程时钟只近似对齐，不能据此定 GPU、编译或所有网络问题的根因。见[传播证据](rollback-desktop-continuous-touch-both4-relay39-c2-peer-propagation.json)。

**另有不含纠正/重演的普通前进 callback 本身很慢。** GPU A1 的 ordinary Save1 单 callback 最大 wall 为 60.395 / 57.000 ms；最大 Page gap 内 B≤2/3、network scale=1，不能全解释为 H8 耗尽或主动网络减速。选中窗口没有同量级已命名 GPU 完整 X 跨度证据，但未覆盖事件仍未知；callback wall 也不是纯 CPU。诊断开销未校准，既不能把 A1 自动当成无探针的生产正常成本，也不能删除该坏样本。见[联合 decision](rollback-desktop-gpu-vsync-decision-c1.json)。

**当前下一步已确定但尚未实现：默认关闭的 release 粗阶段观测。** 既有详细 PerformanceDiagnostics 只在 DEVELOPMENT_HARNESS 中计时，不能从 A1 release raw 事后拆出 60/57 ms；拟保持 DEVELOPMENT_HARNESS=0、原循环与输入/呈现时序，固定数组按 callback 发布短记录，先在正式窗口前校准，再用同一诊断 WASM 做 runtime off→on→off。GPU 三组及联合定位均已结束，不再写测量或时间线在途；先确定普通 callback 墙钟阶段，避免把本地产出中断伪装成网络减速。具体插入点、资格和校准要求见第 8.3 节，VM 最老 preimage shadow 与条件制动保留在后。

以下第 2–9 节分别保留实现边界、真实验证、完整运行历史、未采用候选及文件/命令索引；当时的失败和限制不因最新快照而撤销。

## 2. 已实现：接收端修复及其边界

managed 工作树的生产改动范围是 [th09_web/sdl-runtime/managed.mjs](../../th09_web/sdl-runtime/managed.mjs)。本阶段继续使用既有 release WASM；没有为了接收器修复改动 C++ 游戏、输入采样或回滚算法。另有一处独立 Launcher 工作树的 AppShellProvider 入口候选，见本节追加说明，二者不混为一个性能结论。

| 改动 | 现在的行为 | 仍需注意的边界 |
| --- | --- | --- |
| 同步触控入口脱离异步资源队列 | 已收到的 direct-touch、touch-controls、touch-cancel 直接经过真实接收器资格检查和原生调用；不等不相关的 resources 完成 | 浏览器尚未派发 message task 时，接收器无法抢占主线程 |
| 完整 ready 与 launch settled | module、DATA、字体初始化完整成功后才 ready；launch 连同异步 connect 完成后才接纳活动触控 | core 已赋值不等于 ready，提前置位的 launched 不等于连接已完成 |
| 当前 document 所有权 | 验证原 iframe 仍连接、contentWindow 与 contentDocument 仍指向安装时的 window/document | 相同 WindowProxy 和 epoch 不能单独证明还是同一个 document |
| stop 与旧异步续体 | 真实 onClose 在安排异步 stop 前撤销输入资格；stop、fatal、pagehide 使旧 lifetime 退休；旧 launch/configure 请求和迟返 connect 不能恢复活动输入 | 产品没有 stop message 命令；测试没有偷偷增加这个协议 |
| 触控策略 configure 屏障 | enabled、movement mode、双指/双击策略或 spectator 资格真正变化时，先中和并建立 applying 屏障；只有当前 ticket 的完整 apply 成功才恢复 | apply 失败继续保留屏障，不能把失败后的状态写成“已恢复”；普通 controls 和仅 sensitivity 变化不打断拖动 |
| action serial 中和与恢复 | 不合格输入仍推进 source serial 基线；只把合格的新 edge 映射成 native serial 变化；恢复后重复旧值不会迟发动作 | 中和沿用已有 mapped serial，不制造 bomb/escape pulse；普通 cancel 与 reset 的已有 pulse 语义不同 |
| 初始化取消与 ACK | await initialized 位于逐请求 dispatch 的 try/catch 内，取消后的合法排队请求各自收到一次失败 ACK；初始化 promise 有独立 rejection observer | 无请求的 pre-init pagehide 也不能遗留 unhandled rejection |

原来的 source、origin、protocol、game、epoch 消息门禁仍然生效。候选没有新增直通桥、替换 Runtime provider、合并 MOVE、只留最后一点，也没有在输入 handler 中主动 tick、draw 或 pump。

document.hasFocus 没有成为硬门。真实父层触控 overlay 或 HUD 合法持有焦点时，当前 child 仍需要接纳父层触控。普通 focus、fire、sensitivity 控件也不能被错误解释成“运动 owner 改变”，否则会打断正在进行的拖动。

旧 document 或失效生命周期在第一次识别到失效时可以中和自己的旧 core；这类 cleanup native 调用与活动 DOWN/MOVE 分开计数，不能以“有 native 调用”冒充旧输入通过了门禁。隐藏期间的 action serial 只更新源基线，恢复后重复 controls 不会补发隐藏时的动作。

接收器的本地 lifetime 还有一个明确能力边界：父层已经 post、但子页面尚未收到的同 epoch 旧消息，没有发送端代际字段可供辨认。本轮没有宣称解决这种已在浏览器任务队列中的跨生命周期消息。

### 2.1 追加：AppShell 活动通知的独立入口候选

c9 的写入 trace 之后，只在 D:/workspace/eagler/worktrees/launcher-th09-input-activity-4x 新树修改 app/components/AppShellProvider.tsx；起点 HEAD 为 fed02bf5fc6027615c6253205f8afeb196c29323。canonical Launcher 源码保持只读，本段不是对既有冻结 UI bundle 的就地替换。

实际源码链是 document capture click/change/cancel → queueMicrotask(activityChanged) → 无条件新建 AppShell snapshot 并同步通知 useSyncExternalStore。候选只把 DOM / MutationObserver 的 changed 通知改成单个 pending setTimeout(0)：有 pending 时不清除重设，回调验证当前 active/epoch；cleanup 先撤销 active 并清 timer；pagehide 清同一个 timer。

file picker 的 ref 仍在 capture 中同步修改，window activation fence、服务快照对应的 useLayoutEffect 和既有 disposal 微任务保持原样。SW 在 activation microtask 与实际 reload task 都会重新读取实时 shouldDefer；没有把整个 publish 改异步，也没有更改 checkbox、storage 或 Runtime 输入。

候选文件全文回读一致：88 行、8,435 B，原有 CRLF 保留，SHA-256 c22af28f30ad6fadf837dc2971be24f9e3ec6eacb31c4fb41260f6a4ff788932。它针对的是事件派发期间 React 写回旧受控状态的顺序；setTimeout(0) 不是零毫秒性能承诺。c12/c13 已提供开关路径的真实浏览器对照；select、file picker、SW 的完整 DOM 回归不能据此宣称通过。真实 hosted 启动另由 c15 取得独立证据，c16 又通过了触控 driver 功能门禁；两者均不能替代完整观测资格与流畅性验收。

该 Launcher 修复已本地提交为 c84cc3941c6637fcdf975667db9cf52ad307a43d，仅包含 app/components/AppShellProvider.tsx 的 +14/−2；文件 SHA 仍为上述 c22af28f30ad6fadf837dc2971be24f9e3ec6eacb31c4fb41260f6a4ff788932，与受控候选一致。没有 push、merge 或 deploy；本地提交不增加尚未执行的验收结论。


## 3. 已证明：三层证据，三种不同结论

### 3.1 原生 TouchController 组件

实际 C++ TouchController 组件完成 **9 组、520 个断言，PASS**；既有 deathbomb 验证也通过。生产 TouchController 源码在本轮未改动。[组件汇总](rollback-managed-touch-component-evidence.json)明确将这层范围限定在实际原生控制器。

覆盖包括 cancel 保留已接受的剩余 pulse、reset 清 pulse 但保留 private serial 基线、重复 neutral controls 不重复动作、新的或跳变的 serial 只按既有合同产生动作、孤立 MOVE 不能复活已取消拖动、新 DOWN 可以重新开始，以及自由摇杆的速度/focus 选择。

这些测试补充了真实 native sample/reset/serial 语义；它们没有经过 React Launcher、网络、世界 Save/Restore，也没有测物理手机响应或性能。

### 3.2 同一真实源码 listener fixture：旧版本与候选

| 运行 | 测试模式 | 用例结果 | 目标结果 | 可以得出的结论 |
| --- | --- | --- | --- | --- |
| managed-touch-queue-b1 | 显式 baseline characterization，加载旧 managed 源码 | 22/22 用例通过 | 57 项中 40 项失败、17 项满足 | fixture 成功记录了旧行为及反证，旧接收器不满足目标 |
| managed-touch-queue-c1 | 候选 goals 门禁 | 22/22 用例通过 | 57/57 项满足 | 候选满足本 fixture 的接收器顺序、资格、ACK 和生命周期目标 |

“旧版本 characterization PASS”表示测试正确观察到了旧问题，不表示旧实现已合格。两个运行复用同一 fixture，并保留实际加载源码路径和 SHA，不能只摘录两个 PASS 字样。

fixture 使用真实 managed message/lifecycle listener、真实初始化/launch/stop/configure 路径，资源 fetch 和 connect 的待定状态由受控依赖建立；测试不直接调用内部 command，也不写闭包状态。它覆盖资源阻塞时活动输入、完整 init 但未 launch、launched 但 connect 未完成、真实 onClose 停止、旧续体、同 WindowProxy 的 document 更换、hidden/blur、serial 隐藏恢复、策略配置失败屏障、正常控件下保留每个 MOVE，以及请求 ACK 和无请求取消的 rejection 处理。

native 调用和依赖在 VM 中被记录/控制，时钟是测试时钟。因此，这层能证明“调用何时允许、以什么顺序出现、请求是否完成”，不能证明玩家移动了多少、实际绘制了什么、线路延迟或真实手机毫秒性能。[fixture 源码](../../th09_web/tests/server/managed-touch-queue.test.mjs)与[组件提取日志](../../th09_web/artifacts/multiplayer-tests/managed-touch-component-summary-c1-process.log)可复核两种模式。

### 3.3 Runtime materialization：同一 WASM，不代替 hosted 验收

normal/multiplayer directory Runtime 已按冻结 release WASM 和候选 shell 物化，身份记录保存在 [runtime materialization](rollback-managed-touch-runtime-materialization.json)。本阶段 WASM SHA 为：

`037c9cd52e70e2c8e8ee516cf15b6aee7f0de5951f88d519b2311cc1c5def373`

它与上一阶段移动输入候选 release 一致。这使此前的引擎身份与深回滚正确性证据仍有对应关系，但不能代替新的 hosted 输入集成验证。

真实 Launcher 尝试采用产品已有、经过验证的 web-development Runtime 路线，使用既有 UI 构建产物、原始 host factory、真实 DATA/字体和 Package Store 所有权。没有把 canonical 工作区重新构建成一个未经说明的新 UI，也没有伪造不可变发布。**不可变 publication/offline generation 的验收仍单列待做。** 这些取舍在首次集成前写入了 [hosted integration addendum](rollback-managed-touch-hosted-integration-addendum.json)。

### 3.4 正式 build、开关对照与启动已有证据，c16 功能与严格资格分列

新 Launcher 工作树的候选已执行独立正式 build，命令 781hOzjgXU2BWXA1r6OegrcY 为 PASS；UI artifact identity 为 1d607295335cf04a3877b560a81608bb62b2956e676579e3feb56d73b4b22960。构建没有 source drift，旧 canonical UI artifact 保持稳定。

这个 build PASS 本身只证明正式产物可核对。随后新树的 actual ensureLauncherBuild 也通过：命令 pl2301Jhh53BN2SSB86HF61H，62 项 authored source，编译耗时 574.1932 ms，tracked source 无漂移，旧 UI metadata 保持不变。c10/c11 均在服务器装配检查处失败，尚未开始浏览器初始化。

为了建立入口因果对照，又在相同 HEAD 下恢复原 AppShellProvider 源字节 1542dfdf35a01fc8046477d8d4758f2d16988f8317a678f2b71afd4a92716942，正式构建 fresh baseline UI：命令 _vop3M47ouLM4b9ABqPictMs，7,785.2611 ms，106 个文件，artifact identity b8287322182c4d4032740dc572e5438fa872179b87511d3063d3e1ba6279a0ef。finally 已恢复候选 c22af28f30ad6fadf837dc2971be24f9e3ec6eacb31c4fb41260f6a4ff788932；source 与旧 canonical artifact 保持稳定。这些毫秒是构建/编译耗时，不是游戏性能。

c12 使用候选 UI，c13 使用该 fresh baseline UI；driver 和 server 相同。候选真实开启并持久化 true，基线重新出现旧值写回。此对照支持 AppShell 活动通知修复的入口因果作用；两份 driver 整体仍 exit1，不能据此宣称完整 Launcher 启动、回滚 FPS 或手机性能通过。

既有 tests/ui-main/app-shell-client.test.mjs 也执行 PASS：命令 Qh2NEeL4sjKOQXzaC4lwG1nS，wrapper 耗时 200.0416 ms，脚本 SHA-256 62ce5d1efc25fcf917b741747f272fddb9feb350370491e5849852b3bcff994d，sourceStable=true。它验证 service/client 层的 SW 活动资格与二次检查等，未直接导入 ReactProvider；因此不能扩称实际 DOM 传播、合并 timer 或所有 picker 行为已覆盖。完整日志已确认 15 pass、0 fail、0 skip、0 cancel，test duration=152.6442 ms；它与 wrapper 的 200.0416 ms 分列，均不是游戏性能。


## 4. 真实 Launcher 功能已有进展，历史入口诊断与桌面结果分别保留

以下保留了所有实际尝试。c12 完成真实开关开启和持久化，c15 完成真实 Launcher 启动但后续是 Replay；c16 改经真实触控菜单后，driver 功能目标全部通过，严格全局资格仍未通过。c17 的摇杆输入在真实 gameplay 中触发手势派发上限，保留已完成与未完成的分母。启动前、设置或连接失败不能计入“游戏卡顿次数”；已经采到的 c16/c17 真实长间隔则完整保留，不能随入口问题一起排除。

| 运行 | 已观察到的最远阶段和结果 | 性能含义 |
| --- | --- | --- |
| desktop-touch-c1 | wrapper 在启动 child/browser 前做身份预检，使用了不存在的旧布局路径；失败 JSON 保留，随后仅纠正被检查路径 | 未启动游戏，不是帧率数据 |
| desktop-touch-c2、c3 | 设置/连接入口失败；各自 invocation、日志和失败 report 保留 | 本文未从截断信息推断更细根因，也不把它们计作游戏停顿 |
| desktop-touch-c4 | 真实设置表单的 Enable touch controls switch 经 setChecked 后仍未变为开启；报 Clicking the checkbox did not change its state | 设置入口失败 |
| desktop-touch-c5 | 等待真实 touch switch 开启超时 | 设置入口失败 |
| desktop-touch-c6 | 同一条件的有界等待超时，报告明确 outcome unknown | 设置入口失败，不补写成功 |
| desktop-touch-c7 | 普通 focus + Space 仍未启用 switch；随后出现驱动未安排的可信 mouse/导航事件，详见下文 | 有外部输入/导航干扰证据，但不能归责给用户，也不能倒推解释较早失败 |
| android-touch-c1 | 连接入口 fetch failed | 未获得游戏性能样本 |
| android-touch-c2 | 约 4.390 秒内取得真实 Chrome 157 的 /json/version；驱动过窄的 WebSocket 路径断言拒绝了实际无 UUID 的 /devtools/browser 路径 | 未创建任务自己的页，未进入游戏 |
| android-touch-c3 | HTTP version 与 WebSocket handshake 成功，Playwright connectOverCDP 初始化随后 30 秒超时；整次 33,732.1714 ms | 未创建自己的页，未进入游戏；是测试连接阻塞 |
| android-connect-c1 | 独立 connect-only 诊断在 am start -W 前台阶段 20 秒超时；CDP/mapping 未尝试 | 不是旧 c3 的协议初始化阶段 |
| android-connect-c2 | 普通唤醒后 WS 握手成功，Playwright 页初始化再等 30 秒超时；21 sent / 3 response / 2 events / 18 pending | 连通与初始化边界证据；没有手机游戏成绩 |
| desktop-touch-c8 | headless、CPU4、CDP touch 对照，无未计划 mouse/导航，确认唯一 checkbox 命中，7 个可信事件仍全部 checked=false；普通 Off、没有确认弹窗、仍在 /play/th09 | 入口问题在隔离条件下仍存在；不能与 headed 运行作性能比较 |
| desktop-touch-c9 | checked accessor 写入 trace：window capture click 约 2457 ms 时原生 checked=true，React setter 约 2469.8 ms 又从 true 写成 false；true 未持久化 | 真实设置事件顺序证据，不是游戏 native 或帧率测量 |
| desktop-touch-c10 | 新 UI artifact 被服务器所有权校验拒绝：它不属于当时传入的显式 canonicalRoot；344.509 ms 后 exit1 | 在服务器校验处终止，没有游戏或浏览器初始化 |
| desktop-touch-c11 | 将 owner 指向新树并正式编译 validators 后，冻结服务器仍硬编码读取该树没有的 unused lib/th15-content-layout.mjs；345.156 ms 后 exit1，sourceStable=true | 服务器装配缺文件；没有浏览器或游戏初始化 |
| desktop-touch-c12 | 候选 UI 的 touchPreferenceAfter=true、storage options.touchEnabled=true、finalNativeChecked=true；经过设置并出现真实标题截图；随后 Starting，未见 __th09Runtime / overlay，30 秒条件等待超时 | 开关子目标成功，完整 hosted/probe 阶段未通过；不是回滚性能运行 |
| desktop-touch-c13 | 相同 driver/server 改用 fresh baseline UI，重新超时 real touch switch enabled；起始偏好 false，finalNativeChecked=false，并复现 React 写回 false | 入口开关因果对照的反证端，不能作性能 A/B |
| desktop-touch-c14 | 新 warning trace 中第一次 click 成功，第二次等待退场按钮 10 秒超时；failureState 为 about:blank / epoch0 / Runtime=false，UI Preparing，报告保留的 events=[] | 此次驱动的退场 DOM 竞态已定位；不能据此推断 c12 相同游戏状态 |
| desktop-touch-c15 | warning 仅一次成功点击；实际 runtime=true、overlay=true、epoch=1，真实 launch 已完成；后续 60 秒标题键盘导航得到 touch context=3（Replay），不是要求的 1 | 启动子目标完成；可玩触控/持续移动/回滚性能仍未通过 |
| desktop-touch-c16 | 真实触控菜单后 driver exit0，功能目标全 true，三段 direct 的提交/native type/id 序列一致、native 参数有限；离线 finalIntegrationQualified=false | 功能集成取得证据，严格总资格未通过；带重观测的新状态最大间隔仍有 5689.2 ms，不是流畅性通过 |
| desktop-stick-c17 | 进入真实 gameplay；queue-held 的 264 个计划 MOVE 完成 257 个即触发 20 秒派发上限；driver 与离线总资格均失败，fresh 未执行 | 已有 controls→stick 次序证据与真实长间隔；单机回滚计数全零，不是回滚或手机性能结论 |
| desktop-stick-c18 | documented launch warning / actual launch 边界 180 秒超时；未进入游戏，gestureAttempts=[]，只有 2 次设置 touch 注入、零移动；无 retained probe | 是启动/本地模块请求失败，不能评价 MOVE window4、游戏帧率或回滚性能 |
| Android owned-target raw CDP c1 | 自有新空白页创建、flattened attach、Page.enable 与 1+1 求值通过，诊断完整且清理合格；page WebSocket fallback 未尝试 | 只证明这条最小协议路径可用，不是手机游戏或完整 Launcher 通过 |

c7 的全局 audit 保留了 42 个事件。约 3.1 秒的前 5 个键盘/checkbox 事件中，开关状态均为 false；约 7.3 秒后才出现多次可信 mouse 点击、离开设置、打开 TH08/TH06 等未计划行为。驱动当时仍在等开关，未发 mouse，plannedInjections 为 0。后发生的导航不能作为前面 false 状态的因果解释；日志也不足以指认输入来自谁。c8 去掉这类未计划 mouse/导航后，真实 switch 仍为 false，进一步否定了“仅用 c7 后半段干扰解释开关失败”的说法。

Android c2 的修正只放宽到已观测的合法 DevTools 路径，之后 c3 确实跨过了 HTTP 与 WebSocket 握手。这是入口定位的进展，不能把下一阶段 connect 初始化超时重新描述成相同的路径问题。

[入口诊断摘要](rollback-managed-touch-entry-diagnostics-c1.json)保留 c2/c3/c7 的精简事件和清理：Android c2、c3 的任务映射最终为空，all cleanup 为 true；桌面 c7 的 cleanup 数组为空且 serverClean 为 true，不能自行把空数组扩写成未记录的逐资源清理。三份原始 report 均保持稳定。提取命令的 stdout 超过 16 KB 被截断，**完整文件才是本段的审计对象，不能用截断 stdout 声称全覆盖。** 该诊断文件尚未收入 c8；c8 的 raw 单独保存，记录 cleanup=[]、serverClean=true。


c9 记录到具体写入顺序：window capture click 约 2457 ms 时，浏览器原生 checked 属性已是 true；约 2469.8 ms 的 React commit setter 又写回 false；后续 document input/change 观察到 false，storage 从未落地 true。这里的“原生”指浏览器 checkbox 属性，不是 TH09 C++ 输入。checked accessor 已还原，source 与清理资格保持 clean。

fresh baseline c13 在同一 driver/server 上再次复现：window click 4102.200000 ms 时 nativeChecked=true；React checked setter 4150.5 ms 记录 requested=false、nativeBefore=true、nativeAfter=false；input 4155 ms 与 change 4169.400000 ms 均为 false。gameoptions storage key 在 Before/After 均为 null，overflow=false、accessor restored=true。serverFinal.clean=true，181 项输入文件身份检查（inputsAfter，非触控事件数）stable=true、mismatches=[]、errors=[]，cleanupErrors=[]、sourceStable=true。

候选 c12 则真实持久化 touchEnabled=true，最终 checked=true；当前已经读取的 setter 行均为 requested=true、nativeBefore=true、nativeAfter=true，不声称尚未完整读取的 setter 行数。source/UI 的受控替换和实际字段对照支持入口开关修复；这不是按两次运行耗时或 FPS 做出的性能判断。

c12 的后续失败也保留：完整运行 47,632.1838 ms 后 exit1，driver 的 Starting 条件等待 30 秒，预期 __th09Runtime 观察入口和 overlay 尚未出现。截图已经显示标题，不能仅从缺少这个 probe global 推出引擎从未运行；但也没有完成原定 hosted/probe 门禁，不能补写 launch PASS。driver/server 的记录身份短前缀分别为 6463716b… / 384af56e…，完整身份以该 run report 为准；UI identity 为上述候选 1d607295…。

acceptedWarnings 中同类记录两次，最多说明 driver 记录两次同类点击/观察，不能证明两次有效授权、prepare 或 launch。实际 gate 以 snapshot !== prompt 拒绝旧提示，最终 accept 先同步 take()/清 pending，再调用一次该 request.accept；退出动画会保留旧 warning DOM，且 settings-launch 有 active.promise 防重入。暂无两次 launch 的原始 ACK 证据。新 Launcher 树的源码边界为 app/services/launch-warnings.ts:37–46、app/components/LaunchWarnings.tsx:23–45、app/components/AnimatedDialog.tsx:72–90、app/services/settings-launch.client.ts:64–67。

c14 加了具体 warning click trace：第一次 click 的驱动记录为 10237.0098→10746.4019，成功；第二次为 10989.0538→20998.1338，locator.click 等已经退出的按钮，10 秒超时。整次 25,598.6132 ms 后 exit1，failureState 为 about:blank、epoch=0、Runtime=false，UI 仍 Preparing，报告保留的 events 数组为空；server clean、sourceStable、cleanup=[]。这些是该次驱动和页面状态，不是游戏输入时延，也不能倒推 c12 的标题截图/Starting 属于同一状态。

driver 已据真实 launchInputWarnings 的单次 Start 每种 kind 至多一次这一边界修正：成功可信点击某种 warning 后，本次 Start 不重复点击同 kind。仍通过原 UI gate，保留 180 秒启动等待与原游戏性能门槛，没有绕过确认。c15 随后跨过 warning 和 launch，但在要求的可玩触控上下文处失败，具体边界如下。

c15（命令 FvGbWj-4leUDAwNB-ErJ_FmF）整次 106,847.8874 ms 后 exit1；warning 只有一次成功点击，driver 记录 11083.0824→13035.9384。report.launched 的 Node 观察区间为 42476.3511→42544.4029，记录 runtime=true、overlay=true、current epoch=1、title frame=44。子页面 configure request 为 runtime-1-1，launch request 为 runtime-1-2，真实 launch 已完成。

父页面的 runtimeEvents 进一步保留了实际启动链，全部属于 parent performance.timeOrigin=1791703450319.5 下的 performance 时点（ms）：

| 父页面事件 | 时点（ms） | 已观察结果 |
| --- | ---: | --- |
| ready | 26412.7 | 接收到 ready |
| configure ACK | 26705.1 | ok=true |
| runtime-info | 38226.9 | 接收到 Runtime 信息 |
| launch ACK | 39112.1 | ok=true |
| first-frame | 39747.4 | 接收到首帧事件 |

子页面另有 startup-image=10734.4、menu-ready=14887.9，child performance.timeOrigin=1791703473584.7。上述 parent、child 和 Node 观察时钟分别保留；本文不直接跨时钟相减，不把 Node 的观察区间当作真实首次启动时刻，也不把这些入口事件换算为物理触控延迟。

随后约 60 秒键盘标题导航后，报告中名为 gameReady 的状态记录是 title frame=3435、session=[1,1917,1917,7996,0,9,1,0]、touch=[3,0,0,0]、iframe focus=false。Application.cpp:320 明确把 touch context=3 定义为 Replay，未达到 driver 要求的 context=1。字段名 gameReady 不构成该门禁已经通过；上述 session/touch 数组原样保留，不套用其它诊断数组的字段解释。

是否进入 attract demo 仍待审计，不能写成已确定的原因；iframe focus=false 也是观测，不单独证明 host 输入没有被转发。真实页面推进和标题/回放状态既不能代替可玩触控，也不能把总 106 秒或这两个 frame counter 相除写成 FPS、预测回滚成本或手机成绩。

c16 通过真实触控菜单继续推进，命令 DIW0FRzDfbap1gTxsfNLm0Ke，102,143.7656 ms 后 exit0；driver 的 functional goals 全部 true，source 和清理正常。三段 direct 的提交数与对应 native 调用数分别同为 110、266、56；自动断言确认 native args 的 type/id（前两项）序列与提交一致，并且 native 参数全部有限。坐标需要经过映射，该断言没有证明全部 native 坐标与 DOM message 逐值相等。这里证明的是这次真实 Launcher 链路的功能与这些交付边界；全部 native 调用总数、物理触控延迟和预测回滚性能不由这些计数推导。

120 Hz 是源计划，不能写成实际注入达到 120 Hz。三段实际注入 duration 分别为 7129.5719、8818.6883、2841.9074 ms；不同 duration 和实际事件数量的段不作因果性能对照。离线汇总保留的相邻可观测新状态长间隔如下，单位为 ms：

| direct phase | 提交数 / 对应 native 数 | 实际注入 duration | 新状态间隔 p99 | max | >50 ms 数 | >100 ms 数 |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| baseline | 110 / 110 | 7129.5719 | 145.516 | 185.300 | 42 | 9 |
| queue | 266 / 266 | 8818.6883 | 154.975 | 5689.200 | 52 | 9 |
| fresh | 56 / 56 | 2841.9074 | 155.258 | 216.900 | 17 | 2 |

离线命令 vhKUP3bG5RQuQya85NjptcDc 用时 178.5154 ms 后 exit1，结果保存在 [c16 evidence](rollback-managed-touch-desktop-c16-evidence.json)。probe 记录完整、三段 movement phases 的 qualified 都为 true，但全局 finalIntegrationQualified=false。唯一 analysisIssue 为 hud-focus 的 lifecycle-owner-or-hidden-boundary：该段有两条 host-blur 记录，记录时 hostHidden 和 childHidden 都为 false。现阶段不能把 capture 监听器收到的这些 blur 直接认作 Window 失焦或 owner 已失效，也不能忽略原资格失败把总结果改写为 PASS。

这是一轮带重 probe 的真实桌面单机输入试验，严重长间隔仍是事实；不能据此称 managed 性能改善，也不能直接当成无 probe 的手机或 relay 回滚结果。snapshot 的 copyWall 记录分别为 beforeRelease=245.7 ms、final=578.8 ms、disposed=633.6 ms；它们不能覆盖所有观测/导出成本，不能用来扣减新状态间隔，也不足以完整解释 5.6892 秒停顿。

后续 browser 采集的测试代码已改成测量中只取 metadata-only snapshot，dispose 后一次导出 full raw；probe 新增 blur target provenance，真实 Window blur 仍保留为生命周期边界，element blur 单独记为 focus-observed。c16 的旧 raw 没有 target 来源，不能事后追认这些旧 blur 属于哪类，也不能重写其严格 analyzer=false。旧 raw、exit1 汇总和所有不利间隔完整保留。这些是观测侧修改，不是游戏生产优化；c17 的实际结果如下。

c17（managed-touch-desktop-stick-c17，命令 MWQFViS-NBrYTkGOjvYN6TUo）进入真实 gameplay 后，queue-held trajectory 触发 20 秒 bounded gesture dispatch duration；整次 113,397.9902 ms 后 exit1。该段计划 264 个 MOVE，仅完成 257 个；baseline 的 108 个 MOVE 完成，但原定 1800 ms 的源计划实际用了 10,643.2461 ms，不能写成计划频率已经达成。baseline 的 touch-controls submit/delivery 均为 110，native controls 与 stick 各 110；queue 对应均为 259、native controls 与 stick 各 259。已观察到的每条消息调用次序验证通过，不等于完整源计划完成；fresh phase 未执行。

| stick phase | 新状态间隔数 n | p99（ms） | max（ms） | >50 ms 数 | >100 ms 数 | C++ rAF callback wall max（ms） |
| --- | ---: | ---: | ---: | ---: | ---: | ---: |
| baseline | 230 | 234.680 | 343.500 | 82 | 12 | 45.200 |
| queue | 462 | 175.429 | 220.300 | 162 | 27 | 41.600 |

这里的 C++ 数值是整个 rAF callback 的墙钟工作时间，不是 pure tick 或 CPU 时间。最终 rollback stats 全部为 0，本次为单机；以上已观察新状态间隔不能归因于回滚，也不能外推手机或网络表现。

[c17 离线 evidence](rollback-managed-touch-desktop-c17-evidence.json)由命令 x6QxzrAbrubT1EdjeSClfIY8 生成，170.2333 ms 后 exit1；driver=false、final=false，probeComplete=true、cleanup=true。hud-focus 仍有 boundary 造成不完整，fresh 未执行。新增来源记录保留 seq1462 的 A element blur（该行 hostHasFocus=true、childHasFocus=false），以及 seq1465 的真实 Window target（该行 hostHasFocus/childHasFocus 都为 true、hostHidden/childHidden 都为 false）。不能把这些记录写成整个浏览器失焦或设备挂起，也不能把完整 probe/清理误当完整运行通过。

c17 的 sourceStable、probeRestored、serverClean 都为 true，cleanupErrors=[]；失败后的 snapshot 记录 216 ms、disposed snapshot 338.2 ms。主要移动 phase 的长间隔发生在 full 导出之前，不能把它们全部归因于旧 snapshot 导出方式。单机、注入和其它观测开销仍在；c16/c17 的运动模式与实际派发节奏不同，不能借两组数字作性能或探针因果 A/B。

c18（managed-touch-desktop-stick-c18，命令 qoPhuv91Ox9Sad_SkRZ3iKHX）已在 documented launch warning / actual launch 边界触发 180 秒等待上限，wrapper 213,521.0294 ms 后 exit1，未进入游戏。gestureAttempts=[]，只有设置过程的 2 次 touch injections，零移动；因此不能评价 MOVE window4 的好坏或游戏性能。观察到本地 /runtime/th09/shared-netplay.mjs 请求 net::ERR_CONNECTION_TIMED_OUT（Node 记录时点 51237.6085 ms），后续有 EAGLER_RUNTIME_SESSION_SUPERSEDED、hidden、epoch=1；超时与会话失效的根因尚未知，不能归因于回滚或设备性能。

c18 的 sourceStable=true、cleanupErrors=[]、serverFinal.clean=true。[严格离线 evidence](rollback-managed-touch-desktop-c18-evidence.json)由命令 L4zJELHc7D3Zu_sBPXWEF3Ek 生成，67.0775 ms 后 exit1，analyzer 仍为 ee1186fc978050f05d8be86c08a95aac83117af966b1e4fd800aed951d91638e；issues=['no retained probe snapshot']、phases=[]。没有从启动耗时或缺失的运动 phase 补出帧率、零卡顿或性能结论。


c10 在加载新 UI 产物时，被服务器正确拒绝“UI 不属于所传显式 canonicalRoot”。随后新源码树的 host validators 正式编译通过，但 c11 又在装配阶段发现冻结服务器硬编码引用的 unused lib/th15-content-layout.mjs 不在该树。两次都没有浏览器或游戏初始化，不能归类为 checkbox 候选运行失败或游戏卡顿。

c12/c13 已使用显式分开的 uiSourceRoot（各自 UI owner）和 canonicalRoot（原冻结 helpers owner）完成装配对照，保留 containment、hash、normalization 与旧 helper 的精确字节。没有把用户当前脏源码整批复制到新树，也没有取消所有权检查。装配阻断已跨过，c15 完成真实启动，c16 取得实际移动与功能门禁证据；当前缺口是严格观测总资格、观测开销及玩家目标下的性能。不能把这些阶段继续混作 c10/c11 同一个装配错误。

独立 managed-touch-android-connect-c1 已结束：命令 WHS70uMtWRyp0k568XLUbK7S，20,625.3751 ms 后 exit1，仅在 am start -W 前台阶段遇到 20 秒 ETIMEDOUT，CDP 和 mapping 都没有 attempted。sourceStable 与 cleanupQualified 为 true，只说明该次源身份和清理范围合格，不代表连接诊断完整或游戏通过。它与旧 Android Launcher c3 的“WebSocket 已握手、Playwright 初始化再等 30 秒”属于不同阶段，不能合并为同一故障。

随后有界 metadata 命令 O6TFNtyQxiWxCwcZNoOdytO9 观察到设备可连接，但屏幕 Asleep/OFF、keyguard showing=true；普通 wake 命令 8oOXgyYqYwHNw4CuQ8NnFJmM 后为 Awake/ON、keyguard secure=false。正常消除非安全锁后，状态为 Awake/ON、secure=false、showing=false，Chrome resumed。这些是实际入口条件，不倒推旧 c3 的超时原因。

在该条件下，managed-touch-android-connect-c2（命令 9qWLzFUu5yYJOLd2UCQrGzwg）仍在 WS 握手后遇到 Playwright 页初始化 30 秒超时，整次 30,821.2302 ms。诊断保留 21 条 sent、3 条 response、2 个 event，18 条 pending，表现为两组各 9 条 Page/Runtime/Network/Target 初始化命令；dispatch queue 最终 0、peak=1，没有 drop 或 overflow，sourceStable/cleanupQualified=true。没有观察到本探针接收 FIFO 的堆积或丢弃，但这些计数不能单独把根因归给设备、游戏或 Playwright 某个内部阶段。连接诊断尚未完成，更没有手机游戏性能结果。

Android owned-target raw CDP c1 已通过，命令 AqroxpGEo5-iUgkMhRJJBRBJ，wrapper 总耗时 2,060.7824 ms。探针只操作创建 ACK 确认归属于本任务的新空白页，分开核对浏览器根协议、自有 target 和 flattened session。报告中的阶段时间值如下；这些值用于复核诊断过程，不是游戏帧间隔、手机触控延迟或各阶段独立耗时：

| 探针阶段记录 | 时间值（ms） | 已观察结果 |
| --- | ---: | --- |
| deviceReady | 1417.9858 | 设备准备条件满足 |
| browser root responsive | 1493.6443 | 浏览器根协议响应 |
| owned-target-confirmed | 1581.3082 | 创建 ACK 已确认自有 target |
| flat Page.enable 与 1+1 | 1837.7365 | enable 与求值通过 |

primary 的 attached/responded/valid 都为 true；pageWSfallback 没有尝试，false 不表示 fallback 失败。diagnosticComplete、sourceStable、cleanupQualified 都为 true，creationOutcomeUnknown=false；自己的 target 已关闭并核对消失，精确 owned forward 清理后不存在。设备前后均为 Awake/ON、Chrome resumed，keyguard 的 secure/showing/inputRestricted 均为 false；本轮未需要 dismiss。

这一历史结果只证明真实设备上的“自有新空白页 + flattened raw CDP”可用。它不否定 connect-only c2 经 Playwright 初始化整个浏览器既有页面时的 30 秒超时，也不证明 Android Launcher、可玩触控或预测回滚通过。Android Launcher driver 仍属于未执行游戏的准备工作，其未知纠正写入不算验证；用户已禁止手机测试，后续 Android 设备/session/gameplay 实验全部停止。桌面 c16/c17 的输入结果与资格另见上文，不据此补造手机成绩。

后续应先解决可重复的真实入口，并保留每一次失败。不能通过改内部开关状态、绕过 ordinary Launcher 或伪造 provider，把这批失败变成集成 PASS。


### 4.1 新增 c19 / c20：真实启动、单机输入功能与严格资格分别记录

c19（managed-touch-desktop-stick-c19，命令 9SHRanEWqzjqqt9ngslLWd55）在 39,931.779 ms 后 exit1，但**游戏实际已经启动**：UI 为 Game running，runtime=true、epoch=1、visible=true，error 为空。失败来自退出动画期间 warnings.count / getAttribute 的分步观察竞态，等待了 30 秒；没有 gesture 或 probe 样本。它是驱动的 warning 观察失败，不能写成游戏未启动，也不能当作游戏卡顿或回滚成绩。c20 将 warning kinds 改为一次 evaluateAll 读取，保留正常 UI 操作和门禁。

c20（managed-touch-desktop-stick-c20，命令 saXAWnBk1jCgmDa2jjZbz4S4）在 37,227.8118 ms 后 exit0。实际使用固定 normal Chrome 153、headed、CPU4、dispatch window4；本次 GPU 读取正面绑定当前 epoch=1 的 core / canvas / context，实际 Intel UHD / D3D11，software=false。此前“读取到的 canvas 是否属于当前 core”与摘要未透传后端信息的缺口已修复；这些资格只属于本次记录，不能追认旧缺失样本。

| c20 运动 phase | 源计划时长 | 实际派发时长 | 实际 MOVE 派发数 | DOM MOVE 观察数 | controls post / child delivery / native controls / native stick |
| --- | ---: | ---: | ---: | ---: | --- |
| baseline | 1,800 ms | 1,825.4613 ms | 108 | 105 | 106 / 106 / 106 / 106 |
| queue-held | 2,200 ms | 2,224.4664 ms | 264 | 217 | 213 / 213 / 213 / 213 |
| fresh | 900 ms | 917.6258 ms | 54 | 54 | 56 / 56 / 56 / 56 |

源 MOVE、DOM 观察和 controls/native 调用是不同层的分母，不能强行写成逐 MOVE 一对一，也不据计数差直接判消息丢失。七项功能 goal 全部为 true，sourceStable、清理资格、probeRestored、serverClean 通过；这些支持本轮真实 Launcher 单机输入通路和受控资源等待期间的功能门禁。

| c20 运动 phase | child sample 新状态间隔数 N | p99 | max | >50 ms 数 |
| --- | ---: | ---: | ---: | ---: |
| baseline | 127 | 22.096 ms | 30.400 ms | 0 |
| queue-held | 155 | 27.944 ms | 29.100 ms | 0 |
| fresh | 74 | 26.635 ms | 27.000 ms | 0 |

上表是 **child sample 时钟上的相邻可观测新状态间隔**，不是物理显示时间，也不是正式 smoothness 的 rAF 时间戳指标。三段运动局部资格均为 true；本轮是单机输入通路验证，全部八词 rollback stats 最终为 0，不能称多人回滚性能通过。c17→c20 同时改变了多个浏览器、派发和测试条件，没有形成单一生产改动的严格 A/B，不能据此宣称某一项生产优化带来了提速。

[c20 严格离线 evidence](rollback-managed-touch-desktop-c20-evidence.json)由命令 I6ROgE4R_3BwgWJEtW6oGiME 生成，137.4926 ms 后 exit1；finalIntegrationQualified=false，唯一问题在 phase2 hud-focus 的 1 项局部 diagnostic。该版 analyzer 只新增后端 metadata 透传，没有放宽 frame/input/lifecycle 资格，SHA-256 为 3a1f351ea689fb82c9b2c1493ca3a2b98c4c5234a16cf0970bc32e7b5f2f492c。137.4926 ms 是离线程序耗时，不是游戏性能。

HUD 小份复核（命令 wfgYdRe4ammdsx70841oMdiJ，exit0、552 ms）确认：唯一 lifecycle 在 rawRecordIndex=1583 / seq=1584，boundary=host-blur、windowTarget=true、eventPhase=2；hostHasFocus 与 childHasFocus 都为 true，hostHidden 与 childHidden 都为 false。此前 seq=1581 是元素 A 的 focus-observed host-blur，windowTarget=false、childHasFocus=false。该形态与 c17 已知 HUD refocus 记录相似，但真实 Window blur 边界仍保留，整体 false 不改成 true；它既不证明系统失焦或整页不可见，也不证明该边界造成卡顿。75 项有效 diagnosticRead 的最大记录为 0.3 ms，probe 完整；这不是全部探针开销的校准结论。

## 5. 必须保留的性能反证：持续拖动和长停顿仍在

### 5.1 held analog 修复只解决了部分负载

上一阶段 C++ held analog 候选在 mode0、D0、H8 下保留当前目标/向量，保持 full-field correction 和 restore 前 once-only capture。直接触控三世界 baseline/candidate 的实际完整 12B 输入 tape 和 Replay 完全一致，整个运行的 correction/resim 约下降 5%。

这个平均变化主要不能替代动作分段检查：双方 60 个 hold 帧内，恢复起点由 60 降到 20，实际重演 frame visits 由 480 降到 202；但是 **216 帧连续主拖动仍然是 216 次恢复起点**。candidate 的手工三世界总调用墙钟更慢也完整保留，没有归咎给机器或从结果中删掉。

三世界是单个受限 renderer 顺序手工调用三个世界的正确性/工作量试验，不能把整个启动加参考世界耗时除以 900 帧，称作正式 relay FPS。分段修正来源按对端真实 capture 的手势段取上下文，不把本地手势当成远端纠错原因，也不把时间相邻当精确因果。[上一阶段报告](rollback-mobile-work.md)及[纠正后的 deep 证据](rollback-mobile-deep-corrected-evidence.json)保留完整结果。

### 5.2 正式 release 数据既有好结果，也有不能通过的长尾

下表是上一阶段 CPU4 的 P2、端点闭合的新状态观察结果。本轮 managed 修复没有重跑并替换这些数据。

| 正式运行 | 连续 frontier Hz | 相邻可观测新状态 sampledAt 最大间隔 | 该最坏窗口内合格已登记 rAF 请求等待并集 |
| --- | ---: | ---: | ---: |
| relay39 B1 | 34.422 | 175.270 ms | 108.665 ms |
| relay39 C1 | 59.109 | 117.005 ms | 104.720 ms |
| relay39 C2 | 59.552 | 67.325 ms | 55.800 ms |
| relay39 B2 | 59.740 | 78.980 ms | 67.165 ms |
| burst120 candidate | 59.209 | 86.270 ms | 65.115 ms |
| direct120-c2 candidate | 59.948 | 53.270 ms | 42.145 ms |

六份 release 的正确性检查通过，不意味着六份都达到了玩家的无大停顿目标。**C1 即使平均 59.109 Hz，117.005 ms 的长间隔仍是不利结果。** B1 和 B2 自身差异很大，而且实际采样 tape 在四次 ABBA 间不同，不能把 B1 到 C1 的 Hz 改善宣称为候选的因果速度收益。

rAF 列仅涵盖已保留、能关联到 onGameFrame 的请求。资格区间从 max(requestedAt, parent.exitedAt) 到 enteredAt，在同一 Page clock 中裁切并求并集。它证明请求已经登记且在这段可观察时间没有进入回调，不能进一步指认 OS、浏览器、GPU、其它 JS 或宿主编译是哪一个原因。也不能用墙钟减进程/线程 CPU counter 构造“纯等待”。

release sample 的 s[2] 是整个 C++ rAF callback 墙钟，可能含零次或多次 tick、同步桥接、音频及绘制；它不是纯单 tick CPU。统计完整 body 的同时必须分列 fresh-single-step、恢复混合与没有新 frontier 的调用，防止大量空回调中位数掩盖真实推进成本。

rollback_info 的 stats[5] 及 release sample 对应索引 8 是 **Channel RepairsSent / repairsSent**，不是 native wait。旧 deep v1 的错误名称与首次产物留档，正确解释应使用 v2；returnCode 2 也不能无证据地等同网络等待。[命名纠正记录](rollback-mobile-analysis-corrections.json)、[release 汇总](rollback-mobile-release-evidence.json)和[gap 请求证据](rollback-mobile-gap-dispatch-evidence.json)保留这些限制。


### 5.3 新结果：桌面双端 CPU4、双方变化输入的独立 relay c1 / c2

这两次是新的桌面双端 CPU4 relay 系列，与上表的旧 release ABBA 以及 Launcher 单机 c16–c18 分开。人工 joystick 输入经过 native ABI；120 Hz 是独立源计划，不是已经证明达成的事件、采样或新状态频率，也不替代真实 React overlay 的逐消息功能资格。[c1 完整证据](rollback-desktop-touch-both4-relay39-c1-evidence.json)与[c2 完整证据](rollback-desktop-touch-both4-relay39-c2-evidence.json)分别保留结果。

**c1 的 wrapper exit0，但严格连续窗口不合格。** 命令 nk2Sq_3nefErgv_p7d54emI_，wrapper 62,727.217 ms，源码稳定。两端均为 Chrome 153.0.8010.12，CPU 限速 [4,4]；既有 normal executable 身份与 flags 已核对，实际游戏图形后端记录为 Intel UHD / ANGLE Direct3D11。两端实际 targetChanges 为 1,157 / 1,093，15 个规定确认点一致。采集漏了 native prediction frontier，因此以下 Hz / 间隔只能作为已观察区间的描述，不能冒充通过严格窗口门禁的正式成绩。

**c2 保留了 native frontier 采集，但没有覆盖预定终点。** 补充独立于 rAF 的观测后，wrapper 约 157,609 ms 超时；工作负载仅推进到约 1,707 帧，14 个确认点到 1,680 一致。终点缺失不能从剩余样本拼接或外推完成；以下统计保留的是不完整运行中真实观察到的长间隔。

| 已观察的新状态指标 | c1 P1 | c1 P2 | c2 P1 | c2 P2 |
| --- | ---: | ---: | ---: | ---: |
| 描述性新状态 Hz | 57.1846 | 57.1014 | 约 15.73 | 约 15.58 |
| 相邻可观测新状态间隔 p99 | 30.290 ms | 30.305 ms | 224.220 ms | 218.020 ms |
| 相邻可观测新状态间隔 max | 63.160 ms | 47.415 ms | 339.380 ms | 284.755 ms |
| 大于 100 ms 的间隔数 | 0 | 0 | 250 | 268 |

c1 大于 50 ms 的间隔为 3 / 0；正文 corrections 为 1,077 / 1,117，resimulated 为 3,451 / 3,533，captures 均为 1,620。c2 大于 250 ms 的间隔为 9 / 6。这些不利区间不会因为确认前缀一致或某次接近实时的平均值而删除；c1 的缺字段和 c2 的未闭合正文也不会被改写成 PASS。

c2 正文的整个 C++ rAF callback 墙钟，p50 约为 22.360 / 22.235 ms，p99 约为 53.060 / 53.160 ms。它包含该 callback 的真实工作，不能叫纯重演或单 tick CPU。一个 339 ms 级的新状态间隔跨度内，单 callback 最大仅 29.765 ms，且对应 1 次 correction / 5 次 resimulation；这些事实不足以把整个长间隔解释成“某一次巨量重演”。仍需用同一时钟的 callback、请求/派发与网络记录继续定位跨度内发生了什么。

c2 的网络观测摘要中，信息缺口中位数为 1，backlog 中位数为 0，scale 中位数为 1、偶发到 2。它们不能把所有长停顿都归为网络信息不足。宿主平均 busy 则从 c1 的 45.73% 降到 c2 的 34.69%；现有证据没有证明后台编译造成了 c2 退化，也不能用平均宿主负载排除某个关键时段或其它路径的阻塞。

后端资格同样分开：c1 有实际 renderer 证据；c2 的失败采集只有 executable / flags，缺少实际 GPU renderer，不能追认 backend 已验证。两组还存在测量变化和终点覆盖差异，未在这里证明跨 run 的实际输入 tape 相同，不能当作匹配输入的性能 A/B。当前结论是继续定位并复验，不宣传双端 CPU4 已达标，也不简单归因设备性能。

c1 原始 report 为 26,013,187 B，SHA-256 32185bd7f93526011a547e9a77e178bc7313fd641c7edb364bc9a3d2dd5762e2；对应 evidence 为 523,031 B，SHA-256 219abe527782d5acfb26285ca5c12bd116da8b53dc1644b188d36e44c6e71fb7。c2 原始 report 为 26,249,699 B，SHA-256 f6e2b4a50ec5bafb2e476d7424f6f1a3f48713da00d4d53a03a2a02d354d5d3f，运行命令 eA9ENAFIjcZ_uewQIXmMh1L1；对应 evidence 为 656,491 B，SHA-256 508861cc07e07b1d98b5d8fa56fcb656c62ece8ecc5779810c82ebb8e0526768。两份原始 report 和不完整/失败结果保持不变。


#### c2 后续核对：最大端点与普通前进 callback 的描述性子集

[独立 followup](rollback-desktop-touch-both4-relay39-c2-followup.json)由命令 6xbhr_5COxjqH-SXsq_Nbazm 生成，exit0、805 ms；65,733 B，SHA-256 0acff6c37b12f57df8392ebe093865d6a1c88f6876bf9f110fd7844dd99616dc。805 ms 仅为离线核对耗时。authoritative 连续窗口 helper 确认 c2 两端最大 gap 的原始端点索引分别为 2541→2546、1835→1844，与此前副分析相同；此核对只覆盖这两个最大端点，未给其它副排名背书。

另按“相邻正文 callback 的 session phase=1、next 增加 1、correction/resimulation 的相邻增量均为 0、capture 增加 1”提取普通前进的描述性子集。它没有被改称纯 tick、纯 CPU 或严格 fresh-single-step 成本，仍是完整 C++ rAF callback 墙钟：

| 运行 / peer | 子集 callback 数 n | callback wall p50 | callback wall p99 |
| --- | ---: | ---: | ---: |
| c1 P1 | 590 | 3.490 ms | 7.005 ms |
| c1 P2 | 570 | 3.610 ms | 7.970 ms |
| c2 P1 | 925 | 26.095 ms | 54.335 ms |
| c2 P2 | 894 | 25.975 ms | 56.235 ms |

不同运行的墙钟输入样本与观察条件不同，这不是严格 A/B 或纯 CPU 比较；但较高成本也出现在没有新增 correction/resimulation 的普通前进子集中，因此不能把 c2 的全部退化都归为深回滚重演。仍需按当前实际后端与观测开销证据进一步分解，不能仅据这张表指定原因。


另一次字段核对确认，c1/c2 的 frameDetails 一直存在；capture 次数不等于 WorldState Save 次数，不能以 captures 作 Save 成本分母。c1 的普通前进子集全部 WorldSave=1。c2 同一描述性子集按 WorldSave=0 分组，P1 n=121、整个 callback wall p50=20.745 ms，P2 n=129、p50=20.090 ms；WorldSave=1 分组的 p50 则为 27.110 / 26.865 ms。

因此，即使该 callback 没有执行 WorldState Save，c2 的部分普通前进调用仍然很慢；不能把全部成本归于 Save/restore。这里依旧是不同实际墙钟输入负载下的整个 callback，不是纯 Save、纯 tick 或纯 CPU 的测量，也不是对 VM 整链去重候选收益的预先证明。


### 5.4 relay c3：完整双端 CPU4 窗口通过，但仍约 56.7 Hz

本次完成的是桌面 native ABI 人工触控的双端 relay 运行，不是 Launcher 多人输入集成或手机测试。运行命令 fn-hhXaYZPc6pEW3mO5COavZ，exit0、wrapper 55,282.1303 ms，sourceStable=true；没有新增生产改动，WASM 仍为第 3.3 节的同一 037c9cd52e70e2c8e8ee516cf15b6aee7f0de5951f88d519b2311cc1c5def373。

本轮 smoothness-browser.mjs 已落地初始/失败分支的实际 GPU 读取和 opt-in RENDER_WORK 10 词数值观测，冻结为 714 行、69,739 B，SHA-256 6d400d7906a6e3af1d988739f95ba534a0b28a8c64133aa475a5be22255a4ce5。diff 审查与 syntax preflight 均已完成；后者命令 OV5nay-8I0zjP0Az1MvqhFbC，exit0、514 ms，属于语法检查耗时。本次 RENDER_WORK=1，新增观测的开销尚未校准，不能把新旧测量差异直接写成生产提速。

两端为 Chrome 153 headed、CPU [4,4]。初始实际 GPU 均为 Intel UHD / D3D11，core/canvas/context 身份核验全部为 true，结束 renderer 相同。两端 native frontier 完整，180→1800 的严格测量窗口 complete；规定的 15 个 confirmed hash 到 1800 全部一致。

| c3 正文指标 | P1 | P2 |
| --- | ---: | ---: |
| body 新状态间隔数 N | 1,619 | 1,619 |
| 观察新状态 Hz | 56.739209 | 56.714960 |
| logic Hz | 56.734598 | 56.711649 |
| 新状态间隔 p99 | 30.320 ms | 24.295 ms |
| 新状态间隔 max | 48.485 ms | 36.335 ms |
| >50 ms / >100 ms | 0 / 0 | 0 / 0 |
| 整个 C++ callback wall p99 | 14.025 ms | 10.515 ms |
| 整个 C++ callback wall max | 19.590 ms | 13.800 ms |

完整窗口与表中的 body gap 分母分别按原证据保留，不从启动或全部 tape 长度重算。该次没有观察到 >50 ms 的正文新状态间隔，是有用的正面结果；**约 56.7 Hz 仍不是稳定 60 Hz，也不能消除 c2 的真实坏样本或替全部玩家负载验收通过。** [c3 派生 evidence](rollback-desktop-touch-both4-relay39-c3-evidence.json)保留完整资格与结果。

双方持续触控有效，targetChanges=1,071 / 1,156，actualInputFrames=1,807 / 1,808；changedHistory 与 captureGaps 均为 0，MOVE coalesced=851 / 740。它们是实际输入/观测分母，不能把计划 120 Hz 写成两端实际逐事件交付；本次仍经 native ABI，不能替代 Launcher 多人链路。

图形记录为 4,697 / 5,150 行，无 overflow 或 counter regression，index 与 presentation 一致。沿用第 5.3 节普通前进的描述性条件，P1/P2 子集 N=582 / 609，整个 C++ callback wall p50=4.460 / 3.225 ms；其中 WorldSave=1 的计数为 580 / 609，不把 captures 直接当成 Save 次数。

仅在该普通前进子集中，图形中位数为 29 batches、5 次 buffer replacement，vertexUploadBytes 中位数为 67,844 / 67,780 B；graphics.uploadBytes（texture）、readBytes（pixel）、programCompiles、resamples（GPU）的总 delta 均为 0。非零的顶点上传与这四个零字段是不同计数，不能混为“没有任何上传”，也不能把子集的零值外推到整个运行或其它负载。两端所附宿主整段平均 busy 均为 25.0054%；它只是宿主背景概况，不是 c3 改善的因果解释。

原始 [c3 report](../../th09_web/artifacts/multiplayer-tests/desktop-touch-both4-relay39-c3-report.json) 为 29,358,458 B，SHA-256 4c0fee9625eaccf3ead1bdb96e505150ec3e52ab7c3f74f9c4dca371cc8aa685。派生 evidence 为 115,114 B，SHA-256 9a7f7b748f6177b571852471ea236a55039561762e855fb4f7552720ef6b7083；离线命令 kZ79qksomxVDvCLIJdOGhJuv，exit0、985 ms，后者不是游戏运行耗时。

### 5.5 RTC direct c1：已验证直连路径仍出现明显长间隔

direct c1 已完成，命令 LWJrtxz7cbX_IGZ_8_14NP_S，exit0、wrapper 59,074.5264 ms，sourceStable=true。同一 WASM、同一冻结 driver、CPU [4,4]、RENDER_WORK=1；delay/jitter=0、site=none，运行要求 EXPECT_RTC_PATH=direct。两端实际 selected candidate pair 均为 host↔host UDP、succeeded；input lane 为 unordered、maxRetransmits=0，buffer=0。实际 GPU 同为 Intel UHD / D3D11。这些是本次所观察到的直连证据，不从标签推定路线，也不外推不同 WAN/NAT 条件。

双方严格窗口 complete，15 个确认 hash 到 1800 全部相同。但确认一致和约 59 Hz 不能抵消大停顿：

| direct c1 正文指标 | P1 | P2 |
| --- | ---: | ---: |
| logic Hz | 约 59.114 | 约 59.149 |
| 观察新状态 Hz | 约 59.075 | 约 58.945 |
| rAF 时间戳上的新状态间隔 p99 | 18.620 ms | 18.295 ms |
| rAF 时间戳上的新状态间隔 max | 369.685 ms | 472.630 ms |
| rAF 间隔 >50 ms / >100 ms / >250 ms | 1 / 1 / 1 | 1 / 1 / 1 |
| 整个 C++ callback wall max | 10.320 ms | 14.405 ms |

**这次 direct 仍未达到“不突然大卡”的目标。** 长间隔发生在已记录实际 RTC direct 通路的运行中，不支持把所有现象简单等同于独立 relay 的问题。后续双时钟核对已显示 P1 的信息等待与 P2 的 Page 观察空洞；这些证据仍不能指定 OS、GPU、RTC 丢包或某个函数原因，不能用较好的 p99 或平均 Hz 抹掉长间隔。

#### direct c1 最长跨度：rAF 时间戳与 Page 观察时钟不能混用

两份后续派生文件保留了原始端点与完整上下文：[gap-context](rollback-desktop-touch-both4-direct-c1-gap-context.json)和[gap-clocks](rollback-desktop-touch-both4-direct-c1-gap-clocks.json)。下表中的数字是原始 sample 索引，不是逻辑帧号。

| 最大值选择与端点 | rAF 时间戳间隔 | 同一对端点的 Page 观察间隔 |
| --- | ---: | ---: |
| P1 最大 rAF 新状态间隔，838→898 | 369.685 ms | 379.305 ms |
| P2 最大 rAF 新状态间隔，825→826 | 472.630 ms | 13.640 ms |
| P2 最大 Page 新状态观察间隔，822→825 | 18.180 ms | 489.755 ms |

**P1 在这段时间仍有回调运行，但新信息不足时没有推进新状态。** 838→898 跨度内保留的 60 个 callback 仍在运行；F=309、A=301（相差 8，记录中的 B=8）、scale=2 长时间不变，直到尾部 A 推进到 302 / 303 / 308。区间累计 Δcorrection=3、resimulation=16、WorldSave=11，单个 C++ rAF callback 墙钟最大 10.320 ms、合计 36.770 ms；图形 counter 仅增加一次新 presentation，texture upload、pixel read、program compile 与 GPU resample 未增加。它支持“此段限制推进、等待新信息”的描述，不证明 RTC 本身丢包。

**P2 的 rAF 最大值与 Page 最大值来自不同端点。** 825→826 的 rAF gap 为 472.630 ms，但 Page 观察只相隔 13.640 ms，next 从 302 到 306、presentation counter 增加 4，correction/resimulation/WorldSave 均不增加。不能把这个 rAF 数值直接当作同一对 Page 观察之间的实际空洞，更不能称四次物理画面呈现。

真正最大的 Page 新状态观察间隔是 822→825 的 489.755 ms，对应 rAF 间隔仅 18.180 ms。其中相邻 callback 样本 824→825 的 Page 观察相隔 479.530 ms，rAF timestamp 只增加 6.055 ms；当前 C++ rAF callback 的 ms 为 2.410 ms，Page sample 时间比本次 rAF timestamp 晚 473.975 ms。该相邻区间 F/A 从 301/301 到 302/302，B=0，无 correction/resimulation/WorldSave 增量，presentation counter 增加 1。它说明存在需要继续分段定位的 Page 观察时间空洞，不能由较小的 native ms 直接指定阻塞函数或原因。

本节 smoothness 的 Page 观察时点是 `m.started + samples[0]`：位于 audio 观察（本轮关闭）和 `play.frame` 之后、`updateInput` / metrics 等步骤之前。它**不是 JS callback 完成时间，也不是物理呈现时间**。rAF timestamp、这个 Page 观察时点以及整个 C++ callback 的 ms 是不同量，必须分别保留；不能把它们简称为同一种“样本完成时间”。这一定义也不覆盖第 4.1 节另一个 Launcher probe 的 child sample 记录。

当前支持的两端描述是“P1 等待新信息 / P2 另有较大 Page 观察空洞”。两页面仅按 timeOrigin 换算尚未独立同步校准，不能精确因果对齐并断言 P2 的该空洞就是 P1 等待的原因；也不足以指认 RTC 丢包、OS、GPU 或某个 JS 函数。



[direct c1 派生 evidence](rollback-desktop-touch-both4-direct-c1-evidence.json)为 114,773 B，SHA-256 24f43f33c1785a4821a0cacd798e4406dfd4fd09f8f35adacb7873ffe86a894f；离线命令 Ohxnn_5V145B5hr5O_kGKsrh，exit0、969 ms。direct 原始 report 为 24,198,823 B，SHA-256 50cebeb50ac38dc5bbc681a6cbe48d70fb892842301117107efb705317629570。

后续 gap-context 为 61,188 B，SHA-256 0339d5c59f4f88e728f2d23410b52f2120e09cbdaea03c95029f3e5e55486bea，命令 EBpbnbQC8xNFPF0oNjv8ttqB，exit0、654 ms；gap-clocks 为 120,735 B，SHA-256 5d84d8433680b4d2c7382d3e42cc9d8ef320d2c6dd6e78a404c5a5200c579260，命令 TZ04VoOdbG6DqhbltmZMjxhs，exit0、584 ms。上述毫秒均为离线提取耗时，不是游戏性能。


### 5.6 direct-raf-c1 已完成：已登记游戏 rAF 的进入前等待

desktop-touch-both4-direct-raf-c1 已完成，命令 yAAKft8om6WUU3TBr3_hbTM-，exit0、wrapper 64,763.2827 ms，sourceStable=true。保持同一冻结 driver 6d400d7906a6e3af1d988739f95ba534a0b28a8c64133aa475a5be22255a4ce5、同一 WASM 和双端 CPU4；启用既有 RAF_DIAGNOSTICS=1，profile、anomaly、ChromeTrace、LIVE_COST 关闭。两端实际 host↔host UDP direct、Intel UHD / D3D11 及相关身份有效，15 个 hash 到 1800 一致，严格窗口完整。诊断开销未校准，这不是与前轮的速度 A/B，也没有新增生产优化。

| direct-raf-c1 指标 | P1 | P2 |
| --- | ---: | ---: |
| logic Hz | 59.590349 | 59.575623 |
| 新状态 rAF Hz | 59.536064 | 59.486222 |
| 最大 rAF 新状态间隔 | 133.360 ms | 127.270 ms |
| >50 ms / >100 ms 间隔数 | 2 / 1 | 2 / 1 |
| 整个 C++ callback wall max | 18.690 ms | 13.370 ms |

人工触控负载有效：targetChanges=1,149 / 1,117，actual input frames=1,802 / 1,803，coalesced=454 / 538；graphics 记录 4,824 / 4,887 行有效。本轮仍为 native ABI 人工输入，不能升级为真实 Launcher 多人集成或手机验证。平均约 59.5 Hz 仍伴随两端各一次 >100 ms 间隔，不能宣称已经消除大卡顿；前轮 direct 的 489.755 ms Page 空洞与 relay c2 等坏样本继续保留。

#### 本轮最大间隔与相应请求

| peer / sample 端点 | rAF gap | Page 观察 gap | request / parent | 已登记且父返回后的合格等待 |
| --- | ---: | ---: | --- | ---: |
| P1，2497→2500 | 133.360 ms | 145.010 ms | 2539 / 2538 | 124.145 ms |
| P2，2502→2505 | 127.270 ms | 136.110 ms | 2687 / 2686 | 117.885 ms |

P1 的 request2539 在其 Page 时钟 enteredAt=27052.195 ms；相应当前 C++ callback 为 14.305 ms，JS frame 记录为 0.425 ms，F=A=923、B=0、scale=1，区间 correction/resimulation/WorldSave 增量均为 0。P2 的 request2687 在自己的 Page 时钟 enteredAt=22313.560 ms；当前 C++ callback 为 13.370 ms，JS frame 记录为 0.200 ms，F=924、A=922、B=2、scale=1，correction/resimulation 增量为 0、WorldSave 增加 1。JS frame 记录也不被改称全部主线程 CPU 时间。

这两条请求的 parent 已退出、没有取消、pause=0，相关循环 active、页面 visible；requests/hooks 完整，正文映射分别为 4,351 / 4,351 与 4,397 / 4,397。合格等待按各自 Page clock 从 max(requestedAt, parentExitedAt) 到 enteredAt 计算，证明的是**下一游戏 rAF 已经登记，但尚未进入回调的区间**，不是拿 rAF gap 减去 C++ ms 得到的残差。

这比仅有新状态长间隔提供了更具体的定位，但仍不足以指认 OS、GPU、RTC 或某个 JS 函数。两页时钟没有独立同步校准，不精确因果对齐；Page sample 观察边界继续采用第 5.5 节定义，不称 JS callback 完成或物理呈现。

#### 同一冻结 raw 的触控 timer 核对

[timer-context](rollback-desktop-touch-both4-direct-raf-c1-timer-context.json)仅重查现成 raw，没有启动新的浏览器。以下区间采用 touch-relative 毫秒，不能直接与上一表各自 Page 的 enteredAt 混算。

| 被核对的等待 | touch-relative 区间（ms） | 源计划与实际提交 | 可支持的结论 |
| --- | --- | --- | --- |
| P1 最大 124.145 ms | 15526.605→15650.750 | 无计划输入，也无提交 | 没有事件本来就应发生，不能据此判主线程停住 |
| P2 最大 117.885 ms | 15523.260→15641.145 | 仅末约 4 ms 的 15637 ms 到期 up + controls 两项，约 15656.390 / 15656.590 提交 | 没有连续 MOVE 覆盖，不能将整段等待解释为 timer 被阻塞 |
| P1 次大 67.705 ms，sample2529 / request2568 | 15848.110→15915.815 | due=8、coalesced=5；3 次 native touch 提交并完成 | 该等待内至少两个时点可运行 timer/native input，不是整段游戏 JS 持续占满 |

最后一行的原始提交区间为 contact 15887.225→15887.405、drag 15887.405→15887.405、drag 15906.905→15907.115。它只证明这些具体时点进入并完成了输入调用，不保证其它时刻主线程可用。**这属于次大的 67.705 ms 等待，不能挪作最大 124.145 ms 空档的归因证据。** 最大空档目前仍缺少连续源输入可用来判断主线程忙闲。

本轮原始 report 为 36,603,176 B，SHA-256 eab798213ba54bfd7b933aa8c5fc2560ec4a058555ae852146b140c947d2bbc5；[一般 evidence](rollback-desktop-touch-both4-direct-raf-c1-evidence.json)为 114,867 B，SHA-256 69c040896ed8d3f41afb831ea4e65c0890a3644476a76fe47872c632c55d12c6，命令 55tcVftEnnvkgeEW0hYgI5r7；[细分 timing](rollback-desktop-touch-both4-direct-raf-c1-timing.json)为 493,445 B，SHA-256 1a26e3ab69e0c0a6552542a55a61e331289afd5b3a83f432e5fffc3115881412，命令 AicY__im87CWKXX0yrw5YWQv。timer-context 为 93,838 B，SHA-256 cbd307c0faf4f195b5837433195e0636dd13c78e41b8a23764bc885e3e24a7b8，命令 BhUCADAL75_awjyiQAftRf76，exit0、610 ms；610 ms 是离线核对耗时。

### 5.7 direct-sched-c1 已完成：游戏窗口完整，scheduler trace 不合格

desktop-touch-both4-direct-sched-c1 的浏览器命令 fkCe5OH57vWQHzo3uP_TuTFg 已 exit0，wrapper 为 103,290.3618 ms，sourceStable=true。保持同一冻结 driver 6d400d7906a6e3af1d988739f95ba534a0b28a8c64133aa475a5be22255a4ce5、同一 WASM、双端 CPU4 与 direct-raf-c1 配置，只增加既有 CHROME_SCHED_TRACE=1；没有生产改动。两端实际 host↔host UDP direct、Intel UHD / D3D11 及身份有效，严格窗口完整，15 个 confirmed hash 到 1800 一致。末次 input buffer 字段双方均为 56，不能沿用前轮的 0，也不能由这个末次值推断整个运行中的缓冲或卡顿原因。

| direct-sched-c1 指标 | P1 | P2 |
| --- | ---: | ---: |
| logic Hz | 59.757528 | 59.716942 |
| 新状态 rAF Hz | 59.772742 | 59.719275 |
| rAF 新状态间隔 p99 / max | 18.325 / 121.175 ms | 18.310 / 109.070 ms |
| >50 ms / >100 ms 间隔数 | 1 / 1 | 1 / 1 |
| 整个 C++ callback wall max | 17.635 ms | 18.325 ms |

人工触控有效：targetChanges=1,204 / 1,243、actual input frames=1,805 / 1,805、coalesced=674 / 457，changedHistory 与 capture gaps 均为 0；graphics 4,591 / 4,850 行有效。它仍是 native ABI 人工输入，不代表 Launcher 多人集成或手机测试。平均约 59.7 Hz 与两端各一次 >100 ms 间隔同时成立，不能称流畅性修复通过；本轮诊断开销也未校准，不作速度 A/B。

#### 独立 rAF timing 仍有效

[本轮 timing](rollback-desktop-touch-both4-direct-sched-c1-timing.json)独立于 scheduler trace 判断资格。两端 RAF diagnostics 均有效、无 overflow，正文映射分别为 4,105 / 4,105 与 4,359 / 4,359。

- P1 最长间隔为 sample2915→2917，rAF=121.175 ms、Page 观察=124.050 ms；request2952 的 parent2951 已返回，合格已登记等待为 112.275 ms。相应当前 C++ callback=10.190 ms、JS frame=0.415 ms，correction/resimulation/WorldSave 增量均为 0，F=1078、A=1079、B=0、scale=1。
- P2 最长间隔为 sample2950→2951，rAF=109.070 ms、Page 观察=114.255 ms；request3084、parent3083，合格已登记等待为 103.735 ms。相应当前 C++ callback=10.095 ms、JS frame=0.225 ms，correction/resimulation 增量为 0、WorldSave 增加 1，F=1080、A=1077、B=3、scale=1。

这些是各自 Page 时钟中的已登记、父回调返回后至当前回调进入前的等待。它们可以继续支持“下一游戏 rAF 尚未派发”的定位，但不能借用下面不合格的 trace 进一步指定 OS task、GPU、RTC 或某个 JS 函数。Page sample 仍为第 5.5 节定义的观察边界，不是 callback 完成或物理呈现；双页面时钟也没有独立同步校准。

#### scheduler trace 的采集与覆盖失败

本轮固定 6 类 scheduler/compositor/GPU/UserTiming 事件，设置 350,000 条 / 96 MiB / 90 秒上限，没有系统 trace 或 CPU profiler。保留的两份 trace 如下；文件 SHA 在第 9.1 节逐份列出。

| trace | 保留 events | 保留事件首末时间戳跨度 | 文件大小 |
| --- | ---: | ---: | ---: |
| [P1 scheduler trace](../../th09_web/artifacts/multiplayer-tests/desktop-touch-both4-direct-sched-c1-P1-scheduler-trace.json) | 159,486 | 7.910054 s | 25,668,050 B |
| [P2 scheduler trace](../../th09_web/artifacts/multiplayer-tests/desktop-touch-both4-direct-sched-c1-P2-scheduler-trace.json) | 168,739 | 8.183941 s | 26,982,597 B |

两端虽然没有 local overflow，但都出现 tracing-complete-timeout 和 CDP detach timeout；tracingCompleteReceived=false、dataLoss=null。Page 侧已有 capture-stop，而 trace 内没有相应停止标记；clockMapped=false、collectionValid=false、activeWindowCovered=false。正式 active 窗口约 30.2 秒，上表仅为已保留事件自身的首末跨度；由于没有合格时钟映射，不能称它们已经覆盖正式活动的前约 8 秒，更不能推成全程覆盖。早期两个 marker 不能替代缺失的停止标记或放宽资格。

bufferUsage 最后长期停在 0.996245，而当前 guard 仅在值 >=1 时标记，因此 chromeBufferFull=false。该 false 与没有 local overflow 都不证明 Chrome 采集无损；dataLoss 仍为未知。现有事实也不足以断言 Chrome buffer 已经填满，不能把“接近上限”改写成确定丢失机制。

因此，**浏览器 exit0、完整回滚窗口和有效 rAF 请求证据，与 scheduler trace 不合格必须分开保留**。这两份 trace 不是 CPU/GPU 根因证据，也不是修复通过证据。这一失败后续促成第 5.8 节的诊断修正；旧 raw / trace / evidence 不重写，也不因新修正追认本轮资格。

本轮 [原始 report](../../th09_web/artifacts/multiplayer-tests/desktop-touch-both4-direct-sched-c1-report.json)为 36,168,002 B，SHA-256 a24d75bc216ee2a20360ea7bf3e63134adbc1657fd7ced5801108770c924332e。[一般 evidence](rollback-desktop-touch-both4-direct-sched-c1-evidence.json)、[trace metadata](rollback-desktop-touch-both4-direct-sched-c1-trace-metadata.json)、[trace limits](rollback-desktop-touch-both4-direct-sched-c1-trace-limits.json)与独立 timing 的完整身份和真实命令分别见第 9.1 / 9.2 节，均保留原始失败与未知字段。

### 5.8 scheduler 采集修正后的 tasks-c1：trace 合格，长间隔仍在

针对第 5.7 节的无效 trace，诊断代码已作窄修正。默认 pipeline 仍固定 6 类；新增 tasks profile 固定为 toplevel、renderer.scheduler、blink.user_timing 三类。tasks 不采集 compositor/GPU 详细类别，其 coverage 记为 null，不能解释为这些工作没有发生。高水位阈值改为 0.80，触及后结束采集并保留 censored；旧的完整性、时钟映射和 timeout 门槛没有放宽。

结束阶段改为：测量 ended 后先暂停两端，等待两端 trace stop 阶段完成，再串行 serialize/write，最后进行大 sample export。这样避免快端的大量同步写出先阻塞慢端的停止处理；这项收尾修正没有改变活动游戏中的预测、输入或生产代码，也没有把此前的 trace 失败追认为有效。

主代理已串行通过 4 项 syntax 检查，以及 24 项 VM/mock 测试（19 项 helper + 5 项 finalizer）。命令 4TxAHMWKfvwqKx82SUI-BNhR exit0，882 ms 为检查耗时，不是游戏耗时或流畅性成绩。[preflight JSON](../../th09_web/artifacts/multiplayer-tests/desktop-scheduler-tasks-c1-preflight.json)为 2,903 B，[preflight log](../../th09_web/artifacts/multiplayer-tests/desktop-scheduler-tasks-c1-preflight.log)为 3,724 B；完整源码、测试和证据 SHA 在第 9.1 节保留。新的 smoothness driver 为 731 行 / 70,664 B，SHA-256 46b7e954f03c5cc2d6f9ccf9b35db29187d3fa9529dac7c541393c21c14ec7c5。

#### tasks-c1 的完整窗口与实际路径

desktop-touch-both4-direct-tasks-c1 已完成，命令 bYeqEk9PlFxULeh4OhGQOX-X exit0，wrapper 为 59,588.7703 ms，sourceStable=true，实际执行 driver 保持上述 46b7e954f03c5cc2d6f9ccf9b35db29187d3fa9529dac7c541393c21c14ec7c5。两端 CPU4、桌面 native ABI 人工触控、tasks 三类、FRAMES=1200，正文 180→1200。实际 nominated host↔host UDP direct、Intel UHD / D3D11 与 core/canvas/context owner 均合格；10 个 confirmed hash 到 1200 相同，严格窗口两端 complete。

| direct-tasks-c1 指标 | P1 | P2 |
| --- | ---: | ---: |
| 新状态 rAF Hz | 59.4064721542 | 59.4648802693 |
| logic Hz | 59.4708493720 | 59.4590970962 |
| rAF 新状态间隔 p99 / max | 18.320 / 163.595 ms | 18.325 / 157.535 ms |
| >50 ms / >100 ms / >250 ms 间隔数 | 1 / 1 / 0 | 1 / 1 / 0 |
| 整个 C++ callback wall max | 16.705 ms | 13.140 ms |
| 正文 callback 数 | 2,755 | 2,781 |

人工触控有效：targetChanges=808 / 802、actual input frames=1,204 / 1,204、coalesced=228 / 189，history change / capture gaps 均为 0。计划 120 Hz 不改称实际到达率。这是桌面 native ABI 刺激，没有手机测试，也不是 Launcher 多人全链路验证。缩短窗口与新观察配置不能作为性能改善 A/B；两端各一次 >100 ms 间隔仍不符合无大卡顿的目标，没有新的生产 C++ 改动。

#### tasks trace 资格通过，详细类别范围保持明确

本轮两端 collectionValid、clockMapped、allThreeMarks、activeWindowCovered 均为 true；收到 TracingComplete，dataLossOccurred=false，未出现 overflow、采集 error、高水位结束、deadline 或 censored。这与旧 direct-sched-c1 的失败分开记录，没有回写或删去旧 trace。tasks 未采集 compositor/GPU 详细类别，对应 coverage=null；toplevel 可以保留 GPU Process 线程的普通任务，但实际后端身份或线程名称都不等于详细 GPU 运算时间线。

[一般 evidence](rollback-desktop-touch-both4-direct-tasks-c1-evidence.json)与[trace metadata](rollback-desktop-touch-both4-direct-tasks-c1-trace-metadata.json)已经保留资格结果，现已具备同一正式窗口的任务分析输入。**采集完整不是具体任务归因，也不是流畅性通过**；第 5.9 节追加同窗任务与其它线程分析；其中的时间相关性仍不能直接判定某个 task、OS、GPU 或 RTC 是原因。

#### 同窗 rAF 请求等待

[本轮 timing](rollback-desktop-touch-both4-direct-tasks-c1-timing.json)保持完整正文链接：P1 为 2,755 / 2,755，P2 为 2,781 / 2,781。

- P1 最大间隔 sample1678→1680：Page 观察=171.845 ms、rAF=163.595 ms；request1709、parent1708，合格已登记等待=158.005 ms。进入后的 C++ callback=11.030 ms、JS frame=0.395 ms，F=A=622、B=0、scale=1，correction/resimulation/WorldSave 增量均为 0。
- P2 最大间隔 sample1695→1697：Page 观察=167.105 ms、rAF=157.535 ms；request1817、parent1816，合格已登记等待=148.520 ms。进入后的 C++ callback=10.445 ms、JS frame=0.395 ms，F=623、A=621、B=2、scale=1，correction/resimulation 增量为 0、WorldSave 增加 1。

等待继续采用各自 Page 时钟下已登记、父回调返回后至下一回调进入的资格口径，不是从间隔减 C++ 耗时得到的残差。Page 观察并非 callback 完成或物理呈现，两页时钟也没有独立同步校准。同窗任务寿命区间与限定结论在第 5.9 节另列，不把已登记等待直接改称 CPU 或 GPU 计算成本。

[原始 report](../../th09_web/artifacts/multiplayer-tests/desktop-touch-both4-direct-tasks-c1-report.json)为 29,685,068 B；[P1 trace](../../th09_web/artifacts/multiplayer-tests/desktop-touch-both4-direct-tasks-c1-P1-scheduler-trace.json)为 37,130,407 B，[P2 trace](../../th09_web/artifacts/multiplayer-tests/desktop-touch-both4-direct-tasks-c1-P2-scheduler-trace.json)为 36,020,016 B。一般 evidence 为 114,346 B，metadata 为 34,283 B，timing 为 478,502 B，完整 SHA 与命令见第 9.1 / 9.2 节；773 ms 是 timing 的离线提取耗时。

### 5.9 同窗任务分析：GPU 进程任务与等待高度重叠，尚未确认瓶颈

主代理已对同一冻结 tasks-c1 记录完成 [task-context](rollback-desktop-touch-both4-direct-tasks-c1-task-context.json)、[task-review](rollback-desktop-touch-both4-direct-tasks-c1-task-review.json)与 [thread-context](rollback-desktop-touch-both4-direct-tasks-c1-thread-context.json)离线分析，没有新增浏览器运行。这些结果承接本轮合格时钟映射与 renderer PID/TID，仍逐页使用各自 Page clock，不做未经同步校准的两页精确因果对齐。

#### 主线程已保留任务区间的覆盖

对闭合主线程 X 事件的寿命区间去重，并传播 offset 误差后，最长等待中的覆盖上下界如下。RunTask 是其中另列的已知任务集合。

| peer | 合格 rAF 等待 | 闭合主线程 X 覆盖上下界 | 已知 RunTask 数 | RunTask 覆盖上下界 |
| --- | ---: | ---: | ---: | ---: |
| P1 | 158.005 ms | 7.165–15.646 ms | 20 | 7.165–15.285 ms |
| P2 | 148.520 ms | 4.780–13.816 ms | 18 | 4.780–13.222 ms |

这些仅是已保留任务寿命区间的覆盖，不是 CPU busy 时间；未覆盖的部分也不能算主线程 idle，不能拿等待减去覆盖值生成确定的阻塞成本。两端最长等待内均没有计划触控 due，也没有提交。因此这两段不能证明连续移动压力已经过关，不能仅凭没有输入事件判断线程是否卡住。

#### GPU Process / CrGpuMain 的强时间相关

额外读取的是当前 trace 已保留的 toplevel 其他线程事件，没有启用新的 GPU 类别。两端最长 rAF 等待分别与 GPU Process 的 CrGpuMain 线程中的一条长 RunTask 高度重叠：

| peer | GPU Process PID / CrGpuMain TID / event | RunTask wall | 与 rAF 等待重叠的上下界 | rAF 进入（本页 ms） |
| --- | --- | ---: | ---: | ---: |
| P1 | 30668 / 31528 / 116985 | 165.527 ms | 157.395–157.804 ms | 20946.880 |
| P2 | 61176 / 21216 / 117215 | 154.916 ms | 148.058–148.448 ms | 16279.145 |

传播时钟 offset 误差后的 Page 映射保留为区间：P1 任务起点 20780.743–20781.152 ms、终点 20946.270–20946.679 ms；P2 起点 16123.767–16124.157 ms、终点 16278.683–16279.073 ms。原始 tdur 字段分别为 588 / 613 μs，仅按记录字段保留，不将其与 wall 相减后改称已证明的某种等待。

这是值得继续验证的新方向，但**高度时间重叠不等于已确认 GPU 瓶颈**。仅凭 toplevel RunTask，无法区分 GPU API 等待、驱动行为、系统调度或真实 GPU 运算；165.527 / 154.916 ms 是任务 wall 寿命，不是 GPU 计算时间。tasks profile 的 gpu/compositor 详细 coverage 仍为 null，不能由线程名称补出未采集的详细时间线。

#### 终态 selector 更正与实际暂停

task-context 初稿的终态 selector 误用 c.at，得到空数组；task-review 明确改用 controls.enteredAt。原先 artifact 保留，这一终态提取更正不影响原始 body、trace 或主任务分析，不能把初稿空数组当作没有暂停证据。

独立核验的最后 pause=1 调用为 P1 30650.225→30650.400 ms、P2 25982.140→25982.315 ms，均为各自 Page 时钟。此后没有 control、onGameFrame 或新增 sample，hooks 有效且无 overflow。本轮未见恢复撤销；ended 后、pause 前的尾部回调不算复活，不因此把本轮测量判为无效。

task-context 为 877,454 B；task-review 为 21,006 B；thread-context 为 290,813 B，完整 SHA 与命令见第 9.1 / 9.2 节。629 ms / 998 ms 分别是两项离线复核耗时，不是游戏成绩。下一步围绕 GPU 进程任务与呈现链路继续验证，并补上最长等待内真正持续变化的人工触控源；第 5.10 节记录已实现的诊断/刺激和预检，实际新运行仍待结果，不宣布根因或修复通过。

tasks-c1 的冻结 raw flags 审计另由命令 EHE8M4wD-OlEA1N01IxHjsgp exit0 完成：审计 allowed 列表虽然包含 frame/vsync 开关，但实际两端均没有 disable-frame-rate-limit 或 disable-gpu-vsync；实际保留三项 background-throttling 相关 flags，后端为 Intel UHD / D3D11。允许列表不等于实际启用，不能以未使用的开关解释本轮长等待。

### 5.10 continuous / graphics：正文保留完整，实际运行收尾整体失败

为补足最长等待内没有计划触控 due 的覆盖缺口，已增加 opt-in continuous 刺激分支并接入 TOUCH_PATTERN 选择。源计划 warmup=2500+137×side ms，之后每 1200 ms 一条 leg，8 leg / 9600 ms 为周期，在四个点之间往返。MOVE 使用独立 Page 时钟，计划中没有中途 hold/up 的静止空窗；controls 后在同一 deadline 立即重写 axes，保留控制边界，只有同 gesture/leg 的连续 MOVE 可 coalesce。最大 20,000 事件与每次 wake 64 的限制没有放宽；duration 尾部仍计划 UP/zero→cancel→neutral。

这些刺激计划规则本身不能替代实际源事件、提交、capture 或游戏流畅性的证据。默认 lifecycle plan 的 JSON 保持兼容，install 原函数所在后缀未改；与同一份冻结 raw 内 P1/P2 两份计划的 SHA 比对均一致（2/2）。本轮 continuous 计划 P1 为 14,301 events、P2 为 14,382 events，完整计划 SHA 见第 9.1 节；future-censored、due、coalesced 和未提交项继续如实保留，尾部动作也不预称已在本轮执行。

新的 graphics profile 只收 gpu、viz、blink.user_timing 三类，不采 toplevel、renderer.scheduler 或 cc。scheduler/compositor detailed coverage 均为 null；gpu 覆盖按实际 events 判断，viz 按 categories.viz 记录。caps、三个 marker、高水位、clock、complete / dataLoss 资格门槛保持，本轮实际事件和字节更多，不能声称采集量或观察开销改善。这个 profile 为检验 GPU 进程任务/呈现链路提供诊断能力，不等于已得到 GPU 根因。

主代理 preflight 命令 yE4s0_iwoiomugAPuNx_tVwE exit0，6 项 syntax + 38 项测试通过（touch12 / helper21 / finalizer5），sourceStable；它是测试与兼容性验证，不是游戏性能 PASS。[preflight JSON](../../th09_web/artifacts/multiplayer-tests/desktop-continuous-graphics-c1-preflight.json)为 9,628 B，[log](../../th09_web/artifacts/multiplayer-tests/desktop-continuous-graphics-c1-preflight.log)为 5,528 B。driver、触控计划 helper/test、trace helper/test 的新身份及未改 finalizer fixture 的身份在第 9.1 节按版本分列。

desktop-continuous-touch-both4-direct-graphics-c1 已 exit1，命令 ZZ-JwYZ8nCcUVmMnZHCW_-w_，wrapper 86,641.0887 ms，sourceStable=true，执行 driver 仍为 2998a975240915b1ceb7a4dc015cf0471328ed9e9565cf14eea52f83986c634c。预声明双端 CPU4、同一 WASM、D0 / 回滚历史 8、FRAMES=1200 / 正文 180→1200，TOUCH_PATTERN=continuous、source120Hz、duration=120000 ms，graphics 三类，其余 hostLoad / RAF / RENDER_WORK 保持。新 stimulus 与新 profile 不能称 matched A/B；没有手机测试或新增生产代码。

失败为 driver 第 653 行的 RTC selected candidate evidence unavailable，发生在双方 1200 帧测量结束、pause、约 15.5 秒 trace drain 与大 export 之后。失败快照中 P1 的 m.closed 保留 Confirmed peer input stopped advancing，live stats 已清 0。旧 measure 尚保留 3,209 / 3,280 samples、ended 约 20.109 / 20.093 s、最后 next=1201 / confirmed=1200；主代理后续已从这些冻结数据核实完整正式 body 和 10 个相同 confirmed hash，结果见下文；driver 当时在 hash 断言前退出。本轮整体没有通过，不能将测试 label、配置或默认 flags 当作 actual direct 的证据，也不能先将收尾失败归因为正文回滚卡顿。

两端 graphics trace 的 collection、clock、all3marks、activeWindowCovered 为 true，dataLoss=false，无 censor / overflow；独立 input valid 两端为 true。完整正式正文、输入分母、请求等待与提交/呈现区间的离线证据分列如下。各项局部资格不替代整体 exit1 或缺失的 actual RTC direct 证据，持续 planned source 也不保证每帧模拟接受。

[本轮 failure raw](../../th09_web/artifacts/multiplayer-tests/desktop-continuous-touch-both4-direct-graphics-c1-failure.json)为 71,766,293 B，SHA-256 9c7ffe42fab18919260f6a9811d1c143e1992138957a46b667c08b7dfdc64812；原始失败完整保留，后续分析不能以新摘要覆盖或删除这次 exit1。

#### 正式正文与输入：离线核实有效，整体 exit1 保持

主代理使用冻结 failure raw 派生的[一般 evidence](rollback-desktop-continuous-touch-both4-direct-graphics-c1-evidence.json)确认，正文 180→1200 两端完整，10 个按顺序排列的 confirmed hash 到 1200 相同。这是离线派生核验；driver 在执行 hash 断言之前已经失败，不能将其改写为 driver 整轮通过，也不补填缺失的 actual RTC direct 证据。

| continuous / graphics c1 正文指标 | P1 | P2 |
| --- | ---: | ---: |
| logic Hz | 60.0229323 | 59.9976883 |
| 新状态 rAF Hz | 59.8986670 | 59.9362603 |
| 新状态间隔数 n | 1,017 | 1,018 |
| rAF 新状态间隔 p99 / max | 18.335 / 36.365 ms | 18.315 / 36.370 ms |
| >25 ms 间隔数 | 3 | 2 |
| >50 / >100 / >250 ms 间隔数 | 0 / 0 / 0 | 0 / 0 / 0 |
| 正文 callback 数 | 2,735 | 2,786 |
| 整个 C++ callback wall max | 17.745 ms | 9.765 ms |

| 人工触控记录分母 / 活动 | P1 | P2 |
| --- | ---: | ---: |
| planned events | 14,301 | 14,382 |
| due / submitted | 2,145 / 1,704 | 2,141 / 1,854 |
| coalesced | 439 | 285 |
| actualInputFrames | 1,205 | 1,205 |
| analogActiveFrames | 990 | 884 |
| targetChanges / positionChanges | 982 / 1,061 | 879 / 1,049 |
| changedHistory / captureGaps | 0 / 0 | 0 / 0 |

planned、due、submitted、coalesced 与实际活动帧分别保留，不能称每帧 active，也不把源计划 120 Hz 当作实际输入到达率或模拟接受率。本轮正文没有 >50 ms 新状态间隔，但观察 profile 和刺激都变了，整体又缺 actual route 资格；不能据此宣布性能 A/B 提升、旧大卡顿已经解决或完整验收通过。

#### 合格等待与 SwapBuffers 的短区间相关性

[本轮 timing](rollback-desktop-continuous-touch-both4-direct-graphics-c1-timing.json)中的最大合格已登记等待为 P1 23.350 ms（26984.560→27007.910，sample2722），进入后 C++ callback=11.910 ms，correction/resimulation 增量=1/1、WorldSave=2；P2 为 29.515 ms（22636.110→22665.625，sample2779），进入后 C++ callback=8.370 ms，correction/resimulation 增量=1/1、WorldSave=2。它们是各自 Page clock 下的请求等待，不能与另一种新状态 gap 指标混为一谈。

[trace-context](rollback-desktop-continuous-touch-both4-direct-graphics-c1-trace-context.json)确认两端 collection、clock、all3mark、activeWindow 和 sourceStable 有效，无 overflow / censor / deadline / dataLoss。gpu/viz 实际已有事件；未请求完整 scheduler/compositor 类别，二者 coverage 仍为 null。当前已知的 SkiaOutputSurfaceImplOnGpu::SwapBuffers 区间如下：

| peer / 对应等待 | PID / TID / event | SwapBuffers wall | 与等待重叠上下界 | 该等待内 due / submitted |
| --- | --- | ---: | ---: | ---: |
| P1 第二长等待 22.665 ms，sample2654 | 59232 / 61184 / 139525 | 23.967 ms | 20.191–20.566 ms | 3 / 2 |
| P2 最大等待 29.515 ms，sample2779 | 49772 / 60828 / 143724 | 25.071 ms | 24.820–25.050 ms | 6 / 4 |

P1 的 SwapBuffers 映射 Page 起点为 26538.944–26539.319 ms、终点为 26562.911–26563.286 ms，对应的是 26542.720→26565.385 ms 的第二长等待。**P1 最长的 23.350 ms 等待不是这一条 SwapBuffers，不能合并成同一个原因。** P2 映射起点为 22635.859–22636.089 ms、终点为 22660.930–22661.160 ms，与本页最大等待重叠。

这些证据增加了短停顿与提交/呈现路径的相关性；SwapBuffers 的 wall 不等于 GPU 硬件执行时长。本轮没有重现、也没有解释前轮约 150 ms 的长等待；两页时钟未独立同步校准，不能精确跨页因果对齐。graphics profile 实际事件和字节更多，不能说它更低开销，更不能把这些不同条件的运行当成 A/B 改善。

一般 evidence 为 105,635 B，timing 为 478,705 B，trace-context 为 83,958 B；P1/P2 trace 分别为 45,036,852 / 45,052,301 B。完整 SHA 和命令在第 9.1 / 9.2 节保留，旧 failure raw 不改。

#### 收尾失败链与已通过预检的 harness 端点修正

源码复核确认：pause 不停止 SharedNetplay 的 16 ms Pump；长约 15.5 秒的 trace drain 期间仍可触发 timeout→close→network_end→rollback.Clear / published=0，随后 P1 stats=0、peer=[]。这解释了为何在 drain/export 后读取 live 端点会失去已结束测量所需的证据；不能由此清空后的字段回推正文路由，也不能给本轮补填 actual direct。

harness 端点修正已实现：在 trace drain 前开始保存小型原生端点与异步 RTC 摘要，并与 trace stop/drain 并行。原生读取和 RTC 异步返回不是同一原子时刻，分别保留捕获信息；资格要求 PC connected、input/control 均 open，且 capture 身份在开始与完成时一致。begin/collect/qualify 与原 finish 阶段分开，完成 collect 后才作大导出和资格判断，没有修改生产 C++，原 trace finish 函数代码保持不变。

报告保留 postDrain 的当前 stats、peerCount、closed、lateClosed，并独立报告 teardownClean；历史 selectedPath 不冒充当前连接状态。passed 必须结合 passScope 解读为其明确的测量范围，teardownClean 的检查范围仅到 browser disposal 前，不能扩称整个浏览器清理已通过。

主代理 preflight 命令 dumBb-uYiaj9H0WSWBfZ7bIW exit0，6 syntax + 51 tests（touch12 / helper21 / finish18）PASS、sourceStable。test duration=492.7934 ms 是测试耗时，不是游戏性能；默认旧计划与同一份冻结 raw 内 P1/P2 计划的 SHA 仍为 2/2 一致。新 driver 为 805 行 / 79,424 B，finish fixture 为 18 tests / 18,277 B；[preflight JSON](../../th09_web/artifacts/multiplayer-tests/desktop-continuous-graphics-c2-preflight.json)9,654 B、[log](../../th09_web/artifacts/multiplayer-tests/desktop-continuous-graphics-c2-preflight.log)7,128 B，完整 SHA 见第 9.1 节。

端点修正随后已在 direct graphics c2 实测中验证：测量与历史路由资格通过，trace-drain 晚尾 teardownClean=false 仍保留，详见第 5.11 节。连续 relay39 c1 的正文、端点和 relay 资格也已通过，详见第 5.12 节；新增 burst160 也已完成，测量资格通过但仍有 60.505 / 90.335 ms 间隔，性能目标未达标，详见第 5.13 节。旧 c1 failure 不改，没有新的生产 C++ / build 变更，没有手机测试。

### 5.11 continuous direct graphics c2：测量与历史端点通过，trace drain 晚尾仍不 clean

主代理命令 YHeNbCuxro3h3ICBtzB5C3uE exit0，wrapper 62,994.7504 ms、sourceStable=true。使用冻结的 23790f4409b8ec835d47f21c1d8293280acd37f084d1ba8b7d94d6a1b04bf1ff 版 driver，WASM 未变；两端 CPU4、continuous 人工触控、D0 / history8、1200 帧及 graphics profile。**本轮 PASS 的范围是正式测量与捕获时段的历史 actual route，trace drain 之后 teardownClean 仍为 false。** 不能把 exit0 扩写成连接始终有效、完整 disposal clean 或生产网络 timeout 已修复。

正式 body180→1200 两端 complete，10 个 ordered confirmed hashes 到 1200 全部相同。两端捕获开始及 RTC query 完成时的 owner / peer / PC 身份一致，PC connected 且 input/control open，测量端点各项 qualification=true；当时 actual selected path 为 host↔host UDP direct。实际 GPU 均为 Intel UHD / D3D11。原生读取与异步 RTC 摘要不是同一原子时刻，这些资格支持的是记录下来的捕获时段，不能替代 drain 后的当前状态。

| 正式窗口指标 | P1 | P2 |
| --- | ---: | ---: |
| logic Hz | 60.00031765 | 59.99299494 |
| rAF 新状态 Hz | 60.00566491 | 59.98069316 |
| 新状态间隔数 n | 1,019 | 1,019 |
| rAF 新状态间隔 p99 / max | 24.000 / 24.150 ms | 23.560 / 24.070 ms |
| Page 新状态观察间隔 max（后续九组汇总） | 30.425 ms | 27.755 ms |
| rAF >25 / >50 / >100 / >250 ms 间隔数 | 0 / 0 / 0 / 0 | 0 / 0 / 0 / 0 |
| 整个 C++ callback wall max | 14.110 ms | 10.660 ms |

略高于 60 的窗口比值来自采样边界，不表示逻辑跑得更快，也不是物理呈现速率。**回滚负载明显不对称：P1 整段累计仅 1 correction / 1 resimulation，P2 则为 955 / 1,582。** P1 正文的 1,019 个普通前进 callback 全部 WorldSave=0，中位耗时 2.200 ms；P2 普通前进子集 n=218、全部 WorldSave=1，中位耗时 2.840 ms。这些都是整 callback wall，不是纯 tick CPU 或独立恢复成本。P1 的平滑结果不能作为深回滚验收；本轮没有新生产优化，也不是匹配实际输入的性能 A/B。

P1 正文 missingA=0、backlog=0、networkScale 全为 1。P2 信息缺口 B 中位数 1 / p99=2 / max=2，backlog=0、networkScale 全为 1。低信息缺口与 P1 几乎没有重演共同限定了本轮负载，不能用这一轮抹去前述深负载及长等待反证。

| 人工触控记录分母 / 活动 | P1 | P2 |
| --- | ---: | ---: |
| planned events | 14,301 | 14,382 |
| due / submitted | 2,149 / 1,933 | 2,148 / 1,841 |
| coalesced / notSubmitted | 214 / 2 | 306 / 1 |
| actual captures | 1,202 | 1,203 |
| analogActiveFrames | 939 | 869 |
| targetChanges / positionChanges | 928 / 1,065 | 862 / 1,042 |
| changedHistory / captureGaps | 0 / 0 | 0 / 0 |

两端 touch valid；仍分别保留 planned、实际提交和游戏捕获，不能称每帧 active，也不能把计划 120 Hz 当实际到达率。host 全轮 mean busy=25.0658%，24 个逻辑 CPU，只是宿主概况，不解释某个间隔的原因。

两端 graphics trace 的 collection / clock / all3marks / active coverage 有效，无 overflow/error，GPUobserved=true；scheduler/compositor 详细 coverage 仍为 null。P1/P2 trace 大小为 41,657,328 / 41,245,968 B。本轮未再次提取完整 GPU X-event 上下文；没有长 gap 的这次运行先用于端点验证，不能因此宣称 GPU 原因已排除、采集开销更低或呈现链路已修复。

#### 捕获端点与晚尾状态分开保留

原生历史端点八词原样为 P1 [1202,1202,1,1,1202,1,1,1202]，P2 [1203,1201,955,1582,1203,2,1,1201]；其中 stats[5] 是 repairsSent，不是 wait。以下时间均属于各自 Page clock，没有跨页同步校准：

| 时刻 / 晚尾字段 | P1 | P2 |
| --- | --- | --- |
| 原生 capture 开始→完成 | 29172.150→29172.545 ms | 25005.275→25005.490 ms |
| RTC query 完成 | 29174.970 ms | 25008.110 ms |
| postDrain 观察 | 44233.555 ms | 40045.125 ms |
| postDrain 当前 stats | 八词全 0 | [1203,1202,955,1582,1203,2,1,1201] |
| 当前 peer / transport | peerCount=null，transport 不存在 | peerCount=1 |
| 当前连接字段 | connected=false | connected=true，同时 disconnected=true |
| closed / lateClosed | 两者均记录 Confirmed peer input stopped advancing | closed=[] |

约 15 秒 drain 后，P1 已丢失当前 transport 和 live stats；P2 的 connected/disconnected 原始字段也如实并列，不能替换成一个自行推断的状态。**teardownClean=false 保留，检查范围仍只到 browser disposal 前。** 这次真实晚尾失败同时验证了小端点修正能够保住测量和当时路由证据；它没有修复生产网络 timeout，也不证明整套清理已通过，更不能反向为 c1 缺失的 actual route 补证。

[本轮 raw](../../th09_web/artifacts/multiplayer-tests/desktop-continuous-touch-both4-direct-graphics-c2-report.json)为 31,015,079 B；[general evidence](rollback-desktop-continuous-touch-both4-direct-graphics-c2-evidence.json)为 116,463 B，离线命令 8mvV93LXgD-mkvzPT2t3bMGI exit0、825 ms。该耗时只是摘要提取。raw、派生与两份 trace 的完整 SHA 见第 9.1 节，运行命令见第 9.2 节。

### 5.12 continuous relay39 c1：双方回滚负载下无 >50 ms 正文间隔，约 58.5 Hz 仍需解释

主代理命令 rGHvkV4h1mV_z_rKdPk4IsRl exit0，wrapper 45,813.139 ms、sourceStable=true；driver 仍为 23790f4409b8ec835d47f21c1d8293280acd37f084d1ba8b7d94d6a1b04bf1ff，冻结 WASM 未变。两端桌面 CPU4、同 continuous 120 Hz 源计划、1200 帧、D0 / history8；独立 relay 注入单程 39±5 ms，保留 RAF_DIAGNOSTICS / RENDER_WORK / HOST，不启用 GPU trace。与 direct graphics c2 的网络、观察条件不同，不能把两轮差值归因于传输方式或某项生产优化。

两端正式 body180→1200 complete，10 个 ordered confirmed hashes 到 1200 相同，端点各项 qualification=true，relayTransport.valid / exercised 均通过。**这里 qualification 中的 rtc 字段按 fallback 语义表示该条件不适用，不是 RTC 通过或 direct 证据。** 两端 teardownClean=true 只覆盖既定 browser disposal 前的检查。实际 GPU 均为 Intel UHD / D3D11。

| 正式窗口指标 | P1 | P2 |
| --- | ---: | ---: |
| logic Hz | 58.54105364 | 58.54537137 |
| rAF 新状态 Hz | 58.52784941 | 58.57547211 |
| 新状态间隔数 n | 1,019 | 1,019 |
| rAF 新状态间隔 p99 / max | 30.545 / 38.160 ms | 24.580 / 31.025 ms |
| >25 ms 间隔数 | 12 | 10 |
| >50 / >100 / >250 ms 间隔数 | 0 / 0 / 0 | 0 / 0 / 0 |
| 整个 C++ callback wall max | 18.680 ms | 12.025 ms |

双方确有回滚工作：历史原生端点为 P1 [1209,1208,860,2019,1209,0,1,1207]，P2 [1211,1207,911,3799,1211,0,1,1207]，对应累计 correction=860 / 911、resimulation=2,019 / 3,799。它们不是单次回滚深度，负载也仍不完全对称。普通前进子集 n=308 / 259、全部 WorldSave=1，整 callback wall 中位数为 3.010 / 2.640 ms；不能把这些数字改称纯 tick 或恢复 CPU 成本。

信息缺口 B 的中位数 / p99 / max 为 P1 2 / 4 / 5、P2 4 / 5 / 6。两端 backlog 的 p99=0、max=1；P1 nativeNetworkScale 全为 1，effective scale p50=0.998 / max=1；P2 nativeNetworkScale p50=1 / p99=1.28 / max=1.36，effective scale 的 p99 / max 相同。**nativeNetworkScale 是 interval multiplier，>1 表示延长步长、减速，不是更快。** 本轮看到了网络节拍调节生效，但不能把约 58.5 Hz 与 60 Hz 的全部差距归因网络；P1 无新增 resimulation、WorldSave=1 的普通前进子集中也有 18.680 ms 整 callback wall。

| 人工触控记录分母 / 活动 | P1 | P2 |
| --- | ---: | ---: |
| planned events | 14,301 | 14,382 |
| due / submitted | 2,213 / 1,681 | 2,206 / 1,653 |
| coalesced / unsubmitted | 529 / 3 | 551 / 2 |
| actual input frames | 1,209 | 1,211 |
| analogActiveFrames | 1,007 | 877 |
| targetChanges / positionChanges | 991 / 1,077 | 865 / 1,054 |
| changedHistory / captureGaps | 0 / 0 | 0 / 0 |

两端 touch valid，源计划持续变化、提交与实际活动帧分别保留；不是每帧 active 或真实 120 Hz 输入已经达成。host 全轮 mean busy=24.4229%、24 个逻辑 CPU，不提供逐 gap 因果解释。本轮双方有回滚负载且正文没有 >50 ms 间隔，是一个合格的有利样本；仍不是稳定 60 Hz、全部网络条件下流畅或整体目标已经完成的证据，前轮坏样本不变。

[本轮 raw](../../th09_web/artifacts/multiplayer-tests/desktop-continuous-touch-both4-relay39-c1-report.json)为 32,241,884 B；[general evidence](rollback-desktop-continuous-touch-both4-relay39-c1-evidence.json)为 109,464 B，离线命令 imQ5u6J7pBTW8-Lp9Jnd-IlT exit0、609 ms。完整 SHA 与命令见第 9.1 / 9.2 节。

后续 desktop-continuous-touch-both4-relay39-burst160-c1 已完成：burst 确实覆盖双方正式发送帧，测量资格通过，但仍有 60.505 / 90.335 ms 最大新状态间隔。它不把 normal relay 的有利样本变成全网络条件验收；注入边界、完整分母和下一步见第 5.13 节。

### 5.13 continuous relay39 burst160 c1：测量通过，60 / 90 ms 间隔仍未达到流畅目标

主代理命令 CTKMEhQXcIY8YVoEUeJLpvMe exit0，wrapper 46,691.2372 ms、sourceStable=true。相对 normal relay39，本轮配置只增加从 8,000 ms 起、持续 1,200 ms、额外延迟 160 ms 的 burst：以 relay 首个非空 input 为时钟起点，仅 input lane 在指定 1200 ms 内从单程 39±5 ms 增至 199±5 ms。这不是全 transport 断网，也不把 repair lane 说成承受相同延迟。

其余仍为桌面双端 CPU4、人工 continuous 120 Hz 源计划、D0 / H8、1200 帧及 body180→1200，冻结 WASM 和 23790f4409b8ec835d47f21c1d8293280acd37f084d1ba8b7d94d6a1b04bf1ff 版 driver 未变；没有新的 C++ 构建或生产算法优化部署，既有 51 tests 的结果与源身份不变。本轮同样没有手机测试。两端正文完整、10 个 ordered confirmed hashes 到 1200 相同，端点与实际 relay 有效、teardownClean=true（仍限既定 disposal 前检查），实际 GPU 均为 Intel UHD / D3D11。

**measurement passed 表示证据窗口、输入/确认、传输和端点资格通过，不表示性能目标通过。** 两端仍出现 60.505 / 90.335 ms 最大新状态间隔，超过 50 ms 的间隔为 5 / 1 次。用户允许网络波动时减速，但这类停顿仍是待解决问题；没有 >100 ms 不能替代流畅性验收。

| 正式窗口指标 | P1 | P2 |
| --- | ---: | ---: |
| logic Hz | 56.2303658860 | 56.2014281224 |
| rAF 新状态 Hz | 56.1639988767 | 56.2154480493 |
| 新状态间隔数 n | 1,018 | 1,019 |
| rAF 新状态间隔 p99 / max | 38.925 / 60.505 ms | 38.075 / 90.335 ms |
| >25 / >50 ms 间隔数 | 44 / 5 | 32 / 1 |
| >100 / >250 ms 间隔数 | 0 / 0 | 0 / 0 |
| 整个 C++ callback wall max | 16.730 ms | 12.090 ms |

历史原生端点八词原样为 P1 [1205,1203,909,2269,1205,0,1,1203]、P2 [1206,1202,918,3855,1206,0,1,1202]；累计 correction / resimulation 为 909 / 2,269 与 918 / 3,855，不是单次回滚深度，stats[5] 仍是 repairsSent。信息缺口 B 的中位数为 2 / 4、p99 为 8 / 7、max 两端均为 8；backlog 的 p99 为 1 / 0、max 为 6 / 4。两端 nativeNetworkScale 的 p50=1、p99=max=2，这是步长 interval multiplier 增大、减速，不是加速。

普通前进子集 n=275 / 255、全部 WorldSave=1，整 callback wall 中位数为 3.225 / 2.815 ms、max=12.795 / 5.230 ms。不得把这些值或 whole callback max 改称纯 tick CPU、恢复耗时，也不能将约 56.2 Hz 的全部差距直接归因线路。

| 人工触控记录分母 / 活动 | P1 | P2 |
| --- | ---: | ---: |
| planned events（同冻结源计划） | 14,301 | 14,382 |
| due / submitted | 2,294 / 1,702 | 2,297 / 1,708 |
| coalesced / notSubmitted | 590 / 2 | 587 / 2 |
| actual input frames | 1,205 | 1,206 |
| analogActiveFrames | 1,003 | 995 |
| targetChanges / positionChanges | 984 / 1,067 | 992 / 1,037 |
| changedHistory / captureGaps | 0 / 0 | 0 / 0 |

两端 touch valid，源计划、实际提交与游戏实际接受/活动分母分开；计划 120 Hz 不等于实到或每帧 active。host 全轮 mean busy fraction=0.2424635198（约 24.2464%）、24 个逻辑 CPU，不能用于逐间隔归因。

#### 实际 burst 覆盖与 relay 发送观测

burst 覆盖的 119 个包全部已发送，P1/P2 为 60 / 59，sender frame 范围 467–504，位于双方正式 body180→1200 内。该批 receive→send 的中位数为 201.802 / 202.902 ms、最大值为 215.2865 / 211.7082 ms；timer lateness max 为 14.042 / 12.309 ms，负 lateness 也保留。bufferedBefore/After max 均为 0，没有 throws 或 budget failure。

relay 总记录 receive=scheduled=3,777、sent=3,775、pending=2、无 cancel；pending 不能删掉或直接当丢包。这些是 relay 接收/计划/发送边界，exactNativeAcceptance=false、osArrivalIncluded=false，不能倒推出游戏 native 或 OS 的准确接收时刻，也不代表每个发送包都已逐包证明对端接受。

#### 大间隔内仍有 callback：信息耗尽与连续恢复要分别处理

[本轮 timing](rollback-desktop-continuous-touch-both4-relay39-burst160-c1-timing.json)确认严格正文映射 2,298/2,298 与 2,355/2,355，RAF diagnostics 有效且无 overflow。全正文最大合格已登记请求等待仅 22.025 / 21.255 ms；它们与下面最大新状态 gap 是不同指标，不能互相替代。

[gap-network-context](rollback-desktop-continuous-touch-both4-relay39-burst160-c1-gap-network-context.json)保留全部六个 >50 ms 间隔。下表只填已核到的区间字段，未列出的数值不补推：

| peer / sample 区间 | rAF gap / Page 观察 gap | 区间内已知状态与恢复 | 已知单 callback / 合格等待 |
| --- | --- | --- | --- |
| P1 1027→1034 | 55.645 / 51.645 ms | F=475，A467→468，B=8，backlog=0，correction/resimulation=0/0 | C++ max=3.035 ms；eligible wait=8.425 ms |
| P1 1100→1106 | 51.840 ms / 未在此列值 | 1 correction / 7 resimulations，backlog=6 | 未在此列值 |
| P1 1106→1112 | 53.980 ms / 未在此列值 | 1 correction / 7 resimulations | 未在此列值 |
| P1 1140→1146 | 60.505 / 68.180 ms | F=501，A495→497，B=6，backlog=4，2 corrections / 11 resimulations | C++ max=13.025 ms；eligible wait=7.500 ms |
| P1 1150→1156 | 51.995 ms / 未在此列值 | 2 corrections / 11 resimulations | 未在此列值 |
| P2 1046→1058 | 90.335 / 91.425 ms | 多个 rAF 中 F=475、A=467、B=8、scale=2；末尾 A 到 468 后有 1 correction，resimulation 分段为 6+2，再到 F=476 并呈现 | C++ max=8.240 ms；eligible wait=7.815 ms |

P2 的大间隔包含远端实际输入前沿固定、预测余量耗尽的段落；P1 还存在多个 callback 连续恢复的区间。因此，间隔不能整体解释成一次重演很慢，也不同于旧约 150 ms 的下一 rAF 未获派发等待。观察到网络信息缺口不等于线路是唯一原因；单 callback 成本、恢复分片与呈现条件仍需分开核查。Page 观察时间仍按既定 harness 采样点解释，不是 callback 完成或物理呈现，两页也不能精确跨时钟相减。

具体线索是 P1 sample1144 已恢复到 next==F501，却没有 present；sample1145 又收到新的 correction 并回卷。后续已据此形成第 5.14 节补尾 B 候选并通过控制流正确性门，但不能据门测试反推每个实测样本的新分支触发或性能原因；第 5.15 节保留其性能收益未成立的结果。

沿这两个方向，主代理先完成补尾 B 的独立实现、构建、正确性和实测，再回切原版；结果不支持 B 的性能收益，暂不采用。随后另建完全不含 B 的提前网络制动 C，构建与组件通过，普通 relay 约 54.4 Hz 的不利样本已保留，C burst与普通A回切也已完成，分别保留约31Hz和约半秒停顿，B/C均不采用，详见第5.14–5.17节。不盲加 H、输入延迟或关闭 vsync；GPU单flag A0/B1/A0已另行预声明，测试支持冻结且3 syntax/60项纯测试通过，A1测量已完成且约34Hz的坏样本完整保留，B1已完成且约56.5Hz/rAF无>50ms但P1 Page max53.795ms，A2回切也约58Hz/rAF无>50ms，未建立参数可逆关联（第5.18节），旧长等待没有据此结案。

[本轮 raw](../../th09_web/artifacts/multiplayer-tests/desktop-continuous-touch-both4-relay39-burst160-c1-report.json)为 32,647,281 B；[general evidence](rollback-desktop-continuous-touch-both4-relay39-burst160-c1-evidence.json)为 109,596 B；timing 为 577,848 B；gap-network-context 为 122,745 B。离线命令 4UNX1QmlAWuV2M5osG-oqbKM / -cJe1UPox7zFQGWHas5qwQWJ / ZoO5_CmjFexcjPMC7WgUkHDa 均 exit0，耗时分别为 721 / 648 / 593 ms，仅为提取耗时。完整身份与命令见第 9.1 / 9.2 节，全部旧失败、截断窗口与坏样本继续保留。

### 5.14 恢复尾部候选 B：构建与正确性门通过，补一个已 due 的前进 tick

主代理在独立 worktree D:/workspace/eagler/worktrees/th09-recovery-tail-4x、branch experiment/recovery-tail-4x、HEAD 63127575d43ea5ef35b317ff8137b00a1ab5b1cc 实施候选 B。初始化时发现的 PerformanceDiagnostics.hpp 差异仅为 CRLF，已恢复冻结原字节，初始 275 个构建源输入匹配基线；后续语义改动只在 Application.cpp。原版 Application.cpp SHA 为 d14f97f819e96a15bed9acbe4a685cdf571b6dfb785210ad93552f79bf2d294d，候选为 5a8580c706cde19b8ea411bbb4f54ccae776524e5971f7c658c58e8bb94a066b。

改动针对原 8 ms 预算用尽后的 break：当 next 已追到旧 catchup 前沿、生命周期允许活动、已经持有本地 capture，且只读 Prepare 允许推进时，让原循环再兑现一个**已经 due** 的普通前进 tick；不满足条件仍 break→publish。这里 activePlay 排除 title/pause/over/complete/pending，不严格等于正式 Match phase。H8、D0、pacing、once capture、Draw/Seal/hash 约束不变。8 replay+1 fresh 的 cap 是**每次 rollback_tick 调用**，不是整个 rAF 的上限；外部 10 ms 检查是事后检查，不保证 callback≤10 ms 或≤16.7 ms。纯 B=8 且没有新远端实际输入的等待不会被这项改动消除。

[base identity](rollback-desktop-recovery-tail-base-identity.json)与[预声明 plan](rollback-desktop-recovery-tail-plan.json)保留初始身份和单变量范围。[build identity](rollback-desktop-recovery-tail-build-identity.json)验证 275 源列表、规范路径后的 flags / perSourceFlags / imports / exports：只有 Application.cpp 改变。release 与 DEV 构建均 exit0、sourceStable=true：

| 构建 | 工件 / 用途 | Wasm 大小 | SHA-256 |
| --- | --- | ---: | --- |
| release，55,713.9234 ms | desktop-recovery-tail-release-c1/th09.wasm，profile0，性能运行使用 | 2,828,005 B（+98 B） | 1060150871b6ce658a131f9930c28cc0504532a16c441f404ba222bc363d551c |
| DEV，56,222.7756 ms | desktop-recovery-tail-live-c1/th09-presentation.wasm，profile1，只用于正确性 | 3,155,633 B（+89 B） | 866f8238480a8b65e96a37d765887f570257225af52c45924bbe0ef762128c9c |

工件位于该 recovery-tail worktree 的 th09_web/artifacts 下，loader 身份未变。构建耗时不是游戏性能；工件字节差异也不表示性能收益。

#### 三世界与实际 App 控制门是不同层次的正确性证据

原有四组件 PASS、sourceStable，但这些组件不编译 Application.cpp 新分支，不能单独验证补尾控制流。随后 desktop-recovery-tail-joystick-h8-oracle-c1 真实三世界门通过：600 帧、D0、接收方 frontier 延迟 8 帧、人工 joystick 120 Hz 的 capture-frame 受控时钟。三个世界在同一个 CPU4 renderer 内顺序执行，**不是独立 Page wall-clock 输入，也不是双端各自限速的 FPS 测试**。

| 三世界记录 | P1 | P2 / 对照 |
| --- | --- | --- |
| 原生 summary 八词 | [600,600,519,4132,600,267,1,600] | [600,600,522,4167,600,263,1,600] |
| actualRestoredDepth max | 8 | 8 |
| partialRecoveryCalls / 单次 max resim | 413 / 8 | 409 / 8 |
| analogActive / targetChanged / positionChanged | 400 / 354 / 395 | 393 / 359 / 385 |
| raw touch 事件数 | 738 | 744 |
| 最终 world hash | 1960487508 | P2 与第三世界均为 1960487508 |

5 个确认点至 600 与第三世界相同，48 词 audit 三者 SHA 均为 af8dc77906c57aaf8b54e82ee42b5e2b1fe9b4126623a016d1b3755c9fbf6215，framebuffer 相同，完整 Replay 为 10,636 B、SHA 3ec779cbeb310057262a719e532927230943ca427f4ef3df03189fe38790653e。Intel UHD / D3D11 已确认，cleanup complete=true、errors=[]。summary[5] 的 267 / 263 仍是 repairsSent，不是等待。oracle wrapper 103,554.5964 ms 不是游戏吞吐指标；[correctness evidence](rollback-desktop-recovery-tail-correctness-evidence.json)保留这组深 8 帧正确性证据与四组件结果。

新增 recovery-tail-control.mjs 则提取**实际 Application 函数**作控制门：候选 14 项全通过；原版 Application 用同门测试 exit1，恰有 4 个补尾场景失败、其余 10 项通过。这是符合预声明预期的负对照，不把原版写成普通 PASS，也不隐藏其失败。覆盖 Save / Draw / Seal / Complete / hash / confirmed publication / audio、早退与生命周期条件；[control evidence](rollback-desktop-recovery-tail-control-evidence.json)保留完整结果。

这些门支持候选的控制流和回放正确性，不证明每个实际性能样本都触发了新分支，也不构成速度归因。B 的采纳判断继续由实际 normal / burst 与原版回切证据决定。

### 5.15 候选 B 的实测与 A 回切：性能收益未成立，隔离保留而暂不采用

下列实测均用正式 F180→1200 窗口、两个独立桌面 renderer 各 CPU4、独立 Page-clock continuous 人工触控 120 Hz 源计划、D0 / H8、冻结 23790f4409b8ec835d47f21c1d8293280acd37f084d1ba8b7d94d6a1b04bf1ff driver。新 B normal、B burst 和 A 回切两端端点、实际 relay 路径、10 个 ordered confirmed hashes 至 1200 以及既定 cleanup 资格通过。A 为原版 Wasm 037c9cd52e70e2c8e8ee516cf15b6aee7f0de5951f88d519b2311cc1c5def373，B 仅替换成第 5.14 节候选 Wasm。

B normal relay39（desktop-recovery-tail-both4-relay39-c1）exit0、wrapper 54,440.9057 ms。两端新状态 Hz 为 59.95212655 / 59.87215707，p99 gap=18.285 / 18.285 ms、max=25.095 / 30.295 ms、>50 ms=0/0，整个 C++ callback wall max=15.015 / 14.525 ms。累计 correction=902/952、resimulation=2,972/3,218；B 缺口 max=5/4、backlog max=1/1；普通前进整 callback 中位数=3.200/3.065 ms。它比前述 normal relay 样本更接近 60 Hz，但不能据这一次动态输入和宿主状态下的结果宣布单项收益。

burst 条件均为 first-nonempty-input+8000 ms 起、持续 1200 ms、input lane 额外增加 160 ms。A1 是第 5.13 节原版首次 burst；A2 是在 B 之后回切**原版 Wasm**重新运行，没有把 B 补尾留在 A2：

| burst 运行 | 新状态 Hz（P1 / P2） | rAF gap p99（P1 / P2） | 最大 rAF gap（P1 / P2） | >50 ms 次数 | 整 C++ callback wall max |
| --- | --- | --- | --- | --- | --- |
| A1：原版 burst160-c1 | 56.1639988767 / 56.2154480493 | 38.925 / 38.075 ms | 60.505 / 90.335 ms | 5 / 1 | 16.730 / 12.090 ms |
| B：recovery-tail burst160-c1 | 56.35725629 / 56.43164281 | 36.375 / 36.345 ms | 66.525 / 96.750 ms | 1 / 1 | 22.895 / 16.735 ms |
| A2：原版 burst160-c2 | 57.63071044 / 57.66984945 | 此处未列值 | 66.530 / 66.560 ms | 1 / 1 | 12.155 / 12.215 ms |

B burst（desktop-recovery-tail-both4-relay39-burst160-c1）exit0、47,894.8233 ms；累计 correction=897/897、resimulation=2,338/3,715，信息缺口 B max 两端均为 8，普通前进整 callback 中位数=4.470/3.030 ms。A2（desktop-continuous-touch-both4-relay39-burst160-c2）exit0、51,644.8519 ms；累计 correction=908/973、resimulation=3,143/3,265，B max 同样为 8，普通前进中位数=3.015/3.050 ms。

**B 的性能收益尚未成立，保留隔离、暂不采用。** A2 回切原版也把 >50 ms 次数降到 1/1，不能把 B 相比 A1 少了几个间隔归功于补尾；B burst 最大 rAF 间隔和 C++ callback 尾部没有改善。名义参数受控不等于实际 Page-clock 输入、世界负载和宿主状态逐样本配对。host whole mean busy fraction 分别为 B normal 0.3817092679、B burst 0.2892368260、A2 0.2684939782；这些全轮概况不支持把差异简单归因环境或设备算力。

#### B 与 A2 的最大间隔仍有信息等待，不是单次 C++ 超长调用

| 运行 / peer | 最大 rAF 间隔与同端点 Page 观察间隔 | 区间内证据 | 单 callback / 已登记等待 |
| --- | --- | --- | --- |
| B burst P1 | 66.525 / 73.520 ms | B=8 等待后重演帧数分段为 5+3；1 correction / 8 resimulations | C++ max=8.720 ms；eligible wait max=6.390 ms |
| B burst P2 | 96.750 / 100.335 ms | B=8 等待后重演帧数分段为 7+1；1 correction / 8 resimulations | C++ max=8.965 ms；eligible wait max=8.030 ms |
| A2 P1 | 66.530 / 65.120 ms | F=485、A=477、B=8；0 correction / 0 resimulation，信息等待 | C++ max=2.620 ms |
| A2 P2 | 66.560 / 71.220 ms | 分 4+4 次重演 | C++ max=8.275 ms |

**B 的 P2 确实观察到 100.335 ms 的 Page 新状态间隔，不能因 rAF 指标为 96.750 ms 而写“没有观测到 100 ms”。** 两类时间戳保持原含义，不冒充物理呈现或 callback 完成；这些长间隔里的单次 callback 和已登记等待较短，也不支持整体归为一次重演或旧式约 150 ms 的 rAF 未派发。

B burst 实际 130 包全部发送、P1/P2 为 64/66，sender frames467 以外的实际范围分别为 464–502 / 465–503；timer lateness max=14.8252/12.204 ms，buffered=0、error=0。A2 为 125 包、62/63 全发，lateness max=15.2621/14.3194 ms、buffered=0。仍仅为 relay 观察边界，不能升级为 native/OS 接收时刻或唯一根因证据。

#### evidence v2 的 scope 勘误透明保留

两个 B 初版 evidence 的 scope 曾错误写入“No production code change between these runs”。实际上 B 已改 Application.cpp。主代理仅更正这一 scope，分别生成[normal evidence-v2](rollback-desktop-recovery-tail-both4-relay39-c1-evidence-v2.json)与[burst evidence-v2](rollback-desktop-recovery-tail-both4-relay39-burst160-c1-evidence-v2.json)，旧文件保留为 superseded，raw 和所有数值未改；本报告引用 v2。更正命令 PA4T_Y5wjT0eqxR72KL6hU88 exit0、536 ms。

[B burst gap-network-context](rollback-desktop-recovery-tail-both4-relay39-burst160-c1-gap-network-context.json)与[A2 gap-network-context](rollback-desktop-continuous-touch-both4-relay39-burst160-c2-gap-network-context.json)继续保留反证；[A2 evidence](rollback-desktop-continuous-touch-both4-relay39-burst160-c2-evidence.json)使用原版身份。完整 raw、派生、命令及 SHA 见第 9.1 / 9.2 节。

### 5.16 网络提前制动候选 C：不含 B，普通与 burst 均有不利表现，暂不采用

主代理另建 D:/workspace/eagler/worktrees/th09-network-brake-4x，同 HEAD 63127575d43ea5ef35b317ff8137b00a1ab5b1cc。C **完全不包含 B 的 Application.cpp 补尾**，只把 RollbackSession 网络 target 的 (B−4)/3 改为 (B−3)/3，提前一个预测帧：B=4 开始提高 target，B=6 时 target 达到 interval multiplier 2，实际 scale 仍经原有渐变。H8 / D0、50 ms 升 / 150 ms 降、once capture 与通道 Pump 不改。[预声明 plan](rollback-desktop-network-brake-plan.json)保留该单变量和边界。

这里 B 是预测前沿减去连续远端实际 input 前沿，是信息缺口，不是 replay 债务或设备算力指标；它也不证明信息滞后只来自物理线路。预测历史 8 帧耗尽后仍可能停；固定 A 的理想模型仅大致多一个 tick 的有限余量，不承诺消除 160 ms burst，也不通过增加 H 或固定输入延迟来换取结果。

C release 构建 exit0、50,559.2387 ms、sourceStable=true。[build identity](rollback-desktop-network-brake-build-identity.json)已 PASS：275 个源输入中只有 RollbackSession.cpp 改变，候选该文件 SHA=198776ef3209f4d02c6954f4864648d0308355bd4477ee937e7924a26b47709d，Application.cpp 与原版相同；flags / perSourceFlags / imports / exports / kind / audio / current-source / manifest 均通过。release 工件位于新树 th09_web/artifacts/desktop-network-brake-release-c1/{th09.wasm,th09.mjs,build.json}；Wasm 为 2,827,901 B（较原版 −6 B）、SHA f0e04a7069fe3aeaf4ab52018aa8d1b944811cad0cc7b4dc17d9f9d7e1b9ae7b，loader 与原版相同；字节差异不是性能指标。

C 的 session.cpp **合同测试 fixture** 为 20,660 B、SHA57ec08b06a51deec5197e63281d0afbfb2f24218257cbb1592413683f46bb30f，新增固定 B=3/4/5/6 点并保留原断言；它不是生产 RollbackSession.cpp 的身份。四组件 desktop-network-brake-components-c1 已 exit0、17,483.7563 ms、sourceStable=true，主代理完整读取新树 th09_web/artifacts/multiplayer-tests/desktop-network-brake-components-c1-process.log（97 行 / 9,353 B），保留 16 移动变化、D0/H8/full-input/once-only 合同。组件与构建 PASS 均不能代替双端实际游戏性能。

#### C 普通 relay 实测资格通过，性能结果混合且不利

desktop-network-brake-both4-relay39-c1 已完成，命令 bMAN7D1yiLGeR6JWp5XqGkNT exit0、wrapper 50,413.4904 ms、sourceStable=true。正式 body180→1200 的资格、端点、实际 relay、10 个 ordered confirmed hashes 到 1200 与既定 cleanup 均 PASS，仍为两端独立桌面 CPU4、同 continuous 人工输入与 D0 / H8 条件。

| C 普通 relay 指标 | P1 | P2 |
| --- | ---: | ---: |
| logic Hz | 54.49400579 | 54.41240603 |
| rAF 新状态 Hz | 54.42203055 | 54.36908760 |
| rAF 新状态间隔 p99 / max | 42.395 / 48.695 ms | 30.305 / 42.420 ms |
| Page 新状态观察间隔 max（后续九组汇总） | 51.280 ms | 47.015 ms |
| rAF >25 / >50 ms 次数 | 72 / 0 | 25 / 0 |
| 整 C++ callback wall max | 24.895 ms | 13.835 ms |
| 普通单前进子集 n / 中位数 | 294 / 4.695 ms | 302 / 3.210 ms |
| 普通单前进子集 WorldSave | 全部 1 | 全部 1 |

历史 stats 为 P1 [1206,1204,943,2180,1206,0,1,1204]、P2 [1207,1203,897,3299,1207,0,1,1203]。信息缺口 B 中位数为 2/4、max=4/5，backlog max=3/3；nativeNetworkScale 的 p99 / max 为 P1 1.093 / 1.320、P2 1.333 / 1.513，仍是步长 interval multiplier，不能把它解读为加速。

触控 actual input frames=1,206/1,207、analogActive=1,013/970、targetChanges=953/966、positionChanges=1,097/1,049；due=2,392/2,391、submitted=1,490/1,768、coalesced=901/621、notSubmitted=1/2，changedHistory/captureGaps=0。源计划与实际活动仍分开，不能称每帧 active。host whole mean busy fraction=0.2729873341，不能据此对单个间隔或整轮差距作因果判断。

**C 普通 relay 的测量 PASS 不等于有性能收益。** 本轮 rAF 口径没有 >50 ms 间隔，但 Page 观察的 P1 max=51.280 ms；约54.4 Hz、较多 >25 ms 间隔和较高整 callback 成本仍是不利结果。后续普通 A 回切也仅约55.2 Hz且有约半秒停顿，实际输入与宿主状态又是动态的，不能把C下降全部归为阈值代码，也不能据回切为C坏样本免责（第5.17节）。

[本轮 evidence](rollback-desktop-network-brake-both4-relay39-c1-evidence.json)为 109,695 B，离线命令 c4x1G7_na4RH-1GJI8ea_Am6 exit0、936 ms；raw 为 35,114,817 B。完整 SHA 与运行身份见第 9.1 / 9.2 节，离线耗时不作游戏性能指标。

#### C burst 首跑约 30.9 Hz，全部不利数据保留

desktop-network-brake-both4-relay39-burst160-c1 已完成，命令 BKUxLr0R9i1SkdbCUwqTDrdU exit0、wrapper 83,095.0256 ms、sourceStable=true。两端资格、端点、实际 relay、10 个 ordered confirmed hashes 至 1200 与既定 cleanup 均 PASS；**这个 PASS 只指正确性/测量资格，绝不指性能目标通过。**

| C burst 正文指标 | P1 | P2 |
| --- | ---: | ---: |
| logic Hz | 31.14440102 | 31.21784485 |
| rAF 新状态 Hz | 30.93140708 | 30.89697789 |
| rAF gap p99 / max | 139.365 / 181.775 ms | 139.365 / 224.050 ms |
| Page 新状态观察间隔 max（后续九组汇总） | 180.645 ms | 213.025 ms |
| rAF >50 / >100 ms 次数 | 153 / 40 | 148 / 37 |
| 整个 C++ callback wall max | 54.640 ms | 55.665 ms |
| allObservedSingleForward n | 391 | 365 |
| 该子集整 callback wall 中位数 / max | 13.525 / 51.235 ms | 14.270 / 55.665 ms |
| 该子集 Save0 / Save1 样本数 | 11 / 380 | 20 / 345 |
| Save1 子集整 callback wall 中位数 | 13.510 ms | 13.365 ms |

allObservedSingleForward 按相邻 callbacks 的 next+1 / correction+0 / resimulation+0 / captures+1 分类；不保证该 callback 只尝试一次 tick。整 native callback wall 不是 CPU self time，也不是独立 Save/Restore 成本。即使这个没有新增重演的描述子集也有较高 wall 成本，仍不能据此指定是设备、阈值或某个子步骤所致。

历史 stats 原样为 P1 [1202,1201,832,1864,1204,0,1,1201]、P2 [1201,1200,835,2729,1203,0,1,1200]。B 缺口中位数=2/3、max 两端均为 8；nativeNetworkScale 中位数均为 1、p99/max 均为 2，backlog max 两端为 5。不能仅因存在 B=8 或 scale=2，就把所有长间隔归为纯线路等待。

两端 touch valid，actual frames=1,204/1,203、analogActive=1,071/941、targetChanges=956/869、positionChanges=1,089/1,042；due=4,088/4,110、submitted=1,454/1,648、coalesced=2,633/2,457、notSubmitted=1/5，changedHistory/captureGaps=0。大量 coalesced 与实际分母原样保留，不借持续计划掩盖实际提交差异。host mean busy fraction=0.3370934868，既不能当卡顿因果，也不能作为删除这一轮的理由。

**C 暂不采用。** 首跑明显不达标，保留完整样本；不因 host/noise 排除结果，也不凭单轮把下降全部归为阈值。后续普通 A 回切及九组双时钟/条件响应分母汇总已完成，见第5.17节；它们没有消除C这组反证。

#### C 坏样本进一步定位：后半段普通 callback 变慢，不能全部解释为主动网络减速

[C burst timing](rollback-desktop-network-brake-both4-relay39-burst160-c1-timing.json)保留全正文2,341/2,341与2,604/2,604 callback映射，valid=true、无overflow；最大合格registered wait为51.570/35.910 ms，与原版回切的单个477.630 ms等待不是同一种形状。

[固定 frontier 分箱](rollback-desktop-network-brake-both4-relay39-burst160-c1-adverse-context.json)只用于定位，不删样本、不代表同场景或CPU self time：

| frontier区间 | logic Hz（P1/P2） | 普通整C++ callback中位数（P1/P2） |
| --- | --- | --- |
| 180–360 | 53.23 / 53.36 | 6.000 / 3.245 ms |
| 360–540 | 45.97 / 46.17 | 5.110 / 3.040 ms |
| 540–720 | 53.96 / 54.09 | 5.065 / 3.255 ms |
| 720–900 | 27.65 / 27.28 | 17.555 / 17.940 ms |
| 900–1080 | 18.63 / 18.78 | 22.140 / 22.090 ms |
| 1080–1200 | 20.19 / 20.32 | 18.530 / 21.460 ms |

adverse文件选取的P1最大gap位于F1063，区间后续5个callbacks合计2次correction、5帧resimulation，native C++为11.330–36.285 ms，信息缺口B3→0且scale全为1；P2所选最大gap位于F837，7个callbacks合计3次correction、7帧resimulation，native为11.495–26.245 ms，B2→0且scale全为1。具体最大pair保留在artifact，不据分箱重新选择更好端点。

已知实际输入补齐后，仍有高普通耗时和追平旧前沿又遇新correction的段落。不能把整轮约31Hz全部解释成网络主动降速，也不能断言只由编译、游戏负载或阈值中的某一项造成。两份离线产物和所有不利区间均保留。

[C burst evidence](rollback-desktop-network-brake-both4-relay39-burst160-c1-evidence.json)为 107,859 B、raw 为 33,516,070 B；分析命令 BgKbXeyDHUNvgRCSMo3FgzJi exit0、719 ms，只是离线提取时间。B 仍隔离不采用，C 没有部署；GPU单flag A0/B1/A0支持代码及纯测试门已通过，A1测量已完成但性能不达标，B1和A2均完成且较A1有利，未建立参数可逆关联、不采用为修复，全部旧 trace、失败、截断及不利样本保留，仍仅桌面人工触控与 nig。

### 5.17 原版普通 relay 回切：约半秒停顿也会出现，九组比较保留双时钟与输入分母

原版 desktop-continuous-touch-both4-relay39-c2 已完成，命令 Xpcxu9qrovmPlXM4_o-dpWXJ exit0、wrapper 50,734.7632 ms、sourceStable=true。原版 A Wasm 不含 B/C 改动；两端仍为独立桌面 CPU4、continuous 人工触控、D0/H8、普通 relay39、正式 body180→1200。端点、实际 relay 路径、10 个 ordered confirmed hashes 至 1200 和既定 cleanup 资格均 PASS，**但性能显然没有达标**。

| A 普通回切指标 | P1 | P2 |
| --- | ---: | ---: |
| rAF 新状态 Hz | 55.24502442 | 55.15438895 |
| rAF 新状态间隔 p99 / max | 30.360 / 424.230 ms | 30.265 / 478.705 ms |
| Page 新状态观察间隔 max | 435.625 ms | 493.565 ms |
| rAF >50 / >100 / >250 ms 次数 | 1 / 1 / 1 | 1 / 1 / 1 |
| 整个 C++ callback wall max | 19.090 ms | 15.365 ms |
| allObservedSingleForward n / 中位数 | 290 / 4.270 ms | 277 / 3.025 ms |
| 该子集 Save0 / Save1 样本数 | 1 / 289 | 7 / 270 |
| Save1 整 callback wall 中位数 | 4.270 ms | 3.030 ms |

信息缺口 B 中位数=2/4、max=8/6，backlog max=6/3；host mean busy fraction=0.2793057363，不作逐间隔或整轮差异的因果解释。这里的普通前进分类仍是相邻 callback 增量条件，不保证只尝试一次 tick，也不是 CPU self time。

[严格 timing](rollback-desktop-continuous-touch-both4-relay39-c2-timing.json)保留全正文 2,701/2,701 与 2,907/2,907 callback 映射，valid=true、overflow=false。P1 最大新状态区间 sample732→795 的 rAF gap=424.230 ms、Page gap=435.625 ms，但 P1 全正文最大合格已登记 rAF 等待为 15.810 ms。P2 最大区间 sample801→803 为 rAF 478.705 / Page 493.565 ms；其中 sample802 的合格已登记请求等待达到 **477.630 ms**，请求有效、active/visible/intact/unpaused，结束后该 callback 的 native C++ 测值仅 0.005 ms，correction/resimulation/capture/Save/present 增量均为 0。0.005 ms 是进入后那个 callback 的测值，不能把等待的 477.630 ms 写成 CPU 连续忙碌。

A 回切也只有约 55 Hz，并出现约半秒停顿，说明此类问题不能只归新 C；但这并不为 C burst 约 31 Hz 的坏样本免责。当前需要区分：单端游戏 rAF 未派发造成的新输入供给断档、普通 native callback 变慢，以及网络信息本身不足。B/C 均暂不采用，旧坏样本不删。

#### 单端供给断档传播：frame 次序与 relay 记录相互核对

[peer-propagation](rollback-desktop-continuous-touch-both4-relay39-c2-peer-propagation.json)显示，在 P2 的该段已登记等待期间，没有 P2 game callback 样本，也没有新 input send-intent。本地 frame287 最后观测约为 Unix 1791721937071.000 ms、frame288 首次约为 1791721937556.975 ms，间隔 485.975 ms。relay 对应 sender1 的 fresh input frame287 接收为 1791721937071.9768 ms，frame288 为 1791721937557.610 ms，接收间隔 485.6332 ms；该段没有该 sender 的新 receive。

此前已在 relay 的 4 个包仍正常发出，receive→send 最大 46.5044 ms、timer lateness 最大 5.6368 ms、buffered=0、error=0。同期 P1 仍有 79 个 callback、11 个新 input send-intent，F286→296、A 最多到 288、B 达到 8，随后停止新前沿等待。这支持“单端游戏调度/输入供给断档传播到对端”的解释；不能说 relay 把已有的 P2 新输入扣留约半秒，也不能扩写为全部网络均无问题。

跨进程 Unix 使用各自 performance.timeOrigin+now 近似对齐，不是独立校准的 OS 接收时钟；frame 次序提供另一层核对。这里没有 GPU trace，不能据这一轮定 GPU、SwapBuffers、编译或某项系统调度原因。

窗口状态还有限制：setup 时两 Chrome windowState=normal；延迟取得的 measurement-complete 快照已在等待起点约 31.47 秒后，此时两者 minimized，而 Page 仍报 visible / focus=true。不能用这个晚快照反推卡顿当时 native 窗口状态；既有 Page active/visible 资格不证明持续前景或物理显示。上述等待与供给断档证据保留，但“始终处于前台显示”的范围未被证明。

#### 条件响应代理不能隐藏合并掉的到期事件

[input-gap-denominators](rollback-desktop-continuous-touch-both4-relay39-c2-input-gap-denominators.json)独立核对该 477 ms 区间：

| 按本区间到期事件计算 | P1 | P2 |
| --- | ---: | ---: |
| 到期的人工事件 | 57 | 57 |
| 这些事件在区间内提交 | 51 | 0 |
| 被合并的这些事件 | 6 | 56 |
| 已知区间后才提交的这些事件 | 0 | 1 |

P2 区间内另有 1 次提交，但它属于前一个到期事件，不计入上表这 57 个事件的区间内提交分母。P2 这 56 个合并事件没有进入成功条件响应样本，因此其整轮成功代理 max=35.855 ms **不能证明停顿期间输入一直低延迟**。保持合理 MOVE 合并，不靠事后补跑历史事件制造工作量；完整分母和响应删失必须一起看。

#### 已完成的九组比较 artifact

[continuous-touch-comparison-c1](rollback-desktop-continuous-touch-comparison-c1.json)保存 9 run / 18 端，核对全部原 raw 与 evidence SHA、完整事件计数恒等、body helper 的新状态 pair 数与最大值。各 run 完整 due/submitted/coalesced/censored 和逐 event 的 submit→new-render 条件代理记录均保留；不能把两段分位数相加当作总响应分位数，也不能让未提交、合并或删失事件从分母消失。

以下是 root 已核的条件归属代理统计；n 仅指对应已观测条件样本，不能作全部 intent 成功率或独立验收：

| 运行 | 条件代理 n（P1/P2） | submit→new-render p99（P1/P2） | 已提供的 max / >100 ms |
| --- | --- | --- | --- |
| A 普通 relay 首轮 | 993 / 869 | 28.895 / 22.905 ms | 此处未列值 |
| B 普通 relay | 1,056 / 990 | 21.720 / 21.805 ms | 此处未列值 |
| C 普通 relay | 952 / 968 | 37.040 / 26.510 ms | 此处未列值 |
| C burst | 941 / 866 | 116.160 / 122.305 ms | max=178.190/201.495 ms；>100=24/25 |
| A 普通 relay 回切 | 1,024 / 981 | 34.780 / 26.675 ms | max=414.805/35.855 ms |

代理记录不等于物理触控延迟，也不证明每个 event 导致对应游戏效果；同一 Page 内的条件观测不能跨页或跨进程直接相减。9 组的实际输入、角色负载和部分观察 profile 不完全配对，不能拿这些分位数直接宣布生产 A/B 收益。

双时钟最大值也分列保留。direct graphics c2 的 rAF max=24.150/24.070 ms，Page max=30.425/27.755 ms；C normal 的 Page max=51.280/47.015 ms，虽然其 rAF max=48.695/42.420 ms；C burst 的 Page max=180.645/213.025 ms，与 rAF 181.775/224.050 ms 不同；B burst 的 P2 Page=100.335 ms，原版普通回切 Page=435.625/493.565 ms。各列独立取最大，不能默认来自同一对端点；Page sample 仍是 harness 既定观察点，不是 callback 完成或物理 scanout。

本轮 raw 为 34,989,227 B、[general evidence](rollback-desktop-continuous-touch-both4-relay39-c2-evidence.json)109,473 B、timing 911,129 B、peer-propagation 117,164 B、input-gap-denominators 45,344 B；9 组 comparison 为 312,716 B。comparison 命令 Ub06KBMo2ltdOpKIxcXyvanx exit0，只知道首次 yield 为 1,014 ms，不把它编作总耗时。完整身份和命令见第 9.1 / 9.2 节。

### 5.18 GPU 单 flag A0/B1/A0：三组测量完成，参数可逆关联未成立

新的[预声明计划](rollback-desktop-gpu-vsync-plan.json)要求三组都使用原版 A Wasm、同一份冻结后的新 driver、同一 continuous 人工触控计划、双方 CPU4、普通 relay39 及 graphics trace，唯一切换 DISABLE_GPU_VSYNC=0/1/0。这里 B1 只表示浏览器单 flag 条件，不是 recovery-tail 候选 B；B/C 的生产 C++ 候选均不并入这个实验。

两端必须独立 PID，并从实际 argv 核对 --disable-gpu-vsync 的有无；不允许 --disable-frame-rate-limit（含带 = 的变体），--disable-gpu-vsync 本身也只接受规定的裸 flag，不把 = 变体、重复 flag 或缺失观察算合格。未设置参数时保留默认行为；显式 0/1 还要求 headed。allowed flag 清单不能替代 actual argv 证据；Draw、WASM、CPU、源计划和其余采集条件保持一致。

原 driver 字节已归档为 th09_web/artifacts/multiplayer-tests/smoothness-browser-before-gpu-vsync-23790f44.mjs，79,424 B、SHA 23790f4409b8ec835d47f21c1d8293280acd37f084d1ba8b7d94d6a1b04bf1ff。新的测试支持已实现并冻结，仅改 smoothness-browser.mjs 并新增 GPU 单 flag helper/契约测试，没有生产 C++ 或构建变动。driver 为 818 行 / 81,690 B，helper 为 64 行 / 4,192 B，新测试为 79 行 / 5,512 B；完整 SHA 见第 9.1 节。

[预检证据](rollback-desktop-gpu-vsync-preflight-c1.json)记录 3 项 syntax 与 60 项纯测试（51 项既有、9 项新增）全部通过，sourceStable=true；命令 hh7z2bSiVE5uuduzu5SKUzLt exit0，内部 elapsed=904.7354 ms。JSON 为 4,980 B，process log 为 7,257 B；这是参数、实际 argv 资格和原测试合同的门禁，不是游戏性能成绩。

**A1=0 的正式测量已完成，但性能不达标。** desktop-gpu-vsync-both4-relay39-a1，命令 8uBJDIM15YtKeOc71AoLNOLs exit0，wrapper=133,262.5858 ms，driver=d899f3cf…、sourceStable=true。模式 0、两端独立实际 argv、原版 A Wasm、双 CPU4、Intel D3D11、正式 body、历史端点、relay 路径及至 1200 帧的 10 个确认 hash 均合格；这里的 exit0 和测量资格不能解释为玩家流畅性通过。

| A1 正文指标 | P1 | P2 |
|---|---:|---:|
| 新状态 Hz | 33.9771 | 34.1456 |
| rAF 新状态最大间隔（ms） | 254.510 | 290.525 |
| Page 观察新状态最大间隔（ms，独立最大值） | 246.485 | 263.000 |
| rAF 新状态间隔 >50 / >100 / >250 ms 次数 | 124 / 31 / 1 | 117 / 35 / 1 |
| 整个 C++ rAF callback 最大 wall 时间（ms） | 68.220 | 57.000 |
| Save1 普通前进子集 callback 中位数（ms） | 5.990 | 5.940 |

host whole-run mean=0.31232672，只是宿主整体摘要，不是逐 gap 原因；普通前进子集仍按既定相邻样本描述，callback wall 不是纯 CPU 时间或单个 tick 的成本。A2 回切也较快，A1→B1→A2 未显示参数可逆关联；不能把 A1 的慢或卡顿单独归因为 vsync，也不能因后两组或其他样本较快就排除本组。

**trace 资格勘误与晚尾状态分开保留。** 首份派生 evidence 错把含零计数的 overflow 对象按 !object 求值，因而错误给出 trace=false；这不是本轮实际 trace 丢失。主代理严格核对旧 evidence / raw SHA 后，确认 7 个 overflow 计数均有限且为 0，firstAt/firstTraceTs=null、errors 为空，stopped 及 coverage 均为 true。两端 trace 覆盖合格，v2 只修正 gpuVsyncQualification.checks.trace、gpuVsyncQualification.passed 两字段及勘误元数据，逐结构深等比确认其他数字、端点和 teardown 均未改变。

正式端点到 trace-drain 晚尾检查约相隔 25.21 s，检查时两端已因 Confirmed peer input stopped advancing 关闭，teardownClean=false；25.21 s 是观测时间差，不是精确断开时刻，也不是正式 body 内关闭。测量和历史端点合格不能改写为收尾干净。raw 保持 39,201,425 B 原字节；[首份 evidence](rollback-desktop-gpu-vsync-both4-relay39-a1-evidence.json)114,063 B 原样保留，资格字段由 [evidence v2](rollback-desktop-gpu-vsync-both4-relay39-a1-evidence-v2.json)114,818 B 更正。勘误命令 7-b7eu1iXNQzxYYU7smy8wxK exit0、527 ms，没有重跑游戏或修改性能数字。

**B1=1 已完成，当前有利但不构成参数因果证明。** desktop-gpu-vsync-both4-relay39-b1，命令 gSpPyPI6vt9aT7wNdW6Bn3xx exit0、wrapper=97,748.5978 ms，冻结 driver=d899f3cf…、sourceStable=true。launch、mode1、实际 argv、双方 CPU4、native 正文、历史端点、至 1200 帧的 10 个确认 hash、relay 和 trace 覆盖均合格，overflow 计数均为 0。

| B1 正文与输入指标 | P1 | P2 |
|---|---:|---:|
| 新状态 Hz | 56.54624 | 56.52740 |
| rAF 新状态间隔 p99 / max（ms） | 30.620 / 45.060 | 29.630 / 45.660 |
| Page 观察新状态最大间隔（ms，独立最大值） | 53.795 | 47.580 |
| rAF 新状态间隔 >50 / >100 / >250 ms 次数 | 0 / 0 / 0 | 0 / 0 / 0 |
| 整个 C++ rAF callback 最大 wall 时间（ms） | 18.315 | 16.085 |
| Save1 普通前进子集 callback 中位数（ms） | 4.900 | 3.250 |
| 输入信息缺口 B 中位数 / 最大值 | 2 / 4 | 4 / 6 |
| backlog 最大值 | 2 | 3 |
| network interval scale 中位数 / 最大值 | 1 / 1 | 1 / 1.433 |
| 到期 / 提交 / 合并 / 未提交事件 | 2,304 / 1,444 / 860 / 0 | 2,303 / 1,613 / 688 / 2 |
| actualInputFrames / analogActive / targetChanges | 1,209 / 1,077 / 1,014 | 1,209 / 932 / 922 |
| positionChanges | 1,101 | 1,070 |
| changedHistory / captureGaps | 0 / 0 | 0 / 0 |

B1 的 host whole-run mean=0.2924056221 仍不是因果证据；这里的源计划频率不能替代实际输入事件/帧分母。该组比 A1 的长间隔和平均推进情况好，但尚不足以确认 DISABLE_GPU_VSYNC 的作用；平均约 56.5 Hz 也不改写为稳定 60 Hz。“无 >50 ms”只限 rAF 时钟；P1 的 Page 观察最大间隔为 53.795 ms，不能宽泛写为正文没有 >50 ms 间隔。正式端点到 trace-drain 晚尾检查约相隔 18.57 s，检查时双端已 late close，teardownClean=false；这是观测时间差，不是精确断开时刻，正式历史端点仍合格，没有把晚尾关闭放进正文或隐去。

本组 raw 为 38,258,240 B；[evidence](rollback-desktop-gpu-vsync-both4-relay39-b1-evidence.json)114,266 B，分析命令 lO_8H-Dauoj7TYsuq9Nko_tp exit0、664 ms，只是离线摘要时间。完整 raw / evidence SHA 已列入第 9.1 节。

**A2=0 回切已完成，结果未支持把 B1 的改善归功于开关。** 命令 VPIWtBg26KmE890KnqT_HYai exit0、wrapper=91,784.9012 ms，sourceStable=true。mode0 实际 argv、双 CPU4、原版 Wasm、正式 body/历史端点、至 1200 帧的 10 个确认 hash、relay、两端 trace 覆盖和零 overflow 均合格。与 B1 相同，这里的资格通过不等于稳定 60 Hz 或整条收尾干净。

| A2 正文与输入指标 | P1 | P2 |
|---|---:|---:|
| 新状态 Hz | 58.02829 | 57.93130 |
| rAF 新状态间隔 p99 / max（ms） | 30.295 / 42.425 | 24.270 / 34.440 |
| Page 观察新状态最大间隔（ms，独立最大值） | 45.630 | 37.870 |
| rAF 新状态间隔 >50 / >100 / >250 ms 次数 | 0 / 0 / 0 | 0 / 0 / 0 |
| 整个 C++ rAF callback 最大 wall 时间（ms） | 22.695 | 16.445 |
| Save1 普通前进子集 callback 中位数（ms） | 3.825 | 2.835 |
| 输入信息缺口 B 中位数 / 最大值 | 2 / 5 | 4 / 6 |
| backlog 最大值 | 2 | 2 |
| network interval scale 中位数 / 最大值 | 1 / 1.060 | 1 / 1.413 |
| 到期 / 提交 / 合并 / 未提交事件 | 2,256 / 1,645 / 610 / 1 | 2,252 / 1,640 / 611 / 1 |
| actualInputFrames / analogActive / targetChanges | 1,204 / 1,074 / 1,042 | 1,204 / 821 / 812 |
| positionChanges | 1,100 | 1,063 |

A2 的 host whole-run mean=0.2748690779，不能用整体负载摘要解释 A1 的坏样本。正式端点到 trace-drain 晚尾检查约相隔 19.58 s，检查时双端已 late closed，teardownClean=false，但正式历史端点合格；19.58 s 是观测时间差，不是精确断开时刻。raw 为 39,168,047 B；[evidence](rollback-desktop-gpu-vsync-both4-relay39-a2-evidence.json)114,119 B，分析命令 IYw0sCxa5Sp9kJz_ZCA2KVDw exit0、673 ms，仍是离线摘要耗时。

**本轮判断是 A 坏→B 好→A 也好，参数可逆关联未成立，因此不采用关闭 GPU vsync 作为修复。** 这不排除 GPU/呈现链路仍有问题，也不意味着 A1 全由编译或 CPU 造成；同一冻结配置不能替代实际输入逐样本相同，实际采样/合并分母必须一起保留。A1 的 254.510 / 290.525 ms 长间隔、v1 qualifier 勘误和三组晚尾关闭都继续保留。

**三组离线 timing 与双时钟/逐事件比较已完成。** timing 批次命令 kOwLJaY4Nzk6J1CGDpn8TLx6 exit0，三份 timing 全部 valid、无 overflow、完整映射正式 body callback。下表按各时钟独立取最大值，不能假设同一对端点，更不能相减当作某段因果延迟；A2 P2 的 rAF 与 Page 最大值已确认来自不同端点对。

| 组 / 端 | rAF 新状态 max（ms） | Page 观察新状态 max（ms） | 合格已登记 rAF 等待 max（ms） | body callback 完整映射 |
|---|---:|---:|---:|---:|
| A1 / P1 | 254.510 | 246.485 | 86.915 | 2,475 / 2,475 |
| A1 / P2 | 290.525 | 263.000 | 56.390 | 2,473 / 2,473 |
| B1 / P1 | 45.060 | 53.795 | 15.750 | 2,439 / 2,439 |
| B1 / P2 | 45.660 | 47.580 | 18.990 | 2,716 / 2,716 |
| A2 / P1 | 42.425 | 45.630 | 16.825 | 2,673 / 2,673 |
| A2 / P2 | 34.440 | 37.870 | 17.800 | 2,855 / 2,855 |

[三组比较 artifact](rollback-desktop-gpu-vsync-comparison-c1.json)为 115,833 B，命令 jhLNojUNxQJmGYxC9pENda9h exit0、1,006 ms；原 raw / evidence SHA 全部核对，并保留双时钟、每个事件的条件输入分布与完整分母。以下 submit→render 仅是全已观测且条件归属事件的代理，不能代表被合并的全部输入、逐事件游戏效果或物理触控延迟，也不能把不同阶段分位数相加推导该值。

| 全已观测条件 submit→render p99（ms） | P1 | P2 |
|---|---:|---:|
| A1 | 158.650 | 146.025 |
| B1 | 30.250 | 28.470 |
| A2 | 29.020 | 26.745 |

A1 同一条件代理的最大值为 236.845 / 253.150 ms。B1/A2 的较低成功条件分位数不免除检查 coalesced / censored / 未提交分母的要求。Page 时间仍是 harness 的观察时刻，不是 JS callback 完成或物理呈现；合格 rAF 等待的最大值也不能与不同区间的 CPP 最大值拼成单次停顿原因。

**六份 trace 与 raw 的联合时间线和 decision 已完成。** [联合时间线](rollback-desktop-gpu-vsync-joint-timeline-c1.json)为 829,386 B，命令 xn99rhj5_67YbB7z_-6SSPAp exit0；覆盖 3 组 × 2 端，逐端选择最长 registered wait、最大 Page 新状态 gap 和最慢 ordinary Save1 callback。六份 trace 的 SHA、标记映射及资格均已核验，映射传播完整 offset 区间；只分析 GPU/viz 中闭合的完整 X 跨度，不配对异步 b/e 或 flow，也不把跨度当作 GPU busy 或因果。

A1 最长合格 registered wait 期间，本端均没有新 wire 观察或 game callback；两端源输入分别 11 个到期中 10 个合并、7 个到期中 6 个合并。这些是该选中区间的分母，不能以全已观测成功代理覆盖被合并事件，也不能无证据填补其余事件在何时完成。

[decision](rollback-desktop-gpu-vsync-decision-c1.json)为 190,076 B，命令 96gLTox07u_l2GWnOBAKY4TH exit0、461 ms。最大 Page gap 的 callback 成本严格取 (fromSample,toSample]，与 timeline 用于展示样本上下文的半开 [start,end) 范围不同；下表不混用两种选择器。

| A1 最大 Page gap 的严格 callback 区间 | P1 | P2 |
|---|---:|---:|
| fromSample→toSample / callback 数 | 415→422 / 7 | 367→374 / 7 |
| CPP whole callback wall 总和 / max（ms） | 114.815 / 22.310 | 124.720 / 27.030 |
| corrections / resim 增量 | 3 / 6 | 3 / 7 |
| B 最大值 / network interval scale 最大值 | 2 / 1 | 3 / 1 |

这些最大 Page 间隔不能全部称作 H8 预测余量用尽或主动网络降速。ordinary Save1 子集明确无新增 correction / resim、capture+1，仍遵循既有相邻 callback 定义，不保证只尝试一个 tick；其单 callback 最大 wall 为：

| ordinary Save1 最大 CPP callback wall（ms） | P1 | P2 |
|---|---:|---:|
| GPU A1 | 60.395 | 57.000 |
| GPU B1 | 12.755 | 6.445 |
| GPU A2 | 12.820 | 6.070 |

A1 的联合记录在每个选中窗口按可能重叠量排序，最多保留 8 条已命名完整 X 和 3 条未命名完整 X；这些保留列表中的最大 wall 分别为 0.652 ms 和约 1–2 ms。它们不是所有重叠跨度的时长上界，也没有提供足以确认同量级 GPU 阻塞的证据；不能据此排除图形、OS 或其他未覆盖事件。A2 P1 最长 registered wait 附近有未命名 Compositor 完整 X：wall=17.273 ms、reported thread duration=0.068 ms，与等待存在重叠；它不能命名为 SwapBuffers，也不能把这两个字段改称 CPU self 时间。

三组 setup 窗口均为 normal；晚到的测量快照里 A1/A2 双端 minimized，B1 双端 normal。A2 较好而晚快照也 minimized，因此既不能用该状态解释 A1，也不能将晚状态反推到先前 gap；Page visible 不证明持续原生前景。正式端点到 drain 后已关闭观察之间的 25.21 / 18.57 / 19.58 s 是观测时间差，不是精确断开时刻。

只读 compact inspection 命令 KfiQzLuykVFIp_P8m_Cqt-er exit0，但约 14k stdout 被截断，不能把那次终端输出称完整覆盖；原 artifact 完整且未变，后续 decision 已使用更紧凑输出完整复核。没有未确定写入，也没有重跑浏览器。

三组使用同一诊断配置，但观测开销尚未校准；A1 高负载不自动等同无探针生产开销，也不因此删样本。GPU 单 flag 不采用为修复，且不排除 GPU 链路或认定全是编译/CPU。联合定位已完成；下一步准备独立、默认关闭的 release 粗阶段观测，复用已有 callback 采样边界，先校准再比较，准确阶段范围见第 8.3 节。该观测尚未实施，不是新增生产优化或 DEV live-cost 重跑；此处无新浏览器实验、C++ 构建或手机操作。

计划文件为 3,346 B，命令 OjpPL9IJhJSmAFNDr2OXD5R3 exit0、461 ms；它只证明预声明保存。实测应核对合格 rAF 等待、GPU 提交/呈现相关 trace、双时钟新状态间隔以及实际输入/删失分母，不能凭一次不重现宣布解决，更不是生产关闭 vsync 的方案。仍仅桌面人工触控与 nig，无手机测试，保留所有 A/B/C/回切和未合格 trace 样本。

## 6. 已执行的离线新思路：固定可见前缀预测影子

mobile-prefix-shadow-c1 已处理 **9 份 raw、18 个 peer、H=1..8**，自检与分析通过；摘要提取 mobile-prefix-shadow-summary-c1 也通过，未丢掉不利 run。CV 仍是离线影子，**没有在生产启用**。

这里 H 的定义是：预测目标帧 f 时，只能看到截至 f−H 的完整输入前缀，第二个点最多取 f−H−1。H 表示固定的“已知前缀年龄”，不是该 run 实际回滚深度、ACK lag 或网络到包间隔。算法不能看目标帧、未来输入或手势 phase；动作标签仅用于事后解释。

held 使用当前已修复的保持算法，包括对旧 baseline raw 也使用该算法；所以影子不是重建旧版本当时的预测决策。CV 仅对满足条件的连续 DirectTouch 点做 float-accurate 外推：模式、flags、相关按钮 mask 必须相容；差值、乘 H、加回 anchor 分步 Math.fround。缺少历史、模式/资格变化、非有限或合法范围失败就回到 held；joystick 始终 held。

以下是所有九组的 H1/H8 **完整 FrameInput 数值相等次数**。箭头是 held → CV；P1/P2 分列，不能把这些计数换算成减少了多少回滚或增加多少 FPS。

| raw 组 | H1 P1 | H1 P2 | H8 P1 | H8 P2 |
| --- | ---: | ---: | ---: | ---: |
| direct deep B1 | 122 → 239 | 154 → 238 | 54 → 69 | 72 → 92 |
| direct deep C1 | 122 → 239 | 154 → 238 | 54 → 69 | 72 → 92 |
| joystick deep C1 | 122 → 122 | 146 → 146 | 54 → 54 | 66 → 66 |
| relay39 B1 | 348 → 411 | **805 → 614** | **93 → 92** | **188 → 186** |
| relay39 C1 | 369 → 497 | 462 → 505 | 110 → 148 | 158 → 179 |
| relay39 C2 | 311 → 462 | 526 → 572 | 102 → 134 | 193 → 225 |
| relay39 B2 | 401 → 592 | 498 → 558 | 143 → 183 | 179 → 213 |
| burst120 C1 | 544 → 655 | 365 → 452 | 224 → 255 | 140 → 165 |
| direct120 C2 | 118 → 196 | 125 → 151 | 48 → 63 | 55 → 61 |

这组结果支持“值得继续检验”而不是“已经找到通用解”。尤其 relay B1/P2 在 H1 由 805 降到 614，H8 也略差。直接触控 deep 的 H8、实际 active 坐标改变子集里，P1 仅从 0/589 到 17/589 完整匹配，P2 从 0/545 到 22/545；不能用总体计数盖过持续变化段的命中仍有限。

转向、抬手和 neutral 边界的完整命中仍很少。H8 某些外推相对 held 的目标误差显著增加，例如 relay B1/P2 最坏增加约 80.106 个 TH09 wire 游戏坐标单位；它不是屏幕像素、玩家真实位移或毫秒。抬手后的 stale-offset 也是遗留预测目标相对最后 active target 的偏移代理，不能称为实际视觉偏移。

full 比较涵盖 mode、flags、buttons、x/y 的 C++ 数值语义；+0 和 −0 在数值比较中相等，12B 位差另列。coordinatePerfectFullFieldCeiling 仅表示“保留同一 held 非坐标字段、假设坐标完美”时可达到的完整匹配数，不是 CPU 或 correction 的理论上界。真实 tape 又受世界位置和采样时钟影响；生产改预测器后，不能预设仍会得到同一 tape。

完整 [shadow 证据](rollback-mobile-prefix-shadow-evidence.json)为 8,415,566 B；[摘要](rollback-mobile-prefix-shadow-summary.json)为 614,889 B。小型 [H1/H8 提取日志](../../th09_web/artifacts/multiplayer-tests/mobile-prefix-shadow-summary-c1-process.log)可直接核对本表；完整分母、H2..H7、回退理由和最坏样本保留在 JSON 中。本报告未重新解析这些大 raw。

## 7. TH06MP / TH07MP 的具体参考方向

用户指出 TH06MP/TH07MP 能承受很高回滚深度，是本任务应持续对照的工程参照。但必须核对现代 authored 源码实际启用了什么，不能将默认关闭的实验项解释成已发布性能的原因。

| 源码对照候选 | 有价值的区别 | 下一步等价性风险 |
| --- | --- | --- |
| stable joystick axis | TH06/07 直接归一化轴；TH09 当前路径先用 state.x + stick × speed，再减 state.x/缩放，可能引入随位置变化的 f32 差异 | 即使数学上等价，实际 float 结果和完整输入位可能改变，必须独立核对 native 行为、状态与 Replay |
| 完整 VM 恢复链去重 | 跨相邻恢复记录识别同一完整 AnmVm，可能减少重复恢复工作；目前仅完成审查，尚未实现 | 必须保留 owner identity、pool/unknown 回退、写入顺序、ring reuse、Save 边界；callback / 多态 AttackState 等情况下的整链回退仍待落实，实际命中率未知，不能只按指针跳过 |
| DirectTouch delta 加 authoritative 余量 | TH06/07 的 TakePlayerDelta 与余量进入 journal 的路径，为连续拖动表示提供了具体参照 | 需要新的 TH09 状态/回放/编码与兼容设计；默认 delta 预测年龄为 0，不会自动减少 correction |

TH07 的 maxDirectTouchDeltaPredictionFrames 默认 0，量化默认 0，EquivalentAbsorb 默认 false。不能用这些未启用的开关说明 TH07 已有的高深度性能。common 的 mode3/4 与 TH09 spectator 中 mode3 的含义还存在冲突，迁移必须明确协议、序列化和旧记录兼容，不能只替换一个枚举值。

本轮只读源码对照的确切入口如下；这些引用记录实现位置，不代表候选已经做完：

- 稳定轴：D:/workspace/eagler/th07-eagler/src/Touch.cpp 的 GetFreeJoystickVector（968–987 行）；TH09 的 portable/input/TouchController.hpp:58 与 th09_web/cpp/sdl/Application.cpp:345 对照。
- VM 恢复：TH09 th09_web/cpp/sdl/Application.cpp:512–519、WorldState.cpp:213–234；TH07 D:/workspace/eagler/th07-eagler/src/Th07RollbackState.cpp:564–692。TH09 现有链涉及 journal、逐 VM 与后续 undo/Enemy/Effect/Attack，不能只去掉表面重复的函数调用。
- Delta 与余量：TH07 Touch.cpp 的 TakePlayerDelta（1016–1024 行）、Th07RollbackState.cpp:345；隔离 common 的 D:/workspace/eagler/worktrees/th09-rollback-input-batch-common/include/eagler/netplay/DirectTouchState.hpp:7–31。
- 编码边界：TH09 Application.cpp:571 的 spectator 0/1/2/3 与 common 的 Delta=3、Begin=4 不能直接互通；RollbackSession.cpp:196–205 当前只接纳 None/Joystick/DirectTouch，touchBomb 也另有拒绝规则。

这些都是后续候选，没有在本阶段生产启用，也没有新增性能 PASS。后续还应按实际字段证据调整优先级；下面已完成的固定 stick 摘要，使“稳定轴”只能保留为局部核验候选，不能列为当前主要回滚优化。连续拖动表示和 VM 恢复链仍需各自的最小候选与独立正确性门禁。

### 7.1 新反证：固定 stick hold 的独立离线字段摘要

mobile-stick-stability-summary-c1 已执行 PASS，命令 uk3fNvNJXB4SrZgYg9XxZjRu。**73.4395 ms 是离线摘要程序的运行时间，不是游戏帧时间、native 成本或性能收益。** 输出 [rollback-mobile-stick-stability-evidence.json](rollback-mobile-stick-stability-evidence.json) 为 79,650 B，SHA-256 e55b9efa5e08b3469f419e7f50b0ead11b6a6fdabe7abd593452886bd4caf8bc；[小型执行日志](../../th09_web/artifacts/multiplayer-tests/mobile-stick-stability-summary-c1-process.log)可核对计数。

四个 hold-stick-vector 区间均为半开区间，合计 120 帧、116 对段内邻接；各段 source hold 内 interveningEvents 均为空。所有段合计 **XY-only 数值变化为 0**，即没有发现 mode、flags、buttons 均不变而仅 X/Y 变化的邻接输入。

| peer / capture frame 区间 | Joystick / None 帧数 | 完整数值变化对数 | 按钮变化对数 | XY 数值变化对数 | 区间说明 |
| --- | ---: | ---: | ---: | ---: | --- |
| P1 [276,306) | 30 / 0 | 9 | 9 | 0 | 向量只有一个取值 |
| P1 [588,618) | 30 / 0 | 9 | 9 | 0 | 向量只有一个取值 |
| P2 [285,315) | 30 / 0 | 9 | 9 | 1 | 微小 x 变化同时伴随 buttons 变化 |
| P2 [597,627) | 14 / 16 | 10 | 9 | 1 | XY 变化伴随 Joystick→None 与 flags 变化 |

P2 的 302→303 中，x bits 从 bff99980 到 bff999a0，数值 dx=-3.814697265625e-6；y bits bf666600 不变，mode=1、flags=2 不变，但 buttons 同时由 4 变为 5。不能把它当成“其余完整输入相等、唯独坐标扰动导致纠错”的例子。

610→611 时 buttons 同为 5，但 mode 从 1 变为 0、flags 从 2 变为 0、xy 归零；这发生在 source UP 627 之前，且属于同一个 source hold 区间。它不能命名为正常松杆，也不能混入轴浮点抖动计数；缺少准确 capture 时刻的 position、speed、base/effect scale、readiness 等上下文，原因仍未确定。

合计 37 次 full 数值变化中，36 次伴随 button 变化。相邻 XY-only=0 也不等于 H8 实际预测绝无额外坐标差异：这份摘要没有重建真实可见历史和预测决策，更不能推出能少多少 correction。它不外推所有摇杆或手机；生产 stable-axis 尚未实施，现有证据不支持把消除位置算术当成当前主要收益来源。


### 7.2 重复最终呈现的离线审计：有实现入口，当前不优先

[repeat-present audit](rollback-desktop-touch-repeat-present-audit.json)量化了既有四轮正式窗口。每端的呈现分母均为 1,620；下表区分发生多次呈现的 callback 数与超出每 callback 一次的额外呈现数，不把两者混为同一统计。

| 既有运行 | 多 present callback，P1 / P2 | 额外 present，P1 / P2 |
| --- | ---: | ---: |
| relay c3 | 0 / 0 | 0 / 0 |
| direct c1 | 1 / 4 | 1 / 6 |
| direct-raf-c1 | 1 / 2 | 1 / 2 |
| direct-sched-c1 | 0 / 0 | 0 / 0 |

direct c1 的额外次数相对上述分母为 0.0617% / 0.3704%。最新 direct-sched-c1 没有多 present callback，却仍有超过 100 ms 的合格已登记 rAF 等待。因此，TH06/07 集中 present 的源码差异可以作为实现参考，但现有数据不支持将它列为当前优先修复。TH09 已有 defer/commit 入口，备选范围可以限定为合并最终 blit/swap，并保留 semanticDraw、flush、Save、Seal；本轮没有实现或改动生产代码。呈现次数占比不是 CPU 成本占比，也没有据此估算性能收益。

该审计为既有数据的离线处理，命令 adHi2AAwR6m9er9RV2krD_3X exit0；输出 11,324 B，SHA-256 6b576d0d6e2e23857fabe0d72d18f33be33232efc4939e6891bf9f2c4efc179d。它没有新增浏览器运行，不能消除前述坏样本或替代多端、变化输入的验收。

## 8. 当前计划：桌面双端 CPU4 与持续变化触控的完整路径验收

### 8.1 补齐真实 hosted 观测资格与开销校准

普通开关的 c12/c13 因果对照、owner 装配、c15 的实际 launch 和 c16 的触控 driver 功能目标已有证据。c17 仍有 HUD 边界、手势派发不完整和移动长间隔；c18 未进入游戏，不能为移动窗口提供结论。c19 已实际启动但 warning 观察失败；c20 完成单机输入功能及三段运动局部资格，HUD 的真实 Window blur 边界仍使严格整体资格失败。下一步仅在桌面真实 Launcher 中人工构造触控，独立核对焦点事件、源派发节奏与观测开销，完成预声明负载后再判断严格资格。c16 的未知 blur 来源、所有失败资格、Replay 门槛和 phase 前后 geometry 保留；剩余 select、file picker / SW fence 回归也不因本地提交而算完成。手机和设备测试已取消，不是当前计划的前置步骤。

c20 已在 pinned normal headed Chrome、真实 core/canvas/context 的 native D3D11 门禁、CPU4、dispatch window4 与完整 probe 条件下执行。它补齐本次后端和单机输入通路的证据，不替代双端 relay/direct 回滚验收，也不把 c17/c20 的多变量差异解释成单项生产优化收益。

有效 run 至少需要：真实单个当前 iframe/document/epoch，真实初始化与首个游戏 frame；probe 在 launch 成功后显式安装；owner、hook、getter、时钟、bounded ring 无失效或溢出；完整清理资格与原始输入/日志可追溯。失败或没有 probe 的 run 保留 null/unknown，不生成假的零延迟或零卡顿。

接收队列的集成门禁是在 launch 后，显式让一条既有 resources 请求保持待定，同时从真实 overlay 发活动输入。必须同时看到 pending ACK、真实 pointer/host 提交、对应 native touch 或 controls→stick 顺序和真实新状态推进，不能用“没有任何 native 输入，所以没有阻塞”得到 PASS。这个受控请求不等于正常 Launcher 安装资源过程。

### 8.2 双端 CPU4、双方持续变化输入：独立 relay 与已验证 RTC direct

双方都施加并验证 CPU4，双方的触控目标/方向持续变化；对两端分别用实际采样 tape 核对活动与变化，不能只凭源计划标签确认负载。以下均为桌面人工构造触控，不开展手机测试。 relay c1/c2 的不完整资格保留；新 relay c3 与 RTC direct c1 已完成严格窗口及确认前缀，但 c3 尚非稳定 60 Hz，direct 出现 369.685 / 472.630 ms rAF 长间隔，P2 最大 Page 观察新状态 gap 又为 489.755 ms。已保留两种时钟与原始端点；下一步优先分段定位请求派发、play/sample 与 callback 中的时间空洞，保留同配置基线、输入 tape、后端与观测开销资格，不把不同运行直接称为优化收益。

| 层次 | 固定负载与路由 | 必须保留的观察 |
| --- | --- | --- |
| 精确引擎正确性 | mode0、D0、frontier-delay8，direct/joystick，持续 drag、反转、hold、UP/cancel/recontact、射击与 focus | 完整实际 12B tape、重复一致性、第三世界 world/audit/framebuffer/Replay；按对端 capture 段解释恢复 |
| 桌面 hosted 单机入口 | 真实 React overlay，人工构造触控；独立墙钟源计划，direct 与 joystick 分开 | 实际 DOM/native 数量、被证明的参数范围、队列/生命周期与可观测新状态；单机结果独立于回滚性能 |
| 独立 relay | mode0、D0、桌面双端 CPU4，双方输入持续变化；既有 39±5 ms 单程与 burst 条件分列 | 两端 confirmed 前缀、实际 tape、完整 body、修正/重演/fresh 成本、relay 收发/积压、信息预算与最长新状态间隔 |
| 已验证 RTC direct | 相同双端 CPU4 与双方变化输入，实际核对 selected ICE、RTC transport 和 input/control lane | 保存真实 candidate pair / route 证据；只标注已验证范围，不把测试名称当路由证明，也不把本地 UDP 成功外推 WAN/NAT |

源事件时钟必须独立于 onGameFrame、实际采样与 frontier。计划 60/120 Hz 不等于实际达成频率；分别报告 planned、due、future-censored、coalesced、not-submitted、实际活动 analog、target changes 与新状态数。由 backpressure 或游戏采样合并后的实际 tape 如实保留，不能为匹配计划补造输入。

tasks-c1 两端最长等待都落在没有计划触控 due 的区间，尚不足以覆盖持续移动压力。现已实现 opt-in continuous 源计划与 TOUCH_PATTERN 选择，默认 lifecycle JSON 保持兼容；6 syntax / 38 tests 及同一份冻结 raw 内 P1/P2 两份计划 SHA 比对通过（2/2）。新 continuous + graphics 桌面双端 CPU4 运行已在测量结束后的 RTC 证据收尾处 exit1；离线已核独立 input valid、完整正文、10 个相同确认点及 graphics trace，正文无 >50 ms 新状态间隔，部分合格等待内确有 due / submitted。actual direct 未补填、整体 exit1 保留；这不是匹配输入 A/B 或每帧 active 的证明。harness 周期操作另行只读审计；没有手机测试或生产 C++ 改动。

每个同 run 的双端 confirmed hash 必须一致，并核对各轮预声明前缀：1800 帧为 15 项、1200 帧为 10 项、900 帧为 7 项。不同 run 的实际 tape 不同就明示；不能把源计划相同写成匹配输入的 CPU A/B。

新状态应同时看到 session/frame 推进与 presentation 推进。连续回调或重复呈现同一个旧状态不等于新游戏画面；presentation 一次跳多次时也只能称相邻可观测新状态。记录 p50/p95/p99/max、所有 >50 ms 和 >100 ms 区间、最坏端点及附近原始记录。正文不拼接非单调游标、生命周期间断或漏采区间，startup、尾部和不完整窗口都保留。

> 验收计划：先把已出现的 >100 ms 新状态长间隔作为明确待解决项，任何这类不利区间都不能靠平均约 60 Hz 抵消。低于 100 ms 也不自动等于玩家感觉流畅；仍需分位、连续慢帧、输入活动与桌面人工触控的实际体验共同判断。阈值和窗口在新一轮运行前固定。

### 8.3 归因必须跨完整路径，但每一步有证据

主机有时在编译或存在其它负载，不能一次坏样本就否定候选；同样不能没有证据就把每个坏样本归给宿主。构建、浏览器和大 raw 分析继续串行，保留 host CPU/process 概况与相邻 baseline，明确计数器单位、有效区间、进程新建/退出和 coverage。

后续每轮继续记录实际游戏 canvas 的 GL renderer、图形后端、headless/headed 状态和双端 CPU 限速。c16/c17 等既有 Launcher 样本使用 channel=chrome、headless，未声明 native GPU flags，也未记录真实 GL renderer；此前正式 smoothness 的 ANGLE D3D11 / headed Chrome 153 条件不同。新双端 relay c1 已核实际 Intel UHD / ANGLE Direct3D11，c2 却缺少实际 renderer，资格不能互相借用。c20 已核 current core/canvas/context、epoch=1 与实际 Intel UHD D3D11；记录中的 software=false，新增 analyzer 透传了对应元数据。relay c3 又记录了双端初始与结束的实际 Intel UHD / D3D11 及 core/canvas/context 绑定；direct c1 的实际后端也一致。旧坏样本全部保留；缺失后端信息既不证明已落软件 GPU，也不足以判断生产 CPU4 极限。

按实际证据区分：源事件未按期产生、父层 post 尚未发生、子层任务未派发、已进入 native 的工作、已登记 rAF 请求未进入、网络信息不足触发的速度调整。父/子 performance.timeOrigin 分开记录；未校准时不能直接跨 clock 相减。晚注册的 child message observer 可能在 native 后记录，不能冒充最早 native entry。当前 Launcher probe 没有 rAF 请求 identity，不能由它的残差推出“rAF 未派发”。

探针自身的开销仍需 default-off/on 校准，单列资格。它只观察，不主动追加 native 输入、调 rAF、tick 或 coalesce。条件 render-progress 关联不等于逐事件移动因果，更不等于物理 input-to-visible 延迟。

smoothness 的初始/失败分支实际 GPU 证据与 opt-in RENDER_WORK 10 词数值观测已落地，并随本轮 relay c3 / direct c1 执行；没有生产改动。两组都启用 RENDER_WORK=1，开销尚未校准，不能把图形计数视为零成本。独立 cadence-only probe 已完成源码审查，尚未执行或校准；不能把减少每 MOVE 观察的设计直接记为已获得性能收益。完整 VM 恢复链去重仍未实施，callback/多态 AttackState 回退和实际命中率仍待验证。

既有 RAF_DIAGNOSTICS=1 的 direct-raf-c1 已完成，完整窗口与请求映射证明本轮两次最长间隔包含 124.145 / 117.885 ms 的合格进入前等待。最大等待内缺少连续到期输入，timer 观察不能单独给主线程忙闲结论；另一段 67.705 ms 等待内有真实 timer/native input 执行，但不能代替最大等待的证据。相关时钟、分母和完整身份见第 5.6 节；开销未校准，不作速度 A/B。

desktop-touch-both4-direct-sched-c1 的不合格 trace、未知 dataLoss、末次 input buffer=56 / 56 和全部游戏统计继续保留。诊断修正后的 desktop-touch-both4-direct-tasks-c1 已完成：实际 direct / GPU owner、1200 帧的严格窗口、10 个确认点及 tasks trace 资格均通过，旧完整性/时钟/timeout 门槛未放宽。两端仍有 163.595 / 157.535 ms rAF 间隔及 158.005 / 148.520 ms 合格请求等待，同窗分析已得到主线程闭合 X 任务覆盖上下界，并观察到 GPU Process / CrGpuMain 长 RunTask 与最大等待高度重叠；针对这些旧长等待，仍需保留 GPU 进程任务/呈现链路的后续验证，仍不把任务 wall 当 GPU 运算或确认瓶颈。continuous 人工触控与 graphics 三类诊断已实现并通过预检，新运行在测量结束后的 RTC 证据收尾处 exit1；保留旧 measure，离线核实完整正文、10 个相同确认点和有效 input/trace。短 SwapBuffers 区间与部分等待相关，但未解释旧约 150 ms 长等待；pause 后 Pump 继续运行导致 drain 后证据清空的链路已由源码核对。harness 的 drain 前小端点/RTC 异步摘要修正已实现并通过 6 syntax / 51 tests，非原子捕获、passScope 与独立 teardownClean 边界保留，direct c2 已通过测量与历史 actual route 资格，但晚尾 teardownClean=false、P1 几乎无回滚的限制保留；随后 continuous relay39 c1 在双方有回滚工作时正文无 >50 ms 间隔，约 58.5 Hz 的速度差距不能全部归因网络，burst160 随后通过测量资格但仍有 60.505 / 90.335 ms 间隔，信息余量耗尽与分 callback 恢复的证据已保留，后续补尾 B 已通过构建、深 8 帧三世界与实际 App 控制门，但 B burst 与原版回切未证明性能收益，隔离不采用；C 在完全独立于 B 的原版基础提前一预测帧制动，构建与四组件通过，普通 relay 约 54.4 Hz 的不利结果保留，C burst 已完成且仅约30.9 Hz、最大间隔181.775/224.050 ms，C也暂不采用；原版普通relay回切也已完成，约55.2Hz并出现Page435.625/493.565ms间隔，P2有477.630ms已登记等待和输入供给断档传播证据；C后半段普通callback高耗时另行保留，不把三类问题混成一个原因。九组双时钟/输入分母汇总已完成，GPU单flag三组已按预声明执行完毕，支持代码冻结、3 syntax/60项纯测试门通过，A1已完成且约34Hz的坏样本保留，trace资格经v2勘误合格但晚尾teardownClean=false，B1已完成且约56.5Hz/rAF无>50ms但P1 Page max53.795ms，晚尾仍不clean，A2回切也约58Hz且rAF无>50ms，A坏→B好→A也好，参数可逆关联未成立；三组双时钟、逐事件比较、六份trace联合时间线和decision均已完成；A1最大Page gap内的网络B≤2/3且scale=1，ordinary Save1最大callback达60.395/57.000ms，选中GPU完整X未提供同量级已命名阻塞证据。当前准备默认关闭的release粗阶段观测，尚未实施，先校准runtime及与原A的来源差异；第 5.10 节保留整体失败与缺失 route，不宣布生产改善。tasks-c1 不采集 compositor/GPU 详细类别，对应 coverage=null；短窗口和新观察配置不是速度 A/B，也不代表流畅性通过。第 7.2 节的重复 present 审计没有支持当前优先实施该生产备选，次数比例也不换算 CPU 收益。

#### 呈现调用链核对与未实施的备选

根已核对 [Renderer.cpp](../../portable/sdl/Renderer.cpp)285–296 行与 [Application.cpp](../../th09_web/cpp/sdl/Application.cpp)610–647 行：最终链为 Renderer::present→自有 batch flush→commit 的 glBlitFramebuffer→SDL_GL_SwapWindow；这里的 flush 不是 glFlush/glFinish，stats.presentations 计提交调用次数，不是物理 scanout。

现存 SDK 文件为 D:/workspace/eagler/th08-eagler/tools/emsdk/install/emscripten/cache/ports/sdl3/SDL-release-3.4.2/src/video/emscripten/SDL_emscriptenopengles.c。其 47–66 行 SetSwapInterval 调整 classic MainLoop timing，141–148 行 SwapWindow 仅在 Asyncify + hint 条件下 sleep(0)，未直接执行 D3D swap、glFinish 或 WebGL commit；TH09 Application.cpp614 行却使用独立的 emscripten_request_animation_frame_loop。因此不能把 SDL 调用与 Chrome GPU trace 的同名 SwapBuffers 当成同一次同步调用，也不能把 SDL interval0 当成这条 rAF 链已知有效的开关。该已安装 SDK 路径与 TH09_EMSDK 根相符，但不是冻结 WASM 的完整链接 manifest，结论保留此限制。

GPU单flag诊断现已正式预声明为A0/B1/A0（第5.18节）：使用原版A Wasm，三组同一冻结新driver、双CPU4、continuous源计划、普通relay39与graphics trace，仅DISABLE_GPU_VSYNC=0/1/0；核对两端实际argv与独立PID，禁止disable-frame-rate-limit及相关非法=变体。旧23790f44 driver字节已归档，测试driver/helper已冻结，3 syntax与60项纯测试通过；A1=0已完成，约34Hz、最大rAF间隔254.510/290.525ms仍不达标，v2测量资格通过与晚尾teardownClean=false分别保留；B1=1已完成，约56.5Hz且rAF无>50ms，但P1 Page max53.795ms，晚尾仍不clean；A2=0回切也已完成，约58Hz、rAF无>50ms，晚尾仍不clean；A坏→B好→A也好，未建立参数可逆关联，因此不采用为修复。三组rAF timing、双时钟/逐事件比较及六份trace联合时间线/decision均已完成，不再把这一组测量或分析写成在途。当前准备默认关闭的release粗阶段观测，方案如下但尚未实现；诊断开销未校准，不将A1高负载自动等同无探针生产开销。当前优先继续核查单端rAF未派发/供给断档传播、普通native callback变慢与网络信息不足，不能提前写GPU已修复；B/C均暂不采用，原版回切与C坏样本共同保留，不盲增H、固定输入延迟或把关vsync当生产方案。

#### 下一步：release 粗阶段观测（方案已确定，尚未实现）

主代理已小片回读 PerformanceDiagnostics.hpp：详细计时只存在于 TH09_DEVELOPMENT_HARNESS 分支，release 宏全部擦除，不能从 A1 的冻结 release raw 事后还原 60/57 ms 的阶段分布。现有 exclusive 是父墙钟减去已计时子项，不是 CPU self；不能借用 DEV 成本标签把未覆盖时间改称 CPU 忙。

拟新增独立且默认关闭的 release 粗阶段观测宏，保持 TH09_DEVELOPMENT_HARNESS=0。次数/ms 存在固定数组中，按 callback serial 一次发布；JS 通过已有 onGameFrame 读取短记录，不为每个阶段跨语言发事件，不加 GPU query/readback 或逐帧 CDP。原 loop、输入、present 和现有 ms 起止保持原样。以下是已核对的 [Application.cpp](../../th09_web/cpp/sdl/Application.cpp) 插入位置与解释边界：

| 位置 / 阶段 | 拟观察范围 | 不能误称 |
|---|---|---|
| rollback_tick，485–509 | transport、capture、deferred flush 分阶段 | 不能把全部时间命名为回滚逻辑 |
| checkpoint，101 与 102–111 | 两级前置 flush，与 Save+附加字段分开 | 原 checkpoint 计时含 flush，不是纯 Save |
| tick_inputs，251–268 | Update/UI/world 整段及已明确子边界 | Update 不是纯 world |
| draw，289 与 291 | semantic draw 与 Present 调用分别计时 | Draw 可能包含 GPU 提交，wall 不是 GPU 运算时长 |
| rollback_tick，554–557 | Seal/Complete/hash | 保留原事务和发布顺序 |
| rollback_tick，563–578；callback，644 | confirmed 发布/音频，以及尾部 audio | 不把同步桥或音频成本隐藏进未命名逻辑 |
| sample_keys，326–350 | SDL_PollEvent、JS 键盘 getter、触控采样与转换 | 不是纯本地键盘采样 |
| warm_resources，284；调度簿记 | 先保留未分解余额 | 余额不是 CPU self，也不证明线程一直忙 |

实施前先预声明校准：在 CPU4 的正式 body 之前测空阶段/时钟开销，再用同一诊断 WASM 的 runtime 计时 off→on→off 衡量真实观测开销。冻结宏、flags、source、WASM、loader、schema 和 collector SHA，核对与原 A 的差异；保留原完整窗口、实际输入、确认 hash、正确性门、所有缺口与溢出，不能只挑低成本回调。未测就不声称该探针低开销、优化有效或目标达标。同一诊断 WASM 的 off→on→off 主要估计 runtime 记录/读取开销，不能证明加入宏后的代码布局和编译产物与原版 A 成本等价；原版 A 保留独立冻结来源控制，只有该差异构成剩余风险时再加针对性对照，不把开发构建或启探针后的慢自动视为真实生产成本。

这项方案优先回答 ordinary callback 墙钟花在哪些阶段，避免把本地产出中断伪装为网络降速。VM 最老 preimage 的 metadata shadow 是后续候选，条件网络制动再后；二者既有保守语义门槛不变，不为提高命中率放宽观察者合同。

并行保留两个有门槛的源码设计，均先做影子审计，尚无实现或收益：

1. **完整bullet VM跨记录最老preimage去重。** 参考TH07，先仅用metadata统计eligible恢复链与可省bytes。正常CharacterAttacks.cpp会创建非空多态AttackState，保守整链gate遇到saved/current任一非空state或AfterRestore就退回，因此实际战斗准入可能很低。不得为提高命中率放宽观察者语义，也不重复已落地first-write pages、背景登记、enemy索引或owning容量工作；可省bytes不能换算CPU收益。
2. **有条件的临时提前网络制动。** 保留原版(B−4)主规则，仅在A停滞、F另进2帧且经过2个计划周期、重演债务R=0、RollbackFrame()==INVALID_FRAME无待处理rollback request，并且本次Pump的PacketsReceived无符号before/after差<256时，影子评估临时用B−3。这里“无request”不是重发需求；Pump未用满256也不能证明浏览器没有尚未派发的message。正常3帧批量可能误触发，并经150ms回落累计减速；paused不能当作network.active=false，需独立活动重置，持续MOVE引发的rollback也不能永久清暖机。没有Pump时间序列的精确观察就保留区间未知；固定原F/A影子不叫反事实FPS，更不据此直接编译新候选。

如果证据指向网络信息不足，目标是保留 D0、在预算内平滑调整推进速度；如果证据指向恢复工作或主线程长任务，就继续减工作量/定位阻塞。离线影子仅给候选方向，必须经过生产正确性和新的实际线路运行后才可进入“有效优化”结论。

## 9. 可审计文件、身份与运行索引

### 9.1 本阶段源码与证据身份

工作树：D:/workspace/eagler/worktrees/th09-managed-touch-4x；分支 experiment/managed-touch-4x；起点 commit 为 63127575d43ea5ef35b317ff8137b00a1ab5b1cc。冻结旧 baseline 在 D:/workspace/eagler/worktrees/th09-mobile-rollback-4x。本阶段以下身份使用精确文件 SHA，不把候选文件称为已经发布的版本。

| 对象 | SHA-256 |
| --- | --- |
| 旧 managed.mjs | 1f4ef1810d58c76c6fb85a1032257af5d49ab09f81de56a1114eae2dc7a76608 |
| 候选 managed.mjs | 830f521ffa13c3be443c8bb277da510734c9625efcb224b1d1ed6ae3e7eee4f4 |
| managed-touch-queue.test.mjs | 9fef92e2f198dd77f87dfd8daaeaac638d2e51eae14acb4475a25ebeb4f67151 |
| 独立 Launcher AppShellProvider.tsx，8,435 B | c22af28f30ad6fadf837dc2971be24f9e3ec6eacb31c4fb41260f6a4ff788932 |
| 独立 Launcher 正式 build artifact identity | 1d607295335cf04a3877b560a81608bb62b2956e676579e3feb56d73b4b22960 |
| 同 HEAD 原 AppShellProvider 的 fresh baseline UI artifact，106 files | b8287322182c4d4032740dc572e5438fa872179b87511d3063d3e1ba6279a0ef |
| 既有 SW service/client 测试脚本 | 62ce5d1efc25fcf917b741747f272fddb9feb350370491e5849852b3bcff994d |
| 原生 TouchController.hpp | d70c7ce6890f7693f9d51cb601bee9d9cd30336bb15147c5570772e8f83c8028 |
| touch-controller.cpp 组件 fixture | 23c9175f20155ebe363e1a80e2538a07a124dfae0ecc8d970c3365e6d156c985 |
| managed-touch-launcher-probe.mjs 此前冻结稿 | 91a5b58591a62adc02548692c3a9b2ce282773c1d29caf52da13463541ce6d0b |
| managed-touch-launcher-summary.mjs 此前冻结稿（不冒充 c16 执行版本） | 592966231464282705f5ff116f52066aabdc5459c0702efd6a37e3eb81d6e7dc |
| c16 执行 driver | 4a146af5ab1bbc5be95bb1ec7deb817711b730484e4d2eb5bad2ee56d5223030 |
| c16 实际执行 analyzer，544 行、42,404 B | ee1186fc978050f05d8be86c08a95aac83117af966b1e4fd800aed951d91638e |
| c17 执行 driver | 3d19c97af05d1c64865add9f2d33784c2c1ac9b42ad58c8a043cdd0d0a8ba165 |
| c18 执行 driver | a2230862e3ba06bf9021a26bf82c37a802cef8c76f6278ef8d190d3da33445cc |
| c20 执行 driver | 4e7d7a28cd8c15970f4d4a3017b3e17552b807937e4751c4bd31e46e921e6f38 |
| c20 实际执行 analyzer，仅新增 metadata 透传 | 3a1f351ea689fb82c9b2c1493ca3a2b98c4c5234a16cf0970bc32e7b5f2f492c |
| relay c3 / direct c1 / direct-raf-c1 / direct-sched-c1 执行 smoothness-browser.mjs，714 行、69,739 B | 6d400d7906a6e3af1d988739f95ba534a0b28a8c64133aa475a5be22255a4ce5 |
| rollback-desktop-touch-both4-relay39-c3-evidence.json，115,114 B | 9a7f7b748f6177b571852471ea236a55039561762e855fb4f7552720ef6b7083 |
| rollback-desktop-touch-both4-direct-c1-evidence.json，114,773 B | 24f43f33c1785a4821a0cacd798e4406dfd4fd09f8f35adacb7873ffe86a894f |
| rollback-desktop-touch-both4-direct-c1-gap-context.json，61,188 B | 0339d5c59f4f88e728f2d23410b52f2120e09cbdaea03c95029f3e5e55486bea |
| rollback-desktop-touch-both4-direct-c1-gap-clocks.json，120,735 B | 5d84d8433680b4d2c7382d3e42cc9d8ef320d2c6dd6e78a404c5a5200c579260 |
| rollback-desktop-touch-both4-direct-raf-c1-evidence.json，114,867 B | 69c040896ed8d3f41afb831ea4e65c0890a3644476a76fe47872c632c55d12c6 |
| rollback-desktop-touch-both4-direct-raf-c1-timing.json，493,445 B | 1a26e3ab69e0c0a6552542a55a61e331289afd5b3a83f432e5fffc3115881412 |
| rollback-desktop-touch-both4-direct-raf-c1-timer-context.json，93,838 B | cbd307c0faf4f195b5837433195e0636dd13c78e41b8a23764bc885e3e24a7b8 |
| rollback-desktop-touch-both4-direct-sched-c1-evidence.json，114,915 B | 2c60f2458ed22ad0fb58347e69f425c6b0c83dd0a1192993e49f977b05ecdb8a |
| rollback-desktop-touch-both4-direct-sched-c1-timing.json，472,555 B | 905b6c860937b188d2615d4050e10aa324a56dfa22a584015b81d4c352d3fe55 |
| desktop-touch-both4-direct-sched-c1-P1-scheduler-trace.json，25,668,050 B | ad1a333b617b145ac8c501e83b3ec25de1696aac74d985a9facd48a3e6adf296 |
| desktop-touch-both4-direct-sched-c1-P2-scheduler-trace.json，26,982,597 B | 1ae09a3f2e1079a4a5b364609979e98d363d5286a14a631ace630c21c84fe7ab |
| rollback-desktop-touch-both4-direct-sched-c1-trace-metadata.json，16,922 B | d4827a8206eea6b33ee83d0aa48e190f4d0aa914df673804010e779032160821 |
| rollback-desktop-touch-both4-direct-sched-c1-trace-limits.json，21,213 B | ca737018ba09538cea374fe84c6a35094066a1dcaa79d6b69f0e282e2aca24eb |
| tasks 诊断版 [smoothness-browser.mjs](../../th09_web/tests/multiplayer/smoothness-browser.mjs)，731 行、70,664 B | 46b7e954f03c5cc2d6f9ccf9b35db29187d3fa9529dac7c541393c21c14ec7c5 |
| tasks 预检版 [chrome-scheduler-trace.mjs](../../th09_web/tests/multiplayer/chrome-scheduler-trace.mjs)，353 行、25,600 B | b7d8d236ab7699d3b97c66f957a86da68aae334fdf9f7e1b0e37a38d3fd0eaec |
| tasks 预检版 [chrome-scheduler-trace.test.mjs](../../th09_web/tests/multiplayer/chrome-scheduler-trace.test.mjs)，23,558 B | f01048efebfa2876589ab39375c3e648cbba0ff6e30884f0d2d01be6f3299ce3 |
| c1 预检版 [chrome-scheduler-finish.test.mjs](../../th09_web/tests/multiplayer/chrome-scheduler-finish.test.mjs)，7,897 B | 344a41f1046387a30e05e580e4a7716138a145384ebe6386b9eaa7c8279a4891 |
| desktop-scheduler-tasks-c1-preflight.json，2,903 B | 91c038cae07a0258e04e9c7e57215eb6b171b7ac909aa5c102cab11ce56b04ce |
| desktop-scheduler-tasks-c1-preflight.log，3,724 B | 515a1ebd4d12b364366a1e9d7db7f72ad1b280347d792507e939f01d5ca1befa |
| continuous/graphics 版 smoothness-browser.mjs，734 行、70,924 B | 2998a975240915b1ceb7a4dc015cf0471328ed9e9565cf14eea52f83986c634c |
| [touch-input-probe.mjs](../../th09_web/tests/multiplayer/touch-input-probe.mjs)，256 行、20,658 B | 2fbec4adb37f80acd74745a69366c3ab77e510d5a259a10ae105c0c1dd0df785 |
| [touch-input-probe.test.mjs](../../th09_web/tests/multiplayer/touch-input-probe.test.mjs)，179 行、13,370 B | a3f626c631387e7d4c0e55c4a3aab471881fc88576434d1f9a0e27d0b015f2cb |
| graphics 版 chrome-scheduler-trace.mjs，357 行、26,116 B | 368f6a7e3f9315f01852547f5657d0857afa6b97e678d26ec128e9059c96b273 |
| graphics 版 chrome-scheduler-trace.test.mjs，344 行、28,149 B | db5d32db4f472f383de05048b13e174f64665ca83de03baa53c860e4a0af44f0 |
| desktop-continuous-graphics-c1-preflight.json，9,628 B | 718d3286f0cdeb07df249f2123c44580e1ab16aa02c17207edf47a0fb685c689 |
| desktop-continuous-graphics-c1-preflight.log，5,528 B | 06c55fe598131ab0951c4d57a748543527ab3e13e9c22244eab514338a92e97d |
| 端点 c2 版 smoothness-browser.mjs，805 行、79,424 B | 23790f4409b8ec835d47f21c1d8293280acd37f084d1ba8b7d94d6a1b04bf1ff |
| 端点 c2 版 chrome-scheduler-finish.test.mjs，18 tests、18,277 B | b2daf00b7d8350e612313fd2021b004f90f6f5aca57bf6dd18ba9c149cb5cb00 |
| desktop-continuous-graphics-c2-preflight.json，9,654 B | e3f624de1d37ea246213b34f5713fc6df31d0f08546ccb11e9056c7c0216b626 |
| desktop-continuous-graphics-c2-preflight.log，7,128 B | b210aa1b151a274763097f31703ce54237203f4eea874d61e72c856075016dc7 |
| desktop-continuous-touch-both4-direct-graphics-c1-failure.json，71,766,293 B | 9c7ffe42fab18919260f6a9811d1c143e1992138957a46b667c08b7dfdc64812 |
| rollback-desktop-continuous-touch-both4-direct-graphics-c1-evidence.json，105,635 B | 81b624c088bfb436cfd48f773e3eacd49edd3ac0366b563628f0d5f5408f53c2 |
| rollback-desktop-continuous-touch-both4-direct-graphics-c1-timing.json，478,705 B | 7cb1d9db0a28880af8a31dbff9c67d7896a3783d0c790802329c002d43d910ed |
| rollback-desktop-continuous-touch-both4-direct-graphics-c1-trace-context.json，83,958 B | ddf6059a44d530a040003a858cb272b0306bd83786b0aad1be1acc647ee30c3b |
| continuous / graphics c1 P1 scheduler trace，45,036,852 B | c3c366ce355deeca3de986d075bf676af6e8ca273ebabb3034a31a8803d2b2ae |
| continuous / graphics c1 P2 scheduler trace，45,052,301 B | f0a905656eaa76274d648c8f98c17c0996fadd3beb26f65ac6835fa5a4f5312a |
| desktop-continuous-touch-both4-direct-graphics-c2-report.json，31,015,079 B | d34892775465704c27aca89245b2c6adda904f97fa677f35f2ca8b3d0c2795f6 |
| rollback-desktop-continuous-touch-both4-direct-graphics-c2-evidence.json，116,463 B | 6d7302d7540f6c323f8c19ea05ca5eb1878ad91d341a7e3019325b6080c69923 |
| continuous / graphics c2 P1 scheduler trace，41,657,328 B | fdf40e84df7879b0d8a88a1111affe7ffeb4bacf018f8596f6005a290c553e4c |
| continuous / graphics c2 P2 scheduler trace，41,245,968 B | a75e71cb4e7ddd943e5ad1bdd85791e098b2e9cddf6d4b7fa006bc42c3d9fc0b |
| desktop-continuous-touch-both4-relay39-c1-report.json，32,241,884 B | 2f1a14fadc9c5007b19d15f4eee65a662788e473a489060623df2ae67f9ce220 |
| rollback-desktop-continuous-touch-both4-relay39-c1-evidence.json，109,464 B | 892273d8b8b0e78662865abc222463a7f291f551824e45940eb7bd75a9a05eee |
| desktop-continuous-touch-both4-relay39-burst160-c1-report.json，32,647,281 B | 24fe21737131713f372ef01b3d260e964861226bbe314bd0ee3a948acb01c9b7 |
| rollback-desktop-continuous-touch-both4-relay39-burst160-c1-evidence.json，109,596 B | bf5e71fb5677adeb02a5d44ae17ec0570aed13b4f6958a1d8a252d095f9369bd |
| rollback-desktop-continuous-touch-both4-relay39-burst160-c1-timing.json，577,848 B | 6eef34cf9190ff8006310931d74b69207c684ab62f5a109bbfe88930c4ac9f52 |
| rollback-desktop-continuous-touch-both4-relay39-burst160-c1-gap-network-context.json，122,745 B | 9611fa083a8c9e2ecf4d8f7e6ba08e377492759c2cc0578f280f851510158c82 |
| rollback-desktop-recovery-tail-base-identity.json，49,519 B | 6f5d4331e83c5c3240c2070a18bae0a3e24983cbc72bc3d60f8b52a16acaa174 |
| rollback-desktop-recovery-tail-plan.json，2,364 B | 2052535b4c174c788aa2513e4360d155f8b1d3c127907d10906d17bb04d07675 |
| B Application.cpp | 5a8580c706cde19b8ea411bbb4f54ccae776524e5971f7c658c58e8bb94a066b |
| rollback-desktop-recovery-tail-build-identity.json，7,956 B | 7ff1dff8c6b3326439deb5250d6f8c177016536bf48ece20525bae15007ddd1b |
| B release th09.wasm，2,828,005 B | 1060150871b6ce658a131f9930c28cc0504532a16c441f404ba222bc363d551c |
| B DEV th09-presentation.wasm，3,155,633 B | 866f8238480a8b65e96a37d765887f570257225af52c45924bbe0ef762128c9c |
| desktop-recovery-tail-joystick-h8-oracle-c1-report.json，1,912,992 B | 942aa23bd1f6635f3df74360a7e74eceed421a021c927d6ff761dbb1e1ee3c99 |
| rollback-desktop-recovery-tail-correctness-evidence.json，8,218 B | c028edb6dc020ca392563e7eeaf14b3eebedb814508dada0c3a654fbac6cb580 |
| B oracle complete Replay，10,636 B | 3ec779cbeb310057262a719e532927230943ca427f4ef3df03189fe38790653e |
| recovery-tail-control.mjs，23,204 B | 1bc4876c5d535d72f770b19456d8d6694c511d1b9ed937929e44e99ddb9fc297 |
| rollback-desktop-recovery-tail-control-evidence.json，23,375 B | 1c047280da706c50e22bb44757c1eb767557d3ccdc9f193c177642f30ebd3b74 |
| desktop-recovery-tail-both4-relay39-c1-report.json，34,828,685 B | 4d43591b230a9de653f146091695f56fc27ea03d55fb04b8a91126ef2cb5b4b5 |
| rollback-desktop-recovery-tail-both4-relay39-c1-evidence-v2.json，110,282 B | f1214fde3ea0ad8d1b37626d63f8a8c13036fc415384249f93742367093e9f38 |
| desktop-recovery-tail-both4-relay39-burst160-c1-report.json，34,008,735 B | b73fd742241d37f256028bcd013268c9ef6bf66d514ab76f9111c2802fe4f6ff |
| rollback-desktop-recovery-tail-both4-relay39-burst160-c1-evidence-v2.json，110,228 B | 3b5b7af310718c293bf4b4b6719403f07301d42f817f6e7390e13e636a092cf4 |
| rollback-desktop-recovery-tail-both4-relay39-burst160-c1-gap-network-context.json，118,879 B | 0f78f7a5aab026186e0815dbce1701a160ea64f0e4440d1f87a0e03c5dc6e37e |
| desktop-continuous-touch-both4-relay39-burst160-c2-report.json，35,669,804 B | 54974090a3dab5ad47d1c010d440cadb6a758a3ac7a09631bd02d7584760878c |
| rollback-desktop-continuous-touch-both4-relay39-burst160-c2-evidence.json，109,680 B | 76e2f950894e766c2a4dbedacfa39a1c9dc52ca0804b6b39f36361d46c272aff |
| rollback-desktop-continuous-touch-both4-relay39-burst160-c2-gap-network-context.json，108,819 B | 62691270048038b8a4339f4b3b18e490a9f6b1171f1c04999ed0d7e4aac69a2a |
| rollback-desktop-network-brake-plan.json，2,156 B | 1587c7d426689f7b0eaeed92529f3f6b4b82ef55b245c9591a455aede8d23a57 |
| C RollbackSession.cpp | 198776ef3209f4d02c6954f4864648d0308355bd4477ee937e7924a26b47709d |
| C session.cpp 合同测试 fixture，20,660 B | 57ec08b06a51deec5197e63281d0afbfb2f24218257cbb1592413683f46bb30f |
| rollback-desktop-network-brake-build-identity.json，4,252 B | 8f8adb99467045e6c2b4a972f33a764579df202602593f4a844ab0d1df0363b5 |
| C release th09.wasm，2,827,901 B | f0e04a7069fe3aeaf4ab52018aa8d1b944811cad0cc7b4dc17d9f9d7e1b9ae7b |
| C release build.json，87,055 B | d0413f4b1afaca0c8932d349e5341f437d77063aa8222b253a895915866e182d |
| C 四组件 runner | 9f459ccd0874fcfcc523f3f85bc7672a4dee47274a4dc9e1d0810c23cb17c294 |
| desktop-network-brake-both4-relay39-c1-report.json，35,114,817 B | 65b8f84ae53793e5ee982c4b0af2d8b7ebd70aa20eb091f03b238bf65e6f4bae |
| rollback-desktop-network-brake-both4-relay39-c1-evidence.json，109,695 B | f8593115d83f22f600909d73510884773da13c764a3dd9765c179302a81b2cd2 |
| desktop-network-brake-both4-relay39-burst160-c1-report.json，33,516,070 B | e02f6dbb7414ccd713ab45658dd84e54412611d506fcb73cc250ad9d21e74969 |
| rollback-desktop-network-brake-both4-relay39-burst160-c1-evidence.json，107,859 B | 729c4887008d8f19a7a84c07ba02615c30c9bbb878cbcb2714202613bc43a6ab |
| desktop-continuous-touch-both4-relay39-c2-report.json，34,989,227 B | 72ed169c373cb8e1ba4f1dcfaed7c9da677bfec6eb5996057226876a6625a3a6 |
| rollback-desktop-continuous-touch-both4-relay39-c2-evidence.json，109,473 B | 329dc8cfeeb5e4fe5db079e5b787d91f8299bff0e6214b87f549da21851db0ae |
| rollback-desktop-continuous-touch-both4-relay39-c2-timing.json，911,129 B | 47c3d007249fc859982ad0edd3fd023138f4f649ca7ca514a27fefc714a63be2 |
| rollback-desktop-continuous-touch-both4-relay39-c2-peer-propagation.json，117,164 B | fba5383c7ebf97a8dca0e2ebe2919cb12ef4d1ffda2aa3a1d20a2f16ab031483 |
| rollback-desktop-continuous-touch-both4-relay39-c2-input-gap-denominators.json，45,344 B | d1ac71c6244c8829a981399e21f90dbd965701a2949d2c0b7873759c691020eb |
| rollback-desktop-continuous-touch-comparison-c1.json，312,716 B | 9ecebbb86f68b6a42f76f659767cdd9fc4d780c7e4668f5f29c9657c470abd56 |
| rollback-desktop-network-brake-both4-relay39-burst160-c1-timing.json，538,578 B | bf71529652a0c8384e29807d2292510d687b0e63f145cc74b09b1ddd2621fc5b |
| rollback-desktop-network-brake-both4-relay39-burst160-c1-adverse-context.json，36,286 B | a64af87e6a793450b4c96df6b172e1beacd149acd5a82ffd2813f9e089b88d6f |
| rollback-desktop-gpu-vsync-plan.json，3,346 B | f5fdfde58e12c61010c5c65f78652d08193b90979f6b8e0ca9ab4d77609893b1 |
| smoothness-browser-before-gpu-vsync-23790f44.mjs，79,424 B | 23790f4409b8ec835d47f21c1d8293280acd37f084d1ba8b7d94d6a1b04bf1ff |
| GPU 单 flag 冻结 smoothness-browser.mjs，818 行 / 81,690 B | d899f3cf53ab6b769ccb6df7ae86e9e1aa28b091cae8ef60ff3fddc66ad27001 |
| gpu-vsync-control.mjs，64 行 / 4,192 B | d41c9213c38555d89d97196dac73c58b36d35ad47d76dd409c97468527bb3083 |
| GPU 单 flag 新增契约测试，79 行 / 5,512 B | 1f6a488f56f7df19857813312949f58bd419e17f27aac10a8318854f98d6a9f0 |
| rollback-desktop-gpu-vsync-preflight-c1.json，4,980 B | e44bc523a8d186bd7a7c4d3a4cdd9c1ba0cc2095749214c569958776e538095f |
| desktop-gpu-vsync-preflight-c1-process.log，7,257 B | 1469bfa99f21b7dc0d7c7ad58ff97226ad29604e85e0e1d37cf0cc476ca37cf9 |
| desktop-gpu-vsync-both4-relay39-a1-report.json，39,201,425 B | bf2cdc5c3def238bfb8cb150fc4660b7bf434c3d62079513c9825140103231b6 |
| rollback-desktop-gpu-vsync-both4-relay39-a1-evidence.json，114,063 B，资格勘误前保留 | ada720b81e3081be58085f441e23dd159e3fd43aba8ce61262b3b40fa3c5addc |
| rollback-desktop-gpu-vsync-both4-relay39-a1-evidence-v2.json，114,818 B | 0b4e88fbd4d744b5b5bd35a25985f043892b0cde618d3cd53aef8c9367c76b7f |
| desktop-gpu-vsync-both4-relay39-b1-report.json，38,258,240 B | 4be42dd9d2efe666b81175b6f453c8507f69e54f685e7ca8f45de1e9163e6764 |
| rollback-desktop-gpu-vsync-both4-relay39-b1-evidence.json，114,266 B | c46225627ca11d5453c06a0f2baf7c5bef3a6bca3fb048fbc33c077de7250160 |
| desktop-gpu-vsync-both4-relay39-a2-report.json，39,168,047 B | 3bcc7978288707e56e1603f7e2b30d4a3df942644d0b3e298dc08318affea3bf |
| rollback-desktop-gpu-vsync-both4-relay39-a2-evidence.json，114,119 B | cad3404053d183417c9a399920f8451f992a6f958507c40a2534ad52a754d722 |
| rollback-desktop-gpu-vsync-both4-relay39-a1-timing.json，549,538 B | ac88efd2c8fee78c9192dfed4b69537ef5d1ca48ad55c31c74dedb3029037037 |
| rollback-desktop-gpu-vsync-both4-relay39-b1-timing.json，513,716 B | 556a62d877a75c7a928d05cbbf319f4eea0ced6a1961e29ff2a79b7e3580301a |
| rollback-desktop-gpu-vsync-both4-relay39-a2-timing.json，513,280 B | 69038ef2dcc89489d24dc864e175f9298407caafbca11d8d5a80f41e49c838a7 |
| rollback-desktop-gpu-vsync-comparison-c1.json，115,833 B | 07de0d47711f58848f9e57e098dbc7b1620b7f0ea011d6fb51e32aeafb0a9ca9 |
| rollback-desktop-gpu-vsync-joint-timeline-c1.json，829,386 B | cd547d95986043945451a4a45e8c80b9173dd82b911b744846e9093bed4ef1e9 |
| rollback-desktop-gpu-vsync-decision-c1.json，190,076 B | be96ba1027b018f2ca613cbb255642169d35518b550c5aa4c73326e738c0a0aa |
| continuous 源计划 P1，14,301 events | 9b3bbc4d7772fdbdcc0a17399a04b5ed84464340cd652075eab566efa340b810 |
| continuous 源计划 P2，14,382 events | fc932b82b29d4d8b0d7124ffc29f0f1f96de329118381d8464b138a4875bb2ca |
| desktop-touch-both4-direct-tasks-c1-report.json，29,685,068 B | c19307e11c86183f3d6816078814a7da6cc091874ec77990cdd16bfdc42adb54 |
| rollback-desktop-touch-both4-direct-tasks-c1-evidence.json，114,346 B | 2109dd9dd16514e830f5b0da75fe57197dfc295c4a5fd761d0eb148f6c0e4bc9 |
| rollback-desktop-touch-both4-direct-tasks-c1-trace-metadata.json，34,283 B | 605a437c88302e0cb7b8a958686b5acfabf83ac62038c103daf33d0e7c0652f2 |
| rollback-desktop-touch-both4-direct-tasks-c1-timing.json，478,502 B | d129fcb4485645abff75d468a4e8b9762e715f85982a27a07ca15529c9d4180f |
| rollback-desktop-touch-both4-direct-tasks-c1-task-context.json，877,454 B | bf4294e90fb4ef23ee39edd7d6522666a6ac8baebac8c527a15a5a02e620fe19 |
| rollback-desktop-touch-both4-direct-tasks-c1-task-review.json，21,006 B | 605700961dfe05cc2461d22e29f907945bb73917f48675652f5ed63eb44d160a |
| rollback-desktop-touch-both4-direct-tasks-c1-thread-context.json，290,813 B | d8303769b74b3cfc43a76add82b5a1c408602767625364f996e432f9a036c8f6 |
| desktop-touch-both4-direct-tasks-c1-P1-scheduler-trace.json，37,130,407 B | 7e6d167c0aca1249b948d0cf0f4e12c7e4a0313154302a4a35f6f23de205452e |
| desktop-touch-both4-direct-tasks-c1-P2-scheduler-trace.json，36,020,016 B | eb48934a32ae80fd70a63b53d0aebdf880b6c51c4926cda4b4b2712f84da58b7 |
| rollback-desktop-touch-repeat-present-audit.json，11,324 B | 6b576d0d6e2e23857fabe0d72d18f33be33232efc4939e6891bf9f2c4efc179d |
| rollback-managed-touch-desktop-c20-evidence.json，1,075,416 B | b4e5b05cb96ff57a3f7701dd04dd0c60ec1d71e0b32401eaff00533b3756bf57 |
| rollback-managed-touch-desktop-c18-evidence.json，127,322 B | 399be9bdd43d4d228be23caf209de1ecc57fdf1e7f75cd9232af806e73813493 |
| c17 执行 probe，27,074 B | 01e142e935d0050703ef1d8fab8ad1fa92bcf33198060b17b05e8eed5f3489de |
| rollback-managed-touch-desktop-c17-evidence.json，1,037,708 B | fd830590edf4d77244be68a240d1a796fb7f5c422d6ea88f7be7ac5dff3438e3 |
| rollback-managed-touch-desktop-c16-evidence.json，1,082,056 B | 1cf1e4acc524f579b4326685d8d8e6be611238a0b8847dd60363fad68cfac770 |
| [managed-touch-android-owned-target-probe.mjs](../../th09_web/tests/multiplayer/managed-touch-android-owned-target-probe.mjs)，本次执行源码 | 15d339596509e74167dc4604cae6061eedaaacb30644f4f1f8d49d9ae97c644f |
| rollback-managed-touch-component-evidence.json，234,345 B | 65b3c35c3413371924cf1732a1a58bc29bbbe5c320f6d671d753e1f8144d18f3 |
| rollback-mobile-prefix-shadow-evidence.json，8,415,566 B | 24422b707ae5a87e015db2db933a35a849df502b515793204162d03e259e367e |
| rollback-mobile-prefix-shadow-summary.json，614,889 B | e5c3b9cfa716689e70d6be29b197450ed7fd61022b1f1796996fb479e76ed59a |
| rollback-mobile-stick-stability-evidence.json，79,650 B | e55b9efa5e08b3469f419e7f50b0ead11b6a6fdabe7abd593452886bd4caf8bc |

Launcher probe 和 summary 源文件的存在不是执行 PASS。c16 的实际离线 analyzer 已执行并保留 exit1；表中已列明c16 当次完整执行身份 ee1186fc978050f05d8be86c08a95aac83117af966b1e4fd800aed951d91638e，不能用此前冻结稿 SHA 替代。c16 结果保留 hud-focus 边界问题、完整原始计数与三段合格 movement 数据，并未宣称全局资格或流畅性通过。独立 Android owned-target probe 的 PASS 另由真实命令、报告 SHA 与协议/清理资格支持，范围仅为自有空白页。 c20 使用表中单列的 3a1f351ea689fb82c9b2c1493ca3a2b98c4c5234a16cf0970bc32e7b5f2f492c，仅增加后端 metadata 透传；它仍为严格 exit1，没有放宽旧资格或重写旧 raw/evidence。

### 9.2 运行命令及保留产物

managed 接收器、离线输入分析和浏览器尝试的产物位于 [th09_web/artifacts/multiplayer-tests](../../th09_web/artifacts/multiplayer-tests/)。已提供的同标签 invocation JSON 保留真实 args、开始/结束、退出码、source before/after、日志路径与 SHA。Launcher 正式 build 与 host validators 编译按其独立 owner、真实命令 ID 和 artifact identity 核对，不假定与 driver 共用同一目录布局，也不重建未记录的命令行。

| run label | 入口或已知 nig command ID | 结果 / 审计入口 |
| --- | --- | --- |
| managed-touch-native-c1 | run-tests.mjs touch-controller | exit0；[invocation](../../th09_web/artifacts/multiplayer-tests/managed-touch-native-c1-invocation.json) |
| managed-touch-queue-c1 | managed-touch-queue.test.mjs，候选 goals | exit0；[invocation](../../th09_web/artifacts/multiplayer-tests/managed-touch-queue-c1-invocation.json) |
| managed-touch-queue-b1 | 同一 fixture，显式旧源码 characterization | exit0；[invocation](../../th09_web/artifacts/multiplayer-tests/managed-touch-queue-b1-invocation.json) |
| managed-touch-component-summary-c1 | 离线组件结果提取 | PASS；[小日志](../../th09_web/artifacts/multiplayer-tests/managed-touch-component-summary-c1-process.log) |
| managed-touch-materialize-c1 | normal/multiplayer Runtime 物化 | [invocation](../../th09_web/artifacts/multiplayer-tests/managed-touch-materialize-c1-invocation.json)、[身份报告](rollback-managed-touch-runtime-materialization.json) |
| mobile-prefix-shadow-c1 | 冻结九份实际 tape 的离线影子 | PASS；[invocation](../../th09_web/artifacts/multiplayer-tests/mobile-prefix-shadow-c1-invocation.json) |
| mobile-prefix-shadow-summary-c1 | 离线全组摘要 | PASS；[invocation](../../th09_web/artifacts/multiplayer-tests/mobile-prefix-shadow-summary-c1-invocation.json) |
| mobile-stick-stability-summary-c1 | uk3fNvNJXB4SrZgYg9XxZjRu | PASS、73.4395 ms；[执行日志](../../th09_web/artifacts/multiplayer-tests/mobile-stick-stability-summary-c1-process.log)，仅离线摘要耗时 |
| managed-touch-desktop-touch-c1 | Lb5J2YzRgFRakGlsFD8ehLP8 | exit1、880 ms，启动前失败；[专用失败记录](../../th09_web/artifacts/multiplayer-tests/managed-touch-desktop-touch-c1-wrapper-preflight-failure.json) |
| managed-touch-android-touch-c2 | hgcesUmobrou5lBRQvuCx6os | exit1；[report](../../th09_web/artifacts/multiplayer-tests/managed-touch-android-touch-c2-report.json) |
| managed-touch-android-touch-c3 | C95bU_ObhOQQpip-PEvGBG2u | exit1、33,732.1714 ms；[report](../../th09_web/artifacts/multiplayer-tests/managed-touch-android-touch-c3-report.json) |
| managed-touch-desktop-touch-c7 | m01vhiJIXu9SxwBJtJUUNxc2 | exit1、18,404.1301 ms；[report](../../th09_web/artifacts/multiplayer-tests/managed-touch-desktop-touch-c7-report.json) |
| managed-touch-desktop-touch-c8 | vBCBFzQ_dQXnNT22FGCShdeV | exit1、20,540.7716 ms；[report](../../th09_web/artifacts/multiplayer-tests/managed-touch-desktop-touch-c8-report.json) |
| managed-touch-desktop-touch-c9 | U4NlQ5uHYFHGHyxwbTI_EAl0 | checked 写入 trace；[report](../../th09_web/artifacts/multiplayer-tests/managed-touch-desktop-touch-c9-report.json) |
| Launcher 候选正式 build | 781hOzjgXU2BWXA1r6OegrcY | PASS；无 source drift，artifact identity 见上表 |
| Launcher fresh baseline 正式 build | _vop3M47ouLM4b9ABqPictMs | 7,785.2611 ms，106 files；finally 恢复候选，source / old canonical stable |
| SW service/client 既有测试 | Qh2NEeL4sjKOQXzaC4lwG1nS | 15 pass / 0 fail / 0 skip / 0 cancel；test 152.6442 ms、wrapper 200.0416 ms；未渲染 ReactProvider |
| managed-touch-desktop-touch-c10 | 0wH3kp4JscnvSmYXTglf3Hnr | exit1、344.509 ms；[report](../../th09_web/artifacts/multiplayer-tests/managed-touch-desktop-touch-c10-report.json)；浏览器初始化前所有权拒绝 |
| 新树 actual ensureLauncherBuild | pl2301Jhh53BN2SSB86HF61H | PASS、62 authored source、574.1932 ms；编译时间 |
| managed-touch-desktop-touch-c11 | FSBSL2GOq3jD4oNBmRnlcZD2 | exit1、345.156 ms、sourceStable=true；[report](../../th09_web/artifacts/multiplayer-tests/managed-touch-desktop-touch-c11-report.json) |
| managed-touch-desktop-touch-c12 | hFitD1sUAYe2wzyb8OdKxyX8 | exit1、47,632.1838 ms；[report](../../th09_web/artifacts/multiplayer-tests/managed-touch-desktop-touch-c12-report.json)；开关成功，后续启动观察未通过 |
| managed-touch-desktop-touch-c13 | L_0YzF2m_sCM1XRPgyA4ug2P | exit1、20,079.6573 ms；[report](../../th09_web/artifacts/multiplayer-tests/managed-touch-desktop-touch-c13-report.json)；fresh baseline 开关失败 |
| managed-touch-desktop-touch-c14 | 9fK83215jl6oHS_Qb0jgtQTL | exit1、25,598.6132 ms；[report](../../th09_web/artifacts/multiplayer-tests/managed-touch-desktop-touch-c14-report.json)；第二次 warning locator 超时 |
| managed-touch-desktop-touch-c15 | FvGbWj-4leUDAwNB-ErJ_FmF | exit1、106,847.8874 ms；[report](../../th09_web/artifacts/multiplayer-tests/managed-touch-desktop-touch-c15-report.json)；真实 launch 成功，后续 context=3 |
| managed-touch-desktop-touch-c16 | DIW0FRzDfbap1gTxsfNLm0Ke | exit0、102,143.7656 ms；[report](../../th09_web/artifacts/multiplayer-tests/managed-touch-desktop-touch-c16-report.json)；driver 功能全 true，不代表严格汇总通过 |
| desktop c16 offline summary | vhKUP3bG5RQuQya85NjptcDc | exit1、178.5154 ms；[evidence](rollback-managed-touch-desktop-c16-evidence.json)；全局资格 false，离线耗时不是游戏性能 |
| managed-touch-desktop-stick-c17 | MWQFViS-NBrYTkGOjvYN6TUo | exit1、113,397.9902 ms；[report](../../th09_web/artifacts/multiplayer-tests/managed-touch-desktop-stick-c17-report.json)；queue 手势派发触及 20 秒上限，fresh 未执行 |
| desktop c17 offline summary | x6QxzrAbrubT1EdjeSClfIY8 | exit1、170.2333 ms；[evidence](rollback-managed-touch-desktop-c17-evidence.json)；driver/final=false，probeComplete/cleanup=true |
| managed-touch-desktop-stick-c18 | qoPhuv91Ox9Sad_SkRZ3iKHX | exit1、213,521.0294 ms；[report](../../th09_web/artifacts/multiplayer-tests/managed-touch-desktop-stick-c18-report.json)；未进入游戏、零移动，sourceStable / serverClean=true |
| desktop c18 offline summary | L4zJELHc7D3Zu_sBPXWEF3Ek | exit1、67.0775 ms；[evidence](rollback-managed-touch-desktop-c18-evidence.json)；no retained probe snapshot、phases=[] |
| managed-touch-desktop-stick-c19 | 9SHRanEWqzjqqt9ngslLWd55 | exit1、39,931.779 ms；[report](../../th09_web/artifacts/multiplayer-tests/managed-touch-desktop-stick-c19-report.json)；实际游戏已启动，warning 观察竞态，无 gesture/probe |
| managed-touch-desktop-stick-c20 | saXAWnBk1jCgmDa2jjZbz4S4 | exit0、37,227.8118 ms；[report](../../th09_web/artifacts/multiplayer-tests/managed-touch-desktop-stick-c20-report.json)；七项功能 goal=true，单机 rollback stats 全 0 |
| desktop c20 offline summary / HUD 复核 | I6ROgE4R_3BwgWJEtW6oGiME / wfgYdRe4ammdsx70841oMdiJ | summary exit1、137.4926 ms；[evidence](rollback-managed-touch-desktop-c20-evidence.json)；HUD 复核 exit0、552 ms，整体 false 保留 |
| 双端 CPU4 relay39 c1 | nk2Sq_3nefErgv_p7d54emI_ | wrapper exit0、62,727.217 ms；[evidence](rollback-desktop-touch-both4-relay39-c1-evidence.json)；native frontier 字段缺失，严格窗口不合格 |
| 双端 CPU4 relay39 c2 | eA9ENAFIjcZ_uewQIXmMh1L1 | wrapper 约 157,609 ms 超时、正文终点未到；[evidence](rollback-desktop-touch-both4-relay39-c2-evidence.json)；14 个确认点相同，全部长间隔保留 |
| 双端 CPU4 relay39 c2 followup | 6xbhr_5COxjqH-SXsq_Nbazm | exit0、805 ms；[followup](rollback-desktop-touch-both4-relay39-c2-followup.json)；核两个最大端点与普通前进子集，不是游戏运行 |
| smoothness driver syntax preflight | OV5nay-8I0zjP0Az1MvqhFbC | exit0、514 ms；只为语法检查 |
| 双端 CPU4 relay39 c3 | fn-hhXaYZPc6pEW3mO5COavZ | exit0、55,282.1303 ms；[raw](../../th09_web/artifacts/multiplayer-tests/desktop-touch-both4-relay39-c3-report.json)；严格窗口/15 确认点通过，仍约 56.7 Hz |
| 双端 CPU4 relay39 c3 derived | kZ79qksomxVDvCLIJdOGhJuv | exit0、985 ms；[evidence](rollback-desktop-touch-both4-relay39-c3-evidence.json)，离线耗时 |
| 双端 CPU4 RTC direct c1 | LWJrtxz7cbX_IGZ_8_14NP_S | exit0、59,074.5264 ms；真实 host↔host UDP/input lane 与完整窗口确认，仍有明显长间隔 |
| 双端 CPU4 direct c1 derived | Ohxnn_5V145B5hr5O_kGKsrh | exit0、969 ms；[evidence](rollback-desktop-touch-both4-direct-c1-evidence.json)，不能以派生 PASS 代表无卡顿 |
| direct c1 最大 gap 上下文 | EBpbnbQC8xNFPF0oNjv8ttqB | exit0、654 ms；[gap-context](rollback-desktop-touch-both4-direct-c1-gap-context.json)，离线提取 |
| direct c1 双时钟核对 | TZ04VoOdbG6DqhbltmZMjxhs | exit0、584 ms；[gap-clocks](rollback-desktop-touch-both4-direct-c1-gap-clocks.json)，Page 观察不是 callback 完成 |
| desktop-touch-both4-direct-raf-c1 | yAAKft8om6WUU3TBr3_hbTM- | exit0、64,763.2827 ms；请求诊断/严格窗口完整，仍有 >100 ms 间隔，未校准开销 |
| direct-raf-c1 evidence / timing | 55tcVftEnnvkgeEW0hYgI5r7 / AicY__im87CWKXX0yrw5YWQv | [evidence](rollback-desktop-touch-both4-direct-raf-c1-evidence.json)、[timing](rollback-desktop-touch-both4-direct-raf-c1-timing.json) 已保存；不是新浏览器运行 |
| direct-raf-c1 timer context | BhUCADAL75_awjyiQAftRf76 | exit0、610 ms；[timer-context](rollback-desktop-touch-both4-direct-raf-c1-timer-context.json)，最大/次大等待分开 |
| desktop-touch-both4-direct-sched-c1 | fkCe5OH57vWQHzo3uP_TuTFg | exit0、103,290.3618 ms；游戏窗口完整但 scheduler trace 不合格，仍有 >100 ms 间隔 |
| direct-sched-c1 general evidence | pE1c3bkPAUO3W1VvAQfnpuPK | exit0；[evidence](rollback-desktop-touch-both4-direct-sched-c1-evidence.json)，不代表 trace 或流畅性通过 |
| direct-sched-c1 RAF timing | Yq8_idQHeqvAZ2KfKNvKEzCw | exit0、738 ms；[timing](rollback-desktop-touch-both4-direct-sched-c1-timing.json)，离线提取；RAF 资格独立于 trace |
| direct-sched-c1 trace metadata | 6lFSIePoacrIJDcZwReoc9rx | [trace-metadata](rollback-desktop-touch-both4-direct-sched-c1-trace-metadata.json) 已保存；collectionValid / activeWindowCovered=false |
| direct-sched-c1 trace limits | eYEJxsq-9HUdWGzQfQEY4hIv | [trace-limits](rollback-desktop-touch-both4-direct-sched-c1-trace-limits.json) 已保存；dataLoss=null，保留完成/分离超时 |
| scheduler tasks preflight | 4TxAHMWKfvwqKx82SUI-BNhR | exit0、882 ms；4 syntax + 24 VM/mock（19 helper / 5 finalizer），不是游戏流畅性验证 |
| desktop-touch-both4-direct-tasks-c1 | bYeqEk9PlFxULeh4OhGQOX-X | exit0、59,588.7703 ms；strict / 10 hashes / 实际 direct / tasks trace 合格，仍有 >100 ms 间隔 |
| direct-tasks-c1 general evidence | diRvZ8vrriJXqOGnnlznb40A | exit0；[evidence](rollback-desktop-touch-both4-direct-tasks-c1-evidence.json)，不代表流畅性通过 |
| direct-tasks-c1 trace metadata | Wim2Xm0NBQa4yA0z8O7cY86O | exit0；[trace-metadata](rollback-desktop-touch-both4-direct-tasks-c1-trace-metadata.json)，活动覆盖和时钟映射均合格 |
| direct-tasks-c1 RAF timing | 5ftu_lC0E1G6VCTBgY-40LY4 | exit0、773 ms；[timing](rollback-desktop-touch-both4-direct-tasks-c1-timing.json)，离线提取耗时 |
| direct-tasks-c1 同窗任务区间 | _7fLEBmarXefhVLKxdLK0FF2 | exit0；[task-context](rollback-desktop-touch-both4-direct-tasks-c1-task-context.json)，终态 selector 初稿问题保留 |
| direct-tasks-c1 终态与任务复核 | qKsX9E6WGarO35iJKSKnioVA | exit0、629 ms；[task-review](rollback-desktop-touch-both4-direct-tasks-c1-task-review.json)，controls.enteredAt 更正与暂停证据 |
| direct-tasks-c1 其它线程上下文 | Jba8z7Yc-bbKDgT3d5dnkLK2 | exit0、998 ms；[thread-context](rollback-desktop-touch-both4-direct-tasks-c1-thread-context.json)，只分析已有 toplevel 事件 |
| tasks-c1 冻结 raw 实际 flags 审计 | EHE8M4wD-OlEA1N01IxHjsgp | exit0；实际未启用 disable-frame-rate-limit / disable-gpu-vsync，不混同 allowed 列表 |
| continuous / graphics preflight | yE4s0_iwoiomugAPuNx_tVwE | exit0；6 syntax + 38 tests、旧计划 SHA 2/2 一致，非游戏性能验证 |
| continuous / graphics c2 端点 preflight | dumBb-uYiaj9H0WSWBfZ7bIW | exit0；6 syntax + 51 tests，test duration 492.7934 ms；仅预检，实测结果另列 |
| desktop-continuous-touch-both4-direct-graphics-c1 | ZZ-JwYZ8nCcUVmMnZHCW_-w_ | exit1、86,641.0887 ms；测量结束后 RTC selected candidate evidence unavailable，离线核正文/hash 不改变 driver 失败 |
| continuous / graphics c1 general evidence | VYm31YEC7zH57aQfG22y-_PJ | exit0；[evidence](rollback-desktop-continuous-touch-both4-direct-graphics-c1-evidence.json)，正文/hash 合格，route 不补填 |
| continuous / graphics c1 RAF timing | en-VWhgkkRIbnHfkcZrpqfCc | exit0；[timing](rollback-desktop-continuous-touch-both4-direct-graphics-c1-timing.json)，最大等待与其它间隔分开 |
| continuous / graphics c1 GPU/viz 上下文 | xg8rhe7MVIZaQBrp_8vu8OVf | exit0；[trace-context](rollback-desktop-continuous-touch-both4-direct-graphics-c1-trace-context.json)，短 SwapBuffers 相关性，不是硬件 GPU 时长 |
| desktop-continuous-touch-both4-direct-graphics-c2 | YHeNbCuxro3h3ICBtzB5C3uE | exit0、62,994.7504 ms；正式测量及历史 direct 资格通过，晚尾 teardownClean=false，不扩大 passScope |
| continuous / graphics c2 general evidence | 8mvV93LXgD-mkvzPT2t3bMGI | exit0、825 ms；[evidence](rollback-desktop-continuous-touch-both4-direct-graphics-c2-evidence.json)，仅离线提取耗时 |
| desktop-continuous-touch-both4-relay39-c1 | rGHvkV4h1mV_z_rKdPk4IsRl | exit0、45,813.139 ms；正文、确认、历史端点和实际 relay 资格通过，teardownClean=true 的范围截至 disposal 前 |
| continuous / relay39 c1 general evidence | imQ5u6J7pBTW8-Lp9Jnd-IlT | exit0、609 ms；[evidence](rollback-desktop-continuous-touch-both4-relay39-c1-evidence.json)，仅离线提取耗时 |
| desktop-continuous-touch-both4-relay39-burst160-c1 | CTKMEhQXcIY8YVoEUeJLpvMe | exit0、46,691.2372 ms；测量资格通过，但 60.505 / 90.335 ms 间隔不算性能达标 |
| continuous / relay39 burst160 c1 general evidence | 4UNX1QmlAWuV2M5osG-oqbKM | exit0、721 ms；[evidence](rollback-desktop-continuous-touch-both4-relay39-burst160-c1-evidence.json)，仅离线提取耗时 |
| continuous / relay39 burst160 c1 RAF timing | -cJe1UPox7zFQGWHas5qwQWJ | exit0、648 ms；[timing](rollback-desktop-continuous-touch-both4-relay39-burst160-c1-timing.json)，已登记等待与新状态间隔分开 |
| continuous / relay39 burst160 c1 gap / network context | ZoO5_CmjFexcjPMC7WgUkHDa | exit0、593 ms；[gap-network-context](rollback-desktop-continuous-touch-both4-relay39-burst160-c1-gap-network-context.json)，保留全部六个 >50 ms 间隔与 relay 观察边界 |
| recovery-tail 独立树冻结输入 setup | 8-eYjT9jf5lXLta8IAaNakav | exit0；只恢复 CRLF 字节差异后，初始 275 构建输入匹配 |
| recovery-tail base identity | NiVm-WFZkY6SfrMPsTN9jWkV | exit0；[base identity](rollback-desktop-recovery-tail-base-identity.json) |
| recovery-tail release 构建 | NMfmuafGYWTsoixqaGasivde | exit0、55,713.9234 ms；sourceStable，构建耗时非性能 |
| recovery-tail DEV 构建 | 5kYCGp7aDpYhToJvmGhkHeOc | exit0、56,222.7756 ms；sourceStable，只作正确性 |
| recovery-tail build identity | K0mGY_cleCRVc903aK9SOX1u | exit0、745 ms；[build identity](rollback-desktop-recovery-tail-build-identity.json) |
| recovery-tail 四组件 | aplQGnl0tKZNewKprU4C-v8j | exit0、17,935.9959 ms；sourceStable，不编译 App 新分支 |
| recovery-tail joystick H8 三世界 oracle | EdBtJNXmAaoNf2-7cyOU6kib | exit0、103,554.5964 ms；600 帧正确性，非双端 FPS |
| recovery-tail 四组件 + oracle 摘录 | AUORDXJ0yCkuQaH_ZaEoNpnH | exit0、511 ms；[correctness evidence](rollback-desktop-recovery-tail-correctness-evidence.json)，离线提取耗时 |
| recovery-tail 实际 App 控制门 | jrsK2BLarKU0G1Rk7DhTbYFm | wrapper exit0、3,272.1186 ms；候选 14/14，原版目标门 exit1 恰 4 失败 / 10 通过，合格负对照 |
| B normal relay39 | psLdFjow9FZs0aC72Tjy5n45 | exit0、54,440.9057 ms；[evidence-v2](rollback-desktop-recovery-tail-both4-relay39-c1-evidence-v2.json) |
| B relay39 burst160 | 6blxQaSKCv0ZNfCqnsr1Xb74 | exit0、47,894.8233 ms；[evidence-v2](rollback-desktop-recovery-tail-both4-relay39-burst160-c1-evidence-v2.json)，性能未证明改善 |
| B evidence scope 更正 | PA4T_Y5wjT0eqxR72KL6hU88 | exit0、536 ms；只更正 scope，旧 evidence superseded 保留，raw/数值不变 |
| A 原版 burst160 回切 c2 | a9iwRIptfazw7x_2y4CxRbuo | exit0、51,644.8519 ms；[evidence](rollback-desktop-continuous-touch-both4-relay39-burst160-c2-evidence.json) |
| network-brake C release 构建 | L8P13H6FVUfawYQfbOrZwVjm | exit0、50,559.2387 ms；sourceStable，不含 B App 改动 |
| network-brake C build identity | INyhiuJXupczXKsIkQZKthvn | exit0、641 ms；[build identity](rollback-desktop-network-brake-build-identity.json)，275 源仅 RollbackSession 改变 |
| network-brake C 四组件 | XSunWtoMAI69Lhu99yKhd7HN | exit0、17,483.7563 ms；sourceStable，含新 B3/4/5/6 合同点 |
| C normal relay39 | bMAN7D1yiLGeR6JWp5XqGkNT | exit0、50,413.4904 ms；测量资格通过，约 54.4 Hz 的不利结果不作单因果归因 |
| C normal relay39 general evidence | c4x1G7_na4RH-1GJI8ea_Am6 | exit0、936 ms；[evidence](rollback-desktop-network-brake-both4-relay39-c1-evidence.json)，离线提取耗时 |
| C relay39 burst160 | BKUxLr0R9i1SkdbCUwqTDrdU | exit0、83,095.0256 ms；测量资格通过，但约30.9 Hz和181.775/224.050 ms长间隔明显不达标 |
| C burst general evidence | BgKbXeyDHUNvgRCSMo3FgzJi | exit0、719 ms；[evidence](rollback-desktop-network-brake-both4-relay39-burst160-c1-evidence.json)，离线提取耗时 |
| A 原版 normal relay39 回切 c2 | Xpcxu9qrovmPlXM4_o-dpWXJ | exit0、50,734.7632 ms、sourceStable；测量通过但Page最大435.625/493.565ms，不算性能达标 |
| A normal relay39 回切 general evidence | Xnhyj_FQUbVCuKcILd0BNHyP | exit0、687 ms；[evidence](rollback-desktop-continuous-touch-both4-relay39-c2-evidence.json)，离线提取耗时 |
| A normal relay39 回切 RAF timing | U9tKnrD1qomRUSOGDmT4MnHh | exit0、722 ms；全body映射与P2已登记477.630ms等待 |
| A normal relay39 回切供给传播 | 6-KksGVObuKFZQUi6bD-xl8W | exit0、661 ms；[peer-propagation](rollback-desktop-continuous-touch-both4-relay39-c2-peer-propagation.json)，近似时钟与frame次序核对 |
| A normal relay39 回切输入分母 | wZsKCQLGaa1usevR8BaJFAK1 | exit0、666 ms；[input-gap-denominators](rollback-desktop-continuous-touch-both4-relay39-c2-input-gap-denominators.json) |
| 九组双时钟/条件输入代理比较 | Ub06KBMo2ltdOpKIxcXyvanx | exit0；只知首次yield1014ms，不编作总耗时；[comparison](rollback-desktop-continuous-touch-comparison-c1.json) |
| C burst 完整RAF timing | po-XW2hlhEVX5PzZnkA6bgPX | exit0、728 ms；[timing](rollback-desktop-network-brake-both4-relay39-burst160-c1-timing.json) |
| C burst 固定frontier分箱与坏区间 | Xpthco0RT7SrU_V9OYZrMQoh | exit0、732 ms；[adverse-context](rollback-desktop-network-brake-both4-relay39-burst160-c1-adverse-context.json)，保留所有样本 |
| GPU单flag三组预声明 | OjpPL9IJhJSmAFNDr2OXD5R3 | exit0、461 ms；[plan](rollback-desktop-gpu-vsync-plan.json)，预声明文件保存 |
| GPU单flag预检 | hh7z2bSiVE5uuduzu5SKUzLt | exit0；3 syntax + 60纯测试 PASS，内部 elapsed=904.7354 ms、sourceStable；非游戏性能 |
| GPU单flag A1=0 | 8uBJDIM15YtKeOc71AoLNOLs | exit0、133,262.5858 ms；[raw](../../th09_web/artifacts/multiplayer-tests/desktop-gpu-vsync-both4-relay39-a1-report.json)，测量资格经v2勘误合格但性能不达标、晚尾teardownClean=false |
| GPU单flag A1 qualifier勘误 | 7-b7eu1iXNQzxYYU7smy8wxK | exit0、527 ms；[evidence v2](rollback-desktop-gpu-vsync-both4-relay39-a1-evidence-v2.json)，只更正trace/passed资格和勘误元数据，v1保留 |
| GPU单flag B1=1 | gSpPyPI6vt9aT7wNdW6Bn3xx | exit0、97,748.5978 ms；[raw](../../th09_web/artifacts/multiplayer-tests/desktop-gpu-vsync-both4-relay39-b1-report.json)，正式资格通过、晚尾teardownClean=false |
| GPU单flag B1摘要 | lO_8H-Dauoj7TYsuq9Nko_tp | exit0、664 ms；[evidence](rollback-desktop-gpu-vsync-both4-relay39-b1-evidence.json)，离线耗时不算游戏性能 |
| GPU单flag A2=0 | VPIWtBg26KmE890KnqT_HYai | exit0、91,784.9012 ms；[raw](../../th09_web/artifacts/multiplayer-tests/desktop-gpu-vsync-both4-relay39-a2-report.json)，正式资格通过、晚尾teardownClean=false |
| GPU单flag A2摘要 | IYw0sCxa5Sp9kJz_ZCA2KVDw | exit0、673 ms；[evidence](rollback-desktop-gpu-vsync-both4-relay39-a2-evidence.json)，离线耗时不算游戏性能 |
| GPU单flag三组 rAF timing | kOwLJaY4Nzk6J1CGDpn8TLx6 | exit0；三组完整body映射、valid且无overflow；不填未提供的总耗时 |
| GPU单flag双时钟/逐事件比较 | jhLNojUNxQJmGYxC9pENda9h | exit0、1,006 ms；[comparison](rollback-desktop-gpu-vsync-comparison-c1.json)，raw/evidence SHA与完整分母保留 |
| GPU单flag联合时间线 | xn99rhj5_67YbB7z_-6SSPAp | exit0；[timeline](rollback-desktop-gpu-vsync-joint-timeline-c1.json)，6trace SHA/时钟门核对，只分析完整X跨度 |
| GPU单flag decision | 96gLTox07u_l2GWnOBAKY4TH | exit0、461 ms；[decision](rollback-desktop-gpu-vsync-decision-c1.json)，严格(fromSample,toSample]成本及方案结论 |
| GPU单flag compact inspection | KfiQzLuykVFIp_P8m_Cqt-er | exit0但约14k stdout截断；完整artifact未变，后续decision以更紧凑输出完整复核，无未知写入 |
| 既有窗口 repeat-present audit | adHi2AAwR6m9er9RV2krD_3X | exit0；[audit](rollback-desktop-touch-repeat-present-audit.json)，离线计数，不是生产优化收益 |
| managed-touch-android-connect-c1 | WHS70uMtWRyp0k568XLUbK7S | exit1、20,625.3751 ms；[report](../../th09_web/artifacts/multiplayer-tests/managed-touch-android-connect-c1-report.json)；前台 am start 超时，CDP/mapping 未尝试 |
| managed-touch-android-connect-c2 | 9qWLzFUu5yYJOLd2UCQrGzwg | 30,821.2302 ms 后初始化超时；[report](../../th09_web/artifacts/multiplayer-tests/managed-touch-android-connect-c2-report.json)；WS 已握手，18 pending |
| managed-touch-android-owned-target-c1 | AqroxpGEo5-iUgkMhRJJBRBJ | PASS，wrapper 2,060.7824 ms；[report](../../th09_web/artifacts/multiplayer-tests/managed-touch-android-owned-target-c1-report.json)；仅自有空白页 flattened CDP |
| Android 有界 metadata / wake | O6TFNtyQxiWxCwcZNoOdytO9 / 8oOXgyYqYwHNw4CuQ8NnFJmM | Asleep/OFF → Awake/ON；secure=false；不是游戏性能运行 |
| 入口精简诊断 c1 | tSBxfFm_HuDboBj1qhjJcarT | exit0、591 ms；[完整 JSON](rollback-managed-touch-entry-diagnostics-c1.json)，stdout 截断不能代替文件 |

以下十五份 driver / 连接诊断未通过的 report 原始身份保留；c12 的开关子目标和 c15 的真实启动子目标已满足，不能把这些子结果抹成全失败：

- android-touch-c2：122,461 B；SHA-256 bed6cd3939d02784c5c33231aae11ed70252ecfa21786326ea1a29047877dfed。
- android-touch-c3：122,781 B；SHA-256 eb48099ea1d8ebc7856212be226d4c164263a5376bf63d5af6f8a9244a5f1f07；执行 driver SHA 43a1375be54adf4cd340c29993f32cc02bba3bd5d2cfbc9eaca028fe22ce5cd9。
- desktop-touch-c7：226,772 B；SHA-256 7d8a86e60a7ed4ee066a2d91f0d2fed51a57bc676df93315d36076e146767e79。
- desktop-touch-c8：142,556 B；SHA-256 5fa8169f68df04ad507b81d0113d3593573bec8a38f1a5cc48434b08c9dbdab9；执行 driver SHA 096d1825f72e8a7093d625284ccb30214e1c27b232e961816af014472f535b49。
- desktop-touch-c9：150,203 B；SHA-256 674a2d3036dd1d373deeab679089ac78b484aab73bffb62adbdd3904fed92e45。
- desktop-touch-c10：3,091 B；SHA-256 c13f2854596e6fd367042020a71c53f30b836b3913bb58260ca1891c33a5a590。
- desktop-touch-c11：3,417 B；SHA-256 38125937b80a851361a147b3c9bfe1badcec3a477184965eedc75d6d961733fa。
- desktop-touch-c12：231,485 B；SHA-256 b59c1b25ce24ef8869238cb799a6ae2363efe96c711c8c915142f8d8e24d3966。
- desktop-touch-c13：155,844 B；SHA-256 0ad2445e656e67fde9a37b6a7c339366f05982a91729a42876d7defaa57fbeca。
- desktop-touch-c14：234,960 B；SHA-256 d2a7f8602ac90ed3762ba3a4b1ae7e497b7c4ef2e69c701184d739c1ecd4bf4b；driver SHA-256 45c884b59537b963dfd16623f6befd9d3c7c612e5d0581c3fea38ee63ec8fded。
- desktop-touch-c15：324,000 B；SHA-256 b01db7062f292328d73fabd02bb361edff482aa41bc61825a5e74959280a2df0；driver SHA-256 04ffdf14cd29d444eae4a20ab43e7560b9295957ada140d424857168de556b2f。
- android-connect-c1：9,310 B；SHA-256 482b4f1560597f50e839f98554b75ce22283a90c92efc5cf1e16f3d319ef79ad。
- android-connect-c2：27,284 B；SHA-256 44acb947344252ad2c4ba50b9df891507eb673296d85b36847e562fcba282b39。
- desktop-stick-c17：8,582,419 B；SHA-256 93f162e77b0ad800c6776480e6ed963d38b75f862c1e6ce456a08ee8ce2d2977；对应离线 evidence 为 1,037,708 B，SHA-256 fd830590edf4d77244be68a240d1a796fb7f5c422d6ea88f7be7ac5dff3438e3。
- desktop-stick-c18：1,746,987 B；SHA-256 d255a519c496ae49bed2baca96f450795e453ee963f3648c471d3fdb61e0963d；对应离线 evidence 为 127,322 B，SHA-256 399be9bdd43d4d228be23caf209de1ecc57fdf1e7f75cd9232af806e73813493。

c16 的 [driver report](../../th09_web/artifacts/multiplayer-tests/managed-touch-desktop-touch-c16-report.json) 单独分列，因为 driver exit0 而离线严格资格 exit1：13,169,660 B，SHA-256 10f2a394faea55d73e1557b8334d15238e1d1b1531337845ef161efab006e41a；离线 evidence 为 1,082,056 B，SHA-256 1cf1e4acc524f579b4326685d8d8e6be611238a0b8847dd60363fad68cfac770。两份均保留，没有把功能通过或资格失败抹去。

c19 的 [driver report](../../th09_web/artifacts/multiplayer-tests/managed-touch-desktop-stick-c19-report.json) 为 314,162 B，SHA-256 09c8e47c85d0086c9c63546fb32f5ffc46b8e2276027d591fe2cb0318d3a5e22；单独记录“实际启动已完成但 driver 观察失败”，未删除该失败。c20 的 [driver report](../../th09_web/artifacts/multiplayer-tests/managed-touch-desktop-stick-c20-report.json) 为 5,113,651 B，SHA-256 9eb42eebc6f8f960bf11331ece8a43d548a968669be809ebad22360a6d97217e；driver exit0 与严格 evidence exit1 分列，后者为 1,075,416 B，SHA-256 b4e5b05cb96ff57a3f7701dd04dd0c60ec1d71e0b32401eaff00533b3756bf57。

另有已经通过的协议诊断原始报告：[managed-touch-android-owned-target-c1](../../th09_web/artifacts/multiplayer-tests/managed-touch-android-owned-target-c1-report.json)，40,340 B，SHA-256 c2e565f145299b40e8e9dc23a6941a89342a82167e2b295cdbdd3d190284271b，命令 AqroxpGEo5-iUgkMhRJJBRBJ。它不列入上述十五份 driver / 连接诊断失败报告，也不是手机游戏成绩。

desktop c2–c6 与 Android c1 的失败也仍在同目录，以各标签的 invocation、process.log、report 为审计入口；没有把它们从系列中删掉。入口精简诊断 c1 文件为 34,003 B，SHA-256 6fed10f5b446b2fe761c7eecc2daa4541c859fe002393fd2237fa300687fdaf3；其覆盖截止 Android c2/c3 与桌面 c7，不能把后续桌面 c8–c20 或独立 Android connect / owned-target 诊断也算进该文件。

## 10. 当前交接状态

接收器候选、native/VM 证据、AppShell 入口写入 trace 与本地提交、实际失败装配记录和两项离线输入摘要已经形成可审查的工作成果。当前方向已改为桌面人工构造触控，核实双端 CPU4、双方持续变化输入、独立 relay 与实际 RTC direct，并把输入链、恢复成本、rAF 等待和网络信息预算纳入同一验收。手机测试不再推进。

AppShell 开关子目标已有 c12/c13 对照，Launcher 本地提交 c84cc3941c6637fcdf975667db9cf52ad307a43d 保持候选源身份；c15 有成功启动 ACK 与 first-frame，c16 的触控 driver 功能目标通过，但严格资格仍未通过。c17 的源计划未完成且有真实长间隔；c18 未进入游戏，不能评价移动窗口。c19 已实际启动但 warning 观察失败；c20 七项功能和三段运动资格通过，child sample 新状态最大间隔为 30.400 / 29.100 / 27.000 ms，但整体资格仍受 HUD 真实 Window blur 边界限制，且 rollback stats 全 0，不能外推多人表现。新增双端 CPU4 relay c1 的严格窗口缺 native frontier；补充观测后的 c2 未覆盖终点，并出现 339.380 / 284.755 ms 最大新状态间隔。c2 followup 另确认两个最大端点，并保留无新增 correction/resimulation 的普通前进子集仍有较高整 callback 成本；它不是严格 A/B 或纯 CPU 成本。新 relay c3 与 RTC direct c1 已完成双端 CPU4、实际 GPU、严格窗口及 15 确认点验证；c3 约 56.7 Hz 且正文无 >50 ms 间隔，但 direct 仍有 369.685 / 472.630 ms rAF 长间隔，且 P2 最大 Page 观察新状态 gap 为 489.755 ms，来自另一对端点。双时钟复核支持 P1 等信息 / P2 时间空洞的描述，尚不能指定根因；RAF_DIAGNOSTICS 定位已完成，本轮仍有 133.360 / 127.270 ms rAF 间隔，确认合格请求等待 124.145 / 117.885 ms，但尚不能归因 OS/GPU/具体 JS。timer 核对只在另一个 67.705 ms 等待的两个时点证明输入能运行，未替最大等待提供忙闲归因。direct-sched-c1 也已完成并保留 121.175 / 109.070 ms 间隔，独立 rAF timing 确认 112.275 / 103.735 ms 合格请求等待；但 scheduler trace 两端均缺完成通知与停止标记，collectionValid / activeWindowCovered=false，不能据其判 CPU/GPU 根因。诊断采集修正已实现并通过 4 syntax + 24 VM/mock 检查，1200 帧 tasks 短桌面诊断已完成，实际 direct / GPU、10 个确认点、严格窗口和 trace 资格通过，仍有 163.595 / 157.535 ms 长间隔与 158.005 / 148.520 ms 合格请求等待；同窗已保留主线程任务覆盖较小，GPU 进程长 RunTask 与最大等待高度重叠，但仅有任务寿命区间，不能确认 GPU 瓶颈。终态按 controls.enteredAt 更正后，两端 pause 后没有新 control/frame/sample；旧 selector 初稿和旧 trace 失败均保留。continuous 源计划与 graphics profile 已实现，6 syntax / 38 tests 预检及默认旧计划 SHA 2/2 一致性通过；对应桌面双端 CPU4 运行在测量结束后的 RTC 证据收尾处 exit1；离线已核完整正文、10 个相同确认点、独立输入与 graphics trace；最大新状态间隔 36.365 / 36.370 ms，无 >50 ms，但 actual direct 缺失且整体 exit1 不改。短 SwapBuffers 相关性没有解释旧约 150 ms 等待，新 profile 事件/字节更多，不能当低开销或匹配输入 A/B。收尾 pause 不停 Pump 的失败链已核实，drain 前端点/RTC 摘要的 harness 修正已实现并通过 6 syntax / 51 tests，捕获非原子、passScope 与 teardownClean 分开；continuous direct graphics c2 已通过测量与历史 direct 资格，rAF max=24.150/24.070ms、该口径无>25ms，Page max另为30.425/27.755ms；但 trace-drain 晚尾 teardownClean=false，且 P1 累计仅 1 correction / 1 resimulation，不能当深回滚验收。随后 continuous relay39 c1 双方端点及 relay 资格、10 个确认点和正文通过；累计 correction=860 / 911、resimulation=2,019 / 3,799，约 58.5 Hz、max=38.160 / 31.025 ms、无 >50 ms，teardownClean=true 仅按既定范围解读。网络 interval multiplier 的调节已观测到，速度差距仍不能全归网络，两轮观察条件不同也不是 A/B。relay input-lane burst160 已完成，确认 119 个 burst 包全部发送且覆盖 sender frame467–504，测量资格通过；约 56.2 Hz、max=60.505 / 90.335 ms、>50 ms 为 5 / 1 次，仍未达到流畅目标。全正文合格请求等待最大 22.025 / 21.255 ms，大间隔内既有信息余量耗尽，也有多 callback 连续恢复；不混同旧约 150 ms 未派发等待、不把线路当唯一原因。后续在独立树完成补尾 B 的 C++ 构建、四组件、深 8 三世界和实际 App 14 项控制门；原 App 负对照恰 4 失败/10 通过。但 B burst 的 rAF max=66.525/96.750 ms、P2 同端点 Page=100.335 ms，原版 A 回切也将 >50 ms 次数降为1/1，性能收益未成立，B 隔离不采用。C 则在完全不含 B 的原版上提前一预测帧制动，构建身份和四组件通过；普通 relay 测量合格但新状态约54.4 Hz、表现不利，不能由单样本确认阈值因果。C burst随后只有约30.9 Hz、最大rAF间隔181.775/224.050 ms、>100 ms为40/37次，测量资格通过不等于性能通过；C也暂不采用。C固定frontier分箱显示后半段普通callback成本上升，最大区间在scale1且B补齐时仍有恢复工作，不能把全部31Hz解释为网络主动降速。原版普通relay回切已完成，约55.2Hz、Page最大435.625/493.565ms；P2有477.630ms已登记等待，frame和relay记录支持一端输入供给断档传播对端，非relay将已有新输入扣留半秒，但未定GPU/编译根因。回切也坏不为C免责，不因host/noise丢样本。9run/18端完整双时钟与输入分母汇总已保存；P2该段57到期事件中56合并、1后交，成功代理max35.855ms不代表整段低延迟。晚到的窗口状态观察不能证明异常时持续前景。GPU单flagA0/B1/A0已预声明，测试driver/helper已冻结且3 syntax/60项纯测试通过；A1已完成，约34Hz、rAF max254.510/290.525ms，性能仍不达标；v2只纠正overflow对象误判，正式测量/trace合格与晚尾teardownClean=false分别保留。B1也已完成，约56.5Hz且rAF口径无>50ms，P1 Page max却为53.795ms，晚尾teardownClean=false仍保留；A2回切也已完成，约58Hz且rAF口径无>50ms，Page max45.630/37.870ms，晚尾仍不clean；A坏→B好→A也好，未建立参数可逆关联，不采用为修复，但不据此排除GPU链路或认定全是编译/CPU。三组双时钟/逐事件比较和完整rAF timing已完成，六份已存trace的联合时间线和decision也已完成；A1最大Page gap的7回调CPP总和114.815/124.720ms，B≤2/3且scale=1，ordinary Save1单callback最大60.395/57.000ms，不能全说成H8或网络主动减速。选中窗口未见同量级已命名GPU完整X，仍不排除未覆盖图形/OS事件。晚窗口状态不能反推先前gap，25.21/18.57/19.58s均为观测时间差而非精确断开时刻。当前准备独立、默认关闭且保持DEVELOPMENT_HARNESS=0的release粗阶段观测，尚未实施；先校准空阶段/时钟及同诊断WASM的runtime off→on→off，原版A另作来源控制，不把布局差异视为已消除。观测开销未校准，A1高负载不能自动称生产正常开销；VM最老preimage shadow在后，条件制动再后，二者仍只是有门槛的设计。没有生产部署或手机测试。重复最终 present 已有离线计数，但暂不优先实施，也未改生产代码。旧双端 relay c2 中没有 WorldState Save 的普通前进子集也慢，不能把所有问题归为 Save/restore。旧双端 relay c1/c2 未证明达标，宿主 busy 反证和该 c2 缺实际 renderer 一并保留；当前尚未证明 managed 的正式回滚性能收益或玩家流畅性，也未证明 CV / TH06/07 迁移候选有效。Android 只有历史连接与 owned blank 诊断，没有实际游戏测试；未执行准备和未知纠正写入不算验证，手机实验已停止。c16 约 5.69 秒长间隔、c17 反证、c18 启动失败、c19 观察失败与 c20 整体资格 false、双端 relay c1/c2 的不完整资格和坏结果、RTC direct 的新长间隔、此前的大间隔与基线漂移、连续拖动 216/216 恢复和旧误标产物全部保留。环境操作继续仅用 nig，构建/桌面运行/大 raw 分析由主代理串行；没有 push、merge 或部署。

后续仅追加桌面 CPU4 与双方变化输入的实际结果，保留本截点及全部失败记录，不把这份事实稿标成最终验收完成。
