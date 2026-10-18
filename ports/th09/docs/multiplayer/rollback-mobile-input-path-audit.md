# TH09 手机持续移动：输入链路审计与最小观测设计

## 1. 范围与当前状态

本阶段先验证同一模式下的模拟移动预测：**mode 0、D0、H8**。玩家目标仍是预测降低输入延迟、稳定呈现；允许网络信息不足时平滑降速。本文不是新调速策略，也不将输入缺口等同于线路问题。

基线为 root 已确认的主仓 **b243bb1**、隔离 common **512353**。文档所在工作树为 `D:/workspace/eagler/worktrees/th09-mobile-rollback-4x`；common 读取来自 `D:/workspace/eagler/worktrees/th09-rollback-input-batch-common`。Launcher/relay 读取来自 `D:/workspace/eagler/eagler-touhou`；本审计未执行命令获取该仓新的身份，不把它的源码状态冒充当前候选构建的输入 SHA。

审计只使用 nig 小份源码读取/搜索与本文写入；**未执行 Node、组件、构建、浏览器或性能测量，未读取大 raw**。root 正组织独立的桌面持续触控刺激与生产候选。真实手机验收本轮尚未执行：root 已只读确认一台现有 Android 实验设备连接，尚未接管浏览器、forward 或改变设备状态。后续真实 Launcher/hosted 桥测试仍需合法 carrier 与明确的 session owner。

已遵循 `eagler-touhou/docs/playbooks/input-touch.md` 的输入所有权：设备事件更新手势状态，每个新的逻辑帧采样一次；恢复、重绘和显示刷新不能重复消费设备 delta。本文的诊断设计默认关闭；生产 managed 路径暂不修改。

## 2. 源码证据：从手指到已确认模拟输入

### 2.1 两条触控入口不能混为一种覆盖

| 入口 | 当前源码路径/函数 | 实际语义与边界 |
| --- | --- | --- |
| Launcher 触控表面 | `app/runtime/RuntimeTouchOverlay.tsx`：约 45–48、203–224 行 | 当前 platform.direct 同时包含 iOS 和 Android。非 joystick 模式在宿主绘制覆盖层：Android 使用 PointerEvent；iOS 使用原生 passive:false TouchEvent。不能按 Runtime 的旧平台注释判断实际 owner。 |
| Launcher 直接拖动 | `app/services/touch-input.client.ts`：`directDown/directMove/directUp`，约 159–174 行 | 捕获当前 ready/launched/non-spectator epoch。为接触分配稳定负 ID，归一化几何；每个 directMove 直接 post，未按游戏逻辑帧采样。当前普通 Launcher 的 Android direct 覆盖层也调用这条路径。 |
| Launcher 摇杆 | 同文件 `joystickDown/Move/Up`、`queueSync`，约 84–95、147–158 行 | 保存最新有符号轴，至多排一个 host rAF 发送 controls 快照。host rAF 是显示调度，不能当作已生成的逻辑输入帧。 |
| Launcher 投递 | `app/services/runtime.client.ts`：`getInputContext/postInput`，约 296–306、1044–1049 行 | 验证当前文档身份、ready、launched、非 spectator，且命令属于 optional response；成功返回表示投递函数接受，不表示 native 已执行。 |
| 可选同步桥 | `src/launcher/touch-runtime-protocol.mts`：`deliverRuntimeInput`，约 55–79 行 | 已有 schema `eagler-touhou/direct-input/1`，匹配 protocol/game/epoch/origin；submit true=已消费一次，false=未触碰可 fallback，throw/非布尔结果不能重试。**TH09 sdl-runtime 当前未安装该 bridge**。 |
| TH09 hosted message | `th09_web/sdl-runtime/managed.mjs`：message listener/command，约 65–104 行 | 校验 source、origin、protocol、game、epoch。keyboard 与 keyboard-clear 立即 dispatch；direct-touch、touch-controls、touch-cancel 仍进入串行 Promise queue，并等待 initialized。 |
| TH09 child DOM | 同文件约 117–121 行 | 真正落在子文档的 pointer 事件直接调用 native；此 handler 没有 Android 平台判断。源码保留“Android 直接、iOS host”的旧注释，但当前普通 Launcher 在 Android direct 模式也有 host 覆盖层。child handler 存在不证明该次手势经过它，host 摇杆 controls 也不走这条直达路径。 |
| native 手势入口 | `th09_web/cpp/sdl/Application.cpp`：`th09_touch/th09_touch_cancel/th09_touch_controls/th09_touch_stick`，约 651–656 行 | 更新共享 TouchController 的接触/目标/按钮状态，不在事件回调中生成网络 FrameInput。 |

直接调用 `core._th09_touch` 的桌面实验可覆盖共享手势转换及后续 netplay；它没有覆盖实际 DOM pointer 分发、host rAF 合并、postMessage、managed queue 或真实手机浏览器调度。协议 helper 的 mock PASS 同样不能替代 TH09 runtime message 处理测试。

### 2.2 相对手势成为绝对目标；持有输入仍会每次采样存在

`portable/input/TouchController.hpp` 的 `pointer` 约 24–45 行，用相邻 pointer 坐标的差乘灵敏度与速度比例，累加到有边界的 target_x/target_y。DOWN 以当前玩家位置初始化目标；MOVE 不是直接将屏幕绝对坐标当作游戏位置。上下文、实例、ready 与 primary ownership 控制手势存活。

