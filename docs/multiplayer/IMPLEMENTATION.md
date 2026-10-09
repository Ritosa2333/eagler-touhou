# TH11 Multiplayer：实现、规则边界与验收记录

记录日期：2026-10-09。首次实现与首轮 UI 审查分别保留在第 7、9 节。用户随后明确指出：原作已有 Replay 播放流程，08 / 10 MP 使用共享分数，不能自造网页播放器和逐人成绩页；相应修正在第 10 节。随后确认多人集合只显示生命 / Power、其他原生个人信息只显示本机、连接 UI 由标准 Launcher 负责，碎片始终原作一倍，当前规则与标准 Launcher 联合验证以第 11 节为准。用户允许补足未定义行为，并不授权忽略原作和参考实现已经定义的行为。第 5 / 6 节未获确认的其余临时规则仍须高危提醒；不 push 或部署。

## 1. 本轮要求与基线

用户要求：参照 TH08 / TH10 MP 实现 TH11 MP，严格遵循 Launcher 规则，未定义行为必须告知；沿用现有 MP 分支名；**本轮不做预测与回滚**。

| 角色 | 工作区 / 分支 | 核对提交 |
| --- | --- | --- |
| TH11 Eagler 基线 | `th11-eagler` / `eagler` | `0a582e63598fd0a4c89063a37829d92af65c2a90` |
| TH11 MP | `worktrees/th11-multiplayer` / `eagler-mp` | 从上述基线建立 |
| TH08 MP 参考 | `th08-eagler` 的 `eagler-mp` ref | `b5b4091cb19619576035ea64ce2913bb22b3833b` |
| TH10 MP 参考 | `th10-eagler` 的 `eagler-mp` ref | `288ea8d5676ace5152ca5cd317061511ee660887` |
| 两个参考 MP 的共享库依赖 | `eagler-common` gitlink | `43add4dc9085c65468f7ab15428c00206cbe3834` |
| TH11 MP 当前共享库依赖 | `eagler-common` gitlink，TH11 identity、退休 ACK、观战尾帧与三人退休测量广播修复 | `e02347fac98c9e599d30a6ffd7ef30756555b948`（初次实现为 `d81eda4`） |
| Launcher 规则来源 | `eagler-touhou` / `experiment/ui-main` | `039b6e0034a70536afe5d7561d078d7e7e2c295d` |
| Launcher 独立提交基线 | `experiment/ui-main` 本轮期间更新后的提交 | `fed02bf5fc6027615c6253205f8afeb196c29323` |
| Launcher TH11 接入提交 | `worktrees/launcher-th11-multiplayer` / `experiment/th11-multiplayer` | `4eacf9efc182ae005060c71fbae8ffe8bb554ebf` |
| Launcher 临时边界补充 / 当前 tip | 同上，P/F 优先级与 Replay 容量 / 溢出说明 | `0409d7f80d8850427d907deec40e8df93d77139c` |

表中路径相对于 `D:\workspace\eagler`。08 / 10 的实际 MP 分支都叫 `eagler-mp`，不能沿用旧文档的 `experiment/thXX-multiplayer`。初次审计时引用的 Launcher 规则文件无工作区 diff；本轮新增 TH11 临时规则与能力接入。其余 Launcher 存在别人的未提交修改，必须保留。本轮不改普通 TH11 工作区，不混用普通 / MP Runtime；消费者须显式声明共享库版本，不能任意使用兄弟目录 HEAD。

## 2. 规则权威

先读工作区 `AGENTS.md`、`WORKSPACE.md`、`docs/agent/ROUTES.md`。以下路径除机器契约外均位于 `eagler-touhou/`：

| 来源 | 权威范围 |
| --- | --- |
| `docs/playbooks/multiplayer.md`、`adaptation-worktrees.md` | primary playbook、产品隔离、输入 owner、帧零、观战和工作区晋升 |
| `content/MULTIPLAYER.md` | Launcher 当前面向玩家的玩法规则 |
| `docs/ADAPTER_BEHAVIOR_INVARIANTS.md` | 原作默认语义、多人呈现、确定性转移、团灭和原地救援 |
| `src/contracts/adapter-capabilities.mts` 的 `REQUIRED_MULTIPLAYER_BEHAVIORS` | 声明 MP 后必须完成的完整 profile |
| `src/contracts/product-catalog.mts` | 产品、人数、难度、机体、时序与 Runtime 路径 |
| `docs/ADAPTER_CAPABILITIES.md`、`ADAPTING_A_GAME.md`、`GAME_ADAPTER_CONTRACT.md` | 能力分类、所有权与完成标准 |
| `docs/playbooks/adonis-adaptation.md` | 实际输入通道校准、纯延迟与帧零协商 |
| `docs/playbooks/replay-determinism.md`、`testing-acceptance.md` | 权威帧、Replay 与证据等级 |

原作 gameplay / menu 是默认基线，只有明确产品规则才覆盖它；旧 MP 代码只是参考。玩法介绍现已加入 TH11 及高危提示，普通 P 掉落条款原先已经明确点名地灵殿。适用通用规则保持，第 5 节记录补足空白的临时决定。行为不变量中的 `P 4/8` 是历史反馈示例；当前玩法明确为 **5 次点按，从第 3 次显示计数**，不能复制旧 8 次实现。

## 3. 已明确的共通玩法

### 战斗、死亡和共享进度

| 行为 | 必须保持的规则 | 来源：`content/MULTIPLAYER.md` 章节 |
| --- | --- | --- |
| Boss 耐久 | 不改当前 / 最大生命值和脚本阈值；Boss 受伤系数按可操作人数取 1 人 1、2 人 0.75、3 人 2/3；幽灵不算可操作玩家。 | Boss 耐久与玩家人数 |
| 最终死亡 | 耗尽生命后成为不能操控、不能射击、随机飘动的幽灵；最终死亡掉落内容保持原作。幽灵随机行为仍须由各端一致的权威状态推进。 | 死亡行为 |
| 过关复活 | 下一关让幽灵复活；关卡出生与场内救援是不同生命周期边界。 | 死亡行为 |
| 团灭 | 全部参战者进入幽灵 / 可复活终态后计时 180 个正常逻辑帧，任何席位恢复就取消；期满进原生 Game Over / Result，不能进入 Continue 或执行其资源 / Rank / 计数重置。 | 死亡行为；行为不变量“团队全灭不进入 Continue” |
| P1 权威 | 剧情、路线、自机 / 机体 / 装备决定的共享关卡分支和道中符卡跟随 P1；此条没有定义全部敌人的攻击目标。 | 剧情、路线与共享关卡内容 |
| 共享符卡 | 符卡收取 / 失败状态共享，各机体产生原生失败事件的条件仍保留。 | 杂项 |
| Rank | 死亡 / Bomb 降低量按人数分摊，避免每席都施加完整单人降幅。 | 难度等级（Rank）调整 |
| 解锁 / 存档 | 所需角色、Extra、练习与适用路线解锁只载入内存；不保存多人分数 / 解锁进度，只有 Replay 可保存。 | 解锁状态同步 |

### 残机与 Power 转移

| 行为 | 必须保持的规则 | 来源 |
| --- | --- | --- |
| 残机操作 | 距离 20 像素以内，赠送者松开 Shoot、低速保持 1.5 秒，即 60 Hz 下 90 逻辑帧；连续低速只成功一次，再次转移须松开低速。 | 玩法“残机转移” |
| 残机效果 | 赠送者扣 1 残机；给存活者生成 1 个原作生命道具定向飞行并增加 1 残机；给幽灵则救援。 | 同上 |
| 残机目标 | 幽灵优先；同类选残机更少者；仍相同选较低席位。不能使用 RNG、墙钟、到达顺序或本地 viewer 作 tie-break。 | 同上；行为不变量“多人资源转移目标选择必须确定” |
| 救援资源 / 位置 | 奖励生命不能自行复活幽灵；救援设置为本机体半满 Power，有独立 Bomb 数时恢复初始数。保持幽灵当帧位置，不能进入会重新定位的 Respawn。 | 玩法“残机转移”；行为不变量“幽灵救援必须原地复活” |
| 救援无敌 | 06 / 07 / 08 明文为不消弹、240 帧；10 为不消弹、280 帧；TH11 临时采用不消弹、280 帧，见第 5 节 A。 | 玩法“残机转移” |
| Power 操作 | 20 像素内点按 Shoot 5 次，相邻间隔不超过 24 逻辑帧，从第 3 次起在自机上方显示计数。 | 玩法“Power 转移” |
| Power 资格 | 赠送者至少有完整一次量；接收者须可操作且未满 Power，幽灵不合法；按 Power 更少、然后席位更低选择。 | 同上；行为不变量“多人资源转移目标选择必须确定” |
| Power 效果 | 扣除对应 Power，生成等价值原作道具自动追踪接收者；TH11 每次双方各自一档，按显示档数比较，见第 5 节 A。 | 玩法“Power 转移” |

### 掉落、奖励与呈现

| 行为 | 必须保持的规则 | 来源 |
| --- | --- | --- |
| 普通 P 倍率 | **地灵殿已明确为 2P 原作一倍、3P 两倍**；Full Power 不倍增。 | 玩法“杂项” |
| 整残机 / Bomb 掉落 | 道中对应整道具 2P 一倍、3P 两倍；TH11 残机碎片不使用该倍数，任意人数均为原作一倍。 | 同上；用户本轮明确覆盖碎片规则 |
| Extend | 分数 / 点数目标奖励分配给队友，服从各自上限；只有 TH08 明文限定仍可操作者。TH11 共享 5 碎片向全队各发 1 命、各自 cap 9、不自动复活幽灵已经用户确认。 | 同上；第 5 节 B |
| 初始 Bomb | 有独立 Bomb 数的作品为原作每命数量减 1、最低 1；不据此新造 TH11 Bomb 存量或改其 Power 耗费。 | 同上 |
| 可见性 / 立绘 | 队友透明化，遵守本地玩家可见性选项；取消符卡释放立绘，保留文字宣言；不改权威 gameplay。 | 同上；MP profile |
| 出生站位 | 在原作允许区域围绕默认点对称分散；间距可按场地确定，不改变活动范围。 | 行为不变量“多人初始站位必须分散” |
| HUD / 给予反馈 | 多人集合只放生命 / Power，1P / 2P / 3P 放各组开头，本机使用原作黄色强调。Graze、点值、通信等其余个人信息在原作位置只显示本机。原位共享 HiScore / Score 保持；字体、数字与图标复用原作。转移进度跟随自机；用原作道具 / 音效反馈，不做右侧进度条或现代卡片。 | 用户本轮明确；行为不变量“多人 HUD 必须延续原作 HUD 的视觉语言”“残机与 Power 给予反馈必须出现在游戏区域内” |

## 4. 逐席保留的 TH11 原作语义

本节路径相对于 TH11 仓库。`th11_web/cpp/game/ShtResource.hpp` 定义 SHT 的 `max_power_level` / `power_step`，`PlayerFrame.cpp` 按所选 SHT 覆盖 economy；不能以 `GameEconomy.hpp` 的默认 `400 / 100` 代替实际资源。本轮按已定义 SHT 格式检查得到：

| 机体 | 最大档数 | 原始 `power_step` | 原始 `max_power` |
| --- | --- | --- | --- |
| 灵梦 A / B / C、魔理沙 B / C | 4 | 20 | 80 |
| 魔理沙 A | 8 | 12 | 96 |

每席保留所选机体的速度、射击、Option、Bomb、Deathbomb、上限与耗费。`ItemRewards.cpp` 中小 P 加 1、大 P 加接收者 step、F 加本机体 max；救援半满也取本机体上限，不能照搬 TH10 的 raw 100。

**分数为全队共享；通信率、擦弹、点值和个人资源保留各自机体语义。** 初版把整个 economy 拆为逐席对象，连 score 也独立累计，这是对 08 / 10 MP 参考的错误应用，已在第 10 节修正。当前各席的 score 引用同一主 owner，HUD、结算和 MP Replay 元数据读同一份团队总分；不靠每帧拷贝或最后相加。`Communication.cpp` 仍按该玩家坐标和奖励更新，封顶 13000；点值与道具吸取仍使用该玩家的通信率。共享分数不等于把这些机体状态也合并。

