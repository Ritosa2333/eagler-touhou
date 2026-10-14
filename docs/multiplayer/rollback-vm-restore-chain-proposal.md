# TH09 完整 bullet VM 跨记录恢复去重方案

状态：**只读设计提案，尚未实现，尚未测量性能收益。** 日期：2026-10-11。

本提案来自对当前 TH09 生产恢复链和受授权 TH07 现代源码的只读核查。当前 enemy-index A/B 测试使用的生产、组件和 runner 源码继续冻结。本次只创建此文档，不修改生产实现、不构建、不运行组件、浏览器或性能测试。在途 enemy-index 实验与这份新提案分别归档，不将它的结果用作本提案的性能证据。

## 1. 结论和实施边界

**完整 bullet VM 是可以独立考虑的跨记录重复恢复消除点。** 当前生产 owned 恢复和唯一 texture 通知都不读取 bullet VM；但公开 AfterRestore 能执行任意观察逻辑，AttackState 也是开放的多态接口。因此不能直接给现有单记录 Restore 无条件跳过 memcpy，必须定义一个窄范围的同步 RestoreChain，并对无法证明安全的记录链完整回退到旧路径。

推荐最保守的第一版：

- 保留单记录 Restore 的公开语义、capture 和 Seal 语义。
- 仅在 Application 已有的同步 correction 恢复循环中使用批量规划。
- 任何记录曾登记 AfterRestore，整条 correction 链回退。现有 texture revision 回调也回退，不做特殊豁免。
- 任一待恢复 AttackActor 的当前 state，或任一参与记录保存的 AttackStateSnapshot.state 非空，整条链回退。第一版不尝试认证某个多态类型安全。
- eligible 链仍逐记录执行原有 journal、VM、undo、Enemy、Effect、Attack 顺序，只省掉一定被更老记录完整覆盖的 VM memcpy。
- 不改普通 POD journal、拥有对象的保存内容、纹理字节、hash、sounds、输入采样、输入延迟、重演起点或 checkpoint 间距。

这是一项关于正确恢复相同状态所需复制量的改进，不意味着回滚 tick 的游戏逻辑成本已经解决。是否能改善 CPU4 下的单次恢复尾部，要在后续冻结 A/B 中实测；不预承诺帧率或玩家可感知收益。

## 2. 被核查的源码身份

主仓：D:/workspace/eagler/worktrees/th09-rollback-4x，HEAD 5d4b7f0a1cc047143e3f5da5ed3749ad5af9b897，含已冻结但尚未提交的 enemy-index 最小改动。common 候选：D:/workspace/eagler/worktrees/th09-rollback-input-batch-common，HEAD 512353cab9eaa4f6744f867b0eb07c61ee7f2b0e。不使用脏的旧 common prototype。

写文档前再次只读确认的 SHA-256：

| 生产输入 | SHA-256 |
| --- | --- |
| th09_web/cpp/multiplayer/WorldState.cpp | 772d9f9b29c3a1fdf89e08516f8fb18dc72578af74b8aab4b8652aa50210d77c |
| th09_web/cpp/multiplayer/WorldState.hpp | f87dc52b5955ec3e25cfc2e8657a4d6b42cf7e5fa8f1b64332e1f051a8f90ec6 |
| th09_web/cpp/multiplayer/DynamicState.cpp | ff407840ff35059f0f2babed108090131bbbeea1e473a731fd59ee101a1a2c0a |

已冻结 release manifest：D:/workspace/eagler/worktrees/th09-rollback-4x/th09_web/artifacts/rollback4x-enemy-index-release/build.json，SHA-256 ad19bc3a5f0c4e2bb39a9051d7fa8337a5f4b31748f327de85407829f72b61e9。它明确包含 -fno-rtti；build-sdl.mjs 的公共 flags 说明 diagnostic 也受此限制。已有 enemy-index 组件通过只能证明其自身改动，不能当作此 RestoreChain 的测试结果。见同目录 rollback-enemy-index-evidence.json。

## 3. 当前恢复顺序和观察者

源位置编号的完整路径在文末。行号针对上述冻结源码。

