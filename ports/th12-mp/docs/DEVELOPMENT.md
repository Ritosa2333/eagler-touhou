# 星莲船多人版开发入口

[源码说明](../README.md) · [适配项目](../ADAPTATION.md) · [SDL3 应用指南](../portable/sdl/README.md)

正式构建 `scripts/build-eagler.mjs` 生成 SDL3／GLES Runtime，输出 `build-eagler-multiplayer`。`portable/sdl/Application.cpp` 驱动菜单、逻辑回调及显示；`Multiplayer.cpp` 连接现有 C++ Session，`MultiplayerPresentation.cpp` 提供三席 HUD 和所有者显示策略。

`src/multiplayer` 保留输入确认、回滚、换面屏障与已确认录像。`site/game-sdl.mjs` 和 `src/multiplayer.mjs` 连接启动器、浏览器传输、观战控制和 IndexedDB。房间等待不进入普通版 Demo；开局复用资源与实例；暂停和重开沿原同步状态推进。结局、成绩排名、姓名输入、25 个 Replay 槽位在 C++ 应用中显示。

```sh
node ports/th12-mp/scripts/build-eagler.mjs
npm --prefix ports/th12-mp run test:sdl
node ports/th12-mp/scripts/check-sdl-multiplayer-browser.mjs
```

浏览器验证设置 `TH12_ASSETS` 和 `TH12_FONTS`，输出在本地 `artifacts/sdl-multiplayer-browser`。测试用真实 RTC 输入，并支持 `TH12_INPUT_LATENCY_MS` 与 `TH12_MULTIPLAYER_SCENARIOS`。旧 JS 宿主通过 `build-legacy-eagler.mjs` 单独构建，正式包的模块闭包不引用它或旧渲染器。

依赖与数据准备见 [SDL3 指南](../portable/sdl/README.md)，同步来源按 [目录维护流程](../../../docs/adaptation/port-sync.md) 登记。

## 局次、旁观与结算

多人输入规则版本为 7。菜单按席位维护按下边沿与释放状态，队友持续射击时其他席位可以确认。R 经暂停菜单建立新局，普通游戏中的 R 沿本局继续。录像记录生效后的原生输入，按录制程序的构建身份校验。

每次重开更新旁观局次与 seed，从新的原生 Replay 关头重建。玩家历史按连续确认前沿发布，发送受阻时延后发送，结算时由共用尾包保存器排空本局输入和结束标记。旁观暂停位置与玩家结算分别推进。

观众播完尾部并收到同局、同构建的完整录像后，进入本机原生选槽与署名流程。保存沿本作编码器写入多人录像，旁观过程及结算不计入玩家成绩。返回房间先清理会话输入与异步操作，再等待同一原生标题实际呈现。

`test:sdl` 检查浏览器边界与适配模块。原生 CTest 的 `th12_native_smoke`、`th12_multiplayer_menu_safety` 检查席位菜单、回滚和输入编码。共有 `ports:mp` 的 `--lifecycle --restart=both --recover --wait-end` 检查真实连接、重开、旁观恢复、尾帧和原生录像保存。

## 呈现性能验证

多人前端与普通前端共用动画缓存代次、静止轨道年龄快进、背景裁剪缓冲复用的实现。轨道回退按原始绑定重算；优化不修改 Session、确认帧、输入预测或世界快照。稀疏年龄、移动中的停止 VM、中断和显示随机数通过 `tests/sdl-animation.test.mjs` 与原版 ANM 基准验证；延迟 RTC、观战及录像重建通过 `scripts/check-sdl-multiplayer-browser.mjs` 验证。方法与数据见[迁移版性能验证](../../../archive/th12-sdl-performance-20261029/README.md)。

## SDL 浏览器输入与输出

Web 画布沿用 640×480 离屏目标，窗口呈现负责缩放；背景复制在首次校验后复用纹理。桌面保持 2 倍离屏目标。启动器的 direct-touch 坐标按 iframe 归一化，thprac-mouse 坐标使用 CSS 像素，均通过画布实际边界换算。

SDL 输入保留触控 Esc 的 0x100 位。ImGui 鼠标与键盘、菜单手势、原生初始化和对局首帧分别遵守各自生命周期。正式代码包在入口内嵌布局样式，房间及观战使用同一页面布局。验证范围见[输入与呈现记录](../../../archive/th12-sdl-input-20261029/README.md)。

## SDL 呈现工作复用

绘制调度沿用 Web 适配器的优先级分桶：顺序已正确的桶直接拼接，只排序发生逆序的桶。同帧身份查询使用复用槽位和帧代次，重复身份保留首次登记，稀疏优先级和并列顺序维持原输出。

背景裁剪、圆形特效、标题、加载和结局缓冲随场景复用；背景深度排序以原插入次序处理并列深度。三个常用 HUD 中断指令内联存储，长序列完整保留。多人额外呈现与网络等待复用场景捕获，世界帧、关卡或回滚恢复序号变化时重建；实时 FPS 字段独立更新。

组件耗时、临时分配、原版动画输出及浏览器回归见[呈现性能记录](../../../archive/th12-sdl-render-perf-20261029/README.md)。
