# 已归档 | 不再维护

以下为改用网页前的历史实现记录，不属于当前原生验收范围。

# 站点与帮助

站点文档以发布基准的原文为准。Flutter 使用本地文档和原生导航。外部项目、社区、兼容性工具及邮件链接交给系统处理。

| 编号 | 页面或状态 | 布局、行为和来源 |
| --- | --- | --- |
| CONTENT-01 | 站点入口 | 桌面左侧工具区提供常见问题、支持和站点菜单。手机只在单机书架顶栏显示抽屉入口。抽屉包含共用账户卡、帮助、站点和设置，过长时自身滚动。由书架搜索进入资料，返回后恢复搜索。 |
| CONTENT-02 | 常见问题与作品适配进度 | FAQ 使用仓库原文、标题、链接和表格。目录跳到对应段落。作品进度从 FAQ 链接进入。宽表单独横向滚动，周边正文按屏宽折行。网页版兼容性指引使用系统浏览器。 |
| CONTENT-03 | 联机玩法介绍 | 联机大厅和站点菜单均可进入，打开顺序是介绍、协作通用规则或作品子页。桌面左栏按照指南顺序排列，手机从顶部切换。通用规则包含机制表、页内链接、说明和对应作品特例。 |
| CONTENT-04 | 作品联机指南 | 协作包含 th06、th07、th08、th10、th11、th15，对战包含 th09、th19。沿用各作说明。th06 使用基准站的无专属规则提示。返回先回介绍，再回原入口。 |
| CONTENT-05 | 条款、隐私与首次使用须知 | 站点为独立阅读页。登录和注册仍使用原流程的原生阅读弹窗，同意后回原表单。阅读页与弹窗使用同一份条款内容与排版。首次使用须知从菜单手动打开。 |
| CONTENT-06 | 关于与支持 | 关于页提供原文中的贡献、GitHub Issues、邮件、Discord 和 QQ 链接。支持我们打开原站赞赏码面板，图像有语义标签，关闭后回现有页面。 |
| CONTENT-07 | 服务状态 | 读取正式站公开的 24 小时、7 天及 90 天数据。四组服务共用一个连续表格。桌面按状态、服务名、历史、响应和可用率对齐，手机将历史移到下一行。历史区支持点按、悬停，以及键盘按钮逐条读取。 |
| CONTENT-08 | 状态更新与失败 | 可手动刷新，页面可见时每 30 秒刷新，回到前台时重读。时段切换忽略已过期响应。刷新失败保留已读历史，实时状态显示未知，可重试。没有数据时有单独的读取和空态。 |
| CONTENT-09 | 阅读语言与文内跳转 | 帮助、关于、首次使用须知和各作联机指南使用发布基准的简体中文、英语和日语正文。带语言参数的链接只影响当前文档，并传递给后续阅读页。目录和正文锚点共用基准编号。提示和警告使用可由读屏区分的标识，保留原文与链接。作品适配进度按原站提供简体中文正文。 |
| CONTENT-10 | 文章末尾 | 修改日期和作者来自发布基准的 Git 记录，作者链接指向对应提交。相邻阅读页沿用原站顺序。按返回回到实际来源页和滚动位置。 |
| CONTENT-11 | 服务状态语言 | 标题、时间段、服务分组、当前状态、历史采样和刷新反馈使用三语文案。服务名称来自公开接口。通过菜单进入时沿用启动器语言。链接中的语言只作用于当前页面。桌面范围控件最多 360 px，窄屏保留三个选项。 |

## 对照与边界

抽屉按固定网页与 Pencil 恢复为最多 292 px，窄屏在右侧留出 32 px。外沿方正，内部分组使用 10 px 细边框，标题与菜单文字分别沿 16 px 和 64 px 左边线排列。共用账户组件沿用其 54 px 头像，原网页为 52 px。「关于」子页容纳站点信息、GitHub、条款、隐私与首次使用须知，系统返回先退出子页，再关闭抽屉。320 px 英文、390 px 日文与中文的亮暗、游客与成员、100% 与 200% 字体共 24 个场景验证滚动和返回。

