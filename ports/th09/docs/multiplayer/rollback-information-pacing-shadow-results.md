# TH09 信息预算影子实验：执行结果与后续判断

日期：2026-10-11。范围：一次预声明离线分析和一次派生摘要提取；未改变生产策略、harness、输入延迟或历史窗口，未启动浏览器/构建，未重跑原始实测。

## 1. 决策

**不要直接上线本次“历史 gap 条件 p90 / overdue”预警规则。** 在旧 burst-response 的完整报告中，两端共出现 99 个区间首预警，没有取得可确认的首预警模型机会：82 个闭合且观测字段未标污染的区间，下一次 A 都在保持当前计划速率的一帧安全余量预算内推进；另外 17 个区间后续发生观测污染，应保留未知，不能计成成功或误报。

但不能据此说全部机会为零。对所有点及从未预警的 gap 独立回看，burst 两端各有 1 个满足预声明严格条件的模型机会，且规则都漏掉了。两点均已有 32 个合格历史 gap，条件 survivor 分别为 32 和 13，因预测剩余时间低于 hold 预算而没有发出预警。这里的主要问题是这些历史信号没有选准少数未来尾部，不是正文尚未暖机。

这些是旧轨迹上的连续预算模型结论，不能换算成反事实 FPS、输入延迟收益、避免了多少实际停顿或已验证的网络根因。来源：[派生摘要](rollback-information-pacing-shadow-summary.json) 的 `results[0].peers[*].ranges.all`、`ranges.body` 和 `noWarningGapRetrospective`。

## 2. 执行与身份

| 操作 | 结果 | 记录 |
|---|---|---|
| `node --check .../analyze-information-pacing-shadow.mjs` | exit 0；无输出错误 | [syntax](rollback-information-pacing-shadow-syntax.json) |
| 预声明 analyzer，正式执行一次 | exit 0；12 个数学/区间契约通过；Node elapsed 838 ms | [execution](rollback-information-pacing-shadow-execution.json) |
| 只读派生 evidence 的摘要提取一次 | exit 0；Node elapsed 141 ms；未重新读取 raw 或计算预警模型 | [summary execution](rollback-information-pacing-shadow-summary-execution.json) |

正式执行记录的 Node 文件版本为 24.13.0。四端样本/每样本 pacing 行/有效读取区间分别为 3997、3897、5765、5749；所有源校验、上下文校验和重复读取区间检查均无错误。analyzer 与 plan 在正式执行前后 SHA 相同。没有失败被删除或重试。

| 产物或输入 | 字节数 | SHA-256 |
|---|---:|---|
| `analyze-information-pacing-shadow.mjs` | 32,764 | `d0c0cb5eea57b89b138b3f53ce8d763789a1325bea680818543fc31fb99674e9` |
| [原始预声明 plan](rollback-information-pacing-shadow-plan.json) | 9,982 | `3c242cc7974c9cd546a43bd6dd6c8aa70fbc0f8a8103d17e5b4bef27e3361139` |
| [完整 shadow evidence](rollback-information-pacing-shadow-evidence.json) | 11,695,729 | `e08302b1efce870057e8197eb8d17d10f66f20d133c3939fe3433356b54fda6c` |
| [派生 summary](rollback-information-pacing-shadow-summary.json) | 161,764 | `00e8543522581189a1ad2230ca119538edb18daf7f4fcbcea2a5fda6bd844c6e` |
| [正式 execution](rollback-information-pacing-shadow-execution.json) | 5,237 | `6fc153d803571bd725e1550d359cf8562b34cff30e9692b4899ab501e44fa85e` |
| [syntax](rollback-information-pacing-shadow-syntax.json) | 519 | `30ab8746044047875459b2c4df57c24c15244394efd3d505b987884afa211933` |
| 旧 burst-response raw，路径见 plan.inputs[0] | 21,525,676 | `09bb173bee138e05916d0557c0a42d5797c6be200e4825e97e42695063582376` |
| 旧 relay125 raw，路径见 plan.inputs[1] | 28,866,220 | `d03833c48742160afa7956a70b4aa82f135069005e3b4b38422a6b3c08bca770` |

两份 raw 都先通过预声明字节数和 SHA 校验，随后才解析；对应 WASM 身份是 `47367732b9bc4972d8adf2ffc49ace32cf9eca4a3868ff73c9c6494a0de4df97`。plan 保留声明时尚未获准执行的历史状态，没有事后改参数或回写声明。派生摘要脚本 SHA 为 `50dae38eb02b14c92c97bc424ac071a71d61f49ad870c5a239f7d89444472bda`。

## 3. 连续窗口与首预警

