# BRO Runtime 产物落地说明（eagler fork `bro-integration`）

> 更新：2026-11-25 · 对应 BRO 仓库 `TSake/TouHou-BossRushOnline`（3AY nearest-player + 3AZ 联机主循环/远端真身 + 3BA 双实例 e2e/卡外对象绑定 + 3BD 纯视觉GHOST分叉定性/3BE 3人房后 build_id `5df2b19a`；第七轮 3BF 正式房间默认 arm 轻量回滚+回滚支持 3 人 / 3BG 纯观战端跳过联机门面，仅 JS 改动；第八轮 3BI 回滚单人 Unlock* 全解锁、岐道四季 card 177 补 pin_task，Lua 有改，build_id `715a2bbd`）
> 本目录 `ports/bossrush/web/` 是 **BRO Web runtime 的实际构建产物**，由 BRO 侧 `web/build.sh` 产出后入库。

## 1. 产物清单（31 文件 / 28 MB）

| 文件 | 体积 | 说明 |
|---|---|---|
| `web/bro.wasm` | 26.9 MB | 自包含 WASM：纯 C Lua 5.1.5 + `lstg.*` C 绑定 + WebGL2/WebAudio，内嵌 167 个游戏脚本 + BadApple 逐帧数据 + Update.txt/manual（HTTP gzip 传输约 4.7 MB，初始内存 80 MB）。**2026-11-24**：此前游戏树里还残留一份 BadApple 副本被 `--embed-file` 重复内嵌（21 MB），清理后 46.3 → 26.9 MB。 |
| `web/bro.mjs` | 111 KB | Emscripten ES6 glue（`createBROModule`） |
| `web/src/*.mjs` | ~490 KB | 21 个模块：renderer/postfx/audio/input/storage/save_files/netcode/netmesh/input_buffer/rollback/route_*/eagler_adapter/multiplayer 等 |
| `web/dist/*.json` | 272 KB | `package-manifest.json`（1004 文件/162.2 MiB 清单，SE 已恢复原生 PCM）/ `app-shell.json` / `runtime.json` |
| `web/manifest.json` | 5 KB | BRO 产品契约（components / musicMounts / packageManifest） |

来源与可复现性记在 `source-import.json`（31 项逐文件 sha256；首次落地 commit `848f8f7`，当前 build_id `715a2bbd`（BRO 3BI 提交 9b6ad0a，Lua 有改：boot.lua/web_patches.lua 回滚单人解锁、spell_player_mp.lua 岐道四季 card 177 补 pin_task；3AB~3BB 的 nearest-player/自动生成/同帧诞生/本地槽修复、3BD digest 跳过 GHOST 组均在；第七轮 3BF/3BG 仅 JS 改动——回滚接线/3 人修复 + 纯观战端门面守卫）。
重编：`cd <BRO repo> && bash web/build.sh`，产出后同步 `web/` 目录即可。

## 2. 游戏资源包（162.2 MiB 资源 / 约 170.7 MB ZIP）——不入库，importOnly

`package-manifest.json` 描述的是 `game/` 下逐文件资源（BGM 96k ogg / 立绘 / 背景 / 字体 / 脚本）。
按 catalog 的 `importOnly: true`，这部分由**玩家导入原始游戏包**或 eagler package-store 分发，
不进本 fork（避免 138 MB 二进制入库）。已排除的项见 manifest 的 `distExcluded`：
BadApple!! 视频分卷 20.7 MB（Web 端视频解码非当前范围，不可玩，不分发）。

## 3. 压缩记录（保证清晰度）

- 音频：158 MB → 55 MB（96 kbps 单声道 ogg，逐文件就地压缩，`.wav` 扩展名保留、内容嗅探解码）。
  **不再降码率**——96k 是清晰度与体积的平衡点。
- **2026-11-22 修订**：音效（SE）恢复原生 PCM 不再压缩（178 文件 / 24.4 MB，其中 `THlib/se/` 175 个），
  只有 BGM 保持 96 kbps mono。分包体积 138.5 → 169.8 MB。若玩家想要原始 BGM，
  导入**可选音乐包**覆盖即可（路径指针覆写，不重建 generation）——见
  [`docs/MUSIC_PACK.md`](docs/MUSIC_PACK.md)。
