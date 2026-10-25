# 启动与资源

[适配流程](README.md) | [文档目录](../README.md)

Runtime 是编译后的游戏程序和浏览器接口。启动器负责界面、安装与资源准备，Runtime 负责游戏循环和本作文件。

## 准备

按游戏开发指南生成 Runtime，在启动器中导入本地游戏数据。打开浏览器开发者工具，同时查看 Console 和 Network。协议字段见[Runtime 接口参考](../../launcher/docs/GAME_ADAPTER_CONTRACT.md)。

## 逐项检查

| 编号 | 操作 | 通过时应看到 |
| --- | --- | --- |
| <a id="runtime-01"></a>RUNTIME-01 \| 协议身份与会话代次 | 从启动器启动、退出，再启动；检查协议、作品、origin、source 与 `runtimeEpoch`。 | 每次实例只处理属于自己的消息。 |
| <a id="runtime-02"></a>RUNTIME-02 \| 就绪、配置、启动、首帧 | 依次观察 `ready → configure → launch → first-frame`。 | 配置完成后开始游戏；画面呈现后才报告首帧。 |
| <a id="runtime-03"></a>RUNTIME-03 \| 命令响应与错误信息 | 调用配置、文件和输入命令，再用一个无效参数调用。 | 请求收到对应结果，错误可由启动器显示。 |
| <a id="runtime-04"></a>RUNTIME-04 \| 后台、恢复与退出清理 | 切换前后台、退出、再次进入；中途保持按键和触点。 | 返回后声音和画面可恢复，退出后监听器与临时输入已释放。 |
| <a id="runtime-05"></a>RUNTIME-05 \| Runtime 与资源管理分工 | 从启动器安装资源，再启动 Runtime。 | 启动器管理安装和下载；Runtime 读取已提供的资源。 |
| <a id="runtime-06"></a>RUNTIME-06 \| 重开与常显判定点 | 暂停后按 R 重开；分别开关常显判定点并移动、低速。 | 重开进入本局初始状态；判定点沿用本作视觉，移动速度与碰撞规则正常。 |
| <a id="package-01"></a>PACKAGE-01 \| 共用游戏包存储 | 从启动器安装并查看本地游戏包。 | 资源进入共用 Package Store。 |
| <a id="package-02"></a>PACKAGE-02 \| DATA 身份与挂载目标 | 核对 DATA 文件、挂载路径与清单身份。 | 实际字节、哈希和 Runtime 声明一致。 |
| <a id="package-03"></a>PACKAGE-03 \| 完整更新后切换版本 | 在更新中断开下载，再重新打开游戏。 | 使用上一完整版本；恢复更新后才切换。 |
| <a id="package-04"></a>PACKAGE-04 \| 本地导入与在线安装一致 | 分别用在线安装、本地 ZIP 和支持的原版文件导入。 | 进入同一套游戏包与版本管理流程。 |
| <a id="package-05"></a>PACKAGE-05 \| 已安装游戏离线启动 | 首次安装并运行后断网，刷新或重开页面。 | 启动器、Runtime 和所需本地资源均能加载。 |
| <a id="package-06"></a>PACKAGE-06 \| 统一安装和更新入口 | 检查 Runtime 的安装、缓存和更新调用。 | 安装状态由启动器维护，游戏读取已安装资源。 |

## 验收方法

真实窗口场景在启动等待期间持续记录宿主页、Runtime 文档与原生页面／窗口身份。启动前台意外变化时，驱动最多恢复两次已核验的自有窗口；身份改变或无法恢复直接结束该启动。生命周期操作在启动完成后执行，保留自然发生的后台与恢复事件。

### RUNTIME-01 | 协议身份与会话代次

正式 Runtime 的两次导航分别执行同一协议矩阵。真实 parent postMessage 请求检查活跃通道，受控 MessageEvent 逐一改变 protocol、game、epoch、source、origin 和 command 类型，独立记录 Runtime 的全部响应；同一注入入口的有效消息作为正对照。每个反例后继续真实请求，确认拒绝与通道故障可区分。该字段注入用于核对 Shell 的生产消息处理器，浏览器跨源隔离由浏览器负责。
- 操作：用正式 Launcher 启动 Runtime，完成一次请求后重开实例，并对旧实例发送带旧 epoch 的请求。
- 观察与判定：响应的协议、游戏、origin、request 与当前 epoch 匹配；旧实例请求不被新实例接受才通过。
- 反例：把旧 epoch 请求发给当前实例却收到成功响应，应判失败。
- 入口与覆盖：`runtime-browser.mjs` 检查实际普通版身份与跨导航旧 epoch；`test-runtime-protocol-contract.mjs` 静态遍历产品 normal/MP Shell，不能单独证明 MP 浏览器运行。

