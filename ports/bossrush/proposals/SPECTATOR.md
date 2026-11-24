# BRO 联机观战（spectator）适配评估与分阶段方案

> 结论先行：**当前不适配，但可做**。缺的不是「渲染」，而是三处接线：
> ①产品目录没开 `spectator`；②relay 的观战子系统不认识 BRO 的帧；③runtime 没有
> 「收历史帧 → 本地重演 → 报播放状态」的播放引擎。
> BRO 侧唯一的重资产（确定性重演）**已经有了**（`BRO_net_replay_*` + `BRO_net_frame_digest`），
> 所以这条路可行，只是工作量在 relay + runtime 两端，需要分阶段上线。

---

## 1. 现状取证（每条都可核对）

### 1.1 产品目录把 BRO 的观战关掉了

`common/contracts/product-catalog.mjs` 的 `bromp` 条目（约 :321-337）：

```js
multiplayer: Object.freeze({
    titleKey: "game.title.bromp",
    gameplay: "cooperative",
    playerCounts: STANDARD_MULTIPLAYER_PLAYER_COUNTS,
    relaySourceEnvelope: true,
    spectator: false,            // ← 这里
    ...
```

对照其它合作作（th06/th07/th09/*）：`spectator: true`。房间 UI 直接读这个字段
决定要不要出「观战」入口，`false` 时前端根本不会发起 `{type:'spectate'}`。

### 1.2 relay 的观战子系统按「协议标记」白名单收帧

- `services/relay/src/spectator-stages.mjs` 的 `COOPERATIVE_SPECTATOR_MARKERS`
  只有 `th05mp/th06mp/th07mp/th08mp/th10mp/th11mp/th12mp/th13mp/th14mp/th15mp/th17mp`，
  **没有 bromp**；`SpectatorStageRelay` 构造时 `if (!markers.has(marker)) throw`。
- `services/relay/src/spectator-frame.mjs` 的 `isSpectatorFrameForRoom()` 也是先按
  `multiplayerConfigForProduct(product)` + 标记分支判帧，BRO 落到最后一条通用分支后
  要求 `payload[0]===0x45 ('E')` 的 24+12*n 帧格式 —— BRO 现在的包不是这个格式。
- 例外先例：th09 与 th075 在同一个文件里有**整段自己的解析分支**（`th09mp-` 前缀
  走 `'T9SP'` 魔数 + 46/126 字节两种包）。所以「给 BRO 加一段专用解析」是这套代码
  的既有做法，不需要发明新机制。

### 1.3 runtime 侧要实现的是一份「原生播放状态」

`common/browser/spectator-playback.mjs`（72 行）是给各作复用的桥：

- 它每 250ms 从 **WASM 堆**里读一段以 `\0` 结尾的 JSON（`state()` 给的指针），字段
  版本 `version:1`：`run / revision / min / position / received / edge / target /
  paused / seeking / following / ended / nodes[]`；`nodes` 是 stage/boss 历史节点。
- 它把 `spectator-playback` 命令（`pause|play|seek|live`）编码成 `run\0action\0`
  写回堆里，交给原生 `control()`。
- 也就是说：**播放引擎（记录、定位、恢复、时间轴）必须由游戏自己实现**，桥只搬字节。

`launcher/src/contracts/runtime-protocol.mts` 里：

```ts
"spectator-playback":  { phase:"live", response:"required", requirement:"conditional",
                         when:"receive-only cooperative Runtime reports spectatorPlayback version 1" },
"spectator-room-state":{ ..., games:['th09'] },
```

`ports/bossrush/web/src/eagler_adapter.mjs:38-41` 已经把这俩列进
`RUNTIME_PROTOCOL_OPTIONAL_COMMANDS`，`isValidNetplayTiming` 也已经允许
`route:'spectator'` —— 信封层的空位是留好的。

### 1.4 BRO 已有的地基：确定性重演

`web/src/multiplayer.mjs`（1433 行）里已经有一整套「录输入 + 重放 + 校验」：

- `BRO_net_replay_capture_start` / `BRO_net_replay_begin(stage, seed, vars)` /
  `BRO_net_replay_push(v)` / `BRO_net_replay_end`（:679/:721/:729/:736）
- `BRO_net_apply_keys_at(p, frame, v, predicted)`（:805/:979）
- `BRO_net_frame_digest(frame)`（:192/:698/:825/.）+ `BRO_net_digest_stats` /
  `BRO_net_divergence`

观战播放要的三件事（按帧喂输入、逐帧推进、定位后重新对齐）在这套接口里都有对应物。

---

## 2. 分阶段方案

### 阶段 A（MVP）：只做「实时观战」，不做历史定位

目标：观战者能看当前对局，不能拖时间轴。这条最省事，也覆盖了用户主要诉求。

1. **runtime 发出观战帧**（新增 `web/src/spectator_source.lua` 或扩 `multiplayer.lua`）：
   - 每逻辑帧把「本帧所有槽位的输入字节 + 帧号 + 会话 id」打成 BRO 自己的包
     （建议魔数 `'B' 'S' 'V'` + 版本 + `session` + `frame` + `players` + 每槽 4 字节输入）；
   - 复用现有 `relaySourceEnvelope: true` 的镜像通道发给 relay；
   - 每 N 帧（或每次进卡/换 boss）发一次 **状态快照事件**（`nodes` 用得到：stage/boss 轴）。
2. **relay**：在 `spectator-stages.mjs` + `spectator-frame.mjs` 里加 `bromp` 专用分支
   （照 th09 的写法：魔数校验 + 长度校验 + 输入字节范围校验 + `SpectatorStageRelay`
   的 history 上限），并把 `bromp` 加进 `COOPERATIVE_SPECTATOR_MARKERS`（分配一个未用标记）。
3. **runtime 播放**：观战端 `startBRO` 走「spectator 模式」：
   - 不进网络对局（`BRO_NetState.started` 保持 false，避免成就/统计污染），
     而是把收到的帧序列喂给 `BRO_net_apply_keys_at` + 逐帧推进；
   - 本地输入全部忽略；HUD 显示「观战中」。
4. **产品目录** `spectator: true`（此时才安全，否则房间会放观战者却没人喂帧）。

### 阶段 B：暂停 / 拖拽 / 回到实时

在 A 的基础上：

- 环形缓冲保存最近 `historyFrames` 帧（relay 侧上限 108000 帧、16MB）；
- 定期（如每 600 帧）打一份「快照」（BRO 需要新增 `BRO_net_snapshot/restore`，
  或者退化成「从 run 起点 replay 到目标帧」——慢但不需要新序列化）；
- 实现桥要的 `position/min/edge/target/paused/seeking` 语义与 `pause|play|seek|live` 命令。

### 阶段 C：观战后结算（对应 `spectator-menu` / `complete` 分支）

`spectator-playback.mjs` 里有 `complete(pointer,len)` 分支：播放到 `ended` 且 run
带 `s<session>_` 前缀时，把录像字节交给原生「保存观战录像」逻辑。BRO 已有
`replay/*.multi` 多流录像，可以接这个分支，让观战者可保存刚看完的一局。

---

## 3. 风险与前置条件

- **relay 必须先上线**：BRO 的观战帧得由用户自己 fork 的 relay（`services/relay`）收下
  并转发；公共 relay 不认 `bromp` 的话，观战端永远收不到历史。上线顺序：
  先 relay 兼容（不改变现有对局行为）→ 再开产品目录 `spectator: true`。
- **确定性**：观战端是纯重演，任何「读本机玩家」的逻辑都会让观战画面与玩家不同
  （这一批联机适配已经把符卡统一钉到 P1，正好是这个前提的一部分）。
- **带宽**：每帧每槽 4 字节 + 头，2~3 人 60fps ≈ 1.5~2.2KB/s，可忽略；大头是快照。
- **回放/结算污染**：观战端必须 `ext.replay.IsReplay()` 语义等价（不写成就、不写
  收卡数、不写资金），否则观战会污染存档 —— 这一步可以复用本轮刚加的
  `achievement_in_mp()`（`web/src/web_patches.lua`）与 `BRO_NetState.started` 判定。

---

## 4. 建议的第一刀

阶段 A 的 1+2（runtime 发帧 + relay 认帧）是**互不依赖、可分别自测**的两块：
relay 侧可以先用构造包跑单测；runtime 侧可以用现有
`tools/ports/mp-browser.mjs` 那种「本地 relay + 两个客户端」的架子验证收到帧。
这两块落地后，观战端的播放（阶段 A 的 3）才值得动手。
