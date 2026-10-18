# th08 开发指南

当前适配记录见[th08 适配表](../ADAPTATION.md)。命令从仓库根目录运行；示例使用 PowerShell。

## 构建并在 Launcher 启动

需要 Node.js 22.18 以上、Python 3 和 Emscripten SDK 6.0.9。未安装时运行 `python ports/th08/tools/download-emscripten.py`。首次使用安装 Launcher 依赖。准备本机 `th08.dat`；打包字体目录须含 `blend.bin`、`cp932.bin`。Launcher Host 另需可读的日文字体和 Unifont。

```powershell
npm --prefix launcher ci --ignore-scripts # 仅首次安装
$env:EAGLER_FONT_ROOT='C:\local-content\th08-fonts'
$env:EAGLER_TH08_DATA_FILE='C:\Games\th08\th08.dat'
$env:EAGLER_DEVELOPMENT_VANILLA_FONT='C:\Windows\Fonts\msgothic.ttc'
$env:EAGLER_DEVELOPMENT_UNICODE_FONT='C:\local-content\fonts\unifont-15.1.05.otf'
$env:EAGLER_DEVELOPMENT_GAMES='th08'
node launcher/scripts/build-runtime.mjs --game=th08 --variant=normal --emsdk='/path/to/emsdk'
node ports/th08/portable/package-eagler.mjs
npm --prefix launcher run build
npm --prefix launcher run dev:workspace
```

根配方的普通版启用 `thcrap` 和 `thprac`。编译产物在 `ports/th08/web/artifacts/sdl3/`；打包 Runtime 在 `ports/th08/build-eagler/`。Host 在 <http://127.0.0.1:8130/>。如需外置 OGG 音乐，准备含 21 首曲目的 `bgm-ogg/`，并将 `EAGLER_TH08_CONTENT_DIR` 设为它的上级目录；不提供 OGG 时可从原版数据使用 MIDI 或关闭音乐。

## 源码与验证

`web/cpp/` 含游戏和 SDL 宿主；`portable/` 含渲染、数值、输入与浏览器代码；回放适配在 `tools/replay-verifier/`。目录键盘模块由 `eagler-common` 生成：从仓库根目录运行 `node common/tools/sync-directory-keyboard.mjs ports/th08/web/sdl-runtime/directory-keyboard.mjs`。键盘映射与 Shell 接线回归运行 `python launcher/tests/test-keyboard-mapping-browser.py`，在 Chromium 中覆盖全部 15 个 normal/MP Runtime 变体。从 `ports/th08/` 运行 `node portable/check-high-refresh-contract.mjs`；呈现工作台改动再运行 `node portable/check-presentation-purity.mjs`。Launcher 手柄动作在原生取样边界合并进游戏输入；运行 `node launcher/tests/test-gamepad-contract.mjs` 校验全部登记变体与共享 C++ 映射。
