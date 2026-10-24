# th10 多人版开发指南

`th10_native_observation_v1()` 沿用普通版的共用 12-word 只读 ABI，输入和坐标来自本地席位，另提供实际多人逻辑帧。未建立字段由有效位标明，MP 暂停和重开通过真实房间协商流程验收。

本目录打包入口固定使用 th10 产品身份，普通版与 MP 由显式构建参数选择。编译模块文件名可与其他作品相同。

当前适配记录见[th10 多人适配表](../ADAPTATION.md)。命令从仓库根目录运行；示例使用 PowerShell。

## 构建并在 Launcher 启动

需要 Node.js 22.18 以上、Python 3 和 Emscripten SDK 6.0.9（准备方式见[共用工具链](../../../docs/adaptation/build.md#工具链)）；Launcher 打包还需 `ffmpeg`。首次使用安装 Launcher 依赖。原版目录须含 `th10.dat`、`thbgm.dat`；`EAGLER_FONT_ROOT` 须含 `blend.bin`、`codepages.bin`。

```powershell
npm --prefix launcher ci --ignore-scripts # 仅首次安装
node launcher/scripts/prepare-th10-content.mjs --original='C:\Games\th10' --output='ports/th10/assets-ogg'
$env:EAGLER_FONT_ROOT='C:\local-content\th10-fonts'
$env:EAGLER_TH10_CONTENT_DIR=(Resolve-Path 'ports/th10/assets-ogg').Path
$env:EAGLER_DEVELOPMENT_VANILLA_FONT='C:\Windows\Fonts\msgothic.ttc'
$env:EAGLER_DEVELOPMENT_UNICODE_FONT='C:\local-content\fonts\unifont-15.1.05.otf'
$env:EAGLER_DEVELOPMENT_GAMES='th10'
$env:EAGLER_TOUHOU_NETPLAY_RELAY='ws://127.0.0.1:18142/'
node launcher/scripts/build-runtime.mjs --game=th10 --variant=multiplayer --emsdk='/path/to/emsdk'
node launcher/scripts/package-runtime.mjs --game=th10 --variant=multiplayer
npm --prefix launcher run build
npm --prefix launcher run dev:workspace
```

多人版配方启用 `thcrap`、关闭 `thprac`。准备目录含 `th10.data` 和 `bgm-ogg/`；多人编译产物在 `ports/th10-mp/web/artifacts/multiplayer/`，Runtime 在 `ports/th10-mp/build-eagler-multiplayer/`。th10 和 th10 Multiplayer 使用同一 Launcher 内容目录。Host 在 <http://127.0.0.1:8130/>；房间联调另开终端启动 Relay：首次运行 `npm --prefix services/relay ci --ignore-scripts`，之后运行 `npm --prefix services/relay start`。

## 源码与验证

多人源码在 `web/cpp/multiplayer/` 和 `portable/multiplayer/`；共享 GLES、输入接口在 `portable/`。目录键盘模块由 `eagler-common` 生成：从仓库根目录运行 `node common/tools/sync-directory-keyboard.mjs ports/th10-mp/web/sdl-runtime/directory-keyboard.mjs`。键盘映射与 Shell 接线回归运行 `python launcher/tests/test-keyboard-mapping-browser.py`，在 Chromium 中覆盖全部 15 个 normal/MP Runtime 变体。从 `ports/th10-mp/` 运行 `node portable/check-high-refresh-contract.mjs --multiplayer` 和 `node portable/check-presentation-purity.mjs --multiplayer`；纯度报告以 `unknown` 标记尚待归类的结果。Launcher 手柄动作在原生取样边界合并进游戏输入；运行 `node launcher/tests/test-gamepad-contract.mjs` 校验全部登记变体与共享 C++ 映射。

`th10_touch_motion_trace_v2(seat, kind)` 从按席位绑定的 `PlayerUpdate` 回调记录原生移动完成后的实际坐标。直接触控目标来自已提交的 fixed-hundredths 输入；正向、预测、回滚重演与回放各自使用独立轨迹流，本机触控灵敏度沿用捕获配置，远端与历史输入按 unknown 标志保留源数据边界。限速向量先按原生速度径向限制，再量化到速度圆盘内误差最小的整数格点；逐轴量化不会越过速度预算，未饱和微移保留百分之一像素精度，无限速触控仍沿用原生取整。

## 普通版修改同步

普通版目录历史通过真实 Git 合并接入 MP，来源与接收范围见[普通版与多人版同步](../../../docs/adaptation/port-sync.md)。通用功能、练习源码和专项检查沿普通版更新，MP 保留席位输入、回滚、旁观与多人录像所有权。构建配方继续选择对应模式。
