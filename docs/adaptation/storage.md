# 存档与 Replay

[适配流程](README.md) | [文档目录](../README.md)

先打通文件管理，再检查刷新、退出、更新和离线后的结果。Replay 的文件操作与回放一致性分别验证。

## 准备

记录本作普通版和 MP 的存储根。保存一个设置、一份成绩与一个 Replay，按下面的步骤操作。存储根的当前定义见[产品目录](../../common/contracts/product-catalog.mjs)和[MP 路径规则](../../launcher/src/launcher/replay-storage-policy.mts)。

跨源迁移按普通版与多人版各自的数据库归属传输存档和 Replay，覆盖全部注册产品。迁移检查将公开页面的数据库白名单与同一套 MP 存储规则逐项核对，新增作品时同步验证其普通版和多人版迁移入口。

## 逐项检查

| 编号 | 操作 | 通过时应看到 |
| --- | --- | --- |
| <a id="replay-01"></a>REPLAY-01 \| 导入原版 Replay | 导入本作原版 Replay 并播放到声明结束点。 | 可读取并按对应原版规则回放。 |
| <a id="replay-02"></a>REPLAY-02 \| 按原版格式导出可表达输入 | 用可由原版格式表达的键盘或手柄输入录制并导出。 | 导出为原版兼容格式。 |
| <a id="replay-03"></a>REPLAY-03 \| 记录实际生效输入 | 分别用键盘、手柄和触控录制同类动作。 | 记录的是输入合并后实际生效的动作。 |
| <a id="replay-04"></a>REPLAY-04 \| 扩展格式与版本 | 用原版格式无法表达的输入录制，检查扩展标识和版本。 | 扩展信息明确、可验证，回放可还原输入。 |
| <a id="replay-05"></a>REPLAY-05 \| Replay 文件管理 | 列出、读取、写入、删除 Replay，并重新打开游戏。 | 操作作用于本作允许目录，结果持久保存。 |
| <a id="replay-06"></a>REPLAY-06 \| 回放与实时输入隔离 | 回放中移动摇杆、滑动屏幕、按射击。 | 实时操作只影响明确的回放或菜单控制。 |
| <a id="replay-07"></a>REPLAY-07 \| 回放独立于显示刷新率 | 同一回放在 60 Hz 和高刷新率下播放。 | 逻辑状态与结束结果一致。 |
| <a id="save-01"></a>SAVE-01 \| 已声明的存储根 | 核对产品目录中的存储根、成绩文件和配置文件。 | 正常版和 MP 使用各自声明的路径。 |
| <a id="save-02"></a>SAVE-02 \| 用户文件允许范围 | 调用文件操作，包含允许文件与一个目录外路径。 | 用户文件正常操作；目录外请求返回错误。 |
| <a id="save-03"></a>SAVE-03 \| 明确持久化时机 | 修改设置、保存成绩或 Replay，然后执行同步。 | 同步完成后可关闭实例。 |
| <a id="save-04"></a>SAVE-04 \| 退出重开后数据保留 | 退出 Runtime 后创建新的实例。 | 已同步的成绩、设置与 Replay 仍在。 |
| <a id="save-05"></a>SAVE-05 \| 离线读取已有数据 | 已安装并运行一次后断网，关闭页面再打开。 | 同一份用户数据可继续读取和保存。 |
| <a id="save-06"></a>SAVE-06 \| 游戏包与用户数据分开 | 安装新版游戏包，再读取原有成绩与 Replay。 | 游戏资源版本切换后用户数据保留。 |

## 验收方法

