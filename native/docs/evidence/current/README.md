# 截图采集登记

本目录 210 张图像于 2026-11-28 在资源校验移出界面线程后重新采集，覆盖书架、游戏详情、普通设置、放大字体与大厅。210 张同条件采集图与上一版逐字节一致，产品源码和采集入口已记录 SHA-256。这里是 Windows 控件渲染图，WebView2 网页区域的新图另见[账号验收](../../account.md#windows-真实网页区域)；图像登记不代表设计已获用户认可。个人主页截图已随旧原生实现归档。

读取旧的 `../flutter/` 图像时，按对应改动的历史证据处理。判断当前状态前先在 native 目录运行 `node tools/check-specs.mjs --current`。命令会校验当前图片及采集源码的 SHA-256，并核对 Windows 交付记录的包、AOT 与完整源码清单。图片或界面源码不匹配时应重拍，交付记录不匹配时应核对实际包及其构建输入，不能把旧记录当成当前界面。

普通设置使用完整作品目录、游客会话、空最近记录、暗色及正常字号，共 25 张。输入与音量另覆盖亮暗、100%／200% 字体及 320、390、781、1440 px，共 48 张。系统字体和图标由共用采集入口加载，明确使用 Windows 平台样式。设置存储与文件选择器使用隔离样例。采集信息及校验值见 [manifest.json](manifest.json)。

| 当前界面 | 桌面中文 | 手机中文 |
| --- | --- | --- |
| 显示 | [1440 × 734](game-settings/desktop-game-settings-display-zh-CN.png) | [390 × 844](game-settings/mobile-game-settings-display-zh-CN.png) |
| 输入 | [1440 × 734](game-settings/desktop-game-settings-input-zh-CN.png) | [390 × 844](game-settings/mobile-game-settings-input-zh-CN.png) |
| 音量 | [1440 × 734](game-settings/desktop-game-settings-volume-zh-CN.png) | [390 × 844](game-settings/mobile-game-settings-volume-zh-CN.png) |
| 高级 | [1440 × 734](game-settings/desktop-game-settings-advanced-zh-CN.png) | [390 × 844](game-settings/mobile-game-settings-advanced-zh-CN.png) |
| 存档与录像 | [1440 × 734](game-settings/desktop-game-files-zh-CN.png) | [390 × 844](game-settings/mobile-game-files-zh-CN.png) |

另有 320 px 英文、390 px 日文及 1440 px 英文的同五组图像。大厅包含 131 张连接、断连、空态及弹层图像，覆盖三语、亮暗及两种字体比例。会员、录像、评分和网络检查弹层使用隔离样例、Windows 平台样式及已加载的作品封面。书架与游戏详情各有 320、390 和 1440 px 首屏。完整登记见 [manifest.json](manifest.json)。未列入登记的图像继续按历史证据处理。

| 核心界面 | 桌面中文 | 手机中文 |
| --- | --- | --- |
| 书架 | [1440 × 734](core/desktop-library.png) | [390 × 844](core/mobile-library.png) |
| 游戏详情 | [1440 × 734](core/desktop-game-detail.png) | [390 × 844](core/mobile-game-detail.png) |
| 大厅 | [1440 × 734](lobby/desktop-lobby-live.png) | [390 × 844](lobby/mobile-lobby-live.png) |
