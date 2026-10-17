# 旁观协议与全作品下放契约

状态：公共行为约束已整理，时间线能力按作品迁移和验收。核对日期：2026-10-16。

本文依据 Launcher `main` 工作树 `8b20f38`、TH09 `eagler` 的
`80aa50e` 及本地源码整理。两棵树已有其他未提交改动，因此提交号不是
工作区全部内容的发布证明。后续已开始公共层实现，增加下述 `E?SV/1` 候选报文、
Relay 与前端接缝。六作原生初始化器和控制桥正在接入，逐作原生验收尚未完成，
不能据此宣称全部作品或公网已经开放中途旁观。

## 1. 适用范围与权威

所有声明 Multiplayer 的作品遵守身份、确认输入、只读、顺序、故障隔离和
生命周期约束。中途加入、暂停、进度定位、跨局回看是本轮准备下放的完整目标，
不能从 `multiplayer.spectator: true` 推导为已经实现。

| 现行 Runtime 接入（尚未迁移） | 帧格式 | 准入与时间线现状 |
| --- | --- | --- |
| TH06MP / TH07MP | `E6NP/4` / `E7NP/4` | 开局前登记，开局后仅有已登记观众的连接宽限期 |
| TH08MP / TH10MP / TH11MP / TH15MP | `E8NP/4` / `EANP/4` / `EBNP/4` / `EFNP/4` | main 的公共准入仍为开局前登记。时序能力服从各产品的 `inputTiming`，TH15 只允许纯延迟 |
| TH09MP | `T9SP/1`，附带 `T9BG` / `T9RD` / `T9SE` | 中继启用 `EAGLER_NETPLAY_TH09_SPECTATOR_TIMELINE=1` 时允许中途加入。完整初始化器支持本地跨局回看 |
| TH20 | 当前目录未声明 Multiplayer | 当前不激活此 profile。以后声明时遵守同一契约 |

该表记录目录、校验器和准入源码，不代表所有作品的当前二进制、公网部署或
真实设备都通过。产品事实来自 `src/contracts/product-catalog.mts`，公共
适配义务来自 `src/contracts/adapter-capabilities.mts`，Host 消息来自
`src/contracts/runtime-protocol.mts`。不要另建作品编号开关表。

| owner | 职责 |
| --- | --- |
| Launcher | 房间入口、显式角色、私有成员身份、Runtime 代次、返回房间与再次准入 |
| Relay | 房间/run 授权、P1 发布权限、有限历史、历史到直播的有序交接、旁观流停用 |
| `eagler-common` | 公共字节编解码、只读传输、队列/背压、可复用播放策略 |
| 作品 Runtime | 确认前沿、全部席位的有效输入、完整初始化/重建、原作生命周期、状态一致性证明 |

Relay 不模拟游戏。旁观者不加入玩家 HELLO/READY、ACK 或确认屏障，不能让玩家
等待观众追上。P1 是现行唯一旁观发布者，断开后没有隐式的发布者接管。

## 2. 身份、准入与 Host 边界

1. 观众必须主动选择旁观角色。空闲成员收到玩家 `start` 不能因此启动 Runtime。
   Launcher 只在已确认的旁观身份和本次启动许可下启动接收端。
2. lobby 的 `clientId` 是公开路由标识。私有 `member` 绑定房间成员和本次
   `run`；旁观 WebSocket 必须使用绑定身份。URL 中同时带 `spectator` 与
   `player` / `signal` 不构成合法双角色。成员凭据不进入公开快照。
3. 开局前准入在 `startSerial` 对应的 run 中快照。默认连接宽限为 60000 ms，
   可由 `EAGLER_NETPLAY_SPECTATOR_CONNECT_GRACE_MS` 配置。宽限只让已登记
   观众连接，不允许新观众补入旧产品的正在进行局。迟来的申请保留到下一次开局。