### REPLAY-01 | 导入原版 Replay
- 操作：选择来源和 SHA-256 已登记的原版 Replay，导入后按其声明阶段播放至结束点。
- 观察与判定：文件被识别为对应原版格式，播放到声明终点并满足该样本的既定结束条件。
- 结果定位：逐项结果和汇总同时保留尚缺的具体证据。先核对导入文件身份、原生推进与最终错误审计，再核对绑定本 Replay 的独立自然终点。
- 模式归属：浏览器驱动按实际作品和普通版／MP 选择终点判据，格式解析、输入游标与终点评估连到同一目标的结果。
- 反例：损坏文件被当作成功载入，或播放未到声明终点就报告完成，应判失败。
- 入口与覆盖：`tools/ports/replay-browser.mjs` 核对正式包、DATA 与 Replay 身份，执行 Shell 导入和 120 个默认速率原生帧。th08 normal 的 SHA-256 绑定格式解析器独立解密 v6 Replay、解压 payload 并计算所选 stage 的最大成功读取容量；Runtime `ReplayStream` 帧数、输入游标、timing 游标、块长度、stride、stage 和自然结束状态按源码关系核对。容量定义文件块允许的读取上限，不能单独定义原版自然终点。原版捕获绑定 Replay 身份、完整阶段和最终分数；自然终点没有映射到 `ReplayStream` 游标，本样本的 REPLAY-01 保持 `incomplete`，报告保留容量与实际终点计数供核对。原版画面采样行数只表示显示采样，不充当输入游标。其他标题依各自源码和格式记录 cursor/total、阶段 metadata 与可证明的终止范围。普通版播放和多人录像分别记录；MP 新录像按真实多人输入验证。公共浏览器启动实现 `tools/browser/launch.mjs` 与 `tools/browser/playwright.mjs` 也纳入 Replay 决策输入哈希。
- 最终错误审计：同源 Runtime iframe 的 `error`、`abort` 与拒绝命令通过页面绑定实时留存，关闭前排空绑定调用并完成响应 capture 的 flush/stop；浏览器 context 关闭后再审计页面、Console、HTTP、协议、响应身份和 teardown 六个原始频道。任何频道非空都将场景标为失败，当前 epoch 的资源请求失败也照此判定。晚到错误会把最终 `scenarioStatus` 设为 `failed`，并将 teardown 前的状态留在 `preTeardownScenarioStatus`；终止步骤及其原始观测仍保留。覆盖报告只统计记录状态为 `passed` 且 `scenarioStatus` 为 `passed` 的目标。复核报告的 `finalErrorAudit.status`、`failedChannels`、原始 `channels` 与 `lifecycle`；以晚到协议 abort、请求中断和 response stop 错误作负例验证失败门控。

### REPLAY-02 | 按原版格式导出可表达输入
- 操作：录制一个能由原版格式表示的键盘/手柄序列，导出后用原版格式解析器重新读取。
- 观察与判定：导出文件的格式身份及解析后的按键序列与实际录制的可表达输入一致。
- 反例：序列被静默写成扩展格式或键位变化，应判失败。
- 入口与覆盖：`test:local-replay-recorded:browser` 覆盖 th06/07、th08/10 的部分原生录制工作流，但没有完整键盘/手柄矩阵的原版格式导出对照；该编号完整方法待接入。

### REPLAY-03 | 记录实际生效输入
- 操作：分别用键盘、虚拟手柄和触控执行同一段输入，录制后检查 Runtime 消费到的输入和录像帧。
- 观察与判定：每个设备的录像记录实际合并并消费的动作；按下与释放边界均能在帧序列中定位。
- 反例：映射桥看见按钮但原生状态无输入，而录像仍记录按下，应判失败。
- 入口与覆盖：Launcher 的 `test:local-replay-recorded:browser` 录制部分作品的原生输入；`runtime-browser.mjs --gamepad` 检查输入效果，录像字节与三种设备逐项录制验证待接入。

### REPLAY-04 | 扩展格式与版本
- 操作：录制一个原版格式无法表达的扩展输入，检查格式标识、版本字段，再用播放器重新打开。
- 观察与判定：扩展及版本可识别，回放还原所声明的输入；不支持的版本给出明确失败。
- 反例：删掉扩展版本字段后仍把文件按新格式成功解释，应判失败。
- 入口与覆盖：`test-replay-determinism-contract.mjs` 对 th06/07 检查部分 Replay 源码契约，属于静态检查；扩展版本读写的实际浏览器往返场景待接入。

### REPLAY-05 | Replay 文件管理
- 操作：在 Runtime 中创建 Replay 文件，列出、读取、同步，重开实例复读后删除并再次同步。
- 观察与判定：路径、文件字节和 SHA-256 在同步后保持一致；删除后新实例列表中不再出现该文件。
- 反例：路径逃逸请求写到允许目录外，或删除只影响当前页面而刷新后复现，应判失败。
- 入口与覆盖：`runtime-browser.mjs` 的 `replay-file-roundtrip-persistence` 在同一产品包与 normal Runtime 中使用已登记的 Replay 样本，调用官方 Runtime 文件接口完成写入、读取、列出、同步、新 epoch 恢复、删除和再次同步。官方 Replay 检查器或打开入口确认样本格式；一字节伪文件作为拒收负例。报告以游戏、variant、包哈希、存储根、相对文件路径、逐步字节数和 SHA-256 绑定结果；自然播放终点由 REPLAY-01 场景单独记录。

