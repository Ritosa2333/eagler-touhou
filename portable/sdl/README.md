# TH12 SDL3 / GLES 共用应用

参照 TH07 的 SDL 回调入口，桌面与 Web 共用 `SDL_AppInit / SDL_AppIterate / SDL_AppEvent / SDL_AppQuit`。标题、选人、七个关卡、关卡衔接、结算、闲置 Demo、音乐室、设置、成绩、录像和结局由同一套 C++ 前端管理，直接链接所在作品的 `th12_runtime`。

SDL3 负责窗口、输入、音频设备与计时；GLES 3 负责绘制。浏览器编译使用 WebGL 2；桌面可以使用 GLES 3 或 OpenGL 3.3 核心上下文，macOS 请求 OpenGL 4.1。FreeType 负责文字，miniaudio 混合音效，并支持 OGG 流式或整曲解码；原版 `thbgm.dat` 使用 PCM 预读。

## 桌面构建

需要 CMake 3.24+、C/C++20 编译器、Python 3，以及 SDL3、FreeType 开发文件。在 monorepo 根目录运行：

```sh
cmake -S ports/th12 -B artifacts/native/th12 -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DTH12_BUILD_SDL_HOST=ON -DTH12_ENABLE_THPRAC=ON -DTH12_FETCH_SDL_DEPS=ON
cmake --build artifacts/native/th12 --config Release --target touhou12 --parallel
```

`TH12_FETCH_SDL_DEPS=ON` 使用固定源码构建 SDL3 3.4.2 和 FreeType 2.14.1；版本、完整提交与 SHA-256 在 [Host.cmake](Host.cmake) 中。已有源码可通过 `FETCHCONTENT_SOURCE_DIR_TH12_SDL3`、`FETCHCONTENT_SOURCE_DIR_TH12_FREETYPE` 指定。MinGW 默认静态链接运行库，可用 `TH12_STATIC_RUNTIME=OFF` 切换。

```sh
artifacts/native/th12/touhou12 --data-dir /path/to/th12 --save-dir /path/to/saves
```

数据目录使用自有 TH12 1.00b 的 `th12.dat` 和 `th12.exe`。程序从 EXE 读取数据表；`thbgm.dat` 提供原版音乐。也可提供 `msgothic.ttc`、`msmincho.ttc`、`unifont.otf`，或用 `TH12_FONT_FILE` 指定字体。Windows、macOS 与 Linux 的系统字体路径由 `TextRaster.cpp` 统一查找。

`TH12_DATA_ROOT` 也可指定数据目录。桌面默认存档位于 SDL 用户目录 `EaglerTouhou/th12`；`--save-dir` 可指定独立目录。成绩、配置与普通键盘／手柄录像沿用原版格式。

## Web 构建与验证入口

