# 联机大厅

大厅从手机底部导航和桌面“联机”进入。目录来源为站点发布清单中的联机地址，房间通过同一公开 WebSocket 目录读取。两端用当前发布作品限制列表，桌面和手机作品选单共用单机作品行。

| 编号 | 状态和元素 | 布局与验收 |
| --- | --- | --- |
| LOBBY-01 | 联机玩法、检查网络、排行榜 | 先显示三项支持入口。指南和独立榜单交给系统浏览器打开。检查网络打开当前 Relay 的连接和直连诊断。 |
| LOBBY-02 | 桌面侧栏 | 与单机共用 Prism 分类行、作品行、搜索和滚动容器。对齐数据见[书架侧栏规格](library.md#共用侧栏对齐)。联机顺序为对战、原作、格斗、旧作。圈选在封面左上角，点击作品正文单选，圈选可增减作品。 |
| LOBBY-03 | 手机选作 | 支持入口之后是列表与全部两个正方形按钮，后续封面横向滚动。作品简称在封面右下角。选单、封面和侧栏同用筛选状态。 |
| LOBBY-04 | 房间工具 | 筛选在最左侧，包含全部、可加入、等待开局、可观战四项和对应数量。右侧是建房、房间号、录像与刷新。窄屏用图标按钮单行呈现，名称供提示和辅助技术读取。 |
| LOBBY-05 | 房间列表 | 封面、作品日文名、难度、房号、阶段、旁观数和时间。中部按房间容量分配座位。双人对战留出中间的 VS 或比分，三人房保持等宽。窄屏从上到下呈现房间身份、规则、座位和动作。 |
| LOBBY-06 | 刷新与状态 | 初次加载与刷新使用固定高度的进度条。连接中断放在列表内部，并保留上次成功读取的房间。可手动重试，传输层会尝试发布清单中的后备地址。首次无房间和筛选无匹配有各自的操作。 |
| LOBBY-07 | 布局与返回 | 桌面侧栏与房间列表各自滚动，列表计数独立于滚动区。顶部和底部导航由外层持有。桌面与手机选择不同目的地再回联机时保留筛选，关闭弹窗回到房间列表。 |
| LOBBY-08 | 建房、房号和旁观入口 | 新房表单按作品的联机规则提供人数、难度与房间选项。加入弹窗接受 4 至 8 位数字及当前站点的邀请链接。粘贴后显示房号和作品，旁观邀请的确认按钮显示「旁观」。 [房间规格](room.md) |
| LOBBY-08.1 | 邀请直达与返回 | Windows 启动参数与 Android 应用链接可直达房间。旁观邀请沿用旁观入口。明确从大厅加入的邀请按房间状态选空位或旁观。退出回到进入前的大厅状态，已占用的房间在大厅处理。 [邀请直达](#邀请直达) |
| LOBBY-09 | 录像 | 选择作品后进入该作联机录像目录，复用本地录像管理和原生运行占位。联机对局归档、录像来源识别和跨作品导入仍需验证。 |
| LOBBY-10 | 超量目录 | 全部作品目录达到 Relay 返回上限后，单选作品走同一连接按作刷新。全部和多选从全部作品目录筛选。等待时保留能匹配的已知房间，固定进度条位置，等最新请求结束再判断空列表。 |
| LOBBY-11 | 退出旧房间 | Relay 报告本机已有房间时，在大厅工具上方显示房号。建房、房号加入、房间行的加入和观战暂停。按「退出原房间」后等待服务确认，期间保留提示。会话已更新或连接失败时可重新确认。旧服务显示说明。 |
| LOBBY-12 | 花映塚房间评分 | 联机席位显示 WHR 和局数。登录后，同交手组的单人招募房按评分差值排序。已注册玩家的头像打开公开评分卡。 [评分卡规格](th09-ratings.md#大厅评分卡) |

## 验收

房间列表采用 8 px 圆角和共用细边框，底色沿用原生面板调色板。三项帮助入口使用 12 px 文字和 16 px 间隔，窄宽时自动换行。断连提示与保留的房间在列表内部共用滚动，无房间时的连接提示也可滚动，避免放大字体撑出外框。重连期间显示固定进度条，成功后恢复房间操作。三种尺寸、三语、亮暗及双倍字体的 108 组对照核对侧栏或底部导航的像素、列表宽度、底边和房间内部几何。[窄屏修正前](evidence/flutter/lobby-layout-before.png) | [修正后](evidence/flutter/lobby-layout-after.png) | [断连大字号](evidence/flutter/lobby-disconnected-large-text.png)

旧房间提示也纳入高度边界：顶部内容过长时在自身区域滚动，房间面板至少留出 96 px 以容纳一个操作和房间计数。提示的顺序与操作含义一致，退出和等待文案共用按钮尺寸。320 px 英文、390 px 日文及桌面英文以双倍字体验证确认、等待、未确认、旧服务不支持和重试，另保留三种尺寸的中文常规字号流程。[旧房间大字号](evidence/flutter/lobby-membership-large-text.png)

- 正式站实际返回了公开目录，手动刷新保持连接。桌面、390 px 与 320 px 的截图采用同一份四房间样例，覆盖 live 和断连保留列表。现有 Pencil 导出缺少大厅画面。 [截图对照](comparison.md)
- 320 px 验证两种封面触控和选单联动、五项工具同行、房号长度、返回及断连重试。桌面滚动 15 条房间时侧栏搜索位置固定。回环 WebSocket 校验建立目录连接、请求刷新、解析发布作品和保留断连前的行。
- 320 px 英文使用网页固定版本生成的 th06 邀请链接。验证粘贴、确认旁观、进入房间及返回大厅。来源不同、重复参数、格式错误和建房链接均停留在输入框。 [邀请预览](evidence/flutter/mobile-narrow-room-invite.png)
- 原生对局和实时观战画面继续验收。

## 邀请直达

启动器接收正式站的 `/?j=…` 及旧版 `?game=th06mp&mpRoom=…`。Windows 将网页链接作为启动参数，Android 使用 `touhouonline://open/?j=…`。两端进入同一个房间页面。已占用旧房间时显示原因并允许返回大厅处理。重复收到当前房间链接时沿用已连接的会话。界面语言沿用使用者的偏好。

以固定网页版本的 th06 #4157 邀请和同一房间样例核对三个宽度。Flutter 测试覆盖冷启动、旁观、加入空位、重复链接、旧房间占用、错误回退及退出消息。从单机或主页接收链接后，退出房间回到原大厅，作品筛选保留。网页的运行资源准备区对应游戏运行功能，Flutter 仍使用原生运行占位。Pencil 的等待室与观战稿待补。Android 真机及系统 HTTPS 关联待验。 [Flutter 链接机制](https://docs.flutter.dev/ui/navigation/deep-linking)

| 视口 | 网页 | Flutter |
| --- | --- | --- |
| 320 × 640 | [截图](evidence/web/mobile-narrow-room-invite-direct.png) | [截图](evidence/flutter/mobile-narrow-room-invite-direct.png) |
| 390 × 844 | [截图](evidence/web/mobile-room-invite-direct.png) | [截图](evidence/flutter/mobile-room-invite-direct.png) |
| 1440 × 734 | [截图](evidence/web/desktop-room-invite-direct.png) | [截图](evidence/flutter/desktop-room-invite-direct.png) |

## 大厅网络检查

LOBBY-NET-01 从「检查网络」打开独立结果页。服务器组显示 WebSocket 回声与 TURN 中继，直连组显示 NAT 类别和公网 IPv6。四项独立更新。重试从当前 Relay 重新探测，关闭时释放探测连接。

网页从固定版本的正式站入口采集。Flutter 截图使用固定诊断结果来复现布局。Windows 首次探测正式站 TURN 超时。复测收到 WebSocket 83 ms、TURN 42 ms、NAT3，IPv6 不可用。正式站与隔离 TURN 均验证了双端中继链路的回声收发。现有 Pencil 导出未覆盖该结果页。

| 视口 | 网页 | Flutter |
| --- | --- | --- |
| 320 × 640 | [截图](evidence/web/mobile-narrow-network-check.png) | [截图](evidence/flutter/mobile-narrow-network-check.png) |
| 390 × 844 | [截图](evidence/web/mobile-network-check.png) | [截图](evidence/flutter/mobile-network-check.png) |
| 1440 × 734 | [截图](evidence/web/desktop-network-check.png) | [截图](evidence/flutter/desktop-network-check.png) |

## 超量目录

以固定版本的 Relay 目录模块生成 201 个公开房间。全部目录返回 200 个 th06 房间，单选 th09 后返回房间 #1048，回到全部恢复 200 / 201。回环测试覆盖快速换作品、刷新和断线后恢复筛选。

网页与 Flutter 使用同一房号场景。桌面、390 px 和 320 px 核对当前作品、房间标识、计数及刷新前后的工具栏。Flutter 使用固定高度的列表和本地主题。Pencil 未提供该状态。

| 视口 | 网页 | Flutter |
| --- | --- | --- |
| 320 × 640 | [截图](evidence/web/mobile-narrow-lobby-capped.png) | [截图](evidence/flutter/mobile-narrow-lobby-capped.png) |
| 390 × 844 | [截图](evidence/web/mobile-lobby-capped.png) | [截图](evidence/flutter/mobile-lobby-capped.png) |
| 1440 × 734 | [截图](evidence/web/desktop-lobby-capped.png) | [截图](evidence/flutter/desktop-lobby-capped.png) |

## 退出旧房间

本机联机身份按站点保存在启动器偏好中。读取房号与释放能力以当前 Relay 为准。释放请求绑定本次服务确认的旧会话，恢复信息留在内存。

固定版本 Relay 测试通过两个大厅实例复用同一份启动器偏好，验证识别 #4162 私人房间、退出后关闭原房间连接及其他房间继续连接。回环测试覆盖两次会话不同、连接中断和旧服务。

网页和 Flutter 使用相同房号及目录样例。窄屏操作位于文案下方，桌面与文案同行。三种宽度验证退出中保持卡片尺寸，确认前保留禁用状态。Pencil 未提供该状态。

| 视口 | 网页 | Flutter |
| --- | --- | --- |
| 320 × 640 | [截图](evidence/web/mobile-narrow-lobby-membership.png) | [截图](evidence/flutter/mobile-narrow-lobby-membership.png) |
| 390 × 844 | [截图](evidence/web/mobile-lobby-membership.png) | [截图](evidence/flutter/mobile-lobby-membership.png) |
| 1440 × 734 | [截图](evidence/web/desktop-lobby-membership.png) | [截图](evidence/flutter/desktop-lobby-membership.png) |
