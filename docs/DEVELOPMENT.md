# TH15 多人版开发指南

当前适配记录见[TH15 多人适配表](../ADAPTATION.md)。以下命令从仓库根目录运行，示例使用 PowerShell。

桌面生产 Runtime 专项使用 `TH15_TEST_ARCHIVE`、`TH15_TEST_FONT`、`TH15_TEST_UNICODE_FONT` 显式提供 DATA 与共用字体，运行 `node ports/th15-mp/portable/multiplayer/browser-test.mjs --output=/absolute/results`。该场景覆盖纯延迟双人／三人、双人旁观与离线多人 Replay，输入哈希和实际打包清单随结果保存。

## 构建并在 Launcher 启动

需要 Node.js 22.18 以上、Python 3、Emscripten SDK 6.0.9 和 `ffmpeg`。原版目录需含 `th15.dat`、`thbgm.dat`；`EAGLER_FONT_ROOT` 指向含 `cp932.bin` 的目录。未安装 Emscripten 时可运行 `python ports/th15-mp/tools/download-emscripten.py`。

```powershell
npm --prefix launcher ci --ignore-scripts # 仅首次安装
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
