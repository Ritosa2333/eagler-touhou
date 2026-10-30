# 构建与验证

正式 Runtime 使用 SDL3 / GLES 共用 C++ 应用，源码和平台入口见 [开发指南](DEVELOPMENT.md)及 [SDL 指南](../portable/sdl/README.md)。

## 正式 Web 构建

需要 Node.js 22+、Python 3、CMake 3.24+、Ninja 和 Emscripten。根 CI 的 SDK 版本以 [构建指南](../../../docs/adaptation/build.md#工具链)为准。设置 `EMSDK` 后，在本作品目录运行：

```sh
npm run build:eagler
npm run test:sdl
```

`EMSDK_PYTHON` 指定 SDK 使用的 Python，`CMAKE` 指定 CMake，`TH_BUILD_JOBS` 设置并行任务数。SDL3 和 FreeType 的固定版本与摘要登记在 [Host.cmake](../portable/sdl/Host.cmake)。已有依赖源码通过 `TH12_SDL3_SOURCE`、`TH12_FREETYPE_SOURCE` 复用。

输出为 `build-eagler-multiplayer/th12.html`，包括 `game.mjs`、`core.mjs`、`core.wasm`、`manifest.json` 和 `runtime-files.json`。构建记录校验本作及 common 的编译输入，打包同时校验模块与 Wasm 的大小和 SHA-256。源码或产物身份变化后重新编译。

普通版与 MP 各自构建和打包，目录归属及历史同步见 [维护流程](../../../docs/adaptation/port-sync.md)。桌面构建和启动参数见 [SDL 指南](../portable/sdl/README.md#桌面构建)。

## 自动检查

`test:sdl` 执行 SDL 原生时钟、动画、Replay、音频和浏览器边界测试，以及托管协议检查。原生夹具需要 C++ 编译器，可通过 `CXX` 指定。音频夹具使用生成的 PCM 数据和设备替身，检查实际混音与暂停、停止、重新启用音乐。

真实素材的浏览器入口为 `npm run check:sdl-browser`。设置 `TH12_ASSETS` 为本地提取素材目录，`TH12_FONTS` 为共用字体目录。`TH12_TEST_MUSIC=ogg` 加入真实 OGG 输出检查，验证暂停与定位期间的输出。多人入口使用本机 Relay 与真实 WebRTC，验证双人、三人、回滚、旁观及原生录像保存。

共有生命周期和多人验收使用 [验证指南](../../../docs/adaptation/validation.md)中的 `ports:runtime`、`ports:mp`。结果记录实际构建身份、场景和观察范围。

## 内容与旧前端工具

游戏数据、OGG 与字体由本地自有素材准备，步骤见 [内容准备](CONTENT_PREPARATION.md)。代码构建和 Runtime 包独立于这些资源。

`scripts/build-legacy-eagler.mjs` 用于旧网页前端，`npm run build` 用于既有 Web/Node 行为工具。旧渲染器测试和 `test:portable` 对应各自的模块或诊断核心。正式 SDL Runtime 按本页入口构建和验收。

原版 Replay 对照、诊断构建与固定基准的流程见 [录像验证器](../tools/replay-verifier/README.md)。
