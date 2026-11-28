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
3. ~~阶段3 导入原版存档~~ **已决定不实现**（2026-11-21）：用户放弃原版存档支持，整包 `importData()` 保持禁用，不再评估。
4. **阶段4 Replay 下载**：放开 `.rep/.multi` 策略。
5. **阶段5 Replay 管理/播放**：`playback.inspect`→暴露 `ReadReplayInfo` 元数据；`open`→`stage.Set("load",...)`；`runtime-info.playback` 状态上报；强制走 `list` 文件会话。
6. **阶段6 线上 manifestUrl 增量接入**：接通 `resource_cache.syncOnline()`（启动链此前从未调用 refreshManifest），配置入口 `configure.options.resourceManifestUrl` / `?manifestUrl=` / meta；offline 降级，不阻塞启动。详见 RUNTIME_ARTIFACTS.md §11。

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
- runtime 入口（`bro.html`/`shell.mjs`）**已落地**，见 `../RUNTIME_ARTIFACTS.md` §8：已含 meta provider、
  数据身份桥、epoch 门禁、`storage.loadAll()` boot 前时序、主循环 `storage.tick(frame)` 与 pagehide/visibilitychange `flush()`。
  旧的 5 个契约缺口（adapter 文件名、缺失目录键盘模块、dataProvider、meta、tick）已由产物摆位与 catalog 声明消解。
- 阶段2~5 见上节，均未开始。

## 9. 阶段1b：Runtime 入口 + 增量资源缓存（2026-11-21，已完成）

- 入口三件套：`web/bro.html` → `web/shell.mjs` → `web/managed-shell.mjs` + `web/src/runtime_entry.mjs`。
- 资源层 `web/src/resource_cache.mjs`：manifest（逐文件 sha256）驱动的内容寻址缓存；
  更新时只拉 hash 变化的资源，未变文件复用缓存，WASM/代码/素材各自独立版本。
- `resources` 命令由“空实现”改成“按 sha256 落地导入包 blob 并延迟回包”，避免宿主 revoke blob URL 后资源丢失。
- 该缓存与 BRO 私有存档 IndexedDB 分离：缓存只放游戏资源，不放 `/User`、`/replay`、`/bro/data`，
  因此不会出现 RGO 那种整包导入覆盖网页设置/补丁表的问题。

## 10. 阶段2：存档下载 / 上传（2026-11-21，已完成）

复用网页通用 UI（`files.ts` 的 `exportFiles('save')` / `importFile('save')`，只读写 `storage.scoreFile`
即 `Score.dat`），但真实缺口在 runtime 侧，已由 `web/src/save_files.mjs` 补齐：

1. **路径不对**：eagler 给的是相对名 `Score.dat`，BRO 游戏本体写 `/User/Score.dat`；
   直接透传会读写 `/Score.dat`（空档）。现在双向映射 eagler 视图 ↔ BRO MEMFS。
2. **时序不对**：`withFileSession()` 要求 `phase=prepared && !launched`，即下载/上传发生在
   `launch` 之前，此时 Lua `BRO_fs_*` 未 boot。现在由 `createLazySaveFS()` 直接用 `Module.FS`，
   首次文件命令才建 Module 并 `storage.loadAll()`，`startBRO()` 复用同一环境。
3. **restart 校验**：import 后 launcher 调 `restart({sync:false})` 再逐字节 `read` 校验。
   `write()` 现立即 `storage.flush()`，新实例 `loadAll()` 读到的就是新字节。
   `remove()` 改走新增的 `storage.removeFile()`，避免 IDB 旧 key 让删除文件复活。
4. **RGO 防护落地**：白名单只放 `Score.dat` / `Spellcard.dat` / `.bak` / `savingN.file` / `replay/*`；
   `setting`（网页权威）与 `/bro/data`（Lua 补丁）拒写，`..` 段级穿越拒写。

验证：`web/test/save_files_unit.mjs` 7/7；BRO 纯 Node 全量 42/42 PASS。
阶段3（原版存档整包导入）**已决定不实现**（用户 2026-11-21："原版存档我想了想不打算支持了"）：
`storage.importData()` 保持禁用，不是暂缓、不再 re-evaluate。存档只保留本节的单文件白名单读写。

## 11. 阶段4/5：Replay 下载 / 管理 / 播放（2026-11-21，已完成）

三处真缺口，不是单纯"放开策略"：

1. **扩展名被 importOnly 排除**：`replay-files.mts` 的 `replayExtensions` 对
   `importOnly:true` 产品跳过扩展名，导致 `.rep/.multi` 不进白名单，`isReplayFilePath()`
   拒绝、文件选择器 accept 也没有。现在显式收录 bro 的 extensions（不动其他 importOnly
   产品，hsob 的 `.dat` 仍走专用路径），并给 `isValidReplayName('bro', ...)` 加
   `slotN.rep/.multi` 特例（仿 hsob 先例）。
2. **列表来源不对**：BRO replay 存在自己的 `BossRush-save` IndexedDB，不在 Emscripten
   IDBFS v21 库。`replayStorage.read()` 冷读拿不到。现在 `replays.ts` 加
   `replayUsesColdStorage()`，BRO 强制走 runtime `list` 文件会话（阶段2 已打通）。
3. **playback 空实现**：adapter 原本把 `playback` 落到 `default:{ok:true,ignored:true}`。
   现在新增：
   - `web/src/replay_playback.lua`（编进 VFS）：`BRO_replay_inspect()` 把
     `plus.ReplayManager.ReadReplayInfo()` 的 STGR 元数据映射成 eagler
     `ReplayDescription`（name/character=0/difficulty=0/stages[{id,score,frames,label}]）；
     `BRO_replay_open()` 复刻 `title.lua` 的 `stage.IsReplay=true; stage.Set('load',...)`；
     `BRO_replay_control()` 对 STGR 无法 O(1) 定位的 `seek` 明确返回不支持；
     `BRO_replay_status()` 供 `runtime-info.playback` 上报。
   - `eagler_adapter.mjs` 加 `playback` case 与 `_cmdPlayback()`：inspect 经
     `bro_eval` 取 description 并原样回包；open 调用 Lua 回放入口后上报
     `runtime-info.playback`；file-session 阶段 Module 未挂 ccall 时主动从
     `fs.environment()` 取一次。
   - 多人 `.multi` 旁路 inspect 给最小合法 description，`open` 时喂 `BRO_mre_load`
     复原远端输入流。

RGO 防护延续：replay 读写仍走单文件白名单 `replay/*`，不启用整包 `importData()`。

验证：`replay_playback_unit.mjs` 6/6（真 WASM，STGR 解析/多关卡/多人/control/status）、
`eagler_adapter_unit.mjs` 22/22（playback 三命令）、replay-files 策略独立校验 4/4、
BRO 纯 Node 全量 43/43 PASS；WASM 重建 build_id=`dbd2ba79`，boot 后回放桥全局函数就绪。
真浏览器端到端冒烟因 BRO 源树缺 `web/game/` 素材目录未能跑通，留待 eagler 完整部署环境验证。
