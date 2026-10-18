# TH10 多人版开发指南

当前适配记录见[TH10 多人适配表](../ADAPTATION.md)。命令从仓库根目录运行；示例使用 PowerShell。

## 构建并在 Launcher 启动

需要 Node.js 22.18 以上、Python 3 和 Emscripten SDK 6.0.9（准备方式见[共用工具链](../../../docs/adaptation/build.md#工具链)）；Launcher 打包还需 `ffmpeg`。首次使用安装 Launcher 依赖。原版目录须含 `th10.dat`、`thbgm.dat`；`EAGLER_FONT_ROOT` 须含 `blend.bin`、`codepages.bin`。

```powershell
npm --prefix launcher ci --ignore-scripts # 仅首次安装
node launcher/scripts/prepare-th10-content.mjs --original='C:\Games\TH10' --output='ports/th10/assets-ogg'
$env:EAGLER_FONT_ROOT='C:\local-content\th10-fonts'
$env:EAGLER_TH10_CONTENT_DIR=(Resolve-Path 'ports/th10/assets-ogg').Path
$env:EAGLER_DEVELOPMENT_VANILLA_FONT='C:\Windows\Fonts\msgothic.ttc'
$env:EAGLER_DEVELOPMENT_UNICODE_FONT='C:\local-content\fonts\unifont-15.1.05.otf'
$env:EAGLER_DEVELOPMENT_GAMES='th10'
$env:EAGLER_TOUHOU_NETPLAY_RELAY='ws://127.0.0.1:18142/'
node launcher/scripts/build-runtime.mjs --game=th10 --variant=multiplayer --emsdk='/path/to/emsdk'
node ports/th10-mp/portable/package-eagler.mjs --multiplayer
npm --prefix launcher run build
npm --prefix launcher run dev:workspace
```

多人版配方启用 `thcrap`、关闭 `thprac`。准备目录含 `th10.data` 和 `bgm-ogg/`；多人编译产物在 `ports/th10-mp/web/artifacts/multiplayer/`，Runtime 在 `ports/th10-mp/build-eagler-multiplayer/`。TH10 和 TH10 Multiplayer 使用同一 Launcher 内容目录。Host 在 <http://127.0.0.1:8130/>；房间联调另开终端启动 Relay：首次运行 `npm --prefix services/relay ci --ignore-scripts`，之后运行 `npm --prefix services/relay start`。

## 源码与验证

多人源码在 `web/cpp/multiplayer/` 和 `portable/multiplayer/`；共享 GLES、输入接口在 `portable/`。从 `ports/th10-mp/` 运行 `node portable/check-high-refresh-contract.mjs --multiplayer` 和 `node portable/check-presentation-purity.mjs --multiplayer`；纯度报告以 `unknown` 标记尚待归类的结果。
