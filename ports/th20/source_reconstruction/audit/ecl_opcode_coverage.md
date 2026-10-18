# 随附 ECL 与生产 opcode 覆盖审计

静态审计扫描 **21 个 ECL、730 个子程序、23,760 条指令、169 种 opcode**。重新读取资产所得直方图与 `reports/ecl_binary_scan.json` 完全一致。

原敌机分发器的 **224 个非默认 opcode 均有具体 C++ case 和生产适配器**；通用 VM 的 **75 个非默认 opcode** 也都有对应实现。未发现原分发器已知 opcode 缺少生产处理器，也未发现源码路由区间内的空 case。此结论是静态覆盖，不是行为等价或完整关卡验收。

## 方法和证据

- 校验只读原 EXE 的 SHA-256，从 PE 数据直接解出两级分发表，而非使用区间注释推测覆盖。`48c085` 减去 300，`48c090` 比较 703，`48c0a6` 查 704 字节表 `4966b8`，`48c0ad` 查目标表 `496400`。
- 704 个选择槽中，224 个映射到具体动作，480 个映射到 `4963df` 的 `xor eax,eax` 返回。上界之外同样跳到该默认返回。通用 VM `53b5c0` 的 98 项表位于 `53e128`，默认 `53def5` 调用实体分派。
- 逐函数提取首个分发 switch 的顶层数值 case，排除注释、字符串、其他辅助函数和嵌套 switch。连续标签必须落到实际非空语句体；对全部 65,536 个原始 opcode 值核对生产路由与具体处理器集合。
- 核对六类处理器的实际 adapter override 方法，以及生产 CMake 中的处理器/适配器成员；没有靠 oracle 的测试服务冒充生产实现。
- 新 JSON 保留每个 opcode 的原入口目标、源码 case/函数/行、调用服务、随附资源出现位置与全部输入哈希。本次未运行原版、源码游戏或构建。

## 具体处理器

| 处理器 | 独立 case 数量 | 源码 |
|---|---:|---|
| `execute_enemy_animation_opcode` | 45 | `source_reconstruction/gameplay/enemy_opcode_animation.cpp`，生产绑定 `source_reconstruction/gameplay/enemy_opcode_animation_adapter.cpp` |
| `execute_enemy_movement_opcode` | 49 | `source_reconstruction/gameplay/enemy_opcode_movement.cpp`，生产绑定 `source_reconstruction/gameplay/enemy_opcode_movement_adapter.cpp` |
| `execute_enemy_state_opcode` | 75 | `source_reconstruction/gameplay/enemy_opcode_state.cpp`，生产绑定 `source_reconstruction/gameplay/enemy_opcode_state_adapter.cpp` |
| `execute_enemy_bullet_opcode` | 34 | `source_reconstruction/gameplay/enemy_shot.cpp`，生产绑定 `source_reconstruction/gameplay/enemy_shot_adapter.cpp` |
| `execute_enemy_laser_opcode` | 15 | `source_reconstruction/gameplay/enemy_opcode_laser.cpp`，生产绑定 `source_reconstruction/gameplay/enemy_opcode_laser_adapter.cpp` |
| `execute_enemy_misc_opcode` | 6 | `source_reconstruction/gameplay/enemy_opcode_misc.cpp`，生产绑定 `source_reconstruction/gameplay/enemy_opcode_misc_adapter.cpp` |

## 随附脚本的分类

| 分类 | opcode 种数 | 指令数 |
|---|---:|---:|
| 通用 VM C++ 处理器 | 47 | 15,872 |
| 敌机 C++ 处理器 | 116 | 7,844 |
| 原版默认无动作 | 6 | 44 |

## 默认无动作不是遗漏

| opcode | 随附出现次数 | 原行为证据 | 首个位置 |
|---|---:|---|---|
| 901 | 1 | 两级表直接指向 `4963df` | `assets/raw/default.ecl` / `DebugSkipFunc` / `0x1364` |
| 902 | 1 | 两级表直接指向 `4963df` | `assets/raw/default.ecl` / `DebugSkipStopFunc` / `0x13a8` |
| 1010 | 1 | 大于上界 1003，`48c09a` 跳至 `4963df` | `assets/raw/common.ecl` / `WorldWave01` / `0x174` |
| 1011 | 22 | 大于上界 1003，`48c09a` 跳至 `4963df` | `assets/raw/common.ecl` / `WorldWave01` / `0x358` |
| 1012 | 1 | 大于上界 1003，`48c09a` 跳至 `4963df` | `assets/raw/common.ecl` / `WorldWave01` / `0x568` |
| 1013 | 18 | 大于上界 1003，`48c09a` 跳至 `4963df` | `assets/raw/common.ecl` / `WorldWave01` / `0x184` |

`569` 也确实指向原默认返回，随附文件没有使用它。源码显式排除 569 的行为正确。默认无动作仅指实体操作本身；外层 VM 仍执行原有指令前进和参数栈清理。

`if (!result) throw "Unrecovered enemy ECL opcode"` 是处理器漏接时的保护。当前 224 个路由值分别在实际处理器中有具体 case，因此未发现由缺少 case 触发此异常的已知 opcode。参数无效、资源缺失或下游异常不由这个集合检查排除。

## 保留的验证边界

此审计不检查每条指令的完整语义，也不代替实际子弹/激光 ETEX、回调表索引、特殊资源、分配失败或线程组合验证。静态存在的指令可能受难度、时间、分支及调用关系限制而不执行；扫描所有子程序不意味着跑过全部关卡。已有局部 CPU 报告的输入范围仍然适用，整局同输入/种子逐帧与全角色/关卡验收仍未完成。

复核：从工程根运行 `python source_reconstruction/audit/audit_ecl_opcode_coverage.py`。脚本只读取原 EXE/现有源码/资源，并重写本审计自己的 JSON 和 Markdown。
