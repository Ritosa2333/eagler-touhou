# 云存档

原生入口包括桌面账号菜单、手机启动器设置和单作设置里的存档组。正式站个人主页内的管理由网页负责。单作设置带入该作品。确认前的文件草稿和本机比较数据仅保存在当前页面。

| 编号 | 元素和状态 | 布局与交互 |
| --- | --- | --- |
| CLOUD-01 | 页面、返回及权限 | 使用共用账户导航与桌面顶栏，资料与安全入口进入正式站应用内网页。保留返回、云存档标题和刷新。游客从本页登录并回到原入口。成功登录才读取私人目录。返回发起页面并沿用其状态。 |
| CLOUD-02 | 作品目录 | 平铺作品行与分隔线，展开行使用选中底色。行高至少 62 px，封面 36 px、圆角 5 px，标题与上传日期在旁。详情在桌面缩进 56 px，520 px 及以下缩进 4 px。无版本时通过共用紧凑选择器添加，桌面最大宽 280 px，手机沿用正文宽度，也可从单作设置预选。 |
| CLOUD-03 | 比较卡 | 本地存档与云端存档各用一张细边框卡。标题 14 px，数据 13 px；时间标签 11 px，时间值独立下一行，按设备的 12/24 小时偏好显示。宽屏并列等宽等高，手机堆叠，相隔 12 px。卡片内边距横向 16 px、纵向 14 px。两侧分别显示时长、次数、通关难度、最高分及可展开的文件清单。哈希相等时复用云端解析结果。解析本机存档可把当前作品的本机文件交给账号解析接口预览。th05 缺少通关记录时提示同时上传 `game.cfg`。 |
| CLOUD-04 | 选择文件 | 可选择原生目录中的已保存文件，或打开系统选择器多选。只有配置文件时提示主存档缺失，可继续从设备选取。显示主要文件、草稿文件大小和取消选择。校验文件名、重复项和大小。切换作品、账户栏目、顶栏单机或联机以及页面返回均确认放弃草稿。取消后可继续操作和再次返回。 |
| CLOUD-05 | 上传与冲突 | 提交文件和读取时的基准版本。替换已有版本前确认。服务器返回冲突时刷新版本，保留原草稿和提示。再次上传由用户发起。成功刷新游戏详情和自己的个人主页统计。 |
| CLOUD-06 | 下载与恢复 | 下载输出按版本命名的 ZIP。恢复到本机需确认，下载后核对允许的文件名、大小与每项 SHA-256。完整通过后写入，失败恢复旧文件。恢复仅作用于所选单机作品和当前语言。 |
| CLOUD-07 | 删除与失败 | 删除云端版本需确认，发送同一版本约束。删除不影响本机。首次加载、空目录和读取失败在作品区保留至少 54 px，重试动作沿正文左侧排列。续读失败、超额、无效文件、冲突和缺失主存档在当前区域提示。 |
| CLOUD-08 | 自动同步 | 入口显示当前不可用，等待原生游戏运行与真实存档写入周期。 |
| CLOUD-09 | 语言与窄屏 | 操作、比较字段、读写状态及确认提示随界面语言切换。320 px 英文中，两个云端操作同行，两个文件来源操作也同行。 |
| CLOUD-10 | 刷新本机比较 | 同一作品和存档路径刷新时，文件清单、展开状态和滚动位置保持稳定。文件内容变化时清除旧解析结果。读取期间暂停依赖本机文件的操作，结束后焦点回到刷新按钮。 |
| CLOUD-11 | th10 分语言恢复 | 先核对云版本的主文件和当前游戏语言。缺少对应文件时提示原因，仍可下载完整 ZIP。可返回「显示设置」切换游戏语言，再进入云存档恢复。恢复只替换匹配语言目录中的文件。 |

网页入口来自个人主页的账户游戏设置。Flutter 额外从单作存档设置打开同一页面。网页的自动同步依赖游戏运行数据，Flutter 对照验证手动传输链路。现有 Pencil 导出未包含云存档管理页。

