# TH15 开发指南

构建先串行准备 SDK 中的 SDL 与字体依赖，再并行编译游戏源码；本地对象缓存与 SDK 缓存独立维护。

当前适配记录见[TH15 适配表](../ADAPTATION.md)。以下命令从仓库根目录运行，示例使用 PowerShell。

Launcher 的改键与标准化手柄动作经共用浏览器桥进入原生键盘路径。常显判定点复用本作原生低速动画，在独立绘制状态中按原有层级呈现，开关由 `alwaysHitbox` 控制。

## 构建并在 Launcher 启动

需要 Node.js 22.18 以上、Python 3、Emscripten SDK 6.0.9 和 `ffmpeg`。原版目录需含 `th15.dat`、`thbgm.dat`；`EAGLER_FONT_ROOT` 指向含 `cp932.bin` 的目录。未安装 Emscripten 时可运行 `python ports/th15/tools/download-emscripten.py`。

```powershell
npm --prefix launcher ci --ignore-scripts # 仅首次安装
node launcher/scripts/prepare-th15-content.mjs --original='C:\Games\TH15' --output='ports/th15/assets-ogg'
$env:EAGLER_FONT_ROOT='C:\local-content\th15-fonts'
$env:EAGLER_TH15_CONTENT_DIR=(Resolve-Path 'ports/th15/assets-ogg').Path
$env:EAGLER_DEVELOPMENT_VANILLA_FONT='C:\Windows\Fonts\msgothic.ttc'
$env:EAGLER_DEVELOPMENT_UNICODE_FONT='C:\local-content\fonts\unifont-15.1.05.otf'
$env:EAGLER_DEVELOPMENT_GAMES='th15'
node launcher/scripts/build-runtime.mjs --game=th15 --variant=normal --emsdk='/path/to/emsdk'
node ports/th15/portable/package-eagler.mjs
npm --prefix launcher run build
npm --prefix launcher run dev:workspace
```

普通版启用 `thcrap`、`thprac`。内容在 `ports/th15/assets-ogg/`，含 `th15.data` 和 18 首 `music/*.ogg`；SDL 编译产物在 `ports/th15/web/artifacts/sdl-release/`，Runtime 包在 `ports/th15/build-eagler/`。Launcher 地址为 <http://127.0.0.1:8130/>。

## 源码与验证

游戏逻辑在 [`web/cpp/game/`](../web/cpp/game/)，SDL 宿主在 [`web/cpp/sdl/`](../web/cpp/sdl/)，构建和对照脚本在 [`web/scripts/`](../web/scripts/)。原版路径见 [`target.json`](../web/target.json)。

| 验证类别 | 先准备的输入 | 命令与产物 |
| --- | --- | --- |
| WASI 源码模块 | Node.js 22.18 以上；WASI SDK 34.0 安装目录 | 设置 `$env:WASI_SDK_ROOT='C:\tools\wasi-sdk-34.0-x86_64-windows'`，运行 [cpp/build.mjs](../web/scripts/cpp/build.mjs)：`node ports/th15/web/scripts/cpp/build.mjs`。产物：`ports/th15/web/artifacts/cpp/game-core-test.wasm`、`game-core-test-build.json`。 |
| 本机输入恢复 | 含 `FILE-SHA256.json` 的 TH15 完整开发包 | 运行 [import-local-inputs.py](../tools/import-local-inputs.py)：`python ports/th15/tools/import-local-inputs.py 'D:\TH15-full-development-package'`。脚本按清单校验并恢复原版安装、字体、Node 依赖、参考资源、分析数据和原生采集文件。 |
| 原版存档基线 | 上述本机输入；WASI 模块；`target.json` 指定的 TH15 1.00b `th15.exe` | 运行 [probe-score.mjs](../web/scripts/native/probe-score.mjs)：`node ports/th15/web/scripts/native/probe-score.mjs`。产物位于 `ports/th15/web/artifacts/cpp/analysis/score/`，含 `original-default-scoreth15.dat`、`original-saved-state.bin`。 |
| 原版 C++ 对照套件 | 原版 `th15.exe`、`th15.dat` 和 `replay/th15_ud0186.rpy`；`ports/th15/web/reference/replays/tester-extra-sanae.rpy`、`ports/th15/web/reference/native/tester-extra-sanae-frames.bin`；`ports/th15/web/artifacts/native-process-replay/native-replay-frames.bin`、`ports/th15/web/artifacts/native-process-extra/extra-probe.rpy`、`ports/th15/web/artifacts/native-process-extra/native-extra-frames.bin`；`ports/th15/web/reference/native/unicorn-bounded.wasm`；`ports/th15/web/artifacts/cpp/verification/font-definitions-original.json`；`ports/th15/web/assets/` 中的本机字体 | 将本机 `th15.dat` 复制到 `ports/th15/web/artifacts/thprac-private-assets/th15.dat`：设置 `$dat=(Resolve-Path 'ports/th15/[th15] 东方绀珠传 (汉化版+日文版)/th15.dat').Path`，运行 `New-Item -ItemType Directory -Force 'ports/th15/web/artifacts/thprac-private-assets'` 和 `Copy-Item $dat 'ports/th15/web/artifacts/thprac-private-assets/th15.dat'`。Windows 需 GDI。运行 [manual-image-native.test.mjs](../web/tests/cpp/manual-image-native.test.mjs) 生成 `ports/th15/web/artifacts/cpp/verification/manual-page-0.bin` 至 `ports/th15/web/artifacts/cpp/verification/manual-page-8.bin`，再运行 [test-suite.mjs](../web/scripts/cpp/test-suite.mjs)：`node ports/th15/web/scripts/cpp/test-suite.mjs local-oracle`。结果写入 `ports/th15/web/artifacts/cpp/verification/local-oracle-result.json`。 |

本机输入包由 `import-local-inputs.py` 按 `FILE-SHA256.json` 逐项校验；运行测试时，原版游戏目录位于 `ports/th15/[th15] 东方绀珠传 (汉化版+日文版)/`。字体定义 JSON 和 THPrac 私有 DAT 是完整对照套件的独立输入。
