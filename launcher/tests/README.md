# 测试

以下命令从 `launcher/` 运行。测试按所需输入分开，先运行与你的改动相符的一组。

| 工作 | 命令 | 输入 |
| --- | --- | --- |
| 前端修改 | `npm run typecheck`、`npm test` | `npm ci --ignore-scripts` 安装的依赖 |
| Launcher 全部核心模块 | `npm run check` | 源码及 `services/` 中被检查的模块 |
| 跨作品集成 | `npm run check:workspace` | 按工作区配方准备的源码与本地资源 |
| 单个 Node 测试 | `node tests/test-名称.mjs` | 文件中声明的夹具 |
| 浏览器与设备 | `package.json` 中对应的 `test:*:browser` | 浏览器及该测试声明的游戏包、地址和资源 |

核心计划在 [`test-plan.mjs`](test-plan.mjs)，由 [`scripts/check.mjs`](../scripts/check.mjs) 执行。新测试归入计划或稳定 npm 入口；`test-test-ownership.mjs` 检查入口归属，默认执行范围由[根 CI](../../.github/workflows/ci.yml)中的任务决定。

`test-gamepad-contract.mjs` 编译共用原生映射，遍历 4096 种逻辑按键组合并验证中立、断开与键盘独立持有；生成副本按构建目录枚举并与共用实现核对。根 CI 的 `port-adaptation` 执行该检查。共享 R 键的发送由 `test-touch-runtime-protocol.mjs` 检查，实际暂停和重开按本作 Runtime 验收；合作 MP 使用 `ports:mp --lifecycle`。各层按自己的输入与结果记录。

`npm run test:support` 的 Package 模块检查通过正式生成器准备 FAQ 静态样式，再在独立目录验证合约编译、模块依赖与 HTTP 服务。生成样式覆盖 `styles.css`、`features.css` 与 `site-pages.css`，检查目录保留源码输入及生成文件的逐字节一致性。

房间丢包测量的状态和双端计数由 `npm run check` 检查；`npm run test:room-packet-loss:browser` 在本机浏览器中验证真实 DataChannel 与房间面板。

`npm run test:site-mirror:browser` 使用本地 HTTPS 服务检查网页与 Runtime 的镜像回退、完整性校验、离线刷新及浏览器重启。通过 `EAGLER_TEST_TLS_CERT`、`EAGLER_TEST_TLS_KEY` 提供测试证书与密钥。`react-main/relay-entry-routing.test.mjs` 和 `site-mirror-routing.test.mjs` 覆盖入口选择、超时、取消、请求身份和发布配置。

共享顶栏的直接页面导航、当前页面标识、帮助与设置的 500 毫秒悬停等待及取消、即时点击和键盘操作、语言菜单和键盘焦点由 `react-main/shell-links.test.mjs` 检查。`node tests/test-netplay-telemetry.mjs` 检查遥测接口兼容性；从 `services/telemetry/` 运行 `npm test` 检查服务探测、历史恢复、旧地址跳转和下载服务分组开关。

th075 生产配置的语言包边界由 `npm run check:workspace` 检查；`npm run test:th075-language-upgrade:browser` 使用历史导入包、现行站点和原生 Runtime 验证升级流程。th12 中文贴图合成由 `npm run check` 检查。

Host 工作区检查覆盖 th12 的原始音轨输入与 `assets-eagler` 成品内容标记，确保两种内容准备方式都能被识别。

产品目录检查识别两种目录型 Runtime 键盘入口：现有作品发布 `directory-keyboard.mjs`，th12 由 `managed-shell.mjs` 接收 Launcher 输入并连接共享映射。

`npm run test:keyboard-mapping:browser` 枚举 STG 普通版与 MP 的生产 JS 输入边界，单作使用 `-- --game=th08 --variant=all`。角色动作和设备验收见[输入指南](../../docs/adaptation/input.md)。

