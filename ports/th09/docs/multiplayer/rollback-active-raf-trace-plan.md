# Active rAF 调度与 GPU trace 方案

当前进展（2026-10-10）：下文保留最初设计及当时的失败依据。headed 发布控制已完成 A/B/C/A、relay39、burst 和实际 direct；relay39 仍复现约 1.4 秒的 active rAF 待派发。默认关闭的 `CHROME_SCHED_TRACE` 助手及 harness 接点已保存为本地提交 `fd64e7eeb831ad2a7fe0ec23815b9d58680aa7cd`，15 项 helper mock 和 3 项 finalizer contract 通过。首轮长窗回传超时、第二轮短窗本地容量及音频采样门槛失败均保留；第三轮短窗实际采集已完整通过，未复现 active 秒级停顿，因此尚未得到旧停顿的根因或修复证据。

本轮具体实现见 `rollback-chrome-scheduler-trace.md`，当前证据和结论见 `rollback-chain-work.md` 及 `rollback-chain-chrome-scheduler-evidence.json`。成功采集使用 B 输入合并标量发布版、relay 每方向 39±5 ms、360 帧 tape、CPU[1,4]；helper SHA `e15457ff9a36e15a49cbdfb7fdb762bbc4da819087991234c95040517a03db58`。Chrome 32 MiB / recordUntilFull、六类别、90 秒截止保持原值，本地保守预算 96 MiB / 350,000 事件、结束回传等待 30 秒。两端完整性、三标记时钟映射及 active 覆盖均通过，零 overflow。`AUDIO_OUTPUT=0` 只关闭输出块抽样；游戏音频与原音频运行时钟 gate 保留。开始仍在两页 openProbe 和 CPU 配置完成后；标记为 armed、first-active、capture-stop。短窗不能代替完整性能/音频验收，下文的无注入 900 帧、冷加载和未实施控制表述均属于历史候选方案。游戏调度和生产构建没有为追踪改变。

## 要解决的问题

release relay baseline 的本次 raw 已证明 P1 的请求 #233 已发出，却在最终快照仍未进入：pending 至少 15,615.730 ms。前一回调只占 10.630 ms，后续 pause(1) 发生在缺派发开始约 15.44 秒之后。本次音频输出回调仍继续。同期 host 明确记录了 wasm-opt CPU 增长，因此这份失败不能称为空载基线，也不能直接归因于 GPU。

已保存证据：rollback-chain-release-raf-timeout-evidence.json，SHA256 da7f6ae419aa96702eb7e6f25148703b2d22c9dab8e5f60d1866cc424c6453b2。它包含原始记录的 SHA、所有超 250 ms 异常、pending/parent、音频和 host 样本。

新 trace 只回答：rAF 已排队以后，Chrome 的 renderer main、compositor/viz、GPU 相关线程各发生了什么。短 C++ present/glcommit 不能排除提交后的异步等待；GPU 线程的长 wall span 也不等于 GPU 硬件持续运算。

RTC constructor 已由独立三点证明存在另一类同步长调用。不要把该结果与 active rAF 缺派发混为同一个原因。

## 成本更低的先行候选：headed 控制

可先考虑一个相同冻结 release、native GPU 参数、CPU、tape 和零注入条件，仅将 launchBrowser 的 headless 选项改为 false；不同时加入完整 tracing。当前 rAF 已排队而 timer/audio 继续的证据，使窗口/BeginFrame 路径成为值得直接比较的条件。此选项尚未实施或运行，root 可在它与完整 trace 中择一，避免同时扩散变量。

launch helper 以 ...options 覆盖默认 headless:true，支持传入 headless:false。只读检查的当前依赖环境没有 executable override，chromium.executablePath() 指向已存在的正常 chrome.exe。该检查没有打开 browser，也不代表历史 raw 已记录了 executable 身份。若实际环境走到 helper 的 headless-shell fallback，就不能把它当作可见窗口控制；应记录不可用，而非悄悄换另一套 browser binary。

Playwright 的 headless 分支会额外添加 --mute-audio、--hide-scrollbars 及固定 --blink-settings。仅切换 headless:false 会同时移除这些默认参数，因此最终启动条件并非只少一个 --headless。为更接近单因素，可在 headed 条件显式保留这三项辅助参数；否则必须披露音频输出和输入设备声明也发生了配置变化。

已读到的固定第三项是 --blink-settings=primaryHoverType=2,availableHoverTypes=2,primaryPointerType=4,availablePointerTypes=4；这是安装的 Playwright 默认值，方案未自行发明另一组设置。

两台 owned browser 需要同一可交互桌面；在采样前固定窗口状态并记录 bounds、windowState、viewport、visibility 和 focus，保留重叠/最小化条件。不要在采样中反复 bringToFront，不更改系统或安全软件。document.visibilityState=visible 不单独证明窗口被物理显示；现有 Present 统计仍是新状态提交，不是物理 scanout。

