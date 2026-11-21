# Issue 草案：认领并接入 Touhou BossRush（第三方衍生作）到 eagler-touhou

> ⚠️ 草案，供清酒复制到 eagler-touhou 主库开 Issue 用（发之前按实际账号/链接改一遍）。
> 标题建议简短中文（CONTRIBUTING 要求 PR/Issue 标题交维护者审阅时用简短中文）。

---

## 标题

认领接入：Touhou BossRush（第三方衍生作，合作联机 WASM）

## 正文

你好，我是清酒（Tidew / TSake）。想认领把 **Touhou BossRush（BRO）** 接入 eagler-touhou 网页端的工作，先开这个 Issue 跟维护者确认可行性和接入方式，避免撞车。

### 这是什么

BRO 是基于 LuaSTG Sub 的第三方 Boss Rush 同人游戏，原作由吃药图书（Cyoltose）制作（`https://github.com/Cyoltose509/boss-rush`）。我参与了 Web 化，产物在 fork `https://github.com/TSake/TouHou-BossRushOnline`（分支 `web/multiplayer-port`）。

目前已跑通：
- 单一自包含 WASM（纯 C Lua 5.1.5 + `lstg.*` 绑定 + WebGL2/WebAudio，3.3MB），Node 回归 30/30 绿；
- 合作联机（WebRTC DC 直连 / Full Mesh / 输入缓冲 / 预测回滚 / 选路 / eagler 契约），**首期上 2~3 人**，与官方标准一致；引擎内部是 4 槽但暂不开放第 4 席；
- 多人 replay（旁路多流，借 `write(path, bytes, replayMatch?)` 的官方预留字段）、IndexedDB 存档；
- 11 项系统多人兼容修复（团队资源共享 + 个人数值独立）。

### 已对齐的契约

- Runtime 信封已按 `eagler-touhou/1` 实现（13 命令 / 8 事件，`runtimeEpoch` 校验），见 `web/src/eagler_adapter.mjs`；
- `web/manifest.json` 按 product-catalog 同构字段写好，差集分析见我准备的对照文档；
- 分包清单已按三层（`package-manifest`/`app-shell`/`runtime`）生成。

### 想确认的问题

1. **授权与归类**：BRO 是第三方衍生作，`docs/TOUHOU_PROGRESS.md` 目前只列正作/PC98/小数点作，没有第三方衍生作栏目。是否接受新增一个分类来登记 BRO？署名方式（吃药图书 + 清酒）是否合适？
2. **游戏 ID**：建议用 `bro`，若主库有命名规范请指正。
3. **玩家数**：首期按官方 `STANDARD_MULTIPLAYER_PLAYER_COUNTS=[2,3]` 接，不另开 4 人。BRO 引擎内部虽支持 4 槽，但这次不开放第 4 席，后续有需要再单独提。
4. **资源挂载**：BRO 用 `scripts.json` + 分片 diff 分发（约 199MB，9 组件），与官方 `.data` 单文件不同。是接受自定义 provider，还是希望我打包成 `/bro.data`？
5. **测试版标记**：我打算先按 `hsob` 那样用 `testOnly + buildOptIn` 登记，可以吗？
6. **旁观**：BRO 目前 `spectator:false`，官方多数联机作品支持旁观。接入阶段是否可以先不支持？

### 我会做的事

如果以上确认，我会在 fork（`TSake/eagler-touhou`）里：
- 按 `docs/adaptation/` 流程登记 `workspace.json` / `runtime-builds.json` / `product-catalog.mjs` 三处配置；
- 建 `ports/bossrush/` 三件套（README / DEVELOPMENT / ADAPTATION），跑 `ports:check` 与 `docs:check`；
- 按 `CONTRIBUTING.md` 补齐 `docs/migration/` 来源登记与 `check-history.py` 校验；
- 完成后开 PR。

适配表草案 / 配置片段 / manifest 差集我都已准备好，可直接贴到 PR 里。

### 环境说明

前端工具链我已在装（Bun 1.4.4 / Node 24），克隆用 sparse-checkout，不会拉全库。希望加入开发者群（Discord / QQ）方便对接。

---

> 附：如果你希望我先把草案发出来供审阅，我可以把它作为评论贴到这个 Issue，或先开一个草稿 PR。
