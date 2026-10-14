# TH11MP ScreenDeformation 回滚组件验证记录

日期：2026-10-13。实施树：`D:/workspace/eagler/worktrees/th11-rollback-smoothness`。

本记录证明真实 ScreenDeformationPool 与 ANM owner 的独立恢复合同。**GameBattle / Bomb 的 pool 接线未由本次 runner 编译或执行，尚不能称为已验证；完整世界预测也未由此证明。**

## 实现范围与所有权

生产组件为 `th11_web/cpp/game/ScreenDeformation.hpp/.cpp` 和新增 `ScreenDeformationPool.hpp/.cpp`。MP 使用 world-owned 稳定 physical slot；typed frame 保存每个 logical slot 的 columns / rows / occupied / vertices / positions / animations 值，不复制 vector、unique_ptr、引用或多态对象头。

create 默认 17×17；release(nullptr) 返回 true，foreign / double release 返回 false。release 先按原语义标记 strip ANM erase，保留对象及 vector payload，直到所有旧 animations 在 manager.find 中不存在才允许复用。部分初始化失败也可能留下已经创建的 strip VM，因此该槽 payload 必须保留，不能马上销毁或清空其 callback backreference。

Undo 可降低 logical size，physical storage 高水位仍保留；随后重建获得相同 ScreenDeformation 地址。callback 持有对象地址，未持有 vector.data 指针；typed vector 值恢复允许 backing capacity 不同。普通非 MP 保留原 ScreenDeformation 析构语义；MP pool 负责语义退休，pool 里的物理对象不在最终析构时误删已恢复 timeline 的同号 ANM。

所有 observer 结束后，恢复顺序是：

1. common journal 完成全部字节 UndoTo。
2. ANM 与 deformation pool 等所有 typed owner 恢复 metadata / vector 值。
3. 才释放被撤销的未来 checkpoints / geometry leases。

不能提前释放未来 snapshot：其独占 geometry allocation 仍可能是 Undo 的目标地址。ANM script / sprite 等 resource 生命周期也有独立 owner；本测试资源全程存活。

## 已执行的命令与收据

根代理通过 nig 串行执行 `2Q8MtCdpembuQqX3s33woPxG`，exit 0。UTC 时间：2026-10-13T07:31:20.387Z 至 2026-10-13T07:31:41.123Z。

```text
node portable/multiplayer/deformation-rollback.mjs --output artifacts/rollback-smoothness/deformation-component-01
```

收据：`artifacts/rollback-smoothness/deformation-component-01/receipt.json`，schema `th11-deformation-rollback-component/1`，`passed=true`、`sourceStable=true`。已有收据目录不可覆盖，复测需新目录。

| lane | 结果 | WASM 字节数 | WASM SHA-256 |
| --- | --- | --- | --- |
| ordinary | 1 group(s) / 155 checks | 364787 | `0219b295337848cc90138e3d353f70bc1bc1e81924528bc53953cce9e3022c9b` |
| multiplayer | 7 group(s) / 809 checks | 500280 | `0a685fb38030fc10701cbca386a32191194396871cfcd2e03ef3d806acb6d1d5` |

源码聚合 SHA-256：`1d0e23db194d3673d739693b8699c0dec830744b3305840ba0a4aec3f36b810e`。

| 源文件 | 本次 SHA-256 |
| --- | --- |
| `portable/multiplayer/deformation-rollback.cpp` | `a24a121d9d10c85033853d5dac6f6e4294fef078a1362cad1f9fbd6d7414cf9d` |
| `portable/multiplayer/deformation-rollback.mjs` | `0ba4ac0cb6b43b23b59aea562aa734663a1347c867e140769c44391bcade194e` |
| `th11_web/cpp/game/ScreenDeformation.cpp` | `4e2f0c8722f14783dd81edb00cba49258239cd6d23b163d1296a3f6543b3bc87` |
| `th11_web/cpp/game/ScreenDeformation.hpp` | `a0d7d5c2d21594cfa1b7e656648ed1511f1d26cadfd0f1eaea96d20343334e5e` |
| `th11_web/cpp/game/ScreenDeformationPool.cpp` | `b5bd9bdc132604533aaf9092e126b4fc1bf4b1bcbb05160b760139d4695c686d` |
| `th11_web/cpp/game/ScreenDeformationPool.hpp` | `5fbb6bd881ea7da2112ac448d4a51cd1ce4f58f7440aaa098dc2408db09734f3` |

本次使用 WASI SDK 34.0 的 clang++、wasm32-wasip1、C++17、O2、关闭 harness；MP 单独开启 TH11_MULTIPLAYER 并链接真实 common journal，Node v24.13.0 执行。runner 保存完整编译/执行参数、编译器身份、stdout/stderr、game/common 源码状态；编译前后与最终检查源文件一致。它不是正式 Emscripten Web Release 构建。

## 真实组件 oracle

测试链接真实 ScreenDeformation / Pool、EnemyAnimations、EclContext、Movement、ANM manager/interpreter/geometry/renderer、Interpolation、GraphicsMath、SceneCamera。图形后端为只记录提交的 fixture；未替换 deformation / interpreter / geometry 逻辑。自编 scripts 81 / 82 和 sprite 资源不需要零售 data。

全部 logical slot（包括 retired / partial-init）比较完整 typed vector 内容和稳定对象地址；ANM oracle 比较完整 VM 字节、active/available、registry/layers、RNG、geometry 原指针与字节。fixture 对外部 EnemyState 做独立完整 Touch，不把 EnemyState 错算为 pool 捕获能力。

已通过的 7 组：

1. 普通路径 scripts 81 / 82、目标 layer 27 / 28、geometry 交错 strip、真实 draw callback、renderer、update 与析构延迟 erase。
2. 多级 checkpoint 中完整 vector 值、所有 ANM owner 与独立 EnemyState 恢复。
3. release 后旧 strip 仍活时拒绝复用，实际退休后同址复用不同 grid，再 Undo 还原 retired payload 和初始 active 图。
4. 首次未来创建 → Undo 到空 logical frontier → 保留 physical reserve → 以同一 deformation / VM 地址和 ID 再创建。
5. 真实 RNG 指令让第一次 sprite bind 成功、第二次失败；保留第一 strip、部分 vector 与 callback，阻止过早复用，再验证其复用和 Undo。
6. 未打开/重复 Begin、错误 owner/frame、observer active、已使用 frame、owner identity 过期、foreign / double / null release 与无效尺寸。
7. 共享 journal 容量失败拒绝 End / Undo；两个 geometry generation 的 lease 跨所有 Undo 与 metadata 恢复保持有效，只在之后释放未来独占 allocation。

测试没有故意提前释放 geometry 再 Undo，避免以触发 UAF 的方式验证错误顺序。

## 尚未覆盖与后续门禁

- 未编译/执行 GameBattle、BombController、PlayerRollback 的生产接线；其新增代码需要独立集成证据。
- 未证明完整 WorldRollback、资源切换图、三世界 exact、确认输出隔离或网络会话调度。
- 未运行手机、浏览器、4× CPU、输入延迟或回滚深度性能测试。typed vector snapshot 成本尚未测量。
- 本次收据早于后续 ANM literal destination pre-write hook；新增 self-modifying script 专项需要新收据，不能借用本次成功。
- 未更改既有 canonical hash 门禁，也不能因这份组件报告提前宣布 TH11 预测回滚可交付。