- BadApple 20.7 MB：从分发清单排除（运行时不加载）。
- 2026-11-22 起 SE 恢复原生 PCM，清单 138.5 → 162.2 MiB（+23.7）。
- 字体子集化 **未做**：收益仅约 7 MB，但有动态文本缺字风险，且破坏 LFS 可回滚性，稳妥优先。
- 图片未降分辨率：用户要求保证清晰度。
- WASM 3.3 MB：主体是内嵌脚本，去调试导出仅省 KB 级且会破坏回归断言，保留。

## 4. 署名（用户 2026-11-20 指定）

- **游戏作者**：吃药图书（Cyoltose）—— 原作 `https://github.com/Cyoltose509/boss-rush`
- **网页端维护**：清酒（Tidew）—— BRO Web 迁移/联机适配
- **eagler 开发者**：接入平台与大厅
- 落地位置：
  - `common/contracts/product-catalog.mjs` 的 `bro.support.credit.name`（三方署名）
  - 游戏内结尾 Special Thanks：`@Tidew`（BRO 侧 `game/mod/summary.lua` 与 `th16AEX/summary.lua`，已编进 WASM）

## 5. workspace.json / runtime-builds.json 的取舍（用户："看情况"）

- **workspace.json**：已加 `"bro": "ports/bossrush"`（纯目录映射，安全，让 fork 能定位产物）。
- **runtime-builds.json**：**刻意不注册**。
  原因：`common/contracts/product-catalog.mjs` 中 bro 以 `importOnly + testOnly + buildOptIn` 登记，
  `ports/bossrush/ADAPTATION.md` 已声明不提供原生源码配方；一旦在 runtime-builds.json 注册，
  `ports:check` 的 source-contract/native 门禁即适用，而 BRO 是 Lua/WASM 自制栈，无 CMake/portable 源码可满足。
  这是经评估的**例外方案**：产物直接入库 + importOnly，不走源码构建门禁。
  若 eagler 维护者要求纳入标准构建，需另写 `ports/bossrush/scripts/build-eagler.mjs` 与适配层，作为后续 PR。

## 6. 验证状态

- `node tools/docs/check.mjs`：672 文档 / 27 源码目录通过（含新增文档索引）。
- `ports:check --game=bro`：本机缺 `fs-native-extensions` 依赖无法运行（与既有状态一致，非本次回归）。
- BRO 侧全套纯 Node 回归 42 项文件全绿；适配层单测 19/19、设置桥接单测 9/9、资源缓存单测 4/4；新 WASM build_id `14a7b16b`，两处署名确认为 `@Tidew`。
- 联机/流畅度评估与剩余缺口见 BRO 仓库 `web/PROBE_3AK.md`。

## 7. 网页设置对齐（2026-11-21）

网页设置是权威来源（launcher 按 `gameId='bro'` 存 localStorage，与单人/联机共享 `eagler-touhou-game-options-v1-bro`），
经 `configure.options` 下发，游戏本体应用；这样避免"网站调了设置但游戏本体不生效"。

- 下发字段（`game === 'bro'` 时才写入 wire，其他作品不带）：`broResolution`（输出高度 360–1440）、
  `broDisplayBG`（3D 背景）、`broAuraDistortion`（Boss 法阵）、`broEasyRender`（简单 UI 渲染）、
  `broDisplayKey`（显示按键状态）、`broBgmVolume`/`broSfxVolume`（复用 launcher 通用音量滑块）、
  `broLock60`（复用 `frameLimit60Enabled`）、`broFrameskip`（跳帧）、`broKeys`（网页改键折算后的 VK）。
- 音量必须走这条链路：网页 WebAudio 劫持只 wrap `createScriptProcessor/createChannelMerger`，
  而 BRO 的 `audio.mjs` 用 `createGain` 直连 destination —— 网页音量原本对 BRO **完全无效**。
  现由游戏本体 `BRO_web_apply_settings` 写 `setting.bgmvolume/sevolume` 并调 `SetBGMVolume/SetSEVolume` 生效。
