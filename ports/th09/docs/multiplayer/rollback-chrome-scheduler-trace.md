# Chrome 调度 trace 实现约定

此助手只定位已请求的 active rAF 长时间未进入时，Chrome 的 renderer、compositor/viz、GPU 相关线程发生了什么。它不修改游戏调度、输入、音频、渲染或网络。真实采集由主任务串行安排；纯 mock 通过不代表实际 Chrome 的事件覆盖已经验证。

## 接点与 API

新文件为 `th09_web/tests/multiplayer/chrome-scheduler-trace.mjs`，默认关闭，主 harness 用 `CHROME_SCHED_TRACE=1` 显式启用。每个由本次 harness 创建的独立 Chrome 各有一个 controller：

```js
const controller = await startChromeSchedulerTrace({
  enabled: true,
  browser,
  page,
  ownedBrowsers: browsers,
  side,
  sourceFiles: {harness: fileURLToPath(import.meta.url)},
});
const initial = controller.snapshot();
assert(initial.availability.available && initial.status === 'recording');
// 原有首次 active 回调中的单次分支：
window.chromeSchedulerTraceProbe?.mark('first-active');
// 已取得原游戏结果，或已记录原失败后：
const sanitized = await controller.stop('capture-stop'); // 失败用 'failure'
```

两端 `openProbe` 和 CPU 配置全部完成后、`netplay-setup-start` 前同时启动。助手核实 browser 引用在 `ownedBrowsers` 中、page 属于该 browser、类型为 chromium，且 CDP product 为 Chrome/HeadlessChrome。它只使用 owned browser 的 CDP；不会连接其他已有浏览器。

这一时间窗是对早期 `rollback-active-raf-trace-plan.md` 冷启动方案的收窄：本轮不采 page.goto/openProbe，也不声称覆盖这些启动成本。RTC constructor 由已有独立 startup probe 解释。

启动成功后助手安装 `window.chromeSchedulerTraceProbe` 并立即打 `th09-sched-armed`。原 active 分支只打一次 `th09-sched-first-active`。正常或失败停止先打 `th09-sched-capture-stop`，然后结束 trace。每个标记只保留紧邻 `performance.mark` 的 before/after、timeOrigin；不会增加 rAF、heartbeat 或逐帧标记。

## 范围、停止与数据

启动前必须通过 `Tracing.getCategories` 确认全部六类：`toplevel`、`renderer.scheduler`、`cc`、`viz`、`gpu`、`blink.user_timing`。缺任何一类即 `availability.reason='required-categories-missing'`，不会自行扩大类别。backend 固定 `chrome`；关闭 sampling 和 systrace，不请求 V8、系统、网络、截图或 disabled-by-default 类别。

`Tracing.start` 使用 `ReportEvents`、`recordUntilFull`、32 MiB Chrome buffer 和 argument filter。`dataCollected` 在内存逐条清洗，原始 trace 从不落盘或进入错误消息。主 harness 可以把完整净化 snapshot 写为独立 `<label>-P1/P2-scheduler-trace.json`；主 report 保存其路径、bytes、SHA 与去掉 events 的 metadata，避免重复大数组。文件使用唯一 label，不能覆盖旧 raw。

每端最多 350,000 个事件。本地 retained-data 预算为 96 MiB，按紧凑 JSON 字节、每条记录额外 128 字节计数增长余量，以及 64 KiB metadata 预留计算。这不是 JavaScript 堆大小或 pretty-print 文件大小；写独立 artifact 应使用紧凑 JSON。Chrome 原生 buffer 仍为 32 MiB。附加界限为 128 条 bufferUsage、8 个标记、32 个错误、2,048 个未知名字哈希、1,024 个线程。

