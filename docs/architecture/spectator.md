# 旁观协议

本文说明当前旁观流的身份、报文、历史和播放生命周期。合作作品使用公共 `E?SV/2`；th09 使用独立的比赛/回合格式。

各作采集位置与原生加载入口见[流式 Replay 旁观](spectator-replay.md)。

## 所有权与身份

| 层 | 负责内容 |
| --- | --- |
| Launcher | 显式旁观身份、当前 Runtime epoch、播放控制与状态展示；见 [`runtime-protocol.mts`](../../launcher/src/contracts/runtime-protocol.mts) 和 [`spectator-playback.mts`](../../launcher/src/launcher/spectator-playback.mts) |
| Relay | 当前 run 的旁观授权、P1 上传、有限历史及有序回放；见 [`netplay-relay.mjs`](../../services/relay/src/netplay-relay.mjs) 和 [`spectator-stages.mjs`](../../services/relay/src/spectator-stages.mjs) |
| `common` | E?SV 编解码、确认发布历史、接收历史、播放状态和只读传输；见 [`SpectatorStageProtocol.hpp`](../../common/include/eagler/netplay/SpectatorStageProtocol.hpp)、[`SpectatorStagePublisher.hpp`](../../common/include/eagler/netplay/SpectatorStagePublisher.hpp)、[`SpectatorStageHistory.hpp`](../../common/include/eagler/netplay/SpectatorStageHistory.hpp)、[`SpectatorStagePlayback.hpp`](../../common/include/eagler/netplay/SpectatorStagePlayback.hpp) |
| 作品 Runtime | 原生 Replay 关头与多人资源、原生关卡重建、确认输入和作品节点；各作入口见下表 |

旁观者是只读接收端：不占玩家席，不进入玩家握手、ready 或确认屏障，也不提交玩家输入。当前只有 P1 发布旁观数据。合作 Runtime 的发布端启用 `stageSpectatorHistory=true`，因此即使当前没有观众也保留上传历史；停用旁观输出不会停止玩家联机。

流身份由作品 marker、`playerCount`、非零 `sessionId`、非零 `gameplayAbi` 和递增 `serial` 共同约束。Relay 把这条流绑定到获准的房间 run；E?SV payload 本身不含 Launcher 的 run 字符串。逻辑帧从第一个可重建 checkpoint 起连续编号，checkpoint 的 `frame` 是下一份待消费输入位置。输入须包含全部玩家席位的有效输入。

## 合作作品 `E?SV/2`

所有整数均为 little-endian。以下偏移针对去除 Relay 上传封套后的 payload。P1 上传使用 `0xe8 || payload`；Relay 转发给旁观者时去掉封套。

### 32 字节公共头

| 偏移 | 长度 | 字段与约束 |
| --- | --- | --- |
| 0 | 4 | ASCII `E?SV`；marker 必须与房间产品一致 |
| 4 | 1 | 版本，固定 `2` |
| 5 | 1 | kind：`1` checkpoint、`2` node、`3` end、`4` bootstrap |
| 6 | 1 | 玩家数；当前为 2 或 3 |
| 7 | 1 | 保留，必须为 `0` |
| 8 | 8 | `sessionId:u64`，非零 |
| 16 | 4 | `gameplayAbi:u32`，非零 |
| 20 | 4 | `frame:u32`；不得为 `INVALID_FRAME` |
| 24 | 4 | `stage:u32` 路线键，范围 `0..65535` |
| 28 | 4 | `serial:u32`，非零；每个发布事件严格递增 |

### 事件体

