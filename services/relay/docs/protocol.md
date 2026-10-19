# Relay 客户端协议

Relay 使用 WebSocket 提供房间目录、等待室、信令、游戏传输和旁观服务。产品 ID、房间规则、时序策略、快捷聊天 ID 和昵称规则由 [`common/contracts/`](../../../common/contracts/README.md) 定义。

## 连接入口

| 用途 | 查询参数 |
| --- | --- |
| 房间目录 | `directory=1`、`member` |
| 等待室 | `room`、`lobby`、`member`；创建时可附 `intent`、`visibility`、`players`、`difficulty`、`disableCheatMovement`、`chapterThreshold`、`challengeMode`、`prankMode` |
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
| `settings` | `playerCount`、`difficulty`、`visibility`、`disableCheatMovement`、`challengeMode`；TH15 可带 `chapterThreshold` |
| `start` | `inputDelay`、`adonisMode`、`inputDelayAuto`、`predictionReserve`、可选 `predictionLimit` |
| `quick-chat` | 当前 `serial` 与 `phrase` |
| `state` 的 `room` | `playerCount`、`difficulty`、`visibility`、`disableCheatMovement`、`chapterThreshold`（TH15）、`challengeMode`、`prankMode`、`inputDelay`、`adonisMode`、`inputDelayAuto`、`predictionReserve`、`timing`、`predictionLimit`、`settingsVersion`、`phase`、`runtimeGeneration`（版本化 run）、`startSerial`、`spectators`、`spectatorCount`、`seats` |

版本化 `runtimeGeneration` 为 64 个小写十六进制字符；同一局准备的玩家必须匹配。消息有效性、成员权限、房间状态和数值范围以 Relay 实现与 [`tests/`](../tests/) 为准。

## 服务端处理

- `common/contracts/` 是静态产品和协议规则的唯一来源；Relay 不依赖 Launcher UI。
- `Origin` 与客户端自报的昵称或 ID 不作为身份凭据。服务端按成员、角色、来源地址和消息类型检查连接。
- TURN 凭据通过服务端短期签名生成，Relay 与 coturn 使用同一共享密钥。

本服务源码见 [`../src/netplay-relay.mjs`](../src/netplay-relay.mjs)，部署见 [`../deploy/README.md`](../deploy/README.md)。
