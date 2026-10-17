# TH09 无游戏传输对照：五个已完成条件

本稿供 rollback-chain-work.md 整合。它只解释独立传输对照，不能替代真实游戏的输入响应、持续新画面、完整音频或 4× CPU 性能验收。

工作区：D:/workspace/eagler/worktrees/th09-rollback-4x。

## 结论与失败保留

五次运行均完成每向 1,440 个确定性包的完整交付；API、实际发送、浏览器 message 入口和 JS poll-copy 均有完整记录，收到的所有字节均经过冻结浏览器夹具逐字节检查。独立分析器再次校验序列、包头、预期指纹、目的端、调用结果和跨阶段关联。

所有原始报告的 passed 都是 false，现有证据没有被改成“全通过”。五次均发现宿主 Kaspersky 注入资源请求；这不是隔离的空白页实验。RTC-block 另有一条启动期选中候选状态没有满足冻结测试的严格条件。摘要把数据交付、严格路径证据、页面隔离和游戏性能验收分开，保留每份原始结果及失败摘要。注入请求的完整 URL 和 query 仅存在于原始报告，没有复制到本稿或可提交分析 JSON；没有关闭或绕过安全服务。

**本机 RTC 在没有 WASM、游戏逻辑和回滚时，P2 仍出现百毫秒量级的回调派发及输入消费延迟。健康判据不是 4× 游戏性能验收。** 这些结果支持继续区分传输、浏览器调度与应用执行成本；它们没有证明两秒停顿已经解决，也不能仅凭路由切换解释真实游戏的吞吐变化。

## 1. 固定方法和实际覆盖

固定 common 工作树为 D:/workspace/eagler/worktrees/th09-rollback-transport-common，HEAD 为 43add4dc9085c65468f7ab15428c00206cbe3834。夹具执行这个版本 BrowserPeerTransport.cpp 中六个实际 EM_JS 函数的原始 body，以 JS Uint8Array 模拟其 ABI 内存，不创建 WASM 实例，不装载游戏、音频或绘制代码。

函数包括 connect、close、is_open、send、send_to 和 poll_copy。poll_copy 保留 common43 的队列实现，包含已有的头索引和偶尔压缩。control 调用的是 eagler_peer_send(control=1)；**没有调用 eagler_peer_send_repair_to，也没有验证该专用 API 的 32 KiB 大小、背压条件或 C++ SessionChannel 的自动 repair 策略**。

两端各有独立的本机 Chromium 进程，版本都是 153.0.8010.12。P1 请求 CPU 1×，P2 请求 CPU 4×。页面初始可见并有焦点；这不是全程前台调度优先级的证明。两个页面都准备完成才同时连接，以免把第二个浏览器的启动等待混入 RTC 路由选择。

每端以固定 60 Hz 计划发送 720 组包，每组一个 input、一个 control 包；每包 56 B。所有条件使用同一计划和两个方向完全相同的预定字节序列。到达发送计划时直接调用共享发送函数，没有附加客户端网络延迟定时器；落后的计划保留并补发，不丢弃。发送完成后保留约 2 秒排空时间。poll 周期为 4 ms，heartbeat 周期为 10 ms，rAF 只记录实际回调入口，没有画面绘制。

实际 relay 源码来自固定夹具目录：
D:/workspace/eagler/worktrees/th09-rollback-4x/th09_web/artifacts/rollback4x-responsive-relay-fixture/server/netplay-relay.mjs。

它是真实共享 relay/signaling 实现，运行在独立子进程；使用中立的 transport-only room，没有覆盖产品 lobby admission。RTC 条件通过实际共享信令建立数据通道：

- input：unordered，maxRetransmits=0。
- control：ordered，可靠传输。
- 已观察到的候选都是本机 host↔host / UDP。没有使用 TURN 候选；没有把 relay fallback 计为 direct。
- 同机 host 候选不能证明真实远端、公网、移动端或复杂 NAT 可用。

关键源码身份：