- 适配层 `configure` 与 `launch` 各尝试一次下发，但两者都早于 `bro_boot`（`ccall` 未注入、
  Lua VM 未建立），实际是 no-op。审计后新增 `applyAfterBoot()`：由 `runtime_entry.startBRO()`
  在 `bro_boot` 成功后、进关前调用，才是网页设置与房间选项的真正下发点；
  游戏侧 `web_settings.lua` 缓存到 `BRO_WEB_SETTINGS`，`BRO_complete_load` 再 `flush()`（幂等）。
- 游戏内设置菜单中 Web/WASM 无效的「全屏模式」(windowed)、「垂直同步」(vsync) 已在补丁层移除，
  `game/` 源码保持原样；分辨率/锁定60帧保留。
- 网页端只对 BRO 显示/隐藏的设置（`state.gameId === 'bro'`，不影响其他作品）：
  - 显示：分辨率、3D背景、Boss法阵、简单UI渲染、显示按键状态、跳帧（对应游戏内「画面/性能」页）。
  - 隐藏：全部触屏设置（本作不提供触屏支持）、高级设置里的「游戏版本/美化版」（本作无美化版）、
    输入设置里的手柄改键（本作不支持手柄）。
- 键盘改键打通：launcher 的 `KeyboardEvent.code` 别名经 `bro-key-bindings.mts` 折算成 BRO 的
  Windows VK 数字，随 `broKeys` 下发；游戏侧只写 `setting.keys[name]` / `setting.keysys[name]`
  （不替换整表，避免 `Linput.lua` 的缓存引用失效），不映射到网页动作的键位保持 BRO 默认值。
- 两端测试：BRO `web/test/web_settings_unit.mjs`（9/9）+ `web/test/eagler_adapter_unit.mjs`（19/19）；
  eagler `game-preparation.test.mjs` 的 `bro`/`bromp` 下发断言 + `bro-key-bindings.test.mjs`（3/3）。

## 8. Runtime 入口与增量更新（2026-11-21）

### 8.1 入口形态

- `web/bro.html`：eagler 目录型 runtime 入口；`<meta name="eagler-data-provider" content="retail-memory">`
  与 `window.parent.__eaglerPrepareManagedRuntimeDataV1` 身份桥已按契约接入。
- `web/managed-shell.mjs`：`eagler-touhou/1` 协议壳，做 epoch / origin / source 门禁；`resources` 与 `launch`
  均为异步命令，等资源落缓存/游戏 boot 完成才回包。
- `web/src/runtime_entry.mjs`：把 `storage.loadAll()`（boot 前）→ `bro_boot()` → renderer/audio/input →
  固定 60Hz 逻辑帧 + rAF 呈现 → `storage.tick(frame)` / `flush()` 合流，补上原先 play.html 没接的存档时序。
- `web/src/eagler_adapter.mjs`：`resources` 不再空实现；launcher 送来的 blob URL 当场读取并按 sha256 缓存。

### 8.2 可更新性设计（用户硬要求：不重下 138MB）

- 版本真相是 `dist/package-manifest.json` 的全体 `(path, sha256)` 摘要，不是手写“整包版本号”。
- 浏览器缓存按 **内容寻址**：`https://bro.invalid/blob/<sha256>` 对应一个 Cache API 条目；
  文件名只是 manifest 里的索引。
- 更新流程：拉新 manifest → 与本地 manifest 逐文件对比 → 只下载 `added/changed` → 原子切换 active manifest；
  未变文件命中同一 hash，零下载；旧 manifest 留一代用于回滚/GC。
- WASM（3.3MB）/ App Shell / 素材包各自独立版本：改一行 Lua 只重编并下载 WASM，不碰 138MB 资源。
- 当前分发仍是导入包（catalog `importOnly: true`）：resources 命令把导入文件按同一 hash 空间落地；
  后续接线上分发只需让 manifest URL 指向线上地址，`prefetch()` 已留好按需更新接口。
- 验证：`resource_cache_unit.mjs` 4/4（hash 差异、缓存命中、版本切换、导入包路径覆盖）；
  eagler 协议壳/数据 provider 契约扫描通过，`tools/docs/check.mjs` 673 文档通过。

