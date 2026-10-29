# 星莲船多人版开发入口

[源码说明](../README.md) · [适配项目](../ADAPTATION.md) · [SDL3 平台库](../portable/sdl/README.md)

多人游戏继续由现有 `src/multiplayer` 核心和 `src/multiplayer.mjs` 会话宿主管理。房间、输入确认、回滚、观战、关卡同步和多人录像沿用多人专属协议。正式 Web 构建为本目录的 `scripts/build-eagler.mjs`。

`portable/sdl` 与普通版保持同源，提供 C++ 资源、菜单、图形、音频、字体和普通扩展录像编解码；编译时直接链接本目录多人核心。平台库构建：

```sh
cmake -S ports/th12-mp -B artifacts/native/th12-mp -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DTH12_BUILD_SDL_HOST=ON -DTH12_ENABLE_THPRAC=ON -DTH12_FETCH_SDL_DEPS=ON
cmake --build artifacts/native/th12-mp --target th12_sdl_host --config Release --parallel
```

普通版的 `TH12WEB1` 扩展录像与多人录像分别由自己的编解码器管理。联机启动、网络传输和多人画面继续由多人宿主管理；平台库作为共用跨平台基础随普通版维护。

依赖版本、平台构建、代码包和原版资源的划分见 [SDL3 指南](../portable/sdl/README.md)。修改普通版通用代码时同步本目录，并依 [目录同步流程](../../../docs/adaptation/port-sync.md) 合并历史、写回及登记模式专属决定。
