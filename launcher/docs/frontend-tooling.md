# 前端检查与测试

前端使用 Bun 1.4.4，版本记录在 `.bun-version` 和 `packageManager`，依赖由 `bun.lock` 固定。在 `launcher/` 安装：

```sh
bun run install:locked
```

`install:locked` 执行冻结锁文件安装，禁用依赖生命周期脚本，并核对所有前端直接依赖及环境记录。

包内命令通过 `bun --bun run` 执行工具，Vitest 的主进程和每个 worker 都检查实际 Bun 版本。游戏编译、Relay 及其他独立服务遵循各自的环境要求。

## 日常命令

```sh
bun run dev
bun run lint
bun run format:check
bun run i18n:check
bun run typecheck
bun run test:unit
bun run build
```

`lint:fix` 只应用 Biome 的安全修复，`format` 统一格式。生命周期、可访问性和安全规则使用推荐级别；需要例外时在相应语句说明具体原因。格式调整应与功能修改分开审查。

## 测试分组

| 入口 | 范围与输入 |
| --- | --- |
| `test:unit`、`test:unit:watch` | React 交互、状态模型与纯逻辑，准备翻译后运行 |
| `test:build` | 在临时目录验证真实构建、发布、离线与首次启动 |
| `test:support` | 历史输入与支持夹具 |
| `test:repository` | 仓库内格式、业务与工具契约，自动准备完整前端 |
| `test` | 上述仓库内 Vitest 分组与公告专项检查 |
| `test:workspace` | 显式选择的跨作品契约，需要对应源码和资源 |
| `test:host-node` | 使用 Node 验证自托管首次安装与依赖记录 |
| `test:*:browser` | 真实浏览器专项，按脚本声明准备地址、Runtime 与 DATA |

浏览器布局检查包含输入窗口打开期间转屏、关闭后底层选项栏的位置核对，以及触控默认布局在同方向缩放后的重排。检测器还检查普通控件的宽高、位置、间距过渡和活动动画，并指出被旧样式覆盖的反馈时长。新增设置同时检查这些连续操作，静态截图与交互结果分别记录。

单个测试可用 `bun --bun run vitest run <测试路径>`。分组来自 `tests/test-plan.mjs`；新增入口须登记用途与输入，测试归属检查会发现未登记文件。

迁移保留原有断言。测试注册、生命周期、清理和假时钟由 Vitest 管理，文件在隔离子进程中顺序执行。浏览器模拟中的 Request 使用 Undici 的标准实现，局部注入到 Service Worker 夹具。

## 构建和安装校验

独立工作树启动源码预览时，`EAGLER_WORKSPACE_ROOT` 指向提供 `games`、`dependencies` 和作品仓库的工作区目录，React 产物目录单独配置。启动真实业务验收前，核对 Site Manifest 中的 DATA、游戏字体和 Runtime 地址均可访问。

快速测试只生成翻译。完整准备会编译服务和 React 页面，冷构建测试自行创建隔离输出。消息、语言配置与编译依赖参与缓存身份。生成任务用操作系统文件锁协调，进程退出或被强制终止后自动释放。`.lock` 文件持续存在，不要删除或替换。子编译器先写独立暂存区，由持锁进程发布。

从旧版本升级时，如果提示 `.lock` 仍是目录，先停止使用该缓存的构建进程，再移走旧目录。工具不会自动删除旧锁。

源码检查用 Acorn 解析 JavaScript，检查过程不执行被检查文件。Python、浏览器与跨作品输入各有独立入口，详见[测试指南](../tests/README.md)。

自托管同时提供 Bun 和 npm 锁文件。Bun 根据所选锁文件安装，记录清单与锁文件哈希、运行时和依赖版本；锁、清单、安装版本或所选运行时变化后重新校验。Node 自托管入口使用 npm 锁文件及自己的记录，发布包消费预编译前端。

三语维护和文案审计见[界面语言](paraglide-i18n.md)，组件与浏览器验收见[UI 指南](ui-controls.md)。