`ItemRewards.cpp`：类型 5 为碎片，5 片换 1 命；类型 7 为整残机；上限 9。原生换算 / 上限继续保留，多人共享计数与分发见第 5 节 B。`BombController.cpp` 的 selection 5 为魔理沙 C 河童护盾，未触发返还 10 原始 Power、被击中才计 Bomb / 符卡失败，须逐席保留，不能因共享符卡而让展开护盾立即失败。混合 Bomb 伤害先按攻击所属席位套该机体原作机制再汇总，不能因一位护盾影响全部队友；局部状态与真正共享背景 / 音效 / 敌人效果的 owner 拆分是实现责任。

## 5. 规则补足与已确认边界

**高危：用户授权“先实现，未定按理解决定但必须高危提醒，不要停”。下列尚未获确认的规则为实现者补足，并非从旧文档推导出的既有共识。第 5 节 B 的共享碎片奖励与一倍掉落现已获用户明确确认。** 其余决定会改变资源经济、敌人目标和特殊攻击难度，尚未有完整多人平衡 / 流程证据；后续改变可能影响 Replay 兼容性。新的未定义行为须继续记录并提醒，不能静默改掉规则。

### A. 资源转移、满 Power 与救援

一次交换扣赠送者自己的一个 `power_step`，接收者增加自己的一个 `power_step`，各自按本机体上限封顶。魔理沙 A 的 raw 12 与其他机体 raw 20 不做原始点数守恒。目标按显示档数 `power / power_step` 比较，较少者优先，再按席位；确定性比较使用整数交叉乘积，不能依浮点显示舍入。

TH11 救援采用 **不消弹、280 个逻辑帧无敌**，继续原地复活、半满本机体 Power。场上 **全部可操作玩家各自满 Power** 后才触发全场 P 转点；幽灵既不阻挡转换，也不能接受 Power。20 像素、5 次点按、24 帧间隔和第 3 次显示计数不变。

**高危：定向 Power 道具在飞行途中，若接收者变成幽灵，立即取消定向并按普通 P 留场，供合法存活玩家收取；赠送者不退款。** 原规则没有定义这个途中失去资格的边界。本轮选择保留场内道具、避免向幽灵发 Power，不把已扣资源回滚给赠送者。

**高危：本轮 Boss 人数系数和全员满 Power 判定中的“可操作席位”，具体采用尚未进入最终幽灵的参战者；仍包括原生死亡 / 重生动画过渡中的席位。** 敌人、子弹、激光和普通道具选择目标时另按 `alive()` 筛选，不把正在死亡的席位作为合法目标。临时失去操作不会立即降低 Boss 系数分母，也不会从满 Power 条件中移除该席位。

**高危：残机赠送道具起飞时的接收者若在飞行途中变成幽灵，仍向原接收者增加储备残机，服从上限，但不会自动复活。** 这与 Power 道具取消定向的策略不同；救援与过关仍是复活边界。原规则没有定义这项途中变化，本轮没有将其静默视为一次现场救援。

### B. 残机碎片、奖励分发与阶段条件

**已确认：** 使用 **全队共享碎片计数**，每收满 5 片向全部参战席位各发 1 残机，各自服从 9 命上限；奖励不自动复活幽灵。碎片掉落在 **所有人数下均为原作一倍**。各席位个人原作计数不再独立触发第二次碎片 Extend，避免重复奖励。此前 3P 碎片翻倍的临时规则撤回；普通 P 与整残机的既有掉落倍数不因此改变。

**任何玩家 Miss 都纳入共享阶段死亡条件**；符卡成功 / 失败仍共享。各机体原作何时真正产生 Bomb / 失败事件的条件保留，尤其河童护盾展开时不能提早失败。脚本计数的具体 owner 和重置点在原生实现中统一，不能仅读 P1 或重复累加同一事件。

### C. 共享场地对多个玩家的目标与作用

敌人 / 子弹 / 激光在原作查询目标的时点，按 **来源位置到 Alive 玩家的最近距离平方** 选择，并列取低席位。道具先按每席原作通信率、当帧移动后位置和道具原生移动 / 吸取步骤确定拾取 / 吸取资格，再在合法者中按最近、并列低席分配。无可操作玩家时保留原作查询所需的确定性场地回退坐标，不将幽灵作为合法目标。

**高危：普通 P（含 Full Power / F）在合法接收者中先优先未满 Power 的席位，再按距离、并列低席选择；只有没有合法的未满 Power 接收者时，才回退到满 Power 席位。** 这是本轮采用的合作资源分配策略，可能让较远的未满 Power 玩家优先于近处的满 Power 玩家；它不是原文“最近合法者”自动包含的既定含义。定向赠送的 Power 道具仍遵循第 5 节 A 的指定接收者及其途中失去资格规则。

TH11 `AttractPlayerAndBullets` 等全场特殊吸引临时作用于 **全部 Alive**，分别以各自当前位置应用原作效果；同一帧完成环境同步。P1 仍决定剧情 / 路线 / 机体依赖的共享关卡内容。

参考代码并不统一：TH10 `WorldTargetsMultiplayer.cpp` 为来源最近 Alive；TH08 `PlayerRoster.hpp` / `BulletSystem.hpp` 的弹幕选最近 eligible，但部分 `EnemySystem.cpp` / `GameplayScene.cpp` ECL 留有 seat-zero 兼容。本节是明确的 TH11 临时决定，不把这两种历史实现伪称为 Launcher 通用规范。

## 6. 仅纯锁步与实际通道延迟

`adonis-adaptation.md` 已有完整流程：资源 / 世界准备、真实输入链路 HELLO、稳定等待和 129 轮测量、proposal / accept / commit / ack、全席位帧零屏障。3P 必须包含 P2–P3；使用本地高精度单调时钟，严格遵守有效样本、中断、路由变化与超限拒绝，不按手机数或 Launcher RTT 猜 D。

自动纯模式 `P = 0, D = B`，其中 `B = max(1, ceil(floor(max_required_RTT_us / 2) * 60 / 1000000))`。手动模式同样测量 B，但使用用户明确选择的 D（含 0），不要求手动 D 等于 B；D 依声明验证，自动溢出显式失败。每个输入只采样一次归属 N+D，缺真实输入不能预测前进；Replay / spectator 不再加 D。对局内冻结 D，相位修正只调整壁钟调度，保持权威 60 Hz。

能力声明使用 `rollbackLimit: 0`，所有配置只接受 `netplayAdonisMode: 1`，实际选定 / 上报的预测量为 0。启动请求仍携带 common v5 契约要求的 `predictionReserve: 2`，该预留在纯模式不使用，不能传 0 绕过 1/2 校验。挑战模式沿用既有 cooperative 开关与 TH08 / TH10 行为，见第 11.7 节；它不放宽 P=0、D 范围或实测启动要求。此前自行排除挑战模式的决定已撤回。

必须绕过 undo 分配、捕获、恢复和修正；资源回收、确认音频 / 文件、Replay / 观战仍需 owner。Launcher 声明、UI、配置校验和 native 必须一致限制纯模式，不能仅默认关回滚却仍接受 hybrid，也不能加 TH11 特判绕过能力模型。

### Replay 与重开边界（实现决策）

TH11 MP Replay 采用公共 `EAGLRPY1` 扩展，`gameId = 11`，版本化 128 字节 `T11M` 描述记录配置 / 实测 D、`recordedPlayer` 与全部席位已确认输入。自动保存优先使用 `/savesth11mp/replay/th11_##.rpy` 的 01–99 空槽。**Replay 接入 TH11 原生 `TitleScreen::Replays` 的 ANM、25 行列表和关卡选择；没有另一个 HTML 列表、播放工具条或滑杆。** JS 仅处理文件 / Host 接口、构建身份和定位期间的只读提示，不新增 Launcher 命令。

**高危：本版浏览器 Runtime 拒绝不同 WASM 构建指纹的 MP Replay，避免临时规则变化后静默错误回放；升级构建可能使旧录制无法在新版播放。** 章节跳转从帧零使用同一 `GameSession::begin / update / draw` 生命周期重建，不读写回滚快照，不再次应用 D；每个 RAF 以约 8 ms 预算推进。当前修正版的短程 native / browser 一致性见第 10 节，整关及长程 seek 不在本次已通过的证据范围内。

**高危：01–99 自动槽全满后，本轮自动保存继续选择现有 Replay 命名空间中的空闲 `th11_udXXXX.rpy`，其中 XXXX 为四位小写 base36。** 先检查完整编号区，再按 base36 递增检查扩展区；不会覆盖原有编号或导入录像。同一代首次选中的自动路径由后续 flush 复用，下一代重新选择空路径。手动指定编号的语义不变。真正的文件 I/O 失败仍显式报错，不能把逻辑槽位扩展宣称为磁盘 / 浏览器存储故障容错。

**高危：每一代最多记录 250,000 个已确认输入帧，按 60 Hz 约为 69 分 27 秒，包含暂停、Ending 等仍有输入的帧。** 达到最后一个合法帧后，不再执行第 250,001 帧，而是通过现有确认与 ACK 退休流程结束本代，保存标记为途中（partial）的 Replay，向标准 Launcher 发送 `notice` 说明帧数上限，然后保存并通过既有 `exit` 返回房间。需要重新开始新的对局；不分卷、不跨上限继续，不将容量终止标成普通通关。该上限来自公共 Replay 格式的容量约束；本轮选择的是明确结束策略，不能声称已做完整 69 分钟浏览器实跑。

P1 在暂停状态按 R 发起重开，先完成旧 generation 的 ACK retirement，再派生新的 sessionId 与下一 seed；旧的未来输入不能流入新局。**高危：这里的重开使用新一代种子，不保证复现上一局的相同开场随机序列。** 这是本轮已记录的实施选择，不新增任意席位独立重启共享世界的权限。

每一代重开都重新进行 129 次实际通道测量，不能把上一代的 D 当作新一代测量结果。刚退休一代的合法尾包只用于严格限定的退休边界，旧终局 ACK 可继续重发；不会把它们送入新一代输入 Core。Replay 和观战消费到录制的 R 时正常结束旧流，不加入下一代。

正常观战终局最多冻结 256 帧待发送的已确认输入，每次最多发送 32 包或用时 2 ms；在 15 秒内独立排空。末尾数据与正常 STOP 走同一条有序 relay 流。观战压力不阻塞玩家确认、退休或重开；超出尾帧上限或排空超时则明确标记观战失败。接收端先消费已缓存的尾帧，再处理 EOF。

## 7. 首次实现与验收证据（2bd03f0）

MP profile 仍要求：独立 Runtime / build / storage / Replay 身份、普通 / MP DATA layout 一致、Catalog 人数 / 难度 / 机体与双语标签、准备和帧零同步、每席确定性输入 / mismatch、同步 pause / restart 新 generation、MP Replay、显式 start-only 观战准入与只读隔离、WebRTC / 共享 Relay fallback、本地玩家可见性 / 诊断 / 返回房间生命周期。不能用 spectator=false 掩盖未完成功能。Relay 使用 `EAGLER_NETPLAY_*`，不新造 title 服务命名。

实现复用 Launcher 现有触控 / 导航 / Package / 存储；逐席保留所选机体的运动、射击、Bomb、Deathbomb、经济和通信率。共享世界每帧只推进一次，P1 决定共享关卡分支。Always Hitbox 使用原生判定圈的独立呈现 owner，队友透明度和右侧 HUD 背景采样只作用于绘制副本，不改权威世界。

团灭 180 帧后进入原生 Game Over / Result 动画，移除 Continue / Retry / 普通 Replay 保存项，仅 P1 的新 Confirm 边沿可返回。标准关卡终止走原生 Ending / Staff / Title Results，并显示各席位成绩；普通分数、解锁和排名不会被 MP 写回。原生测试通过注入 StageExit 边界进入真实 Ending 生命周期，此证据不等于实际打通全流程。

