# TH17 开发指南

[开发入口](../README.md) · [适配表](../ADAPTATION.md) · [Runtime 契约](../../../launcher/docs/GAME_ADAPTER_CONTRACT.md)

## 构建

使用 Node.js 22.18 以上、Python 3、Emscripten 6.0.9。编译默认读取仓库 `common/` 的键盘与手柄边界，在每次原生模拟采样时消费 Host 映射。TH17 的渲染、原版帧节奏与触控运动轨迹保留本作实现。

在仓库根目录运行：

```sh
node tools/workspace.mjs select th17 --variant=normal
node launcher/scripts/build-runtime.mjs --game=th17 --variant=normal --emsdk=/path/to/emsdk
node ports/th17/web/scripts/build-sdl-application.mjs --release --print-inputs
```

直接构建可设置 `TH17_EMSDK` 或 `EMSDK`，Python 可由 `TH_PYTHON` 指定。产物位于 `web/artifacts/sdl-release/`。生产构建去除开发探针，保留游戏逻辑、文件格式验证与原生录像启动接口。

设置 `EAGLER_FONT_ROOT` 为个人提供的测量字体目录（原开发包 `th17_web/assets/sdl-native/fonts`），运行：

```sh
node ports/th17/portable/package-eagler.mjs
```

`build-eagler/` 输出 `th17.html`、程序、Runtime 字体表、资源清单和逐文件 SHA-256，可通过共用 Runtime Release 打包入口使用。`EAGLER_RUNTIME_OUTPUT` 可指定输出目录。字体由 Runtime 挂载至 `/fonts`，原版 DATA 与音乐由 Package Store 提供。

## 本地输入

`games/th17/original/` 放个人原版 `th17.dat`、`thbgm.dat` 与开发对照用 `th17.exe`。`web/assets/`、`web/reference/assets/` 为被忽略的开发输入，可从个人交付包连接或准备，源码迁移清单记录了来源身份。原版素材、音乐、录像、测量字体、SDK 和编译输出分别位于仓库忽略目录。

```sh
node launcher/scripts/prepare-th17-content.mjs --original=/path/to/original --output=/path/to/assets-ogg
```

该入口生成原版 archive、18 首 OGG 及来源记录；编码与共用 Host 的固定 OGG 基线核对。已准备的内容可由 Host 的 `assets-ogg` 输入使用。单个 `th17.dat` 也可通过共用启动器的原版文件导入流程安装。

## 协议与存储

Runtime 使用 `eagler-touhou/1`，校验同源父窗口、作品与正整数 `runtimeEpoch`，按 `ready → configure → launch → first-frame` 运行。DATA provider 为 `retail-memory`，Host 提供原始 `/th17.dat`，资源清单提供字体表，OGG 挂载到 `/music/`。原版文件与 OGG 的安装代次由 Host 统一管理。

配置支持 OGG 与无音乐、连续触控、摇杆、100–300 灵敏度、双指低速、双击符卡，以及指定文件录像启动。用户文件限定 `/savesth17/scoreth17.dat`、`th17.cfg` 和 `replay/th17_*.rpy[x]`；写入先由原生解析器验证。Runtime 定期保存，失焦、隐藏、退出时清除临时输入，复用 Host 音频上下文。帧与音频健康事件携带同一会话代次。

当前产品使用日文原文和普通版玩法，产品能力表登记实际构建功能。逐项运行验收记录见适配表。

当前生产构建的接入结果见[2026-10-30 验证记录](../../../archive/th17-development-20261030/migration-validation.md)，覆盖桌面 Chromium 的生产 Wasm、两种音乐模式、存档恢复与指定原版演示文件启动。

## 检查

```sh
npm run ports:check -- --game=th17 --variant=normal
node --test ports/th17/web/tests/adapter-browser.test.mjs
npm run docs:check
```

`adapter-browser.test.mjs` 读取 `EAGLER_TH17_BUILD`（默认 `build-eagler/`）、`EAGLER_TH17_DATA`（默认 `games/th17/original/th17.dat`）、`EAGLER_TH17_MUSIC`（默认 `games/th17/assets-ogg/music`）和 `EAGLER_TH17_REPLAY`（默认个人解包的 `demo1.rpy`）。检查覆盖两种音乐模式、会话隔离、用户文件、输入清除、原生退出、IDBFS 跨文档恢复和原版录像启动。

原生模块对照使用 WASI SDK，可通过 `TH17_WASI_CXX` 指定 clang++，先运行 `web/scripts/cpp/build.mjs`，再执行 `web/tests/cpp/*.test.mjs`。`TH17_UNICORN_MODULE` 指向个人安装的 `@alexaltea/unicorn-js/dist/unicorn_x86.js`；`web/reference/native/` 放开发包对照用 `unicorn-bounded.wasm`。字体重测工具的 `TH17_NATIVE_PACKAGE` 指向提供 Koffi 的 package.json。对照工具使用个人原版 EXE 和解包素材。原交付构建的版本、路线与设备范围随历史记录保留。
