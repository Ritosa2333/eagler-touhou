# TH02 主项目适配

`ports/th02` 将《东方封魔录》的 PC-98 DOSBox-X 运行时接入 Eagler Touhou。主项目负责 Package Store 安装、`/savesth02` 与 `/savesth02mp` 文件空间、房间成员、准备状态和开局；运行时继续使用封魔录的原生盘片与游戏逻辑。

在 monorepo 根目录运行：

```powershell
npm --prefix launcher run build:ui-rewrite
node ports/th02/scripts/dev.mjs
```

打开 `http://127.0.0.1:18320/?test=th02`。脚本从相邻 `../th02-coop-lab` 构建运行时及本地 `content/th02.data`，并启动主 Launcher 与 Relay。主项目存档文件为 `HUUHI.DAT`，设置文件为 `HUUMA.CFG`；它们从游戏磁盘 `HUMAROKU` 导出。合作模式支持两人和三人，保留原项目的三种机体射击配置。

可重复验证：

```powershell
node ports/th02/tests/contracts.mjs
node ports/th02/tests/hit-clear-content.mjs
node ports/th02/tests/transport.mjs
node ports/th02/tests/browser.mjs
node ports/th02/tests/launcher-browser.mjs
node ports/th02/tests/launcher-room-browser.mjs
```

`browser.mjs` 使用真实模拟器、WASM 和 DATA 检查单人、联机输入同步、Relay 与资源加载；Launcher 测试从主项目验证安装和房间流程。`PC98_TEST_PLAYERS=3`、`PC98_TEST_RELAY=1`、`PC98_TEST_ROLLBACK=1` 可切换运行人数、Relay 路由和回滚配置。日志与截图写入 `.cache/`。

入口仍标记为 `testOnly`。适配由主项目提供房间与存档界面，DOSBox-X 内继续运行原来的封魔录。原实验仓库不被构建脚本修改。来源提交与 Git 子树状态见 [迁移登记](../../docs/migration/th02-eagler-20261025.md)。

本地验收覆盖主 Launcher 单人安装与呈现、双人建房与加入、座位、预载、游戏检查、准备开局、键盘按下与释放、协议结束后同房再开，以及离开房间。真实 DOSBox-X 三人 Relay 回滚模式已推进超过 480 个确认帧。游戏原生暂停菜单退出、移动设备与远程网络另作验收；多人录像工作台与观战为后续接入项。

封面采用 THBWiki 原版外包装扫描，来源链接与 SHA-256 保存在 `artwork/cover-source.json`。

多人模式任意玩家中弹都会清除全屏普通敌弹和独立激光池，P1／P2／P3 共用原生触发规则。原仓库的测试无敌下，三位玩家撞弹均会清弹且保留库存；正常中弹保留各自的扣命流程。清弹状态随原生内存一同回滚与重放。构建器要求来源包含 `hitClear.version = 1`，`hit-clear-content.mjs` 校验中日文 Package Store 内容中的实际 MAIN 补丁和联机碰撞补丁身份。

单人游戏保留原有浏览器练习器。点击画面右上角“练习 / 辅助”，或按 F12 选择机体、难度、关卡、Boss 与攻击阶段；Backspace 展开辅助菜单，F1–F5、U 切换辅助，Tab 显示信息，Shift+F 从已确认的目标重新练习。

辅助菜单与信息浮层沿用原布局，放在画面右侧信息区；F12 完整设置面板随实际画面缩放并可滚动，打开时进入原生暂停。参数表单、练习结果与快捷重试继续由浏览器控制。面板与结果弹窗打开时，主项目触控按钮暂时隐藏；移动手势从游戏画布开始，面板内保留点选与滚动。

特定跳关沿用原资源预载与后台预备实例：目标就绪后准备下一次起点，Shift+F 使用已确认的目标与最新辅助选项，预备实例在激活前保持冻结。高级设置中的预备快速重试选项可控制后台准备。练习 REP 存入主项目录像文件空间。手机可直接操作浏览器面板；合作房间使用原有同步内核。

统一练习器验证：启动包含 TH02、TH04、TH05 的主 Launcher 后运行 `node common/tests/pc98-practice-browser.mjs`。测试使用真实模拟器验证目标阶段、浏览器表单、辅助开关、后台预备完成与命中、暂停起点冻结、预备开关、右侧布局与窄屏面板。设置 `PC98_PRACTICE_MOBILE=1` 可验证移动浏览器面板与画面触控移动。
