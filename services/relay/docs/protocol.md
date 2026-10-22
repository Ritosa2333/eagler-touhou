# Relay 客户端协议

Relay 使用 WebSocket 提供房间目录、等待室、信令、游戏传输和旁观服务。产品 ID、房间规则、时序策略、快捷聊天 ID 和昵称规则由 [`common/contracts/`](../../../common/contracts/README.md) 定义。

## 连接入口

| 用途 | 查询参数 |
| --- | --- |
| 房间目录 | `directory=1`、`member` |
| 等待室 | `room`、`lobby`、`member`；创建时可附 `intent`、`visibility`、`players`、`difficulty`、`disableCheatMovement`、`disableLocalChargePriority`、`chapterThreshold`、`challengeMode`、`prankMode` |
| 玩家连接 | `room`、`run`、`player`、`players`、`member`；`signal=1` 选择信令连接，版本化 run 另带 `runtimeGeneration` |
| 旁观连接 | `room`、`run`、`spectator`、`players`、`member`；版本化 run 另带 `runtimeGeneration` |
| 网络诊断 | `diagnostic=1` |

已加入房间的玩家和旁观连接以 `member` 校验成员身份。联机程序已版本化的 run 还要求 `runtimeGeneration` 与 run 一致。`run` 首局默认为 `0`，`player` 从 `0` 开始。等待室的 `lobby` 与 `member` 是客户端会话 ID。

## 等待室消息

客户端和服务端以 JSON 对象的 `type` 字段区分消息。常用客户端消息为 `take-seat`、`stand-up`、`spectate`、`leave-spectator`、`set-name`、`set-loadout`、`set-ready`、`settings`、`start`、`remove-player`、`move-player-to-spectator`、`remove-spectator`、`quick-chat`、`room-probe`、`timing-result` 与 `resource-progress`。服务端发送 `state`、`start`、`error` 等消息。

关键字段：

| 消息 | 字段 |
| --- | --- |
| `set-ready` | `ready`、`runtimeGeneration`、`movementMode`、`touchEnabled`、`mobileDevice` |
| `settings` | `playerCount`、`difficulty`、`visibility`、`disableCheatMovement`、`challengeMode`，th09 可带 `disableLocalChargePriority`，th15 可带 `chapterThreshold` |
| `start` | `inputDelay`、`adonisMode`、`inputDelayAuto`、`predictionReserve`、可选 `predictionLimit` |
| `quick-chat` | 当前 `serial` 与 `phrase` |
| `state` 的 `room` | `playerCount`、`difficulty`、`visibility`、`disableCheatMovement`、`disableLocalChargePriority`（th09）、`chapterThreshold`（th15）、`challengeMode`、`prankMode`、`inputDelay`、`adonisMode`、`inputDelayAuto`、`predictionReserve`、`timing`、`predictionLimit`、`settingsVersion`、`phase`、`runtimeGeneration`（版本化 run）、`startSerial`、`spectators`、`spectatorCount`、`seats` |

`disableLocalChargePriority` 是 th09mp 的布尔规则，默认 `false`，对应启用本地优先蓄力。创建房间时，查询参数使用 `disableLocalChargePriority=0` 或 `1`。修改权限属于等待室中的 P1，`settings` 消息使用布尔值，并携带有效的 `playerCount` 与 `difficulty`。修改会清除双方准备状态、更新 `settingsVersion`，并通过 `state.room` 同步当前值。开局后规则固定。参数映射、蓄力行为和录像格式见 [th09 开发指南](../../../ports/th09/docs/DEVELOPMENT.md#回滚蓄力与-replay)。

版本化 `runtimeGeneration` 为 64 个小写十六进制字符；同一局准备的玩家必须匹配。消息有效性、成员权限、房间状态和数值范围以 Relay 实现与 [`tests/`](../tests/) 为准。

## 萃梦想旁观

th075 使用独立的 `EPTM` 时序记录与 `EPNP` 全席位已确认输入，由原生对局入口重放。Relay 按本作身份校验两人输入和实测时序，并与合作作品的分关协议分别处理。

## 合作作品旁观

`E?SV/2` 携带原生 Replay 关头、关卡与 Boss 导航节点、结束事件和首次追赶目标。关头携带作品版本号，大小上限为 4096 字节。Relay 保存最新关头与其后连续的全席位已确认输入，按原始顺序为新旁观者分发，并在关卡切换后整体替换历史前缀。

客户端与 Relay 共同校验产品、人数、会话、ABI、事件序号和输入帧连续性。慢旁观者使用独立的发送队列与容量边界。数据格式、原生 Replay 入口和播放控制见[流式 Replay 旁观](../../../docs/architecture/spectator-replay.md)。

## 服务端处理

th09 按当前局保存完整 Replay 前缀，原生开局头到达时释放上一局历史。包含两人本地蓄力样本的输入记录为 126 字节，20 分钟对局约占 9.1 MB；服务端为每场前缀提供 16 MiB 的内存预算。旁观发送分批推进。历史预算耗尽时保留已接入直播的旁观连接，暂停新加入，下一局自动重新开放。

- `common/contracts/` 是静态产品和协议规则的唯一来源；Relay 不依赖 Launcher UI。
- `Origin` 与客户端自报的昵称或 ID 不作为身份凭据。服务端按成员、角色、来源地址和消息类型检查连接。
- TURN 凭据通过服务端短期签名生成，Relay 与 coturn 使用同一共享密钥。

本服务源码见 [`../src/netplay-relay.mjs`](../src/netplay-relay.mjs)，部署见 [`../deploy/README.md`](../deploy/README.md)。

## 已发布客户端的观战协议

Relay 同时接收 `E?SV/1` 与 `E?SV/2`，每场对局在首份关卡记录中固定版本，后续关卡与旁观初始化沿用该版本。v1 保留 60 KiB 的状态上限，v2 使用 4 KiB 的原生 Replay 关卡头；同一对局的版本切换会被拒绝。各作品可以独立更新 Runtime，并保持现有旁观连接可用。