| 本次证据 | 状态与边界 |
| --- | --- |
| 规则、基线与参考 ref | 已核对；文档新增前 TH11 MP 工作树干净，没有全仓 fetch |
| TH11 MP 原生 gameplay | `node portable/multiplayer/native-test.mjs`：1,996 项检查、5,541 个真实逻辑与绘制 tick PASS；另有 `--risk` 241 项、709 tick PASS。覆盖六机体、混合 Bomb、2P / 3P、不同本地席位、目标 / 道具 / 资源转移、幽灵 / 救援 / 团灭、Ending 边界及呈现隔离 |
| 最后两处修复的原生回归 | `node portable/multiplayer/native-test.mjs --repair`：1,389 项检查、2,152 tick PASS；2P / 3P × 同 seed / 新 seed 的暂停后重建均与 fresh peer 从 frame 0 连续 220 帧一致，全部席位移动且计时推进；另校验三席 HUD、原纹理 / VM 不变与 Always Hitbox。日志 `artifacts/multiplayer-native/native-repair.log` |
| 原生协议与共享库 | `node portable/multiplayer/network-test.mjs` PASS：2P / 3P、D=0/2/9、P=0、undoBytes=0，N+D 单次采样、Replay 不重复加 D、P3 身份、跨作品 / malformed 拒绝、退休 ACK 丢失恢复、新 generation 实测、观战尾帧；报告 `artifacts/multiplayer-network/report.json` |
| Replay 容量与自动路径 | `node portable/multiplayer/replay-capacity-test.mjs` PASS：真实 Archive 写入 250,000 帧并保留末帧、拒绝额外 Append、容量编码强制 partial，native validator / 浏览器识别 P3；99 个编号加 ud0000 占用后选 ud0001、不替换、base36 进位及命名空间耗尽均通过。报告 `artifacts/multiplayer-replay-capacity/report.json`；此处没有 69 分钟 gameplay 实跑证据 |
| 普通版回归 | 重建普通 WASI 后对既有 quick golden 比较 4 段 Demo，共 18,049 tick，economy / player / RNG / entities 全部一致、无 optional divergence；报告 `artifacts/replay-verifier/quick-result.json`。未运行原作 EXE、未重生 golden |
| 打包隔离 | `node --test portable/multiplayer/package-isolation.test.mjs`：7/7 PASS，覆盖实际 WASM exports、错误指纹 / 源码 / pin / 校准拒绝、普通与 MP 隔离、闭合文件清单和保留已有输出 |
| 最终 Emscripten 构建 / 生产包 | 普通 `--thprac` 与 MP `--multiplayer` 均 PASS；生产闭包分别 18 / 21 个文件，实际 WASM / 源码 / 依赖 / exports 校验 PASS。MP WASM 为 `d60aa35750704601fbad0a3a1fb53f1983b49594b230e826001d77b0326167e7`（2,746,324 bytes）；普通版为 `bf1f1ef272ee122f835ef1d798e3ba8ae07640e1583601b331b13c0ec8e007e4`（3,003,971 bytes） |
| 生产 WASM 真实浏览器 | 最终四组全部 PASS，600 个同帧采样点、206 个 Replay 采样点一致，另有旧代观战尾帧 72 点一致，四次向后定位均匹配；每席实际移动且无页面异常。具体 RTC / Relay、R / 首包丢失 / 槽满 / 观战范围见下文，不与合成 Runtime 的 Launcher 页面测试混淆 |
| Launcher 接入 | 独立提交已完成；产品 / 构建变体、pure-only / challenge=false、EBTM / EBNP 过滤与房间 / Runtime 入口均已接入，具体通过项和基线阻塞见下文 |
| 真实设备、实际全关通关 / 全 Ending 分支、公开网络 | 未验证。当前本机 Chromium 短程验收不能替代这些证据 |
| Canonical 晋升、push、发布 / 部署 | 未执行 |

原生一致性比较包含 title 所有者的选定状态和 ECL 线程 / 栈（资源地址作稳定归一），不是逐字节全内存证明。实际浏览器另要求每个参战席位发生真实移动，比较同一逻辑帧和 Replay / seek 的世界哈希；缺输入期间不得推进世界。房间加入、可见画面和构建成功不作为游戏同步证明，实验晋升另按隔离 playbook 完成。

### 生产包真实浏览器矩阵

使用上表同一 MP WASM、真实原作 DATA、Chromium 和当前 Launcher 的共享 Relay。每组参战者都检查 129 次实际通道测量、P=0、校准提交前世界不推进、每席确有移动、所选机体与 Replay / 向后定位的一致性。这里是本机无头 Chromium 多会话测试，不是设备帧率、公开网络或整关通关证明。

| 场景 | 原生世界同帧比较 | Replay 比较 / 向后定位 | 其他结果 |
| --- | --- | --- | --- |
| 2P RTC，自动 D，机体 0/5，240 帧 | 151 次 PASS | 46 次 PASS；倒退到 123 帧匹配 | 实测 D=1；真实关闭另一浏览器上下文后显示断线错误、世界冻结 |
| 3P Relay，手动 D=2，机体 1/3/4，360 帧 | 玩家与观战 153 次 PASS | 77 次 PASS；倒退到 154 帧匹配 | 观战 Runtime 延后 150 帧启动；只读输入不影响世界、未发送玩家包 |
| 2P Relay，R + 首包丢失 + 观战 + 满槽，机体 2/5 | 新代 139 次 PASS，旧代观战尾帧另 72 次 PASS | 旧代 19 次 PASS；倒退到 92 帧匹配 | 第 1 代重新完成 129 次测量，D=2 / P=0；所有席位实际移动至 240 帧。99 个编号与 ud0000 共 100 份既有文件哈希不变，旧代保存为 ud0001；后续 flush 复用新代路径 |
| 3P RTC，手动 D=2，机体 0/3/5，240 帧 | 157 次 PASS | 64 次 PASS；倒退到 145 帧匹配 | 全网状三席输入链路，无页面异常 |

逐组 JSON、截图和本机实跑导出的 Replay 均在 `artifacts/multiplayer-browser/`。最终 2P / 3P 截图已检查：HUD 首行与难度标签分开，三席标签 / 数字 / 图标可读，原生右侧背景的烘焙装饰字不再与 P2 / P3 重叠。

本轮运行中暴露并修复了 MP 关闭音乐时仍请求 OGG、暂停后重建继承 `rate=0`、HUD 覆盖原作烘焙装饰字、自动录像编号槽耗尽等实际问题。旧版 R 后 Replay 倒退曾在比较帧 87 不一致；最终版补了暂停后 2P / 3P 同种子 / 新种子逐帧重建回归，并通过上述真实 R / seek。另一次 3P Relay 初测是 fixture 错把手动 D 断言成实测 B，已按 Launcher 明文修正断言，未为通过测试改动生产延迟公式。旧失败不作为当前构建的通过证据；最终矩阵的四份报告都记录同一个 `d60aa357...` WASM 指纹。

Launcher 与 Runtime wire 使用 base game `th11` 加独立 `runtimeVariant: multiplayer`；产品身份为 `th11mp`，Runtime 为 `./runtime/th11/multiplayer/th11.html`，物理存储为 `/savesth11mp`，Replay 文件名仍遵循 base prefix `th11`。共享资源 / DATA 仍取 `th11`，不能借改变 wire game 绕过 RuntimeSession 的 GameId 校验。编译身份须来自产物，不由 URL 推断。

### Launcher 独立提交的验证

独立工作树 `D:\workspace\eagler\worktrees\launcher-th11-multiplayer`，分支 `experiment/th11-multiplayer`，以 `fed02bf5fc6027615c6253205f8afeb196c29323` 为基线。接入提交 `4eacf9efc182ae005060c71fbae8ffe8bb554ebf` 包含本轮 21 个作者文件，718 行增加 / 52 行删除；后续 `0409d7f80d8850427d907deec40e8df93d77139c` 只补充玩法 Markdown 和 guide 断言（35 行增加 / 1 行删除），保留前一提交，累计路径仍恰为同一 21 个。当前 tip 为后者，工作树 clean，没有 push 或部署。

- `npm run build:launcher`（62 个 TypeScript 源文件）、`npm run typecheck:ui`、`npm run build:content`、`npm run build:ui`：全部 PASS。
- `node --test tests/ui-main/multiplayer-room.test.mjs tests/ui-main/lobby-directory.test.mjs tests/test-multiplayer-runtime-options.mjs tests/test-multiplayer-lobby-snapshot.mjs tests/test-multiplayer-guide.mjs`：接入提交 **67/67 PASS**。`0409d7f` 补充 P/F 与 Replay 临时边界后，两份工作树的 `node --test tests/test-multiplayer-guide.mjs` 均 **9/9 PASS**；新增两项只证明 authored Markdown 已披露规则，不作为 native 或长程浏览器证据，不将两轮检查简单相加冒充一次完整 suite。
- 精确暂存路径集合与 `git diff --cached --check`：PASS。三个已跟踪生成页面在验证后恢复到该提交，生成物没有混入作者文件提交。
- **未通过：**`node tests/test-th11-multiplayer-policy.mjs` 在该隔离基线上导入失败，尚未执行断言或 Relay 场景，原始错误为 `Error: invalid workspace repository path: th15`。基线产品 Catalog 已包含 TH15，但已提交的 `config/workspace.json` 没有 `repositories.th15`；`lib/workspace-layout.mjs` 验证所有产品路径时拒绝。没有复制别人未提交的 TH15 配置，也没有放宽校验隐藏失败。

### Launcher 原工作树的补充证据与保护

以下仅适用于 `D:\workspace\eagler\eagler-touhou` 当时包含既有并发改动的工作树，不能转写成上述干净基线的全量 PASS：

- `node tests/test-th11-multiplayer-policy.mjs`：PASS，覆盖六机体 / 五难度、2P / 3P、pure-only / challenge 拒绝、跨作品 EBTM / EBNP 过滤，本机真实房间 Relay 的 2P 自动 / 3P 手动配置与结果提交；不加载游戏引擎。
- `test-netplay-measured-timing`、`test-runtime-build-profiles`、`test-adapter-capabilities`：PASS。既有 Catalog 旧断言未包含别人的 TH15；旧 Relay product-policy 的 lobby URL 缺当前 member 凭证，连接关闭。这些既存差异没有写成 TH11 PASS。
- `npm run build:ui` 与 `C:\Python314\python.exe tests/test-th11-multiplayer-launcher-browser.py`：PASS。真实 Framework 页面、localhost **房间 Relay** 与明确标注的 **synthetic Runtime / DATA**；覆盖 2P Normal 自动 D、3P Extra 手动 D=9、六机体切换、pure-only UI、预登记观战和当前 Watch Replay 入口。报告 `eagler-touhou/.cache/th11mp-launcher-browser/result.json` 明确 `nativeRuntime:false`、`retailData:false`，Replay 入口亦为 `nativePlayback:false`，不是生产 WASM、游戏输入链路或 Replay 确定性证据。报告 SHA256 为 `d48943fdf61feeb5feabef9e5be47e5fa4abf227041ee6dd38847e3bf8f84d40`。

修改前逐文件快照和本轮 patch 分别位于 `eagler-touhou/.cache/th11mp-launcher-snapshots.json`、`.cache/th11mp-launcher-own.patch`。最终 patch 为 91,210 bytes，SHA256 为 `3c5cee296646ae9182c97aac08d15acec2ec7c1b6136dae25180939cc7fa053e`。并发工作将原 Launcher HEAD 从 `039b6e0` 推进至 `fed02bf` 后，仅按实际上下文移植本轮语义；21 个路径与快照集合完全相等，未带入 TH15、Relay 流控重构或 prank 的其他改动。原始 before 快照保持不变，reverse-apply check PASS，原 index 和其他改动保留；最后一次文案补充没有修改两份工作树的生成页面。

## 8. 构建与本机验证入口

在 `D:\workspace\eagler\worktrees\th11-multiplayer`、`eagler-mp` 分支执行。共享库由 gitlink 和构建 / 打包脚本同时固定到第 1 节的完整提交号，不能使用兄弟目录最新 HEAD 替代。

```powershell
$env:EMSDK = 'D:/workspace/eagler/toolchains/emsdk'
$env:EM_CONFIG = 'D:/workspace/eagler/toolchains/emsdk/.emscripten'
$env:EAGLER_FONT_ROOT = 'D:/workspace/eagler/th11-eagler/build-eagler/fonts'
node portable/build.mjs --multiplayer
node portable/package-eagler.mjs --multiplayer
```

MP 产物在 `th11_web/artifacts/multiplayer/`，可交付 Runtime 闭包在 `build-eagler-multiplayer/`。普通版使用 `node portable/build.mjs --thprac` 与不带 `--multiplayer` 的 package 命令，分别输出 `th11_web/artifacts/sdl3/` 和 `build-eagler/`。MP 明确拒绝 THPrac。私有游戏 DATA、字体、编译缓存和产物均不提交 Git。

