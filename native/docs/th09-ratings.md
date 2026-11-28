# 花映塚评分与对局

当前原生维护大厅和等待室的评分卡。个人主页及其评分、战绩与录像使用正式站应用内网页，大厅的「排行榜」在系统浏览器打开网页总榜。旧 Flutter 个人页和独立榜单已归档。共用评分数据、机体名单与立绘继续随大厅和等待室使用。

WHR-06、07、10 至 13 及下方独立榜单截图为归档前的历史登记，不再作为原生验收要求。个人主页及其对局录像现统一使用网页并停止原生维护。大厅及等待室评分卡随联机配套维护。

| 编号 | 状态与控件 | 布局与验收 |
| --- | --- | --- |
| WHR-01 | 个人主页入口与权限 | 花映塚视图可用时显示在游戏数据切换器。打开机体榜里的玩家入口时携带机体编号，公开配置允许且该机体有历史才显示对照。匿名访问和本人访问按现有账号接口传递会话。 |
| WHR-02 | WHR 与名次 | 概览平铺，数值与模式切换同行，日期在曲线下方。图高按内容宽度的 18% 在 64 至 108 px 间调整，省略纵轴刻度和网格。名次越小越靠上。空数据省略图表，预览得分可单独显示。折线触摸和鼠标可选日期，前后按钮也可用键盘逐点读取。 |
| WHR-03 | 机体分对照 | 从机体选择器或该行评分进入。同一日期轴绘制个人 WHR 和机体分。桌面更换原卡片内容，手机打开底部弹层，系统返回与可见返回按钮均关闭弹层。 |
| WHR-04 | 战绩与对手 | 胜场、局数、胜率、时长由服务端提供。机体按局数排，默认展开前 3 项。对局按日期分组，默认 2 项，支持展开与游标翻页。对手可公开访问时进入其个人主页。 |
| WHR-05 | 逐局录像 | 有录像的对局可展开。列表显示局号、双方机体、时长、播放和下载。接口出错可重试；切换账号身份后重新读取录像列表。下载上限 1 MiB。原生文件选择器接收服务器返回的文件，播放经原生游戏入口并显示占位反馈。 |
| WHR-06 | 榜单入口与工具 | 从联机大厅推入排行榜。从榜单返回恢复大厅。可搜索昵称和账号，按定级、交手组和局数过滤，切换排序方向及清除筛选。机体榜同分按计分局数、用户名排序。 |
| WHR-07 | 榜单行、分布和反馈 | 桌面直接显示名次、玩家、评分、偏差、局数、胜率、交手组、最近对局和定级，机体榜另列总分。窄屏可展开玩家行看详细数据。分布按评分区间统计并以颜色标记搜索命中。加载、空榜、无搜索结果和请求失败分别显示。 |
| WHR-08 | 大厅评分卡 | 向账号服务批量读取大厅当前花映塚玩家的 WHR 和公开机体分。席位只显示服务给出的分数，缺分显示横线，可同时显示公开局数。桌面悬停或点击头像打开卡片，手机点击打开底部弹层。卡内链接打开正式站应用内个人主页。 |
| WHR-09 | 等待室评分卡 | 花映塚房间的席位和旁观列表与大厅共用批量评分刷新及评分卡。关闭卡片或从资料页返回时保留房间。 |
| WHR-10 | 排行榜头像和刷新 | 榜单使用公开头像缩略图和榜单已提供的机体分。头像打开评分卡。机体榜的个人主页入口携带当前机体。前台每 12 秒刷新，短暂读失败时保留原榜单。 |
| WHR-11 | 在全榜定位 | 搜索命中后可定位排名最高的匹配玩家。保留当前筛选和排序，显示目标及相邻名次，并将键盘焦点移到目标行。 |
| WHR-12 | 三语榜单与筛选 | 榜单、机体选择、排名状态、排序、图表、错误与空态使用当前界面语言。总榜筛选包含交手组，机体榜省略交手组。 |
| WHR-13 | 桌面列与断点 | 表头和数据共享列宽，点击表头换排序，切换宽度后沿用榜单状态和可见位置。 |
| WHR-14 | 个人主页三语与重试 | 个人主页标题、全作切换、评分图、机体名、对局状态与录像沿用界面语言。对局首次重试保持简要视图。录像列表失败后原位重试。 |

## 验收

