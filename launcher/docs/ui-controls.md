# 启动器 UI | 源码研究与扩展设计

启动器以完整模块为设计单位。通用控件共享视觉和交互，业务组件组合这些控件，页面负责组织内容。新增作品、设置和工具沿用同一套边界。

开发服务器的 `/ui-review.html` 提供交互式 UI 手册，按基础、组合、页面组织。页面目录列出区域、业务部件、子视图和检查状态，可跳到操作样例或实际界面。联机房间单独展示生产席位、管理菜单、快捷发言、观战席、设置和网络部件，演示状态仅影响本页。当前沿用顶栏组织并补入帮助和状态入口，侧栏与手机底栏供对照。

触控选项示例直接使用 `TouchSettingsFields` 和内存偏好模型，取值、预设及手势提交与实际界面共用。资料和账户入口使用实际页面，登录、资料保存和云存档由账号 API 提供；公开体验服按发布开关隐藏账号入口。布局草稿单独标注预览。资源示例展示共用进度组件，下载与重试流程由实际工作区提供。

本文的“源码观察”来自官方实现，“实施设计”是启动器的目标方案，“当前接入”描述现有代码。目标方案按完整模块逐步落地。

## 1. 官方源码基线

研究基线：[shadcn/ui 固定提交](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/README.md)，提交 b8d7baa15b1dca5716f13153e63795fc3d741f6b，日期 2026-10-30。源码链接固定到该提交，线上页面用于观察实际呈现。

| 资源 | 完整源码入口 | 用途 |
| --- | --- | --- |
| 源码与生成流程 | [apps/v4/registry/README.md](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/README.md) | 区分手写源、风格定义和生成产物 |
| Radix 基础控件 | [apps/v4/registry/bases/radix/ui/](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui) | 63 个控件源文件，排除注册表 |
| Radix 功能块 | [apps/v4/registry/bases/radix/blocks/](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks) | 27 个功能块目录，另有 3 个预览目录 |
| 图表示例 | [apps/v4/registry/new-york-v4/charts/](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts) | 70 个示例源文件，排除注册表 |
| 完整示例页 | [apps/v4/app/(app)/examples/](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/app/(app)/examples) | Dashboard、Tasks、Authentication 等页面 |
| 基础交互选择 | [apps/v4/registry/bases.ts](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases.ts) | Radix、Base UI、React Aria 的登记 |
| 视觉风格 | [apps/v4/registry/styles.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/styles.tsx) | 风格与交互底座独立配置 |
| 主题 | [apps/v4/registry/themes.ts](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/themes.ts) | 成对的亮暗语义颜色 |

目录数量只描述这份基线。下文精读与启动器有关的实现，完整文件索引置于文末。

官方当前的 Radix 源模板使用 cn-* 类名和图标占位符，再与风格文件生成可安装组件。new-york-v4 是另一套直接维护的旧注册表。应用接入时应选择一套生成后的组件与风格，业务代码统一从本地 UI 层导入。源码研究不要求复制官网的构建系统。

## 2. 源码观察 | 精致来自哪些组合

| 设计方法 | 官方实现 | 对启动器的用途 |
| --- | --- | --- |
| 外观与尺寸独立 | [Button](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/button.tsx) 的 variant 与 size 分开，asChild 复用真实链接或触发器 | 主次操作、图标操作和紧凑工具栏可共享按钮行为 |
| 用部件组合容器 | [Card](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/card.tsx) 拆分 Header、Title、Action、Content、Footer | 房间、资料和导入面板按内容组合，不强制每块都带副标题 |
| 设置按容器重排 | [Field](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/field.tsx) 提供 FieldGroup、Content、Label、Error，并支持容器查询 | 同一个字段放进侧栏、弹窗或整页时自行重排 |
| 一个输入组合只有一个边框 | [InputGroup](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/input-group.tsx) 区分输入、附加图标和操作区域 | 搜索与路径输入共享焦点和错误外观，消除双边框 |
| 同组控件共同处理边界 | [ButtonGroup](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/button-group.tsx) 处理相邻圆角、边框和焦点层级 | 并列选项由组定义密度与边界，避免每项自行修饰 |
| 相同语义有明确的视觉变体 | [Tabs](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/tabs.tsx) 在同一组件内提供默认与线形样式 | 一级栏目与工具视图可有不同外观，但同一组内一致 |
| 弹层由交互部件构成 | [Dialog](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/dialog.tsx) 组合 Root、Trigger、Portal、Overlay、Content、Title、Close | 通用弹窗共享焦点、关闭、标题关联与层级 |
| 同一表单适配不同载体 | [DrawerDialogDemo](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/examples/radix/drawer-dialog.tsx) 复用 ProfileForm | 桌面弹窗与手机抽屉共享业务内容与保存入口 |
| 导航只共享必要状态 | [Sidebar](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/sidebar.tsx) 用局部 Provider 管理展开和手机开关，支持外部控制 | 统一应用导航，不把所有临时状态提升到全局 |
| 空状态也有组合结构 | [Empty](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/empty.tsx) 将标题、内容、媒体与操作拆开 | 无录像、无房间、无搜索结果按实际原因提供操作 |
| 图表的业务含义集中配置 | [Chart](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/chart.tsx) 的配置连接系列名、颜色、图标和提示 | 生涯统计共享图例含义，后续增加统计维度可扩展 |

### 页面组合

[apps/v4/registry/bases/radix/blocks/dashboard-01/page.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/dashboard-01/page.tsx) 只组织侧栏、页头、数据区和表格。导航分组、图表和表格分别维护自己的组合。

~~~mermaid
flowchart TD
  Page["页面 | 内容与入口"] --> Shell["布局 | 导航与主内容"]
  Shell --> Toolbar["工具栏 | 搜索、筛选、主操作"]
  Shell --> Feature["业务模块 | 列表、表单、播放器"]
  Feature --> Pattern["组合 | FieldGroup、Item、Dialog"]
  Pattern --> Control["控件 | Button、Input、Select"]
  Control --> Primitive["Radix | 键盘、焦点、交互状态"]
  Tokens["视觉变量 | 颜色、尺寸、排版、动效"] --> Control
  Tokens --> Pattern
~~~

Tasks 的 [DataTable](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/app/(app)/examples/tasks/components/data-table.tsx) 接收数据与列定义，排序、筛选和选择在表格附近维护。[Toolbar](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/app/(app)/examples/tasks/components/data-table-toolbar.tsx) 使用同一表格实例。这里值得复用的是边界与状态归属。启动器的简单列表可继续使用普通 React 列表。

### 响应式需要筛选参考

官方 [Dashboard 示例页](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/app/(app)/examples/dashboard/page.tsx) 和 [Tasks 示例页](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/app/(app)/examples/tasks/page.tsx) 在手机宽度显示静态图片。官网 [首页](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/app/(app)/(root)/page.tsx) 也为手机展示单独的静态预览。

