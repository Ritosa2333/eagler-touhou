# 全作品 Replay 逻辑检验系统规划书

日期：2026-09-21

状态：公共第一阶段及 TH06/TH07/TH08/TH10 全部内置 Demo 闭环已实施。

长局执行状态以 [2026-09-21 复核记录](replay-verifier-long-playback-review-2026-09-21.md)
及各作已发布 golden manifest 为准：TH08/TH10 的 Lunatic、Extra 已完成当前
Eagler 与原版的逐 tick 对照并成为日常基准；TH06/TH07 零售长局仍未完成。
历史 TH07 行为重构的失败不能记作原版或当前 Eagler 的失败。

首批作品：TH06、TH07、TH08、TH10；后续作品使用同一接入协议。

截至 2026-09-21，`testkit/replay-verifier` 已有严格 trace 契约、流式比较、
JSONL、报告、CLI、adapter 生命周期 framing 和负向测试。TH08 已把历史原版
Present oracle 与当前 Browser Runtime 接入同一比较路径，四个内置 Demo 共
35,974 个逻辑 tick 全部通过；TH10 四个内置 Demo 又有 12,000 tick 通过。
TH07 三个内置 Demo 的 20,097 tick 也已通过只读 Present 原版观察器与候选
观察出口的逐 tick 对照。TH06 使用 D3D8 设备创建后的只读 Present observer，
唯一内置 Demo 的 3,720 tick 已与 base/single-player Web 诊断构建全部通过。
当前覆盖仍是迁移期的有限字段 schema。TH08/TH10 已发布内容寻址、压缩的
Demo/Lunatic/Extra golden set 及 quick/daily 入口；C++ 公共编码器、XXH3、
全量实体 schema 以及 TH06/TH07 规定长局仍未完成，不能把现状扩大解释成
全作品、全字段已认证。

## 1. 目标和交付边界

建立一套可随各作仓库独立使用的轻量逻辑检验系统：播放指定 Replay，与原版产生的逐逻辑帧基准比较，报告最早被观测到的状态分歧，并提供定位所需的实体字段和 RNG 证据。

正式交付包含两部分：

1. 公共系统：协议、规范化编码、摘要、基准文件、执行调度、比较器、报告和适配契约测试，统一归属 `eagler-common`。
2. 作品适配：原版采集器、移植版逻辑帧观察接口、字段映射、Replay 生命周期和测试样本，分别归属各作仓库。

日常检查只需要移植版和已认证基准。生成或更新基准时才需要原版环境。检验器不要求普通玩家安装原版执行器，不进入 Launcher 的正常游戏流程。

首批语义范围为原版单人 Replay 和 Demo。多人是隔离编译的独立运行时，不要求
与原版单人轨迹一致；若以后需要，只建立自身确定性 profile。触摸扩展 Replay、
练习补丁或辅助路线也使用显式独立 profile。本系统不修改现有 Replay 文件格式，
也不将 TH06/TH07 的扩展协议引入 TH08/TH10。

检验发现逻辑错误后，修复仍在该作权威 owner 中完成。不能在比较器中增加帧偏移、忽略字段或强制写回 RNG 来使错误通过。

「全作品通用」意味着新增作品不修改公共比较器；不意味着同一内存布局、同一 Replay 解析器或同一个原版执行后端能够覆盖所有作品。

## 2. 总体体系和代码归属

```text
原版 EXE + 原版资源 + Replay
             │  各作 OriginalProvider
             ▼
     原版逐帧标准化记录 ──→ 基准认证 ──→ 不可变 golden bundle
                                             │
移植版 + 相同资源身份 + 相同 Replay             │
             │  各作 CandidateProvider        │
             ▼                               ▼
     移植版逐帧标准化记录 ───────────→ 公共流式比较器
                                             │
                          PASS / 最早分歧 / 未完成 / 不兼容
                                             │
                         重跑指定窗口 → 详细字段与 RNG 诊断
```

目标目录如下；已落地部分使用现有 `testkit/replay-verifier` 扁平目录，后续在
不破坏公开契约的前提下补齐：

