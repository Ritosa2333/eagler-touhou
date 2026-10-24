# th15 多人版开发指南

构建先串行准备 SDK 中的 SDL 与字体依赖，再并行编译游戏源码；本地对象缓存与 SDK 缓存独立维护。

当前适配记录见[th15 多人适配表](../ADAPTATION.md)。以下命令从仓库根目录运行，示例使用 PowerShell。

桌面生产 Runtime 专项使用 `TH15_TEST_ARCHIVE`、`TH15_TEST_FONT`、`TH15_TEST_UNICODE_FONT` 显式提供 DATA 与共用字体，运行 `node ports/th15-mp/portable/multiplayer/browser-test.mjs --output=/absolute/results`。该场景覆盖纯延迟双人／三人、双人旁观与离线多人 Replay，输入哈希和实际打包清单随结果保存。

连接中断时 Runtime 显示错误并冻结当前画面，Launcher 保留返回房间操作；原生确认会话结束后，Runtime 继续使用正常完成流程。

运行多人生命周期回归：`node --test ports/th15-mp/web/tests/multiplayer/disconnect.test.mjs`。该测试使用打包布局中的 Runtime bridge 和共享 calibration，校验多人断线提示冻结、原生正常终局收尾和桥接回调重复调用的幂等性。

旁观从原生 Replay 关头重建时，将应用时钟对齐到连续已确认帧。`portable/multiplayer/spectator-relay-browser.mjs --fixtures` 比较独立入场、回跳、切关和后续 600 帧的角色、敌机、子弹、道具归属及激光状态。

分关旁观使用玩法身份 20，每关统一初始化幽灵私有随机流。`node ports/th15-mp/portable/multiplayer/native-test.mjs --ghost-stage-only` 比较两人、三人各席位跨关后的 720 帧幽灵轨迹，使用 `TH15_TEST_ARCHIVE` 与 `TH15_EMSDK` 指定输入。

设置 `TH15_TEST_UNINTERRUPTED_FRAMES=3600`，运行旁观驱动的 `--fixtures --uninterrupted-only`，可连续逐帧比较玩家与旁观场景。私有驱动用双方一致的碰撞保护保持活跃演算，全程按正常速度播放。`TH15_TEST_EVIDENCE_NAME` 指定独立的结果目录。

## 构建并在 Launcher 启动

需要 Node.js 22.18 以上、Python 3、Emscripten SDK 6.0.9 和 `ffmpeg`。原版目录需含 `th15.dat`、`thbgm.dat`；`EAGLER_FONT_ROOT` 指向含 `cp932.bin` 的目录。未安装 Emscripten 时可运行 `python ports/th15-mp/tools/download-emscripten.py`。

```powershell
npm --prefix launcher ci --ignore-scripts # 仅首次安装
node launcher/scripts/prepare-th15-content.mjs --original='C:\Games\th15' --output='ports/th15/assets-ogg'
$env:EAGLER_FONT_ROOT='C:\local-content\th15-fonts'
$env:EAGLER_TH15_CONTENT_DIR=(Resolve-Path 'ports/th15/assets-ogg').Path
$env:EAGLER_DEVELOPMENT_VANILLA_FONT='C:\Windows\Fonts\msgothic.ttc'
$env:EAGLER_DEVELOPMENT_UNICODE_FONT='C:\local-content\fonts\unifont-15.1.05.otf'
$env:EAGLER_DEVELOPMENT_GAMES='th15'
$env:EAGLER_TOUHOU_NETPLAY_RELAY='ws://127.0.0.1:18142/'
node launcher/scripts/build-runtime.mjs --game=th15 --variant=multiplayer --emsdk='/path/to/emsdk'
node launcher/scripts/package-runtime.mjs --game=th15 --variant=multiplayer
npm --prefix launcher run build
npm --prefix launcher run dev:workspace
```

多人配方启用 `thcrap`、关闭 `thprac`。普通版与多人版共用 `ports/th15/assets-ogg/` 内容目录，含 `th15.data` 和 18 首 `music/*.ogg`；多人 SDL 产物在 `ports/th15-mp/web/artifacts/sdl-multiplayer/`，Runtime 包在 `ports/th15-mp/build-eagler-multiplayer/`。Launcher 地址为 <http://127.0.0.1:8130/>。房间联调另开终端：首次运行 `npm --prefix services/relay ci --ignore-scripts`，之后运行 `npm --prefix services/relay start`。

