# TH11 多人版开发指南

`portable/multiplayer/browser-acceptance.mjs` 使用生产包、显式共用字体与真实通道验收原生旁观、重开和多人 Replay。设置 `TH11_TEST_ARCHIVE`、`TH11_TEST_FONT`、`TH11_TEST_UNICODE_FONT` 后运行，结果记录 `pure-delay`；产品的有界回滚场景使用 `rollback-browser-acceptance.mjs`，两种时序分别记录。

当前适配记录见[TH11 多人适配表](../ADAPTATION.md)。以下命令从仓库根目录运行，示例使用 PowerShell。

Launcher 手柄动作通过共用浏览器桥同步进入本地原生采样，再写入对应席位的联机输入。原生菜单的扫描码与游戏动作使用同一份映射快照；Launcher 接管时停用 SDL 的重复手柄读取。

## 构建并在 Launcher 启动

需要 Node.js 22.18 以上、Python 3、Emscripten SDK 6.0.9 和 `ffmpeg`。原版目录需含 `th11.dat`、`thbgm.dat`；`EAGLER_FONT_ROOT` 指向含 `cp932.bin` 的目录。

MP 构建使用[Wasm 写入日志插桩](../../../tools/rollback/README.md)。首次构建执行 `npm --prefix tools/rollback ci --ignore-scripts`。构建器在最终链接后自动插桩并核验标志，源码清单包含插桩器、锁文件与模拟堆分配器；打包时继续核对最终 Wasm 身份。

游戏动态对象及分配器元数据位于独立的 192 MiB 模拟堆，`GameSession` 自身作为精确的外部根范围登记；每个逻辑帧记录 32 字节块的首次写入。`GameScope` 为游戏对象分配内存，`SystemScope` 为字体、图形、网络和长期缓存分配内存。纹理像素与渲染状态继续通过原有事务恢复；暂停、资源更替和关卡退出沿用确认帧边界。音效和 Replay 在确认后发布，纠正过程重新生成对应输出。完整所有权与提交流程见[整堆回滚设计](ROLLBACK.md)，本轮实测环境与结果见 [2026-10-24 验收记录](rollback-20261024.md)，此前结果见 [2026-10-23 记录](rollback-20261023.md)。

运行 `node --test tools/rollback/instrument.test.mjs` 验证真实 Wasm 写入覆盖；设置 `EMSDK` 后运行 `node tools/rollback/memory-test.mjs` 验证未启用后端的符号隔离，以及启用后端的分配器、容器、重复回滚、范围边界和真实 Wasm 陷阱处理。两者与完整游戏浏览器验收分别执行、分别记录。该后端由 MP 构建显式设置 `EAGLER_WRITE_JOURNAL=1`，普通版继续使用自己的源码与构建入口。

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

## 回滚性能验收与诊断

[`rollback-browser-acceptance.mjs`](../portable/multiplayer/rollback-browser-acceptance.mjs) 使用两个独立浏览器进程运行生产包，分别保存原生回调、呈现计数器与模拟前沿的时间记录。正式性能比较关闭下列诊断开关，并保持移动输入覆盖；定位开销时另开诊断运行，使用独立输出目录保留结果。

| 开关 | 诊断用途 |
| --- | --- |
| `--native-profile` | 记录恢复、捕获、Update、Draw、封帧等原生阶段耗时与详细 Draw pass 分项。 |
| `--audio-profile` | 记录音频混音、提交、队列读取及其耗时，增加诊断时钟与读取。 |
| `--cpu-samples` | 启用 CDP CPU 采样，并保存每端的 `.cpuprofile`。 |
| `--diagnostic-stationary` | 使用静止输入辅助定位开销；正式验收保持移动与射击。 |

上述任一开关开启时，回执标记 `scope.diagnosticOnly=true`，`performancePassed` 固定为 `false`。`diagnosticPerformanceThresholdsMet` 仍记录这次诊断运行的数值是否满足原阈值，便于定位问题；它不授予正式性能通过资格。`--profile stable|spike` 选择网络延迟形态，与上述采样开关分别设置。

正式对比固定浏览器版本、GPU 模式、CPU 倍率、输入载体、网络路由、延迟注入位置、延迟与抖动、测量时长和阈值，并记录各自 Runtime 的身份。下面示例使用 1× CPU、Relay、发送端 50±5 ms 注入和 15 秒测量，三个 profiler 开关均未开启；字体与 `TH11_TEST_ARCHIVE` 按本文开头准备。

```powershell
$stamp=Get-Date -Format 'yyyyMMdd-HHmmss'
node ports/th11-mp/portable/multiplayer/rollback-browser-acceptance.mjs `
  --runtime ports/th11-mp/build-eagler-multiplayer `
  --duration 15 --cpu 1 --peer-cpu 1 --route relay `
  --delay-location sender --profile stable --latency-ms 50 --jitter-ms 5 `
  --max-gap-ms 100 --replay-oracle --pixel-oracle `
  --output "ports/th11-mp/artifacts/rollback-smoothness/formal-$stamp"
```

[`rollback-browser-metrics.mjs`](../portable/multiplayer/rollback-browser-metrics.mjs) 要求两端的原生回调最大耗时、整个窗口内呈现与前沿最大停顿都不超过所设阈值，且数据完整有效；上述命令保持 100 ms 门槛。`performancePassed` 与整体 `passed` 分别读取，整体结果还包含原生状态、精确 Replay、完整游戏区域像素、输入与观测覆盖、音频和源码稳定性检查。

原生回调耗时是包含图形提交及可能等待的 wall time；呈现计数器在回调退出时观察，表示提交节奏。测量后的长尾分解使用同一页面时钟的既有时间戳，区分原生执行、shell、观测代码与回调外空档，并关联发送延迟注入计时器的迟到情况；原始停顿与阈值仍完整保留。像素读回和 Replay 对照在性能窗口结束后执行。

[2026-10-23 验收记录](rollback-20261023.md) 保留原有证据与口径：`jr-10` 的原生、Replay、游戏区域像素检查通过，100 ms 长尾性能门禁失败。带阶段采样的历史数值按该记录引用；后续正式性能比较按本节关闭诊断开关产生新回执，分别保留正确性、性能和诊断结论。

[2026-10-24 新鲜回执](rollback-20261024.md)使用修正后的 FIFO 注入器进行同脚本对照：基线通过本次 100 ms 门槛，绘制版本的原生、Replay 与完整游戏区域像素通过，性能门禁失败。原始回执、逻辑推进率、全部旧注入器失败与脚本身份分别保留；该结果没有建立绘制改动的性能提升或稳定通过门槛的结论。