同文件 `sample` 约 47–63 行：正在拖动时，即使这一帧没有新的 DOM MOVE，也返回持有的目标；free joystick（mode 3）产生模拟移动，digital joystick（mode 2）主要产生方向键。不能把所有手机移动都归入同一种 analog 负载。

`Application.cpp::sample_keys` 约 325–352 行只在新的逻辑采样时消费动作脉冲。网络 direct drag 写入 MotionSample 的绝对 field target；free joystick 转为有界速度向量。`rollback_tick` 的 capture lambda（约 492–507 行）将其转换为：

- buttons、analogMode（DirectTouch / Joystick）、x、y、unlimited、touchUsed；
- 通过 `RollbackSession::Capture` → Core ScheduleLocalInput → SessionChannel LocalCaptured；
- `NeedsCapture` 要求 next 已追上 replayEnd 且该帧没有本地采样。恢复历史 tick 不再读设备状态。

capture 成功计数是提交与 channel 调用后的结果；若要区分“已经 sample”“已存入 Core”“发送失败”，应分别记现有调用的结果，不能只用最后的 captures 计数倒推最早阶段。已经准备的前沿输入会在恢复开始前采样并 flush，不能为了记录事件再次 capture。

同一 source schedule 不自动保证跨候选的 full input tape 一致：DOWN 取当时本地玩家位置作为目标，ready/实例/死亡也参与 sample。不同预测轨迹可能改变这些状态。A/B 应同时保留每端 rawHex tape SHA 与活动量；若 tape 不同，注明实际输入负载差异。每次测试内部的双端/第三世界一致，证明的是该次实采 tape 上的正确性，不是两次候选输入相同，也不能把所有耗时差值都解释成纯计算收益。

### 2.3 b243 的预测差异是手机负载的重要源码假设

基线 `RollbackSession::Configure` 仅在 Hybrid 打开 directTouchIsAbsolute；`Prepare` 对所有非 Hybrid 的 predicted seat 清空 analogMode、x/y、unlimited/touchUsed/touchBomb。它清的是整个模拟移动覆盖，**不只清坐标变化的那一帧**。

common `NetplayCore.cpp::PredictInput` 约 235–275 行已有持有 joystick 与绝对 target 的能力，同时保留 predictableButtons、三帧数字方向预测限制和 touchBomb=false。`NetplayProtocol.cpp::operator==` 约 119–125 行比较完整 FrameInput，包括 exact float x/y 和 flags。`SubmitRemoteInput` 约 138–172 行只有在实际输入与已使用的预测输入不同且该帧已模拟时才请求回滚；多个不符收敛为最早的待回滚帧。

所以持有目标在 mode 0 中被预测成 None，可以在实际样本到达时制造修正负载；停指不等于没有 analog 输入。这是可直接从源码推导的假设，**尚不是对真实手机卡顿原因或改善幅度的实测结论**。连续 MOVE 也会使 held-target 预测跟不上新的真实目标，候选不能保证消除所有回滚。

另有一个尚未验证的表示层线索：free joystick 先在 TouchController 中计算 state.x + stick_x * speed，随后 Application::sample_keys 又减去 state.x、除 scale 并限速。数学上是持有速度，但浮点加减、位置量级、focus/scale 变化可能让固定轴生成不同的实际数值。当前新 probe 已存 xBits/yBits，可在 hold-stick-vector 段核实；只出现位型不同（例如 +0/-0）还不能直接等同于 Core 数值比较不符。此处未新增测试参数、未改输入量化或协议，也未认定它已经造成实际回滚。

本轮生产候选由 deep 独立修改两处：非 Delay 设置 absolute，移除 Prepare 的 title 层 analog 清空。只读审查发现 Capture/NeedsCapture、Complete/Restored、发送、Pump、H8、D0、phase/帧调度未因此改变，也没有新增逐帧分配/日志。原输入最终仍以完整 FrameInput 对比与恢复校正。候选的组件与性能结果由 root 单独记录；本文不将“已看源码”写成“已通过”。

## 3. 每帧移动会不会让包、队列、repair 变重

### 3.1 网络包大小由帧数和 ACK 尾部决定

common `NetplayProtocol.hpp/.cpp`：协议 v4，最多 32 个冗余样本，每个样本固定 **12 B**，header 固定 **44 B**；InputPacket 最大 **428 B**。定向 relay envelope 加 2 B，成为 430 B。坐标/flags 数值改变不改变样本长度，没有“坐标越变包越大”的编码分支。

`NetplayCore.cpp::BuildInputPacket` 约 429–467 行按未 ACK 的连续帧复制尾部，最多 32 帧；已 ACK 完整时可构造 ACK-only。它不是变化量压缩。同样的 held 输入仍可出现于连续逻辑帧，键盘样本也使用同一结构。

`SessionChannel.cpp::LocalCaptured/SendInputs`：新 capture 触发已有发送义务，普通定时重发默认 32 ms（Adonis phase 为 16 ms）；已有 capture deferral 不造新采样、不提前建包，并在模拟/恢复/yield 前 flush。发送端 bufferedAmount 超过 256 KiB 会抑制普通 SendTo，但代码仍可能构造和编码该轮包。repair 根据 ACK frontier 停滞/仍欠 ACK，TH09 interval 为 InputRepairBudget::StalledMs = **80 ms**，不检查触控坐标是否变化。SendRepairTo 的实际成功与尝试需分开计数。

可能的间接链为：预测不符更多 → 恢复/本地调度更忙 → 新输入或 ACK 接纳进度变慢 → 尾部变长/重发更多。这里只证明各机制存在，未证明这条因果链在某个手机样本中成立。

