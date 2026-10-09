# TH11 Multiplayer：实现、规则边界与验收记录

记录日期：2026-10-09。用户已授权先实现，未定义行为按实现者理解决定并作高危提示。多人玩法、原生协议、Replay / 观战与 Launcher 接入已完成；普通 / MP 生产构建和最终四组真实浏览器验收通过。逐项证据、独立 Launcher 基线的既有阻塞和未验证范围见第 7 节。第 5 / 6 节的临时规则不能当作已获平衡验证的既有共识，本轮不晋升 canonical、不 push 或部署。

## 1. 本轮要求与基线

用户要求：参照 TH08 / TH10 MP 实现 TH11 MP，严格遵循 Launcher 规则，未定义行为必须告知；沿用现有 MP 分支名；**本轮不做预测与回滚**。

| 角色 | 工作区 / 分支 | 核对提交 |
| --- | --- | --- |
| TH11 Eagler 基线 | `th11-eagler` / `eagler` | `0a582e63598fd0a4c89063a37829d92af65c2a90` |
| TH11 MP | `worktrees/th11-multiplayer` / `eagler-mp` | 从上述基线建立 |
| TH08 MP 参考 | `th08-eagler` 的 `eagler-mp` ref | `b5b4091cb19619576035ea64ce2913bb22b3833b` |
| TH10 MP 参考 | `th10-eagler` 的 `eagler-mp` ref | `288ea8d5676ace5152ca5cd317061511ee660887` |
| 两个参考 MP 的共享库依赖 | `eagler-common` gitlink | `43add4dc9085c65468f7ab15428c00206cbe3834` |
| TH11 MP 共享库依赖 | `eagler-common` gitlink，TH11 EBTM identity、退休 ACK 与观战尾帧 | `d81eda48917625ff1d3f7d156bbdb2fdd816fde7` |
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
| 整残机 / Bomb 掉落 | 道中对应道具 2P 一倍、3P 两倍；TH11 碎片临时包含在内，见第 5 节 B。 | 同上 |
| Extend | 分数 / 点数目标奖励分配给队友，服从各自上限；只有 TH08 明文限定仍可操作者。TH11 碎片 Extend 临时全队分发，见第 5 节 B。 | 同上 |
| 初始 Bomb | 有独立 Bomb 数的作品为原作每命数量减 1、最低 1；不据此新造 TH11 Bomb 存量或改其 Power 耗费。 | 同上 |
| 可见性 / 立绘 | 队友透明化，遵守本地玩家可见性选项；取消符卡释放立绘，保留文字宣言；不改权威 gameplay。 | 同上；MP profile |
| 出生站位 | 在原作允许区域围绕默认点对称分散；间距可按场地确定，不改变活动范围。 | 行为不变量“多人初始站位必须分散” |
| HUD / 给予反馈 | 复用 TH11 原作标签、字体、数字、图标和布局，仅增加必要席位排列。转移进度跟随自机最终呈现位置；用原作道具 / 音效反馈，不做右侧进度条或现代卡片。 | 行为不变量“多人 HUD 必须延续原作 HUD 的视觉语言”“残机与 Power 给予反馈必须出现在游戏区域内” |

## 4. 逐席保留的 TH11 原作语义

本节路径相对于 TH11 仓库。`th11_web/cpp/game/ShtResource.hpp` 定义 SHT 的 `max_power_level` / `power_step`，`PlayerFrame.cpp` 按所选 SHT 覆盖 economy；不能以 `GameEconomy.hpp` 的默认 `400 / 100` 代替实际资源。本轮按已定义 SHT 格式检查得到：

| 机体 | 最大档数 | 原始 `power_step` | 原始 `max_power` |
| --- | --- | --- | --- |
| 灵梦 A / B / C、魔理沙 B / C | 4 | 20 | 80 |
| 魔理沙 A | 8 | 12 | 96 |

每席保留所选机体的速度、射击、Option、Bomb、Deathbomb、上限与耗费。`ItemRewards.cpp` 中小 P 加 1、大 P 加接收者 step、F 加本机体 max；救援半满也取本机体上限，不能照搬 TH10 的 raw 100。

**通信率自然逐席保留，不额外改成团队池。** `GameEconomy.hpp` 含 communication / graze / score / point value；`Communication.cpp` 按该玩家坐标和奖励更新，封顶 13000；`GameBattle.cpp` 按其 y 调用。`GameEconomy.cpp` 点值与 `ItemManager.cpp` 吸取条件消费它。每席 economy、擦弹、坐标和死亡清理都保留原作公式；共享池没有规则授权。共享道具归最近合法者，见第 5 节 C。

`ItemRewards.cpp`：类型 5 为碎片，5 片换 1 命；类型 7 为整残机；上限 9。原生换算 / 上限继续保留，多人共享计数与分发见第 5 节 B。`BombController.cpp` 的 selection 5 为魔理沙 C 河童护盾，未触发返还 10 原始 Power、被击中才计 Bomb / 符卡失败，须逐席保留，不能因共享符卡而让展开护盾立即失败。混合 Bomb 伤害先按攻击所属席位套该机体原作机制再汇总，不能因一位护盾影响全部队友；局部状态与真正共享背景 / 音效 / 敌人效果的 owner 拆分是实现责任。

