# th09 开发指南

适配能力见[th09 适配表](../ADAPTATION.md)。以下命令从仓库根目录运行，示例使用 PowerShell。th09 多人入口为对战模式。

暂停菜单的 R 键进入 Retry 流程，按菜单退场时序和当前机体、难度、开局参数重建本局。普通版与对战版使用同一入口，联机时按已确认输入同步处理。验证比较暂停前后的原生局帧，并检查重开后继续推进。

## 构建并在 Launcher 启动

需要 Node.js 22.18 以上、Python 3、Emscripten SDK 6.0.9、`ffmpeg` 和 `ffprobe`。原版目录需含日文 th09 1.50a 的 `th09.exe`、`th09.dat`、`thbgm.dat`。准备独立字体目录，包含 `blend.bin`（131072 字节）、`msgothic.ttc`，以及 `cp932.bin`（131072 字节）或 `codepages.bin`（262144 字节）。`msgothic.ttc` 可从 Windows 字体目录复制，其余私有字体资源来自已有字体资源包。

```powershell
npm --prefix launcher ci --ignore-scripts # 仅首次安装
node ports/th09/web/scripts/prepare-retail-assets.mjs --help
node ports/th09/web/scripts/prepare-retail-assets.mjs 'C:\Games\TH09' --font-root 'C:\local-content\th09-fonts'
node launcher/scripts/prepare-th09-content.mjs --original='C:\Games\TH09' --output='ports/th09/assets-ogg'
$env:EAGLER_TH09_CONTENT_DIR=(Resolve-Path 'ports/th09/assets-ogg').Path
$env:EAGLER_DEVELOPMENT_VANILLA_FONT='C:\local-content\th09-fonts\msgothic.ttc'
$env:EAGLER_DEVELOPMENT_UNICODE_FONT='C:\local-content\fonts\unifont-15.1.05.otf'
$env:EAGLER_DEVELOPMENT_GAMES='th09'
$env:EAGLER_COMMON_ROOT=(Resolve-Path 'common').Path
$env:EAGLER_TOUHOU_NETPLAY_RELAY='ws://127.0.0.1:18142/'
node launcher/scripts/build-runtime.mjs --game=th09 --variant=normal --emsdk='C:\tools\emsdk'
node ports/th09/web/scripts/build-eagler.mjs
node ports/th09/web/scripts/build-eagler.mjs --multiplayer
npm --prefix launcher run build
npm --prefix launcher run dev:workspace
```

资源准备入口为 [`prepare-retail-assets.mjs`](../web/scripts/prepare-retail-assets.mjs) 和 [`prepare-th09-content.mjs`](../../../launcher/scripts/prepare-th09-content.mjs)。准备的游戏内容在 `ports/th09/assets-ogg/`，包括 `th09.data` 和 19 首 `music/*.ogg`。Runtime 内容目录还需 `ports/th09/web/assets/sdl-native/cp932.bin`、`blend.bin`。编译产物在 `ports/th09/web/artifacts/sdl-release/`；两种模式的打包产物在 `ports/th09/web/build-eagler/`、`ports/th09/web/build-eagler-multiplayer/`。Launcher 地址为 <http://127.0.0.1:8130/>。房间联调另开终端：首次运行 `npm --prefix services/relay ci --ignore-scripts`，之后运行 `npm --prefix services/relay start`。

## 源码与验证

游戏逻辑和 SDL 宿主位于 [`web/cpp/`](../web/cpp/)，浏览器运行时位于 [`web/sdl-runtime/`](../web/sdl-runtime/)。原版路径由 [`target.json`](../web/target.json) 指定。

| 验证类别 | 先准备的输入 | 命令与产物 |
| --- | --- | --- |
| WASI 源码模块 | Node.js 22.18 以上；WASI SDK 34.0 安装目录 | `$env:WASI_SDK_ROOT='C:\tools\wasi-sdk-34.0-x86_64-windows'` 后运行 [cpp/build.mjs](../web/scripts/cpp/build.mjs)：`node ports/th09/web/scripts/cpp/build.mjs`。产物：`ports/th09/web/artifacts/cpp/game-core-test.wasm`、`game-core-test-build.json`。 |
| 原版 C++ 对照 | 上表 WASI 模块；日文 1.50a `th09.exe`、`th09.dat`、`thbgm.dat`；`ports/th09/web/reference/native/unicorn-bounded.wasm`；Node 依赖 `@alexaltea/unicorn-js@2.1.4` | 首次运行 `npm --prefix ports/th09/web ci` 和 `npm install --prefix ports/th09/web --no-save --package-lock=false @alexaltea/unicorn-js@2.1.4`；之后运行 `npm --prefix ports/th09/web test`。 |
| 对战回滚组件 | WASI SDK 34.0；仓库 `common/`；Node.js 22.18 以上 | 设置 `$env:WASI_SDK_BIN='C:\tools\wasi-sdk-34.0-x86_64-windows\bin'`、`$env:EAGLER_COMMON_ROOT=(Resolve-Path 'common').Path`，运行 `npm --prefix ports/th09/web run test:rollback`。报告写入 `ports/th09/web/artifacts/multiplayer-tests/`。 |
| Ghidra 源码分析 | th09 原版 EXE；Ghidra 与 JDK 安装目录 | 设置 `$env:TH09_DECOMP_TOOLS='C:\tools\decomp'`，运行 `npm --prefix ports/th09/web run cpp:analyze`。分析产物写入 `ports/th09/web/artifacts/cpp/analysis/`。 |

