# TH09 输入运输链诊断计划

## 状态与目标

本文件保留最初的合成联机 fixture 最小 packet trace 设计。编写方案时 trace 尚未在浏览器中运行；下文方案和旧 smoke 分析仍保留当时的证据边界。现已完成 P / M 两次 browser-only burst-01 诊断，实测身份、startup 异常、逐包链和限制见末尾“已执行的浏览器网络诊断”。这些诊断不替换主任务的正式 HOST 对照、release 像素检查或 paired CPU 结果。

目标是将同一个输入包沿以下边界关联起来：客户端发送意图、实际调用 WebSocket.send、relay 的 message handler、relay 实际 send、对端 message handler、JS 接收队列复制到 native、native 返回后的远端实际连续输入 frontier。只记录合成 fixture 的协议元数据，不记录完整 URL、member token、命令行或输入 payload。

本轮先实现浏览器侧 opt-in 模块；relay 侧只保留下面的独立设计，不修改 canonical relay、生产 C++ 或已经证明逐字节来源的 prebuilt fixture。诊断有运行开销，不宣称对性能零成本。默认关闭时保留原来的游戏、输入和 14-field samples 行为。

## 现有证据与不能回溯的部分

已有报告：

- `th09_web/artifacts/multiplayer-tests/rollback4x-responsive-probe-smoke-01-report.json`
- SHA-256：`2b07e98d17158d5d93376c276fbeeeb1cdb2908c7131ce60494ca42f41ccbbd0`
- 参数：release、relay、wall-clock、responsiveness、audio output、rAF diagnostics；900 帧；应用发送 delay 39 ms / jitter 5 ms；**slow=1，非 4× CPU 测量**；无 burst、无配置丢包。

对 raw 的只读复核结果如下。索引是各自 `measure.samples` 的 0-based 索引；新画面间隔指新 renderer submission 所在回调的浏览器 rAF timestamp 差，不是物理 scanout。回调 work 是原报告 C++ 计时字段。

| 端点 | 两次新提交的 sample 索引 | 新画面 rAF 间隔 | 中间 onGameFrame 数 | 中间 C++ work 总和 | 中间 reported confirmed |
| --- | --- | ---: | ---: | ---: | --- |
| P1 | 1406 → 1662 | 1672.815 ms | 255 | 37.825 ms | 416 |
| P2 | 1420 → 1668 | 1640.500 ms | 247 | 31.025 ms | 417 |

P1 的旧 now 采样点从 9503.550 ms 到 11170.780 ms；P2 从 9519.555 ms 到 11155.630 ms，均相对各自 `measure.started`。P1 的中间 frame 值包含 424、423，不能强行按单调 frame 删掉其中行；P2 为 425。

相关本地 capture 已进入 impairment 之前的 wire hook：

| 本地端点 / 输入帧 | firstWireAtMs |
| --- | ---: |
| P1 / 416 | 9031.270 |
| P1 / 417 | 9050.245 |
| P2 / 416 | 9073.775 |
| P2 / 417 | 9086.075 |

这些时间相对各自 `responsiveness.startedAt`，与 `measure.started` 相差约 0.31 ms；不能直接将不同页面的 performance.now() 相减。

两个页面的 `networkPacing` 都为空。`reported confirmed` 来自现有 rollback info，**不是独立记录的 raw actualContiguousExclusive**。本报告不足以逐包证明精确的远端缺口。

现存 `responsiveness.captures` 只保留 frame、buttons、firstWireAtMs 等本地 capture 数据，没有 sequence、ackSequence、ackFrame、每包 first/count。现存 `impairment.planned` 与 `delivered` 是无 packet key 的数组；jitter 会改变 timer 完成顺序，不能按数组下标将二者拼成包级 trace。

已记录成功 send 的 timer 开始到 send 后采样的最大耗时是 P1 453.580 ms、P2 439.715 ms；快照时分别仍有 5、4 个 pending。这个统计不能替代某个具体输入包的发送、relay 接收或对端到达时间。报告没有 relay 逐包收发、客户端 message、native dequeue 时间。因此，**不能从该 raw 还原 416/417 附近的 ACK 或重发覆盖，也不能确定 1.6 s 停顿出在哪一段。**

源码审查没有找到可直接解释此次 1.6 s 的固定阈值。以下是已经核实的语义，不是原因推定：

- `SessionChannel::Receive` 先调用 `ApplyInputPacket`，随后才检查 sequence 是否更新；旧 sequence 不会单凭序号旧就丢弃其中的合法输入。
- `SendInputs` 正常发送与可靠 repair 使用同一次编码后的 bytes；后者是 RTC control lane 的重复包。当前 relay fallback 的 `eagler_peer_send_repair_to` 返回 0，不存在独立的 WS repair lane；relay 模式仍有正常输入重发。
- active gameplay 的 `receiveBinary` 先入 JS 队列；startup 的 onReceive 快速 pump 在 active 后被清空。之后需要原有游戏 / timer pump 消费队列。