先按[构建指南](../../../../docs/adaptation/build.md#工具链)准备 SDK，并设置 `EMSDK`，在作品目录运行：

```sh
npm run build:eagler
npm start -- --port=8114
```

人工入口为 `http://127.0.0.1:8114/adapter-preview.html?runtime=sdl`，使用现有 `artifacts/eagler-content` 素材、字体和静态语言包。直接访问 `sdl.html` 时，资源来自本地 `site/assets`。构建脚本只编译；打包脚本只产生代码包，不运行游戏或测试。

`build-sdl.mjs` 支持 `--build-dir`、`--generator` 和 `--jobs`。环境变量 `TH12_SDL3_SOURCE`、`TH12_FREETYPE_SOURCE` 指定已解压依赖，`TH12_CMAKE_PREFIX` 指定已安装开发库。构建输出位于 `site/runtime/sdl`，代码包位于 `build-sdl-eagler`。

浏览器边界 `site/game-sdl.mjs` 接启动器协议、键盘映射、触控、资源安装及浏览器存档。`src/sdl-content.mjs` 将既有素材包挂载到虚拟文件系统：关卡 ANM/ECL/MSG 保留原始字节，显示数据表和本地化文字转为有界二进制表。早期素材包缺少的菜单／结局文件可由其无损指令元数据恢复。PNG 语言贴图由 C++ 解码。

存档继续使用 `/savesth12` 的既有浏览器数据库结构，导入导出沿用启动器白名单；临时文件和日志不写入数据库。游戏界面、渲染与逻辑推进由 SDL 回调拥有，浏览器不另建一套游戏循环。

主站登记的 `scripts/build-eagler.mjs` 现在编译并打包 SDL3 前端，标准输出仍为 `build-eagler/th12.html`、`manifest.json` 与 `runtime-files.json`，沿用目录 Runtime 协议、DATA 格式和 `/savesth12` 存档根。`--print-inputs` 提供源码身份；SDK、common、并行任务和练习开关接收主站构建环境。`package-launcher.mjs` 使用默认 Runtime 路径即可组装主站，不需要修改主站页面或消息协议。

`build-sdl-eagler` 为同一应用的检查副本。正式 Runtime 清单登记 `cpp-sdl3-gles`，打包时逐项核对已编译源码，防止源码与 Wasm 不一致。旧网页前端通过 `scripts/build-legacy-eagler.mjs` 构建，源码 `site/index.html` 继续保留。

## 输入、练习与显示

- 方向键移动；Z 射击／确认；X Bomb／取消；Shift 低速；Ctrl 跳过对话；Esc 暂停；Enter 确认；R 重试。
- 桌面 Alt+Enter 全屏。SDL 标准手柄支持热插拔、方向键、左摇杆和游戏内 Key Config。
- `TH12_ENABLE_THPRAC=ON` 将现有 C++ ImGui 练习界面直接绘入同一窗口。Practice 可选分段；Backspace 打开工具菜单，Tab 切换信息，F12 打开高级选项。速度设置作用于逻辑时钟。
- 默认逻辑为 60 Hz。桌面 `--high-refresh`、Web 的高刷新率选项允许额外显示帧；按稳定实体标识对连续位置插值，出生、消失、切关和瞬移保持各自边界，显示数据不回写游戏状态。
- `--always-hitbox` 开启显示用判定点；`--no-thprac`、`--no-music` 分别关闭练习工具和音乐；`--full-music` 将当前 OGG 整曲解码为 PCM 后播放。整曲 PCM 在换曲时释放，单曲上限 256 MiB。

SDL 入口播放原版格式及 `TH12WEB1` 扩展录像，支持暂停、变速和从关卡起点重算定位。触控记录逻辑帧实际生效的固定坐标位移；自定义练习记录开局参数；跨关伤害源使用现有的版本化字段布局。扩展沿用网页版本的 UTF-8 JSON、长度与 CRC32 校验，Web 和桌面使用同一个 C++ 编解码器。普通可表达输入继续写原版键流；无限速实际移动和辅助练习将落率标为 100%，不进入正常排行。

自动 Bomb 保存为生效后的按键。直接改写游戏状态的无敌、无限资源等工具不保存为普通录像。定位期间暂停音乐并丢弃跳过区间的瞬时音效，定位完成保持此前的播放／暂停选择。浏览器音乐选项直接选择同一个 C++ 播放器的流式或整曲解码模式。

## 普通版与多人版

`ports/th12` 与 `ports/th12-mp` 分别链接自己的 C++ 游戏核心，两侧正式 Web 构建均使用 SDL3 回调应用。多人专属的 `Multiplayer.cpp`、`MultiplayerPresentation.cpp` 管理原生多人场景、三席 HUD、所有者透明度及结局后的成绩与录像流程。

```sh
node ports/th12/scripts/build-eagler.mjs
node ports/th12-mp/scripts/build-eagler.mjs
```

普通版输出 `build-eagler/`；多人版输出 `build-eagler-multiplayer/`。多人 `src/multiplayer/Session` 继续拥有输入确认、12 帧回滚、换面屏障与已确认录像。浏览器负责启动器 RPC、WebRTC 和观战控制；SDL 的逻辑回调请求这些边界，不另起 JavaScript 游戏循环。房间等待保持动态标题，开始游戏复用现有实例。

两侧高刷从当前逻辑帧外推，按新增画面统计 FPS；停住的同一世界重复绘制不计为新画面。全屏切换重置显示时钟积压，保持联机帧与回滚历史。THPrac 的只读面板保留触控待定、点击及滑动交还路由。

验证入口为 `scripts/check-sdl-browser.mjs`（普通版）和 `scripts/check-sdl-multiplayer-browser.mjs`（多人版）。`TH12_ASSETS` 指定自行提取的素材目录，`TH12_FONTS` 指定共享字体目录；多人用例通过本机 relay 和真实 RTC 通道核对暂停、观战、Replay 槽位和状态重建，`TH12_INPUT_LATENCY_MS` 设置测试输入延迟。

## 动画与浏览器逐帧开销

ANM 的位置、颜色、缩放和透明度插值以最多三个字段的定长数组保存，指令替换保持原有插入顺序与浮点舍入。嵌套动画复用显示树节点的子列表，逻辑仍按新建在前的次序推进，显示仍按创建顺序提交；立即父节点的缩放、旋转及附着／脱离偏移保持原样。

浏览器按原生实例记住已存在与正在下载的音乐，避免同一曲目逐帧遍历虚拟文件系统。键盘采样直接遍历独立按键持有者，并与手柄方向合并，不为每帧创建键值数组。两侧通过动画输出哈希、分配计数、音乐异步加载和多输入持有检查核对行为，量化记录见[动画存储复用验证](../../../../archive/th12-sdl-animation-storage-20261029/README.md)。

## 平台发行与来源

Switch 分支保留 libnx 数据路径、共享字体及 GLES 支持，构建需要 devkitPro 的 SDL3、FreeType、Mesa 和 libnx。NRO 图标与封装由发行构建提供；`TH12_SWITCH_SMALL_BLOCK_POOL=ON` 可接入 Switch 小块分配器。

前端来源为 [saekaze/th12-switch](https://github.com/saekaze/th12-switch) 的固定提交 `ea1709f0a18b71a493ab48395f15d2f6612fe7d6`，原核心未迁入。SDL3 应用入口和主仓库桥接在此基础上维护。依赖来源见 [source.json](source.json)，文件许可见 [LICENSES.md](LICENSES.md)。源码和 Runtime 包不携带原版游戏、字体、图像、音效或音乐。
