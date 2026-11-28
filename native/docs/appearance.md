# 主题与背景

网页基准为 `85941f0d56`。Pencil 的[手机主页稿](evidence/pencil/mobile-home-expanded.png)给出了主题分组的位置和行级关系。背景面板的控件与状态来自[发布版源码](../../launcher/app/components/launcher/CustomBackground.tsx)与操作截图。

| 编号 | 控件和状态 | 布局与行为 |
| --- | --- | --- |
| APPEAR-01 | 主题入口和返回 | 桌面顶栏及手机抽屉打开主题设置。主页主题组先显示基底选择，背景行打开精简版。关闭、桌面 Esc 与手机系统返回回到触发页面。 |
| APPEAR-02 | 明暗基底 | 桌面和手机共用等宽的分段控件。系统、亮色、暗色即时可选，并写入启动器偏好。弹层背景、标题和内容一起更新，位置不变。主页精简版沿用外层的基底选项。 |
| APPEAR-03 | 背景预设 | 空白、星点、方格、青海波使用本主题预览。有本机图片时增加自定义项。320 px 和 390 px 的四个内置预设同行。导入图片后按可用宽度调整列数。选中态用强调底色，读屏可读取选中状态。 |
| APPEAR-04 | 本机图片 | 使用系统选择器。读取上限 10 MiB，写入前解码验证。替换成功后清理之前的文件。取消或损坏输入保留之前的背景。移除当前图片后回到空白。 |
| APPEAR-05 | 铺放与遮罩 | 自定义图片提供铺满和完整显示。预设和图片共用背景遮罩，设置时可以观察底层页面。 |
| APPEAR-06 | 容器透明度 | 顶栏、作品分类、封面标题与内容面板使用独立比例。水平调节立即反映背景。四种表面默认 88%，背景遮罩默认 45%。空白预设采用主题的默认表面。 |
| APPEAR-07 | 窄屏和动效 | 桌面居中，手机从底部打开，内容在面板内滚动。两端有固定标题及关闭入口，减少动画设置同时作用于两种弹层。 |
| APPEAR-08 | 控件边界颜色 | Material 与 Prism 共用中性灰 `outline`，亮暗分别取固定版网页的 `--kit-ring` 值 `#8b8b9c` 与 `#a8a9b4`。较弱的面板边界继续使用 `outlineVariant`。聚焦沿用主色，校验错误沿用错误色，禁用状态由组件库提供。 |