Application 在发现 correction 后先 flush renderer/backend，然后在一个同步 C++ 调用中从 newest 向 correction 逐记录 Restore，每次成功后清该记录 sounds，最后才调用 rollback.Restored(correction)。中间不执行 tick、Draw、浏览器回调或 JS yield。[S3:512-518]

单个 WorldState 的顺序是：Unbind → journal.EndFrame（若尚开）/UndoTo(0) → 完整 bullet VM → undo 中的容器/外部 action → Enemy → Effect → Attack。[S1:212-234] 源码明确说明 POD 在 owning/texture undo 之前恢复；这是当前可观察语义，不能仅凭“最终画面相同”绕过。

| 恢复部分 | 当前行为、源码依据 | 对完整 bullet VM 的观察情况 |
| --- | --- | --- |
| 普通 journal POD | NamedFields/Fields/Touch 注册完整平凡可复制字段；UndoTo 只复制已记录地址。[S1:113-127,145-209; S2:40-59; S13:303-353] | 不执行游戏回调。sparse VM 缓冲与 journal 由适配器契约分开；不能扩展为任意重叠地址的去重。 |
| bullet VM pages | 每个身份保存整个 AnmVm，包括 padding；按记录的 visualOrder 逆序复制。[S1:75-90,217-228; S2:98-115] | 本身是 POD memcpy。两侧的视觉池独立，不能只按 slot 号合并。 |
| Vector / Text / AppendOnly | 恢复自有容器值或长度。[S2:61-72] 当前 draw_lists 保存的是指针值，不在赋值中解引用指针。 | 当前调用者不读取 bullet VM，也不调整其 backing vector。公开 helper 不应被当作对任意外部容器的安全授权。 |
| Recording undo | 恢复 recording parts 和各 inputs/rates 长度，用索引重新找到元素，不保留易失效的外层元素地址。[S1:130-139] | 只修改 recording 自己的容器。 |
| EnemyState | 从 saved 复制完整命名字节和 asynchronous contexts，再重绑 active/locals 到恢复后的自身。[S4:21-50] | 不执行 ECL，不解引用 world/field/program 指针，不读 live bullet VM。 |
| EffectState | 数据字段、自有 AnmVm、EffectBurst 和颜色/纹理顶点 vector 的复制。[S4:57-67; S17:31-45] | 不调用 effect update/draw，也不读 bullet VM。 |
| AttackStateSnapshot | 复制 animations、behavior 指针，然后调用 state.copy_into 或 reset。[S4:71-87; S7:11-28] | 现有九种 concrete state 是数据复制；但接口是开放多态，未知 copy_into/析构可能观察任意游戏状态。首版一律回退非空状态。 |
| texture AfterRestore | 唯一生产外部登记：before_text 先 Touch 像素，再登记 changed(id)。changed 只递增 revision。[S6:89-90; S15:67-83] | 当前通知不读 VM、不会上传纹理；然而通用 AfterRestore(std::function) 的契约允许任意观察者，首版仍回退。 |
| 单记录之外 | Application 每次 Restore 后清声音命令，全部记录后通知 rollback.Restored。[S3:516-518] | 清理命令不读取 VM，但必须保留每记录次数和相同失败位置。 |

现有 Attack concrete types 位于 CharacterAttacks.hpp:29-72，包括 Travel、Cirno、Tewi、Aya、Sakuya、Medicine、Mystia、Reisen、Field。它们没有自定义观察性复制/析构；其中 parent 等指针只按值复制。这个源审计说明优化窗口存在，**不构成对任意未来动态类型的授权**。[S8; S7:20-28]

## 4. 去重的精确单位和等价性

两侧各有 537 个 bullet slots，每个 slot 有 5 个完整 AnmVm，即每侧 2685 个固定身份。[S10:19-23; S9:16-20; S2:98-115] VM 身份至少由 BulletVisuals owner、实际 instances.data/size、slot、animation 共同约束。配对不能依赖两条记录中 range 数组恰好使用同一索引。

