# TH07 开发指南

当前适配记录见[TH07 适配表](../ADAPTATION.md)。命令从仓库根目录运行；示例使用 PowerShell。

## 构建并在 Launcher 启动

需要 Node.js 22.18 以上、Python 3、CMake 3.20 以上、Ninja 和 Emscripten SDK 6.0.9（示例路径为 `C:\tools\emsdk`，命令中的 `--emsdk` 指向 SDK 根目录）。首次使用先初始化 SDL 子模块，并安装 Launcher 依赖。`--assets` 指向含本机 `th07.dat` 的目录；档案会编入 `.data`。默认音乐配置为 `BASE`，Host 音乐另行提供。

```powershell
git -C ports/th07 submodule update --init --recursive
npm --prefix launcher ci --ignore-scripts # 仅首次安装
$env:EAGLER_DEVELOPMENT_VANILLA_FONT='C:\Windows\Fonts\msgothic.ttc'
$env:EAGLER_DEVELOPMENT_UNICODE_FONT='C:\local-content\fonts\unifont-15.1.05.otf'
$env:EAGLER_DEVELOPMENT_GAMES='th07'
$env:EAGLER_TOUHOU_NETPLAY_RELAY='ws://127.0.0.1:18142/'
node launcher/scripts/build-runtime.mjs --game=th07 --variant=normal --emsdk='C:\tools\emsdk' --assets='C:\Games\TH07'
node launcher/scripts/build-runtime.mjs --game=th07 --variant=multiplayer --emsdk='C:\tools\emsdk' --assets='C:\Games\TH07'
npm --prefix launcher run build
npm --prefix launcher run dev:workspace
```

资源目录至少有 `th07.dat`。默认 `thcrap`、`thprac` 关闭。`build-runtime` 已生成 Launcher 使用的 Runtime，无额外端口打包命令。产物为 `ports/th07/build/web/normal/` 和 `ports/th07/build/web/multiplayer/`，各含 `th07.html`、`th07.js`、`th07.data`。Host 在 <http://127.0.0.1:8130/>。多人房间需另开终端启动 Relay：首次运行 `npm --prefix services/relay ci --ignore-scripts`，之后运行 `npm --prefix services/relay start`。需要游戏配乐时，将原版 `thbgm.dat` 放入 `ports/th07/assets/`，或将已准备的曲目放入 `ports/th07/assets-ogg/bgm-ogg/`。

## 源码与验证

游戏逻辑在 `src/`，网络和多人代码在 `src/netplay/`、`src/multiplayer/`，浏览器适配在 `portable/`。核心检查从 `ports/th07/` 运行：`bash scripts/run-core-tests.sh`。`CMakePresets.json` 中的 CI 预设使用外置资源执行编译检查；本地含资源的 Runtime 构建按本页上方步骤运行。

红点使用最近未满 Power 优先的原生吸引。直接拾取、上线收点、满 Power 转樱点及双人局席位范围由 `tests/item-collection-browser.py` 驱动真实游戏验证。为独立的 Web 多人测试构建启用 `-DTH_ITEM_COLLECTION_FIXTURES=ON`，再运行 `python ports/th07/tests/item-collection-browser.py --runtime-directory=<测试构建目录> --font=<msgothic.ttc> --output=<结果.json>`；驱动覆盖梦 A／梦 B 的双人局与三人局。正式 Runtime 使用默认关闭的测试选项。