4. 当前 TH09 时间线分支通过 `spectate` 绑定当前 run，并回复
   `spectator-start {serial, room}`。目录的 `joinable` 代表玩家席位，
   `spectatable` 代表运行中旁观，二者独立。当前目录后者仅对 TH09 时间线开放。
   旁观流停止后必须撤下运行中旁观入口。
5. 同一旁观身份不能同时领取两条接收流。当前 TH09 时间线在旧流关闭后释放
   claim，重连重新补当前局。它不续传旧连接的播放游标或本地档案。
6. Host `configure` 使用现有 `netplaySpectator`、`netplaySpectatorId`、
   `netplaySpectatorCount` 和 `netplayUrl`。`count` 不是准入凭据。
   `netplaySpectatorTimeline` 是现有 TH09 房间模式提示，不是全作品能力证明。
   TH09 目前还依据 native control 导出判断播放能力，下放时需核对两端能力组合。
7. `eagler-touhou/1` 的来源、同源、game、epoch 检查仍然有效。当前
   `spectator-room-state {runId, waiting}` 只适用 TH09，`runId` 为 u32，
   `waiting` 为布尔值；旧 run 不影响当前播放。该命令通知房间已回等待态或
   已进入更新 run，不能制造结束帧、重置画面或取消观众暂停。

移除观众、主动离房、成员释放、Runtime 退出与房间关闭要分别测试。只有当前
身份拥有的连接和本地档案可以清理，不能关闭玩家传输或另一个 run。

## 3. 确认输入与三种时间

发布只从连续、已经 reconcile 的确认记录读取。预测帧、回滚重演、Draw、
设备重新采样都不能重复发布。每帧携带全部玩家的有效按钮和移动输入；
玩家输入延迟 D 已经体现在这份流中，观众不能再施加一次 D。

必须区分以下坐标：

- **wire frame**：该 run 的原始逻辑输入序号。TH09 原生选角也消耗序号，
  所以游戏开始的第一帧可以不是 0。
- **match**：由初始化器确定的可独立重建单元。若开始为 S、结束为 E，
  输入恰为 `[S, E)`，结束标记指向下一个待输入帧 E。下一局可以从更大的
  S 开始，局间菜单空隙不补中性输入，也不伪造可播放帧。
- **timeline position**：当前观众收到的各局 gameplay 输入数之和。
  局间菜单不计时。TH09 每秒 60 逻辑帧；局标记和回合标记语义不同。

接收前沿、已经交给 native 的前沿、native 已模拟游标、本地档案读取游标
也必须分离。暂停/倒退只改变播放游标，不能使接收前沿倒退或把旧包再次计入
未读网络积压。回看时仍然接收新确认帧。

单局状态机为：初始化 -> 连续输入（可穿插回合边界）-> 明确结束 -> 下一初始化。
固定开局参数的旧作品可由已验证的 configure 建立起点；中途准入必须有独立
完整起点。缺帧、版本/ABI 不符或无效输入必须显式失败，不能用预测、跳帧或
改 hash 掩盖。严格接收验证由 Runtime 完成，Relay 的格式筛选不能代替它。

### 非对战作品的换面与 Boss 节点

TH06/07/08/10/11/15/20 的逐作入口和可行性见
[换面与 Boss 节点调查](SPECTATOR_NODES.md)。非对战作品准备采用面首与 Boss
入场分段，TH09 保持 match/round。必须区分显示标记、独立重建点、入场播放点
与直播边缘。Boss 标记若依赖本面起点，服务器仍保留面首初始化器及到该点的
完整确认输入。换面标记或 Boss 注册本身不证明该处已经能独立恢复。

下放提案允许从最近已确认的段落开始展示，后台从最近有效 checkpoint 重建；
因此观众的入场画面位置可以晚于实际重建起点。该入口策略尚未启用，不能把
现有合作作品从 frame 0 接收的实现描述为已经支持这些中途恢复节点。

## 4. 当前 wire 格式

