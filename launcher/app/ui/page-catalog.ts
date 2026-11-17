/** Chinese maintainer inventory. Locale variants share page boundaries. */
export interface CatalogPart {
  id: string;
  name: string;
  states: readonly string[];
}
export interface CatalogRegion {
  id: string;
  name: string;
  parts: readonly CatalogPart[];
  live?: string;
}
export interface CatalogPage {
  id: string;
  name: string;
  area: string;
  href: string;
  entryPath?: string;
  views: readonly string[];
  regions: readonly CatalogRegion[];
}
type Part = [id: string, name: string, states: string];
const region = (id: string, name: string, ...parts: Part[]): CatalogRegion => ({
  id,
  name,
  parts: parts.map(([id, name, states]) => ({ id, name, states: states.split('、') })),
});
export const catalogPages: readonly CatalogPage[] = [
  {
    id: 'library',
    name: '游戏书库',
    area: '浏览',
    href: './',
    views: ['封面 / 列表', '中文 / English / 日本語'],
    regions: [
      region(
        'find',
        '作品查找',
        ['search', '搜索与清除', '正常、无结果、长输入、键盘焦点'],
        ['category', '作品分类与收藏筛选', '全部、分类选中、空收藏；封面分类切换时作品淡入淡出并平滑移动；悬停和键盘焦点时图标与文字柔和移动'],
        ['sort', '排序与显示', '默认按 TH 数字编号递增排列，包含小数；TH 后接英文的同人作品排在数字编号之后；支持编号倒序、作品名称与封面 / 列表切换'],
      ),
      region(
        'shelf',
        '作品书库',
        ['cover', '大封面、作品名与英文副标题', '正常封面、缺失封面、长标题截断、卡片悬停浮起放大、列表悬停留白'],
        ['number', '作品名后的小号灰字编号', '亮暗对比、窄屏标题截断'],
        ['favorite', '收藏操作', '黄色已收藏星形、未收藏、点击回弹、键盘焦点'],
        ['recent', '最近游玩', '桌面紧凑封面与作品信息，悬停或聚焦时一起移动；手机横向排列方形封面，聚焦时轻微放大；封面与列表视图文字一致、无记录、打开作品选项'],
      ),
      region(
        'navigation',
        '站点导航',
        ['brand', '徽标与单机 / 联机入口', '顶栏与单机、联机内容边界对齐，桌面侧栏与主内容起点一致；页面切换采用柔和淡出淡入，导航文字与图标突出'],
        ['tools', '主题与账户入口', '圆角方形头像描线两端顺时针追逐并闭环、主题图标悬停轻转；明暗切换柔和渐变，封面短暂变暗后恢复'],
        ['more', '帮助、语言与偏好菜单', '菜单行高亮时平滑移动与渐变；弹窗短暂渐显渐隐，支持减少动态效果、Esc 与焦点返回'],
        ['background', '主题设置与本地背景图片', '可选择星点、方格、青海波或本地图片；联机房间同步背景和内容面板不透明度，玩家状态卡片保持不透明；切换预设保留图片，刷新后保留'],
      ),
    ],
  },
  {
    id: 'launch',
    name: '启动选项',
    area: '游玩',
    href: './?game=th10',
    views: ['普通版 / 多人版', '常用 / 高级设置'],
    regions: [
      region(
        'launch',
        '作品与启动',
        ['heading', '封面、作品名与返回', '长标题、缺失封面、关闭重开'],
        ['resources', '资源提示', '缺少 DATA、准备中、资源就绪、失败'],
        ['actions', '启动与导入', '可启动、准备中禁用、导入入口'],
      ),
      region(
        'files',
        '存档与录像',
        ['save', '存档上传 / 下载', '有存档、无存档、导入错误'],
        ['replay', '录像下载 / 管理', '有录像、空目录、打开管理器'],
        ['hint', 'Hint 上传 / 删除', '已有文件、无文件、删除确认'],
      ),
      region(
        'options',
        '游戏设置',
        ['display', '显示与高刷新率', '开 / 关、作品不支持、苹果提示'],
        ['input', '键盘与手柄入口', '正常、嵌套窗口、捕获按键'],
        ['language', '游戏语言与音乐', '选中、资源缺失、长选项'],
        ['advanced', '高级与触控分组', '折叠 / 展开、作品能力差异、窄屏'],
      ),
      {...region('th05-music', '怪绮谈音源',
        ['source', '原版 / 幺乐团专辑', '原版、专辑、无音乐、偏好恢复、资源缺失'],
      ), live: './?game=th05'},
      {...region('th05-display', '怪绮谈联机显示',
        ['boss-bar', '血条与阶段提示', '原版侧栏、顶部、环形、关联选项禁用'],
        ['meters', '梦槽 / 火力数字', '开启、关闭、偏好恢复、普通版不显示'],
      ), live: './?game=th05mp'},
    ],
  },
  {
    id: 'bindings',
    name: '键盘与手柄',
    area: '游玩',
    href: './?game=th10',
    entryPath: '作品选项 | 输入设置',
    views: ['键盘改键', '手柄改键', '本地玩家设备分配'],
    regions: [
      region(
        'devices',
        '设备与预设',
        ['tabs', '键盘 / 手柄页签', '选中、切换、窄屏'],
        ['preset', '键位布局与配置场景', '预设、自定义、菜单展开'],
        ['connection', '连接设备与玩家分配', '无设备、已连接、设备断开、设备冲突'],
      ),
      region(
        'mapping',
        '动作映射',
        ['bindings', '动作行与按键', '默认、长键名、多键、未绑定'],
        ['capture', '按键捕获', '等待输入、取消、冲突、恢复原键位'],
        ['reset', '预设恢复与保存布局', '已修改、保存、恢复预设'],
      ),
      region(
        'window',
        '窗口与返回',
        ['frame', '固定标题与内部滚动', '长映射表、短横屏、转屏'],
        ['menu', '嵌套选择菜单', '打开、Esc、外部关闭'],
        ['close', '关闭与焦点返回', '捕获中、菜单展开、返回选项栏'],
      ),
    ],
  },
  {
    id: 'touch',
    name: '触控工作台',
    area: '游玩',
    href: './?game=th10',
    entryPath: '作品选项 | 触控设置',
    views: ['操作方式', '按键布局', '位置与尺寸'],
    regions: [
      region(
        'options',
        '触控选项',
        ['movement', '移动方式与预设', '摇杆、跟随触摸、禁用'],
        ['sliders', '灵敏度与透明度', '拖动预览、松手提交、取消手势'],
        ['actions', '按住 / 切换与功能键', '按住、切换、作品能力差异'],
      ),
      {
        ...region(
          'canvas',
          '布局画布',
          ['controls', '摇杆与游戏按键', '选中、隐藏、拖动、缩放'],
          ['geometry', '位置、尺寸与边界', '横屏、竖屏、安全区、越界约束'],
        ),
        live: './?game=th10',
      },
      region(
        'draft',
        '草稿与退出',
        ['save', '保存 / 重置 / 取消', '未修改、已修改、重置确认'],
        ['discard', '放弃修改确认', '继续编辑、放弃、返回选项栏'],
      ),
    ],
  },
  {
    id: 'runtime',
    name: '游戏播放器',
    area: '游玩',
    href: './?game=th10',
    entryPath: '作品选项 | 启动',
    views: ['普通游戏', '全屏 / 窗口', '触控 / 键盘 / 手柄'],
    regions: [
      {
        ...region(
          'viewport',
          '运行区域',
          ['screen', '游戏画面与缩放', '准备中、首帧、暂停、退出全屏'],
          ['touch', '触控叠层', '横屏、竖屏、隐藏、外接设备'],
        ),
        live: './?game=th10',
      },
      region(
        'tools',
        '会话工具',
        ['controls', '暂停、输入与返回', '运行、暂停、嵌套设置'],
        ['saving', '保存与退出交接', '保存中、保存失败、留在游戏、退出'],
        ['diagnostics', '诊断与错误', '连接等待、资源失败、复制诊断'],
      ),
    ],
  },
  {
    id: 'replays',
    name: '录像管理器',
    area: '浏览',
    href: './?game=th10',
    entryPath: '作品选项 | 录像 | 管理',
    views: ['目录管理', '文件操作', '播放工作区'],
    regions: [
      region(
        'files',
        '文件区',
        ['folders', '目录与导入', '空目录、文件校验'],
        ['selection', '关卡展开与文件菜单', '文件展开、菜单展开'],
        ['operations', '重命名、移动与删除', '重名、无效名称、移动、删除确认'],
      ),
      region(
        'playback',
        '播放区',
        ['controls', '暂停、继续与速度', '播放、暂停、倍速、重播'],
        ['seek', '进度条与选段', '拖动预览、提交跳转、跳转中、跨关'],
        ['return', '轮次与返回', '普通录像、联机录像、短横屏、返回目录'],
      ),
    ],
  },
  {
    id: 'directory',
    name: '多人大厅',
    area: '联机',
    href: './lobby.html',
    views: ['作品筛选', '房间列表', '创建 / 加入'],
    regions: [
      region(
        'filters',
        '作品与房间筛选',
        ['games', '作品图标与完整列表', '全部联机无需勾选；悬停或键盘聚焦时作品图文柔和移动；单作圆环过渡为绿色粗勾，含描画动效、黄色收藏星形和手机菜单；收藏排序时作品与对应房间平滑移动'],
        ['filters', '房间筛选', '桌面和手机工具栏左侧，菜单向右展开；筛选作品或房间时列表平滑移动，返回全部联机时房间直接恢复并柔和淡入'],
      ),
      region(
        'rooms',
        '房间列表',
        ['identity', '正方形封面与日文作品名', '长标题截断、缺失封面、房间号与观众信息'],
        ['rules', '房间玩法', '独立玩法列：无特殊规则时显示常规，否则显示禁用作弊移动或挑战模式；窄屏置于房间信息下方'],
        ['count', '列表内结果计数', '底部对齐、单行列表、末条操作可见、无结果'],
        ['arrival', '房间进入与关闭', '新房间柔和淡入上移，关闭房间淡出移除；首屏和已有房间更新保持稳定'],
        ['members', '玩家头像、姓名与操作方式', '圆形头像和空席占位，姓名下方显示操作方式；两人 / 三人按人数均匀排列，窄屏适当缩小头像'],
        ['facts', '标题后的难度与房间状态', '状态与加粗房间号统一配色：招募中为绿色，已满与游戏中为红色；状态文字左对齐'],
        ['duel', 'th09 对战座位与比分', '双方座位对齐三人房的左右两列，比分或 VS 同行显示；桌面略向左偏，手机居中'],
        ['entry', '加入 / 旁观操作', '可加入、已满、游戏中、断线'],
      ),
      region(
        'entry',
        '创建与加入',
        ['create', '建房与房间代码', '正常、空值、无效代码、连接中；房间加载和刷新时右上角刷新图标旋转，录像入口位于顶栏菜单'],
        ['help', '联机说明与网络检查', '打开、检查中、服务不可用、取消'],
      ),
    ],
  },
  {
    id: 'room',
    name: '联机房间',
    area: '联机',
    href: './lobby.html?game=th10mp',
    entryPath: '联机大厅 | 创建 / 加入房间',
    views: ['房主 / 成员 / 观战', '两人 / 三人', '等待 / 游戏中'],
    regions: [
      region(
        'seats',
        '玩家席位与准备状态',
        ['status', '三色卡片与艺术字', '加载、未准备、已准备、重连、空席'],
        ['identity', '昵称、角色与操作方式', '本人、其他成员、长昵称、两人 / 三人'],
        ['resources', '资源环与进度', '准备中、已完成、资源失败'],
      ),
      region(
        'management',
        '玩家管理',
        ['menu', '三个点管理菜单', '房主、普通成员、空席、游戏中'],
        ['transfer', '移到观战席 / 移除玩家', '允许操作、权限受限、操作后空席'],
        ['personal', '本人角色与昵称', '角色切换、起身、未入座、昵称过长'],
      ),
      region(
        'chat',
        '聊天与快捷发言',
        ['messages', '消息记录', '圆形头像、昵称在消息上方；玩家与观众标记、无头像回退；举报时填写理由；游戏内消息区限五条标准消息的高度'],
        ['phrases', '快捷发言菜单与箭头', '展开、Esc、外部关闭、断线禁用'],
      ),
      region(
        'spectators',
        '观战席',
        ['list', '观众头像与完整名单', '空名单、多名观众、本人、长昵称'],
        ['membership', '加入 / 离开观战席', '玩家转观战、观战退出、尚未同步'],
        ['moderation', '观众管理', '房主可移除、成员只读、游戏中'],
      ),
      region(
        'settings',
        '房间设置',
        ['basic', '可见性、人数与难度', '房主可编辑、成员只读、私密、两人 / 三人'],
        ['timing', '回滚、延迟与规则', '自动、手动延迟、开关选中、长选项'],
      ),
      {...region('th05-stock', '怪绮谈房间设置',
        ['stock', '初始残机 / Bomb', '默认 2 / 3、零值、房主可编辑、成员只读、开局后禁用、修改后重新准备'],
      ), live: './lobby.html?game=th05mp'},
      region(
        'network',
        '网络详情',
        ['links', '直连 / TURN 指标', '检查中、已连接、不可用、重连'],
        ['retry', '重试与连接检查', '可重试、检查中、断线'],
      ),
      region(
        'session',
        '准备、启动与离房',
        ['preparation', '本人资源准备', '准备中、失败重试、取消、就绪'],
        ['ready', '准备与启动', '尚未准备、已准备、等待成员、可启动'],
        ['leave', '返回与保存交接', '房间等待、游戏中、保存中、留在游戏'],
      ),
    ],
  },
  {
    id: 'spectating',
    name: '旁观与联机回放',
    area: '联机',
    href: './lobby.html?game=th10mp',
    entryPath: '联机大厅 | 进行中的房间 | 加入旁观',
    views: ['实时旁观', '回看', '联机录像'],
    regions: [
      region(
        'playback',
        '观看控制',
        ['timeline', '时间线与关卡标记', '追赶、落后、暂停、定位'],
        ['live', '暂停 / 继续 / 回到直播', '直播、回看、定位中、等待新数据'],
        ['exit', '退出旁观与全屏', '嵌入、全屏、横竖屏、返回房间'],
      ),
      region(
        'connection',
        '身份与连接',
        ['membership', '观众席与资源准备', '加入、资源准备、退出、换局'],
        ['recovery', '断线与返回', '重连、保存交接、身份切换'],
      ),
    ],
  },
  {
    id: 'profile',
    name: '玩家资料与账户设置',
    area: '账户',
    href: './profile.html',
    entryPath: '玩家主页 | 账户设置 | 编辑资料 / 隐私与个性化 / 账户与安全',
    views: ['公开主页', '本人 / 访客', '账户设置'],
    regions: [
      region(
        'identity',
        '玩家资料与栏目',
        ['hero', '头像、昵称与简介', '圆形头像、未填写、长昵称、本人、访客只读'],
        ['anchors', '栏目导航与顺序', '隐藏栏目、调整顺序、锚点、前进 / 后退'],
        ['records', '收藏与游玩记录', '空记录、有记录、长列表'],
      ),
      region(
        'career',
        '生涯通关图',
        ['map', '作品、难度与机体', '未读取、未通关、通关、1CC、展开与收起'],
        ['tools', '缩放、筛选与全屏', '放大、缩小、适应屏幕、窄屏'],
        ['editing', '编辑与本地备份', '本人编辑、访客只读、未保存、导入校验、取消'],
      ),
      region(
        'social',
        '内容与好友',
        ['draft', '内容草稿', '空内容、编辑、空白校验、保存、访客只读'],
        ['friends', '好友与关注', '空列表、切换、提醒开关、上线提醒'],
      ),
      region(
        'settings',
        '账户设置',
        ['profile', '资料编辑', '未保存、校验、保存反馈、取消'],
        ['privacy', '隐私与个性化', '可见 / 隐藏、栏目顺序、访客预览'],
        ['security', '账户与安全', '未绑定、表单校验、服务尚未接入'],
      ),
    ],
  },
  {
    id: 'account',
    name: '登录与注册',
    area: '账户',
    href: './login.html',
    entryPath: '账户预览 | 服务尚未接入',
    views: ['登录', '注册'],
    regions: [
      region(
        'form',
        '账户表单',
        ['mode', '登录 / 注册切换', '登录、注册、窄屏'],
        ['credentials', '标识与密码字段', '空值、无效输入、密码确认'],
        ['submit', '提交与反馈', '提交中、服务错误、服务尚未接入'],
      ),
      region(
        'navigation',
        '返回与语言',
        ['return', '返回个人页', '保留语言、保留返回地址'],
        ['language', '界面语言', '中文、English、日本語、长标签'],
      ),
    ],
  },
  {
    id: 'resources',
    name: '资源与更新',
    area: '支持',
    href: './?game=th10',
    entryPath: '作品选项 | 导入 / 资源操作',
    views: ['导入游戏包', '下载与节点回退', '程序更新'],
    regions: [
      region(
        'import',
        '资源输入',
        ['file', '文件 / 链接输入', '未选择、错误文件、校验中、重新选择'],
        ['validation', '类型与校验反馈', '支持格式、资源不匹配、导入失败'],
      ),
      region(
        'transfer',
        '下载任务',
        ['progress', '进度、速度与节点', '未知总量、慢传输、节点回退、完成'],
        ['recovery', '取消与重试', '下载中、已取消、失败、重试'],
        ['update', '程序更新入口', '已是最新、更新可用、游戏运行中'],
      ),
    ],
  },
  {
    id: 'help',
    name: '帮助与首次提醒',
    area: '支持',
    href: './faq.html',
    views: ['常见问题', '关于', '首次须知', '兼容帮助'],
    regions: [
      region(
        'content',
        '说明正文',
        ['typography', '标题、正文与列表', '长文、长链接、窄屏、无脚本'],
        ['navigation', '语言与返回入口', '中文、English、日本語、跨页返回'],
      ),
      region(
        'notices',
        '站内提醒',
        ['notice', '首次须知、捐赠与苹果提示', '打开、内部滚动、关闭、焦点返回'],
        ['error', '错误详情与诊断', '简短错误、长诊断、复制反馈'],
      ),
      {
        ...region(
          'compatibility',
          '浏览器兼容',
          ['result', '检测结果与建议', '检测通过、能力缺失、旧浏览器'],
          ['return', '帮助与返回', '三语、窄屏、无脚本'],
        ),
        live: './compatibility.html',
      },
    ],
  },
  {
    id: 'status',
    name: '服务运行状态',
    area: '支持',
    href: './uptime/',
    views: ['24 小时', '7 天', '90 天'],
    regions: [
      region(
        'history',
        '历史与服务',
        ['range', '时间范围与服务分组', '24 小时、7 天、90 天、空列表'],
        ['service', '状态、历史条与指标', '正常、异常、未知、读取失败'],
        ['retry', '刷新与重试', '读取中、超时、失败重试、后台暂停'],
      ),
      region(
        'navigation',
        '站点导航',
        ['tools', '徽标、主题与帮助', '亮色、暗色、窄屏、语言'],
        ['return', '跨页返回', '切范围保留列表、回到启动器'],
      ),
    ],
  },
  {
    id: 'migration',
    name: '旧站数据迁移',
    area: '支持',
    href: './migrate.html',
    views: ['来源检查', '授权与复制', '迁移结果'],
    regions: [
      region(
        'source',
        '来源与数据',
        ['origin', '源站与授权', '来源不允许、来源可用、等待授权'],
        ['summary', '存档与录像统计', '无数据、有数据、部分可用'],
      ),
      {
        ...region(
          'copy',
          '复制与结果',
          ['progress', '迁移进度', '等待、复制中、完成'],
          ['recovery', '失败与重试', '部分失败、全部失败、重试、返回'],
        ),
        live: './migrate.html',
      },
    ],
  },
];
export function catalogAnchor(page: string, region?: string, part?: string, state?: number): string {
  return ['page', page, region, part, state === undefined ? undefined : `state-${state}`].filter(Boolean).join('-');
}
export function catalogTarget(hash: string) {
  let id: string;
  try {
    id = decodeURIComponent(hash.replace(/^#/, ''));
  } catch {
    return null;
  }
  if (id === 'room-components') id = 'page-room';
  for (const page of catalogPages) {
    if (id === catalogAnchor(page.id)) return { page, id };
    for (const region of page.regions) {
      if (id === catalogAnchor(page.id, region.id) || id === `example-${page.id}-${region.id}`)
        return { page, region, id };
      for (const part of region.parts) {
        if (id === catalogAnchor(page.id, region.id, part.id)) return { page, region, part, id };
        const state = part.states.findIndex((_, index) => id === catalogAnchor(page.id, region.id, part.id, index));
        if (state >= 0) return { page, region, part, state, id };
      }
    }
  }
  return null;
}
