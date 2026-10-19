# th15 开发指南

构建先串行准备 SDK 中的 SDL 与字体依赖，再并行编译游戏源码；本地对象缓存与 SDK 缓存独立维护。

当前适配记录见[th15 适配表](../ADAPTATION.md)。以下命令从仓库根目录运行，示例使用 PowerShell。

Launcher 的改键与标准化手柄动作经共用浏览器桥进入原生键盘路径。常显判定点复用本作原生低速动画，在独立绘制状态中按原有层级呈现，开关由 `alwaysHitbox` 控制。

## 构建并在 Launcher 启动

需要 Node.js 22.18 以上、Python 3、Emscripten SDK 6.0.9 和 `ffmpeg`。原版目录需含 `th15.dat`、`thbgm.dat`；`EAGLER_FONT_ROOT` 指向含 `cp932.bin` 的目录。未安装 Emscripten 时可运行 `python ports/th15/tools/download-emscripten.py`。

```powershell
npm --prefix launcher ci --ignore-scripts # 仅首次安装
node launcher/scripts/prepare-th15-content.mjs --original='C:\Games\th15' --output='ports/th15/assets-ogg'
$env:EAGLER_FONT_ROOT='C:\local-content\th15-fonts'
$env:EAGLER_TH15_CONTENT_DIR=(Resolve-Path 'ports/th15/assets-ogg').Path
$env:EAGLER_DEVELOPMENT_VANILLA_FONT='C:\Windows\Fonts\msgothic.ttc'
$env:EAGLER_DEVELOPMENT_UNICODE_FONT='C:\local-content\fonts\unifont-15.1.05.otf'
$env:EAGLER_DEVELOPMENT_GAMES='th15'
node launcher/scripts/build-runtime.mjs --game=th15 --variant=normal --emsdk='/path/to/emsdk'
node launcher/scripts/package-runtime.mjs --game=th15 --variant=normal
npm --prefix launcher run build
npm --prefix launcher run dev:workspace
```

普通版启用 `thcrap`、`thprac`。内容在 `ports/th15/assets-ogg/`，含 `th15.data` 和 18 首 `music/*.ogg`；SDL 编译产物在 `ports/th15/web/artifacts/sdl-release/`，Runtime 包在 `ports/th15/build-eagler/`。Launcher 地址为 <http://127.0.0.1:8130/>。

## 源码与验证

游戏逻辑在 [`web/cpp/game/`](../web/cpp/game/)，SDL 宿主在 [`web/cpp/sdl/`](../web/cpp/sdl/)，构建和对照脚本在 [`web/scripts/`](../web/scripts/)。原版路径见 [`target.json`](../web/target.json)。

| 验证类别 | 先准备的输入 | 命令与产物 |
| --- | --- | --- |
| WASI 源码模块 | Node.js 22.18 以上；WASI SDK 34.0 安装目录 | 设置 `$env:WASI_SDK_ROOT='C:\tools\wasi-sdk-34.0-x86_64-windows'`，运行 `npm --prefix ports/th15/web run cpp:build`。产物：`ports/th15/web/artifacts/cpp/game-core-test.wasm`、`game-core-test-build.json`。 |
| 本机输入恢复 | 含 `FILE-SHA256.json` 的 th15 完整开发包 | 运行 [import-local-inputs.py](../tools/import-local-inputs.py)：`python ports/th15/tools/import-local-inputs.py 'D:\th15-full-development-package'`。脚本按清单校验并恢复原版安装、字体、Node 依赖、参考资源、分析数据和原生采集文件。 |
| 原版存档基线 | 上述本机输入；WASI 模块；`target.json` 指定的 th15 1.00b `th15.exe` | 运行 [probe-score.mjs](../web/scripts/native/probe-score.mjs)：`node ports/th15/web/scripts/native/probe-score.mjs`。产物位于 `ports/th15/web/artifacts/cpp/analysis/score/`，含 `original-default-scoreth15.dat`、`original-saved-state.bin`。 |
| 原版 C++ 对照套件 | 原版 `th15.exe`、`th15.dat` 和 `replay/th15_ud0186.rpy`；`ports/th15/web/reference/replays/tester-extra-sanae.rpy`、`ports/th15/web/reference/native/tester-extra-sanae-frames.bin`；`ports/th15/web/artifacts/native-process-replay/native-replay-frames.bin`、`ports/th15/web/artifacts/native-process-extra/extra-probe.rpy`、`ports/th15/web/artifacts/native-process-extra/native-extra-frames.bin`；`ports/th15/web/reference/native/unicorn-bounded.wasm`；`ports/th15/web/artifacts/cpp/verification/font-definitions-original.json`；`ports/th15/web/assets/` 中的本机字体 | 将本机 `th15.dat` 复制到 `ports/th15/web/artifacts/thprac-private-assets/th15.dat`：设置 `$dat=(Resolve-Path -LiteralPath 'D:\private\th15\th15.dat').Path`，运行 `New-Item -ItemType Directory -Force 'ports/th15/web/artifacts/thprac-private-assets'` 和 `Copy-Item $dat 'ports/th15/web/artifacts/thprac-private-assets/th15.dat'`。Windows 需 GDI。运行 [manual-image-native.test.mjs](../web/tests/cpp/manual-image-native.test.mjs) 生成 `ports/th15/web/artifacts/cpp/verification/manual-page-0.bin` 至 `ports/th15/web/artifacts/cpp/verification/manual-page-8.bin`，再运行 `npm --prefix ports/th15/web run cpp:test`。套件会校验已构建 WASM、源码与测试文件哈希，结果写入 `ports/th15/web/artifacts/cpp/verification/local-oracle-result.json`。 |
| SDL 浏览器场景 | 对应 SDL 构建及本地原版资源 | 运行 `npm --prefix ports/th15/web run test:sdl-browser`。 |
| THcrap 实装站点浏览器入口 | 含 `index.html` 与准备好的 th15 thcrap 资源包的完整 assembled site；Playwright 与 Chromium | 设置 `$env:TH15_HOST_SITE='D:\path\to\assembled-site'` 后运行 `node --test ports/th15/web/tests/sdl/thcrap-launcher-browser.test.mjs`。该入口逐项验证日文、简中、英文启动与字体安装。 |
| 原版过程回放对照 | 原版 th15 replay `th15_ud0186.rpy`、对应 `native-replay-frames.bin`，以及 `extra-probe.rpy` 与 `native-extra-frames.bin` | 在完整 C++ 对照套件中运行时，两组外部过程回放都要求上述私有输入文件存在并匹配记录身份；缺失的文件会直接报错。 |