若 headed 下未复现，只能说明本轮条件没有出现该问题；不能认定 headless 就是唯一原因，更不能计为代码优化收益。若仍出现同类 pending，则继续检查该次完整 lifecycle/窗口状态，再决定是否需要下面的 Chrome trace。所有原 headless 失败和并发 host 观察继续保留。

## 若选择 tracing：建议的首次条件

在 root 完成当前构造/GPU 参数对照后，只运行一个带新诊断的 release 条件，优先使用已知 baseline 的冻结 WASM 身份；每次仍使用唯一 label。

- relay、同步 client send、无注入、frame tape 900、CPU[1,4]。
- RAF_DIAGNOSTICS=1、HOST_LOAD=1、AUDIO_OUTPUT=1 和既有 relay 观测。
- 不同时打开 V8 CPU profiler、LIVE_COST 或新的每包浏览器 trace。
- 新开关暂名 CHROME_SCHED_TRACE=1，默认关闭。
- 两个由本 harness 创建的独立 browser 分别采集；不附着其他 browser，不启用系统 tracing。
- 所有原始 gameplay 样本、失败、输入和 route gate 保留，不用 trace/host 有效性静默筛除样本。新诊断会增加观测开销，本轮不能直接作为无 tracing 的性能收益对照。

当前 live 配置含 --enable-gpu、--use-gl=angle、--use-angle=d3d11；复现条件保持这些参数。不要用 glFinish、修改 rAF 为 timer、改 vsync 或安全软件设置来制造不同的执行条件。

## 最小 CDP 配置

通过每个已创建 browser 的 browser CDP session，先调用 Tracing.getCategories。只查询本次 browser 的能力，不先跑额外预热场景。

候选 category 列表如下，具体可用性由实际 Chrome 返回值决定；尚未验证 Chrome 153 的事件覆盖或具体事件名。

- toplevel：线程任务的 wall span。
- renderer.scheduler：renderer 调度信息。
- cc、viz：合成与帧派发。
- gpu：GPU 进程相关任务。
- blink.user_timing：只用于三次固定名称的页面标记及 renderer 线程/时钟对应。

若必需类别不可用，记录 unavailable，不能自动扩大到所有类别。首次不打开 disabled-by-default GPU service、完整 devtools.timeline、网络、截图、内存或 CPU sampling 类别。

建议 start 参数：

~~~json
{
  "transferMode": "ReportEvents",
  "bufferUsageReportingInterval": 1000,
  "tracingBackend": "chrome",
  "traceConfig": {
    "recordMode": "recordUntilFull",
    "traceBufferSizeInKb": 32768,
    "enableSampling": false,
    "enableSystrace": false,
    "enableArgumentFilter": true,
    "includedCategories": [
      "toplevel",
      "renderer.scheduler",
      "cc",
      "viz",
      "gpu",
      "blink.user_timing"
    ]
  }
}
~~~

采用 ReportEvents 是为了停止后逐批过滤 dataCollected，避免新增原始 JSON stream 文件和大块解析设施。正常条件在游戏采样结束或首次失败之后再停止并处理 trace，避免在正式窗口内大量处理 trace 数据。

已从安装的 Playwright CDP 类型确认：TraceConfig 支持上述 buffer、sampling、argument filter；tracingComplete 提供 dataLossOccurred；默认未指定 buffer 时会使用 200 MB。本设计显式收紧到每 browser 32 MiB。

## 时间窗和停止条件

1. browser 创建后、page.goto/openProbe 前开始采集，保留 setup 和首用阶段。
2. page.goto 后、openProbe 前记录 page-ready 标记；openProbe 后记录 after-open 标记。
3. 在第一次已存在的 active game callback 采样完成后记录 first-active 标记，仅一次，不新增 rAF 请求或 heartbeat。
4. 原游戏结果或失败已获得后，调用 Tracing.end，等待 tracingComplete，再关闭 owned browser。
5. 设置最长 90 秒的 owned Node 截止，只停止 trace，不改变游戏 loop、输入、route 或原失败处理。若提前触顶，明确标注 trace 未覆盖其后的区间。
6. 不使用 recordContinuously 覆盖早期数据；buffer 满、tracingComplete 数据丢失、停止失败或本地保留上限触发都明确标为不完整。

这能覆盖既有 baseline 的约 31 秒 setup 和后续约 15 秒 pending 窗口。它不保证更迟发生的 stall 一定在 trace 内；没有复现或超出窗口时如实标注，不重命名为成功修复。

## 页面线程与时钟对应

三个标记使用固定名称 th09-sched-page-ready、th09-sched-after-open、th09-sched-first-active。不使用页面标题、URL、房间号、用户输入或动态字符串。

每次记录 performance.now() 的 before/after 和 performance.timeOrigin，紧邻 performance.mark。trace 中匹配到的固定标记提供 renderer pid/tid 及 ts 对应。首次仅增加三次标记，不逐帧写 trace 标记，也不增加 rAF heartbeat。