| kind | 总长与偏移 | 语义 |
| --- | --- | --- |
| checkpoint (`1`) | `40 + stateBytes` 字节；偏移 32 为 `schema:u32`，36 为 `stateBytes:u32`，40 起为 state | schema 非零，state 长度 `1..4096` 字节。checkpoint 携带原生 Replay 的分关入口和多人资源。其 frame 是第一份本段输入的位置。 |
| node (`2`) | 固定 40 字节；32 为依赖的 `checkpointSerial:u32`，36 为 `nodeKind:u8`，37 为 `bossSlots:u8`，38–39 保留且必须为零 | `nodeKind=1` 表示 stage，mask 必须为零；`nodeKind=2` 表示 boss，mask 必须非零。节点不消费输入帧。Boss 注册发生于 tick F 时以 F+1 为节点 frame。 |
| end (`3`) | 固定 32 字节，无事件体 | frame 是最后一份输入之后的下一个位置。它结束当前旁观流尾段；Relay/room 生命周期仍独立。 |
| bootstrap (`4`) | 固定 32 字节，无事件体 | 仅 Relay 发给历史回放完成的观众。字段身份、stage、frame 和 serial 必须与刚交付历史末端相同；不能由 P1 上传，也不进入历史。 |

`stateBytes` 上限为 4 KiB，checkpoint 最大为 4,136 字节。作品以原生 Replay 字段或其原生文件头组成有版本的 schema，并在恢复时校验完整消耗；公共 `SpectatorScalars` 读写器也按 little-endian 编码且拒绝非有限浮点数，见 [`SpectatorStageScalars.hpp`](../../common/include/eagler/netplay/SpectatorStageScalars.hpp)。

输入沿用该产品的 `E?NP/4` type `3` frame packet，而非 E?SV 事件。Relay 和本地历史均要求输入的 `sessionId`、ABI、玩家数匹配当前 checkpoint，frame 连续递增。

### 发布、Relay 与容量

作品在每次执行帧时记录全席输入、阶段号、可选完整 checkpoint 和本帧 Boss slots；回滚重演也更新尚未确认的记录。发布器只在 reconcile 后按确认前沿与已模拟前沿的较小值顺序发送 checkpoint、输入和节点。新阶段 checkpoint 必须先于该阶段第一份输入；Boss 节点依赖当前 checkpoint，并保留该 checkpoint 到节点之间的所有输入。发布环容量为 256 帧。

Relay 先完整校验新 checkpoint，再把 late-join 历史起点切换到该 checkpoint；新的观众收到当前有效 checkpoint、其后连续输入/节点及 Relay 生成的 bootstrap。Relay 当前历史和单个本地观众档案上限均为 108,000 帧或 16 MiB wire bytes。每个 checkpoint state 最多 4 KiB。超限、缺帧或身份不符会停止该旁观流；玩家流继续。

### 合作观众生命周期

1. 观众下载历史时，Relay 同时收集其间到达的新输入；先按序交付历史，再交付后续输入，最后发送 bootstrap。bootstrap 确认历史末端，不是新 checkpoint，也不代表已经追到直播。
2. 首次进入从已接收历史的首个原生 Replay 关头建立场景，随后正常播放。Boss 只作为导航标记，加入时不自动跳到 Boss。原生关头安装完成后开放播放与跳转控件；加载画面在关头初始化完成前保持。接收新关头不会替换暂停中的画面。
3. 初次重建完成后播放已收历史，活动流保留 90 帧缓冲。没有输入时等待，不因暂停、seek、历史回看或 ended 状态判定断线。
4. `pause` 在原生已模拟游标停止并取消当前 seek/live 目标；接收继续。`seek` 只接受本地保留起点至当前 edge 的 frame，恢复必要 checkpoint 后重演到固定目标。暂停期间 seek 完成后仍暂停；重建中再次 pause 会停在实际游标。
5. `live` 在命令时固定当前 edge，完成一次有界追赶后恢复正常速度。追赶期间重复点击保留原目标；后续输入和网络积压按正常节奏播放。`seek` 与 `live` 的快进只由用户操作触发。
6. 接收 `end` 后播放已排入的尾部；它不代表 Relay、房间或 run 已关闭。退出时清理该旁观身份自己的接收流和档案。

