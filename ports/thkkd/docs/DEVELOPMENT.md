# 网页单机开发

本作使用 Emscripten 6.0.9 将 C++ 核心与显示计算编译为 WebAssembly，通过共用启动器接入资源管理、文件工具、输入设置和 Replay 工作台。代码分层见 [架构说明](architecture.md)，项目接口状态见 [适配表](../ADAPTATION.md)。

## 构建与运行

准备 Node.js 24、Bun 1.4.4、Python 3.14 和 Emscripten 6.0.9。SDK 默认位于根目录的 `toolchains/emsdk`，也可通过 EMSDK 指向已有安装。从仓库根目录执行：

```powershell
bun install --cwd launcher --frozen-lockfile --ignore-scripts
python -m pip install -r ports/thkkd/scripts/web-requirements.txt
powershell -ExecutionPolicy Bypass -File ports/thkkd/scripts/build-site.ps1
node launcher/scripts/serve-static.mjs ports/thkkd/build/site 8174
```

打开 `http://127.0.0.1:8174/`。构建结果位于 `build` 和 `runtime`；`build-site.ps1 -SkipCompile` 复用已编译 WASM 并刷新浏览器代码。只构建游戏程序可运行 `node ports/thkkd/scripts/build-eagler.mjs`。编译固定使用 `-fno-fast-math -ffp-contract=off` 和原 24 位有效数规则。

本作按需参与构建，站点脚本显式选择 `thkkd` 并使用 `--test-build=1` 组装验证站点，游戏 Runtime 模式为 `normal`。默认全作品发布流程沿用已有游戏清单。

## 资源与显示

网页导入两份独立 DXA 文件或原版 ZIP。启动器的 `thkkd-original`、`thkkd-dxa` 和 `thkkd-image` 模块读取 DXA v3/v4，按 `launcher/src/launcher/thkkd-original-assets.json` 校验 327 项内容，生成 301 项运行资源。这份清单由启动器统一维护，支持只检出启动器与公共目录的前端构建。原始图片与遮罩生成 PNG，四段 WMV 在本机转为 WebM，MP3 和 WAV 使用原字节。游戏资源保留在本机被忽略的 `assets` 或 `content` 中。

`prepare-assets.py` 用于生成本机显示与性能对照测试的 DATA；正式网站构建使用玩家导入流程。字体与转换依赖见 [THIRD_PARTY.md](../THIRD_PARTY.md)，透明混合、字体参数和 DPI 适配见 [RENDERING.md](RENDERING.md)，批量绘制与性能测量见 [PERFORMANCE.md](PERFORMANCE.md)。

## 输入、存档和 Replay

标准操作使用原输入逻辑；触控移动在原移动结果与碰撞之间应用，并量化为 24 位有效数。扩展录像记录各原生游标上的实际坐标，实际使用无限速移动时记录处理落率标记。

`.rpy` 保持原格式。`.thkkdr` 以 `THKKDR1\0` 开始，随后是 little-endian JSON 长度、版本 `thkkd/extended-replay/1` 的元数据和完整原生录像。元数据包含原生流 SHA-256、递增游标与坐标。播放器按原生 `cursor + 1 == count` 停止条件报告结束，播放、暂停、快进和定位共用原生状态机。

原生存档、配置与录像写入关闭后同步 IndexedDB。游戏文件通过共用启动器导入导出，存档与录像均支持多选。存档批次核对目标文件后统一确认覆盖并重启校验；录像批次在同一个文件会话中预留空闲名称，再写入和刷新工作台。原作 Practice 与 Omniverse 保留在游戏菜单内，MIDI、语言包和通用 THPrac 按产品登记范围显示。

详情页使用启动器的 `thkkd-progress` 解析器读取 `th_kaikeidou01.dat`：前 56 字节是键位与设置，随后为五难度、三角色、三武器、七关的 DWORD 进度。Easy～Lunatic 读取第六面结果，Extra 读取第七面结果，数值 1 为续关通关，2 为不续关通关。读取使用只读事务，页面刷新与原存档写入相互独立。

## 验证

```powershell
node --test ports/thkkd/tests/extensions.test.mjs
bun run --cwd launcher test:repository tests/test-thkkd-original.mjs tests/react-validation-paths.test.mjs
python -X utf8 -B ports/thkkd/tests/test_release_source_inventory.py
python -X utf8 -B ports/thkkd/tests/test_replay_reference.py
python -X utf8 -B ports/thkkd/scripts/test-omniverse-clear.py
node tools/ports/check.mjs --game=thkkd --variant=normal
```

浏览器检查使用 CHROME_PATH 指定 Chromium 系浏览器，默认使用 Edge。`production-driver.mjs` 驱动正式游戏卡片、启动按钮和 Runtime；测试输出写入 `build`，执行记录放入仓库的 `archive`。默认使用临时浏览器配置；需要串行复用已安装资源时，THKKD_TEST_PROFILE 指向专用测试目录。

`original-import-browser.mjs` 用 `--dat=路径 --bgm=路径` 或 `--zip=路径` 检查真实文件选择器、安装、开局和离线复用。`files-ui-browser.mjs` 检查存档与录像导入导出，`flow-browser.mjs` 检查原生录像保存与持久化，`system-browser.mjs` 检查机体、关卡入口和媒体，`dialogue-browser.mjs` 检查对话操作，`playback-cadence-browser.mjs` 检查播放、暂停与快进。

`replay-regression-browser.mjs --url=站点 --replays=本机录像目录 --output=结果目录` 对完整原版录像逐步核对游标、随机数和对象摘要。数学或核心状态调整时，使用 `build-eagler.mjs --probe`、`verify-wasm.py --archive=本机采样目录 --all` 和相关浏览器录像回归。原始采样与录像作为本机验证输入保留。

`verify-wasm.py` 核对 EXE、DXA、录像和原始采样哈希，逐区域重建固定基准并比较候选输出。结果默认写入新的 `build/verification` 目录，也可用 `--output=新目录` 指定；`--case=corpus-05 --inject=1:player:20` 注入候选残机字段差异，用于验证首个差异采样帧、字段与字节偏移的报告。当前基准按实际采样帧比较，原始输入保持固定。

`input-adaptation-browser.mjs` 检查原生触控位移、低速、复活、Bomb 与共用按钮；`progress-browser.mjs` 检查本地通关进度；`package-adaptation-browser.mjs` 检查更新中断、恢复和用户数据；`audio-adaptation-browser.mjs` 检查 MP3 错误与音效时延；`presentation-adaptation-browser.mjs` 检查九机体显示、只读绘制及四种刷新率调度。上述脚本接受 `--url=站点 --profile=专用浏览器目录 --output=结果目录`。`flow-browser.mjs --gamepad` 或 `--free-touch` 检查实际输入录制与原生保存流程，通过 `THKKD_SITE_URL`、`THKKD_TEST_PROFILE`、`THKKD_TEST_OUTPUT` 指定环境。

字体覆盖检查先安装 `tests/font-requirements.txt`，再运行 `python -X utf8 -B ports/thkkd/tests/font-coverage.py --output=ports/thkkd/build/font-coverage.json`。该检查汇集核心文本、显示常量和网页标记，核对随包 WOFF2 的字符映射。

## 本地交付

`python -X utf8 -B ports/thkkd/scripts/release-web.py --site ports/thkkd/build/site --output ports/thkkd/dist/版本目录` 生成源码 ZIP 和本地网站 ZIP，并核对构建身份、文件清单、哈希。双击本地网站目录中的“启动海惠堂.cmd”启动浏览器服务器；它使用 Node 托管相同的网页程序。具体规则见 [packaging.md](packaging.md)。