设恢复区间是 correction 到旧 frontier 之前。对于某个完整 VM v，取这个区间内**最早真正保存 v 的记录** m(v)。旧 newest→oldest 循环对 v 的最后一次写入正是 m(v) 的完整保存值。因此在无中间观察者且 backing storage 稳定的链中，仅保留 m(v) 那次写入，最终 VM 字节完全相同。

注意“最早保存”不一定是 correction 那一帧：冷槽可能直到中途 spawn/type-change/Draw 才 first-write；更早记录没有该 VM 时，必须保留中途第一次保存的值。这也是不能只恢复目标记录，或只拿目标记录的 bitmap 去过滤其余记录的原因。

建议 oldest→newest 规划每记录 keep mask：

1. 对每侧稳定 pool 建立空 covered 集合。
2. keep(record) = record.visualSaved 中尚未 covered 的身份。
3. 将本记录所有 visualSaved 加入 covered。
4. 按原 newest→oldest 顺序 Restore，只对 keep 里的身份执行完整 VM memcpy。

规划不修改原 visualSaved、visualOrder、pages 或计数。执行仍要消费每个完整记录，原有的 bitmap/count/FirstWriteBlocks 清理和 ring 容量保留都必须发生，不能因为本记录没有需要复制的 VM 就跳过其 journal/undo/owning 恢复。

页是分配单位，**不是去重单位**。每页容纳 64 个 VM，但没捕获的页内位置可能未初始化；不能复制整页、不能把 page 存在等同于 64 个身份都保存。首版不加入新的连续 memcpy 合并，先只消除重复完整 VM 的复制。[S1:82-88; S2:110-115]

## 5. 必须落实的保守准入

### 5.1 生产没有 RTTI

build-sdl.mjs:21 的 release/diagnostic 共用 flags 包含 -fno-rtti；实际 release build.json 也记录了该开关。[S11:21] 不能为此功能使用 dynamic_cast、typeid，不能依赖 std::function::target_type 推断 lambda 的行为。WASI 组件某次构建未传 -fno-rtti，不能据此给生产设计加入 RTTI。

### 5.2 任意 AfterRestore 都是观察屏障

AfterRestore 的现有入参是任意 std::function<void()>，无法安全地通过函数对象地址、捕获字节、调用者文件名或 lambda 类型推断“不会观察 VM”。[S2:36]

拟议实现应在 AfterRestore 登记时明确设置该记录的 external-undo 标志，并与 undo 的真实生命周期一起在新 Save 重置。任何参与恢复的记录有该标志，**整条链从开始就调用原 Restore 循环**。不要在已经省略部分写入后才临时遇到屏障。

首版保守地让现有 GraphicsDevice 的 texture callback 也触发回退。后续若实际命中率证明值得，再单独设计纯 revision 通知的显式数据类型/可信登记入口；那是另一项语义扩展，不能通过比对某个 std::function 身份悄悄豁免。

### 5.3 非空 AttackState 一律回退，不猜动态类型

保存时的 type_key 不足以完整证明安全。copy_into 是虚函数，目标类型不同时 reset/赋值会销毁当前目标；即使 snapshot 是已知类型，当前 destination 也可能是未知类型。扫描只检查保存时 tag 会漏掉这个析构观察者。[S4:76; S7:11-28]

第一版最小且可证明的 gate：

- 对链中每个 AttackStateSnapshot 的保存内容检查 saved.state 是否为空。
- 对链中每个将被恢复的 AttackActor 的**当前** destination.state 检查是否为空；不能只检查最新记录出现的 actor。
- 任何一项非空，整条链回到旧路径。
- 查询应由窄的 const 元数据访问器完成，使用 unique_ptr 的 bool 状态；不调用 type_key、copy_into、clone、checksum 或其他虚方法来决定准入。
- 当全部保存状态和初始目标状态均为空、也没有外部回调时，每次恢复后的目标 state 仍为空。由此可逐记录推导，批量路径内不会进入未知多态复制或析构。

这个 gate 不改变未知 state 的保存/恢复成功条件；只是让它继续使用旧 Restore。它还保留当前带 animations 但 state 为空的 actor 的正常完整恢复。