| 对象 | SHA-256 |
| --- | --- |
| health browser harness | c52534f75eaa4c15ea828a47b64efa6d2aed4939d28024e65fb96f5e22ba5a20 |
| health page fixture | 90cb3d5d43eaccb4f0abc574dbf717037bc9009b71967c8aca9838fcc81493b6 |
| common43 BrowserPeerTransport.cpp | 64a66f9bbb31accf5e88b64893a6b62eef40b1060e0c0c0e303b10114197a42f |
| 固定 shared relay source | 56c44ab7520381859b05d21130da40ad605722b42e49ed37c089d0ecf8bbe90c |
| deterministic schedule | 4d8e530fbc8dc5fee4bcaa8206b84a28a748c8f488a9d001cb251e2ea74796dd |
| P1 packet tape | a51aaa7cc1a6d64e93e977528613c8f2494a7a8ae921543efa500c99080a87a5 |
| P2 packet tape | 867959132f52cdfd820dba5d2a8093402e6394f0402068a4e141193b46a21599 |

## 2. 分项结果

| 条件 | 数据交付一致性 | 严格路径证据 | 页面隔离 | 原始总结果 | 额外事实 |
| --- | --- | --- | --- | --- | --- |
| relay-none-01 | 通过 | 通过 | 失败 | false | 30 条注入请求 |
| rtc-none-01 | 通过 | 通过 | 失败 | false | 29 条注入请求，真实本机 RTC |
| relay-block-01 | 通过 | 通过 | 失败 | false | 26 条注入请求；P2 实际阻塞 400.335 ms |
| rtc-block-01 | 通过 | 失败 | 失败 | false | 28 条注入请求及 1 条启动路径状态失败；P2 阻塞 402.390 ms |
| relay-burst-01 | 通过 | 通过 | 失败 | false | 26 条注入请求；独立 relay 注入实际覆盖 116 包 |

每次每端的 1,440 个包均完成所有阶段，完整字节检查错误、未配对包和观察溢出均为 0；最后队列和 relay pending 都为 0。RTC 两个物理通道都交付完整；健康包的成功不能替代实际 repair 行为验证。

relay-burst 是 relay 应用层按包安排发送定时器，可能改变消息转发顺序。本次两个“计划 lane”出现乱序，摘要保留计数；这不是声称 TCP 在链路上丢失或重排数据，也不适用 RTC control ordered 的验证条件。

## 3. 无注入、无显式阻塞的两次观察

以下都是毫秒，单元格为 p99 / max，分布包含完整样本。

| 边界 | relay-none | rtc-none |
| --- | ---: | ---: |
| P1 send → P2 message 入口 | 58.630 / 77.805 | 102.340 / 144.135 |
| P2 send → P1 message 入口 | 35.380 / 38.750 | 4.820 / 17.120 |
| P2 计划发送时刻 → API 入口 | 27.707 / 39.975 | 61.355 / 104.013 |
| P2 message → poll-copy | 23.495 / 36.435 | 74.190 / 106.860 |
| P2 heartbeat 实际入口间隔 | 34.010 / 64.965 | 76.985 / 146.380 |
| P2 rAF 实际入口间隔 | 24.865 / 45.120 | 61.230 / 125.390 |

RTC 的 P2→P1 方向在这次窗口里较快，P1→P2 的长尾却更大；慢端本地的计划发送、消息消费和 heartbeat/rAF 同时较差。因此，不能把 send→message 的全部时间当成网络传播时间，也不能把两次运行的差值宣称为 RTC 因果回归或收益。

relay-none 可以进一步定位：P1→P2 的 relay receive→send p99/max 只有 0.258/1.999 ms，relay send→P2 message 则为 51.164/66.198 ms。该次最长可观察等待主要在 relay 已执行发送之后至慢页 message 入口之间。边界仍包含浏览器网络服务和主线程派发等未细分部分，不能将其归责给某一个宿主组件。