```powershell
node --test portable/multiplayer/package-isolation.test.mjs
node portable/multiplayer/network-test.mjs
node portable/multiplayer/replay-capacity-test.mjs
node portable/multiplayer/native-test.mjs
node portable/multiplayer/native-test.mjs --repair
node portable/multiplayer/browser-acceptance.mjs --players 2 --route rtc --auto --loadouts 0,5 --frames 240 --disconnect
node portable/multiplayer/browser-acceptance.mjs --players 3 --route relay --loadouts 1,3,4 --frames 360 --spectator --spectator-lag-frames 150
node portable/multiplayer/browser-acceptance.mjs --players 2 --route relay --loadouts 2,5 --frames 240 --restart --spectator --drop-first-input --full-replay-slots
node portable/multiplayer/browser-acceptance.mjs --players 3 --route rtc --loadouts 0,3,5 --frames 240
```

浏览器脚本使用真实生产包、原作 `th11.dat`、当前 Launcher 的共享 Relay 和 Chromium，Host 测试夹具只使用现有命令契约并观察 native 状态。默认校验已确认输入的 MP Replay 和向后定位；`--restart` 通过 P1 原生暂停 / R 进入新 generation，旧 Replay / 观战结束旧流；`--drop-first-input` 使用 Relay 既有测试开关；`--disconnect` 关闭一个真实浏览器上下文检查可见错误。`--full-replay-slots` 在启动前通过真正 Host 校验导入同轮 2P RTC 导出的 Replay，占满 99 个编号槽和 ud0000，验证原有 100 份文件不变、新旧代分开保存与重复 flush 复用路径；它依赖前一条 RTC 命令导出的同构建 `.rpy`，也可用 `--existing-replay` 显式指定。脚本不注入原生世界状态，不执行原作 EXE，也不替代真实设备 / 公网验收。命令清单是复现入口，实际通过状态以第 7 节和对应报告为准。

## 9. 首轮 NIG / UI 审查记录（f5b444d，设计结论已由第 10 节纠正）

**历史记录：** 本节的测试能证明当时的实现运行情况，不能证明产品语义正确。原网页 Replay 列表 / 工具条、逐席 HiScore / Score 和自造 Player Data 结果页违背本次任务的参考要求，已经按第 10 节撤下。原生素材或截图一致都不能为这些偏差提供依据。

### 9.1 后续 agent 实际改了什么

再次进入 NIG 时，TH11 MP 仍为干净的 `2bd03f0`；Launcher 隔离接入仍为干净的 `0409d7f`。原 Launcher 工作树中，本任务的 21 个作者文件与此前 `.cache/th11mp-launcher-snapshots.json` 的 after 快照逐字相同，没有被后续工作覆盖。

后续提交在另一条 Launcher main：`7d4408926a5c88b72ead7496b75a6433e8516911` 移除昼夜模式、快捷语音及 CI，`2836d4b2f90f07166928d683ad9dbf08ce39763b` 更新 TH15 THPrac / thcrap 等行为。它们不是 TH11 MP 的后续实现。main 与 React `experiment/ui-main` 以 `9899dff` 为共同祖先，分别有 25 / 48 个独有提交；main 仍使用旧 Launcher 页面结构，没有本任务的 React 房间接入。不能把两个分支当作前后版本，或直接将 21 文件补丁整包套到 main。

main 已有 TH15 workspace 映射，来源 `29accba`，但这不消除旧 `fed02bf` 隔离接入的前置缺口，也不构成在 main 上运行 TH11 MP policy 的 PASS。本轮保留别人的脏文件和两条 Launcher 分支，没有 merge、push 或部署。canonical `eagler-common/main` 保持干净的 `cedb710`，只将非当前 `th11-multiplayer` 分支本地 fast-forward 到 `e02347f`。

### 9.2 先看实图，再修 UI

本轮先捕捉旧正式包的连接、战斗、暂停、Replay 列表 / 播放 / 定位和断线界面，再用原作资源、真实 SDL / GLES / WebGL 渲染器捕捉普通暂停 / Replay 参考与多人终局。截图位于 `artifacts/multiplayer-ui/20261009/{before,after}/`，没有使用原作 EXE，也没有重写 golden。

| 流程 | 本轮看到的问题与处理 | 接受证据 |
| --- | --- | --- |
| 1. 连接测量 | 原先独立蓝色圆角通知改为暗底、白字的简洁状态提示；仍等待实际游戏链路完成测量。 | `after/browser-ui-connection-desktop.png`；正式包 3P / RTC 新轮次均实际完成 129 次测量。 |
| 2. 战斗 HUD | 保留 TH11 原生标签、数字、生命图标、Power 制式和三席排列；没有把合作进度移入右侧 HUD。 | `after/mp-3p-local1.png` 至 `local3.png`；这三张诊断图与 before 字节相同。 |
| 3. 暂停与恢复 | 旧版只有暂停标题。现在使用原生 text 75 抓屏背景、front 89 装饰 / 标题 / Resume 与原作动画。独立呈现 owner 保持 1x，combat / RNG 仍冻结。P1 新确认或任意席位 Pause 可恢复，R 仍属于协议 owner。抓屏 helper 按 PauseMenu 的真实 owner 找 VM。 | `after/mp-pause.png`、正式包 `after/browser-ui-pause-desktop.png`；native pause / RNG / 输入边沿断言。 |
| 4. 残机给予与救援 | 补上机体附近原生数字贴图百分比；幽灵随已确认的操作进度变亮。90 帧资源 / 复活边界保持，成功使用原生 Extend 音效。Power 仍从第 3 次点按显示原作道具及数字。 | `after/life-gift-30.png`、`60.png`、`89.png`；`after/ghost-rescue-45.png`、`89.png`、`90.png`；`after/power-tap-3.png` 至 `5.png`。 |
| 5. Game Over / Extra / Ending 结果 | 当时将 Ending 后 Name Regist 换成 Player Data、撤下 P1 机体铭牌并显示各席行内机体；这些设计已在第 10 节纠正。原记录所谓“十分制的 00”措辞也不准确：原生排名格式是 stored units 后附一位 Continue 标记，MP 为 0。 | `after/game-over.png`、`extra-result.png`、`ending-title-results.png` 为历史图。终局图来自明确的 StageExit UI 接点；Ending / Staff 跑实际解释器，不能当完整通关证据。 |
| 6. Replay 列表与播放 | 旧表格在 390 像素宽度截掉信息，工具条遮挡游戏底部。新增 MP 专用样式：白字暗底、黄色选择、等宽表格、窄屏分行，控件预留独立空间且保持游戏 4:3。菜单保留 / 恢复文件焦点；方向键 / Z / Enter 可选播，Tab 与进度条键盘操作不再被全局吞掉。 | `after/browser-ui-replay-list-{desktop,portrait}.png`、`browser-ui-replay-seek-{desktop,portrait}.png`，以及中文 `runtime-zh-*` 对应图片；实际 DOM 布局和键盘断言。 |
| 7. 断线与返回 | 旧界面直接铺满调用栈。现在明确说明本局停止，错误详情折叠，返回房间按钮有默认焦点；停止后不再接受 gameplay 输入。 | `after/browser-ui-disconnect-desktop.png`、`after/runtime-zh-ui-disconnect-portrait.png`；实际关闭另一个浏览器上下文触发故障，没有伪造 disconnect 标记。 |

游戏内菜单 / HUD / 给予反馈直接使用原作资源；多人录像列表和播放控件仍是 Runtime 的网页界面。这里的结论是风格与交互经过实图协调，不宣称新增网页控件使用了原版菜单的全部贴图。桌面检查为 660×510 外层与 640×480 Runtime；竖屏检查为 390×844 Runtime。没有把窗口改尺寸称作真机触控验收。

原生诊断 after 共 18 张 MP 图；相同输入安排下 17 项权威 hash 前后相同，仅新增原生菜单状态的 Pause 不同，11 张非本轮修改目标 PNG 完全相同。普通两张视觉参考保留在 before；本轮 after 诊断使用 `--mp-only`，没有混入普通 THPrac 缓存。完整记录见 `after/{build-evidence,screenshot-evidence,comparison}.json`。

### 9.3 高危故障：三人 R 重开时误拒绝旧测量广播

旧正式 WASM `d60aa357…` 在本轮 `3P Relay → 暂停稳定 18 ticks → 横 / 竖屏截图 → P1 R` 中真实失败，错误为 `Gameplay packet before calibration commit or excessive deferred HELLOs`。失败报告保留在 `before/browser.json`，不能用之前四组短程 PASS 抹掉这个反例。

共同层 `SessionChannel` 会广播带目标席位的 Phase 样本；正常接收允许非目标席位忽略这种包。但退休包识别曾要求目标必须恰好是本机，导致三人房里发给第三席位的合法旧轮次尾包，在新轮次测量时被错误拒绝。新增回归在旧实现明确 exit 1；修复只允许已授权紧邻退休代内、来源合法且目标在本房间席位范围内的广播被丢弃。当前代 / 外来代、缺退休上下文、自发送者、非法目标继续拒绝。

修复提交为共同层 `e02347fac98c9e599d30a6ffd7ef30756555b948`，仅改 `AdonisConnection.hpp` 和退休回归测试；TH11 gitlink、构建、打包与 package 测试都固定到该提交。没有在 TH11 私设另一套协议，没有放宽当前局 gameplay 入口，没有增加预测或回滚。

同一套生产浏览器时序在新正式包重新通过：新代三人再次完成 129 次实际通道测量，272 个同帧权威状态一致；143 个旧观战同帧比较一致，旧观战按正常 EOF 结束；41 个 Replay 比较与从帧零回退到 114 的定位一致。共同层反例与正式浏览器复现均保留。

### 9.4 本轮验证与产物身份

| 检查 | 实际结果与范围 |
| --- | --- |
| 共同协议定向回归 | 先旧实现失败，再新实现 5/5 CTest PASS：startup、timing、session-channel、calibration-retirement、packet-transaction；包含全部三席正例和 8 类拒绝条件。日志在 nested common 的 `artifacts/th11-phase-retirement-20261009/{before,after}.log`。 |
| Native UI / 终局 / 重建 | `node portable/multiplayer/native-test.mjs --risk`：1736 checks / 3312 logic+draw ticks PASS；原生 Pause、combat / RNG 冻结、给予 / 救援、Game Over / Extra / Ending、暂停后新旧种子重建。日志 `artifacts/multiplayer-native/native-ui-risk.log`。 |
| 真实 GPU UI | before 20 张、after 18 张 MP 图已逐图检查；diagnostic WASM `18ed7de9d0978d9ee5f0cca8cdba368cf3a2149dae816a30077227e932001308`。它不是生产 Runtime，也不证明网络或完整关卡。 |
| 生产 3P Relay | `after/browser.json` PASS：272 同帧、41 Replay、143 旧观战同帧、回退定位 114、P0、两代实测启动、16 张界面图、方向键选播 / Z / Tab / range 键盘定位及无横向裁切 / 无控件遮挡。 |
| 生产 2P RTC 中文 | 最终 `after/runtime-zh-final.json` PASS：自动 D=1 / P0，124 同帧、42 Replay、回退定位 133，12 张桌面 / 竖屏图及相同键盘 / 布局断言；Esc 返回 Replay 列表后恢复所选文件焦点；实际断线后点击返回按钮完成 Host exit，房间 WebSocket 和本人成员身份均保留。此前 `runtime-zh.json` 的 104 / 38 比较也通过，但不与最后一次简单相加作为一次测试的覆盖量。 |
| 普通模式回归 | 重新构建当前 WASI 诊断 core；4 份 Demo 共 18,049 ticks 的 economy / player / RNG / entities 与既有不可变 golden 完全一致。报告 `artifacts/replay-verifier/ui-followup-quick-result.json`。没有执行原作 EXE 或修改 golden。 |
| 普通 / MP 闭包 | 两种正式 Emscripten 构建和打包均 PASS；普通 18 个文件，MP 因专用 `multiplayer.css` 增为 22 个文件；普通包不包含该 MP 样式和 MP 模块。package isolation 7/7 PASS。 |
| Launcher 当前房间 UI | 重新捕捉并检查 2P Normal 自动延迟 / 3P Extra D9 的 4 张图，pure-only、无 challenge、启动 / 观战 / Replay 交接 PASS。报告 `eagler-touhou/.cache/th11mp-launcher-browser-followup-20261009-062902/result.json`。仍是 Framework + 真房间 Relay + synthetic Runtime / DATA，不能升级为真实游戏联合验收。 |

该历史轮次的正式 MP WASM 为 **`868022aabce50c7e768a83821ad498ef87b44667ee4df6fdfbc7b21da7094e0c`**，2,748,527 bytes；普通 `--thprac` WASM 为 **`f27bb5786d0dfc172efcd59fe9d7431ef02412261e0acb29be0954bca0b04fef`**，3,003,973 bytes。它们不作为第 10 节修正版的产物身份。闭包目录仍为 `build-eagler-multiplayer/` 与 `build-eagler/`，不会把诊断导出、原作 DATA 或源码快照塞进正式包。

