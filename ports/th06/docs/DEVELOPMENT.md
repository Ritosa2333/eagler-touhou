# TH06 开发指南

当前适配记录见[TH06 适配表](../ADAPTATION.md)。以下命令均从仓库根目录运行；示例使用 PowerShell。

## 构建并在 Launcher 启动

需要 Node.js 22.18 以上、Python 3、CMake 3.20 以上、Ninja 和 Emscripten SDK 6.0.9（示例路径为 `C:\tools\emsdk`，命令中的 `--emsdk` 指向 SDK 根目录）。首次使用先初始化 SDL 子模块，并安装 Launcher 依赖。`--assets` 指向本机 TH06 资源目录，必须有六个档案文件；它们会编入 `.data`。默认音乐配置为 `BASE`，游戏音乐由 Host 外置提供。

```powershell
node launcher/scripts/print-runtime-submodule-init.mjs --game=th06
# 按输出命令分层初始化本作构建依赖
npm --prefix launcher ci --ignore-scripts # 仅首次安装
$env:EAGLER_DEVELOPMENT_VANILLA_FONT='C:\Windows\Fonts\msgothic.ttc'
$env:EAGLER_DEVELOPMENT_UNICODE_FONT='C:\local-content\fonts\unifont-15.1.05.otf'
$env:EAGLER_DEVELOPMENT_GAMES='th06'
$env:EAGLER_TOUHOU_NETPLAY_RELAY='ws://127.0.0.1:18142/'
node launcher/scripts/build-runtime.mjs --game=th06 --variant=normal --emsdk='C:\tools\emsdk' --assets='C:\Games\TH06'
node launcher/scripts/build-runtime.mjs --game=th06 --variant=multiplayer --emsdk='C:\tools\emsdk' --assets='C:\Games\TH06'
node launcher/scripts/package-runtime.mjs --game=th06 --variant=normal
node launcher/scripts/package-runtime.mjs --game=th06 --variant=multiplayer
npm --prefix launcher run build
npm --prefix launcher run dev:workspace
```

资源目录需含 `紅魔郷CM.DAT`、`紅魔郷ED.DAT`、`紅魔郷IN.DAT`、`紅魔郷MD.DAT`、`紅魔郷ST.DAT`、`紅魔郷TL.DAT`。默认 `thcrap`、`thprac` 关闭。代码 Runtime 输出在 `ports/th06/build-eagler/normal/` 和 `ports/th06/build-eagler/multiplayer/`；游戏 DATA 与预加载 JS 使用 `ports/th06/build/web/` 对应模式目录。统一打包入口生成包凭据，供 Launcher 来源验收读取。Host 在 <http://127.0.0.1:8130/>。多人房间需另开终端启动 Relay：首次运行 `npm --prefix services/relay ci --ignore-scripts`，之后运行 `npm --prefix services/relay start`。如需音乐，把准备好的 WAV 放在 `ports/th06/assets/bgm/`，或 OGG 放在 `ports/th06/assets-ogg/bgm/`。

Vendored SDL_image、libpng 与 zlib 会按 tracked 输入复制到各模式的 `build/web/<模式>/eagler-staging/sdl-image/<SHA-256>/`。zlib `zconf.h` 保留为源码输入，构建目录生成配置后的头文件。

启动时的 GLES renderer、GLSL 版本与 LAN 状态信息写入 stdout 信息流；SDL 渲染错误继续写入错误流。

## 源码与验证

游戏逻辑在 `src/`，网络和多人代码在 `src/netplay/`、`src/multiplayer/`，浏览器适配在 `portable/`。核心检查从 `ports/th06/` 运行：`bash scripts/run-core-tests.sh`。CI CMake 预设位于 `CMakePresets.json`；它们是外置资源编译检查，不生成上面的含资源运行包。

Runtime 提供 `th06_native_observation_v1()` 只读观测接口。它结合 `Supervisor.curState` 与 `GameManager.isInMenu` 区分标题、可操作游戏、暂停、结算、结尾和转场，以 `g_GameManager.gameFrames` 提供模拟帧；暂停/重试时该帧保持不变。普通构建读取当前已消费的 `g_CurFrameInput`，多人构建读取消费输入 lane `g_CurFrameGameInputs[GetLocalPlayerSlot()]`。多人位置与本地座位按已配置玩家槽读取，暂离或离场玩家的位置保持不可用。TH06 没有独立应用帧或多人逻辑帧时钟，因此不导出对应字段。

`th06_touch_motion_trace_v2(seat, executionKind)` 提供完成移动步的只读轨迹，按座位和执行类型分环记录；类型 0/1/2/3 分别表示前进、重放、回滚重演和预测步。每条记录在 `Player::HandlePlayerInputs` 的位置边界限制后写入，包含移动前坐标、未经速度限幅与场地边界处理的目标坐标、实际坐标、角色原始速度、炸弹与有效帧率缩放、灵敏度和模拟帧。键盘步使用 mode 0 且目标等于移动前坐标；触控/摇杆步使用 mode 1，取消限速模式使用 mode 2。focus flag 记录原生 `isFocus` 状态，移动限速由 mode 与 speed 表达。重放和网络输入在没有原始灵敏度元数据时以 flags 明示未知。v2 header/row 布局和 selector 范围见 `common/include/eagler/input/TouchMotionTrace.hpp`。
