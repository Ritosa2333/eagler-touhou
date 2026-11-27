# thsss 适配检查

[开发入口](README.md) · [练习说明](PRACTICE.md) · [检查方法](../../docs/adaptation/README.md) · [全部作品](../README.md)

源码目录是 `ports/thsss`，来自 THSSS-WASM 子树。本目录是单人接线，产品仍为 `testOnly`。构建模式：`normal`。

实现情况与实际验收分别记录。源码接口和模块检查通过后记为“已实现”；完成对应操作并附执行记录后记为“已验证”。

状态：`待核对`、`进行中`、`待验证`、`已实现`、`已验证`、`检查失败`、`不适用`。

[自动检查结果与源码身份](../../docs/adaptation/status.json) · `npm run ports:check -- --game=thsss --variant=both`

## 源码与构建

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| BUILD-01 | [源码目录与普通版／MP归属](../../docs/adaptation/build.md#build-01) | 已验证 | normal：[source-inputs](../../docs/adaptation/status.json) |
| BUILD-02 | [工具链与外部依赖](../../docs/adaptation/build.md#build-02) | 已验证 | normal：[dotnet-publish](../../docs/adaptation/status.json) |
| BUILD-03 | [对应模式独立构建](../../docs/adaptation/build.md#build-03) | 已验证 | normal：[dotnet-publish](../../docs/adaptation/status.json) |
| BUILD-04 | [Runtime 文件与来源记录](../../docs/adaptation/build.md#build-04) | 已验证 | normal：[staged-runtime](../../docs/adaptation/status.json) |
| BUILD-05 | [前端搭配单个游戏运行](../../docs/adaptation/build.md#build-05) | 已验证 | `EAGLER_DEVELOPMENT_GAMES=thsss` 启动器进入首帧，画面与操作正常。见[运行验收记录](#运行验收记录)。 |
| BUILD-06 | [源码与素材来源](../../docs/adaptation/build.md#build-06) | 已验证 | `LICENSE` 为 MIT，Copyright (c) 2026 LC_022。上游 `main` `c08d073` 见 [thsss-subtree-20261122.md](../../docs/migration/thsss-subtree-20261122.md)。素材目录写在 [README-UPSTREAM.md](README-UPSTREAM.md)，不入库。 |

## 启动与资源

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| RUNTIME-01 | [协议身份与会话代次](../../docs/adaptation/runtime.md#runtime-01) | 已实现 | normal：[runtime-protocol](../../docs/adaptation/status.json) |
| RUNTIME-02 | [就绪、配置、启动、首帧](../../docs/adaptation/runtime.md#runtime-02) | 已验证 | `ready` 之后是成功回复，2623 ms 时收到 `first-frame`，画面已经画出。见[运行验收记录](#运行验收记录)。 |
| RUNTIME-03 | [命令响应与错误信息](../../docs/adaptation/runtime.md#runtime-03) | 已实现 | normal：[runtime-protocol](../../docs/adaptation/status.json) |
| RUNTIME-04 | [后台、恢复与退出清理](../../docs/adaptation/runtime.md#runtime-04) | 已验证 | 切后台／恢复后声音与画面可继续；退出后监听器与临时输入已释放。见[运行验收记录](#运行验收记录)。 |
| RUNTIME-05 | [Runtime 与资源管理分工](../../docs/adaptation/runtime.md#runtime-05) | 已验证 | 启动器提供已装资源；Runtime 读取 `__thsssFiles`／内容目录后出首帧。见[运行验收记录](#运行验收记录)。 |
| RUNTIME-06 | [重开与常显判定点](../../docs/adaptation/runtime.md#runtime-06) | 已验证 | 暂停后 R／重开回到本局初始；常显判定点开关后移动与低速视觉正常。见[运行验收记录](#运行验收记录)。 |
| PACKAGE-01 | [共用游戏包存储](../../docs/adaptation/runtime.md#package-01) | 已实现 | normal：[package-store](../../docs/adaptation/status.json) |
| PACKAGE-02 | [DATA 身份与挂载目标](../../docs/adaptation/runtime.md#package-02) | 已实现 | normal：[package-store](../../docs/adaptation/status.json) |
| PACKAGE-03 | [完整更新后切换版本](../../docs/adaptation/runtime.md#package-03) | 已实现 | normal：[package-store](../../docs/adaptation/status.json) |
| PACKAGE-04 | [本地导入与在线安装一致](../../docs/adaptation/runtime.md#package-04) | 已实现 | normal：[package-store](../../docs/adaptation/status.json) |
| PACKAGE-05 | [已安装游戏离线启动](../../docs/adaptation/runtime.md#package-05) | 已验证 | 安装并运行一次后断网重开，启动器与 Runtime 本地资源可加载进局。见[运行验收记录](#运行验收记录)。 |
| PACKAGE-06 | [统一安装和更新入口](../../docs/adaptation/runtime.md#package-06) | 已实现 | normal：[package-store](../../docs/adaptation/status.json) |

## 键盘、手柄与触控

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| INPUT-01 | [键盘逻辑输入](../../docs/adaptation/input.md#input-01) | 已验证 | 方向键移动后画面亮点位置改变。按住 Z 连续播出 `se_plst00.wav`。X 放出炸弹（现场可见）。菜单方向键播出 `se_select00.wav`。见[运行验收记录](#运行验收记录)。 |
| INPUT-02 | [启动器键盘改键](../../docs/adaptation/input.md#input-02) | 已验证 | WASD 布局下 `KeyS` 经 `__eaglerKeyboardInputBridge.submit` 映射为 `ArrowDown`；标题菜单从 Game Start 逐项移到 Replay。见[运行验收记录](#运行验收记录)。 |
| INPUT-03 | [启动器手柄映射](../../docs/adaptation/input.md#input-03) | 已验证 | 手动：启动器手柄改键保存后进局，映射立即生效。见[运行验收记录](#运行验收记录)。 |
| INPUT-04 | [手柄方向与按钮](../../docs/adaptation/input.md#input-04) | 已验证 | 手动：摇杆移动、按钮射击／Bomb／低速／菜单与键盘同路径。见[运行验收记录](#运行验收记录)。 |
| INPUT-05 | [多种输入独立持有](../../docs/adaptation/input.md#input-05) | 已实现 | normal：[launcher-input-owners](../../docs/adaptation/status.json) |
| INPUT-06 | [失焦与断开释放](../../docs/adaptation/input.md#input-06) | 已验证 | 按住移动时切后台／失焦后输入释放；重获焦点可继续操作。见[运行验收记录](#运行验收记录)。 |
| INPUT-07 | [原作菜单与对话输入](../../docs/adaptation/input.md#input-07) | 已验证 | 键盘与手柄完成标题、难度、选人确认与对话快进；Esc 打开暂停“继续游戏”。见[运行验收记录](#运行验收记录)。 |
| TOUCH-01 | [限速滑动移动](../../docs/adaptation/input.md#touch-01) | 已验证 | `touchMovementMode=touch`，`InputSurface=play`。自右向左 `direct-touch` 拖动后，画面亮点从右侧集中区移到左侧。见[运行验收记录](#运行验收记录)。 |
| TOUCH-02 | [无限速滑动移动](../../docs/adaptation/input.md#touch-02) | 已验证 | 开局前设为 `touch-unlimited`，远距拖动跟随请求位置；改回 `touch` 后恢复限速。见[运行验收记录](#运行验收记录)。 |
| TOUCH-03 | [方向式虚拟摇杆](../../docs/adaptation/input.md#touch-03) | 已验证 | `touchMovementMode=joystick`。标题推下：菜单从 Game Start 移到 Replay；进局后 `joystickX` 推左右，自机在 play 表面移动。见[运行验收记录](#运行验收记录)。 |
| TOUCH-04 | [自由方向虚拟摇杆](../../docs/adaptation/input.md#touch-04) | 已验证 | 开局前设为 `joystick-free`。局内绕圈与变推杆幅度：自机连续转向，逻辑步进仍为原速。见[运行验收记录](#运行验收记录)。 |
| TOUCH-05 | [按住低速](../../docs/adaptation/input.md#touch-05) | 已验证 | `touchFocusMode=hold-button`，按钮文案“低速（按住）”。局内 `focusEnabled=true` 时与摇杆一并送出，抬起后 `focusEnabled=false`。见[运行验收记录](#运行验收记录)。 |
| TOUCH-06 | [切换低速](../../docs/adaptation/input.md#touch-06) | 已验证 | `touchFocusMode=toggle-button`，按钮文案“低速（切换）”。连续点击 `aria-pressed` 在 true／false 间切换。见[运行验收记录](#运行验收记录)。 |
| TOUCH-07 | [双指低速](../../docs/adaptation/input.md#touch-07) | 已验证 | `touch`／`touch-unlimited` 下 `touchFocusMode=two-finger`。移动中按下第二指进入低速，抬起恢复；移动指独立。见[运行验收记录](#运行验收记录)。 |
| TOUCH-08 | [射击、Bomb 与暂停](../../docs/adaptation/input.md#touch-08) | 已验证 | 开启触摸后左侧有开火／低速／BOMB／ESC。`touch-controls` 开火锁住后有 `se_plst00.wav`；键盘 X 与 BOMB 放出炸弹（现场可见）；`escapeSerial` 后进入暂停菜单。见[运行验收记录](#运行验收记录)。 |
| TOUCH-09 | [暂时禁移与触点连续性](../../docs/adaptation/input.md#touch-09) | 已验证 | 死亡后同一触点保留；可移动后从当前位置续拖。复活瞬间有小跳变，未跳回旧拖点。见[运行验收记录](#运行验收记录)。 |
| TOUCH-10 | [无限速移动落率记录](../../docs/adaptation/input.md#touch-10) | 已验证 | 仅开 `touch-unlimited` 并实际移动后结算／保存 Replay，处理落率为 `100.0%`。见[运行验收记录](#运行验收记录)。 |
| TOUCH-11 | [双击 Bomb](../../docs/adaptation/input.md#touch-11) | 已验证 | `doubleTapBombEnabled` 开启后，play 表面双击触发 Bomb（手动确认可见）。见[运行验收记录](#运行验收记录)。 |
| TOUCH-12 | [菜单与对话手势](../../docs/adaptation/input.md#touch-12) | 已验证 | 标题静止点按 `se_ok00.wav`；滑动／摇杆可选菜单。对话短按确认、长按跳过。见[运行验收记录](#运行验收记录)。 |
| TOUCH-13 | [确认键的新按下边沿](../../docs/adaptation/input.md#touch-13) | 已验证 | 保持射击进入对话后再触控确认，产生新按下边沿并推进流程。见[运行验收记录](#运行验收记录)。 |
| TOUCH-14 | [100–300% 灵敏度](../../docs/adaptation/input.md#touch-14) | 已验证 | 同一段 0.05 画面宽的限速滑动，开局灵敏度 100%／200%／300% 写入录像的位移总和为 28／57／86，每帧绝对值最大都是 4。见[运行验收记录](#运行验收记录)。 |
| TOUCH-15 | [触控与其他输入隔离](../../docs/adaptation/input.md#touch-15) | 已验证 | 键盘保持射击时取消触控或切换触控模式，键盘动作继续。见[运行验收记录](#运行验收记录)。 |
| TOUCH-16 | [触控取消与生命周期](../../docs/adaptation/input.md#touch-16) | 已验证 | `touch-cancel` 经 `__eaglerDirectInputBridge.submit` 成功。隐藏页面后的残留触点没单独测。见[运行验收记录](#运行验收记录)。 |
| TOUCH-17 | [共用布局编辑器](../../docs/adaptation/input.md#touch-17) | 已验证 | 未开局打开“按键布局与触控设置”，出现移动方式（八向／自由等）与灵敏度面板，可保存布局。见[运行验收记录](#运行验收记录)。 |
| TOUCH-18 | [共用重开按钮](../../docs/adaptation/input.md#touch-18) | 已验证 | 暂停后点 `#touchRestart`，播出 `se_ok00.wav`，BGM 从关卡曲重新开始，画面回到关卡。见[运行验收记录](#运行验收记录)。 |
| TOUCH-19 | [共用双指放大镜](../../docs/adaptation/input.md#touch-19) | 已验证 | 对局中双指把视口放到 `scale(3)`，画面框从 1400×900 变为 4200×2700，画面中心仍映射到 (0.5, 0.5)。复位后回到 `scale(1)`。见[运行验收记录](#运行验收记录)。 |

## 画面与声音

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| DISPLAY-01 | [固定逻辑步进](../../docs/adaptation/presentation.md#display-01) | 已实现 | normal：[normal-presentation](../../docs/adaptation/status.json) |
| DISPLAY-02 | [跟随显示器刷新率](../../docs/adaptation/presentation.md#display-02) | 已实现 | normal：[normal-presentation](../../docs/adaptation/status.json) |
| DISPLAY-03 | [额外绘制无逻辑副作用](../../docs/adaptation/presentation.md#display-03) | 已实现 | normal：[normal-presentation](../../docs/adaptation/status.json) |
| DISPLAY-04 | [连续视觉字段插值](../../docs/adaptation/presentation.md#display-04) | 已验证 | 高刷下自机／弹幕连续位置平滑，离散贴图与状态按逻辑帧切换。见[运行验收记录](#运行验收记录)。 |
| DISPLAY-05 | [出生、销毁和瞬移](../../docs/adaptation/presentation.md#display-05) | 已验证 | 出生、死亡与对象复用时画面直接切到新状态，无残留旧位姿。见[运行验收记录](#运行验收记录)。 |
| DISPLAY-06 | [暂停画面的表现](../../docs/adaptation/presentation.md#display-06) | 已验证 | Esc 后出现“游戏暂停 / Pause Menu”，关卡画面留在菜单后面，并播出 `se_pause.wav`。见[运行验收记录](#运行验收记录)。 |
| DISPLAY-07 | [实际呈现后报告首帧](../../docs/adaptation/presentation.md#display-07) | 已验证 | `first-frame` 到达后，640×480 画布中心像素为不透明的 `(26, 48, 65)`。见[运行验收记录](#运行验收记录)。 |
| DISPLAY-08 | [限制到 60 Hz](../../docs/adaptation/presentation.md#display-08) | 已验证 | 高刷设备上开 60 Hz 限制后仅画面频率变化，逻辑速度与输入一致。见[运行验收记录](#运行验收记录)。 |
| AUDIO-01 | [正常 OGG 音乐](../../docs/adaptation/presentation.md#audio-01) | 已验证 | 启动器音乐为 OGG。18 首曲目进入 `/BGM/` 后，标题循环 `OP.ogg`，选人进入关卡后切到 `Boss01.ogg`、`Stage01.ogg`。AudioContext 为 running。见[运行验收记录](#运行验收记录)。 |
| AUDIO-02 | [无 BGM 模式](../../docs/adaptation/presentation.md#audio-02) | 已验证 | 启动器选无 BGM 后背景曲关闭，射击等音效仍正常。见[运行验收记录](#运行验收记录)。 |
| AUDIO-03 | [切曲、渐变、暂停、停止](../../docs/adaptation/presentation.md#audio-03) | 已验证 | 标题 `OP.ogg` → 关卡 `Boss01.ogg`／`Stage01.ogg`；暂停与退出时切曲／停止跟随状态。见[运行验收记录](#运行验收记录)。 |
| AUDIO-04 | [返回前台后恢复声音](../../docs/adaptation/presentation.md#audio-04) | 已验证 | 切后台自动暂停 BGM，音效不受影响；返回前台从原游标与循环状态续播。见[运行验收记录](#运行验收记录)。 |
| AUDIO-05 | [音乐资源错误信息](../../docs/adaptation/presentation.md#audio-05) | 已验证 | `/BGM` 缺曲软失败上报 `music-error`；`retry-music` 接受并保留未解决项。选无 BGM 清空待重试。单测 `music-transfer.test.mjs`；playwright AUDIO-05 通过。 |
| AUDIO-06 | [高刷新与音频事件分开](../../docs/adaptation/presentation.md#audio-06) | 已验证 | 高刷下同一真实事件只发声一次，无随显示帧重复触发。见[运行验收记录](#运行验收记录)。 |
| AUDIO-07 | [音效即时输出](../../docs/adaptation/presentation.md#audio-07) | 已验证 | `OP.ogg` 循环期间菜单确认播出 `se_ok00.wav`；关卡中继续播出 `se_graze.wav`、`se_enep00.wav`、`se_tan00a.wav`、`se_item00.wav`。见[运行验收记录](#运行验收记录)。 |
| MIDI-01 | [声明 MIDI 选项](../../docs/adaptation/presentation.md#midi-01) | 不适用 | normal：本作使用 OGG 音乐 |
| MIDI-02 | [可听到的 MIDI 输出](../../docs/adaptation/presentation.md#midi-02) | 不适用 | normal：本作使用 OGG 音乐 |
| MIDI-03 | [MIDI 生命周期](../../docs/adaptation/presentation.md#midi-03) | 不适用 | normal：本作使用 OGG 音乐 |
| MIDI-04 | [MIDI 与 OGG 分开](../../docs/adaptation/presentation.md#midi-04) | 不适用 | normal：本作使用 OGG 音乐 |

## 存档与 Replay

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| REPLAY-01 | [导入原版 Replay](../../docs/adaptation/storage.md#replay-01) | 已验证 | 导入原版 `.rpy`／`.trpy` 后播放到声明结束点，画面与结束时机正确。见[运行验收记录](#运行验收记录)。 |
| REPLAY-02 | [按原版格式导出可表达输入](../../docs/adaptation/storage.md#replay-02) | 已验证 | 键位局导出 `.rpy`，可用原版兼容路径读回。见[运行验收记录](#运行验收记录)。 |
| REPLAY-03 | [记录实际生效输入](../../docs/adaptation/storage.md#replay-03) | 已验证 | `replay/AutoSave.trpy` 每逻辑帧 2 字节键位加 4 字节触控。右移+射击写成同一个键字 `5120`；触控位移是 int16，不另写方向键。见[运行验收记录](#运行验收记录)。 |
| REPLAY-04 | [扩展格式与版本](../../docs/adaptation/storage.md#replay-04) | 已验证 | `.trpy` 带 `TouchDelta=1`；inspect 跳过扩展行后回放还原正确。见[运行验收记录](#运行验收记录)。 |
| REPLAY-05 | [Replay 文件管理](../../docs/adaptation/storage.md#replay-05) | 已验证 | IndexedDB `thsss-saves` 列表 `Replay/thSSS_*.rpy`／`.trpy`；导入后重开仍可见。见[运行验收记录](#运行验收记录)。 |
| REPLAY-06 | [回放与实时输入隔离](../../docs/adaptation/storage.md#replay-06) | 已验证 | 回放中按射击／触控不改变回放轨迹。见[运行验收记录](#运行验收记录)。 |
| REPLAY-07 | [回放独立于显示刷新率](../../docs/adaptation/storage.md#replay-07) | 已验证 | 60／高刷下回放步进一致，由 60 Hz 逻辑时钟驱动。见[运行验收记录](#运行验收记录)。 |
| SAVE-01 | [已声明的存储根](../../docs/adaptation/storage.md#save-01) | 已验证 | `ready.saveRoot` 为 `/savesthsss`。IndexedDB `thsss-saves` 有 `Setting.INI`、`Score.dat`、`Replay/AutoSave.trpy`。见[运行验收记录](#运行验收记录)。 |
| SAVE-02 | [用户文件允许范围](../../docs/adaptation/storage.md#save-02) | 已验证 | `Replay/thSSS_ud0001.rpy` 可写可读。`../THSSS.Browser.wasm` 返回 errno 1，Path is not a user file。见[运行验收记录](#运行验收记录)。 |
| SAVE-03 | [明确持久化时机](../../docs/adaptation/storage.md#save-03) | 已验证 | 写入 4 字节 Replay 后 `sync` 成功，关闭页面再启动仍能读到该文件。见[运行验收记录](#运行验收记录)。 |
| SAVE-04 | [退出重开后数据保留](../../docs/adaptation/storage.md#save-04) | 已验证 | 新实例列出 `Setting.INI`、`Score.dat`、`replay/AutoSave.trpy` 和刚才同步的 `replay/thSSS_ud0001.rpy`（4 字节）。见[运行验收记录](#运行验收记录)。 |
| SAVE-05 | [离线读取已有数据](../../docs/adaptation/storage.md#save-05) | 已验证 | 断网后重开，原有成绩与 Replay 可继续读取和保存。见[运行验收记录](#运行验收记录)。 |
| SAVE-06 | [游戏包与用户数据分开](../../docs/adaptation/storage.md#save-06) | 已验证 | 切换／更新游戏包后用户成绩与 Replay 保留。见[运行验收记录](#运行验收记录)。 |

## 语言与练习

| 编号 | 检查项 | normal | 记录 |
| --- | --- | --- | --- |
| PRACTICE-01 | [发布包中的练习能力](../../docs/adaptation/localization.md#practice-01) | 已验证 | 当前 Release 运行时打开原生练习面板，与目录 `thprac` / `native` 一致。见[运行验收记录](#运行验收记录)。 |
| PRACTICE-02 | [开局前启用与语言](../../docs/adaptation/localization.md#practice-02) | 已验证 | 开局前 thprac 为开。面板标题为「練習」，语言为 `ja-JP`。见[运行验收记录](#运行验收记录)。 |
| PRACTICE-03 | [游戏内练习状态](../../docs/adaptation/localization.md#practice-03) | 已验证 | `thpracEnabled=false` 开局后 Backspace 不打开练习面板。标题 Practice 进入原版难度选择，再进入 STAGE SELECT。见[运行验收记录](#运行验收记录)。 |
| PRACTICE-04 | [关卡与资源参数](../../docs/adaptation/localization.md#practice-04) | 已验证 | 「ステージ 6」把林间关卡换成星空。保存的 Replay 关卡为 St6，资源为残机 8、炸弹 0、Power 120。见[运行验收记录](#运行验收记录)。 |
| PRACTICE-05 | [练习 Replay 元数据](../../docs/adaptation/localization.md#practice-05) | 已验证 | `replay/thSSS_01.trpy` 在 `TouchDelta=1` 后写 `Prac=1,1,1,1,8,0,120`。回放菜单选 Stage06 后播出 `Stage06.ogg`。见[运行验收记录](#运行验收记录)。 |
| PRACTICE-06 | [触控功能键与指针](../../docs/adaptation/localization.md#practice-06) | 已验证 | Backspace 打开原生面板；触控 F1 切换「無敵」。见[运行验收记录](#运行验收记录)。 |
| PRACTICE-07 | [练习语言跟随设置](../../docs/adaptation/localization.md#practice-07) | 已验证 | 站点界面为英文，游戏语言是内置日文。练习菜单为日文。见[运行验收记录](#运行验收记录)。 |
| PRACTICE-08 | [练习界面的可用功能](../../docs/adaptation/localization.md#practice-08) | 已验证 | 无敌、禁炸弹、失焦三项按下后写入 `Prac=1,1,1,1`。关卡按钮跳到 St6。失焦暂停由 `Form_Main_Deactivate` 处理，浏览器宿主没有调用 `RaiseDeactivate`。见[运行验收记录](#运行验收记录)。 |
| LANG-01 | [内置日语原文](../../docs/adaptation/localization.md#lang-01) | 已验证 | 语言只有「Japanese (original)」。资源请求里没有语言包，局内 HUD 为原文。见[运行验收记录](#运行验收记录)。 |
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
| VERIFY-01 | [全部内置 Demo 对照](../../docs/adaptation/validation.md#verify-01) | 不适用 | 没有机制问题，不做像素与逻辑帧对照。 |
| VERIFY-02 | [长 Replay 样本](../../docs/adaptation/validation.md#verify-02) | 不适用 | 没有机制问题，不做像素与逻辑帧对照。 |
| VERIFY-03 | [原版基准与内容哈希](../../docs/adaptation/validation.md#verify-03) | 不适用 | 没有机制问题，不做像素与逻辑帧对照。 |
| VERIFY-04 | [读取固定基准](../../docs/adaptation/validation.md#verify-04) | 不适用 | 没有机制问题，不做像素与逻辑帧对照。 |
| VERIFY-05 | [单独维护原版基准](../../docs/adaptation/validation.md#verify-05) | 不适用 | 没有机制问题，不做像素与逻辑帧对照。 |
| VERIFY-06 | [逐逻辑帧比较](../../docs/adaptation/validation.md#verify-06) | 不适用 | 没有机制问题，不做像素与逻辑帧对照。 |
| VERIFY-07 | [诊断构建与正常构建](../../docs/adaptation/validation.md#verify-07) | 不适用 | 没有机制问题，不做像素与逻辑帧对照。 |
| VERIFY-08 | [本作状态映射与结束条件](../../docs/adaptation/validation.md#verify-08) | 不适用 | 没有机制问题，不做像素与逻辑帧对照。 |

<!-- manual-verification -->

## 运行验收记录

环境：CachyOS，内置 Chromium，WebGL2 可用。页面 `http://127.0.0.1:8130/?test=thsss`。启动器使用 `EAGLER_DEVELOPMENT_GAMES=thsss`，内容目录为 `THSSS.Browser/wwwroot/assets`。Runtime 为修过 `FX/` 解包后的 Release 发布；这次又把 `configure.resources` 写入 `__thsssFiles`，再经 `stage:eagler` 写入 `build-eagler/`。THSSS-WASM `fb68e56` 与 eagler-touhou `361ec4efd` 都带未提交改动。

先前一次首帧：`ready` 之后是成功回复，2623 ms 时收到 `first-frame`，640×480 中心像素为不透明的 `(26, 48, 65)`。那次 `plays` 里没有 BGM。服务端对 `/games/thsss/music/ogg/OP.ogg` 返回 404，文件实际在内容目录的 `BGM/`。

这次按下启动后，`__thsssFiles` 有 18 个 `/BGM/*.ogg`。AudioContext 为 running，标题循环 `mem:./BGM/OP.ogg`。Z 与 Enter 确认到选人界面，画面出现博丽灵梦与少名针妙丸；再确认后 `plays` 依次出现 `Boss01.ogg`、`Stage01.ogg`，并播出 `se_ok00.wav`、`se_graze.wav`、`se_enep00.wav`、`se_tan00a.wav`、`se_item00.wav`。Esc 后画面为“游戏暂停 / Pause Menu”，同时播出 `se_pause.wav`。

存档：`Replay/thSSS_ud0001.rpy` 写入 `Prac` 四字节，读回一致；`../THSSS.Browser.wasm` 被拒绝。`sync` 后刷新并重新启动，列表仍有该文件、`Setting.INI`、`Score.dat` 和 `replay/AutoSave.trpy`，标题再次循环 `OP.ogg`。核对后删除该 Replay 并再次 `sync`。可见时 `frame-health` 为 60；强制 `document.hidden` 后下一笔为 15。AUDIO-05：`/BGM` 缺曲／坏曲经 `music-error`／`music-incomplete` 上报，`retry-music` 接受并保留未解决项；选无 BGM 清空待重试。

INPUT／TOUCH：开启触摸后左侧出现开火／低速／BOMB／ESC／R。`InputSurface` 与 `TouchShift` 已绑定；标题为 menu，关卡为 play。方向键改变自机附近亮点；Z 连续 `se_plst00.wav`；X 与 BOMB 放出炸弹（现场可见）。标题静止点按 `direct-touch` 播出 `se_ok00.wav`。`touchMovementMode=touch` 时自右向左拖动，亮点从右侧移到左侧。`joystick` 推下标题菜单从 Game Start 移到 Replay，进局后左右推杆移动自机。`hold-button`／`toggle-button` 低速文案与切换正确；`doubleTapBombEnabled` 双击放出炸弹（手动确认）。`touch-controls` 开火锁住后有射击音；`escapeSerial` 进入暂停；`touch-cancel` 成功。布局编辑器可打开并保存。键位面板 WASD 改键生效；手柄映射与摇杆／按钮手动通过。自由摇杆绕圈、双指低速、死后同指续拖、无限速远距对照、无限速落率、对话短按／长按已手动通过。灵敏度 100% 与放大镜缩放见 2026-11-27 记录。

这次核对覆盖 BUILD-05、RUNTIME-02、DISPLAY-06、DISPLAY-07、AUDIO-01、AUDIO-07、SAVE-02、SAVE-03、SAVE-04、INPUT-01、INPUT-02、INPUT-03、INPUT-04、TOUCH-01、TOUCH-03、TOUCH-05、TOUCH-06、TOUCH-08、TOUCH-11、TOUCH-16、TOUCH-17。联机、MIDI、语言包按产品声明记为不适用。AUDIO-05 已接入（实现＋单测）。VERIFY 按没有机制问题跳过逐帧对照。

后续手动＋视觉核对：BUILD-05、RUNTIME-05、RUNTIME-06、PACKAGE-05、INPUT-06、INPUT-07、TOUCH-13、TOUCH-15、DISPLAY-04、DISPLAY-05、DISPLAY-08、AUDIO-02、AUDIO-03、AUDIO-06、REPLAY-01、REPLAY-02、REPLAY-04、REPLAY-05、REPLAY-06、REPLAY-07、SAVE-05、SAVE-06。AUDIO-04：切后台自动暂停 BGM，回前台从原游标续播。RUNTIME-04：切后台／恢复后声音与画面可继续，退出后监听器与临时输入已释放。

2026-11-27，构建模式 `normal`，页面 `http://127.0.0.1:8130/en.html?test=thsss&game=thsss`。开局选项：thprac 开、触控开、移动方式为限速拖动、灵敏度 100%、放大镜开、重开按钮开、游戏语言 Japanese (original)。

SAVE-01：`ready` 的 `saveRoot` 为 `/savesthsss`。`list` 与 IndexedDB `thsss-saves` / `files` 给出 `Setting.INI`（401 字节，`UseDirectInput=0`，Shoot=Z）、`Score.dat`（76512 字节）、`Replay/AutoSave.trpy`。与目录声明一致。本作没有 MP 存储根。

LANG-01：语言选择只有「Japanese (original)」，选项行隐藏，界面写着 Built-in。这次加载的资源是 `thsss.dat` 与卡片图，没有语言包。局内侧栏是原文「最高得分 / 得分 / 剩余人」。

REPLAY-03：`AutoSave.trpy` 为 8190 字节、1365 帧，每帧 6 字节（键字 2 字节 + int16 dx/dy）。右移加射击是同一个键字 `5120`（右键位 + Z 键位），63 帧。触控右移是键字 `0` 加正 dx，529 帧，不另写方向键。射击键位是 `1024`，键盘与触控开火共用这一位。手柄右移+射击注入的也是这一组键位。

练习：Backspace 打开 `.thsss-prac`。英文站点下标题是「練習」，条目为「無敵 / ボム無効 / 非アクティブでも止めない」。F1、F2、F3 把这三项的 `aria-pressed` 设为 true。残机、炸弹、Power 写入 7、1、80 后点了「初期資源を適用」，并点了「ステージ 6」。触控面板上的 F1 再点一次，把「無敵」从 true 切回 false。局内录像还没有收尾的 `Prac=` 行。关闭练习后的原版选关、以及无敌和关卡跳转在画面上的结果，留在下一局。

TOUCH-14：这次开局灵敏度是 100%。录像里触控 dx 的绝对值最大为 4，对应原作高速 4.5 的四舍五入。200% 和 300% 要在新的开局里用同一段滑动再记一次。

TOUCH-18：画面左侧有共用 R（`#touchRestart`）。结算表停在 1'27 之后，点 R 与发送 KeyR 都没有换到新的一局。

TOUCH-19：两个触点从相距约 100px 拉开后，`#gameViewport` 的变换为 `scale(3)`，标尺 300%。点复位后回到 `translate3d(0px, 0px, 0px) scale(1)`，标尺 100%。放大后的触点坐标留到对局仍在步进时再量。

2026-11-27 续，构建模式 `normal`。限速拖动。同一段滑动是按下 (0.35, 0.55)、移到 (0.40, 0.55)，按住到队列排空再抬起。`configure` 的 `touchSensitivity` 分别为 100、200、300。

TOUCH-14：录像里这段位移的绝对值最大都是 4。总和分别是 28（7 帧）、57（14 帧为 4、1 帧为 1）、86（21 帧为 4、1 帧为 2）。三段都顶在原作高速 4.5 四舍五入后的上限，总距离按 100%／200%／300% 拉开。

TOUCH-18：暂停播出 `se_pause.wav`。点 `#touchRestart` 后播出 `se_ok00.wav`，BGM 按 Stage01／Boss01／Stage01 重新开始，画面回到林间关卡。

TOUCH-19：对局中两个触点把 `#gameViewport` 变为 `matrix(3, 0, 0, 3, -525, -462.5)`，标尺 300%，画面框 4200×2700。框中心在 1400×900 和 4200×2700 都映射到 (0.5, 0.5)；距中心 200px 的点从 x=0.357 变为 x=0.452。复位后为 `matrix(1, 0, 0, 1, 0, 0)`，标尺 100%。

PRACTICE-04／05／08：练习面板写入残机 8、炸弹 0、Power 120，三项开关为按下。点「ステージ 6」后画面从林间变为星空。暂停里保存到 `replay/thSSS_01.trpy`，收尾为 `TouchDelta=1` 与 `Prac=1,1,1,1,8,0,120`，关卡行是 St6。同目录没有旁路文件。回放菜单槽 01 显示 26/11/27 20:39、Reimu、Stage06；选中 Stage06 后播出 `Stage06.ogg`，画面为同一片星空。加载时 `Prac=` 行进入 `RestoreTrailer`。

PRACTICE-03：`configure.options.thpracEnabled` 为 false。Backspace 后 `.thsss-prac` 保持隐藏。标题菜单进入 Practice，经过 RANK SELECT 与自机选择，出现 STAGE SELECT，条目含 Stage1。

PRACTICE-08：失焦暂停在 `Form_Main_Deactivate`，条件是未打开 `IgnoreFocusLoss`。`Form.RaiseDeactivate` 在本浏览器宿主没有调用方，这条暂停不会被页面失焦触发。

TOUCH-04／07／09（2026-11-24 手动）：开局前改设置。`joystick-free` 局内绕圈与变幅度连续转向、原速步进。滑动模式下 `two-finger` 第二指低速、移动指独立。死后同指续拖，从当前位置重建；复活瞬间有小跳变。对照 [input.md](../../docs/adaptation/input.md) 三项记为已验证。

TOUCH-02／10／12（2026-11-24 手动）：`touch-unlimited` 远距跟随请求位置，改回 `touch` 恢复限速。仅无限速实际移动后结算／Replay 落率 `100.0%`。菜单点按与滑动已有记录；对话短按确认、长按跳过通过。三项记为已验证。

BUILD：`node tools/workspace.mjs select thsss --variant=normal` 列出 launcher、common、tools、docs、ports/thsss。本机 .NET SDK 10.0.404、wasm-tools 10.0.115、Bun 1.4.4。`dotnet publish THSSS.Browser/THSSS.Browser.csproj -c Release -p:StageGameAssets=false` 退出码 0，`THSSS.Browser.wasm` sha256 `4e719e4cdbf9c07162c31aa82e6e7ad0365129470e2ca4ef12fcf8997fa5f6c3`。帧步进拆出后重新 `npm run stage:eagler`，`shell.mjs` 171717 字节，wasm 哈希未变，`runtime-files.json` 与 5 个运行文件一致。`node tools/ports/check.mjs --game=thsss --variant=normal` 为 10/10。

VERIFY-01 至 VERIFY-08：没有机制问题，不做像素与逻辑帧对照，八项记为不适用。
