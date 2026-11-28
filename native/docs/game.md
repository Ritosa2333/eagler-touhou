# 游戏详情、通关进度与单作设置

[当前截图登记](evidence/current/README.md)记录设置页图像及源码校验值。本页其余 Flutter 图片按各自实现阶段引用。

[桌面详情](evidence/web/desktop-game-detail.png) | [手机详情](evidence/web/mobile-game-detail.png) | [桌面设置](evidence/web/desktop-game-settings.png) | [窄屏设置](evidence/web/mobile-narrow-game-settings.png) | [手机设置 Pencil](evidence/pencil/mobile-game-settings.png)

## 详情与启动

| 编号 | 元素 | 排布和交互 | 样例 |
| --- | --- | --- | --- |
| GAME-01 | 返回 | 左上角，作用于打开此详情的导航栈。桌面独立作品侧栏和顶栏保留位置。 | 书架、搜索与联机目录进入；反复返回；桌面鼠标和手机系统返回。 |
| GAME-02 | 作品标识 | 封面保持方形，作品名在旁，编号与作品名同基线，英文名在下。1440 px 桌面使用 174 px 封面、48 px 标题和 16 px 英文名。手机详情标题 20 px，设置页作品名 18 px。长英文允许自然断行。 | th06 英文长标题、th07.5 展示编号与路由标识、th11 窄宽。 |
| GAME-03 | 运行数据 | 最后运行日期和游戏时间成对。桌面在启动同一行，手机在作品标识下。有云存档时在旁显示云同步标志。缺值使用短横线。深色封面背景在亮暗主题均使用浅色文字。 | 游客、已有统计、未知时间和存档同步中。最后运行日期读取单机成功运行的本机记录，以本地时区显示月日。 |
| GAME-04 | 收藏 | 手机固定在详情头部右上，与返回对应。桌面位于动作组尾部。 | 点按、取消、切回书架，检查书架与侧栏同步。 |
| GAME-05 | 启动 | 主操作优先占据剩余宽度。原生占位返回同一游戏详情。 | 单击及连续点击，确认每次只有一份启动流程。 |
| GAME-06 | 导入与设置 | 导入用于游戏资源。设置入口在同一动作行，宽度不足时保留设置图标和可访问名称。 | 320 px 中英文，打开设置，回到详情原位。 |
| GAME-07 | 通关与好友列 | 登录后显示好友区。详情内容超过 740 px 时，好友区在右侧，占 184 px，通关和符卡区使用剩余空间。窄屏时好友区接在统计下方。 | 1440 px 桌面、390 和 320 px 竖屏。 |
| GAME-07.1 | 好友游玩记录 | 从自己的好友列表中取最多三位公开了本作游玩证据的好友，按服务返回顺序显示。证据来自游玩时长、次数、通关或分数。 | th07 样例包括其他作品、私密记录、次数、通关、分数和超过三人。 |
| GAME-07.2 | 加载、空态与失败 | 账号加载后独立读取好友区。好友列表不可见或无符合记录时，显示空态。读取失败且没有可用记录时，可就地重试。 | 游客访问、列表请求失败、重试后不可见、关系刷新。 |
| GAME-07.3 | 好友入口与返回 | 点击昵称进玩家个人主页。「查看好友」打开当前账号的好友标签。返回保留同一详情的滚动位置。 | 两个入口从详情打开并用系统返回。 |

## 通关与符卡