### 9.5 本轮必须继续告知的高危边界

1. **暂停恢复时序 / 旧 Replay 兼容性：** 原生退出动画约需 12 个已确认输入帧，期间不推进战斗。当前构建的网络 / Replay / seek 已验证；旧 `2bd03f0` 的暂停恢复输入历史没有这段新增菜单时序。现有 WASM 指纹检查会拒绝跨构建 MP Replay，本轮没有迁移器，不承诺旧实验录像在新版播放。
2. **Launcher 集成主线：** TH11 MP 接入在 React 工作分支和干净的 `experiment/th11-multiplayer` 提交中；最新 main 尚无这套 TH11 MP 接入，而且页面结构分叉。本轮没有将“本地实现完成”写成“main 已合并 / 线上已提供”。
3. **第 5 / 6 节临时玩法仍未获多人平衡确认：** 各机体一档 Power 交换而非 raw 点数守恒、普通 P/F 优先未满者、280 帧不消弹救援、共享碎片全队 Extend、死亡 / 重生过渡仍计入 Boss 分母与全满条件、途中目标变幽灵的两种不同赠送处理、最近 Alive 瞄准及全体 Alive 特殊吸引都保持已披露实现。本轮 UI 验证不把这些决定变成既定共识。
4. **重开 / 容量 / 观战边界：** R 仍换种子并重新测量；观战和录像停在旧代，不自动加入新代。250,000 确认帧上限、包含暂停 / Ending、达到后结束并保存 partial、99 槽后选择空闲 udXXXX、真实 I/O 失败显式报错等策略未改变。
5. **验证范围：** 真实网络检查为本机 Chromium RTC / Relay 的短局；原生终局截图用了注明的 UI 接点，救援诊断固定幽灵漂移。没有完整多人战役 / Extra 真人通关、长程 seek、69 分钟实跑、Android / iOS 真机或公网表现结论。

### 9.6 复现 UI 检查

以下保留 f5b444d 轮次使用的历史命令。当前修正版使用第 10.6 节的 `native-correction` 目录，不再覆盖这里的 `before` / `after` 证据：

```powershell
node portable/multiplayer/native-test.mjs --risk
node portable/multiplayer/native-ui-build.mjs --phase after --mp-only
node portable/multiplayer/native-ui-capture.mjs --phase after --mp-only
node portable/multiplayer/browser-acceptance.mjs --players 3 --route relay --loadouts 0,3,5 --frames 360 --restart --spectator --disconnect --ui-audit --verify-ui --output artifacts/multiplayer-ui/20261009/after/browser.json
node portable/multiplayer/browser-acceptance.mjs --players 2 --route rtc --auto --loadouts 0,5 --frames 240 --disconnect --ui-audit --verify-ui --language lang_zh-hans --output artifacts/multiplayer-ui/20261009/after/runtime-zh-final.json
```

`native-ui-README.md` 说明诊断接点和 cache 宏签名；诊断导出使用 package gate 已拒绝的 `mp_fixture_ui_*` 前缀。`--phase` 只是产物目录名，不能把当前源码输出重新标成旧 before。上述旧版 before / after 证据均保留。这里提到的范围控件等是旧测试能力；当前原生菜单验收已经移除对应网页交互，见第 10 节。

## 10. 原生 Replay、共享分数与原生结算修正

### 10.1 承认并纠正原来的设计错误

本轮从 NIG 中干净的 `eagler-mp` / `f5b444d98204def84bea24591a26a328ee558950` 继续。用户指出“Replay 播放原版就有”“每个人弄个分数”后，再次逐项比对第 1 节两个实际 MP ref 及 TH11 原生状态机。网页播放器、逐席分数池和自造 Player Data 成绩页并非未定义行为的合理补足，而是错误应用参考；第 9 节对这些设计的接受结论撤回。

| 实际参考 | 已核对源码与行为 | TH11 当前处理 |
| --- | --- | --- |
| TH08 MP `b5b4091` | `multiplayer/PlayerResources.cpp` 将 score / display / high 绑定共享 GameGlobals；`game/GuiView.cpp` 只绘制一组原位分数。 | 各席引用同一个团队 score owner；HUD 原位绘制一组 HiScore / Score。 |
| TH10 MP `288ea8d` | `multiplayer/EconomyView.hpp` / `platform/GameState.hpp` 的个人视图引用 TeamEconomy；`game/GuiScore.cpp` / `GuiDraw.cpp` 保留共享分数与个人资源分工。 | 命、Power、擦弹、通信率及点值仍按席位，击破与符卡分数写共享 owner 一次。 |
| TH08 原生 Replay 接入 | `game/TitleReplays.cpp` 把 MP Archive preview 投射到原生元数据和 ANM 文件 / 关卡菜单。 | 复用 TH11 `TitleScreen::Replays`、ANM 92 / 99、原始列表、光标、转场及原生关卡选择。 |
| TH10 原生 Replay 接入 | `platform/ReplayFiles.cpp::ReplayDocument::load` 加载 MP preview；`game/TitleReplays.cpp` 使用原生目录 / 关卡菜单。没有每章分数快照时只显示章节名称。 | 没有快照的 MP 章节不伪造分数；JS 不另建 Replay 表格、播放工具条、暂停控件或任意拖动滑杆。 |
| 原生结果流程 | 原作与参考使用排名 / 录名及 Replay 槽位 / 录名流程。 | 恢复 Game Over、Extra、Ending 后的原生流程；只去除明确禁止的 Continue / Retry。 |

Launcher `content/MULTIPLAYER.md` 的“解锁状态同步”明确只有 Replay 可保存。因此原生高分录名 / 排名状态只留本局内存，重开或下一局重新初始化；不会落盘 `scoreth11.dat` 或解锁。不能照搬旧参考中与此规则不符的持久化行为，也不能声称这里恢复了多人高分存档。

### 10.2 共享计分与终局奖励

`GameEconomy.hpp/.cpp` 在 `TH11_MULTIPLAYER` 下将每席 `score_units` 永久绑定到 session 的主 owner，复制资源快照时保留目的对象绑定。普通构建的数据布局与单人复制行为不受该宏外改动。`GameBattleCallbacks.cpp` / `MultiplayerBattle.cpp` 删除敌人和符卡奖励的逐席广播；拾取、射击等以触发者原生规则计算后写入同一总分。不是每帧互相抄分，也不是结算时才相加。旧版敌人 / 符卡本来就广播到各个独立分数池，不能误述为“旧版 P2 完全收不到这两种奖励”。

`Hud.cpp` / `MultiplayerPresentation.cpp` 只保留原位、原尺寸的一组 HiScore / Score；按席位重复的只有必要个人资源，继续使用原生纹理、字体和图标。`MultiplayerMenus.cpp`、`PauseEnd.cpp`、`TitleResults.cpp` / `TitleText.cpp` 撤下逐席 Player Data，接回原作排名录名和 Replay 保存；共享成绩使用 P1 机体分类，菜单仍消费已确认的权威输入。

**高危：多人 all-clear 的个人点值聚合没有明确规则。本轮采用“关卡基础奖励只加一次，每个席位按自己的原生点值、剩余命及 Power 计算个人部分，再按席位顺序各加一次”。** 通信率、点值仍为个人状态，不能用 P1 数值代替所有席位，也不能将已共享的 stage 基础项重复加 N 次。幽灵的负残机 / Power 按 0 计个人剩余资源；普通十分制、原生整数处理及总分上限保留。这是明确的临时经济规则，短程一致性通过不代表多人全通关平衡已经确认。

原生固定输入检查包含：三席射击合计 3 stored units、敌人 137 points 仅加 13 stored units、P2 点道具按自身通信率 / 擦弹贡献 50,500 points、符卡 12,340 points 只加一次、999,999,999 stored units 封顶。固定 StageCompletion 向量得到 stage 1 的 4,000,000、Normal stage 6 的 280,200,000、Extra 的 353,200,000 points；另用三组普通期望值限制单人回归。这些是规则断言，不是实际通关成绩。

### 10.3 Replay 目录、播放与保存

`Application.cpp` 新增原生菜单入口与只读状态接口 `th11_mp_replay_menu` / `th11_mp_replay_ui_status`。Runtime 的 Replay 入口直接运行原生菜单循环；`ReplayMenu.hpp` 按原作每页 25 行翻页，覆盖 01–99 与已存在的全部合法 ud 文件，不再把 26–99 或后续 ud 录像藏起来。扫描保留路径和元数据，释放完整输入数据；真正开始播放时重新读取并完整验证文件及本次注册的构建身份。

原生标题栏沿用 P1 机体字段，尾部用原生字体附加人数与录制席位。选择章节从帧零重建到实际录制章节点；不伪造章节，不增加预测、回滚或章节快照。播放期间物理移动 / Bomb 不影响录制世界；物理 Esc 返回原生 Replay 目录并保留页码 / 选项，录制 EOF 也回目录。目录 Back 完成原生退出转场后使用既有 Host exit 返回。EOF 自动回目录沿用 TH08 路径；没有发明一个新的结束播放器页面。

本节修正当时网页层仍保留连接 / 错误 / 观战终局等 Runtime 状态，以及定位章节时的只读进度提示；前者现已按第 11 节全部交给标准 Launcher。删除旧 `th11-mp-playback`、Replay table / toolbar 和为工具条缩短 canvas 的 CSS。游戏始终按原生 4:3 画面呈现；窄屏仍缩放原生界面，没有另造手机版文件表。

`Application.cpp` 服务原生 Title / Pause 的扫描、元数据及保存请求。保存在本帧已 `MarkSimulated` 并加入 Archive 之后执行，所以选择保存的确认帧包含在文件中。Replay / 观战只重放菜单输入，不执行写文件。Archive 的成绩字段读取唯一共享 score。

自动备份若与玩家手动选择的编号同槽，手动保存后清除自动路径；下一次自动 flush 重新选空槽。这样玩家录入的名字和文件内容不会被之后的 `PLAYER` 自动备份覆盖。原有 99 槽后 ud 扩展策略不变。实际 I/O 验证见下节。

### 10.4 当前证据与可见画面

本轮证据统一位于 `artifacts/multiplayer-ui/20261009/native-correction/`，没有覆盖第 9 节旧图。生产网络测试始终使用下节同一个 MP WASM；GPU 菜单 / 存档诊断单独构建并清楚标记受控 score / StageExit 接点，不当作真实联机通关证据。

| 验证 | 当前修正版的实际结果 |
| --- | --- |
| Native score / result / 既有高危规则 | `node portable/multiplayer/native-test.mjs --risk`：**1,768 checks / 3,622 logic+draw ticks PASS**。覆盖共享 score、不重复奖励、三类终局原生录名 / 保存请求、P1 输入、无 Continue / Retry、暂停 / 重开。日志 `artifacts/multiplayer-native/native-score-correction.log`。 |
| 普通 Demo 回归 | 重新构建 WASI candidate 后，4 / 4 段既有 Demo、**18,049 ticks** 的 economy / player / RNG / entities 与不可变 quick golden 全部一致。报告 `artifacts/replay-verifier/native-correction-quick-result.json`。没有运行原作 EXE 或重写 golden。 |
| 2P RTC 生产包 | `native-correction/2p-rtc.json` PASS：自动 D=1 / P=0，**123 同帧世界比较、38 Replay 比较**；从帧零回建至 119 匹配。22 张桌面 / 竖屏流程截图；真实断线后返回且保留房间成员身份。 |
| 3P Relay 生产包 | `native-correction/3p-relay.json` PASS：D=2 / P=0，**222 同帧世界比较、33 Replay 比较、124 旧局观战比较**；同步暂停 / R 后新代重新实测，旧观战正常结束；从帧零回建至 106 匹配。26 张桌面 / 竖屏流程截图；真实断线返回且保留房间身份。 |
| 原生 Replay 交互 / 拒绝条件 | 两组生产测试都用真实浏览器键盘进入原生目录 / 关卡 / 播放，翻到 slot 26，验证物理 gameplay 输入隔离、Esc 保留选项、EOF 回目录、目录 Back → Host exit。异构建且结构合法的文件与损坏文件都被导入和原生扫描拒绝。只选择实际录到的第一关；没有用合成章节冒充后续关卡验收。 |
| 原生 GPU / 实际 App 保存 | 29 张 MP 诊断 PNG；Game Over、Extra、Ending 各包含 live / readOnly，共 **6 条菜单 / 文件流程**通过。原生 Archive 解码核对共享 score、姓名 A、席位、完成标记和包含保存确认帧；只读文件字节不变、`scoreth11.dat` 不存在。手动占用自动 01 后，新自动选择 02，原手动 01 字节不变。报告 `native-correction/native-save-evidence.json` / `screenshot-evidence.json`。 |
| 构建 / 产物隔离 | 普通 `--thprac` 与 MP `--multiplayer` 都已构建、打包通过；闭包 18 / 22 文件。package isolation **7 / 7 PASS**，含实际 WASM export gate；普通包没有 MP 模块 / CSS，诊断 `mp_fixture_ui_*` 不进入生产。 |

