# 星莲船多人版开发入口

[源码说明](../README.md) · [适配项目](../ADAPTATION.md) · [SDL3 应用指南](../portable/sdl/README.md)

正式构建 `scripts/build-eagler.mjs` 生成 SDL3／GLES Runtime，输出 `build-eagler-multiplayer`。`portable/sdl/Application.cpp` 驱动菜单、逻辑回调及显示；`Multiplayer.cpp` 连接现有 C++ Session，`MultiplayerPresentation.cpp` 提供三席 HUD 和所有者显示策略。

`src/multiplayer` 保留输入确认、回滚、换面屏障与已确认录像。`site/game-sdl.mjs` 和 `src/multiplayer.mjs` 连接启动器、浏览器传输、观战控制和 IndexedDB。房间等待不进入普通版 Demo；开局复用资源与实例；暂停和重开沿原同步状态推进。结局、成绩排名、姓名输入、25 个 Replay 槽位在 C++ 应用中显示。

```sh
node ports/th12-mp/scripts/build-eagler.mjs
node --test ports/th12-mp/tests/sdl-clock.test.mjs
node ports/th12-mp/scripts/check-sdl-multiplayer-browser.mjs
```

浏览器验证设置 `TH12_ASSETS` 和 `TH12_FONTS`，输出在本地 `artifacts/sdl-multiplayer-browser`。测试用真实 RTC 输入，并支持 `TH12_INPUT_LATENCY_MS` 与 `TH12_MULTIPLAYER_SCENARIOS`。旧 JS 宿主通过 `build-legacy-eagler.mjs` 单独构建，正式包的模块闭包不引用它或旧渲染器。

依赖与数据准备见 [SDL3 指南](../portable/sdl/README.md)，同步来源按 [目录维护流程](../../../docs/adaptation/port-sync.md) 登记。

## 呈现性能验证

多人前端与普通前端共用动画缓存代次、静止轨道年龄快进、背景裁剪缓冲复用的实现。轨道回退按原始绑定重算；优化不修改 Session、确认帧、输入预测或世界快照。稀疏年龄、移动中的停止 VM、中断和显示随机数通过 `tests/sdl-animation.test.mjs` 与原版 ANM 基准验证；延迟 RTC、观战及录像重建通过 `scripts/check-sdl-multiplayer-browser.mjs` 验证。方法与数据见[迁移版性能验证](../../../archive/th12-sdl-performance-20261029/README.md)。
