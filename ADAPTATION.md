# th12 MP 适配检查

[开发入口](README.md) · [检查方法](../../docs/adaptation/README.md) · [全部作品](../README.md)

源码目录：`ports/th12-mp`。构建模式：`multiplayer`。

[SDL3 / GLES 3 原生层](portable/sdl/README.md)提供链接当前多人核心的平台库 `th12_sdl_host`；本表中的房间、回滚和多人录像仍对应现有 Web 多人宿主。

实现情况与实际验收分别记录。源码接口和模块检查通过后记为“已实现”；完成对应操作并附执行记录后记为“已验证”。

状态：`待核对`、`进行中`、`待验证`、`已实现`、`已验证`、`检查失败`、`不适用`。

[自动检查结果与源码身份](../../docs/adaptation/status.json) · `npm run ports:check -- --game=th12 --variant=both`

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

# TH12 多人适配

[开发入口](README.md) · [普通版](../th12/ADAPTATION.md)

源码目录：`ports/th12-mp`。产品：`th12mp`。构建模式：`multiplayer`。多人模式通过独立源码与执行身份接入共用启动器及 Relay。

## 状态所有者

| 内容 | 实现 |
| --- | --- |
| 各席资源、射击、Bomb 与伤害 | 永久 Pilot 与显式 Scope，动画回调保持席位归属 |
| UFO 槽与奖励 | 共用 UFO 管理器，死亡保留槽位，碎片奖励分发到各席 |
| P／残机互赠、幽灵及伤害缩放 | `eagler/gameplay/CooperativeRules.hpp` 与 TH10／TH11 规则 |
| 输入延迟及回滚 | 共用 Netplay Core／SessionChannel／Adonis，原生世界快照 |
| 观战 | 共用阶段协议，TH12 阶段初始化、RNG 与全席资源重建 |
| Replay | 已确认输入存档、构建身份校验、原生重放与进度定位 |
| 房间预加载 | 动态标题、禁止 Demo、Start 复用资源与 WASM |
| 暂停及结束菜单 | 原版确认、排名、姓名与 25 槽 Replay 界面 |

## 自动验证

原生测试使用 C++ 核心；资源测试使用本地合法提取的 `ports/th12/site/assets`。浏览器测试使用正式 WASM、启动器 RPC 与真实本地 WebSocket Relay，配置 25 ms 有序传输延迟。

```sh
cmake -S ports/th12-mp -B ports/th12-mp/artifacts/native-multiplayer
cmake --build ports/th12-mp/artifacts/native-multiplayer --config Release
ctest --test-dir ports/th12-mp/artifacts/native-multiplayer -C Release --output-on-failure
node --test ports/th12-mp/tests/multiplayer-menus.test.mjs
node ports/th12-mp/scripts/check-multiplayer-browser.mjs
```

原生用例验证共享 UFO、自动分 P、P 档位互赠、救援、伤害余数、回滚 RNG、触控及录像校验。资源用例覆盖 Normal／Extra、六种机体、2／3 席、实际弹幕及 Bomb、世界和动画回滚、六面跨关继承。

浏览器用例覆盖双人 0f、三人 3f、直接回滚、自动实测；核对输入帧对应的世界状态、已确认暂停、预加载复用、晚加入旁观、时间轴回看、原版 Replay 槽位与录像重建。报告输出至忽略目录 `artifacts/multiplayer-browser/result.json`。

## 历史维护

普通版来源、多人派生连接点与完整子树登记保存在 `docs/migration/`；目录投影和同步沿 `docs/adaptation/port-sync.md` 维护。修改普通版时，按该作品基线处理多人版的对应修改。

## SDL3 共用应用

桌面与 Web 应用层的资源、流程、绘制和练习工具统一由 [SDL3 运行层](portable/sdl/README.md) 管理。普通版提供单独的 SDL Web 人工验收包，既有正式 Web 构建保留；多人版同步平台库并继续使用原多人会话。构建入口和录像、音乐、存档的归属见该指南。
