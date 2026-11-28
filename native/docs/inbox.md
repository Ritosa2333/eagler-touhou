# 消息中心

## 入口与预览

| 编号 | 位置与内容 | 行为 |
| --- | --- | --- |
| INBOX-01 | 桌面顶栏右侧为公告、私信、账户。公告与私信各有未读计数。手机单机顶栏保留相同的两个入口。 | 桌面点击打开当前入口下方的小窗。再次点击、按 Esc 或点窗外关闭。手机直接进入对应栏目。 |
| INBOX-02 | 公告预览左侧为「最新公告」，右侧为「查看全部」。正文行的 32 px 头像与文字相隔 10 px。时间先于正文，作者和 UTC+8 发布时间在正文下方。 | 只显示最新一条。正文默认最多 6 行，溢出时可原位展开或收起。打开后只记当前预览的公告 ID。 |
| INBOX-03 | 私信预览与公告共用小窗。游客提示打开收件箱，登录后显示未读数。 | 游客打开预览只读取公共数据。进入完整私信页后按现有登录流程返回。 |
| INBOX-04 | 小窗与触发按钮右缘对齐，宽 392 px，向下隔 7 px。正文超过窗口可用高度时在小窗内滚动。 | 打开、展开和关闭都留在当前页面，书架和侧栏位置保持。只有「查看全部」进入消息页，返回原入口所在页面。 |
| INBOX-05 | 公告列表与预览读取同一份最新公告数据和头像。全文列表默认最多 10 行。 | 当前公告栏目将已加载的公告记为已读。公告 ID 按设备保存。私信从账户服务读取未读数，对话确认已读后更新入口计数。 |
| INBOX-06 | 加载失败时在当前小窗显示重试，公告列表为空时显示暂无公告。 | 重试保持原窗口与栏目。打开私信预览、登录状态变化、读过会话、应用回到前台都有对应的刷新入口。 |

