# 历史规格 | 已停止原生维护

2026-11-27：本页功能改用正式站网页。以下是归档前记录，不是当前 Flutter 开发或验收要求。

# 手机主页与个人主页

以 `/u/:username` 为个人主页入口。桌面从账户菜单直达，手机主页的账号行进入个人主页，三个计数各自进入对应列表。Pencil 的两端个人主页使用示例账号和“全部作品”视图。运行对照使用正式站公开的 @ritosa 资料。

| 编号 | 状态与控件 | 布局与验收 |
| --- | --- | --- |
| HOME-01 | 游客与账号行 | 游客看到登录和注册。登录后显示共用头像、昵称、账号、地区和进入个人主页的箭头。窄屏文字留在头像与动作之间。 |
| HOME-02 | 关系计数 | 关注、粉丝、好友等宽排列。触发时打开相应列表，返回回到主页。账号退出后收起个人资料和计数。 |
| HOME-03 | 设置组 | 启动器、主题和账户在同一卡片内折叠。语言和减少动画归启动器，明暗外观归主题。账户组随登录出现，可进入资料、安全与云存档。背景入口打开与顶栏共用的[背景设置](../../../docs/appearance.md)。 [窄屏语言](../../../docs/evidence/flutter/mobile-narrow-home-language.png) |
| PROFILE-01 | 封面、头像和身份 | 远端媒体读取同源版本地址。手机封面通栏高 150 px，64 px 头像叠入封面 24 px，正文边距 16 px。普通手机宽度下身份与关注操作同行。返回和刷新位于封面内，随封面滚动。头像加载失败时用共用占位。 |
| PROFILE-01.2 | 窄宽身份与动作 | 固定版网页在 365 px 及以下把动作组移到身份下方，Flutter 使用相同断点。放大字号后按标题的有效宽度判断，昵称允许完整换行，动作不挤占其文字列。桌面的本人动作在所分配宽度内自然换行。 |
| PROFILE-01.1 | 桌面顶栏与资料列 | 44 px 共用顶栏位于页面转场之外。封面紧接顶栏，可见高 160 px，两者共占 204 px。容器最大宽 1016 px，头像 96 px 叠入封面 26 px，身份间隔 14 px。左资料列宽 200 px，与正文相隔 32 px。返回和刷新在封面顶部。 |
| PROFILE-02 | 简介、注册时间和链接 | 桌面位于资料左列，手机在游戏数据上方。长简介可换行。外链由系统浏览器打开。 |
| PROFILE-03 | 关系、关注和私信 | 私密列表显示说明。统计接口省略好友数时用“—”。游客可登录后关注，登录后可关注、取消关注和打开会话。 |
| PROFILE-03.1 | 关系计数排版 | 个人主页按固定版网页把数字和标签按文字基线同行排列，项目之间留 12 px，整项可换行。单项仍放不下时，数字和完整标签上下排列，避免逐字拆开标签。手机主页继续使用等宽的上下排列；两处共用原来的关系入口与状态。 |
| PROFILE-04 | 关系列表 | 关注、粉丝、好友可切换。首次读取在分类下使用共用静态加载面板。列表行显示真实身份，用统一头像组件。打开他人资料再返回应回到原列表。 |
| PROFILE-05 | 游戏数据切换 | 16 px 中等字重标题与视图选择同行。花映塚与全部作品共用内容容器，按公开配置决定可用视图和初始选项。切换保持当前外层滚动位置。窗口跨桌面和手机断点时沿用当前视图及游戏数据实例。 |
| PROFILE-06 | 单机生涯 | 读取公开存档统计，等待时在游戏数据区使用共用加载面板。作品列优先保留宽度，难度列均分剩余空间。紧凑视图缩写难度，完整标签可通过提示读取。通关表纵向滚动，单作明细返回外层原位置。 |
| PROFILE-07 | 累计游玩 | 显示服务端公开总时长，按作品降序绘制相对时长。缺失或不可见的时长从列表省略。 |
| PROFILE-07.1 | 游玩条排版 | 平面细边框、10 px 圆角与 12 px 内容边距。普通字号时，桌面作品列 120 px、时长列至少 94 px；紧凑内容分别为 104 px 和至少 76 px，色条占余宽。封面 25 px、3 px 小圆角，文字使用正文色。色条高 12 px，依次沿用原稿的粉、绿、紫、红。大字号下作品列随字号增加，时长列按实际文字测量，色条不足 28 px 时移到该行下方。 |
| PROFILE-08 | 花映塚 | [评分、机体和逐局录像规格](../../../docs/th09-ratings.md) |
| PROFILE-09 | 账户专属操作 | 资料编辑、可见性设置和云存档使用同一账号身份。公开访问隐藏私有操作。 |
| PROFILE-09.1 | 资料编辑入口 | 桌面账户菜单、手机主页账户设置和个人主页的本人操作可进入。私有草稿首次读取使用共用静态加载面板，不再叠加顶栏进度。失败重试沿正文左边线排列。退出登录或会话失效后隐藏草稿。 |
| PROFILE-09.2 | 头像与封面 | 紧凑预览区放在桌面左列和手机表单上方，使用 40 px 共用头像，呈现当前私有草稿。两张图分别选取、裁剪和移除，手机的两组上传动作同行。头像输出正方形及联机用缩略图。封面选取 3:1 的中心区域，保存为适配宽屏的 6:1 图像。支持 PNG、JPEG、WebP，输入上限 20 MiB，输出上限 150,000 B。 |
| PROFILE-09.2a | 裁切工具与预览 | 拖动裁切框或用方向键定位。滑杆、加减按钮调整范围，重置恢复居中。头像下方显示实际联机缩略图。封面 3:1 与 6:1 同时预览，宽屏同行，手机上下排列。编码期间保留确认状态。取消、Esc 或系统返回恢复资料表单。 |
| PROFILE-09.3 | 昵称、简介和链接 | 窗口宽于 1000 px 时，预览与字段按 .85:1.4 分列，间距 28 px；其余宽度顺序纵排。昵称最多 12 个字符、简介最多 500 字，链接最多 12 个。桌面链接名称与 HTTP/HTTPS 地址按 1:2 同行，删除在末尾，手机名称在地址上方。添加动作位于链接组下方。 |
| PROFILE-09.4 | 显示和默认视图 | 花映塚和全部作品各自控制可见性。可从已显示视图中选默认项。关系列表单独控制，旧配置中的顺序与额外字段随保存提交。 |
| PROFILE-09.5 | 保存和返回 | 桌面保存动作归正文末尾，窄屏固定在底部，含审核状态和失败反馈。保存按钮至少宽 104 px，等待时在原图标位置显示 16 px 进度，不挤占旁边状态文字。预览卡可打开公开个人主页，返回时恢复草稿。返回按钮、系统返回、账户栏目及顶栏单机／联机均检查草稿，可继续编辑或放弃。叠加打开的编辑页分别保护各自草稿，保存后回原入口。 |
| PROFILE-09.6 | 三语与紧凑动作 | 表单、权限、图片校验、审核、重试及放弃草稿随界面语言显示。图片行宽度低于 360 px 时，选图动作使用带提示的图标按钮；较宽时显示文字，并沿用共用控件尺寸。 |
| PROFILE-10 | 状态和返回 | 包含加载、找不到玩家、读取错误、私密、无记录。首次加载时以静态骨架预留封面和头像的位置。页面返回先退到实际来源，有直接打开场景时回单机。 |
| PROFILE-11 | 关系地址与三语 | 网页的 `?social=following`、`?social=followers`、`?social=friends` 和旧 `#profile-friends` 可直达。分类切换替换当前地址，返回个人主页。列表、空态、私密及操作提示随界面语言显示。 |