## 5. 已获实施授权的三组临时决定

**高危：用户授权“先实现，未定按理解决定但必须高危提醒，不要停”。以下为实现者补足的规则，并非从旧文档推导出的既有共识。** 它们会改变资源经济、碎片收益、敌人目标和特殊攻击难度，尚未有完整多人平衡 / 流程证据；后续改变可能影响 Replay 兼容性。已在 Launcher 玩法介绍中显式标注。新的未定义行为须继续记录并提醒，不能静默改掉规则。

### A. 资源转移、满 Power 与救援

一次交换扣赠送者自己的一个 `power_step`，接收者增加自己的一个 `power_step`，各自按本机体上限封顶。魔理沙 A 的 raw 12 与其他机体 raw 20 不做原始点数守恒。目标按显示档数 `power / power_step` 比较，较少者优先，再按席位；确定性比较使用整数交叉乘积，不能依浮点显示舍入。

TH11 救援采用 **不消弹、280 个逻辑帧无敌**，继续原地复活、半满本机体 Power。场上 **全部可操作玩家各自满 Power** 后才触发全场 P 转点；幽灵既不阻挡转换，也不能接受 Power。20 像素、5 次点按、24 帧间隔和第 3 次显示计数不变。

**高危：定向 Power 道具在飞行途中，若接收者变成幽灵，立即取消定向并按普通 P 留场，供合法存活玩家收取；赠送者不退款。** 原规则没有定义这个途中失去资格的边界。本轮选择保留场内道具、避免向幽灵发 Power，不把已扣资源回滚给赠送者。

**高危：本轮 Boss 人数系数和全员满 Power 判定中的“可操作席位”，具体采用尚未进入最终幽灵的参战者；仍包括原生死亡 / 重生动画过渡中的席位。** 敌人、子弹、激光和普通道具选择目标时另按 `alive()` 筛选，不把正在死亡的席位作为合法目标。临时失去操作不会立即降低 Boss 系数分母，也不会从满 Power 条件中移除该席位。

**高危：残机赠送道具起飞时的接收者若在飞行途中变成幽灵，仍向原接收者增加储备残机，服从上限，但不会自动复活。** 这与 Power 道具取消定向的策略不同；救援与过关仍是复活边界。原规则没有定义这项途中变化，本轮没有将其静默视为一次现场救援。

### B. 残机碎片、奖励分发与阶段条件

使用 **全队共享碎片计数**，每收满 5 片向全部参战席位各发 1 残机，各自服从 9 命上限；奖励不自动复活幽灵。碎片掉落采用 **2P 一倍、3P 两倍**。各席位个人原作计数不再独立触发第二次碎片 Extend，避免重复奖励。

**任何玩家 Miss 都纳入共享阶段死亡条件**；符卡成功 / 失败仍共享。各机体原作何时真正产生 Bomb / 失败事件的条件保留，尤其河童护盾展开时不能提早失败。脚本计数的具体 owner 和重置点在原生实现中统一，不能仅读 P1 或重复累加同一事件。

### C. 共享场地对多个玩家的目标与作用

敌人 / 子弹 / 激光在原作查询目标的时点，按 **来源位置到 Alive 玩家的最近距离平方** 选择，并列取低席位。道具先按每席原作通信率、当帧移动后位置和道具原生移动 / 吸取步骤确定拾取 / 吸取资格，再在合法者中按最近、并列低席分配。无可操作玩家时保留原作查询所需的确定性场地回退坐标，不将幽灵作为合法目标。

**高危：普通 P（含 Full Power / F）在合法接收者中先优先未满 Power 的席位，再按距离、并列低席选择；只有没有合法的未满 Power 接收者时，才回退到满 Power 席位。** 这是本轮采用的合作资源分配策略，可能让较远的未满 Power 玩家优先于近处的满 Power 玩家；它不是原文“最近合法者”自动包含的既定含义。定向赠送的 Power 道具仍遵循第 5 节 A 的指定接收者及其途中失去资格规则。

TH11 `AttractPlayerAndBullets` 等全场特殊吸引临时作用于 **全部 Alive**，分别以各自当前位置应用原作效果；同一帧完成环境同步。P1 仍决定剧情 / 路线 / 机体依赖的共享关卡内容。

参考代码并不统一：TH10 `WorldTargetsMultiplayer.cpp` 为来源最近 Alive；TH08 `PlayerRoster.hpp` / `BulletSystem.hpp` 的弹幕选最近 eligible，但部分 `EnemySystem.cpp` / `GameplayScene.cpp` ECL 留有 seat-zero 兼容。本节是明确的 TH11 临时决定，不把这两种历史实现伪称为 Launcher 通用规范。