公共 Runtime 报告为 `spectatorPlayback` version 1：`run`、递增 `revision`、`min`、`position`、`received`、`edge`、`target`、`paused`、`seeking`、`following`、`ended`、`nodes`。节点含 `id`、`kind`、`frame`、`stage`、`bossSlots` 和可选 `label`；最多 512 个。Host 命令 `spectator-playback {run, action, frame?}` 的 action 为 `pause | play | seek | live`；th09 继续使用作品自己的控件。

### 各作 marker 与代码 owner

| 作品 | marker | checkpoint、节点与 Runtime 入口 |
| --- | --- | --- |
| th06-mp | `'6'` | [`Th06LanStageProbe.cpp`](../../ports/th06/src/netplay/Th06LanStageProbe.cpp)：发布 journal、原生 Replay 关头初始化及 Boss slot 记录。 |
| th07-mp | `'7'` | [`Th07LanStageProbe.cpp`](../../ports/th07/src/netplay/Th07LanStageProbe.cpp)：发布 journal、原生 Replay 关头初始化及 Boss slot 记录。 |
| th08-mp | `'8'` | [`SpectatorReplayStart.hpp`](../../ports/th08-mp/web/cpp/multiplayer/SpectatorReplayStart.hpp) 定义初始化器；[`RollbackDriver.cpp`](../../ports/th08-mp/web/cpp/multiplayer/RollbackDriver.cpp) 记录阶段边界，[`NetworkConnection.cpp`](../../ports/th08-mp/web/cpp/multiplayer/NetworkConnection.cpp) 发布确认历史。 |
| th10-mp | `'A'` | [`SpectatorStageState.cpp`](../../ports/th10-mp/web/cpp/multiplayer/SpectatorStageState.cpp) 编码多人 Replay 的关头；[`WorldPlayerMultiplayer.cpp`](../../ports/th10-mp/web/cpp/multiplayer/WorldPlayerMultiplayer.cpp) 接入阶段边界。 |
| th11-mp | `'B'` | [`SpectatorStageState.hpp`](../../ports/th11-mp/web/cpp/multiplayer/SpectatorStageState.hpp) 编码原生 ReplayStageState 与多人资源；[`NetplayRuntime.cpp`](../../ports/th11-mp/web/cpp/multiplayer/NetplayRuntime.cpp) 负责发布与播放。 |
| th15-mp | `'F'` | [`SpectatorReplayStart.cpp`](../../ports/th15-mp/web/cpp/multiplayer/SpectatorReplayStart.cpp) 捕获/恢复阶段点；[`NetplayRuntime.cpp`](../../ports/th15-mp/web/cpp/multiplayer/NetplayRuntime.cpp) 发布确认历史。 |
| TH20 | — | 当前产品目录未声明 Multiplayer，因此没有 E?SV 旁观流；见 [`product-catalog.mts`](../../launcher/src/contracts/product-catalog.mts)。 |

## th09 独立格式与时间坐标

th09 的对战局有完整比赛初始化器和回合标记，不使用 E?SV、stage key 或合作作品的公共面板。所有多字节数值和 float32 为 little-endian；`build` 是 12 字节 Runtime build 标识。

### th09 报文