### 3.2 transport 与 relay 的已知复制/排队

| 边界 | 当前源码行为 | 应怎样解释 |
| --- | --- | --- |
| WASM → JS send | common `BrowserPeerTransport.cpp`，`eagler_peer_send_to` 约 610–637 行 | HEAPU8.slice 拷贝 payload。relay 再分配 payload+2 envelope 并 set；RTC inputDc.send 使用 payload。send 返回 true 是 API 接受，不是对端接纳。 |
| RTC repair | 同文件 `eagler_peer_send_repair_to` 约 638–652 行 | 只走 RTC controlDc，有现成 32 KiB packet/buffer gate；relay 路由直接返回 false。不能把 relay 与 RTC repair 数量不一致当失败。 |
| relay server | `server/netplay-relay.mjs` 约 1098–1165 行 | 有每连接 gameplay message/byte gate，定向 envelope 用 subarray(2) 去除；每 edge forwardCounters 已存在，普通接收/转发不判断模拟坐标。 |
| relay 保留 payload | `server/relay-flow-control.mjs::createBoundedRelaySender` 约 53–94 行 | 为避免小 view 持有大 TCP buffer，发送前精确分配并复制；带 delay 的路径在 timer 到期递归 send 时再次复制。pending 字节与 WS bufferedAmount 合并计 256 KiB；延迟 timer 上限 1024。不能为减少 copy 私自去掉资源边界。 |
| JS receiveBinary | common `BrowserPeerTransport.cpp` 约 93–117 行 | ArrayBuffer 使用 Uint8Array view；ArrayBufferView 分支 slice 字节；Blob 有异步 arrayBuffer 阶段。随后 push 到 received。此队列在所读函数未见独立长度 cap。 |
| JS → WASM Poll | 同文件 `eagler_peer_poll_size/copy` 约 703–731、C++ Poll 约 921 行 | 用 receivedHead 读头，HEAPU8.set 一次复制到可复用 vector；没有每包 Array.shift。head≥256 且占数组至少一半时 slice 剩余引用，属于偶发尾部整理成本。 |

因此正常 dequeue 是 O(1) 头索引；尾部整理仍与队列余量相关，不是随坐标变化直接增长。relay 的 payload copy、RTC 的 repair control lane、host 的 Promise queue 是不同层，不能从某层 heartbeat 持续就断言其他层没有排队。

## 4. native Pump 接纳边界及可复用证据

`RollbackSession::Poll`（候选约 169–190 行）先消耗 startupPending，再读取 transport；它最多循环 256 次筛除 startup/hash 特殊消息，并对本会话 InputPacket 解码及 ValidInput 检查。`SessionChannel::Receive` 另有 256 个 Poll 调用预算，再 DecodeInputPacket、检查会话/座位/时序并调用 Core ApplyInputPacket。两层 cap 不能简单相乘后当作每次实际工作量，也不能将外层 cap 写成“最多只处理 256 个底层消息”。

`ApplyInputPacket` 先验证整包事务，再提交 remote input/ACK。每个实际样本的 SubmitRemoteInput 得到 Accepted/Duplicate/PredictionCorrect/RollbackRequired 等结果；当前 Channel 已接收 `RemoteInputResult result`，但未将其公开为逐包诊断。ConfirmedThrough 是连续实际输入边界，后来的帧已到但前面有洞时，A 可以保持不变。

| 已有观测 | 可以复用 | 不能证明 |
| --- | --- | --- |
| `th09_network_pacing_info` | version、B、network/effective scale、F（prediction frontier）、A（remote actual contiguous exclusive） | A 不动不是线路归因；B 不等于 replay debt。 |
| `th09_rollback_info` 8 项 | Frame/next、公开 confirmed exclusive、corrections、resimulated、captures、RepairsSent、Ready、rollback_published | 第 5 项索引为 repairs，第 6 项为 ready；**这里没有 sent/received packet counts**。公开 confirmed 也不是 remote A。 |
| Channel getters | LatestCapture、PacketsSent/Received/Ignored、RepairsSent、PeerFrame、Adonis statistics | packet 收到计数涵盖控制/重复/ACK，不等于新实际输入，PeerFrame 是建议性 senderFrame。 |
| `th09_touch_state/probe` | context/ready/dragging 与玩家位置/速度/当前 applied MotionSample | touch_probe 可能是恢复帧的 applied motion，非刚采样输入；无 world 时早退可保留旧 static data，必须伴随 validity。 |
| `received.length - receivedHead`、transport buffered getters | O(1) 队列深度、发送 buffer、RTC repair 已成功计数 | 没有每个队列头的到达时刻，不能用“最后任意包到达时间”代替 oldest age。 |
| 既有 `network-transport-probe.mjs` | 默认关闭；intent/API WS send/message/poll-copy 的包身份、前后队列深度、clock/overflow/coverage；安全边界 pacing 读取 | scope 明确排除 OS arrival、物理输入、exact native acceptance、Blob decode；只覆盖 relay，不可称为已覆盖 RTC receive。 |
| 既有 relay transport probe | relay receive/scheduled/send/cancel、pending/bytes/buffer、双时钟及 hook ownership | 不证明浏览器 message 派发或 native Apply，heartbeat 也不是 gameplay acceptance。 |
| 已有 rAF / callback 诊断 | request identity、active epoch、取消/暂停、未恢复 pending 与 callback 成本 | Node polling 缺样本不能当 rAF 未派发；page 与 Node clocks 不直接相减。 |