## 6. 仅纯锁步与实际通道延迟

`adonis-adaptation.md` 已有完整流程：资源 / 世界准备、真实输入链路 HELLO、稳定等待和 129 轮测量、proposal / accept / commit / ack、全席位帧零屏障。3P 必须包含 P2–P3；使用本地高精度单调时钟，严格遵守有效样本、中断、路由变化与超限拒绝，不按手机数或 Launcher RTT 猜 D。

自动纯模式 `P = 0, D = B`，其中 `B = max(1, ceil(floor(max_required_RTT_us / 2) * 60 / 1000000))`。手动模式同样测量 B，但使用用户明确选择的 D（含 0），不要求手动 D 等于 B；D 依声明验证，自动溢出显式失败。每个输入只采样一次归属 N+D，缺真实输入不能预测前进；Replay / spectator 不再加 D。对局内冻结 D，相位修正只调整壁钟调度，保持权威 60 Hz。

能力声明使用 `rollbackLimit: 0`，所有配置只接受 `netplayAdonisMode: 1`，实际选定 / 上报的预测量为 0。启动请求仍携带 common v5 契约要求的 `predictionReserve: 2`，该预留在纯模式不使用，不能传 0 绕过 1/2 校验。TH11 暂不提供挑战模式，声明 `challengeMode: false`，UI、房间、Relay 与 native 均拒绝 true；其他作品保持既有默认。

必须绕过 undo 分配、捕获、恢复和修正；资源回收、确认音频 / 文件、Replay / 观战仍需 owner。Launcher 声明、UI、配置校验和 native 必须一致限制纯模式，不能仅默认关回滚却仍接受 hybrid，也不能加 TH11 特判绕过能力模型。

### Replay 与重开边界（实现决策）

TH11 MP Replay 采用公共 `EAGLRPY1` 扩展，`gameId = 11`，版本化 128 字节 `T11M` 描述记录配置 / 实测 D、`recordedPlayer` 与全部席位已确认输入。自动保存优先使用 `/savesth11mp/replay/th11_##.rpy` 的 01–99 空槽。Replay 由 Runtime 自有列表 / 关卡选择器控制，不新增 Launcher 命令。

**高危：本版浏览器 Runtime 拒绝不同 WASM 构建指纹的 MP Replay，避免临时规则变化后静默错误回放；升级构建可能使旧录制无法在新版播放。** 章节跳转从帧零使用同一 `GameSession::begin / update / draw` 生命周期重建，不读写回滚快照，不再次应用 D；每个 RAF 以约 8 ms 预算推进。短程 native / browser 一致性已按第 7 节验证，整关及长程 seek 不在本次已通过的证据范围内。

**高危：01–99 自动槽全满后，本轮自动保存继续选择现有 Replay 命名空间中的空闲 `th11_udXXXX.rpy`，其中 XXXX 为四位小写 base36。** 先检查完整编号区，再按 base36 递增检查扩展区；不会覆盖原有编号或导入录像。同一代首次选中的自动路径由后续 flush 复用，下一代重新选择空路径。手动指定编号的语义不变。真正的文件 I/O 失败仍显式报错，不能把逻辑槽位扩展宣称为磁盘 / 浏览器存储故障容错。

**高危：每一代最多记录 250,000 个已确认输入帧，按 60 Hz 约为 69 分 27 秒，包含暂停、Ending 等仍有输入的帧。** 达到最后一个合法帧后，不再执行第 250,001 帧，而是通过现有确认与 ACK 退休流程结束本代，保存标记为途中（partial）的 Replay，并在现有终局控件中明确提示帧数上限。需要返回房间开始新的对局；不分卷、不跨上限继续，不将容量终止标成普通通关。该上限来自公共 Replay 格式的容量约束；本轮选择的是明确结束策略，不能声称已做完整 69 分钟浏览器实跑。

P1 在暂停状态按 R 发起重开，先完成旧 generation 的 ACK retirement，再派生新的 sessionId 与下一 seed；旧的未来输入不能流入新局。**高危：这里的重开使用新一代种子，不保证复现上一局的相同开场随机序列。** 这是本轮已记录的实施选择，不新增任意席位独立重启共享世界的权限。

每一代重开都重新进行 129 次实际通道测量，不能把上一代的 D 当作新一代测量结果。刚退休一代的合法尾包只用于严格限定的退休边界，旧终局 ACK 可继续重发；不会把它们送入新一代输入 Core。Replay 和观战消费到录制的 R 时正常结束旧流，不加入下一代。

正常观战终局最多冻结 256 帧待发送的已确认输入，每次最多发送 32 包或用时 2 ms；在 15 秒内独立排空。末尾数据与正常 STOP 走同一条有序 relay 流。观战压力不阻塞玩家确认、退休或重开；超出尾帧上限或排空超时则明确标记观战失败。接收端先消费已缓存的尾帧，再处理 EOF。

## 7. 实现与验收证据

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
