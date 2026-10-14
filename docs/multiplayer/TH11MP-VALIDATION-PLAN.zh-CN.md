# TH11MP 独立验证与诊断计划

更新时间：2026-10-13。责任范围：validation_plan 子任务。本文补充独立的只读审查和离线诊断，不能替代根代理的最终验收或 worklog。

## 当前冻结边界

正式 10 的全程 job 正在执行时，只允许 nig 读取以及新增未被 job 引用的离线汇总脚本。没有执行命令、构建或浏览器，没有修改 production、full runner、background job helper 或根 worklog。

实际全程入口 [rollback-full-game.mjs](../../portable/multiplayer/rollback-full-game.mjs) 的 chromium.launch 使用 headless:true，三席为三个独立浏览器进程。硬件 GPU 要以 receipt 中实际 GPU/参数为证；这不是人工可见窗口的肉眼全程检查。三个 disable background flags 不能证明或排除任何一次卡顿的原因。

## 一、内存所有权只读结论

目前未发现确认帧后 typed checkpoint 永久持有旧 StageResources 的明确路径；也不能用 WASM buffer 从约 193 MB 增至约 401 MB 宣称没有泄漏或保证后半程不会 OOM。

### 有证据的释放路径

- [MultiplayerRollback.inc](../../th11_web/cpp/sdl/MultiplayerRollback.inc) 的 rollback_commit 先 DiscardBefore，再 reset 已确认 World/Texture snapshots，最后发布并 pop_front。恢复时先共享 Undo、全部 typed Restore，再销毁未来快照。rollback_release 按 journal.Clear、frames.clear、world.reset 次序清理。
- 生命周期 barrier 在执行原生 exact transaction 前要求 history deque 为空且输入已协调。WorldRollbackFrame 的 Battle/Stage/StageResources 引用是裸指针，不强持有这些资源。
- [AnmRollback.hpp](../../th11_web/cpp/game/AnmRollback.hpp) 的 geometry shared_ptr lease 随 World frame 释放；[AnmManager.cpp](../../th11_web/cpp/game/AnmManager.cpp) release_geometry 删除当前 manager 的 owner。
- [GameBattle.cpp](../../th11_web/cpp/game/GameBattle.cpp) 等 outgoing stage 结束并消耗 ANM retirement 标记后，调用 retire_previous。
- [GameSessionResources.cpp](../../th11_web/cpp/game/GameSessionResources.cpp) retire_previous 先调用 effects.release，再 previous_stage.reset；[Application.cpp](../../th11_web/cpp/sdl/Application.cpp) release 卸载旧 stage background/logo/enemies；[GraphicsDevice.cpp](../../th11_web/cpp/sdl/GraphicsDevice.cpp) unload 释放 backend texture 并 erase CPU texture/resource maps。

### 明确存在的容量保留

[RollbackJournal.hpp](../../third_party/eagler-common/include/eagler/netplay/RollbackJournal.hpp) 与 [RollbackJournal.cpp](../../third_party/eagler-common/src/netplay/RollbackJournal.cpp) 采用固定槽 ring。DiscardBefore/UndoTo 释放逻辑历史，保留每槽 arena/blocks capacity 以复用；Clear 才销毁。TH11 配置为 14 槽、单槽 32 MiB、每槽最多 65536 blocks。因此仅 byte arena 理论总上限是 448 MiB，不是 32 MiB；这是按需增长的上限，不是预分配或当前占用值。普通关卡 barrier 不 Clear journal。

[EnemyRollback.cpp](../../th11_web/cpp/game/EnemyRollback.cpp) 的 Enemy/ECL stable storage 留到 manager 析构，destroy/reset 归还逻辑槽并释放线程。ANM VM storage 同样保留高水位；Laser 是固定 256 槽池。[ScreenDeformationPool.cpp](../../th11_web/cpp/game/ScreenDeformationPool.cpp) 在 release 后保留退休回调 payload，旧 strip VM 消失后才能复用；每份 frame 当前也复制逻辑槽的 typed vectors。以上说明可复用容量跨关存在，并不证明不可复用对象逐次泄漏。

原 archive/core 资源常驻、换关时 current/previous 暂时并存以及解码临时内存必须分开考虑。多人 [InputReplay.cpp](../../third_party/eagler-common/src/netplay/InputReplay.cpp) Begin 一次分配 Frame[MaxFrames]，Append 写既有数组，不是逐关扩容的解释。

