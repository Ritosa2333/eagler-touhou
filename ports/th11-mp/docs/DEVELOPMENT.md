# th11 多人版开发指南

`portable/multiplayer/browser-acceptance.mjs` 使用生产包、显式共用字体与真实通道验收原生旁观、重开和多人 Replay。设置 `TH11_TEST_ARCHIVE`、`TH11_TEST_FONT`、`TH11_TEST_UNICODE_FONT` 后运行，结果记录 `pure-delay`；产品的有界回滚场景使用 `rollback-browser-acceptance.mjs`，两种时序分别记录。

当前适配记录见[th11 多人适配表](../ADAPTATION.md)。以下命令从仓库根目录运行，示例使用 PowerShell。

Launcher 手柄动作通过共用浏览器桥同步进入本地原生采样，再写入对应席位的联机输入。原生菜单的扫描码与游戏动作使用同一份映射快照；Launcher 接管时停用 SDL 的重复手柄读取。

## 构建并在 Launcher 启动

需要 Node.js 22.18 以上、Python 3、Emscripten SDK 6.0.9 和 `ffmpeg`。原版目录需含 `th11.dat`、`thbgm.dat`；`EAGLER_FONT_ROOT` 指向含 `cp932.bin` 的目录。

```powershell
npm --prefix launcher ci --ignore-scripts # 仅首次安装
node launcher/scripts/prepare-th11-content.mjs --original='C:\Games\th11' --output='ports/th11/assets-ogg'
$env:EAGLER_FONT_ROOT='C:\local-content\th11-fonts'
$env:EAGLER_TH11_CONTENT_DIR=(Resolve-Path 'ports/th11/assets-ogg').Path
$env:EAGLER_DEVELOPMENT_VANILLA_FONT='C:\Windows\Fonts\msgothic.ttc'
$env:EAGLER_DEVELOPMENT_UNICODE_FONT='C:\local-content\fonts\unifont-15.1.05.otf'
$env:EAGLER_DEVELOPMENT_GAMES='th11'
$env:EAGLER_TOUHOU_NETPLAY_RELAY='ws://127.0.0.1:18142/'
node launcher/scripts/build-runtime.mjs --game=th11 --variant=multiplayer --emsdk='C:\tools\emsdk'
node ports/th11-mp/portable/package-eagler.mjs --multiplayer
npm --prefix launcher run build
npm --prefix launcher run dev:workspace
```

多人配方启用 `thcrap`、关闭 `thprac`。普通版与多人版共用 `ports/th11/assets-ogg/` 内容目录，含 `th11.data` 和 `music/`；多人 SDL 产物在 `ports/th11-mp/web/artifacts/multiplayer/`，Runtime 包在 `ports/th11-mp/build-eagler-multiplayer/`。Launcher 地址为 <http://127.0.0.1:8130/>。房间联调另开终端：首次运行 `npm --prefix services/relay ci --ignore-scripts`，之后运行 `npm --prefix services/relay start`。

## 源码与验证

游戏逻辑与多人 ABI 位于 [`web/cpp/`](../web/cpp/)；跨平台多人模块位于 [`portable/multiplayer/`](../portable/multiplayer/)；浏览器入口和 C++ 测试位于 [`web/tests/`](../web/tests/)。原版路径见 [`target.json`](../web/target.json)。

| 验证类别 | 先准备的输入 | 命令与产物 |
| --- | --- | --- |
| WASI 源码模块 | Node.js 22.18 以上；WASI SDK 34.0 安装目录 | 设置 `$env:WASI_SDK_ROOT='C:\tools\wasi-sdk-34.0-x86_64-windows'`，运行 `npm --prefix ports/th11-mp/web run cpp:build`。产物：`ports/th11-mp/web/artifacts/cpp/game-core-test.wasm`、`game-core-test-build.json`。 |
| Ghidra 分析输入 | th11 原版 EXE；Ghidra 12 与 JDK 21 安装目录 | 设置 `$env:TH11_DECOMP_TOOLS='C:\tools\decomp'`，运行 [analyze.mjs](../web/scripts/cpp/analyze.mjs)：`node ports/th11-mp/web/scripts/cpp/analyze.mjs`。产物：`ports/th11-mp/web/artifacts/cpp/analysis/jp/functions/004298c0.asm`。 |
| 原版 C++ 对照 | 上述 WASI 模块与分析文件；`target.json` 指定的 th11 EXE/DAT；`ports/th11-mp/web/reference/native/unicorn-bounded.wasm`；Windows GDI；Node 依赖 `@alexaltea/unicorn-js@2.1.4`、`koffi@3.2.1` | 首次运行 `npm install --prefix ports/th11-mp/web --no-save --package-lock=false @alexaltea/unicorn-js@2.1.4 koffi@3.2.1`；之后运行 `npm --prefix ports/th11-mp/web run cpp:test`。 |
| Browser shell contracts | Node.js 22.18 以上 | 运行 `npm --prefix ports/th11-mp/web run test:browser-contracts`。 |
| 多人运行时 Node 检查 | Node.js 22.18 以上；`EAGLER_COMMON_ROOT` 可选，默认使用仓库 `common/` | 运行 `npm --prefix ports/th11-mp/web run test:multiplayer-runtime`。该命令启用 Node VM 模块支持，检查 managed touch、package isolation、relay timing observer 与 runtime diagnostics。 |
| 多人音效浏览器检查 | Emscripten SDK 6.0.9；已构建的 multiplayer objects；私有 `th11.dat`；Launcher Playwright 与可用 Chromium | 运行 `npm --prefix ports/th11-mp/web run test:multiplayer-audio-output`。设置 `$env:EMSDK` 与 `$env:TH11_TEST_ARCHIVE`；可用 `EAGLER_LAUNCHER_ROOT`、`EAGLER_PLAYWRIGHT_MODULE`、`EAGLER_TEST_CHROMIUM` 选择 Playwright 安装和浏览器。检查链接生产对象并在浏览器中验证 56 路原生音效输出。 |

原版文件放在 `ports/th11-mp/[th11] 东方地灵殿 (汉化版+日文版)/`。源码或测试输入变更后，先重建 WASI 模块，再运行对照测试。

原版对照套件通过根 `tools/testing/frozen-oracle.mjs` 执行，核对当前核心、编译输入与完整用例清单。每个选中用例必须执行成功，记录保存在本作 `web/artifacts/cpp/verification/local-oracle-result.json` 及同名日志目录。根 `npm run test:oracles` 统一运行已准备的五个源码目录。
