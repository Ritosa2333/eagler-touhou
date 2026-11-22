# thsss 适配检查

[开发入口](README.md) · [练习说明](PRACTICE.md) · [检查方法](../../docs/adaptation/README.md) · [全部作品](../README.md)

源码目录是 `ports/thsss`，来自 THSSS-WASM 子树。本目录是单人接线，产品仍为 `testOnly`。

实现情况与实际验收分别记录。源码接口和模块检查通过后记为“已实现”；完成对应操作并附执行记录后记为“已验证”。

状态：`待核对`、`进行中`、`待验证`、`已实现`、`已验证`、`检查失败`、`不适用`。

## 源码与构建

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| BUILD-01 | [源码目录与普通版／MP归属](../../docs/adaptation/build.md#build-01) | 待验证 | 普通版源码在 `ports/thsss`，无 MP 目录。`runtime-builds` 已声明 `sourceBuild`。来源子树见 [thsss-eagler-20261122.json](../../docs/migration/thsss-eagler-20261122.json)。 |
| BUILD-02 | [工具链与外部依赖](../../docs/adaptation/build.md#build-02) | 待核对 | 浏览器不能检查 SDK。本机 Release 发布已通过，补上打包模块表后 stage 也已通过，没有单独的执行报告。 |
| BUILD-03 | [对应模式独立构建](../../docs/adaptation/build.md#build-03) | 待核对 | 未适配多人构建。单人 Runtime 已跑到首帧。 |
| BUILD-04 | [Runtime 文件与来源记录](../../docs/adaptation/build.md#build-04) | 待验证 | build-eagler 已有 runtime-files.json。首帧来自这次打包结果，清单哈希还没逐项对照。见[运行验收记录](#运行验收记录)。 |
| BUILD-05 | [前端搭配单个游戏运行](../../docs/adaptation/build.md#build-05) | 已验证 | `EAGLER_DEVELOPMENT_GAMES=thsss` 下启动器加载当前 Runtime 并进入首帧。见[运行验收记录](#运行验收记录)。 |
| BUILD-06 | [源码与素材来源](../../docs/adaptation/build.md#build-06) | 待验证 | 来源为 [THSSS-WASM](https://github.com/sblzdddd/THSSS-WASM) `main` `c08d073`。登记见 [thsss-subtree-20261122.md](../../docs/migration/thsss-subtree-20261122.md)。游戏素材仍在外部 `Assets/`。 |

## 启动与资源

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| RUNTIME-01 | [协议身份与会话代次](../../docs/adaptation/runtime.md#runtime-01) | 待验证 | 本次会话的消息是 `eagler-touhou/1`，`game` 为 thsss。还没做退出后再启动的代次隔离。见[运行验收记录](#运行验收记录)。 |
| RUNTIME-02 | [就绪、配置、启动、首帧](../../docs/adaptation/runtime.md#runtime-02) | 已验证 | `ready` 之后是成功回复，2623 ms 时收到 `first-frame`，画面已经画出。见[运行验收记录](#运行验收记录)。 |
| RUNTIME-03 | [命令响应与错误信息](../../docs/adaptation/runtime.md#runtime-03) | 待验证 | 文件命令和按键回复成功。`retry-music` 回复 Command is not available for this title。未知命令没有回复，无效参数的界面提示没做。见[运行验收记录](#运行验收记录)。 |
| RUNTIME-04 | [后台、恢复与退出清理](../../docs/adaptation/runtime.md#runtime-04) | 待验证 | 可见时 `frame-health` 为 60。把 iframe 的 `document.hidden` 改成 true 后，下一笔是 15，不是 0。恢复和退出清理没做。见[运行验收记录](#运行验收记录)。 |
| RUNTIME-05 | [Runtime 与资源管理分工](../../docs/adaptation/runtime.md#runtime-05) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| RUNTIME-06 | [重开与常显判定点](../../docs/adaptation/runtime.md#runtime-06) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| PACKAGE-01 | [共用游戏包存储](../../docs/adaptation/runtime.md#package-01) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| PACKAGE-02 | [DATA 身份与挂载目标](../../docs/adaptation/runtime.md#package-02) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| PACKAGE-03 | [完整更新后切换版本](../../docs/adaptation/runtime.md#package-03) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| PACKAGE-04 | [本地导入与在线安装一致](../../docs/adaptation/runtime.md#package-04) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| PACKAGE-05 | [已安装游戏离线启动](../../docs/adaptation/runtime.md#package-05) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| PACKAGE-06 | [统一安装和更新入口](../../docs/adaptation/runtime.md#package-06) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |

## 键盘、手柄与触控

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| INPUT-01 | [键盘逻辑输入](../../docs/adaptation/input.md#input-01) | 已验证 | 方向键移动后画面亮点位置改变。按住 Z 连续播出 `se_plst00.wav`。X 放出炸弹（现场可见）。菜单方向键播出 `se_select00.wav`。见[运行验收记录](#运行验收记录)。 |
| INPUT-02 | [启动器键盘改键](../../docs/adaptation/input.md#input-02) | 已验证 | WASD 布局下 `KeyS` 经 `__eaglerKeyboardInputBridge.submit` 映射为 `ArrowDown`；标题菜单从 Game Start 逐项移到 Replay。见[运行验收记录](#运行验收记录)。 |
| INPUT-03 | [启动器手柄映射](../../docs/adaptation/input.md#input-03) | 已验证 | 手动：启动器手柄改键保存后进局，映射立即生效。见[运行验收记录](#运行验收记录)。 |
| INPUT-04 | [手柄方向与按钮](../../docs/adaptation/input.md#input-04) | 已验证 | 手动：摇杆移动、按钮射击／Bomb／低速／菜单与键盘同路径。见[运行验收记录](#运行验收记录)。 |
| INPUT-05 | [多种输入独立持有](../../docs/adaptation/input.md#input-05) | 待验证 | 键盘与触控同时按住射击再分别松开，这一项没做。见[运行验收记录](#运行验收记录)。 |
| INPUT-06 | [失焦与断开释放](../../docs/adaptation/input.md#input-06) | 待验证 | `keyboard-clear` 回复成功。按住移动时切后台、断手柄没有单独测。见[运行验收记录](#运行验收记录)。 |
| INPUT-07 | [原作菜单与对话输入](../../docs/adaptation/input.md#input-07) | 待验证 | 键盘完成标题、难度、选人确认，Esc 打开暂停“继续游戏”。手柄菜单和对话快进没做。见[运行验收记录](#运行验收记录)。 |
| TOUCH-01 | [限速滑动移动](../../docs/adaptation/input.md#touch-01) | 已验证 | `touchMovementMode=touch`，`InputSurface=play`。自右向左 `direct-touch` 拖动后，画面亮点从右侧集中区移到左侧。见[运行验收记录](#运行验收记录)。 |
| TOUCH-02 | [无限速滑动移动](../../docs/adaptation/input.md#touch-02) | 待验证 | 选项可设为 `touch-unlimited`。局内 play 表面远距跳变对照没做成。见[运行验收记录](#运行验收记录)。 |
| TOUCH-03 | [方向式虚拟摇杆](../../docs/adaptation/input.md#touch-03) | 已验证 | `touchMovementMode=joystick`。标题推下：菜单从 Game Start 移到 Replay；进局后 `joystickX` 推左右，自机在 play 表面移动。见[运行验收记录](#运行验收记录)。 |
| TOUCH-04 | [自由方向虚拟摇杆](../../docs/adaptation/input.md#touch-04) | 待验证 | 已切到 `joystick-free`，摇杆 HUD 出现；本地单测覆盖自由推杆位移。局内绕圈像素对照没做成。见[运行验收记录](#运行验收记录)。 |
| TOUCH-05 | [按住低速](../../docs/adaptation/input.md#touch-05) | 已验证 | `touchFocusMode=hold-button`，按钮文案“低速（按住）”。局内 `focusEnabled=true` 时与摇杆一并送出，抬起后 `focusEnabled=false`。见[运行验收记录](#运行验收记录)。 |
| TOUCH-06 | [切换低速](../../docs/adaptation/input.md#touch-06) | 已验证 | `touchFocusMode=toggle-button`，按钮文案“低速（切换）”。连续点击 `aria-pressed` 在 true／false 间切换。见[运行验收记录](#运行验收记录)。 |
| TOUCH-07 | [双指低速](../../docs/adaptation/input.md#touch-07) | 待验证 | 没做双指。见[运行验收记录](#运行验收记录)。 |
| TOUCH-08 | [射击、Bomb 与暂停](../../docs/adaptation/input.md#touch-08) | 已验证 | 开启触摸后左侧有开火／低速／BOMB／ESC。`touch-controls` 开火锁住后有 `se_plst00.wav`；键盘 X 与 BOMB 放出炸弹（现场可见）；`escapeSerial` 后进入暂停菜单。见[运行验收记录](#运行验收记录)。 |
| TOUCH-09 | [暂时禁移与触点连续性](../../docs/adaptation/input.md#touch-09) | 待验证 | 死亡后同一触点续拖没做成。见[运行验收记录](#运行验收记录)。 |
| TOUCH-10 | [无限速移动落率记录](../../docs/adaptation/input.md#touch-10) | 待验证 | 没做到结算与 Replay 落率。见[运行验收记录](#运行验收记录)。 |
| TOUCH-11 | [双击 Bomb](../../docs/adaptation/input.md#touch-11) | 已验证 | `doubleTapBombEnabled` 开启后，play 表面双击触发 Bomb（手动确认可见）。见[运行验收记录](#运行验收记录)。 |
| TOUCH-12 | [菜单与对话手势](../../docs/adaptation/input.md#touch-12) | 待验证 | 标题静止点按播出 `se_ok00.wav`；摇杆模式下画布滑动可选菜单项。对话短按／长按没做。见[运行验收记录](#运行验收记录)。 |
| TOUCH-13 | [确认键的新按下边沿](../../docs/adaptation/input.md#touch-13) | 待验证 | 菜单点按产生确认音并前进。射击保持进入对话后再触控确认没做。见[运行验收记录](#运行验收记录)。 |
| TOUCH-14 | [100–300% 灵敏度](../../docs/adaptation/input.md#touch-14) | 待验证 | 布局编辑器可见 100／150／200% 与自定义；本地单测覆盖灵敏度缩放。三档同滑位移对照没做成。见[运行验收记录](#运行验收记录)。 |
| TOUCH-15 | [触控与其他输入隔离](../../docs/adaptation/input.md#touch-15) | 待验证 | 键盘按住时发过 `touch-cancel`，未对照射击是否继续。见[运行验收记录](#运行验收记录)。 |
| TOUCH-16 | [触控取消与生命周期](../../docs/adaptation/input.md#touch-16) | 已验证 | `touch-cancel` 经 `__eaglerDirectInputBridge.submit` 成功。隐藏页面后的残留触点没单独测。见[运行验收记录](#运行验收记录)。 |
| TOUCH-17 | [共用布局编辑器](../../docs/adaptation/input.md#touch-17) | 已验证 | 未开局打开“按键布局与触控设置”，出现移动方式（八向／自由等）与灵敏度面板，可保存布局。见[运行验收记录](#运行验收记录)。 |
| TOUCH-18 | [共用重开按钮](../../docs/adaptation/input.md#touch-18) | 待验证 | `restartButtonEnabled` 后左侧出现 R；局内点过 R，与暂停菜单重开流程的对照没做成。见[运行验收记录](#运行验收记录)。 |
| TOUCH-19 | [共用双指放大镜](../../docs/adaptation/input.md#touch-19) | 待验证 | 放大镜开关已开。双指缩放没做。见[运行验收记录](#运行验收记录)。 |

## 画面与声音

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| DISPLAY-01 | [固定逻辑步进](../../docs/adaptation/presentation.md#display-01) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| DISPLAY-02 | [跟随显示器刷新率](../../docs/adaptation/presentation.md#display-02) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| DISPLAY-03 | [额外绘制无逻辑副作用](../../docs/adaptation/presentation.md#display-03) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| DISPLAY-04 | [连续视觉字段插值](../../docs/adaptation/presentation.md#display-04) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| DISPLAY-05 | [出生、销毁和瞬移](../../docs/adaptation/presentation.md#display-05) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| DISPLAY-06 | [暂停画面的表现](../../docs/adaptation/presentation.md#display-06) | 已验证 | Esc 后出现“游戏暂停 / Pause Menu”，关卡画面留在菜单后面，并播出 `se_pause.wav`。见[运行验收记录](#运行验收记录)。 |
| DISPLAY-07 | [实际呈现后报告首帧](../../docs/adaptation/presentation.md#display-07) | 已验证 | `first-frame` 到达后，640×480 画布中心像素为不透明的 `(26, 48, 65)`。见[运行验收记录](#运行验收记录)。 |
| DISPLAY-08 | [限制到 60 Hz](../../docs/adaptation/presentation.md#display-08) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| AUDIO-01 | [正常 OGG 音乐](../../docs/adaptation/presentation.md#audio-01) | 已验证 | 启动器音乐为 OGG。18 首曲目进入 `/BGM/` 后，标题循环 `OP.ogg`，选人进入关卡后切到 `Boss01.ogg`、`Stage01.ogg`。AudioContext 为 running。见[运行验收记录](#运行验收记录)。 |
| AUDIO-02 | [无 BGM 模式](../../docs/adaptation/presentation.md#audio-02) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| AUDIO-03 | [切曲、渐变、暂停、停止](../../docs/adaptation/presentation.md#audio-03) | 待验证 | 标题 `OP.ogg` 之后切到 `Boss01.ogg`、`Stage01.ogg`，暂停时播出 `se_pause.wav`。渐变、暂停后的曲目位置和退出停止没有测量。见[运行验收记录](#运行验收记录)。 |
| AUDIO-04 | [返回前台后恢复声音](../../docs/adaptation/presentation.md#audio-04) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| AUDIO-05 | [音乐资源错误信息](../../docs/adaptation/presentation.md#audio-05) | 待核对 | 未接入。`retry-music` 仍回复 Command is not available for this title。没有用缺曲或坏曲复跑。 |
| AUDIO-06 | [高刷新与音频事件分开](../../docs/adaptation/presentation.md#audio-06) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| AUDIO-07 | [音效即时输出](../../docs/adaptation/presentation.md#audio-07) | 已验证 | `OP.ogg` 循环期间菜单确认播出 `se_ok00.wav`；关卡中继续播出 `se_graze.wav`、`se_enep00.wav`、`se_tan00a.wav`、`se_item00.wav`。见[运行验收记录](#运行验收记录)。 |
| MIDI-01 | [声明 MIDI 选项](../../docs/adaptation/presentation.md#midi-01) | 不适用 | normal：本作使用 OGG 音乐 |
| MIDI-02 | [可听到的 MIDI 输出](../../docs/adaptation/presentation.md#midi-02) | 不适用 | normal：本作使用 OGG 音乐 |
| MIDI-03 | [MIDI 生命周期](../../docs/adaptation/presentation.md#midi-03) | 不适用 | normal：本作使用 OGG 音乐 |
| MIDI-04 | [MIDI 与 OGG 分开](../../docs/adaptation/presentation.md#midi-04) | 不适用 | normal：本作使用 OGG 音乐 |

## 存档与 Replay

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| REPLAY-01 | [导入原版 Replay](../../docs/adaptation/storage.md#replay-01) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| REPLAY-02 | [按原版格式导出可表达输入](../../docs/adaptation/storage.md#replay-02) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| REPLAY-03 | [记录实际生效输入](../../docs/adaptation/storage.md#replay-03) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| REPLAY-04 | [扩展格式与版本](../../docs/adaptation/storage.md#replay-04) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| REPLAY-05 | [Replay 文件管理](../../docs/adaptation/storage.md#replay-05) | 待验证 | 写入、读取、同步、重开后仍在，随后删除并再次同步。删除是否跨实例消失还没再打开核对。见[运行验收记录](#运行验收记录)。 |
| REPLAY-06 | [回放与实时输入隔离](../../docs/adaptation/storage.md#replay-06) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| REPLAY-07 | [回放独立于显示刷新率](../../docs/adaptation/storage.md#replay-07) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| SAVE-01 | [已声明的存储根](../../docs/adaptation/storage.md#save-01) | 待验证 | 重开后列出 `Setting.INI`、`Score.dat` 和 `replay/` 下的文件。存储根路径本身没有打开核对。见[运行验收记录](#运行验收记录)。 |
| SAVE-02 | [用户文件允许范围](../../docs/adaptation/storage.md#save-02) | 已验证 | `Replay/thSSS_ud0001.rpy` 可写可读。`../THSSS.Browser.wasm` 返回 errno 1，Path is not a user file。见[运行验收记录](#运行验收记录)。 |
| SAVE-03 | [明确持久化时机](../../docs/adaptation/storage.md#save-03) | 已验证 | 写入 4 字节 Replay 后 `sync` 成功，关闭页面再启动仍能读到该文件。见[运行验收记录](#运行验收记录)。 |
| SAVE-04 | [退出重开后数据保留](../../docs/adaptation/storage.md#save-04) | 已验证 | 新实例列出 `Setting.INI`、`Score.dat`、`replay/AutoSave.trpy` 和刚才同步的 `replay/thSSS_ud0001.rpy`（4 字节）。见[运行验收记录](#运行验收记录)。 |
| SAVE-05 | [离线读取已有数据](../../docs/adaptation/storage.md#save-05) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| SAVE-06 | [游戏包与用户数据分开](../../docs/adaptation/storage.md#save-06) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |

## 语言与练习

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| PRACTICE-01 | [发布包中的练习能力](../../docs/adaptation/localization.md#practice-01) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| PRACTICE-02 | [开局前启用与语言](../../docs/adaptation/localization.md#practice-02) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| PRACTICE-03 | [游戏内练习状态](../../docs/adaptation/localization.md#practice-03) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| PRACTICE-04 | [关卡与资源参数](../../docs/adaptation/localization.md#practice-04) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| PRACTICE-05 | [练习 Replay 元数据](../../docs/adaptation/localization.md#practice-05) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| PRACTICE-06 | [触控功能键与指针](../../docs/adaptation/localization.md#practice-06) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| PRACTICE-07 | [练习语言跟随设置](../../docs/adaptation/localization.md#practice-07) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| PRACTICE-08 | [练习界面的可用功能](../../docs/adaptation/localization.md#practice-08) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| LANG-01 | [内置日语原文](../../docs/adaptation/localization.md#lang-01) | 待验证 | 首帧已呈现。本项操作还没做。见[运行验收记录](#运行验收记录)。 |
| LANG-02 | [语言包身份与发布清单](../../docs/adaptation/localization.md#lang-02) | 不适用 | normal：本作使用内置原文 |
| LANG-03 | [开局前挂载翻译与字体](../../docs/adaptation/localization.md#lang-03) | 不适用 | normal：本作使用内置原文 |
| LANG-04 | [语言包失败时回到原文](../../docs/adaptation/localization.md#lang-04) | 不适用 | normal：本作使用内置原文 |
| LANG-05 | [字体字符覆盖](../../docs/adaptation/localization.md#lang-05) | 不适用 | normal：本作使用内置原文 |
| LANG-06 | [缺译资源使用原文](../../docs/adaptation/localization.md#lang-06) | 不适用 | normal：本作使用内置原文 |

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
| VERIFY-01 | [全部内置 Demo 对照](../../docs/adaptation/validation.md#verify-01) | 待核对 | 未适配。没有原版 Demo、长 Replay 或逐逻辑帧基准。 |
| VERIFY-02 | [长 Replay 样本](../../docs/adaptation/validation.md#verify-02) | 待核对 | 未适配。没有原版 Demo、长 Replay 或逐逻辑帧基准。 |
| VERIFY-03 | [原版基准与内容哈希](../../docs/adaptation/validation.md#verify-03) | 待核对 | 未适配。没有原版 Demo、长 Replay 或逐逻辑帧基准。 |
| VERIFY-04 | [读取固定基准](../../docs/adaptation/validation.md#verify-04) | 待核对 | 未适配。没有原版 Demo、长 Replay 或逐逻辑帧基准。 |
| VERIFY-05 | [单独维护原版基准](../../docs/adaptation/validation.md#verify-05) | 待核对 | 未适配。没有原版 Demo、长 Replay 或逐逻辑帧基准。 |
| VERIFY-06 | [逐逻辑帧比较](../../docs/adaptation/validation.md#verify-06) | 待核对 | 未适配。没有原版 Demo、长 Replay 或逐逻辑帧基准。 |
| VERIFY-07 | [诊断构建与正常构建](../../docs/adaptation/validation.md#verify-07) | 待核对 | 未适配。没有原版 Demo、长 Replay 或逐逻辑帧基准。 |
| VERIFY-08 | [本作状态映射与结束条件](../../docs/adaptation/validation.md#verify-08) | 待核对 | 未适配。没有原版 Demo、长 Replay 或逐逻辑帧基准。 |

<!-- manual-verification -->

## 运行验收记录

环境：CachyOS，内置 Chromium，WebGL2 可用。页面 `http://127.0.0.1:8130/?test=thsss`。启动器使用 `EAGLER_DEVELOPMENT_GAMES=thsss`，内容目录为 `THSSS.Browser/wwwroot/assets`。Runtime 为修过 `FX/` 解包后的 Release 发布；这次又把 `configure.resources` 写入 `__thsssFiles`，再经 `stage:eagler` 写入 `build-eagler/`。THSSS-WASM `fb68e56` 与 eagler-touhou `361ec4efd` 都带未提交改动。

先前一次首帧：`ready` 之后是成功回复，2623 ms 时收到 `first-frame`，640×480 中心像素为不透明的 `(26, 48, 65)`。那次 `plays` 里没有 BGM。服务端对 `/games/thsss/music/ogg/OP.ogg` 返回 404，文件实际在内容目录的 `BGM/`。

这次按下启动后，`__thsssFiles` 有 18 个 `/BGM/*.ogg`。AudioContext 为 running，标题循环 `mem:./BGM/OP.ogg`。Z 与 Enter 确认到选人界面，画面出现博丽灵梦与少名针妙丸；再确认后 `plays` 依次出现 `Boss01.ogg`、`Stage01.ogg`，并播出 `se_ok00.wav`、`se_graze.wav`、`se_enep00.wav`、`se_tan00a.wav`、`se_item00.wav`。Esc 后画面为“游戏暂停 / Pause Menu”，同时播出 `se_pause.wav`。

存档：`Replay/thSSS_ud0001.rpy` 写入 `Prac` 四字节，读回一致；`../THSSS.Browser.wasm` 被拒绝。`sync` 后刷新并重新启动，列表仍有该文件、`Setting.INI`、`Score.dat` 和 `replay/AutoSave.trpy`，标题再次循环 `OP.ogg`。核对后删除该 Replay 并再次 `sync`。可见时 `frame-health` 为 60；强制 `document.hidden` 后下一笔为 15。`retry-music` 回复 Command is not available for this title。

INPUT／TOUCH：开启触摸后左侧出现开火／低速／BOMB／ESC／R。`InputSurface` 与 `TouchShift` 已绑定；标题为 menu，关卡为 play。方向键改变自机附近亮点；Z 连续 `se_plst00.wav`；X 与 BOMB 放出炸弹（现场可见）。标题静止点按 `direct-touch` 播出 `se_ok00.wav`。`touchMovementMode=touch` 时自右向左拖动，亮点从右侧移到左侧。`joystick` 推下标题菜单从 Game Start 移到 Replay，进局后左右推杆移动自机。`hold-button`／`toggle-button` 低速文案与切换正确；`doubleTapBombEnabled` 双击放出炸弹（手动确认）。`touch-controls` 开火锁住后有射击音；`escapeSerial` 进入暂停；`touch-cancel` 成功。布局编辑器可打开并保存。键位面板 WASD 改键生效；手柄映射与摇杆／按钮手动通过。无限速拖动、自由摇杆绕圈、双指、对话手势、灵敏度三档对照和放大镜没做完。

这次核对覆盖 BUILD-05、RUNTIME-02、DISPLAY-06、DISPLAY-07、AUDIO-01、AUDIO-07、SAVE-02、SAVE-03、SAVE-04、INPUT-01、INPUT-02、INPUT-03、INPUT-04、TOUCH-01、TOUCH-03、TOUCH-05、TOUCH-06、TOUCH-08、TOUCH-11、TOUCH-16、TOUCH-17。练习开关、无 BGM、音量、离线启动和其余 TOUCH 项没有做完。联机、MIDI、语言包按产品声明记为不适用。源码归属、来源登记、音乐重试命令和原版逐帧基准没有适配。