```text
eagler-common/
  include/eagler/diagnostics/replay/    C++ 编码、摘要与采集 sink 契约
  src/diagnostics/replay/              不含游戏结构的公共实现
  testkit/replay-verifier/
    contracts/                        manifest / trace / result schemas
    runner.mjs                        生命周期、流读取、预算、取消
    compare.mjs                       帧完整性、类别摘要、详细字段比较
    golden.mjs                        基准身份校验、认证、读取
    report.mjs                        JSON 与 Markdown 报告
    cli.mjs                           公共命令入口
    tests/                            合成轨迹、负控、跨语言编码测试
    templates/                        新作品 adapter 和 manifest 模板
  docs/replay-verifier-plan.md

thXX-eagler/
  <现有源码目录>/diagnostics/          作品状态读取与 tick 发布
  tools/replay-verifier/
    adapter.mjs                       作品唯一注册入口
    original/                         原版后端包装、版本与地址映射
    candidate.mjs                     native / Web 驱动
    state-schema.json                 字段语义、类型、类别、比较规则
    profiles/                         原版版本及资源身份配置
  tests/replay-verifier/
    corpus.json                       Demo / Lunatic / Extra 等样本目录
    baselines.lock.json                被接纳基准的内容哈希和位置
    fixtures/                         可分发的小型测试数据
    contracts/                        本作语义与边界测试
```

游戏仓通过现有 `third_party/eagler-common` 固定 Git revision；本地开发继续支持 `EAGLER_COMMON_ROOT`。新增独立 CMake 组件 `eagler::replay_diagnostics`，不得要求游戏启用 netplay 或链接网络传输组件。

统一命令和比较器只有一个 authority，各作 `adapter.mjs` 提供布局与生命周期差异。正式入口不能依赖 `D:\workspace\...` 绝对路径；本机原版路径由未入库配置或命令参数提供。

基准必须按内容哈希管理。压缩后足够小且可合法分发的基准可以直接随作品仓库
发布，TH08/TH10 当前采用此方式，使独立 clone 能直接运行日常比较；超过仓库
预算的后续基准改用内容寻址的发布资产或构建产物存储。运行产物不写入玩家
存档目录。

### 2.1 复用现有公共诊断体系

`eagler-common/testkit/presentation-lab` 已确立「公共控制器 + 游戏驱动 + 游戏状态解释」边界，应沿用其版本化契约和完整 tick receipt 语义。

各作 Replay 检验和 Presentation Lab 应调用同一个作品级完整 tick 驱动，不各自实现一次裸 `RunCalcChain()`。完整 tick 必须覆盖原有输入、调度、队列及状态发布事务。

Replay 检验不需要实现截图、alpha 扫描等全部 Presentation Lab 接口。先复用确有共同语义的驱动与报告辅助函数；两种工具的业务协议分别维护。只有两个消费者确认一致后才提取新的公共生命周期模块。

TH08/TH10 的既有 JIT、Win32、D3D 支持应保留并包装现有入口。先接通，再提取已证明相同的基础设施；不把重写统一 x86 执行器作为前置项目。

## 3. 公共接口

每个作品 adapter 注册以下能力。名字为规划契约，落地时须同时提供类型声明、运行时校验与示例。

| 边界 | 必须提供的行为 | 不负责的事项 |
| --- | --- | --- |
| `describe()` | 游戏 ID、adapter 版本、支持的 EXE profile、schema、能力与缺失项 | 判断所有游戏的字段含义 |
| `inspectReplay(bytes)` | 校验并解析角色/配置/难度/关卡/输入范围，返回内容哈希 | 改写 Replay 内容 |
| `OriginalProvider.open(spec)` | 校验原版资产，启动既有后端，返回记录流和关闭句柄 | 浏览器 fixed-step 调度 |
| `CandidateProvider.open(spec)` | 启动 native 或 Web 诊断构建，返回相同语义的记录流 | 模拟另一套游戏逻辑 |
| `StateEncoder` | 按本作 schema 读取并编码权威字段 | 通用比较策略和报告生成 |
| `captureDetail(window)` | 从相同起点重跑，输出指定区间实体字段 | 用不完整快照跳过前面的逻辑 |
| `captureRng(window)` | 可选：调用序号、种子、结果和调用位置 | 每帧矫正 RNG |

`open` 返回异步记录流、取消和关闭接口；原版进程、JIT 会话、native 文件及 Web 二进制批次均可承载该流。公共代码不理解游戏菜单、WASM export 名称、固定地址或角色编号。