`build-sdl.mjs` 直接构建宿主时可显式指定 Emscripten SDK：`node ports/th09/web/scripts/build-sdl.mjs --release --emsdk 'C:\tools\emsdk'`。该 SDK 路径须含 `.emscripten` 和 Emscripten 6.0.9 的 `install/` 或 `upstream/` 文件；宿主文件写入 `ports/th09/web/artifacts/sdl-release/`。

## 房间菜单返回

联机诊断同时展示 RTT、波动、输入停顿、当前缺少的远端输入帧与累计修复发送数。输入缺帧和修复活动帮助识别延迟平稳时的输入交付问题，回滚及重模拟计数展示本地恢复工作量。

旁观按当前局保留 Replay 输入，进入下一局后释放已播放的旧局。20 分钟对局与暂停、回放共享同一完整前缀，原生队列可容纳 131072 帧输入。返回房间后，选角同步校验只包含当前菜单状态，蓄力释放队列在进入实际对局后参与校验。

`tests/multiplayer/in-game-selection-browser.mjs --relay --reentry-only` 连续执行五轮返回房间与开局，并比较每轮双方的已确认帧 hash。开发构建的 `tests/multiplayer/menu-hash-isolation-browser.mjs` 验证菜单 hash 与旧世界蓄力队列隔离。Relay 的 `TH09_TIMELINE_STRESS=1 TH09_TIMELINE_STRESS_FRAMES=72000 node services/relay/tests/test-th09-spectator-timeline.mjs` 验证 20 分钟输入量的归档、已有连接和中途加入。

正式 Runtime 导出 `th09_room_return`，结束当前对局、清理旁观状态与待退出标记，回到原生标题菜单。Launcher 保留同一 iframe、Module 和已加载资源，菜单呈现后完成 `return-room` 响应。普通版与多人版共用该生产接口，构建时核对导出存在；连续旁观往返核对菜单持续刷新与再次开局。

## 回滚蓄力与 Replay

### 房间开关与启动参数

回滚对局默认启用本地优先蓄力，房间选项「关闭本地优先的蓄力条进度」默认不勾选。P1 在等待室修改选项后，双方重新准备，开局后规则固定。

房间说明统一展示：「默认启用本地优先蓄力。勾选该选项后，使用原版蓄力逻辑和即时显示。」

房间字段 `disableLocalChargePriority` 默认为 `false`，Launcher 将它作为 `configure.options.netplayDisableLocalChargePriority` 传入 Runtime：

| 参数值 | 回滚对局行为 | 原生配置调用 |
| --- | --- | --- |
| `false`（默认） | 使用本地优先蓄力，双方同步已接受的本机进度 | `th09_local_charge_priority(1)` |
| `true` | 使用原版蓄力逻辑，HUD 即时显示当前进度 | `th09_local_charge_priority(0)` |

Runtime 在 HELLO 前完成配置，并将蓄力规则和批次协议纳入 gameplay ABI，核对双方的规则与交付协议身份。纯延迟模式使用原版蓄力逻辑。

技能切入、停顿和恢复沿用可回滚世界帧推进，恢复工作安排在画面提交之后。历史窗口和资源重建决定确认等待边界。浏览器技能回归记录画面推进间隔、预测位置差异与蓄力 HUD，并以完整确认输入核对原生 Replay、最终世界和画面；`DELAY_CONTROL=1` 同时覆盖输入与控制包延迟，`AUDIT_PREDICTION=1` 单独报告预测画面与确认画面的差异。

