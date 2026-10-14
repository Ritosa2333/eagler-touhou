# TH11MP 固定 Bullet / Item 池回滚基础（2026-10-13）

## 状态与适用范围

本子阶段已实现并通过独立真实组件验证。**玩家预测仍未启用，未接 Application，也不是整作回滚、音频、网络或性能验收。**

实施树：`D:/workspace/eagler/worktrees/th11-rollback-smoothness`。
起始 TH11 HEAD `d32390730ae837fb1333bc1f1d03d746f4697936`；使用现有 pinned common `984a8a10834c35fe9ef86c4ffe03daf1fb61f87c`，未修改共享库。

本次先重读 [工作记录](TH11MP-ROLLBACK-WORKLOG-20261013.zh-CN.md) 和跨作架构第 8.7 节，遵守完整 inactive 首写前像、稳定地址、物理退休三份合同。固定池不需要把每次逻辑复用都延迟到确认之后；但 manager 和其资源必须比所有引用它的检查点活得更久。

## 生产改动

- `th11_web/cpp/game/BulletManager.hpp/.cpp`
- `th11_web/cpp/game/ItemManager.hpp/.cpp`

两个 manager 均增加 MP-only 接口：

```cpp
bool BeginRollbackFrame(Netplay::RollbackJournal&);
void EndRollbackFrame() noexcept;
bool RollbackFrameActive() const noexcept;
```

调用顺序：

1. 外层先对 journal 调用 `BeginFrame(frame)`，每逻辑帧独立 checkpoint；**不能使用 extendPrevious**。
2. 对本事务涉及的 manager 调用 `BeginRollbackFrame(journal)`。
3. 执行该帧完整逻辑与 semantic Draw。
4. 所有 manager 调用 `EndRollbackFrame()` 解除 observer，再由外层封闭 journal。
5. 只有完整事务成功且 journal 未失败才允许 MarkSimulated / 输出提交。
6. 恢复之前 observer 必须已解除；调用 `UndoTo`。manager / 固定 storage / 关联资源仍然存活。

本基础层本身不替外层执行第 3–6 步，也不截获任意外部裸指针写入。

### 捕获布局

两个 manager 逐项登记 metadata，不复制对象头、vtable、unique_ptr、引用或 observer。
开始时按 active 连段保存完整 slot；每事务有独立的 captured 位图，后续 inactive 槽首次写入时保存完整前像。
reset 只登记尚未保存的连续区间，再执行原本的全池清零，避免与 active 段产生 common 不允许的部分重叠。

Bullet metadata 包括 inherited rate/player/default_tangent、cursor、active_count、exclusion_squared、last_error、multiplayer_grazed、cancel 几何、heads/tails、fire_depth。
Item metadata 包括 player、cancel_cursor/cancel_spawn_count、active_count、last_error、targets/current_target、spawning_transfer 和 fan。

首写接入覆盖：

- Bullet spawn（包括被 exclusion 拒绝、仍写 position/speed/angle 的路径）、appearance、release、update_one、cancel、Draw、reset。
- Item ordinary spawn、cancel-item spawn、bind/activate、reset；transfer/fan/conversion 通过生产 spawn 接入并保留 sidecar metadata。
- 两类 mutable `at()` 在返回可修改引用前保存对应完整 slot；新增 const `at()` 不触发保存。
- 开始已 active 槽涵盖 Update、Draw、回调的后续写；帧内从 inactive 产生的槽由首写屏障先保存。

observer 和 captured 位图不入 journal。End 只解除观察，不封闭 journal；失败 Begin 也需要显式 End。

### 失败语义

common 的字节或块数容量失败会锁存 `Failed()` 并使 `EndFrame()` 失败。
**这不保证生产函数当场停止写入。** 例如 spawn 仍可能成功返回且改变槽，外层必须拒绝提交并走明确错误/终止处理，不能把失败事务当成可安全继续的世界。
当前 common 对 failed journal 不提供“撤销到失败中间态”的自动恢复保证。

当前记录必须逐帧独立。跨帧 extend 会改变 active 连段形状，可能与前帧登记的区域部分重叠；本接口明确不支持，不借 common 的一般能力宣称 title 适配已支持 checkpoint span。

## 明确排除的 owner

这里不捕获共享 AnmEnvironment 的 RNG/rate/reference positions、几何分配、外部世界回调结果、资源加载释放、音频、Replay、观战或输入历史。
嵌入 Bullet/Item 的 AnmVm 字节属于槽；其 geometry 的物理内存属于对应 ANM allocation owner，需在更高层同一事务组合。
测试专门验证，单独 Undo 两个 manager 不会回退外部 script RNG。

mutable 全池扫描会扩大捕获量。主执行已另将 `GameBattle::mp_hash()` 的 const_cast + at 改为 const at 读取，hash 字段未改。
其它 mutable 扫描目前优先保证正确性；后续只有在真实测量证实必要、且保留写入覆盖时才缩小范围。

## 验证与保留失败

专属入口：

```text
node portable/multiplayer/rollback-managers-test.mjs --out=artifacts/multiplayer-rollback-managers/o2-02
```

测试实现为 `portable/multiplayer/rollback-managers-test.cpp`；使用实际 BulletManager / ItemManager、ANM 解释器、renderer、数学函数和当前 common Journal；合成 ANM 数据和空设备边界 fixture，不读取原作 DATA，不启动浏览器。

| 运行 | 结果 |
| --- | --- |
| [o2-01 receipt](../../artifacts/multiplayer-rollback-managers/o2-01/receipt.json) | 命令 `GUzlLY1Iq95zhmD8s2kN-Y0H`，exit 1。普通版两 manager 编译后，MP 链接缺 Movement.cpp 的 normalize_angle/polar，以及 EnemyState.cpp 的 aim_angle；未进入测试。失败与 compiler.log 原样保留。 |
| [o2-02 receipt](../../artifacts/multiplayer-rollback-managers/o2-02/receipt.json) | 命令 `NXnxYd0Cd_L-FTemMJu4BEcB`，exit 0。补齐真实依赖，无 stub。**19 cases / 195 checks PASS**；ordinary 两 manager 编译通过；构建/测试前后源码身份相同。 |

o2-02 使用 wasm32-WASI、C++17、O2、ffp-contract=off、fno-strict-aliasing；WASM 441,618 bytes，SHA-256：

`b617fbc74a89bd69e85426b04358c9046487d01bd94fb1d175583e8839613fc5`

覆盖完整同实例 manager 对象表示与整个 pool 字节比较（比较对象头，不恢复对象头），包含 inactive/padding；
实际 spawn、update、Draw、重复复用、active/empty 交错后全池 reset、满池返回码、cancel-item 延迟激活、定向 transfer、fan、conversion、三帧双 owner 共用 journal、observer 解除，以及字节/块数不足拒绝封帧。

AnmManager + AnmRollback 成对列入链接依赖，但这份 suite 不把它们的动态 owner 恢复计入通过范围。
未运行：完整 Application 事务、真实资源 transition、网络延迟纠正、全部副作用 owner、高刷实际像素、4× CPU 性能、手机或公开网络。
