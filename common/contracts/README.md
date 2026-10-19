# 客户端与服务端契约

这里维护 Launcher、Relay 和其他客户端共用的 JavaScript 规则。实现是原生 ECMAScript 模块；同名 `.d.mts` 文件提供 TypeScript 类型。服务端可直接导入这些模块，无需加载 Launcher 或浏览器 UI。

## 修改方式

先定位权威定义，再同步检查所有调用方：

| 文件 | 定义 |
| --- | --- |
| [`product-catalog.mjs`](product-catalog.mjs) | 作品目录、联机能力、运行时资源和功能策略 |
| [`netplay-timing.mjs`](netplay-timing.mjs) | 联机时序配置与测量结果解析 |
| [`multiplayer-quick-chat.mjs`](multiplayer-quick-chat.mjs) | 快捷聊天 ID 与短语映射 |
| [`public-nickname.mjs`](public-nickname.mjs) | 昵称规范化和状态解析 |
| [`th15-chapter-reward.mjs`](th15-chapter-reward.mjs) | TH15 章节奖励规则 |

修改实现时同步更新对应 `.d.mts` 声明及消费方测试。Relay 检查：`cd services/relay && npm test`；遥测服务检查：`cd services/telemetry && npm test`。涉及 Launcher 的规则，也运行对应 Launcher 测试。

## 下一步

提交前确认服务和客户端都从此目录读取同一规则，并查看 [公共开发入口](../../docs/README.md)。本目录适用 [GPL-3.0-or-later](LICENSE)；`common/` 其余共享 C++ 代码另见上层 [`LICENSE`](../LICENSE)。