## 协议解析与关联

只解析 TH09 输入包，精确匹配 magic `E9NP`、version 4、kind 1。先剥离可选的 `[0xe7, targetPlayer]` relay envelope，另行保存 envelope target。协议 header 为 44 字节，每个 input sample 12 字节，inputCount 最大 32。

| 字段 | LE byte offset / 类型 |
| --- | --- |
| senderPlayer / playerCount | 6 / u8；7 / u8 |
| sessionId | 8 / u64，保存为两个 u32，避免 JS Number 精度丢失 |
| sequence / ackSequence | 16 / u32；20 / u32 |
| latestFrame / ackFrame | 24 / u32；28 / u32 |
| firstInputFrame / senderFrame | 32 / u32；36 / u32 |
| frameAdvantage | 40 / i16 |
| inputCount / reserved | 42 / u8；43 / u8 |

保留 count=0 的 ACK-only 包与 INVALID_FRAME=`0xffffffff`；不能把它们当空闲噪声删除。first/count 表示一段冗余输入范围，不是“一包一帧”。同一个 sequence 的重复发送要保留每次事件；允许元数据字典去重，禁止事件去重。

关联 identity 包含 sessionId、sender、destination、sequence，以及不包含 envelope 的 payload 指纹。指纹用于检测意外差异，不视为加密证明。每次事件还保留各进程内递增的 event id 和 socket id。relay 的 `run.forwardCounters` 不是协议 sequence，必须分别命名。

解析失败、非 ArrayBuffer/view、未知 wire version、意外 route 或 socket 替换都要明确计数。诊断不修改或拦截不支持的数据，原 transport 仍收到原对象。

## 浏览器侧最小挂接点

### 1. 实际 WebSocket.send

在**同一个** harness addInitScript 中，先安装网络 probe，再安装既有 impairment wrapper。现有 impairment 在保存 `prototype.send` 后于 timer 中调用它；网络 wrapper 必须成为它保存的底层 send。

记录原 send 调用前后时间、packet id、socket id、bufferedAmount 前后值、是否抛错。原 receiver、参数、返回值、异常对象均原样透传。这个时刻表示调用了 WebSocket API，不表示数据已经到达网卡、relay 或对端。

可在既有 `observeLocalInput` 之后追加一个诊断调用，记录 impairment 之前的同包发送意图。它不能更新 hosted keys，不能取代现有 responsiveness observer。

### 2. 接收 message handler

已构建 loader 的 `wasmImports` 明确包含 `eagler_peer_connect` 和 `eagler_peer_poll_copy`，namespace 为 `env`；同一个 imports 对象也用于 wasi。通过测试 initScript 包装 WebAssembly.instantiate / instantiateStreaming 的 imports，避免重建生产 WASM；两种实例化路径只包装一次，原 Promise / 返回值和 fallback 路径保持不变。

`eagler_peer_connect` 原函数同步返回后，`globalThis.__eaglerPeerTransport` 和 `state.relay.onmessage` 已创建，事件循环尚未让步。此时包装原 relay onmessage，记录真实 handler entry/exit、packet id、queue depth 前后、route/closed/socket identity。保持原 handler 的 receiver、参数和异常。

这测到的是**浏览器将 message 派发到 JS 的时刻**，不是浏览器网络进程首次收到 TCP 字节。当前 relay 使用 binaryType=arraybuffer；如果遇到 Blob，则标记未覆盖，不能用新的异步 Blob 解码替代原语义。

### 3. Native dequeue

包装 WASM import `eagler_peer_poll_copy`。调用原函数前保存队列头 packet 引用和 queue depth，调用后记录返回的 byte count 与时间。只有返回正数才记为 native dequeue；返回 0 的尝试必须另计。

原函数会先清空当前数组元素，再推进 receivedHead，且会压缩 / 重置数组；因此不能通过 receivedHead setter 直接还原 packet identity。只包公开 `_th09_peer_poll` 也会漏掉 C++ 内部 `BrowserPeerTransport::Poll`。

该事件证明包已复制到 native 输入 buffer。**它不证明 SessionChannel 校验通过或 ApplyInputPacket 接受。** 在 import wrapper 内不调用任何 WASM export，不为读取 frontier 重入 C++。

### 4. 安全返回后的 actual frontier

在原有 `_th09_rollback_pump` 的 JS 调用返回后，以及既有 onGameFrame 完成样本记录后的安全边界，读取可选的 `_th09_network_pacing_info()`：

`[1, outstandingFrames, networkIntervalScaleMilli, effectiveIntervalScaleMilli, predictionFrontier, actualContiguousExclusive]`。

每个 frontier 行关联最后一次 dequeue serial、来源和 sample index。无 dequeue 变化时至多 10 Hz；不加新游戏 pump、rAF heartbeat 或 polling timer。没有 export 的构建明确标记 unavailable，不能用 normalized rollback confirmed 冒充。

