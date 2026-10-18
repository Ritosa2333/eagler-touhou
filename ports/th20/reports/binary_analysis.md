# th20.exe 静态逆向证据

本报告描述从用户指定的 `th20.exe` 实际提取的静态证据。原游戏目录未改动；反汇编、函数候选和自动反编译结果都不是可独立运行的 C++ 游戏，也不构成玩法、画面、音频或回放的 1:1 等价证明。

## 输入身份

| 项目 | 实测结果 |
| --- | --- |
| SHA-256 | `a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897` |
| 文件长度 | 1,858,560 字节 |
| PE 类型 | PE32，Intel i386 |
| ImageBase | `0x00400000` |
| 入口 VA | `0x005435E0` |
| 节区 | 6 个 |
| `.text` VA / 实际虚拟长度 | `0x00401000` / 1,483,899 字节 |
| 导入 | 12 个 DLL，211 个入口 |
| 标识字符串 | `TH20 - Fossilized Wonder. ver 1.00c` |

PE32、ImageBase、入口和导入数与工程独立实现的 C++ PE 解析器结果一致；后者结果在 `reports/pe_cpp_inspection.json`。

导入包含 Direct3D 9、D3DX9、DirectInput 8、DirectSound、XInput 及 Win32 API。这些是原程序依赖的实测记录；API 导入本身不能恢复渲染和输入时序。

## 已落盘的独立分析

`tools/binary_analyze.py` 使用项目私有 pefile/Capstone，只读解析 PE 并对可执行节区进行线性反汇编。

| 产物（`analysis/binary/`） | 内容 |
| --- | --- |
| `pe_summary.json` | PE 头、各节区尺寸/熵、源文件哈希和统计 |
| `imports.csv` | 全部 211 个 IAT 条目 |
| `disassembly.asm` | `.text` 完整线性反汇编，466,332 个指令/数据项 |
| `function_candidates.csv` | 6,894 个函数入口候选和证据来源 |
| `calls.csv` | 36,607 个直接调用或绝对间接调用记录 |
| `xrefs.csv` | 65,753 个代码/导入/字符串交叉引用记录 |
| `strings.csv` | 5,099 个 ASCII / UTF-16LE 字符串候选 |
| `retained_source_layout.json` | 81 个原始源文件路径，由原 EXE 保留的诊断字符串恢复 |
| `source_location_xrefs.csv` | 156 个源位置字符串中的 155 个静态引用及最近的函数入口候选 |
| `allocation_size_evidence.csv` | 115 个诊断调用点连接到 96 个类型的常量分配包装函数，记录实测分配尺寸及初始化调用地址 |

线性反汇编可能将内嵌数据识别为指令；字符串候选可能包含机器码的偶然可打印片段。函数入口由直接调用、对齐/填充后的 EBP 序言、入口点及手工锚点推断，候选之间的距离只是范围上界，不能当成已证实的函数大小。手工语义标签以 `candidate_` 前缀记录。

原 EXE 保留了 `src/core/filectrl.cpp`、`src/game/ecl.cpp`、`src/game/bullet.cpp`、`src/game/player.cpp`、`src/game/replay.cpp` 等诊断字符串，也保留部分源行号和类型名。恢复这些路径和名字不等于取得这些 `.cpp` 文件的内容。

`tools/binary_allocations.py` 还沿着诊断字符串之后的直接调用，确认了“常量 `push` → `0x0054275D` 分配例程”的指令模式。例如，关联 `EnemyInf` 的分配尺寸为 `0x428`，`PlayerInf` 为 `0x1485C`，`EclResourceInf` 为 `0x23C`，`ReplayInf` 为 `0x360`。这些只是实测分配范围和相关诊断名称；没有据此推定成员偏移、对象所有权或单个游戏实体的完整布局。

## 自动反编译记录

Reko 0.12.4.0 对整个 PE 的分析遇到内部错误：`fn0042B5D0` 的 SSA 位宽处理失败，以及 `0x0045B072`、`0x0054EE87` 的类型推导失败。600 秒到达后内部超时取消，进程退出码仍为 0。最终写出 23 个 `.c` 后缀文件、15,923,557 字节，包含 6,308 个 `define` 形式的中间 IR 函数定义；内容有 `def`、`Mem` 和 SSA 变量，尚未生成正常的高层 C 伪代码。不能凭退出码或 `.c` 后缀判定成功。日志、中间结果和机器可读统计保存在 `analysis/reko/`，其中 `summary.json` 明确标记高层代码生成未完成。