90 秒截止只结束 trace，不停止游戏。即使正常 stop 正等待一个忙碌页面的尾标记，截止仍先调用 `Tracing.end`。普通 CDP 命令和页面等待仍各为最多 5 秒，`tracingComplete` 的有界等待为 30 秒。重复和并发 stop 共用同一结束操作与最终 snapshot；协议、页面、detach 失败保存固定错误代码，不保存动态 exception message/stack。Chrome buffer 满、dataLossOccurred、任一本地溢出、缺 tracingComplete、source 改变都使完整性失败。

`Tracing.start` 本身超时有单独边界：5 秒限时不能取消已经发出的底层 CDP 命令，它可能晚于超时才原生成功。助手此时返回 unavailable 并 detach，但不会对未确认启动的 trace 盲目发送 `Tracing.end`；detach 本身也不能证明晚成功的 trace 已停止。当前 fresh-owned harness 必须在启动 gate 失败后关闭本轮两台 owned browsers，由 owner-close 限制这条不确定路径。90 秒截止是在确认 start 成功后建立的，不能作为未确认 start 分支的独立停止保证。

正常结果或失败先保留原 gameplay 证据，随后再评估 trace 完整性；不能因 trace 不完整静默剔除那次游戏失败。

## 清洗与时钟

事件只保留数值时间、duration、pid/tid、允许的 phase、固定类别及受限数值/十六进制 flow ID。已审查的静态调度/GPU 名称可读；未知名称只留 SHA-256 和计数。动态 args、URL/query、页面标题、用户输入、网络 payload、截图和栈全部丢弃。Chrome process/thread 元数据只允许固定名字白名单，未知名字也只留哈希。

`clock` 用固定页面标记关联 trace 的 pid/tid。trace ts 按微秒解码，页面时钟为毫秒，计算 `traceTs/1000 = performance.now + offset` 的区间交集。保留原 before/after 括号，并明确增加每侧 0.1 ms 的量化余量。至少两个不同时间、同一 pid/tid 与 timeOrigin 的唯一标记且区间一致，才 `clockMapped=true`。不会把 performance.timeOrigin 直接当作 trace 原点。

`collectionValid` 与 `coverage` 分开：前者说明是否按范围完整取得清洗数据；后者记录实际六类事件计数、线程、未知名称、时钟映射及三标记。`activeWindowCovered` 还要求三标记完整有序且没有截止提前停止。没有事件不能解释为线程空闲，长 wall span 不能解释为 CPU 自耗时或 GPU 硬件运行时间。应与既有 rafDiagnostics 的 request/entered/exited 和控制调用核对；未复现不等于已修复。

## 本地验证

```text
node --check th09_web/tests/multiplayer/chrome-scheduler-trace.mjs
node --test th09_web/tests/multiplayer/chrome-scheduler-trace.test.mjs
```

15 个纯 mock contracts 已通过，覆盖默认关闭、owned/Chrome/category 校验、严格 start 配置、标记与时钟、隐私清洗、所有存储边界、data loss/full、并发停止、source identity、协议/页面/detach 错误、截止抢先停止和缺失 tracingComplete。新增两个合同：5 秒后、30 秒内完成的 flush 必须保留晚到事件；未确认 start 超时后的 late-success 只能由 owner-close 收尾。mock 不打开浏览器、不编译、不跑性能命令。

## scheduler-01 失败与限定修复

首轮 `rollback4x-chain-release-input-batch-headed-relay39-scheduler-01` 完成了原 1,800 帧游戏检查，但两端 trace 都在 5 秒 completion 等待后超时，`dataLossOccurred` 未知。原 helper SHA 为 `6e655330bf70f1434ce3044c544871462095728310cef7cd4f6bfef7c12b7637`；原样源码以 COPYFILE_EXCL 保存在同前缀 `-helper-source.mjs`，原始 report/trace 未修改。

