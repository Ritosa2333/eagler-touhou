# 联机房间

房间从联机大厅的建房、房号或房间行进入。房间状态由站点发布清单指定的 Relay 提供。大厅保留自己的滚动位置与筛选。房间资料、席位和观战者使用 Flutter 控件。

| 编号 | 状态和元素 | 布局与验收 |
| --- | --- | --- |
| ROOM-01 | 创建与进入 | 建房表单随作品展示人数、难度、隐私和移动规则，th09 另有休闲对战。成功建房取得 P1。输入四至八位房号时，Relay 可把请求转到房间所属作品。已有房间行直接进入；满员或游戏中的房间进入旁观席。 |
| ROOM-02 | 工具栏与分享 | 顶部依次提供返回、房间号、邀请链接、游戏设置和有权限时的房间设置。点击房间号可复制。空间紧张时，按钮优先显示完整号码并提供本地化的读屏名称。共享链接包含作品和房号，也可从大厅输入框或应用启动打开。 [邀请验收](lobby.md#邀请直达) |
| ROOM-03 | 作品和状态 | 封面与房间信息同一行，使用两端的稳定宽度。连接、等候、重连、离线和被移出使用同一状态区。重连保留上次席位状态和本机房间身份。 |
| ROOM-04 | 玩家席 | 按人数生成等宽席位。空席可入座，已入座可站起。显示头像、名字、在线、准备和移动状态。动作菜单固定在所在席位右上角。 |
| ROOM-05 | 房主权限与接任 | P1 在等待阶段可修改房间选项、将其他玩家移到旁观席或确认移出玩家及旁观者。原房主主动离席后 P1 空出，其他玩家点「成为房主」入座，原席位自动释放。设置和管理入口跟随 Relay 确认的 P1 身份。 |
| ROOM-06 | 旁观 | 旁观席独立列出人数和成员。选中加入旁观时由 Relay 分配角色。收到可观战的开局通知后提供原生入口；等待时可退出旁观。 |
| ROOM-07 | 游戏入口 | 已入座的玩家以及获准观战的成员可调用原生启动占位。占位结果出现在本页。原生运行可用前，客户端不向 Relay 宣告准备完成，也不发起对局。 |
| ROOM-08 | 返回和离开 | 顶部和系统返回都主动向 Relay 关闭房间会话并释放席位，然后返回大厅原位置。普通网络中断保留同一会话身份以恢复角色。其他窗口占用本机房间时，可从[联机大厅](lobby.md#退出旧房间)退出。 |
| ROOM-09 | 布局 | 桌面内容居中并限定宽度。手机按两席或三席等宽排列。370 px 及以下，入座后的旁观动作采用有名称的图标。根据文字所需宽度决定是否把离席收为图标，底部三项操作在 320 px 保持同行。文字与图标按钮按按钮中心对齐。 |
| ROOM-10 | 聊天和快捷发言 | 桌面聊天在席位区右侧，手机在旁观席下面。日志独立滚动，读旧消息时保留位置并提供返回最新消息的按钮。可看最近 50 条。入座或旁观后可选择 Relay 允许的快捷短语，文字输入需要账号服务确认发言资格。 |
| ROOM-11 | 发送与举报 | 文字等待回执，失败时保留草稿。允许隐藏该成员在本房间的信息，并举报其他账号发送的文字。举报理由可留空。重连会重读当前账号资格及房间记录。 |
| ROOM-12 | 界面语言 | 大厅、席位、房间状态、快捷发言与报告提示提供简中、英语和日语。快捷短语沿用 Relay 编号，在每位成员的界面显示其语言。邀请链接带当前界面语言。 |
| ROOM-13 | 房间规则 | 等待室的表单随作品展示挑战模式、Miss 是否清屏、th05 初始残机与 Bomb、th09 蓄力优先、th15 奖残阈值等 Relay 规则。只有支持的协作作品展示挑战模式，Miss 不清屏随挑战模式出现。切换可在保存前预览，取消不提交。人数、难度与数值规则统一用启动器的紧凑选择器。 |
| ROOM-14 | 联机时序 | 依据共用作品目录显示回滚和输入延迟。P1 在等待阶段保存，成员可在房间摘要查看 Relay 确认的值。回滚启用时折叠手动延迟，关闭后恢复暂存帧数。支持开局实测的作品提供独立开关，手动调整范围取作品允许的上限。窄屏滚动表单时保留标题和操作。 |
| ROOM-15 | 常用发言与语音 | 快捷发言面板可切换常用和全部。九个常用槽位可逐项修改，面板内支持数字键。槽位和语音音量写入启动器偏好，音量在游戏设置的音量组统一调整。本人发送时播放一次，收到其他成员发言时按屏蔽状态和席位决定是否播放。离房释放播放器。 |
| ROOM-16 | 花映塚评分 | 公开席位分数与局数保持独立的一行。玩家和旁观者的头像打开公开评分卡。桌面悬停或点击，手机打开底部弹层，关闭后保持房间连接和席位。 [评分卡对照](th09-ratings.md#等待室与排行榜评分卡) |
| ROOM-17 | 游戏设置 | 等待阶段可从齿轮打开。玩家与旁观者共用单作设置的显示、输入、触控、音量、文件和高级组。联机显示、语言和音量等参数从这部作品的联机偏好读取。关闭后保留席位、房间连接和视口位置。 |
| ROOM-18 | 限速房间 | 玩家已有不限速移动偏好时，入座和启动前提供不限向移动、八向移动和取消。确认后保存所选方式，席位广播和启动使用同一份已确认偏好。房间修改规则时，已在席位的玩家可从提示或触控设置调整。旁观者的本机选项由后续入座动作校验。 |

## 验收

- 使用当前提交的真实 Relay 在本机验证创建、跨作品房号解析、入座、旁观、房主移席与移出、房间公开和私有切换、主动离房释放。每次测试起独立回环端口。
- 320 px 控件回归覆盖从大厅输入房号、掉线重试、使用相同身份重新连接、修改房间设置、调用原生占位和系统返回。三个宽度使用同一双人、单旁观者快照对照。网页等待室仍展示 Runtime 资源准备区。现有 Pencil 导出未包含房间。 [截图对照](comparison.md)
- 本机真实 Relay 与隔离账号桥验证玩家和旁观者发送快捷短语、限制游客文字发言、账号票据、游客接收文字和举报回执。控件测试覆盖打开短语选择、发言回显与举报确认。
- 本机 Relay 验证原 P1 主动离房后 P2 原位等待、接任后空出 P2 并获得设置权限。th06 挑战和 Miss 规则、th05 初始残机与 Bomb、th09 蓄力优先、th15 奖残阈值均以 Relay 回读为准。320 px 三语回归覆盖取消表单、保存规则、旁观后接任和系统返回。
- th08 在本机 Relay 验证回滚、9 帧手动值和自动实测的配置同步。游客端收到状态，越界值和非房主提交被客户端拦截。三种宽度从大厅进入并验证表单调整、取消、保存和返回。
- 本机 Relay 验证快捷发言已发送、本人回显避免重复播放、旁观发言保留文字，以及语音触发规则。320 px 从房间入口修改第一个槽位，读取持久化值，系统返回常用列表并发出更换后的短语。
- 游戏设置保存快捷语音音量后，重新读取启动器偏好验证数值。Windows 用同包中的「等一下」和「谢谢指教」静音验证解码和播放完成事件，时长分别为 840 ms 和 949 ms。32 个发布音频与网页资源逐字节一致。
- 正式站进房与账号写入、主界面 Alt 触发、评级事件、开局实测及原生对局或画面观战继续验收。

## 常用发言

同一 th06 房间以默认九条常用短语打开选择面板。网页在发言入口旁打开选单，Flutter 使用原生底部浮层。两个客户端按同一协议编号使用仓库发布的声音。现有 Pencil 导出没有这个状态。

| 视口 | 网页 | Flutter |
| --- | --- | --- |
| 320 × 640 | [截图](evidence/web/mobile-narrow-room-quick-picker.png) | [截图](evidence/flutter/mobile-narrow-room-quick-picker.png) |
| 390 × 844 | [截图](evidence/web/mobile-room-quick-picker.png) | [截图](evidence/flutter/mobile-room-quick-picker.png) |
| 1440 × 734 | [截图](evidence/web/desktop-room-quick-picker.png) | [截图](evidence/flutter/desktop-room-quick-picker.png) |

## 联机时序

网页和 Flutter 使用同一份 th08 双人等待室数据。回滚关闭，输入延迟设为 2 帧。自动实测是房间开局参数，实测由未来的原生游戏接入完成。现有 Pencil 导出没有这个状态。

| 视口 | 网页 | Flutter |
| --- | --- | --- |
| 320 × 640 | [截图](evidence/web/mobile-narrow-room-timing.png) | [截图](evidence/flutter/mobile-narrow-room-timing.png) |
| 390 × 844 | [截图](evidence/web/mobile-room-timing.png) | [截图](evidence/flutter/mobile-room-timing.png) |
| 1440 × 734 | [截图](evidence/web/desktop-room-timing.png) | [截图](evidence/flutter/desktop-room-timing.png) |

## 房间规则表单

使用同一份 th06 双人等待室样例。网页在房间侧栏切到设置，Flutter 从房间顶栏打开。320 px 与 390 px 竖屏表单距两边 16 px，字段左边距为 40 px，右侧控件按同一条边界排列。现有 Pencil 导出没有这个状态。

| 视口 | 网页 | Flutter |
| --- | --- | --- |
| 320 × 640 | [简中](evidence/web/mobile-narrow-room-rules.png) | [简中](evidence/flutter/mobile-narrow-room-rules.png) / [英语](evidence/flutter/mobile-narrow-room-rules-en.png) / [日语](evidence/flutter/mobile-narrow-room-rules-ja.png) |
| 390 × 844 | [简中](evidence/web/mobile-room-rules.png) | [简中](evidence/flutter/mobile-room-rules.png) |
| 1440 × 734 | [简中](evidence/web/desktop-room-rules.png) | [简中](evidence/flutter/desktop-room-rules.png) |

## 房间内游戏设置

在同一个 th05 双人等待室样例点击齿轮，展开显示组。Flutter 桌面设置面板宽 440 px、靠右边距 16 px，手机采用全屏。已验证 Esc、系统返回和顶部返回。现有 Pencil 导出缺少这个状态。 [Flutter 弹窗](https://api.flutter.dev/flutter/material/showDialog.html)

| 视口 | 语言 | 网页 | Flutter |
| --- | --- | --- | --- |
| 320 × 640 | 英语 | [截图](evidence/web/mobile-narrow-room-game-display-th05-en.png) | [截图](evidence/flutter/mobile-narrow-room-game-display-th05.png) |
| 390 × 844 | 日语 | [截图](evidence/web/mobile-room-game-display-th05-ja.png) | [截图](evidence/flutter/mobile-room-game-display-th05.png) |
| 1440 × 734 | 简中 | [截图](evidence/web/desktop-room-game-display-th05.png) | [截图](evidence/flutter/desktop-room-game-display-th05.png) |

## 限速房间的触控设置

th06 双人房关闭不限速移动，测试账号入房前已保存不限速偏好。弹窗使用站点现有三语文案。取消保留原偏好和房间，确认继续当前入座或启动动作。Pencil 导出缺少该弹窗。

| 视口 | 语言 | 网页 | Flutter |
| --- | --- | --- | --- |
| 320 × 640 | 英语 | [截图](evidence/web/mobile-narrow-room-touch-policy-en.png) | [截图](evidence/flutter/mobile-narrow-room-touch-policy-en.png) |
| 390 × 844 | 日语 | [截图](evidence/web/mobile-room-touch-policy-ja.png) | [截图](evidence/flutter/mobile-room-touch-policy-ja.png) |
| 1440 × 734 | 简中 | [截图](evidence/web/desktop-room-touch-policy.png) | [截图](evidence/flutter/desktop-room-touch-policy.png) |

控件测试从大厅输入房号，验证取消时保留偏好、两种切换动作、启动入参，以及限制变更时编辑器的禁用状态。320 px 在成为旁观者后重试入座，核对取消和确认分别产生的席位结果。本机真实 Relay 回归从 th07 房号入口转到 th06，验证取消入座、允许入座及对手席位上的移动方式更新。

三种宽度均从大厅输入房号进入，选择原版、环形和红魔乡式血条，切换阶段、计时与数字选项。再次打开原设置面板时读回已保存的值。启动占位收到完整的联机显示配置。原连接只建立一次，Relay 未收到修改房间规则的请求。切到旁观席后仍可打开游戏设置。[控件依赖关系](game.md#th05-联机显示)

## 视觉对齐

房间使用 Prism UI 的标题栏、色阶表面、分组工具和底部按钮。房间设置与举报弹层继续使用现有表单控件。重点参考 FlClash 用表面深浅组织层级、把同用途的小工具收进一个组的做法。颜色采用东方 Online 已有的中性色和浅粉强调色。 [FlClash 表面与导航](https://github.com/chen08209/FlClash/blob/main/.agents/surfaces.md) | [Prism UI 组件](https://github.com/chen08209/prism_ui)

以下坐标取实际 Flutter 渲染，逻辑像素与截图像素为 1:1。表格中的坐标以画面左上角为原点。

| 细节 | 320 px 手机 | 390 px 手机 | 1440 px 桌面 | 视觉作用 |
| --- | --- | --- | --- | --- |
| 内容外边 | x = 16–304 | x = 16–374 | x = 200–1240 | 旁观席、封面和玩家席共用相同左界。桌面居中限制在 1040 px，右侧不会被无限拉宽。 |
| 封面与文字 | 封面 56 × 56，文字从 x = 84 开始 | 同左 | 封面 56 × 56，文字从 x = 268 开始 | 房间号、作品名、副标题、规则和连接状态共用一条文字轴。封面与这条轴之间有 12 px 间隔。 |
| 玩家席分组 | 宽 288 px | 宽 358 px | 宽 694 px | 只有一块圆角表面。席位等宽，中间用 1 px 分隔线标记位置，两侧头像按各自的列居中。 |
| 旁观与聊天 | 玩家、旁观、聊天依次以 16 px 间隔叠放 | 同左 | 聊天从 x = 910 开始，宽 330 px，与玩家席上沿同为 y = 180 | 手机逐块往下读。桌面两列起点齐平，旁观仍跟随玩家席。 |
| 底部动作 | 启动按钮命中区 y = 586–634，三项同高 | 启动按钮命中区 y = 790–838，三项同高 | 启动按钮右沿 x = 1240，与聊天外框齐平 | 开始游戏用实色强调，离开席位和旁观用低权重动作。宽度 370 px 及以下，旁观用带名称的图标。 |
| 深色表面 | 页面 `#17181B`，内容 `#212227`，底部 `#1B1C20` | 同左 | 同左 | 深浅先形成区块，边框集中在席位分隔和需提示交互的处所。 |

把等待提示并入作品信息后，在英文窄屏样例的 x = 30 取色，玩家区内填色从 y = 242 提前到 y = 197。桌面取 x = 218，从 y = 226 提前到 y = 181。这 45 px 让玩家信息更靠近标题，同时给聊天留出更多可见空间。

| 视口 | 改前 | Prism UI 深色 | Prism UI 亮色 | 坐标 |
| --- | --- | --- | --- | --- |
| 320 × 640 | [改前](evidence/flutter/mobile-narrow-room-before-prism.png) | [改后](evidence/flutter/mobile-narrow-room-waiting-en.png) | — | [测量](evidence/flutter/mobile-narrow-room-waiting-en.geometry.json) |
| 390 × 844 | [改前](evidence/flutter/mobile-room-before-prism.png) | [改后](evidence/flutter/mobile-room-waiting-en.png) | [亮色](evidence/flutter/mobile-room-waiting-en-light.png) | [测量](evidence/flutter/mobile-room-waiting-en.geometry.json) |
| 1440 × 734 | [改前](evidence/flutter/desktop-room-before-prism.png) | [改后](evidence/flutter/desktop-room-waiting-en.png) | [亮色](evidence/flutter/desktop-room-waiting-en-light.png) | [测量](evidence/flutter/desktop-room-waiting-en.geometry.json) |

## 英日文截图

同一双人房间及公开目录样例。发言正文是发送者原文。英文和日文视图实测从大厅输入房号，读取发言以及系统返回。窄屏另测掉线重试、链接语言、快捷短语回显、举报和设置提交。

### 英语

| 状态 | 桌面 1440 px | 手机 390 px | 手机 320 px |
| --- | --- | --- | --- |
| 大厅房间列表 | [网页](evidence/web/desktop-lobby-live-en.png) / [Flutter](evidence/flutter/desktop-lobby-live-en.png) | [网页](evidence/web/mobile-lobby-live-en.png) / [Flutter](evidence/flutter/mobile-lobby-live-en.png) | [网页](evidence/web/mobile-narrow-lobby-live-en.png) / [Flutter](evidence/flutter/mobile-narrow-lobby-live-en.png) |
| 大厅断连 | [网页](evidence/web/desktop-lobby-disconnected-en.png) / [Flutter](evidence/flutter/desktop-lobby-disconnected-en.png) | [网页](evidence/web/mobile-lobby-disconnected-en.png) / [Flutter](evidence/flutter/mobile-lobby-disconnected-en.png) | [网页](evidence/web/mobile-narrow-lobby-disconnected-en.png) / [Flutter](evidence/flutter/mobile-narrow-lobby-disconnected-en.png) |
| 等待室 | [网页](evidence/web/desktop-room-waiting-en.png) / [Flutter](evidence/flutter/desktop-room-waiting-en.png) | [网页](evidence/web/mobile-room-waiting-en.png) / [Flutter](evidence/flutter/mobile-room-waiting-en.png) | [网页](evidence/web/mobile-narrow-room-waiting-en.png) / [Flutter](evidence/flutter/mobile-narrow-room-waiting-en.png) |
| 聊天记录 | [网页](evidence/web/desktop-room-chat-en.png) / [Flutter](evidence/flutter/desktop-room-chat-en.png) | [网页](evidence/web/mobile-room-chat-en.png) / [Flutter](evidence/flutter/mobile-room-chat-en.png) | [网页](evidence/web/mobile-narrow-room-chat-en.png) / [Flutter](evidence/flutter/mobile-narrow-room-chat-en.png) |

### 日语

| 状态 | 桌面 1440 px | 手机 390 px | 手机 320 px |
| --- | --- | --- | --- |
| 大厅房间列表 | [网页](evidence/web/desktop-lobby-live-ja.png) / [Flutter](evidence/flutter/desktop-lobby-live-ja.png) | [网页](evidence/web/mobile-lobby-live-ja.png) / [Flutter](evidence/flutter/mobile-lobby-live-ja.png) | [网页](evidence/web/mobile-narrow-lobby-live-ja.png) / [Flutter](evidence/flutter/mobile-narrow-lobby-live-ja.png) |
| 大厅断连 | [网页](evidence/web/desktop-lobby-disconnected-ja.png) / [Flutter](evidence/flutter/desktop-lobby-disconnected-ja.png) | [网页](evidence/web/mobile-lobby-disconnected-ja.png) / [Flutter](evidence/flutter/mobile-lobby-disconnected-ja.png) | [网页](evidence/web/mobile-narrow-lobby-disconnected-ja.png) / [Flutter](evidence/flutter/mobile-narrow-lobby-disconnected-ja.png) |
| 等待室 | [网页](evidence/web/desktop-room-waiting-ja.png) / [Flutter](evidence/flutter/desktop-room-waiting-ja.png) | [网页](evidence/web/mobile-room-waiting-ja.png) / [Flutter](evidence/flutter/mobile-room-waiting-ja.png) | [网页](evidence/web/mobile-narrow-room-waiting-ja.png) / [Flutter](evidence/flutter/mobile-narrow-room-waiting-ja.png) |
| 聊天记录 | [网页](evidence/web/desktop-room-chat-ja.png) / [Flutter](evidence/flutter/desktop-room-chat-ja.png) | [网页](evidence/web/mobile-room-chat-ja.png) / [Flutter](evidence/flutter/mobile-room-chat-ja.png) | [网页](evidence/web/mobile-narrow-room-chat-ja.png) / [Flutter](evidence/flutter/mobile-narrow-room-chat-ja.png) |
