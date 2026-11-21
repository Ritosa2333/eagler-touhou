# BRO 存档 / Replay 与 eagler 网页端对接：现状与可行性

> 更新：2026-11-21 · 对应 BRO `TSake/TouHou-BossRushOnline` 与 eagler fork `bro-integration`
> 本文记录 3AM 阶段的现状理清结论，供后续分阶段适配参考。**本文只描述现状与方案，不代表已实现。**

## 0. 一句话结论

BRO 的存档/replay **底层读写、持久化、导入导出接口都已写好并通过单测**，但正式游玩入口没接线、协议层有几处形状不对齐。真正的工作是「接线 + 对齐 + 按 RGO 教训约束导入范围」，而不是从零造轮子。

---

## 1. 存档全景

game/ 源码零改动，靠 `web/src/prelude.lua:66-92` 把 `User\xxx`、`replay\xxx` 的路径分隔符转成 `/`，落到 Emscripten MEMFS 根。

| 原生路径 | MEMFS | 格式 | 内容 | 源码读取点 |
|---|---|---|---|---|
| `User\setting` | `/User/setting` | JSON（`format_json(Serialize)`） | 设置：音量/分辨率/键位/锁帧等（网页设置是当前权威源） | `game/launch.lua:240`（启动当场读）、`launcher.lua:7` |
| `User\Score.dat` | `/User/Score.dat` | `sp:LockString(Serialize(scoredata))` 混淆 | 分数/成就/解锁/资金/连续登录/游玩时长 | `game/THlib/lib/Lscoredata.lua:22,100` |
| `User\Spellcard.dat` | `/User/Spellcard.dat` | 同上 | 符卡记录 | `Lscoredata.lua:77,109` |
| `User\saving.file` → `saving1.file` | `/User/saving1.file` | STGR | 续关存档（打完 `os.rename`） | `THlib/plus/Replay.lua:305-390` |
| `replay\slotN.rep` | `/replay/slotN.rep` | STGR 二进制 | 单人录像（slot 1..16） | `THlib/ext/ext_replay.lua:10,66` |
| `replay\slotN.multi` | `/replay/slotN.multi` | `BRO-MULTI-REPLAY` 自定义 | 多人旁路录像 | `web/src/multi_replay.lua:138-172` |

另有备份 `Score.dat.bak`/`Spellcard.dat.bak`（`Lscoredata.lua:118-129`）。

---

## 2. Web/WASM 持久化现状：机制已就绪，正式入口没接线

`web/src/storage.mjs`（`createStorage(Module)`）是完整的持久化层：

- 后端：IndexedDB `BossRush-save` / store `files`，`setting` 额外镜像到 localStorage `BossRush.save.setting`；降级链 `indexeddb → localStorage-only → memory-only`。
- 时序契约（文件头注释 + `web/manifest.json:37-48`）：`loadAll()` **必须在 `bro_boot()` 之前**（launch.lua 当场读 setting）→ `tick(frame)` 每 30 帧 → `flush()` 在 `pagehide`/`visibilitychange`/结算。
- 单测 `web/test/storage_roundtrip.mjs:105-144` 验证四类文件跨实例恢复。

**接线现状（缺口）**：

| 页面 | 接了 createStorage.loadAll 吗 |
|---|---|
| `test/browser/persist.html:11-26` | 是（临时验证页，注明「不进最终 UI」） |
| `test/browser/play.html:84-90` | 否 |
| `test/browser/eagler.html:16-25` | 否（用内存 fs stub） |
| `web/src/eagler_adapter.mjs` | storage 可选注入，仅 `_cmdSync` 用了 `flush()` |

⇒ 现状：在 play.html/eagler.html 玩一局，关掉网页存档会丢。**接入 eagler 必须补 loadAll+tick+flush 三步。**

---

## 3. 导入导出接口现状（三层，都已可用）

1. **`storage.mjs:263-287` 整包 JSON**：`exportData()` → `{format:'bro-save-v1', files:[{path,data(base64)}]}`；`importData()` 校验 format + roots 白名单（`/User`、`/replay`、`/bro/data`）后 `FS.writeFile` 全量覆盖再 `sync(true)`。**目前无任何 UI 接线。**
2. **`web/src/eagler_fs.lua` 单文件字节**：`BRO_fs_read(path)`→`Serialize(number[])`、`BRO_fs_write(path,bytes)`、`BRO_fs_remove(path)`；由 `boot.lua:84` 在 replay sidecar 步 dofile。
3. **`eagler_adapter.mjs:561-627` 协议命令**：`_cmdList/_cmdRead/_cmdWrite/_cmdRemove/_cmdSync` 分派到上面两层。读字节链路已通。

`playback` 命令虽在白名单/阶段表声明，但 `_dispatch` 无对应 case，落到 `default:{ok:true,ignored:true}` —— **播放入口目前是空的**。也没有 `bro_play_replay(path)` 这类全局函数。

