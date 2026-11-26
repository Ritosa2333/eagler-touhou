# 网页单机开发

浏览器版本使用 Emscripten 6.0.9，将既有核心与显示计算编译为 WebAssembly。`src/core` 和 `src/render` 从来源提交原样继承，浏览器平台位于 `src/platform/web`，浏览器 Shell 位于 `web`，构建入口位于 `scripts`。Windows 构建仍使用原有 `tools/build.ps1`。

进度见 [适配清单](../ADAPTATION.md)。数值实现、24 位有效数精度、随机顺序和游戏规则沿用 Windows 基线。Web 地址映射与呈现缓冲的优化方式、差分检查和全部录像回归入口见[性能与兼容验证](PERFORMANCE.md)。

在仓库根运行 `node ports/thkkd/scripts/build-eagler.mjs` 构建 Runtime；`--probe` 构建无绘图回归程序。SDK 默认位置为根目录下被忽略的 `toolchains/emsdk`，也可通过 `EMSDK` 指定，构建入口核对精确版本。源码和输出摘要写入 `build/web`，Runtime 输出写入 `runtime`。

首次 Clang 编译发现 EX 第 15 段 RGB 初始化中 unsigned 到 int 的列表转换诊断；使用 `-Wno-c++11-narrowing` 接受原 MSVC 的同一转换，游戏源码保持原字节。编译固定 `-fno-fast-math -ffp-contract=off`，恢复的整数数值算法原样运行。验证器的 Windows 二进制流设置由 `tests/compat/io.h` 在 WASM 中适配，输入输出仍保持原始字节。

回归命令为 `python ports/thkkd/scripts/verify-wasm.py --archive /path/to/recovery-archive --all`；固定采样保存在本机归档，游戏构建不依赖该归档。验证结果可复用至核心、输入、工具链或验证器发生相关变化时。

## 首次准备与站点构建

在网页仓库根目录安装 Node.js 24.19.0、Bun 1.4.4 和 Python 3.14。将官方 emsdk 放在 `toolchains/emsdk`，执行 `emsdk install 6.0.9` 和 `emsdk activate 6.0.9`，或设置 EMSDK 指向已有的同版本 SDK。编译器 release 标识为 `f04ea239d533260dd1db760dd2d668d5f9a88d6b`。浏览器视频转换器由 Bun 安装并随网站构建；命令行 FFmpeg 只用于本地历史资源对照脚本。

```powershell
bun install --cwd launcher --frozen-lockfile --ignore-scripts
python -m pip install -r ports/thkkd/scripts/web-requirements.txt
powershell -ExecutionPolicy Bypass -File ports/thkkd/scripts/build-site.ps1
node launcher/scripts/serve-static.mjs ports/thkkd/build/site 8174
```

浏览器打开 `http://127.0.0.1:8174/`。构建脚本读取本仓库的源码、字体和公共模块；原版资源由玩家本地导入。其输出集中在本目录 `build` 与 `runtime`。`-SkipCompile` 复用已编译 WASM 并刷新 Shell。`--probe` 的私有原版采样属于维护回归输入，独立于游戏和资源构建。

原版 ZIP 导入由启动器中的 `thkkd-original`、`thkkd-dxa` 和 `thkkd-image` 模块完成。读取默认编码的 DXA v3/v4，按 `assets.metadata.json` 校验运行所需的 327 项内容；原始图片与遮罩生成 PNG，MP3 直接交给浏览器播放，4 段 WMV 在本机转换为 WebM，结果形成 301 项运行资源。`prepare-assets.py` 保留为本机显示对照工具，读取被忽略的 assets。字体与转换依赖见 THIRD_PARTY.md。

## 平台与增强边界

贴图透明混合、原字体参数和全屏 DPI 的实现与验证入口见[网页显示与原作对照](RENDERING.md)。

`WebHost` 使用原服务调用顺序、原索引表、24 位数学、GBK 解码和原生文件语义。原始核心与显示源码相对 `64d48dc` 保持原字节。WebGL2 批量提交相邻同状态图元，资源解码和纹理缓存由平台拥有。视频在所有引用图像的绘图入口更新纹理；默认相机使用 Windows DxLib 后台的实际矩阵。BGM 和音效使用独立增益，视频原素材均无音轨。

标准操作由原输入逻辑录制。触控移动在原移动结果与碰撞之间的独立适配层应用，并用恢复的数学实现量化为 24 位有效数；扩展记录保存各原生 Replay 游标的实际最终坐标。原生停止条件为 `cursor + 1 == count`，播放器按此报告结束。外部原版练习录像入口依据元数据恢复原菜单的练习标志。

