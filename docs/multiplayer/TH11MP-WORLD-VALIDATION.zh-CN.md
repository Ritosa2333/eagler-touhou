# TH11MP 世界回滚正确性验证（2026-10-13）

## 补充：生产 journal 配置复跑已通过

根代理随后以实际生产配置 `Reset({14,max_bytes,max_blocks,true,true})`（14 个历史槽、快速复制及区块合并）重跑同一独立 oracle；旧 01 证据保留。实际 nig 命令 `-antv-yT5ZpsKNq92s5bMf14`，exit 0。已独立读取 [world-oracle-02-bulk/receipt.json](../../artifacts/rollback-smoothness/world-oracle-02-bulk/receipt.json) 和 stdout，确认 `passed=true`、`sourceStable=true`，仍为 **8 cases / 150373 checks / 1124 Update+Draw**，峰值 **657523 bytes /597 blocks**。

该轮开始/完成 UTC：2026-10-13 08:39:17.209 /08:39:55.971；sourceDigest `b7dde1640d67a23de884606d56c2c7c82377d9ac18a4142c3ba18683dc617ace`；WASM **3854124 bytes**，SHA-256 `dc450afa63d1e56d6ee5b9bcdb3b159815bc4643c9bd9e8094392cdac6992bd1`。这补上优化参数下的语义正确性，仍不提供浏览器性能或 GPU 像素证据。下文 01 的状态和时间线保持原始记录；Application 接线的后续进展以工作日志和浏览器收据为准。

## 结论与边界

本轮 **world-oracle-01 已通过**：8 个场景、150,373 项断言、1,124 次真实多人 Update+Draw。它验证了当前稳定关卡的真实 owner 组合可以经过“参考输入 → 多帧撤销 → 错误预测 → 多帧撤销 → 原输入重演”恢复相同的完整可观察游戏状态、链表拓扑、脚本字节、几何和 CPU 绘制命令。比较不依赖 `mp_hash`，也不是仅重读新 checkpoint 中列出的字段。

这是一项正确性证据。它 **不证明浏览器已接入预测回滚，不证明 WebGL 像素、FontDevice 文本纹理或网络调度已恢复，不证明 4× CPU 下流畅，不等于可交付联机回滚**。当前 Application/协议还需要组合世界、主机纹理、副作用和呈现事务。

## 可复核身份与证据

工作树：`D:/workspace/eagler/worktrees/th11-rollback-smoothness`。

| 项目 | 实际记录 |
| --- | --- |
| nig 根执行命令身份 | `nDuzS61nUUzDAMm23IhjtvTz`，exit 0 |
| 收据 | [world-oracle-01/receipt.json](../../artifacts/rollback-smoothness/world-oracle-01/receipt.json) |
| 标准输出 | [world-execute.stdout.log](../../artifacts/rollback-smoothness/world-oracle-01/world-execute.stdout.log) |
| 测试入口 | [world-rollback.cpp](../../portable/multiplayer/world-rollback.cpp) |
| 构建与证据 runner | [world-rollback-test.mjs](../../portable/multiplayer/world-rollback-test.mjs) |
| 开始/完成（UTC） | 2026-10-13 08:01:32.678 / 08:01:39.624 |
| 源树 HEAD | `d32390730ae837fb1333bc1f1d03d746f4697936` 加收据所列未提交修改 |
| pinned common | `984a8a10834c35fe9ef86c4ffe03daf1fb61f87c`，工作树 clean |
| 源清单摘要 | `65e967e29b567c9301493f42bedb869802b78e67d5ae88179da268e817f94d0c` |
| 源身份门 | `sourceStable=true`，`passed=true` |
| WASM 大小 | 3,854,100 字节 |
| WASM SHA-256 | `0910df7d064105e7026cd6044a09cc8d6ea966d4ac62e85be04d0d4e120d5b85` |
| 实际 archive | 已有 retail `th11.dat`，26,610,794 字节 |
| archive SHA-256 | `3cb521c5d420d8cbad6494d53bcdb048bcc396abf95c18fac68e38fd2d19f8e2` |

