# th08 开发指南

当前适配记录见[th08 适配表](../ADAPTATION.md)。命令从仓库根目录运行；示例使用 PowerShell。

## 构建并在 Launcher 启动

需要 Node.js 22.18 以上、Python 3 和 Emscripten SDK 6.0.9。未安装时运行 `python ports/th08/tools/download-emscripten.py`。首次使用安装 Launcher 依赖。准备本机 `th08.dat`；打包字体目录须含 `blend.bin`、`cp932.bin`。Launcher Host 另需可读的日文字体和 Unifont。

```powershell
npm --prefix launcher ci --ignore-scripts # 仅首次安装
$env:EAGLER_FONT_ROOT='C:\local-content\th08-fonts'
$env:EAGLER_TH08_DATA_FILE='C:\Games\th08\th08.dat'
$env:EAGLER_DEVELOPMENT_VANILLA_FONT='C:\Windows\Fonts\msgothic.ttc'
$env:EAGLER_DEVELOPMENT_UNICODE_FONT='C:\local-content\fonts\unifont-15.1.05.otf'
$env:EAGLER_DEVELOPMENT_GAMES='th08'
node launcher/scripts/build-runtime.mjs --game=th08 --variant=normal --emsdk='/path/to/emsdk'
node launcher/scripts/package-runtime.mjs --game=th08 --variant=normal
npm --prefix launcher run build
npm --prefix launcher run dev:workspace
```

根配方的普通版启用 `thcrap` 和 `thprac`。编译产物在 `ports/th08/web/artifacts/sdl3/`；打包 Runtime 在 `ports/th08/build-eagler/`。Host 在 <http://127.0.0.1:8130/>。如需外置 OGG 音乐，准备含 21 首曲目的 `bgm-ogg/`，并将 `EAGLER_TH08_CONTENT_DIR` 设为它的上级目录；不提供 OGG 时可从原版数据使用 MIDI 或关闭音乐。

语言诊断通过默认关闭的 `language_trace_enable_v1(1)` 记录真实公共 lookup、文本栅格、字形来源与成功 Runtime present。记录按准备阶段和正常场景分组，固定容量并以 JSON 暴露截断及溢出状态；`textHex`、`valueHex`、`extraHex` 保存原始 UTF-8 或 CP932 字节。API、字段位与读取顺序见[语言观察接口](../../../docs/adaptation/language-observation.md)。

标题场景的只读观察通过 `core.status(app, 8/9/11)` 分别读取菜单页、光标与本页状态。字段 25 表示当前标题确认输入门槛已开放，字段 26–28 保存本页计时器与原生输入 current／previous。起始页由 `TitleMenuState.hpp` 的生产守卫等待 Ready 且计时器到 10，难度页按自身 Ready 状态确认；音效场景等待字段 25 后发送输入。超时回执保留页、计时与输入边沿，便于区分导航和声音路径。

`touch-motion` 和 `desktop-input` 使用共用[场景适配器](../../../tests/adaptation/adapters/scene.mjs)准备无敌人的输入场景。驱动在标题导航前调用 `validation_scene_arm(app)`，提交一次性请求；请求被 `GameApplication::enter_game()` 消耗时立即锁定会话的持久文件提交。`GameplayScene::load()` 内部的自动分数初始化仍更新 Runtime RAM 覆盖并成功返回，但不调用 `FileDevice::save`；加载作用域之外及非分数写入均拒绝。`validation_scene_write_state_v1(app)` 记录写入尝试、RAM 覆盖次数、丢弃字节、拒绝次数和持久提交字节，受控场景检查提交字节为零。加载完成后，`GameApplication::enter_game()` 在首个 `EnemySimulation::update()` 前核对新世界，并通过 `ValidationSceneGate::begin_before_first_update()` 启动时间线门；弹数来自 `BulletManagerState::live_bullet_count()`，覆盖 1536 个真实分配槽并排除池尾部的分配哨兵。玩家、输入、原生逻辑 Tick 与实体更新继续运行，门只暂停 ECL 时间线执行。`validation_scene_state(app)` 保持既有 16-word ABI：版本、启用、可准备、玩法就绪、模态、Replay、敌人数、弹数、激光数、stage、enemy frame、control frame、暂停时间线次数、原时间线数、assisted、life state。独立的 `validation_scene_arm_state(app)` 记录请求状态、代次、首条时间线是否已抑制、首次更新是否开始及会话保存锁。普通启动、Replay 和练习不接收该门；新世界 reset 恢复原始时间线执行。

请求被消耗后，`BrowserRuntime::validation_scene_used` 保持本会话的测试身份；即使目标路线不符合受控场景资格，隔离也持续到 Runtime 结束。导出的保存与 Replay 导出立即拒绝，`BrowserRuntime::write` 在 `game.load()` 期间仅为自动 `score.dat` 更新 RAM 覆盖并报告成功，其他写入拒绝；退出自动保存与配置写入也不会提交。持久化验收使用新 Runtime 会话。

