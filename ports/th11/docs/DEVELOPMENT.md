# th11 开发指南

构建先串行准备 SDK 中的 SDL 与字体依赖，再并行编译游戏源码；本地对象缓存与 SDK 缓存独立维护。

当前适配记录见[th11 适配表](../ADAPTATION.md)。以下命令从仓库根目录运行，示例使用 PowerShell。

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
node launcher/scripts/build-runtime.mjs --game=th11 --variant=normal --emsdk='C:\tools\emsdk'
node ports/th11/portable/package-eagler.mjs
npm --prefix launcher run build
npm --prefix launcher run dev:workspace
```

普通版启用 `thcrap`、`thprac`。内容在 `ports/th11/assets-ogg/`，含 `th11.data` 和 `music/`；SDL 编译产物在 `ports/th11/web/artifacts/sdl3/`，Runtime 包在 `ports/th11/build-eagler/`。Launcher 地址为 <http://127.0.0.1:8130/>。

## 源码与验证

游戏和 SDL 宿主位于 [`web/cpp/`](../web/cpp/)；共享渲染、输入和多人代码位于 [`portable/`](../portable/)。原版路径见 [`target.json`](../web/target.json)。

| 验证类别 | 先准备的输入 | 命令与产物 |
| --- | --- | --- |
| WASI 源码模块 | Node.js 22.18 以上；WASI SDK 34.0 安装目录 | 设置 `$env:WASI_SDK_ROOT='C:\tools\wasi-sdk-34.0-x86_64-windows'`，运行 `npm --prefix ports/th11/web run cpp:build`。产物：`ports/th11/web/artifacts/cpp/game-core-test.wasm`、`game-core-test-build.json`。 |
| Ghidra 分析输入 | th11 原版 EXE；Ghidra 12 与 JDK 21 安装目录 | 设置 `$env:TH11_DECOMP_TOOLS='C:\tools\decomp'`，运行 [analyze.mjs](../web/scripts/cpp/analyze.mjs)：`node ports/th11/web/scripts/cpp/analyze.mjs`。产物：`ports/th11/web/artifacts/cpp/analysis/jp/functions/004298c0.asm`。 |
| 原版 C++ 对照 | 上述 WASI 模块与分析文件；`target.json` 指定的 th11 EXE/DAT；`ports/th11/web/reference/native/unicorn-bounded.wasm`；Windows GDI；Node 依赖 `@alexaltea/unicorn-js@2.1.4`、`koffi@3.2.1` | 首次运行 `npm install --prefix ports/th11/web --no-save --package-lock=false @alexaltea/unicorn-js@2.1.4 koffi@3.2.1`；之后运行 `npm --prefix ports/th11/web run cpp:test`。 |
| Browser shell contracts | Node.js 22.18 以上 | 运行 `npm --prefix ports/th11/web run test:browser-contracts`。 |
| Campaign、回放与版本对照 | 私有 `th11.dat`、原版 C++ 测试输入；按场景提供 `TH11_CAMPAIGN_SOAK`、`TH11_EQUIVALENCE_CORE`、`TH11_EXTERNAL_REPLAY`、`TH11_WORLD_REPLAYS`，或成对提供 `TH11_NATIVE_PREFIX` 与 `TH11_PREFIX_CORE` | 在 `web/` 中直接运行对应的 `node --test tests/cpp/campaign-soak.test.mjs`、`core-equivalence.test.mjs`、`campaign-recording.test.mjs`、`external-campaign.test.mjs`、`world-replays.test.mjs` 或 `world-prefix-equivalence.test.mjs`。每个入口检查其选项和文件输入，缺失时以错误结束。 |

原版文件放在 `ports/th11/[th11] 东方地灵殿 (汉化版+日文版)/`，文件名和版本须与 `target.json` 一致。源码或测试输入变更后，先重建 WASI 模块，再运行对照测试。

原版对照套件通过根 `tools/testing/frozen-oracle.mjs` 执行，核对当前核心、编译输入与完整用例清单。每个选中用例必须执行成功，记录保存在本作 `web/artifacts/cpp/verification/local-oracle-result.json` 及同名日志目录。根 `npm run test:oracles` 统一运行已准备的五个源码目录。