runner 编译全部 game C++、真实 common RollbackJournal，使用 WASI SDK 34 / clang 23.1.0-rc3、wasm32-wasip1、C++17、`-O2 -UNDEBUG -DTH11_MULTIPLAYER=1 -DTH11_DEVELOPMENT_HARNESS=0 -ffp-contract=off -fno-strict-aliasing -fno-exceptions -fno-rtti`。全 game 对象缓存按编译器、配置、头文件、源内容身份校验；最终 entry 编译、链接、执行均 exit 0。此构建是 **O2/WASI32 正确性夹具**，不能称为正式 Web Release。

执行和构建均由根代理串行完成，测试作者只用 nig 编辑/读取，不另起构建。源冻结与收据覆盖了所编译的 game 和测试入口；收据 git status 也会列出其它未参与此夹具的 SDL 改动，这不表示 SDL 改动已通过本测试。

## 测试流程

每个 correction 场景先真实建立多人游戏，用现有 retail archive，经过 125 个正常 Update+Draw 激活关卡，确认 WorldRollback::CanCapture 接受当前稳定图。

随后在 **同一个 GameSession/世界实例** 上：

1. 保存独立全状态 baseline。
2. 每个逻辑 tick 先 Begin journal 与 World checkpoint，再执行实际输入、完整 Update 和实际 Draw，End 各观察器后 End journal。
3. 保存 8 帧参考结果，包括完整 owner 值和每帧绘制命令流。
4. 在 Undo 前 Validate owner/resource 图；一次 UndoTo 跨越所有后续 checkpoint；再恢复 typed owner。
5. 完成原始字节和 typed restore 后才释放所有跨越 checkpoint 的强引用，并断言 Stage 历史 lease 已退休；全状态与 baseline 比较。
6. 用不同方向/按键错误预测 8 帧，重复 Undo 并比较 baseline。
7. 再执行原始正确输入 8 帧，每帧与参考帧逐字节比较 owner 与完整绘制命令/顶点流。
8. 最后再次 Undo，并比较 baseline。

这种同实例序列要求 VM/Enemy/ECL 重演地址和逻辑复用次序一致；没有通过重建第二个世界或仅比较有限 hash 隐去生命周期差异。链表在每次观察时额外检查循环、backlink、tail、active map/allocator lease 的一致性。

## 8 个实际场景

| 场景 | 实际执行与断言 | 明确边界 |
| --- | --- | --- |
| 2P 触控移动 | target mode 2 和 analog mode 3、亚像素坐标、不同方向、Shoot/Focus，8 帧 correction | 没用手机；手工构造的真实 MultiplayerInput |
| 3P 触控移动 | 三席同时输入，其中 target/analog 混用，同上完整 correction | 不是网络 packet/触屏事件派发测试 |
| 六 Bomb 的第一组 | 三席选择 0/1/2，全 power，通过正常输入同帧触发，各席实际 active | 8 帧内状态、效果、几何与共享计数；不宣称整个 Bomb 生命周期 |
| 六 Bomb 的第二组 | 三席选择 3/4/5，同帧正常触发；第 3 帧经真实 mp_hit dispatch 命中 Nitori 护盾 | 断言原生护盾 flag 和 secondary timer，未跑完整 900 帧护盾到期 |
| 幽灵救援 | 真实 game_over(false) 进入 ghost；真实 Focus 长按计数从 86 跨越 90，消耗捐赠者一命并复活，错误输入分支不救援 | 检查复活后的原生无敌剩余值，准备计数在 checkpoint 前设置 |
| Pause barrier | 预测事务中另一席 Pause，update_multiplayer 要求 barrier，菜单未构造、资源图未改；部分输入/帧号变化可 Undo | 全部历史退休后原样输入在 P0 confirmed 执行，确实进入暂停 |
| 换关资源 barrier | 预测事务直接调用真实 next_stage，要求 barrier，尚未 load/move/delete；可 Undo | 历史退休后同操作 P0 成功，旧资源进入 previous_stage，overlap 仍拒绝预测；未验整个 overlap 周期 |
| 失败/观察器保护 | 64 字节/1 block 容量门拒绝 Begin、journal 拒绝 seal、安装过的 observer 清理；外来 Bomb observer 不被误拆；外来 ANM observer 使 Validate 在 Undo 前拒绝 | Tiny Begin 未执行游戏 tick，故只此路径断言游戏值不变；不能推导运行中 journal 失败“没有写入” |

