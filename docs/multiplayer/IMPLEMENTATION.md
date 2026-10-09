# TH11 Multiplayer：规则审计与实施边界

记录日期：2026-10-09。状态：规则审计与隔离分支已准备；多人玩法、原生网络协议和 Launcher 接入尚未实施，构建与运行验证尚未进行。本文件不是功能完成声明。

## 1. 本轮要求与基线

用户要求：参照 TH08 / TH10 MP 实现 TH11 MP，严格遵循 Launcher 规则，未定义行为必须告知；沿用现有 MP 分支名；**本轮不做预测与回滚**。

| 角色 | 工作区 / 分支 | 核对提交 |
| --- | --- | --- |
| TH11 Eagler 基线 | `th11-eagler` / `eagler` | `0a582e63598fd0a4c89063a37829d92af65c2a90` |
| TH11 MP | `worktrees/th11-multiplayer` / `eagler-mp` | 从上述基线建立 |
| TH08 MP 参考 | `th08-eagler` 的 `eagler-mp` ref | `b5b4091cb19619576035ea64ce2913bb22b3833b` |
| TH10 MP 参考 | `th10-eagler` 的 `eagler-mp` ref | `288ea8d5676ace5152ca5cd317061511ee660887` |
| 两个参考 MP 的共享库依赖 | `eagler-common` gitlink | `43add4dc9085c65468f7ab15428c00206cbe3834` |
| Launcher 规则来源 | `eagler-touhou` / `experiment/ui-main` | `039b6e0034a70536afe5d7561d078d7e7e2c295d` |

表中路径相对于 `D:\workspace\eagler`。08 / 10 的实际 MP 分支都叫 `eagler-mp`，不能沿用旧文档的 `experiment/thXX-multiplayer`。本次引用的 Launcher 规则文件无工作区 diff，但其余 Launcher 存在别人的未提交修改，必须保留。本轮不改普通 TH11 工作区，不混用普通 / MP Runtime；消费者须显式声明共享库版本，不能任意使用兄弟目录 HEAD。

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

原作 gameplay / menu 是默认基线，只有明确产品规则才覆盖它；旧 MP 代码只是参考。玩法介绍开头仍列 06 / 07 / 08 / 10，但普通 P 掉落条款已明确点名地灵殿。本轮依用户要求继承适用通用规则，TH11 真空白见第 5 节。行为不变量中的 `P 4/8` 是历史反馈示例；当前玩法明确为 **5 次点按，从第 3 次显示计数**，不能复制旧 8 次实现。

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
| 救援无敌 | 06 / 07 / 08 明文为不消弹、240 帧；10 为不消弹、280 帧；TH11 未给值，见待定 A。 | 玩法“残机转移” |
| Power 操作 | 20 像素内点按 Shoot 5 次，相邻间隔不超过 24 逻辑帧，从第 3 次起在自机上方显示计数。 | 玩法“Power 转移” |
| Power 资格 | 赠送者至少有完整一次量；接收者须可操作且未满 Power，幽灵不合法；按 Power 更少、然后席位更低选择。 | 同上；行为不变量“多人资源转移目标选择必须确定” |
| Power 效果 | 扣除对应 Power，生成等价值原作道具自动追踪接收者；TH11 单次量和异机体比较尺度未定义。 | 玩法“Power 转移” |

### 掉落、奖励与呈现

| 行为 | 必须保持的规则 | 来源 |
| --- | --- | --- |
| 普通 P 倍率 | **地灵殿已明确为 2P 原作一倍、3P 两倍**；Full Power 不倍增。 | 玩法“杂项” |
| 整残机 / Bomb 掉落 | 道中对应道具 2P 一倍、3P 两倍；TH11 碎片是否包含在内未定义。 | 同上 |
| Extend | 分数 / 点数目标奖励分配给队友，服从各自上限；只有 TH08 明文限定仍可操作者。TH11 碎片 Extend 的分配未定义。 | 同上 |
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

**通信率自然逐席保留，不额外改成团队池。** `GameEconomy.hpp` 含 communication / graze / score / point value；`Communication.cpp` 按该玩家坐标和奖励更新，封顶 13000；`GameBattle.cpp` 按其 y 调用。`GameEconomy.cpp` 点值与 `ItemManager.cpp` 吸取条件消费它。每席 economy、擦弹、坐标和死亡清理都保留原作公式；共享池没有规则授权。同一共享道具最终归谁是另一问题，见待定 C。

`ItemRewards.cpp`：类型 5 为碎片，5 片换 1 命；类型 7 为整残机；上限 9。原生换算 / 上限继续保留，多人分发另定。`BombController.cpp` 的 selection 5 为魔理沙 C 河童护盾，未触发返还 10 原始 Power、被击中才计 Bomb / 符卡失败，须逐席保留，不能因共享符卡而让展开护盾立即失败。混合 Bomb 伤害先按攻击所属席位套该机体原作机制再汇总，不能因一位护盾影响全部队友；局部状态与真正共享背景 / 音效 / 敌人效果的 owner 拆分是实现责任。

## 5. 真正待定义的三组行为

本节不提供猜测默认值。只有依赖这些决定的玩法需等待；隔离、输入与通用协议可以独立准备。

### A. 资源转移、满 Power 与救援