- 正式站的发布清单确认桌面和手机样本均来自固定基准。抽屉对照 Pencil。FAQ、关于、协作规则、状态及支持面板以网页实拍为参考。 [截图对照](../../../docs/comparison.md)
- Flutter 用原生抽屉和可返回的阅读页承载入口。桌面目录使用左栏和顶部目录按钮，手机使用顶部目录按钮。作品进度里的状态徽章使用原文状态文字，缩略图位置使用作品名称。
- 公开站点清单未启用旧站迁移，因此基准实例没有入口可复测。旧站浏览器数据迁移待验证。站点页脚组件在基准源码存在，但已发布的主界面未挂载。
- 验收读取正式站三组时段的真实响应。隔离场景覆盖快速切换、迟到响应、网络错误、重试和空列表。Flutter 的设备验证见验收总表。

[抽屉整改前](../../../docs/evidence/flutter/drawer-layout-before.png) | [整改后](../../../docs/evidence/flutter/drawer-layout-after.png)。图示为 390 px 中文游客深色状态；截图回归仍以用户认可的设计为准。

## 服务状态三语对照

表格沿用固定版网页的单一细边框、8 px 圆角和分组底色。页面最大宽度为 1160 px，桌面左右留 24 px，手机留 16 px。桌面表头与每行共用列宽，指标按等宽数字右对齐。列宽以当前字体测量，空间不足时改用手机布局，大字号的指标可再换一行。历史条等宽，间隔随采样数量收缩，90 天记录也逐条显示。

范围选择复用启动器的 44 px 分段控件，宽度最多 360 px。完整文案放不下时显示 `24h`、`7d` 和 `90d`，提示和读屏仍提供完整时段。历史按钮也保留 44 px 命中区，其读屏名称包含服务名。窗口跨宽度时保留正在查看的采样，返回宽屏后恢复相同边界。页面延续原生返回标题栏和启动器的中性色阶；网页使用全站顶栏，手机历史区也没有客户端的键盘查看按钮。

下表为整改前直接读取正式站的在线实拍，测量值随采样变化。英日版从启动器菜单进入，在三种宽度验证时间段、历史采样、刷新失败和手动重试。320 px 另从带语言参数的链接进入，再返回原界面。现有 Pencil 稿覆盖菜单入口。

| 语言 | 1440 × 734 | 390 × 844 | 320 × 640 |
| --- | --- | --- | --- |
| 简体中文 | [网页](../../../docs/evidence/web/desktop-site-uptime.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-uptime.png) | [网页](../../../docs/evidence/web/mobile-site-uptime.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-uptime.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-uptime.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-uptime.png) |
| 英语 | [网页](../../../docs/evidence/web/desktop-site-uptime-en.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-uptime-en.png) | [网页](../../../docs/evidence/web/mobile-site-uptime-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-uptime-en.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-uptime-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-uptime-en.png) |
| 日语 | [网页](../../../docs/evidence/web/desktop-site-uptime-ja.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-uptime-ja.png) | [网页](../../../docs/evidence/web/mobile-site-uptime-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-uptime-ja.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-uptime-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-uptime-ja.png) |

## 文档实拍