## 独立 oracle 覆盖

### 世界、输入与副作用

- GameSession 状态、帧号、Pause held、输入、私有状态值、record 表、owner/resource 指针及错误字符串。
- EnemyFrameWorld/LaserWorld/ShotWorld 值；环境与命令服务的值/固定绑定；完成控制、Spell、计时、取消弹、popup、screen fade/shake。
- 所有席位的输入边沿、触控、ghost、救援计数、power taps、communication、rank、economy 与 shared score 引用目标。
- PlayerFrame、PlayerMotion 全值、options、body VM、所有 256 shot 槽、damage areas、锁定目标、转换 laser 参数。
- 六 Bomb 的状态、选择、错误、mesh owner 和每个独立 deformation pool。
- HUD、多人图标、presentation/hitbox ANM、SceneCompositor 摄像机和 copies。
- BattleEvent、DialogueText、AsciiRequest 按字段和完整文本序列化；错误字符串也比较。事件是游戏内队列值，**没有执行音频设备、外部 replay append、文件写入或主机输出 watermark**。

### 活动和非活动槽、拓扑

- 全部 2,000 Bullet 与 2,498 Item 槽，而不只当前 active；各 manager 元数据、grazed/target/fan 等旁表。
- Laser 固定 pool 全部槽字节、occupied、sentinel/tail 和真实链表；从未分配的物理 pool 按其初始全零逻辑字节比较。
- Enemy 逻辑 frontier 内每个 Entry 的完整 Enemy、script error、service 绑定，live map、free 次序和拓扑。
- ECL allocator 所有逻辑分配大小类、每个已出现槽的完整动态 context/node/scratch 字节、地址、free 次序和 active lease。
- ANM 逻辑 frontier 内所有 VM 的完整值，包括 inactive 槽；free 次序、active map、tags、layer/head/tail 及 registry 拓扑。
- Stage 完整状态、object VM、对象/primitives/instances/instruction arguments、deformation mesh、tint 信息与资源错误。
- 各 ScreenDeformationPool 的逻辑 frontier、slot owner/occupied、完整 vertices/positions/animation ids。
- 核心和 stage ANM 的 sprites、script metadata 和全部 script bytes；stage/default ECL 原始资源字节。

观察器读取固定槽使用 const API，避免只为测试读取就触发 manager 的可写槽捕获。oracle 本身不调用生产 mp_hash。

### 唯一物理地址规范化：ANM geometry

`AnmVm.geometry` 的堆地址在复制到 oracle 时规范为“有/无”。**只有这一种物理地址被规范化**，因为 geometry 的 typed 强引用 owner 可以选择不同的堆块，堆地址不是图形语义。

该规范化并没有忽略 geometry：

- 每个 geometry map entry 的 VM owner 地址仍严格比较。
- VM geometry 指针必须与 map 中强引用 memory 一致。
- presence、字节长度与每一个 live payload 字节独立比较。
- VM、Enemy、ECL Entry/context/node 地址、脚本指针和固定 service 引用没有规范化。
- 嵌入式 VM 与独立 VM 都参与其所属 body 的比较。

此约束允许安全的 geometry 所有权实现，同时保留对错误 VM 绑定、长度、内容或遗漏恢复的敏感度。

## Draw 验证和主机边界

RecordingGraphics 是实际 ZunGraphics 接口实现。每个 tick 调用真实 GameSession::draw / Stage / 各实际 ANM Draw，并记录 target、viewport、clear、matrix、texture bind、layout、完整 pipeline 值以及提交的所有顶点字节。断言确实产生 primitives。

