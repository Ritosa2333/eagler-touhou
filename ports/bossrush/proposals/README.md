# BRO 接入提案（待维护者确认）

本目录保存 BRO 接入 eagler-touhou 的原始调研与配置草案，供维护者审阅。**产品身份登记（`common/contracts/product-catalog.mjs` 的 `PRODUCT_GAMES.bro`）已经落地**；其余仍在提案阶段的改动不改动仓库权威配置，以免在授权与资源方案未拍板前破坏默认发布。

## 已落地 vs 提案

| 项 | 状态 | 位置 |
| --- | --- | --- |
| 产品身份 `bro` / `bromp`（`[2,3]` 联机） | 已落地 | `common/contracts/product-catalog.mjs`、`.d.mts` |
| 适配检查表（118 项诚实状态） | 已落地 | [../ADAPTATION.md](../ADAPTATION.md) |
| 作品入口 / 开发指南 | 已落地 | [../README.md](../README.md)、[../docs/DEVELOPMENT.md](../docs/DEVELOPMENT.md) |
| i18n 标题 + 4 机体键（中/英/日） | 已落地 | `launcher/messages/{keys,en,ja,zh-CN}.json` |
| 产品目录硬编码清单（测试） | 已落地 | `launcher/tests/test-product-catalog.mjs`、`launcher/app/components/launcher/products.ts` |
| Runtime 产物摆位 + `bro.html` 入口 | 提案 | [runtime-builds.json.snippet](runtime-builds.json.snippet) |
| workspace 仓库映射（如需独立 MP 目录） | 提案 | [workspace.json.snippet](workspace.json.snippet) |
| 产品目录登记片段原始草案 | 参考 | [product-catalog.mjs.snippet](product-catalog.mjs.snippet)（正式版已落地，草案仅留档对照） |
| manifest ↔ product-catalog 字段差集 | 提案 | [MANIFEST_DIFF.md](MANIFEST_DIFF.md) |
| 认领 / 接入 Issue 正文 | 提案 | [ISSUE_DRAFT.md](ISSUE_DRAFT.md) |

## 待维护者拍板

1. **授权与归类**：BRO 是第三方衍生作，`docs/TOUHOU_PROGRESS.md` 目前只列正作 / PC98 / 小数点作，是否新增分类登记？署名方式（吃药图书 + 清酒）是否合适？
2. **玩家数**：首期按官方 `STANDARD_MULTIPLAYER_PLAYER_COUNTS=[2,3]` 接入；BRO 引擎内部 4 槽，暂不开放第 4 席（官方共享平台目前也只接受 2P/3P 子集）。
3. **资源挂载**：BRO 用 `scripts.json` + 分片 diff 分发（约 199MB / 9 组件），与官方 `.data` 单文件不同。是接受自定义 provider，还是打包成 `/bro.data`？
4. **测试版标记**：已按 `hsob` 用 `testOnly + buildOptIn + importOnly` 登记，避免影响正式发布列表。
5. **旁观 / 触控**：BRO 目前 `spectator:false`、只实测键盘，接入阶段可否先不支持？
6. **Runtime 接入**：需把 `web/build/bro.{mjs,wasm}` + `web/src/*.mjs` + `web/dist/*.json` 搬到启动器 runtime 目录并写 `bro.html` 入口（处理 `runtimeEpoch`）；`runtime-builds.json.snippet` 给出配方草案。

## 建议的后续步骤

1. 维护者确认上述事项后，把 `runtime-builds.json.snippet` 的配方落地，并补齐 `bro.html` 与 runtime 资源。
2. 按 `CONTRIBUTING.md` 在 `docs/migration/` 登记来源提交（含上游 fork 连接点），跑 `check-history.py --base upstream/main`。
3. 用官方流程跑 `ports:check -- --game=bro --variant=both`、`docs:check`，把 [适配检查表](../ADAPTATION.md) 中的 `待核对/待验证` 项按实测结果更新并附验证记录。
4. 完成后开 PR，正文链接适配表与验证记录。