如果包很早 dequeue，但安全返回后 actual frontier 仍未推进，先检查是否仍缺更早帧、session / schema 是否匹配和已有 failure 状态。若需精确证明原生接受 / 拒绝，下一阶段必须在 `SessionChannel::Receive` 的验证 / ApplyInputPacket 前后增加专门 native 诊断；本轮 JS-only trace 不具备该证据。

## Relay 侧后续设计（本轮不实现）

首选独立的 test-only Node preload，复用已核验 fixture 和 ws 依赖：

- 在 server connection 建立时为 socket 分配 bounded numeric id，仅映射 player/run，不输出 request URL。
- 在原 canonical message handler 执行前观察 binary message，记录 relayRecvAt、完整协议 identity 与 targeted envelope。
- 在 `WebSocket.prototype.send` 的原方法调用处记录 relaySendAt、目标 socket、bufferedAmount 与同步错误，原参数与返回值保持不变。
- 每 100 ms 记录一次独立 timer 的 due/actual/late；不做追赶式 busy loop。该指标说明 relay JS 事件循环调度延迟，不能证明某个包已经先到达 OS buffer。
- 只在停止时输出 bounded JSON；不逐包 console.log，不读取进程命令行，不影响第三方进程。

当 relay 配置本身包含 delay/drop 时，仅有 recv/send 不能区分 intentional drop、rate gate、route skip 或待发 timer。需要另一个具备清晰 manifest/diff 的合成 trace fixture，在 gameplay handler 的 gate/target/drop 决策和 `sendBounded` 调度处增加极小观察点，并记录实际生效配置。

`relay-flow-control.mjs` 中 delayMs>0 会再次调度调用 send；调用 sendBounded 不等于已执行 socket.send。只有后者的观察点能测实际转交 ws 的时刻。ws send / callback 完成仍不证明远端收到。canonical relay 与当前 prebuilt fixture 在本任务保持未改。

## 时钟、开销与完整性

每个页面和 relay 记录 performance.timeOrigin、performance.now()、Date.now() 的开始 / 结束配对。同进程差分优先使用单调时间。不能直接减不同页面的原始 performance.now()；跨进程的 epoch 对齐需保留时钟不确定性。

如需更严格的跨进程时间区间，在正式阶段之前 / 之后由 controller 用 host-enter、page clock、host-exit 三点形成 offset 区间；不要在测量中增加网络往返。只按 packet identity 关联并不依赖跨进程时钟精确同步。

元数据数组、packet 字典、socket/state 表、frontier 行与错误表均设硬上限；记录 overflow 数量、首个 overflow 时刻及停止原因。溢出后继续原 transport，原游戏样本仍完整保留；不能把无 trace 的区间说成没有异常。不要每 N 包随机抽样，否则可能恰好丢掉补齐缺口的那一个包。

短诊断可记录全部合成输入包；若必须限制到一个范围，事先声明固定的 frame/时间覆盖，保留该范围内所有重复包与 ACK-only 事件，不在看到结果后筛选。记录安装 / 停止时刻、source SHA、wire schema、hook identity 和实际 hook 次数。诊断的 valid 表示观测完整性，不代表网络或设备正常。

## 判读边界

| 已知边界 | 可支持的定位 | 仍不能证明 |
| --- | --- | --- |
| wire intent 早，actual send 晚 | 客户端 impairment timer 或调用 send 之前的调度段 | relay / 对端问题 |
| actual send 早，relay message 晚 | 两者之间尚未被观察的运输 / relay 派发段 | 纯网络延迟，或 OS 首次收包时间 |
| relay message 早，relay send 晚 | relay handler、明确配置的 delay / gate、事件循环段 | 没有 decision 日志时的具体分支 |
| relay send 早，浏览器 message 晚 | 运输、浏览器网络 / JS 任务派发段 | 纯网络原因或 CPU 原因二选一 |
| browser message 早，native dequeue 晚 | JS 队列等待 / native pump 触发段 | 某个包已经通过 native 验证 |
| dequeue 早，actual frontier 晚 | 缺更早输入或 native 验证 / 接受待解释 | 仅凭 first/count 就保证应该推进 |

## 源码依据与冻结身份

### 浏览器模块实现与真实验收之前的检查

已新增 `th09_web/tests/multiplayer/network-transport-probe.mjs`，并以默认关闭的 `NETWORK_TRACE=1` 接入 smoothness harness。当前只支持显式 `--relay`。probe 与 impairment 使用同一个 addInitScript，probe 在 impairment 保存 send 之前安装。生产 C++、relay fixture 未改。