## 源码与验证

游戏与 SDL 宿主位于 [`web/cpp/`](../web/cpp/)；多人逻辑位于 [`web/cpp/multiplayer/`](../web/cpp/multiplayer/) 和 [`portable/multiplayer/`](../portable/multiplayer/)。原版路径见 [`target.json`](../web/target.json)。

| 验证类别 | 先准备的输入 | 命令与产物 |
| --- | --- | --- |
| WASI 源码模块 | Node.js 22.18 以上；WASI SDK 34.0 安装目录 | 设置 `$env:WASI_SDK_ROOT='C:\tools\wasi-sdk-34.0-x86_64-windows'`，运行 `npm --prefix ports/th15-mp/web run cpp:build`。产物：`ports/th15-mp/web/artifacts/cpp/game-core-test.wasm`、`game-core-test-build.json`。 |
| 本机输入恢复 | 含 `FILE-SHA256.json` 的 th15 完整开发包 | 运行 [import-local-inputs.py](../tools/import-local-inputs.py)：`python ports/th15-mp/tools/import-local-inputs.py 'D:\th15-full-development-package'`。脚本按清单校验并恢复原版安装、字体、Node 依赖、参考资源、分析数据和原生采集文件。 |
| 原版存档基线 | 上述本机输入；WASI 模块；`target.json` 指定的 th15 1.00b `th15.exe` | 运行 [probe-score.mjs](../web/scripts/native/probe-score.mjs)：`node ports/th15-mp/web/scripts/native/probe-score.mjs`。产物位于 `ports/th15-mp/web/artifacts/cpp/analysis/score/`，含 `original-default-scoreth15.dat`、`original-saved-state.bin`。 |
| 原版 C++ 对照套件 | 原版 `th15.exe`、`th15.dat` 和 `replay/th15_ud0186.rpy`；`ports/th15-mp/web/reference/replays/tester-extra-sanae.rpy`、`ports/th15-mp/web/reference/native/tester-extra-sanae-frames.bin`；`ports/th15-mp/web/artifacts/native-process-replay/native-replay-frames.bin`、`ports/th15-mp/web/artifacts/native-process-extra/extra-probe.rpy`、`ports/th15-mp/web/artifacts/native-process-extra/native-extra-frames.bin`；`ports/th15-mp/web/reference/native/unicorn-bounded.wasm`；`ports/th15-mp/web/artifacts/cpp/verification/font-definitions-original.json`；`ports/th15-mp/web/assets/` 中的本机字体 | 将本机 `th15.dat` 复制到 `ports/th15-mp/web/artifacts/thprac-private-assets/th15.dat`：设置 `$dat=(Resolve-Path -LiteralPath 'D:\private\th15\th15.dat').Path`，运行 `New-Item -ItemType Directory -Force 'ports/th15-mp/web/artifacts/thprac-private-assets'` 和 `Copy-Item $dat 'ports/th15-mp/web/artifacts/thprac-private-assets/th15.dat'`。Windows 需 GDI。运行 [manual-image-native.test.mjs](../web/tests/cpp/manual-image-native.test.mjs) 生成 `ports/th15-mp/web/artifacts/cpp/verification/manual-page-0.bin` 至 `ports/th15-mp/web/artifacts/cpp/verification/manual-page-8.bin`，再运行 `npm --prefix ports/th15-mp/web run cpp:test`。套件会校验已构建 WASM、源码与测试文件哈希，结果写入 `ports/th15-mp/web/artifacts/cpp/verification/local-oracle-result.json`。 |
| SDL 浏览器场景 | 对应 SDL 构建及本地原版资源 | 运行 `npm --prefix ports/th15-mp/web run test:sdl-browser`。 |
| 多人经济规则 | Emscripten SDK 6.0.9 安装目录；Python 3 | 设置 `$env:TH15_EMSDK='C:\tools\emsdk'`，运行 `node ports/th15-mp/portable/multiplayer/economy-check.mjs`。临时 WASM 和检查输出位于 `ports/th15-mp/web/artifacts/multiplayer-economy/`。 |