以下长度和偏移均针对 **去掉上传封套后的 payload**，多字节整数和 float32
为 little-endian。P1 上传为 `0xe8 || payload`，Relay 只向观众发送 payload。
观众接收连接只读，发送任何应用消息都会被以 1008 关闭。`0xe7` 是玩家定向
转发封套，不能与旁观封套互换。

### 4.1 合作作品 `E?NP/4`

| 偏移 | 长度 | 内容 |
| --- | --- | --- |
| 0 | 4 | `E6NP` / `E7NP` / `E8NP` / `EANP` / `EBNP` / `EFNP`，必须匹配房间产品 |
| 4 | 4 | version=4、type=3、playerCount、reserved=0 |
| 8 | 8 | sessionId |
| 16 | 4 | frame，不能为 `INVALID_FRAME` |
| 20 | 4 | gameplayAbi，接收端不参加玩家握手，因此每帧都携带 |
| 24 | N × 12 | 按席位顺序排列的 FrameInput，N 必须来自产品允许的人数 |

FrameInput 为 buttons:u16、analogMode:u8、flags:u8、x:f32、y:f32。
flags 低三位分别为 unlimited、touchUsed、touchBomb。mode 和有效移动语义
由公共 codec 与作品 config 校验。不能把 TH09 的 11 字节样本当作该格式。

实测时序作品还有 40 字节 `E?TM/1` 元数据：mode/D/reserve 在 5/6/7，
fullDelay 在 8，sessionId 在 12（8 字节），gameplayAbi 在 20，rttP95Us、
lost、automatic、samples 分别在 24/28/32/36。纯延迟作品拒绝 hybrid 和非零
prediction。Relay 支持某个 marker 不代表具体 Runtime 的 timing codec
已经支持该作品；新增接入必须核对两端，尤其不能照抄 TH09 的 build 字段。

### 4.1a 合作作品换面扩展 `E?SV/1`（实现候选）

公共 codec、确认发布 journal、接收历史与播放策略分别位于 `eagler-common`
的 `SpectatorStageProtocol.hpp`、`SpectatorStagePublisher.hpp`、
`SpectatorStageHistory.hpp`、`SpectatorStagePlayback.hpp`。Relay 使用
`server/spectator-stages.mjs`。仅接受 `6/7/8/A/B/F`，不接管 TH09。

| 偏移 | 长度 | 内容 |
| --- | --- | --- |
| 0 | 8 | `E?SV`, version=1, kind, playerCount=2/3, reserved=0 |
| 8 | 8 | sessionId，非零 |
| 16 | 4 | gameplayAbi，非零。注意与输入包的 frame/ABI 偏移不同 |
| 20 | 4 | frame，下一份待消费输入的位置，不得为 INVALID_FRAME |
| 24 | 4 | stage/route key，0..65535，保留真实路线身份 |
| 28 | 4 | 事件 serial，非零，发布事件严格递增 |

- kind=1 checkpoint：40.. 后为作品定义的标量初始化器，32 处 schema:u32，
  36 处 stateBytes:u32。schema 非零，状态 1..61440 字节，总包最多 61480 字节。
  这是新面第一份输入之前的完整恢复契约，不能填原生内存镜像或单人资源子集。
  允许作品的显式可移植对象 codec，以及有展开上限的压缩。TH15 实测面首状态
  压缩后约 42 KiB，因此 8 KiB 的初始候选预算不足。上传封套后仍小于中继既有
  64 KiB 消息上限；P1 初始化器另受 10 包/秒、256 KiB/秒预算约束，普通玩家
  输入仍受 16 KiB 单包约束。
- kind=2 node：总长 40。32 处依赖的 checkpoint serial，36 处 kind（1 换面、
  2 Boss），37 处 Boss slot mask，38/39 为零。换面 mask=0，Boss mask 非零。
  tick F 产生的 Boss 注册事件按 F+1 发布。标题负责实体重用、重注册和多 Boss 去重。
