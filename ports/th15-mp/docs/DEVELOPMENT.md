# TH15 多人版开发指南

构建先串行准备 SDK 中的 SDL 与字体依赖，再并行编译游戏源码；本地对象缓存与 SDK 缓存独立维护。

当前适配记录见[TH15 多人适配表](../ADAPTATION.md)。以下命令从仓库根目录运行，示例使用 PowerShell。

桌面生产 Runtime 专项使用 `TH15_TEST_ARCHIVE`、`TH15_TEST_FONT`、`TH15_TEST_UNICODE_FONT` 显式提供 DATA 与共用字体。`node ports/th15-mp/portable/multiplayer/browser-test.mjs --output=/absolute/results` 覆盖纯延迟双人／三人、双人旁观与离线多人 Replay；加上 `--rollback` 切换到 mode 0／2 的双人、三人矩阵，并在人为延迟的真实接收链路上检查状态恢复、物理输入归属和已确认输出。输入哈希、打包清单、回滚诊断与 Replay 随结果保存。

旁观从原生 Replay 关头重建时，将应用时钟对齐到连续已确认帧。`portable/multiplayer/spectator-relay-browser.mjs --fixtures` 比较独立入场、回跳、切关和后续 600 帧的角色、敌机、子弹、道具归属及激光状态。

当前会话使用协议 7、玩法身份 21，每关统一初始化幽灵私有随机流；原生 Replay 读取器保留协议 6、玩法身份 20 的纯延迟记录。`node ports/th15-mp/portable/multiplayer/native-test.mjs --ghost-stage-only` 比较两人、三人各席位跨关后的 720 帧幽灵轨迹，使用 `TH15_TEST_ARCHIVE` 与 `TH15_EMSDK` 指定输入。

设置 `TH15_TEST_UNINTERRUPTED_FRAMES=3600`，运行旁观驱动的 `--fixtures --uninterrupted-only`，可连续逐帧比较玩家与旁观场景。私有驱动用双方一致的碰撞保护保持活跃演算，全程按正常速度播放。`TH15_TEST_EVIDENCE_NAME` 指定独立的结果目录。

## 构建并在 Launcher 启动

需要 Node.js 22.18 以上、Python 3、Emscripten SDK 6.0.9 和 `ffmpeg`。原版目录需含 `th15.dat`、`thbgm.dat`；`EAGLER_FONT_ROOT` 指向含 `cp932.bin` 的目录。未安装 Emscripten 时可运行 `python ports/th15-mp/tools/download-emscripten.py`。

```powershell
npm --prefix launcher ci --ignore-scripts # 仅首次安装
npm --prefix tools/rollback ci --ignore-scripts # 安装 Wasm 写入日志插桩器
node launcher/scripts/prepare-th15-content.mjs --original='C:\Games\TH15' --output='ports/th15/assets-ogg'
$env:EAGLER_FONT_ROOT='C:\local-content\th15-fonts'
$env:EAGLER_TH15_CONTENT_DIR=(Resolve-Path 'ports/th15/assets-ogg').Path
$env:EAGLER_DEVELOPMENT_VANILLA_FONT='C:\Windows\Fonts\msgothic.ttc'
$env:EAGLER_DEVELOPMENT_UNICODE_FONT='C:\local-content\fonts\unifont-15.1.05.otf'
$env:EAGLER_DEVELOPMENT_GAMES='th15'
$env:EAGLER_TOUHOU_NETPLAY_RELAY='ws://127.0.0.1:18142/'
node launcher/scripts/build-runtime.mjs --game=th15 --variant=multiplayer --emsdk='/path/to/emsdk'
node ports/th15-mp/portable/package-eagler.mjs --multiplayer
npm --prefix launcher run build
npm --prefix launcher run dev:workspace
```

多人配方启用 `thcrap`、关闭 `thprac`。普通版与多人版共用 `ports/th15/assets-ogg/` 内容目录，含 `th15.data` 和 18 首 `music/*.ogg`；多人 SDL 产物在 `ports/th15-mp/web/artifacts/sdl-multiplayer/`，Runtime 包在 `ports/th15-mp/build-eagler-multiplayer/`。Launcher 地址为 <http://127.0.0.1:8130/>。房间联调另开终端：首次运行 `npm --prefix services/relay ci --ignore-scripts`，之后运行 `npm --prefix services/relay start`。

## 回滚状态与输出