report 的 `parameters.networkTrace` 标注开关，`identity.networkTransportProbe` 保存模块 SHA，成功 / 失败结果都保留 `networkTransport` 完整快照。该快照含 packet 字典、逐次 event 行、pacing 行、对应 columns、counts、overflow、clock 与 hook identity。harness 同时检查 `valid` 和 `coverage.exercised`；未覆盖或溢出不能冒充完成诊断，原游戏样本仍保留。

模块只增加既有 native pump 返回后的只读观察，并在原 onGameFrame 完成样本 / frameDetails / limit 处理后调用 observePacing。它没有独立定时器；不在 poll_copy / connect import 内重入 WASM。停止时仅关闭观察，原 wrapper 继续原样透传，避免 pending send 被停止采样所改变。

已通过 Node 语法检查和纯内存 stub 检查：精确 LE header、targeted envelope、ACK-only INVALID_FRAME、instantiate 两路径共享 import 时不重复包装、原 receiver / 参数对象 / 返回值 / 原异常对象、原实例化 Promise、关闭开关无动作、stop 幂等。stub 没有运行真实浏览器、WASM 或网络。真实 loader hook 覆盖率、诊断开销和长停顿原因必须由后续实际 diagnostic run 验证。

本次只读核查的具体位置：

- `th09_web/tests/multiplayer/smoothness-browser.mjs`：impairment 保存 send 与延迟调用；onGameFrame / report 路径。集成网络 trace 之前冻结 SHA：`42e6ceae2ed5211e4ea4a979a1996523c2370cd45659c011139193b0a2352e62`。
- `th09_web/artifacts/rollback4x-responsive-text-release/th09.mjs`：已生成 loader 的 wasmImports 及实例化路径，只读取文本确认命名。
- `th09_web/sdl-runtime/shared-netplay.mjs`：connect / pump / active 时清空 startup onReceive。
- `th09_web/cpp/multiplayer/netplay/NetplayProtocolConfig.hpp`：MagicGame='9'。
- `th09_web/cpp/multiplayer/RollbackSession.cpp`：title Poll 与实际远端 frontier。
- `th09_web/cpp/sdl/Application.cpp`：read-only network pacing export。
- 实际 common 依赖 `D:/workspace/eagler/th09-eagler/third_party/eagler-common/src/netplay/BrowserPeerTransport.cpp`：connect、openRelay、receiveBinary、poll_copy、relay/RTC repair。
- 同一 common 依赖下 `NetplayProtocol.cpp`、`SessionChannel.cpp`：wire offsets、receive validation、sequence 更新次序、输入 / repair 发送。
- `th09_web/artifacts/rollback4x-responsive-relay-fixture/server/netplay-relay.mjs`：gameplay handler 与 targeted envelope / forward；SHA `56c44ab7520381859b05d21130da40ad605722b42e49ed37c089d0ecf8bbe90c`。
- 同一 fixture 的 `server/relay-flow-control.mjs`：bounded sender / delayed timer / 实际 socket.send。

本文件不声称已找出 ACK 断流、重发停止或固定 1.6 s 阈值，也不从短 smoke 的不同 wall-clock 输入推导跨 run 最终 hash 等价性。

## 已执行的浏览器网络诊断：P / M burst-01

本节记录实际完成的两次诊断，不追溯改写前述旧 smoke 的证据范围。两次报告均通过 harness gate，网络 probe 的 valid 和 exercised 均为 true，所有数组 overflow 为 0。它们没有复现此前 1.6–3.7 s 的停顿；startup 的较长间隔仍存在，以下完整保留。

### 报告与运行身份

| 构建 | 原始报告 | SHA-256 |
| --- | --- | --- |
| P | `th09_web/artifacts/multiplayer-tests/rollback4x-responsive-vm-network-burst-01-report.json` | `1d1344cc91b9914f119fa27c034ff94c5dc7a684c62fbbcb0321cd3a2524c285` |
| M | `th09_web/artifacts/multiplayer-tests/rollback4x-responsive-text-network-burst-01-report.json` | `c4ac38eaedc8b34d02fd9f830a291217167fdd6a0984894c3afb86d842e44fd4` |

两次均为 1800 帧、release、relay fallback、wall-clock 输入、audio output、responsiveness、rAF、HOST_LOAD 和 NETWORK_TRACE；P2 设置 Chrome CPU throttling=4。应用发送 impairment 为 39±5 ms，各页从 active 起 20,000 ms 到 20,500 ms 增加 120 ms，没有配置丢包。

共同 harness SHA 为 `6137632e9b273512a529f2f027a00fa02e1a880f7d274db6bad68c7996df6398`；network probe SHA 为 `090fe224bf5642b3daf4aaf354b67287ba5ad0b39a9d4f336cc1abe6603578c3`。relay 仍为前述 `56c44ab...` 来源，未加入 relay 收发 trace。P WASM SHA 为 `de1760ac76f3aaa8cd49c9f267f37f85e86c70d6bf251679a5c93fefacca9b68`；M WASM SHA 为 `535c358231dc21ada7c2f9df910c1fb0d505cff41612bc79478205afee3993cf`。