每次 Draw 前对测试 renderer 执行 flush/invalidate，并重置 recording graphics pipeline；参考、错误预测、正确重演三条路径操作完全一致。这隔离了主机缓存，使本夹具主要验证权威游戏值及 authored Draw 的 CPU 副作用。也意味着 **它没有验证生产环境跨帧 renderer cache、延迟批次、GPU render target 内容和 texture version 的恢复**。Application 接线不能直接把本夹具的 PASS 扩张为完整呈现 PASS。

Draw 不允许在重演中省略：它可能修改 VM transform/geometry、layers、compositor/Stage 等可回滚状态。后续生产方案必须保留这些语义，同时单独控制 GPU 生成和最终提交。不能把生产 renderer.flush 当成无害的“清空”：生产 flush 会真正提交绘图到 GPU，discard 与 flush 语义不同。

## 资源、失败与提交次序

正常事务约定：

`Begin journal → Begin World → Update → semantic Draw → End World → End journal`

恢复约定：

`End 所有 observer → Validate 完整 owner/resource 图 → 完整 UndoTo → Restore typed owner → 退休跨越的全部 checkpoint lease`

Pause/结果/Ending/换关等资源边界需要在破坏操作前拦住，回退部分帧，清空可回滚历史，然后在 P0 confirmed 原样执行。Stage overlap、资源释放和菜单图目前不在 World::CanCapture 范围内。

journal 运行中容量/overlap 捕获失败必须拒绝 seal/提交；有些 first-write 钩子失败后原函数仍会写，不能误写成“失败就没有副作用”。该场景需由 Application 提供确定的失败处置，不能继续当作有效预测帧使用。

## 成本数字如何解读

测试记录的单 checkpoint 峰值为 **657,523 字节 / 597 blocks**。这是上述早期关卡和 8 帧 correction 场景的实际观测，仅计 journal 的原始字节与 blocks：

- 不含 typed snapshot 的 vector/map/string/强引用开销。
- 不含本测试全状态 oracle、绘图录制、archive 和高水位 retained storage。
- 不代表后期高密度弹幕、Boss/ECL 多线程或所有 Bomb 阶段最坏值。
- 没有 CPU throttle、浏览器 rAF、direct/relay 网络、帧提交间隔或实际设备测试。

因此可以据此确认本轮没有默认每帧全池复制；不能据此承诺 4× CPU 性能。下一层重点应是保留世界事务正确性、补上主机 texture/renderer/output owner，随后才测有意义的浏览器端成本和流畅度。

## 关联经验

- [World owner 清单](TH11MP-WORLD-OWNERS.zh-CN.md)
- [本轮持续工作日志](TH11MP-ROLLBACK-WORKLOG-20261013.zh-CN.md)
- [Bullet / Item](TH11MP-ROLLBACK-POOLS.zh-CN.md)
- [Enemy / ECL](TH11MP-ROLLBACK-ENEMY-ECL.zh-CN.md)
- [Laser](TH11MP-ROLLBACK-LASERS.zh-CN.md)
- [Deformation](TH11MP-ROLLBACK-DEFORMATION.zh-CN.md)

后续改源或改变资源、Draw/host 顺序以后，应保留本收据，不覆盖旧目录，并以新的源身份重新运行相应 gate。

## Application 呈现接线的只读复审（本轮验证之后）

以下为对当前真实源码和根代理拟定方案的静态审计，尚不是新增运行结果。

### 两级队列与 pending 的具体风险

- `game/AnmRenderer.cpp::flush` 把自己的 vertices 交给 ZunGraphics；`invalidate` 首先 flush，再失效 texture/blend/filter/UV cache。它不是 discard。
- `portable/sdl/Renderer.cpp::flush` 把后端 batch 真正 issue 到 GPU；`discard` 只清批次，并不清 `pending`。
- `Renderer::present(id)` 保存的只是 handle；`commit` 以后读取该 handle 当时的 GPU surface、blit 到默认 framebuffer 并 Swap。它没有冻结一份像素副本。
- `Application.cpp` 现有浏览器 callback 会在多个逻辑 tick 后调用 backend.commit。若开始回滚时仍有旧 pending，重演即使不再 present，只要改写 screen FBO，旧 pending 也能使 callback 把中间世界画面显示出来。