本机输入包由 `import-local-inputs.py` 按 `FILE-SHA256.json` 逐项校验；运行测试时，原版游戏目录位于 `ports/th15/[th15] 东方绀珠传 (汉化版+日文版)/`。字体定义 JSON 和 THPrac 私有 DAT 是完整对照套件的独立输入。

原版对照套件通过根 `tools/testing/frozen-oracle.mjs` 执行，核对当前核心、编译输入与完整用例清单。每个选中用例必须执行成功，记录保存在本作 `web/artifacts/cpp/verification/local-oracle-result.json` 及同名日志目录。根 `npm run test:oracles` 统一运行已准备的五个源码目录。TH15 Emscripten 编译使用现有 `EM_CACHE`，未设置时写入仓库 ignored 的 `artifacts/emscripten-test-cache/`。

ANM 字形保留分数屏幕位置以支持连续呈现；popup-render 回归同时验证整数网格字节一致，以及四分之一像素位置与原版最近整数像素网格，正好落在半像素时取偶数的精确关系。

## 原生只读运行观测

SDL 普通版导出 `th15_native_observation_v1`。12-word 布局、有效位和阶段编码见 [`native-observation-contract.mjs`](../../../tools/ports/native-observation-contract.mjs)。`ApplicationState::frames` 是应用帧，`progress.stage_frame` 是实际模拟世界帧。Legacy 与 Practice 的 R 操作重建所选阶段；Pointdevice 的 R 操作恢复最近可用的章节快照，因此其 `stage_frame` 回到该快照记录的帧，不承诺归零。仅在非 replay 且当前重开路径有效时声明 restart frame：Legacy/Practice 可用时直接报告，Pointdevice 还要求 `scene()->checkpoint.ready()`。`progress.restart_frames` 只表示重启遮罩进度，不作重开帧。输入读取 `GameBattle::active_frame().held`，坐标读取当前玩家，暂停状态读取 `PauseState::screen`。阶段为 gameplay 时，原生玩家存活且场景标志允许常规输入；重启遮罩、死亡、转场等状态明确报告 transition。观测只读取运行数据。

发布版同时导出独立的 `th15_input_context_v1` 诊断数组，不改变上述 12-word 观测 ABI。其 12 个 `u32` 依次为版本、有效位掩码、ApplicationState 是否初始化、场景是否存在、多人模式、native 本地 seat、pilot 是否存在、该 seat 的 player 是否存在、ghost、该 player 的 `life.state`、原始 `scene_flags`、`messages.active()`。有效位依次为 `APP=0x001`、`SCENE=0x002`、`MODE=0x004`、`SEAT=0x008`、`PILOT=0x010`、`PLAYER=0x020`、`GHOST=0x040`、`LIFE=0x080`、`FLAGS=0x100`、`MESSAGES=0x200`；未观测项为 `0xffffffff`。该函数只读运行上下文。

记录夹具在隔离原版实例中初始化和保存，再用新的记录管理器解码，并核对完整状态，供记录、暂停与结局对照复用。解码前用独立填充值初始化目标，验证未执行解码会被检出。字体定义和字体目录通过本作私有输入收据提供。`EAGLER_ORACLE_INPUT_RECEIPT` 指定收据，登记绝对路径与 SHA-256，执行器在每组运行前后复核输入身份。
