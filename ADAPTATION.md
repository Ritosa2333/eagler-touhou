# th12 适配检查

[开发入口](README.md) · [检查方法](../../docs/adaptation/README.md) · [全部作品](../README.md)

源码目录：`ports/th12`。构建模式：`normal`。

[SDL3 / GLES 3 原生层](portable/sdl/README.md)提供 CMake 桌面目标 `touhou12`；与 Web Runtime 共用当前 C++ 核心。此入口的实际画面、输入和整局回放由原生运行验收单独确认，以下表格继续记录 Eagler Web 适配。

实现情况与实际验收分别记录。源码接口和模块检查通过后记为“已实现”；完成对应操作并附执行记录后记为“已验证”。

状态：`待核对`、`进行中`、`待验证`、`已实现`、`已验证`、`检查失败`、`不适用`。

[自动检查结果与源码身份](../../docs/adaptation/status.json) · `npm run ports:check -- --game=th12 --variant=both`

## 源码与构建

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| BUILD-01 | [源码目录与普通版／MP归属](../../docs/adaptation/build.md#build-01) | 已验证 | normal：[source-inputs](../../docs/adaptation/status.json) |
| BUILD-02 | [工具链与外部依赖](../../docs/adaptation/build.md#build-02) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| BUILD-03 | [对应模式独立构建](../../docs/adaptation/build.md#build-03) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| BUILD-04 | [Runtime 文件与来源记录](../../docs/adaptation/build.md#build-04) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| BUILD-05 | [前端搭配单个游戏运行](../../docs/adaptation/build.md#build-05) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| BUILD-06 | [源码与素材来源](../../docs/adaptation/build.md#build-06) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |

## 启动与资源

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| RUNTIME-01 | [协议身份与会话代次](../../docs/adaptation/runtime.md#runtime-01) | 已实现 | normal：[runtime-protocol](../../docs/adaptation/status.json) |
| RUNTIME-02 | [就绪、配置、启动、首帧](../../docs/adaptation/runtime.md#runtime-02) | 已实现 | normal：[runtime-protocol](../../docs/adaptation/status.json) |
| RUNTIME-03 | [命令响应与错误信息](../../docs/adaptation/runtime.md#runtime-03) | 已实现 | normal：[runtime-protocol](../../docs/adaptation/status.json) |
| RUNTIME-04 | [后台、恢复与退出清理](../../docs/adaptation/runtime.md#runtime-04) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| RUNTIME-05 | [Runtime 与资源管理分工](../../docs/adaptation/runtime.md#runtime-05) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| RUNTIME-06 | [重开与常显判定点](../../docs/adaptation/runtime.md#runtime-06) | 不适用 | normal：本作未声明判定点显示 |
| PACKAGE-01 | [共用游戏包存储](../../docs/adaptation/runtime.md#package-01) | 已实现 | normal：[package-store](../../docs/adaptation/status.json) |
| PACKAGE-02 | [DATA 身份与挂载目标](../../docs/adaptation/runtime.md#package-02) | 已实现 | normal：[package-store](../../docs/adaptation/status.json) |
| PACKAGE-03 | [完整更新后切换版本](../../docs/adaptation/runtime.md#package-03) | 已实现 | normal：[package-store](../../docs/adaptation/status.json) |
| PACKAGE-04 | [本地导入与在线安装一致](../../docs/adaptation/runtime.md#package-04) | 已实现 | normal：[package-store](../../docs/adaptation/status.json) |
| PACKAGE-05 | [已安装游戏离线启动](../../docs/adaptation/runtime.md#package-05) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| PACKAGE-06 | [统一安装和更新入口](../../docs/adaptation/runtime.md#package-06) | 已实现 | normal：[package-store](../../docs/adaptation/status.json) |

## 键盘、手柄与触控

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| INPUT-01 | [键盘逻辑输入](../../docs/adaptation/input.md#input-01) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| INPUT-02 | [启动器键盘改键](../../docs/adaptation/input.md#input-02) | 已实现 | normal：[launcher-input-owners](../../docs/adaptation/status.json) |
| INPUT-03 | [启动器手柄映射](../../docs/adaptation/input.md#input-03) | 已实现 | normal：[launcher-input-owners](../../docs/adaptation/status.json) |
| INPUT-04 | [手柄方向与按钮](../../docs/adaptation/input.md#input-04) | 已实现 | normal：[native-gamepad](../../docs/adaptation/status.json) |
| INPUT-05 | [多种输入独立持有](../../docs/adaptation/input.md#input-05) | 已实现 | normal：[launcher-input-owners](../../docs/adaptation/status.json) |
| INPUT-06 | [失焦与断开释放](../../docs/adaptation/input.md#input-06) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| INPUT-07 | [原作菜单与对话输入](../../docs/adaptation/input.md#input-07) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| TOUCH-01 | [限速滑动移动](../../docs/adaptation/input.md#touch-01) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| TOUCH-02 | [无限速滑动移动](../../docs/adaptation/input.md#touch-02) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| TOUCH-03 | [方向式虚拟摇杆](../../docs/adaptation/input.md#touch-03) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| TOUCH-04 | [自由方向虚拟摇杆](../../docs/adaptation/input.md#touch-04) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| TOUCH-05 | [按住低速](../../docs/adaptation/input.md#touch-05) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| TOUCH-06 | [切换低速](../../docs/adaptation/input.md#touch-06) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| TOUCH-07 | [双指低速](../../docs/adaptation/input.md#touch-07) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| TOUCH-08 | [射击、Bomb 与暂停](../../docs/adaptation/input.md#touch-08) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| TOUCH-09 | [Deathbomb 与移动保持](../../docs/adaptation/input.md#touch-09) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| TOUCH-10 | [无限速移动落率记录](../../docs/adaptation/input.md#touch-10) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| TOUCH-11 | [双击 Bomb](../../docs/adaptation/input.md#touch-11) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| TOUCH-12 | [菜单与对话手势](../../docs/adaptation/input.md#touch-12) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| TOUCH-13 | [确认键的新按下边沿](../../docs/adaptation/input.md#touch-13) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| TOUCH-14 | [100–300% 灵敏度](../../docs/adaptation/input.md#touch-14) | 已实现 | normal：[touch-sensitivity](../../docs/adaptation/status.json) |
| TOUCH-15 | [触控与其他输入隔离](../../docs/adaptation/input.md#touch-15) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| TOUCH-16 | [触控取消与生命周期](../../docs/adaptation/input.md#touch-16) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| TOUCH-17 | [共用布局编辑器](../../docs/adaptation/input.md#touch-17) | 已实现 | normal：[shared-touch-controls](../../docs/adaptation/status.json) |
| TOUCH-18 | [共用重开按钮](../../docs/adaptation/input.md#touch-18) | 已实现 | normal：[shared-touch-controls](../../docs/adaptation/status.json) |
| TOUCH-19 | [共用双指放大镜](../../docs/adaptation/input.md#touch-19) | 已实现 | normal：[shared-touch-controls](../../docs/adaptation/status.json) |

## 画面与声音

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| DISPLAY-01 | [固定逻辑步进](../../docs/adaptation/presentation.md#display-01) | 已实现 | normal：[normal-presentation](../../docs/adaptation/status.json) |
| DISPLAY-02 | [跟随显示器刷新率](../../docs/adaptation/presentation.md#display-02) | 已实现 | normal：[normal-presentation](../../docs/adaptation/status.json) |
| DISPLAY-03 | [额外绘制无逻辑副作用](../../docs/adaptation/presentation.md#display-03) | 已实现 | normal：[normal-presentation](../../docs/adaptation/status.json) |
| DISPLAY-04 | [连续视觉字段插值](../../docs/adaptation/presentation.md#display-04) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| DISPLAY-05 | [出生、销毁和瞬移](../../docs/adaptation/presentation.md#display-05) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| DISPLAY-06 | [暂停画面的表现](../../docs/adaptation/presentation.md#display-06) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| DISPLAY-07 | [实际呈现后报告首帧](../../docs/adaptation/presentation.md#display-07) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| DISPLAY-08 | [限制到 60 Hz](../../docs/adaptation/presentation.md#display-08) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| AUDIO-01 | [正常 OGG 音乐](../../docs/adaptation/presentation.md#audio-01) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| AUDIO-02 | [无 BGM 模式](../../docs/adaptation/presentation.md#audio-02) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| AUDIO-03 | [切曲、渐变、暂停、停止](../../docs/adaptation/presentation.md#audio-03) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| AUDIO-04 | [返回前台后恢复声音](../../docs/adaptation/presentation.md#audio-04) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| AUDIO-05 | [音乐资源错误信息](../../docs/adaptation/presentation.md#audio-05) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| AUDIO-06 | [高刷新与音频事件分开](../../docs/adaptation/presentation.md#audio-06) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| MIDI-01 | [声明 MIDI 选项](../../docs/adaptation/presentation.md#midi-01) | 不适用 | normal：本作使用 OGG 音乐 |
| MIDI-02 | [可听到的 MIDI 输出](../../docs/adaptation/presentation.md#midi-02) | 不适用 | normal：本作使用 OGG 音乐 |
| MIDI-03 | [MIDI 生命周期](../../docs/adaptation/presentation.md#midi-03) | 不适用 | normal：本作使用 OGG 音乐 |
| MIDI-04 | [MIDI 与 OGG 分开](../../docs/adaptation/presentation.md#midi-04) | 不适用 | normal：本作使用 OGG 音乐 |

## 存档与 Replay

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| REPLAY-01 | [导入原版 Replay](../../docs/adaptation/storage.md#replay-01) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| REPLAY-02 | [按原版格式导出可表达输入](../../docs/adaptation/storage.md#replay-02) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| REPLAY-03 | [记录实际生效输入](../../docs/adaptation/storage.md#replay-03) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| REPLAY-04 | [扩展格式与版本](../../docs/adaptation/storage.md#replay-04) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| REPLAY-05 | [Replay 文件管理](../../docs/adaptation/storage.md#replay-05) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| REPLAY-06 | [回放与实时输入隔离](../../docs/adaptation/storage.md#replay-06) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| REPLAY-07 | [回放独立于显示刷新率](../../docs/adaptation/storage.md#replay-07) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| SAVE-01 | [已声明的存储根](../../docs/adaptation/storage.md#save-01) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| SAVE-02 | [用户文件允许范围](../../docs/adaptation/storage.md#save-02) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| SAVE-03 | [明确持久化时机](../../docs/adaptation/storage.md#save-03) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| SAVE-04 | [退出重开后数据保留](../../docs/adaptation/storage.md#save-04) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| SAVE-05 | [离线读取已有数据](../../docs/adaptation/storage.md#save-05) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| SAVE-06 | [游戏包与用户数据分开](../../docs/adaptation/storage.md#save-06) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |

## 语言与练习

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| PRACTICE-01 | [发布包中的练习能力](../../docs/adaptation/localization.md#practice-01) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| PRACTICE-02 | [开局前启用与语言](../../docs/adaptation/localization.md#practice-02) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| PRACTICE-03 | [游戏内练习状态](../../docs/adaptation/localization.md#practice-03) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| PRACTICE-04 | [关卡与资源参数](../../docs/adaptation/localization.md#practice-04) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| PRACTICE-05 | [练习 Replay 元数据](../../docs/adaptation/localization.md#practice-05) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| PRACTICE-06 | [触控功能键与指针](../../docs/adaptation/localization.md#practice-06) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| PRACTICE-07 | [练习语言跟随设置](../../docs/adaptation/localization.md#practice-07) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| PRACTICE-08 | [练习界面的可用功能](../../docs/adaptation/localization.md#practice-08) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| LANG-01 | [内置日语原文](../../docs/adaptation/localization.md#lang-01) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| LANG-02 | [语言包身份与发布清单](../../docs/adaptation/localization.md#lang-02) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| LANG-03 | [开局前挂载翻译与字体](../../docs/adaptation/localization.md#lang-03) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| LANG-04 | [语言包失败时回到原文](../../docs/adaptation/localization.md#lang-04) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| LANG-05 | [字体字符覆盖](../../docs/adaptation/localization.md#lang-05) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| LANG-06 | [缺译资源使用原文](../../docs/adaptation/localization.md#lang-06) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |

## 多人接入与规则

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| MP-RULE-01 | [死亡、幽灵与救援](../../docs/adaptation/multiplayer.md#mp-rule-01) | 不适用 | normal：单人模式 |
| MP-RULE-02 | [个人资源与团队资源](../../docs/adaptation/multiplayer.md#mp-rule-02) | 不适用 | normal：单人模式 |
| MP-RULE-03 | [给予资源与反馈](../../docs/adaptation/multiplayer.md#mp-rule-03) | 不适用 | normal：单人模式 |
| MP-RULE-04 | [对话、结算与全灭](../../docs/adaptation/multiplayer.md#mp-rule-04) | 不适用 | normal：单人模式 |
| MP-RULE-05 | [自机可见性与多人HUD](../../docs/adaptation/multiplayer.md#mp-rule-05) | 不适用 | normal：单人模式 |
| MP-01 | [独立 MP 产品变体](../../docs/adaptation/multiplayer.md#mp-01) | 不适用 | normal：单人模式 |
| MP-02 | [房间人数、难度与机体](../../docs/adaptation/multiplayer.md#mp-02) | 不适用 | normal：单人模式 |
| MP-03 | [所有席位准备并同步开局](../../docs/adaptation/multiplayer.md#mp-03) | 不适用 | normal：单人模式 |
| MP-04 | [确定性输入与状态检查](../../docs/adaptation/multiplayer.md#mp-04) | 不适用 | normal：单人模式 |
| MP-05 | [各席位输入归属](../../docs/adaptation/multiplayer.md#mp-05) | 不适用 | normal：单人模式 |
| MP-06 | [旁观加入与输入隔离](../../docs/adaptation/multiplayer.md#mp-06) | 不适用 | normal：单人模式 |
| MP-07 | [旁观只读取连续已确认历史](../../docs/adaptation/multiplayer.md#mp-07) | 不适用 | normal：单人模式 |
| MP-08 | [慢旁观者与玩家隔离](../../docs/adaptation/multiplayer.md#mp-08) | 不适用 | normal：单人模式 |
| MP-09 | [旁观尾帧与结束时机](../../docs/adaptation/multiplayer.md#mp-09) | 不适用 | normal：单人模式 |
| MP-10 | [联机暂停与重开](../../docs/adaptation/multiplayer.md#mp-10) | 不适用 | normal：单人模式 |
| MP-11 | [多人 Replay](../../docs/adaptation/multiplayer.md#mp-11) | 不适用 | normal：单人模式 |
| MP-12 | [WebRTC 与 Relay 路径](../../docs/adaptation/multiplayer.md#mp-12) | 不适用 | normal：单人模式 |
| MP-13 | [本地玩家可见性](../../docs/adaptation/multiplayer.md#mp-13) | 不适用 | normal：单人模式 |
| MP-14 | [联机与回滚诊断](../../docs/adaptation/multiplayer.md#mp-14) | 不适用 | normal：单人模式 |
| MP-15 | [游戏结束返回房间](../../docs/adaptation/multiplayer.md#mp-15) | 不适用 | normal：单人模式 |

## 验证与交付

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| VERIFY-01 | [全部内置 Demo 对照](../../docs/adaptation/validation.md#verify-01) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| VERIFY-02 | [长 Replay 样本](../../docs/adaptation/validation.md#verify-02) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| VERIFY-03 | [原版基准与内容哈希](../../docs/adaptation/validation.md#verify-03) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| VERIFY-04 | [读取固定基准](../../docs/adaptation/validation.md#verify-04) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| VERIFY-05 | [单独维护原版基准](../../docs/adaptation/validation.md#verify-05) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| VERIFY-06 | [逐逻辑帧比较](../../docs/adaptation/validation.md#verify-06) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| VERIFY-07 | [诊断构建与正常构建](../../docs/adaptation/validation.md#verify-07) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |
| VERIFY-08 | [本作状态映射与结束条件](../../docs/adaptation/validation.md#verify-08) | 待核对 | normal：按本项步骤执行，结果写入验证记录 |

<!-- manual-verification -->

## 运行验收记录

按检查编号附[验证记录](../../docs/adaptation/record-template.md)，写明源码身份、模式、环境、操作与结果。自动更新保留本节内容。

### 2026-10-20：normal / Chromium / 日文 1.00b

- RUNTIME-01/02：验证受管理的 88,852,083 字节 DATA、共享字体、标题及第一关启动。
- INPUT/THPrac：实际 Wasm 与共用 ImGui 检查覆盖作弊门控、数值编辑、弹出菜单取消、移动端折叠重开和指针归属；日文游戏场景显示紫版高级选项。
- BUILD：普通 Wasm、前端构建、TypeScript、文档和端口 8 项自动检查通过。源码来源为 `Tweet-META/th12` 的 `eagler` 分支 `c322e8ad`，紫版参考为 2.2.2.7。
- test 发布日文 DATA 与音乐；中文本地化采用后续 thcrap 接入路径。

### 2026-10-20：鼠标、飞碟拾取与标题音频回归

- INPUT：画布鼠标保持可见，点击画面可解锁浏览器音频，标题场景保持不变。
- PRESENTATION：使用原版 ANM 的7项 Wasm 检查通过，覆盖三个飞碟槽的15帧移动、库存轮转、提示动画和大 UFO 呈现。
- AUDIO：7项音乐传输检查通过。Chromium 使用无共享 AudioContext 的宿主，完整解码与流式 OGG 在标题停留期间均已播放，无需菜单按键。
- BUILD：普通 Runtime 构建、端口8项检查和文档检查通过。

### 2026-10-20：换面与暂停呈现回归

- PRESENTATION：6项流程与呈现检查通过，覆盖六面衔接、通关奖励等待、30帧淡出／淡入、跨面资源继承、Practice／Continue及日文原版暂停选项的连续高亮。
- DISPLAY/THPrac：暂停与确认页保留状态栏和监视显示，F1–F7／U及复选框在暂停与确认页保持可用，快捷键由THPrac消费；Replay保持只读，快速重开按游戏运行状态判定。实际Wasm／ImGui检查入口为`tools/ports/th12-practice-browser.mjs`。

### 2026-10-20：奖励显示与计分声音

- SC Bonus分隔符、满火力提示、跨面飞碟轮转槽位由完整C++运行库与原版ANM验证；入口为`tests/reward-feedback.test.mjs`。
- 通关计分音乐与保存录像音效的场景请求由`tests/reward-menu-audio.test.mjs`核对，覆盖普通通关、Extra和辅助局。

### 2026-10-20：thcrap 简体中文与英文本地化

- LANGUAGE：与 TH10、TH11 共用 thpatch 依赖解析、CRC 校验、静态包和 Host 安装流程；输入为日文 1.00b DAT。准备命令见 [本地化](docs/LOCALIZATION.md)。
- MESSAGE：实际 MSG Wasm 对两种语言逐条检查42个文件、108个入口，正常确认与 Ctrl 跳过均与日文控制指令及完成帧数一致；简体2272行、英文2341行，覆盖6组自动剧情。翻译中同一时间点普通文字最多两行，注音使用独立槽位。
- PRESENTATION：Chromium 使用生产 WebGL renderer、零售 atlas 与安装的 Unifont 验证音乐室、居中提醒、双行剧情、长符卡和日文回退；113个符卡、17首音乐评论及12个结局数据通过检查。
- SCOPE：本记录覆盖本地化编译、MSG 行为和上述渲染场景，整局通关及实际设备输入另按对应项目验收。

### 2026-10-20：thcrap 标题菜单贴图

- TEXTURE：语言编译按 thcrap 的 sprite 透明度规则合成 PNG，保留翻译图中全透明的原版菜单 sprite；支持裁剪、共享纹理偏移和跨纹理边界的 sprite。
- CHECK：无零售资源的 Python 回归覆盖全透明 sprite 保留、非空 sprite 覆盖及其透明像素清除；简体与英文使用生产 renderer 检查标题菜单及音乐室。

### 2026-10-20：Practice 面板触控确认与返回

- INPUT：Practice 窗口外的单指轻触开始练习、双指轻触返回机体选择，沿用 TH10 的窗口命中与触摸归属规则；窗口内控件和弹出菜单继续由 ImGui 消费。
- CHECK：`tools/ports/th12-practice-browser.mjs` 使用真实 Wasm、共享字体和 Chromium 触控事件，覆盖单指开始、双指返回、控件点击、弹出菜单关闭与触摸取消，同时运行已有键盘、暂停和工具面板检查。

### 2026-10-20：Practice 重新开始

- RETRY：暂停、快捷重开及结果菜单重开使用本局原始练习参数，保留关卡、章节、阶段、难度和初始资源；暂停结果的布尔练习标记不覆盖参数对象。
- CHECK：9项回归覆盖四面原版练习、五面自定义章节及 Extra，在暂停确认、快捷重开和结果菜单三条路径核对重开参数。

### 2026-10-20：切换型低速跨 Miss 保持

- TOUCH：复活更换玩家实例时，切换型低速保留用户已选择的开关状态；旧拖动坐标、触点及待发 Bomb 手势仍按实例边界清理。
- CHECK：7项回归覆盖连续复活、后续关闭低速、新拖动的低速倍率、菜单和 Replay 切换、显式取消、禁用触控及其他低速模式。

### 2026-10-20：换面资源等待保留 UI

- PRESENTATION：换面等待资源期间使用上一逻辑帧的呈现快照，保持外围 HUD 和练习工具，只遮黑原版384×448游戏区域；下一关就绪后恢复正常呈现。
- CHECK：加载画面回归覆盖快照基底、等待结束释放和游戏区域遮罩边界；原计分显示检查通过。

### 2026-10-20：过关回调、HUD 连续性与录像播放

- FLOW：按原版4227dc的120帧等待与422839的30帧游戏区域淡出执行换面；Stage Clear继续运行Player16、Enemy18、Bullet21、Item22和HUD/MSG25，保留移动、弹幕、道具及消息尾部时序。
- PRESENTATION：MSG文本动画退出后以逻辑VM可见性为准；外围front0动画使用整局连续时钟，下一关游戏区域独立淡入。
- REPLAY：暂停菜单“再次播放录像”重新打开原文件及所选起始关卡；Z四倍快进、Shift慢放，紫版Ctrl/Shift帧率选项继续生效。启动器录像工作区启用THPrac回放核心，交互式面板保持关闭。
- CHECK：完整C++运行库检查六关继承、Continue、练习和旧入口；真实日文过关MSG核对消息结束、自机移动与弹幕运动。Chromium使用生产Wasm、零售ANM/STD和WebGL renderer检查Stage Clear、下一关构造及0/15/30帧淡入，外围HUD保持显示。
- REPLAY CHECK：生产Runtime从THPrac练习实局生成并保存Web录像，在工作区打开、定位到第60帧；游戏内暂停菜单重播同一录像，回到记录的起始关卡第0帧。

### 2026-10-20：全屏 Esc 与统一播放器一致

- INPUT：进入播放器全屏时立即锁定 Esc，覆盖 DATA、字体及 Runtime 初始化等待；短按保留暂停/返回输入，长按由浏览器退出全屏。加载取消和退出全屏释放键盘锁。
- CHECK：实际 BrowserLauncher 挂载检查在原生 Ready 延迟时已请求 Escape 锁，并验证取消加载后的释放。

### 2026-10-20：紫版无敌撞弹音效

- THPRAC：无敌撞弹保留原版453d90的掉残音效和6帧碰撞保护，自机保持正常状态，残机、火力、Bomb及Miss记录保持不变；回放不施加实时无敌。
- CHECK：完整C++运行库验证音效ID2、6帧保护、再次撞弹、资源与随机数状态，以及回放隔离。

### 2026-10-20：手机高级状态信息与通关排行

- UI：手机高级状态信息按完整窗口高度贴底，折叠与展开均保留5逻辑像素边距；触控复用TH15的只读面板点击／滑动分流，补发原始按下坐标。
- FLOW：不参与排行的Story及Extra通关也展示Score Ranking，保留成绩写入规则及后续Replay／结果菜单。
- CHECK：[验证入口与行为边界](../../docs/adaptation/th12-mobile-information-20261020.md)。

### 2026-10-22：第一关开场收点线

- PRESENTATION：按原版41d8ea..41d91e的HUD初始化条件创建front51及49／50子动画，使用原资源的展开、闪烁和退出时序；第一关非录像且续关计数为0时显示。
- CHECK：Stage Flow的零售ANM回归覆盖首次进入、动画退出、第二关、续关、普通录像、THPrac录像和第一关Practice。
- RENDER：生产Wasm与WebGL渲染器核对第0／65／76／200帧，收点线和两侧子动画位于原版y=144，展开完成后显示，退出后清除。

### 2026-10-22：对话动画回收与暂停保存录像

- MSG：对话对象统一持有立绘、对话框及文字动画；退出指令释放公开句柄后，退出动画仍归该对话所有。MSG 结束和下一段对话开始时回收整组动画，Ctrl 连续跳过时不会把立绘带入切关。
- REPLAY：暂停菜单的保存录像操作封存当前输入后直接进入游戏画面内的保存页，沿用暂停背景，不经过疮痍、排行榜或续关流程。
- CHECK：零售 ANM 与生产 Wasm 覆盖普通等待、Ctrl 跳过及同时间点退出／结束指令；菜单回归覆盖保存分流和既有录像重播／加速行为。

## SDL3 共用应用

桌面与 Web 应用层的资源、流程、绘制和练习工具统一由 [SDL3 运行层](portable/sdl/README.md) 管理。普通版提供单独的 SDL Web 人工验收包，既有正式 Web 构建保留；多人版同步平台库并继续使用原多人会话。构建入口和录像、音乐、存档的归属见该指南。