| 已收到的 trace 数据 | P1 | P2 |
|---|---:|---:|
| raw / retained events | 3,696 / 3,682 | 101,520 / 101,500 |
| dataCollected 批次 | 28 | 756 |
| artifact bytes | 619,547 | 16,335,947 |
| 本地预算已用，含预留 | 1,171,191 B | 29,323,348 B |
| 所有正时间事件范围，含末事件 duration | 578.426 ms | 4,817.358 ms |
| trace 中实际出现的标记 | armed | armed、first-active |

两端 `bufferUsage.percentFull` 在开始约 23 秒后都停在 `0.9962451457977295`，直到结束。helper 的 full 标记当时只检测 `>=1`，所以 `chromeBufferFull=false` 不能证明原生容量充足；这一共同平台是强烈的容量线索。完整采集窗约 36.7 秒，capture-stop 还比最后游戏样本晚约 5.51 秒。无本地 overflow 仅表示**已经收到的前缀**未超限，不能推断剩余数据能装下，也不能把缺失尾标记解释为页面没有执行标记。

随后 short-02 限定 `FRAMES=360`，覆盖此前约 frontier 282/283 出现过的 active 缺口。当时只把本地净数据预算改为 64 MiB、completion 等待改为 30 秒；Chrome 32 MiB、recordUntilFull、250,000 事件、90 秒安全截止、六类别与完整性判定均保留。这是一次有界采集修复，不能称为完整 1,800 帧流畅性验收。当时按首轮前缀平均和进入 active 后的事件率估算，360 帧的约 7 秒窗需要约 40–50 MiB padded 本地预算；停止前的原结果收集和实际缺口会延长窗口，所以 64 MiB 仍是可能触发的硬上限，不是完成保证。以下保留该估计被实测超过的结果。

## scheduler-short-02 失败与最后一次预算校准

`rollback4x-chain-release-input-batch-headed-relay39-scheduler-short-02` 使用上述 64 MiB / 250,000 事件 / 30 秒 completion 配置。原 helper SHA 为 `48c7a78918f4c2dcf60e5bfe865dba74bde4be0b6e2510d56d54de447957e9a5`，源码已保存在提交 `fd64e7`；原 failure/trace 文件不变。

| 实测采集 | P1 | P2 |
|---|---:|---:|
| 开始至 stop 请求 | 8,566.914 ms | 8,564.863 ms |
| end 请求至 tracingComplete | 20,064.252 ms | 20,272.599 ms |
| raw / retained events | 247,538 / 234,451 | 249,156 / 234,205 |
| dropped events | 13,087 | 14,951 |
| 本地预算已用，含预留 | 67,108,687 B | 67,108,773 B |
| 净化 artifact bytes | 37,234,753 | 37,263,025 |
| Chrome bufferUsage 最大值 | 0.2994185984 | 0.3019621968 |
| tracingComplete / dataLossOccurred | true / false | true / false |
| 三标记齐全 / clockMapped | true / true | true / true |
| collectionValid / activeWindowCovered | false / false | false / false |

两端最先触发的是本地 padded 字节预算；触发后丢弃的后续到达行也累计到 `overflow.events`，不能据此声称先撞到 250,000 事件上限。CDP 到达顺序不保证事件时间顺序，本次存在时间戳倒序，所以不能把丢失范围限定为时间轴末尾。Chrome 完成通知证明 30 秒等待在这次采集内足够，`dataLossOccurred=false` 也不能抵消本地明确记录的丢弃。按这次已收到数据的平均预算估算，完整数据需要约 71 MB（十进制）；实际单次大小仍取决于采集窗与事件组成。

原游戏结果取得后，既有 `Actual audio output not exercised` 断言也失败：两端各 71 个输出抽样块、其中 69 个非零块，未达到原本总块数与非零块数都超过 100 的要求。不能把本次记作完整通过，也不能据此推断音频没有运行。最后一次限定采集仍为 `FRAMES=360`，使用已有 `AUDIO_OUTPUT=0` 仅关闭输出抽样；音频引擎、播放和音频时钟继续运行，原音频输出 gate 源码不改。