多人模拟使用公共 `WriteJournal` 后端：384 MiB 的独立模拟堆承载 `ApplicationState`、关卡、ECL、角色、子弹、ANM、Replay 原生状态和分配器元数据。最终链接的 Wasm 经 `tools/rollback/instrument.mjs --native-filter` 处理，每帧首次写入一个 32 字节块时保存旧值；恢复时同时还原对象内容和分配器状态。构建、打包和原生初始化分别校验 `0x52424a31` 插桩标志。

| 原生模式 | 输入与推进 |
| --- | --- |
| `0` | 无启动输入延迟；最多保留 12 帧预测与撤销记录。 |
| `1` | 使用全席位准确输入；保留现有 0–9 帧延迟与测量启动。 |
| `2` | 使用测量后的延迟与 0–2 帧启动预测预留，状态恢复窗口固定为 12 帧。 |

物理采样、模拟游标和提交水位分别维护。迟到输入先恢复对应帧，再从已保存的各席位输入重演；一次回调最多重演 4 帧或使用 4 ms，重演过程中到达的更早差异会重新定位恢复点。射击、低速和短方向保持可参与预测；Bomb、暂停和资源给予动作使用实际输入。方向预测最长保持 3 帧。

连续准确输入对应的模拟状态完成修正后，先释放这段撤销日志，再提交音效、多人 Replay 和旁观数据。音效事件复制到系统堆，按提交顺序播放。文件写入、资源创建与释放、GPU 截图和关卡切换进入已确认边界：此前的预测状态与输出全部处理完成后执行该操作。

图形、音频设备、字体缓存和网络传输保留独立的系统生命周期。CPU 字体图集随模拟恢复，制表位单独记录。每个保留帧引用前一帧的不可变绘制指令，恢复时重建 GPU 合成结果，为迟到的暂停输入提供准确背景。资源在完成绘制后释放时，保存一次完整分辨率的合成结果作为后续恢复基准；正常回滚重放绘制指令，60 Hz 下无需生成插值采样信息。

### 回滚验收入口

协议层可直接使用 C++17 编译器：`node ports/th15-mp/portable/multiplayer/network-native-test.mjs`；`CXX` 指定编译器路径。该检查覆盖 mode 0／2、两人／三人、修正中的再次回滚、不可变采样、提交边界、旧版纯延迟 Replay 和新版本 Replay 元数据。`node ports/th15-mp/portable/multiplayer/config-test.mjs` 检查浏览器配置桥。

生产构建完成后，私有验收入口复用生产对象，仅重新链接一个包含诊断导出的 `BrowserRuntime`，并应用相同的 Wasm 插桩配置：

```powershell
$env:TH15_EMSDK='/path/to/emsdk'
$env:TH15_TEST_ARCHIVE='/path/to/th15.dat'
$env:TH15_TEST_FONT='/path/to/msgothic.ttc'
$env:TH15_TEST_UNICODE_FONT='/path/to/unifont.otf'
$env:EAGLER_FONT_ROOT='/path/to/fonts' # 含 cp932.bin
$env:EAGLER_CHROME_PATH='/path/to/chrome'
node ports/th15-mp/portable/multiplayer/build-rollback.mjs
node ports/th15-mp/portable/multiplayer/rollback-browser.mjs
node ports/th15-mp/portable/multiplayer/browser-test.mjs --rollback --output=/absolute/production-results
```

`rollback-browser.mjs` 将模式 0／2 的两人、三人实例与独立的准确输入纯延迟实例比较，附加三人模式 2 的 2 倍渲染分辨率场景。比较包含 ECL 状态、角色资源与位置、子弹运动程序、道具归属、激光、随机流、完整分辨率像素和 Replay 全席位输入。嵌套回滚之后继续比较 600 帧，并专门触发迟到暂停、资源释放后的画面恢复及 12 帧等待边界。输入身份、逐段状态哈希、截图、Replay 和结果 JSON 位于 `web/artifacts/sdl-multiplayer/rollback/`；`--output` 可指定结果目录。

生产 Runtime 的 `_th15_mp_rollback_status` 和浏览器 `__eaglerNetplayDiagnostics` 提供模拟／采样／提交水位、保留帧数、实际恢复与重演计数、生命周期边界、日志和模拟堆用量。正式包通过 `node ports/th15-mp/portable/multiplayer/module-test.mjs --multiplayer-only` 检查配置与插桩身份。