`npm run test:audio-output:browser` 渲染生产 WebSfx 的实际 PCM，范围见[声音验收方法](../../docs/adaptation/presentation.md#声音验收方法)。两个浏览器入口由根 CI 的 `shared-input-audio` 执行，环境使用下述锁定 Playwright 依赖和 Chromium。

跨站点存档迁移检查以产品目录为准逐项核对数据库，包含 th12 的 `/savesth12`。

正式发布输入示例包含 th12 的原始游戏目录，保持隐藏测试作品与其他作品共用完整站点清单。

内部 Operator 交付清单也登记 th12 的普通版与 OGG 内容，保持交付配置和作品目录相同顺序。

语言包缓存检查使用完整构建输入指纹，覆盖 th12 贴图编译器与 th075 中文剧情资源脚本，验证相同输入直接复用生成包。

`npm run test:replay-audio:browser -- --help` 列出录像音频测试的输入，支持 th08、th10、th11、th15 的普通版与多人版。共享录像桥测试为异步定位保留有超时的状态等待，覆盖暂停归属和定位前后的音效输出。房间组件测试通过实际管理菜单与产品目录声明的难度选项验证成员管理和原生选角边界。

`react-main/directory-views.test.mjs` 在中英文环境中遍历完整作品目录，检查已发布多人作品的房间信息和创建表单。采用原生菜单选择的作品隐藏大厅难度，其余作品展示房间声明的难度。

`npm run test:workspace-viewport:browser` 使用真实房间、Replay 和触控组件的展示夹具，检查手机 4:3 画面、桌面全宽及高度上限、双人和三人房右侧观战栏、少量观众头像放大、头像预览与完整名单、空席、加入及离开旁观、重新入座、成员移至旁观、竖屏快捷发言位置、面板触控隔离、大尺寸自定义控件边界和房间状态动效。截图输出到 `.cache/workspace-viewport/`。设置 `WORKSPACE_VIEWPORT_PREVIEW_ONLY=1` 可直接启动交互预览，`WORKSPACE_VIEWPORT_PREVIEW_PORT` 指定端口。

交互预览挂载真实设置抽屉与共用设置模型，房间人数、难度和规则会更新展示状态，输入延迟遵循作品目录声明的能力。成员菜单与快捷发言使用同一浮层组件，覆盖展开、收起、键盘返回和减少动态效果。

旧界面的对照输入保存在 [`fixtures/ui-history/`](fixtures/ui-history/README.md)，按固定 SHA-256 校验；完整检出与稀疏检出使用同一组输入。Python 场景比较通过标准输入接收检查脚本，支持 Windows 下的大份历史输入。

`npm test` 最后会运行 [`site-notice.run.mjs`](react-main/site-notice.run.mjs)。该检查把临时 bundle 与源目录分开构建，历史输入仍由 [`ui-history.mjs`](support/ui-history.mjs) 从固定夹具读取并校验。

`npm run test:netplay-windows:browser` 挂载生产连接窗口、标定模型、报告、更新确认和资源导入组件，检查桌面、手机竖屏和横屏的按钮可达性，以及全屏中的报告和更新选择。缺字体的导入提示、下载链接与启动错误分别验证房间内和全屏中的实际点击命中，关闭后恢复房间操作。截图和结果位于 `.cache/netplay-windows/`。网络事件由明确的展示夹具提供。

`react-main/browser-session-mounted.test.mjs` 通过生产页面、房间服务与 Runtime 协议边界检查入房、准备、开局、退出和原生返回菜单。旁观场景按产品目录中的旁观能力遍历，在桌面与手机环境下各连续三次进入和退出，验证房间身份、同一实例、DATA 复用及加载遮罩解除，并检查全屏与角色切换。夹具启用测试作品，作品身份和网络策略沿用产品目录；`multiplayer-services.test.mjs` 同时覆盖晚加入旁观的程序版本匹配与本地 Relay 交互。真实游戏画面、输入同步和多设备链路按对应作品的浏览器验收入口运行。

## 浏览器环境

`npm run test:room-runtime-retention:browser` 按产品目录的旁观能力选择作品，使用真实多人 Wasm、两名玩家、一名旁观者和本地 Relay，连续三次从旁观回到原生菜单并重进同一局。显式设置 `ROOM_RUNTIME_GAME`、`ROOM_RUNTIME_ROOT`、`ROOM_RUNTIME_DATA`、`EAGLER_SHARED_GOTHIC` 和 `EAGLER_SHARED_UNIFONT`；结果与截图位于 `.cache/native-room-retention/<game>/`。

工作区的 `test-multiplayer-session-identity.mjs` 执行 th08、th10、th11、th12、th15 的实际准入函数，检查不同 Relay 地址下的玩家与旁观者共用会话 ID，以及房间和局次隔离。标题预加载浏览器检查支持 `ROOM_PREVIEW_MIXED_RELAY=1`，用同一服务的两个域名及代理路径验证真实双人开局。

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

## 资源切换与房间返回

`react-main/resource-download.test.mjs` 检查失败节点切换、首包与断流超时、慢传输、取消、静态托管、版本参数和跨节点凭据隔离。`transfer.test.mjs` 通过实际语言包准备入口检查换节点、单一进度任务与完整归档校验。分流服务的 `npm test` 检查发布库存和有序节点发现。th09 构建直接验证正式 Wasm 中的房间返回导出。

联机介绍通过固定历史转换器对照当前原文，另用独立的对战章节输入覆盖 th09 分组；正文按内容文档维护。