因此根代理拟新增显式 `cancel_present()` 清 pending：开始 Undo/纠正前先 flush 两级队列，再取消 pending；跨 callback 重演只完成事务内 Draw，不 present；只有纠正抵达完整前沿或正常 forward 的世界、纹理、journal 全部成功封帧后才 present。失败、barrier、abort 路径也必须清 pending。不能用 advanced 数量代替“可提交完整帧”的状态。

### 保留上一次画面的源码依据

`GraphicsDevice::initialize` 将 target 设为 screen handle 1；`select_target(nullptr,0)` 仍选择 handle 1。后端 `surface/target` 为这些 handle 建立独立的 GL framebuffer。正常 draw、clear、copy/resample 都针对相应 FBO。

当前呈现链路中，`Renderer::commit` 才将 DRAW_FRAMEBUFFER 设为 0、blit 并 Swap。由此可推断：取消 pending 后，历史 Draw 改写内部 screen FBO 不会直接改写已显示的默认 framebuffer，callback 的 commit 会因 pending=0 返回 false，从而保留玩家最后看到的完整画面。这一结论来自代码路径；最终浏览器/实际 GPU 集成仍应验证，没有用 WASI oracle 冒充 GPU 实测。

### renderer 值与缓存的最小约束

1. 每个 Application checkpoint 显式保存后端 `State` 的纯值（pipeline、matrix、viewport、texture/target/depth、stride/layout），以及 AnmRenderer 的公开 camera/viewport/offset/fog/tint/多人指针与透明度值。不可 memcpy 整个 renderer，不能复制内部 vector、ZunGraphics 引用或 GL 资源头。
2. Undo 前保证 AnmRenderer vertices 和后端 batch 均已 flush；恢复后再 invalidate 材质缓存，且此时必须已经没有待提交顶点。恢复的 logical backend.state 可以与仍描述实际 GL 状态的后端 cache 作正常 diff；不要把 GL cache 当作世界字节倒回旧值。
3. `AnmRenderer::draw` mode 8 的 `uv_sprite/cached_uv` 确实保留“仅纵向滚动时沿用前纹理矩阵”的原始行为。不过完整 SceneCompositor::Begin 本来每帧 invalidate，因此严格以完整帧边界恢复、每个逻辑帧执行一次完整 Draw 时，不需要新增跨帧 UV cache 恢复。不能任意在半个 Draw 中断后靠重画补救。
4. GameSession::draw 会清/重建 AsciiRequest，调用 VM draw_callback；StageDraw 会写 stage camera/tint/animation值；Compositor 会写 copies 的颜色和 world camera offset。这些 Draw 必须属于每帧事务。重演不能只 Update；最后显示也不能为同一个逻辑 tick 再调用一次有副作用的 Draw。
5. 全部历史帧真实 GPU Draw 是当前可理解的基础路径。以后若为减负改为静默 GPU 提交，必须仍执行完整 semantic Draw，并维护同样的逻辑 pipeline/target/viewport/cache 行为；不能提前在上层返回跳过 Stage、VM callback 和 compositor。

### 本 oracle 仍未覆盖的呈现风险

测试每帧主动 reset RecordingGraphics pipeline，而实际应用主要在初始化配置 pipeline；因此当前 PASS 隔离了跨帧主机 state。Application 新增的 renderer 纯值 owner 应有相应恢复比较，不能假设 world checkpoint 已包含这些值。

CPU Draw trace 也没有比较 A/B render target 的像素、清理 region 外的旧内容、纹理版本同步或 FontDevice 输出。正常 SceneSchedule 每帧清理并重建 A/B，再合成 screen，为整帧重演提供了明确重建顺序；但在 Browser/GPU gate 之前，不能宣称所有后处理读取区域和持久纹理都已验证。主机 CPU 纹理必须完成自己的 first-write/Undo/dirty 同步，Pause capture 和资源变更仍在 P0 confirmed 边界执行。