## 本轮验收

个人主页的旧实现遗漏了 `profile-preview.css` 的 365 px 断点，并给昵称设置了两行截断。相同字体、资料和视口的 48 个场景中，旧版有 20 个身份布局检查失败。本轮恢复窄宽动作行与个人主页的行内关系计数，覆盖游客、本人和其他玩家、亮暗主题、100% 与 200% 字号，以及 320、390、781、1440 px。检查昵称和关系标签的实际字形，执行登录返回、资料编辑返回及关注操作。

| 身份区状态 | 整改前 | 整改后 |
| --- | --- | --- |
| 320 px、英语游客、普通字号 | [截图](../../../docs/evidence/flutter/narrow-guest-light-1-before.png) | [截图](../../../docs/evidence/flutter/narrow-guest-light-1-after.png) |
| 390 px、日语游客、200% | [截图](../../../docs/evidence/flutter/mobile-guest-dark-2-before.png) | [截图](../../../docs/evidence/flutter/mobile-guest-dark-2-after.png) |
| 781 px、本人长昵称、200% | [截图](../../../docs/evidence/flutter/compact-desktop-owner-light-2-before.png) | [截图](../../../docs/evidence/flutter/compact-desktop-owner-light-2-after.png) |

旧规格中的「身份与关注操作同行」缺少窄屏例外，已按固定版源码纠正。首次加载仍为封面、头像和窄宽操作行预留位置。三语样例的同输入网页运行截图尚缺，现有网页公开资料与 Pencil 用于核对结构，当前截图仍待用户视觉验收。

