# 历史与迁移

monorepo 的原仓库来源与导入提交记录在 [import-sources.json](../migration/import-sources.json) 和 [move-sources-20261018.json](../migration/move-sources-20261018.json)。当前开发目录见[工作区](../../WORKSPACE.md)。

原仓库提交保留为当前仓库的祖先，可以按原提交查看作者、时间、说明和差异。普通版与 MP 的真实来源提交和共同祖先接入[目录监控与同步历史](../adaptation/port-sync.md)，`log <game> --original` 展示原提交图。工作区未提交修改与已提交历史分别保存。

[普通版与 MP 提交对账](port-sync-audit.md)逐项列出独有提交的补丁对应、拆分迁入和共同来源证据；完整文件与修改块映射见 [JSON 记录](port-sync-audit.json)，普通版、MP 与合并节点的逐项处理范围见[固定快照决定](port-sync-decisions.json)，待同步项附来源新增与目标缺失证据。

## 查找旧结论

[旧文档去向表](INDEX.md)列出重写前的全部文档、固定版本原文和当前入口。测试日志、原版对照、数值、失败记录及构建哈希保留在原始提交中，使用表内链接可直接阅读。

| 想查什么 | 当前入口 |
| --- | --- |
| 联机测量、启动与节奏 | [联机架构](../architecture/netplay.md) |
| 回滚恢复、性能与各作经验 | [回滚架构](../architecture/rollback.md) |
| 某作构建与适配情况 | [游戏目录](../../ports/README.md) |
| TH20 旧源码重建与分析记录 | [固定版本原文](INDEX.md) · [当前源码布局](../architecture/source-layout.md) |
| 迁移来源和当时验收 | [迁移记录](../migration/README.md) |

## 转入旧分支改动

先确认旧提交属于启动器、普通版、MP 还是 common，再对照当前目录应用差异。共用源码改动同时检查对应模式；旧的根 CI 和嵌入启动器内容由当前根入口接替。

在改动说明中记录原仓库、原基线和迁入范围，随后执行当前组件的检查。