### RUNTIME-02 | 就绪、配置、启动、首帧

中央判据将 ready、configure 响应、launch 响应和 first-frame 与原始事件序列逐项匹配。首帧须提供原生成功提交计数，画布尺寸大于零；提前报告首帧、缺少提交或引用其他 epoch 的事件都会阻断完整结论。
- 操作：从真实宿主页启动当前包，等待 ready，再发送 configure、launch，并等待首帧。
- 观察与判定：事件顺序为 ready、配置成功、启动成功、首帧；首帧对应实际绘制提交，画布尺寸有效。关卡逻辑推进由输入与生命周期场景独立验证。
- 反例：Runtime 在配置完成前报告首帧，或只回 ready 却没有运行画面，应判失败。
- 入口与覆盖：`runtime-browser.mjs` 的 `configure-launch-first-frame` 在选定的普通版包上验证协议事件与有效画布尺寸。配对呈现模式进一步保存冷启动与刷新后的浏览器合成帧，按 [DISPLAY-07](presentation.md#display-07) 重判时序。th08 的首帧附带可选 `presentation` 收据，记录 `eagler-runtime/first-frame/1`、`nativePresentCount` 与发送时的 `sentAtEpochMs`。桌面报告将 host navigation、ready、keyboard bridge、configure、launch、first-frame 六项作为启动 stages；每个有首帧的 Runtime epoch 以唯一 `gameplayReadinessByEpoch` 收据记录原生玩法资格，重启期间的同实例观察另存为 `gameplayReadinessTransitions`。这两类读取分开归档，启动汇总按 epoch 检查唯一启动收据；资格读取不计作第七个启动 stage。逻辑帧推进由后续 `hosted-keyboard-native-effect` 检查。

逐作标题导航放在 normal adapter 的 `flow` 声明中。有限流程只使用 `wait`、`press` 和有最大次数的 `press-until`，条件从具名谓词与安全字段路径组成；路径只读归一化 observation 的自有对象字段，不执行任意表达式或数组索引。每个步骤和谓词都指向本作源码，并受总时长、步骤数和输入次数约束。共享执行器只通过正式运行时的 observe、press 和 waitUntil 接口操作；状态缺失或预算耗尽会停止流程，不继续猜测按键。既有 `confirm` 和 TH20 菜单流程保持兼容，多人版仍走房间验收入口。

### RUNTIME-03 | 命令响应与错误信息
- 操作：发送一条有效命令及一条未知命令，分别等待带相同 request 身份的响应。
- 观察与判定：有效命令返回预期数据；未知命令返回可显示的错误且请求结束，不挂起才通过。
- 反例：未知命令静默成功、超时或让后续有效请求无法响应，应判失败。
- 入口与覆盖：`runtime-browser.mjs` 实际调用未知命令并检查存档列表/同步；`test-runtime-protocol-contract.mjs` 只查协议源码声明，不覆盖所有命令语义。

### RUNTIME-04 | 后台、恢复与退出清理

`launcher-lifecycle` 复用正式 Launcher 的安装与启动准备，在同一会话按住方向键和生产触控低速按钮，执行真实同窗口标签切换。恢复后核对原生输入中立、画面提交继续和实际射击后的八窗混音 PCM，再次按住输入并通过 Launcher 的浏览器返回流程退出，经过生产保存、清空与 iframe 退役。保留的旧输入桥只读引用须已松键且失去映射所有权；新代实例的原生输入回到中立，单次方向键继续驱动玩家。Chrome 直接读取宿主与 Runtime 的 window／document 输入监听器位置和数量，比较退出前后两代。新增作品提供原生观察、动作、导航和混音 AudioContext 路径，操作与判据共用。

该旅程先关闭首次触控说明，再通过可见的生产按钮关闭默认自动射击，记录按钮状态与原生中立输入，再检查实体键的持有与释放。使用完整 Chrome 的原生标签可见性；Windows 窗口遮挡推断关闭以隔离标签场景，参数进入报告，窗口最小化另由桌面后台组执行。恢复过程和输出采样各自留证，退出后重新进入使用新 Runtime epoch，包、DATA、站点及驱动身份在前后复核。反例覆盖旧键未释放、旧映射存活、实例代次未变、重复注册监听器、画面停帧、静音和音频源身份改变。
- 操作：玩法中按住方向键和触点，把页面切到后台再恢复，随后退出并重新进入。
- 观察与判定：恢复后画面与声音继续工作；退出时 held input 回到中立，重新进入后只有一组有效监听器。
- 反例：退出后旧实例仍持续消费按键，或恢复时重复响应同一次输入，应判失败。
- 入口与覆盖：`runtime-desktop-probes.mjs` 用 public `launchServer` 启动完整 headed Chrome，并以 loopback CDP 网关按命令 id、session 和真实 page target 审计 Playwright 焦点改写及 Chrome 成功响应。检查前要求页面原生 visible/hasFocus，记录 Chrome target/window ID 与 bounds，再用真实窗口最小化和同窗标签激活观察浏览器生成的 blur/hidden 事件；最小化期间必须同时观察到 Host 接受 keyboard-clear、原生输入回到中立，才验证 held input 清理。标签窗口中的临时页关闭后，重新核对原游戏 page target、window ID、宿主页与 Runtime iframe 的 visible/hasFocus；音频检查重新取当前状态，要求输出 tap 对应的 AudioContext 在 KeyZ 前已经 running，再要求 Host 接受 KeyZ、原生输入发生变化，并记录 PCM 与输入释放。截图会先恢复真实 Chrome 窗口并核对 page target、window ID 和两个文档的原生焦点，再检查 iframe bounding box 完整落在 viewport 内，以 page screenshot clip 采集当前画面。后台连续运行与挂起后恢复单独记录，KeyZ 手势不能替代后台恢复证据。`--background-only` 运行窗口、标签和声音三项后台检查，并保留包与私有 DATA 前后身份核验；未运行的桌面矩阵以 omitted 单列。`runtime-browser.mjs` 检查暂停、重开与尺寸变化；触点和完整监听器清理分别记录。

桌面窗口恢复集中等待同一 page target、window ID、实际窗口状态和 Host／Runtime 文档焦点共同稳定。窗口已正常可见聚焦时，先聚焦 Runtime canvas 并连续核对两次；需要恢复时，操作作用于已核对的所属窗口，最小化窗口先恢复，再激活真实标签。每次恢复保存观察序列，原生助手另存调用前后窗口与前台进程身份、时间和 API 返回值，身份变化或超时均保留失败／未完成结果。

CDP 网关以有界命令日志关联窗口调整、标签激活、输入与带手势的脚本调用，保存请求、会话、目标及发送／响应单调时钟。日志保留命令身份和错误，脚本正文与输入内容不进入记录；容量溢出单列计数，供后台切换的时间线审查。

网关的握手队列、单条消息与发送积压分别限额：默认8 MiB、384 MiB与512 MiB。消息预算覆盖浏览器响应采集所支持的256 MiB资源及其 Base64 编码；发送前同时计入已排队字节、本条消息和帧头。超出预算时保存端点与限额错误，并关闭所属连接。

### RUNTIME-05 | Runtime 与资源管理分工
- 操作：先由 Launcher 安装/准备声明的资源，再启动 Runtime 并读取其声明的 DATA 或共享资源路径。
- 观察与判定：Launcher 的安装状态与资源字节可核对，Runtime 从提供的挂载读取；游戏运行期间不自行安装或替换 Package Store 内容。
- 反例：在 Runtime 内直接下载并覆盖用户包，绕过 Launcher 版本管理，应判失败。
- 入口与覆盖：`offline-runtime-browser.mjs` 通过 Launcher UI 导入真实包，分别在在线启动与离线重开时核对已安装代次、全部文件引用、存储对象的实际字节哈希和 Runtime 实际取得的 managed DATA 哈希。Runtime 文档在脚本加载前记录 Package Store 的对象、代次和安装表写事务、删库及升级操作，要求这些安装内容保持不变；租约管理和本作用户文件由各自的状态观察。

资源边界旅程在在线会话的正常操作完成后，复用同一 Runtime 执行一次安装对象表写事务，要求原判据检出并保存反例，再继续离线重开。事务保持包字节不变，检查写入尝试本身也会被检出。`--mutate-runtime-package-store` 在初次启动时执行同一反例，可单独验证整个旅程以失败退出；输入与证据均属于本轮临时站点和浏览器存储。

### RUNTIME-06 | 重开与常显判定点

th08 的 `view_options_snapshot_v1` 读取生产常显开关、玩家已消费选项、低速状态、判定点 ANM 对象及实际碰撞盒，位置和尺寸使用原生逻辑坐标。选项传递、对象生成与画面／运动的验收分别取证；设置值本身不替代显示结果。

`view-options` 复用正式 Launcher 安装，从生产设置切换常显选项，每次选择后新开 Runtime。关闭实例内对照原生高速与低速，开启实例内核对高速保持、松键静止、原生判定点对象及碰撞盒尺寸；每个实例复用一个无出怪世界。再次关闭后恢复自动循环，检查真实暂停和同实例重开。三个画面对照进入已有显示场景集，机器结果与显示位置的人审结论分别保存。中央重开 PNG 字节核对身份，并从原始快照复算自动不变量。

`node tools/ports/view-options-review.mjs --game=th08 --report=<原始报告> --prepare --output=<材料目录>` 准备三张画面与一组 `review.json`。集中查看 `review.md` 后填写审查者、时间及 `player-marker` 组结论；接纳时去掉 `--prepare`，增加 `--review=<review.json>` 并选择新的输出目录。接纳工具重新核对原始机器结果与图片字节，中央验收再复算同一组证据。准备与接纳沿用原采集时间和产物，不启动新浏览器。

安装后启动、生命周期重开与选项对照共用 `launcher-runtime-start.mjs`。驱动从实际启动按钮进入，按当前 epoch 核对 ready、原生首帧与错误，音乐提示沿生产文案和确认动作匹配并等待关闭。旧 epoch、重复提示、未知决策和未呈现的启动分别阻断。
- 操作：在关卡中记录逻辑帧和玩家位置，暂停后执行本作重开；产品声明常显判定点时，再分别开关它并移动、低速。重开适用于全部支持该流程的作品与模式。
- 观察与判定：暂停期间世界逻辑帧停止；重开后场景状态重建且逻辑继续；判定点开关与本作碰撞区域显示一致。
- 反例：暂停时世界仍推进、重开沿用旧局状态，或关闭显示选项后判定点仍残留，应判失败。
- 入口与覆盖：`runtime-browser.mjs` 的 `native-pause-and-restart` 检查选定 Runtime 的暂停/重开原生状态； hitbox 视觉与碰撞区域对照需按作品记录，尚无覆盖全部产品的统一场景。

桌面重开场景在每次物理按键前读取宿主页焦点和活动元素，将焦点交给宿主页 body，并再次读取确认；只接受当前 Runtime epoch 的宿主键盘转发记录，且按下与释放都必须被 Runtime 输入桥接受。iframe 内部收到的按键不替代宿主转发证据。

th08 的受控场景锁定后，每次 `game.load()`（包括按 R 重开）都会进入独立写入 scope。`score.dat` 初始化写入进入当前 Runtime 的 RAM overlay，其他路径保持拒绝，持久提交字节为零；未锁定会话继续走原持久化写入路径。

`runtime` 场景组登记当前实际执行的 RUNTIME-01／02／03／06。RUNTIME-05 的资源管理职责边界保留为整作验收要求，由对应的资源获取、挂载与故障操作提供独立证据。

### PACKAGE-01 | 共用游戏包存储
- 操作：在浏览器中安装包，随后从 Launcher 读取当前代次及其文件对象，再重新打开包列表。
- 观察与判定：安装结果出现在同一 Package Store 当前代次，文件引用可读且刷新后仍能定位。
- 反例：成功提示出现但当前代次没有包对象，或包被写入仅当前标签页可见的临时位置，应判失败。
- 入口与覆盖：`test:package-store:browser` 检查浏览器 IndexedDB 事务；`offline-runtime-browser.mjs` 用本次选择的真实 ZIP 经 Launcher UI 安装，核对可读代次与对象引用，并在启动 Runtime 和离线重开后再次读取同一安装状态。

### PACKAGE-02 | DATA 身份与挂载目标
- 操作：对照包描述符中的 DATA 路径、挂载目标与本次私有 DATA 文件，计算实际字节数和 SHA-256。
- 观察与判定：Launcher 提供的 DATA 字节等于声明身份，Runtime 从声明挂载目标读到同一文件才通过。
- 反例：用相同文件名但不同字节替换 DATA，预检或 Runtime 验收应失败。
- 入口与覆盖：`runtime-browser.mjs` 在实际启动前复核包、DATA 和源码收据；覆盖范围仅为输入清单中本次选定的产品与模式。

### PACKAGE-03 | 完整更新后切换版本

传输中断点同时记录浏览器实际读到的文件前缀长度与 SHA-256，并与真实 ZIP 中的同一段字节比较。收到该前缀后才取消下一次流读取，要求实际 AbortError 和浏览器请求取消记录相符。
- 操作：在有可用旧包时中断新包下载，刷新页面，再完整恢复更新。
- 观察与判定：中断期间当前代次仍指向完整旧包；新包全文件校验后才切换到新代次。
- 反例：故意令一个文件长度或哈希不符，当前代次不得切到不完整新包。
- 入口与覆盖：`runtime-offline-update` 复用显式登记的真实旧、新 ZIP。生产远端安装器下载并校验第一个新增文件，原始证据读取暂存代次及对象的实际哈希；随后对第二个文件返回正确 HTTP 200、长度和内容前缀，暂停传输期间取消请求。判据要求浏览器报告真实请求取消、当前代次保持旧版、暂存代次和无引用对象被清理。再次从 Launcher 启动旧版并核对首帧及 DATA，再经导入界面拒绝不完整归档、提交完整新版，核对新代首帧与离线重开。下载夹具只控制传输节奏，文件内容来自同一真实 ZIP，未改写安装器与存储结果。`test:package-store:browser` 和 `test:package-generation-conditions:browser` 另以合成包检查事务取消与双标签竞争。

### PACKAGE-04 | 本地导入与在线安装一致

`package-imports` 将本地 ZIP、在线发布包和产品声明支持的原版 DATA 文件分别放入全新浏览器存储。ZIP 使用导入站点，在线场景使用同一实际 ZIP 发布的 Hosted 清单与文件。原版 DATA 使用提供共享字体的 Hosted 站点及空包目录，保留原始文件名和输入哈希。每条路径经过正式 Launcher 操作与原生首帧，再关闭游戏、重开页面并再次启动，核对版本、当前代次、安装来源及 managed DATA 实际字节。中央按产品目录枚举支持的入口并重算判据；新增作品沿用场景，不需要本作菜单导航接口。各来源独立封存失败并继续下一项，启动错误面板立即提供失败原因。

Runtime 固定代次和 Package Store 当前代次分别取证。后台补充可选资源时，新代次继续引用同一描述符与基础对象；中央逐项比较基础引用、全部已加载文件和 DATA 实际字节，再核对重开后的版本与基础对象。基础对象替换、描述符变化或引用到其他代次的 DATA 都会失败。
- 操作：分别通过在线获取和本地 ZIP 导入同一包，比较安装后目录、代次清单和游戏身份。
- 观察与判定：两条路径经过同一包校验及存储流程，产出的有效文件与包身份一致。
- 反例：本地 ZIP 绕过文件哈希检查或写到独立旧存储，应判失败。
- 入口与覆盖：`test:package-store:browser` 覆盖夹具获取与本地导入流程；`test:th20-import-launch:browser` 覆盖 th20 的真实启动导入路径，不能外推到全部作品。

### PACKAGE-05 | 已安装游戏离线启动
- 操作：在线安装后确认 Runtime 可启动，断开网络并刷新/重开页面，再进入同一游戏。
- 观察与判定：Launcher、包入口、模块、WASM 与所需本地 DATA 均来自已安装内容，并到达首帧。
- 反例：离线后仍请求未缓存的模块，或只能打开页面却不能加载游戏，应判失败。
- 入口与覆盖：`tools/ports/offline-runtime-browser.mjs` 把实际 Launcher 打包到新站点，以真实 ZIP 导入并启动所选普通版 Runtime；HTTPS 静态入口、Runtime HTML/脚本、字体与站点文件由浏览器响应逐字节核验。关闭网络后由已安装 Service Worker 重开站点、恢复 Runtime 与 Replay 文件并再次同步。`runtime-offline` 对八作逐项运行，覆盖本次选择的真实包和 DATA。运行收尾统一核对页面异常、console、HTTP 与请求失败、Runtime 协议中止、响应身份及清理结果，作为场景最终判定。

### PACKAGE-06 | 统一安装和更新入口
- 操作：分别从游戏卡片、资源准备和更新提示进入安装/更新，完成后再次打开游戏并读取当前代次。
- 观察与判定：入口读取同一安装状态；运行中的 Runtime 固定启动代次，后台更新完整提交后，经重开切换到 Launcher 当前代次。
- 反例：卡片显示“已安装”而资源服务读到另一代次，应判失败。
- 入口与覆盖：`offline-runtime-browser.mjs --installation-authority-only` 使用显式 update manifest 指定的真实第二代 ZIP，核对描述符、文件哈希后在隔离站点生成同源 Release Catalog。浏览器经 UI 导入、重开游戏卡片并启动原代次，分别处理音乐确认与“更新已就绪”提示。后台下载提交新代次时，运行中的 Runtime 保持原代次；点击生产“立即重启”后，新 epoch 的 ready、first-frame 与 managed DATA 必须绑定新代次，离线重开继续使用该代次。只读 IndexedDB 日志记录卡片、首次资源准备、更新后重开与离线读取，共享判据重算完整代次链。第二代 ZIP、原代次首帧、新 epoch 或任一读取环节缺失均阻止完整覆盖。PACKAGE-03 与 SAVE-06 使用各自的中断导入及文件恢复场景。
- 该更新旅程输出 PACKAGE-06 的完整收据。基础安装、Runtime 资源边界和存储编号由 `runtime-offline` 的同代次旅程各自生成收据，保留不同场景的代次语义。

## 记录

在本作 `ADAPTATION.md` 的“运行验收记录”按对应编号填写操作和结果。通过时链接[验证记录](record-template.md)，并写明本次构建模式。

## 源码入口

[消息协议](../../launcher/src/contracts/runtime-protocol.mts) | [数据布局](../../launcher/lib/content-definition.mjs) | [包管理](../../launcher/package/README.md)

## 房间内的持续实例

支持房间菜单返回的 Runtime 在 `ready` 声明 `roomReturn`。退出旁观或返回房间使用 `return-room`：结束本局传输、清理回滚和临时输入，恢复原生标题菜单，并在菜单呈现后回复 `presented: true`。iframe、Wasm、DATA 和已安装资源持续保留。原生菜单操作返回时使用 `room-menu` 事件，同步恢复房间工作区。

每次启动携带新的 `launchSerial`；`first-frame` 和 `room-menu` 回传该次编号。启动器只接收当前启动编号的呈现与菜单事件。验收连续三次旁观往返，核对同一 document 和 Module、菜单动画、玩家对局持续推进及零次程序和 DATA 重载。该约定覆盖共用启动器和各作多人 Runtime。

每次旁观重进先清空上次的接收时间线，再以本次收到的历史重建画面。开局初始化期间也能返回菜单，原生场景管理器按当前阶段恢复标题资源；菜单停留后重进与连续快速切换使用同一套生命周期。

## 安装代次与后台更新

Package Store 在领取暂存区和切换当前版本的 IndexedDB 事务中核对预期代次。后台 OGG 下载和共享字体升级携带所属安装代次，多个标签页同时导入或更新时，以完整的新安装为准。Web Locks 用于协调并发，持久化代次检查覆盖没有 Web Locks 的浏览器。

更新提示绑定当前 Runtime 实例。新版本完整下载并校验后才提示，关闭或替换实例后，其过期检查不会占用后继实例的通知标识。启动时先补齐必需资源，再固定 DATA 身份。

对应检查：[`test-package-generation-conditions.mjs`](../../launcher/tests/test-package-generation-conditions.mjs)、[`game-preparation.test.mjs`](../../launcher/tests/react-main/game-preparation.test.mjs) 与 [`runtime-service.test.mjs`](../../launcher/tests/react-main/runtime-service.test.mjs)。