向后定位通过原有 native Replay seek ABI 和测试观察器验证从帧零重建，仅属于诊断调用；报告明确 `userFacingSlider: false`，不能把这项检查写成玩家已拥有网页进度条。两组报告均 `passed: true`、`errors: []`。3P 用 `lang_zh-hans` Runtime 配置；其原生文字取决于加载的资源包，不能据此声称完整中文资源或真机触控均已验收。

实际复核的主要图包括 `mp-3p-local1.png`、`native-game-over-ranking.png`、`native-game-over-replay-slots.png`、`native-extra-actions.png`、`native-ending-ranking.png`、`native-ending-replay-name.png`，以及生产 `2p-rtc-ui-replay-list-desktop-canvas.png`、`2p-rtc-ui-replay-stages-desktop-canvas.png`、`3p-relay-ui-replay-list-portrait.png`。现在核对的是原生单份分数与原生菜单结构；不再用“颜色 / 字体像原作”接受本来就不应额外存在的网页播放器或逐人成绩页。诊断注入固定大分数时同步原生 HUD 的显示计数，避免把从零骤增的人工滚分滞后当成实际结果画面。最终诊断 WASM 为 `8bfb5ef4f7c8b0d253e6ebfee54313cbfc3efeb852c537e62714937a91998bd7`，不作为生产 Runtime。

### 10.5 产物身份与仍须报告的高危边界

本节历史生产 MP WASM：**`dfac7d70905e511397ae78cdd6afb8c1b6e18168dfcf19816f2b7d78d6cd5281`**，2,790,783 bytes，已由第 11 节产物替代。普通 `--thprac`：**`ad7c30dbb8ea3836b3ba5ec8c4c8088337508025f087d16bd31a89aedd8336fa`**，3,003,964 bytes。MP 仍固定 common `e02347fac98c9e599d30a6ffd7ef30756555b948`，`P=0`、不预测、不回滚。以下保留当时边界；共享碎片现已确认，主线接入与当前验收以第 11 节为准。

1. **旧实验 Replay 不兼容：** `f5b444d` / `2bd03f0` 的独立计分与菜单输入序列已经改变；沿用精确 WASM 构建指纹拒绝旧构建，不提供迁移或偷偷按新规则播放。保存旧录制所对应的 Runtime 才能继续使用该旧录制。
2. **终局经济仍有临时规则：** 第 10.2 节的全队 all-clear 点值 / 个人资源聚合会影响总分及平衡。它已明确实施和验证固定向量，但尚未成为已获产品确认的规则。
3. **既有未定义规则不因 UI 修正获得确认：** 第 5 / 6 节的一档 Power 跨机体交换、280 帧救援、目标 / 掉落选择、途中赠送、重开新种子与 250,000 确认帧容量策略保持，并继续标为高危。共享碎片 Extend 后来已由用户明确确认，不再列为未定规则。
4. **证据仍有范围：** 当前是真实生产 WASM 的本机 Chromium 短程 RTC / Relay、原生状态机和实际文件 I/O；没有完整多人战役 / Extra 通关、公网 / Android / iOS、长程 seek 或 69 分钟上限实跑结论。终局诊断的人工 score / StageExit 接点只证明菜单、呈现与 App 保存 owner。
5. **接入与发布边界：** 代码保留在 `eagler-mp`；Launcher 仍为第 9.5 节的独立 React 接入，不能声称旧 main 已合并或线上可用。本轮不 push、merge canonical 或部署。

### 10.6 当前修正版复现入口

使用第 8 节指定的 Emscripten / WASI / 字体环境，从 TH11 MP worktree 运行：

```powershell
node portable/multiplayer/native-test.mjs --risk
node portable/build.mjs --multiplayer
node portable/build.mjs --thprac
node portable/package-eagler.mjs --multiplayer
node portable/package-eagler.mjs
node --test portable/multiplayer/package-isolation.test.mjs
node th11_web/scripts/cpp/build.mjs
node tools/replay-verifier/capture-candidate.mjs --lane quick --output artifacts/replay-verifier/native-correction-candidate
node tools/replay-verifier/run-gate.mjs --lane quick --capture-root artifacts/replay-verifier/native-correction-candidate --golden-root tools/replay-verifier/golden --report artifacts/replay-verifier/native-correction-quick-result.json
node portable/multiplayer/native-ui-build.mjs --phase native-correction --mp-only
node portable/multiplayer/native-ui-capture.mjs --phase native-correction --mp-only
node portable/multiplayer/browser-acceptance.mjs --players 2 --route rtc --auto --loadouts 0,5 --frames 240 --disconnect --ui-audit --verify-ui --output artifacts/multiplayer-ui/20261009/native-correction/2p-rtc.json
node portable/multiplayer/browser-acceptance.mjs --players 3 --route relay --loadouts 0,3,5 --frames 360 --restart --spectator --disconnect --ui-audit --verify-ui --language lang_zh-hans --output artifacts/multiplayer-ui/20261009/native-correction/3p-relay.json
```

生产 builds / packages、诊断 UI 接点和浏览器 Host fixture 各有明确边界。首次做某个比较前必须核对对应源码与产物身份；不能混用普通 THPrac cache、旧 MP wasm 或把 fixture 导出加入生产来凑验收。

## 11. 本机 HUD、碎片一倍与标准 Launcher main 接入

### 11.1 用户本轮明确的规则

本轮从 TH11 `eagler-mp` 的 `92f44036a42fe67a457d6057c2ba96e5f8d43e93` 继续。用户再次指定原生界面和标准联机的所有权，不能把已有行为当作未定义项自行设计。

| 已确认要求 | 当前实现 |
| --- | --- |
| 共同集合只显示生命与 Power | 原生 Life / Power 标签、星形生命与碎片图标、数字纹理保持；各席不再重复 Graze、点值和通信信息。 |
| 1P / 2P / 3P 在各组开头，本机用贴合原作的黄色 | 原生字体在各组左上绘制席位号，本机色值 `0xffffff00`，其他席位白色，幽灵仍遵守原有透明度。 |
| 其他个人信息在原位置，只显示本机 | Graze 回到原生右侧行；点值、通信倍率和通信图形回到原生左下。选择本机视角只改变呈现读取，不推进或改写共享世界。共享 HiScore / Score 仍只有原位一份。 |
| 全队共享碎片，任何人数掉落均为原作一倍 | 2P / 3P 的主掉落和计数掉落都移除碎片倍率。每 5 片只触发一次全队奖励，各参战席位各加 1 命，cap 9，幽灵不自动复活；个人计数不再次 Extend。普通 P 仍为 2P 一倍、3P 两倍。 |
| 连接 UI 由标准 eagler-touhou 前端负责 | Runtime 不再创建测量通知、错误面板或终局返回按钮；使用已有 `runtime-info`、`notice`、`error`、`exit` 和共享传输诊断。 |
| 标准联机实际跑通，先不做预测回滚 | 接实际 Launcher main 的房间、时序契约、Relay、观战、Runtime 入口和原生 Replay。全链路仍限制纯延迟 `P=0`，没有新增协议或假 peer 状态。真实联合验收与诊断证据分开记录。 |

HUD 的原生 Life 行顶部为 104 / 208 / 280，标题在各自顶部前 16 像素，保留 Graze 原生 y=152。各席通信动画仍各自以同一时钟更新，仅选本机副本绘制；玩家遮住左下区域时沿用原生隐藏 / 淡出响应。这些呈现状态不替换权威 economy。

### 11.2 实际主线工作区与产品隔离

标准 Launcher 目标为 `D:\workspace\eagler\worktrees\launcher-quick-index`，分支 **main**，本轮基线 `2836d4b2f90f07166928d683ad9dbf08ce39763b`。它使用 `src/launcher/app.mts` 的既有 DOM / TS 前端，不是第 9 / 10 节的 React 工作分支。开始时已有的 `design-qa.md` 修改保持原样，不纳入本轮提交。canonical React 工作树与旧 `experiment/th11-multiplayer` 接入均未迁移、覆盖或合并。

再次核对 main 的 multiplayer、runtime-package、Adonis playbook 和行为不变量。产品目录声明 `th11mp`、六机体、2 / 3 人、五档难度、`rollbackLimit: 0`、手动 D 上限 9、实测启动；UI、房间请求、Runtime options、Relay 和观战 timing 都解释同一份能力声明。TH11 沿用既有 cooperative 房间的挑战模式开关，默认关闭，P1 可开启，更改规则撤销所有 Ready。此前自行增加的 `challengeMode: false` 及挑战能力豁免已撤回；用户只排除了预测 / 回滚，没有授权排除挑战模式。具体实现和验收见第 11.7 节。已有作品省略时序限制时沿用原行为，不靠 TH11 专用分支绕过共享契约。

普通 `th11` 仍指向 canonical `th11-eagler/build-eagler`；新增 `th11mp` workspace 映射到 `worktrees/th11-multiplayer/build-eagler-multiplayer`。构建配置增加通用的变体级 `workspaceRepository`，必须引用已声明 workspace，防止 MP 构建计划误从普通分支执行。DATA layout 与普通 TH11 保持一致，存档、Replay、Runtime 身份仍隔离。

### 11.3 标准前端的连接与退出所有权

公共 `createAdonisCalibration` 继续通过既有 `runtime-info.netplayTiming` 上报等待、稳定、测量、协商、就绪等状态。Launcher 接收后显示已有测量界面和结果；Runtime 不再渲染同一套连接 UI。测量值依旧由实际 native / common 输入链路产生，Launcher 不根据房间 ping 推算 D。

Runtime 只公开真实 common peer transport 与原生失败、帧、路径诊断。主线现有 `#netplayConnectionWindow` 同时观察传输错误和 native fatal；断线 / 原生停止后的返回动作由这个标准前端处理。用户界面显示简短停止说明，详细错误留在既有诊断 / error 事件。旁观正常 EOF、容量结束使用已有 notice，先完成停止 / 保存 / 同步，再发 exit 回房间。

R 退休不能误触发普通 exit：玩家旧代退休且仍准备新代时只 flush 旧录像，新 generation 继续重新实测；旧观战按旧流 EOF 退出，不自动跨代。Replay 仍走原生目录、章节、播放和返回；网页层只保留章节从头重建时的只读进度提示，没有 Replay 播放工具条或自建联机窗口。

### 11.4 原生画面、资源规则与产物证据

本轮证据目录为 `artifacts/multiplayer-ui/20261009/standard-launcher/`，不覆盖旧 `native-correction` 及 before / after 记录。下表 HUD 诊断图来自补齐挑战前的同一批快照；新增挑战后的原生合并回归见第 11.7 节，生产构建身份则已更新为下面的最终包，不将旧图的 WASM 指纹改写成新包。

