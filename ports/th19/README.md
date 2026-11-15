# 东方兽王园 TH19

TH19 的 C++ / SDL3 / WebAssembly 实现，提供独立浏览器入口与 Eagler Touhou 总仓库适配。普通版和双人对战版共用源码、独立打包，游戏逻辑和原生菜单在浏览器本地执行。

启动器将 th19 普通版与双人对战入口列为抢先体验内容，通过共用测试功能开关显示，使用现有 Runtime、资源、存档和 Relay 流程。

[开发与构建](docs/DEVELOPMENT.md) | [旁观与扩展录像](docs/REPLAY.md) | [适配检查](ADAPTATION.md)

## 运行

保留已有 `content/` 和 `runtime/` 时，运行 `npm start` 或双击“启动兽王园.bat”，访问 `http://127.0.0.1:8130/`。

## 源码

- `web/cpp/game/`：游戏逻辑、ECL、弹幕、对战和检查点。
- `web/cpp/sdl/`：原生菜单、画面、音频、存档与导出接口。
- `web/sdl-runtime/`：Runtime 协议、输入、资源、WebRTC 与回滚驱动。
- `portable/`：SDL/GLES 渲染、共享接口兼容头和 Runtime 打包。
- `scripts/`：编译、完整性检查和本地服务。
- `tests/`：回归检查与真实浏览器验收。

`content/`、`fonts/`、`runtime/`、编译缓存与打包输出由 Git 忽略。源码包与 Runtime 包分别交付；原版游戏数据、音乐和字体由使用者提供。

## 构建

设置 `TH19_EMSDK` 后运行 `npm run build`，再运行 `npm run package:runtime` 与 `npm run package:multiplayer`。`npm test` 检查源码行为；`npm run check` 校验本机游戏资源与运行文件。完整步骤见开发指南。