正文继续使用第一次 F>=180 到第一次 F>=1800 的连续样本区间，不按可能回卷的 next 删点；完整报告还包含前缀与末端。原始样本下标从 0 开始。

| 报告/端 | 完整点数 | 正文范围 | 正文点数 | 首预警：完整 / 正文 | 无必要预警 proxy：完整 / 正文 | 首预警后续污染：完整 / 正文 |
|---|---:|---|---:|---:|---:|---:|
| burst P1 | 3997 | [390,3986) | 3596 | 36 / 36 | 28 / 28 | 8 / 8 |
| burst P2 | 3897 | [369,3888) | 3519 | 63 / 61 | 54 / 53 | 9 / 8 |
| relay125 P1 | 5765 | [573,5745) | 5172 | 0 / 0 | 0 / 0 | 0 / 0 |
| relay125 P2 | 5749 | [561,5729) | 5168 | 0 / 0 | 0 / 0 | 0 / 0 |

burst P1 的 36 个首预警中，2 个为 `predicted-bridgeable`，34 个为 `overdue-unknown`；P2 的 63 个全是 `overdue-unknown`。`predicted-beyond-ramp` 首预警均为 0。所有首预警的末尾右删失和时钟未知计数为 0。后续污染的 8/9 个区间没有作为误报代理或收益计数。没有通过闭合且未标污染资格的首预警落入 `model-bridgeable` 或 `interval-overlap-unknown`。

完整报告的 99 个首预警、正文的 97 个首预警和所有重复预警点是不同口径。正文重复预警点为 P1 42、P2 82，不能当作 124 次独立事件。来源：summary 的 `ranges.*.firstWarningOnly` 与 `originalPredeclaredSummary`。

## 4. 两个未发出预警的模型机会

两点均在正文，B=4、Q=Qfull=4、R=0，network scale=1000，整个所属 gap 未出现本计划可观测的污染；一帧安全余量后，预算按 3 帧计算。下表时间单位为 ms，使用同一端 Page performance 时钟的区间；表中四舍五入不改变原始判定。

| 项目 | burst P1 | burst P2 |
|---|---:|---:|
| sampleIndex | 3603 | 3032 |
| F / A | 1635 / 1631 | 1395 / 1391 |
| effective scale 千分值 | 1002 | 997 |
| 当前 gap age 区间 | [0.000,9.980] | [13.445,24.875] |
| 合格历史 gap 数 | 32 | 32 |
| 条件 survivor 数 | 32 | 13 |
| 预测剩余 p90 | 33.3475 | 21.5875 |
| 保持当前计划速率安全预算 | 50.1000 | 49.8500 |
| 按 50 ms/单位升至 2 的安全预算 | 80.6855 | 80.6853 |
| 下一次 A 推进相对决策时刻的区间 | [51.590,58.205] | [50.260,64.365] |
| 相对原 B 制动阈值的领先区间 | [5.045,23.540] | [2.040,14.610] |
| forward 预警 | 无 | 无 |

没有预警的原因可以直接复查：模型当时有足够的历史和条件支持，得到的剩余 p90 都小于 hold 安全预算，因此按冻结规则不触发；`overdue-unknown` 也不适用，因为这两点处于 supported 状态。关闭后实际 A 推进区间的下界超过 hold、上界未超过 ramp，并有严格正的原制动领先下界，所以才成为回看的模型机会。

这两个记录不能被描述为已经验证的平滑收益。原制动领先下界仅为 5.045/2.040 ms；下一 A 下界超过 hold 仅约 1.490/0.410 ms。模型没有模拟离散 Pump、已有 scheduler credit、未来 phase 变化或 CPU 工作。两者也尚未与旧报告那次 91.840 ms 新状态间隙建立事件对应关系，不能把它们当作该长间隙已可被消除的证明。

精确记录位于 summary：`results[0].peers[0/1].ranges.all.noWarningGapRetrospective.examples`。两端各只有 1 个严格机会点、1 个机会 gap，均没有任何预警；正文与完整报告计数一致。

## 5. 暖机、条件支持与观察污染

| 报告/端 | 首次至少 16 个合格历史 gap 的 sample | 全程训练接纳 / 排除 gap | 最大历史数 | 正文 history 不足点 |
|---|---:|---:|---:|---:|
| burst P1 | 336 | 958 / 583 | 32 | 0 |
| burst P2 | 314 | 1114 / 437 | 32 | 0 |
| relay125 P1 | 356 | 1083 / 589 | 32 | 0 |
| relay125 P2 | 374 | 905 / 693 | 32 | 0 |

burst 正文的制动前候选为 2732/3416 点。当前 gap 未标污染后为 2236/3002 点，其中 supported 为 2196/2920、条件尾部支持不足为 40/82、history 不足为 0。这说明候选点大多已有条件估计，不能把两个漏报或整体规则表现主要归因于未暖机。条件 survivor 门槛约束预测剩余时间；回看的严格机会只要求至少 16 个合格历史 gap，不能把这两个门槛混为一谈。

