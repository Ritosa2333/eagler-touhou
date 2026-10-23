# TH07 开发指南

当前适配记录见[TH07 适配表](../ADAPTATION.md)。命令从仓库根目录运行；示例使用 PowerShell。

## 构建并在 Launcher 启动

需要 Node.js 22.18 以上、Python 3、CMake 3.20 以上、Ninja 和 Emscripten SDK 6.0.9（示例路径为 `C:\tools\emsdk`，命令中的 `--emsdk` 指向 SDK 根目录）。首次使用先初始化 SDL 子模块，并安装 Launcher 依赖。`--assets` 指向含本机 `th07.dat` 的目录；档案会编入 `.data`。默认音乐配置为 `BASE`，Host 音乐另行提供。

```powershell
node launcher/scripts/print-runtime-submodule-init.mjs --game=th07
# 按输出命令分层初始化本作构建依赖
npm --prefix launcher ci --ignore-scripts # 仅首次安装
$env:EAGLER_DEVELOPMENT_VANILLA_FONT='C:\Windows\Fonts\msgothic.ttc'
$env:EAGLER_DEVELOPMENT_UNICODE_FONT='C:\local-content\fonts\unifont-15.1.05.otf'
$env:EAGLER_DEVELOPMENT_GAMES='th07'
$env:EAGLER_TOUHOU_NETPLAY_RELAY='ws://127.0.0.1:18142/'
node launcher/scripts/build-runtime.mjs --game=th07 --variant=normal --emsdk='C:\tools\emsdk' --assets='C:\Games\TH07'
node launcher/scripts/build-runtime.mjs --game=th07 --variant=multiplayer --emsdk='C:\tools\emsdk' --assets='C:\Games\TH07'
node launcher/scripts/package-runtime.mjs --game=th07 --variant=normal
node launcher/scripts/package-runtime.mjs --game=th07 --variant=multiplayer
npm --prefix launcher run build
npm --prefix launcher run dev:workspace
```

资源目录至少有 `th07.dat`。默认 `thcrap`、`thprac` 关闭。代码 Runtime 输出在 `ports/th07/build-eagler/normal/` 和 `ports/th07/build-eagler/multiplayer/`；游戏 DATA 与预加载 JS 使用 `ports/th07/build/web/` 对应模式目录。统一打包入口生成包凭据，供 Launcher 来源验收读取。Host 在 <http://127.0.0.1:8130/>。多人房间需另开终端启动 Relay：首次运行 `npm --prefix services/relay ci --ignore-scripts`，之后运行 `npm --prefix services/relay start`。需要游戏配乐时，将原版 `thbgm.dat` 放入 `ports/th07/assets/`，或将已准备的曲目放入 `ports/th07/assets-ogg/bgm-ogg/`。

Vendored SDL_image、libpng 与 zlib 会按 tracked 输入复制到各模式的 `build/web/<模式>/eagler-staging/sdl-image/<SHA-256>/`。zlib `zconf.h` 保留为源码输入，构建目录生成配置后的头文件。

## 源码与验证

游戏逻辑在 `src/`，网络和多人代码在 `src/netplay/`、`src/multiplayer/`，浏览器适配在 `portable/`。核心检查从 `ports/th07/` 运行：`bash scripts/run-core-tests.sh`。`CMakePresets.json` 中的 CI 预设使用外置资源执行编译检查；本地含资源的 Runtime 构建按本页上方步骤运行。

Runtime 提供 `th07_native_observation_v1()` 只读观测接口。它结合 `Supervisor.curState` 与 `GameManager.notInMenu` 区分标题、可操作游戏、暂停、结算、结尾和转场，以 `g_GameManager.framesThisStage` 提供本阶段模拟帧；暂停/重试时该帧保持不变。普通构建读取当前已消费的 `g_CurFrameGameInput`，多人构建读取消费输入 lane `g_CurFrameGameInputs[GetLocalPlayerSlot()]`。多人位置与本地座位来自配置的玩家槽，暂离或离场玩家的位置保持不可用。TH07 尚无独立应用帧或多人逻辑帧时钟，因此不导出对应字段。

`th07_touch_motion_trace_v2(seat, executionKind)` 提供完成移动步的只读轨迹，按座位和执行类型分环记录；类型 0/1/2/3 分别表示前进、重放、回滚重演和预测步。每条记录在 `Player::HandlePlayerInputs` 的位置边界限制后写入，包含移动前坐标、未经速度限幅与场地边界处理的目标坐标、实际坐标、角色原始速度、炸弹与有效帧率缩放、灵敏度和本阶段模拟帧。键盘步使用 mode 0 且目标等于移动前坐标；触控/摇杆步使用 mode 1，取消限速模式使用 mode 2。focus flag 记录原生 `isFocus` 状态，移动限速由 mode 与 speed 表达。重放和网络输入在没有原始灵敏度元数据时以 flags 明示未知。v2 header/row 布局和 selector 范围见 `common/include/eagler/input/TouchMotionTrace.hpp`。

SDL info/debug 输出使用 stdout，warning 输出使用浏览器 warning 通道，error 保留错误诊断通道。首次创建可选配置和分数文件属于信息，窗口、渲染、资源解压及存档写入失败按错误级别输出。

红点使用最近未满 Power 优先的原生吸引。直接拾取、上线收点、满 Power 转樱点及双人局席位范围由 `tests/item-collection-browser.py` 驱动真实游戏验证。为独立的 Web 多人测试构建启用 `-DTH_ITEM_COLLECTION_FIXTURES=ON`，再运行 `python ports/th07/tests/item-collection-browser.py --runtime-directory=<测试构建目录> --font=<msgothic.ttc> --output=<结果.json>`；驱动覆盖梦 A／梦 B 的双人局与三人局。正式 Runtime 使用默认关闭的测试选项。

旁观以普通 Replay 节奏播放，进度积压由时间线操作定位。原生关卡与 Boss 音乐在重建中跟随场景切换。跨作持续播放检查位于 `ports/th07/tests/netplay-spectator-natural-browser.py`，传入作品、`--runtime-directory`、`--music-directory`、`--font`、`--relay` 和 `--output`；`--physical-input` 覆盖射击、方向、低速和 Bomb 输入。