需确定单次 Power 量和异机体等价值 / “Power 更少”的比较尺度：一档在不同机体上是 raw 12 或 20，固定原始点与固定档数不是同一规则。另需确定 TH11 救援是否消弹和无敌帧数，以及何时全场 P 转点；TH08 / TH10 已避免转换队友仍需要的 P，但 TH11 无本作条款，不能把单人 `ItemRewards::collect` 的 convert 直接作用全场。已定的原地救援、半满本机体 Power、20 像素、5 次点按和 24 帧不重问。

### B. 残机碎片、奖励分发与阶段条件

需确定碎片计数归拾取者还是团队、集满 5 片的 Extend 发给谁、3P 残机道具倍率是否涵盖碎片，以及脚本根据阶段死亡 / 符卡计数判断掉落时纳入哪些席位的事件。不能默认只读 P1，也不能无声明地合并所有事件。实际掉落条件须在 Enemy / ECL owner 核对并记录；“符卡收取共享”不自动定义所有碎片条件。原作 5 片换 1 命、9 命上限、奖励不自动复活幽灵、180 帧团灭结束均已有依据。

### C. 共享场地对多个玩家的目标与作用

需确定敌人 / 子弹 / 激光怎样选择自机及何时更新、多人同时可拾取 / 吸取时一件道具归谁、直接移动玩家的 ECL 对选中席位还是全部可操作席位生效。TH11 `EnemyCallbacks.cpp` 的 `AttractPlayerAndBullets` 调用 `callback_move_player`，当帧修改唯一玩家及环境并影响后续敌人；不能仅换一个全局 Player 指针。

参考并不统一：TH10 `th10_web/cpp/multiplayer/WorldTargetsMultiplayer.cpp` 按敌人 / 发射源 origin 选择最近 Alive、并列较低席位；TH08 `PlayerRoster.hpp` / `BulletSystem.hpp` 的弹幕选最近 eligible，但 `EnemySystem.cpp` / `GameplayScene.cpp` 部分 ECL 仍保留 seat-zero 兼容。它们不是 Launcher 通用规则。P1 剧情权威、资源转移 tie-break 都不能自动外推成全部敌人瞄准规则。

## 6. 仅纯锁步与实际通道延迟

`adonis-adaptation.md` 已有完整流程：资源 / 世界准备、真实输入链路 HELLO、稳定等待和 129 轮测量、proposal / accept / commit / ack、全席位帧零屏障。3P 必须包含 P2–P3；使用本地高精度单调时钟，严格遵守有效样本、中断、路由变化与超限拒绝，不按手机数或 Launcher RTT 猜 D。

纯模式 `P = 0, D = B`，其中 `B = max(1, ceil(floor(max_required_RTT_us / 2) * 60 / 1000000))`。手动 D 依声明验证，自动溢出显式失败。每个输入只采样一次归属 N+D，缺真实输入不能预测前进；Replay / spectator 不再加 D。对局内冻结 D，相位修正只调整壁钟调度，保持权威 60 Hz。

必须绕过 undo 分配、捕获、恢复和修正；资源回收、确认音频 / 文件、Replay / 观战仍需 owner。Launcher 声明、UI、配置校验和 native 必须一致限制纯模式，不能仅默认关回滚却仍接受 hybrid，也不能加 TH11 特判绕过能力模型。

## 7. 完整义务与后续验收

MP profile 仍要求：独立 Runtime / build / storage / Replay 身份、普通 / MP DATA layout 一致、Catalog 人数 / 难度 / 机体与双语标签、准备和帧零同步、每席确定性输入 / mismatch、同步 pause / restart 新 generation、MP Replay、显式 start-only 观战准入与只读隔离、WebRTC / 共享 Relay fallback、本地玩家可见性 / 诊断 / 返回房间生命周期。不能用 spectator=false 掩盖未完成功能。Relay 使用 `EAGLER_NETPLAY_*`，不新造 title 服务命名。

复用 Launcher 现有触控 / 导航 / Package / 存储；逐席保持菜单、对话、StageClear、混合输入、Deathbomb、Always Hitbox 与显示工作不推进 gameplay 等 adapter 不变量。先将用户确认的第 5 节写回玩法与声明，再按 owner 做独立构建、逐席状态、纯锁步与测量、合作玩法、Launcher / Replay / spectator，最后验证普通 / MP 隔离和完整生命周期。

| 本次证据 | 状态与边界 |
| --- | --- |
| 规则、基线与参考 ref | 已核对；文档新增前 TH11 MP 工作树干净，没有全仓 fetch |
| TH11 原作 owner / SHT 差异 | 静态审阅完成，不是多人运行证据 |
| TH11 MP gameplay、原生网络协议、Launcher 接入 | 尚未实施 |
| 构建、native、Replay、browser、2P / 3P、RTC / Relay | 本审计未运行，不能写 PASS |
| 真实设备、长程游戏、公开网络 | 未验证 |
| Canonical 晋升、push、发布 / 部署 | 未执行 |

后续先做最小可证伪门禁，再覆盖实际世界的 2P / 3P、帧零、restart / death / Result、Replay / spectator 与 Launcher。记录首个分歧帧和 owner，保留失败、unknown、未运行结果。房间加入、可见画面和构建成功不等于真实同步或设备验收；实验晋升另按隔离 playbook 完成。
