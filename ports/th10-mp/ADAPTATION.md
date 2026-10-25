# th10 MP 适配检查

[开发入口](README.md) · [检查方法](../../docs/adaptation/README.md) · [全部作品](../README.md)

源码目录：`ports/th10-mp`。构建模式：`multiplayer`。

实现情况与实际验收分别记录。源码接口和模块检查通过后记为“已实现”；完成对应操作并附执行记录后记为“已验证”。

状态：`待核对`、`进行中`、`待验证`、`已实现`、`已验证`、`检查失败`、`不适用`。

[自动检查结果与源码身份](../../docs/adaptation/status.json) · `npm run ports:check -- --game=th10 --variant=both`

## 源码与构建

| 编号 | 检查项 | multiplayer | 记录 |
| --- | --- | --- | --- |
| BUILD-01 | [源码目录与普通版／MP归属](../../docs/adaptation/build.md#build-01) | 已验证 | multiplayer：[source-inputs](../../docs/adaptation/status.json) |
| BUILD-02 | [工具链与外部依赖](../../docs/adaptation/build.md#build-02) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| BUILD-03 | [对应模式独立构建](../../docs/adaptation/build.md#build-03) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| BUILD-04 | [Runtime 文件与来源记录](../../docs/adaptation/build.md#build-04) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| BUILD-05 | [前端搭配单个游戏运行](../../docs/adaptation/build.md#build-05) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| BUILD-06 | [源码与素材来源](../../docs/adaptation/build.md#build-06) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |

## 启动与资源

| 编号 | 检查项 | multiplayer | 记录 |
| --- | --- | --- | --- |
| RUNTIME-01 | [协议身份与会话代次](../../docs/adaptation/runtime.md#runtime-01) | 已实现 | multiplayer：[runtime-protocol](../../docs/adaptation/status.json) |
| RUNTIME-02 | [就绪、配置、启动、首帧](../../docs/adaptation/runtime.md#runtime-02) | 已实现 | multiplayer：[runtime-protocol](../../docs/adaptation/status.json) |
| RUNTIME-03 | [命令响应与错误信息](../../docs/adaptation/runtime.md#runtime-03) | 已实现 | multiplayer：[runtime-protocol](../../docs/adaptation/status.json) |
| RUNTIME-04 | [后台、恢复与退出清理](../../docs/adaptation/runtime.md#runtime-04) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| RUNTIME-05 | [Runtime 与资源管理分工](../../docs/adaptation/runtime.md#runtime-05) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| RUNTIME-06 | [重开与常显判定点](../../docs/adaptation/runtime.md#runtime-06) | 不适用 | multiplayer：本作未声明判定点显示 |
| PACKAGE-01 | [共用游戏包存储](../../docs/adaptation/runtime.md#package-01) | 已实现 | multiplayer：[package-store](../../docs/adaptation/status.json) |
| PACKAGE-02 | [DATA 身份与挂载目标](../../docs/adaptation/runtime.md#package-02) | 已实现 | multiplayer：[package-store](../../docs/adaptation/status.json) |
| PACKAGE-03 | [完整更新后切换版本](../../docs/adaptation/runtime.md#package-03) | 已实现 | multiplayer：[package-store](../../docs/adaptation/status.json) |
| PACKAGE-04 | [本地导入与在线安装一致](../../docs/adaptation/runtime.md#package-04) | 已实现 | multiplayer：[package-store](../../docs/adaptation/status.json) |
| PACKAGE-05 | [已安装游戏离线启动](../../docs/adaptation/runtime.md#package-05) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| PACKAGE-06 | [统一安装和更新入口](../../docs/adaptation/runtime.md#package-06) | 已实现 | multiplayer：[package-store](../../docs/adaptation/status.json) |

## 键盘、手柄与触控

| 编号 | 检查项 | multiplayer | 记录 |
| --- | --- | --- | --- |
| INPUT-01 | [键盘逻辑输入](../../docs/adaptation/input.md#input-01) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| INPUT-02 | [启动器键盘改键](../../docs/adaptation/input.md#input-02) | 已实现 | multiplayer：[launcher-input-owners](../../docs/adaptation/status.json) |
| INPUT-03 | [启动器手柄映射](../../docs/adaptation/input.md#input-03) | 已实现 | multiplayer：[launcher-input-owners](../../docs/adaptation/status.json) |
| INPUT-04 | [手柄方向与按钮](../../docs/adaptation/input.md#input-04) | 已实现 | multiplayer：[native-gamepad](../../docs/adaptation/status.json) |
| INPUT-05 | [多种输入独立持有](../../docs/adaptation/input.md#input-05) | 已实现 | multiplayer：[launcher-input-owners](../../docs/adaptation/status.json) |
| INPUT-06 | [失焦与断开释放](../../docs/adaptation/input.md#input-06) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| INPUT-07 | [原作菜单与对话输入](../../docs/adaptation/input.md#input-07) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| TOUCH-01 | [限速滑动移动](../../docs/adaptation/input.md#touch-01) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| TOUCH-02 | [无限速滑动移动](../../docs/adaptation/input.md#touch-02) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| TOUCH-03 | [方向式虚拟摇杆](../../docs/adaptation/input.md#touch-03) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| TOUCH-04 | [自由方向虚拟摇杆](../../docs/adaptation/input.md#touch-04) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| TOUCH-05 | [按住低速](../../docs/adaptation/input.md#touch-05) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| TOUCH-06 | [切换低速](../../docs/adaptation/input.md#touch-06) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| TOUCH-07 | [双指低速](../../docs/adaptation/input.md#touch-07) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| TOUCH-08 | [射击、Bomb 与暂停](../../docs/adaptation/input.md#touch-08) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| TOUCH-09 | [Deathbomb 与移动保持](../../docs/adaptation/input.md#touch-09) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| TOUCH-10 | [无限速移动落率记录](../../docs/adaptation/input.md#touch-10) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| TOUCH-11 | [双击 Bomb](../../docs/adaptation/input.md#touch-11) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| TOUCH-12 | [菜单与对话手势](../../docs/adaptation/input.md#touch-12) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| TOUCH-13 | [确认键的新按下边沿](../../docs/adaptation/input.md#touch-13) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| TOUCH-14 | [100–300% 灵敏度](../../docs/adaptation/input.md#touch-14) | 已实现 | multiplayer：[touch-sensitivity](../../docs/adaptation/status.json) |
| TOUCH-15 | [触控与其他输入隔离](../../docs/adaptation/input.md#touch-15) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| TOUCH-16 | [触控取消与生命周期](../../docs/adaptation/input.md#touch-16) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| TOUCH-17 | [共用布局编辑器](../../docs/adaptation/input.md#touch-17) | 已实现 | multiplayer：[shared-touch-controls](../../docs/adaptation/status.json) |
| TOUCH-18 | [共用重开按钮](../../docs/adaptation/input.md#touch-18) | 已实现 | multiplayer：[shared-touch-controls](../../docs/adaptation/status.json) |
| TOUCH-19 | [共用双指放大镜](../../docs/adaptation/input.md#touch-19) | 已实现 | multiplayer：[shared-touch-controls](../../docs/adaptation/status.json) |

## 画面与声音

| 编号 | 检查项 | multiplayer | 记录 |
| --- | --- | --- | --- |
| DISPLAY-01 | [固定逻辑步进](../../docs/adaptation/presentation.md#display-01) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| DISPLAY-02 | [跟随显示器刷新率](../../docs/adaptation/presentation.md#display-02) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| DISPLAY-03 | [额外绘制无逻辑副作用](../../docs/adaptation/presentation.md#display-03) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| DISPLAY-04 | [连续视觉字段插值](../../docs/adaptation/presentation.md#display-04) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| DISPLAY-05 | [出生、销毁和瞬移](../../docs/adaptation/presentation.md#display-05) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| DISPLAY-06 | [暂停画面的表现](../../docs/adaptation/presentation.md#display-06) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| DISPLAY-07 | [实际呈现后报告首帧](../../docs/adaptation/presentation.md#display-07) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| DISPLAY-08 | [限制到 60 Hz](../../docs/adaptation/presentation.md#display-08) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| AUDIO-01 | [正常 OGG 音乐](../../docs/adaptation/presentation.md#audio-01) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| AUDIO-02 | [无 BGM 模式](../../docs/adaptation/presentation.md#audio-02) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| AUDIO-03 | [切曲、渐变、暂停、停止](../../docs/adaptation/presentation.md#audio-03) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| AUDIO-04 | [返回前台后恢复声音](../../docs/adaptation/presentation.md#audio-04) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| AUDIO-05 | [音乐资源错误信息](../../docs/adaptation/presentation.md#audio-05) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| AUDIO-06 | [高刷新与音频事件分开](../../docs/adaptation/presentation.md#audio-06) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| MIDI-01 | [声明 MIDI 选项](../../docs/adaptation/presentation.md#midi-01) | 不适用 | multiplayer：本作使用 OGG 音乐 |
| MIDI-02 | [可听到的 MIDI 输出](../../docs/adaptation/presentation.md#midi-02) | 不适用 | multiplayer：本作使用 OGG 音乐 |
| MIDI-03 | [MIDI 生命周期](../../docs/adaptation/presentation.md#midi-03) | 不适用 | multiplayer：本作使用 OGG 音乐 |
| MIDI-04 | [MIDI 与 OGG 分开](../../docs/adaptation/presentation.md#midi-04) | 不适用 | multiplayer：本作使用 OGG 音乐 |

## 存档与 Replay

| 编号 | 检查项 | multiplayer | 记录 |
| --- | --- | --- | --- |
| REPLAY-01 | [导入原版 Replay](../../docs/adaptation/storage.md#replay-01) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| REPLAY-02 | [按原版格式导出可表达输入](../../docs/adaptation/storage.md#replay-02) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| REPLAY-03 | [记录实际生效输入](../../docs/adaptation/storage.md#replay-03) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| REPLAY-04 | [扩展格式与版本](../../docs/adaptation/storage.md#replay-04) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| REPLAY-05 | [Replay 文件管理](../../docs/adaptation/storage.md#replay-05) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| REPLAY-06 | [回放与实时输入隔离](../../docs/adaptation/storage.md#replay-06) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| REPLAY-07 | [回放独立于显示刷新率](../../docs/adaptation/storage.md#replay-07) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| SAVE-01 | [已声明的存储根](../../docs/adaptation/storage.md#save-01) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| SAVE-02 | [用户文件允许范围](../../docs/adaptation/storage.md#save-02) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| SAVE-03 | [明确持久化时机](../../docs/adaptation/storage.md#save-03) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| SAVE-04 | [退出重开后数据保留](../../docs/adaptation/storage.md#save-04) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| SAVE-05 | [离线读取已有数据](../../docs/adaptation/storage.md#save-05) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| SAVE-06 | [游戏包与用户数据分开](../../docs/adaptation/storage.md#save-06) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |

## 语言与练习

| 编号 | 检查项 | multiplayer | 记录 |
| --- | --- | --- | --- |
| PRACTICE-01 | [发布包中的练习能力](../../docs/adaptation/localization.md#practice-01) | 不适用 | multiplayer：该构建未提供 thprac |
| PRACTICE-02 | [开局前启用与语言](../../docs/adaptation/localization.md#practice-02) | 不适用 | multiplayer：该构建未提供 thprac |
| PRACTICE-03 | [游戏内练习状态](../../docs/adaptation/localization.md#practice-03) | 不适用 | multiplayer：该构建未提供 thprac |
| PRACTICE-04 | [关卡与资源参数](../../docs/adaptation/localization.md#practice-04) | 不适用 | multiplayer：该构建未提供 thprac |
| PRACTICE-05 | [练习 Replay 元数据](../../docs/adaptation/localization.md#practice-05) | 不适用 | multiplayer：该构建未提供 thprac |
| PRACTICE-06 | [触控功能键与指针](../../docs/adaptation/localization.md#practice-06) | 不适用 | multiplayer：该构建未提供 thprac |
| PRACTICE-07 | [练习语言跟随设置](../../docs/adaptation/localization.md#practice-07) | 不适用 | multiplayer：该构建未提供 thprac |
| PRACTICE-08 | [练习界面的可用功能](../../docs/adaptation/localization.md#practice-08) | 不适用 | multiplayer：该构建未提供 thprac |
| LANG-01 | [内置日语原文](../../docs/adaptation/localization.md#lang-01) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| LANG-02 | [语言包身份与发布清单](../../docs/adaptation/localization.md#lang-02) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| LANG-03 | [开局前挂载翻译与字体](../../docs/adaptation/localization.md#lang-03) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| LANG-04 | [语言包失败时回到原文](../../docs/adaptation/localization.md#lang-04) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| LANG-05 | [字体字符覆盖](../../docs/adaptation/localization.md#lang-05) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| LANG-06 | [缺译资源使用原文](../../docs/adaptation/localization.md#lang-06) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |

## 多人接入与规则

| 编号 | 检查项 | multiplayer | 记录 |
| --- | --- | --- | --- |
| MP-RULE-01 | [死亡、幽灵与救援](../../docs/adaptation/multiplayer.md#mp-rule-01) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| MP-RULE-02 | [个人资源与团队资源](../../docs/adaptation/multiplayer.md#mp-rule-02) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| MP-RULE-03 | [给予资源与反馈](../../docs/adaptation/multiplayer.md#mp-rule-03) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| MP-RULE-04 | [对话、结算与全灭](../../docs/adaptation/multiplayer.md#mp-rule-04) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| MP-RULE-05 | [自机可见性与多人HUD](../../docs/adaptation/multiplayer.md#mp-rule-05) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| MP-01 | [独立 MP 产品变体](../../docs/adaptation/multiplayer.md#mp-01) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| MP-02 | [房间人数、难度与机体](../../docs/adaptation/multiplayer.md#mp-02) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| MP-03 | [所有席位准备并同步开局](../../docs/adaptation/multiplayer.md#mp-03) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| MP-04 | [确定性输入与状态检查](../../docs/adaptation/multiplayer.md#mp-04) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| MP-05 | [各席位输入归属](../../docs/adaptation/multiplayer.md#mp-05) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| MP-06 | [旁观加入与输入隔离](../../docs/adaptation/multiplayer.md#mp-06) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| MP-07 | [旁观只读取连续已确认历史](../../docs/adaptation/multiplayer.md#mp-07) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| MP-08 | [慢旁观者与玩家隔离](../../docs/adaptation/multiplayer.md#mp-08) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| MP-09 | [旁观尾帧与结束时机](../../docs/adaptation/multiplayer.md#mp-09) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| MP-10 | [联机暂停与重开](../../docs/adaptation/multiplayer.md#mp-10) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| MP-11 | [多人 Replay](../../docs/adaptation/multiplayer.md#mp-11) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| MP-12 | [WebRTC 与 Relay 路径](../../docs/adaptation/multiplayer.md#mp-12) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| MP-13 | [本地玩家可见性](../../docs/adaptation/multiplayer.md#mp-13) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| MP-14 | [联机与回滚诊断](../../docs/adaptation/multiplayer.md#mp-14) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| MP-15 | [游戏结束返回房间](../../docs/adaptation/multiplayer.md#mp-15) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |

## 验证与交付

| 编号 | 检查项 | multiplayer | 记录 |
| --- | --- | --- | --- |
| VERIFY-01 | [全部内置 Demo 对照](../../docs/adaptation/validation.md#verify-01) | 不适用 | multiplayer：MP 使用多人 Replay 自一致性检查 |
| VERIFY-02 | [长 Replay 样本](../../docs/adaptation/validation.md#verify-02) | 不适用 | multiplayer：MP 使用多人 Replay 自一致性检查 |
| VERIFY-03 | [原版基准与内容哈希](../../docs/adaptation/validation.md#verify-03) | 不适用 | multiplayer：MP 使用多人 Replay 自一致性检查 |
| VERIFY-04 | [读取固定基准](../../docs/adaptation/validation.md#verify-04) | 不适用 | multiplayer：MP 使用多人 Replay 自一致性检查 |
| VERIFY-05 | [单独维护原版基准](../../docs/adaptation/validation.md#verify-05) | 不适用 | multiplayer：MP 使用多人 Replay 自一致性检查 |
| VERIFY-06 | [逐逻辑帧比较](../../docs/adaptation/validation.md#verify-06) | 不适用 | multiplayer：MP 使用多人 Replay 自一致性检查 |
| VERIFY-07 | [诊断构建与正常构建](../../docs/adaptation/validation.md#verify-07) | 不适用 | multiplayer：MP 使用多人 Replay 自一致性检查 |
| VERIFY-08 | [本作状态映射与结束条件](../../docs/adaptation/validation.md#verify-08) | 不适用 | multiplayer：MP 使用多人 Replay 自一致性检查 |

<!-- manual-verification -->

## 运行验收记录

WASI32 的多人规则兼容检查使用内存通道运行原生 NetplayRuntime、公共协议和输入核心，覆盖双人、三人录像准入。入口为 `portable/check-multiplayer-rules.mjs --suite multiplayer-rules-compatibility`。

给命回滚检查使用真实席位按键边沿，覆盖第五次低速按下迟到七帧后的重新演算。恢复同时核对给命进度、双方生命、幽灵状态和输入归属，见 `portable/check-multiplayer-rules.mjs --suite cooperative-rollback`。

2026-10-25 multiplayer：共同菜单、局末本地结算、旁观原生保存与三人挑战恢复通过，见[多人生命周期记录](../../docs/adaptation/multiplayer-lifecycle-20261025.md)。

按检查编号附[验证记录](../../docs/adaptation/record-template.md)，写明源码身份、模式、环境、操作与结果。自动更新保留本节内容。

本轮桌面协议、输入、文件持久化与实际 MP 房间结果见[逐作运行记录](../../docs/adaptation/desktop-20261018.md)。按作品、模式和分项读取通过、失败及待复验结果，原始报告身份见同页机器记录。

### 神德练习延迟 | PRACTICE-08、PRACTICE-05

菜单使用与普通版相同的四难度范围和默认值，同难度重开保留设置，Extra 返回普通难度时恢复对应默认值。菜单调整与录像解码分别处理，历史录像的 160 和缺省 -1 保留录制含义。`portable/check-th10-thprac.cpp` 覆盖难度切换、范围、零值和录像参数往返。
