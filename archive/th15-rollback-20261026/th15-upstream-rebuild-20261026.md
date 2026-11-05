# TH15 多人实验分支与上游同步

`experiment/th15-rollback-20261026` 以 `upstream/main` 的 `3375779bf293c492a1e5aa1eeb5d2afcf047af8a` 为基点，保留 TH15 多人回滚、快照优化、真实帧率、错误报告器和对应的公共策略、中继与验证工具。TH15 多人源码树与重建前 `ae6aab22a6cc903a44d07e2ce4d518fa0d824d17` 一致，其余作品目录与该上游基点一致。

来源连接为 `f008e9c848af4a7c9c4bb5f5c131ac325b0e5894`，多人导出基点为 `e02e0266c453054d53d055d29d141eb83d736fdf`。多人历史保留原仓库 `a5cc4652d59d55f2559622da28e21cbbf52b75d3`、已有多人投影 `8affd9a3adb80fec10c109be608872de42aec7aa` 及普通版投影 `8d01b6e1886e53c240a56777574f74a617294c5d` 的祖先关系。普通开发提交不携带子树控制标记，来源记录指向本次独立连接。

标准 `git subtree split` 对 `ports/th15-mp`、`ports/th12` 的导出均通过源码树及原仓库祖先核对。历史工具回归 18 项通过，Windows 跳过 1 项 POSIX 专属检查；产品策略与报告器 6 项、中继 82 项、原生回滚 10 项通过。中继初始模式断言覆盖 TH15 默认延迟模式与可选回滚能力。

逐提交迁移映射、源码树散列、导出结果和恢复引用见 [机器记录](th15-upstream-rebuild-20261026.json)。旧分支备份为 `refs/backup/th15-rebuild-20261026/before-ae6aab22a6`，另存完整 bundle。历史同步不涉及站点部署。
