# BRO Runtime 产物落地说明（eagler fork `bro-integration`）

> 更新：2026-11-21 · 对应 BRO 仓库 `TSake/TouHou-BossRushOnline`（设置对齐后 build_id `14a7b16b`）
> 本目录 `ports/bossrush/web/` 是 **BRO Web runtime 的实际构建产物**，由 BRO 侧 `web/build.sh` 产出后入库。

## 1. 产物清单（24 文件 / 4.2 MB）

| 文件 | 体积 | 说明 |
|---|---|---|
| `web/bro.wasm` | 3.3 MB | 自包含 WASM：纯 C Lua 5.1.5 + `lstg.*` C 绑定 + WebGL2/WebAudio，内嵌 144 个游戏脚本 + Update.txt/manual |
| `web/bro.mjs` | 111 KB | Emscripten ES6 glue（`createBROModule`） |
| `web/src/*.mjs` | ~450 KB | 18 个模块：renderer/postfx/audio/input/storage/netcode/netmesh/input_buffer/rollback/route_*/eagler_adapter/multiplayer 等 |
| `web/dist/*.json` | 257 KB | `package-manifest.json`（972 文件/138.5 MB 清单）/ `app-shell.json` / `runtime.json` |
| `web/manifest.json` | 5 KB | BRO 产品契约（components / musicMounts / packageManifest） |

来源与可复现性记在 `source-import.json`（逐文件 sha256；首次落地 commit `848f8f7`，设置对齐重编 build_id `14a7b16b`）。
重编：`cd <BRO repo> && bash web/build.sh`，产出后同步 `web/` 目录即可。

## 2. 游戏资源包（138.5 MB）——不入库，importOnly

`package-manifest.json` 描述的是 `game/` 下逐文件资源（BGM 96k ogg / 立绘 / 背景 / 字体 / 脚本）。
按 catalog 的 `importOnly: true`，这部分由**玩家导入原始游戏包**或 eagler package-store 分发，
不进本 fork（避免 138 MB 二进制入库）。已排除的项见 manifest 的 `distExcluded`：
BadApple!! 视频分卷 20.7 MB（Web 端视频解码非当前范围，不可玩，不分发）。

## 3. 压缩记录（保证清晰度）

- 音频：158 MB → 55 MB（96 kbps 单声道 ogg，逐文件就地压缩，`.wav` 扩展名保留、内容嗅探解码）。
  **不再降码率**——96k 是清晰度与体积的平衡点。
- BadApple 20.7 MB：从分发清单排除（运行时不加载），总分发 159.2 → 138.5 MB（-13%）。
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
- BRO 侧全套纯 Node 回归 42 项文件全绿；适配层单测 16/16、设置桥接单测 9/9；新 WASM build_id `14a7b16b`，两处署名确认为 `@Tidew`。
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
- 适配层 `applyWebSettings()` 在 `configure` 与 `launch` 各下发一次（应对 WASM boot 时序）；
  游戏侧 `web_settings.lua` 缓存到 `BRO_WEB_SETTINGS`，`BRO_complete_load` 再 `flush()` 重放（幂等）。
- 游戏内设置菜单中 Web/WASM 无效的「全屏模式」(windowed)、「垂直同步」(vsync) 已在补丁层移除，
  `game/` 源码保持原样；分辨率/锁定60帧保留。
- 网页端只对 BRO 显示/隐藏的设置（`state.gameId === 'bro'`，不影响其他作品）：
  - 显示：分辨率、3D背景、Boss法阵、简单UI渲染、显示按键状态、跳帧（对应游戏内「画面/性能」页）。
  - 隐藏：全部触屏设置（本作不提供触屏支持）、高级设置里的「游戏版本/美化版」（本作无美化版）、
    输入设置里的手柄改键（本作不支持手柄）。
- 键盘改键打通：launcher 的 `KeyboardEvent.code` 别名经 `bro-key-bindings.mts` 折算成 BRO 的
  Windows VK 数字，随 `broKeys` 下发；游戏侧只写 `setting.keys[name]` / `setting.keysys[name]`
  （不替换整表，避免 `Linput.lua` 的缓存引用失效），不映射到网页动作的键位保持 BRO 默认值。
- 两端测试：BRO `web/test/web_settings_unit.mjs`（9/9）+ `web/test/eagler_adapter_unit.mjs`（16/16）；
  eagler `game-preparation.test.mjs` 的 `bro`/`bromp` 下发断言 + `bro-key-bindings.test.mjs`（3/3）。