验证输入使用固定 th07 目录与隔离账号服务。真实 th09 存档另用于 ZIP 校验及本机往返。Windows 与 Android 的系统文件选择器、正式账号写入、语言特有的实际存档样本和离线解析继续验收。 [截图对照](comparison.md#云存档三语)

延迟读取同一份 th07 文件，核对 320 和 1440 px 下刷新中与刷新后的文件清单位置。用等长内容替换主存档，再核对旧解析结果被清除。 [320 px 刷新中](evidence/flutter/mobile-narrow-cloud-refresh-en.png) | [320 px 刷新后](evidence/flutter/mobile-narrow-cloud-refreshed-en.png) | [1440 px 刷新中](evidence/flutter/desktop-cloud-refresh-en.png) | [1440 px 刷新后](evidence/flutter/desktop-cloud-refreshed-en.png)

320 px 英文和 1440 px 中文分别使用日文档和简中档的隔离云版本。从游戏设置进入，验证语言不匹配、完整 ZIP 导出、切换语言后恢复，以及另一个语言目录的文件字节一致。网页手动管理提供 ZIP 下载，恢复到本机是 Flutter 入口。样例使用实际 th10 目录和文件命名，档案字节为测试数据。 [320 px 提示](evidence/flutter/th10-cloud-language-320.png) | [1440 px 提示](evidence/flutter/th10-cloud-language-1440.png)

## 目录与比较排版

按固定版 `cloud-saves.css` 与比较卡样式核对，目录改为平铺作品行，移除比较卡外的整块卡片。添加作品复用共用选择器。相同 th07 云端样例用于下列前后图，手机比较卡沿用纵向排列。当前画面仍待视觉验收。

| 视口 | 整改前 | 当前调整 |
| --- | --- | --- |
| 1440 × 734 英文 | [截图](evidence/flutter/desktop-cloud-saves-en-layout-before.png) | [截图](evidence/flutter/desktop-cloud-saves-en-layout-after.png) |
| 320 × 640 英文 | [截图](evidence/flutter/mobile-narrow-cloud-saves-en-layout-before.png) | [截图](evidence/flutter/mobile-narrow-cloud-saves-en-layout-after.png) |

另用同一份支持 th07 的目录验证三种宽度下的游客、非本人资料、首次读取、读取失败和空目录。游客登录后返回原入口，空目录的菜单可用 Esc 关闭，读取失败可重试。 [320 px 空目录](evidence/flutter/mobile-narrow-settings-empty-en.png) | [390 px 读取失败](evidence/flutter/mobile-settings-failed-ja.png) | [桌面游客安全页](evidence/flutter/desktop-settings-guest-security-zh-CN.png)

云存档路由通过 GoRouter 的 `onExit` 复用页面自身的离开判断，回归覆盖重复取消和确认离开。应用内账号网页请求切回原生页面时，先关闭账号覆盖页，再由云存档页判断是否离开。取消或操作繁忙时，云存档页继续可用。资料编辑和账号安全已迁入正式站网页，原生旧页停止维护。

## th09 原档往返复核

重新定位到与既有 Windows 原作文件记录一致的回归样本：`score.dat` 为 3000 字节，`th9_01.rpy` 为 15562 字节。当前源码通过两项现有测试，覆盖带文件元数据和 SHA-256 校验的云存档 ZIP 恢复、本机字节读回，以及存档录像导入导出、重名处理、中文目录移动和单机／联机隔离。两份原档的校验值均未改变。[输入校验值与结果](evidence/windows/cloud-file-recheck.json)

复现时将 `TOUHOU_CAPTURE_TH09_SCORE`、`TOUHOU_CAPTURE_TH09_REPLAY` 指向这批原档，`TOUHOU_CAPTURE_SCRATCH` 指向项目源码外的专用临时目录。运行 `flutter test test/cloud_saves_test.dart --plain-name "verified cloud ZIP round-trips a real th09 save" --concurrency=1` 和 `flutter test tools/verify_game_files_test.dart --concurrency=1`。系统选择器和正式账号上传另作验收，附带的 th10、th15 样例只验证目录隔离与 ZIP 封装规则。