---

## 4. Replay 系统

- 录制：`ext_replay.lua` 调 `replayManager:MakeReplayFilename(slot)` → `replay\slotN.rep`；底层 `plus/Replay.lua:226-301` `SaveReplayInfo` 写 STGR（magic `STGR` + 版本 + 游戏/机签信息 + 每关名称/分数/种子/帧数字节流）。每帧 1 字节，`KEY_BIT={up=128,down=64,left=32,right=16,slow=8,shoot=4,spell=2,special=1}`。
- 解析：`Replay.lua:147-209` `ReadReplayInfo(path)` 返回 `{path,stages:[{stageName,score,frameCount,frameDataPosition,...}]}` —— **元数据齐全，可直接喂给 `playback.inspect`**。
- 播放：原生入口 `stage.Set("load", path, stageName)`（`ext_replay.lua:238-276`），置全局 `replayReader`；UI 触发在 `title.lua:339` / `menus/replay_menu.lua`。
- Web IO：`bro_runtime.c:288-374` 提供 `plus` 真身（`l_fs_dir_exists/create_dir/enum_files`），`boot.lua:199-201` 在 `BRO_complete_load` 里 `BRO_enable_replay_io()` 换回真身；STGR 读写经 `plus.FileStream`→`io.open` 落 MEMFS。`replay_roundtrip.mjs` 验证通。

---

## 5. eagler 网页侧机制

### 5.1 存档上传/下载（`launcher/app/services/files.ts`）

不是传 JSON 包，而是**原版 `.dat` 原始字节**：下载 `read`→Blob+`<a download>`；上传 `input[type=file]`→`write({path,bytes})`→`restart({sync:false})`→逐字节校验。UI 在 `SettingsBody.tsx:187-221`（「存档 下载/上传」「录像 下载/管理」）。

协议命令名单（`runtime-protocol.mts:7-34`）：`configure resources keyboard keyboard-clear touch-controls touch-cancel direct-touch launch list read write remove sync`；`playback` 为条件命令（本地 Replay 工作台场景必需）。文件操作经 `withFileSession()` 门禁（`runtime.ts:1300-1388`）。

对作品的要求：`storage.scoreFile`、`list/read/write/remove/sync` 可用、`ready.saveRoot` 与目录一致。**BRO 的 `Score.dat` 格式与原版一致（同一套游戏逻辑），导入可带过成就/解锁/资金。**

### 5.2 导入原版存档 + RGO 的坑

网页「上传存档」**只写 `scoreFile` 一个文件**（`files.ts:168-169`），不碰 setting/补丁，安全。

**RGO 坑的实证**在 BRO 产物 `ports/bossrush/web/src/logger.mjs:98-102`：导入原版存档后 `DiffSelection` 难度表被原版数据覆盖，缺键时难度掉回 Tutorial、`init_fuse(0)==0`。根因是**整目录覆盖缺少「web 拥有文件」的保护**——`storage.importData()` 会覆盖 `/User`（含 web 权威 setting、派生难度表）、`/replay`、`/bro/data`（Lua 补丁）。

⇒ **告诫**：给 BRO 导入只走网页单文件白名单（`Score.dat`/`Spellcard.dat`），**绝不启用 `importData()` 整包导入**，导入后重启 runtime 重放网页设置（web 权威设置与补丁表不会被原版数据覆盖）。

### 5.3 Replay 下载与管理/播放

- 列表来源两条：runtime `list` 命令（`models/replays.ts:147-162`）；冷读 Emscripten IDBFS `FILE_DATA`（`replay-storage.ts:14-61`，不启动游戏也能浏览）。
- 点击播放：点条目 → `playback{action:'inspect',path}` 取详情 → `playback{action:'open',path,stage}`（`browser-session.ts:739-744`）。另有 `configure.options.replayPath/replayViewer` 入口（th09 用）。
- 下载：`read`→Blob；文件夹 zip（`models/replays.ts:256-274,370-399`）。
- 按作品隔离：无单独开关，`replay-manager-ui` 是 inherited capability；差异靠 `replayFilePolicy(product)`（`replay-files.mts`）+ `replayStorageLocation` 的数据库名。

### 5.4 BRO 产品目录现状（`product-catalog.mjs:259-299`）

`testOnly/buildOptIn/importOnly:true`；`storage:{saveRoot:'/bro', scoreFile:'Score.dat', configFiles:['setting','Spellcard.dat']}`；`replay:{prefix:'bro',extensions:['rep','multi']}`。因 `importOnly`，`.rep/.multi` 被 `replay-files.mts:2` 的扩展名白名单排除。

---

## 6. 可行性结论矩阵