调度参考 HiganRTool 的 [input_point 与 post_present](https://github.com/Mako-th/HiganRTool/blob/33d02817d07f48b57954dd160cdfce292aeabf1e/src/core/src/rollback.rs) 和 [tick 边界判定](https://github.com/Mako-th/HiganRTool/blob/33d02817d07f48b57954dd160cdfce292aeabf1e/src/dll/src/rollback_play.rs)。技能停顿继续运行网络输入与回滚时钟；全局切入和咲夜场地时停由世界自身控制对象更新。预测画面差异单独记录，已确认世界与最终回放保持精确核对。`EXPECT_CONTINUOUS=1` 验证固定延迟场景逐次画面推进，`AUDIT_PREDICTION=0` 可复查旧版逐帧确认屏障的表现。

`skill-stop-browser.mjs` 按输入和可靠控制两条通道分别注入交付条件。`INPUT_DROP_EVERY=N` 丢弃每第 N 个输入消息；`CONTROL_STALL_AT` 与 `CONTROL_STALL_TICKS` 设置可靠控制通道开始等待的逻辑 tick 和等待长度，后续控制消息保持发送顺序。报告保留两条通道的发送、丢弃和等待位置，并以采样输入与不可变蓄力记录核对最终世界、录像和画面。

`transport-health-browser.mjs --rtc` 在两个独立浏览器进程中运行共享传输源码，核对选中的直连候选对、实际发送、消息到达和出队计数。`EAGLER_COMMON_ROOT` 选择共享源码目录，`HEALTH_EXPECTED_COMMON_HEAD` 固定其版本，测试同时校验该目录中的已跟踪文件。`--relay` 覆盖 WebSocket 转发路径，TURN 路径由实际 ICE 候选对区分。

### 蓄力同步与旁观

`LocalChargePlanner` 复用 `ShotControl::update` 接受本机进度。`ChargeChannel` 通过共享无序输入通道发送 `ChargeHistory` 中已经接受的结果，批次携带累计接收确认、最早未确认记录与最新记录。待释放技能属于 Player 的回滚状态。资源重建边界使用已确认输入，新世界随后恢复本地优先。

本地优先在首次推进该帧时固定本机接受的蓄力结果，世界恢复后复用同一份记录。已经接受的释放遇到世界停顿时进入队列，在允许更新时执行一次。双方模拟、录像与旁观共同消费这些记录，保持进度、音效事件与技能释放一致。

单份结果保存进度、两个计时器、释放值及事件标记，共 40 字节。`T9CB` 用 24 字节头携带会话、席位、累计确认和记录数量，每条记录为 4 字节帧号与 40 字节结果；一批最多 8 条，共 376 字节。已有 `T9CP`、`T9CA` 解码继续服务单条记录验证。接收账本按帧号容纳乱序和重复，连续接收前沿限制 8 帧预测范围，接收 ACK 参与会话退休屏障。账本中的 512 个记录槽保留尚待确认的网络记录，世界恢复使用独立的回滚历史。

蓄力结果在首次生成后立即发送，后续批次持续携带最早未确认记录；停帧时每 32 ms 继续发送。累计确认停止前进 80 ms 后，通过共享修复通道追加有界可靠副本，受该通道的 32 KiB 排队门槛管理。接收确认随批次合并，尾帧与退出屏障使用相同交付机制。普通按键采样与发送保持原有时点，录像和旁观沿用既有 40 字节记录格式。

旁观帧 `T9SP/2` 在原有键盘与触控数据后附加两份 40 字节蓄力记录，共 126 字节。历史批次、跳转和实时播放使用同一帧记录。关闭本地优先时继续发送 46 字节 `T9SP/1`。

### 录像格式与兼容

本地优先 Replay 使用 `T9CHARGE/1` 尾部扩展，保存十个关卡计数和按原生录像输入帧索引排列的蓄力记录，经 LZSS 压缩并校验，导出文件名为 `.rpyx`。

关闭本地优先后，游戏逻辑和蓄力输入按原版方式记录。不含触控扩展的录像以原版 `.rpy` 导出，含触控轨道的录像以 `.rpyx` 导出。蓄力轨道与触控轨道共存时，`THMOTION` 保持在文件末尾。

原版 `.rpy` 和已有触控 `.rpyx` 按文件中的记录播放，房间蓄力开关不改变回放规则。

### 验证

验证入口：

- `web/tests/multiplayer/local-charge.cpp`：原有射击行为、停顿、自动低速与待释放技能。
- `web/tests/multiplayer/charge-history.cpp`：不可变记录、接收确认、回滚恢复、环形历史、录像编解码和触控轨道共存。
- `web/tests/multiplayer/charge-channel.cpp`：20 分钟记录环复用、无序批次、最早缺帧补发、双向丢包、ACK 丢失、快速通道中断、可靠通道阻塞和拥塞恢复。
- `web/tests/server/spectator-rewind.test.mjs`：当前局回看、下一局释放旧局前缀及蓄力记录逐字节保留。
- `web/tests/multiplayer/local-charge-browser.mjs`：双方咲夜 C2、延迟与抖动下的实际 HUD、独立确认记录模拟、录像和旁观逐帧对照。`CHAINED_SKILLS=1` 测试双方连续技能，`PAUSE_CYCLE=1` 核对暂停记录与原生录像帧的衔接。
- `web/tests/multiplayer/local-charge-compatibility.mjs`：`PC_BUILD` 为当前开发构建，`BASELINE_BUILD` 为保留的修改前开发构建，关闭功能后比较 6000 帧状态及原生录像字节。
- `web/tests/multiplayer/legacy-replay-compatibility.mjs`：`TH09_REPLAY_FILE` 指向九关旧版样例 `th9_udyt22.rpy`，比较完整原生回放与旧触控录像。基线、本地优先开启和关闭三种配置逐帧核对 48 项场景状态及 8 项播放状态。

浏览器验证需要 `BROWSER_EXECUTABLE`、含 `puppeteer-core` 的 `BROWSER_TEST_PACKAGE`、`TH09_FIXTURE_DATA`、`TH09_FIXTURE_ASSETS` 和 `TH09_FIXTURE_MUSIC`。`PC_BUILD` 指向开发构建目录，`RUN_LABEL` 使用新的报告名。测试报告保留 WASM、脚本身份和首个差异。
