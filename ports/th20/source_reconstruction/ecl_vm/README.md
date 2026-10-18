# ECL 通用执行器

`vm.cpp` / `math.cpp` 是独立编译的 C++ 实现，不加载原 EXE、不含机器码回退。真正通用分发是 **0x0053b5c0**；**0x0048c010** 是游戏实体分发，两者不能混为同一恢复范围。

已实现原通用 switch 的全部 **75 个 opcode**：0、1、10–24、30、31、40–47、50–97，包括同步/异步调用、返回、rank/time、类型栈、局部变量、运算、共享 RNG 算法、角度与全部标量插值模式。`opcode_table.json` 给出每个 case 的原地址和资源中出现次数。实体 opcode 与游戏变量由 `Engine` 的纯虚接口提供，无默认返回或空实现。

关键原布局：runtime 的 `time +0`、`subroutine +4`、`IP +8`、`stack +0xc`、`async id +0x24`、`manager +0x28`、`field +0x2c`、`rank +0x30`、`interpolators +0x34`、`flags +0x44`。栈内 `pointer +0x10`、`frame_base +0x14`，每个 typed 值为 4 字节类型标签和 4 字节值。当前 C++ 使用拥有内存的对象，不能当作原 STL/分配器 ABI 的替换。

原函数依据：参数解析 0x53e970/0x53ed50/0x53eb70/0x53eee0/0x53ec80/0x53efb0；目标地址 0x53e720/0x53e850/0x53e7c0；栈 0x53f260/0x53f0b0/0x540450/0x5405b0/0x540300；子程序调用 0x53f3b0；名字查找 0x540340；异步链 0x53e2b0/0x53e390/0x53e920/0x53e560；插值 0x42a110/0x454ef0；RNG wrapper 0x423ee0/0x4298e0，底层 RNG/Timer 复用既有恢复源码。

`cpu_validation.json` 目前记录 **41,067 次通过的原 CPU 对照**。比较返回码、时间位模式、sub/IP、SP/BP、4096 字节栈、指令头可变字段、完整插值记录、随机数状态。原入口未执行，只为 RNG 的单线程递归锁路径解析了 `GetCurrentThreadId`。同步返回、异步调用参数初始化已比较；异步分配和链遍历的原分配器未运行，另由 `source_tests.cpp` 检查真实 C++ 所有权、次序和参数传递。

对照揭示并修正了反编译误读：通用 tick 实际返回 int；RNG 返回值还取了 modulus；Hermite 先计算基函数再乘控制值，改变括号会产生 1–2 ULP 差异；95–97 的角度对象构造会先执行带 34 次循环上限的 wrap。此处保留这些实际行为。

浮点 NaN/Inf 与边界已用于基础运算和角度 opcode。原 CRT 的 NaN sin/cos/sqrt 探针在未初始化 CRT 的隔离镜像中故障；见 `unavailable_edge_probe.json`。新 CRT 的 NaN 分类测试通过，但不能据此宣称旧 CRT 的错误处理、errno、NaN payload 或所有数学输入都完全一致。无效脚本/越界栈/整数除法异常改为显式 C++ 错误；不声称非法输入时异常 ABI 相同。完整游戏尚需实体、图形及外围模块接入。

```powershell
cmake -S source_reconstruction/ecl_vm -B source_reconstruction/ecl_vm/build -G 'Visual Studio 16 2019' -A Win32 -DTH20_BUILD_ECL_ORACLE=ON
cmake --build source_reconstruction/ecl_vm/build --config Release --parallel
ctest --test-dir source_reconstruction/ecl_vm/build -C Release --output-on-failure
# 仅硬件对照需要用户原 EXE；库本身不需要
source_reconstruction/ecl_vm/build/Release/th20_ecl_cpu_compare.exe 原版EXE路径 source_reconstruction/ecl_vm/cpu_validation.json
python source_reconstruction/ecl_vm/export_opcode_evidence.py
```