本机输入包由 `import-local-inputs.py` 按 `FILE-SHA256.json` 逐项校验；原版游戏目录位于 `ports/th15-mp/[th15] 东方绀珠传 (汉化版+日文版)/`。字体定义 JSON 和 THPrac 私有 DAT 是完整对照套件的独立输入。

原版对照套件通过根 `tools/testing/frozen-oracle.mjs` 执行，核对当前核心、编译输入与完整用例清单。每个选中用例必须执行成功，记录保存在本作 `web/artifacts/cpp/verification/local-oracle-result.json` 及同名日志目录。根 `npm run test:oracles` 统一运行已准备的五个源码目录。TH15 Emscripten 编译使用现有 `EM_CACHE`，未设置时写入仓库 ignored 的 `artifacts/emscripten-test-cache/`。

ANM 字形保留分数屏幕位置以支持连续呈现；popup-render 回归同时验证整数网格字节一致，以及四分之一像素位置与原版最近整数像素网格，正好落在半像素时取偶数的精确关系。

## 原生只读运行观测

SDL MP 导出 `th15_native_observation_v1`。12-word 布局、有效位和阶段编码见 [`native-observation-contract.mjs`](../../../tools/ports/native-observation-contract.mjs)。`ApplicationState::frames` 是应用帧，`progress.stage_frame` 是真实模拟世界帧；网络帧独立报告为 `NetplayRuntime::NextFrame()` 的下一个逻辑帧游标。输入、坐标和 gameplay 阶段资格读取配置的本地 seat `MultiplayerPilot`，不是 seat 0 的共享应用输入。暂停状态来自 `PauseState::screen`；MP 不声明重开帧有效位。原生玩家存活且场景标志允许常规输入时报告 gameplay；重启遮罩、死亡、转场等状态明确报告 transition。观测只读取运行数据。

SDL 还导出按 seat 与执行流隔离的 `th15_touch_motion_trace_v2(seat, execution_kind)`。ABI 使用 8-word header、最多 512 个 16-word native tick row；每行记录 `PlayerMotion::update` 完成位置夹紧后的真实 1/128 固定点坐标、消费目标、速度、倍率与灵敏度。帧来自 `progress.stage_frame`，seat 由 `MultiplayerPilot` 传入；仅本地 seat 的灵敏度可从当前 SDL 配置确认。执行流 0 表示前向模拟，1 表示回放。翻转 Y、非零背景外力或同时启用多个分阶段移动倍率时，会显式标记为不适用于标准触控运动判定。

发布版同时导出独立的 `th15_input_context_v1` 诊断数组，不改变上述 12-word 观测 ABI。其 12 个 `u32` 依次为版本、有效位掩码、ApplicationState 是否初始化、场景是否存在、多人模式、native 本地 seat、pilot 是否存在、该 seat 的 player 是否存在、ghost、该 player 的 `life.state`、原始 `scene_flags`、`messages.active()`。有效位依次为 `APP=0x001`、`SCENE=0x002`、`MODE=0x004`、`SEAT=0x008`、`PILOT=0x010`、`PLAYER=0x020`、`GHOST=0x040`、`LIFE=0x080`、`FLAGS=0x100`、`MESSAGES=0x200`；未观测项为 `0xffffffff`。该函数只读运行上下文。

记录夹具在隔离原版实例中初始化和保存，再用新的记录管理器解码，并核对完整状态，供记录、暂停与结局对照复用。解码前用独立填充值初始化目标，验证未执行解码会被检出。字体定义和字体目录通过本作私有输入收据提供。`EAGLER_ORACLE_INPUT_RECEIPT` 指定收据，登记绝对路径与 SHA-256，执行器在每组运行前后复核输入身份。

## 普通版修改同步

普通版目录历史通过真实 Git 合并接入 MP，来源与接收范围见[普通版与多人版同步](../../../docs/adaptation/port-sync.md)。通用功能、练习源码和专项检查沿普通版更新，MP 保留席位输入、回滚、旁观与多人录像所有权。构建配方继续选择对应模式。