| 文档 | 桌面 1440 px | 手机 390 px | 手机 320 px |
| --- | --- | --- | --- |
| 首次使用须知 | [网页](../../../docs/evidence/web/desktop-site-first-use.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-first-use.png) | [网页](../../../docs/evidence/web/mobile-site-first-use.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-first-use.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-first-use.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-first-use.png) |
| 作品适配进度 | [网页](../../../docs/evidence/web/desktop-site-progress.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-progress.png) | [网页](../../../docs/evidence/web/mobile-site-progress.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-progress.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-progress.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-progress.png) |
| 服务条款 | [网页](../../../docs/evidence/web/desktop-site-terms.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-terms.png) | [网页](../../../docs/evidence/web/mobile-site-terms.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-terms.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-terms.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-terms.png) |
| 隐私条款 | [网页](../../../docs/evidence/web/desktop-site-privacy.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-privacy.png) | [网页](../../../docs/evidence/web/mobile-site-privacy.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-privacy.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-privacy.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-privacy.png) |
| 红魔乡 | [网页](../../../docs/evidence/web/desktop-site-th06.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th06.png) | [网页](../../../docs/evidence/web/mobile-site-th06.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th06.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th06.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th06.png) |
| 妖妖梦 | [网页](../../../docs/evidence/web/desktop-site-th07.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th07.png) | [网页](../../../docs/evidence/web/mobile-site-th07.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th07.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th07.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th07.png) |
| 永夜抄 | [网页](../../../docs/evidence/web/desktop-site-th08.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th08.png) | [网页](../../../docs/evidence/web/mobile-site-th08.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th08.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th08.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th08.png) |
| 风神录 | [网页](../../../docs/evidence/web/desktop-site-th10.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th10.png) | [网页](../../../docs/evidence/web/mobile-site-th10.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th10.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th10.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th10.png) |
| 地灵殿 | [网页](../../../docs/evidence/web/desktop-site-th11.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th11.png) | [网页](../../../docs/evidence/web/mobile-site-th11.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th11.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th11.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th11.png) |
| 绀珠传 | [网页](../../../docs/evidence/web/desktop-site-th15.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th15.png) | [网页](../../../docs/evidence/web/mobile-site-th15.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th15.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th15.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th15.png) |
| 花映塚 | [网页](../../../docs/evidence/web/desktop-site-th09.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th09.png) | [网页](../../../docs/evidence/web/mobile-site-th09.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th09.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th09.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th09.png) |
| 兽王园 | [网页](../../../docs/evidence/web/desktop-site-th19.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th19.png) | [网页](../../../docs/evidence/web/mobile-site-th19.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th19.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th19.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th19.png) |

## 英语与日语

两种语言均对比正式站同一页面。窗口宽度为 1440、390、320 逻辑像素。Pencil 未提供这些正文页面的对应稿件，页面状态以网页实拍为准。

### 英语

| 文档 | 桌面 1440 px | 手机 390 px | 手机 320 px |
| --- | --- | --- | --- |
| 常见问题 | [网页](../../../docs/evidence/web/desktop-site-faq-en.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-faq-en.png) | [网页](../../../docs/evidence/web/mobile-site-faq-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-faq-en.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-faq-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-faq-en.png) |
| 关于本站 | [网页](../../../docs/evidence/web/desktop-site-about-en.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-about-en.png) | [网页](../../../docs/evidence/web/mobile-site-about-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-about-en.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-about-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-about-en.png) |
| 首次使用须知 | [网页](../../../docs/evidence/web/desktop-site-first-use-en.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-first-use-en.png) | [网页](../../../docs/evidence/web/mobile-site-first-use-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-first-use-en.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-first-use-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-first-use-en.png) |
| 联机玩法介绍 | [网页](../../../docs/evidence/web/desktop-site-guide-en.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-guide-en.png) | [网页](../../../docs/evidence/web/mobile-site-guide-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-guide-en.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-guide-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-guide-en.png) |
| 协作通用规则 | [网页](../../../docs/evidence/web/desktop-site-guide-coop-en.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-guide-coop-en.png) | [网页](../../../docs/evidence/web/mobile-site-guide-coop-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-guide-coop-en.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-guide-coop-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-guide-coop-en.png) |
| th06 | [网页](../../../docs/evidence/web/desktop-site-th06-en.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th06-en.png) | [网页](../../../docs/evidence/web/mobile-site-th06-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th06-en.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th06-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th06-en.png) |
| th07 | [网页](../../../docs/evidence/web/desktop-site-th07-en.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th07-en.png) | [网页](../../../docs/evidence/web/mobile-site-th07-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th07-en.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th07-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th07-en.png) |
| th08 | [网页](../../../docs/evidence/web/desktop-site-th08-en.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th08-en.png) | [网页](../../../docs/evidence/web/mobile-site-th08-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th08-en.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th08-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th08-en.png) |
| th10 | [网页](../../../docs/evidence/web/desktop-site-th10-en.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th10-en.png) | [网页](../../../docs/evidence/web/mobile-site-th10-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th10-en.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th10-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th10-en.png) |
| th11 | [网页](../../../docs/evidence/web/desktop-site-th11-en.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th11-en.png) | [网页](../../../docs/evidence/web/mobile-site-th11-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th11-en.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th11-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th11-en.png) |
| th15 | [网页](../../../docs/evidence/web/desktop-site-th15-en.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th15-en.png) | [网页](../../../docs/evidence/web/mobile-site-th15-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th15-en.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th15-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th15-en.png) |
| th09 | [网页](../../../docs/evidence/web/desktop-site-th09-en.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th09-en.png) | [网页](../../../docs/evidence/web/mobile-site-th09-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th09-en.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th09-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th09-en.png) |
| th19 | [网页](../../../docs/evidence/web/desktop-site-th19-en.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th19-en.png) | [网页](../../../docs/evidence/web/mobile-site-th19-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th19-en.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th19-en.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th19-en.png) |