扩展格式以 `THKKDR1\0` 开始，其后是 little-endian JSON 长度、版本 `thkkd/extended-replay/1` 的 JSON 和完整原生 `.rpy` 字节。JSON 保存原生流 SHA-256、递增游标及量化后坐标；实际非零无限速移动增加 `processingDrop:100`，并在画面和结算保留标识。原格式不能表示任意角度和连续坐标，所以使用显式扩展。实时输入在工作台回放期间隔离，重播与定位继续使用原生状态机。

呈现层保存逻辑帧的只读绘图命令。角色、敌弹、敌人与特效按槽位、出生时刻和类别识别，在匹配身份与贴图时外推最多一个逻辑帧；出生、销毁、瞬移、切面和池复用使用当前快照，其余图元保持最新已完成逻辑帧。额外画面不调用核心、音效或文件写入。普通运行与 Replay 的播放、暂停及快进共用呈现截止时刻；FPS 在原 HUD 位置显示实际 WebGL 提交频率。原生存档、配置和 Replay 写入句柄关闭后，排队完成格式转换并立即同步 IndexedDB，文件协议显式同步作为导入、退出及备份屏障。60 Hz 限制使用独立截止时刻，避免 144 Hz 屏幕退化到 48 次呈现。

MIDI 不适用：原作配乐为 MP3，网页正常音乐为对应 OGG。语言包不适用：本作提供内置原文及完整字形。原作 Omniverse 和 Practice 继续通过原菜单使用；通用 THPrac 保持现有产品边界。单机质量修复与验收范围见 [质量验收说明](QUALITY.md)。

## 验证与打包

`node --test ports/thkkd/tests/extensions.test.mjs` 运行输入、扩展录像与时钟检查。`python -X utf8 -B ports/thkkd/tests/test_release_source_inventory.py` 检查源码包同时保留核心调度代码并排除网页编译产物。`node tools/ports/check.mjs --game=thkkd --variant=normal` 运行生产公共模块和本作接口检查。浏览器脚本位于 tests，使用 CHROME_PATH 指定 Chromium 系浏览器；默认 Edge 标准安装位置。Runtime 脚本使用 loopback 8173，正式启动器与触控检查使用已打包站点 8174，离线脚本自动创建并关闭 8175。测试串行运行，结果和截图写入 build，公开验收汇总写入网页仓库 archive。质量复验通过 `production-driver.mjs` 驱动正式卡片、启动器、Runtime 与资源，默认采用浏览器标准音频策略和独立临时配置；这些脚本使用 `THKKD_SITE_URL` 指定站点，默认 loopback 8177。`flow-browser.mjs` 检查原生菜单录像保存及立即持久化，`dialogue-browser.mjs --touch` 经过原生 Practice 进入 Boss 对话，`system-browser.mjs` 检查九机体、八个关卡入口、媒体及只读呈现，`audio-lifecycle-browser.mjs` 检查实际音量与退出释放，`playback-cadence-browser.mjs` 检查录像暂停和快进的实际频率，`files-ui-browser.mjs` 检查网站文件工具。`upgrade-offline-browser.mjs` 从旧程序安装状态经过网站更新提示重启，再断开 HTTP 服务验证新程序的离线刷新与用户数据。长录像只从已有授权材料读取，报告保存哈希及状态统计，发布包排除原始录像。

`python -X utf8 -B ports/thkkd/scripts/release-web.py --site ports/thkkd/build/site --output ../exports/thkkd-web-local-test` 从当前源码和指定站点生成网站成品和源码 ZIP。原游戏资源保留在本机，通过网页的原版 ZIP 导入入口安装。每个版本使用新的输出目录，源码暂存自动放入 `build/release-work/<版本>/source`，也可用 `--work-dir` 指定本轮工作目录。打包前逐项核对普通版 Runtime、WASM 与编译输入，交付附源码提交、目录树、产物哈希和逐文件清单。发布目录自带 Node 本地服务器与许可证，双击启动脚本即可运行；退出时关闭服务器命令窗口。

成品中的 `server/node.exe launch.mjs --no-open --port=18174` 用于独立本地验收，默认启动仍为 loopback 8174 并打开浏览器。`cpu-performance-browser.mjs --url=http://127.0.0.1:18174/ --replay=path/to/authorized.rpy` 从正式启动器测量标题、实际游戏开场和授权 EX 录像的相同游标窗口，分别施加浏览器 CPU 1 倍与 4 倍限速。报告区分实际 WebGL 呈现频率、核心逻辑频率和录像输入消费频率，保存单步及呈现间隔分位数，并核对两次 EX 场景摘要。定位和资源安装在 1 倍 CPU 下完成，测量采用默认 60 Hz 呈现限制。