另对 `0x00422CB0` 开始的 368 字节原样片段做了隔离实验，Reko 生成了 1 份 C 样式伪代码，位于 `analysis/reko_isolated/`。片段中对 `0x00542E90` 的调用在隔离环境下没有实现，因而生成结果包含未解析的调用和不可靠的类型/返回值。这个实验仅用于记录工具行为；不能替代工程已用机器码差分验证的 C++ 实现。

Ghidra 11.4.2 全程序分析实际完成，导出了其自动发现的 **6,928 个函数的 C 样式伪代码**。逐函数文件总长 7,299,082 字节，最终 API 级失败为 0，遗漏输出文件为 0。首次导出中 `0x0048C010` 达到 45 秒上限；对这个 ECL 主函数做了唯一一次 300 秒上限的重试，132.588 秒成功，生成 157,433 字节伪代码。首次失败清单保留为 `decompile_failures_first_pass.csv`，重试已合并到最终函数表、合并伪代码和统计，当前失败清单为空。

| 产物（`analysis/ghidra/`） | 内容 |
| --- | --- |
| `pseudocode/*.c` | 6,928 份逐函数 C 样式伪代码 |
| `th20_pseudocode.c` | 同一批结果的合并文件 |
| `functions.csv` / `function_ranges.csv` | Ghidra 函数元数据和地址区间；仍可能误识别或遗漏 |
| `function_xrefs.csv` | 指向函数入口的引用及调用者函数 |
| `strings.csv` / `string_xrefs.csv` | 1,741 个已定义字符串和引用 |
| `source_location_functions.csv` / `source_module_index.md` | 源诊断到函数/伪代码的索引：155 个引用，涉及 80 个有引用源路径；153 个引用能由 Ghidra 当前区间归属到函数 |
| `native_validation_links.csv` | 工程已做机器码差分的 7 个 C++ 例程与伪代码对应关系 |
| `pseudocode_warning_files.csv` | 693 个含反编译警告的文件；未发现 `halt_baddata` / `halt_unimplemented` 标记 |
| `summary.properties` / `export_validation.json` | 已合并重试的最终统计及文件存在性检查 |

没有坏指令标记不等于程序语义正确。`0x0048C010` 的 Ghidra 元数据当前只登记 196 字节、2 个区间，而反编译器继续恢复了更大的控制流；其 2 个源诊断引用仍未被当前函数区间覆盖。这是需要人工修正的边界问题，索引没有把最近地址候选强行当作已验证的边界。

全程序分析和导出使用 `tools/ghidra_run.ps1`、`tools/ghidra/ExportEvidence.java`；重试使用 `tools/ghidra/RetryEcl.java`，由 `tools/binary_merge_retry.py` 合并统计。单函数成功仅表示反编译器生成了 C 样式文本，不表示原有类型、全部函数、调用约定或游戏行为已完整恢复。

已经确认一处具体局限：`analysis/ghidra/pseudocode/00422cb0.c` 把 `__aullshr` 生成为看似普通的双参数调用，未准确表达原机器码以 `EDX:EAX` 和 `CL` 传递的寄存器约定。因此即使该函数被记为“反编译成功”，输出文本仍不能直接当作正确 C++。工程人工重建的 `native_core` 使用真实机器码差分验证；其 7 个例程共 39,216 个通过用例的证据在 `reports/native_validation.json`。该验证范围不扩展为整个游戏。

## 可重复执行

在工程目录执行：

```powershell
python tools/binary_analyze.py 'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.exe'
python tools/binary_source_map.py
python tools/binary_allocations.py
powershell -ExecutionPolicy Bypass -File tools/ghidra_run.ps1
python tools/binary_ghidra_index.py
```

第三方工具在 `third_party/`，下载缓存和 Python 包在 `.cache/`，未做系统级安装。工具下载地址、版本和官方校验值见 `reports/binary_toolchain.json`。Ghidra 项目保留在 `analysis/ghidra_project/`，便于后续继续标注和修正自动分析。

真正重建为独立 C++ 游戏仍需要按函数和数据结构实现渲染、输入、ANM/ECL 执行、弹幕和激光、玩家/敌人状态、音频、菜单、存档及回放等系统，并对原版建立逐帧和回放差分验证。此处的自动输出是这些工作的输入证据。
