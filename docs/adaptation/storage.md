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

### 原生成绩与配置的共享旅程

`offline-runtime-browser.mjs` 复用同一包安装与离线旅程，按本作 `storage` 声明读取原生文件。th08 的接入在 Options 修改音效音量，启动普通游戏，经生产 sync、stop 与 Launcher 新 epoch 恢复配置和成绩，再比较新包代次与离线实例。配置值和普通游戏次数从独立文件解析器核对；原始文件保存在本地证据目录，报告绑定文件哈希、解析值、Runtime epoch 和包代次。

被动恢复按原始字节比较。生产保存重新加密成绩文件时，重新校验格式、校验和、压缩数据、兼容版本及成绩语义，时间累计与加密随机值按本作格式单独处理。新作品声明自己的配置操作、原生菜单字段和文件解析器，文件读取、同步、重开和代次恢复沿用共享驱动。

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
- 入口与覆盖：统一队列的 `replay-recording` 场景调用真实原生录制与保存入口，以本作独立格式解析器逐帧核对所选键盘／手柄序列。适配器声明格式、消费边界和动作，报告保存实际窗口和字节身份。

### REPLAY-03 | 记录实际生效输入
- 操作：分别用键盘、虚拟手柄和触控执行同一段输入，录制后检查 Runtime 消费到的输入和录像帧。
- 观察与判定：每个设备的录像记录实际合并并消费的动作；按下与释放边界均能在帧序列中定位。
- 反例：映射桥看见按钮但原生状态无输入，而录像仍记录按下，应判失败。
- 入口与覆盖：共享 `replay-recording` 在同一真实录制世界执行键盘、虚拟手柄和触控序列，分别比较独立解析录像、原生轮询和玩家消费者回执。有限窗口包含按下与释放，场景结果按实际设备路径记录。

### REPLAY-04 | 扩展格式与版本
- 操作：录制一个原版格式无法表达的扩展输入，检查格式标识、版本字段，再用播放器重新打开。
- 观察与判定：扩展及版本可识别，回放还原所声明的输入；不支持的版本给出明确失败。
- 反例：删掉扩展版本字段后仍把文件按新格式成功解释，应判失败。
- 入口与覆盖：共享录制场景由本作声明扩展格式与解析器。th08 普通版记录 version 2 的 THMOTION 指针轨迹，独立解析保存字节，再通过原生打开入口播放并核对指针消费。版本拒绝负控与有效扩展往返分别登记。

### REPLAY-05 | Replay 文件管理
- 操作：在 Runtime 中创建 Replay 文件，列出、读取、同步，重开实例复读后删除并再次同步。
- 观察与判定：路径、文件字节和 SHA-256 在同步后保持一致；删除后新实例列表中不再出现该文件。
- 反例：路径逃逸请求写到允许目录外，或删除只影响当前页面而刷新后复现，应判失败。
- 入口与覆盖：`runtime-browser.mjs` 的 `replay-file-roundtrip-persistence` 在同一产品包与 normal Runtime 中使用已登记的 Replay 样本，调用官方 Runtime 文件接口完成写入、读取、列出、同步、新 epoch 恢复、删除和再次同步。官方 Replay 检查器或打开入口确认样本格式；一字节伪文件作为拒收负例。报告以游戏、variant、包哈希、存储根、相对文件路径、逐步字节数和 SHA-256 绑定结果；自然播放终点由 REPLAY-01 场景单独记录。

### REPLAY-06 | 回放与实时输入隔离
- 操作：播放录像时分别按方向、射击和菜单控制，并记录录像帧、原生输入与播放器控制状态。
- 观察与判定：游戏实时输入不会改写正在回放的录像输入；只有产品声明允许的播放器/菜单控制能改变播放状态。
- 反例：按实时射击导致录像当前帧输入或录像文件字节改变，应判失败。
- 入口与覆盖：共享录制组以同一份 SHA-256 绑定录像运行基线与实时输入扰动。th08 普通版覆盖键盘方向、键盘射击、手柄射击与触控射击，核对输入确已到达原生轮询，玩家消费者继续使用录像输入。菜单／播放器允许控制按本作策略另列。

### 有限 Replay 场景接口
`tools/ports/replay-recording-scenes.mjs` 根据标题声明生成键盘、虚拟手柄、触控、THMOTION 指针轨迹、基线播放和逐项实时输入扰动场景。适配器通过 `replay` 声明格式语义、原生 trace ABI、生产录制 finalizer 与回放 opener，并保留标题自有的帧和扩展解析函数；共享 runner 调用真实输入接口、写出原生 Replay，并在相同 Runtime 通过官方打开入口播放。共享判定器对照独立解码字节、原生 poll、ReplayStream 游标和玩家消费回执，结果限定在各场景窗口。

