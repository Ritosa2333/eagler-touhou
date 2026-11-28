# 网页启动器迁移基准

网页源码固定在 [85941f0d56](https://github.com/Ritosa2333/eagler-touhou/tree/85941f0d564cf10378d2af6b14d41a000bd27612) | 正式站 `touhouonline.com` | 2026-11-25。

[复现登记](web-baseline.json)列出实际地址、入口动作、视口和现有 Pencil 导出图。采集前读取正式站发布清单并核对提交号。截图使用干净的游客会话，坐标用 CSS px 记。每份元素清单记录读到的角色、文本、状态和位置。滚动、输入和返回的结果需要另行操作验证。

## 当前交付范围

桌面和手机均以以下四类界面为设计还原与交付范围：

- 书架。
- 从书架进入的游戏详情。
- 单作设置及必要的操作面板。
- 联机大厅。

导航、滚动、返回、动效和公共组件随这四类界面验收。游戏执行仍使用统一原生入口占位。资源导入和本机文件操作随详情与设置验收。

个人主页及全部账号功能统一使用正式站 `https://touhouonline.com`：登录、注册、找回密码、账号安全、资料编辑、头像和封面裁切、关注／粉丝／好友列表。旧 Flutter 页面、表单与裁切交互停止维护，迁移及归档状态见[账号网页入口](account.md)。共用身份、头像显示与必要的服务连接只随核心或配套功能维护。

消息（公告、私信、会话、未读）、云存档（管理、比较、上传恢复、冲突）和联机配套（等待室、座位准备、观战、聊天、快捷发言、语音、网络检查）继续支持，可按实际需要复用网页。网页登录会话与原生调用的衔接需实际验证。

其余独立页面停止原生维护，改用网页或归档。帮助、指南、关于、支持、服务状态与独立排行榜已转交系统浏览器，其旧实现见[归档登记](../archive/web-only/README.md)。共用主题、语言与导航设置随核心界面维护。

## 页面索引

编号用于连接需求、证据、实现和验收。下表包含核心界面、网页功能和历史对照，入口指向网页功能基准，当前覆盖状态见[验收表](acceptance.md)。

| 编号 | 模块 | 网页入口或状态 |
| --- | --- | --- |
| NAV | [全局导航](library.md) | 桌面顶栏、单机和联机入口、账号下拉、消息弹窗、手机三项底部导航、单机搜索、左侧抽屉、站点帮助 |
| I18N | 启动器语言 | 站点菜单、手机抽屉及主页设置，简体中文、英语、日语。详见 [NAV-05](library.md#外层导航) |
| LIB | [单机书架](library.md) | `/`、搜索、收藏、分类展开、封面和列表、作品编号正序和倒序、名称排序、最近游玩 |
| GAME | [游戏详情](game.md) | `/?game=th06` 等、收藏、最后运行日期、游玩时间、启动、导入、游戏设置、好友区域 |
| CLEAR | [通关和符卡](game.md) | 机体组和难度格、最高分弹出、存档来源与云上传、符卡收取图和列表 |
| CLOUD | [云存档](cloud-saves.md) | 账户及单作入口、版本与本机对照、文件选择、解析、上传、恢复、删除、冲突和自动同步 |
| GAME-SET | [单作设置](game.md) | 显示、输入、触控、音量、存档录像、高级、按键和手柄映射、资源大小和下载进度 |
| LOBBY | [联机大厅](lobby.md) | `/lobby.html`、选作、搜索、房间筛选、视图切换、房号、创建或加入、回放、排行榜、连接与空态 |
| ROOM | [联机房间](room.md) | 房号链接、座位、观战、准备、踢人、房间设定、延迟、消息、快捷语音、房主移交、离开和重连 |
| HOME | [手机主页](profile.md) | `/?section=home`、游客或账号卡、关系计数、启动器设置、主题和背景、账户入口 |
| APPEAR | [主题与背景](appearance.md) | 主题基底、内置纹样、本机图片、铺放、遮罩与容器透明度 |
| PROFILE | [个人主页与资料编辑](profile.md) | `/u/:username`、头像和封面、简介链接、关系列表、全作统计、单作进出、公开权限、个人资料编辑与裁切预览 |
| WHR | [花映塚评分与对局](th09-ratings.md) | 个人主页 WHR、名次、机体对照、对局与逐局录像，联机总榜及机体榜 |
| INBOX | [公告和私信](inbox.md) | `/?section=announcements`、`/?section=messages`、未读、发布、列表、会话、翻页、错误与重试 |
| AUTH | [账户](account.md) | `/login.html`、注册、找回密码、验证码、条款和退出 |
| SECURITY | [账户安全](account.md) | `/u/:username#security`、改邮箱、验证码、改密、撤销其他会话 |
| CONTENT | [帮助与站点](site-content.md) | 菜单、关于、FAQ、各作联机指南、隐私、条款、服务状态、迁移说明、反馈和支持 |
| RUNTIME | [游戏入口](game.md) | 单机启动、联机进入、录像播放、资源状态、退出游戏后返回发起页面。原生游戏执行为占位 |
| PLAT | [平台启动与系统配置](platforms.md) | Windows、macOS、Linux、Android、iOS 与 Chromebook 的原生入口和验收边界 |
| A11Y | [键盘与可访问性验收](acceptance.md) | 焦点、辅助标签、逐层关闭、减少动画、文本放大与触控区域，随各模块规格验收 |

## 逐项规格与截图

上表链接到各模块的逐项规格与操作证据。[截图对照](comparison.md)集中列出同状态网页、Pencil 和 Flutter 图像；[验收表](acceptance.md)区分已验证的行为、隔离样例及仍缺的设备或正式账号证据。

| 界面 | 桌面 | 手机 | Pencil 对照 |
| --- | --- | --- | --- |
| 书架 | [实拍](evidence/web/desktop-library.png) | [实拍](evidence/web/mobile-library.png) | [桌面](evidence/pencil/desktop-library.png)、[手机](evidence/pencil/mobile-library.png) |
| 游戏详情 | [实拍](evidence/web/desktop-game-detail.png) | [实拍](evidence/web/mobile-game-detail.png) | [手机](evidence/pencil/mobile-game-detail.png) |
| 登录 | [实拍](evidence/web/desktop-login.png) | [实拍](evidence/web/mobile-login.png) | 待补 |
| 注册 | [实拍](evidence/web/desktop-register.png) | [实拍](evidence/web/mobile-register.png) | 待补 |
| 公告 | [实拍](evidence/web/desktop-announcements-guest.png) | [实拍](evidence/web/mobile-announcements-guest.png) | [手机](evidence/pencil/mobile-announcement.png) |

导出稿中的样例账号、最近游玩和作品数量随测试数据而变。正式站游客截图使用干净存储。移动稿的部分导出包含整页，比较同一组件时以逻辑宽度和相同的滚动位置截取。[截图对照](comparison.md)记录相同视口下的网页、Pencil 和 Flutter 图像。

## 参考实现

FlClash 的页面容器把桌面侧栏和活动页的返回栈留在外层。本项目可从同一份目的地模型生成桌面侧栏和手机导航，具体页面使用 Flutter 布局与控件。[FlClash 页面容器](https://github.com/chen08209/FlClash/blob/afe7136002271fd64f098b154e7fff40be721eb8/lib/pages/home.dart) | [FlClash 侧栏](https://github.com/chen08209/FlClash/blob/afe7136002271fd64f098b154e7fff40be721eb8/lib/manager/app_manager.dart)
