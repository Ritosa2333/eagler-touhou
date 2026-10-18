# C++ 源码恢复覆盖审计

本清单包含 Ghidra 当前识别的 **6,928 个入口**，不是已恢复的完整源码，也不保证已发现全部真实函数。
原有 RNG / Timer 严格组件基线仍为 **7 个入口**。独立归档算法登记 **7 个原地址映射**，调度器有 **29 个限定 CPU 行为覆盖映射**及 **8 个组合或部分映射**。这些类别不能加算成完整原函数恢复数量，也不能换算为游戏完成百分比。
目前没有从独立源码链接出的完整游戏。`compiled_but_unlinked` 明确表示独立库已编译，尚未接成完整游戏。

入口模块另登记 **4 个控制流映射**、**9 个字段/COM 操作映射**及 **2 个内联语句或 helper 覆盖映射**；只有编译/符号审计证据，不含原 CPU 或玩法等价验证。后两项 helper 覆盖不表示独立 ABI 导出。

## 编译与证据状态

| 模块 | 原地址映射 | 状态 |
| --- | ---: | --- |
| archive | 7 | compiled_algorithm_recovery_resource_validated: 7 |
| core_scheduler | 37 | compiled_scheduler_partial_or_composed: 8, compiled_scheduler_component_cpu_validated: 29 |
| native_core | 7 | validated_compilable_cpp_component: 7 |
| program_entry | 15 | compiled_field_or_com_operation_unvalidated: 9, compiled_control_flow_with_unrecovered_dependencies: 4, compiled_inline_or_helper_coverage_unvalidated: 2 |

## 分类（候选分类，不等于所有权结论）

| 分类 | 数量 |
| --- | ---: |
| unclassified_application_or_library | 6,308 |
| possible_crt_stl_or_library_function | 396 |
| recovered_algorithm_with_new_cpp_api | 7 |
| game_or_engine_source_diagnostic | 117 |
| partial_recovered_scheduler_composition | 8 |
| recovered_scheduler_library_component | 29 |
| compiled_field_or_com_operation | 9 |
| partial_recovered_control_flow | 4 |
| compiled_inline_or_helper_coverage | 2 |
| recovered_engine_component | 7 |
| verified_import_jump_thunk | 31 |
| internal_or_unresolved_thunk | 10 |

## 直接编译阻碍

- 693 个函数文件含反编译警告。
- 344 个函数含未解析动态目标的机器码间接调用，共 892 个调用位置。
- `undefinedN`、未声明全局地址、缺失对象布局、寄存器传参、分段变量访问、模板名称和转换辅助宏需要逐项恢复。补 typedef 或空函数只能掩盖部分语法错误，不能恢复语义。

原样复制全量伪代码后使用 MSVC `/TP /std:c++17 /Zs` 实测，退出码 **2**。没有加入 shim、stub 或替代实现。编译器在错误上限处终止，详见 `compile_probe/compiler_output.txt`；这些诊断不是全部恢复工作量。

## 与真正可编译模块的区别

- `native_recovered/native_core.hpp`：七个 RNG / Timer 组件，有明确原地址及验证证据。
- `src/`：新写的 PE / ECL 分析解析器；并非原游戏运行时恢复。
- `incremental/native_bridge/`：调用约定适配代码，依赖保留的原引擎，不满足纯源码要求。
- `source_reconstruction/archive/`：独立 C++ 的解密、文件名参数、持久字典 LZSS 和归档读写算法；285 条资源记录的 152,040,763 字节与独立提取结果相同，不宣称原对象、虚表、分配器或调用约定已重建。
- `source_reconstruction/core_scheduler/`：独立 C++ 的链表、迭代器、回调和帧调度组件。报告为 13,767 次原 CPU 对照及 1 次源码所有权路径检查；不覆盖原分配器、渲染 flush、多线程调度或回调异常展开。
- `source_reconstruction/program_entry/`：WinMain、三个帧节拍控制流、字段和 COM 操作已形成静态库；50 个函数及 13 个全局/环境仍未定义，不能独立链接。22 个函数、194 条静态调用边是分析证据，不能算作 22 个已恢复函数。
- `source_reconstruction/`：本轮其他恢复模块逐步加入；仅源码文件存在不计入已验证覆盖，需明确地址与验证报告登记。
- `analysis/ghidra/`：反编译证据。`scripts/recovered/`：专用脚本语言。均不能当作已经可以编译的完整 C++。

## 复跑

```powershell
python source_reconstruction/audit/audit_source.py --probe-compile
```

编译探针只生成临时源副本和诊断，不生成可运行游戏。新增恢复映射通过 `additional_recoveries.json` 明确登记，必须包含原地址、模块、证据状态、编译集成状态、源码及验证依据。`source_hash_bindings` 每次复跑与文件逐项核验；hash 不符会降低有效状态，保留原声明供追溯。源码快照绑定与验证报告内的源码绑定在 `hash_binding_origin` 区分。