- kind=3 end：总长 32，frame 等于最后输入的下一位置。只有显式成功结束才放开尾部缓冲。
- kind=4 bootstrap：总长 32，**仅由 Relay 发给首次历史下载已完成的观众**。
  frame/serial/stage 必须与刚收到的历史末端一致，不写入历史、不增加 serial，
  P1 上传该类型会使旁观流停止。观众此时固定最近的换面/Boss 入场目标，避免
  刚读到面首时误认为后面的 Boss 历史不存在，也避免目标跟着后续输入移动。

事件只能在输入的连续边界出现。只有完整、顺序正确的新 checkpoint 才替换
服务器旧面前缀。Boss 节点保留本面全部输入；首次历史下载中发生换面时按序
保留中间的段，不能直接跳到最新数组而制造缺帧。服务器每面最多 108000 个
输入、16 MiB wire 字节。慢观众另外受 32 段/16 MiB 等待段及 15 秒背压约束。
本地总历史同样限制为 108000 帧/16 MiB wire 字节，超限明确结束该观众，不能
静默截掉重建前缀。这是候选容量预算，尚无全作品最长段的实机容量证明。

原生接入完整 checkpoint 后，P1 使用 `Connect(..., stageSpectatorHistory=true)`
保留零观众时的历史上传 lane。观众对应的接收队列为 131072 包/16 MiB，避免
完整长面在 native 消费前就撞上旧 16384 包限额。默认参数为 false，旧适配器
及 TH09 保持既有传输/准入策略。不能仅为打开大厅入口而设置该参数。

发布 journal 保存每次实际执行的全部席位输入，回滚重演覆盖尚未发布的记录，
只在 reconcile 后的确认前沿释放。不能等发送时再从可能已经退休的 core 输入
环读取。已经确认的旧前缀可能因 journal 恢复而重跑，相同 wire 输入只做幂等
处理，不能重发；最近保留窗口内的真正输入修订必须使可选旁观流显式失败。
真正可恢复的第一个边界不必是 frame 0；发布者显式设置 firstFrame，不能补造
此前的中性输入或把尚未激活的 stage 数字当作 checkpoint。

### 4.2 TH09 payload

| 类型 | 字节数 | 头部 0..7 | 8..11 | 12..23 | 后续 |
| --- | --- | --- | --- | --- | --- |
| 输入 | 46 | `T9SP`, 1, 3, 2, 0 | frame | 12 字节 Runtime build | 两份 11 字节玩家输入 |
| 初始化 v1 | 74 | `T9BG`, 1, 0, 2, 0 | S | 同上 | 50 字节 RNG + menu input，旧兼容 |
| 初始化 v2 | 102 | `T9BG`, 2, 0, 2, 0 | S | 同上 | 上述 50 字节 + 28 字节完整局参数 |
| 回合边界 | 28 | `T9RD`, 1, 0, 2, 0 | nextFrame | 同上 | round:u32，单局内递增且不超过 1024 |
| 逻辑结束 | 24 | `T9SE`, 1, 0, 2, 0 | E | 同上 | 没有 winner 字段 |
| 实测时序 | 40 | `T9TM`, 1, mode, D, reserve | fullDelay | 同上 | rttP95Us/lost/automatic/samples，各 u32 |

输入位于 24 和 35，每份为 buttons:u16、mode:u8、x:f32、y:f32。
mode 0/1/2/3 分别为无移动、速度/摇杆、绝对目标、无限速绝对目标。
x/y 必须有限，mode 0/1 的绝对值不超过 16，mode 2/3 不超过 4096。
发送绝对目标，接收者按实际应用帧状态求移动量，不能发送从发布者旧位置推导的速度。

v2 初始化器的现有 WASM 布局：

| payload 偏移 | 内容 |
| --- | --- |
| 24..31 | RNG：seed:u16、reserved:u16、calls:u32 |
| 32..73 | InputFrame：held/previous/repeat/pressed/released 各 u16，duration[16]:u16 |
| 74..85 | fresh、difficulty、characters[2]、health[2]、alternate[2]、focus[2]、selector、extra_lives，各 u8 |
| 86..101 | unlocked[16]:u8 |

