# 书架与主导航

[桌面实拍](evidence/web/desktop-library.png) | [手机实拍](evidence/web/mobile-library.png) | [窄屏实拍](evidence/web/mobile-narrow-library.png) | [桌面 Pencil](evidence/pencil/desktop-library.png) | [手机 Pencil](evidence/pencil/mobile-library.png)。

## 外层导航

| 编号 | 元素 | 桌面 | 手机 | 交互检查 |
| --- | --- | --- | --- | --- |
| NAV-01 | 主导航 | 单机和联机居中，实拍顶栏高 44 px。 | 书架顶栏实拍高 48 px。单机、联机、主页在底部分三列，390 px 实拍高度 64 px。 | 点击当前项维持所在区域，点击另一项进入该根页。切换过程检查底栏、侧栏和页面内容的位置。 |
| NAV-02 | 顶栏辅助入口 | 站点入口靠左，公告、私信、账户区靠右。 | 菜单、搜索、公告和私信按从左到右排列。共用 Lucide 线宽，图形中心处于顶栏中线。主页、联机和二级页由各自的导航承接。[竖屏测量](appearance.md#竖屏视觉验收) | 桌面消息弹窗和手机全页入口要能进入同一栏目。 |
| NAV-03 | 末端空间 | 主内容和作品侧栏分开排布。 | 列表最后一项可滚到应用底栏以上，系统安全区按实际设备计算。 | 从长列表回到短列表时，确认内容到边界才收束，固定导航没有中间空白。 |
| NAV-04 | 游戏地址与返回 | 侧栏打开作品，切换到同层另一作时替换当前详情。Windows 启动参数可打开作品。 | 从封面、列表或应用链接进入全屏详情，再进入设置。二级页底栏退场。 | 内部地址 `/game/th07` 和网页形式 `/?game=th07` 进入同一作品。系统返回和可见返回按钮逐级退出。失效的作品地址可回书架。 [作品链接](#作品链接) |
| NAV-05 | 界面语言 | 桌面从站点菜单打开小型语言弹窗。 | 单机抽屉内打开语言子面板，主页的启动器设置也提供同一选择器。 | 首次启动默认简体中文。简中、英语和日语选项按自称显示。切换保留当前书架、筛选和详情。返回先回抽屉或打开设置的原页面，重启读取同一偏好。 |
| NAV-06 | 窗口跨断点 | 781 px 起显示桌面顶栏和侧栏。 | 780 px 及以下使用单机书架顶栏和根页底栏。 | 跨断点时保留详情、搜索、有效滚动位置及房间筛选。手机抽屉切到桌面时关闭。 |
| NAV-07 | 连续切换 | 单机与联机可以在动效中反向切换。 | 单机、联机、主页可在前一段淡入中继续切换。 | 新过渡从点按时的画面继续，导航选中与输入权限立即更新。 |
| NAV-08 | 桌面窗口 | 隐藏系统标题栏。公告、私信和账户共组，右端为最小化、最大化／还原、关闭。空白区域支持拖动。 | 手机使用自己的导航；桌面窗口缩到竖屏时另显示紧凑窗口栏。 | 实窗检查系统按钮、关闭请求、跨断点布局及进入独立页时的导航视口。实窗已观察空白标题栏双击最大化与还原，普通 Release 标题栏关闭的一次进程退出检查通过；另捕获到间歇性原生退出卡住，见[性能排查](../archive/performance-audit-20261128/report.md#退出卡住的原生证据)。右边缘拖窄与 th08 详情最大化跨断点已实测，标题栏拖动与 Snap 菜单继续待验。 |
| NAV-09 | iOS 侧滑返回 | 平板宽度沿用 iOS 转场，侧栏留在内容区外。 | 详情、设置和个人主页支持跟手返回、中途取消及完成返回。 | 减少动画时保留系统手势，使用淡化反馈。普通转场中页面空白不透出上一页控件，壁纸沿用窗口坐标。返回恢复原书架滚动及原大厅。 |

## 跨断点回归

正式站在同一标签页搜索 `th0`，打开 th06，依次切到 781、780、781 px，作品地址与详情保持一致。Flutter 从书架实际打开 th06，在相同宽度间切换后返回，书架一直停在 140 px。另以房间目录样例核对「可观战」筛选，以及手机抽屉在切回窄屏时保持关闭。Pencil 提供桌面与手机详情稿。 [网页记录](evidence/web/resize-breakpoint.json) | [Flutter 记录](evidence/flutter/navigation-breakpoint.json)

| 宽度 | 正式站 | Flutter |
| --- | --- | --- |
| 781 × 550 | [截图](evidence/web/resize-detail-wide.png) | [截图](evidence/flutter/resize-detail-wide.png) |
| 780 × 550 | [截图](evidence/web/resize-detail-compact.png) | [截图](evidence/flutter/resize-detail-compact.png) |

## 作品链接

Windows 可将 `https://touhouonline.com/?game=th07` 作为启动参数。Android 的作品地址为 `touhouonline://open/?game=th07`，由 Flutter 接收冷启动和运行中的链接。两端沿用同一个单机详情路由。有效作品的返回路径为设置、详情、单机书架。重复、格式错误或目录中无对应作品时，显示找不到该作品，返回单机书架。 [Flutter 链接行为](https://docs.flutter.dev/ui/navigation/deep-linking)

Windows 实际进程以正式站参数启动后进入 th07，以其他域名参数启动后落在书架。桌面和 320 px 控件回归覆盖直达 th07、运行中接收 th075、不可用作品、设置返回及桌面侧栏位置。Pencil 的详情和书架适用于这些目的地，链接接收属于交互验证。Android 预览 APK 已核验链接入口。系统级 HTTPS 关联和 Android 实机接收继续验收。 [桌面直达](evidence/flutter/desktop-game-link.png) | [320 px 直达](evidence/flutter/mobile-game-link.png) | [作品不可用](evidence/flutter/mobile-game-link-unavailable.png)

## 书架内容

| 编号 | 元素 | 位置和行为 | 交互检查 |
| --- | --- | --- | --- |
| LIB-01 | 搜索 | 桌面侧栏顶部，手机单机顶栏中段。共享同一查询。可按作品名称或作品代号查找。 | 输入日文标题、英文标题、`th06` 和无匹配词。清空后检视分类和排序。 |
| LIB-02 | 作品分类 | 桌面行含图标、标签、计数和可用的展开箭头。展开时在组下显示作品行。手机在「全部游戏」标题下单行横向排列，允许横滑。 | 筛选和展开分别验证。手指沿分类栏横滑时，外层文档保持宽度。 |
| LIB-02.1 | 分组折叠 | 单机与联机复用折叠容器。整组从标题下展开，后续分类跟随组底移动；反向点击接着当前位移和速度运动。分类标题、侧栏及右侧内容保持位置。收起项退出焦点、点击与读屏树，完全收起后停止其内部动效。 | 两端分别验证完整收起、途中反向、侧栏滚动及减少动画。 |
| LIB-03 | 最近游玩 | 有记录时置于全部作品之前，独立横向滚动。桌面每项宽 230 px，封面、标题、编号和副标题同排。手机并列四张方形封面，读屏以按钮报告作品名和编号。 | 空历史隐藏整段。成功启动单机后将对应作品移到首位，同一作品只保留一条，最多 20 条。打开详情再返回，横向位置延续。 |
| LIB-04 | 视图和排序 | 桌面放在标题同行靠右，手机放在分类下。封面／列表和排序共用 44 px 控件高度、12 px 标签、16 px 图标和相同文字基线。最宽排序选项决定所需空间。320 px 英文书架的视图切换使用带名称的图标。 | 两种视图和编号正序、编号倒序、名称排序都要跑。计数、列表和侧栏一起检查。 |
| LIB-05 | 作品卡 | 封面方形。桌面封面、作品名、编号和英文副标题组成完整卡片，收藏位于封面右上角；文字区也可打开作品。手机是两列，封面下有作品名、编号和收藏动作。较窄时编号在作品名下换行。列表呈现英文副标题，侧栏收藏位于尾部上方，给英文副标题留宽。 | 缩窄到 320 px；核对右列不溢出、长英文显示策略、hover 和触屏点按命中。作品入口具有按钮角色，可用读屏激活与回车打开，收藏是独立按钮。 |
| LIB-06 | 收藏 | 可从书架卡片、侧栏和详情操作。收藏标志传达已选状态，移动或排序时跟随对应的作品。 | 收藏、取消、打开收藏分类、切换视图、重新打开页面。 |
| LIB-07 | 返回和滚动 | 详情带作品标识，退出回到该次打开它的书架。本地状态负责搜索、过滤、选中、滚动与焦点。桌面侧栏滚动独立于内容区。 | 滚动中点作品；页面返回、系统返回、重复开关详情。在减弱动效时重复。 |
| LIB-08 | 空内容 | 搜索筛空、无收藏、未读取最近记录是不同输入。由书架区域呈现状态。 | 检查空状态后切回类别，跳过无内容时的横向溢出。 |

以上视觉值来自固定版本。选中层级、文字读序和伸缩关系是迁移规格，Flutter 主题可调整色彩、圆角与字重。

## 桌面顶栏对齐

用正式站书架与大厅的 1440 × 734 实拍核对布局。Flutter 左列放站点标志、帮助、支持、主题、菜单，中间是单机和联机，右列放公告、私信与账户。两侧分配等宽空间，账户名称变长时在账户按钮内省略。桌面启动器设置从站点菜单进入。

| 部位 | 尺寸与位置 | 检查结果 |
| --- | --- | --- |
| 顶栏和内容交界 | 顶栏高 44 px，主导航控件高 32 px，上下各留 6 px。单机与联机控件间隔 2 px。 | 1440 px 切换单机、联机和开菜单时，中文导航始终位于 x = 645–795，侧栏从 y = 44 开始。 |
| 三组对齐 | 两侧与中央各隔 12 px。宽屏外缘 28 px，小于 900 px 时收至 16 px。 | 781 px 简中、英文和日文导航的中心都在 x = 390.5，书架操作区根据译文长度收缩。 |
| 辅助入口与菜单 | 站点图标 14 px，视觉中心下移 2 px。站点菜单项最小宽 220 px、高 36 px，菜单在顶栏下方展开。 | 验证了登录、账户、私信返回、亮暗切换和标志回书架。公告和私信从同一顶栏打开小窗。 [预览尺寸与交互](inbox.md) |

[改前书架](evidence/flutter/desktop-library-before-masthead.png) | [改后书架](evidence/flutter/desktop-library.png) | [改后大厅](evidence/flutter/desktop-lobby-live.png) | [菜单展开](evidence/flutter/desktop-masthead-site-menu.png) | [781 px 已登录](evidence/flutter/desktop-masthead-narrow-account-light.png) | [781 px 英文](evidence/flutter/desktop-masthead-narrow-en.png) | [781 px 日文](evidence/flutter/desktop-masthead-narrow-ja.png) | [坐标记录](evidence/flutter/desktop-masthead.json)

桌面站点标志使用原有按钮，读屏名称为「東方 Online」。单机、联机、作品行和消息栏目用组件自身的选中状态；导航缩为图标时仍报告完整名称。控件回归覆盖指针、读屏激活和回车，Windows 实窗另检查 1024 与 801 px 导航。210 张同状态图像保持一致。[验收记录](evidence/windows/brand-accessibility.json)

## 共用侧栏对齐

书架、详情、游戏设置与大厅调用同一套分类行和作品行。侧栏宽度沿用网页的 `clamp(248px, 24vw, 296px)`，滚动区独立于右侧。桌面侧栏用较低的表面层级，选中行使用中性底色。分类下的作品缩进 18 px，采用 32 px 封面与较紧凑的文字角色，与原稿的信息层级对应。房间和主界面从同一套亮暗色阶取色。 [FlClash 的侧栏与表面](https://github.com/chen08209/FlClash/blob/main/.agents/surfaces.md)

以下数据来自 1440 × 734 的实际 Flutter 渲染。截图比例为 1:1。书架与大厅可以有不同的分组顺序，纵向坐标随分组变化。

| 位置 | 书架和大厅的共用坐标 | 视觉作用 |
| --- | --- | --- |
| 侧栏、搜索和选中行 | 侧栏 x = 0–296，搜索与分类 x = 16–280 | 两边各留 16 px，选中底色覆盖整行。搜索高 44 px，分类高 38 px。 |
| 分类名与计数 | 分类文字左沿 x = 58，计数右沿 x = 238 | 有无展开箭头都留同一个槽位。单、双位计数末位齐平。 |
| 封面和两行标题 | 封面 x = 44–76，标题和副标题从 x = 86 开始 | 32 px 封面与文字间隔 10 px。两行各有固定起点，底行延伸到 x = 270。 |
| 收藏与圈选 | 收藏命中区 x = 243–279，16 px 星形位于标题高度。联机圈选中心在封面左上方。 | 收藏与选中底色一起移动，英文副标题可以用尾部空间。圈选随封面垂直居中定位，行高变化后仍在封面左上。 |

| 主题 | 单机 | 联机 | 坐标 |
| --- | --- | --- | --- |
| 深色 | [截图](evidence/flutter/desktop-side-library-dark.png) | [截图](evidence/flutter/desktop-side-lobby-dark.png) | [测量](evidence/flutter/desktop-sidebar-alignment-dark.json) |
| 亮色 | [截图](evidence/flutter/desktop-side-library-light.png) | [截图](evidence/flutter/desktop-side-lobby-light.png) | [测量](evidence/flutter/desktop-sidebar-alignment-light.json) |

另有[旧单机截图](evidence/flutter/desktop-library-before-sidebar.png)。大厅用同一公开房间样例对比[改前](evidence/flutter/desktop-lobby-live-before-sidebar.png)和[改后](evidence/flutter/desktop-lobby-live.png)。逐帧回归核对排序、鼠标移向收藏按钮、单机和大厅互切、滚动位置、详情与设置返回。独立入口覆盖手机选单里的圈选操作。

## 工具栏对齐

320 和 390 px 竖屏中，「封面／列表」与排序采用同样的外框和内容中心。分段选中的色块内收 3 px，切换遵循减少动画设置。窄屏英文保留带名称的图标，为完整排序选项让出空间。计数和作品编号使用较轻的颜色与字重。字体按[平台字体约定](appearance.md#平台字体)统一选择。

[320 px 改前](evidence/flutter/mobile-narrow-polish-library-before-type.png) | [320 px 中文](evidence/flutter/mobile-narrow-polish-library.png) | [390 px 中文](evidence/flutter/mobile-polish-library.png) | [320 px 英文](evidence/flutter/mobile-narrow-en-polish-library.png) | [桌面](evidence/flutter/desktop-polish-library.png)

## 首屏设计校准

以本页原稿和固定版 [book-library.css](../../launcher/app/components/launcher/book-library.css) 为依据，恢复桌面完整卡片和右上角收藏。封面下的作品名、编号和英文副标题使用同一背景、边框和点击区域。卡片文字区高度由两行文字、5 px 行距与 12 px 内边距计算，网格行距不随封面大小额外放大。

桌面内容水平边距沿用 `clamp(20px, 3.9vw, 56px)`，封面列距 16 px、行距 20 px。手机内容边距 16 px、封面列距 10 px、行距 16 px。章节标题使用桌面 20 px、手机 17 px。最近游玩仍使用 230 px 桌面项目、56 px 封面和原来的 6 px 交互内边距。标题与较轻的 11 px 编号共用文字基线，英文副标题独立成行。

| 同状态实渲染 | 整改前 | 整改后 |
| --- | --- | --- |
| 1440 × 734，暗色、英语、三条最近记录 | [首屏](evidence/flutter/desktop-shelf-layout-before.png) | [首屏](evidence/flutter/desktop-shelf-layout-after.png) |
| 320 × 734，同一语言与记录 | [首屏](evidence/flutter/narrow-shelf-layout-before.png) | [首屏](evidence/flutter/narrow-shelf-layout-after.png) |

控件回归覆盖 320、390、1024 和 1440 px。核对搜索、视图与排序高度，英文标签单行，方形封面，收藏命中，卡片文字进入详情及返回。普通、减少动画和大字号沿用原操作链路。桌面亮暗侧栏检查单机与联机的相同列距，收藏和圈选分别操作，滚动互不牵动。图像为 Windows 上的 Flutter 控件渲染。

## 放大字体下的书架文字

使用发布目录与同一组三条最近记录，在 320、390、781 和 1440 px 验证简中、英文、日文、亮暗以及 100%／200% 字体，共 48 个场景。原实现的 24 个放大字体场景存在手机文字越过封面或卡片底边、桌面最近记录副标题越界的问题。手机文字区和最近记录行改为按实际字体测量高度，封面仍为方形；常规字号的 24 组卡片坐标不变，其中 12 组手机整屏像素一致。

共用作品标题把编号设为独立尾项，长标题省略时仍显示完整编号。侧栏、最近游玩与桌面卡片共用该规则，并按文字基线对齐。保留原有收藏位置、分类横滑和导航结构。

| 200% 英文字体 | 整改前 | 整改后 |
| --- | --- | --- |
| 320 × 734 | [手机](evidence/flutter/mobile-book-caption-before.png) | [手机](evidence/flutter/mobile-book-caption-after.png) |
| 1440 × 734 | [桌面](evidence/flutter/desktop-book-caption-before.png) | [桌面](evidence/flutter/desktop-book-caption-after.png) |

## 桌面窗口

当前 Windows 发布包已通过真实指针双击确认最大化与还原。另从右边缘拖窄到 390 px，在 th08 详情点击最大化跨回桌面，详情与侧栏选择保持正确，返回书架可用。标题栏拖动与 Snap 菜单继续单独验收。窗口可能被其他应用遮挡，实拍判读前先将目标窗口置前。[包与操作核对记录](../archive/delivery-audit-20261128/audit.json) | [窗口测量](../archive/performance-audit-20261128/window-frames.jsonl)

窗口行为使用 [window_manager 0.7](https://pub.dev/packages/window_manager)。顶栏的三项账户操作共用 Prism 的小号连续圆角表面，普通与紧凑窗口栏共用库提供的 WindowCaptionButton 和 MaximizeButtonArea，沿用原生窗口操作与绘制。已登录头像为 28 px，游客入口沿用同组的线性人物图标。站点入口和两项主导航位置沿用原布局，窄窗口可收起导航文字并保留提示。

独立页面在自己的路由内承载窗口栏，根导航视口不随进入、退出该页而改变。书架和消息中心由共用框架持有桌面顶栏，账号与个人主页使用应用内生产站网页，子页转场作用于顶栏下方的内容。Windows 通过显式 AppUserModelID 声明应用身份。[Microsoft 窗口分组约定](https://learn.microsoft.com/en-us/windows/win32/api/shobjidl_core/nf-shobjidl_core-setcurrentprocessexplicitappusermodelid)

`integration_test/desktop_window_test.dart` 从实际 main 入口打开 Windows 窗口，点击最小化、最大化和还原，拦截并核对关闭请求，再检查独立页导航与 1024、801、390、320 px 窗口。另在 1280、390、320 px 窗口实际点开 th06 封面并返回，核对封面与通关区边线和窗口按钮。这里的宽度为请求的窗口外框，内容绘制范围扣除了系统边框。桌面图像从实际 Flutter 引擎的页面绘制边界导出。[桌面窗口](evidence/windows/desktop-caption.png) | [窄桌面](evidence/windows/desktop-caption-801.png) | [独立页面](evidence/windows/desktop-caption-login.png) | [桌面详情](evidence/windows/detail-layout-1280.png) | [竖屏详情](evidence/windows/detail-layout-390.png) | [窄竖屏](evidence/windows/detail-layout-320.png)

账号容器由 `integration_test/account_window_test.dart` 独立回归。共用 `DesktopWindowPage` 固定页面父层级，只切换窗口栏；780 px 断点不再销毁页面。Windows Release 集成验证同一 State、同一网页纹理及两条退出路径的 th06 标题坐标。[回归范围](../archive/performance-audit-20261128/account-return-summary.json)

窗口按钮提供简中、英文和日文名称，最大化后随原生事件更新为还原。紧凑窗口的内容导航有独立语义边界，避免其 BlockSemantics 连带移除上方窗口栏。实际 Windows 集成覆盖 1280 与 320 px 三语、打开抽屉、最小化、最大化／还原和关闭取消。7 张同状态原生引擎图像与修改前字节一致，210 张核心控件截图也一致。Release 已由代理打开并以 Alt+F4 关闭。后续实测的 Flutter 控件树中，最小化、最大化、关闭与登录均有正确按钮名称。操作工具另出现控件树与截图目标不一致及控件索引失效，Release 鼠标入口仍待可靠复核。[Release 名称核对](evidence/windows/release-caption-accessibility.json)。[语义边界约定](https://api.flutter.dev/flutter/widgets/BlockSemantics-class.html) | [验证记录](evidence/windows/caption-accessibility.json)

| 同状态原生引擎 | 调整前 | 调整后 |
| --- | --- | --- |
| 桌面窗口栏 | [截图](evidence/windows/caption-accessibility-before-desktop-caption.png) | [截图](evidence/windows/caption-accessibility-after-desktop-caption.png) |
| 320 px 紧凑窗口栏 | [截图](evidence/windows/caption-accessibility-before-desktop-caption-320.png) | [截图](evidence/windows/caption-accessibility-after-desktop-caption-320.png) |

## 作品入口语义

封面和最近游玩沿用原有 InkWell 的点击、焦点和动效，补充按钮角色。共用作品图片排除装饰性图像语义，名称与编号由入口本身提供。320、390、1440 px 的六种样例核对读屏激活、回车进入、返回和独立收藏，既有排序与最近记录回归合计 71 项通过。Windows Release 的系统无障碍树另核对作品角色与实际打开。210 张核心控件截图重拍后像素一致，视觉设计仍按既有验收边界处理。[验证记录](evidence/windows/library-accessibility.json) | [Windows 书架](evidence/windows/library-accessibility-release-library.png) | [Windows 详情](evidence/windows/library-accessibility-release-detail.png)

## 键盘排序

排序按钮可用 Enter 或上下方向键打开，初始焦点落在当前选项。上下方向键浏览，Enter 提交，Esc 合上菜单并回到排序按钮。1440 px 用发布作品目录验证普通动效和站内减少动画两种设置。 [Flutter MenuAnchor](https://api.flutter.dev/flutter/material/MenuAnchor-class.html)

网页与 Flutter 均用 11 部发布作品。键盘提交编号倒序，再按键打开，截图核对选中行和可见焦点。 [网页](evidence/web/desktop-library-sort-keyboard.png) | [Flutter](evidence/flutter/desktop-library-sort-keyboard.png)

## 逐帧回归

- 手机的顶栏随书架页进入和退出。三个根页共用带底部导航的 Scaffold，二级页用全屏空间。底部导航在根页切换中位置固定，进入和返回详情时随根页一起过渡。
- 单机和联机的侧栏使用同一组件，各自和内容独立滚动。切换期间保留原页面尺寸。桌面页面采用 Prism／FlClash 的 200 ms 连续淡化：底页维持可见，新页随路由淡入或淡出。iOS 各宽度沿用 Cupertino 的平台过渡，其余平台沿用 Flutter 的平台过渡。系统减少动画和启动器设置均作用于这些切换。 [Prism 桌面转场](https://github.com/chen08209/prism_ui/blob/cae9a43572bddb9b20b9350ef7fde3f434a758c7/lib/navigator.dart) | [Flutter 导航](https://docs.flutter.dev/ui/navigation) | [分支容器](https://pub.dev/documentation/go_router/latest/go_router/StatefulShellRoute-class.html) | [Chrome 层级导航指南](https://developer.chrome.com/docs/modern-web-guidance/guides/ui-components/stack-drill-down)
- 验收使用发布作品目录，覆盖 320 和 390 px、模拟系统安全区、1100 px 桌面、普通和减少动画。记录切换帧中的底栏矩形、书架视口与侧栏矩形，并验证排序外的纵横滚动、详情替换、系统返回，以及从个人主页回原大厅。
- [打开详情中间帧](evidence/flutter/motion-open-game.png) | [进入设置中间帧](evidence/flutter/motion-open-settings.png) | [回书架中间帧](evidence/flutter/motion-back-to-library.png)
- [桌面逐帧记录](evidence/flutter/navigation-desktop-normal.json) | [手机默认](evidence/flutter/navigation-mobile-normal.json) | [站内减少动画](evidence/flutter/navigation-mobile-app-reduced.json) | [系统减少动画](evidence/flutter/navigation-mobile-system-reduced.json)
- 781 px 和 1440 px 从书架打开 th07 后返回，逐 16 ms 记录两页的不透明度与侧栏位置。旧共享轴过渡在 48 ms 时，详情只剩 9.2%，书架仍为 0%；连续淡化后，整个采样期间至少有一页完整覆盖视口，80 ms 时详情仍为 60%。静止页的大小与滚动位置不变。[64 ms 调整前](evidence/flutter/desktop-detail-fade-before.png) | [调整后](evidence/flutter/desktop-detail-fade-after.png) | [记录](evidence/flutter/desktop-detail-fade.json)。这些是控件渲染和测试时钟的结果，设备帧率另验。
- `NAV-07` 用发布目录在 390 × 720 依次点单机、联机、主页、单机，在 1100 × 720 快速往返单机与联机。分别在 64 ms 和 32 ms 中断两段淡入，点击前与点击后的内容区像素相同，导航位置和书架的 110 px 滚动位置保持稳定。深色无背景、亮色星点背景均通过窄屏检查，验证旧页面停止响应点击和焦点，同时从读屏树退出。 [中断前](evidence/flutter/rapid-tabs-source.png) | [旧版中断](evidence/flutter/rapid-tabs-old-interruption.png) | [修复后](evidence/flutter/rapid-tabs-interruption.png) | [星点背景](evidence/flutter/rapid-tabs-wallpaper.png) | [桌面](evidence/flutter/rapid-tabs-desktop.png)
- 主分栏与作品侧栏共用 Prism 的 `fadeTabDuration` 和 `fadeTabCurve`，采用 FlClash 的 220 ms 缓入缓出节奏。两次反向点击仍从当前混合画面接续，输入权限、焦点与读屏栏目立即交给新页面。普通与减少动画的采样延长到 224 ms，覆盖完整过渡。[Prism 时序](https://github.com/chen08209/prism_ui/blob/cae9a43572bddb9b20b9350ef7fde3f434a758c7/lib/motion.dart) | [FlClash 分栏](https://github.com/chen08209/FlClash/blob/afe7136002271fd64f098b154e7fff40be721eb8/lib/pages/home.dart)
- `LIB-04` 与 `LIB-06` 使用发布目录中的 th06、th07、th08 和 th10 验证编号倒序。桌面封面、列表、侧栏和 390 px 手机封面逐帧核对位置，收藏图标随同一作品一起移动。复测 320 px 下的启动器减少动画和系统减少动画，排序直接到位。
- 桌面以鼠标从作品行移到收藏图标，检查高亮连续。`flutter_animate` 只缩放收藏星图标，按钮命中范围和作品行的位置固定。倒序后点星，确认收藏写入对应作品。320 px 截图核对「编号倒序」完整显示。切换英文或日文时，封面和列表按钮可收起文字并保留可访问名称，为排序选项留宽。 [Flutter Animate](https://pub.dev/packages/flutter_animate) | [网页动态排序指南](https://developer.chrome.com/docs/modern-web-guidance/guides/ui-behaviors/dynamic-sort)
- `LIB-03` 用同一组七部作品的本机历史采集[网页与 Flutter 截图](comparison.md)。覆盖 320、390、1440 px。以实际按钮验证成功回执、持久化、日期联动、从横滑后的封面进入详情并返回。占位、录像、联机及未启动的分支保持空历史。
- 记录来自 Flutter 测试渲染。Android 真机手势、系统链接接管、键盘滚动和设备帧率待验。

## iOS 侧滑返回

返回手势由 Flutter 的 Cupertino 转场承接。减少动画时保留手势层，移除大幅平移并随拖动淡化当前页；松手后仍由平台控制器决定完成或取消。平板宽度也使用同一实现。[Cupertino 转场](https://api.flutter.dev/flutter/material/CupertinoPageTransitionsBuilder-class.html)

每个页面有自己的背景和前景反馈层，避免返回途中从表格间隙透出书架。壁纸与预览共用绘制组件，页面只显示对应窗口区域，转场不改变壁纸的铺放比例。点击水波绘制在背景上方。

Windows 控件渲染以 iOS 平台行为覆盖运行 390 × 844 和 834 × 844 两种视口，使用发布目录中的 th08、隔离账户与空闲大厅。普通动效、站内减少动画、系统减少动画各验证空白和星点背景，共 12 种场景。通过封面进入详情，再逐层进入设置及个人主页，分别拖动取消和完成返回，最后核对书架滚动及从个人主页回原大厅。

旧实现的六种手势场景中五种失败：减少动画省略了手势层，平板又被桌面转场覆盖。另用拖动前后的像素和独立壁纸层验证透底问题，修复前能复现，修复后通过。像素检查允许普通平移时最多 2 个色阶的栅格取整差异。

| 状态 | 修复前 | 修复后 |
| --- | --- | --- |
| 手机空白背景，详情返回途中 | [截图](evidence/flutter/ios-back-before-plain.png) | [截图](evidence/flutter/ios-back-after-plain.png) |
| 手机星点背景，同一拖动位置 | [截图](evidence/flutter/ios-back-before-dots.png) | [截图](evidence/flutter/ios-back-after-dots.png) |
| 平板侧栏与详情 | 手势检查失败 | [途中截图](evidence/flutter/ios-back-tablet.png) |
| 手机减少动画 | 手势检查失败 | [淡化反馈](evidence/flutter/ios-back-reduced.png) |

[手机普通坐标](evidence/flutter/ios-back-phone-normal.json) | [站内减少动画](evidence/flutter/ios-back-phone-app-reduced.json) | [系统减少动画](evidence/flutter/ios-back-phone-system-reduced.json) | [平板普通坐标](evidence/flutter/ios-back-tablet-normal.json) | [星点背景坐标](evidence/flutter/ios-back-phone-dots.json)

取消返回的 36 组整页截图与各自起始图完全一致。[回书架后的画面](evidence/flutter/ios-back-returned-library.png)保留原滚动。根页底部导航已归入根页 Scaffold，由外层 ShellRoute 承接详情和设置，桌面侧栏留在该页面栈之外。390 px 的返回松手帧中，导航已在 y = 778、高 66 px 的目标位置，与书架一起露出。[原先的松手帧](evidence/flutter/ios-back-navigation-before.png) | [调整后的松手帧](evidence/flutter/ios-back-navigation-after.png)。这些检查不代表整套界面已通过视觉验收。

重新采集时设置 `TOUHOU_IOS_BACK_CAPTURE` 为任务临时目录，`TOUHOU_NAV_TRACE` 使用同一目录，再运行 `flutter test test/launcher_navigation_test.dart --plain-name "iOS edge-back" --concurrency=1`。截图用 Microsoft YaHei 代替 Apple 字体，仅用于动效和遮挡对照。采样时间来自测试时钟，包含人为停留的手势时间。iOS 设备或模拟器、Apple 字体、真实帧率、自定义图片背景及覆盖页面时即时改变减少动画偏好仍待验证。

## 分组动效

Prism 固定在 FlClash 使用的 `cae9a43572bddb9b20b9350ef7fde3f434a758c7`。侧栏复用其 `UnfoldMotion` 弹簧与箭头进度，分类行显式配置 38 px 最小高度和紧凑间距。作品行沿用文字、封面和上下内边距计算高度。主页与游戏设置继续使用 Material ExpansionTile，共用相同弹簧参数生成的曲线。[折叠实现](https://github.com/chen08209/prism_ui/blob/cae9a43572bddb9b20b9350ef7fde3f434a758c7/lib/collapsible_section.dart)

Windows 控件回归使用发布目录中的 11 部作品，分别从单机和联机侧栏点击展开箭头。记录第一帧、中间帧、收束与反向位置，同时核对标题、侧栏视口和滚动值。320 和 1440 px 主页设置验证展开及途中反向。站内和系统减少动画均直接呈现目标状态。

每一张侧栏采样帧都与开始帧比较右侧内容像素，单机与联机的内容哈希各自保持一致。设置标题关闭默认扩散水波纹，避免灰色圆形盖住正在展开的字段；标题的选中、焦点和按压高亮继续由 Material 提供。[原反馈](evidence/flutter/unfold-settings-press-before.png) | [调整后](evidence/flutter/unfold-settings-press-after.png)

[开始收起](evidence/flutter/unfold-library-first.png) | [收起途中](evidence/flutter/unfold-library-middle.png) | [完全收起](evidence/flutter/unfold-library-closed.png) | [途中反向](evidence/flutter/unfold-library-reverse.png) | [减少动画](evidence/flutter/unfold-library-reduced.png)

[单机坐标](evidence/flutter/unfold-library-normal.json) | [联机坐标](evidence/flutter/unfold-lobby-normal.json) | [主页设置](evidence/flutter/unfold-home-normal.json) | [减少动画设置](evidence/flutter/unfold-home-reduced.json)

帧图使用同一套预加载封面和 Windows 字体。侧栏以物理弹簧接续速度，设置组使用 Flutter ExpansionTile 的曲线过渡，两者的中断实现不同。本轮验证位置连续性，设备实际帧率与触屏手势另行验收。