既有 network probe 的 `attachCore` 包装 JS 暴露的 `_th09_rollback_pump`。C++ rollback_tick 内调用 pump_rollback_transport 不保证经过这个 JS 属性，因此 pumpCalls 不是所有 native Pump 的完整计数。poll-copy 是底层字节已取走的证据，但 title/Channel 之后仍可能过滤或拒绝；必须继续保留 `exactNativeAcceptance:false`，直到补充真实接纳点。

## 5. 最小、默认关闭的观测补丁设计（尚未实现）

先复用上述 counts/身份，只补缺失边界；不新建发送协议、不自动输入、不添加 frame pump、不重试消息，不启用在线 information-pacing shadow。建议唯一实验开关 `MOBILE_INPUT_TRACE=1`，配套 development-only native getter；默认关闭时不安装 wrapper、不分配环、不调用记录时钟。若某条原生 hook 需要编译分支，则未启用版本直接编译掉，不要求所有玩家承担观测成本。

### 5.1 各层最小记录点

| 点位 | 新增的紧凑记录/现有值 | 保持的行为 |
| --- | --- | --- |
| source owner | 原始计划事件 ID/时间（实验才有）、实际事件/投递计数、当前 epoch/generation、命令类型、pointer ID、post 结果 | planned 与 actualSubmitted 分开；不因记录补发迟到事件。 |
| managed message 接收/dispatch | 同一 realm 的 receivedAt/startAt/endAt、command kind、local serial、收到时与执行时 lifecycle generation、队列待处理/处理中数量、gate/drop 原因 | 在原来调用处记录；不挪动 async 命令，不给 live input 增加 request/ACK 消息。 |
| native gesture/capture | 已调用 pointer/control/cancel 次数、fresh sample frame、ScheduleLocalInput 和 Capture 成功分别计数，实采完整 12 B 表示；last changed analog/frame | 在既有 sample/Capture 结果处记录。x/y 保留 float32 位型或准确值，不仅记 buttons，也不从 wire tail 最后值猜采样。 |
| transport send/receive/dequeue | 复用包 header 身份与 lane；enqueue/dequeue ordinal、depth/maxDepth、API 成功/失败、bytes、compaction 次数和当前分支搬移的引用数 | 不遍历队列、不保留 payload，不调用额外 Poll；relay/RTC input/control 必须分列。 |
| native Apply | Channel 既有 packet 的 session/seat/sequence/first/latest/count，Apply 成功/拒绝与现有 result，remote A before/after、rollback request before/after | 复用已解码 packet 和 result。若需要逐 sample 分类，计数置于现有 SubmitRemoteInput 循环，不追加第二次遍历或模拟。 |
| native Pump / replay | Pump entry/return 序号、处理计数差、用尽现有 Receive budget 的标记，Restored 起点/终点、next/F/A、R=max(F-next,0)、corrections/resim/captures | “用完 budget”不自动表示还有包；需同时看已观测 pending。只观察既有调用与完成，不新增调度。 |

补丁的价值是拆分边界：设备/host 没有生成新输入、生成但尚未 native capture、capture 后发送等待、浏览器接收后尚未 dequeued、已 dequeued 但未被 native 接纳、已接纳但仍在恢复。它本身不是平滑度修复。

### 5.2 严格有界与时钟要求

- 每个 owner 使用固定大小结构：例如 64 个 transition record 的环、固定 counters、最后一次 full sample；snapshot 的 peek 只读 scalar，drain 最多 16 项。每条现有事件只做 O(1) 更新，不创建随运行时长增加的 Map/数组。packet identity 使用固定 header，不 hash 整个会话或复制所有 input tail。
- enqueueOrdinal/dequeueOrdinal 不随 receivedHead 整理而重置；owner/state 更换明确加 epoch。若到达时间只保存在 64 格环里，队头 ordinal 已被覆盖时，oldestArrivalAt 必须为 unknown 并记录 overwritten，不能制造年龄。长队列本身仍照原传输语义运行，诊断不能截断/丢包。
- 第一次异常冻结有界前史，保留有限后史及最终未恢复状态。异常门槛仅用于捕获事件类型；250 ms（若复用 rAF latch）不是新的体验验收线。环覆盖与未结束过程有显式 overflow/censor。
- 复用既有 100 ms Node 轮询获取紧凑状态，不每帧 CDP；真正的 send/enqueue/Apply 时间由启用后的本地 hook 记录。Node request/return、host page、runtime page、relay Node 各自携带 clock name/timeOrigin 与有效区间。SDL_GetTicks 和 emscripten_get_now 不直接当同一原点。
- native 只读 getter 在合法的非重入边界读，刷新 HEAP view 后取固定长度。不得在 wasm import 内再次调用 native pacing/诊断导出。wrapper 原调用只执行一次，保留 this/返回值/异常，最终验证 hook ownership；恢复时只恢复自己仍持有的 hook。
- full sample 的 12 B 指协议字段表示；不得将带有编译器 padding 的 C++ FrameInput 对象直接 memcpy 12 B。记录现有字段/float32 位型，再按既有协议布局离线复原即可；capture 的时间与 wire 第一次可见的时间分别命名，未观测 capture 时不得以 firstWireAt 冒充。
- counters 与 sample 不能替代 payload 全字段一致性验收。完整输入 tape 的短测试属于另一个有明确帧数上限的验证产物，不能让日常观测环变成全量录制器。

### 5.3 可下的结论与必须保留的 unknown

