# th13 MP 适配检查

[开发入口](README.md) · [检查方法](../../docs/adaptation/README.md) · [全部作品](../README.md)

源码目录：`ports/th13-mp`。构建模式：`multiplayer`。

实现情况与实际验收分别记录。源码接口和模块检查通过后记为“已实现”；完成对应操作并附执行记录后记为“已验证”。

状态：`待核对`、`进行中`、`待验证`、`已实现`、`已验证`、`检查失败`、`不适用`。

[自动检查结果与源码身份](../../docs/adaptation/status.json) · `npm run ports:check -- --game=th13 --variant=both`

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
| RUNTIME-06 | [重开与常显判定点](../../docs/adaptation/runtime.md#runtime-06) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| PACKAGE-01 | [共用游戏包存储](../../docs/adaptation/runtime.md#package-01) | 检查失败 | multiplayer：[package-store](../../docs/adaptation/status.json) |
| PACKAGE-02 | [DATA 身份与挂载目标](../../docs/adaptation/runtime.md#package-02) | 检查失败 | multiplayer：[package-store](../../docs/adaptation/status.json) |
| PACKAGE-03 | [完整更新后切换版本](../../docs/adaptation/runtime.md#package-03) | 检查失败 | multiplayer：[package-store](../../docs/adaptation/status.json) |
| PACKAGE-04 | [本地导入与在线安装一致](../../docs/adaptation/runtime.md#package-04) | 检查失败 | multiplayer：[package-store](../../docs/adaptation/status.json) |
| PACKAGE-05 | [已安装游戏离线启动](../../docs/adaptation/runtime.md#package-05) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| PACKAGE-06 | [统一安装和更新入口](../../docs/adaptation/runtime.md#package-06) | 检查失败 | multiplayer：[package-store](../../docs/adaptation/status.json) |

## 键盘、手柄与触控

| 编号 | 检查项 | multiplayer | 记录 |
| --- | --- | --- | --- |
| INPUT-01 | [键盘逻辑输入](../../docs/adaptation/input.md#input-01) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| INPUT-02 | [启动器键盘改键](../../docs/adaptation/input.md#input-02) | 检查失败 | multiplayer：[launcher-input-owners](../../docs/adaptation/status.json) |
| INPUT-03 | [启动器手柄映射](../../docs/adaptation/input.md#input-03) | 检查失败 | multiplayer：[launcher-input-owners](../../docs/adaptation/status.json) |
| INPUT-04 | [手柄方向与按钮](../../docs/adaptation/input.md#input-04) | 已实现 | multiplayer：[native-gamepad](../../docs/adaptation/status.json) |
| INPUT-05 | [多种输入独立持有](../../docs/adaptation/input.md#input-05) | 检查失败 | multiplayer：[launcher-input-owners](../../docs/adaptation/status.json) |
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
| TOUCH-09 | [暂时禁移与触点连续性](../../docs/adaptation/input.md#touch-09) | 待核对 | multiplayer：[held-touch-continuity](../../docs/adaptation/status.json) |
| TOUCH-10 | [无限速移动落率记录](../../docs/adaptation/input.md#touch-10) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| TOUCH-11 | [双击 Bomb](../../docs/adaptation/input.md#touch-11) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| TOUCH-12 | [菜单与对话手势](../../docs/adaptation/input.md#touch-12) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| TOUCH-13 | [确认键的新按下边沿](../../docs/adaptation/input.md#touch-13) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| TOUCH-14 | [100–300% 灵敏度](../../docs/adaptation/input.md#touch-14) | 已实现 | multiplayer：[touch-sensitivity](../../docs/adaptation/status.json) |
| TOUCH-15 | [触控与其他输入隔离](../../docs/adaptation/input.md#touch-15) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| TOUCH-16 | [触控取消与生命周期](../../docs/adaptation/input.md#touch-16) | 待核对 | multiplayer：按本项步骤执行，结果写入验证记录 |
| TOUCH-17 | [共用布局编辑器](../../docs/adaptation/input.md#touch-17) | 检查失败 | multiplayer：[shared-touch-controls](../../docs/adaptation/status.json) |
| TOUCH-18 | [共用重开按钮](../../docs/adaptation/input.md#touch-18) | 检查失败 | multiplayer：[shared-touch-controls](../../docs/adaptation/status.json) |
| TOUCH-19 | [共用双指放大镜](../../docs/adaptation/input.md#touch-19) | 检查失败 | multiplayer：[shared-touch-controls](../../docs/adaptation/status.json) |

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
| AUDIO-07 | [音效即时输出](../../docs/adaptation/presentation.md#audio-07) | 待核对 | multiplayer：[immediate-sfx-routing](../../docs/adaptation/status.json) |
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
| LANG-02 | [语言包身份与发布清单](../../docs/adaptation/localization.md#lang-02) | 不适用 | multiplayer：本作使用内置原文 |
| LANG-03 | [开局前挂载翻译与字体](../../docs/adaptation/localization.md#lang-03) | 不适用 | multiplayer：本作使用内置原文 |
| LANG-04 | [语言包失败时回到原文](../../docs/adaptation/localization.md#lang-04) | 不适用 | multiplayer：本作使用内置原文 |
| LANG-05 | [字体字符覆盖](../../docs/adaptation/localization.md#lang-05) | 不适用 | multiplayer：本作使用内置原文 |
| LANG-06 | [缺译资源使用原文](../../docs/adaptation/localization.md#lang-06) | 不适用 | multiplayer：本作使用内置原文 |

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
## 多人里程碑状态（2026-11-10 主代理 Review）

| 里程碑 | 状态 | 验证入口 |
| --- | --- | --- |
| M1 模拟/呈现/副作用拆分 | 已验证 | `tests/multiplayer/run-native.sh --through M1 --scope current`；browser `m1.*` |
| M2 一套世界多席 | 已验证 | `--through M2`（含单席 SP 等价 case） |
| M3 目标/碰撞/共享资源 | 已验证 | `--through M3`（8 case） |
| M4 全队灵界（RULES v2） | 已验证 | `--through M4`（8 case，含 R-T9 代数守卫负控） |
| M5 表现/幽灵/团队生命周期 | 已验证 | 原有 6 case；追加 Bomb 完整保护窗口、逐席 HUD 和灵梦灵界弹路径，原生回归与正负控见下方功能基线记录 |
| M6 结构化快照 | 已验证（有登记缺口） | `--through M6`；缺口见下 |
| M7 确认输入联机 | 已验证 | browser `m7.*`（RTC/relay 路径断言 + 3 席真实会话） |
| M8 12 帧回滚 | 已验证 | browser `m8.rollback_one_three_twelve` + `m8.rollback_full_window_raf`（正式页 rAF 自然泵满窗实测 maxDistance=12；通道自检 th13-input、线上 digest 零失配） |
| M9 Replay/旁观/生命周期 | 已验证（debug 页闭环） | browser `m9.*`（10 case，含 `m9.runtime_page_soak`） |
| M10 launcher 正式接入 | 已验证（真实房间开局） | `tools/ports/room-browser.mjs`：rtc×2 / relay×2 / relay×3 全 PASS；th13 直入对局 |

### 功能基线验证范围

2026-11-13 的功能基线包含逐席原作风格 HUD、幽灵漂移和射击退役、Bomb
完整保护共享、灵梦灵界弹淡出身份、全场上收及确认终局交接。生产入口仍从
第一面开始；定点六面产物只放 ignored 目录，未进入源码。

- 原生 M1–M9 累计 56 项及 Bomb/HUD/灵梦追加 3 项通过；席位表现完整
  16 块另行通过，包含旧焦点位置不碰撞、当前位置受击的正负控。
- 普通版与多人单席四角色及 Extra 各 7200 帧共同导出状态精确一致，20 个
  投影突变负控被拒绝；该门不比较全部内部状态或最终 framebuffer。
- 三个独立 Chromium 的 Lunatic 六面定点流程完成 Ending、各席持久化录像
  和返回房间，36758 帧审计一致。该次使用普通 X 输入包装退出已保存槽页；
  仓内 pilot 对应修复另有 20 项离线测试。完整一至六面和扰动回滚仍待验收。
- HUD 实际 WebGL 布局与 blit 数值检查有界通过；幽灵完整像素数值证明、
  芳香死亡阶段自然路径及独立原版最终输出尚未认证。4× CPU 性能验收暂缓。
- 证据保存在外层 ignored `artifacts/functional-baseline-cumulative-r3-20261113/`、
  `artifacts/functional-single-seat-current-20261113/reference-r3/`、
  `artifacts/stage6-ending-targeted-20261113/` 和 `artifacts/baseline-commit-20261113/`。

### 已知缺口（如实登记）

1. **正式 Runtime 页已接线（P5 轨道 B/C）**：`web/sdl-runtime/multiplayer.mjs` 桥 +
   `MultiplayerExports.cpp` + launcher `th13mp` 产品。relay 路由需
   `netplayAllowWebsocketFallback`——由 relay lobby 快照 `gameplayRelayFallback`
   通告 → launcher 归一化 → runtime options 传递（2026-11-10，relay×3 真实房间
   验证）。剩余：断线重连（见 5）、正式页旁观/Replay 槽 UI 属 launcher 通用面。
2. **M6 快照覆盖空白**：MSG/STD/HUD 大部分域、过渡态、FrameScheduler 动态
   callback、资源 pin 不在快照；跨 Bomb/ECL517 帧回滚存在 RNG 漂移风险。
   已修：`next_bullet_generation`、restore 后活动席窗口刷新、restore 返回值自证。
3. **digest 覆盖不全**：激光字段、敌机 fog、嵌套插值（mover/postime/sine）、
   YoumuBomb 计数域未入 hash。
4. **场景覆盖矩阵难度维度**：E/N/H/L/Extra × 1..6 面完整矩阵未覆盖（录像资产
   仅承载关 1..3）。
5. **断线重连**（transport reconnect + 确认前沿续推）未实现；当前断线语义=保存
   确认历史后结束。
6. **soak fps≥58**：无硬件渲染容器物理不可达（实测 5.9–6.0）；阈值不降，
   `TH13_SOAK_REQUIRE_FPS_58=1` 环境启用该断言。
7. **音频恰播一次的感知残余（P3.1）**：SFX Voice 首播即真响，错误预测多产生的
   声音无法回收；Stop/Pan 延迟一确认窗冲账。native 脚本化 case 覆盖 BGM 恰一次。
8. SP `curve_laser_presentation_test` 2 case 的历史 fixture 缺口没有在本轮重验。
   标题音量前导零、符卡难度取消和练习退出修复已同步两侧；当前三个菜单
   套件通过，替换回修复前实现的对应负控失败。

### 验证命令（容器内，cwd=ports/th13-mp）

```sh
node portable/build.mjs                                   # 生产构建
bash tests/multiplayer/run-native.sh --through M9 --scope cumulative \
     --source-digest <gate-digest> --report <path>          # native 56 case
node tests/multiplayer/acceptance.mjs --through M9 \
     --source-digest <gate-digest> --report <path>          # browser 35 case
sh tests/combat/run.sh && bash tests/e2e/run.sh            # 单席回归
```

外层 gate：`python3 scripts/mp_workflow_gate.py --stage M9 --tier full|final`
（`protect_sp` 在 SP 工作树有未提交修改时如实报错——待提交后复跑）。

### 预测性能（P1：PR#225 ConfirmedHold）

口径：P0 预测命中率评估器（`tests/multiplayer/native/prediction_hitrate_test.cpp`，
mode=2、input_delay=2、maxRollbackFrames=12、远端输入按帧 g+L 提交；jitter
为固定种子 0..3 帧额外延迟），7 份真人录像 × L∈{2,4,6,8} × jitter∈{false,true}
共 56 行。基线 `artifacts/mp-round-20261110/prediction-baseline.json`；
ConfirmedHold `p1-prediction.json`，对照 `p1-prediction-compare.md`。

| L | jitter | 基线回滚 | ConfirmedHold | 变化 | 基线/1000 帧 | ConfirmedHold/1000 帧 |
| --- | --- | --- | --- | --- | --- | --- |
| 2 | false | 52230 | 52550 | +0.613% | 86.870 | 87.402 |
| 2 | true | 70402 | 71332 | +1.321% | 117.094 | 118.641 |
| 4 | false | 52230 | 52559 | +0.630% | 86.870 | 87.417 |
| 4 | true | 95334 | 71334 | -25.175% | 158.562 | 118.644 |
| 6 | false | 92222 | 52559 | -43.008% | 153.386 | 87.417 |
| 6 | true | 96586 | 71334 | -26.145% | 160.644 | 118.644 |
| 8 | false | 92222 | 52559 | -43.008% | 153.386 | 87.417 |
| 8 | true | 96586 | 71334 | -26.145% | 160.644 | 118.644 |

结论：L=6/L=8 非乱序档回滚总数下降 43.0%，为本次接入的核心收益；56 行中
37 行改善、0 行变差超过 10%，最差单行 +4.423%。L=2/L=4 非乱序与 L=2 乱序为
小幅回退（≤1.7%），属允许范围。

### 回滚开销与 digest 交换（P5-C 实测，`m8.rollback_full_window_raf`）

口径：正式页 rAF 自然泵 375 提交帧、383 预测帧 checkpoint、满窗（12 帧）纠正、
重放 12 帧；swiftshader 无头容器（生产 wasm）。

| 面 | 均值 | 峰值 | 说明 |
| --- | --- | --- | --- |
| 预测帧 capture | 137 µs（52300/383） | 700 µs | P4 环槽复用免分配路径 |
| 纠正 restore | 600 µs（600/1） | 600 µs | 单次满窗恢复 |
| 模拟 update | 154 µs（60700/395） | 3400 µs | 含重放帧 |
| 重放 resim | 800 µs（12 帧） | — | 满窗一次 |

digest 交换（P3.4 线上 T13D）：sent 7 / received 7 / desync 0——纠正后权威
态与对端逐次比对一致；停等帧 4（注入稳态引导 ~8 帧）。



## 运行验收记录

按检查编号附[验证记录](../../docs/adaptation/record-template.md)，写明源码身份、模式、环境、操作与结果。自动更新保留本节内容。

残机与灵击拾取在原版结算点同步通知既有 HUD VM。覆盖整件奖励、碎片合成、录像恢复后的拾取和灵击上限；[定向回归](tests/ui/hud_item_refresh_test.cpp) 支持合成脚本与本地原版 ANM 资源。道具、HUD、录像及 ANM 共 71 项回归通过，原版资源定向验证 4 项通过。
