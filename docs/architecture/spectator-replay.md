# 流式 Replay 旁观

旁观使用游戏已有的分关 Replay 入口。关头数据负责恢复难度、机体、原生 RNG seed、初始生命与 Bomb 规则、各席位资源与 Replay 位置，后续持续追加全席位已确认输入。中途加入和时间线跳转从对应关头重建，再以有界快进到目标帧。

| 作品 | 关头来源 | 应用入口 |
| --- | --- | --- |
| th06、th07 | ReplayManager 记录的 StageReplayData 字段与多人资源扩展 | 原生关卡构造中的 Replay 注册阶段 |
| th08 | 原生 ReplayStage 字段、配置、路线、原生加载进度与各席位资源 | GameplayScene 的原生 Replay 初始化位置 |
| th10 | 多人 Replay 录像器的 ReplayCheckpoint | 原生 Replay bootstrap、玩家入场初始化与 activation |
| th11 | 原生 ReplayStageState 与多人共享资源 | GameSession 构造、原生 Replay 输入采样边界 |
| th15 | 每席位的原生 t15r ReplayStageSnapshot | SessionReplay 与 SessionActivation |

玩家子弹、Bomb、ANM、ECL 和场景资源由游戏正常创建。各作按自己的 Replay 生命周期采集和应用入口数据，激活帧与该帧输入一起进入连续历史。th09 的对战旁观沿用其比赛回放入口。

th06、th07 同时取出 Replay 录像器在网络起点之前已记录的初始化输入。旁观从原生关头构造场景，再重放这段输入，与网络帧零衔接；切关也使用同一入口。th07 记录玩家实际消费的按键，包含原生构造阶段清空输入后的首帧。录像事件统一经过可选记录器入口。

th07 的旁观 RNG 链节点在首次开始播放时创建，退出播放时从演算链移除，重复进入沿用同一节点。

th15 的关头恢复将原生应用时钟对齐到已确认输入帧，中途入场与回跳继续沿同一时间基准演算。每关的幽灵私有随机流从房间 seed 与席位重新初始化，连续玩家场景和独立 Replay 场景保持相同的漂移轨迹。

th11 在多人关卡激活时统一初始化玩家子弹与子机相位，采集后的关头可独立重建。原生玩家位置、资源、武器模式和各席位选择进入同一 Replay 入口。多人规则版本 4 将最近未满 Power 的道具归属同时纳入房间握手与录像元数据校验。

th06、th07、th10 的多人录像已有分关数据。th08、th11、th15 的多人录像文件沿用开局参数和确认输入；旁观的分关入口复用原生 Replay 类型与初始化位置。

## 传输与播放

合作作品使用 `E?SV/2` 发送关头、导航节点和结束事件。单个关头最多 4096 字节，按作品自身格式解释。Relay 保存最新关头及其后连续输入，新旁观者从可用关头开始正常播放。

共享层负责连续历史、确认发布、缓存边界和播放控制。暂停期间继续接收输入，跳转目标固定在操作发起时的帧。Boss 节点保存导航位置，重建使用所属关卡的原生 Replay 关头。同一关卡向前跳转直接消费现有场景中的输入。“回到直播”完成一次固定目标的追赶后恢复正常速度，重复点击和后续输入保持本次目标不变。th06、th07 在本局退出演算时以连续已确认游标发布结束事件，旁观者播完缓冲尾帧后结束。旁观输入独立于玩家输入，存档与录像输出沿用旁观只读生命周期。

合作旁观按发布结果区分发送等待与历史错误。关头、输入或 Boss 导航包暂时无法发送时，保留当前发布阶段与游标，在后续网络泵中继续，已发出的包保持单次提交。记录缺失、输入缺失和编码错误结束本次旁观发布，并记录作品、原因和帧号；玩家演算沿独立通道继续。

th06、th07、th08、th10 与 th11 在结算前保存完整的连续确认输入和结束标记，复用共用尾包发送器按每次最多 32 包、2 毫秒发送。发送等待期间结算画面继续运行，观众收到尾部后自然结束；尾包拥有独立的 16 MiB 容量和 15 秒发送期限，离房时释放。th15 沿用同样的尾包发送规则。

