# 测试

以下命令从 `launcher/` 运行。测试按所需输入分开，先运行与你的改动相符的一组。

| 工作 | 命令 | 输入 |
| --- | --- | --- |
| 前端修改 | `npm run typecheck`、`npm test` | `npm ci --ignore-scripts` 安装的依赖 |
| Launcher 全部核心模块 | `npm run check` | 源码及 `services/` 中被检查的模块 |
| 跨作品集成 | `npm run check:workspace` | 按工作区配方准备的源码与本地资源 |
| 单个 Node 测试 | `node tests/test-名称.mjs` | 文件中声明的夹具 |
| 浏览器与设备 | `package.json` 中对应的 `test:*:browser` | 浏览器及该测试声明的游戏包、地址和资源 |

核心计划在 [`test-plan.mjs`](test-plan.mjs)，由 [`scripts/check.mjs`](../scripts/check.mjs) 执行。新测试归入计划或稳定 npm 入口；`test-test-ownership.mjs` 检查归属。

`npm run test:workspace-viewport:browser` 使用真实房间、Replay 和触控组件的展示夹具，检查手机 4:3 画面、桌面全宽及高度上限、双人和三人房右侧观战栏、少量观众头像放大、头像预览与完整名单、空席、加入及离开旁观、重新入座、成员移至旁观、竖屏快捷发言位置、面板触控隔离、大尺寸自定义控件边界和房间状态动效。截图输出到 `.cache/workspace-viewport/`。设置 `WORKSPACE_VIEWPORT_PREVIEW_ONLY=1` 可直接启动交互预览，`WORKSPACE_VIEWPORT_PREVIEW_PORT` 指定端口。

交互预览挂载真实设置抽屉与共用设置模型，房间人数、难度和规则会更新展示状态，输入延迟遵循作品目录声明的能力。成员菜单与快捷发言使用同一浮层组件，覆盖展开、收起、键盘返回和减少动态效果。

旧界面的对照输入保存在 [`fixtures/ui-history/`](fixtures/ui-history/README.md)，按固定 SHA-256 校验；完整检出与稀疏检出使用同一组输入。Python 场景比较通过标准输入接收检查脚本，支持 Windows 下的大份历史输入。

`npm test` 最后会运行 [`site-notice.run.mjs`](react-main/site-notice.run.mjs)。该检查把临时 bundle 与源目录分开构建，历史输入仍由 [`ui-history.mjs`](support/ui-history.mjs) 从固定夹具读取并校验。

`npm run test:netplay-windows:browser` 挂载生产连接窗口、标定模型、报告、更新确认和资源导入组件，检查桌面、手机竖屏和横屏的按钮可达性，以及全屏中的报告和更新选择。缺字体的导入提示、下载链接与启动错误分别验证房间内和全屏中的实际点击命中，关闭后恢复房间操作。截图和结果位于 `.cache/netplay-windows/`。网络事件由明确的展示夹具提供。

`react-main/browser-session-mounted.test.mjs` 通过生产页面、房间服务与 Runtime 协议边界检查入房、准备、开局、退出和原生返回菜单。旁观覆盖七部作品各连续三次进入和退出，验证房间身份、同一原生实例及 DATA 的保留和按次解除的加载遮罩；`multiplayer-services.test.mjs` 同时覆盖晚加入旁观的程序版本匹配与本地 Relay 交互。真实游戏画面、输入同步和多设备链路按对应作品的浏览器验收入口运行。

## 浏览器环境

`npm run test:room-runtime-retention:browser` 使用真实 th06 或 th07 多人 Wasm、两名玩家、一名旁观者和本地 Relay，连续三次从旁观回到原生菜单并重进同一局。显式设置 `ROOM_RUNTIME_GAME`、`ROOM_RUNTIME_ROOT`、`ROOM_RUNTIME_DATA`、`EAGLER_SHARED_GOTHIC` 和 `EAGLER_SHARED_UNIFONT`；结果与截图位于 `.cache/native-room-retention/<game>/`。

`npm run test:th10-room-native-preview:browser` 检查 th10 原生标题动画、同实例双人开局、相同逻辑帧的 RNG 与世界状态，以及主动返回和原生退出后的菜单动画。输入为 `TH10_PREVIEW_RUNTIME`、`TH10_TEST_ARCHIVE` 与上述两项字体路径，`TH10_PREVIEW_MODE` 选择同步模式。`test:room-native-preview:browser` 提供 th15 标题与双人启动检查，使用对应的 `TH15_PREVIEW_RUNTIME`、`TH15_TEST_ARCHIVE`；`test:room-title-preload:browser -- <game> <mode>` 提供各作预加载检查，使用 `ROOM_PREVIEW_RUNTIME`、`ROOM_PREVIEW_DATA` 与上述字体路径。

Python 浏览器测试使用锁定依赖：

```sh
python -m pip install -r tests/requirements-browser.txt
python -m playwright install chromium webkit
```

Node 测试使用锁定的 `puppeteer-core`，通过 `EAGLER_CHROME_PATH` 或标准安装目录寻找 Chrome/Edge。需要 Firefox 的测试再安装 Firefox。

工作区测试先按[本地联调](../docs/DEVELOPMENT.md)准备 Runtime、DATA 与服务，再读取目标脚本的参数。需要真实地址的 Relay/TURN 探测见[维护工具](../tools/maintainer/README.md)。

## 记录结果

记录测试名称、作品与模式、构建标识、浏览器、输入夹具和结果。游戏行为填写本作 `ADAPTATION.md`；[验证模板](../../docs/adaptation/record-template.md)用于保存操作和附件。截图、日志和录屏以链接附在对应条目中。