这两次运行的墙钟长度、实际输入落入的逻辑帧及每个 burst 涉及的包数不同。它们是相同参数下的诊断案例，不能按相同 frame tape 做性能因果比较，也不能跨 run 比较最终状态 hash 来判定确定性。正式共同帧校验仍以每次运行内部的 gate 为准。单次 P 正式区间的逻辑率约 20.224 / 20.253 Hz，M 为 35.702 / 35.824 Hz；该观察不证明 P 获得吞吐收益，也不证明差异只由代码造成。

### 可复核的字段和关联方法

解析依据每个 `results[side].networkTransport.columns`，不假定两个页面的 packet id 或 event id 相同：

- packet 字典行去掉本页 id 后的全部元数据组成关联键，包含 session 两个 u32、sender/destination、sequence/ackSequence、first/count/latest/ack、senderFrame、frameAdvantage、reserved、payloadBytes 和 fnv1a32。FNV 是诊断指纹，不是逐字节一致性的加密证明。
- event phase 0/1/2/3 分别为 intent / ws-send / message / poll-copy。每个键保留各 phase 的全部 occurrence；本次两份报告没有同键多 occurrence，但重发的历史范围依然存在，不能按 input frame 去重。
- 跨页时间用 `clock.started.timeOrigin + event.enteredAt` 对齐。下文包链统一减去本次 P1 的 `timeOrigin + measure.started`。P 的 P2 起点比 P1 晚 48.665 ms，M 晚 11.855 ms。各页开始/结束/快照的 Date 与单调时钟对照没有显示大的漂移，跨进程对齐仍有不确定性；表内保留到 0.001 ms 不代表达到该绝对精度。
- 收包→dequeue 表中的延迟默认是 message handler entry 到正数 poll-copy entry。同步 handler 执行也属于这个观测段。只有返回正数的 poll-copy 才递增 dequeue serial。
- P 的 pacing 行依次为 at、source、sampleIndex、dequeueSerial、version、outstandingFrames、networkIntervalScaleMilli、effectiveIntervalScaleMilli、predictionFrontier、actualContiguousExclusive。将 copy 对应到其后首个时间不早于 copy exit、serial 不小于该次 copy 的安全行；该行可能已经处理一批包，不是这个包的精确接受时间。M 的 pacingAvailable=false、两端 pacing 数组均为空。
- 对新提交间隔，原始 samples 的 `[11]` 是 presentation count，`[13]` 是浏览器传入的 rAF timestamp，`[0]` 是旧采样点的相对 now，`[2]` 是原 C++ 回调计时字段，`[3]` 是逻辑 frame。通过 rafDiagnostics.frames 的 sampleBefore/sampleAfter 找到 onGameFrame 记录，再以 rafIndex 关联 request 的真实 enteredAt/exitedAt。rAF timestamp 不能替代真实 callback entry。
- 按缺口查包时，仅选 count>0 且 first≤frame<first+count 的事件，并分别找最早 intent/send/message/copy，保留其 sequence、ACK 和全部重复历史。P 可以同时观察 raw actual frontier；M 只能报告 transport header 覆盖，不能将 normalized confirmed 当 raw actual。

### 正式区间与 startup 必须同时保留

以下正式区间仍使用原 summary 的约定，不改变之前的 14-field samples 或指标定义。onGameFrame entry 差只是同一对提交的补充定位，不是显示器扫描输出。

| 构建 / 端点 | 正式最大新提交 rAF 间隔 | 对应 sample / frame | 对应真实 onGameFrame entry 差 |
| --- | ---: | --- | ---: |
| P / P1 | 175.605 ms | 8318→8345；1075→1076 | 170.470 ms |
| P / P2 | 224.270 ms | 564→567；478→479 | 240.000 ms |
| M / P1 | 127.390 ms | 7517→7537；1609→1610 | 127.115 ms |
| M / P2 | 145.575 ms | 824→844；713→714 | 151.960 ms |

两次正式区间均无 >250 ms 新提交间隔，但全 raw 并非没有较长间隔：

| 构建 / 端点 | startup sample / frame | rAF timestamp 差 | 旧 now 差 | 真实 onGameFrame entry 差 |
| --- | --- | ---: | ---: | ---: |
| P / P1 | 12→103；8→9 | 569.425 ms | 590.270 ms | 590.140 ms |
| P / P2 | 0→1；0→1 | 145.415 ms | 502.125 ms | 515.460 ms |
| P / P2 | 1→2；1→2 | 496.930 ms | 180.360 ms | 181.205 ms |
| M / P1 | 10→60；8→9 | 315.025 ms | 334.600 ms | 334.615 ms |
| M / P2 | 0→1；0→1 | 102.975 ms | 363.965 ms | 377.150 ms |
| M / P2 | 1→2；1→2 | 345.350 ms | 138.055 ms | 137.600 ms |