可操作的 [Dashboard 功能块](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/dashboard-01/page.tsx) 更适合作为交互参考。它的 [数据表组合](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/dashboard-01/components/data-table.tsx) 按主内容容器切换栏目页签与选择器，详情随设备改变抽屉方向。Field 的 [响应式示例](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/examples/radix/field-responsive.tsx) 则按字段所在容器排列。

线上观察入口：[Dashboard 功能块](https://ui.shadcn.com/view/new-york-v4/dashboard-01)。已观察 1280×800 和 390×844 下的实际页面与手机导航展开。线上内容与固定源码基线分开记录。

官网也有不同风格的混用：本基线首页按钮使用 radix-luma，站点页头使用 radix-nova。启动器应统一选定一套视觉风格，再使用尺寸与布局变体。

## 3. 实施设计 | 可扩展的本地 UI 层

### 技术边界

当前组合是 React + shadcn 风格的本地组件 + Radix Primitives + Tailwind CSS。主页、启动选项、输入、触控、录像、多人和账户页面共用本地 UI 层。作品封面与游戏专属图形独立管理。

React Router 继续负责当前静态页面、导航和历史。游戏运行、资源管理与联机模型沿用已有服务接口。采用 Next.js 应由服务端渲染或服务器业务需求决定。

组件按以下职责分层：

| 层 | 负责 | 依赖边界 |
| --- | --- | --- |
| 视觉变量 | 语义颜色、字体、尺寸、间距、圆角、焦点、层级、动效 | 可被控件和文档样式消费 |
| 通用 UI | Button、Input、Select、Slider、Dialog、Tabs、Menu | React、Radix 和视觉变量 |
| 组合 | FieldGroup、Toolbar、Item、Empty、SettingsSection | 通用 UI，数据与操作由参数传入 |
| 业务模块 | 游戏书库、房间设置、触控工作台、Replay Manager | 组合、业务模型和能力接口 |
| 页面与布局 | 应用导航、路由、模块编排 | 业务模块与路由服务 |

通用 UI 不读取作品目录、浏览器存档或联机服务。组件提供 children、命名部件和受控属性等扩展点，特殊业务通过组合加入。官网的多底座注册表与多风格生成器服务于组件分发，启动器只维护实际使用的底座和风格。

新 UI 样式限定到组件与部件。迁移中的旧样式放在兼容层，模块完成后移除对应规则。尺寸和配色的归属由组件 API 决定，页面主要声明布局。

复杂表格、图表与编辑器按功能入口加载，通用 UI 入口不反向导入这些业务模块。控件颜色、焦点和下拉菜单反馈使用共用 CSS 配方。窗口动效由 Motion/mini 或 CSS 按载体执行，共用窗口参数，业务组件负责可见性、焦点和路由。图形特效独立于控件层，按功能加载。

### 视觉变量与状态

| 类别 | 共用契约 |
| --- | --- |
| 表面与文字 | background / foreground、card / card-foreground、popover / popover-foreground 成对定义 |
| 操作与反馈 | primary、muted、accent、destructive、border、input、ring 按语义引用 |
| 品牌与状态 | 品牌强调色与成功、警告、错误分开，状态同时用文字或图形表达 |
| 尺寸 | 按钮的 variant 与 size 独立，工具栏、设置区使用统一密度 |
| 间距 | 采用共用刻度，组内间距小于组间间距 |
| 圆角 | 从基础半径派生控件、容器与弹层半径 |
| 图标 | 通用操作使用 SVG 与统一尺寸、笔画，图标按钮提供可访问名称；独立箭头按触发器边缘对齐 |
| 动效 | 控件反馈 140 ms，窗口打开 245 ms、关闭 150 ms，支持减少动态效果 |

视觉方向参考官方文档的清晰排版、语义灰阶与轻边界，功能区参考 Nova 的紧凑组合。官网的尺寸和颜色用于建立基准，最终数值由启动器的完整模块验证。

界面采用浅灰画布、白色卡片与柔和的深色灰阶。作品封面使用正方形，空的最近游玩区域省略。资料编辑使用限定宽度的纯色表面。房间玩家卡片使用黄橙、红、绿底色和斜体艺术字区分加载、未准备和已准备，状态文字同时提供给辅助技术。卡片几何独立于状态，艺术字单独裁切，颜色与前景成对定义。通用控件消费同一组语义变量。

作品主标题与英文副标题分别呈现，超长时省略尾部。单机默认按 TH 数字编号递增排列，包括小数编号；TH 后接英文字母的同人作品归入同人分类并排在数字编号作品之后。主页编号位于作品名后，使用小号灰字，窄屏为编号保留空间。单机列表悬停背景采用圆角，封面与背景边缘留有间距。单机作品收藏使用黄色实心星形，悬停时保持按钮尺寸和背景范围稳定。大厅房间行只显示日文本名。大厅座位与个人主页使用圆形头像，空席沿用相同尺寸；窄屏三人房适当缩小头像，为姓名保留空间。玩家姓名与操作方式保留完整信息，长昵称可横向查看。

大厅的「全部」视图将旧作联机入口与房间排在新作之后。th01 至 th04 的联机入口在站点发布对应多人 Runtime 时开放。自由发言失败时在输入框上方显示原因，失败消息保留在聊天记录中。

大厅采用单一布局。封面或作品正文用于只看一作，独立复选框用于增减多选，收藏独立处理。图标栏、桌面列表和贴按钮展开的完整菜单共用选择状态。菜单各行统一复选框、封面、两行作品名和收藏的列位置，手机图标还显示目录提供的汉字简称。工具栏左侧放置房间筛选，右侧放置建房、房间号和刷新；录像入口位于顶栏更多菜单，手机工具栏保持同一行。房间规则显示在独立玩法列，窄屏时位于房间信息下方。普通房间按两人或三人均匀排列座位；花映塚对战房以三列排列，双方分别对齐三人房的左右座位，放大的比分或 VS 位于中间同一行并略向左偏。作弊移动、八向移动和不限向移动使用相同文字颜色。计数位于列表面板底部，房间内容单独滚动，计数不遮挡操作。作品排序、房间筛选和作品选择共用弹层与选项视觉。

亮暗主题都定义完整的表面与前景色。主题声明集中在语义变量层。页面提供自身亮暗主题，并使用 [Dark Reader 的页面标记](https://github.com/darkreader/darkreader/blob/main/CONTRIBUTING.md#disabling-dark-reader-on-your-site)避免再次染色；亮色的 color-scheme 声明也阻止浏览器自动改色。游戏画面使用显式深色范围，触控工作台的应用控件跟随主题，其浮层继承对应范围。Replay 的游戏画面与应用工具栏分别拥有自己的表面颜色。

组件状态通过原生属性、ARIA 或明确的 data-state 表达。按钮的 selected、页签的 active、菜单的 open 各自限定到所属部件。尺寸、颜色和选中态共同来自组件定义。

资料图片选择后先用 react-easy-crop 调整位置和缩放。头像固定 1:1。封面以手机 3:1 为主框，桌面 6:1 预览同步向两侧扩展，超出原图的部分用模糊边缘补齐。确认后生成一张 6:1 封面，手机取正中 3:1。图片在浏览器压缩至 150 KB 以内，头像最长边为 512 像素，封面最长边为 1600 像素。点击保存资料才上传处理后的图片。裁切器和压缩脚本按需加载，处理期间暂停保存。换图、删除或离开时取消过期任务。裁切窗口使用共用 Dialog、Slider、Button 与浮层历史。

登录、注册和找回密码共用账户页与密码输入控件。插画铺满视口，桌面右侧为整高的半透明模糊面板，表单在面板内限宽；窄屏面板占满可用宽度，由页面自然滚动。模糊仅用于账户面板，不支持该效果或偏好降低透明度时使用实色背景。注册按用户名、昵称、邮箱、密码和验证码排列；找回密码通过邮箱验证码重设密码，成功后返回登录。切换模式清空登录密码，已发送验证码对应的注册资料保留至验证结束。

云端每作存一份存档。游戏资源准备完毕后，启动器比较文件指纹和本机上次确认的共同版本。本机未改且云端已更新时自动下载，云端未改时在游戏退出后上传本机变更。两边都有改动或无法确定来源时，显示两张卡片比较时间、时长、局数、难度和最高分，文件清单可展开。已确认的较新版本会标色，两边各有改动时分别标注。玩家可以使用云端、用本机并在退出后更新云端，或仅用本机继续。同一浏览器换标签页启动时，原标签页先保存并退出，新标签页读取刚保存的进度。其他设备有未完成进度时单独确认，玩家可停止检查，或继续并授权本局结束后覆盖当前云端版本。上传用云端版本号校验并发写入，成功上传或确认版本后才结束自己的旧标志。本地日志用于恢复未完成的上传。查询、下载失败或账号失效时照常运行本机游戏。手动上传可选择浏览器或设备文件，图片与压缩存档共用每账号 1 MiB 空间。

资料页的「游戏数据」卡片通过右下角图标切换全作和花映冢。全作展示通关进度和飞行时长，花映冢展示 WHR 和对战记录。资料设置可调整默认视图和各视图的公开显示。大厅和房间的账号头像支持悬停或点按打开个人卡片，编辑资料时使用同一卡片预览封面、头像、昵称和公开机体分。

### 可复用组合

| 场景 | 组合方式 | 扩展方式 |
| --- | --- | --- |
| 设置 | Section → FieldGroup → Field → Label / Control / Error | 增加字段或分组，复用标签、校验与响应式布局 |
| 搜索与筛选 | Toolbar → InputGroup + Filters + ViewChoice + Actions | 将低频筛选收进菜单，主操作保持可达 |
| 普通列表 | ItemGroup → Item → Media / Content / Actions | 同一数据支持列表与卡片，长文本可换行 |
| 复杂数据 | 数据模型 → 列定义 + Table + Toolbar + Pagination | 有排序、列控制或大量数据需求时采用表格模型 |
| 详情与编辑 | 受控面板 → 共享内容 + Dialog / Sheet | 载体变化时保存草稿、校验和当前操作 |
| 任务状态 | Loading / Empty / Error / Ready + 相应操作 | 对应真实任务状态，错误提供重试或恢复入口 |
| 危险操作 | Action → 确认 → 业务命令 → 结果 | 确认与执行分开，防止重复提交 |

按钮外观采用 primary、secondary、outline、ghost、destructive、link 等有限语义，尺寸单独提供 sm、default、lg、icon。导航、页签、开关和单选组使用各自的组件语义。外观相近的控件通过组合复用尺寸与变量。

源码以可访问部件组织布局，说明文字按操作所需加入。必需提示关联到对应字段。可选副标题和空白容器随内容省略。

## 4. 状态、触控与扩展

### 状态归属

| 状态 | 归属 |
| --- | --- |
| 页面地址、详情入口、浏览器返回 | 路由与历史服务 |
| 游戏选项、账号配置、作品能力 | 既有业务模型与目录 |
| 资源下载、播放、房间连接 | 各自的任务或会话模型 |
| 菜单展开、悬停、当前编辑草稿 | 最近的业务组件 |
| 共享侧栏展开与密度 | 对应布局的局部上下文 |

共享状态按参与组件的范围上移。UI 层接收当前值、变化回调和操作状态。业务模型处理保存、取消、重试与异步结果。

视图按字段订阅外部状态。运行遥测由播放器消费，页面骨架订阅呈现状态，触控拖动由编辑器消费。草稿坐标变化不重建导航和启动设置，最近游玩记录独立跟随实际启动。

滑块区分预览与提交：灵敏度拖动时预览，松手时保存。录像拖动时显示目标，提交时发送跳转。取消手势不发送完成操作。横竖屏切换和实时状态更新期间，由业务组件持有草稿与任务身份。

### 响应式与弹层

组件按自身容器宽度调整布局，应用布局再按视口切换导航。触控命中区与视觉尺寸分开设计，常用触控操作以 44 px 命中区为基准。拖动控件与滚动容器明确手势归属。

主题设置从顶栏的主题按钮进入，亮色和暗色作为主题基底，背景与基底分别设置。内置背景包括星点、方格和青海波纹样。图片保存在当前浏览器，铺满按比例覆盖视口并居中裁切，完整显示按比例容纳整张图片并在空白处显示主题底色。背景遮罩、顶部栏、分类条、作品卡片和内容面板的不透明度独立调整；菜单、弹窗、输入控件和游戏画面保持自身表面。刷新和切换页面后恢复设置，移除图片后恢复基底主题。

手机作品快捷条的角标复选区为 24 px，与封面单选区分开。完整作品菜单的复选与收藏入口至少为 44 px，两处共用筛选状态。

共用菜单的最小宽度受 Radix 测得的可用宽度约束，兼顾窄屏、页面缩放与安全区。

大厅房间筛选菜单向下展开时，面板左边和上边分别与房间列表的左边和上边对齐；空间不足时仍允许弹层避开视口边界。

弹层宿主统一处理普通页面、原生 dialog、游戏全屏与嵌套菜单。主题范围、焦点约束、滚动锁和关闭返回共同验证。仅迁移弹层外观时仍沿用原有业务完成协议。

遮罩、弹窗和内部菜单分别使用 `--ui-layer-overlay`、`--ui-layer-dialog`、`--ui-layer-popup`，菜单高于所在弹窗。验收时实际展开菜单，检查绘制层级、Esc、外部关闭与焦点返回。Radix 在菜单展开时会禁用底层指针事件，仅检查点击命中与选中值会漏掉菜单被遮挡的问题。

单机、联机内容区与顶栏共用最大宽度和左右边距，桌面双栏布局的侧栏宽度和主内容起点一致；顶栏的单机与联机入口使用较醒目的文字和图标。页面和桌面大厅共用根元素的稳定滚动条预留策略，避免切页时横向位移。全局滚动条样式放在 base 层，昵称等局部滚动区域可隐藏滚动条。Radix 弹层继续约束焦点与触控滚动。支持稳定滚动条槽的浏览器不再对 body 重复补偿宽度，弹层也不改变 body 的定位方式。浏览器回归检查排序、改键及嵌套菜单开关前后的页面坐标、尺寸和滚动位置，`tests/visual/layout.html` 可逐帧记录实际页面的几何变化与长任务。

同一页面的浮层历史共用页面滚动位置，关闭浮层时不跳回旧历史项的滚动坐标。独立页面按路径分别恢复位置。

窄屏站内切页直接更新内容并淡入。总览抽屉从右边滑入，关闭时立即释放点击。「关于本站」固定在抽屉底部并默认折叠。顶栏独立于页面过渡，登录插画的快照从顶栏下方开始绘制。

Replay 管理器沿用联机房间的内容入场动效与作品菜单。文件夹共用列表容器并各自记住滚动位置。系统返回先关闭文件夹、详情或移动操作等子层，退出管理器后回到原入口。切换作品时，会话服务先完成当前播放实例的保存和清理，再打开新作品的根目录。

输入设置使用受视口约束的固定窗口，内部表单滚动。切换设备、配置场景或连接状态时，窗口与标题的位置和尺寸一起验收。

窗口动效使用透明度、6 px 位移和 96.5% 到 100% 的缩放。裁切窗口沿用时长和缓动，图片测量期间省去缩放。浮层关闭由完成信号驱动，支持中途重开和减少动态效果。动效保留组件自身的定位变换，并在结束、取消或卸载后释放临时样式和监听器。


收藏操作以星形短暂回弹反馈，明暗主题切换以图标轻微旋转淡入反馈；顶部导航悬停时底部指示线柔和展开，当前页面的指示线保持显示；常见问题图标轻微倾斜，捐赠图标以心跳动效反馈，按下时分别给予反馈。顶栏用户头像使用圆角方形，悬停或聚焦时细描线两端顺时针追逐直至闭环；菜单图标中线轻移，展开时保持选中态。下拉菜单展开和收起时柔和伸缩淡入淡出。单机封面与列表切换时选中底色平滑横移；作品卡片悬停时整体轻微浮起放大，辅以柔和阴影，周围布局保持稳定。大厅新房间柔和淡入上移，关闭的房间淡出移除，首次加载及已有房间更新不重复播放；作品多选的圆环过渡为描画出的绿勾，刷新图标点击时旋转。系统或站内开启减少动态效果时跳过这些动效。


官网侧栏包含全局快捷键和 Cookie 写入。启动器接入时，快捷键只在应用导航拥有输入时生效，持久化沿现有偏好接口处理。游戏输入由现有会话边界管理。

彩色表面的按钮显式选择变体与配对前景色。玩家卡片上的管理按钮使用透明背景和卡片前景，避免默认按钮背景覆盖状态色。

席位布局由房间模块定义。浏览器回归通过生产部件检查两人、三人、窄屏和状态切换的几何，防止基础样式重新覆盖席位模板。

公开昵称保持单行，超出可用宽度时支持触控与键盘横向滚动。昵称长度不改变席位高度，短昵称使用普通文本的焦点顺序。

### 新功能的接入方式

| 新需求 | 修改范围 | 验证重点 |
| --- | --- | --- |
| 新作品使用已有设置类型 | 产品能力目录与设置数据 | 字段可见性、默认值、保存与各语言 |
| 新作品需要一种特殊控制 | 作品业务适配与组合插槽 | 通用 UI API 稳定，操作规则清楚 |
| 新增工具页面 | 业务模块与页面注册 | 共用导航、工具栏、主题与状态反馈 |
| 新增图表系列 | 统计数据与图表配置 | 图例、提示、单位与亮暗颜色 |
| 列表增长到大量记录 | 查询、分页或虚拟化的数据边界 | 响应时间、键盘定位与滚动恢复 |
| 更换品牌色或密度 | 视觉变量与共享变体 | 模块对照、触控命中与对比度 |

数据配置适合规则一致的选项、导航项和列定义。录像跳转、触控画布、输入捕获等操作由专门组件实现，继续组合通用部件。

## 5. 当前接入与迁移顺序

现有版本已将选择菜单、房间菜单、复选框、开关、页签、单选组与折叠交互接入 Radix。通用按钮共享入口，保留原有表单提交语义。文本与文件输入使用原生元素。

[MainSelect](../app/components/launcher/MainSelect.tsx) 保留原生表单值、ref 与 change 事件，Radix 负责可见选择菜单。[OptionsGroup](../app/components/settings/OptionsGroup.tsx) 统一设置标题与折叠结构。[RoomActionMenu](../app/components/room/RoomActionMenu.tsx) 已共享房间操作菜单。

快捷发言提供九个本地常用栏位：在联机房间打开快捷发言，使用齿轮进入设置，选择栏位后从全部语音中替换。设置保存在当前浏览器；房间内全部语音可滚动选择。游戏内以九宫格展示，长文本截断并保留完整提示；使用 Alt 打开面板，数字键发送当前页对应语音，也可切换全部语音并翻页。

房间与游戏内聊天共用消息模型、InputGroup 输入组合、发送按钮、快捷发言和屏蔽菜单。游客与注册用户使用相同的 Radix 快捷面板和本地屏蔽，屏蔽同时作用于消息与语音。消息行的操作入口不参与行高计算。注册身份由服务端确认，自由发言的操作菜单提供单步举报，结果通过共用 Toast 展示；撤回按服务端消息序号处理，重连快照与实时消息合并。手册房间样例可切换游客与注册用户，演示发言与撤回只影响当前页面。

游戏内注册用户单按 Alt 后松开可切换聊天，展开后输入框获得焦点，Esc 返回游戏。组合键和输入法继续使用自己的按键；快捷键在现有游戏输入桥接中处理，玩家与旁观者共用规则。聊天获得焦点时清空游戏按键持有，并停止 Android 启动焦点重试。

主页和设置的按钮已将外观与尺寸分开，选择组与排序菜单使用 Radix。触控灵敏度、透明度和 Replay 进度使用共用 Radix 滑块，灵敏度与录像的预览、提交及取消分别处理。旧模块继续使用兼容入口。手册提供普通 Radix 弹窗样板；TouchHelp 已复用 InformationalDialog，Replay 文件操作已复用 ActionMenu 和 Button。游戏全屏窗口仍按各自生命周期管理。

### 主页样板

首页首张封面优先加载，其余使用浏览器原生懒加载。图片加载时使用共用的 shadcn 风格 `Skeleton`，占位沿用封面比例和主题颜色；加载失败显示作品名，缓存命中直接展示图片，减少动态效果时停止脉冲。手册的反馈区展示同一组件。

主页沿用分类、最近游玩、作品网格和列表的组织。搜索、分类、收藏、排序、空状态与设置返回共同验收。

- 通用控件的外观与尺寸使用组件参数，页面通过组合与布局类声明几何关系。
- 新组件使用统一的语义变量，旧共享界面通过同一主题映射接入。亮暗主题都覆盖浮层与状态。
- Tailwind 生成样式不包含全局 Preflight，兼容层、组件层与工具类按顺序排列。
- 分类和视图使用单选语义与键盘移动，收藏保持独立开关。手机常用操作的命中区至少 44 px。
- 设置面板收起后使用 inert 隔离操作，原有关闭时序与焦点返回继续由导航管理。
- 主页已迁出的旧样式随模块一起移除，新增页面从同一组通用控件和视觉变量开始。

迁移按完整模块推进：

1. 用生产组件建立完整样板，覆盖亮暗、尺寸、禁用、选中、错误、加载和键盘焦点。共享 API 与视觉变量在这里验证，对照页直接消费这些实现。
2. 以设置表单、菜单和弹层为首批组合，先统一并列选项与滑块提交，再迁移普通弹窗。
3. 依次完成主页、触控工作台、Replay Manager、多人页面。每个模块一起处理外观、交互、状态与响应式。
4. 模块验收后删除相应旧样式、手写交互与重复包装，剩余兼容层标明实际使用者。
5. 新模块直接消费同一套组件和组合。整页文档使用 Fumadocs 布局，应用内说明共享其官方 prose 排版。兼容性页生成自包含的旧语法 HTML 和纯色 CSS。

UI 对照页继续陈列存在显著差异的方案。尚未确定的选择留在对照页，其他模块继续推进。

### 文档与说明

`app/components/documents/catalog.ts` 统一登记页面、消息和作者源。FAQ、首次须知、联机玩法来自 Markdown；关于页来自 `content/ABOUT.html` 正文片段。`build-content-pages.mjs` 生成中文消息及须知、玩法片段，英文和日文由对应消息文件维护。命令行构建和 Vite 开发入口都先更新正文，再编译消息。

文档整页通过 `DocsLayout` 提供侧栏与目录，顶栏、主题和语言复用站点实现。手机使用可展开的目录长条，将站点与文档入口放进展开区。说明弹窗、触控和生涯帮助使用 `app/ui/document-body.css` 的同一 prose 配方。游戏帮助复用说明弹窗与设置折叠组，按作品和输入方式组织按键、触控、横屏与练习说明，内容在窗口内滚动。编辑器、表单和短通知按原有控件布局组织。

CSS 层级在文档头的样式表链接之前声明为 `properties, foundation, theme, base, components, utilities`，分包加载顺序不会改变层级。组件外观与正文配方分别在所属层生成。定义列表的网格间距由载体管理，抽屉间距限定在抽屉内。文档侧栏响应断点时直接调整列宽。文档页不能改变顶栏和控件的边框、颜色或尺寸。文档与游戏页面跨启动边界时重新加载文档，游戏页面内部继续由路由管理会话。

页尾修改记录在构建时读取作者源的 Git 历史；译文读取对应消息行的历史。未提交原文和译文显示未提交标记，页面访问不请求 GitHub API。

## 6. 模块验收

| 模块 | 必查状态 |
| --- | --- |
| 主页 | 搜索、分类、封面与列表、长标题、无记录、无结果、设置展开 |
| 触控工作台 | 编辑、拖动与缩放、滚动、菜单、灵敏度预览与提交、取消与保存 |
| Replay Manager | 空列表、已有文件、文件菜单、重命名与移动、播放、跳转、任务失败 |
| 多人页面 | 房间列表、手机作品图标与完整选择弹窗、完整昵称、操作方式、人数、旁观数、时长与比分、空席与离线、目录截断、无房间、断线、创建与加入、成员菜单、设置与全屏返回 |
| 共用弹层 | 打开、Esc、外部点击、确认、嵌套菜单、焦点返回与重复触发 |

部件在交互手册直接检视，页面特有部件放在所属页面下。截图用于整页布局、横竖屏、亮暗主题及必要的高风险流程；对照固定浏览器、视口、字体、数据、展开项与滚动位置，并说明明显差异。

输入设置共用标题、页签、字段与表格的左边界。场景、设备和布局选择器沿同一控制列排列，键帽对应“按键”表头，恢复按钮与同一行键位居中。浏览器检查同时测量这些内部位置、表单控件高度和阈值滑块的两端。

行为检查覆盖键盘、鼠标和触控。主题检查覆盖正文、占位符、禁用、选中、错误、遮罩和所有脱离文档流的菜单。视觉对照与真实操作共同作为验收输入。

开发服务器启动后运行 `bun run test:ui-system:browser -- http://127.0.0.1:8151/`。该入口检查生产主页、设置、个人页、账户页和帮助页，以及由生产部件组成的大厅、房间、Replay、输入和浮层样例；覆盖亮暗主题、横竖屏、窄横屏和连续操作。检查包括溢出、文字对比、控件对齐、焦点和拖动布局。它不代替真实 Runtime、网络和设备验收。结果写入 `.cache/ui-system`，包含截图、来源、视口和失败清单。浏览器路径可用 `CHROME_PATH` 指定，输出目录可用 `UI_AUDIT_OUTPUT` 指定。

房间、录像、输入设备和运行浮层的开发展示入口放在 tests/visual，使用实际业务组件与内存数据，不进入发布路由。原版对照与新版使用同一份示例数据。截图比较必须核对视图、展开项和滚动位置，测试命令与实际执行结果分开记录。真实游戏、实体触控及公网联机仍由对应的集成测试验收。

`bun run test:ui-handbook:browser` 检查生产房间部件和有内容的大厅。大厅采用单一布局，按亮暗主题和四种视口检查字段可见性、昵称滚动、比分对称、难度位置、封面回退与实际表面颜色，并验证多处作品选择同步。手册按人数和成员状态检查席位几何。DOM 中存在文字不能替代这些检查。

## 7. 官方源码索引

以下索引按上述固定提交生成。基础控件、功能块和图表分开列出，完整路径可直接打开。实现时先查相关完整功能块，再查它使用的控件与状态示例。

<details>
<summary>基础控件 | 63 项</summary>

| 名称 | 完整源码路径 |
| --- | --- |
| accordion | [apps/v4/registry/bases/radix/ui/accordion.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/accordion.tsx) |
| alert-dialog | [apps/v4/registry/bases/radix/ui/alert-dialog.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/alert-dialog.tsx) |
| alert | [apps/v4/registry/bases/radix/ui/alert.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/alert.tsx) |
| aspect-ratio | [apps/v4/registry/bases/radix/ui/aspect-ratio.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/aspect-ratio.tsx) |
| attachment | [apps/v4/registry/bases/radix/ui/attachment.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/attachment.tsx) |
| avatar | [apps/v4/registry/bases/radix/ui/avatar.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/avatar.tsx) |
| badge | [apps/v4/registry/bases/radix/ui/badge.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/badge.tsx) |
| breadcrumb | [apps/v4/registry/bases/radix/ui/breadcrumb.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/breadcrumb.tsx) |
| bubble | [apps/v4/registry/bases/radix/ui/bubble.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/bubble.tsx) |
| button-group | [apps/v4/registry/bases/radix/ui/button-group.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/button-group.tsx) |
| button | [apps/v4/registry/bases/radix/ui/button.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/button.tsx) |
| calendar | [apps/v4/registry/bases/radix/ui/calendar.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/calendar.tsx) |
| card | [apps/v4/registry/bases/radix/ui/card.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/card.tsx) |
| carousel | [apps/v4/registry/bases/radix/ui/carousel.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/carousel.tsx) |
| chart | [apps/v4/registry/bases/radix/ui/chart.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/chart.tsx) |
| checkbox | [apps/v4/registry/bases/radix/ui/checkbox.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/checkbox.tsx) |
| code-block | [apps/v4/registry/bases/radix/ui/code-block.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/code-block.tsx) |
| collapsible | [apps/v4/registry/bases/radix/ui/collapsible.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/collapsible.tsx) |
| combobox | [apps/v4/registry/bases/radix/ui/combobox.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/combobox.tsx) |
| command | [apps/v4/registry/bases/radix/ui/command.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/command.tsx) |
| context-menu | [apps/v4/registry/bases/radix/ui/context-menu.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/context-menu.tsx) |
| dialog | [apps/v4/registry/bases/radix/ui/dialog.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/dialog.tsx) |
| direction | [apps/v4/registry/bases/radix/ui/direction.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/direction.tsx) |
| drawer | [apps/v4/registry/bases/radix/ui/drawer.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/drawer.tsx) |
| dropdown-menu | [apps/v4/registry/bases/radix/ui/dropdown-menu.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/dropdown-menu.tsx) |
| empty | [apps/v4/registry/bases/radix/ui/empty.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/empty.tsx) |
| field | [apps/v4/registry/bases/radix/ui/field.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/field.tsx) |
| hover-card | [apps/v4/registry/bases/radix/ui/hover-card.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/hover-card.tsx) |
| input-group | [apps/v4/registry/bases/radix/ui/input-group.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/input-group.tsx) |
| input-otp | [apps/v4/registry/bases/radix/ui/input-otp.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/input-otp.tsx) |
| input | [apps/v4/registry/bases/radix/ui/input.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/input.tsx) |
| item | [apps/v4/registry/bases/radix/ui/item.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/item.tsx) |
| kbd | [apps/v4/registry/bases/radix/ui/kbd.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/kbd.tsx) |
| label | [apps/v4/registry/bases/radix/ui/label.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/label.tsx) |
| marker | [apps/v4/registry/bases/radix/ui/marker.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/marker.tsx) |
| menubar | [apps/v4/registry/bases/radix/ui/menubar.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/menubar.tsx) |
| message-scroller | [apps/v4/registry/bases/radix/ui/message-scroller.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/message-scroller.tsx) |
| message | [apps/v4/registry/bases/radix/ui/message.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/message.tsx) |
| native-select | [apps/v4/registry/bases/radix/ui/native-select.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/native-select.tsx) |
| navigation-menu | [apps/v4/registry/bases/radix/ui/navigation-menu.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/navigation-menu.tsx) |
| pagination | [apps/v4/registry/bases/radix/ui/pagination.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/pagination.tsx) |
| popover | [apps/v4/registry/bases/radix/ui/popover.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/popover.tsx) |
| progress | [apps/v4/registry/bases/radix/ui/progress.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/progress.tsx) |
| questionnaire | [apps/v4/registry/bases/radix/ui/questionnaire.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/questionnaire.tsx) |
| radio-group | [apps/v4/registry/bases/radix/ui/radio-group.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/radio-group.tsx) |
| resizable | [apps/v4/registry/bases/radix/ui/resizable.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/resizable.tsx) |
| scroll-area | [apps/v4/registry/bases/radix/ui/scroll-area.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/scroll-area.tsx) |
| select | [apps/v4/registry/bases/radix/ui/select.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/select.tsx) |
| separator | [apps/v4/registry/bases/radix/ui/separator.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/separator.tsx) |
| sheet | [apps/v4/registry/bases/radix/ui/sheet.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/sheet.tsx) |
| sidebar | [apps/v4/registry/bases/radix/ui/sidebar.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/sidebar.tsx) |
| skeleton | [apps/v4/registry/bases/radix/ui/skeleton.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/skeleton.tsx) |
| slider | [apps/v4/registry/bases/radix/ui/slider.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/slider.tsx) |
| sonner | [apps/v4/registry/bases/radix/ui/sonner.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/sonner.tsx) |
| spinner | [apps/v4/registry/bases/radix/ui/spinner.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/spinner.tsx) |
| switch | [apps/v4/registry/bases/radix/ui/switch.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/switch.tsx) |
| table | [apps/v4/registry/bases/radix/ui/table.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/table.tsx) |
| tabs | [apps/v4/registry/bases/radix/ui/tabs.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/tabs.tsx) |
| textarea | [apps/v4/registry/bases/radix/ui/textarea.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/textarea.tsx) |
| toggle-group | [apps/v4/registry/bases/radix/ui/toggle-group.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/toggle-group.tsx) |
| toggle | [apps/v4/registry/bases/radix/ui/toggle.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/toggle.tsx) |
| tooltip | [apps/v4/registry/bases/radix/ui/tooltip.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/tooltip.tsx) |
| trace | [apps/v4/registry/bases/radix/ui/trace.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/ui/trace.tsx) |

</details>

<details>
<summary>功能块 | 27 项</summary>

| 名称 | 完整源码路径 |
| --- | --- |
| dashboard-01 | [apps/v4/registry/bases/radix/blocks/dashboard-01](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/dashboard-01) |
| login-01 | [apps/v4/registry/bases/radix/blocks/login-01](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/login-01) |
| login-02 | [apps/v4/registry/bases/radix/blocks/login-02](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/login-02) |
| login-03 | [apps/v4/registry/bases/radix/blocks/login-03](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/login-03) |
| login-04 | [apps/v4/registry/bases/radix/blocks/login-04](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/login-04) |
| login-05 | [apps/v4/registry/bases/radix/blocks/login-05](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/login-05) |
| sidebar-01 | [apps/v4/registry/bases/radix/blocks/sidebar-01](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/sidebar-01) |
| sidebar-02 | [apps/v4/registry/bases/radix/blocks/sidebar-02](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/sidebar-02) |
| sidebar-03 | [apps/v4/registry/bases/radix/blocks/sidebar-03](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/sidebar-03) |
| sidebar-04 | [apps/v4/registry/bases/radix/blocks/sidebar-04](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/sidebar-04) |
| sidebar-05 | [apps/v4/registry/bases/radix/blocks/sidebar-05](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/sidebar-05) |
| sidebar-06 | [apps/v4/registry/bases/radix/blocks/sidebar-06](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/sidebar-06) |
| sidebar-07 | [apps/v4/registry/bases/radix/blocks/sidebar-07](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/sidebar-07) |
| sidebar-08 | [apps/v4/registry/bases/radix/blocks/sidebar-08](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/sidebar-08) |
| sidebar-09 | [apps/v4/registry/bases/radix/blocks/sidebar-09](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/sidebar-09) |
| sidebar-10 | [apps/v4/registry/bases/radix/blocks/sidebar-10](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/sidebar-10) |
| sidebar-11 | [apps/v4/registry/bases/radix/blocks/sidebar-11](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/sidebar-11) |
| sidebar-12 | [apps/v4/registry/bases/radix/blocks/sidebar-12](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/sidebar-12) |
| sidebar-13 | [apps/v4/registry/bases/radix/blocks/sidebar-13](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/sidebar-13) |
| sidebar-14 | [apps/v4/registry/bases/radix/blocks/sidebar-14](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/sidebar-14) |
| sidebar-15 | [apps/v4/registry/bases/radix/blocks/sidebar-15](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/sidebar-15) |
| sidebar-16 | [apps/v4/registry/bases/radix/blocks/sidebar-16](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/sidebar-16) |
| signup-01 | [apps/v4/registry/bases/radix/blocks/signup-01](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/signup-01) |
| signup-02 | [apps/v4/registry/bases/radix/blocks/signup-02](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/signup-02) |
| signup-03 | [apps/v4/registry/bases/radix/blocks/signup-03](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/signup-03) |
| signup-04 | [apps/v4/registry/bases/radix/blocks/signup-04](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/signup-04) |
| signup-05 | [apps/v4/registry/bases/radix/blocks/signup-05](https://github.com/shadcn-ui/ui/tree/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/signup-05) |

</details>

<details>
<summary>图表 | 70 项</summary>

| 名称 | 完整源码路径 |
| --- | --- |
| chart-area-axes | [apps/v4/registry/new-york-v4/charts/chart-area-axes.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-area-axes.tsx) |
| chart-area-default | [apps/v4/registry/new-york-v4/charts/chart-area-default.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-area-default.tsx) |
| chart-area-gradient | [apps/v4/registry/new-york-v4/charts/chart-area-gradient.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-area-gradient.tsx) |
| chart-area-icons | [apps/v4/registry/new-york-v4/charts/chart-area-icons.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-area-icons.tsx) |
| chart-area-interactive | [apps/v4/registry/new-york-v4/charts/chart-area-interactive.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-area-interactive.tsx) |
| chart-area-legend | [apps/v4/registry/new-york-v4/charts/chart-area-legend.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-area-legend.tsx) |
| chart-area-linear | [apps/v4/registry/new-york-v4/charts/chart-area-linear.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-area-linear.tsx) |
| chart-area-stacked-expand | [apps/v4/registry/new-york-v4/charts/chart-area-stacked-expand.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-area-stacked-expand.tsx) |
| chart-area-stacked | [apps/v4/registry/new-york-v4/charts/chart-area-stacked.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-area-stacked.tsx) |
| chart-area-step | [apps/v4/registry/new-york-v4/charts/chart-area-step.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-area-step.tsx) |
| chart-bar-active | [apps/v4/registry/new-york-v4/charts/chart-bar-active.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-bar-active.tsx) |
| chart-bar-default | [apps/v4/registry/new-york-v4/charts/chart-bar-default.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-bar-default.tsx) |
| chart-bar-horizontal | [apps/v4/registry/new-york-v4/charts/chart-bar-horizontal.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-bar-horizontal.tsx) |
| chart-bar-interactive | [apps/v4/registry/new-york-v4/charts/chart-bar-interactive.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-bar-interactive.tsx) |
| chart-bar-label-custom | [apps/v4/registry/new-york-v4/charts/chart-bar-label-custom.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-bar-label-custom.tsx) |
| chart-bar-label | [apps/v4/registry/new-york-v4/charts/chart-bar-label.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-bar-label.tsx) |
| chart-bar-mixed | [apps/v4/registry/new-york-v4/charts/chart-bar-mixed.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-bar-mixed.tsx) |
| chart-bar-multiple | [apps/v4/registry/new-york-v4/charts/chart-bar-multiple.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-bar-multiple.tsx) |
| chart-bar-negative | [apps/v4/registry/new-york-v4/charts/chart-bar-negative.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-bar-negative.tsx) |
| chart-bar-stacked | [apps/v4/registry/new-york-v4/charts/chart-bar-stacked.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-bar-stacked.tsx) |
| chart-line-default | [apps/v4/registry/new-york-v4/charts/chart-line-default.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-line-default.tsx) |
| chart-line-dots-colors | [apps/v4/registry/new-york-v4/charts/chart-line-dots-colors.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-line-dots-colors.tsx) |
| chart-line-dots-custom | [apps/v4/registry/new-york-v4/charts/chart-line-dots-custom.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-line-dots-custom.tsx) |
| chart-line-dots | [apps/v4/registry/new-york-v4/charts/chart-line-dots.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-line-dots.tsx) |
| chart-line-interactive | [apps/v4/registry/new-york-v4/charts/chart-line-interactive.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-line-interactive.tsx) |
| chart-line-label-custom | [apps/v4/registry/new-york-v4/charts/chart-line-label-custom.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-line-label-custom.tsx) |
| chart-line-label | [apps/v4/registry/new-york-v4/charts/chart-line-label.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-line-label.tsx) |
| chart-line-linear | [apps/v4/registry/new-york-v4/charts/chart-line-linear.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-line-linear.tsx) |
| chart-line-multiple | [apps/v4/registry/new-york-v4/charts/chart-line-multiple.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-line-multiple.tsx) |
| chart-line-step | [apps/v4/registry/new-york-v4/charts/chart-line-step.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-line-step.tsx) |
| chart-pie-donut-active | [apps/v4/registry/new-york-v4/charts/chart-pie-donut-active.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-pie-donut-active.tsx) |
| chart-pie-donut-text | [apps/v4/registry/new-york-v4/charts/chart-pie-donut-text.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-pie-donut-text.tsx) |
| chart-pie-donut | [apps/v4/registry/new-york-v4/charts/chart-pie-donut.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-pie-donut.tsx) |
| chart-pie-interactive | [apps/v4/registry/new-york-v4/charts/chart-pie-interactive.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-pie-interactive.tsx) |
| chart-pie-label-custom | [apps/v4/registry/new-york-v4/charts/chart-pie-label-custom.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-pie-label-custom.tsx) |
| chart-pie-label-list | [apps/v4/registry/new-york-v4/charts/chart-pie-label-list.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-pie-label-list.tsx) |
| chart-pie-label | [apps/v4/registry/new-york-v4/charts/chart-pie-label.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-pie-label.tsx) |
| chart-pie-legend | [apps/v4/registry/new-york-v4/charts/chart-pie-legend.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-pie-legend.tsx) |
| chart-pie-separator-none | [apps/v4/registry/new-york-v4/charts/chart-pie-separator-none.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-pie-separator-none.tsx) |
| chart-pie-simple | [apps/v4/registry/new-york-v4/charts/chart-pie-simple.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-pie-simple.tsx) |
| chart-pie-stacked | [apps/v4/registry/new-york-v4/charts/chart-pie-stacked.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-pie-stacked.tsx) |
| chart-radar-default | [apps/v4/registry/new-york-v4/charts/chart-radar-default.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-radar-default.tsx) |
| chart-radar-dots | [apps/v4/registry/new-york-v4/charts/chart-radar-dots.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-radar-dots.tsx) |
| chart-radar-grid-circle-fill | [apps/v4/registry/new-york-v4/charts/chart-radar-grid-circle-fill.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-radar-grid-circle-fill.tsx) |
| chart-radar-grid-circle-no-lines | [apps/v4/registry/new-york-v4/charts/chart-radar-grid-circle-no-lines.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-radar-grid-circle-no-lines.tsx) |
| chart-radar-grid-circle | [apps/v4/registry/new-york-v4/charts/chart-radar-grid-circle.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-radar-grid-circle.tsx) |
| chart-radar-grid-custom | [apps/v4/registry/new-york-v4/charts/chart-radar-grid-custom.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-radar-grid-custom.tsx) |
| chart-radar-grid-fill | [apps/v4/registry/new-york-v4/charts/chart-radar-grid-fill.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-radar-grid-fill.tsx) |
| chart-radar-grid-none | [apps/v4/registry/new-york-v4/charts/chart-radar-grid-none.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-radar-grid-none.tsx) |
| chart-radar-icons | [apps/v4/registry/new-york-v4/charts/chart-radar-icons.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-radar-icons.tsx) |
| chart-radar-label-custom | [apps/v4/registry/new-york-v4/charts/chart-radar-label-custom.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-radar-label-custom.tsx) |
| chart-radar-legend | [apps/v4/registry/new-york-v4/charts/chart-radar-legend.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-radar-legend.tsx) |
| chart-radar-lines-only | [apps/v4/registry/new-york-v4/charts/chart-radar-lines-only.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-radar-lines-only.tsx) |
| chart-radar-multiple | [apps/v4/registry/new-york-v4/charts/chart-radar-multiple.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-radar-multiple.tsx) |
| chart-radar-radius | [apps/v4/registry/new-york-v4/charts/chart-radar-radius.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-radar-radius.tsx) |
| chart-radial-grid | [apps/v4/registry/new-york-v4/charts/chart-radial-grid.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-radial-grid.tsx) |
| chart-radial-label | [apps/v4/registry/new-york-v4/charts/chart-radial-label.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-radial-label.tsx) |
| chart-radial-shape | [apps/v4/registry/new-york-v4/charts/chart-radial-shape.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-radial-shape.tsx) |
| chart-radial-simple | [apps/v4/registry/new-york-v4/charts/chart-radial-simple.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-radial-simple.tsx) |
| chart-radial-stacked | [apps/v4/registry/new-york-v4/charts/chart-radial-stacked.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-radial-stacked.tsx) |
| chart-radial-text | [apps/v4/registry/new-york-v4/charts/chart-radial-text.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-radial-text.tsx) |
| chart-tooltip-advanced | [apps/v4/registry/new-york-v4/charts/chart-tooltip-advanced.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-tooltip-advanced.tsx) |
| chart-tooltip-default | [apps/v4/registry/new-york-v4/charts/chart-tooltip-default.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-tooltip-default.tsx) |
| chart-tooltip-formatter | [apps/v4/registry/new-york-v4/charts/chart-tooltip-formatter.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-tooltip-formatter.tsx) |
| chart-tooltip-icons | [apps/v4/registry/new-york-v4/charts/chart-tooltip-icons.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-tooltip-icons.tsx) |
| chart-tooltip-indicator-line | [apps/v4/registry/new-york-v4/charts/chart-tooltip-indicator-line.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-tooltip-indicator-line.tsx) |
| chart-tooltip-indicator-none | [apps/v4/registry/new-york-v4/charts/chart-tooltip-indicator-none.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-tooltip-indicator-none.tsx) |
| chart-tooltip-label-custom | [apps/v4/registry/new-york-v4/charts/chart-tooltip-label-custom.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-tooltip-label-custom.tsx) |
| chart-tooltip-label-formatter | [apps/v4/registry/new-york-v4/charts/chart-tooltip-label-formatter.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-tooltip-label-formatter.tsx) |
| chart-tooltip-label-none | [apps/v4/registry/new-york-v4/charts/chart-tooltip-label-none.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/new-york-v4/charts/chart-tooltip-label-none.tsx) |

</details>

### 深入阅读入口

- [apps/v4/app/globals.css](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/app/globals.css)
- [apps/v4/registry/styles/style-nova.css](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/styles/style-nova.css)
- [apps/v4/components/site-header.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/components/site-header.tsx)
- [apps/v4/components/page-header.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/components/page-header.tsx)
- [apps/v4/registry/bases/radix/blocks/dashboard-01/page.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/dashboard-01/page.tsx)
- [apps/v4/registry/bases/radix/blocks/dashboard-01/components/app-sidebar.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/dashboard-01/components/app-sidebar.tsx)
- [apps/v4/registry/bases/radix/blocks/dashboard-01/components/data-table.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/registry/bases/radix/blocks/dashboard-01/components/data-table.tsx)
- [apps/v4/app/(app)/examples/tasks/page.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/app/(app)/examples/tasks/page.tsx)
- [apps/v4/app/(app)/examples/tasks/components/data-table.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/app/(app)/examples/tasks/components/data-table.tsx)
- [apps/v4/app/(app)/examples/tasks/components/columns.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/app/(app)/examples/tasks/components/columns.tsx)
- [apps/v4/app/(app)/examples/tasks/components/data-table-toolbar.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/app/(app)/examples/tasks/components/data-table-toolbar.tsx)
- [apps/v4/examples/radix/field-responsive.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/examples/radix/field-responsive.tsx)
- [apps/v4/examples/radix/drawer-dialog.tsx](https://github.com/shadcn-ui/ui/blob/b8d7baa15b1dca5716f13153e63795fc3d741f6b/apps/v4/examples/radix/drawer-dialog.tsx)

联机房间的音乐播放器在自定义背景下沿用内容面板不透明度，封面取色渐变随背景一起变化，文字与播放控件保持清晰。