若未来要放行现有九类类型，必须单独定义可信类型登记和观察者契约。现有 CopyableAttackState<T>::key 为正确实现提供稳定 tag，源码全量审计加“保存的每个动态类型 + 初始目标的每个动态类型”白名单可以作为受控代码库的条件证明；但开放虚接口允许新类继承或覆写 type_key/copy_into，tag 本身不是实现身份证明。没有 RTTI 时，不应把未经约束的 tag 白名单写成绝对安全保证。首版完全绕开这个问题。

### 5.4 其余前提

- 整条恢复仍在一个同步调用中完成，没有游戏执行、事件回调、呈现或 yield 穿插。首版不把 batch 分片跨回调。
- 每条参与记录必须属于当前稳定的 two-human versus world，frame number/captured/state 与现有历史校验一致。
- 准入读取保存时的实际 capture 路径，不仅依赖之后可变的 public sparse/hashEnabled 值。所有记录都使用 sparse 且非 hash 的完整 VM pages 路径；混入 dense/hash、缺少有效 range 或 owner/data/size 不匹配时回退，而不是用相同 slot 编号猜测。
- BulletVisuals.instances 的分配从 checkpoint 到 retirement 保持不变。其构造按固定数量分配；当前 runtime 写实例值，不改变 backing storage。[S9:16-23; S1:142-199]
- Journal 与 sparse VM 地址不重叠；保留现有断言和完整字节预算，不将 VM bytes 重复写入 journal。[S1:5-15; S2:38-58]
- 不执行任何新游戏/资源分配与销毁，不改变 texture 像素的 journal 恢复顺序，不调用 behavior.dispose。
- 无法证明准入时回退应发生在任何优化恢复之前。原本无效/丢失记录仍走原失败路径，不能被跳过或静默成功。
- 已保留的 >260 EnemyState 容器 relocation 边界不是此次修复范围；若不能证明记录是稳定的正常 owner 前缀，应回退。不得把已有失败边界误报成 batch 支持。

## 6. 最小实现形状，待后续批准后才进入源码

建议变动面仅为 WorldState 的窄批量入口/元数据查询与 Application correction 循环接入；共享 journal、保存布局、cp3、游戏 tick 和网络逻辑保持现状。

RestoreChain 的处理分三步：

1. 收集当前 correction 链并检查资格，读取 owner / backing storage / masks / observer barrier / polymorphic-null 状态。判定不能使用会执行游戏代码的虚调用。
2. eligible 时以有限 scratch 空间构造 per-record keep mask；不改变记录本身。scratch 大小要由当前最大历史长度限制，不能每 VM 建哈希表或做逐记录嵌套线性搜索。
3. 使用同一个单记录实现执行原顺序，内部仅让 VM memcpy 接受 keep mask。公开 Restore() 仍使用完整 mask/原路径。每记录成功后沿用原 sounds.clear 和 checkpoint count；失败立即保留原错误退出。

不 eligible 时直接执行原循环，不增加网络等待或输入延迟。非空 polymorphic state 的保守 gate 可能降低实际命中率；如果合资格字节量小，或 Restore 本来只占回调很小一部分，应据实搁置此候选，不能为命中率放宽正确性前提。可以提供 diagnostic-only eligible/fallback 原因、完整 VM copied/skipped bytes 计数来确认这项改动是否真的覆盖了工作负载；不要把这些计数扩展成新的用户界面。

不建议第一版把最终 VM 值提前整体写好再做全部 journal/owned 恢复，或把所有 POD 放第一阶段、所有 owned 放第二阶段。那会重新定义更多中间顺序，扩大证明范围。也不建议为了绕过观察者而削减捕获状态。

## 7. 最小验证门槛

### 7.1 组件：独立旧路径和完整状态 oracle

必须用独立的旧 newest→oldest 完整恢复顺序作为 oracle，不使用 keep mask 的镜像实现算预期。至少分别验证：