### REPLAY-06 | 回放与实时输入隔离
- 操作：播放录像时分别按方向、射击和菜单控制，并记录录像帧、原生输入与播放器控制状态。
- 观察与判定：游戏实时输入不会改写正在回放的录像输入；只有产品声明允许的播放器/菜单控制能改变播放状态。
- 反例：按实时射击导致录像当前帧输入或录像文件字节改变，应判失败。
- 入口与覆盖：`test-local-replay-native.py` 检查实际 WASM 播放控制；没有覆盖键盘、手柄、触控并发隔离的统一行为矩阵，该项待接入。

### 有限 Replay 场景接口
`tools/ports/replay-recording-scenes.mjs` 根据标题声明生成键盘、虚拟手柄、触控、THMOTION 指针轨迹、基线播放和逐项实时输入扰动场景。适配器通过 `replay` 声明格式语义、原生 trace ABI、生产录制 finalizer 与回放 opener，并保留标题自有的帧和扩展解析函数；共享 runner 调用真实输入接口、写出原生 Replay，并在相同 Runtime 通过官方打开入口播放。共享判定器对照独立解码字节、原生 poll、ReplayStream 游标和玩家消费回执，结果限定在各场景窗口。

浏览器侧复用接口位于 `tests/adaptation/adapters/replay-trace.mjs`：`controlReplayTrace({runtime, contract, enabled, reset})` 显式控制声明的只读 trace，`readReplayTrace({runtime, contract, afterSequence})` 按标题字段映射返回新增样本和环形缓冲区丢样状态。新增标题声明同一版本化样本语义并提供其 production source anchors；标题格式解析器留在本作 `titles/<game>-replay-format.mjs`，共享入口不复制文件布局算法。

th08 普通版的只读 trace 通过 `sdl_replay_trace_control_v1(enabled, reset)` 显式启停；默认关闭，查询状态不会启用或消费采样。`sdl_replay_trace_status_v1(field)` 可读取 ABI 元数据和环形记录：字段 29 返回 ABI 版本 2，30–33 分别返回 next sequence、first available、sample count 和 capacity；从字段 34 开始，每样本 36 个字。word 0–31 包含 post-update 阶段、输入轮询、录制/回放帧与光标、MotionTrack 光标以及玩家消费序号/控制帧/当前与前一输入。word 32–35 是已应用指针的 id、x/y Float32 位模式和 released 标记；点数在 word 24，id 从 `MotionTrack::playback_points` 的有序 map 读取。本场景声明单活动指针，th08 坐标单位为归一化 `[0,1]`，比较容差为 `1e-6`。环形记录的读取由调用方用 sequence 区间检查丢样。各场景结果标记为有限窗口；自然结束和完整 Replay 终点仍按对应适配的终止语义单独判定。

### REPLAY-07 | 回放独立于显示刷新率
- 操作：对同一已登记 Replay，在 60 Hz 和更高显示刷新设置下运行到相同声明结束点。
- 观察与判定：按逻辑帧对齐后，游戏状态和结束结果一致；同时记录显示刷新设置，不能把绘制帧当逻辑帧。
- 反例：只比较两段截图或都退出零码，却没有核对录像逻辑终点，不足以通过。
- 入口与覆盖：`test-replay-determinism-contract.mjs` 静态检查 th06/07 的部分节奏契约；跨刷新率实际回放与状态比较驱动待接入。

### SAVE-01 | 已声明的存储根
- 操作：分别启动 normal 和 MP，读取产品目录声明的存储根，并列出成绩、设置与 Replay 文件。
- 观察与判定：每个文件落在该模式允许的根下，normal/MP 分别核对本作声明的路径与文件策略。
- 反例：MP 成绩写入 normal 的成绩根，应判失败。
- 入口与覆盖：`runtime-browser.mjs` 对当前选中的 normal Runtime 执行文件生命周期，并通过适配器声明的 Runtime 映射读取它自己的 IDBFS 挂载点；产品目录与 `launcher/src/launcher/replay-storage-policy.mts` 共同生成该产品的 normal/MP 命名空间和语言子目录。一次 normal 运行的报告把 MP 路径标为策略派生、把 MP Runtime 标为未执行。MP 实际隔离由对应 MP Runtime 验收报告确认。