Relay 收到合作 Replay 的有效结束事件后关闭新入场，已入场观众继续保留连续历史、暂停位置和未播完的尾帧。随后到达的主机停止标记确认上传结束，观众播完自己的缓冲后自然结束。主机主动关闭旁观时，Relay 记录房间、局次、停止通道、是否正常结束和受影响观众数。Runtime 错误通过状态栏与持续 8 秒的提示同步展示，旁观退出后可直接读取原因。异常退出没有附带详细错误时，显示异常退出提示。

th06、th07 的退出桥接携带原生网络失败原因，即使主循环正常返回也按失败退出报告，启动器保留对应提示。

## 验证入口

th06、th07、th08 的旁观输入按 SDL 普通 60 Hz 时钟播放，积压输入由用户的跳转或直播操作定位。原生 Replay 重建保留音乐载入、切曲与停止命令，BGM 跟随当前关卡和 Boss；历史音效在显式快进期间静音。th06、th07 以共享音乐作用域保留 Replay 重建的持久声音状态；th10 的历史音效接收器让音乐命令继续进入原生队列。

- `common/tests/spectator-stage-test.cpp` 覆盖协议、连续历史、暂停、跳转和直播控制。
- `common/tests/spectator-stage-publisher-boundary-test.cpp` 覆盖确认发布和关头输入边界。
- `services/relay/tests/test-spectator-stages.mjs` 与 `test-spectator-stage-relay.mjs` 覆盖事件身份、缓存替换和真实 WebSocket 旁观分发。
- `npm run ports:mp` 覆盖六作普通／挑战模式的真实多人与旁观行为，包括上传受阻恢复、暂停回跳和独立观众入场；`--wait-end` 验证普通局结算时保留暂停位置，再播完完整尾部。
- `ports/th07/tests/netplay-spectator-natural-browser.py` 对比正式 th06、th07 Runtime 的持续播放、Boss 导航与回跳后的同帧状态，支持真实键盘的射击、移动、低速和 Bomb 输入，并核对 OGG 曲目载入。
- `ports/th07/tests/netplay-spectator-stage2-browser.py` 验证 th06、th07 的原生切关、独立旁观和持续跟播。
- 持续跟播核对同帧游戏状态、接收连续性和固定直播目标，网络积压按正常播放节奏消费。状态一致性按相同演算帧比较；播放与实时进度的距离另行记录。
- `ports/th08-mp/portable/multiplayer/check-spectator.py` 验证各席位输入、原生加载入口、第二关与播放控制。
- `ports/th10-mp/portable/multiplayer/check-spectator-stage.py` 验证多人 Replay 入口与第二关连续 600 帧演算。
- 同一驱动的 `--music stream` 接入原版 OGG 并检查旁观重建后的原生解码器。`--navigation-only` 验证原生关头重建、向前定位、反复回跳与恢复播放，`--lifecycle-only` 验证固定直播目标和普通播放节奏。
- th11 的 `portable/multiplayer/build-spectator-stage.mjs --production` 将私有观测入口链接到当前正式游戏对象，再由浏览器驱动比较独立实例。
- `ports/th11-mp/portable/multiplayer/spectator-relay-browser.mjs` 验证原生关头首帧、后续 600 帧和两位独立旁观者。`TH11_TEST_UNINTERRUPTED_FRAMES=12000` 持续核对 12000 个输入帧，期间保持正常播放。
- `ports/th15-mp/portable/multiplayer/spectator-stage-codec.cpp` 使用原生 Replay 关头验证两人、三人和截断数据的事务式解码。

原生 Replay 验证独立实例在相同输入下的资源、RNG seed、玩家、敌机、ECL、弹幕与道具演算。th15 的私有旁观观测同时覆盖幽灵随机流、敌弹运动程序、道具位置与归属及激光状态；关头重建的背景与章结算动画计数保留为呈现诊断，关卡结算门控仍参与状态对比。伤害源按原生活动位参与演算比较，已释放伤害源的旧字段作为诊断记录。完整 world hash 同时保留为诊断数据；从关头重建的界面累计统计和切关展示使用原生 Replay 的初始化值。