详情头部和统计正文共用居中的 1040 px 最大内容框及同一条左边线，手机左右边距为 16 px。封面背景复用本作图片并模糊压暗。通关、符卡和好友使用同一个 19 px 分区标题角色。原稿的导入文案与手机收藏位置按后续要求更新。[同状态整改对照](comparison.md#详情首屏整改)

| 编号 | 元素 | 排布和状态 | 样例 |
| --- | --- | --- | --- |
| CLEAR-01 | 通关表 | 第一列显示机体，分支列显示配置。难度列等宽并优先收缩色块。机体名和分支代码保持完整。 | th06 两角色 A/B，th07 多机体和 Phantasm，th11 A/B/C，320 px。 |
| CLEAR-02 | 难度集合 | 默认 Easy、Normal、Hard、Lunatic；按作品能力产生 Extra、Phantasm、Overdrive。存档实际提供的难度也可出现。 | th19 无 Extra，th20 Overdrive，th15 Legacy 和 Pointdevice 切换。 |
| CLEAR-03 | 色块和图例 | 区分一命通关、已通关、未通关、未尝试、存档未提供、未读取。通关与符卡卡片共用细边框和页面底色，未读取、未尝试及未通关各有独立的中性色层次。图例在表下可换行。 | 亮暗主题都能辨认未读取格，未通关另有描边。各状态含语义标签，读屏与长按不依赖颜色。 |
| CLEAR-04 | 分数详情 | 可查询的数据格提供最高分。辅助技术可读出机体、分支、难度和通关状态。按机体、配置、难度及模式记住当前选择。 | th07 选灵梦 A 的 Easy，刷新云端分数后检查选中格、符卡范围、分数及页面位置。离开设置后返回同一表。 |
| CLEAR-05 | 多作品表 | 每行显示封面、作品名和共有难度；点击作品名进入本区的单作视图。 | 全部作品切换至 th08，外围滚动和侧栏保持稳定。 |
| CLEAR-06 | 符卡视图 | 标题下说明统计范围。右侧切换收取图和列表；分别呈现按关卡的 Midboss/Boss、收取进度和完整符卡名。悬停、键盘聚焦及点按显示编号、原名和收取次数。 | 核对长名称、三位数次数与格子基线。Tab 移到下个格时提示随焦点更新，Esc 关闭提示。 |
| CLEAR-07 | 音乐、云同步和来源 | 通关与符卡数据读取实际存档或服务结果。云上传动作、登录入口、空态、服务错误给出对应反馈。 | 游客、授权账号、损坏存档、成功同步、远端数据缺失。 |

Flutter 游戏详情使用共用通关表；个人主页已改用正式站应用内网页。单作表按机体分组，作品支持该难度或存档含该难度时显示该列。320 px 使用难度简称。th06、th07 与 th08 的符卡区随选中的机体或难度更新。网页个人主页的数据视图由正式站维护。

难度标题按当前字体的实际字宽决定是否使用既有简称，放大字体后也沿用同一规则。781 px 英文、200% 字体曾把 Normal 和 Lunatic 截成省略号，现两种主题均显示完整简称，提示保留完整名称。20 个同状态详情截图覆盖 320 px 英日文、390 px 中文、781 和 1440 px 英文的亮暗及双倍字体，其余 18 张截图像素一致。设置往返另检查当前详情位置。[修正前](evidence/flutter/detail-caption-before.png) | [修正后](evidence/flutter/detail-caption-after.png)

符卡区的标题与统计范围共用文字基线，宽度不足时自然换行。表头按实际字宽分配关卡、Midboss 和收取进度列，色块优先收缩，再在所属组内换行。普通字号下三种宽度均无需横滑；极窄屏的双倍字体沿用网页的表内横滑，通关表与外围页面宽度不变。列表统一测量编号和次数列，名称共用左边线，避免 `No.04` 和 `11/11` 被拆开。以同一 th07 存档覆盖 320、390、1440 px、中英日、亮暗主题和正常／双倍字体的 36 个场景，分别核对首屏、收取图、列表及返回。原实现其中 22 个场景有布局问题。[标题修正前](evidence/flutter/detail-content-heading-before.png) | [修正后](evidence/flutter/detail-content-heading-after.png) | [列表修正前](evidence/flutter/detail-content-list-before.png) | [修正后](evidence/flutter/detail-content-list-after.png)

游戏详情用账号的云存档目录确认作品及当前版本，读取该版本的通关记录、符卡和游玩时长。未登录时展示未读取的表，登录后回到同一游戏详情。游玩时间由服务返回。最后运行日期和书架的最近游玩共用本机记录库。测试通过启动成功回执和持久化校验，正式原生运行接入后由相同入口写入。云端上传、下载、恢复与本机预览见[云存档](cloud-saves.md)。

当存档确认为某机体或模式的游玩次数为零，难度记录缺少独立次数且显示未通关时，通关表显示「未尝试」，并隐藏该格的默认榜分。明确记录的通关和单难度次数优先。游戏详情和公开战绩共用这条规则。公开战绩沿用玩家的记录可见性设置。 [真实存档对照](comparison.md#真实存档通关状态)

好友区使用正式网页的固定版本和同一份 th07 好友样例核对。详情在 1440、390 和 320 px 上分别实拍。Pencil 的详情稿有可对照的主区，现有导出缺少该好友状态。 [网页与 Flutter 对照](comparison.md) | [网页读取逻辑](https://github.com/Ritosa2333/eagler-touhou/blob/85941f0d564cf10378d2af6b14d41a000bd27612/launcher/app/components/launcher/SocialSection.tsx#L140)

th06 和 th07 的符卡序号、关卡、Midboss 所属和未知名称长度从网页数据生成，原名从存档返回。收取图各行色块等宽，窄屏可在提示中读完整次数。符卡列表在窄屏提供名称和次数，宽屏另显示收率。关卡进度按已收取的符卡张数计算，色块强度按单张符卡的收率变化。网页和 Flutter 对照使用同一份 th07 云存档样例，包含过期版本、六种难度识别、不同通关状态及日文长名。Pencil 展示另一组收取次数，按布局对照。 [收取图](evidence/pencil/desktop-spell-map.png) | [收取列表](evidence/pencil/mobile-spell-list.png) | [提示](evidence/pencil/spell-tooltip.png)


th08 沿用收取图、列表、提示和色块。正作游玩与符卡练习分别统计当前机体与难度，关卡区分 4A、4B、6A 和 6B。Last Word 单列 17 张卡，使用当前机体的练习次数和收取数。该作按关卡集中显示符卡。原档的空记录显示未遇到，名字取自已遇到的记录。 [th08 存档字段](https://github.com/y-iihoshi/ThScoreFileConverter/blob/8fad2e3e95667ad9088a43a5f786ad86c1d6e8d7/ThScoreFileConverter/Models/Th08/CardAttack.cs) | [th08 分支和编号](https://github.com/y-iihoshi/ThScoreFileConverter/blob/8fad2e3e95667ad9088a43a5f786ad86c1d6e8d7/ThScoreFileConverter.Core/Models/Th08/Definitions.cs)

逐卡统计由账号 API 解析。现有 th08 云端快照在新解析器上线后可通过重新上传补齐。

## 单作设置

| 编号 | 分组 | 控件与边界 |
| --- | --- | --- |
| GAME-SET-01 | 框架 | 显示、输入、触控、音量、存档与录像、高级依次排成折叠组。全部分组共用一圈边线。分组标题行左图标右展开标志，展开后的字段标签与组标题左边缘对齐。用对应标题定位每个组，返回时保持游戏详情位置。 |
| GAME-SET-02 | 显示 | 高刷新率开关及说明、可用渲染倍率、游戏语言和语言包大小、邻近缩放。按作品提供高速连射、低速判定点、信仰槽与 th05 联机场景专用显示项。 |
| GAME-SET-03 | 输入 | 键盘、手柄设置为并列入口，窄宽时自然改为上下两行，避免标签断字。按 th075、th19 和 th09 本地双人的输入配置显示可用项目。 |
| GAME-SET-04 | 触控 | 启用触摸、始终显示判定点、放大镜与说明、按键布局与触控设置入口。映射编辑验证保存、取消、恢复与系统返回。 |
| GAME-SET-05 | 音量 | 音乐、音效、可用时的快捷发言语音。滑条右整列对齐百分比，拖动和键盘可调。 |
| GAME-SET-06 | 存档和录像 | 存档下载、上传。录像下载、管理。支持路线提示文件的作品另显示导入、删除。控件保持同高度、同行对齐及相同状态反馈。 |
| GAME-SET-07 | 高级 | 程序版本选择和帮助、可用音频的播放模式，thprac 可用性、联机本地玩家可见性、外部 MIDI 和设备选择。区分已安装资源、无可用设备和权限状态。 |
| GAME-SET-08 | 响应布局 | 选择器沿用书架的控件样式。标题和短选项同行，长说明可换行。320 px 下「中文（简体）」完整显示。存档动作根据文案宽度成对收为有完整名称的图标。 |
| GAME-SET-09 | 界面语言 | 分组、选项、存档动作和页面反馈随启动器切换简中、英语和日语。游戏语言包保留原有名称和独立的选择值。组的展开状态以固定编号保存。 |

### 输入与音量布局校准

依据固定版网页的输入入口分栏与窄宽规则，字段宽度超过 480 px 且两项名称均能完整显示时，键盘和手柄入口并排，间距 16 px；其余宽度上下排列。沿用原有 ListTile、改键弹窗和保存入口。百分比列按当前字体测量 `100%` 的宽度，数字使用等宽字形，改变音量时列宽与右边线固定。窄屏大字号下，标签独占一行，滑条与百分比在下一行。

从当前源码拍摄同一 th06、完整作品目录与游客状态，覆盖四种宽度、三语、亮暗和正常／双倍字体，共 24 个布局场景。原实现 16 个场景存在排列或百分比断行问题，修正后均通过。24 组桌面侧栏像素不变；滑条操作核对实际保存值，并检查返回详情的位置。

[桌面输入改前](evidence/flutter/settings-input-before.png) | [改后](evidence/flutter/settings-input-after.png) | [320 px 双倍字体音量改前](evidence/flutter/settings-volume-before.png) | [改后](evidence/flutter/settings-volume-after.png)。当前源码与资源校验值见[当前截图索引](evidence/current/README.md)。

### 外部 MIDI

| 编号 | 控件与状态 | 布局与行为 |
| --- | --- | --- |
| GAME-SET-07.1 | 可用性与入口 | 音乐模式来自发布清单，仅选择 MIDI 时显示外部设备开关。原生 MIDI 使用 `flutter_midi_command` 的系统设备枚举。 |
| GAME-SET-07.2 | 开关与设备 | 沿用设置字段列、开关与紧凑选择器。启用后显示读取状态，首次使用默认设备；用户选择的 `externalMidiDeviceId` 随单作配置保存。开关只作用于当前会话，重开时关闭。 |
| GAME-SET-07.3 | 设备变化 | 插拔后刷新列表，所选设备失联时保留选择并显示「未连接」。无设备及读取失败提供刷新。切出 MIDI 模式停止监听；过期读取结果不能覆盖新列表或恢复已关闭的开关。 |
| GAME-SET-07.4 | 启动边界 | 统一原生入口接收设备编号与本次启用状态。游戏执行为既定占位，音乐发送、设备连接与实际播放由运行模块接入。 |

固定版清单尚未发布 MIDI 资源，当前十一部作品按该清单显示音乐选项。回归采用共享产品目录明确支持 MIDI 的 th06，给隔离样例启用 MIDI 资源可用状态。样例验证界面与配置，未修改交付目录的资源声明。网页来源为 `SettingsBody`、`external-midi.ts` 和共享适配能力约定中的 `externalMidiDeviceId`。

| 能力样例 | 选择设备 | 设备失联 |
| --- | --- | --- |
| 1440 px 简中 | [截图](evidence/flutter/desktop-midi-selected.png) | [截图](evidence/flutter/desktop-midi-disconnected.png) |
| 390 px 日语 | [截图](evidence/flutter/mobile-midi-selected.png) | [截图](evidence/flutter/mobile-midi-disconnected.png) |
| 320 px 英语 | [截图](evidence/flutter/mobile-narrow-midi-selected.png) | [截图](evidence/flutter/mobile-narrow-midi-disconnected.png) |

控件回归核对字段左边线、标签与选择器的垂直中心、错误重试、关闭监听、选择持久化和启动入参。Windows Release 的只读探测通过同一服务枚举本机四个输出端点。[原生枚举记录](evidence/windows/midi-devices.json)保留实际名称。Android 设备枚举及音频发送待设备和运行模块验收，当前 Pencil 没有此状态的独立导出。[插件手册](https://pub.dev/packages/flutter_midi_command/versions/1.4.0)

Android 应用将 MIDI 声明为可选，覆盖插件默认的必需声明。蓝牙也作为可选能力，避免依赖权限被推导为安装分发的硬件条件。[最终 APK 检查](evidence/android/midi-package.json)记录合并结果、三个架构、签名与对齐。Windows 包含原生通知插件，恢复为[正常启动器入口](evidence/windows/midi-launcher-main.jpg)后核对首屏。[Android 功能过滤](https://developer.android.com/guide/topics/manifest/uses-feature-element) | [清单合并规则](https://developer.android.com/build/manage-manifests)

### th05 联机显示

| 编号 | 元素 | 状态 |
| --- | --- | --- |
| GAME-SET-02.1 | Boss 血条样式 | 原版侧栏、红魔乡式顶部血条和环形血条，初始为原版侧栏。 |
| GAME-SET-02.2 | 阶段提示 | 侧栏允许开启阶段标记，顶部血条允许开启阶段星数。切换样式时清除已不适用的提示。 |
| GAME-SET-02.3 | 阶段剩余时间与攻击循环 | 顶部和环形样式自动启用阶段剩余时间。侧栏由玩家开启。启用后可将攻击循环换算为时间。 |
| GAME-SET-02.4 | 梦槽与火力 | 两个数字显示开关可独立修改，随联机显示配置写入本机。 |

三语截图与房间返回见[房间内游戏设置](room.md#房间内游戏设置)。th05 简体语言包的 21,050 字节显示为 20.6 KiB，内置语言显示本地化的内置提示。

| 编号 | 键盘改键 | 验收 |
| --- | --- | --- |
| GAME-SET-03.1 | 键盘布局 | 方向键、WASD、自定义与命名布局。标准作品共用，th075、th19 各自保存。预设映射从网页键位模型生成。 |
| GAME-SET-03.2 | 录入与恢复 | 点按动作的键位后监听物理按键。Esc 取消，无法录入的键有明确提示。单行可恢复原键，重置回当前布局的基础预设。 |
| GAME-SET-03.3 | 持久化 | 命名长度最多 32 个字符，最多 32 套，重名覆盖，删除时当前键位转入自定义。启动入口读取当前布局的映射。 |
| GAME-SET-03.4 | 共用输入面板 | 两个设置入口分别选中键盘或手柄。切换时外框、标题和完成动作位置固定，当前面板从顶部开始滚动。设备和布局选择共用紧凑控件。键盘、手柄及本地双人复用同一绑定表，动作列根据文字分配宽度，键位列吸收余量，恢复列固定 48 px。保存完成后可以切换或退出，关闭后回到原入口。 |
| GAME-SET-03.5 | 手柄列表 | 选择已连接设备、切换映射开关、提示已保存设备断开。普通作品与 th19 分别保存。进入手柄面板开始监听，切换设备面板或关闭后停止监听。 |
| GAME-SET-03.6 | 手柄改键 | 松开当前按压再监听按钮或摇杆。正在触发的动作高亮，支持清除单项、恢复默认及 10% 至 90% 摇杆阈值。Esc 或系统返回先取消录入。 |
| GAME-SET-03.7 | 手柄回归 | 320 px 控件回归从游戏设置进入，模拟平台接线、按住按钮、松开、录入右摇杆向上、修改阈值及重新连接，检查启动入参。 |
| GAME-SET-03.8 | th09 本地双人 | 单机 th09 的输入弹窗提供「配置场景」。选择「本地双人」后，玩家 1 和玩家 2 分别配置键盘或手柄，切回「单人 / 联机」使用该场景的配置。 |
| GAME-SET-03.9 | 双人键位 | 两位玩家分别使用原版左区、原版右区，也可切换 WASD、方向键、命名和恢复布局。两位玩家的动作键互斥，暂停、确认、跳过和重试允许共用对应动作的按键。 |
| GAME-SET-03.10 | 双人手柄 | 手柄选择沿用设备设置，共用每只手柄的动作配置。分配互斥，退回上一级后保留当前玩家。启动入口接收两位玩家的设备、键位与启用状态。 |
| GAME-SET-03.11 | 三语 | 动作、录入状态、预设、清除、错误和键位冲突随界面语言。预设编号、物理键值和用户命名保持原值。以 320 px 英文、390 px 日文和桌面英文验证取消、改键保存、冲突与关闭。 |

窄屏回归以 th06 为输入，用 Esc 取消第一次录入，随后录入物理 J，重读命名布局并验证原生启动入口收到 `KeyJ → ArrowUp`。键盘改键的三种尺寸见[截图对照](comparison.md)。

手柄事件使用插件提供的标准按钮编号和轴方向。默认映射从网页模型生成，实际设备兼容性需在实体手柄上验收。 [Just Gamepads](https://pub.dev/packages/just_gamepads/versions/0.1.1)

本地双人使用网页模型生成的三套预设。窄屏回归验证了键位冲突后继续录入、保存玩家 2 命名布局、拒绝重复的手柄分配，以及混合设备设置传入启动入口。Flutter 的键盘录入和命名使用系统样式的子弹窗，手柄在面板内录入。 [输入弹窗焦点](https://api.flutter.dev/flutter/material/showDialog.html)

| 编号 | 触控编辑 | 验收 |
| --- | --- | --- |
| GAME-SET-04.1 | 方向和范围 | 横屏、竖屏各保存独立草稿，可恢复当前方向默认值。普通作品共用布局，th075 独立。默认位置取正式站 th06 和 th075 的实测坐标。 |
| GAME-SET-04.2 | 按键位置与大小 | 在预览中选中、拖动、调整按键大小。用按键选单和横纵滑条也可定位。碰撞项描边提示。收起设置可扩展预览。 |
| GAME-SET-04.3 | 画面位置 | 拖动或用滑条调整画面横向偏移，支持复位。预览使用原生占位画面。 |
| GAME-SET-04.4 | 即时选项 | 移动、低速、拖动灵敏度、双击 Bomb、R、thprac 和不透明度自动保存。摇杆模式禁用双指低速及拖动灵敏度，th075 显示 A/B/C/D 和摇杆选项。 |
| GAME-SET-04.5 | 保存和返回 | 保存布局后继续编辑；退出有改动的布局可继续编辑或放弃草稿。窄屏回归拖动 th06 开火键、偏移画面、保存竖屏，放弃横屏修改，再读取启动入参。 |
| GAME-SET-04.6 | 三语与可见区域 | 编辑器标题、工具提示、预览按键、滑条、即时选项、读取失败、保存和退出确认随界面语言。方向使用共用分段控件，右侧两个图标分别复位本方向和画面位置。窄屏保持操作区可达，收起设置后再展开时恢复滚动位置。 |
| GAME-SET-04.7 | 拖动与逐层返回 | 从按键起拖调整位置，从右下角起拖调整大小。窄屏其余区域可纵向滚动。Esc 先关选单，系统返回可先结束画面调整，有草稿时提示继续编辑或放弃。放弃草稿后保留即时保存的移动选项。 |
| GAME-SET-04.8 | 触控与房间规则 | 玩家席位下，房间的限速规则实时控制不限速移动选项的禁用状态。从设置切换移动方式或开启触控后更新席位。入座和启动读取已确认的配置。 [房间对照](room.md#限速房间的触控设置) |
| GAME-SET-04.9 | 灵敏度试拖 | 从预览背景起拖，准星按当前灵敏度倍增两轴位移，限制在预览边界内。松开或取消回到中央。摇杆和画面偏移模式下隐藏准星。滑条拖动可预览，确认后保存，指针取消后恢复。 |
| GAME-SET-04.10 | 横竖屏切换 | 视口转向时工作台切换当前方向，保留两份草稿。宽度足够的手机横屏将设置和预览并列，预览操作收进一行。支持方向切换的 Android 手机可从标题栏请求旋转，退出工作台后恢复系统默认方向策略。 |

### 触控工作台三语

宽屏并列显示预览与设置，窄屏纵向排列。网页工作台覆盖实际游戏画面，Flutter 预览显示可移动的原生占位画面。Pencil 现有导出包含游戏设置的入口，触控工作台缺少单独的设计稿。

| 视口 | 作品与语言 | 网页 | Flutter |
| --- | --- | --- | --- |
| 1440 × 734 | th06 简中 | [截图](evidence/web/desktop-touch-layout.png) | [截图](evidence/flutter/desktop-touch-layout.png) |
| 390 × 844 | th075 日语 | [截图](evidence/web/mobile-touch-layout-fighting-ja.png) | [截图](evidence/flutter/mobile-touch-layout-fighting-ja.png) |
| 320 × 640 | th06 英语 | [截图](evidence/web/mobile-narrow-touch-layout-en.png) | [截图](evidence/flutter/mobile-narrow-touch-layout-en.png) |
| 844 × 390 | th06 英语横屏 | [截图](evidence/web/mobile-landscape-touch-layout-landscape-en.png) | [截图](evidence/flutter/mobile-landscape-touch-layout-en.png) |

竖屏与桌面从书架进入游戏设置，核对语言、方向选择、摇杆模式、对角拖动、退出确认与返回。320 px 额外验证 296 px 短窗口中的预览与恢复滚动。th06 横竖屏回归使用 390 × 844 和 844 × 390 的本机视口，检查两份按键草稿、横屏双栏、请求失败提示以及 Android 方向请求和退出释放。设备上的实际旋转待验。 [Flutter 方向请求](https://api.flutter.dev/flutter/services/SystemChrome/setPreferredOrientations.html) | [Android 大屏规则](https://developer.android.com/about/versions/16/behavior-changes-16#adaptive-layouts)

th06 在 150% 下横拖 10 px、竖拖 25 px，准星分别移动 15 px、37.5 px。桌面保留设置，320 px 收起设置让准星可见。Flutter 回归还检查边缘限制、取消、移动方式、键盘调整、即时选项保存及按键拖动。试拖和按键编辑共用双轴识别器，另从预览外拖动可滚动设置。 [网页桌面](evidence/web/desktop-touch-sensitivity-drag.png) | [Flutter 桌面](evidence/flutter/desktop-touch-layout-sensitivity-drag.png) | [网页 320 px](evidence/web/mobile-narrow-touch-sensitivity-drag-en.png) | [Flutter 320 px](evidence/flutter/mobile-narrow-touch-layout-en-sensitivity-drag.png) | [Flutter 双轴拖动](https://api.flutter.dev/flutter/gestures/ImmediateMultiDragGestureRecognizer-class.html)

| 编号 | 存档与录像 | 验收 |
| --- | --- | --- |
| GAME-SET-06.1 | 本机存档 | 常规作品按目录中的原始存档文件名导出，上传前确认覆盖。th15 支持同时导入和导出成绩及中断存档的 ZIP。文件为空、格式不符或读取不完整时显示错误。 |
| GAME-SET-06.2 | 录像管理 | 导入单个录像或 ZIP，下载单文件或列表 ZIP。th09 可在根目录和子文件夹之间移动，支持新建目标文件夹与下载该文件夹。重命名和删除针对选中的文件。 |
| GAME-SET-06.3 | 布局与返回 | 存档和录像按钮同行对齐。弹窗显示加载、空态和重试。录像操作收进每条记录的菜单，底部导入按钮在窄屏收缩。Esc 和系统返回逐层退出。退出子目录后恢复外层滚动和目录焦点。关闭回到设置或大厅发起控件。 |
| GAME-SET-06.4 | 隔离与路线提示 | 单机和联机分目录。th10 按简体补丁和原版语言分目录，路线提示支持 TXT 导入与删除。 |
| GAME-SET-06.5 | 文件校验 | ZIP 校验路径、大小和文件内容，校验通过后再写入。导入同一个录像分配可用槽位。 |
| GAME-SET-06.6 | 三语 | 文件校验返回错误类型，由界面显示对应语言与原因。录像管理的目录、动作、空态与返回也使用界面语言。验证空文件提示后重选有效录像，显示文件菜单，再关闭回设置。 |
| GAME-SET-06.7 | 联机录像入口 | 从大厅按「观赏录像」，选定作品后打开录像管理。花映塚联机的文件选择器可以选 `.th09r`，限 1 MiB。文件临时传给原生占位，反馈显示文件名。返回管理后清理临时文件，保留所在列表。 |
| GAME-SET-06.8 | 花映塚对局资料 | 联机录像 ZIP 内的同名 `.json` 可提供双方名称、胜者、时间和时长。录像重命名、移动、删除及文件夹 ZIP 导出会一起处理有效资料。单机文件目录独立，坏资料不会阻止导入对应录像。 |
| GAME-SET-06.9 | 存档确认与文件选择 | 手动导入存档先确认覆盖，再选文件。无存档时从下载提示进入导入，确认一次即可。取消确认或选文件后，键盘焦点回到发起按钮。录像选择取消后保留列表和滚动位置。 |
| GAME-SET-06.10 | 文件按钮对齐 | 存档、录像和路线提示共用两列等宽按钮。窄屏在两列一起收成图标，保留触控区域和完整辅助标签。320 px 英文覆盖确认的两个按钮同行。 |

真实回归使用 th09 的 `score.dat`、`th9_01.rpy` 和正式站公开对局的 5796 字节录像。比较导出前后的字节，检查移动、重命名、删除和单机／联机隔离。th15 使用封装样例核对两个文件的位置与字节。联机 ZIP 中的对局资料为构造样例，`.th09r` 使用不透明字节验证入口、上限、返回和清理。实际播放由原生运行模块接入。云同步见 CLEAR-07。 [联机录像截图](#联机录像)

### th15 存档导入

| 状态 | 1440 px 网页 | 1440 px Flutter | 320 px 网页 | 320 px Flutter |
| --- | --- | --- | --- | --- |
| 存档与录像按钮 | [截图](evidence/web/desktop-th15-save-tools-en.png) | [截图](evidence/flutter/desktop-th15-save-tools-en.png) | [截图](evidence/web/mobile-narrow-th15-save-tools-en.png) | [截图](evidence/flutter/mobile-narrow-th15-save-tools-en.png) |
| 文件选择前的覆盖确认 | [截图](evidence/web/desktop-th15-confirm-save-en.png) | [截图](evidence/flutter/desktop-th15-confirm-save-en.png) | [截图](evidence/web/mobile-narrow-th15-confirm-save-en.png) | [截图](evidence/flutter/mobile-narrow-th15-confirm-save-en.png) |

从书架打开 th15 设置，验证确认取消、文件选择取消和无档导入入口。用包含 `scoreth15.dat` 和 `autosave/save0_0.dat` 的样例 ZIP 导入并导出，逐项比对两份文件的字节。样例用于验收封装。Pencil 提供设置框架，覆盖确认的单独设计稿待补。th15 ZIP 使用隔离的文件选择接口回归，Windows 共用选择器的实际验收见下文。

### Windows 存档与录像往返

Release 预览窗口缩为竖屏，从书架搜索 th09，进入详情、游戏设置与存档分组。使用真实的 `score.dat` 和 `th9_01.rpy`，经 Windows 系统选择器导入，再通过另存为对话框导出。两个输出的大小和 SHA-256 均与输入一致。录像列表 ZIP 中仅有 `replay/th9_01.rpy`，其字节也一致。[文件核对](evidence/windows/file-roundtrip.json)

| 实际操作 | Windows 画面 |
| --- | --- |
| 存档导入后回到设置 | [截图](evidence/windows/save-imported.jpg) |
| 存档导出后仍在原分组 | [截图](evidence/windows/save-exported.jpg) |
| 录像导入后显示名称和大小 | [截图](evidence/windows/replay-imported.jpg) |
| 单个录像导出后回到列表 | [截图](evidence/windows/replay-exported.jpg) |
| 键盘取消后回到导入按钮 | [截图](evidence/windows/replay-keyboard-cancel.jpg) |
| 随后按空格重新打开系统选择器 | [截图](evidence/windows/replay-keyboard-reopen.jpg) |
| 录像 ZIP 导出后回到设置 | [截图](evidence/windows/replay-zip-exported.jpg) |

Esc 从录像管理回到设置，设置的分组与滚动位置一致。此次使用此前为空的本机游戏文件目录，验收后仅清理本次导入的两份样例。Android 系统选择器交互待设备验收。

### 联机录像

| 情景 | 1440 px 中文 | 390 px 日文 | 320 px 英文 |
| --- | --- | --- | --- |
| 导入 `.th09r` 后的原生占位 | [桌面](evidence/flutter/desktop-lobby-match-replay.png) | [手机](evidence/flutter/mobile-lobby-match-replay.png) | [窄屏](evidence/flutter/mobile-narrow-lobby-match-replay.png) |
| 联机 ZIP 中的对局资料 | [桌面](evidence/flutter/desktop-lobby-replay-metadata.png) | [手机](evidence/flutter/mobile-lobby-replay-metadata.png) | [窄屏](evidence/flutter/mobile-narrow-lobby-replay-metadata.png) |

网页对照入口在大厅录像管理与[个人主页逐局录像](th09-ratings.md)。本地联机 ZIP 和原生启动反馈缺少同状态 Pencil 导出。

桌面从大厅用 Enter 打开录像管理，在滚动后的目录进入子目录。按 Esc 返回原位置，再按 Esc 回到大厅录像按钮。手机在 320 px 和 390 px 用系统返回验证同一层级。 [弹窗键盘行为](https://www.w3.org/WAI/ARIA/apg/patterns/dialog-modal/) | [层级返回](https://developer.chrome.com/docs/modern-web-guidance/guides/ui-components/stack-drill-down)

来源路径为网页 `BookGameDetail`、`MobileGameDetail`、`ClearMatrix`、`SpellCardArchive`、`SettingsBody`、`ReplayDialog` 与 `replay-storage-policy.mts`。 [ZIP 流式处理](https://pub.dev/packages/archive) | [原生文件导出](https://pub.dev/packages/file_saver)



## 游戏资源导入

| 编号 | 状态 | 约定 |
| --- | --- | --- |
| GAME-06.1 | 入口与关闭 | 桌面在动作行显示文字，320 px 详情将“导入资源”和“游戏设置”收成有可访问名称的图标。选择器取消后回到导入窗，键盘焦点回到「选择文件」，可按空格再次打开。简中、英语、日语的两个动作等宽同行，关闭控件在标题右侧。 |
| GAME-06.2 | 本地 DATA | 游戏目录记录发布清单的文件名、大小与 SHA-256。选择文件后显示复制进度并校验，完成后保存到本应用支持目录。启动入口可读取已校验的数据路径。 |
| GAME-06.3 | 加载失败 | 作品、大小和校验失败对应独立提示。损坏文件留在原输入位置，已有有效安装可继续使用。 |
| GAME-06.4 | 发布版 ZIP | 识别现代 `package.json` 和兼容旧版 `manifest.json`，按声明读取当前作品的 DATA。大小及 SHA-256 必须符合内置发布清单。其他压缩条目不进入安装目录。 |
| GAME-06.5 | 备用下载地址 | 从站点清单读取 HTTPS 链接与提取提示。读取失败时在原窗重试，清单没有地址时显示空态。链接打开系统浏览器，下载完成后可回到本地导入。桌面 Esc 和手机系统返回先退到导入窗，再回详情。键盘焦点回到相应入口。 |
| GAME-06.6 | 原作与附加资源 | 原作散文件、RAR、7z、加密 ZIP、附加字体、音乐和语言包继续按各作能力接入。 |

所有作品的资源导入共用校验链路。SHA-256 与 ZIP 解压在工作 isolate 内执行，通过文件路径读取数据，界面接收阶段与结果。th07 的真实 DATA、以其构造的现代与旧版 ZIP、损坏替换及三种宽度的导入返回已回归，组件开销见[资源校验测量](../archive/performance-audit-20261128/report.md#原作资源校验)。

回归从 th06 详情提交真实发布 ZIP，核对启动占位收到的 8,064,942 字节 DATA 及其 SHA-256。桌面通过同名、同大小损坏样本验证替换失败后仍读取原安装。另以旧版 ZIP、其他作品声明和无效路径核对提示。Windows Release 从详情打开[系统选择器](evidence/windows/resources-select.png)，取消后回到原导入窗，再次选择真实 DATA 并成功安装。[安装文件核对](evidence/windows/resource-import.json)的大小与 SHA-256 和内置发布清单一致。三种宽度的控件回归覆盖取消、空格重新打开与校验失败后的焦点恢复。更新包已从真实系统选择器验证[取消返回](evidence/windows/resources-cancel.png)与[按空格重新打开](evidence/windows/resources-keyboard-reopen.png)。 [截图对照](comparison.md#游戏资源导入三语)

320 px 英文、390 px 日文和 1440 px 中文从详情进入备用下载窗，经连接失败、重试、显示已发布清单的地址及返回。桌面另验清单未配置地址时显示空态。 [窄屏英文](evidence/flutter/mobile-narrow-game-package-reference-retry.png) | [手机日文](evidence/flutter/mobile-game-package-reference-retry.png) | [桌面中文](evidence/flutter/desktop-game-package-reference-retry.png)

游戏选项按作品和单机／联机范围保存。显示设置以目录能力决定可见项目，低速判定点与高刷新率使用清单和共用作品目录，语言和音乐选项取已发布资源。Flutter 桌面默认收起显示设置，手机默认展开。外部 MIDI 和其他资源版本按上方规格继续验收。

[Flutter 文件选择](https://pub.dev/packages/file_picker) | [Flutter 本地支持目录](https://pub.dev/packages/path_provider)