### SAVE-02 | 用户文件允许范围
- 操作：对允许路径执行写入/读取/列出/删除，并对 `../`、绝对路径、反斜线和空路径执行同样请求。
- 观察与判定：允许文件往返字节一致；越界路径得到明确拒绝且列表及文件系统未增加越界文件。
- 反例：拒绝响应后仍在目录外生成文件，应判失败。
- 入口与覆盖：`runtime-browser.mjs`、`test-runtime-storage-conformance.py` 检查浏览器真实文件命令及越界路径；Shell VM 合同测试补充 th06/07，覆盖范围按所选游戏记录。

### SAVE-03 | 明确持久化时机
- 操作：修改设置、成绩或 Replay，等待写入后显式执行 sync，再关闭 Runtime。
- 观察与判定：sync 返回成功，关闭前从持久层读取的字节与写入内容一致。
- 反例：sync 尚未完成就关闭，驱动仍记录“已持久化”，应判失败。
- 入口与覆盖：`test-runtime-storage-conformance.py` 检查 sync、持久层内容与临时实例恢复；它使用声明的浏览器和游戏输入，不证明掉电级耐久性。

### SAVE-04 | 退出重开后数据保留
- 操作：同步一份设置、成绩和 Replay，销毁 Runtime 文档后创建新实例并读取相同路径。
- 观察与判定：新实例读出的字节和哈希与关闭前相同，文件列表也包含对应路径。
- 反例：只在旧实例内存可读，新实例返回缺文件，应判失败。
- 入口与覆盖：`test-runtime-storage-conformance.py` 和 `runtime-browser.mjs` 的新 epoch/存档恢复检查实际刷新边界；各数据类型须在本次记录中逐项列明。

### SAVE-05 | 离线读取已有数据
- 操作：先在线同步用户数据，完全断网并关闭页面，再从已安装站点重新打开并读取原文件。
- 观察与判定：离线启动成功，读出的原始字节及哈希与断网前相同，并能完成一次新写入和同步。
- 反例：页面离线可打开但数据为空，或读取依赖在线请求，应判失败。
- 入口与覆盖：`tools/ports/offline-runtime-browser.mjs` 将真实 Launcher 与普通版 Runtime 放在已安装 App Shell 下，在线同步 Replay 后断网重开页面，再核对原始字节并完成离线读取、写入与同步。成绩、配置等数据类型按各自适配报告记录。
- App Shell 响应身份：`app-shell-sw.js` 动态生成 `/__app-shell-update-status__` JSON。离线站点响应观察器只对该精确路由接受 Service Worker 的 HTTP 200、`application/json`、`Cache-Control: no-store` 响应；其 `build` 和完整 `entries` 必须匹配部署 App Shell 合同及冻结站点各文件 SHA-256，`integrity`、时间戳、安装计数和 `runtimeTrusted` 子集按 worker 元数据结构核对。报告保留原响应字节数与 SHA-256、解析出的 metadata 和语义 receipt；其他未登记同源路径仍须匹配静态站点快照。

### SAVE-06 | 游戏包与用户数据分开
- 操作：保存并同步用户文件，安装包含新 Runtime/DATA 的完整新包，再创建新实例读取原文件。
- 观察与判定：包代次更新到新版本，用户文件路径和字节保持原值，且仍挂载到该作品用户数据区。
- 反例：新包安装覆盖成绩/Replay，或读取到另一作品的同名文件，应判失败。
- 入口与覆盖：`runtime-offline-update` 用登记的真实第二 ZIP 产生不同包描述符代次，完整导入后以新 Runtime 读取并离线恢复同一份 Replay 字节。该场景覆盖 Replay 跨代次保留；成绩、配置及 DATA 字节变化的恢复继续按各自场景记录。

## 记录

在本作 `ADAPTATION.md` 的“运行验收记录”按对应编号填写操作和结果。通过时链接[验证记录](record-template.md)，并写明本次构建模式。

## 源码入口

[持久化桥](../../common/browser/save-sync.mjs) | [存储检查](../../launcher/docs/RUNTIME_STORAGE_TESTING.md)