## 源码与验证

游戏与 SDL 宿主位于 [`web/cpp/`](../web/cpp/)；多人逻辑位于 [`web/cpp/multiplayer/`](../web/cpp/multiplayer/) 和 [`portable/multiplayer/`](../portable/multiplayer/)。原版路径见 [`target.json`](../web/target.json)。

| 验证类别 | 先准备的输入 | 命令与产物 |
| --- | --- | --- |
| WASI 源码模块 | Node.js 22.18 以上；WASI SDK 34.0 安装目录 | 设置 `$env:WASI_SDK_ROOT='C:\tools\wasi-sdk-34.0-x86_64-windows'`，运行 [cpp/build.mjs](../web/scripts/cpp/build.mjs)：`node ports/th15-mp/web/scripts/cpp/build.mjs`。产物：`ports/th15-mp/web/artifacts/cpp/game-core-test.wasm`、`game-core-test-build.json`。 |
| 本机输入恢复 | 含 `FILE-SHA256.json` 的 TH15 完整开发包 | 运行 [import-local-inputs.py](../tools/import-local-inputs.py)：`python ports/th15-mp/tools/import-local-inputs.py 'D:\TH15-full-development-package'`。脚本按清单校验并恢复原版安装、字体、Node 依赖、参考资源、分析数据和原生采集文件。 |
| 原版存档基线 | 上述本机输入；WASI 模块；`target.json` 指定的 TH15 1.00b `th15.exe` | 运行 [probe-score.mjs](../web/scripts/native/probe-score.mjs)：`node ports/th15-mp/web/scripts/native/probe-score.mjs`。产物位于 `ports/th15-mp/web/artifacts/cpp/analysis/score/`，含 `original-default-scoreth15.dat`、`original-saved-state.bin`。 |
| 原版 C++ 对照套件 | 原版 `th15.exe`、`th15.dat` 和 `replay/th15_ud0186.rpy`；`ports/th15-mp/web/reference/replays/tester-extra-sanae.rpy`、`ports/th15-mp/web/reference/native/tester-extra-sanae-frames.bin`；`ports/th15-mp/web/artifacts/native-process-replay/native-replay-frames.bin`、`ports/th15-mp/web/artifacts/native-process-extra/extra-probe.rpy`、`ports/th15-mp/web/artifacts/native-process-extra/native-extra-frames.bin`；`ports/th15-mp/web/reference/native/unicorn-bounded.wasm`；`ports/th15-mp/web/artifacts/cpp/verification/font-definitions-original.json`；`ports/th15-mp/web/assets/` 中的本机字体 | 将本机 `th15.dat` 复制到 `ports/th15-mp/web/artifacts/thprac-private-assets/th15.dat`：设置 `$dat=(Resolve-Path 'ports/th15-mp/[th15] 东方绀珠传 (汉化版+日文版)/th15.dat').Path`，运行 `New-Item -ItemType Directory -Force 'ports/th15-mp/web/artifacts/thprac-private-assets'` 和 `Copy-Item $dat 'ports/th15-mp/web/artifacts/thprac-private-assets/th15.dat'`。Windows 需 GDI。运行 [manual-image-native.test.mjs](../web/tests/cpp/manual-image-native.test.mjs) 生成 `ports/th15-mp/web/artifacts/cpp/verification/manual-page-0.bin` 至 `ports/th15-mp/web/artifacts/cpp/verification/manual-page-8.bin`，再运行 [test-suite.mjs](../web/scripts/cpp/test-suite.mjs)：`node ports/th15-mp/web/scripts/cpp/test-suite.mjs local-oracle`。结果写入 `ports/th15-mp/web/artifacts/cpp/verification/local-oracle-result.json`。 |
| 多人经济规则 | Emscripten SDK 6.0.9 安装目录；Python 3 | 设置 `$env:TH15_EMSDK='C:\tools\emsdk'`，运行 `node ports/th15-mp/portable/multiplayer/economy-check.mjs`。临时 WASM 和检查输出位于 `ports/th15-mp/web/artifacts/multiplayer-economy/`。 |

本机输入包由 `import-local-inputs.py` 按 `FILE-SHA256.json` 逐项校验；原版游戏目录位于 `ports/th15-mp/[th15] 东方绀珠传 (汉化版+日文版)/`。字体定义 JSON 和 THPrac 私有 DAT 是完整对照套件的独立输入。