按完整 gap 统计，主要污染标记是纠错/重演：burst P1 的 replay/correction gap 各 582，native>10 ms gap 1；burst P2 分别 379/370，native>10 ms 198、sample gap>50 ms 2。relay125 P1 replay/correction 各 588、native 0；P2 分别 688/639、native 74、sample gap 1。标记可以重叠，不能把各列相加当独立排除数。原始读取区间缺失、宽区间、RAF request wait、隐藏/暂停及全局诊断无效等标记在这两份记录中为 0；未测边界仍然未知。

完整四端各保留 1 个初始左删失 gap 和 1 个最终右删失 gap；最终每端都有 1 个尚无下一 A 关闭事件的点。全部未预警 gap 数为 1505/1488/1672/1598，不能把它们统称真阴性；两端 burst 各含 1 个漏报模型机会，末端删失也在其中。出处：summary `warmup`、`ranges.all.originalPredeclaredSummary.gapAccounting` 与 `finalCensoredRows`。

## 6. relay125 为何没有严格提前机会

relay125 的正文同样已经暖机。P1 在有超过一帧余量的 4828 个点中，全部已经触发现有或本 gap 较早的制动，因此没有制动前候选。P2 的 4926 个相应点中，4922 个已存在制动，余下 4 个当前 gap 都带观察污染。

只看下一 A 区间是否处于 hold 与 ramp 之间，正文仍有 P1 24 个、P2 11 个几何候选点；但它们全部不满足“现有制动之前”的规则，因此不是提前控制机会。完整报告另有启动期几何候选被 history 暖机门槛排除，不能把启动期原因当作正文原因。

这支持当前规则在持续高延迟记录中的提前适用空间很小，并不排除改善稳态控制、输入接纳、预测历史或 CPU 成本。summary 的 sequential gate counts 只是按声明顺序统计排除项，顺序会影响归到哪个门槛，不能当作因果贡献比例。

## 7. 下一步的最小实验建议

1. 保持本规则为离线诊断，不把 overdue-unknown 直接变成降速命令，也不在已经看过的两个 raw 上提高分位数、降低触发阈值后覆盖本证据。82 个已确认无必要预警 proxy 和两个 supported 漏报，足以否决直接上线当前规则；17 个污染区间继续保留未知。
2. 优先完成现有 producer / relay / browser / native 接纳边界的定位，再决定控制信号。下一份最小观测可以在原有有界采样中保留：输入入浏览器队列计数及队首年龄、每次 Pump 前后 A、poll 服务计数/预算用尽与退出后队列余量、对端已发布 frame 进度。使用同一端明确时钟、O(1) 计数和有界前史，不需要逐帧 CDP、全队列遍历或新 frame pump。
3. 上述观测要区分“浏览器尚未看到输入”“JS 已入队而 native 尚未接纳”“持续缺某个最早连续输入”与“本地 Pump/回调未被调度”。旧报告未记录每 Pump 服务、剩余队列、逐输入浏览器到达和远端 producer 的实际推进，因此本实验的 A 静止不能归因线路。新增这些观测的收益仍未验证，本次没有实现。
4. 若仍要评估另一条提前规则，应先在新计划中明确前提、未知处理、误报成本和样本集合，再用独立记录验证。不能承诺两个回看机会在真实反馈闭环仍然存在：降速会改变本端生产输入和后续到达节奏。

窗口扩大至 12 与 checkpoint 合并属于不同实验。更深窗口提供额外无新信息空间，也增加历史保留与潜在恢复成本；cp3 可能减少 capture 税，但不提供对端未来输入。本次结果没有验证任一策略的收益，也没有替代当前 CPU 与跨进程调度工作。具体源码边界继续见 [原提案第 6 节](rollback-information-pacing-proposal.md)。

## 8. 解释边界

这些记录的 `rafDiagnostics.valid` 以及 timeline 结构/顺序检查是整份记录的事后证据资格门，不能声称在线控制器在对应时刻已经知道所有资格。A 事件只按读取上下界定位，不是精确包到达时间；批量 A 推进仍是一次事件。旧 RAF snapshot 没有 timeOrigin，本实验未跨端或跨 Node/relay 时钟相减。

所有“未标污染”仅表示计划已经记录的字段没有标记。它不证明设备没有调度迟滞，也不证明远端生产或 relay 没有排队。此次没有修改生产控制、增加固定输入延迟、扩展窗口或上线 fallback；没有模拟反事实 FPS，也没有把未复现或模型机会写成大停顿已解决。