P2 的 sample 0 是 active 初始化观察点，presentation count=0，因此其到首次提交的间隔是启动响应，不能伪装成两个已显示画面的物理间隔。P 的 sample 1 原 C++ 计时为 433.130 ms；M 为 311.310 ms，M sample 2 另有 120.580 ms。长跨度落在该回调计时区间，但本 trace 没有将其分解到某个 native 子系统。P1 的启动等待期间则持续有回调：P 有 90 个中间 onGameFrame、原 C++ 计时总和 18.505 ms；M 有 49 个、12.500 ms。

上述 P2 相邻两行体现了旧 rAF timestamp 的归属问题：大的 rAF 间隔可以落到下一次提交，不能据此说下一帧本身耗时数百毫秒。原始异常与所有端点均保留，不以 warm-up、host 负载或时钟差为名删除。

### Startup 首包有明确的发送前延迟

以下均为 sequence=1、first=0、count=1、ackSequence=0、ackFrame=INVALID_FRAME 的首个输入包，时间使用各次运行统一的 P1-start 轴：

| 运行 / 方向 | intent | 实际 WS.send entry | 对端 message entry | 正数 poll-copy entry |
| --- | ---: | ---: | ---: | ---: |
| P / P2→P1 | 233.625 ms | 768.685 ms | 771.600 ms | 772.350 ms |
| P / P1→P2 | 23.935 ms | 94.225 ms | 735.080 ms | 761.715 ms |
| M / P2→P1 | 109.055 ms | 524.880 ms | 535.190 ms | 538.295 ms |
| M / P1→P2 | 25.185 ms | 85.575 ms | 384.710 ms | 392.310 ms |

P2→P1 的 intent→actual-send 分别为 535.060 ms 和 415.825 ms。它们发生在客户端实际调用 WS.send 之前，包含既定 impairment 与其后的调度等待；不是 relay 或对端 dequeue 的证据。P 的 P1 首个安全行在 781.385 ms 观察到 actual 从 0 到 1，prediction=8、outstanding=7、network/effective scale=2000；M 无相应 export，不能填入一个推算的 actual 时间。

反方向 P1→P2 的 WS→message 分别有 640.855 ms 和 299.135 ms 的未分解段。其时间与 P2 的长回调/派发跨度重叠，但没有 relay recv/send 和浏览器网络进程时间，不能将全部间隔归因给 relay、网络或某一段 C++。全程所有 >250 ms 的 WS→P2 message 关联都位于 startup：P 为最初 seq 1..20，最大 675.425 ms；M 为 seq 1..12，最大 412.085 ms。它们仍在原报告中逐包保留。


### 发送端注入的解释边界与下一项对照（尚未执行）

当前 39±5 ms 的基础 delay 和 burst 都由发送页面的 impairment wrapper 通过 `setTimeout` 实施；`smoothness-browser.mjs` 中计划 delay 后的 timer 才调用其保存的底层 send。这把模拟发送等待放到了运行游戏 C++ / WASM 的同一条浏览器主线程上。

因此，P / M 首包的 intent→WS 535.060 / 415.825 ms 包含注入 timer 被同一主线程上 433.130 / 311.310 ms 的首次长回调延迟触发的影响。它是本测试中实际发生的现象，说明这项注入条件会把客户端回调阻塞传递到实际发送时刻；**不能据此外推为没有发送端注入的生产路径也必须等当前任务结束后才调用 WS.send。** 这项解释不撤销首次 C++ 长回调本身、其间输入/呈现延迟或任何 startup 异常，也不把全部发送前时间都归因给单个回调。

下一项可区分这个影响的条件应为“客户端直接同步 send + 独立 relay 进程注入 delay/jitter/burst + relay 逐包 trace”。它是后续计划，本次没有实施、没有启动新测试，也没有修改冻结 harness/probe 或旧 raw。实现时必须提供真正绕过客户端 timer 的显式发送路径；仅把 APP DELAY 设为 0 不够，当前 wrapper 仍会执行 `setTimeout(..., 0)`，依旧属于异步派发。

已只读核实任务内 fixture 的现有入口：

- `th09_web/artifacts/rollback4x-responsive-relay-fixture/server/netplay-relay.mjs` 的 delay/jitter 配置读取 `EAGLER_NETPLAY_RELAY_DELAY_MS` 和 `EAGLER_NETPLAY_RELAY_JITTER_MS`；gameplay 转发处计算 spread / wait，再调用 `sendBounded(target, forwardedPayload, { binary: true, delayMs: wait })`。
- 同目录 `relay-flow-control.mjs` 的 bounded sender 对 delayMs>0 在 relay 进程中调度 timer；timer 到期递归进入无 delay 的 send 分支，后者才调用实际 `socket.send`。因此应分别记录 relay receive、计划 due、实际 send 和事件循环延迟，不能把调度 sendBounded 的时刻当成已发送。
- relay 当前已有固定 delay/jitter 配置；等价 burst 的调度及上述逐包观测仍需在独立合成 fixture / opt-in 诊断中明确实现并记录身份，不应暗中改动 canonical 源码。relay jitter 的逐包算法也不同于当前浏览器注入，不能假定相同名义参数产生相同包序列。