浏览器侧复用接口位于 `tests/adaptation/adapters/replay-trace.mjs`：`controlReplayTrace({runtime, contract, enabled, reset})` 显式控制声明的只读 trace，`readReplayTrace({runtime, contract, afterSequence})` 按标题字段映射返回新增样本和环形缓冲区丢样状态。新增标题声明同一版本化样本语义并提供其 production source anchors；标题格式解析器留在本作 `titles/<game>-replay-format.mjs`，共享入口不复制文件布局算法。

th08 普通版的只读 trace 通过 `sdl_replay_trace_control_v1(enabled, reset)` 显式启停；默认关闭，查询状态不会启用或消费采样。`sdl_replay_trace_status_v1(field)` 可读取 ABI 元数据和环形记录：字段 29 返回 ABI 版本 2，30–33 分别返回 next sequence、first available、sample count 和 capacity；从字段 34 开始，每样本 36 个字。word 0–31 包含 post-update 阶段、输入轮询、录制/回放帧与光标、MotionTrack 光标以及玩家消费序号/控制帧/当前与前一输入。word 32–35 是已应用指针的 id、x/y Float32 位模式和 released 标记；点数在 word 24，id 从 `MotionTrack::playback_points` 的有序 map 读取。本场景声明单活动指针，th08 坐标单位为归一化 `[0,1]`，比较容差为 `1e-6`。环形记录的读取由调用方用 sequence 区间检查丢样。各场景结果标记为有限窗口；自然结束和完整 Replay 终点仍按对应适配的终止语义单独判定。

### REPLAY-07 | 回放独立于显示刷新率
- 操作：对同一已登记 Replay，在 60 Hz 和更高显示刷新设置下运行到相同声明结束点。
- 观察与判定：按逻辑帧对齐后，游戏状态和结束结果一致；同时记录显示刷新设置，不能把绘制帧当逻辑帧。
- 反例：只比较两段截图或都退出零码，却没有核对录像逻辑终点，不足以通过。
- 入口与覆盖：统一队列的 `display-replay` 用同一 Replay 配对正式包与诊断包，再按 60、120、144 Hz 生产回调节奏比较声明的600个逻辑步、原生消费与权威状态。报告绑定真实产物、输入和观察接口；完整自然终点由本 Replay 的独立终点判据负责。物理高刷显示与观感集中进入 Presentation Lab 人审。

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
- 入口与覆盖：共享离线旅程按本作 `storage` 接口执行真实配置操作和普通游戏保存。th08 核对 Options 音量变化、确认键前的菜单观察、原生已选难度与角色，以及对应的游戏次数增量，再通过 sync、成功退出事件和新 Runtime 独立解析保存字节。`test-runtime-storage-conformance.py` 补充 sync、持久层内容与临时实例恢复。

### SAVE-04 | 退出重开后数据保留
- 操作：同步一份设置、成绩和 Replay，销毁 Runtime 文档后创建新实例并读取相同路径。
- 观察与判定：新实例读出的字节和哈希与关闭前相同，文件列表也包含对应路径。
- 反例：只在旧实例内存可读，新实例返回缺文件，应判失败。
- 入口与覆盖：共享原生文件旅程记录成功退出事件与新 epoch 的 first-frame，分别复核配置、成绩和同一 Replay。Replay 各次读取保留字节，由共享判据重算哈希并关联源文件；成绩重存按解码语义核对，被动重开按原始字节核对。完整判据同时核对已安装包与各 Runtime 的实际代次；`test-runtime-storage-conformance.py` 和 `runtime-browser.mjs` 补充刷新与文件生命周期。

### SAVE-05 | 离线读取已有数据
- 操作：先在线同步用户数据，完全断网并关闭页面，再从已安装站点重新打开并读取原文件。
- 观察与判定：离线启动成功，读出的原始字节及哈希与断网前相同，并能完成一次新写入和同步。
- 反例：页面离线可打开但数据为空，或读取依赖在线请求，应判失败。
- 入口与覆盖：`tools/ports/offline-runtime-browser.mjs` 将真实 Launcher 与普通版 Runtime 放在已安装 App Shell 下，在线保存后断网重开页面，再核对原始字节并完成离线读取与生产重存。声明 `storage` 的作品在同一旅程检查成绩、配置和 Replay；th08 的解析器重新核对完整成绩语义，单列时间累计与加密随机值。
- App Shell 响应身份：`app-shell-sw.js` 动态生成 `/__app-shell-update-status__` JSON。离线站点响应观察器只对该精确路由接受 Service Worker 的 HTTP 200、`application/json`、`Cache-Control: no-store` 响应；其 `build` 和完整 `entries` 必须匹配部署 App Shell 合同及冻结站点各文件 SHA-256，`integrity`、时间戳、安装计数和 `runtimeTrusted` 子集按 worker 元数据结构核对。报告保留原响应字节数与 SHA-256、解析出的 metadata 和语义 receipt；其他未登记同源路径仍须匹配静态站点快照。

