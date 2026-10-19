# th09 开发指南

当前适配记录见[th09 适配表](../ADAPTATION.md)。以下命令从仓库根目录运行，示例使用 PowerShell。th09 多人入口为对战模式。

暂停菜单的 R 键进入本作现有 Retry 流程，保留菜单退场时序，再按当前机体、难度与开局参数重建本局。普通版与对战版使用同一入口，联机时按已确认输入同步处理；运行验收比较暂停前后的原生局帧，并检查重开后继续推进。

Music Room 的未解锁曲目沿用原作五列编号前缀，一位和两位曲目编号使用同一列宽。菜单原版对照逐字节验证曲目列表及注释。

## 构建并在 Launcher 启动

需要 Node.js 22.18 以上、Python 3、Emscripten SDK 6.0.9、`ffmpeg` 和 `ffprobe`。原版目录需含日文 th09 1.50a 的 `th09.exe`、`th09.dat`、`thbgm.dat`。准备独立字体目录，包含 `blend.bin`（131072 字节）、`msgothic.ttc`，以及 `cp932.bin`（131072 字节）或 `codepages.bin`（262144 字节）。`msgothic.ttc` 可从 Windows 字体目录复制；其余私有字体资源来自已有字体资源包。

```powershell
npm --prefix launcher ci --ignore-scripts # 仅首次安装
node ports/th09/web/scripts/prepare-retail-assets.mjs --help
node ports/th09/web/scripts/prepare-retail-assets.mjs 'C:\Games\th09' --font-root 'C:\local-content\th09-fonts'
node launcher/scripts/prepare-th09-content.mjs --original='C:\Games\th09' --output='ports/th09/assets-ogg'
$env:EAGLER_TH09_CONTENT_DIR=(Resolve-Path 'ports/th09/assets-ogg').Path
$env:EAGLER_DEVELOPMENT_VANILLA_FONT='C:\local-content\th09-fonts\msgothic.ttc'
$env:EAGLER_DEVELOPMENT_UNICODE_FONT='C:\local-content\fonts\unifont-15.1.05.otf'
$env:EAGLER_DEVELOPMENT_GAMES='th09'
$env:EAGLER_COMMON_ROOT=(Resolve-Path 'common').Path
$env:EAGLER_TOUHOU_NETPLAY_RELAY='ws://127.0.0.1:18142/'
node launcher/scripts/build-runtime.mjs --game=th09 --variant=normal --emsdk='C:\tools\emsdk'
node launcher/scripts/build-runtime.mjs --game=th09 --variant=multiplayer --emsdk='C:\tools\emsdk'
node launcher/scripts/package-runtime.mjs --game=th09 --variant=normal
node launcher/scripts/package-runtime.mjs --game=th09 --variant=multiplayer
npm --prefix launcher run build
npm --prefix launcher run dev:workspace
```

资源准备入口为 [`prepare-retail-assets.mjs`](../web/scripts/prepare-retail-assets.mjs) 和 [`prepare-th09-content.mjs`](../../../launcher/scripts/prepare-th09-content.mjs)。准备的游戏内容在 `ports/th09/assets-ogg/`，包括 `th09.data` 和 19 首 `music/*.ogg`。Runtime 内容目录还需 `ports/th09/web/assets/sdl-native/cp932.bin`、`blend.bin`。统一 Launcher 构建入口为普通版写入 `ports/th09/web/artifacts/sdl-release/`、MP 写入 `ports/th09/web/artifacts/sdl-multiplayer/`，各自生成 loader、WASM 和 `build.json`；两种模式的打包产物位于 `ports/th09/web/build-eagler/`、`ports/th09/web/build-eagler-multiplayer/`。Launcher 地址为 <http://127.0.0.1:8130/>。房间联调另开终端：首次运行 `npm --prefix services/relay ci --ignore-scripts`，之后运行 `npm --prefix services/relay start`。

## 源码与验证

游戏逻辑和 SDL 宿主位于 [`web/cpp/`](../web/cpp/)，浏览器运行时位于 [`web/sdl-runtime/`](../web/sdl-runtime/)。原版路径由 [`target.json`](../web/target.json) 指定。

