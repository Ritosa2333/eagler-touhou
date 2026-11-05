/** This maintainer handbook is authored in Chinese; it is separate from user-facing locale catalogs. */
export const catalogLocale = 'zh-CN' as const;
export const catalogSections = [
  { id: 'foundations', title: '色彩与排版', group: '基础' },
  { id: 'actions', title: '操作与导航', group: '基础' },
  { id: 'fields', title: '输入与选择', group: '基础' },
  { id: 'settings', title: '设置组合', group: '组合' },
  { id: 'overlays', title: '弹层与菜单', group: '组合' },
  { id: 'feedback', title: '反馈与任务状态', group: '组合' },
  { id: 'replay', title: '列表与播放控制', group: '组合' },
  { id: 'navigation', title: '导航候选', group: '应用' },
  { id: 'modules', title: '页面拆解', group: '应用' },
  { id: 'boundaries', title: '差异与扩展边界', group: '应用' },
  { id: 'checks', title: '验收方式', group: '应用' },
] as const;

export const catalogColors = [
  ['background', '页面背景'],
  ['card', '内容表面'],
  ['popover', '菜单与浮层'],
  ['primary', '主要操作'],
  ['secondary', '次要操作与选中'],
  ['muted', '弱背景'],
  ['muted-foreground', '次要文字'],
  ['border', '内容边界'],
  ['brand', '品牌'],
  ['destructive', '危险操作'],
] as const;

export const catalogBoundaries = [
  {
    name: '栏目与分段选择',
    decision: '按用途区分',
    detail: '个人主页的栏目定位到同页区域，账户设置用页签切换表单。封面 / 列表、筛选和模式使用选择组。',
  },
  {
    name: '设置密度',
    decision: '已选分组列表',
    detail: '同一组设置共用边界和行距。独立卡片用于作品与独立实体，避免把每个开关做成卡片。',
  },
  {
    name: '弹窗载体',
    decision: '普通界面优先 Radix',
    detail: '游戏全屏和保存退出仍涉及原生顶层窗口。迁移前验证焦点、菜单宿主、关闭和返回协议。',
  },
  {
    name: '滑块提交',
    decision: '已选 Radix 滑块',
    detail: '触控灵敏度拖动预览，松手保存。录像松手才跳转，取消手势撤销预览。',
  },
  {
    name: '游戏画面与触控',
    decision: '独立几何',
    detail: '画面、摇杆和游戏按键按游戏布局计算。周围的工具栏、设置和菜单使用应用主题。',
  },
  {
    name: '状态与服务',
    decision: '由业务模块持有',
    detail: '账号会话、下载任务、录像跳转和联机连接各自管理状态。通用控件只接收值、状态和操作回调。',
  },
] as const;

export const catalogChecks = [
  { name: '整页布局', detail: '截图检查亮暗主题、横竖屏与高风险流程。部件状态在交互手册直接操作。' },
  { name: '控件状态', detail: '默认、悬停、键盘焦点、选中、禁用、加载、错误，以及长文本。' },
  { name: '弹层', detail: '打开、Esc、外部点击、嵌套选择、滚动和关闭后的焦点返回。' },
  { name: '触控', detail: '常用操作以 44 px 命中区为基准。检查滚动与拖动的归属、转屏和安全区。' },
  { name: '高风险流程', detail: '保存退出、录像跳转、输入捕获、全屏返回、房间身份切换和下载取消。' },
  { name: '扩展', detail: '新字段沿用 Field，新工具沿用工具栏与任务状态。模块验收后清理被替换的样式与交互。' },
] as const;