- 保存 mark 的完整时间括号，不把既有 rAF timestamp 当成回调真实开始。
- Chrome legacy ts 按微秒解码，并用两次标记的时间差核对比例和偏移；不直接把 performance.timeOrigin 当成 trace 的时钟原点。
- 保留匹配数量、每个偏移区间及最大标记括号宽度；匹配缺失或互相矛盾时 clockMapped=false。
- 用既有 rafDiagnostics 的 request、enteredAt、exitedAt、onGameFrame 三点和 controls 做同一页面内关联。
- 跨 peer/host 使用已保存的页面 clock pair；保留校准误差。GPU/renderer 关联以 Chrome trace 的共同时间线和安全 flow ID 为依据，不编造精确 packet 或 Present 因果 ID。

## 有界存储与隐私

未来 helper 的建议上限：每 browser 250,000 个清洗后的事件、32 MiB 清洗后内容、128 条 bufferUsage、8 个标记/校准记录、32 条错误。任何 overflow 记录数量和首次时间。

只落盘清洗后的事件。允许数字时间、duration、pid/tid、安全 flow/id 字段和类别；不保存网络 payload、URL/query、SDP、candidate 文本、截图、堆或 JavaScript 栈。

- arguments 默认丢弃；仅保留固定标记和已明确允许的 Chrome process/thread 元数据。不能只依赖 enableArgumentFilter 保证隐私。
- process/thread 名称采用已知 Chrome 名称白名单；未知名称只留哈希和计数。
- event name 使用已审查的静态调度/GPU 名称及三个固定标记白名单；未知名称只留哈希和计数，不能为了保留可读性而保存任意字符串。
- 仅保留数值或受限格式的 flow/id；无法安全保留的关联字段计入 droppedFields。
- 统计每个类别、线程及未识别事件数量；少了某类事件时不得将“没有观测”写成“该线程空闲”。
- Node 不把原始 dataCollected 写入日志或临时文件。

首次配置只求线程/阶段边界，不承诺获得着色器源码级或 GPU 硬件执行时间。若隐私白名单丢弃了完成特定关联所需的字段，应报告 coverage 不足，先评审必要字段再补采。

## 有效性与结果解释

collectionValid 至少要求：两端按预期启动/停止、tracingComplete 到达、dataLossOccurred=false、本地无 overflow、原 source identity 稳定。coverage 单独列出 renderer 标记、调度类别、cc/viz、gpu 事件是否被实际采到，以及 episode 是否在完整窗口内。

| 同一 pending 窗口内的观察 | 可支持的定位 | 仍不能推出 |
|---|---|---|
| renderer 的其他任务继续，cc/viz 没有对应的帧派发证据 | 继续查 frame eligibility / compositor 派发边界 | 仅凭缺事件就断言浏览器 bug；必须先核对该类别在前后正常区间有覆盖 |
| renderer main 有横跨缺口的长 task | 问题位于该任务的 wall span，需与 C++/audio/pump 已有边界关联 | wall span 不等于 CPU 自耗时；不能把整个跨度计入上一游戏 callback |
| 上游帧任务继续，GPU/提交相关事件出现长等待或缺 completion | 可以针对 GPU 进程、提交/反馈边界提出下一问题 | 不能仅凭 GPU 线程事件断言硬件卡死、驱动故障或着色器编译 |
| 多线程缺事件且 host 有并发工作 | 有调度竞争线索，保留 host 时窗 | 不能由 aggregate CPU 确定单核饱和或进程因果 |
| 本轮没有再出现长 pending | 此次未复现 | 不能证明已修复，也不能删掉原失败 |

本轮不会改变已有 14 字段 samples、4 字段 frameDetails、输入/tape 定义或 route gate。普通结果与 tracing collectionValid 分开保存，完整失败继续进入主 evidence。

## 未来最小实现接点与验证

- 一个独立 Node helper 管理 start/end、过滤、上限、时钟标记元数据和 SHA。
- smoothness 只接 browser 创建后的 start、三处单次 marker，以及成功/失败/finally 的幂等 stop。
- 不改生产 C++、common、canonical relay 或已有 raw。
- 先用 Node 假 CDP 事件验证 end 幂等、未知字段清洗、overflow/dataLoss、缺失 marker、错误传播和 source identity。
- root 评审通过后才实施并串行运行一个真实 Chrome 条件；当前文档不代表已经接入或执行。

## 本次只读能力依据

- th10_web/scripts/native/browser-launch.mjs：headless 和显式 args 覆盖行为。
- th09_web/tests/multiplayer/smoothness-browser.mjs：实际 native GPU 参数、openProbe 与限速顺序、已有 RAF diagnostics。
- D:/workspace/eagler/worktrees/launcher-th11-multiplayer/node_modules/playwright-core/types/protocol.d.ts 的 Tracing 类型：TraceConfig、getCategories、recordClockSyncMarker、dataCollected、tracingComplete、start/end。
- Chrome 的实际 categories 和事件覆盖尚未查询；不能将本地类型存在当作该次运行一定采到所需事件。

