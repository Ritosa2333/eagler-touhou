# 东方星莲船网页版与行为还原

[monorepo 适配检查](ADAPTATION.md) · [统一工作区](../../WORKSPACE.md)

原生桌面入口使用 [SDL3 / GLES 3 运行层](portable/sdl/README.md)，链接与网页版相同的当前 C++ 游戏核心。构建目标为 `touhou12`。

本作源码位于 `ports/th12`，在 monorepo 根目录运行 `npm run build:runtime -- --game=th12 --emsdk=工具链目录` 构建普通版。浏览器代码包输出至 `ports/th12/build-eagler/`；内容准备与站点组装见 [Launcher 接入](docs/LAUNCHER_INTEGRATION.md)。

`ports/th12` 的 [Git 子树](../../docs/migration/th12-subtree-20261024.md)继承原 th12 仓库历史；后续作品源码提交可按上游祖先关系独立导出。

TH12 1.00b 的 C++／WebAssembly 网页实现，以及用于核对原版行为的分析工具。
原作游戏、角色、贴图、文字和音乐版权属于上海爱丽丝幻乐团。

本仓库包含还原相关源码、验证工具、必要测试夹具和行为说明。原版 EXE／DAT、
素材、反编译生成物、原版关卡脚本、录像、过程记录、移植参考及本地工具安装包保存在本地忽略目录中。

## 内容

- `src/game/`：独立编译的 C++20 游戏模块；`src/wasm/`：网页版接口。
- `src/` 的其余文件：兼容头文件、输入及网页绘制。
- `site/`：网页版入口、页面和样式；运行资源需在本地从自有原版准备。
- `scripts/`、`tools/replay-verifier/`：构建、资源准备、原版行为分析与严格对照工具。
- `tools/frame-inspector/`：Windows 原版逐帧停帧、画面保存和内部数据对照；使用说明见其 README。
- `tests/`：逻辑测试、测试程序和局部行为夹具，不包含原版游戏安装文件。
- `decompiled/`：本地伪代码、原版指令对照和函数关系索引；生成物由 Git 忽略，准备方式见目录说明。
- `docs/behavior/`：UFO、战斗、动画与界面的行为说明和实现路线。

## 构建与运行

需要 Node.js 22+、Python 和 Emscripten 4.0.22。激活 Emscripten 环境后：

```sh
npm run build
npm start
```

打开 `http://127.0.0.1:8112/`。方向键移动，Z 射击，Shift 低速，X Bomb，Esc 暂停。
默认入口为完整游戏。方向键、Z／X操作菜单；Ctrl快进对话，回放时四倍速；F1打开移植版操作说明。
单关调试、录像拖动定位和逐帧检查保留在 `/preview.html`，原版检查器保留独立入口。
详细的本地资源准备见 [构建说明](docs/BUILD.md)。无原版资源也能编译源码，
但运行游戏、执行原版探针和资产相关测试需要自己的 TH12 1.00b 输入。

```sh
npm run test:portable
```

完整测试需要本地资源及自行生成的录像清单／Oracle 数据；它们不随仓库发布。

原生 C++ 核心与不依赖原版资源的验证：

```sh
cmake -S . -B artifacts/native -DCMAKE_BUILD_TYPE=Release
cmake --build artifacts/native
ctest --test-dir artifacts/native --output-on-failure
```

模块职责和验证范围见 [C++ 结构说明](docs/CPP-RESTORATION.md)。
`decompiled/` 中的生成文件是本地分析材料，不参与构建和提交。

## 渲染性能

可复用顶点缓冲、绘制层整理和贴图状态缓存降低密集弹幕的绘制开销。呈现节奏适配 60 Hz 与高刷新率，游戏逻辑、输入和 Replay 保持原有步进。

关卡结算期间预读下一关脚本、背景、贴图与音乐，在切关边界安装资源，普通游戏与录像持续呈现原生结算和淡入。HI SCORE 读取当前机体、难度及 Practice 关卡的保存记录，以整数分数和独立续关末位显示。网格快照、背景裁剪、动画查找与音效节点复用各自的存储，额外绘制沿既有游戏状态呈现。上述场景的固定构建与验证范围见 [2026-10-25 维护记录](../../archive/th12-maintenance-20261025/th12-switch-fixes-20261025.md)。

[2026-10-24 性能测量](../../docs/history/th12-performance-20261024.md) 收录 4× CPU 关卡帧率、绘制基准、一致性结果与复测方法。

## 还原边界

`eagler` 是 Eagler Touhou Web 适配的集成分支；接入架构、包边界及能力目录见 [Web 适配](docs/WEB_ADAPTATION.md)。
使用真实 Launcher 的本地构建、资源包与启动入口见 [Launcher 接入](docs/LAUNCHER_INTEGRATION.md)。

`main` 保留建立 `eagler` 前的已提交版本，后续 Web 适配改动在 `eagler` 集成。
`experiments` 保留已确认的还原基线；`eagler` 包含完整游戏流程：原版标题与选择菜单、
六面连续游玩、Extra、Practice、Replay、成绩与符卡履历、音乐室、设置、闲置 Demo、
暂停／续关／重试、十二种结局及staff、名字与录像保存。
浏览器设置／成绩使用移植版存档结构；`.rpy` 导出按原版加载器的加载、解密和解压路径验证。
整局跨关、菜单与结局的画面及时序以原版逐帧对照为准；短流程测试验证对应场景的状态和输出。
实现、原版依据及验证范围见 [完整游戏流程](docs/FULL_GAME_FLOW.md)。
自动反编译结果不是原作者源码；寄存器入参、类型、函数边界和浮点表达式需要结合
指令与原版执行核对。网页核心是手写的行为实现，不在浏览器中执行原版 EXE。

反编译资料的本地准备见 [decompiled/README.md](decompiled/README.md)。
核心飞碟规则见 [UFO 行为说明](docs/behavior/ufo.md)。

## 版权与许可

仓库已有的 MIT 许可适用于本项目手写的移植代码和工具；
不授予原版游戏、素材或原版程序衍生反编译内容的许可。
这些内容的权利仍属于原权利人。范围见 [NOTICE.md](NOTICE.md)。