| 能力 | 底层是否已就绪 | 缺口 | 可否给 BRO 用 |
|---|---|---|---|
| 存档落 IndexedDB | 是（storage.mjs） | 正式入口没接 loadAll/tick/flush | 接线即可 |
| 存档下载 | 是（read→Blob） | `list` 返回字符串数组非 `{path,size}`；`saveRoot` `/User` vs `/bro` 不一致 | 对齐即可 |
| 存档上传（原版 Score.dat） | 是（write+restart 校验） | 同上对齐 | 对齐即可，安全 |
| 导入原版存档（整包） | 接口在 | 会覆盖 web 设置/补丁（RGO 坑） | **不启用** |
| Replay 下载 | read 已有 | `.rep/.multi` 被 importOnly 过滤 | 放开策略即可 |
| Replay 管理/点击播放 | 积木在（ReadReplayInfo + stage.Set） | `playback` 空实现、无 `runtime-info.playback`、configure 不读 replayPath；BRO 私有存储冷读拿不到 | 需三层薄封装，强制走 `list` 文件会话 |

---

## 7. 分阶段方案

1. **阶段1 运行时持久化接线（前置）**：补 runtime 入口接 `createStorage.loadAll(boot 前)/tick(主循环)/flush(卸载)`；修 `list` 返回形状、`sync` 等待异步 flush、`saveRoot` 对齐。此步与遗留缺口「卡片点启动不出画」是同一块 runtime 入口工作。
2. **阶段2 存档下载/上传**：复用网页通用 UI，只需验证阶段1 的对齐结果。
3. **阶段3 导入原版存档**：单文件白名单 `Score.dat`/`Spellcard.dat`，明确禁用整包 `importData`。
4. **阶段4 Replay 下载**：放开 `.rep/.multi` 策略。
5. **阶段5 Replay 管理/播放**：`playback.inspect`→暴露 `ReadReplayInfo` 元数据；`open`→`stage.Set("load",...)`；`runtime-info.playback` 状态上报；强制走 `list` 文件会话。

> 阶段1 之前需先确认 runtime 入口形态：eagler 期望 `bro.html`/`shell.mjs`，当前 `ports/bossrush/web/` 只有 `bro.mjs`/`bro.wasm`/`storage.mjs`/`eagler_adapter.mjs` 等，入口尚未落地。

---

## 8. 阶段1 进展（2026-11-21，已完成部分）

### 已完成：adapter 三处协议对齐（纯逻辑，两侧 adapter 已同步）
- `ready.saveRoot`：默认从 `/User` 改为 `/bro`（对齐 `product-catalog.mjs` 的 `storage.saveRoot`），可由构造参数 `saveRoot` 覆盖。
- `_cmdList`：统一返回 `{path,size}` 对象数组，兼容字符串/对象两种 fs 输入；这是 launcher `listing()`（`files.ts:59-65`）要求的形状，此前返回裸字符串数组导致网页读不到列表。
- `_cmdSync`：`flush()` 返回 Promise 时通过 `deferred` 让 `_handleMessage` **等落盘完成才回包**，response 不再塞不可序列化的 `promise`/`async` 字段；launcher 的 `await sync()` 现在真正等到数据落 IndexedDB。

### 验证
- `web/test/eagler_adapter_unit.mjs`：18/18（新增 saveRoot 覆盖、sync 等待/回包形状两例）。
- 真浏览器 e2e：`web/test/browser/storage_bridge.html` + `storage_bridge_check.mjs`（真 IndexedDB + 真 `bro_eval` + 真 postMessage）：saveRoot=/bro、list 形状、read 字节数组、sync 等 flush、跨刷新 Score.dat 仍在——**8/8 PASS**。
- 基线 `persist_check.mjs` 跨刷新闭环仍 ALL PASS。

### 未完成（阶段1 余下 / 后续阶段）
- runtime 入口（`bro.html`/`shell.mjs`）落地：需把 `persist.html` 的 loadAll 时序、`play.html` 的渲染主循环、`eagler.html` 的 adapter 三者合流。**落地前必须先解 5 个契约缺口**：
  1. catalog 写 `eagler-adapter.mjs`，实际文件是 `eagler_adapter.mjs`（连字符 vs 下划线）。
  2. catalog 声明 `directory-keyboard.mjs`，BRO 目录缺该文件（共享版在 `common/browser/directory-keyboard.mjs`）。
  3. `dataProvider:'emscripten-preload'` 只允许 flat layout，与 `runtimeFileLayout:'directory'` 冲突。
  4. 目录型 HTML 必须含 `<meta name="eagler-data-provider" content="...">`，且协议壳要 `__eaglerPrepareManagedRuntimeDataV1` 桥。
  5. 主循环里尚未接 `storage.tick(frame)`（30 帧节流），尾帧 `flush` 待入口落地后接。
- 阶段2~5 见上节，均未开始。