使用 Wolt Modal Sheet 处理对话框和底部面板。纹样预览和背景共用绘制方式，设置项沿用 Prism UI 的交互表面和滑条。 [Wolt Modal Sheet](https://pub.dev/packages/wolt_modal_sheet) | [Prism UI](https://pub.dev/packages/prism_ui) | [File Picker](https://pub.dev/packages/file_picker)

## 共用控件形状

原配色只同步了较弱的边框色，普通输入框仍使用粉色种子生成的暖灰边线。公共配色入口现已同步两套组件的 `outline`。三种宽度、三语与亮暗主题从实际登录入口验证普通、聚焦、校验失败和提交中四种状态，输入框矩形以及主色、错误色与调整前一致。中性边线与页面底色的对比度均至少为 3:1。[Material 边界颜色角色](https://api.flutter.dev/flutter/material/ColorScheme/outline.html)

390 px 日语亮色输入：[调整前](evidence/flutter/mobile-light-form-outline-before.png) | [调整后](evidence/flutter/mobile-light-form-outline-after.png)。桌面截图先加载既有账户背景插画，未以空白背景代替实际页面。

主页折叠组的内容使用 14 px 正文字号，组标题沿用原有标题样式。语言、减少动画、主题基底、背景及账户入口之间的分隔线从内容轴开始，与标签左侧一致。 [调整前](evidence/flutter/mobile-home-rows-before.png) | [当前调整](evidence/flutter/mobile-home-rows-after.png)

Material 的实心、描边和文字按钮共用 `LauncherControl.geometry`：44 px 最小高度、16 px 图标、8 px Prism 圆角和 10 px 水平内边距。各操作的颜色和状态仍由对应按钮角色提供。普通对话框共用 Prism 的 16 px 圆角。输入编辑器采用稳定的 608 px 框架，受视口边距约束；键盘、手柄及配置切换不改变窗口矩形，长内容在内部滚动。

房间的席位、旁观和聊天面板使用 Prism Card 默认的 16 px 圆角，移除原来单独指定的 20 px 大圆角。三块面板沿用同一边线与 16 px 间距。[调整前](evidence/flutter/mobile-room-panels-before.png) | [调整后](evidence/flutter/mobile-room-panels-after.png)

| 相同视口与状态 | 调整前 | 当前调整 |
| --- | --- | --- |
| 390 px 登录 | [截图](evidence/flutter/mobile-login-controls-before.png) | [截图](evidence/flutter/mobile-login-controls-after.png) |
| 320 px 注册 | [截图](evidence/flutter/mobile-narrow-register-controls-before.png) | [截图](evidence/flutter/mobile-narrow-register-controls-after.png) |
| 390 px 键盘设置 | [截图](evidence/flutter/mobile-game-keyboard-controls-before.png) | [截图](evidence/flutter/mobile-game-keyboard-controls-after.png) |
| 390 px 手柄设置 | [截图](evidence/flutter/mobile-game-gamepad-controls-before.png) | [截图](evidence/flutter/mobile-game-gamepad-controls-after.png) |

这组截图来自显式 Windows 平台的同入口渲染，反映公共控件的形状和尺寸调整。输入编辑器仍为较少内容的手柄状态留出同一空间，以免切换时窗口跳动。截图尚未作为用户认可基准，页面整体布局仍按验收索引复核。

## 平台字体

Material 与 Prism 共用 `launcherFonts`，通过 Prism 的 `systemFontOf` 和 `appFontFamilyFallback` 选择平台字体与当前语言的后备字形。Windows 使用 Segoe UI，简中文字形优先 Microsoft YaHei UI，日文按平台后备序列选择。取消原来全平台优先 Noto Sans SC 的设置。[Prism 字体实现](https://github.com/chen08209/prism_ui/blob/cae9a43572bddb9b20b9350ef7fde3f434a758c7/lib/font.dart)

昵称和其他用户内容可以使用不同于界面的文字。共用入口先按当前语言排列后备字体，再通过同一 Prism 接口补入应用三语的系统字体，去重后供两套控件使用。日语界面中的简体昵称曾在控件渲染里缺字；三语、常规及粗体分别与系统微软雅黑参考逐像素核对。字形图左侧是应用，右侧是独立参考。 [修正前](evidence/flutter/glyphs-ja-400-before.png) | [修正后](evidence/flutter/glyphs-ja-400-after.png)

控件测试引擎会把未注册的字体替成 Ahem 方块。校验在本机缺少 Meiryo 时，将测试中的该候选注册为已经优先使用的 Segoe UI，因而不增加可选字形，模拟跳过缺少字体。正式应用没有此别名。相同校验环境下，旧代码仍有两个日语字形失败，新代码六项通过。

Windows 控件验证显式指定 Windows 平台，并加载对应系统字体。320、390、1024、1440 px 书架及 150%、200% 三语字号使用真实字形检查。macOS、iOS、Android 与 Linux 的字体选择继续由 Prism 的平台约定承接，设备显示另行验收。

## 竖屏视觉验收

- Android 现行布局指南以 16 dp 为紧凑宽度的常用内容边距，并要求同类元素采用一致的间距与边线。本应用的书架与主页使用 16 个 Flutter 逻辑像素。主题面板的基底和预设左对齐，主页的分组标题、主题基底与背景入口实测均从 x = 64 开始。 [当前布局准则](https://developer.android.com/design/ui/mobile/guides/layout-and-content/content-structure) | [Material 3 自适应布局](https://developer.android.com/codelabs/adaptive-material-guidance)
- 图形和点击区分别验收。现行无障碍指南建议触控目标至少为 48 × 48 dp。本应用的顶栏操作行高 48 逻辑 px，搜索底形高 34 逻辑 px。320 px 中英文及 390 px 中文的实拍中，四个图标的可见中心均为 y = 24。 [触控目标](https://developer.android.com/guide/topics/ui/accessibility/apps#touch-targets) | [像素测量](evidence/flutter/portrait-visible-glyphs.json)
- 按作用分配空间。三种主题基底用等宽分段，主题预设用一致的缩略图和按压反馈。中英文窄屏均保留完整选择。书架编号跟随作品名，宽度不足时单独下一行。 [Apple 分段控件](https://developer.apple.com/design/human-interface-guidelines/segmented-controls) | [FlClash 表面层级](https://github.com/chen08209/FlClash/blob/main/.agents/surfaces.md)

| 本机画面 | 书架 | 主页 | 主题面板 |
| --- | --- | --- | --- |
| 390 px 中文 | [截图](evidence/flutter/mobile-polish-library.png) | [截图](evidence/flutter/mobile-polish-home.png) | [截图](evidence/flutter/mobile-background-settings.png) |
| 320 px 中文 | [截图](evidence/flutter/mobile-narrow-polish-library.png) | [截图](evidence/flutter/mobile-narrow-polish-home.png) | [截图](evidence/flutter/mobile-narrow-background-settings.png) |
| 320 px 英文 | [截图](evidence/flutter/mobile-narrow-en-polish-library.png) | [截图](evidence/flutter/mobile-narrow-en-polish-home.png) | [截图](evidence/flutter/mobile-narrow-en-background-settings.png) |

## 同状态对照

两端使用暗色基底、方格背景和同一组默认透明度。手机网页从抽屉打开，Flutter 打开面板时先关闭抽屉。

| 视口 | 网页 | Flutter |
| --- | --- | --- |
| 1440 × 734 | [主题设置](evidence/web/desktop-background-settings.png) | [主题设置](evidence/flutter/desktop-background-settings.png) |
| 390 × 844 | [主题设置](evidence/web/mobile-background-settings.png) | [主题设置](evidence/flutter/mobile-background-settings.png) |
| 320 × 640 | [主题设置](evidence/web/mobile-narrow-background-settings.png) | [主题设置](evidence/flutter/mobile-narrow-background-settings.png) |

三个视口从书架的实际入口打开，验证方格、滑条、离开后书架矩形和重新读取偏好。320 px 另经主页进入背景面板并用系统返回。测试用仓库内现有 WebP 样例验证复制、重启读取、坏文件拒绝、替换和删除。手机排布在本机以竖屏逻辑视口验证。

## 弹层内切换主题

Windows 走查发现，弹层把打开时的背景和标题颜色缓存到页面配置里，内容随后更新，造成白底白字。改用 Wolt 的动态主题背景，并在标题自身读取当前主题。沿用原弹层和内容实例。

回归从书架打开主题设置，连续切换亮色、暗色，检查实际绘制表面的亮度、标题对比度至少 4.5:1，以及基底控件位置。覆盖桌面、390 px、320 px 中文及 320 px 英文。

| 视口 | 亮色 | 暗色 |
| --- | --- | --- |
| 1440 × 734 | [截图](evidence/flutter/desktop-theme-switch-light.png) | [截图](evidence/flutter/desktop-theme-switch-dark.png) |
| 390 × 844 | [截图](evidence/flutter/mobile-theme-switch-light.png) | [截图](evidence/flutter/mobile-theme-switch-dark.png) |
| 320 × 640 | [截图](evidence/flutter/mobile-narrow-theme-switch-light.png) | [截图](evidence/flutter/mobile-narrow-theme-switch-dark.png) |

Windows Release 从顶栏实测亮色转暗色再转回亮色。[修复前](evidence/windows/theme-switch-before.png) | [暗色](evidence/windows/theme-switch-dark.jpg) | [亮色](evidence/windows/theme-switch-light.jpg)

## 大字号与换行

按 Flutter 的[字号验收建议](https://docs.flutter.dev/release/breaking-changes/android-14-nonlinear-text-scaling-migration)检查 200% 文本。以固定版 11 部作品和隔离账号接口走实际导航入口，在本机渲染 320 px 英语、390 px 简中和日语、1440 px 英语，分别使用 150% 和 200% 线性文本倍率。Android 系统的非线性字号和设备读屏另待验收。

| 编号 | 排版与行为 | 验证 |
| --- | --- | --- |
| A11Y-04.1 | 设置标签和选择器共用自适应行。大字号下同排空间不足时，上下排列，文字、控件与分隔线沿用原内容边距。 | 语言、游戏设置和 MIDI 设备共用此行。320 px 英语原来把 Interface 拆成两行；回归读取实际字形边界，确认 Interface 与 language 各自完整。 |
| A11Y-04.2 | 共用作品标题在空间不足时把编号移到下一行。大字号详情必要时把封面与文字上下排列。 | 原来 320 px 的五字标题分成三行，调整后完整显示。详情及设置仍用同一个标题组件。检查设置返回详情，再回原书架。 |
| A11Y-04.3 | 窄屏大字号的大厅连接提示先排图标与重试，再以完整内容宽度显示标题和说明。 | 英语 unavailable 不拆词，重试入口可点击。根页底部导航始终在视口内。 |
| A11Y-04.4 | 原尺寸的视图、排序、启动与返回继续从原入口使用。 | 两种倍率均完成排序、打开作品、修改语言、返回；手机继续进入主页、主题和大厅。普通字号用原有三语及侧栏回归检查。 |
| A11Y-04.5 | 大字号主题改用共用选择器。下拉使用根浮层，显示在固定底栏上方。 | 连续选择跟随系统、亮色、暗色，选项文字完整，靠近底栏的暗色项仍能点按。采用 Flutter [MenuAnchor 的根浮层选项](https://api.flutter.dev/flutter/material/MenuAnchor/useRootOverlay.html)。 |

| 320 × 640、200% | 调整前 | 调整后 |
| --- | --- | --- |
| 主页语言设置 | [截图](evidence/flutter/text-scale-before-home.png) | [截图](evidence/flutter/text-scale-home.png) |
| 游戏详情 | [截图](evidence/flutter/text-scale-before-detail.png) | [截图](evidence/flutter/text-scale-detail.png) |
| 游戏语言 | 原标签和选择器相互挤压 | [截图](evidence/flutter/text-scale-language.png) |
| 大厅连接提示 | 先前检查发现断词 | [截图](evidence/flutter/text-scale-lobby.png) |
| 底栏附近的菜单 | [底栏挡住暗色项](evidence/flutter/text-scale-before-menu.png) | [暗色项完整并可点按](evidence/flutter/text-scale-menu.png) |

[390 px 简中详情](evidence/flutter/text-scale-mobile-zh-detail.png) | [390 px 日语主页](evidence/flutter/text-scale-mobile-ja-home.png) | [1440 px 英语设置](evidence/flutter/text-scale-desktop-settings.png)

截图采用实际字体，不用 Ahem 方块字代替英文换行检查。底部导航沿用 Flutter [NavigationBar 的字号行为](https://api.flutter.dev/flutter/material/NavigationBar-class.html)，其尺寸和目的地标签不会按页面倍率放大，长按提示随字号缩放。上述记录验证布局和返回，不代替真机触控与读屏。