### 日语

| 文档 | 桌面 1440 px | 手机 390 px | 手机 320 px |
| --- | --- | --- | --- |
| 常见问题 | [网页](../../../docs/evidence/web/desktop-site-faq-ja.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-faq-ja.png) | [网页](../../../docs/evidence/web/mobile-site-faq-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-faq-ja.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-faq-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-faq-ja.png) |
| 关于本站 | [网页](../../../docs/evidence/web/desktop-site-about-ja.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-about-ja.png) | [网页](../../../docs/evidence/web/mobile-site-about-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-about-ja.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-about-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-about-ja.png) |
| 首次使用须知 | [网页](../../../docs/evidence/web/desktop-site-first-use-ja.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-first-use-ja.png) | [网页](../../../docs/evidence/web/mobile-site-first-use-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-first-use-ja.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-first-use-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-first-use-ja.png) |
| 联机玩法介绍 | [网页](../../../docs/evidence/web/desktop-site-guide-ja.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-guide-ja.png) | [网页](../../../docs/evidence/web/mobile-site-guide-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-guide-ja.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-guide-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-guide-ja.png) |
| 协作通用规则 | [网页](../../../docs/evidence/web/desktop-site-guide-coop-ja.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-guide-coop-ja.png) | [网页](../../../docs/evidence/web/mobile-site-guide-coop-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-guide-coop-ja.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-guide-coop-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-guide-coop-ja.png) |
| th06 | [网页](../../../docs/evidence/web/desktop-site-th06-ja.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th06-ja.png) | [网页](../../../docs/evidence/web/mobile-site-th06-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th06-ja.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th06-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th06-ja.png) |
| th07 | [网页](../../../docs/evidence/web/desktop-site-th07-ja.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th07-ja.png) | [网页](../../../docs/evidence/web/mobile-site-th07-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th07-ja.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th07-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th07-ja.png) |
| th08 | [网页](../../../docs/evidence/web/desktop-site-th08-ja.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th08-ja.png) | [网页](../../../docs/evidence/web/mobile-site-th08-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th08-ja.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th08-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th08-ja.png) |
| th10 | [网页](../../../docs/evidence/web/desktop-site-th10-ja.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th10-ja.png) | [网页](../../../docs/evidence/web/mobile-site-th10-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th10-ja.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th10-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th10-ja.png) |
| th11 | [网页](../../../docs/evidence/web/desktop-site-th11-ja.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th11-ja.png) | [网页](../../../docs/evidence/web/mobile-site-th11-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th11-ja.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th11-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th11-ja.png) |
| th15 | [网页](../../../docs/evidence/web/desktop-site-th15-ja.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th15-ja.png) | [网页](../../../docs/evidence/web/mobile-site-th15-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th15-ja.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th15-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th15-ja.png) |
| th09 | [网页](../../../docs/evidence/web/desktop-site-th09-ja.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th09-ja.png) | [网页](../../../docs/evidence/web/mobile-site-th09-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th09-ja.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th09-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th09-ja.png) |
| th19 | [网页](../../../docs/evidence/web/desktop-site-th19-ja.png) / [Flutter](../../../docs/evidence/flutter/desktop-site-th19-ja.png) | [网页](../../../docs/evidence/web/mobile-site-th19-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-site-th19-ja.png) | [网页](../../../docs/evidence/web/mobile-narrow-site-th19-ja.png) / [Flutter](../../../docs/evidence/flutter/mobile-narrow-site-th19-ja.png) |

