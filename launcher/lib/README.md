# `lib/` 目录归属

`lib/` 存放可复用的 Node 端策略与构建逻辑。常见模块负责解析 workspace 和构建配置、校验契约、组装清单与产物，以及实现脚本和测试共用的策略。

`lib/contracts/` 提供 Node 可导入的契约模块；浏览器侧类型入口位于 `src/contracts/`，跨组件共享契约位于 `../common/contracts/`。

目录归属按职责划分：

- 一次性参数解析和控制台输出放在 `scripts/`。
- 浏览器应用行为放在 `app/` 和 `src/`。
- 浏览器 Package 存储和安装流程放在 `package/`。
- 测试放在 `tests/`。

适合放入 `lib/` 的模块应是可复用策略、组装边界、校验器或归属清晰的适配器。