概览按网页的紧凑曲线恢复，机体战绩使用平铺条目与选中底色；机体对照保留边框和返回动作。曲线防止插值越过实际值，选中点提供标记。标题供读屏读取，逐点日期和数值仍可通过键盘操作。320、390 和 1440 px 的公开资料入口均走过评分、名次、机体对照、录像和榜单。 [个人主页整改对照](../archive/web-only/docs/profile.md#本轮验收)

- 以正式站公开的 @ritosa 为输入。读取个人评分、名次、机体历史、对局、公开逐局录像列表和二进制文件，并从大厅读取公开总榜。文件经启动器现有暂存、导出和原生启动服务复验。 [网页与 Flutter 对照](comparison.md)
- 机体对照使用 Flutter 弹层保持手机个人主页的滚动位置。排行榜以可展开条目承载窄屏细节。
- 正式账号私密录像以及 Android 触摸和系统导出选择器继续验收。真实游戏执行依照迁移范围由原生入口占位。

## 大厅评分卡

同一份三房间样例含两名定级玩家及一名未定级玩家。#4201 的席位显示 1,438 和 26 局。打开头像可看公开总分、灵梦、咲夜、铃仙的机体分。公开设置隐藏的卡片字段不展示。

登录参考样例后，同交手组单人招募房按评分差由近至远排列，保留其他房间的位置。前台每 12 秒刷新，离开大厅栏目后停止查询。三种宽度回归覆盖头像入口、返回和列表位置，390 px 额外验证从卡片进入个人主页并返回。

用 Flutter 的账户客户端从正式站读取 239 条公开评分和 3 条公开机体分。网页与 Flutter 截图使用同一批隔离样例。网页采用锚定浮层，Flutter 的手机原生弹层覆盖底部导航。Pencil 当前导出未覆盖该状态。

| 视口 | 网页 | Flutter |
| --- | --- | --- |
| 320 × 640 | [截图](evidence/web/mobile-narrow-lobby-rating-card.png) | [截图](evidence/flutter/mobile-narrow-lobby-rating-card.png) |
| 390 × 844 | [截图](evidence/web/mobile-lobby-rating-card.png) | [截图](evidence/flutter/mobile-lobby-rating-card.png) |
| 1440 × 734 | [截图](evidence/web/desktop-lobby-rating-card.png) | [截图](evidence/flutter/desktop-lobby-rating-card.png) |

## 等待室与排行榜评分卡

网页与 Flutter 使用同一公开评分样例。等待室为房间 #4157，榜单为两名已定级玩家。灵梦评分卡显示总分及相同的三条公开机体分。两处复用头像、机体立绘、资料入口和公开字段检查。

验证三种宽度下关闭卡片回到原位置。等待室使用回环 WebSocket 验证打开、关闭卡片时仍在 P1。390 px 验证进入个人主页后返回原席位，并查看未定级旁观者的名片。榜单验证自动刷新可替换评分，失败保留原列表。从机体榜打开个人主页时保留机体编号，资料页在前台时暂停榜单轮询。

正式站榜单由 Flutter 账户客户端读取。读取了 239 条记录，并验证其中 177 个公开头像地址可由原账户站点解析。Pencil 当前导出未覆盖两个评分卡状态。

| 视口 | 等待室 | 排行榜 |
| --- | --- | --- |
| 320 × 640 | [网页](evidence/web/mobile-narrow-room-rating-card.png)、[Flutter](evidence/flutter/mobile-narrow-room-rating-card.png) | [网页](evidence/web/mobile-narrow-board-rating-card.png)、[Flutter](evidence/flutter/mobile-narrow-board-rating-card.png) |
| 390 × 844 | [网页](evidence/web/mobile-room-rating-card.png)、[Flutter](evidence/flutter/mobile-room-rating-card.png) | [网页](evidence/web/mobile-board-rating-card.png)、[Flutter](evidence/flutter/mobile-board-rating-card.png) |
| 1440 × 734 | [网页](evidence/web/desktop-room-rating-card.png)、[Flutter](evidence/flutter/desktop-room-rating-card.png) | [网页](evidence/web/desktop-board-rating-card.png)、[Flutter](evidence/flutter/desktop-board-rating-card.png) |

## 在全榜定位

两端注入同一份 65 人榜单。搜索 `@rated` 后点击「在全榜定位」，第 65 名进入视口并获得键盘焦点，第 64 名可见。Flutter 回归还覆盖无结果时禁用入口、定时刷新更新分数时保留视口和焦点，以及清除搜索恢复榜单。

| 视口 | 网页 | Flutter |
| --- | --- | --- |
| 320 × 640 | [截图](evidence/web/mobile-narrow-board-locate.png) | [截图](evidence/flutter/mobile-narrow-board-locate.png) |
| 1440 × 734 | [截图](evidence/web/desktop-board-locate.png) | [截图](evidence/flutter/desktop-board-locate.png) |

## 榜单筛选与三语

桌面把榜单、搜索和筛选放在表格右上方，从表头排序。手机把榜单与筛选放在第一行，搜索与排序放在第二行，配对控件均为 44 px 高。点问号打开评分说明。角色名称随界面语言切换，玩家昵称显示账号原值。

网页从筛选按钮打开锚定浮层。Flutter 使用可滚动的原生对话框，筛选值即时更新，点关闭、Esc 或系统返回可回到榜单。三种视口从联机大厅进入，用同一份两人榜单验证加载失败重试、最少局数、无匹配后清除筛选、倒序切换、机体榜和分布图。Pencil 当前导出缺少排行榜筛选状态。 [榜单词库](https://github.com/Ritosa2333/eagler-touhou/blob/85941f0d564cf10378d2af6b14d41a000bd27612/launcher/messages/en.json#L2392) | [Flutter 排序菜单](https://api.flutter.dev/flutter/material/MenuAnchor-class.html)

| 语言与视口 | 网页 | Flutter |
| --- | --- | --- |
| 简中 1440 × 734 | [截图](evidence/web/desktop-board-filters-zh-CN.png) | [截图](evidence/flutter/desktop-board-filters-zh-CN.png) |
| 英语 320 × 640 | [截图](evidence/web/mobile-narrow-board-filters-en.png) | [截图](evidence/flutter/mobile-narrow-board-filters-en.png) |
| 日语 390 × 844 | [截图](evidence/web/mobile-board-filters-ja.png) | [截图](evidence/flutter/mobile-board-filters-ja.png) |

## 桌面列与断点

表头和数据行使用同一份列定义。玩家列吸收剩余宽度，分数、偏差、局数和日期使用等宽数字。点击表头切换排序。以同一份两人样例验证按胜率降序后第 2 名排在第 1 名前面，原名次保持。机体榜的灵梦示例显示机体分 1,410、总分 1,438、计分 13 局。 [网页表格](https://github.com/Ritosa2333/eagler-touhou/blob/85941f0d564cf10378d2af6b14d41a000bd27612/launcher/app/ui/th09-ratings-preview.tsx) | [按需渲染列表](https://pub.dev/documentation/super_sliver_list/latest/super_sliver_list/SuperSliverList-class.html)

默认字号下，801 px 使用表格，800 px 使用可展开的玩家行。回归覆盖两种布局之间往返、机体选择、打开头像卡、查看个人主页并返回。用 65 人样例定位第 65 名，缩窄窗口后放宽，验证目标始终可见并保有键盘焦点。现有 Pencil 导出缺少榜单表格状态。

| 状态 | 网页 | Flutter |
| --- | --- | --- |
| 1440 × 734 | [深色总榜](evidence/web/desktop-board-columns.png)、[亮色总榜](evidence/web/desktop-board-columns-light.png) | [深色总榜](evidence/flutter/desktop-board-columns.png)、[亮色总榜](evidence/flutter/desktop-board-columns-light.png) |
| 801 × 640 | [机体榜](evidence/web/desktop-narrow-board-character-columns.png) | [机体榜](evidence/flutter/desktop-narrow-board-character-columns.png) |

## 个人主页三语

从公开个人主页进入花映塚，WHR 和名次共用选择器，机体分使用同一日期轴对照。图表日期按 UTC 显示，对局按玩家设备本地日期分组。胜、负、平同时用文字和颜色呈现。点击公开对手的头像可查看卡片，点击姓名进入对手主页。

两端使用同一份 32 局、20 胜的样例。回归核对 WHR 1,438、名次 #65、上一点 #72、灵梦机体分 1,410、录像双方机体名，以及对局与录像失败后的重试。320 px 英文机体分对照用系统返回关闭。Flutter 在手机用原生底部弹层，网页用独立对照视图。现有 Pencil 名次和逐局录像截图采用其他样例。 [公开战绩网页实现](https://github.com/Ritosa2333/eagler-touhou/blob/85941f0d564cf10378d2af6b14d41a000bd27612/launcher/app/components/launcher/ProfileTh09Duels.tsx)

| 状态 | 网页 | Flutter |
| --- | --- | --- |
| 简中名次，1440 × 734 | [截图](evidence/web/desktop-profile-th09-rank-zh-CN.png) | [截图](evidence/flutter/desktop-profile-th09-rank-zh-CN.png) |
| 英文机体分，320 × 640 | [截图](evidence/web/mobile-narrow-profile-th09-comparison-en.png) | [截图](evidence/flutter/mobile-narrow-profile-th09-comparison-en.png) |
| 日语逐局录像，390 × 844 | [截图](evidence/web/mobile-profile-th09-replays-ja.png) | [截图](evidence/flutter/mobile-profile-th09-replays-ja.png) |