累计游玩区曾均分作品名和色条的宽度，320 px 英文界面因此只显示「东方…」。本轮按固定版 `profile-game-data.css` 和手机 Pencil 恢复列宽、色条和小封面，原来的整块阴影改为 Material 的描边卡片。保留至少 44 px 的作品点击行，较原稿的紧凑文字行更高。资料编辑的显示分组也改用描边卡片，沿用既有 8 px 圆角和字段位置。

同一份三作游玩样例、相同字体与视口对照：

| 视口与状态 | 整改前 | 整改后 |
| --- | --- | --- |
| 320 × 640、英语、亮色 | [截图](../../../docs/evidence/flutter/narrow-light-playtime-1-before.png) | [截图](../../../docs/evidence/flutter/narrow-light-playtime-1-after.png) |
| 390 × 844、日语、暗色 | [截图](../../../docs/evidence/flutter/mobile-dark-playtime-1-before.png) | [截图](../../../docs/evidence/flutter/mobile-dark-playtime-1-after.png) |
| 1440 × 734、简中、暗色 | [截图](../../../docs/evidence/flutter/desktop-dark-playtime-1-before.png) | [截图](../../../docs/evidence/flutter/desktop-dark-playtime-1-after.png) |

三种宽度各以亮暗主题和 100%、200% 字号验证完整标题、色条最小宽度、时长右缘和明细返回后的滚动位置。原实现的两个 320 px 普通字号用例能复现标题截断。网页已有运行截图采用公开玩家资料，本轮三作样例的网页实拍尚缺；样例前后图不作为用户认可基准。

先前以公开 @ritosa 资料核对了封面与正文层级，样例为 WHR 1,388、名次 #163、14 位粉丝。移除独立手机标题栏和纵排关注区后，390 px 首屏的游戏数据从约 y = 447 移到约 y = 347。下表保留这轮历史对照；本轮身份区调整以上方的新截图为准。

| 视口 | 整改前 | 当前调整 |
| --- | --- | --- |
| 1440 × 734 | [截图](../../../docs/evidence/flutter/desktop-profile-frame-before.png) | [截图](../../../docs/evidence/flutter/desktop-profile-frame-after.png) |
| 390 × 844 | [截图](../../../docs/evidence/flutter/mobile-profile-layout-before.png) | [截图](../../../docs/evidence/flutter/mobile-profile-frame-after.png) |
| 320 × 640 | [截图](../../../docs/evidence/flutter/mobile-narrow-profile-layout-before.png) | [截图](../../../docs/evidence/flutter/mobile-narrow-profile-frame-after.png) |

这些是 Windows 字体渲染下的竖屏检查，整体视觉尚待用户验收。桌面个人主页与书架、消息中心共用顶栏框架，正文在顶栏下方滚动。固定版网页的顶栏叠在封面上；Flutter 使用独立的实色顶栏，并让封面下沿同样位于 y = 204。

1440 px 的游戏数据正文从 x = 444 开始，与 Pencil 的正文轴一致。781 和 1440 px 进出个人主页的 24 个采样帧中，站点区像素保持一致。 [元素坐标](../../../docs/evidence/flutter/desktop-profile-frame-geometry.json)

