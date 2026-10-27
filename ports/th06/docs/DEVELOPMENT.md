# TH06 开发指南

当前适配记录见[TH06 适配表](../ADAPTATION.md)。以下命令均从仓库根目录运行；示例使用 PowerShell。

## 构建并在 Launcher 启动

需要 Node.js 22.18 以上、Python 3、CMake 3.20 以上、Ninja 和 Emscripten SDK 6.0.9（示例路径为 `C:\tools\emsdk`，命令中的 `--emsdk` 指向 SDK 根目录）。首次使用先初始化 SDL 子模块，并安装 Launcher 依赖。`--assets` 指向本机 TH06 资源目录，必须有六个档案文件；它们会编入 `.data`。默认音乐配置为 `BASE`，游戏音乐由 Host 外置提供。

```powershell
git -C ports/th06 submodule update --init --recursive
npm --prefix launcher ci --ignore-scripts # 仅首次安装
$env:EAGLER_DEVELOPMENT_VANILLA_FONT='C:\Windows\Fonts\msgothic.ttc'
$env:EAGLER_DEVELOPMENT_UNICODE_FONT='C:\local-content\fonts\unifont-15.1.05.otf'
$env:EAGLER_DEVELOPMENT_GAMES='th06'
$env:EAGLER_TOUHOU_NETPLAY_RELAY='ws://127.0.0.1:18142/'
node launcher/scripts/build-runtime.mjs --game=th06 --variant=normal --emsdk='C:\tools\emsdk' --assets='C:\Games\TH06'
node launcher/scripts/build-runtime.mjs --game=th06 --variant=multiplayer --emsdk='C:\tools\emsdk' --assets='C:\Games\TH06'
npm --prefix launcher run build
npm --prefix launcher run dev:workspace
```

资源目录需含 `紅魔郷CM.DAT`、`紅魔郷ED.DAT`、`紅魔郷IN.DAT`、`紅魔郷MD.DAT`、`紅魔郷ST.DAT`、`紅魔郷TL.DAT`。默认 `thcrap`、`thprac` 关闭。`build-runtime` 已生成 Launcher 使用的 Runtime，无额外端口打包命令。产物为 `ports/th06/build/web/normal/` 和 `ports/th06/build/web/multiplayer/`，各含 `th06.html`、`th06.js`、`th06.data`。Host 在 <http://127.0.0.1:8130/>。多人房间需另开终端启动 Relay：首次运行 `npm --prefix services/relay ci --ignore-scripts`，之后运行 `npm --prefix services/relay start`。如需音乐，把准备好的 WAV 放在 `ports/th06/assets/bgm/`，或 OGG 放在 `ports/th06/assets-ogg/bgm/`。

## 源码与验证

游戏逻辑在 `src/`，网络和多人代码在 `src/netplay/`、`src/multiplayer/`，浏览器适配在 `portable/`。核心检查从 `ports/th06/` 运行：`bash scripts/run-core-tests.sh`。CI CMake 预设位于 `CMakePresets.json`；它们是外置资源编译检查，不生成上面的含资源运行包。

旁观以普通 Replay 节奏播放，进度积压由时间线操作定位。原生关卡与 Boss 音乐在重建中跟随场景切换。跨作持续播放检查位于 `ports/th07/tests/netplay-spectator-natural-browser.py`，传入作品、`--runtime-directory`、`--music-directory`、`--font`、`--relay` 和 `--output`；`--physical-input` 覆盖射击、方向、低速和 Bomb 输入，`--difficulty=4` 从原生 Extra 入口检查持续播放与定位后的状态一致性。`--join-frame=1800` 覆盖中途加入，`--join-stage=5` 在来源端切入第五关后开始计数。`--frames` 指定当前关头之后的检查长度，`--challenge` 使用原生挑战规则持续演算。`--until-end` 核对普通局尾帧消费与原生结果菜单，`--music=none` 运行独立的游戏状态场景。旁观游标直接读取原生报告，结算后的本机菜单状态单独记录。

旁观播完局末输入后进入本机原生结算界面，沿用选槽、署名与 Replay 保存流程。结算界面自行加载标题资源包，直接从 Replay 关头进入的游戏实例同样能够完整呈现结果页。

私有观测构建通过 `private-spectator.json` 登记原生起始关、受保护席位、来源对象和 WASM 身份。驱动按实际接收关头选择音乐与比较范围，并记录各席位的死亡、复活和幽灵状态。

具体场景与结果见[合作旁观状态验收](records/2026-10-26-spectator-browser.md)。