这是当前 TH09 scalar ABI，不能作为跨作品内存快照格式。native 校验 fresh、
难度、角色、生命、配色、focus、selector 等范围。v2 包括 Retry，也必须能在
没有上一局对象的观众端独立造 world。v1 依赖既有上下文，只保留前进兼容，
不能承诺任意换角重建或倒退。

TH09 timing 在输入前发送，接收端校验 build、已选 mode、自动/手动 D、
reserve 与测量统计，局中不得变更。Relay 和 native/JS 的校验范围并不完全
相同，下放时以一致的有效值集合为目标，不能仅凭 Relay 接收成功判合法。

### 4.3 历史交接与停流

TH09 Relay 按次序重放保留历史，每批最多拼接 64 个连续 46 字节输入；
初始化、时序、回合、结束包保持独立消息。观众拆批后逐包走同一验证路径。
重放期间新包仍写入同一历史，追到前沿后才切换直播，不能重复发送或漏帧。
如果重放期间换局，旧 history 引用先排空，再切入新的当前局 history。

`0xe8 53 54 4f 50 01` 是 P1 上传的 STOP 封套，不是 `T9SE`：它表示旁观
输出停止，不能作为成功结束证明，也不转发给玩家输入队列。停流关闭旁观准入，
清理服务器历史并通知观众，玩家游戏和信令继续。当前服务以 1011 和
`spectator stream stopped; players continue` 关闭接收流。

故障可通过 P1 的 `spectator-stop` 信令尽快停流。正常尾帧收尾必须与最终数据
排在同一 Relay 流上，不能让另一条信令通道抢先清理尾帧。公共
`StopSpectators(graceful)` 已区分这两种用途，调用者必须明确选择。

## 5. 历史范围、播放与生命周期

### 历史有两个 owner

- **服务器 bootstrap**：TH09 在每个 `T9BG` 替换为最新 timing + 当前局
  初始化和后续输入/边界。新加入或新连接只补这一局，从 S 开始，不请求以前局。
- **观众本地档案**：保留该观众实际收到的 begin/input/round/end，直到退出。
  所以老观众可跨局倒退，新观众没有以前局。当前实现为会话内存，关闭 Runtime、
  离开或刷新即释放，不等于自动保存 Replay，也不承诺断线续档。

收到新局不等于立即替换画面。暂停观众保留旧局尾部和后续局，恢复后顺序播放。
只有用户显式“回到直播”可以跳过未看完的局；跳过后本地档案仍保留。

### 播放控制

暂停、继续、定位和回到直播只作用于接收端。键盘、触屏、手柄、菜单确认、
暂停/重开快捷键都不能进入玩家权威输入或改变房间 ready/start。

TH09 native control 的当前 action 为 0 暂停、1 向前定位、2 回直播、3 继续、
4 从档案重建后的固定前进目标。它们是作品 native API，不是新增 Host 命令。
倒退必须恢复该局的完整 v2 初始化，再有限批次重喂已确认输入。只修改 frame
计数或恢复 RNG 无法证明 world、菜单边沿、角色、资源和效果一致。

活动局的可播放边缘保留 90 帧缓冲，明确结束后开放全部尾部。回直播只在点击时
取一次目标，后续网络突发不能移动目标或重新开启加速。达到目标后按正常节奏
播放。TH09 正常回调最多消费 min(dueTicks, 4, available)，定位时仍受整次
回调 8 ms 的追加 tick 起始预算约束，不能跳过逻辑帧；单 tick 超过预算不构成
硬实时保证。Draw 有状态副作用时沿用已验证调度，不能额外 Draw 一次补画面。
这些数值是 TH09 策略，其他作品需测量后接入公共策略，不能改变玩家 pacing。

### 结束不是断线