| 联合证据 | 可定位到 | 仍不能声称 |
| --- | --- | --- |
| planned 到期但 actualSubmitted 晚 | 实验 source 定时器/本地事件调度迟到 | 真实手指在该时刻必然产生事件；不能报告为网络输入延迟。 |
| host post 成功，runtime 收到晚/未见 | hosted 投递到 handler 的边界尚未闭合 | postMessage 成功等于 native 执行；单端时钟不能精确拆跨 realm 延迟。 |
| runtime handler 已收到，dispatch 被 queue 挡住 | managed 消费排队 | 这是 rollback 计算成本或线路丢包。 |
| gesture 更新，但 fresh capture 停、R>0 | 本地恢复期间延后新采样，与 once-only 契约相容 | 仅凭未 capture 就认定手势丢失。 |
| capture 已提交，SendTo 未接受/普通 lane backpressure | 发送端义务/缓冲边界 | 不能因为 capture++ 就说对端已收到。 |
| receiver enqueue 增加，dequeue 不动且 depth 增大 | JS queue 到 native Poll 尚未推进 | OS 线速、relay 或浏览器线程单一根因。 |
| dequeue 后 Apply 拒绝/duplicate/A 不前进 | 原生验证/重复/连续输入缺口可分列 | 最新 sequence 到达即补齐 A；A 不动即线路异常。 |
| Apply 推进 A，R 仍大且 replay 无完成 | 真实输入已接纳，剩余本地恢复/调度债务 | 不能把这段停顿记作等待网络信息。 |
| send 有证据但接收无覆盖，或环已覆盖 | 未观测区间/删失 | 不填造网络时延，也不当作已解决。 |

## 6. managed fastpath 候选及其屏障（本轮不实施）

既有 keyboard 快路给了结构参考，但 touch 不能只加进同一个 if。当前 touch 命令执行体没有 keyboard 的 launched/stopping/hidden/network overlay/spectator 完整 gate；初始化也要等 core/FS/resources。configure 与 launch 可 await，launch 包含启动画面/网络连接等待。只因 core 存在便即时应用 controls，会越过仍未完成的配置/开局。

最小独立实验应先验证“当前可接收输入”的同步 owner 状态与 **同 epoch 内的 lifecycle generation**：

1. 保留 message 的 source/origin/protocol/game/epoch 校验；handler 进入时记录 local generation，真正执行前复核。无初始化完成/未 launched/正在关闭/隐藏或覆盖态时，新的 DOWN/MOVE/非零摇杆不能抢先应用，也不能为了将来 launch 保留成有效手势。
2. blur、visibility、stop、context loss、pagehide、模式禁用/手势所有权更换是明确屏障；在屏障记录 generation 并执行现有 cancel。普通 controls/sensitivity 快照不能被滥用为无条件 cancel，因为会破坏正在拖动。
3. DOWN→MOVE→UP 顺序必须属于同一 pointer/generation。已有 queued DOWN 若在 cancel 后被排到，可能重新开启拖动；单纯使 cancel 快速执行而让其余事件继续排队并不充分。MOVE 在 native cancel 后通常不会单独重建 dragging，但不能以此掩盖此前仍排队的 DOWN 或非零 controls。
4. 若采用现有同步 bridge，应复用 exactly-once 契约：false 只表示完全未消费，才允许投递 fallback；已明确接收并按生命周期拒绝的旧事件不能再误报 false 后排到另一条路径。收到时有效但当前已失效的消息如何被“消费但丢弃”，必须有明确的本地结果分类，不新增协议命令。
5. 不先做 directMove 合并优化。TouchController 在每次 delta 后限幅，焦点速度比例/边界条件也能改变中间结果；将多个相反方向 MOVE 直接换成最后一个绝对屏幕点可能改变 target。需要单独证明输入等价后再考虑合并。
6. async configure/resources/launch/stop 仍按原命令序列运行。输入快路只处理既有同步输入命令，不能借机重写协议、绕过生命周期 gate，或影响资源/保存所有权。

## 7. 可复用测试与尚未验证项

| 既有测试 | 可复用的证据/fixture | 当前覆盖缺口 |
| --- | --- | --- |
| `eagler-touhou/tests/ui-main/touch-input.test.mjs` | ready/launched/spectator/epoch gate、joystick 一次 rAF 合并、稳定接触 ID、cancel 后 MOVE 不再发送、suspend/dispose | mock ports，不运行 TH09 managed handler/native/网络。 |
| `eagler-touhou/tests/test-touch-runtime-protocol.mjs` | direct bridge true 不重复 post、false fallback、throw 不重试、旧 epoch/origin fallback | helper 使用 mock bridge；不能证明 TH09 已有该桥或 lifecycle 处理正确。 |
| `eagler-touhou/tests/ui-main/runtime-service.mjs` | 当前文档身份、optional-response live input、spectator 拒绝、替换文档/旧 session 清理 | Launcher 服务边界，不是 runtime Promise queue 内部。 |
| `th09_web/tests/server/managed-storage.test.mjs` | generated normal/multiplayer shell，真实 mount ownership 的 VM fixture | window.addEventListener 是空实现，直接取 command()；未经过 message gate/排队/事件次序，也未验证移动。 |
| `th09_web/tests/multiplayer/touch-peer-browser.mjs` | native TouchController、双端 FrameInput/replay 可用的基础；本轮已扩展计划化刺激 | 计划仍由 capture slot 推进，是确定性输入 oracle，不是独立 wall-clock 手势，未经过 hosted managed 链。 |
| `eagler-touhou/tests/test-relay-flow-control.mjs` | relay pending/buffer/close 的独立边界 | 不代替浏览器接收/native acceptance。 |