| 维度 | 必须覆盖的情况 |
| --- | --- |
| 记录深度 | 1、2、8；只有一条记录时复制量不得增加，结果等于旧路径。 |
| 覆盖集合 | 每帧重复同一 VM、彼此不交、部分交、目标帧无保存而后续帧才 first-write、完全未触及的 VM。 |
| 身份和页 | 两个 pool 相同 slot、全部五种 animation、63/64 页边界、另一页、尾部 sentinel。 |
| 生命期 | spawn、消失后 slot 复用、type change/全 slot 写、真实 Draw first-write；不能丢掉冷 VM 原始字节。 |
| ring | 恢复后再次 Save，以及 Seal/退休但未 Restore 后复用；prefix、bitmap、keep scratch 和源 page 不串轮次。 |
| 完整状态 | 全 bullet/全部 VM 字节含 padding、所有既有 named state/hash、Enemy/Effect/Attack 拥有内容、恢复插入顺序。 |
| 容器 | draw_lists 指针顺序、recording 分段/长度、text 和动画 vector 的完整恢复。 |
| 外部观察者 | AfterRestore 主动读取当前 VM 并记录每个中间值，或再调用 BeforeEnemy；确认整链回退，回调次数/顺序和旧路径一致。 |
| 多态观察者 | 保存 state 非空但目标空、保存空但目标非空、不同中间记录出现 state；自定义 copy_into/析构读取 VM。每种都在优化前回退，保留旧观察值。 |
| texture | 像素字节、revision 次数/顺序；现有 text callback 的记录必须走回退，不能少发 revision。 |
| 不合资格 | 混合 dense/hash、owner/data/size 不匹配、记录缺失/过期；range 顺序变化需显式按 owner 匹配或回退。不得出现未证明的 skip 或假成功。 |
| 已有失败边界 | 不弱化原异常上下文、旧 >260 owning relocation 断言；这类记录保持旧结果而非被过滤掉。 |

复制量断言按完整 VM 的保存次数及区间身份并集独立计算：eligible 链 copied bytes 等于并集中每个完整 VM 的大小之和；skipped bytes 等于旧总复制量减 copied。计数是结构证据，不是耗时收益。

测试还要确认回退链复制/恢复顺序与原代码完全一致。不能只核对最后 hash；中间观察者测试就是为了阻止“最终一样但回调见到错误状态”的实现。

### 7.2 真实适配器正确性

复用并扩展现有 Application 的 sparse/dense、真实 reset/Draw、page 边界和 ring 复用 oracle。[S3:778-871] 现有例子只有单记录，必须增加真正跨记录恢复，不能因旧单记录测试通过就声称 batch 正确。

随后由 root 串行执行深 8 的完整一致性门槛，包括两端和独立 reference 的状态，以及保存/恢复预算边界。需要保持输入、随机种子、build flags、加载器和游戏内容一致，保存源码和 Wasm 指纹。

### 7.3 CPU4 和玩家目标

当前优先完成在途 enemy-index ABBA；不要把此提案并入那些冻结构建。新候选只有在组件和真实适配器正确性门槛后，才单独冻结 baseline/candidate 做串行 CPU4 对比。

报告应分开：Restore 原子调用耗时、复制量、forward/replay tick 耗时、整个回调时长，以及连续新状态提交间隔。保留不利结果、初始卡顿和宿主负载；编译等外部竞争应记录，不能只取最快一次。

深 8 peer harness 的 legacy costs 按 nextBefore >= 180 且 < limit 选样，会漏掉回卷到 180 以下的恢复片段。必须保留该 legacy 字段并解释口径；若计算连续区间总成本，按每端 nextBefore/nextAfter 高水位取第一次跨 180 至 1800 的连续调用区间，纳入其间所有回卷调用，明确终点与尾部。手动驱动、同页多世界和 CPU throttle 的结果仍然不是实网生产 rAF FPS 认证。

最终玩家门槛仍是低输入延迟和连续新画面：不能把重复呈现同一状态称为无掉帧，也不能用设备性能导致的降速掩盖单次大停顿。网络信息不足导致的平滑降速要与 CPU 原子阻塞分别归因。

## 8. 与 TH07 cp3 / coalesced restore 的关系