`T9SE(E)` 只完成当前局，不能自动关闭仍等待下一局的 room/run。房间已回到
等待态或开启更新 run 时，旧观众要先消费明确结束、全部已收队列和确认尾部，
并尊重本地暂停，才退休旧 Runtime。新 run 重新申请许可，不能复用旧 run 身份。

EOF 后先分批排空已经收到的传输队列。若档案含合法结束，则可以继续播完
完整尾部；若没有结束，必须报告流中断，不能伪造成功。观众退出回到 Launcher
仍拥有的房间。旁观者不提交成绩、玩家局数或玩家自动录像副作用。

## 6. 有限资源与故障隔离

| 层 | 当前实现限制 | 失败边界 |
| --- | --- | --- |
| 普通准入历史 | 8192 个 payload / 1 MiB | 不丢前缀，显式停旁观流 |
| TH09 时间线服务器历史 | 当前局 32401 个 payload / 2 MiB，含元数据 | 不滚动成缺初始化的半局，超限停旁观流 |
| Relay 重放背压 | bufferedAmount > 64 KiB 时让出，连续受阻 15 秒失败 | 关闭该慢观众 |
| Relay 发送队列 | 旁观默认 1 MiB，可配置 `EAGLER_NETPLAY_SPECTATOR_MAX_BUFFERED_BYTES` | 断开超限接收端 |
| 公共旁观接收队列 | 16384 个传输包 | 接收失败后仍可排空先前入队的完整包 |
| TH09 JS 未交付档案 | 32404 个未交付 payload / 2 MiB | 明确报错并退出该观众 |
| TH09 native 待播输入 | 32400 帧 | 不覆盖尚未消费的输入 |
| TH09 发布队列 | 最多 8192 个待发输入；每 pump 最多 32 包 / 2 ms | 停旁观输出，保持玩家对局 |

必须同时说明“包数”和“帧数”，批次不能绕过限额。历史和发送队列持有等长
副本，不持有大型接收缓冲的切片。暂停、断开、异常和离房必须释放对应资源。
TH09 已消费的本地档案目前没有总字节上限，会随会话增长；未读配额不是总内存
上限。下放前要确定总档案预算及可见的截断/退出策略，不能把当前实现说成
内存完全有界，也不能静默删掉尚未观看的局。当前 TH09 流 15 秒无进展会报错，
首次游戏内选角等待和已经明确结束的局另行处理。

## 7. 下放顺序与合入条件

### 公共播放接口的接缝

下放实现应统一以下语义，再由作品提供初始化器与 native 操作。这里规定接口
需要表达的内容，不注册尚无消费者的新 Host 命令，也不改变现行 wire。

| 接缝 | 输入/输出与约束 |
| --- | --- |
| 身份 | product、run、作品 session/build/ABI、match 起点。禁止仅用一个跨 run 的 frame 区分历史 |
| 初始化 | 版本化的完整 scalar initializer、S、兼容性校验、能否独立恢复。解码后才交给作品造 world |
| 输入与边界 | 按顺序的全席位有效输入、段落标记、排他结束 E。marks 不消费输入序号 |
| 状态 | received、delivered、played 各自的游标，保留档案区间、liveEdge、固定 target、paused/seeking/following、当前局 ended、run retired、显式 error |
| 本地操作 | 暂停、继续、定位、回直播、离开。定位参数带明确的局/时间线坐标，不混用原始网络帧与累计 gameplay 帧 |
| 调度 | native 报告实际已模拟前沿，公共层有限批次补给。目标再远也只能消费已验证输入，预算结束后保留余量 |
| 清理 | 关闭订阅、计时器和 native 播放，释放本地档案和 pending 队列；不调用玩家会话结束入口 |

合作作品的段落可以是 Stage/Result/Retry，TH09 是 match/round，不能为了复用
一个标题或比分控件而改变原作分段。段落标签和局参数交给作品适配，时间线
和接收/播放状态由公共层统一呈现。各坐标在局切换时如何转换必须有测试。

