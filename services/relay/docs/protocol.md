# Relay 客户端协议

Relay 使用 WebSocket 提供房间目录、等待室、WebRTC 信令和旁观服务。玩家之间的游戏输入走 WebRTC，直连与 TURN 均通过同一套信令建立。产品 ID、房间规则、时序策略、快捷聊天 ID 和昵称规则由 [`common/contracts/`](../../../common/contracts/README.md) 定义。

WebRTC 建立失败时，服务向两端发送 `{"type":"route","mode":"failed"}` 并结束本局，等待室恢复到可重新准备的状态。WebSocket 游戏数据回退需要服务端 `EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK=1` 与客户端 `netplayAllowWebsocketFallback=true` 同时启用；默认对局不使用该路径。旁观上传继续使用独立的 WebSocket 连接。

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

合作旁观的完整录像由 P1 原生 Replay 编码器生成，通过 `0xe9` 二进制信封传输。20 字节头依次包含标记、版本 1、扩展录像标志、保留字节、64 位局次、32 位总长度和 32 位偏移，数值使用小端序。每片最多 32768 字节，文件最多 8 MiB。Relay 按当前 P1 连接和局次接收连续分片，完成后分发给观众，重连观众可取得同一份文件。文件缓存随换局更新，发送窗口为 15 秒。协议定义见 [spectator-replay.mjs](../../../common/contracts/spectator-replay.mjs)。

## 服务端处理

th09 按当前局保存完整 Replay 前缀，原生开局头到达时释放上一局历史。包含两人本地蓄力样本的输入记录为 126 字节，20 分钟对局约占 9.1 MB；服务端为每场前缀提供 16 MiB 的内存预算。旁观发送分批推进。历史预算耗尽时保留已接入直播的旁观连接，暂停新加入，下一局自动重新开放。

- `common/contracts/` 是静态产品和协议规则的唯一来源；Relay 不依赖 Launcher UI。
- `Origin` 与客户端自报的昵称或 ID 不作为身份凭据。服务端按成员、角色、来源地址和消息类型检查连接。
- TURN 凭据通过服务端短期签名生成，Relay 与 coturn 使用同一共享密钥。

本服务源码见 [`../src/netplay-relay.mjs`](../src/netplay-relay.mjs)，部署见 [`../deploy/README.md`](../deploy/README.md)。

## 已发布客户端的观战协议

Relay 同时接收 `E?SV/1` 与 `E?SV/2`，每场对局在首份关卡记录中固定版本，后续关卡与旁观初始化沿用该版本。v1 保留 60 KiB 的状态上限，v2 使用 4 KiB 的原生 Replay 关卡头；同一对局的版本切换会被拒绝。各作品可以独立更新 Runtime，并保持现有旁观连接可用。

## 一起听

设置 `EAGLER_NETPLAY_LISTEN_TOGETHER=1` 后，首次等待室 `state` 带有 `listenTogether: 2`。功能默认关闭。一起听复用等待室连接，按玩家席校验权限。

客户端发送 `type: "listen-together"`：

| `action` | 字段 | 行为 |
| --- | --- | --- |
| `invite` | `track: {platform, id}`、可选 `queue`、`positionMs`、`paused`、`repeat`、`shuffle` | 有另一位在线入座玩家时发起邀请，记录发起者当时的队列和播放状态 |
| `join` | `session` | 同意邀请或加入正在播放的会话 |
| `select` | `session`、`revision`、`track`、可选 `queue` | 已加入的玩家切换曲目，提供队列时替换当前列表 |
| `pause` / `resume` | `session`、`revision` | 已加入的玩家同步暂停或继续 |
| `seek` | `session`、`revision`、`positionMs` | 跳转进度，沿用当前暂停状态 |
| `previous` / `next` | `session`、`revision` | 切换曲目，播放超过三秒时上一首先回到开头 |
| `mode` | `session`、`revision`、`repeat`、`shuffle` | 循环模式为 `off`、`all` 或 `one`，随机顺序由 Relay 生成 |
| `enqueue` / `remove` | `session`、`revision`、`track` 或 `index` | 已加入的玩家增删队列，当前曲目不能移除 |
| `ended` | `session`、`playback`、`durationMs` | 已加入的玩家报告播放结束，按模式自动续播 |
| `leave` | `session` | 退出播放，少于两人时结束会话 |
| `sync` | | 读取当前状态并校对时间 |

