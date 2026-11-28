# BRO manifest ↔ eagler product-catalog 字段差集

> 对照源：BRO `web/manifest.json` vs eagler-touhou main `common/contracts/product-catalog.mjs`
> 的 `createLocalProductManifest()` 与 hsob/th07/th20 实际条目。
> 结论先行：**契约层已高度对齐（协议/命令/事件/多人字段），差集集中在「产品身份字段」「资源挂载模型」「触控/能力声明」三块。**

---

## 1. 已有对应（✅ 好消息）

| eagler 字段 | BRO 位置 | 状态 |
|---|---|---|
| `hostProtocol: "eagler-touhou/1"` | manifest `_eaglerContract.hostProtocol` | ✅ 完全一致 |
| 命令 13 项 / 事件 8 项 | `_eaglerContract.commands/events` | ✅ 已实现（`eagler_adapter.mjs`） |
| epoch 校验 | `_eaglerContract.epochQueryParameter = runtimeEpoch` | ✅ 与官方 URL 注入一致 |
| `multiplayer.gameplay: "cooperative"` | `multiplayer.gameplay` | ✅ |
| `multiplayer.playerCounts` | 草案首期 `[2,3]`（标准常量） | ✅ 与官方标准一致；BRO 引擎内部仍是 4 槽，但暂不开放第 4 席（用户要求） |
| `multiplayer.loadouts` | manifest 仅说明 4 角色 | ⚠️ 需按官方 loadouts 结构展开（草案已给 `BRO_MULTIPLAYER_LOADOUTS`） |
| `multiplayer.peerTransportGlobal` | `__broPeerTransport` | ✅ |
| `multiplayer.inputTiming` | rollbackLimit 12 等 | ✅ 形状与官方一致 |
| `replay.prefix` | `replay/slotN.rep` + `.multi` | ⚠️ 官方 prefix 是单值，BRO 有单流+旁路两种格式（草案写 extensions:["rep","multi"]） |
| `storage.saveRoot` | `/User`,`/replay`,`/bro/data` | ⚠️ 官方 saveRoot 是单根，BRO 三根（需映射到 `/bro`） |
| `features.{thprac,languages,focusHitbox}` | 草案写全 false | ✅ BRO 无 thprac/语言包/常显判定点发布功能 |
| `support.highRefreshRate` | 渲染 rAF 跟随刷新率 | ✅ 逻辑固定 60Hz |
| `support.sourceRepository` | fork 地址 | ✅ |

---

## 2. 缺口与需决策（⚠️ / ❌）

### 2.1 产品身份字段（BRO manifest 没有，必须补）
官方登记条目要求但 BRO manifest 缺失：
- `number`（草案 `"BRO"`）
- `title` / `subtitle`（草案 `"東方 BossRush"` / `"Touhou BossRush"`）
- `cardArtwork`（需提供卡片图，如 `bro-card.webp`）
- `testOnly` / `buildOptIn`（第三方衍生作建议先用，参照 hsob）
- `support.credit`（署名吃药图书 + 清酒）
- `runtimeFileLayout: "directory"` + `runtimeAssets` 完整清单

### 2.2 资源挂载模型（最大差异）
| 官方惯例 | BRO 现状 | 处理建议 |
|---|---|---|
| `package.dataTarget: "/thXX.data"` 单文件 | `dataTarget: "scripts.json"` + 分片 `scripts/**` | 需维护者拍板：接受自定义 provider，还是把资源打成 `/bro.data` |
| `dataProvider: "retail-memory"/"emscripten-preload"` | 无对应字段 | 草案暂填 `emscripten-preload` |
| `musicSourceDirectories:{ogg:"music"}` `musicMounts:{ogg:"/music"}` | manifest 用 `package://music` 组件 | 草案映射成官方结构；实际 47 首 ogg 已就地压缩 |
| `musicCapabilities.midi: false` | 无 | 草案补 false |
| `musicRuntime.localOggConfigureMode: "runtime-selection"` | 无 | 草案补 |

### 2.3 触控/输入
- `touchFire: TOGGLE_TOUCH_FIRE` 草案已声明，但 BRO **只实测键盘**，官方 TOUCH-01~19 全套未验。
- 启动器改键所有权（INPUT-02/03/05）：BRO 用引擎内 `setting.keys`，未接官方 launcher-input-owners。

### 2.4 多人细节
- `spectator: false`：BRO 无旁观，官方 th06/07/08/10/11/15 都支持旁观；若主库要求旁观，是较大工作量。
- `titleRoomEntry: false` ✅（与 th07 一致）。
- `relaySourceEnvelope: true` ✅。
- 官方 `inputTiming` 还有 `measuredStartup/manualDelayLimit` 等字段，BRO 只填了 4 个；需按 `netplay-timing.mjs` 契约补全或确认可选。

### 2.5 BRO 有但官方契约没有的（保留为扩展，不冲突）
- `_eaglerContract`（BRO 自己的对齐说明区）、`display`、`rollback` 字段是 BRO 内部文档，迁入时可不进 product-catalog。
- 多人旁路 replay 的 `replayMatch` 用法是官方 `write()` 的预留字段，天然兼容，无需新协议（见 PROBE_3AC）。

---

## 3. 建议的落地策略

1. **先用 `testOnly + buildOptIn` 登记**（同 hsob），避免影响正式发布列表。
2. **资源挂载拍板后再定 dataProvider**：如果维护者坚持 `.data` 单文件，需要用 `gen_package_manifest.py` 把 9 组件打成一个 `/bro.data`（BRO 运行时加载器要小改）；如果接受分片则零改动。
3. **玩家数**：首期用官方 `STANDARD_MULTIPLAYER_PLAYER_COUNTS=[2,3]`，不额外申请 4 人；BRO 引擎内部 4 槽保留，只是大厅不开放，后续需要时再单独提。
4. **触控列为接入后的第一优先级缺口**：当前只验键盘，官方对触控支持有完整验收项。
5. 所有差集在 `ADAPTATION.md` 里以「待核对/待验证」如实记录，不虚报已实现。
