/** Chinese maintainer inventory. Locale variants share the same page contract. */
export interface CatalogPage {
  id: string; name: string; area: string; href: string; entryPath?: string; views: readonly string[];
  regions: readonly {name: string; parts: string; states: string; sample: string}[];
}
export const catalogPages: readonly CatalogPage[] = [
  {id: 'library', name: '游戏书库', area: '浏览', href: './', views: ['中文 / English / 日本語', '封面 / 列表'], regions: [
    {name: '作品查找', parts: '搜索、作品分类、收藏、排序、视图选择', states: '无结果、长标题、排序切换、键盘焦点', sample: '#actions'},
    {name: '书库', parts: '大封面、作品名、收藏按钮、最近游玩', states: '缺失封面、没有记录、选中作品、打开和关闭选项', sample: '#navigation-current'},
    {name: '站点导航', parts: '徽标、单机与联机、主题、账户、帮助菜单', states: '亮暗、窄屏、语言选中、Esc 和焦点返回', sample: '#navigation'},
  ]},
  {id: 'launch', name: '启动选项', area: '游玩', href: './?game=th10', views: ['普通版 / 多人版', '常用 / 高级设置'], regions: [
    {name: '作品与启动', parts: '大封面标题、返回书库、资源提示、启动操作', states: '缺少 DATA、准备中、返回后位置一致', sample: '#feedback'},
    {name: '选项栏', parts: '存档、录像、显示、输入、语言与音乐、触控、高级分组', states: '折叠、禁用、不同能力、并列选项和长文案', sample: '#settings'},
    {name: '资源工具', parts: '导入文件、资源状态、下载和更新入口', states: '校验、取消、失败、重试', sample: '#feedback'},
  ]},
  {id: 'bindings', name: '键盘与手柄', area: '游玩', href: './?game=th10', entryPath: '作品选项 | 输入设置 | 键盘改键 / 手柄改键', views: ['键盘改键', '手柄改键', '本地玩家设备分配'], regions: [
    {name: '设备与预设', parts: '设备页签、键位布局、配置场景、连接设备', states: '无设备、切换预设、窄屏菜单、转屏', sample: '#settings'},
    {name: '映射表', parts: '动作行、按键捕获、冲突提示、重置', states: '捕获中、取消捕获、长键名、重复映射', sample: './?game=th10'},
    {name: '窗口', parts: '固定标题、内部滚动、关闭和焦点返回', states: '嵌套菜单打开、关闭后选项栏几何一致', sample: '#overlays'},
  ]},
  {id: 'touch', name: '触控工作台', area: '游玩', href: './?game=th10', entryPath: '作品选项 | 触控设置', views: ['操作方式', '按键布局', '位置与尺寸'], regions: [
    {name: '触控选项', parts: '移动方式、灵敏度、透明度、预设', states: '拖动预览、松手提交、取消手势', sample: '#fields'},
    {name: '布局画布', parts: '摇杆、按键、边界、位置和尺寸工具', states: '选中、拖动、缩放、转屏、安全区', sample: './?game=th10'},
    {name: '草稿', parts: '保存、重置、取消、放弃修改确认', states: '未修改、已修改、确认返回、取消不写盘', sample: '#overlays'},
  ]},
  {id: 'runtime', name: '游戏播放器', area: '游玩', href: './?game=th10', entryPath: '作品选项 | 启动', views: ['普通游戏', '全屏 / 窗口', '触控 / 键盘 / 手柄'], regions: [
    {name: '运行区域', parts: '游戏画面、缩放、全屏、触控叠层', states: '首帧、暂停、横竖屏、退出全屏', sample: './?game=th10'},
    {name: '会话工具', parts: '暂停、输入、返回、保存交接、诊断', states: '资源失败、保存失败、继续游戏、退出后清理', sample: '#feedback'},
  ]},
  {id: 'replays', name: '录像管理器', area: '浏览', href: './?game=th10', entryPath: '作品选项 | 录像', views: ['目录管理', '文件操作', '播放工作区'], regions: [
    {name: '文件区', parts: '目录、搜索、导入、选择、文件菜单', states: '空目录、重命名错误、移动、删除确认', sample: '#replay'},
    {name: '播放区', parts: '画面、暂停与继续、速度、进度条、轮次和返回', states: '拖动预览、提交跳转、跨关、重播、短横屏', sample: '#replay'},
  ]},
  {id: 'directory', name: '多人大厅', area: '联机', href: './lobby.html', views: ['作品筛选', '房间列表', '创建 / 加入'], regions: [
    {name: '作品与房间', parts: '作品筛选、房间行、成员、旁观人数、连接状态', states: '空列表、断线、满员、长昵称', sample: '#feedback'},
    {name: '房间入口', parts: '建房按钮、房间代码表单、联机说明、网络检查', states: '输入错误、连接中、取消、服务不可用', sample: '#fields'},
  ]},
  {id: 'room', name: '多人房间', area: '联机', href: './lobby.html?game=th10mp', entryPath: '联机大厅 | 创建 / 加入房间', views: ['等待与准备', '聊天 / 房间设置', '观战席 / 网络详情'], regions: [
    {name: '玩家席位', parts: '三色卡片、资源环、艺术字、透明管理菜单', states: '加载、重连、未准备、已准备、空席、两人 / 三人', sample: '#room-components'},
    {name: '聊天与观战', parts: '快捷发言、SVG 箭头、消息、观战列表和身份操作', states: '菜单展开、Esc、外部关闭、移到观战席、加入 / 离开', sample: '#room-components'},
    {name: '房间设置', parts: '可见性、人数、难度、回滚与延迟、规则开关', states: '房主可编辑、成员只读、长选项、窄屏', sample: '#room-components'},
    {name: '网络与运行', parts: '连接详情、重试、准备、启动、离房和保存交接', states: '检查中、已连接、不可用、开局和返回', sample: '#room-components'},
  ]},
  {id: 'spectating', name: '旁观与联机回放', area: '联机', href: './lobby.html?game=th10mp', entryPath: '联机大厅 | 选择进行中的房间 | 旁观', views: ['实时旁观', '回看', '联机录像'], regions: [
    {name: '观看控制', parts: '观看时间线、暂停、速度、回到直播、退出旁观', states: '追赶、落后、暂停、定位、回到直播', sample: '#replay'},
    {name: '身份与连接', parts: '观众席、资源准备、断线提示、换局与返回', states: '加入、重连、保存交接、身份切换', sample: '#room-components'},
  ]},
  {id: 'profile', name: '玩家资料与账户设置', area: '账户', href: './profile.html', entryPath: '玩家主页 | 账户设置 | 编辑资料 / 隐私与个性化 / 账户与安全', views: ['概览 / 生涯 / 游玩记录 / 内容 / 好友', '编辑资料', '隐私与个性化', '账户与安全'], regions: [
    {name: '玩家资料', parts: '头像、昵称、简介、栏目页签、最近游玩与空状态', states: '长昵称、空记录、隐藏栏目、窄屏', sample: '#actions'},
    {name: '生涯图', parts: '作品与难度网格、记录编辑、缩放、筛选、全屏', states: '选中格子、草稿、取消不写盘、长表滚动', sample: '#fields'},
    {name: '账户设置', parts: '资料表单、隐私开关、密码和安全表单', states: '未保存草稿、校验、保存反馈、亮暗主题', sample: '#settings'},
  ]},
  {id: 'account', name: '登录与注册', area: '账户', href: './login.html', views: ['登录', '注册'], regions: [
    {name: '账户表单', parts: '模式页签、标识、密码、错误和提交', states: '空值、无效输入、提交中、服务错误', sample: '#fields'},
    {name: '导航', parts: '返回、登录与注册切换、语言', states: '保留查询参数、返回资料页；账户后端后续接入', sample: '#actions'},
  ]},
  {id: 'resources', name: '资源与更新', area: '支持', href: './?game=th10', entryPath: '作品选项 | 导入游戏包 / 资源操作', views: ['导入游戏包', '下载与节点回退', '程序更新'], regions: [
    {name: '资源输入', parts: '文件选择、链接输入、类型和校验反馈', states: '错误文件、校验中、取消、重新选择', sample: '#fields'},
    {name: '任务', parts: '下载进度、节点、速度、取消、重试和完成', states: '失败回退、慢传输、下载取消、更新可用', sample: '#feedback'},
  ]},
  {id: 'help', name: '帮助与首次提醒', area: '支持', href: './faq.html', views: ['常见问题', '关于', '首次使用须知', '中文 / English / 日本語兼容帮助'], regions: [
    {name: '说明正文', parts: '标题层级、列表、链接、语言导航与返回', states: '长文、无脚本、亮暗、窄屏、语言传递', sample: './about.html'},
    {name: '站内提醒', parts: '首次须知、捐赠、苹果刷新率提示、错误详情', states: '打开、滚动、关闭、复制诊断与焦点返回', sample: '#overlays'},
    {name: '浏览器兼容', parts: '诊断结果、操作建议、返回入口', states: '检测通过 / 不通过、旧浏览器、三语', sample: './compatibility.html'},
  ]},
  {id: 'status', name: '服务运行状态', area: '支持', href: './uptime/', views: ['24 小时', '7 天', '90 天'], regions: [
    {name: '历史与服务', parts: '历史范围、服务分组、状态、历史条、响应和可用率', states: '读取中、正常、异常、未知、空列表、失败重试', sample: './uptime/'},
    {name: '站点导航', parts: '共用顶栏、三语、主题、帮助与偏好', states: '切范围保留列表、后台暂停、跨页返回', sample: '#navigation-current'},
  ]},
  {id: 'migration', name: '旧站数据迁移', area: '支持', href: './migrate.html', views: ['来源检查', '授权与复制', '迁移结果'], regions: [
    {name: '授权与数据', parts: '源站确认、存档与录像统计、授权、复制和结果', states: '来源不允许、无数据、部分失败、重试；实际迁移需单独验收', sample: './migrate.html'},
  ]},
];