## 9. 存档下载 / 上传桥接（阶段2，2026-11-21）

- 新增 `web/src/save_files.mjs`：eagler 相对路径 `Score.dat` ↔ BRO MEMFS `/User/Score.dat` 双向映射；
  `replay/slotN.rep` ↔ `/replay/slotN.rep`。此前直接透传会落到 `/Score.dat`（空档），
  且 launcher 的 file session 在 `launch` 之前发生，Lua `BRO_fs_*` 尚未 boot。
- 现在走 `Module.FS` + `createLazySaveFS()`：首次文件命令才建 Module 并 `storage.loadAll()`，
  `startBRO()` 复用同一环境（不重复加载 WASM / 不破坏 boot 前时序）。
- 写入立即 `storage.flush()`：`restart({sync:false})` 重建实例后，新 `loadAll()` 读到的就是刚导入的字节，
  通过 launcher 的逐字节校验。删除走新增的 `storage.removeFile()`，否则 IDB 旧 key 会让文件"复活"。
- RGO 坑防护：单文件白名单只放行 `Score.dat` / `Spellcard.dat` / `.bak` / `savingN.file` / `replay/*`；
  `/User/setting`（网页权威源）与 `/bro/data`（Lua 补丁表）永不接受宿主写入，段级 `..` 穿越直接拒绝。
- `eagler_adapter.mjs` 的 `list/read/write/remove` 支持异步 fs 并通过 `deferred` 等真实结果才回包。
- 验证：`save_files_unit.mjs` 7/7；BRO 全量纯 Node 回归 42/42 PASS。

## 10. Replay 下载 / 管理 / 播放（阶段4/5，2026-11-21）

- eagler 侧策略：`replay-files.mts` 显式收录 bro 的 `rep/multi` 扩展（不影响 hsob 等其它
  importOnly 产品），`isValidReplayName('bro',...)` 认 `slotN.rep/.multi`；`replays.ts`
  的 `replayUsesColdStorage()` 让 BRO 跳过 IDBFS 冷读、强制走 runtime `list` 文件会话
  （BRO replay 在自己的 `BossRush-save` IndexedDB，IDBFS v21 库里没有）。
- BRO 侧回放桥：新增 `web/src/replay_playback.lua`（编进 VFS），`BRO_replay_inspect()` 把
  STGR `ReadReplayInfo` 元数据映射成 eagler description；`BRO_replay_open()` 复刻原生
  `stage.IsReplay=true; stage.Set('load',...)`；`BRO_replay_control()` 对 `seek` 明确
  返回不支持；`BRO_replay_status()` 供 `runtime-info.playback`。多人 `.multi` 旁路
  inspect 给最小 description，open 喂 `BRO_mre_load` 复原远端输入。
- `eagler_adapter.mjs` 补 `playback` 命令分派与 `_cmdPlayback()`：inspect/open/控制/relocate
  全实现，file-session 阶段主动从 `fs.environment()` 取 ccall，open 后上报 runtime-info。
- WASM 重建（build_id `dbd2ba79`）把 boot.lua 新 step 与回放桥编进 VFS；source-import 30 项
  哈希刷新，bro.wasm 同步。
- 验证：`replay_playback_unit.mjs` 6/6（真 WASM）、`eagler_adapter_unit.mjs` 22/22、
  replay-files 策略独立校验 4/4、BRO 全量纯 Node 43/43 PASS；boot 后回放桥全局函数就绪。
  真浏览器端到端留待 eagler 完整部署环境（BRO 源树无 `web/game/` 素材）。

### 10.1 真机回放视觉确认（2026-11-21，已完成）

新增 `web/test/browser/replay_playback_e2e.mjs`（Edge headless + CDP，零依赖，测试服 8767）：
play.html 起真 WASM → `BRO_enable_replay_io()` + `CreateDirectory('replay')` → 生成 120 帧真 STGR
→ `BRO_replay_inspect` → `BRO_replay_open` → 确认画面出画且像素随时间变化 → 截图
`web/build/bro_replay_screen.png`（TH06 关卡 + Boss + HUD + Stage Clear 面板，paintedPct 29.5%）、零运行时错误。
**过程中发现并修复两个真 bug**（此前单测 mock 未暴露）：
1. `replay_playback.lua` 的 `M.open()` 从不写 `state`，`BRO_replay_status()` 的 path/stage/playing
   恒空/idle → open 后写 `state.path/stageName/action/playing`，control 的 pause/play 同步 playing，
   status 从 `ext.replay.IsReplay()` 兜底推断 playing。