后续最小验证分两条，不混成一个“手机 PASS”：

- **当前候选**：mode0/D0/H8 保持相同 native 刺激和输入合同；持有 target/vector、连续慢/快移动、释放与重按；完整 12 B 实采 tape、首次发送/重发、远端 confirmed 与最终状态一致。计划事件时间、实际投递时间、实采 logical frame、可见位置反应分别报告。键盘/analogNeutral 的旧响应 helper 不直接用作 analog 正确性裁判。
- **managed 独立路径**：先用保留真实 message listener 的小 VM 契约，阻塞 initialized/configure/launch、在 queue 中穿插 DOWN/MOVE/controls、触发 blur/cancel/epoch 替换，验证无 stale rearm 和 exactly-once；再经过合法 Launcher carrier 的真实 browser/device 入口，记录实际命中的 host TouchEvent / host PointerEvent / child PointerEvent owner。当前普通 Launcher 的 Android direct 和 joystick 都有 host 路径，不能把平台名当作 bypass 证据。桌面伪装 mobile UA 仍标为桌面刺激。

当前没有实测数据证明 held prediction 已解决手机卡顿，没有数据证明 managed queue 是该设备故障的主因，也没有降低延迟/提高 FPS 的反事实收益。已有 information-pacing shadow 继续离线；它的误报/漏报结果不因本轮手机假设而重写。

## 8. 独立补丁审查记录

- 已只读复核 deep 锁定的 RollbackSession.cpp 两处候选与 session.cpp/adonis.cpp 的旧 None 断言更新。源级没有发现把设备 delta 重复消费、绕过完整 input 校正、扩大 H8 或改变模式调度的新增逻辑。
- 已读完 `mobile-prediction.cpp` 311 行与 run-tests 注册：三组 held 1–8 帧及第 9 帧阻塞；16 类 late change 固定最早回滚到 F2、预捕获 F6 后恢复四帧且 captures 不增；首次未知、三帧按钮界限、乱序补洞/duplicate/conflict/非法输入均有显式断言。State 模型不是真实 WorldState，但 used/confirmed 每帧的独立全字段 oracle 补足输入契约；未发现阻断项，未执行。runner 将间接 include 的 session.cpp 纳入来源记录。
- 已读完新版 `touch-peer-browser.mjs` 197 行：新增计划明确用 capture slot 换算时刻，是确定性 oracle 刺激；三世界顺序执行、接收方 frontier 延迟及达到 limit 后的 delivery-only 尾部均有明确 scope。local wire 保留完整 12 B 且重发不许改历史；confirmed spectator tuple 只包含 buttons、motion kind/unlimited、x/y，不能独立比较 touchUsed 等全 flags，后者依赖 native 组件而非伪称 tuple 全字段覆盖。提出 failure snapshot 应参照已有 peer harness 使用有界等待，避免页面异常拖住失败报告。
- `touch-input-probe.mjs` 首轮反馈的两处报告口径已在锁定版本回读确认：status/分布改为条件渲染推进代理，明确 causalEffectVerified=false 与位置/焦点/死亡/覆盖等混杂；events + futureEvents 保留完整计划，dueEvents 与 futureCensoredEvents 分列。新观察到的 v4/seat/count gate 与三个 uint32 的重复样本比较保留 12 B 精确性，同时避免为冗余样本反复格式化 hex。未执行测试。
- `smoothness-browser.mjs` 最后只读复核确认 TOUCH_PROFILE 默认 off 且懒加载，拒绝与 keyboard tape/旧 RESP 混用；在 connect 前安装观测，mode0/D0 显式配置并由 native ABI 验证；事件定时器在第一次 active callback 设原点，此后不依赖 F/capture/transport 前进。已有窗口汇总与生产 native 帧规则未被该接入更改。其 observeLocalInput 位于原 send 调用和 client delay/drop 注入之前，因此 firstWireAt 是序列化本地输入的 send-intent 观察，不是 API 已接受或对端接纳；最终 helper 的 wireObservation boundary 字段已明确这一限制，并已回读确认。
- `touch-peer-browser.mjs` 最终收尾已只读确认 5 秒 failure snapshot Promise.race、预先检查 report/failure/cleanup 文件身份和分阶段 cleanup；未发现本轮新刺激/输入 oracle 的阻断项。最终文件/构建/执行身份与实际结果仍由 root 的串行执行记录负责，源码审查不等于运行通过。

## 9. free joystick 表示链的后续只读结论

本节只确定一个可独立实验的输入生成改法，**未实现、未执行数值 fixture，也未证明这条链在实测中制造了回滚**。root 已另行执行候选组件、JS 契约和桌面深回滚 oracle；这些结果不验证本节尚未修改的摇杆生成方案，也不验证真实 Android 或 managed queue 的因果贡献。性能解释仍以完整对照及其独立执行记录为准。

### 9.1 可复用的是现有原始轴，修改位置只能在 fresh capture 的 adapter

`portable/input/TouchController.hpp` 约 18–23 行已有公开的 `stick_x` / `stick_y`；`controls()` 与 `Application.cpp::th09_touch_stick` 将传入的有符号轴除以 `32767.f` 并夹在 [-1,1]。这两个字段不经过玩家位置，也无需新 wire 字段。`TouchSample::stick` 已区分 free joystick 和 direct drag。