| 检查 | 实际结果 |
| --- | --- |
| 原生资源 / HUD / 既有规则回归 | `node portable/multiplayer/native-test.mjs --risk` 一次 **1,973 checks / 3,872 logic+draw ticks PASS**。包含 2P / 3P 碎片两种掉落入口、连续 10 片仅第 5 / 10 片分发、cap 9、幽灵不复活、P 倍率保留及本机信息原生绘制。日志 `artifacts/multiplayer-native/native-hud-resources-20261009.log`。 |
| 同一世界三种本机视角 | `mp-local-hud-{1p,2p,3p}.png` 全部 **frame 125 / hash 3535129381**。只切本机视角并绘制，未 step / begin；本机 Graze 分别为 111 / 222 / 333，原位点值 / 通信为 `050000*0.26` / `060000*0.52` / `070000*1.03`，只对应本机标题标黄。根 agent 和实现 agent 均亲看三图。 |
| GPU / 原生保存流程 | 最终 **32 PNG、0 errors，6 条 native save I/O flows PASS**；只读文件字节不变、自动与手动同槽保护、无 score 存档写入仍通过。见 `screenshot-evidence.json`、`native-save-evidence.json`。 |
| 最终生产构建与包隔离 | 补齐挑战后重新执行普通 `--thprac` 和 MP `--multiplayer` 构建；随后连同第 11.8 节启动错误回传修正打包，普通 18 文件、MP 22 文件。package isolation **7 / 7 PASS**，诊断 `mp_fixture_ui_*` 不进入生产。编译日志 `artifacts/multiplayer-challenge-final-{mp-build,ordinary-build}.log`，最终打包 / 隔离日志 `artifacts/multiplayer-final-{package-mp,package-normal,package-isolation}.log`，保留各轮旧日志不覆盖。 |
| 真实安装发现的普通包静态依赖缺口 | 官方 `package-server` 拒绝普通 shell 永假分支中的 `import('./multiplayer.mjs')`：清单没有该文件。改用真实 shell 的隔离回归先得到 **6 / 7、指定断言失败**；修正变体装配后 **7 / 7 PASS**，详见 `artifacts/multiplayer-standard-launcher-package-closure-{before,after}.log`。普通包直接装配空 factory，MP 才保留导入，不放宽 Launcher gate，也不向普通包加入 MP 文件。 |
| 标准静态闭包与最终身份 | main 原 `assertPortableRuntimeSources` 对挑战补齐后的最终 18 / 22 文件两包均通过；编译源码 288 / 241 项 hash 和 sourceDigest 全部一致，WASM / loader / 清单身份与 exports 隔离均通过，见新版 `source-identity-evidence.json`。旧闭包负向探针保留在 `static-closure-evidence.json`；旧源码身份记录保留为 `source-identity-evidence-pre-challenge.json`，不宣称本次再次执行那一旧探针。 |

当前正式 MP WASM 为 **`1037f8a836417ce23a97fd3493c61ae60b9096f44036ab1961cb66dbc94bc85e`**，**2,793,230 bytes**，构建记录时间 `2026-10-09T11:11:24.978Z`。普通 WASM 为 **`ad7c30dbb8ea3836b3ba5ec8c4c8088337508025f087d16bd31a89aedd8336fa`**，**3,003,964 bytes**，本次重新构建后仍与第 10 节已通过 18,049 Demo ticks 的普通产物字节相同；本轮不把旧 Demo 检查重复计为新执行。common 仍为 `e02347fac98c9e599d30a6ffd7ef30756555b948`，无共同层改动，canonical 普通 TH11 也保持原 HEAD 且 clean。

前一生产 MP `dbbb0500dde756c47d99f835f8a6c895c436c71cfda4a9e1ad0fee12181edacc` 的全部 22 个 Runtime 文件已校验并保存到 `artifacts/runtime-archive/<该 SHA>/`，用于保留其旧 Replay 的精确运行版本；不能用新包继续播放后又把差异归因于原始录制。

本轮 GPU 诊断 WASM 为 `069bd3f11239a9dc4cf73491714f1dafb0565a82e05dc2c2125e36187481c02e`，使用明确记录的 HUD / StageExit 接点；它不是生产包，不证明实际传输或完整关卡。真实 Launcher 联合证据在下一节单独记录。

### 11.5 真实标准 Launcher 联合验证

验收入口为 main 的 `tests/browser/test-th11mp-launcher.py`。从产品卡、标准房间创建 / 加入、准备和开始进入实际生产 TH11 MP；校验正式包全部文件及 WASM 身份，使用真实 TH11 DATA / fonts，不替换 Host Manifest 或注入合成 Runtime。只读观察器在原始 `Module.onGameFrame` 之后读取状态，不 pump、不 step、不停止或改写 native 世界。Relay 场景关闭 RTC 能力，让共享 transport 走真实 fallback；不伪造网络包。

本机 main 开发服务器默认发行目录为空，标准 Installer 按既有规则拒绝没有 descriptor 的自动 DATA fallback。验收按现有官方打包 / 导入路径准备真实游戏包，不放宽 Installer；这属于开发预览准备，不是线上发行已部署的证据。

最终官方 `package-server` 接受普通 / MP 两个闭包；`package-offline-game --without-ogg` 生成 `th11-local-package.zip`，revision `13c596945571b642`、31,717,556 bytes，最终 ZIP SHA `01cfecaadd9cab7901573337090af39a0061410f5673b5aea51464ded4564d7b`。两个 payload 为原 DATA（26,610,794 bytes、SHA `3cb521c5d420d8cbad6494d53bcdb048bcc396abf95c18fac68e38fd2d19f8e2`）和 Unicode 字体，另有 `package.json` 描述；`components={}`，没有把打包命令的通用 `--music=midi` 参数写成 TH11 已验收 MIDI。普通 DATA 与 MP 共用布局，通过原 Package Store 托管，不依赖开发 HTTP 根目录以外的未使用 DATA / 全局 TTF URL。

真实前端报告和截图单独位于 `artifacts/launcher-main-th11mp/`，不是上节 GPU 诊断目录。

| 挑战补齐前的标准前端实际链路 | 历史结果（旧 dbbb 构建） |
| --- | --- |
| 2P RTC | `2p-rtc.json` **PASS**。双端官方包导入、标准准备 / 开始、129 次实际测量、自动 D=1 / P=0；双方真实移动，**150 个同帧采样 F60–427 hash 一致**。实际关闭对端后 native 报 `RTC peer P2 ICE restart exhausted`，F662 冻结；点击标准 Return 后 member / client / room / P1 身份均保留。 |
| 2P 原生 Replay | 同一报告保存真实 `th11_01.rpy`（16,064 bytes），从标准 Launcher 进入原生目录 / 选关 / 播放，**48 个回放同帧采样 F61–276** 与该次实际运行一致；Esc 返回原生目录，再返回 Launcher。没有网页播放器、伪造章节或重写输入。 |
| 2P 画面与请求 | **12 张实际链路截图，errors=[]**；根 agent 亲看标准测量 / 停止窗口、真实战斗 HUD、原生 Replay 目录和选关。测试构建的既有前端诊断叠层按原默认保留。11 次 HTTP 404 全部是既有 TH09 卡片素材，TH11 Runtime / DATA 无请求失败；不宣称全站零 HTTP 错误。 |
| 3P Relay / 重开 / 观战 | 该旧包的下列尝试未完成完整 gate，不能记为 PASS；最终新包的三组报告另行记录。 |

三人前两次未完成联合验收，原始失败报告分别保留为 `3p-relay-attempt-shared-browser.json` 与 `3p-relay-attempt-independent-screenshot.json`。首轮在 `Locator.focus` 30 秒超时，之后仍观察到 P1 的 native 世界推进到 F779、没有 native error；第二轮在 `Locator.screenshot` 的 `taking element screenshot` 阶段超时。不能把这两次失败改写成旁观协议失败、已证实资源饱和或完整三人通过。首轮成功的三行标准测量窗两张图片及其 hash 另存于 `attempt-shared-browser/`；根 agent 亲看 P1 图，无溢出，但这仅证明当次实际 UI。

验收脚本随后先采集原生状态与真实输入结果，再在真实 P1 暂停下使用既有 `#netplayCalibrationReport` 取稳定报告。标准 ready 窗本来只保留 8 秒，并每 250 ms 重建内容；稳定报告使用原有 dialog 的打开 / 关闭，不修改产品 DOM 或原生循环。浏览器后端改为明确指定 ANGLE / SwiftShader 并记录实际 renderer；此前仅有 `--enable-unsafe-swiftshader` 不能证明选中了软件后端。前两次失败原因仍未证实，最终软件渲染验证也不能替代真机显卡适配结论。超时后的清理只作用于记录过 PID 与创建身份的本测试浏览器。

随后第三次未完成结果保留为 `3p-relay-attempt-resource-fetch.json`；当时 P2 字体、P3 `resources.json` 的 fetch 错误缺少时间记录，不能排除清理取消，也不能据此判定资源根因。之后移除所有 Python 请求拦截，浏览器直接使用标准 HTTP；前置检查逐文件校验完整 Runtime，网络观察记录请求 / 响应 / 完成 / 失败、客户端、epoch 与阶段，失败证据在清理前冻结。

实际发现并修正的预览准备错误是：主线前端重编译后，长驻开发服务器仍持有旧 `FRONTEND_PACKAGE_FILES`，请求当前 `chunk-3D6FSSZY.mjs` 返回 404。仅按已记录 PID 与创建身份重启本测试 HTTP / Relay；同一文件随后 200。不能把这个已确认的旧资源表问题推广成此前所有未完成测试的原因。

旧包的直接 HTTP 启动诊断仍未通过，原报告保留为 `3p-startup-direct-http-wasm-timeout.json`：四份 Runtime 的同 URL WASM 请求没有观察到 response / finished，约 110–120 秒后由 Launcher 的“本地游戏加载超时”清空 iframe 并取消，早于最终 browser.close；服务也记录了相应请求的 Premature close。进一步 `http-encoding-diagnostic.json` 的 identity / gzip / br 与四并发 br 共 7 次读取均 200、完整且匹配旧 hash，耗时 64–1,772 ms，没有复现压缩阻塞。新包 `browser-fetch-probe.json` 在相同真实 Chromium / 服务 / flags 的普通同源页以 br 完整读取新 WASM，headers 404.3 ms、body 422.2 ms，字节和 SHA 一致，`WebAssembly.compile` 6.7 ms；没有实例化游戏。这些探针缩小排查范围，不替代完整房间启动，也不提供前次四世界加载停滞的根因结论。

main 最终前端编译为 65 个 TS 源文件；TH11 Catalog / pure-only / Relay / 观战 policy 专项通过，相关 11 项现有模块检查和 Relay 套件单独复跑通过，native fatal 的中英文短正文 / 传输仍连接时的停止 / 旁观停止模型断言通过。初次并行 Relay 套件的旧 5 秒截止发生一次未重现超时；同请求 HEAD 与 working 都在约 4.58 秒进入 Relay，路由和默认 4.5 秒计时源码未改。不把 HEAD 说成失败，也不把短程本地结果说成公网表现。完整命令和对照记录位于 main 的 `.cache/agent-evidence/th11mp-main-integration-20261009.json`。

### 11.6 本轮仍须报告的高危项

1. **跨构建 MP Replay 不兼容。** 本次把 3P 碎片从旧实现两倍修正为一倍，确实改变模拟结果；当前精确 WASM 指纹会拒绝旧实验录像，没有迁移器。必须保留对应旧 Runtime 才能继续播放旧录像。
2. **经济与资源边界仍有未确认决定。** 跨机体按各自一档 Power 交换、普通 P / F 优先未满者、all-clear 的全队个人资源聚合仍按第 5 / 10.2 节临时规则执行。这些不是用户本轮确认的共享碎片规则。
3. **救援、目标与途中失去资格仍有临时规则。** 280 帧不消弹救援、死亡 / 重生过渡计入 Boss 分母与全满条件、最近 Alive 瞄准、全体 Alive 特殊吸引，以及 Power 赠送失去目标转普通掉落 / 生命赠送继续加幽灵储备命的不同处理仍须提醒。
4. **重开与长局边界。** R 使用新一代 seed 并重新测量，旧观战 / Replay 不跨代；250,000 确认帧后结束并保存 partial、99 槽后的 ud 自动命名策略保持。这不等于长程 seek 或约 69 分钟浏览器实跑已经通过。
5. **验收和发布范围。** 本轮标准联合验收使用本机 Chromium 与实际主线前端 / 生产包；没有完整多人战役 / Extra 真人通关、公网、Android / iOS 真机结论。代码在本地 main 与 eagler-mp 工作区处理，不把本地接入写成线上已发布。

**已经确定、不得继续当作未定义项的规则：** 生命 / Power 集合、本机黄色席位号、其他个人 HUD 原位且只显示本机、标准前端拥有连接 UI、全队 5 片各加 1 命 / cap 9 / 不自动复活幽灵，以及任意人数碎片原作一倍。

### 11.7 补齐旧作已有的挑战模式

此前把“先不做预测回滚”扩大成“TH11 不支持挑战模式”没有用户依据，已撤销这一范围缩减。主线 `content/MULTIPLAYER.md` 已定义挑战玩法；实际 TH08 / TH10 的 `eagler-mp` 又明确了库存与累计 Miss 的边界，因此复用既有行为，不另立每作品的可选能力豁免。