- 320 px 与 1440 px 用延迟资料样例核对封面、头像坐标，骨架和真实资料到达时位置相同。 [手机加载中](../../../docs/evidence/flutter/profile-loading-320.png) | [手机已加载](../../../docs/evidence/flutter/profile-loaded-320.png) | [桌面加载中](../../../docs/evidence/flutter/profile-loading-1440.png) | [桌面已加载](../../../docs/evidence/flutter/profile-loaded-1440.png) | [Skeletonizer](https://pub.dev/packages/skeletonizer)

- 真实 @ritosa 资料验证了头像、封面、简介、粉丝列表、单机统计和花映塚对局。证据包含 1440、390 和 320 px 的网页和 Flutter。 [截图对照](../../../docs/comparison.md)
- 320 px 控件回归走过主页、粉丝列表、另一玩家的私密关系列表、关注和返回。桌面从账户菜单直接进入个人主页，单作打开和关闭前后的通关标题位置一致。三个宽度均验证了从全部作品切换到较短的对局区时外层滚动不跳变，且首次打开通关表从表头开始。
- 个人主页的单作明细复用详情页通关表，按作品目录显示角色名、分支和玩法。成绩公开时提供同一最高分提示，记录私密时隐藏分数。[共享通关表](../../../docs/game.md#通关与符卡)。
- 桌面从账户菜单进入，以 th06 的公开记录样例点击灵梦 A 的 Normal。提示显示一命通关及最高分 1,111，关闭后回到全部作品原位置。[入口实拍](../../../docs/evidence/flutter/desktop-profile-work-score.png)。
- 网页和 Flutter 使用同一隔离账号样例验证编辑布局，覆盖桌面、手机及 320 px 窄屏。320 px 回归通过系统返回、继续编辑、打开公开主页、地址校验、可见性保存和审核状态更新。桌面验证放弃草稿后回书架。测试服务器校验读写请求的会话与提交字段。
- 正式账号资料写入、系统文件选择器和 Android 真机待验。云存档及账户安全见[云存档](../../../docs/cloud-saves.md)与[账户安全](account.md#账户安全)。
- 地区名沿用与网页相同的英文地区名来源，地区图标使用 Flutter 组件。 [Country Flags](https://pub.dev/packages/country_flags)

## 裁切预览

网页和 Flutter 都使用仓库的账户插画，打开资料编辑中的头像与封面裁切。三个视口验证缩放、复位、方向键、关闭和返回。确认时逐字节核对联机头像预览与返回的缩略图。封面预览沿用上传的采样范围，手机视图取桌面封面的中间一半。Flutter 使用可移动裁切框，网页移动框后的图片。桌面封面两翼沿用 Flutter 的镜像填充，网页会额外柔化镜像边缘。Pencil 的编辑与裁切画面待补。 [裁切组件](https://pub.dev/packages/crop_your_image/versions/2.0.1)

| 视口 | 头像 | 封面 |
| --- | --- | --- |
| 1440 × 734 | [网页](../../../docs/evidence/web/desktop-profile-crop-avatar.png) / [Flutter](../../../docs/evidence/flutter/desktop-profile-crop-avatar.png) | [网页](../../../docs/evidence/web/desktop-profile-crop-cover.png) / [Flutter](../../../docs/evidence/flutter/desktop-profile-crop-cover.png) |
| 390 × 844 | [网页](../../../docs/evidence/web/mobile-profile-crop-avatar.png) / [Flutter](../../../docs/evidence/flutter/mobile-profile-crop-avatar.png) | [网页](../../../docs/evidence/web/mobile-profile-crop-cover.png) / [Flutter](../../../docs/evidence/flutter/mobile-profile-crop-cover.png) |
| 320 × 640 | [网页](../../../docs/evidence/web/mobile-narrow-profile-crop-avatar.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-profile-crop-avatar.png) | [网页](../../../docs/evidence/web/mobile-narrow-profile-crop-cover.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-profile-crop-cover.png) |

## 关系列表与关注

个人主页和手机主页共用关注、粉丝、好友计数。计数使用当前语言的数字格式，服务端省略好友数时显示横线。本人资料隐藏关注和私信动作，比较账号时忽略字母大小写。游客的关注入口先进入登录，系统返回回到原玩家。

列表支持网页地址与早期 Flutter 的 `/u/:username/social?tab=followers`。切换分类复用当前页面，地址同步更新。进入他人资料、打开私信后返回，可继续原入口。会话变更后按当前身份重新读取列表，旧身份的延迟响应不能覆盖当前结果。 [网页关系模块](https://github.com/Ritosa2333/eagler-touhou/blob/85941f0d564cf10378d2af6b14d41a000bd27612/launcher/app/components/launcher/SocialSection.tsx) | [GoRouter 同页替换](https://pub.dev/documentation/go_router/latest/go_router/GoRouter/replace.html)

同一份两人列表用于网页和 Flutter 对照。三种视口分别核对粉丝、空好友列表与私密粉丝列表。控件回归覆盖读取失败重试、关注失败重试、保存与刷新期间禁用重复提交、打开私信与返回，以及退出后拒绝旧身份的私密响应。关注写入使用隔离样例，正式账号写入和 Android 真机仍待验收。Pencil 的个人主页可对照计数位置，当前导出没有关系列表页面。

| 状态 | 网页 | Flutter |
| --- | --- | --- |
| 简中粉丝，1440 × 734 | [截图](../../../docs/evidence/web/desktop-social-followers-zh-CN.png) | [截图](../../../docs/evidence/flutter/desktop-social-followers-zh-CN.png) |
| 英文空好友，320 × 640 | [截图](../../../docs/evidence/web/mobile-narrow-social-empty-en.png) | [截图](../../../docs/evidence/flutter/mobile-narrow-social-empty-en.png) |
| 日语私密，390 × 844 | [截图](../../../docs/evidence/web/mobile-social-private-ja.png) | [截图](../../../docs/evidence/flutter/mobile-social-private-ja.png) |

## 资料编辑三语

资料编辑恢复账户设置的共用顶栏与栏目，预览区缩为身份卡，移除大块空白封面。昵称和简介复用账户字段，链接恢复原分组，显示偏好以全部作品、花映塚的顺序呈现。窗口从桌面变为手机再恢复，以及 200% 文字缩放后，草稿与输入实例保持一致。 [桌面整改前](../../../docs/evidence/flutter/desktop-profile-settings-before.png) | [整改后](../../../docs/evidence/flutter/desktop-profile-settings-after.png)

两端读取同一份隔离草稿，含昵称、简介、一个 HTTP 链接及公开配置。三个视口从资料编辑地址进入，验证加载失败重试、审核拒绝后保留草稿、系统返回继续编辑、重新保存和离开。窄屏选图按钮使用提示说明，保存栏与表单分别布局。图片选取、裁切和正式账号写入的验收范围见上文。

| 视口与语言 | 网页 | Flutter | 保存失败 | 放弃确认 |
| --- | --- | --- | --- | --- |
| 1440 × 734 简中 | [截图](../../../docs/evidence/web/desktop-profile-edit-zh-CN.png) | [截图](../../../docs/evidence/flutter/desktop-profile-edit-zh-CN.png) | [截图](../../../docs/evidence/flutter/desktop-profile-edit-zh-CN-rejected.png) | [截图](../../../docs/evidence/flutter/desktop-profile-edit-zh-CN-discard.png) |
| 320 × 640 英文 | [截图](../../../docs/evidence/web/mobile-narrow-profile-edit-en.png) | [截图](../../../docs/evidence/flutter/mobile-narrow-profile-edit-en.png) | [截图](../../../docs/evidence/flutter/mobile-narrow-profile-edit-en-rejected.png) | [截图](../../../docs/evidence/flutter/mobile-narrow-profile-edit-en-discard.png) |
| 390 × 844 日语 | [截图](../../../docs/evidence/web/mobile-profile-edit-ja.png) | [截图](../../../docs/evidence/flutter/mobile-profile-edit-ja.png) | [截图](../../../docs/evidence/flutter/mobile-profile-edit-ja-rejected.png) | [截图](../../../docs/evidence/flutter/mobile-profile-edit-ja-discard.png) |

## 等待与提交状态

资料编辑、单机生涯、花映塚数据和关系列表复用静态加载面板。尺寸来自固定版的 `ProfilePanelLoading`：至少高 128 px，内边距 20 px，两条 12 px 骨架相隔 16 px。Flutter 使用已有 Skeletonizer 的 `SolidColorEffect` 和 Material 3 细边框卡，圆角为 12 px，网页为 10 px。资料初次读取不再重复显示顶栏进度，失败重试沿正文左边线排列。24 个等待场景读取实际语义树，核对 status 角色、加载标签及骨架隐藏。原稿没有单独的等待状态导出，目前的网页证据为固定版源码，运行截图待补。

保存按钮恢复网页规定的 104 px 最小宽度。原来插在按钮外的进度会压缩状态文字 28 px，改在按钮的 16 px 图标位置显示后，按钮和文字区域的矩形在提交前后相等。该检查使用带审核中状态的同一草稿，覆盖三种宽度的亮暗主题。提交期间返回仍等待请求结束。

| 状态 | 整改前 | 当前调整 |
| --- | --- | --- |
| 桌面首次读取 | [截图](../../../docs/evidence/flutter/desktop-dark-editor-loading-zh-CN-before.png) | [截图](../../../docs/evidence/flutter/desktop-dark-editor-loading-zh-CN-after.png) |
| 320 px 英文提交 | [截图](../../../docs/evidence/flutter/mobile-narrow-dark-editor-saving-en-before.png) | [截图](../../../docs/evidence/flutter/mobile-narrow-dark-editor-saving-en-after.png) |
| 390 px 日文关系列表读取 | [截图](../../../docs/evidence/flutter/mobile-light-social-loading-ja-before.png) | [截图](../../../docs/evidence/flutter/mobile-light-social-loading-ja-after.png) |