| 类型 | 长度 | 布局 |
| --- | ---: | --- |
| 输入 `T9SP/1` | 46 | `0..7`: `T9SP,1,3,2,0`；`8..11`: frame；`12..23`: build；`24..34` 与 `35..45`: 两份输入 |
| begin v1 `T9BG` | 74 | `0..7`: `T9BG,1,0,2,0`；`8..11`: 起始帧 S；`12..23`: build；`24..73`: RNG 8 字节 + 菜单 `InputFrame` 42 字节。保留旧格式。 |
| begin v2 `T9BG` | 102 | 头部如上但版本为 `2`；`24..73`: 同一 RNG 与菜单输入；`74..101`: 完整 28 字节比赛参数。v2 可独立建立新局和 Retry。 |
| round `T9RD/1` | 28 | `0..7`: `T9RD,1,0,2,0`；`8..11`: 下个输入 frame；`12..23`: build；`24..27`: round:u32，严格递增且不大于 1024。 |
| match end `T9SE/1` | 24 | `0..7`: `T9SE,1,0,2,0`；`8..11`: 排他结束帧 E；`12..23`: build；没有 winner 字段。 |
| timing `T9TM/1` | 40 | `0..7`: magic、版本、mode、D、reserve；`8..11`: fullDelay；`12..23`: build；`24/28/32/36`: `rttP95Us/lost/automatic/samples`，各 u32。 |

begin v2 的 28 字节比赛参数依次是 `fresh:u8`、`difficulty:u8`、`characters[2]:u8`、`health[2]:u8`、`alternate[2]:u8`、`focus[2]:u8`、`selector:u8`、`extra_lives:u8`、`unlocked[16]:u8`。前 12 字节占 74–85，解锁位占 86–101。v2 校验这些范围后构造新 world；恢复旧局或倒退时使用 v2 初始化器。

每份 11 字节玩家输入为 `buttons:u16, mode:u8, x:f32, y:f32`。mode 0/1/2/3 分别为无移动、速度/摇杆、绝对目标、无限速绝对目标。坐标必须有限；mode 0/1 的 `|x|,|y| <= 16`，mode 2/3 `<= 4096`。绝对目标按观众实际播放位置求移动量。

### th09 坐标和观众生命周期

th09 wire frame 从进入 Runtime 后开始计数，游戏内选角也会消耗序号，所以首局 S 可以大于零。每局输入区间为 `[S,E)`，其中 E 是下一份待输入帧。round marker 使用同一 wire frame。UI 时间线位置是已收到各局 gameplay 输入帧数的累计值；局间菜单不计入时间，逻辑速率为每秒 60 帧。

Relay 为新观众保留当前局的 timing、begin 和后续输入；新观众从当前局 S 开始，不获得此前比赛。已连接观众的 Runtime 内存档案保留它实际收到的比赛，可跨局回看；离开 Runtime 后释放，不是 Replay 文件。Relay 当前局上限为 32,401 个 payload/2 MiB；native 待播输入为 32,400 帧；JS 未交付队列为 32,404 个 payload/2 MiB。已消费档案会留在本地供回看。

播放边缘保留 90 帧缓冲；收到 `T9SE(E)` 后开放该局尾部。native 控制 action 为 `0=pause`、`1=向前定位`、`2=live`、`3=play`、`4=固定目标重建`。暂停时继续接收并排队新局；Play 顺序消费旧尾部和新局；Seek 通过对应 v2 begin 重建完整比赛，再按固定 frame 重喂输入；Live 可以丢弃尚未观看的旧局并跳到最近 begin，之后按正常速率跟播。活动局输入到达边缘时等待，不自动改变目标。

`T9SE` 结束一局，不结束房间。房间返回等待态或切到新 run 时，观众先排空已接收队列和明确结束的局尾部，并遵守本地暂停，再退休旧 Runtime。新 run 需要新的授权和连接；重新进入不会续用旧游标。传输 EOF 会先处理已入队 payload；没有合法 `T9SE` 时按中断处理，不补造成功结束。

th09 的 codec、回合/比赛排队和时间线分别见 [`shared-netplay.mjs`](../../ports/th09/web/sdl-runtime/shared-netplay.mjs)、[`spectator-tape.mjs`](../../ports/th09/web/sdl-runtime/spectator-tape.mjs)、[`SpectatorPlayback.hpp`](../../ports/th09/web/cpp/multiplayer/SpectatorPlayback.hpp) 与 [`SpectatorSchedule.hpp`](../../ports/th09/web/cpp/multiplayer/SpectatorSchedule.hpp)。