当前 `TouchController::sample` 约 56 行先算 `s.x + stick_x * sampleSpeed`，`Application.cpp::sample_keys` 约 345 行再算 `(sample.x - state.x) / scale`。去掉位置加减的最小候选，是**仅在原 sample.stick 分支**用现有轴计算 `stick_x * sampleSpeed` / `stick_y * sampleSpeed` 作为分子，随后继续既有除 scale、零 scale 置零和向量限速。保留现有 motion、ready、方向键、上下文、启用及拖动优先级门，不更改 direct target 路径。

这里不需要将 `TouchSample::x/y` 的全局含义改成速度，也不需要新增持有缓存。`GameSession::MotionSample`（`th09_web/cpp/game/GameSession.hpp` 约 31 行）已有 enabled、x/y、unlimited、target；仍然保存最终捕获的速度向量。不要在 `GameSession::movement`、恢复 tick 或重绘中读取当前 gestures，也不要把玩家现有 velocity / step 当作原始轴：前者会重采设备状态，后两者已经受游戏帧、scale 和 rate 影响。

### 9.2 三个速度读取位置必须保留，不能借此统一焦点阈值

| 位置 | 已有选择/处理 | 对最小候选的限制 |
| --- | --- | --- |
| `TouchController.hpp::sample` 约 54–56 行 | key_slow 或 out.keys[16] 为真时用 s.slow，否则 s.fast；out.keys[16] 包含 touch focus / two-finger | raw stick 位移使用这个 sampleSpeed；不能无条件用 state.fast，也不能改用后面的 automatic_focus 结果。 |
| `Application.cpp::touch_state/sample_keys` 约 321、343–346 行 | s.fast/slow 直接来自 resource movement，没有预乘 scale；capture 限速用 `automatic_focus ? player.input.fire_frames >= 7 : keys[16]`；先除 base_scale * effect_scale，再 limit_vector | 两个速度选择含义不同，除法和零 scale 行为必须原样保留；不能把两处速度当作总能约掉。 |
| `PlayerMotion.cpp::update` 约 20–24、36–48 行；`GameSession.cpp::movement` 约 82–93 行 | 实际游戏帧用 `automatic_focus ? fire_frames > 7 : bool(keys & 4)`；movement 再限速，然后应用 scale、rate、位置累加与边界 | capture 与实际帧读取输入状态的时点不同，>=7 / >7 不能按字面顺手统一；不改现有游戏运动、位置舍入、边界或时间倍率。 |

即使位置消减项消失，实际轴变化、focus / two-finger、角色速度、base/effect scale 和动作按钮变化仍可改变完整 FrameInput。持有时若只观察 x/y 位型，还不足以证明其余输入字段恒定。

### 9.3 能保持的契约与不能承诺的等价

- **能保持格式与所有权**：v4 的每样本 12 B、Joystick 作为有界速度、MotionSample 布局、只采样一次、Core 完整字段比较和回滚校正都无需改变。已有 float32 x/y 装下新采样结果；不需要新消息、协议版本或 epsilon 比较。
- **不能承诺旧新 tape 一致**：浮点 `fl(fl(position + delta) - position)` 不保证等于 `delta`。绕开它正是有意改变部分新采样值，因此相同手势的旧新 wire、游戏轨迹及新录 Replay 字节可能不同；数学公式等价不是位级兼容证明。如果要求所有历史可达状态的新采样位型都原样保持，就不能同时消除真实存在的位置相关结果差异。
- **旧 Replay 读取路径可保留**：`GameSession.cpp` 约 84 行在 is_replay 时直接调用 motion.playback；`TouchMotionTrack.hpp` 约 10–13、29–36、40–59 行的 THMOTION v2 记录玩家 movement callback 的 pre-timescale velocity，回放不重新调用输入 sampler。保持记录/播放格式与消费顺序可保留旧文件语义，但候选自身的新录 Replay 仍必须验证重放与原执行一致，不能以格式没变替代 oracle。
- **回滚状态无新增字段需求**：`WorldState.cpp::Save` 约 146–148 行已经保存 motion_input、MotionTrack tick/cursor/flags 与 append-only stages。只在 fresh capture 生成输入、不新增外部缓存时，不需扩展状态格式；这并不意味着新旧候选的具体 state hash 必然相同。

### 9.4 进入实现前的最小证据门槛

1. 在既有 hold-stick 段先判定是否真有 **数值 x/y 改变**，把 +0/-0 仅位型变化单列。必须同时记录/固定原始轴、context/instance/ready、方向键、manual/two-finger focus、automatic_focus/fire_frames、resource speed、scale 和 source command 次序。现有 rawHex/xBits/yBits 足以发现变化，单靠它们不足以排除所有上述来源；auto-fire 本身也会改变 buttons。
2. 经 root 另行排期后，用实际 float32/WASM 算术做小的 capture fixture：不同位置量级/临界舍入点、零/非均匀 scale、焦点转换、对角限速、neutral/释放/重按、方向键优先、disabled/not-ready 与 direct 模式不变。精度判据先固定；不能只用 JS double 的近似值证明 native 等价，也不能为了减少 mismatch 盲目降低轴精度。
3. 如出现旧值随位置变、新值在固定所有非位置状态时稳定，仍只证明输入表示差异。要认定它制造实际回滚，需对齐接收方该帧实际用过的预测输入、native 接纳与最早 pending rollback，而不只是本地 send-intent 或总 corrections。多个不符可合为同一次回滚，不能将数值变化数当作回滚次数。
4. 如值得做独立候选，只改该 sampler 分支，保持此轮 held-prediction、H8/D0 和 managed 链不变；用每次实际 tape 的双端/第三世界状态与 Replay oracle 验证。若新旧 actual tape 改变，明确这是输入行为变化的独立试验，不能按完全相同 workload 的纯计算 A/B 解释。此时仍不承诺它解决手机大卡或任意网络断流。

