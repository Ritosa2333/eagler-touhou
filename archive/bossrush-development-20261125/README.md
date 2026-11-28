# BRO 源码交付来源

`FILE-SHA256.json` 固定 2026-11-25 接入前的 BRO Web 交付身份（BRO `TSake/TouHou-BossRushOnline` 提交 `9b6ad0a`，WASM build_id `715a2bbd`）。迁入映射见 `../../docs/migration/bossrush-eagler-20261125.json`，当前开发入口为 `../../ports/bossrush/README.md`。

本记录覆盖 `ports/bossrush/` 落地的 Web 产物、适配层与文档；162 MiB 游戏资源包按 `importOnly` 由玩家导入，音乐与预编译 Runtime 之外的资产不进入本记录。