## Replay consumption boundary

`ReplayStream::play` 成功读取后才递增 `frame` 和 `input_cursor`，每 30 个成功帧读取 timing 并递增 `timing_cursor`；`ReplayPlayback::block_end` 以 payload 与后续 stage/timing offset 确定当前块的真实范围。Replay 检查使用 SHA-256 绑定的原始 v6 文件，由独立 JS 解码器解密、解压并解析所选 stage，计算块长度允许的最大成功读取数，再核对 Runtime 帧数、输入游标、timing 游标、stage、块长度与 stride。该容量描述读取上限，不定义原版自然终点。原版捕获保存完整阶段、结束状态与最终分数，但没有映射到 `ReplayStream` 游标的自然终点值；因此 th08 normal 的 REPLAY-01 结论保持 `incomplete`，直到存在绑定同一 Replay 哈希的独立游标参考。

SDL video/events 子系统归 WebAssembly Runtime 模块所有，并在游戏会话 stop/relaunch 之间保持初始化。每个 Renderer 独立持有 SDL window 与 GL context；释放前先切回所属 context，再删除其 GL 对象，最后销毁 context 和 window。

只读 `status(app, field)` 字段 12–21 提供已加载状态、原生流帧、输入游标和字节数、timing 游标和字节数、stride、游戏 flags、玩法状态与 stage；`eagler_replay_state()` 的 frame/ended 来自正式 Replay 播放状态。未加载 stage 时流字段为 0；不能把原版画面采样行数解释为 Replay 输入总帧。

`TH_PRESENTATION_AUDIT` 构建额外提供 `status` 字段 22–24：经 `BrowserRuntime::write` 发起的游戏文件写入次数、`FileDevice::save` 成功次数与成功字节数。`BrowserRuntime::write` 是本作游戏文件保存入口，壳层导入 Replay 使用资源写入入口，二者分别观察。

## Presentation Lab

工作台使用独立诊断构建，支持冻结、逐逻辑帧推进、固定 alpha 绘制、对象定位、帧图导出和人工视觉记录。以下命令适用于 Windows、Linux 和 macOS 的 Node 环境；先在当前终端设置 `EMSDK`、`EAGLER_FONT_ROOT`、`TH08_LAB_FONT` 和 `TH08_LAB_DATA`。它们分别指向 Emscripten SDK、含 `blend.bin`／`cp932.bin` 的字体目录、本机 MS Gothic 字体和 `th08.dat`。

本地完整录像目录按服务公布的 `campaignAvailable` 加载。安装 `campaign/manifest.json` 后，工作台展示登记录像；准备 Demo 的环境直接使用 Demo 或手动导入入口，不请求未安装的完整录像清单。

```sh
node ports/th08/portable/build.mjs --th08 --presentation-lab
node ports/th08/portable/package-eagler.mjs --presentation-lab
node ports/th08/portable/presentation-lab/serve.mjs
```

打开 <http://127.0.0.1:8132/>，加载 DATA 并启动。先点击“运行”观察实际运动，再冻结并导出帧图。[人工显示审查表](../../../docs/adaptation/presentation.md#人工显示审查表)列出各项操作和预期，[记录模板](../../../docs/adaptation/record-template.md#显示人工记录)汇总本次对象、场景、设备及自动结果。原生插值资格、循环提交和实际显示观感分别记录。人工导出记录绑定原始报告、构建、工具、DATA 与 Replay 身份，结论范围为本次观察窗口。

玩家与附属物的诊断身份取自权威对象；呈现使用的栈副本沿用同一身份，便于在原始逻辑绘制与插值绘制之间比较端点。

```sh
node --test ports/th08/portable/presentation-lab/test-analyzer.mjs common/testkit/presentation-lab/release-contract.test.mjs common/testkit/presentation-lab/review-record.test.mjs
python ports/th08/portable/presentation-lab/verify-browser.py
```

浏览器检查验证工作台操作、真实 WASM 的采样、负对照和导入导出；显示器观感由实际操作人填写。运行报告保存在本地 `ports/th08/artifacts/presentation-lab/evidence/`。

## 源码入口

`web/cpp/` 含游戏和 SDL 宿主；`portable/` 含渲染、数值、输入与浏览器代码；回放适配在 `tools/replay-verifier/`。目录键盘模块由 `eagler-common` 生成：从仓库根目录运行 `node common/tools/sync-directory-keyboard.mjs ports/th08/web/sdl-runtime/directory-keyboard.mjs`。键盘映射与 Shell 接线回归运行 `python launcher/tests/test-keyboard-mapping-browser.py`，在 Chromium 中覆盖全部 15 个 normal/MP Runtime 变体。从 `ports/th08/` 运行 `node portable/check-high-refresh-contract.mjs`；呈现工作台改动再运行 `node portable/check-presentation-purity.mjs`。Launcher 手柄动作在原生取样边界合并进游戏输入；运行 `node launcher/tests/test-gamepad-contract.mjs` 校验全部登记变体与共享 C++ 映射。