协商能力包括 `fullTickTrace`、`details`、`rngCalls`、`native`、`web`、`renderEvidence`。不支持某能力时明确报缺失；禁止静默降级后仍声称完整逻辑验证通过。

## 4. 时间、输入与生命周期协议

### 4.1 两端的采样所有权

原版侧沿用 D3D Present 驱动和观测：TH08/TH10 的既有 oracle 已采用该路径，用户也明确确认原版逻辑与 Present 的结合适合作为采集边界。

移植版侧在完整固定逻辑 tick 完成后发布记录，浏览器 RAF、Draw、插值和 Present 不负责发布逻辑帧记录。

各作必须验证「原版 Present 读到的字段」与「移植版 tick 完成后的字段」处于相同语义阶段。若原版 Draw 内仍有影响下一 tick 的权威写入，要映射至移植版已迁移的逻辑 owner；不能把整个原版 Draw 粗略视为永远纯读。

帧对齐不能只依赖 Present 次数，也不能直接比较不同格式的 Replay 字节偏移。

### 4.2 帧标识和收据

记录分为 `run-start`、`segment-start`、`tick`、`segment-end`、`run-end`；segment 是一次关卡实例，包含实际分支和进入序号。同一关重试、重新进入或不同结局分支使用不同 segment。

每条 tick 记录至少包含：

```text
sequence                 本次运行中严格递增的标准记录序号
segment_id               双方按同一关卡生命周期规则生成
logical_tick             本 segment 中完整逻辑事务的标准序号
source_clock             原版/移植版实际的游戏、关卡、控制帧计数
replay_sample_index       语义化输入样本序号；变化点格式不可用字节偏移代替
source_replay_cursor      原始 cursor，仅作本作诊断证据
applied_input             本 tick 实际作用于玩法的输入
clock_disposition         advanced / skipped / paused 等本作明确映射
```

比较键使用 `(segment_id, logical_tick)`，同时严格校验语义化 Replay 样本序号、适用的 source clock 和输入。`sequence` 用于发现丢包、重复或乱序，不能独自证明游戏帧对齐。

未完成的加载过程、没有推进的重复 Present、暂停或 calc-chain 提前退出必须提供显式控制收据，不能伪造成功 tick。具体作品中哪些计数器仍会前进，由 adapter 文档规定。

TH07 FPS side-stream 和 lag skip 是 Replay 语义的一部分。按记录值执行，不能用测试机器实测 FPS 替换。诊断快进只能完整执行既有事务，不能绕过 lag 分支或生产 Replay 的 cadence 规则。

段结束必须声明原因和已处理的输入范围。达到帧数上限、超时、进程退出均不等价于 Replay 正常结束。Demo 正常回标题应由本作 manifest 指定为允许终态；长局要求覆盖 manifest 指定的全部关卡和结局范围。

## 5. 状态协议与轻量实现

### 5.1 公共类别，作品自有字段

协议统一类别和容器格式；每作 schema 定义字段清单、来源、类型和有效条件。

| 类别 | 要覆盖的内容 |
| --- | --- |
| `clock-input` | 生命周期、输入、Replay 进度、逻辑时钟与影响模拟的标志 |
| `rng` | 所有能影响玩法的 RNG 状态和可取得的调用计数 |
| `player` | 位置、速度、状态、命中/Bomb、资源、子机及自机弹 |
| `world` | 分数、rank、难度、关卡分支、全局计时和流程状态 |
| `enemy-script` | 敌机、ECL/脚本位置、变量、栈、等待状态和计时 |
| `projectile` | 敌弹、激光、状态、运动参数、生命周期和生成相关数据 |
| `item` | 物品槽、种类、位置、回收/吸附状态及计时 |
| `aux-gameplay` | 本作其他会影响后续模拟的 Bomb、结界、effects/ANM 等 |

视觉特效若共享 gameplay RNG 或参与后续逻辑，就不能因为名字像渲染而排除。TH10 独立视觉 RNG 可进入可选渲染验证层；玩法 gate 必须覆盖脚本 RNG。

active 数量和位置相同不代表内部状态一致。每个类别都要列出完整字段覆盖表和已知缺口；释放槽中会影响下次分配的字段、空闲链表顺序等也属于潜在权威状态。不会再读取的陈旧字节才可忽略。

### 5.2 规范化编码

