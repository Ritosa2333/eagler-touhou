# TH11MP Enemy / ECL 回滚组件

日期：2026-10-13。范围：实验树 `D:/workspace/eagler/worktrees/th11-rollback-smoothness`。本说明只描述 EnemyManager / EnemyScriptServices 的组件基础，不代表已经接通世界回滚、网络预测或流畅度验收。

## 接口和调用顺序

仅 `TH11_MULTIPLAYER` 提供：

~~~cpp
EnemyRollbackFrame frame;
journal.BeginFrame(frame_number);
enemy_manager.BeginRollbackFrame(journal, frame);
// 各外部 owner 必须分别捕获：environment、commands、world、ANM、deformation 等。
// 执行完整语义 tick / Draw。
enemy_manager.EndRollbackFrame();
bool complete = journal.EndFrame();
~~~

每帧使用独立的新 `EnemyRollbackFrame` 和新 journal checkpoint；本层没有证明 `extendPrevious` 合并 checkpoint 的使用契约，不得直接使用。任何 Begin 失败后仍要调用 End，解除可能已安装的 observer。后续分配发生的 first-write 失败可能发生在 Begin 成功之后，`frame.valid()` 只表示初始捕获成功且 owner 仍存活，**不能代替 `journal.Failed()` / `journal.EndFrame()` 的封帧检查**。

恢复顺序：

1. 停止模拟，解除所有组件 observer。
2. 保持目标帧和所有较新帧的 typed snapshots / leases 存活。
3. 执行完整共享 `journal.UndoTo(target, &restored_frame)`，先恢复所有已登记的值字节。
4. 调用 `EnemyManager::RestoreRollbackFrame(frame, restored_frame)` 恢复 typed 容器；其它 owner 按自己的依赖恢复。
5. 所有 owner 恢复完成后再释放废弃 snapshot，重演相同输入。

Restore 检查 observer 已解除、owner 身份、生命周期 token 和准确帧号。它不能凭自身证明调用者已完成整个共享 UndoTo，因此这条先后顺序属于 orchestrator 的硬契约。manager 销毁后 snapshot 会失效；此时不能再向已经销毁的 manager 地址执行 journal undo。

## 对象所有权与保存内容

| 对象 | 保存方式 | 物理生命周期 |
| --- | --- | --- |
| EnemyManager 的链首/链尾、数量、总生成数、Timer、错误、program 指针、spawn depth、逻辑 Entry frontier | 显式值字段 journal Touch | manager 存活期间 |
| Enemy payload，包括内嵌 ECL root/stack、EnemyState、链节点、回调与外部引用指针 | 完整 `sizeof(Enemy)` 字节；开始帧捕获活跃 Entry；复用/初始化前 first-write | 稳定 Entry 存储保留到 manager 高水位 |
| EnemyScriptServices 的 error | 独立值字段 journal Touch | 随 Entry |
| EnemyScriptServices / EnemyGlobals 的 vtable、引用、allocator observer | 不做字节快照；Entry 构造时固定绑定 | 服务对象始终和同一 EnemyState、environment、commands 绑定 |
| live Enemy map、可复用 Entry 次序 | typed shared_ptr map / vector 值快照 | snapshot 强引用保留 Entry；不复制容器头 |
| ECL context、thread node、diagnostic scratch | 开始帧捕获活跃分配；任何分配/复用清零前捕获完整 block | 按 bytes 分池的稳定物理 storage；typed lease 保留内存 |
| ECL live allocation map、各尺寸 free list / frontier | typed value snapshot | 逻辑恢复与物理保留分开 |

`Enemy`、`EclContext`、`EclThread` 有 trivially-copyable 编译期检查。容器、unique_ptr/shared_ptr、虚表、引用和 observer 对象头均不在 memcpy 快照范围中。

## 嵌套生成与同址重演

原逻辑在 Enemy 加入链表前执行第一次 ECL frame。这个 frame 可以再次 spawn 子 Enemy，因此实际链序是子先、父后。

MP 路径在执行第一次 ECL 前先保留父 Entry 槽，递归子生成使用下一个槽；仍然在第一次 ECL 返回后才把父加入链表。这样避免“两个仍在构造中的 Enemy 使用同一个 frontier 槽”，同时不改变子先入链、RNG 消耗和 total_created 对应的原有语义。

缺失脚本、第一次 frame 失败两条返回路径都释放逻辑 Entry reservation。第一次 frame 已成功生成的子 Enemy 保留原有行为，不因为父失败被组件擅自销毁。回滚通过共享 journal 和 typed metadata 恢复它们。

Entry 和 ECL 物理存储不会在 destroy/release 时归还给系统 allocator。逻辑 free list 和 frontier 恢复后，同输入分配重演选择原地址；退役对象的内存直到 manager 高水位存储析构以及最后强引用消失时才真正释放。这是正确性基础，不是已验证的性能结论。高水位内存不会在每次 reset 后下降。

## 本轮补齐的分配调查

在真实源码中搜索 `services.allocate/release`，ECL 有三类调用，而非只有 context 和 node：

- `EclOwner::spawn_thread` 分配 `sizeof(EclContext)` 和 `sizeof(EclThread)`。
- `EclInterpreter.cpp::diagnostic` 分配并释放 1024 字节 scratch；诊断参数还可能读取引用变量或 RNG，不能整个跳过。
- `EclOwner::update_threads/release_threads/reset_threads` 释放、重建动态线程链；节点自身和指向的完整 context 都必须保留。