### SAVE-06 | 游戏包与用户数据分开
- 操作：保存并同步用户文件，安装包含新 Runtime/DATA 的完整新包，再创建新实例读取原文件。
- 观察与判定：包代次更新到新版本，用户文件路径和字节保持原值，且仍挂载到该作品用户数据区。
- 反例：新包安装覆盖成绩/Replay，或读取到另一作品的同名文件，应判失败。
- 入口与覆盖：`runtime-offline-update` 用登记的真实第二 ZIP 产生不同包描述符代次，完整导入后以新 Runtime 读取并离线恢复同一份用户数据。th08 在新代次与离线实例逐文件核对成绩、配置和 Replay 的字节身份；此路线的两代包保持同一 DATA 身份，DATA 内容更新由对应资源场景验证。

## Replay 音频接管

播放器按回放控制能力组织，作品类型与普通版、MP 的资源和存储身份由各作维护。

| 播放器类别 | 当前作品 | 共享入口 | 本作职责 |
| --- | --- | --- | --- |
| 关卡录像 | th06、th07、th08、th10、th11、th15 的普通版与 MP | `common/browser/local-replay.mjs`、`local-replay-native.mjs` | 解析格式、选关初始化、逻辑步进、结束判定与音频输出 |
| 对战录像 | th09 普通版与 MP | `common/browser/versus-replay-th09.mjs`、`playback-clock.mjs` | 原生双人回放、对局信息、回合边界与本作音频接口 |
| 游戏内菜单回放 | th075 普通版与 MP、th20 普通版 | 本作菜单与游戏循环 | 原生选片、回放操作与音频生命周期 |

th09 对战播放器保留完整的本作实现，普通版和 MP 打包均直接读取共享目录。开发 shell 使用逐字节一致的本地副本，通过 `node common/tools/sync-versus-replay.mjs --check` 核对。该模块随本作的 `auto-replay.mjs` 一同发布，继续读取对局与回合信息。新增对战作品时按本作格式和原生 ABI 接入，再提取已验证的共同能力。

th06、th07 通过原生 `Render()` 推进录像，停止自动迭代时保留音频设备，声音由 `SoundPlayer` 与本作前后台入口管理。目录 Runtime 通过独立的输出适配接口交接设备和 PCM 填充；两类入口分别核对。

录像工作台在同一个游戏 iframe 中切换文件和关卡。共享 `local-replay` 桥先停止普通循环，在原生录像打开完成后恢复音频设备，再由录像时钟推进固定逻辑步。浏览器音频恢复异步进行，命令立即回复；等待输出就绪期间保留当前逻辑帧。

目录 Runtime 通过 `pauseAudio` 连接本作原生输出，已有前台恢复函数通过 `resumeAudio` 继续管理 SDL 节点连接。暂停、定位、结束与后台共同决定输出状态；定位结束保留原来的暂停选择，并清理跳过区间的瞬时音效。切换文件后重新应用输出状态，关闭工作台时暂停输出。

th08 普通版与 MP 的 `eagler_replay_audio_pump`、th10 普通版与 MP 的 `sdl_audio_pump` 在录像逻辑步后填充 PCM，持续提供 OGG 背景音乐。th11、th15 的原生录像步包含 PCM 填充。th09 使用对战播放器中的恢复、音频填充和静音流程。

验收首次选片、连续换片、重播、暂停定位与前后台切换。分别检查 BGM 和 SFX，确认定位结束后按原来的暂停选择恢复输出，跳过区间的瞬时音效不补播。浏览器恢复测试包含 pending 的 `AudioContext.resume()`，核对命令响应和逻辑帧保持。

共享桥的行为测试位于 [`local-replay-bridge.test.mjs`](../../common/tests/local-replay-bridge.test.mjs)。[`test-replay-audio.py`](../../launcher/tests/browser/test-replay-audio.py) 通过 `--game` 和 `--multiplayer` 选择 th08、th10、th11、th15 的正式 Runtime。明确提供录像、DATA、OGG 与字体后，在 iframe 中分别采集 BGM 和 SFX 音频图，检查持续填充、换片、暂停与定位，并记录输入哈希。

## 记录

在本作 `ADAPTATION.md` 的“运行验收记录”按对应编号填写操作和结果。通过时链接[验证记录](record-template.md)，并写明本次构建模式。

## 源码入口

[持久化桥](../../common/browser/save-sync.mjs) | [存储检查](../../launcher/docs/RUNTIME_STORAGE_TESTING.md)