### 迁移步骤

1. **保持现有协议可读**：以上两种输入格式继续有自己的 decoder 和版本。
   抽取行为，不把 TH09 的 magic、build、局参数、两个席位或原生菜单复制到
   合作作品。不先新增一个所有 Runtime 都还不能读取的 wire 版本。
2. **补作品重建 owner**：每个作品给出可独立开始/Retry/换面/Result 后重开的
   初始化边界、完整 scalar 状态与 ABI、确认发布位置、逻辑结束和必要的段落标记。
   `seed + loadouts` 只有经过状态等价验证才能称为完整起点。Checkpoint 若采用，
   必须另有版本、内容身份、容量与恢复证明，不能发送 native 指针或全 world 原始内存。
3. **收敛公共策略**：有限档案、接收/交付/模拟游标、固定目标定位、预算、
   尾部排空与故障隔离优先进入 `eagler-common`；作品仅保留 codec/重建/边界。
   Launcher 的时间线展示和状态传递使用公共契约，不再通过 DOM id 搬运作为
   新作品接入方式。现有 TH09 panel mount 是兼容实现，不是跨作品 ABI。
4. **Relay 与 Launcher 同步接入**：从 TH09 的 env/title 分支迁到实际支持
   完整流的产品/版本组合，统一准入、目录入口、换 run 重入和只读控制。
   `spectator-room-state` 的全作品扩展要同时更新 typed authority、产物和实际
   Runtime 接收端。不能仅移除 `games: ['th09']` 或把 catalog 改成 true。
5. **逐作验收后激活**：TH06/07、TH08、TH10、TH11、TH15 都须通过下表。
   TH15 保持纯延迟，其他作品保持各自玩家时序和玩法。更新 Launcher 不更新
   游戏内嵌的 common/WASM，记录真实依赖提交和 Runtime 代次再发布。

| 验收组 | 必须证明 |
| --- | --- |
| 身份与入口 | 目录/房码/原生入口一致，未选角色不启动，伪造 member、旧 epoch/run、双角色和双连接被拒绝 |
| 顺序与校验 | 非零起始帧、缺帧/重复、错误产品/ABI、NaN/越界输入、旧包、正常批次与坏批次、timing 在输入前 |
| 历史交接 | 重放期间仍有新输入和换局，先完整前缀再直播；新观众只得当前局，老观众保留旧局 |
| 本地控制 | 暂停继续接收，前后定位、跨局换角/换配置、连续回直播、网络突发和重建中拖动，不改玩家状态 |
| 确定性 | 玩家确认帧、顺播观众、定位观众在同一逻辑边界的 canonical state 一致，失败给出首个分歧 owner |
| 尾部与生命周期 | 单局结束、Retry、换面、Result、关流/EOF、房间先变更、暂停尾部、新 run、主动退出分别覆盖 |
| 压力与隔离 | 慢观众、长局、多局档案、丢上传、无观众、队列/内存上限，玩家确认进度、传输和 Replay 不受影响 |
| 产物与设备 | 普通/MP 隔离、2P/3P 按产品覆盖、玩家 RTC 与 WS 两路径，实际 Release Runtime 的完整前端路径；公网/移动设备另记 |

已发现的接入缺口：main 工作区安全修复候选将新房码改成八位，而 TH09
`shared-netplay.mjs` 当前只接受四位 `th09mp-` 房码。它来自独立安全改动，
不是旁观 wire 变化，接入验证必须覆盖实际创建的房码后才可激活组合。
另需补总档案容量政策，以及统一 Relay/Runtime timing 校验边界。

## 8. 源码与验证入口

Launcher 仓库：

