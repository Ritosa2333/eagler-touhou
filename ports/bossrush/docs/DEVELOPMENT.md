# BossRush 开发指南

[作品入口](../README.md) · [适配检查表](../ADAPTATION.md) · [适配总指南](../../../docs/adaptation/README.md) · [接入提案](../proposals/README.md)

## 1. 源码与归属

- 网页化产物库（我们的 fork）：`https://github.com/TSake/TouHou-BossRushOnline`，分支 `web/multiplayer-port`。
- 游戏原作：`https://github.com/Cyoltose509/boss-rush`（upstream，只读，绝不 push）。
- 本作是 LuaSTG Sub v0x1000 + LuaJIT 的第三方衍生 Boss Rush 游戏，v5.1.2。
- 许可口径：启动器 GPLv3+；本作素材归属吃药图书（Cyoltose），Web 化由清酒参与——**正式接入前须由维护者确认第三方衍生作的授权与署名方式**。

## 2. Runtime 形态与加载契约

- 单一自包含 WASM：纯 C Lua 5.1.5 + `lstg.*` C 绑定 + WebGL2 / WebAudio；Emscripten 模块导出 `createBROModule`。
- 启动顺序：`loadAll()`（IndexedDB 存档）→ `bro_boot()` → 帧循环 `bro_frame` / `bro_render`。
- 资源：png / ogg 走 JS HTTP（不进 MEMFS）；先取 `scripts.json`，再并行取分片写 MEMFS。
- 默认逻辑 1280×720 / 60Hz，渲染 rAF 可跟随刷新率。
- 入口协议：`web/src/eagler_adapter.mjs` 实现 `eagler-touhou/1` 信封（13 命令 / 8 事件，epoch 从 URL `runtimeEpoch` 校验）。
- 存档：IndexedDB `BossRush-save` + MEMFS `/User`、`/replay`、`/bro/data`；`setting` 有 localStorage 镜像。

## 3. 构建

Runtime 由 BRO 产物库构建，不在本 monorepo 内编译：

```sh
# BRO 仓库根目录（不是本 monorepo 根目录）
bash web/build.sh                      # 重编 bro.wasm，产出 web/build/bro.{mjs,wasm}
node web/test/node_smoke.mjs           # 启动链冒烟
```

- 依赖：emsdk + Lua 5.1.5 源码复用 RGL `.toolchain/`（用 `BRO_EMSDK`/`BRO_LUA`/`BRO_EMCACHE` 覆盖），不在仓库内复制、不全局安装。
- Python 工具：`web/tools/gen_asset_manifest.py`（图片尺寸表）、`gen_package_manifest.py`（分包清单）等。
- 打包产物：`web/build/bro.{mjs,wasm}` + `web/dist/*.json` 分包清单。
- 把产物按 `proposals/config-patches/` 的清单摆位到启动器 runtime 目录，是正式接入阶段的工作（见[接入提案](../proposals/README.md)）。

## 4. 与本作相关的差异（接入时注意）

1. **游戏 ID**：用 `bro`（联机 `bromp`），与产品目录登记一致。
2. **玩家数**：首期只上 `[2,3]`（用官方 `STANDARD_MULTIPLAYER_PLAYER_COUNTS`）。BRO 引擎内部是 4 槽（`BRO_MAX_PLAYERS=4`），只是大厅暂不开放第 4 席；官方共享平台目前只接受声明 2P/3P 子集，后续要开需平台侧先放开。
3. **多人 replay**：原版 STGR 单流只录本地输入；BRO 用旁路多流 `replay/slotN.multi`（JSON），借 eagler `write(path, bytes, replayMatch?)` 的预留字段存取。
4. **回滚边界**：引擎自身帧 ~182 有非确定性分叉（另案），`inputTiming.rollbackLimit` 需如实设置（当前 12 帧）。
5. **触控 / 手柄**：BRO 目前只实测键盘。`touchFire: TOGGLE_TOUCH_FIRE` 已声明但未做官方触控全套验收。
6. **资源挂载**：BRO 用 `scripts.json` + 分片分发（约 199MB，9 组件），与官方 `.data` 单文件不同，须维护者拍板 provider 方案。

## 5. 验证入口（BRO 产物库）

```sh
node web/test/system_multiplayer_check.mjs   # 系统多人 A~M 13 段
node web/test/replay_roundtrip.mjs           # STGR 录像/回放
node web/test/multi_replay_check.mjs         # 旁路多流
node web/test/storage_roundtrip.mjs          # 存档
node web/test/stage_determinism.mjs          # 确定性
node web/test/browser/serve.mjs 8767         # 本地测试页 play.html?coop=1
```

各阶段铁证见 BRO 产物库 `web/PROBE_*.md`，接手总览见 `web/HANDOVER.md` 与 `web/handover/`。
