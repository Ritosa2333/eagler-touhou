# 测试

以下命令从 `launcher/` 运行。测试按所需输入分开，先运行与你的改动相符的一组。

| 工作 | 命令 | 输入 |
| --- | --- | --- |
| 前端修改 | `npm run typecheck`、`npm test` | `npm ci --ignore-scripts` 安装的依赖 |
| Launcher 全部核心模块 | `npm run check` | 源码及 `services/` 中被检查的模块 |
| 跨作品集成 | `npm run check:workspace` | 按工作区配方准备的源码与本地资源 |
| 单个 Node 测试 | `node tests/test-名称.mjs` | 文件中声明的夹具 |
| 浏览器与设备 | `package.json` 中对应的 `test:*:browser` | 浏览器及该测试声明的游戏包、地址和资源 |

核心计划在 [`test-plan.mjs`](test-plan.mjs)，由 [`scripts/check.mjs`](../scripts/check.mjs) 执行。新测试归入计划或稳定 npm 入口；`test-test-ownership.mjs` 检查归属。全仓统一入口、并发与时间预算见[测试目录](../../tests/README.md)。

会建立临时 Git 仓库、索引和子模块的构建凭据测试共用 `runtime-git-fixture` 资源锁。它们仍可与其他测试并行，但彼此串行，避免 Git 夹具同时运行争用磁盘与 Git 子进程资源。资源归属集中声明在测试计划中。

依赖凭据夹具分别验证完整的八个必需子模块，以及带一个真实 pin 的增量执行。前者检查必需／可选输入和身份变化，后者检查编译步骤、输出刷新和复用边界。

跨作品 Runtime 协议 L0 (`node tests/test-runtime-protocol-contract.mjs`) 从 `runtime-builds.json` 枚举 normal/MP 变体，按声明的源码归属核对命令、事件与触控范围。

旧界面的对照输入保存在 [`fixtures/ui-history/`](fixtures/ui-history/README.md)，按固定 SHA-256 校验；完整检出与稀疏检出使用同一组输入。Python 场景比较通过标准输入接收检查脚本，支持 Windows 下的大份历史输入。

`npm test` 最后会运行 [`site-notice.run.mjs`](react-main/site-notice.run.mjs)。该检查把临时 bundle 与源目录分开构建，历史输入仍由 [`ui-history.mjs`](support/ui-history.mjs) 从固定夹具读取并校验。

## 浏览器环境

Python 浏览器测试使用锁定依赖：

```sh
python -m pip install -r tests/requirements-browser.txt
python -m playwright install chromium webkit
```

Launcher 键盘映射回归通过 Chromium 运行当前 monorepo 的 Runtime Shell 与 Launcher 映射代码，覆盖 15 个 normal/MP 变体：

```sh
python tests/test-keyboard-mapping-browser.py
```

也可从 `launcher/` 运行 `npm run test:keyboard-mapping:browser`。`--variants th08-normal th10-multiplayer` 可选跑诊断子集，`--negative-control` 会切断 th10 MP 的 Shell 映射接线并确认行为断言失败。测试使用工作区的 Shell 页面、初始化、协议身份校验、键盘监听和 Launcher 映射，生成 WASM 模块由原生输入观察 sink 承接。

Node 测试使用锁定的 `puppeteer-core`，通过 `EAGLER_CHROME_PATH` 或标准安装目录寻找 Chrome/Edge。需要 Firefox 的测试再安装 Firefox。

工作区测试先按[本地联调](../docs/DEVELOPMENT.md)准备 Runtime、DATA 与服务，再读取目标脚本的参数。需要真实地址的 Relay/TURN 探测见[维护工具](../tools/maintainer/README.md)。

## 记录结果

记录测试名称、作品与模式、构建标识、浏览器、输入夹具和结果。游戏行为填写本作 `ADAPTATION.md`；[验证模板](../../docs/adaptation/record-template.md)用于保存操作和附件。截图、日志和录屏以链接附在对应条目中。