该新条件仍需保留客户端 intent / actual-send、对端 message / native copy 和可用的安全 actual frontier，并注明注入位置、两个时钟的对应关系、完整配置与 source SHA。独立 relay 进程可以把人工 delay 的 timer 从游戏主线程移开，但仍共享宿主 CPU，不能预先假定不存在外部竞争。新条件用于区分发送端任务派发、relay 注入以及其余运输等待；不回写旧运行的测量定义或因果结论。


### 正式最大间隔的具体包链

P/P1 最大间隔内，raw actual 的主要停留点为 1067；prediction 约 1074–1075，network scale=2000。最早观察到覆盖输入 1067 的远端包为 P2 seq=2156、ackSeq=2563、first=1063、count=5、latest=1067、ack=1070、senderFrame=1067、FNV=1852984538。统一时间链为：

`intent 51842.535 → WS 52063.950 → message 52068.120 → copy 52072.010 ms`。

intent→send 为 221.415 ms，send→message 4.170 ms，message→copy 3.890 ms。52074.245 ms 的安全行已到 actual=1068、prediction=1076、outstanding=8；serial=2157 表明这是批处理后的观察。不能将 copy 到该行的时间解释成该包精确的 native 接受耗时。

P/P2 最大间隔处于 burst 尾部。最早观察到覆盖输入 474 的 P1 包为 seq=1060、ackSeq=907、first=469、count=6、latest=474、ack=465、senderFrame=474、FNV=39460458：

`intent 20308.425 → WS 20473.530 → message 20615.645 → copy 20631.230 ms`。

发送前 165.105 ms 含本次额外 delay；WS→message 为 142.115 ms，仍是未分解段；message→copy 为 15.585 ms。20657.940 ms 的安全行 actual=475，20730.985 ms 已为 actual=480、prediction=478、outstanding=0。该间隔中间的两次 onGameFrame 原 C++ 计时合计 38.785 ms，没有出现同长度的单个 C++ 阻塞。

M 没有 raw pacing。其 P1 最大间隔期间仍有 19 次中间回调（原 C++ 计时合计 3.210 ms），P2 最大间隔也有 19 次（12.975 ms）。仅从 transport header 可以核实：

- M/P1：首个覆盖输入 1601 的入站包 seq=2916、ackSeq=3033、first=1598、count=4、ack=1605、FNV=1350009078；统一时间 intent=46714.595、WS=46879.610、message=46882.645、copy=46882.930 ms。
- M/P2：首个覆盖输入 705 的入站包 seq=1322、ackSeq=1257、first=699、count=7、ack=698、FNV=402490272；统一时间 intent=20229.680、WS=20406.255、message=20470.040、copy=20470.310 ms。

这些是包字节的覆盖时间，不能冒充 M 的 native 实际接受、精确缺口或 pacing scale。两份 trace 都没有提供 native 验证/拒绝原因。

### Burst 与 pacing 的实际表现

按 intent 的既定 impairment 计划区分 burst 包，不按其最终送达时刻重新分类：

| 运行 / 发送端 | 受额外 delay 影响的包数 | intent→WS 最大 | WS→message 最大 |
| --- | ---: | ---: | ---: |
| P / P1 | 23 | 176.185 ms | 159.275 ms |
| P / P2 | 23 | 292.925 ms | 8.850 ms |
| M / P1 | 31 | 178.050 ms | 75.305 ms |
| M / P2 | 32 | 204.505 ms | 70.435 ms |

同样 500 ms 的窗口会覆盖不同数量的包；此处不是同一 frame tape。P 的 P2 旧 deliveredDelay 有 9 个 >250 ms：1 个 startup，8 个 burst。按新增 intent→actual-send 的边界则是 startup 1 个、burst 7 个；其中一包为 249.470 ms，而旧 timer-through-send 边界为 252.015 ms。两个定义都保留，不能挑选更好看的一个代替另一个。8 个旧 burst 长延迟包各自在等待实际 send 时，两端仍有数次新提交，不能将这些包延迟逐个等同为游戏冻结。

P 的 pacing 结论按各页自己的 `measure.started` 年龄读取：

