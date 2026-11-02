/** This maintainer handbook is authored in Chinese; it is separate from user-facing locale catalogs. */
export const catalogLocale = 'zh-CN' as const;
export const catalogSections = [
  {id: 'foundations', title: '色彩与排版', group: '基础'},
  {id: 'actions', title: '操作与导航', group: '基础'},
  {id: 'fields', title: '输入与选择', group: '基础'},
  {id: 'settings', title: '设置组合', group: '组合'},
  {id: 'overlays', title: '弹层与菜单', group: '组合'},
  {id: 'feedback', title: '反馈与任务状态', group: '组合'},
  {id: 'replay', title: '列表与播放控制', group: '组合'},
  {id: 'navigation', title: '导航候选', group: '应用'},
  {id: 'modules', title: '模块用途', group: '应用'},
  {id: 'boundaries', title: '差异与扩展边界', group: '应用'},
  {id: 'checks', title: '验收方式', group: '应用'},
] as const;

export const catalogColors = [
  ['background', '页面背景'], ['card', '内容表面'], ['popover', '菜单与浮层'],
  ['primary', '主要操作'], ['secondary', '次要操作与选中'], ['muted', '弱背景'],
  ['muted-foreground', '次要文字'], ['border', '内容边界'], ['brand', '品牌'], ['destructive', '危险操作'],
] as const;

/** Application inventory, separate from the presentation recipes it composes. */
export const catalogModules = [
  {id: 'library', name: '游戏书库', area: '浏览', purpose: '查找作品、收藏和继续游玩。', parts: '封面 / 列表、搜索、筛选、排序、最近游玩', states: '长标题、缺失封面、无结果、无游玩记录', href: './'},
  {id: 'launch', name: '启动选项', area: '游玩', purpose: '为当前作品准备资源和运行设置。', parts: '大封面标题、设置分组、资源操作、启动按钮', states: '缺少 DATA、导入中、能力不可用、折叠与返回'},
  {id: 'bindings', name: '键盘与手柄', area: '游玩', purpose: '配置按键映射和本地玩家设备。', parts: '设备页签、绑定表、输入捕获、重置', states: '未连接、冲突、正在捕获、双人设备分配'},
  {id: 'touch', name: '触控工作台', area: '游玩', purpose: '调整触控方式、位置、尺寸和透明度。', parts: '设置字段、滑块、画布、预设、保存与取消', states: '拖动、缩放、转屏、安全区、取消手势'},
  {id: 'runtime', name: '游戏播放器', area: '游玩', purpose: '运行游戏，并承载游玩与观看操作。', parts: '游戏画面、全屏、触控、状态叠层', states: '资源准备、首帧、暂停、全屏与退出交接'},
  {id: 'replays', name: '录像管理器', area: '浏览', purpose: '管理本地录像，选择片段并回放。', parts: '文件工具栏、目录列表、文件菜单、播放器、时间线', states: '空目录、导入、重命名、移动、跳转与失败'},
  {id: 'directory', name: '多人大厅', area: '联机', purpose: '按作品查找、创建或加入房间。', parts: '作品筛选、房间列表、连接状态、建房与口令表单', states: '连接中、离线、无房间、满员、表单校验', href: './lobby.html'},
  {id: 'room', name: '多人房间', area: '联机', purpose: '组织成员、准备资源并进入同一局游戏。', parts: '席位、观战席、设置抽屉、聊天、网络信息、播放器', states: '两人 / 三人、准备、断线、身份切换、离房保存'},
  {id: 'spectating', name: '旁观与联机回放', area: '联机', purpose: '在实时画面与回看之间切换。', parts: '观看时间线、播放控制、回到直播、退出旁观', states: '追赶直播、落后、暂停、重连、横竖屏'},
  {id: 'profile', name: '玩家资料', area: '账户', purpose: '展示生涯、记录与内容，并管理展示范围。', parts: '资料头、栏目导航、记录列表、编辑表单、隐私开关', states: '长昵称、空记录、编辑草稿、隐藏栏目', href: './profile.html'},
  {id: 'account', name: '账户入口', area: '账户', purpose: '提供登录、注册和账户安全入口。', parts: '表单、字段错误、提交状态、恢复入口', states: '输入校验、提交中、服务错误、登录失效', href: './login.html', note: '现有页面 | 服务端账户与数据库后续接入'},
  {id: 'resources', name: '资源与更新', area: '支持', purpose: '导入、校验、下载和更新游戏资源。', parts: '文件选择、链接输入、任务进度、取消、重试', states: '校验中、下载失败、切换节点、完成与待更新'},
  {id: 'help', name: '帮助与说明', area: '支持', purpose: '在需要时解释安装、操作和恢复方法。', parts: '首次提醒、操作指南、错误详情、关于、常见问题', states: '首次打开、较长内容、外部链接、返回', href: './faq.html'},
] as const;

export const catalogBoundaries = [
  {name: '页签与分段选择', decision: '按用途区分', detail: '资料栏目用页签切换内容。封面 / 列表、筛选和模式用选择组，改变同一份内容的呈现。'},
  {name: '设置密度', decision: '已选分组列表', detail: '同一组设置共用边界和行距。独立卡片用于作品与独立实体，避免把每个开关做成卡片。'},
  {name: '弹窗载体', decision: '普通界面优先 Radix', detail: '游戏全屏和保存退出仍涉及原生顶层窗口。迁移前验证焦点、菜单宿主、关闭和返回协议。'},
  {name: '滑块提交', decision: '已选 Radix 滑块', detail: '触控灵敏度拖动预览，松手保存。录像松手才跳转，取消手势撤销预览。'},
  {name: '游戏画面与触控', decision: '独立几何', detail: '画面、摇杆和游戏按键按游戏布局计算。周围的工具栏、设置和菜单使用应用主题。'},
  {name: '状态与服务', decision: '由业务模块持有', detail: '账号会话、下载任务、录像跳转和联机连接各自管理状态。通用控件只接收值、状态和操作回调。'},
] as const;

export const catalogChecks = [
  {name: '外观矩阵', detail: '亮色 / 暗色 × 横屏 / 竖屏。固定数据与动画状态，比较同一模块。'},
  {name: '控件状态', detail: '默认、悬停、键盘焦点、选中、禁用、加载、错误，以及长文本。'},
  {name: '弹层', detail: '打开、Esc、外部点击、嵌套选择、滚动和关闭后的焦点返回。'},
  {name: '触控', detail: '常用操作以 44 px 命中区为基准。检查滚动与拖动的归属、转屏和安全区。'},
  {name: '高风险流程', detail: '保存退出、录像跳转、输入捕获、全屏返回、房间身份切换和下载取消。'},
  {name: '扩展', detail: '新字段沿用 Field，新工具沿用工具栏与任务状态。模块验收后清理被替换的样式与交互。'},
] as const;
