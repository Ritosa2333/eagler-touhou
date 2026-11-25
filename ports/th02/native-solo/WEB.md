# SoEW Native Web：完整单人版

网页与 Win32 版运行同一个 SoEW Native 单人程序。从原版标题进入 Story 1–5 面、Extra、练习、录像、排行榜和音乐室。Canvas 显示原生画面，Web Audio 播放原生合成器输出，IndexedDB 保存该网址下的设置、成绩和录像。Extra 沿用原生版解锁条件。

## 本地运行

从 Eagler 仓库的 `ports/th02/native-solo` 运行。使用 Emscripten 4.0.15、CMake、Ninja 与 Node.js；先激活 Emscripten 环境，并用 `bootstrap.ps1` 准备 SoEW 的 th98port 依赖，或用 `-DTH98PORT_DIR=<目录>` 指向已准备好的 checkout。

```sh
emcmake cmake -S . -B build-web -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-web --target soew-full soew-web
node web/stage-content.mjs <解包资源目录> build-web
node web/serve.mjs build-web 18323
```

打开 [本地游戏](http://127.0.0.1:18323/)，按 Z 或 Enter 操作原生菜单；首次按键或点击启用音频。标题界面的 F8 打开本机 `.rep` 文件。

资源目录可用 `python tools/prepare_content.py <自己的日文 TH02 HDI> <新的资源目录>` 生成，也可以直接使用已有 Win32 版的 `content`。浏览器沿用原生版对 `locale/`、`zunsoft/` 和 MIDI SoundFont 的资源约定。当前入口独立于大厅和 DOSBox-X 版。对外分发前按 [上游构建说明](README.md#build-and-test-notes) 核对游戏素材和 th98port 的授权。

## 跨平台录像校验

保留 `?stage1` 作为逐帧诊断入口。用 Windows MinGW 构建录像探针，从同一份资源生成原生 Story `.rep` 与每帧状态摘要，再在 Chromium 中回放并逐帧对比。使用 Launcher 已安装的 `puppeteer-core` 和本机 Chromium。

```sh
cmake -S . -B build-probe -G Ninja -DCMAKE_BUILD_TYPE=Release -DSOEW_BUILD_DEV_TOOLS=ON
cmake --build build-probe --target soew-replay-probe
node web/browser.test.mjs build-web build-probe/soew-replay-probe.exe
```
