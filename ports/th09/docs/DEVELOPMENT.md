# TH09 开发指南

当前适配记录见[TH09 适配表](../ADAPTATION.md)。以下命令从仓库根目录运行，示例使用 PowerShell。TH09 多人入口为对战模式。

## 构建并在 Launcher 启动

需要 Node.js 22.18 以上、Python 3、Emscripten SDK 6.0.9、`ffmpeg` 和 `ffprobe`。原版目录需含日文 TH09 1.50a 的 `th09.exe`、`th09.dat`、`thbgm.dat`。准备独立字体目录，包含 `blend.bin`（131072 字节）、`msgothic.ttc`，以及 `cp932.bin`（131072 字节）或 `codepages.bin`（262144 字节）。`msgothic.ttc` 可从 Windows 字体目录复制；其余私有字体资源来自已有字体资源包。

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
| Ghidra 源码分析 | TH09 原版 EXE；Ghidra 与 JDK 安装目录 | 设置 `$env:TH09_DECOMP_TOOLS='C:\tools\decomp'`，运行 `npm --prefix ports/th09/web run cpp:analyze`。分析产物写入 `ports/th09/web/artifacts/cpp/analysis/`。 |

`build-sdl.mjs` 直接构建宿主时可显式指定 Emscripten SDK：`node ports/th09/web/scripts/build-sdl.mjs --release --emsdk 'C:\tools\emsdk'`。该 SDK 路径须含 `.emscripten` 和 Emscripten 6.0.9 的 `install/` 或 `upstream/` 文件；宿主文件写入 `ports/th09/web/artifacts/sdl-release/`。
