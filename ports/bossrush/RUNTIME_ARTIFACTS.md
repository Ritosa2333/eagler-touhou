# BRO Runtime 产物落地说明（eagler fork `bro-integration`）

> 更新：2026-11-20 · 对应 BRO 仓库 `TSake/TouHou-BossRushOnline` 分支 `web/multiplayer-port`
> 本目录 `ports/bossrush/web/` 是 **BRO Web runtime 的实际构建产物**，由 BRO 侧 `web/build.sh` 产出后入库。

## 1. 产物清单（24 文件 / 4.2 MB）

| 文件 | 体积 | 说明 |
|---|---|---|
| `web/bro.wasm` | 3.3 MB | 自包含 WASM：纯 C Lua 5.1.5 + `lstg.*` C 绑定 + WebGL2/WebAudio，内嵌 144 个游戏脚本 + Update.txt/manual |
| `web/bro.mjs` | 111 KB | Emscripten ES6 glue（`createBROModule`） |
| `web/src/*.mjs` | ~450 KB | 18 个模块：renderer/postfx/audio/input/storage/netcode/netmesh/input_buffer/rollback/route_*/eagler_adapter/multiplayer 等 |
| `web/dist/*.json` | 257 KB | `package-manifest.json`（972 文件/138.5 MB 清单）/ `app-shell.json` / `runtime.json` |
| `web/manifest.json` | 5 KB | BRO 产品契约（components / musicMounts / packageManifest） |

来源与可复现性记在 `source-import.json`（BRO commit `848f8f7`、build_id `eb0494bb`、逐文件 sha256）。
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

- `node tools/docs/check.mjs`：671 文档 / 27 源码目录通过。
- `ports:check --game=bro`：本机缺 `fs-native-extensions` 依赖无法运行（与既有状态一致，非本次回归）。
- BRO 侧全套纯 Node 回归 44/44 PASS；新 WASM build_id `eb0494bb`，两处署名确认为 `@Tidew`。
- 联机/流畅度评估与剩余缺口见 BRO 仓库 `web/PROBE_3AK.md`。
