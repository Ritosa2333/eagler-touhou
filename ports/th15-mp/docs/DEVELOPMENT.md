# TH15 多人版开发指南

构建先串行准备 SDK 中的 SDL 与字体依赖，再并行编译游戏源码；本地对象缓存与 SDK 缓存独立维护。

触屏与桌面默认使用相同的 2 倍渲染倍率，不再按触屏类型自动降低画质。显式指定 `renderScale=1` 时使用 1 倍，`node --test tools/ports/mobile-render-scale.test.mjs` 验证默认值、参数优先级和原生启动接线。

当前适配记录见[TH15 多人适配表](../ADAPTATION.md)。以下命令从仓库根目录运行，示例使用 PowerShell。

桌面生产 Runtime 专项使用 `TH15_TEST_ARCHIVE`、`TH15_TEST_FONT`、`TH15_TEST_UNICODE_FONT` 显式提供 DATA 与共用字体，运行 `node ports/th15-mp/portable/multiplayer/browser-test.mjs --output=/absolute/results`。该场景覆盖纯延迟双人／三人、双人旁观与离线多人 Replay，输入哈希和实际打包清单随结果保存。

旁观从原生 Replay 关头重建时，将应用时钟对齐到连续已确认帧。`portable/multiplayer/spectator-relay-browser.mjs --fixtures` 比较独立入场、回跳、切关和后续 600 帧的角色、敌机、子弹、道具归属及激光状态。

分关旁观与新录像使用玩法身份 22，每关统一初始化幽灵私有随机流。已登记的版本 20 录像按录制时的关卡与护盾规则回放。`node ports/th15-mp/portable/multiplayer/native-test.mjs --ghost-stage-only` 比较两人、三人各席位跨关后的 720 帧幽灵轨迹，使用 `TH15_TEST_ARCHIVE` 与 `TH15_EMSDK` 指定输入。

设置 `TH15_TEST_UNINTERRUPTED_FRAMES=3600`，运行旁观驱动的 `--fixtures --uninterrupted-only`，可连续逐帧比较玩家与旁观场景。私有驱动用双方一致的碰撞保护保持活跃演算，全程按正常速度播放。`TH15_TEST_EVIDENCE_NAME` 指定独立的结果目录。

## 回滚与运行诊断

房间默认启用回滚，提供十二帧纠正窗口，也可手动切换实测纯延迟。帧快照覆盖应用、战斗、动画及实体所有权，确认帧统一提交音频、录像和旁观。FPS 按可见画面的实际进展计量，等输入时保持画面并更新数字。错误报告器支持下载包含堆栈、近期日志、构建身份和运行状态的 JSON。

[实验与压力验证归档](../../../archive/th15-rollback-20261026/th15-rollback-experiment-20261026.md)保存对应构建与验证口径。原生网络协议检查同时覆盖旧录像的已登记构建准入、未知构建拒绝及重编码，护盾检查分别验证版本 20 回放与版本 22 新对局。

回滚性能驱动为 [`ports/th15-mp/rollback-performance.mjs`](../rollback-performance.mjs)。沿用本页的 DATA 与字体环境变量，先设置 `TH15_OUTPUT=artifacts/rollback-performance` 并运行 `node ports/th15-mp/web/scripts/build-sdl-application.mjs --multiplayer --profile`，再运行性能驱动。它默认逐组采样 600 个实际推进帧，覆盖一面、Extra 的双人／三人，以及近满子弹池压力。日志场景使用 4x CPU 限速；强回滚场景使用 2x，每个样本都纠正五帧。

`--require-budget` 要求所有所选场景 P95 小于 16.67ms，且世界、确认输入和画面与完整输入执行一致。`TH15_PROFILE_FRAMES` 指定样本数，`TH15_PROFILE_CASES` 接受含 `stage`、`players`、`depth`、`mismatch`、`cpu` 的 JSON 数组，可另设 `warmup` 与 `dense`。输出记录原始样本、分段耗时、构建和机器身份，见 [性能验证归档](../../../archive/th15-rollback-performance-20261105/README.md)。

高刷新率回归使用 `node --test ports/th15-mp/web/tests/sdl/high-refresh-browser.test.mjs`。`TH15_ORIGINAL_DAT` 指定 DATA，字体沿用上述两个显式输入；`EAGLER_FONT_ROOT` 需含 `cp932.bin`。可用 `TH15_TEST_BUILD=artifacts/rollback-performance` 指定当前开发构建，检查 60／120／144／165／240Hz 的固定逻辑步进与呈现节奏。

键盘时序验证使用 `node ports/th15-mp/portable/multiplayer/input-latency-browser.mjs --samples=60`。多人模块沿用上述开发构建；普通版对照先设置 `TH15_OUTPUT=artifacts/input-latency`，运行 `node ports/th15/web/scripts/build-sdl-application.mjs --profile`。驱动读取单机、双人静态输入和双人／三人 RTC 纠正场景的 DOM 按键到本机移动提交时间，并分别记录事件分发、帧推进和回滚计数。

`--recovery-only` 验证收包纠正保持本地采样前沿，与完整实际输入执行比较世界指纹和确认录像输入，并检查三人暂停边界打断恢复后仍执行到期的新帧。完整测试另覆盖等包唤醒、时钟额度及停止后旧任务失效。`--build`、`--single-build`、`--bridge` 和 `--output` 可指定对照构建与结果目录。联机场景在首次快照之前设置同一碰撞保护，维持各次采样的角色活动状态。

## 构建并在 Launcher 启动

需要 Bun 1.4.4、Node.js 22.18 以上、Python 3、Emscripten SDK 6.0.9 和 `ffmpeg`。原版目录需含 `th15.dat`、`thbgm.dat`；`EAGLER_FONT_ROOT` 指向含 `cp932.bin` 的目录。未安装 Emscripten 时可运行 `python ports/th15-mp/tools/download-emscripten.py`。

```powershell
bun --bun run --cwd launcher install:locked # 仅首次安装
node launcher/scripts/prepare-th15-content.mjs --original='C:\Games\TH15' --output='ports/th15/assets-ogg'
$env:EAGLER_FONT_ROOT='C:\local-content\th15-fonts'
$env:EAGLER_TH15_CONTENT_DIR=(Resolve-Path 'ports/th15/assets-ogg').Path
$env:EAGLER_DEVELOPMENT_VANILLA_FONT='C:\Windows\Fonts\msgothic.ttc'
$env:EAGLER_DEVELOPMENT_UNICODE_FONT='C:\local-content\fonts\unifont-15.1.05.otf'
$env:EAGLER_DEVELOPMENT_GAMES='th15'
$env:EAGLER_TOUHOU_NETPLAY_RELAY='ws://127.0.0.1:18142/'
node launcher/scripts/build-runtime.mjs --game=th15 --variant=multiplayer --emsdk='/path/to/emsdk'
node ports/th15-mp/portable/package-eagler.mjs --multiplayer
bun --bun run --cwd launcher build
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

## 普通版修改同步

普通版目录历史通过真实 Git 合并接入 MP，来源与接收范围见[普通版与多人版同步](../../../docs/adaptation/port-sync.md)。通用功能、练习源码和专项检查沿普通版更新，MP 保留席位输入、回滚、旁观与多人录像所有权。构建配方继续选择对应模式。
