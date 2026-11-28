# BRO 未压缩音乐包（可选覆盖包）

> 2026-11-22 · 对应 BRO 仓库 `TSake/TouHou-BossRushOnline` 的 `web/tools/package_music_pack.py`
> 目标：玩家在网页端**导入一个音乐包**，用原始 ogg 覆盖官方分发的 96 kbps 压缩 BGM。

## 1. 为什么是"覆盖包"而不是整包

官方分发的 `bro-import-package.zip`（`eagler-touhou/package/1`）走
`installParsedPackageZip()`：它用 ZIP 内的文件**重建 generation**
（`desiredFileIds = [...parsed.files.keys()]`，且 `reuseCurrent: false`）。
包里只有 47 首音乐时导入，会把图像 / 脚本 / SE 全部丢掉。

BRO 运行时另有一条**路径指针覆写**通道：`web/src/resource_cache.mjs` 的
`installResources()`。宿主（launcher / 站点）发 `resources` 命令，运行时：

1. `fetch` 每个 `url` → 校验 `sha256` → 按 hash 写入 Cache API；
2. 写 `overrides[pathKey(path)] = { p, h, n }`；
3. `resolveFile()` 里 **overrides 优先于 `package-manifest.json`** ⇒ 同路径此后
   一律取覆盖后的字节（`fetchAsset` / `url` / `httpUrl` 三条路都生效）。

因此音乐包只需要「47 首原始 ogg + 指针清单」，不碰其它组件。

## 2. 包结构

```
game/music/<原名>.ogg   47 首原始 44.1kHz 立体声 ogg（STORE，未转码）
pointers.json           指针表  schema=bro/music-pack-pointers/1
resources.json          resources 命令数组（含相对 url）
music-pack.json         元信息：component=music / version / bytes
README.md               用法与红线
```

`pointers.json` 逐项：

```json
{ "p": "music/TH06_1.ogg", "target": "/game/music/TH06_1.ogg",
  "h": "<原文字节 sha256>", "n": 1381630 }
```

`version` 是全体 `(p, h)` 排序拼接后的 `sha256-…`，与官方清单的 `component.music.revision`
同一算法，便于对账（覆盖前后 revision 不同，说明确实没走压缩版）。

`resources.json` 直接可用：

```js
// 解压后把每个文件变成 blob URL（或站点直接给出 URL）
await resourceCache.installResources(
  resources.map((r) => ({ path: r.path, url: r.url, sha256: r.sha256, bytes: r.bytes }))
);
```

## 3. 指针语义（宿主可改）

- key 归一化：`normalizeAssetPath()` 会去掉开头的 `game/`、`/`、`./`，
  `pathKey()` 再小写 —— `music/TH06_1.ogg`、`/game/music/TH06_1.ogg`、`MUSIC/th06_1.OGG`
  命中同一条覆盖。宿主若要改名，请保持 `p` 与官方清单的相对路径一致，
  否则覆盖不生效（会静默回落到压缩版）。
- `h` 必须是**原文字节**的 sha256；写错会在 `verify()` 抛错，`installResources`
  记入 `errors` 且**不登记**该指针（不会写坏表）。
- 覆盖表存在 `localStorage`（`BRO.resource.paths.v1`），blob 在 Cache API
  （默认 `BossRush-assets-v1`）。清站点数据 = 覆盖失效，需要重新导入。
- `gc()` 会保留 override 引用的 blob，不会把音乐包缓存清掉。

## 4. 体积

| 项 | 压缩版（官方分包） | 原始版（音乐包） |
|---|---|---|
| BGM 合计 | 81.1 MB（96 kbps mono） | **207.2 MB**（44.1 kHz 立体声 194–274 kbps） |
| 增量 | — | +126.1 MB |
| 单曲最大 | 3.9 MB (`TH16_AEX_0`) | 11.9 MB (`TH16_AEX_0`) |

解码内存也要留意：BGM 解码成 Float32 PCM，立体声 47 首合计约 2.78 GB（单声道 1.39 GB）；
单首最大 `TH16_AEX_0`（6:30）= **131 MB**，已超过运行时 `BUF_BUDGET = 128 MB` 的
LRU 预算 ⇒ 触屏端导入原版音乐时建议同时提高 `bufferBudgetMB`，或只导入少数几首。

## 5. 红线

- **不要**给音乐包补 `package.json` 当整包导入（见 §1）。
- 不要改 `game/music/` 的原文件名大小写：清单 `p` 与 C 侧 `LoadMusic` 路径都是原文。
- 压缩版音频（`web/tools/replace_audio_inplace.py`）的 scope 默认只处理 `music/`；
  **SE 自 2026-11-22 起一律保持原生 PCM**，不要再压。
- 音乐包只覆盖当前浏览器；官方分包仍是压缩版，未导入的玩家听压缩版属预期行为。

## 6. 生成与校验

```bash
# BRO 仓库根目录（读 git HEAD 的 LFS 指针 → .git/lfs/objects 的原始字节）
python web/tools/package_music_pack.py             # 产出 bro-music-pack.zip
python web/tools/package_music_pack.py --dry-run   # 只统计
```

脚本自带校验：每个路径必须存在于 `web/dist/package-manifest.json` 的 `music` 组件；
原始字节数必须大于压缩版，否则告警（防止误把压缩版当原件打包）。

运行时语义有单测：`web/test/resource_cache_override_unit.mjs`（4 项：覆盖生效 /
清单不变 / 路径归一化 / 失败不写坏指针）。