经此次实测后，最后一次调整仅为本地预算 **64 → 96 MiB**、事件数 **250,000 → 350,000**，为约 71 MB 数据和接近 250,000 的事件量留出有限余量。Chrome 仍为 32 MiB / recordUntilFull，completion 30 秒、普通 CDP/page 5 秒、截止 90 秒、六类别、collector 和完整性门槛均不改。这仍只定位早期 active 缺口，不是完整 1,800 帧流畅性验收；任何后续不完整采集继续保留为失败。

原始身份：

| 同前缀文件 | bytes | SHA-256 |
|---|---:|---|
| -failure.json | 12,167,242 | ff430e9d733fe5977619a37fbb9f5ae7837c0a1f82dd12f7477a523323f41ea8 |
| -P1-scheduler-trace.json | 37,234,753 | 647b1cf633692cdf068a959187e5cf70dd92e99c959e58fe99d48da8e76f2bfe |
| -P2-scheduler-trace.json | 37,263,025 | 7ed927ea6b9f5db5ff461c0e192e0085ce501e5184a620c71b466bdcdb38175f |

## scheduler-short-03 完成采集

`rollback4x-chain-release-input-batch-headed-relay39-scheduler-short-03` 使用冻结 helper `e15457ff9a36e15a49cbdfb7fdb762bbc4da819087991234c95040517a03db58`，原运行 HEAD 仍为 `fd64e7`。本轮为 360 帧、CPU [1,4]、relay 39±5 ms，使用最后一次 96 MiB / 350,000 事件预算。游戏报告 `passed=true`，两端 trace 的 `collectionValid`、三标记、`clockMapped` 和 `activeWindowCovered` 全部为 true，所有 overflow、malformed 与 errors 均为零。

| 实测采集 | P1 | P2 |
|---|---:|---:|
| 开始至 stop 请求 | 8,666.223 ms | 8,664.183 ms |
| end 请求至 tracingComplete | 23,313.702 ms | 22,909.434 ms |
| raw / retained events | 247,427 / 247,400 | 249,621 / 249,594 |
| 本地预算已用，含 64 KiB 预留 | 70,793,071 B | 71,548,412 B |
| 净化 artifact bytes | 39,274,718 | 39,748,249 |
| tracingComplete / dataLossOccurred | true / false | true / false |

每端 27 条 `droppedEvents` 属于 collector 明确的范围外过滤；本轮没有本地容量丢弃或 malformed 事件。此前两次采集失败仍保留。完整性与时钟检查通过只证明这个限定采集窗可用于调度分析；本次没有重现此前约 1.4 秒的 active 缺口，不能据此声称已经修复，也不是完整 1,800 帧流畅性验收。

本轮明确 `AUDIO_OUTPUT=0`：没有安装输出抽样 observer，也没有声称满足其 `>100` 个总块和非零块门槛。音频引擎继续运行，两端 audio state 均为 running，时钟分别推进 6.200 / 6.189 秒。原门槛与游戏音频代码均未修改；后续正式 1,800 帧性能/响应样本单独使用 `AUDIO_OUTPUT=1`。

| 同前缀文件 | bytes | SHA-256 |
|---|---:|---|
| -report.json | 5,061,969 | cc99293e048bc5c6943f19b41f5f1e9200bc033c131bb3929f4350918a063393 |
| -P1-scheduler-trace.json | 39,274,718 | 67d9341d2e4671cdba4239fe2b10341b9f1eb0c4e9cddeea53b7a05551566ad4 |
| -P2-scheduler-trace.json | 39,748,249 | 092e6bdda2f11734000ff9751398f0e2a08f281b963331b7cec4ec1053d6aaa0 |

三次采集的合并证据为 `docs/multiplayer/rollback-chain-chrome-scheduler-evidence.json`；其 short-03 分析仍区分 pre-active、active 和结果收集后的线程任务，不把窗口外长任务归为对局停顿。