服务端发送同类型的快照，包含 `session`、`revision`、`track`、`participants`（等待室客户端 ID）、`startedAt` 和 `serverTime`。`session` 为随机会话标识，`revision` 随状态变化递增。换歌携带当前版本，版本冲突返回最新快照，客户端重新确认操作。时间以 Unix 毫秒表示。发起时 `startedAt` 为 `null`，另一位玩家同意时，从邀请记录的进度和暂停状态建立共享时间线。参与者的播放与队列权限相同。客户端仅在自己属于 `participants` 且有播放起点时切入共享播放。`session: null` 表示空闲或已退出。席位清除时结束该玩家的参与资格，现有席位重连宽限期内可恢复状态。

曲目用原站 `netease` 或 `qq` 与最多 64 字符的 ID 表示。播放端通过音乐服务按需解析地址。音乐状态独立于对局设置和房间目录。状态按操作发布，每连接每秒最多处理十条音乐消息，限流时返回 `error: "rate-limited"`，退出操作独立处理。入座玩家重连会收到当前会话或空闲快照，客户端也可用 `sync` 请求快照。发送共用 Relay 的有界队列，拥塞超限后断开连接，客户端重连恢复最新状态。

队列最多 100 首并按平台与 ID 去重，每首可附带最多 64 字符的 `name` 与 `artist`。快照增加 `queue`、`queueIndex`、`repeat`、`shuffle`、`hasPrevious`、`hasNext`、`ended` 和 `playback`。切歌、跳转与重播递增 `playback`，同一播放轮次只接受一次结束报告，避免多个客户端连续跳歌。顺序播放到末尾暂停，列表循环回到首曲，单曲循环仅作用于自动续播。Relay 无音乐计时器，客户端在结束事件中推动续播。

播放端根据服务器时间估算进度，在媒体就绪、播放启动及重连后校准。解析返回试听、鉴权错误或无可用地址时显示失败并允许退出。异步解析完成后须核对当前会话、版本和参与资格，避免退出或换歌后恢复旧音频。

暂停快照携带 `pausedAt`，进度固定为 `(pausedAt - startedAt) / 1000`；继续时服务端将暂停时长补入 `startedAt` 并清空 `pausedAt`。暂停与继续共用曲目资源，暂停时停止进度定时器。

手动启用“测试功能”后，可在“房间 / 难度设置”右侧展开或收起观战席上方的小播放器。播放器默认使用个人队列，点击同步按钮才发出或接受邀请。退出同步后可继续之前的个人队列。播放器提供播放、暂停、进度拖动、切歌、随机与循环控制，音量和退出收在菜单中。展开共用房间面板搜索、读取公开歌单或管理队列，歌单可将已加载的最多 100 首加入播放。封面采色只在图片加载时运行，缓存最多 32 组颜色，跨域采色失败时使用默认背景。播放控制同步给已加入的玩家，音量由各浏览器独立调整。各浏览器保存自己上次选择的曲目。关闭测试功能时不创建音乐模型、播放器或音乐定时器。

音乐适配器默认使用 Motues 的 Meting 接口。构建变量 `VITE_METING_API_URL` 可指定兼容的完整接口地址，跨域服务需允许站点来源。浏览器从原站 CDN 加载音频并读取时长。Relay 同步曲目信息、队列和播放状态。替换音乐服务时实现 `MusicProvider` 的四个读取方法即可。
