# TH11 多人版开发指南

旁观逐帧检查可从指定原生关头运行。`portable/multiplayer/build-spectator-stage.mjs --production --stage=6` 为当前正式游戏对象链接私有观测入口，`spectator-relay-browser.mjs` 对比来源端与独立观众的逐帧游戏状态。`--stage=7` 对应 Extra；驱动记录关卡与难度，并使用对应房间参数。

`--stage=5` 配合浏览器驱动的 `--transition-first --hold-input --playback-only`，覆盖三人原生切入第六关后持续射击、低速与对话快进的独立旁观。`TH11_TEST_UNINTERRUPTED_FRAMES` 设置逐帧比较长度，比较从第六关 Replay 入口后的首个输入开始，首次差异记录具体状态字段。`TH11_TEST_ADMISSION_FRAME` 指定首次旁观入场前来源端已运行的帧数。

背景变形使用独立的呈现随机流。分关 Replay 恢复与连续切关各自运行原生背景时钟，敌机、弹幕和道具始终沿同一战斗随机流演算。

`portable/multiplayer/browser-acceptance.mjs` 使用生产包、显式共用字体与真实通道验收原生旁观、重开和多人 Replay。设置 `TH11_TEST_ARCHIVE`、`TH11_TEST_FONT`、`TH11_TEST_UNICODE_FONT` 后运行，结果记录 `pure-delay`；产品的有界回滚场景使用 `rollback-browser-acceptance.mjs`，两种时序分别记录。

生产包驱动由大厅策略确定预测上限，再以纯延迟模式开局。大厅返回的操作错误直接进入测试诊断，开局参数与当前产品契约共同核对。晚入场旁观的来源端持续演算，观众到达目标帧后统一停止观测，以相同帧的原生状态验证已确认历史。

当前适配记录见[TH11 多人适配表](../ADAPTATION.md)。以下命令从仓库根目录运行，示例使用 PowerShell。

Launcher 手柄动作通过共用浏览器桥同步进入本地原生采样，再写入对应席位的联机输入。原生菜单的扫描码与游戏动作使用同一份映射快照；Launcher 接管时停用 SDL 的重复手柄读取。

联机高刷沿完整原生事务生成连续画面。网络逻辑由 `SimulationCadence` 计账，显示帧由 `PresentationSchedule` 从最后两个完整端点取样。缺输入时画面到达当前端点后保持，纠正过程中保留已有画面；恢复、换关、暂停和 Replay 定位分别重新建立插值对。`limitPresentationTo60` 控制额外呈现。

旁观按当前原生动画归属展示所有玩家与低速判定点，双人、三人重叠时保持相同可见性。玩家视角继续显示本地判定点；回滚和额外绘制沿同一归属表重建。`native-test.mjs --ui` 覆盖两种视角切换与原生低速动画。

使用 `rollback-browser-acceptance.mjs --launcher-root launcher --display-hz 120 --compare-display --replay-oracle`，可在真实双端中比较高刷与 60Hz 限制。驱动统计纯显示绘制，剔除前进和纠正提交，并把共用字体传给实时与独立 Replay 两个入口。源码和结果见[高刷验证记录](records/2026-10-24-display-high-refresh.md#mp-接收与逻辑时钟)。

## 构建并在 Launcher 启动

需要 Node.js 22.18 以上、Python 3、Emscripten SDK 6.0.9 和 `ffmpeg`。原版目录需含 `th11.dat`、`thbgm.dat`；`EAGLER_FONT_ROOT` 指向含 `cp932.bin` 的目录。

```powershell
npm --prefix launcher ci --ignore-scripts # 仅首次安装
node launcher/scripts/prepare-th11-content.mjs --original='C:\Games\TH11' --output='ports/th11/assets-ogg'
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
| WASI 源码模块 | Node.js 22.18 以上；WASI SDK 34.0 安装目录 | 设置 `$env:WASI_SDK_ROOT='C:\tools\wasi-sdk-34.0-x86_64-windows'`，运行 [cpp/build.mjs](../web/scripts/cpp/build.mjs)：`node ports/th11-mp/web/scripts/cpp/build.mjs`。产物：`ports/th11-mp/web/artifacts/cpp/game-core-test.wasm`、`game-core-test-build.json`。 |
| Ghidra 分析输入 | TH11 原版 EXE；Ghidra 12 与 JDK 21 安装目录 | 设置 `$env:TH11_DECOMP_TOOLS='C:\tools\decomp'`，运行 [analyze.mjs](../web/scripts/cpp/analyze.mjs)：`node ports/th11-mp/web/scripts/cpp/analyze.mjs`。产物：`ports/th11-mp/web/artifacts/cpp/analysis/jp/functions/004298c0.asm`。 |
| 原版 C++ 对照 | 上述 WASI 模块与分析文件；`target.json` 指定的 TH11 EXE/DAT；`ports/th11-mp/web/reference/native/unicorn-bounded.wasm`；Windows GDI；Node 依赖 `@alexaltea/unicorn-js@2.1.4`、`koffi@3.2.1` | 首次运行 `npm install --prefix ports/th11-mp/web --no-save --package-lock=false @alexaltea/unicorn-js@2.1.4 koffi@3.2.1`；之后运行 `node --test ports/th11-mp/web/tests/cpp/*.test.mjs`。 |
| 多人运行时检查 | Node.js 22.18 以上；上方已设置的 `WASI_SDK_ROOT` | 运行 `node --test ports/th11-mp/portable/multiplayer/*.test.mjs`。 |

原版文件放在 `ports/th11-mp/[th11] 东方地灵殿 (汉化版+日文版)/`。源码或测试输入变更后，先重建 WASI 模块，再运行对照测试。

## 普通版修改同步

普通版目录历史通过真实 Git 合并接入 MP，来源与接收范围见[普通版与多人版同步](../../../docs/adaptation/port-sync.md)。通用功能、练习源码和专项检查沿普通版更新，MP 保留席位输入、回滚、旁观与多人录像所有权。构建配方继续选择对应模式。

音频采用 1024 帧请求块、2048/3072 帧补音阈值与最多三块补音，诊断和普通输出共用队列策略。并行编译前串行准备 SDL 与字体依赖。
