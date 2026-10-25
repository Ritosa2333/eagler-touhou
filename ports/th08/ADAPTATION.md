# th08 适配检查

[开发入口](README.md) | [检查方法](../../docs/adaptation/README.md) | [全部作品](../README.md)

源码目录：`ports/th08`。构建模式：`normal`。

实现情况与实际验收分别记录。源码接口和模块检查通过后记为“已实现”；完成对应操作并附执行记录后记为“已验证”。

状态：`待核对`、`进行中`、`待验证`、`已实现`、`已验证`、`检查失败`、`不适用`。

[自动检查结果与源码身份](../../docs/adaptation/status.json) | `npm run ports:check -- --game=th08 --variant=both`

运行验收快照：2026-10-25。[逐项结果、范围与证据](docs/verification/adaptation.md) | [结构化记录](docs/verification/adaptation.json)。

源码检查列记录接口、模块与构建检查；验收记录列记录各次运行所绑定的源码和产物。后续版本按相应身份复验。部分验证的已测行为与剩余范围见逐项记录。

## 源码与构建

| 编号 | 检查项 | normal 源码检查 | normal 验收记录 | 记录 |
| --- | --- | --- | --- | --- |
| BUILD-01 | [源码目录与普通版／MP归属](../../docs/adaptation/build.md#build-01) | 已验证 | [已验证](docs/verification/adaptation.md#build-01) | normal：[source-inputs](../../docs/adaptation/status.json) |
| BUILD-02 | [工具链与外部依赖](../../docs/adaptation/build.md#build-02) | 待核对 | [已验证](docs/verification/adaptation.md#build-02) | normal：此项尚未登记源码检查结果 |
| BUILD-03 | [对应模式独立构建](../../docs/adaptation/build.md#build-03) | 待核对 | [已验证](docs/verification/adaptation.md#build-03) | normal：此项尚未登记源码检查结果 |
| BUILD-04 | [Runtime 文件与来源记录](../../docs/adaptation/build.md#build-04) | 待核对 | [已验证](docs/verification/adaptation.md#build-04) | normal：此项尚未登记源码检查结果 |
| BUILD-05 | [前端搭配单个游戏运行](../../docs/adaptation/build.md#build-05) | 待核对 | [已验证](docs/verification/adaptation.md#build-05) | normal：此项尚未登记源码检查结果 |
| BUILD-06 | [源码与素材来源](../../docs/adaptation/build.md#build-06) | 待核对 | [待材料](docs/verification/adaptation.md#build-06) | normal：此项尚未登记源码检查结果 |

## 启动与资源

| 编号 | 检查项 | normal 源码检查 | normal 验收记录 | 记录 |
| --- | --- | --- | --- | --- |
| RUNTIME-01 | [协议身份与会话代次](../../docs/adaptation/runtime.md#runtime-01) | 已实现 | [已验证](docs/verification/adaptation.md#runtime-01) | normal：[runtime-protocol](../../docs/adaptation/status.json) |
| RUNTIME-02 | [就绪、配置、启动、首帧](../../docs/adaptation/runtime.md#runtime-02) | 已实现 | [已验证](docs/verification/adaptation.md#runtime-02) | normal：[runtime-protocol](../../docs/adaptation/status.json) |
| RUNTIME-03 | [命令响应与错误信息](../../docs/adaptation/runtime.md#runtime-03) | 已实现 | [已验证](docs/verification/adaptation.md#runtime-03) | normal：[runtime-protocol](../../docs/adaptation/status.json) |
| RUNTIME-04 | [后台、恢复与退出清理](../../docs/adaptation/runtime.md#runtime-04) | 待核对 | [已验证](docs/verification/adaptation.md#runtime-04) | normal：此项尚未登记源码检查结果 |
| RUNTIME-05 | [Runtime 与资源管理分工](../../docs/adaptation/runtime.md#runtime-05) | 待核对 | [已验证](docs/verification/adaptation.md#runtime-05) | normal：此项尚未登记源码检查结果 |
| RUNTIME-06 | [重开与常显判定点](../../docs/adaptation/runtime.md#runtime-06) | 待核对 | [部分验证](docs/verification/adaptation.md#runtime-06) | normal：此项尚未登记源码检查结果 |
| PACKAGE-01 | [共用游戏包存储](../../docs/adaptation/runtime.md#package-01) | 已实现 | [已验证](docs/verification/adaptation.md#package-01) | normal：[package-store](../../docs/adaptation/status.json) |
| PACKAGE-02 | [DATA 身份与挂载目标](../../docs/adaptation/runtime.md#package-02) | 已实现 | [已验证](docs/verification/adaptation.md#package-02) | normal：[package-store](../../docs/adaptation/status.json) |
| PACKAGE-03 | [完整更新后切换版本](../../docs/adaptation/runtime.md#package-03) | 已实现 | [已验证](docs/verification/adaptation.md#package-03) | normal：[package-store](../../docs/adaptation/status.json) |
| PACKAGE-04 | [本地导入与在线安装一致](../../docs/adaptation/runtime.md#package-04) | 已实现 | [已验证](docs/verification/adaptation.md#package-04) | normal：[package-store](../../docs/adaptation/status.json) |
| PACKAGE-05 | [已安装游戏离线启动](../../docs/adaptation/runtime.md#package-05) | 待核对 | [已验证](docs/verification/adaptation.md#package-05) | normal：此项尚未登记源码检查结果 |
| PACKAGE-06 | [统一安装和更新入口](../../docs/adaptation/runtime.md#package-06) | 已实现 | [已验证](docs/verification/adaptation.md#package-06) | normal：[package-store](../../docs/adaptation/status.json) |

## 键盘、手柄与触控

| 编号 | 检查项 | normal 源码检查 | normal 验收记录 | 记录 |
| --- | --- | --- | --- | --- |
| INPUT-01 | [键盘逻辑输入](../../docs/adaptation/input.md#input-01) | 待核对 | [已验证](docs/verification/adaptation.md#input-01) | normal：此项尚未登记源码检查结果 |
| INPUT-02 | [启动器键盘改键](../../docs/adaptation/input.md#input-02) | 已实现 | [已验证](docs/verification/adaptation.md#input-02) | normal：[launcher-input-owners](../../docs/adaptation/status.json) |
| INPUT-03 | [启动器手柄映射](../../docs/adaptation/input.md#input-03) | 已实现 | [已验证](docs/verification/adaptation.md#input-03) | normal：[launcher-input-owners](../../docs/adaptation/status.json) |
| INPUT-04 | [手柄方向与按钮](../../docs/adaptation/input.md#input-04) | 已实现 | [已验证](docs/verification/adaptation.md#input-04) | normal：[native-gamepad](../../docs/adaptation/status.json) |
| INPUT-05 | [多种输入独立持有](../../docs/adaptation/input.md#input-05) | 已实现 | [已验证](docs/verification/adaptation.md#input-05) | normal：[launcher-input-owners](../../docs/adaptation/status.json) |
| INPUT-06 | [失焦与断开释放](../../docs/adaptation/input.md#input-06) | 待核对 | [已验证](docs/verification/adaptation.md#input-06) | normal：此项尚未登记源码检查结果 |
| INPUT-07 | [原作菜单与对话输入](../../docs/adaptation/input.md#input-07) | 待核对 | [已验证](docs/verification/adaptation.md#input-07) | normal：此项尚未登记源码检查结果 |
| TOUCH-01 | [限速滑动移动](../../docs/adaptation/input.md#touch-01) | 待核对 | [已验证](docs/verification/adaptation.md#touch-01) | normal：此项尚未登记源码检查结果 |
| TOUCH-02 | [无限速滑动移动](../../docs/adaptation/input.md#touch-02) | 待核对 | [已验证](docs/verification/adaptation.md#touch-02) | normal：此项尚未登记源码检查结果 |
| TOUCH-03 | [方向式虚拟摇杆](../../docs/adaptation/input.md#touch-03) | 待核对 | [已验证](docs/verification/adaptation.md#touch-03) | normal：此项尚未登记源码检查结果 |
| TOUCH-04 | [自由方向虚拟摇杆](../../docs/adaptation/input.md#touch-04) | 待核对 | [已验证](docs/verification/adaptation.md#touch-04) | normal：此项尚未登记源码检查结果 |
| TOUCH-05 | [按住低速](../../docs/adaptation/input.md#touch-05) | 待核对 | [已验证](docs/verification/adaptation.md#touch-05) | normal：此项尚未登记源码检查结果 |
| TOUCH-06 | [切换低速](../../docs/adaptation/input.md#touch-06) | 待核对 | [已验证](docs/verification/adaptation.md#touch-06) | normal：此项尚未登记源码检查结果 |
| TOUCH-07 | [双指低速](../../docs/adaptation/input.md#touch-07) | 待核对 | [已验证](docs/verification/adaptation.md#touch-07) | normal：此项尚未登记源码检查结果 |
| TOUCH-08 | [射击、Bomb 与暂停](../../docs/adaptation/input.md#touch-08) | 待核对 | [已验证](docs/verification/adaptation.md#touch-08) | normal：此项尚未登记源码检查结果 |
| TOUCH-09 | [Deathbomb 与移动保持](../../docs/adaptation/input.md#touch-09) | 待核对 | [已验证](docs/verification/adaptation.md#touch-09) | normal：此项尚未登记源码检查结果 |
| TOUCH-10 | [无限速移动落率记录](../../docs/adaptation/input.md#touch-10) | 待核对 | [已验证](docs/verification/adaptation.md#touch-10) | normal：此项尚未登记源码检查结果 |
| TOUCH-11 | [双击 Bomb](../../docs/adaptation/input.md#touch-11) | 待核对 | [已验证](docs/verification/adaptation.md#touch-11) | normal：此项尚未登记源码检查结果 |
| TOUCH-12 | [菜单与对话手势](../../docs/adaptation/input.md#touch-12) | 待核对 | [已验证](docs/verification/adaptation.md#touch-12) | normal：此项尚未登记源码检查结果 |
| TOUCH-13 | [确认键的新按下边沿](../../docs/adaptation/input.md#touch-13) | 待核对 | [已验证](docs/verification/adaptation.md#touch-13) | normal：此项尚未登记源码检查结果 |
| TOUCH-14 | [100–300% 灵敏度](../../docs/adaptation/input.md#touch-14) | 已实现 | [已验证](docs/verification/adaptation.md#touch-14) | normal：[touch-sensitivity](../../docs/adaptation/status.json) |
| TOUCH-15 | [触控与其他输入隔离](../../docs/adaptation/input.md#touch-15) | 待核对 | [已验证](docs/verification/adaptation.md#touch-15) | normal：此项尚未登记源码检查结果 |
| TOUCH-16 | [触控取消与生命周期](../../docs/adaptation/input.md#touch-16) | 待核对 | [已验证](docs/verification/adaptation.md#touch-16) | normal：此项尚未登记源码检查结果 |
| TOUCH-17 | [共用布局编辑器](../../docs/adaptation/input.md#touch-17) | 已实现 | [已验证](docs/verification/adaptation.md#touch-17) | normal：[shared-touch-controls](../../docs/adaptation/status.json) |
| TOUCH-18 | [共用重开按钮](../../docs/adaptation/input.md#touch-18) | 已实现 | [已验证](docs/verification/adaptation.md#touch-18) | normal：[shared-touch-controls](../../docs/adaptation/status.json) |
| TOUCH-19 | [共用双指放大镜](../../docs/adaptation/input.md#touch-19) | 已实现 | [已验证](docs/verification/adaptation.md#touch-19) | normal：[shared-touch-controls](../../docs/adaptation/status.json) |

## 画面与声音

| 编号 | 检查项 | normal 源码检查 | normal 验收记录 | 记录 |
| --- | --- | --- | --- | --- |
| DISPLAY-01 | [固定逻辑步进](../../docs/adaptation/presentation.md#display-01) | 已实现 | [已验证](docs/verification/adaptation.md#display-01) | normal：[normal-presentation](../../docs/adaptation/status.json) |
| DISPLAY-02 | [跟随显示器刷新率](../../docs/adaptation/presentation.md#display-02) | 已实现 | [部分验证](docs/verification/adaptation.md#display-02) | normal：[normal-presentation](../../docs/adaptation/status.json) |
| DISPLAY-03 | [额外绘制无逻辑副作用](../../docs/adaptation/presentation.md#display-03) | 已实现 | [已验证](docs/verification/adaptation.md#display-03) | normal：[normal-presentation](../../docs/adaptation/status.json) |
| DISPLAY-04 | [连续视觉字段插值](../../docs/adaptation/presentation.md#display-04) | 待核对 | [部分验证](docs/verification/adaptation.md#display-04) | normal：此项尚未登记源码检查结果 |
| DISPLAY-05 | [出生、销毁和瞬移](../../docs/adaptation/presentation.md#display-05) | 待核对 | [部分验证](docs/verification/adaptation.md#display-05) | normal：此项尚未登记源码检查结果 |
| DISPLAY-06 | [暂停画面的表现](../../docs/adaptation/presentation.md#display-06) | 待核对 | [部分验证](docs/verification/adaptation.md#display-06) | normal：此项尚未登记源码检查结果 |
| DISPLAY-07 | [实际呈现后报告首帧](../../docs/adaptation/presentation.md#display-07) | 待核对 | [已验证](docs/verification/adaptation.md#display-07) | normal：此项尚未登记源码检查结果 |
| DISPLAY-08 | [限制到 60 Hz](../../docs/adaptation/presentation.md#display-08) | 待核对 | [部分验证](docs/verification/adaptation.md#display-08) | normal：此项尚未登记源码检查结果 |
| AUDIO-01 | [正常 OGG 音乐](../../docs/adaptation/presentation.md#audio-01) | 待核对 | [已验证](docs/verification/adaptation.md#audio-01) | normal：此项尚未登记源码检查结果 |
| AUDIO-02 | [无 BGM 模式](../../docs/adaptation/presentation.md#audio-02) | 待核对 | [已验证](docs/verification/adaptation.md#audio-02) | normal：此项尚未登记源码检查结果 |
| AUDIO-03 | [切曲、渐变、暂停、停止](../../docs/adaptation/presentation.md#audio-03) | 待核对 | [已验证](docs/verification/adaptation.md#audio-03) | normal：此项尚未登记源码检查结果 |
| AUDIO-04 | [返回前台后恢复声音](../../docs/adaptation/presentation.md#audio-04) | 待核对 | [已验证](docs/verification/adaptation.md#audio-04) | normal：此项尚未登记源码检查结果 |
| AUDIO-05 | [音乐资源错误信息](../../docs/adaptation/presentation.md#audio-05) | 待核对 | [已验证](docs/verification/adaptation.md#audio-05) | normal：此项尚未登记源码检查结果 |
| AUDIO-06 | [高刷新与音频事件分开](../../docs/adaptation/presentation.md#audio-06) | 待核对 | [已验证](docs/verification/adaptation.md#audio-06) | normal：此项尚未登记源码检查结果 |
| MIDI-01 | [声明 MIDI 选项](../../docs/adaptation/presentation.md#midi-01) | 待核对 | [已验证](docs/verification/adaptation.md#midi-01) | normal：此项尚未登记源码检查结果 |
| MIDI-02 | [可听到的 MIDI 输出](../../docs/adaptation/presentation.md#midi-02) | 待核对 | [已验证](docs/verification/adaptation.md#midi-02) | normal：此项尚未登记源码检查结果 |
| MIDI-03 | [MIDI 生命周期](../../docs/adaptation/presentation.md#midi-03) | 待核对 | [已验证](docs/verification/adaptation.md#midi-03) | normal：此项尚未登记源码检查结果 |
| MIDI-04 | [MIDI 与 OGG 分开](../../docs/adaptation/presentation.md#midi-04) | 待核对 | [已验证](docs/verification/adaptation.md#midi-04) | normal：此项尚未登记源码检查结果 |

## 存档与 Replay

| 编号 | 检查项 | normal 源码检查 | normal 验收记录 | 记录 |
| --- | --- | --- | --- | --- |
| REPLAY-01 | [导入原版 Replay](../../docs/adaptation/storage.md#replay-01) | 待核对 | [部分验证](docs/verification/adaptation.md#replay-01) | normal：此项尚未登记源码检查结果 |
| REPLAY-02 | [按原版格式导出可表达输入](../../docs/adaptation/storage.md#replay-02) | 待核对 | [已验证](docs/verification/adaptation.md#replay-02) | normal：此项尚未登记源码检查结果 |
| REPLAY-03 | [记录实际生效输入](../../docs/adaptation/storage.md#replay-03) | 待核对 | [已验证](docs/verification/adaptation.md#replay-03) | normal：此项尚未登记源码检查结果 |
| REPLAY-04 | [扩展格式与版本](../../docs/adaptation/storage.md#replay-04) | 待核对 | [已验证](docs/verification/adaptation.md#replay-04) | normal：此项尚未登记源码检查结果 |
| REPLAY-05 | [Replay 文件管理](../../docs/adaptation/storage.md#replay-05) | 待核对 | [已验证](docs/verification/adaptation.md#replay-05) | normal：此项尚未登记源码检查结果 |
| REPLAY-06 | [回放与实时输入隔离](../../docs/adaptation/storage.md#replay-06) | 待核对 | [已验证](docs/verification/adaptation.md#replay-06) | normal：此项尚未登记源码检查结果 |
| REPLAY-07 | [回放独立于显示刷新率](../../docs/adaptation/storage.md#replay-07) | 待核对 | [已验证](docs/verification/adaptation.md#replay-07) | normal：此项尚未登记源码检查结果 |
| SAVE-01 | [已声明的存储根](../../docs/adaptation/storage.md#save-01) | 待核对 | [已验证](docs/verification/adaptation.md#save-01) | normal：此项尚未登记源码检查结果 |
| SAVE-02 | [用户文件允许范围](../../docs/adaptation/storage.md#save-02) | 待核对 | [已验证](docs/verification/adaptation.md#save-02) | normal：此项尚未登记源码检查结果 |
| SAVE-03 | [明确持久化时机](../../docs/adaptation/storage.md#save-03) | 待核对 | [已验证](docs/verification/adaptation.md#save-03) | normal：此项尚未登记源码检查结果 |
| SAVE-04 | [退出重开后数据保留](../../docs/adaptation/storage.md#save-04) | 待核对 | [已验证](docs/verification/adaptation.md#save-04) | normal：此项尚未登记源码检查结果 |
| SAVE-05 | [离线读取已有数据](../../docs/adaptation/storage.md#save-05) | 待核对 | [已验证](docs/verification/adaptation.md#save-05) | normal：此项尚未登记源码检查结果 |
| SAVE-06 | [游戏包与用户数据分开](../../docs/adaptation/storage.md#save-06) | 待核对 | [已验证](docs/verification/adaptation.md#save-06) | normal：此项尚未登记源码检查结果 |

## 语言与练习

| 编号 | 检查项 | normal 源码检查 | normal 验收记录 | 记录 |
| --- | --- | --- | --- | --- |
| PRACTICE-01 | [发布包中的练习能力](../../docs/adaptation/localization.md#practice-01) | 待核对 | [已验证](docs/verification/adaptation.md#practice-01) | normal：此项尚未登记源码检查结果 |
| PRACTICE-02 | [开局前启用与语言](../../docs/adaptation/localization.md#practice-02) | 待核对 | [已验证](docs/verification/adaptation.md#practice-02) | normal：此项尚未登记源码检查结果 |
| PRACTICE-03 | [游戏内练习状态](../../docs/adaptation/localization.md#practice-03) | 待核对 | [已验证](docs/verification/adaptation.md#practice-03) | normal：此项尚未登记源码检查结果 |
| PRACTICE-04 | [关卡与资源参数](../../docs/adaptation/localization.md#practice-04) | 待核对 | [已验证](docs/verification/adaptation.md#practice-04) | normal：此项尚未登记源码检查结果 |
| PRACTICE-05 | [练习 Replay 元数据](../../docs/adaptation/localization.md#practice-05) | 待核对 | [已验证](docs/verification/adaptation.md#practice-05) | normal：此项尚未登记源码检查结果 |
| PRACTICE-06 | [触控功能键与指针](../../docs/adaptation/localization.md#practice-06) | 待核对 | [已验证](docs/verification/adaptation.md#practice-06) | normal：此项尚未登记源码检查结果 |
| PRACTICE-07 | [练习语言跟随设置](../../docs/adaptation/localization.md#practice-07) | 待核对 | [已验证](docs/verification/adaptation.md#practice-07) | normal：此项尚未登记源码检查结果 |
| PRACTICE-08 | [练习界面的可用功能](../../docs/adaptation/localization.md#practice-08) | 待核对 | [部分验证](docs/verification/adaptation.md#practice-08) | normal：此项尚未登记源码检查结果 |
| LANG-01 | [内置日语原文](../../docs/adaptation/localization.md#lang-01) | 待核对 | [已验证](docs/verification/adaptation.md#lang-01) | normal：此项尚未登记源码检查结果 |
| LANG-02 | [语言包身份与发布清单](../../docs/adaptation/localization.md#lang-02) | 待核对 | [已验证](docs/verification/adaptation.md#lang-02) | normal：此项尚未登记源码检查结果 |
| LANG-03 | [开局前挂载翻译与字体](../../docs/adaptation/localization.md#lang-03) | 待核对 | [已验证](docs/verification/adaptation.md#lang-03) | normal：此项尚未登记源码检查结果 |
| LANG-04 | [语言包失败时回到原文](../../docs/adaptation/localization.md#lang-04) | 待核对 | [已验证](docs/verification/adaptation.md#lang-04) | normal：此项尚未登记源码检查结果 |
| LANG-05 | [字体字符覆盖](../../docs/adaptation/localization.md#lang-05) | 待核对 | [部分验证](docs/verification/adaptation.md#lang-05) | normal：此项尚未登记源码检查结果 |
| LANG-06 | [缺译资源使用原文](../../docs/adaptation/localization.md#lang-06) | 待核对 | [已验证](docs/verification/adaptation.md#lang-06) | normal：此项尚未登记源码检查结果 |

## 多人接入与规则

| 编号 | 检查项 | normal 源码检查 | normal 验收记录 | 记录 |
| --- | --- | --- | --- | --- |
| MP-RULE-01 | [死亡、幽灵与救援](../../docs/adaptation/multiplayer.md#mp-rule-01) | 不适用 | [不适用](docs/verification/adaptation.md#mp-rule-01) | normal：单人模式 |
| MP-RULE-02 | [个人资源与团队资源](../../docs/adaptation/multiplayer.md#mp-rule-02) | 不适用 | [不适用](docs/verification/adaptation.md#mp-rule-02) | normal：单人模式 |
| MP-RULE-03 | [给予资源与反馈](../../docs/adaptation/multiplayer.md#mp-rule-03) | 不适用 | [不适用](docs/verification/adaptation.md#mp-rule-03) | normal：单人模式 |
| MP-RULE-04 | [对话、结算与全灭](../../docs/adaptation/multiplayer.md#mp-rule-04) | 不适用 | [不适用](docs/verification/adaptation.md#mp-rule-04) | normal：单人模式 |
| MP-RULE-05 | [自机可见性与多人HUD](../../docs/adaptation/multiplayer.md#mp-rule-05) | 不适用 | [不适用](docs/verification/adaptation.md#mp-rule-05) | normal：单人模式 |
| MP-01 | [独立 MP 产品变体](../../docs/adaptation/multiplayer.md#mp-01) | 不适用 | [不适用](docs/verification/adaptation.md#mp-01) | normal：单人模式 |
| MP-02 | [房间人数、难度与机体](../../docs/adaptation/multiplayer.md#mp-02) | 不适用 | [不适用](docs/verification/adaptation.md#mp-02) | normal：单人模式 |
| MP-03 | [所有席位准备并同步开局](../../docs/adaptation/multiplayer.md#mp-03) | 不适用 | [不适用](docs/verification/adaptation.md#mp-03) | normal：单人模式 |
| MP-04 | [确定性输入与状态检查](../../docs/adaptation/multiplayer.md#mp-04) | 不适用 | [不适用](docs/verification/adaptation.md#mp-04) | normal：单人模式 |
| MP-05 | [各席位输入归属](../../docs/adaptation/multiplayer.md#mp-05) | 不适用 | [不适用](docs/verification/adaptation.md#mp-05) | normal：单人模式 |
| MP-06 | [旁观加入与输入隔离](../../docs/adaptation/multiplayer.md#mp-06) | 不适用 | [不适用](docs/verification/adaptation.md#mp-06) | normal：单人模式 |
| MP-07 | [旁观只读取连续已确认历史](../../docs/adaptation/multiplayer.md#mp-07) | 不适用 | [不适用](docs/verification/adaptation.md#mp-07) | normal：单人模式 |
| MP-08 | [慢旁观者与玩家隔离](../../docs/adaptation/multiplayer.md#mp-08) | 不适用 | [不适用](docs/verification/adaptation.md#mp-08) | normal：单人模式 |
| MP-09 | [旁观尾帧与结束时机](../../docs/adaptation/multiplayer.md#mp-09) | 不适用 | [不适用](docs/verification/adaptation.md#mp-09) | normal：单人模式 |
| MP-10 | [联机暂停与重开](../../docs/adaptation/multiplayer.md#mp-10) | 不适用 | [不适用](docs/verification/adaptation.md#mp-10) | normal：单人模式 |
| MP-11 | [多人 Replay](../../docs/adaptation/multiplayer.md#mp-11) | 不适用 | [不适用](docs/verification/adaptation.md#mp-11) | normal：单人模式 |
| MP-12 | [WebRTC 与 Relay 路径](../../docs/adaptation/multiplayer.md#mp-12) | 不适用 | [不适用](docs/verification/adaptation.md#mp-12) | normal：单人模式 |
| MP-13 | [本地玩家可见性](../../docs/adaptation/multiplayer.md#mp-13) | 不适用 | [不适用](docs/verification/adaptation.md#mp-13) | normal：单人模式 |
| MP-14 | [联机与回滚诊断](../../docs/adaptation/multiplayer.md#mp-14) | 不适用 | [不适用](docs/verification/adaptation.md#mp-14) | normal：单人模式 |
| MP-15 | [游戏结束返回房间](../../docs/adaptation/multiplayer.md#mp-15) | 不适用 | [不适用](docs/verification/adaptation.md#mp-15) | normal：单人模式 |

## 验证与交付

| 编号 | 检查项 | normal 源码检查 | normal 验收记录 | 记录 |
| --- | --- | --- | --- | --- |
| VERIFY-01 | [全部内置 Demo 对照](../../docs/adaptation/validation.md#verify-01) | 待核对 | [检查失败](docs/verification/adaptation.md#verify-01) | normal：此项尚未登记源码检查结果 |
| VERIFY-02 | [长 Replay 样本](../../docs/adaptation/validation.md#verify-02) | 待核对 | [检查失败](docs/verification/adaptation.md#verify-02) | normal：此项尚未登记源码检查结果 |
| VERIFY-03 | [原版基准与内容哈希](../../docs/adaptation/validation.md#verify-03) | 待核对 | [部分验证](docs/verification/adaptation.md#verify-03) | normal：此项尚未登记源码检查结果 |
| VERIFY-04 | [读取固定基准](../../docs/adaptation/validation.md#verify-04) | 待核对 | [已验证](docs/verification/adaptation.md#verify-04) | normal：此项尚未登记源码检查结果 |
| VERIFY-05 | [单独维护原版基准](../../docs/adaptation/validation.md#verify-05) | 待核对 | [待材料](docs/verification/adaptation.md#verify-05) | normal：此项尚未登记源码检查结果 |
| VERIFY-06 | [逐逻辑帧比较](../../docs/adaptation/validation.md#verify-06) | 待核对 | [已验证](docs/verification/adaptation.md#verify-06) | normal：此项尚未登记源码检查结果 |
| VERIFY-07 | [诊断构建与正常构建](../../docs/adaptation/validation.md#verify-07) | 待核对 | [已验证](docs/verification/adaptation.md#verify-07) | normal：此项尚未登记源码检查结果 |
| VERIFY-08 | [本作状态映射与结束条件](../../docs/adaptation/validation.md#verify-08) | 待核对 | [部分验证](docs/verification/adaptation.md#verify-08) | normal：此项尚未登记源码检查结果 |

<!-- manual-verification -->

## 运行验收记录

按检查编号附[验证记录](../../docs/adaptation/record-template.md)，写明源码身份、模式、环境、操作与结果。自动更新保留本节内容。

本轮桌面协议、输入、文件持久化与实际 MP 房间结果见[逐作运行记录](../../docs/adaptation/desktop-20261018.md)。按作品、模式和分项读取通过、失败及待复验结果，原始报告身份见同页机器记录。

2026-10-24 的逐项证据、源码与产物范围见[运行验收记录](docs/verification/adaptation.md)，[结构化快照](docs/verification/adaptation.json)供表格生成复用。源码检查与运行验收分别展示。
