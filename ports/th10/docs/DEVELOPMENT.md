# TH10 开发指南

当前适配记录见[TH10 适配表](../ADAPTATION.md)。命令从仓库根目录运行；示例使用 PowerShell。

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
node launcher/scripts/build-runtime.mjs --game=th10 --variant=normal --emsdk='/path/to/emsdk'
node launcher/scripts/package-runtime.mjs --game=th10 --variant=normal
npm --prefix launcher run build
npm --prefix launcher run dev:workspace
```

普通版配方启用 `thcrap`、`thprac`。准备目录会生成 `th10.data` 和 `bgm-ogg/`；编译产物在 `ports/th10/web/artifacts/sdl3/`，Launcher Runtime 在 `ports/th10/build-eagler/`。Host 在 <http://127.0.0.1:8130/>。

## 源码与验证

`web/cpp/` 含游戏、SDL 宿主和 Runtime 导出；`portable/` 含 GLES、数值、输入和浏览器接口；`portable/presentation-lab/` 是独立诊断工作台。源码检查从 `ports/th10/` 运行 `node portable/check-high-refresh-contract.mjs` 和 `node portable/check-presentation-purity.mjs`；纯度报告以 `unknown` 标记尚待归类的结果。

`th10_native_observation_v1()` 以共用 12-word ABI 提供只读阶段、已消费输入、玩家坐标、Results 暂停状态与关卡逻辑计时。有效位区分尚未建立的状态，读取接口只复制数据。普通版重开对照关卡逻辑计时；MP 按本地席位读取已提交输入及坐标，暂停和重开通过真实房间协商流程验收。

`th10_touch_motion_trace_v2(seat, kind)` 在原生 `Player::move` 完成固定点写回与场地夹紧后记录运动轨迹。TH10 的位置单位为百分之一像素，目标保持触控控制器给出的游戏坐标，限速前的原生速度与倍率独立记录；触控灵敏度取自 SDL 控制器配置。限速后的浮点向量量化为速度圆盘内误差最小的整数格点，保证逐轴取整后仍不超过本作原生速度预算；未饱和微移保留原有百分之一像素精度，无限速模式沿用原生取整。