2. `M.status().frames` 误用 `GetCurrentReplayIdx()`（返回**关卡序号** replayStageIdx，非帧号，恒 1）。
   字段保留但语义明确为关卡序号；回放推进改由 E2E 的画面像素变化验证。
WASM 重编 build_id=`7bc47a91`，replay_playback_unit 7/7（新增 pause/play→status），
真机 E2E PASS。runtime.json / source-import 30/30 / app-shell 56/56 哈希已刷新。

## 11. 线上 manifestUrl 增量接入（2026-11-21）

用户硬要求延续（§8.2）：BRO 修 bug 频繁，资源必须增量更新，不能每次重下 138MB。
§8.2 已把 `resource_cache.mjs` 的 manifest+sha256 内容寻址能力做好，但此前**启动链从未调用
`refreshManifest`**，`manifestUrl` 也写死为本地 `./dist/package-manifest.json`——线上分发时
无法真正增量。本轮把这条链路接通：

- `resource_cache.mjs` 新增 `setManifestUrl(url)` 与 `syncOnline(options)`：
  - `syncOnline` = 拉清单 → `planManifestUpdate` 算差集 → `prefetch` 只下变化文件 → 提交新版本；
  - **不可达返回 `{status:'offline'}` 而非抛出**：导入包形态下本地清单不存在时静默降级到导入包缓存，
    不阻塞启动；清单一致返回 `current`（零下载）；有变化返回 `updated`（含 downloads/downloadBytes）。
  - `refreshManifest` 保持纯语义（只算差集不提交），提交由 `syncOnline`/`commitManifest` 显式完成。
- 配置入口（三选一，优先级从高到低）：`configure.options.resourceManifestUrl`（宿主下发）>
  URL `?manifestUrl=` > `<meta name="eagler-bro-manifest" content="...">`（`bro.html` 已留注释示例）
  > 默认本地 `./dist/package-manifest.json`。`assetBase` 亦可 `?assetBase=` 覆盖。
- `shell.mjs` 在 `launch` 前（WASM boot 前）执行 `syncOnline`，进度与结果以 `transfer` 事件
  （`phase=manifest` / `manifest-progress`）上报宿主；adapter 在 `configure` 阶段只登记地址、
  零网络，`runtime-info` 回传当前 manifestUrl。**未新增协议命令**（`runtime-protocol.mts` 的 13
  必需命令不变，复用开放的 `configure.options` 字段）。
- 验证：`resource_cache_unit.mjs` 8/8（新增 offline 降级 / current 零下载 / updated 增量 / setManifestUrl）、
  `eagler_adapter_unit.mjs` 23/23（新增 configure 下发地址）、BRO 全量纯 Node 44/44 PASS。
  source-import 30/30、app-shell 56/56 哈希一致（按 BRO 源树算）。本轮审计修复重编 WASM，build_id `544898ba`。
- 边界：线上清单仍需部署方提供（CDN/静态站），本仓库只保证"给了地址就能增量"；清单 sha256
  按路径逐文件校验，缓存只放游戏资源，不碰 `/User` `/replay` `/bro/data`（延续 RGO 防护）。

## 12. 决定不实现：原版存档整包导入

用户 2026-11-21 明确："原版存档我想了想不打算支持了"。故阶段3（导入 `boss-rush.exe` 原版存档）
**正式放弃**，不是暂缓：
- `storage.importData()` 整包导入保持禁用（它曾导致 RGO 难度表被覆盖掉回 Tutorial）。
- 存档只保留网页单文件白名单读写（`Score.dat`/`Spellcard.dat`/`.bak`/`savingN.file`），
  与 §9 一致。此决定已同步 BRO 侧 README/记忆与本文档，后续不再 re-evaluate。

