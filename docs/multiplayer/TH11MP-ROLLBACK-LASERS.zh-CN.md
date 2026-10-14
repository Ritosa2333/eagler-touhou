# TH11MP Laser 回滚组件：实现与验证边界

日期：2026-10-13。实施树：`D:/workspace/eagler/worktrees/th11-rollback-smoothness`。

本阶段完成的是 LaserManager 的独立 owner 回滚能力；尚未把 Application 的预测打开，也没有以组件测试替代完整世界、网络、浏览器或 4× CPU 流畅度验收。

## 源码范围与存储合同

生产改动限定为 `th11_web/cpp/game/LaserManager.hpp` 与 `LaserManager.cpp`。新增真实组件测试 `portable/multiplayer/laser-rollback-test.cpp` 及独立 runner `laser-rollback-test.mjs`。未修改 LaserState / LaserLine / LaserInfinite 的既存游戏语义。

普通非 MP 继续原来的 new/delete。MP 使用最多 256 个稳定地址槽，首次需要时分配 backing；每槽按 Line / Infinite 最大大小和最大对齐排列。WASI32 下 Line 为 3724 字节、Infinite 为 3760 字节，完整 storage stride 为 3760 字节，总 backing 为 962560 字节（940 KiB）。这里只说明分配规模，不代表实际运行成本已经得到性能验证。

`BeginRollbackFrame(journal)` / `EndRollbackFrame()` / `RollbackFrameActive()` 分别启动、停止和查询首次写入观察：

- 捕获纯值 manager metadata、sentinel/tail/链状态和 occupied 数组；帧开始 active 的完整槽立即捕获。
- 帧开始 inactive 槽在 placement construction / 清零之前保存完整 stride，包括较大旧类型尾部与 padding。不能只保存 active 位或新类型 payload。
- release 维持原有摘链与数量行为，MP 只改变逻辑占用，不释放物理槽；下一代可同址跨 Line / Infinite 复用。
- LaserState / Line / Infinite 已有 trivially-copyable 与 standard-layout 编译期门禁。禁止复制 manager 的 vptr、引用、unique_ptr 头；物理 backing 所有权不属于字节快照。
- 原容量、id 分配顺序、失败返回、初始化失败清理以及满容量时 nested spawn 的原行为不变。
- world、ANM、资源、输出副作用有独立 owner。组件 fixture 显式捕获其 effect ledger、rate、player；这不能证明完整 world 已覆盖。
- owner 和 backing 必须活到所有引用它们的 checkpoint 退休后。journal 失败锁会使 EndFrame 失败；不承诺容量不足后所有生产 mutation 自动无效或无写入。

接口目前只用于 fresh checkpoint；没有证明 extendPrevious 跨帧合并合同。

## 实际验证

根代理串行执行，nig 命令 ID：`Pfg2b5Vk40NI7H589Rh-ryUz`，exit 0。执行时间为 2026-10-13 07:17:24.720 至 07:17:47.829 UTC。

在实施树根目录运行：

```text
node portable/multiplayer/laser-rollback-test.mjs --output artifacts/rollback-smoothness/laser-component-01
```

该输出目录已存在收据，不可覆盖。将来复测须换新目录，失败收据也要保留。

收据：`artifacts/rollback-smoothness/laser-component-01/receipt.json`（schema `th11-laser-rollback-component/1`），`passed=true`、`sourceStable=true`。包含源码逐文件 SHA-256、聚合摘要、编译器与配置摘要、game/common HEAD 与 dirty 状态、各子命令参数/时间/exit、stdout/stderr 路径和 WASM 身份。

| lane | 结果 | WASM 字节数 | WASM SHA-256 |
| --- | --- | --- | --- |
| ordinary | 1 group / 21 checks，编译和执行 exit 0 | 367815 | `73f12980e8f7e0be05417517b2b66ad54ffe1bca5992752921327641cabf94ce` |
| multiplayer | 9 groups / 2108 checks，编译和执行 exit 0 | 444733 | `28a085c53227c447fe7e427ea5f87bbedbae908e680d44b94fbf3b70937ec216` |

源码聚合 SHA-256：`3b7aa0c8fbe96ea24765160166cd166f6d0b288dcdc39d6c481da514956493c7`。

关键源文件 SHA-256（以本次 receipt 为准，后续其他 owner 编辑 header 会产生新的源码身份）：

| 文件 | SHA-256 |
| --- | --- |
| LaserManager.cpp | `dbf40ba9e1d26d5f32b138b1d7c17f7dd2b72f926dd641bb480d46ec8124d0e8` |
| LaserManager.hpp | `900ee77704ec47ac82019b10592966daa52726c2442404dc685d28f2782e521d` |
| laser-rollback-test.cpp | `6985d50ac63453620ebc7101100e761113f5542beb553d6e4d3bfb9c29a155de` |
| laser-rollback-test.mjs | `d3cd196a0144aa691e136c030602a6930584d67bf2a59553c493bb4d22b70056` |

工具链：WASI SDK 34.0 的 clang++，目标 wasm32-wasip1，C++17，`-O2 -UNDEBUG -ffp-contract=off -fno-strict-aliasing -fno-exceptions -fno-rtti`，`TH11_DEVELOPMENT_HARNESS=0`。MP 单独增加 `TH11_MULTIPLAYER=1` 和 common journal。Node v24.13.0 运行 WASI；不是 Emscripten 正式 Web Release 构建。runner 观察全部 game / portable SDL 相关 header 以及 MP common include，编译前后与结束时检查身份未改变。

## Oracle 和已覆盖行为

测试链接真实 LaserManager / LaserState / LaserLine / LaserInfinite / 切割代码、真实 ANM VM/interpreter/geometry/render、真实 common journal。资源为自编 ANM scripts / sprites，无零售 data。完整 oracle 比较纯值 metadata 和全部 256 槽完整字节，并验证 occupied 与 intrusive chain、sentinel、tail 的对应关系；没有降低现有 hash 门禁。

实际覆盖：

1. 普通版本创建、update、draw、取消与 clear。
2. Line → Infinite → Line 跨类型同槽复用，多级 Undo 恢复完整旧尾部。
3. 首次分配后物理池保留，Undo 回空逻辑前沿，再创建得到相同地址。
4. inactive 旧字节、appearance 127 拒绝、初始化 sound 失败的清理与恢复。
5. 真实 rectangle cut / infinite cut 所产生的子 Line，以及外部 effect ledger 恢复。
6. 真实 0x100 reflection 在 Update 内触发 nested spawn。
7. 满 256 槽、容量不增加、原 full-spawn 行为、id wrap 与复用。
8. warning、ECL 经 find 的直接写入、frozen/paused、protected / unprotected circle、cancel-all 和 clear。
9. journal 未打开、重复 Begin、metadata 捕获容量失败、之后 inactive 首写容量失败；失败 EndFrame 与拒绝 Undo。

## 尚未证明的边界

这些结果证明本 fixture 中被测 owner 的恢复与实际行为一致，不证明三世界 exact 一致、完整 Stage/Enemy/Player/资源图覆盖、确认输出水位、网络波动处理或高回滚深度稳定帧间隔。没有移动设备或浏览器测试，也没有绝对性能承诺。

下一步是完成其余 owner 与世界事务，按“结束所有 observer → common UndoTo 全部字节 → typed owner metadata → 释放未来 checkpoint”的整体顺序集成，然后通过完整世界与有限性能门禁。不得因独立组件通过而提前宣称 TH11 可交付。

