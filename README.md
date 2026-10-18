# Eagler Touhou

启动器、共享 common 和各作品适配源码在同一个仓库维护。

- `launcher/`：启动器及构建配方。
- `common/`：全部作品默认消费的共享源码，不为各作固定另一份 common 提交。
- `ports/`：各作源码。已有普通版和 MP 分支分别保留，MP 内现有构建模式继续存在。
- `services/relay/`：可独立构建、测试和运行的联机服务，供启动器及其他兼容客户端接入。
- `services/turn/`、`services/telemetry/`：TURN 配置与运维入口、独立统计服务。
- `tools/ci/`：按改动范围选择检查。common 修改检查全部消费者，普通版修改同时检查对应 MP。

贡献方式见 [贡献指南](docs/CONTRIBUTING.md)，项目入口见 [WORKSPACE.md](WORKSPACE.md)。

原工作区各来源已提交并与远端同步，迁移在本候选中进行。原仓库与历史保留，是否正式切换由搬迁后的 Runtime 和行为验收决定。

当前验收见 [搬迁验收记录](docs/migration/move-acceptance-20261018.md)，搬迁前记录见 [本地验收记录](docs/migration/acceptance-20261018.md)。Launcher 来源为 `main`，没有纳入 `launcher-refactor`。
