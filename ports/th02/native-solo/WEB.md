# SoEW Native Web：单人战斗

网页直接运行 SoEW Native C++ 核心：浏览器采集输入并按 PC-98 节奏调用逻辑帧，WASM 绘制原版关卡画面与 HUD。当前入口从第一关开局，适合验证战斗与 Story 录像。原生菜单、音频、练习 UI、Launcher 安装入口和多人接入属于后续阶段；现有 DOSBox-X 版继续使用原入口。

## 本地运行

在本目录运行。使用 Emscripten 4.0.15、CMake、Ninja 与 Node.js；先激活 Emscripten 环境，并用 `bootstrap.ps1` 准备 SoEW 的 th98port 依赖，或用 `-DTH98PORT_DIR=<目录>` 指向已准备好的 checkout。

```sh
emcmake cmake -S . -B build-web -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-web --target soew-web
node web/stage-content.mjs <解包资源目录> build-web
python -m http.server 18322 --bind 127.0.0.1 --directory build-web
```

打开 [本地游戏](http://127.0.0.1:18322/)。方向键移动，Z 射击，X 灵击，Shift 低速，P 暂停，R 重开。解包资源目录可通过 `python tools/prepare_content.py <自己的日文 TH02 HDI> <新的资源目录>` 生成。关卡素材和链接 th98port 的构建产物留在本地；对外分发前需按 [上游构建说明](README.md#build-and-test-notes) 核对依赖授权。

## 跨平台录像校验

用 Windows MinGW 构建录像探针，从同一份资源生成原生 Story `.rep` 与每帧状态摘要，再在 Chromium 中回放并逐帧对比。使用 Launcher 已安装的 `puppeteer-core` 和本机 Chromium。

```sh
cmake -S . -B build-probe -G Ninja -DCMAKE_BUILD_TYPE=Release -DSOEW_BUILD_DEV_TOOLS=ON
cmake --build build-probe --target soew-replay-probe
node web/browser.test.mjs build-web build-probe/soew-replay-probe.exe
```