挑战模式中，原生 Life 行使用原生数字字体显示本席累计 Miss，Power 与其他本机 HUD 位置保持。Bomb 同时在确认输入、原生可用状态、席位回调和原生启动入口被禁止，包含六机体、Nitori 护盾与死亡 Bomb。死亡仍执行原生点值 / Power 损失、普通 P 掉落、动画、重生和无敌；不扣隐藏生命库存，不发最终生命 F，不进入幽灵或因耗尽生命结束团队。

每席累计 Miss 是独立的饱和 `u32`，纳入一致性 hash，跨关保留，新局 / R 新代清零。与 TH10 一致，生命转移和 Extend 仍操作各席隐藏且 cap 9 的生命库存，不改变累计 Miss；用户确认的共享 5 片和任意人数碎片一倍仍适用。此处没有把隐藏库存改成无限值，也没有新增一次个人 Extend。

标准前端沿用原挑战开关；`netplayChallengeMode` 经 Runtime、SessionSetup 的严格 `w21=0/1`、Application 和 MultiplayerOptions 传入原生。原生 Replay 的保存、元数据检查、播放与下一代均保留模式。原有 Gameplay ABI 已包含挑战状态和构建指纹，仍拒绝模式不匹配、不同 exact WASM、非法 `w21` 和非零预测；未更换协议或伪造兼容。

| 本轮挑战相关检查 | 结果与边界 |
| --- | --- |
| 原生挑战专项 | `node portable/multiplayer/native-test.mjs --challenge`，**2,920 checks / 3,655 logic+draw ticks PASS**；`artifacts/multiplayer-native/challenge.log`。 |
| 普通风险与挑战合并回归 | `node portable/multiplayer/native-test.mjs --risk --challenge`，**4,892 checks / 7,527 logic+draw ticks PASS**；`artifacts/multiplayer-native/challenge-combined-risk.log`。完整覆盖本轮原 `--risk` 集合，未把仅无参数全量入口才执行的旧基础长测组算入。 |
| 网络与 Replay | `node portable/multiplayer/network-test.mjs` PASS，普通 / 挑战 × 2P / 3P × D=0 / 2 / 9，含逐帧 Replay、下一代模式保留、模式 / 构建 ABI 隔离、非法配置和既有 ACK / 测量 / 观战回归；`artifacts/multiplayer-network/challenge-network.log`。 |
| 普通与 MP 编译边界 | 9 个真实 Emscripten `-fsyntax-only` 入口全部通过；`artifacts/multiplayer-native/challenge-syntax.log`。共享 C++ 行为由 MP 宏隔离，多人源文件 / JS 由既有构建与打包名单隔离。 |

这些原生测试读取真实资源并执行真实逻辑，绘制后端用于检查原生 draw 请求，不冒充 GPU 截图或完整浏览器联机。新增挑战后的正式构建身份见第 11.4 节；旧构建的 GPU / 浏览器证据不能自动视为新构建通过。

### 11.8 WASM 启动失败的错误所有权

排查四客户端加载停滞时，另行找到一个能够由源码证实的缺口：TH11 的 `instantiateWasm` hook 返回异步 Promise，而实际 Emscripten `createWasm()` 仅等待成功回调、不观察返回 Promise 的 rejection。因此加载或实例化已经失败时，外层 `initialized.catch` 仍收不到错误，只能等标准 Launcher 的 120 秒 ready watchdog。

修正仅在 shell 本次初始化中创建私有拒绝 Promise，并与实际 `createModule()` 做 `Promise.race`；hook 的异步失败传给这个 owner，成功仍先保存原生 exports 再调用原成功回调，hook 同步返回 `{}`。失败通过原 `initialized.catch → error(e) → eagler-touhou/1 error` 交还标准前端。普通 / MP 使用同一块代码，没有新 UI、协议、模块、生成 loader 修改或额外 WASM 请求；生产 WASM 指纹不变。

回归入口 `portable/multiplayer/bootstrap-failure-test.py` 使用真实生产 HTML / shell / Emscripten loader 和隔离 HTTP 服务，仅给准确的 WASM URL 返回 HTTP 503 或损坏字节。不替换 Module / WebAssembly、不复制初始化算法、不拦截浏览器请求；父 fixture 只接收标准事件，不冒充实际主线联机。

| 实际失败用例 | 修正前 | 修正后 |
| --- | --- | --- |
| 普通版 / HTTP 503 | 响应后约 2.5 秒仍无标准 error | 75.8 ms 内观察到 error |
| 普通版 / 坏 WASM | 同上 | 15.5 ms |
| MP / HTTP 503 | 同上 | 28.5 ms |
| MP / 坏 WASM | 同上 | 39.7 ms |

最终 **4 / 4 PASS**：每例恰好 1 条标准 error、WASM 请求 1 次、DATA 调用 0 次，不暴露 Module/core、不发 ready/first-frame，并触发对应既有错误 owner。修正前的指定失败保存在 `artifacts/multiplayer-bootstrap/before.json`，最终通过在 `after.json`，日志为 `artifacts/multiplayer-native/bootstrap-{before,after}.log`。首次 after 已正确回传错误，但测试对 HTTP 503 的错误文字断言过严；只修测试断言后复跑，原证据仍保留为 `after-message-assertion.json` 和对应日志。测试所有自有浏览器与 HTTP 服务均清理，未写生产包。

这项修正**不被视为此前四份请求一直无 response 的根因**，两者证据不同。补齐挑战后的首轮 2P 完整通过记录（609 个同帧采样、156 个 Replay 同帧采样、11 PNG）完整归档在 `artifacts/launcher-main-th11mp/pre-bootstrap-fix/`，不充当修改 shell 后最终三组通过。最终包重新通过原主线静态闭包、包隔离与官方 `package-server`；新身份在 `source-identity-evidence.json`，启动修正前身份保留为 `source-identity-evidence-pre-bootstrap-fix.json`。

### 11.9 最终生产包的标准前端验收

本节只记录第 11.8 节 shell 修正后的完整门禁。共同身份为 MP WASM `1037f8a836417ce23a97fd3493c61ae60b9096f44036ab1961cb66dbc94bc85e`、Runtime inventory SHA `22582caa65c45c330bd60048dc9494e76dac2faaa53084f9ef7c55562fb37e7d`；官方导入 ZIP 使用第 11.5 节的最终身份。使用实际 main 的标准产品入口与房间，不拦截 Runtime / DATA HTTP 请求。启动前通过真实 HTTP 核对全部 22 个 Runtime 文件和清单，DATA 由标准导入包核对。

| 最终门禁 | 实际证据 |
| --- | --- |
| 普通 2P RTC | `artifacts/launcher-main-th11mp/final-2p-rtc.json`：**PASS / fullAcceptance=true**。双端官方导入、准备、129 次真实测量，自动 **D=4 / P=0**；P1 / P2 的 P95 为 130,700 / 69,550 微秒，各 120 个有效样本、无丢失。双方真实移动，**656 个同帧采样 F60–813 hash 一致**。D 是这轮实测值，不沿用旧轮次的 D1 或 D2。 |
| 普通 2P 实断与返房 | 真正关闭 P2 后原生报告 `RTC peer P2 ICE restart exhausted`，世界冻结于 **F1640**；标准 Return 回房，member / client / room / seat 四项身份全部保留。 |
| 普通 2P 原生 Replay | 同一次实际会话保存 `th11_01.rpy`，**39,536 bytes / 1,640 帧**，challenge=false。生产 JS parser 与原生 exact-build 校验通过；标准 Launcher 进入原作 25 行列表、原生选关、播放、Esc 回目录、原生 Back 返 Launcher，**121 个 Replay 同帧采样 F61–229** 匹配该次实际运行。没有网页播放器或伪造章节。 |
| 普通 2P 请求与截图 | **11 张实际流程图，errors=[]、requestFailures=[]**。11 次 HTTP 404 全部是既有 `assets/th09-card.webp`；TH11 Runtime / DATA 请求没有失败，不把结果表述为全站零 HTTP 错误。 |
| 挑战 2P RTC | `artifacts/launcher-main-th11mp/final-2p-challenge.json`：**PASS / fullAcceptance=true**。使用标准房间中的原挑战开关；129 次真实测量得到 **D=4 / P=0**，P1 / P2 的 P95 为 102,749 / 83,400 微秒，各 120 有效样本、无丢失。双方真实移动，**632 个同帧采样 F60–794 hash 一致**。真实关闭 P2 BrowserContext 后冻结于 **F1612**，标准 Return 四项身份均保留。 |
| 挑战 2P 原生 Replay | 同次真实 `th11_01.rpy`，**38,864 bytes / 1,612 帧 / challenge=true**；生产解析器及原生 exact-build 校验通过，原列表 / 选关 / 播放 / Esc / 返 Launcher 完整通过，**135 个 Replay 同帧采样 F60–224** 匹配。**12 张流程图，errors=[]、requestFailures=[]**；同样只有 11 次既有 TH09 卡图 404，TH11 请求无失败。 |

根 agent 亲看本轮 `final-2p-rtc-native-gameplay-canvas.png`、`native-replay-list.png`、`native-replay-stage.png` 与 `standard-disconnect.png`：战斗使用原生 4:3 画面和纹理，一份 HiScore / Score；每席重复的只有 Life / Power，1P 在集合开头标黄；本机 Graze 与左下点值 / 通信保留原位。Replay 保留原有 ANM 背景、字体、25 行列表和黄色选择态；停止提示由既有标准前端给出简短正文和返回动作，没有把错误栈作为停止窗正文。

根 agent 也亲看 `final-2p-challenge-` 前缀的 `standard-challenge-room`、`native-gameplay-canvas`、`native-replay-list` 和 `native-replay-playback` 四图：房间设置仍用标准前端原开关及原布局；挑战 Life 行使用原生数字字体展示累计 Miss，Power / 本机 Graze / 左下点值位置保持，Replay 同样沿用原菜单和画面，没有新增统计页或模式面板。两组浏览器门禁的断线动作都是关闭 P2 的真实 BrowserContext；普通 2P 的进程随后单独清理，不能把这个动作描述成先强杀整个浏览器进程。

这批实际 Chromium 使用 ANGLE / SwiftShader 软件渲染，截图中的原生 FPS 明显低于 60；测试构建已有诊断叠层保持原默认。**完整功能门禁通过不能表述为 60 FPS 性能验收通过**，亦不替代真机显卡、公网、完整多人战役 / Extra 或移动设备验证。

## 12. 最终 HUD、暂停重启与前端禁回滚修正

用户最终要求：本机 Graze 独立于各席生命 / Power，保持标签和数值列对齐，在最后一席参数下留一行空白，不增加 LOCAL 标题。保留原生字体、标签与个人数值；玩家组顶部为 104 / 176 / 248，Graze 数值为 2P y=248、3P y=320，标签锚点复用 Power 列。本节取代第 11 节关于 Graze 留在 P1 下方原位的布局结论。

原生暂停恢复 Restart 脚本 80，方向选择跳过已移除的单人 Return / Replay Save。P1 确认 Restart 与 R 均由同一 confirmed-frame 退休围栏保存旧局并建立下一代、重新测量；菜单不得直接重建本机世界。Resume、任意席 Pause 键和只读观战边界保持。

main 前端在 product-catalog 中声明 rollbackLimit=0，回滚控件隐藏且 disabled，并在不支持时清除旧开关值。点击处理、Runtime 参数、房间快照与 relay 继续拒绝预测 / 回滚。真实浏览器门禁同时检查 disabled、aria-checked=false 和强制派发点击后的不变状态。

最终生产 WASM：f4d650518257e4d58e9947294968d40c96e8eac6b73a68a028c6172a5455f250。`artifacts/launcher-main-th11mp/aligned-hud-restart-2p.json` 为真实 main、2P RTC、原生暂停 Restart、已入场观战退休及晚加入拒绝门禁：passed=true、fullAcceptance=true，下一代 F60–227 的 132 个同帧采样 hash 一致，errors / httpFailures / requestFailures 均为零。根 agent 检视 P2 原生暂停画面，核对独立 Graze、列对齐、间距及无 LOCAL 标题。

最终原生 UI / Restart / 三视角门禁 1,643 断言、2,777 逻辑 / Draw tick 通过；`artifacts/multiplayer-ui/20261009/aligned-hud-restart/` 的截图与数值断言通过。此前同轮全原生门禁 6,822 断言、12,407 tick、协议与封装隔离通过；最终布局改动仅改变 Draw 坐标，随后重跑 UI 与真实双人菜单重启。main 产品、relay 策略、Runtime 诊断和工作区映射回归通过。本轮不声称三人真实浏览器全流程、移动设备、公网或性能验收。
