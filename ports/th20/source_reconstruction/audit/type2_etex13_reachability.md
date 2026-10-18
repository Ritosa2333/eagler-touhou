# Type2 ETEX13：随附脚本静态可达性审计

结论：随附 21 个 ECL 中找到 4 处 ETEX13 设置、2 处 Type2 创建。两个创建子程序的局部控制流均先执行必经的 queue 0 重置，再只写入 ETEX7/3，因此局部流入 ETEX13 已排除。进入重置前的队列内容可以任意；本报告不宣称整个原生程序全局不可达。

这是有条件的静态排除，不能当作全游戏逐帧一致性验收，也不能消除 `laser_system/TYPE2_ETEX13.md` 所记录的原始越界行为差异。未运行游戏、原版函数或构建程序；生产文件未修改。

## 范围与可重跑方法

在工程根目录执行 `python source_reconstruction/audit/audit_type2_etex13_reachability.py`。仅写本审计 JSON/Markdown。JSON 记录全部输入 SHA-256、二进制偏移、原始参数、DSL 行号、CFG 边和源码证据。

- 重新解析 21 文件、730 子程序、23760 条指令；opcode 直方图与既有 `reports/ecl_binary_scan.json` 完全相同。
- 609/610/611/612 共 683 处 ETEX 设置（分别 128/32/515/8）。按 `enemy_shot.cpp` 的参数布局读取命令类型，并检查 parameter mask：命令类型参数动态引用数为 **0**。
- 609/610 的 ETEX 编号在参数 3，611/612 的编号在参数 2（均从 0 开始计数）。仅搜索文本中的数字 13 会误报颜色等参数。

## 全部 ETEX13 设置位置

| ECL / 子程序 | 二进制偏移 | DSL 行 | 指令 / queue / ETEX槽 | 同子程序普通弹发射 |
|---|---|---:|---|---|
| st01bs.ecl / `BossCard3_at` | `0x49f0` | 894 | 612 / 0 / 2 | ECL601 0x4b20 (行 902) |
| st04bs.ecl / `BossCard1_at` | `0x40bc` | 556 | 610 / 0 / 4 | ECL601 0x41b8 (行 561) |
| st04bs.ecl / `BossCard3_at` | `0x6024` | 901 | 610 / 0 / 4 | ECL601 0x6170 (行 909) |
| st07mbs.ecl / `MBossCard1_at` | `0x1520` | 215 | 610 / 0 / 5 | ECL601 0x171c (行 227) |

上述 4 处实际进入普通弹的配置队列；这里只证明存在 ECL601 消费位置，未声称每次实玩都会走到该 ETEX。它们各自子程序没有 ECL711。第 1 处的槽 2 来自重置后连续 append；其余槽号直接为常量。完整命令如下：

```text
st01bs.ecl:894 ins_612(0, 0, 13, 1, 8, 1, 1, 4000000.0f, 0.0f, 1.0f, 1.0f);
st04bs.ecl:556 ins_610(0, 4, 0, 13, 8, 8, 4, 1, -9999994.0f, 0.7853982f, 0.0f, 0.5f);
st04bs.ecl:901 ins_610(0, 4, 1, 13, 8, 8, 1, 1, -9999994.0f, 0.7853982f, -1.0f, -0.5f);
st07mbs.ecl:215 ins_610(0, 5, 1, 13, 8, 8, 1, 1, -9999994.0f, 0.7853982f, -1.0f, -0.5f);
```

## 两个 Type2 创建点的排除证据

对每个子程序从入口建立 CFG：ECL12 是无条件跳转，13/14 同时保留两条分支，1/10 退出。固定点计算支配集合；重置与每一处 ETEX 写入都必须支配 ECL711。条件分支过近似，因此不靠具体随机数、难度或循环次数证明。

### st05bs.ecl / `BossCard4_at`

- ECL711 位于 `0x6aec` / DSL 行 1122，queue 为常量 0；必经重置位于 `0x6938` / 行 1108。
- 每次创建时的完整命令类型向量为 `[7, 3]`；所有 ETEX 写入均支配创建点，参数中的 queue、槽号、类型均非动态引用。
- 子程序共 27 条指令，时间戳全部为 0，rank mask 全部为 255，跳转的目标时间全部为 0。无同步/异步调用、等待、queue copy 或 cursor 回退指令。故重置和创建之间不会把执行权让给其他 ECL runtime。
- 创建后 Type2 保存命令向量的值拷贝，之后在别的子程序里改队列也不会修改该曲线的命令。

### st07mbs.ecl / `MBossCard2_at`

- ECL711 位于 `0x1fd0` / DSL 行 309，queue 为常量 0；必经重置位于 `0x1c14` / 行 287。
- 每次创建时的完整命令类型向量为 `[7, 3, 3, 3, 3, 3, 3, 3]`；所有 ETEX 写入均支配创建点，参数中的 queue、槽号、类型均非动态引用。
- 子程序共 47 条指令，时间戳全部为 0，rank mask 全部为 255，跳转的目标时间全部为 0。无同步/异步调用、等待、queue copy 或 cursor 回退指令。故重置和创建之间不会把执行权让给其他 ECL runtime。
- 创建后 Type2 保存命令向量的值拷贝，之后在别的子程序里改队列也不会修改该曲线的命令。

`st07mbs.ecl` 确实同时包含 ETEX13 和 Type2：`MBossCard1_at` 的 ETEX13 可先写 queue 0，但 `MBossCard2_at` 的必经 ECL600 会丢弃它。不能因为它们位于同一个文件就判定曲线 reemit 可达。后者 `ins_602(0, 0, 13)` 的 13 是颜色，不是 ETEX 编号。