RTC 也包含连接维护、getStats 和额外通道状态观测工作，这些在没有 RTC peer 的 relay 页面里并不相同。本机候选、不同宿主时段、页面注入和诊断开销均限制跨次路由性能推论。

## 4. 400 ms 标记如何区分等待层次

relay-block 的 P2 busy-loop 实际持续 400.335 ms，开始比计划晚 21.805 ms。这个区间内：

- P1 有 46 次实际发送，两个计划 lane 均有覆盖。
- 这 46 包都在 P2 阻塞尚未结束时，被独立 relay 接收并实际执行转发。
- P2 的 message 回调数是 0，46 包的 message 入口全在阻塞结束之后。
- P2 的已记录队列峰值为 52，结束为 0。P1→P2 send→message 最大 445.540 ms，而该方向 relay receive→send 最大仅 1.100 ms。

这给出了一个具体的正控制：接收页面没有派发 message 时，独立 relay 可以持续完成收发。相应等待落在 relay send 之后，不是 relay 应用处理暂停。被阻塞页面自身的发送也要等它恢复，因此“对方没有新输入”可能来自对方的执行调度，而不只是中继链路。

rtc-block 的 P2 busy-loop 实际为 402.390 ms，开始比计划晚 64.375 ms。期间 P1 实际发送 48 包，P2 message 回调仍为 0；这些包均在标记结束后进入 P2 message。没有 relay E9NP 数据，不能进一步定位 RTC 的 OS/网络服务入站时刻。P2 队列峰值为 56，最后清空。发送计划恢复、消息回调恢复和 JS 消费恢复是不同边界，不能合称同一个“网络停顿”。

这些是刻意阻塞整个页面主线程的标记。真实游戏中如果只是 rAF 缺口而 pump、message 或其他回调继续执行，就不属于同一种全主线程阻塞形态；不能用本标记为那种停顿自动定因。

### rtc-block 的路径失败不能略去

唯一未满足严格路径条件的样本来自 P1 的 ready getStats，位于共同发送起点之前 1514.020 ms。它的 route 是 rtc，PC 与 ICE 状态为 connected，local 和 remote 候选字段完整且都是 host/UDP；选中 pair 的状态却是 in-progress，nominated=true，已记录 bytesSent=2261 和 bytesReceived=2401。

原错误字符串将其统称为“unknown or relay selected candidate”，但此次具体失败是 pair.state 没有达到 succeeded，既不是观察到 TURN，也不是候选字段缺失。P1 其余 17 个带 pair 的样本和 P2 全部 18 个样本均为 succeeded/host/host。摘要保留启动期原样本和严格失败，不能只取发送阶段就把原测试改绿。

## 5. 独立 relay burst

该条件没有人为阻塞页面。延迟发生在独立 relay 进程：正常计划延迟 39±5 ms，首个非空 E9NP input 抵达 relay 后第 4 秒开始，持续 500 ms 的窗口额外增加 120 ms。burst 时钟不是各浏览器的开始年龄。

实际覆盖 P1→P2 的 60 包和 P2→P1 的 56 包。burst 计划延迟均约 154–164 ms。实际 relay receive→send 的 burst p50/max 分别为 163.495/169.851 ms、161.783/172.947 ms，说明新增等待确实出现在独立 relay 的转发阶段，而不是客户端发送注入定时器。

完整 send→message 分布仍包含其他边界：P1→P2 p99/max 为 243.870/270.540 ms，P2→P1 为 168.445/177.530 ms。不能用这些端到端数字替代 relay 的单阶段时间，也不能把各阶段分布的最大值相加当成同一个包。

原始计划 dueAt 与观察到的发送时刻有小幅有符号偏差，包括约 −1.77 ms 的值。分析保留它们；计划定时器时刻不是硬实时的最早发送承诺。这与跨进程边界的微小负值也分开处理，均不解释为负的网络传播时间。

## 6. 宿主时序和测量边界