### 后续必要的有限诊断

现有全程结束后再决定是否加容量观测，不在当前 job 内改 observer。最有辨别力的是关卡 barrier 前后 journal 总 capacity / active frames、各 stable pool 物理与活跃槽数、Graphics CPU pixel bytes、allocator 活跃与空闲内存。缺这些数据时，不把 linear memory byteLength 当作 live heap，也不把容量增长直接归因于某 owner。

## 二、离线卡顿汇总脚本（已准备，尚未执行）

入口：[summarize-fullgame-stalls.mjs](../../artifacts/rollback-smoothness/summarize-fullgame-stalls.mjs)。

由根代理在 job 结束后串行运行。脚本只使用 Node 内置 fs/path/crypto/assert，不导入 runner，不启动进程或浏览器。要求 receipt.completedAt，要求 fresh output；原始收据、raw 段及原 verdict 均不改写。逐文件核验 SHA-256、逐席 previousSha256 链，第二遍再核验文件身份，结尾核验 receipt 未变。

示例（当前源码树作为 cwd）：

    node artifacts/rollback-smoothness/summarize-fullgame-stalls.mjs --run artifacts/rollback-smoothness/fullgame-lunatic-01/run --output artifacts/rollback-smoothness/fullgame-lunatic-01-stalls-01

产物为 summary.json 与 summary.zh-CN.md。脚本尚未 node --check 或实际执行；不能把代码准备完成写成汇总已验证通过。

### 保留的证据

- 每关每席 present/frontier/native Top 10。
- 所有超过 1 秒的观测区间独立置顶，不受 Top 10 上限影响。
- present 与 high-water frontier 跨 raw 段保持连续；阶段/时钟/缺段或观测结束产生下界，明确标 censored，不用两个较短段洗掉一个长停顿。
- 原始 nativeMs、前后 callback/nextFrame、history 计数差、nativeTicks、observer/shell hook 成本、最大无回调间隔、networkWait/cpuYield 标记。
- 页内 timeOrigin + 时间给出 epoch/UTC；present 是 counter 变化在 callback exit 的观测，不是 GPU scanout。
- 重叠的注入 send-timer delivery 记录给出 due/actual/requestedDelay/lateness，区分延迟注入 timer 与真实网络 RTT。
- capture 与 serialize 的总量、单块最大值、块数、checker request/receive/transfer/write/hash 等原始 snapshotTiming。
- WASM 容量跳变的采样区间、music track/fade/paused 状态变化及资源传输的原始 checker 区间。
- 原 receipt performance/failures/errors/snapshotErrors/watchdog 原样保留，不减 observer 成本、不改最终失败结论。

### 已确认缺失，禁止补造

当前全程 raf 行不含原生 profile，也不含逐次 audio/draw/capture 耗时或可靠 forward tick 分项。historyTicks 是累计计数差；nativeTicks 原值保留，但不冒称 forward。nativeMs 是原生 wall time，不能凭低 nativeMs 将 5 秒停显自动归因浏览器或网络。

motion 在 Stage 中约每 100 ms 采样，stage 字段在间隔中沿用最近一次 game-status，因此关卡转换和 cue 精确请求时刻不可由它直接确定。audio ABI 为 [5] track、[6] fade、[7] 音量 × 1e6、[8] music_paused；不能把 [7] 说成 remaining 或 [8] 说成资源 pending。

正常 raw snapshotTiming 未保存每个 chunk 的开始/结束 timestamp，只有 serialize sum/max、transfer sum/max 和 section block 数。整次 checkerCaptureWallMs 包含浏览器调度、多个块之间游戏执行、传输和分析等，不能当连续的主线程阻塞时长。

musicTransfers.checkerMs/completedMs 属于 Node runner 的 performance 时钟；receipt 没有保存 checker timeOrigin。原区间原样输出，并关联邻近 segment 的 checker capture 区间，但不伪装成与页面 gap 的精确 epoch 对齐或因果证明。

根代理提示的 Stage 5 约 5 秒停显位于 segment 0137、frames 89925–90387；不是提示时最新 progress 的 frame 104736。脚本从 raw 自动定位，不把该提示硬编码成归因。

