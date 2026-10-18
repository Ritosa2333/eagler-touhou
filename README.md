# Eagler Touhou

启动器、共享 common 和各作品适配源码在同一个仓库维护。

- `launcher/`：启动器及构建配方。
- `common/`：全部作品默认消费的共享源码，不为各作固定另一份 common 提交。
- `ports/`：各作源码。已有普通版和 MP 分支分别保留，MP 内现有构建模式继续存在。
- `tools/ci/`：按改动范围选择检查。common 修改检查全部消费者，普通版修改同时检查对应 MP。

贡献方式见 [贡献指南](docs/CONTRIBUTING.md)，项目入口见 [WORKSPACE.md](WORKSPACE.md)。

当前为独立迁移候选，尚未替换现有开发工作区。尚未提交的旁观改动须在原任务完成后刷新到本候选，再执行完整 Runtime 与行为验收。

当前来源、已验证范围和明确失败项见 [本地验收记录](docs/migration/acceptance-20261018.md)。Launcher 来源为 `main`，没有纳入 `launcher-refactor`。