宿主统计使用共同计划起点至最后一个页面实际 stop 的窗口，包括约 2 秒排空；只取完整落在窗口内的 CPU 区间和按 source 时间完全落在窗口内的进程快照。不会把报告序列化完成时刻当成浏览器实际 stop，也不会把进程首次出现的 lifetime CPU 当作本次 CPU。

| 条件 | 有效窗口加权 aggregate CPU busy | 该窗口的编译候选证据 |
| --- | ---: | --- |
| relay-none | 41.54% | wasm-opt 同一 PID 两个连续差累计 +40.34375 CPU 秒；ninja 增量为 0 |
| rtc-none | 37.31% | 完整进程快照未观察到编译候选 |
| relay-block | 29.06% | 完整进程快照未观察到编译候选 |
| rtc-block | 30.00% | 完整进程快照未观察到编译候选 |
| relay-burst | 33.86% | 完整进程快照未观察到编译候选 |

host.valid/complete 为 true 表示采样完成。24 逻辑 CPU 的聚合负载不能证明单核可用性、频率、温控或进程优先级稳定；5 秒进程采样会漏掉短命任务。RTC-none 没有记录到编译进程，不应把其较大长尾笼统归因于编译。安全软件注入也没有被证明解释了所有差值。所有条件都保留，没有按宿主数据删样本或归一化所谓“校正 FPS”。

跨进程时刻使用各自 timeOrigin 映射；Date.now 校准只有整数毫秒精度。relay-none 有一条 send→relay 为 −0.000732 ms，relay-burst 也有微小负值；原值均保留。这种证据足以区分数百毫秒等待属于哪一段，但不是抓包或硬件同步时钟。

## 7. 来源和复现

独立分析器：
D:/workspace/eagler/worktrees/th09-rollback-4x/th09_web/tests/multiplayer/summarize-transport-health.mjs。

分析 JSON：
D:/workspace/eagler/worktrees/th09-rollback-4x/th09_web/artifacts/multiplayer-tests/transport-health-chain-evidence.json。

所有原始报告均位于 D:/workspace/eagler/worktrees/th09-rollback-4x/th09_web/artifacts/multiplayer-tests。以下文件名与 SHA 固定绑定，分析器缺任何显式请求的来源或发现已知原始 SHA 改变都会失败：

| 原始文件 | SHA-256 |
| --- | --- |
| rollback-transport-health-relay-none-01-report.json | 3dbe5667f3d420b948c5c2fef8ce2be6bc32cf3f55143f8a03e51401e94aecd9 |
| rollback-transport-health-rtc-none-01-report.json | ebc8d613e682d8ba5f6ce1ab219a60b61a7ec94b1e07c4e3af563f853926cf3a |
| rollback-transport-health-relay-block-01-report.json | e54509e7d57d9f3482141134240c943cc46a679102f52609aed64446c797bf2c |
| rollback-transport-health-rtc-block-01-report.json | 26535b389eaabbe4bcbd3a03ff344df8a8e8efe84c40cbecda3d10b8bccea90f |
| rollback-transport-health-relay-burst-01-report.json | b2fe3d7840b7c8629bd1fdcaf64054adf57b6f661a4eeed06afdc471e3b3e375 |

在工作区根目录执行：

~~~powershell
node --check th09_web/tests/multiplayer/summarize-transport-health.mjs
node th09_web/tests/multiplayer/summarize-transport-health.mjs --include-blocks --include-relay-burst
~~~

这两个命令只检查或读取已有文件，不启动浏览器、relay、构建或 benchmark。分析器独立重算并核对原存储分布，确认五次计划、包 tape、浏览器版本、CPU 请求和固定传输源码一致；保留输入/control 分项、候选状态、完整阶段分布、未匹配尾部、marker 逐包链和按实际窗口截取的宿主统计。

已完成最小五条件矩阵，不建议为了形成更好看的数字追加同类空载轮次。下一步应结合真实游戏已有的 callback/packet 链证据，针对仍未解释的边界工作；游戏目标仍是低输入延迟、连续新画面和保留完整音频，不能用本传输对照替代验收。