## 10. 真实 Android 普通 Launcher 的既有入口与 owner 验收边界

后续采用 `eagler-touhou/docs/playbooks/mobile-browser.md` 与 `runtime-launcher-package.md` 的普通同源 Launcher iframe。root 已确认现成 Android 实验设备连接；**本审计没有探测、forward、启动/接管浏览器或执行真机验收**。本节是待执行入口与记录要求，不是设备不可用的声明。

### 10.1 先复用正常准备和启动流程

- 产品入口来自 `src/contracts/product-catalog.mts` 约 228–267 行：普通 TH09 是 `./runtime/th09/th09.html`，多人是 `./runtime/th09/multiplayer/th09.html`。多人角色/房间和 measured startup 仍由既有 Launcher 流程准备；`app/services/multiplayer-launch.client.ts` 约 245–315 行先完成 package/Runtime prepare，核对当前 room intent 与 prepared epoch，再调用 runtime.launch 并等待 firstFrame / gameplay path。不能只打开裸 th09.html 或 presentationServer 就声称普通 Launcher 通过。
- `app/runtime/RuntimeHost.tsx` 约 22–52 行维持根生命周期唯一 iframe / RuntimeService，视图切换不另建 Runtime；只有真实 detached frame 才可释放该 owner。不得安装第二套 DATA/provider/Runtime，或用第二浏览器/CDP client 接管已有 harness session。
- `app/services/runtime.client.ts::prepareOwned` 约 658–716 行要求无既有 session/close/file owner 冲突、正确 Package generation、同源 entry、published Runtime 或已验证 development Host authority；创建当前 session 并 retain generation 后才加载 Runtime。`src/launcher/runtime-preparation.mts::managedRuntimeUrl` 约 90–104 行添加 hosted=1、managedData=1、gameGeneration、runtimeVariant；service 再添加 runtimeEpoch。已有生成目录/版本名不能替代最终 HTML/JS/WASM 与 candidate 构建身份校验。
- 同 service 的 `runtimeIdentity/provideData` 约 391–404、446–461 行验证 child URL、origin、epoch 与 document identity，异步 DATA 读取前后复核同一 owner。TH09 `managed.mjs` 约 123–133 行要求当前 parent managed DATA provider，并经 Package generation 取得资源后才 ready。普通/MP save root 仍各自隔离，关闭继续走 guarded save/exit；不伪造 provider 或以 Blob/about:blank carrier 绕开这些边界。
- `RuntimeService::launch` 约 808–832 行只接受 prepared phase，发送 launch 后等待当前 epoch 的 first-frame 才进入 running；超时可能已经执行，不自动销毁进度。后续验收应保留 ready、prepared、launched、firstFrame 与 native gameplay-ready 的各自时间，不将首帧当作触控/网络均已通过。

### 10.2 Android 要验证当前实际输入 owner

普通 Android direct 的源码主路径是 host PointerEvent → RuntimeTouchOverlay → TouchInputController → RuntimeService.postInput → 既有投递 helper → TH09 managed queue → native TouchController；joystick 经 host rAF 合并 controls。iOS direct 使用同一 host owner 下的原生 TouchEvent。Runtime 内 child PointerEvent handler 是另一条入口，只有真正命中它才算覆盖。

`touch-input.client.ts` 约 64–66 行要求当前 epoch、ready、launched、非 spectator；suspend/cancel/dispose 会清 pending host rAF、直接接触和摇杆，并发送既有取消/controls。`runtime.client.ts::getInputContext/postInput` 约 296–306、1044–1049 行再验证当前文档和 envelope。TH09 message listener（`managed.mjs` 约 99–104 行）检查 parent/source/origin/protocol/game/epoch 后，touch 仍走异步队列；它与 keyboard 的立即分支不同。不能因为同源、有 bridge helper 或 child handler 存在就推断 native 已即时执行。

真实验收至少分别保留：命中的 host/child element 和 pointer owner、一次 DOWN/MOVE/UP/CANCEL 的 source 次序、发送/queue 执行/native sample/输入接纳/回滚完成/呈现的实际观测边界。每段时间使用所属 Page/Node 时钟及 timeOrigin；观察不到的阶段标 unknown，不用较早阶段的成功推断较后阶段。焦点/帮助面板、orientation、背景/前台、指针捕获丢失、普通 controls 快照与 session 替换要分别检查，确认旧 DOWN/非零 controls 不在异步屏障后重新激活。

### 10.3 验收结果单独命名

记录具名设备、实际 OS/browser/version、方向、origin、代码/资源 generation 与两端角色；先新页面正常进入并记录 user gesture、首帧、音频和输入焦点，再做 direct 持续移动/停指/释放与 free joystick。源级预期、桌面 native 刺激、桌面 mobile-UA、真实 Android 和网络条件分别报告。现有 `tests/smoke-th09-mobile-fit.mjs` 明确只是桌面模拟手机视口的几何检查，且保留旧 UI selector；不能直接充当当前 React Launcher 的真实手机回归入口。

若后续使用 adb reverse / TCP forwarding，必须单独记录局部调试路线与手机数据连接情况，不将实验转发说成公网部署或正常 relay 网络。真实手机短测也不等于所有 Android 浏览器、长时间稳定或独立 relay/direct 路由均已通过。当前没有本节方案的实测收益结论。