TH07 的 BeginFrame 在 checkpointLogicalFrames 区间内延续同一 first-write capture；EndFrame 更新区间进度；RestoreTo 恢复到真正 checkpoint 起点，并把 replayFrom 告知调用方。DiscardBefore 还保留跨确认边界的 checkpoint，并在 confirmed-only 间隙后重开。[S12:570-692]

共享 RollbackJournal::UndoTo 在一个同步函数内部，仅当一个完整 block 被更早记录完整覆盖时跳过较新的复制；partial overlap 仍保持原顺序。该实现假设中间没有游戏回调。[S13:303-353] TH09 当前 WorldState 各自只拥有一个 journal checkpoint，虽然 coalesceRestore 已为 true，单次 UndoTo(0) 看不到别的 WorldState，不能产生跨记录收益。[S1:4]

共享 PartitionedPoolJournal::UndoTo 已提供精确的 oldest-first covered 思路，按固定 part 身份只恢复最早保存值，明确说明中间没有 game callback。[S14:128-159] 本提案借用这个可证明的覆盖原则，暂不把 TH09 自定义拥有对象/texture undo 强行塞入通用 journal。

cp3 是另一项更大的区间协议改动。TH09 若要做它，必须一起处理动态 owner first-write 范围、texture undo、每帧 sounds、确认/退休、restoredFrame/replayFrom 以及跨 exact-only 间隙的行为；只改一个 checkpoint 常数既不能获得等价语义，也不能解决这些依赖。本提案不增加 input delay，不削减状态，不改变 checkpoint 间距。

## 9. 源位置索引

以下均为本次只读读取的现代源码，未使用原 EXE、反编译或外部游戏 oracle。路径采用工作区绝对位置，方便回查。

| 编号 | 完整路径 |
| --- | --- |
| S1 | D:/workspace/eagler/worktrees/th09-rollback-4x/th09_web/cpp/multiplayer/WorldState.cpp |
| S2 | D:/workspace/eagler/worktrees/th09-rollback-4x/th09_web/cpp/multiplayer/WorldState.hpp |
| S3 | D:/workspace/eagler/worktrees/th09-rollback-4x/th09_web/cpp/sdl/Application.cpp |
| S4 | D:/workspace/eagler/worktrees/th09-rollback-4x/th09_web/cpp/multiplayer/DynamicState.cpp |
| S5 | D:/workspace/eagler/worktrees/th09-rollback-4x/th09_web/cpp/multiplayer/DynamicState.hpp |
| S6 | D:/workspace/eagler/worktrees/th09-rollback-4x/th09_web/cpp/sdl/GraphicsDevice.cpp |
| S7 | D:/workspace/eagler/worktrees/th09-rollback-4x/th09_web/cpp/game/AttackQueue.hpp |
| S8 | D:/workspace/eagler/worktrees/th09-rollback-4x/th09_web/cpp/game/CharacterAttacks.hpp |
| S9 | D:/workspace/eagler/worktrees/th09-rollback-4x/th09_web/cpp/game/BulletVisuals.hpp |
| S10 | D:/workspace/eagler/worktrees/th09-rollback-4x/th09_web/cpp/game/BulletManager.hpp |
| S11 | D:/workspace/eagler/worktrees/th09-rollback-4x/th09_web/scripts/build-sdl.mjs |
| S12 | D:/workspace/eagler/th07-eagler/src/netplay/Th07RollbackState.cpp |
| S13 | D:/workspace/eagler/worktrees/th09-rollback-input-batch-common/src/netplay/RollbackJournal.cpp |
| S14 | D:/workspace/eagler/worktrees/th09-rollback-input-batch-common/include/eagler/netplay/PartitionedPoolJournal.hpp |
| S15 | D:/workspace/eagler/worktrees/th09-rollback-4x/th09_web/cpp/sdl/FontDevice.cpp |
| S16 | D:/workspace/eagler/th07-eagler/src/netplay/LiveBulletSnapshot.hpp |
| S17 | D:/workspace/eagler/worktrees/th09-rollback-4x/th09_web/cpp/game/EffectManager.hpp |

**推进状态：只完成调查和这份设计文档；RestoreChain 未实现、未编译、未测试，性能收益未知。当前生产源码冻结继续有效。**