## 跨子程序、异步与其他生成路径

- 已检查的生产源码中，同步/异步调用从子程序 offset 0 开始；两个创建子程序内部没有调用或等待，ScriptManager 逐个执行 ECL runtime。因此在此顺序执行模型下，重置到创建之间无其他 ECL runtime 写入。原生程序跨 OS 线程写入、外部回调重入及全局别名没有单独完成全面审计，保留为未知；不能把源码顺序执行观察升级为全原生程序不可达证明。
- ECL614 确有复制队列的能力，但两个创建子程序均没有该指令。无需把全局所有队列别名求解完毕：进入 Type2 设置区间时队列内容可以视为任意值，必经重置仍会覆盖。
- 生产创建链是 ECL711 → `execute_enemy_laser_opcode` → adapter `create(kind=2)` → `spawn_laser` → `spawn_type2`。已检查的其他 Type2 生成路径只有现有曲线的 split；它复制原有参数，并在 path 分支将 command_index 置 99，不会添加 ETEX13。
- Type2 的 ETEX3/7 只建立轨迹段/写字段，不改命令类型。解释器里 ETEX6 只修改 words[4]，ETEX16 只改索引，也不会凭空合成 words[8]=13。普通弹 ETEX27 只支持 Type0/Type1，不能将这些普通弹 ETEX13 配置转成 Type2。

## 生产源码证据

| 文件:行 | 所支持的事实 |
|---|---|
| `source_reconstruction/gameplay/enemy_shot.cpp:22` | ECL600 replaces metadata with fresh defaults and resets the append cursor; prior/aliased commands are discarded. |
| `source_reconstruction/bullet_system/metadata.cpp:3` | Fresh metadata has two value-initialized zero command records. |
| `source_reconstruction/bullet_system/command.hpp:8` | Command words are initially zero. |
| `source_reconstruction/gameplay/enemy_shot.cpp:31` | Only the mapped ETEX argument is written to words[8]; append and explicit index setters differ. |
| `source_reconstruction/gameplay/enemy_shot.cpp:24` | The third argument of ECL602 sets color; a literal 13 here is not ETEX13. |
| `source_reconstruction/gameplay/enemy_shot.cpp:33` | Queue copy exists globally but is absent between reset and both Type2 creation sites. |
| `source_reconstruction/gameplay/enemy_opcode_laser.cpp:25` | ECL711 copies queued commands by value into Type2Parameters and creates kind 2. |
| `source_reconstruction/gameplay/enemy_opcode_laser_adapter.cpp:8` | Production adapter calls spawn_laser. |
| `source_reconstruction/laser_system/spawn.cpp:10` | Kind 2 dispatch calls spawn_type2. |
| `source_reconstruction/laser_system/type2.cpp:26` | Type2 owns a copy; subsequent queue writes cannot inject ETEX13 into an existing curve. |
| `source_reconstruction/laser_system/type2.cpp:44` | Curve splitting clones path and disables command replay in the child. |
| `source_reconstruction/laser_system/type2_cancellation.cpp:36` | The only additional Type2 spawn path copies existing Type2 parameters during splitting. |
| `source_reconstruction/laser_system/type2_commands.cpp:30` | The exceptional reemit path requires command.words[8] == 13. |
| `source_reconstruction/laser_system/type2_commands.cpp:23` | Command mutation decrements words[4], not words[8]; existing 3/7 codes do not synthesize 13. |
| `source_reconstruction/bullet_system/laser_spawn.cpp:8` | Bullet ETEX27 supports Type0 and Type1 only; it cannot turn a bullet ETEX13 list into Type2. |
| `source_reconstruction/gameplay/enemy_vm.cpp:114` | Synchronous/async subroutine calls enter at offset zero, before the dominating reset. |
| `source_reconstruction/gameplay/enemy_vm.cpp:132` | Interpreter yields at a future instruction time; both inspected bodies have time 0 and no waits. |
| `source_reconstruction/gameplay/enemy_vm.cpp:283` | Asynchronous runtimes run sequentially; there is no instruction-level preemption in a body. |

## 限制

- This is a static result for the hashed 21 supplied ECL resources and inspected production implementation, not an executed whole-game replay or proof that all recovered native semantics are equivalent.
- Cross-OS-thread writes, reentrant external callbacks and whole-program global aliases were not independently audited against the native program. Their absence is an assumption, not a proved global non-reachability result; unknown behavior outside the inspected sequential ECL execution is retained as unknown.
- The conclusion assumes normal subroutine entry at offset zero, valid VM rank/state, sequential runtime scheduling and memory-safe state. It does not model corrupt pointers, arbitrary instruction-pointer injection, external modified scripts or another program version.
- The CFG overapproximates both conditional branches and does not prove which difficulties, phases or loops execute. A writer/fire site is a syntactic candidate, not proof that a bullet reaches its ETEX13 at runtime.
- Dynamic angles, counts and other values remain unresolved. All 683 command-type arguments are literal; both Type2 queue operands, their reset queues and their command indices/types are literal. Incoming queues and cross-subroutine state may be arbitrary because the dominating reset discards them.
- The original Type2 ETEX13 out-of-bounds-copy limitation remains real for synthetic or modified inputs. This audit does not change that code, execute the invalid copy or count it as an oracle pass.

没有改动或弱化原 `TYPE2_ETEX13.md` 的行为边界，也没有把原始 0x420 越界复制算入成功 CPU 对照；本报告仅补齐“随附脚本是否有正常静态流入点”这一问题。