## 13. 4 项游戏本体 bug / 多人规则修复（2026-11-21，阶段 3AN）

用户交代并全部完成，WASM build_id=`7fa07d9b`（BRO 纯 Node 57/57 + 真机端到端零错误）：

1. **X 键按一次后持续按下**（按键显示可见）：根因是中文输入法驻留时 keyup 被吞成 IME
   形状 → `clearAll` 把 X 记进 clearRecent 收养表 → 残留 repeat 接回 → 字母键不自愈。
   修 `web/src/input.mjs`（BRO 与 fork 同步）：keyup 优先用 `e.code` 精确释放；`clearAll`
   加 `noAdopt` 参数，IME/unmapped keyup 清表不收养。方向键走位自愈不受影响。
2. **符卡实时间计时异常**：`web/src/bro_runtime.c` 的 StopWatch 用全局帧数且 `Reset()`
   空操作 → real_timer 从游戏启动累计。改为每实例起点帧 + Reset 真归零；ext.lua 差值
   与 Lstage 关卡耗时语义保持。新单测 `stopwatch_unit.mjs` 6/6。
3. **多人数据统一 P1**：新 `web/src/p1_data.lua`（编进 WASM）。收卡率 X/Y（startSpellHist
   的 player key）与随自机变化的符卡（seasonid、th08/th11 读 player_name）此前按各客户端
   localSlot 取、不一致。hook `stage.Option` 尾部，自机实体按本机角色生成后把全局
   `player_name`/`seasonid` 钉到 P1(slot0)；单人零变化。`p1_data_unit.mjs` 5/5。
4. **多人 boss 伤害减免**：新 `web/src/boss_damage_scale.lua`（编进 WASM）。hook
   `boss.take_damage`/`take_damage2`，按在线人数缩放：2人0.75、3人2/3、4人0.5（4人值
   用户未指定、暂定可改），单人恒1。`boss_damage_scale_unit.mjs` 5/5。

## 14. 新旧弹型贴图切换（2026-11-21，阶段 3AO）

网页设置新增「弹幕贴图：新贴图 / 原生贴图」，替换原对 BRO 隐藏的「游戏版本 / 原版 / 美化版」
位置（其他作品不受影响）。BRO 5.x 用柔光渐变弹幕，原生贴图是早期 v4.1.3 的硬边像素风经典弹幕。

- **素材来源**：图书提供 v4.0.0 发布包 + 口令 `zs235711131719`，从 ZipCrypto 加密的 `core.dat`
  解出 9 张旧弹幕图，SHA256 与 2023 年 git 删除时的 LFS oid 逐位吻合。落
  `game/THlib/bullet/img/legacy/`，走现有 manifest 差量下发（新增 9 项，约 500KB）。
- **判定隔离**：判定是对象字段 a/b，与弹型贴图无关（全局无 SetCollider）；切换只改渲染
  用的 image 名，a/b/size/逻辑一概不碰。
- **实现**：BRO 侧新 `web/src/bullet_texture.lua`（编进 WASM），用旧坐标注册独立 `*_lg`
  纹理/image（避开 WebGL 按名缓存 + 旧图 16px 格子保持 scale=1），hook 34 个弹型类 init
  按 `BRO_WEB_SETTINGS.bulletTexture` 改写 self.img。3 个新版独有弹型（arrow_big_c/diamond/
  money_big）旧图无对应，混合方案保持新图。
- **eagler 侧本仓改动**：`launcher/src/launcher/game-preferences.mts` 加
  `broBulletTexture: 'new'|'legacy'` + normalize/持久化；`game-preparation.ts` configure.options
  下发；`SettingsBody.tsx` 把版本位对 BRO 改成贴图选择器；中/英/日 + keys.json 加 4 键；
  `ports/bossrush/web/src/eagler_adapter.mjs` 加字符串白名单映射。零新协议命令。
- **验证**：BRO 单测 7/7 + 纯 Node 全量 58/58；真机 e2e 9 张旧图 HTTP 200、两态画面不同、
  判定不变、零错误；本仓 `bun run typecheck` exit=0。BRO WASM build_id=`041646ef`。
- 边界：只切敌方弹型贴图；ball_light 新旧皆柔光球截图差异不显，硬边弹型在对应符卡显现。