| 验证类别 | 先准备的输入 | 命令与产物 |
| --- | --- | --- |
| WASI 源码模块 | Node.js 22.18 以上；WASI SDK 34.0 安装目录 | `$env:WASI_SDK_ROOT='C:\tools\wasi-sdk-34.0-x86_64-windows'` 后运行 [cpp/build.mjs](../web/scripts/cpp/build.mjs)：`node ports/th09/web/scripts/cpp/build.mjs`。产物：`ports/th09/web/artifacts/cpp/game-core-test.wasm`、`game-core-test-build.json`。 |
| 原版 C++ 对照 | 上表 WASI 模块；日文 1.50a `th09.exe`、`th09.dat`、`thbgm.dat`；`ports/th09/web/reference/native/unicorn-bounded.wasm`；Node 依赖 `@alexaltea/unicorn-js@2.1.4` | 首次运行 `npm --prefix ports/th09/web ci` 和 `npm install --prefix ports/th09/web --no-save --package-lock=false @alexaltea/unicorn-js@2.1.4`；之后运行 `npm --prefix ports/th09/web test`。 |
| 资源无关 Node 集成 | Node.js 22.18 以上；首次运行先执行 `npm --prefix ports/th09/web ci --ignore-scripts` 安装已声明的 `ws` | `npm --prefix ports/th09/web run test:server` 执行 managed Runtime 生命周期、触控队列、Relay、Replay、字体输入与观战模块测试；`test:spectator-timeline` 单独执行观战时间线适配器。 |
| 当前 WASM Package 集成 | 已构建的当前 th09 `artifacts/sdl-release/th09.wasm`（或显式设置 `TH09_RELEASE_WASM`） | `npm --prefix ports/th09/web run test:server-package`，在临时目录重建 normal 与 multiplayer Package 并验证存档和 Replay 的持久化隔离。 |
| 发布站点检查 | 已构建的 `artifacts/sdl-release/site/` | `npm --prefix ports/th09/web run test:release-site`，启动发布服务并核对访问控制、WASM、DAT、缓存和范围请求。 |
| 独立外部回放 | 已提供的 th09 campaign `.rpy`；上表原版 EXE、DAT、音频归档与 WASI 模块 | 设置 `$env:TH09_REPLAY_FILE='C:\private-content\th09\campaign.rpy'` 后运行 `node --test ports/th09/web/tests/cpp/external-replay.test.mjs`。 |
| 独立 relay125 窗口证据 | 已冻结的 smoothness-window 原始 JSON 及其独立验收记录 | 设置 `$env:TH09_SMOOTHNESS_WINDOW_RAW='C:\private-evidence\th09\relay125-window.json'` 后运行 `node --test ports/th09/web/tests/multiplayer/smoothness-window.test.mjs`。 |
| 对战回滚组件 | WASI SDK 34.0；仓库 `common/`；Node.js 22.18 以上 | 设置 `$env:WASI_SDK_BIN='C:\tools\wasi-sdk-34.0-x86_64-windows\bin'`、`$env:EAGLER_COMMON_ROOT=(Resolve-Path 'common').Path`，运行 `npm --prefix ports/th09/web run test:rollback`。报告写入 `ports/th09/web/artifacts/multiplayer-tests/`。 |
| Ghidra 源码分析 | th09 原版 EXE；Ghidra 与 JDK 安装目录 | 设置 `$env:TH09_DECOMP_TOOLS='C:\tools\decomp'`，运行 `npm --prefix ports/th09/web run cpp:analyze`。分析产物写入 `ports/th09/web/artifacts/cpp/analysis/`。 |

`build-sdl.mjs` 直接构建宿主时可显式指定 Emscripten SDK：`node ports/th09/web/scripts/build-sdl.mjs --release --emsdk 'C:\tools\emsdk'`。该 SDK 路径须含 `.emscripten` 和 Emscripten 6.0.9 的 `install/` 或 `upstream/` 文件；直接运行时默认写入 `ports/th09/web/artifacts/sdl-release/`，也可用 `TH09_OUTPUT` 指定其他构建目录。统一 Launcher 构建入口会按 normal／MP 配方设置此目录。

原版对照套件通过根 `tools/testing/frozen-oracle.mjs` 执行，核对当前核心、编译输入与完整用例清单。每个选中用例必须执行成功，记录保存在本作 `web/artifacts/cpp/verification/local-oracle-result.json` 及同名日志目录。根 `npm run test:oracles` 统一运行已准备的五个源码目录。

`renderer.test.mjs` 为 DISPLAY-04 提供 CPU 几何证据：9,000 个固定种子的可见输入覆盖正负缩放和 alpha 1–255，并逐字节比较屏幕 X/Y 以外的顶点字段（含 Z）、提交顺序、返回值和渲染状态；flags 0/1/2 与 alpha 0 的独立案例核对不可绘制返回、空提交及四边形不变。像素网格样本保留完整原版字节对照，fractional 位置以字面 XYZ 四角验证，四边触边与越界样本验证裁切。

## 原生只读运行观测

`build-sdl.mjs` 将 `th09_native_observation_v1` 导出到普通版与开发构建。该函数返回 12 个 `uint32` 字段；字段编号、有效位、阶段编码和严格解码器见 [`native-observation-contract.mjs`](../../../tools/ports/native-observation-contract.mjs)。TH09 的 session 帧同时是世界帧与本局重开帧，暂停状态来自 `Application::paused`；输入和坐标读取网络本地 side（单机为 side 0）的游戏玩家状态。输入 capability 只在玩家存活且 `player_state==0` 时声明；`match` 阶段和位置仍按实际状态独立报告。应用帧由宿主 `Application::frames` 单独提供。观测调用只刷新自己的静态结果缓冲区，不改游戏状态。