因此 allocator 按调用给定的 bytes 管理稳定分池，覆盖所有这三类；不能只对两个已知 struct 做特判而漏掉 scratch。

分配前即使 Touch 失败，仍保持原来的分配/清零/执行语义；journal 负责锁存失败，外层拒绝封帧。**日志耗尽不是物理内存分配失败，不能把它转换为 ECL allocator 返回 nullptr 或 trap。** 测试必须断言“失败被拒绝提交”，不能错误断言“失败时完全没有写入”。

## 外部 owner 边界

本组件明确不拥有：

- `EnemyEnvironment` 的共享 RNG、rate、boss、enemy_count、shared variables 与 target 输入。
- `EnemyCommandEnvironment` 的 bosses、manager_flags、health_segments、spell/stage/ANM/bullet/laser/其它服务对象。
- `EnemyFrameWorld` 的 shot targets、伤害、得分、音效、掉落、玩家碰撞、world.target 和 spell 状态。
- Enemy 上的 `ScreenDeformation*` 所指物理对象、该对象的 vector 和 VM callback 链。
- EclProgram 及其文件/字节 buffer 的所有权；快照中的 instruction / script name 指针依赖它们保持存活。

`destroy` 仍按原流程先调用 `world.release_deformation`，并保留它拒绝销毁的行为；组件没有擅自 delete 或声称恢复 deformation。完整世界接入必须由独立 world/deformation owner 解决其生命周期。

重查还确认：`GameBattle::clear_shot_targets` 已擦除 `mp_damage_remainders[enemy]`，`mp_stage_reset` 会 clear 全表。不能把正常清理误报为遗漏；未来 rollback world owner 要对该 map 做 typed capture / restore，保持与 Enemy 指针身份对应。

资源替换、场景切换、Restart、manager 析构前必须先处理历史和 resource leases。本组件不能允许历史指向已经释放的 ECL resource，也不能把 raw program 指针快照当作资源强引用。

## 专属测试

文件：

- `portable/multiplayer/enemy-rollback-test.cpp`
- `portable/multiplayer/enemy-rollback-test.mjs`

runner 由主执行者串行运行，顺序生成 ordinary / multiplayer WASI `-O2` 测试，关闭浮点 contraction、RTTI 和 exceptions。每次用新输出目录，保留旧失败 receipt。编译/执行前后对真实 cpp、game/common/portable-sdl headers 和 runner 计算身份，禁止编译期间更改被观察文件。

测试使用真实 EnemyManager、EnemyScriptServices、ECL interpreter/allocator 调用及空的真实 ANM manager，使用内存中的合成解码 ECL 资源，不读取游戏 DATA、不用浏览器/手机、不替换游戏算法为 stub。fixture 单独捕获 environment / commands / world target 的值字段，以展示组件合作边界；这不证明生产 world 已完整接入。

覆盖意图：

- ordinary 模式的真实嵌套 spawn、线程链序、线程执行、reset、diagnostic、父失败语义。
- nested spawn 的父先保留槽、子先入链，以及回滚重演地址/完整 payload 相等。
- inactive stale Entry 复用、destroy 次序、reset 后重建、missing subroutine 和父失败回收路径。
- 实际 secondary threads 取消后释放、再生成同地址；包括 stack 尾部残留字节、thread node links、service error。
- 实际 diagnostic opcode 的 1024 字节 scratch first-write 与完整清零。
- 两个 checkpoint 的递次恢复、typed live/free/frontier、snapshot 强引用和 owner 身份/生命周期失效。
- 字节/块预算失败不冒充 ECL 分配失败，并拒绝封帧。

## 已验证结果

主执行者串行运行命令 `xwAnRiHr45EvLVH_KKse_79H`，exit 0；收据为 `artifacts/multiplayer-enemy-rollback/o2-01/receipt.json`。

- 开始：`2026-10-13T07:32:23.406Z`；完成：`2026-10-13T07:33:15.099Z`。
- ordinary：**1 case / 14 checks PASS**，WASM 865543 bytes，SHA-256 `ee82ed7c6d20bea81f9ac39c480113cd0dd56f6475fbb2706ff8ddb9a6cff994`。
- multiplayer：**12 cases / 416 checks PASS**，WASM 989939 bytes，SHA-256 `6d8b56861cd76e1b4aef01d5eb1e48bf22d95ead63c25fa17959f3b984fa66f6`。
- 两个变体 stderr 为空，`sourcesUnchanged: true`，`ok: true`。首轮即通过，没有删去失败记录或未运行的测试冒充 PASS。

这些结果支持上文列明的组件恢复/生命周期语义；没有证明 4× CPU、高刷、弱网、触控移动或生产预测回滚通过，不能据此宣称完整 TH11 可交付。

## ECL destination 追加复审

`EclContext.cpp::integer_reference/float_reference` 在 references 位未置时返回 nullptr，不会回退到 instruction 参数地址。合法引用分别指向本 context 的 stack 或 EnemyGlobals 提供的 Enemy / boss 字段、environment.shared_integers。完整 Enemy 前像覆盖 root stack 和 Enemy 字段；动态 context block 覆盖 secondary stack；shared_integers 必须由 world/environment owner 单独捕获。

在当前 ECL/Enemy 写目的地搜索中，没有发现 ANM 那种可直接写 script bytes 的非引用参数 fallback；call_subroutine 只读资源并写 target.stack，diagnostic 写 scratch。现存 stack local offset/frame_base 缺少完整边界检查，是畸形 ECL 的既有输入验证风险；本组件不声称通过回滚捕获解决了越界输入。