## 15. 启动加载优化（2026-11-24，#35）

真机反馈：搬到 eagler 上「加载进去要很久」，联机因加载超时无法测试。headless + CDP 测得
根因是三处**串行等待**加一处**冗余体积**：

1. `shell.mjs` 的 `start()` 先 `await resourceCache.syncOnline()` —— 首次访问要**下载完整
   资源包**（1004 文件）才继续，并发上限只有 4。
2. `runtime_entry.mjs` 的 `startBRO()` `await renderer.requestAll({})` —— 首帧前等**全部
   536 张贴图** + 字体。
3. `await audio.warm()` —— 首帧前解码 179 个 SE。
4. `bro.wasm` 46.3 MB：`--embed-file 'vfs/bro/game@/bro/game'` 里还残留一份 Nov 22 时代的
   `BadApple`（21 MB）副本，与 `vfs/root/BadApple` 重复内嵌。

改法（BRO 仓 `8b9a585`）：

- `resource_cache.mjs`：`syncOnline({commitOnly:true})` 只取清单并提交、立即返回；
  新增 `prefetchManifest({groups,concurrency,onProgress,signal})` 按组件组后台预取。
- `shell.mjs`：`start()` 改成「提交清单 → 立刻启动」，`waitFirstFrame()` 轮询
  `runtime.frames > 0`，**首帧之后**再后台预热 `images-ui/fonts/shader/misc/scripts/images-bg`，
  每 10 个文件或 800 ms 报一次 `transfer` 事件。
- `runtime_entry.mjs`：贴图预热带预算（boot 后 800 ms、进关后 600 ms，字体仍同步等完）；
  SE 预热 setTimeout 1200 ms 后并行进行，不再阻塞首帧。
- `web/build.sh`：`rm -rf "$VFS/bro/game/BadApple"`，wasm 46.3 → 26.9 MB（`build_id`
  `96902659`）。

实测（headless Edge + CDP，2 MB/s + 80 ms RTT + gzip）：

| 指标 | 改前 | 改后 |
|---|---|---|
| 首帧 | 90.3 s | **12.7 s** |
| 首帧前请求数 | 1663 | **260** |
| 首帧前字节 | 223.8 MB | **80.1 MB** |
| 本机不限速首帧 | 3751 ms | **2675 ms** |
| bro.wasm | 48,646,195 B | **26,897,155 B** |

标题页与 `?stage=TH11@BossRush` 关卡截图均正常出画（贴图 172/536、0 failed，剩余在后台
继续预热），控制台零错误；`BadApple` 去重后 `io.open('BadApple\\file1')` 仍可读。

单测：`resource_cache_unit` 10/10（新增 commitOnly / prefetchManifest 组过滤两条）、
新 `boot_preload_budget_unit` 19/19、BRO 全量 61 个单测全绿。

复现上述数据：

```bash
cd ports/bossrush/web
node tools/serve-dev.mjs --port 8801 --gz 1     # /game/ 默认指向同级的 BRO 仓库 game/
node tools/load-probe.mjs --port 8801 --kbps 2000 --latency 80 --settle 2000
```

`tools/serve-dev.mjs` = 本地静态服（`bro.html?standalone=1` 可脱离启动器自启动，`/game/` 指向 BRO 的
`game/`，`--gz 1` 现压并缓存）；`tools/load-probe.mjs` = headless Edge + CDP 探针，输出
`toFirstFrameMs`/`requests`/`megabytes`/渲染器状态。用新探针复测：2 MB/s + 80 ms 下首帧 **17.5 s**
（其中字体 12 MB、wasm gzip 后 2.3 MB 是大头），旧行为受「先下完整包 191 MB」限制 ≥95 s。

同步注意：`web/dist/runtime.json`、`web/dist/app-shell.json` 直接取 BRO 仓产物；
`web/dist/package-manifest.json` 留端口本地版本（音乐包为 96 kbps mono，体积与 BRO 全质量包
不同），**不要**用 BRO 的覆盖。`web/src/*.mjs` 全部按 BRO 仓同步（含新增 `bgm_stream.mjs`）。