| 端点 / 年龄 | actual / prediction 与 outstanding | network interval scale |
| --- | --- | --- |
| P1，19–20 s，burst 前 | outstanding=6–8 | 1827–2000，多数已到 2000 |
| P1，20–20.5 s | outstanding=7–8 | 2000 |
| P1，20.635 s | actual=474，prediction=480，outstanding=6 | 1887 |
| P1，20.709 s | actual=478，prediction=483，outstanding=5 | 1633 |
| P1，20.765 s | actual=479，prediction=485，outstanding=6 | 1333 |
| P1，20.800 s | actual=479，prediction=486，outstanding=7 | 又到 2000 |
| P2，19–20 s，burst 前 | outstanding=0 | 1000 |
| P2，20.526 s | actual=474，prediction=478，outstanding=4 | 1000 |
| P2，20.609 s | actual=475，prediction=478，outstanding=3 | 1000 |
| P2，20.682 s | actual=480，prediction=478，outstanding=0 | 1000 |

scale 以 milli 保存，2000 表示目标间隔倍率 2.000，不是实际帧率。P1 在 burst 前就已经受实际远端输入缺口制动，收到后续输入时 scale 短暂回落，随后再次达到 2.000；不能写成“burst 后恢复正常速率”。P2 此窗口的 network scale 一直为 1.000，effective scale 为 0.983/0.984，属于既有 channel phase 因子；不应将它解释成网络降速。两端 raw actual 在整个记录中均无回退。

这说明本次可以观察到真实远端输入缺口与节奏变化，同时也说明“有网络缺口”不能进一步推出“设备成本没有影响”。远端产出输入的速度、发送前任务调度与浏览器派发仍可能受设备/宿主运行状态影响。HOST_LOAD 仅提供伴随观测，不能自动排除或删掉某次游戏结果。

### 完整性、响应覆盖和仍未覆盖的边界

| 运行 / 端点 | intent / WS.send | inbound message / 正数 copy | pacing 行 |
| --- | --- | --- | ---: |
| P / P1 | 4353 / 4350 | 3640 / 3640 | 2519 |
| P / P2 | 3645 / 3642 | 4348 / 4348 | 1746 |
| M / P1 | 3382 / 3376 | 3254 / 3254 | 0，unavailable |
| M / P2 | 3264 / 3258 | 3374 / 3374 | 0，unavailable |

完整键关联中未见中途无配对事件。停止边界上 P 每向都有 3 个 intent 尚未观察到 send、2 个 send 尚未观察到对端 message；M 每向各有 6 个 intent 未见 send，P1→P2 有 2 个、P2→P1 有 4 个 send 未见 message。它们位于结束观察的尾部，原样保留为 unresolved，不能当作已知丢包或删除。不能因为没有 trace overflow 就宣称停止后也没有待发包。

全程 message entry→copy 最大值如下，均未达到 100 ms：

| 运行 | 在 P1 的接收队列段 | 在 P2 的接收队列段 |
| --- | ---: | ---: |
| P | 23.215 ms | 51.185 ms |
| M | 18.525 ms | 40.580 ms |

因此这两次没有复现“JS message 已进入后，再在 native dequeue 队列卡住数秒”的情况。它不排除旧运行发生过这种情况，也不证明 WS 到 message 之间没有浏览器或 relay 调度停顿。

responsiveness 的全部 intent 行已独立重算，并保留未捕获/未提交输入：

| 运行 / 端点 | planned / submitted / captured / presented | 明确未完成的 intent |
| --- | --- | --- |
| P / P1 | 356 / 356 / 344 / 344 | 12 个已提交未捕获，均在 capture 前被后续 intent 替代 |
| P / P2 | 356 / 326 / 326 / 326 | 30 个未提交，分布于启动、burst 与运行中；另 1 个 same-mask 保留在覆盖计数 |
| M / P1 | 209 / 209 / 207 / 207 | 2 个已提交未捕获 |
| M / P2 | 209 / 201 / 201 / 201 | 8 个未提交 |

两次均无已捕获未提交到预测状态的新逻辑帧 intent，输入 probe 的 gap/mismatch/changedHistory/error 为 0。条件延迟统计只对确实到达对应阶段的 intent 计算，分母必须与未完成计数一并呈现，不能拿条件 p99 表示所有预定输入都获得了同等响应。P2 的最大 schedule→presentation 是 P 499.070 ms、M 358.200 ms，均来自 startup intent 0；P1 分别为 193.960 ms 和 243.595 ms。这里的 presentation 是输入参与后预测状态的新逻辑帧提交观察，仍不是物理 scanout 或经过视觉识别的响应。

本轮达成的是有效的浏览器逐包定位与更清晰的异常边界。它没有证明秒级停顿已修复，没有确定旧 smoke 的 ACK/重发断流原因，也没有将未观察到的 relay 内部时间补齐。下一次若复现“WS.send 已早发生、peer message 却很晚”，前述 relay recv/actual-send/事件循环诊断才是区分中段的最小补充；若 message 与 copy 都早，而安全 actual 仍晚，则需要 native 接受/拒绝诊断。二者应针对已出现的边界选择，不增加无关观测后再以其结果过滤游戏异常。