- `src/contracts/{product-catalog,runtime-protocol,adapter-capabilities}.mts`
- `app/services/{multiplayer-room,runtime}.ts`、`server/netplay-relay.mjs`
- `server/{spectator-frame,relay-flow-control,room-directory}.mjs`
- `tests/test-{th09-spectator-frame,th09-spectator-history-relay,spectator-stop-relay}.mjs`
- `server/tests/test-th09-spectator-timeline.mjs`
- `tests/browser/test-th09-spectator-{directory,panel,timeline,continuity}.mjs`

工作区 TH09：`th09-eagler/th09_web/` 下的
`sdl-runtime/{shared-netplay,spectator-tape,spectator-timeline}.mjs`、
`cpp/sdl/Application.cpp`、`cpp/multiplayer/Spectator{Playback,Schedule}.hpp`、
`tests/server/spectator-{tape,rewind,live,room-state}.test.mjs`、
`tests/multiplayer/spectator-{playback.cpp,schedule.cpp,output.mjs}`。

公共库：`eagler-common` 的 `NetplayProtocol`、`BrowserPeerTransport`、
`AdonisSpectatorTiming.hpp`、`FrameBudget.hpp` 和
`tests/browser-peer-spectator-tail.test.mjs`。

这些入口分别提供模型、隔离 Relay、native 策略与真实前端验证。存在测试文件
不等于本轮已运行，更不等于全作品公网 PASS。每次验收记录源码/common 提交、
dirty 差异、Runtime 路径和代次、角色、人数、输入路径与证据等级。

## 9. 公共前端接入与实施状态

大厅的“满员”只描述玩家席。所有 MP 满员且尚未开局的房间仍可通过目录进入，
显式携带 `RoomInvite.s=true`，前端直接申请旁观席，不自动请求玩家席。进行中的
合作房间还必须具备实际 stream 的 `spectatable`，旧 Runtime 不因此获准中途加入。
TH09 原生 match/round 机制与面板保持原状。

合作作品的公共面板接收已认证、当前 game/epoch 的
`runtime-info.spectatorPlayback`。version=1 报告包含 run、递增 revision、
min/position/received/edge/target、paused/seeking/following/ended 及 nodes。
node 包含 id、stage/route key、kind、frame、bossSlots，可带 1..48 字符的
作品显示 label，以正确表达 Final A/B、Phantasm 等路线。

报告只列可以恢复到达的历史。最多 512 个节点，frame/边界和身份均通过公共
decoder 校验；旧报告不能降低接收前沿，退出或 epoch 变化会清空面板。TH09
报告不挂载此合作面板，避免双份 UI。

公共命令为 `spectator-playback {run, action, frame?}`，action 为
`pause/play/seek/live`。仅针对当前已启动的只读观众，run 必须匹配实际播放 owner。
seek 范围为本地保留起点至确认直播边缘。控件等待 native 新报告，不自行声称
已经暂停或定位。拖动期间固定 range 边界，提交时再次按最新边界校验。

截至本次实施检查：

| 层 | 已完成 | 尚未完成 |
| --- | --- | --- |
| common | E?SV codec、确认事件 journal、总历史预算、固定目标播放/重建调度；零观众 P1 上传 lane 与长面接收预算 | 标量初始化器和 native world 生命周期属于各作，公共策略 fixture 不能证明它们 |
| Relay | 六个合作产品识别、当前面历史、Boss 依赖、首次下载完成标记、下载中换面、故障隔离 | 公网与实体设备验收 |
| 前端 | 七作满员大厅旁观入口；合作面板、节点/拖动/暂停/直播命令和 epoch 隔离 | 真实 Runtime 产出状态并执行控制，当前没有该原生端报告 |
| TH06/07/08/10/11/15 | 已调查对应 owner 与共享接缝 | **尚未接入本次原生换面初始化器、Boss journal 和控制桥，不能称为六作下放完成** |

上述改动未提交、未发布，也没有复制进游戏的 vendored common 或更新依赖 pin。
下一阶段必须逐作做真实 Stage2+ 的完整重建与同帧 canonical state 对比，再接
原生流与浏览器准入。不得用 server 识别新包或模型 PASS 替代这个门禁。