- 字段按 schema 固定顺序编码，整型明确位宽、符号和 little-endian；布尔编码为 0/1。
- 浮点默认比较实际存储精度对应的 IEEE 位模式，保留有符号零和非有限值证据。禁止全局 epsilon 或四舍五入掩盖分歧。
- 对行为还原中确有不同表示的字段，在映射表中明确共同语义与转换证据；有损/容差比较单列为诊断结果，不能冒充 bit-exact PASS。
- ECL 指令地址转换为「脚本资源 ID + 指令偏移」；对象引用转换为已定义的稳定逻辑身份。原始指针只作诊断附件。
- 固定池使用 slot 和必要的生命周期标识；动态对象以双方可独立复现的分配序号/逻辑列表语义识别，不能为消除差异随意按位置排序。
- 不直接散列整个 C++ struct。排除 padding、未定义字节、纯插值缓存与设备状态。

TH06/TH07 现有 CanonicalHash 可参考遍历代码，但必须重新审计字段。含 `prevRenderPos` 等表现缓存的摘要不能直接成为跨实现 golden 协议。

### 5.3 摘要、存储和性能

摘要采用固定版本的 XXH3-128、固定 seed 和统一 digest 编码；使用成熟实现，不自行设计散列。官方实现支持跨平台一致结果，但前提仍是输入规范化字节一致。[xxHash 官方说明](https://xxhash.com/doc/v0.8.2/group___x_x_h3__family.html)

EXE、资源、Replay、schema 和整个基准文件的身份使用 SHA-256。基准完整性与快速游戏状态比较使用不同职责的哈希。

同一个公共编码/散列核心供 native C++ 与诊断 WASM 使用；原版 JS adapter 可将规范化记录交给同一 WASM 核心。提供跨 JS/native/WASM 的固定字节向量测试，避免两套算法偶然一致。

每帧保存少量标量和各类别 128-bit 摘要。对象字段逐个输入流式 hasher，无须每帧构造完整 JSON；默认不保存全部实体。trace 使用有版本的分块二进制，metadata/report 使用 JSON，压缩在 tick 外进行。metadata 使用 JSON Schema 2020-12 与成熟 validator 校验。[JSON Schema 规范](https://json-schema.org/draft/2020-12)

预计每帧摘要记录控制在 512 字节以内，100,000 帧约 51.2 MB 压缩前；这是格式预算，实际大小须实测。默认 CPU 额外开销目标不超过同机相同场景的 5%，每帧无 heap 分配；若未达标，优化编码和传输，不能降低成每 30 帧采一次。

诊断编译开关关闭时不编入采集器或导出接口。native 可流式落盘；Web 使用有序、有上限的二进制批次，批次背压使测试驱动让出执行，不能丢弃帧。运行中的未落盘缓冲必须有固定内存上限。

摘要有碰撞可能，PASS 只能表述为「声明字段覆盖内的逐帧摘要一致」。需要逐字段确认时重跑详细记录，不能宣称摘要等于形式化证明。

## 6. 基准生成、认证与比较

### 6.1 基准身份

golden bundle 包含：

```text
manifest.json        schema/profile/资源/Replay/工具身份与字段覆盖
trace.bin[.gz]       每逻辑帧摘要及生命周期事件
completion.json     关卡、帧范围、预期终态、实际终态、完整性摘要
certification.json  两次原版运行的一致性证据、审核记录
```

manifest 必须记录 EXE 和有关资源的哈希、语言/版本、Replay 哈希、角色/配置/难度、路线分支、启动/存档条件、原版后端 revision、address-map revision、adapter revision、schema 哈希及 instrumentation profile。

资源经过解包/转换时，记录从原始内容到候选资源的可验证映射；不要求 DAT 与转换后资源文件的字节哈希相同，也不能只靠文件名声称内容等价。

golden 身份不绑定某一个候选提交；候选构建提交、common revision、编译选项和运行平台写入本次结果。未知原版版本或不匹配 schema 必须在比较前拒绝。

### 6.2 认证流程

1. 解析样本并核实 EXE/资源身份；使用独立测试存档配置。
2. 原版从同一起点完整播放两次，对逐帧摘要、输入范围和终态做一致性检查。
3. 检查 instrumentation 不改变原版轨迹。补丁/钩子的位置和作用明确入档；修改生命、火力或自动 Bomb 的路线不认证为普通 Replay 基准。
4. 验证基准所有必需类别均覆盖，且无丢帧、截断或非正常终止。
5. 生成不可变 bundle，显式更新作品 `baselines.lock.json`。

原版 JIT 也是需要认证的执行环境。TH08/TH10 历史对照产物作为支持证据，并为每种原版后端至少保留零售进程检查点/已有可信对照的交叉验证记录；只有同一 JIT 重复一致不能独自证明后端语义正确。

仅有候选重复运行一致时，报告为 `candidate-repeatability`，不能登记为 `original-equivalence`。既有每 30 tick 的稀疏报告可作迁移交叉检查，不能认证为逐帧基准。

普通检查命令绝不自动更新 golden。修复候选导致对照失败时先分析差异；原版 profile、字段语义或采集器修正才触发显式重建和审核。

### 6.3 最早分歧与结果类型

比较器顺序验证身份、生命周期/帧连续性、输入/时钟、RNG 与各状态类别。任何必需记录缺失、额外帧或关卡遗漏都需报告；不能自动平移轨迹寻找匹配。

结果枚举：

- `PASS`：指定覆盖、样本范围与平台全部满足且完整结束。
- `DIVERGED`：给出最早观测到的不等帧、类别、双方摘要/标量。
- `INCOMPLETE`：超时、截断、缺帧或未达预期终态。
- `INCOMPATIBLE`：版本、资源、schema 或必要能力不匹配。
- `ERROR`：启动、工具或采集器故障。

非 PASS 的 CLI 均返回非零退出码，并在 JSON 中区分原因。覆盖不足不转换为完整 gate PASS。

suite 中缺少必需 Replay、基准或运行环境时，整组标记未完成/不兼容并失败退出，不能通过跳过样本获得绿色结果。可选样本允许跳过，但报告必须列出，且不计入覆盖率分母中的已完成数量。

首次不等帧 F 出现后，从已认证的同一起点重跑，默认采集 `[F-120, F+30]` 详细状态。区间可配置。没有已验证的完整恢复快照时，必须执行此前全部 tick。

报告展开为「类别 → 稳定实体 ID → 字段 → 期望/实际值」。RNG 窗口诊断根据调用序号追踪多调/少调；日志只能观察，不能再调用游戏 RNG。

F 是当前覆盖下最早观测分歧，不一定就是根因发生的帧。若之前隐藏字段或 RNG 调用路径已不同，需要扩大字段覆盖或启用更细的调用诊断。

## 7. 各作适配工作

### 7.1 TH08：首个闭环，复用最直接的 Replay oracle

原版来源目录：`D:\workspace\东方测试.zip\TouhouDev\th08_web`。

复用 `scripts/cpp/native-replay-oracle.mjs`、`compare-replay-oracle.mjs`、RNG/Replay diagnostics 及 `scripts/native` 执行支持。将路径和输出注入参数化，保留当前已验证的原版 Present 驱动。

移植版 authority：`th08-eagler`。从 `th08_web/cpp/sdl/GameHost.cpp`、`game/GameplayScene.cpp` 的完整 tick 生命周期发布；`platform/BrowserExports.cpp` 的 trace 和 audit_state 作为现有字段锚点。

工作项：

1. 对照既有字段定义，建立原版地址 → 语义字段 → 候选 owner 映射。
2. 补足 ItemPool、脚本执行状态和其他遗漏的权威字段；不把现有有限摘要当成完整覆盖。
3. 四个内置 Demo 接入同一 CLI，保存各自真实终态和有效帧数。
4. 接入分支路线与长局；Final A/B 属于路线分支，不能当成新的角色或新的难度。

普通 Replay 没有每帧输出 oracle；调试格式存在不等于官方 Demo 含有该信息。以原版外部基准为正式依据。

### 7.2 TH10：第二个消费者，检验协议是否过度贴合 TH08

原版来源目录：`D:\workspace\东方测试.zip\TouhouDev\th10_web`。

复用 `scripts/native/session.mjs`、`tests/browser-world-oracle.test.mjs` 与 `scripts/native/rebuild-game-probe.mjs`。当前 session 预期资产目录缺失，但零售资产在 `D:\workspace\original touhou\th10`；正式实现改为传入 asset root 并校验 profile。

逐帧 world oracle 已比较 RNG、敌机、自机弹、玩家和绘制包；保留它作为 adapter 交叉验证。原有 Replay probe 每 30 tick 的采样必须扩展为完整 Replay tick 流，不能只换报告格式。

移植版 authority：`th10-eagler`。检查 `th10_web/cpp/sdl/ApplicationHost.cpp`、`game/ApplicationLoop.cpp` 和实际 Replay 输入 owner，在完整事务出口采样。

工作项：动态敌机身份、脚本与视觉 RNG 分离、Item/faith 池与自机弹覆盖、四 Demo 目录、日文/汉化版本隔离。

原 oracle 为渲染比较进行的视觉 RNG 初始化只能出现在独立 `renderEvidence` profile。普通玩法基准不加入辅助生命、火力、自动 Bomb，也不逐帧重置任何 RNG。

### 7.3 TH06：使用完美反编译映射建立原版观察层

地址 authority：`th06/config/mapping.csv`、`globals.csv`。参考已有 `generate_detours.py` 的符号解析；不从 Eagler 对象地址反推零售 EXE。

原版 provider v1 规划为本地 Windows 零售进程观察器：Present 边界、必要逻辑计数器和字段只读采集。地址表生成与 hook 安装明确分开；先验证 build identity、ABI、挂钩前后寄存器/浮点环境和轨迹一致，再认证基准。

移植版接入 `src/GameWindow.cpp` 完整事务，参考 `src/netplay/Th06CanonicalHash.cpp` 的状态清单。Demo 输入回调位置与 TH07 不同，必须验证本 tick 实际输入与 Replay cursor 的对应关系。

工作项：一个内置 Demo、输入变化点解析、stage 快照恢复、Item 在 Bullet 更新内的嵌套顺序、完整池/脚本字段映射。普通 Replay 本身不提供逐帧结果验证。

如果只读外部观察无法取得所需完整帧，先解决采集边界；不能静默降级为异步轮询内存。也不以移植版生成所谓原版基准。

### 7.4 TH07：复用 TH06 的进程观察基础设施，保留 AUX 优势

地址 authority：`th07/resources/csv/funcs.csv`、`globals.csv` 和原源码函数/全局地址标注。与 TH06 共用已经验证的 Windows 观察器基础设施，作品地址、结构和帧语义分别维护。

移植版从 `src/GameWindow.cpp`、`Th07DeterminismProbe::AfterSimulationTick` 和 `Th07CanonicalHash` 的状态清单接入。

保留现有 AUX 检查，作为不需要外部基准的快速事件检查；同一报告列出 `aux-events` 与 `original-equivalence` 两种证据。AUX 一致不能代替玩家/敌弹轨迹与脚本状态一致。

专项工作：Replay FPS/lag skip、普通 Replay 与 Demo 调度区别、结界/Bomb/物品事件、同关重入及 Phantasm 路线。三个内置 Demo 全部纳入。

### 7.5 后续作品接入清单

新增一作只需新增注册 adapter、字段映射、原版 profile 和 corpus，完成以下步骤：

1. 确认 Replay 解析和真实输入 owner，列出所有逻辑时钟与正常结束条件。
2. 接入可信原版 provider，注册可比较字段和明确的覆盖缺口。
3. 在生产逻辑事务出口加入只读诊断发布，接通公共编码器。
4. 原版重复认证和采集无扰动检查通过。
5. 接入所有 Demo 与规定长局，完成负控及 native/Web 对照。
6. 固定 common revision、adapter/schema 版本与 golden 锁文件。

若第七步变成修改公共比较器里的 `if game == ...`，说明作品语义泄漏，必须回到 adapter 边界处理。

## 8. Replay 样本矩阵

用户的「第三个难度」按额外特殊难度规划，例如 TH07 Phantasm；上份交接推测为第三角色不准确，本计划不沿用。角色/机体选择单独记录。

| 样本组 | 首批要求 | 通过条件 |
| --- | --- | --- |
| `demo` | TH06 1、TH07 3、TH08 4、TH10 4，共 12 个已定位内置 Demo | 各自完整播放至预期终态，逐帧比较 |
| `lunatic` | 第二角色第二配置，完整主线 | 全段连续，记录真实路线和结局 |
| `extra` | 同配置完整 Extra | 全段连续，达到 manifest 终态 |
| `special` | 有额外特殊难度的作品，例如 TH07 Phantasm | 对应难度完整覆盖 |
| `semantic` | lag stream、stage 重入、分支等本作边界 | 对明确语义单独验收 |

TH08 的队伍/单人选择不与其他作品「角色 × 机体」一一对应。适配时以实际选择菜单/Replay 元数据制定显式 loadout 映射，不直接把数值 1 或 shot_type 1 称为 MarisaB。补齐样本前该行保持缺失状态。

规定长局样本现已从 Royalflare 公开档案补齐并完成格式级验证：TH06、TH07、
TH10 使用 MarisaB Lunatic/Extra，TH07 另有 MarisaB Phantasm；TH08 按本作
实际选择结构使用第二组 Magic Team（Marisa & Alice）Lunatic Final B/Extra。
格式、版本、校验和/解压、loadout、难度和完整 stage 集均已核实并进入各作
corpus。TH08/TH10 已完成双方全程采集和当前有限字段 schema 的 golden 对照；
TH06/TH07 仍不得只凭 Replay 文件就标记长局 oracle gate 通过。

所有 Replay 登记来源、SHA-256、游戏版本、完整/练习、slowdown 和终态。额外路线可逐步增加，但所选有限样本只证明对应覆盖，不能宣称穷举全部角色和弹幕。

## 9. 使用方式和持续集成

正式测试分为三个层级：

1. `quick`：运行全部内置 Demo，与已发布 golden 比较；用于开发中的快速门禁。
2. `daily`：运行规定的 Lunatic、Extra 和作品特有特殊模式，与已发布 golden
   比较；这是常用的完整逻辑回归，不需要原版环境。
3. `oracle`：从原版重新采集并审核 golden；这是低频进阶维护，不能由普通候选
   测试自动触发，也不能自动覆盖已发布基准。

`.rpy` 是输入样本，golden trace 是日常预期结果，oracle capture 是生成预期
结果的维护过程；三者在命令、目录和报告中必须保持区分。

以下为计划命令，不是当前已经可以执行的工具：

```text
node <common>/testkit/replay-verifier/cli.mjs doctor --adapter <game-adapter>
node <common>/testkit/replay-verifier/cli.mjs inspect --adapter <game-adapter> --replay <file>
node <common>/testkit/replay-verifier/cli.mjs golden generate --adapter <game-adapter> --suite demo --assets <local-config>
node <common>/testkit/replay-verifier/cli.mjs golden certify --bundle <bundle>
node <common>/testkit/replay-verifier/cli.mjs check --adapter <game-adapter> --suite demo --target web
node <common>/testkit/replay-verifier/cli.mjs diagnose --report <report.json>
```

各作可以保留一个命令转发入口，不能复制 runner/compare。跨作汇总是同一个 CLI 接收多个 adapter 配置，无须开发第二套工作区检查器。

CI 分为：公共协议测试、各作 Demo gate、完整长局 gate、原版基准认证。普通 PR 运行受影响作品的 Demo；common 协议/编码/调度变更运行四作 Demo；发布前运行规定长局。原版认证在具备本地合法资产的维护环境运行，普通 CI 消费已认证基准。

Web 必须实际运行当前 Web 诊断构建。native 通过只能证明该 native lane。至少在首次接入及调度变更时比较相同 Replay 在正常、额外 Draw 和不稳定呈现节奏下的逻辑记录；不改写生产 cadence 以制造高吞吐量。

新增游戏适配完成后需同步更新正式接入指南；Launcher 不增加原版采集或测试管理职责。

## 10. 实施顺序与阶段验收

| 阶段 | 交付物 | 完成条件 |
| --- | --- | --- |
| P0：证据与边界冻结 | 四作字段/时钟映射、版本 profile、12 Demo 清单、长局缺口表 | 原版身份可验证；采样边界能逐项解释；所有未知项明确登记 |
| P1：公共协议 + TH08 闭环 | 公共编码器/比较器/CLI、TH08 双端 adapter、四 Demo golden | 同一逻辑可完整逐帧比较；故意扰动在预期首帧被检出；有限覆盖明确标注 |
| P2：TH10 第二消费者 | TH10 原 oracle wrapper、动态对象映射、四 Demo | 无游戏分支进入公共比较器；旧逐帧 oracle 与新记录交叉一致 |
| P3：TH06/TH07 | 共享 Windows 原版观察器、CSV 地址生成、两作 adapter、AUX 桥接 | 全部 12 Demo 通过规定覆盖；原版采集无扰动；TH07 lag 语义通过 |
| P4：完整覆盖与长局 | 完整字段审计、Lunatic/Extra/特殊难度基准、分支/结束检查 | 所有规定 fixture 完整通过；缺失样本和字段清零才能声称四作适配完成 |
| P5：正式交付 | 依赖锁定、构建/CI 接入、性能报告、模板与使用文档 | 各作独立 clone 可用；生产构建无诊断接口；新作按模板接入无公共算法修改 |

P1/P2 可以产出有限字段的可用报告，但不得提前标记完整适配完成。整体完成按 P4/P5 判定。

公共机制与 gameplay 修复分成独立变更；baseline 更新也独立审查。这样可以辨别是游戏修复、观察器修正还是期望值变更。

## 11. 必需验证与风险处理

| 风险/错误类型 | 必需验证 |
| --- | --- |
| 观察器改变 RNG、浮点状态或游戏行为 | 同起点采集开/关及摘要/详细模式对照；hook 保存环境 |
| 编码器读错字段而双端同错 | 原版已有 oracle/明确字段样例交叉检查；映射审查；独立负控 |
| 丢帧、重复、乱序、提前结束 | 合成 trace 注入缺陷；不得出现 PASS |
| 一次额外 RNG 调用 | 测试专用受控负控；在首次 RNG 不等帧报告，不只等最终轨迹变化 |
| 不耗 RNG 的确定性位置错误 | 单 tick 位置/计时负控；证实实体摘要确实覆盖 |
| 槽位复用和同关重入 | spawn/despawn、slot generation、segment reset 测试 |
| Draw 影响模拟 | 额外 Draw、可变 alpha、不同呈现节奏后逻辑摘要仍一致 |
| x87/native/WASM 数值差异 | 位模式定位首差；保留 strict 失败；在作品语义层查根 |
| 基准被候选错误污染 | 原版-only provenance 与显式认证；候选不能写入 golden authority |
| 超预算 | 记录单 tick 采集耗时、峰值内存、输出大小；优化实现，不抽帧掩盖 |

完整报告必须附带 build、平台、fixture、profile、schema、覆盖清单、终态、首差及可复现命令。报告中有未知覆盖时不能写「100% 全作一致」。

## 12. 现有依据与交付记录

本方案依据以下现有文档/实现制定：

- `eagler-common/README.md`：公共基础设施单一归属、游戏布局归作品、submodule 固定 revision。
- `eagler-common/testkit/presentation-lab/CONTRACT.md`：完整 tick 事务、诊断隔离、证据覆盖和对象身份。
- `eagler-touhou/docs/playbooks/replay-determinism.md`：逻辑帧 ownership、首差与表现层排除。
- `eagler-touhou/docs/playbooks/testing-acceptance.md`：验收证据维度与失败纪律。
- 工作区 `docs/replay-determinism-audit.md`：TH06/TH07 Replay cadence、TH07 FPS/lag、输入独占和生命周期。
- `TouhouDev/th08_web/scripts/cpp/native-replay-oracle.mjs` 与比较/诊断入口。
- `TouhouDev/th10_web/tests/browser-world-oracle.test.mjs`、`scripts/native/session.mjs`、`rebuild-game-probe.mjs`。
- TH06/TH07 原始仓 machine-readable 地址表及现有 canonical/probe 状态遍历。

历史 oracle 是设计依据；本轮又实际执行了四作当前 Demo gate。通过范围仅限本轮
列出的有限字段和内置 Demo，不能据此宣称完整字段或规定长局已经认证。外部
`TouhouDev` 目录仅为既有工具来源，迁入时记录来源 revision/文件哈希和许可；
当前 runtime owner 仍是各 `thXX-eagler`。

本轮已经实施公共比较器、四作 adapter/语料目录和隔离的只读观察入口；
TH08/TH10 又发布了可独立使用的 quick/daily golden。未修改任何作品的正常
Demo 数量、轮换、Replay 消费、输入、RNG、tick 调度或游戏状态。只有显式
诊断构建暴露只读字段，原版 Present observer 仅记录状态。
