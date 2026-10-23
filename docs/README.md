# 文档

| 要做的事 | 阅读顺序 |
| --- | --- |
| 第一次贡献 | [参与开发](../CONTRIBUTING.md) → [选择组件](../WORKSPACE.md) |
| 修改前端 | [启动器](../launcher/README.md) → [开发与单游戏联调](../launcher/docs/DEVELOPMENT.md) |
| 修改游戏 | [游戏目录](../ports/README.md) → 本作开发指南 → 本作适配表 |
| 适配新游戏 | [适配流程](adaptation/README.md) → 对应分项 → [验证记录](adaptation/record-template.md) |
| 理解结构 | [启动器架构](../launcher/docs/ARCHITECTURE.md)、[联机](architecture/netplay.md)、[回滚](architecture/rollback.md)、[流式 Replay 旁观](architecture/spectator-replay.md) |
| 部署站点 | [自托管](../launcher/docs/SELF_HOSTING.md) → [配置参考](../launcher/docs/SELF_HOSTING_REFERENCE.md) |
| 找旧结论 | [历史与迁移](history/README.md) → [旧文档去向](history/INDEX.md) |

## 适配分项

| 分项 | 内容 |
| --- | --- |
| [源码与构建](adaptation/build.md) | 源码归属、依赖、普通版与 MP、Runtime 产物 |
| [启动与资源](adaptation/runtime.md) | 协议、启动阶段、资源、安装、退出 |
| [输入与触控](adaptation/input.md) | 键盘、手柄、每种触控模式、菜单与生命周期 |
| [画面与声音](adaptation/presentation.md) | 逻辑频率、高刷新率、插值、音效、OGG、MIDI |
| [存档与 Replay](adaptation/storage.md) | 文件范围、持久化、原版与扩展回放 |
| [语言与练习](adaptation/localization.md) | 翻译包、字体、thprac 和练习回放 |
| [多人](adaptation/multiplayer.md) | 连接、时序、规则、Replay 与旁观 |
| [验证与交付](adaptation/validation.md) | Demo、长回放、原版对照、结果记录 |

[全部可读文档](INDEX.md)逐份列出路径、名称和用途。新建或移动文档后，执行 `npm run docs:index` 更新总表。