预览用 Flutter 的锚定弹层处理键盘、窗外点击和可用高度。三语标题、空态、重试、未读与公告时间随应用语言切换。 [Flutter MenuAnchor](https://api.flutter.dev/flutter/material/MenuAnchor-class.html)

## 完整页面

| 编号 | 页面和权限 | 当前验证 |
| --- | --- | --- |
| INBOX-07 | 公告栏目按接口权限显示编辑区和发布按钮。每条公告使用作者用户名读取公开头像。 | 用隔离账户验证空列表发布、空文本禁用、提交中防重复、正文更新及返回。同一篇草稿重试使用原请求编号。 |
| INBOX-08 | 私信列表行含昵称、账号、末条消息和未读。可按合法用户名或从个人主页进入会话，显示对方身份、消息方向、历史与发送区。 | 覆盖游客登录后回到私信、无会话、加载、失败重试、翻页、清除未读、空消息和发送失败后重试，返回原会话列表。正式账号写入待验。 |
| INBOX-09 | 桌面保留 44 px 主顶栏。内容容器最大宽 1030 px，上沿距顶栏 58 px。左栏宽 220 px，右侧间距 40 px。手机以 52 px 返回标题栏和 42 px 双页签进入。 | 栏目变化只替换当前消息地址，回退仍回原入口。直接打开消息地址时，返回可到书架。 |
| INBOX-10 | 桌面栏目行高至少 42 px，间隔 4 px。选中行为实色圆角底，侧栏以细线与正文分隔。多条公告由头像中心线连接，头像 35 px。手机页签使用 38 px 下划线。 | 1440 px 下外框左沿 x = 205 与 Pencil 一致，右侧内容从 x = 465 开始。781 px 时左右起点为 x = 24 和 284。打开同一栏目的「查看全部」维持当前层级。 |
| INBOX-11 | 登录后，桌面私信并列显示最近会话与对话。左栏宽 164–250 px，右侧自适应。选中会话使用圆角底，右栏输入区贴底，消息区独立滚动。手机进入独立会话页。 | 1440 px、781 px 验证列宽和搜索提示。切换联系人复用列表，已读后更新会话行、栏目及顶栏计数。缩至 390 px 后保留当前会话地址，返回先回列表，再回原入口。 |
| INBOX-12 | 桌面选中会话使用独立地址。右上角关闭按钮清掉选中项，搜索仍可按用户名或 @用户名打开会话。 | 使用两位隔离联系人验证收件箱、发送失败和原编号重试。正式账号写入与 Android 真机待验。 |
| INBOX-13 | 登录后每 30 秒刷新未读和当前私信，切回前台立即刷新。翻历史和补取新消息时保留阅读位置，重新滚到最新一条后更新已读。 | 390 px 和 1440 px 用 44 条不同高度的模拟消息验证分页、后台停止、恢复刷新、跨页补取和已读。记录消息到视口上沿的位置，前后偏差不超过 1 px。 |
| INBOX-14 | 公告作者、会话、账户入口与关系列表使用共用头像。用户名解析到后端版本媒体地址，多处读取共用资料缓存。 | 加载中、账号不存在、没有头像和图片解码失败使用同一替代图。切换账号后不能显示上个账号的头像，资料更新应刷新相应头像。 |

会话用居中的 `CustomScrollView` 固定已加载消息的位置，以 `SuperSliverList` 布局不同高度的消息气泡。 [Flutter 滚动锚点](https://api.flutter.dev/flutter/widgets/ScrollView/center.html) | [Super Sliver List](https://pub.dev/packages/super_sliver_list)

气泡沿用固定网页的 90% 宽度上限、10 px 圆角及平面底色。正文内边距为横向 12 px、纵向 8 px，11 px 时间与正文间隔 4 px。发出消息使用品牌色 15% 混入页面底色。桌面、390 px 日文和 320 px 英文的亮暗及双倍字体实拍覆盖短消息、连续长串和左右对齐；补读历史及前台刷新继续验证原消息位置。

## 共用头像回归

[气泡整改前](evidence/flutter/bubble-layout-before.png) | [整改后](evidence/flutter/bubble-layout-after.png)。同为 390 px 日文深色状态，使用相同的隔离会话数据。

用目录内两张机体图作为隔离头像样例，从公告地址进入 320 px 和 1440 px 界面。资料更新后，公告作者与桌面账户入口一起换图，头像矩形保持一致。共用控件另验首次加载、旧账号请求晚到、缺失图片、404、无法解码的图片和游客状态，均不会把上个账号的图片带入新身份。

本机 HTTP 服务按正式接口结构返回版本媒体地址，验证公开资料查询参数、同一账号并发读取合并、失效后刷新、空头像缓存，以及 404 后可再次读取。这些检查不上传头像、不写入正式账号。

[320 px 公告](evidence/flutter/announcement-avatar-320.png) | [1440 px 公告和账户入口](evidence/flutter/announcement-avatar-1440.png)

## 同状态对照

网页截图来自正式站 `85941f0d56`。Flutter 预览读取同一正式站的公开公告，私信游客状态使用未登录会话。Pencil 公告稿采用多条演示公告，私信稿采用登录态示例。

| 状态 | 网页 | Pencil | Flutter |
| --- | --- | --- | --- |
| 1440 × 734 公告预览 | [截图](evidence/web/desktop-announcement-preview.png) | 待补 | [截图](evidence/flutter/desktop-announcement-preview.png) |
| 1440 × 734 私信游客预览 | [截图](evidence/web/desktop-inbox-preview-guest.png) | 待补 | [截图](evidence/flutter/desktop-inbox-preview-guest.png) |
| 1440 × 734 完整公告页 | [截图](evidence/web/desktop-announcements-guest.png) | [设计](evidence/pencil/desktop-message-center.png) | [截图](evidence/flutter/desktop-announcements-guest.png) |
| 390 × 844 完整公告页 | [截图](evidence/web/mobile-announcements-guest.png) | [设计](evidence/pencil/mobile-announcement.png) | [截图](evidence/flutter/mobile-announcements-guest.png) |
| 游客私信页 | [桌面](evidence/web/desktop-messages-guest.png)、[手机](evidence/web/mobile-messages-guest.png) | 待补 | [桌面](evidence/flutter/desktop-messages-guest.png)、[手机](evidence/flutter/mobile-messages-guest.png)、[320 px](evidence/flutter/mobile-narrow-messages-guest.png) |
| 1440 × 734 登录态私信列表 | [截图](evidence/web/desktop-inbox-member.png) | [设计](evidence/pencil/desktop-inbox.png) | [截图](evidence/flutter/desktop-inbox-member.png) |
| 1440 × 734 已选中会话 | [截图](evidence/web/desktop-inbox-thread.png) | [设计](evidence/pencil/desktop-inbox-thread.png) | [截图](evidence/flutter/desktop-inbox-thread.png) |
| 781 × 640 已选中会话 | 待补 | 待补 | [截图](evidence/flutter/desktop-narrow-inbox-thread.png) |

坐标回归覆盖 1440 px 宽屏和 781 px 窄窗，核对小窗尺寸、按钮右缘、窗外点击以及切换预览后的状态。网页与 Flutter 同为触发按钮右对齐。顶栏的账户按钮宽度不同，使绝对横坐标有所不同。截图保留各自的边框、阴影和选中状态。较长公告使用 12 行独立样例。登录状态、私信已读和持久化用隔离账户与独立存储验证。390 px 检查手机直接进入完整栏目。三种输出视口均留有游客私信与公开公告截图。登录态网页与 Flutter 截图使用同样的测试账号、联系人和正文。网页按发布版在列表与对话间切换，Flutter 桌面采用 Pencil 的并列布局，并沿用消息中心整体对齐。
