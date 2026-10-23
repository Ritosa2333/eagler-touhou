# TH08 多人版开发指南

本目录打包入口固定使用 TH08 产品身份，普通版与 MP 由显式构建参数选择。编译模块文件名可与其他作品相同。

当前适配记录见[TH08 多人适配表](../ADAPTATION.md)。命令从仓库根目录运行；示例使用 PowerShell。

## 构建并在 Launcher 启动

需要 Node.js 22.18 以上、Python 3 和 Emscripten SDK 6.0.9（示例路径为 `C:\tools\emsdk`，命令中的 `--emsdk` 指向 SDK 根目录）。首次使用安装 Launcher 依赖。准备本机 `th08.dat`；`EAGLER_FONT_ROOT` 目录须含 `blend.bin`、`cp932.bin`。Host 另需可读的日文字体和 Unifont。

```powershell
npm --prefix launcher ci --ignore-scripts # 仅首次安装
$env:EAGLER_FONT_ROOT='C:\local-content\th08-fonts'
$env:EAGLER_TH08_DATA_FILE='C:\Games\TH08\th08.dat'
$env:EAGLER_DEVELOPMENT_VANILLA_FONT='C:\Windows\Fonts\msgothic.ttc'
$env:EAGLER_DEVELOPMENT_UNICODE_FONT='C:\local-content\fonts\unifont-15.1.05.otf'
$env:EAGLER_DEVELOPMENT_GAMES='th08'
$env:EAGLER_TOUHOU_NETPLAY_RELAY='ws://127.0.0.1:18142/'
node launcher/scripts/build-runtime.mjs --game=th08 --variant=multiplayer --emsdk='C:\tools\emsdk'
node ports/th08-mp/portable/package-eagler.mjs --multiplayer
npm --prefix launcher run build
npm --prefix launcher run dev:workspace
```

根配方的多人版启用 `thcrap`、`thprac`。编译产物在 `ports/th08/web/artifacts/multiplayer/`；Runtime 在 `ports/th08-mp/build-eagler-multiplayer/`。Host 在 <http://127.0.0.1:8130/>。进入房间还需在第二终端启动 Relay：首次运行 `npm --prefix services/relay ci --ignore-scripts`，之后运行 `npm --prefix services/relay start`。

## 源码与验证

`web/cpp/` 是游戏和 SDL 宿主；`portable/multiplayer/` 是合作玩法和网络接入；`portable/browser/`、`portable/input/`、`portable/sdl/` 提供浏览器端代码。源码检查从 `ports/th08-mp/` 运行：`node portable/check-multiplayer-cooperation.mjs` 和 `node portable/check-high-refresh-contract.mjs`。

## 普通版修改同步

普通版目录历史通过真实 Git 合并接入 MP，来源与接收范围见[普通版与多人版同步](../../../docs/adaptation/port-sync.md)。通用功能、练习源码和专项检查沿普通版更新，MP 保留席位输入、回滚、旁观与多人录像所有权。构建配方继续选择对应模式。

练习按键监视器使用独立内容快照，回滚复制容器内容；练习标量状态继续进入日志与校验。
