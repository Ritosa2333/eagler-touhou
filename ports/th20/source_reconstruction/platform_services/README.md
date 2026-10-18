# 配置、时钟和平台服务源码

本模块提供新的 C++ 实现，生产库不加载原 EXE。`entry_adapter.cpp` 已接回恢复中的程序入口。原 EXE 只被可选 `th20_platform_cpu_compare` 测试读取、核验哈希并映射到独立测试进程。

| 原地址 | 源码 | 范围及验证 |
|---|---|---|
| 0x41fb10 | configuration.cpp / initialize_bindings | 两套手柄及键盘绑定，48 字节原 CPU 对照 |
| 0x4b9b80 | initialize_configuration_flags | 保留高位、清除低 9 位再设置 bit 7，原 CPU 对照 |
| 0x4b9c10 | initialize_configuration | 完整 176 字节配置对象，比较所有字节，包括未写入的 padding |
| 0x542e00 | clock.cpp / signed_counter_to_double | 原 SSE2 分支，有符号 64 位转换，原 CPU 对照 |
| 0x41cb10 | performance_clock / multimedia_clock / read_clock | 真实 QPC/timeGetTime，原 CPU 对照确定性样本、4 种舍入模式、结果及 offset 的 64 位比特 |
| 0x54a4e0 | set_rounding_mode | 双浮点单元的舍入控制，原 CPU 对照控制寄存器及返回值 |
| 0x410e70 | services.cpp / write_loose_file | CP932 路径、Win32 写入、区分打开失败 -1 与短写 -2；隔离文件执行测试 |
| 0x410aa0 | read_loose_file | 仅 loose-only 分支；归档选择仍由独立归档模块继续整合 |
| 0x4dc1c0 | load_configuration | 默认配置、验证、日志、disable_vsync 和回写，隔离目录执行测试 |
| 0x41aa70 | save_configuration | APPDATA 派生目录内 th20.cfg 保存，隔离执行测试 |
| 0x41b4f0 | initialize_platform / initialize_directories | 原系统设置调用、时钟初始化、存档/截图/回放路径；目录部分隔离执行测试 |

`cpu_validation.json`：66,270 项通过，其中配置解码边界的 14 项是源码契约检查，其余 66,256 项执行原机器码并对照源码。QPC 原函数测试替换的是 Win32 采样导入，给两侧相同输入；不改原指令。未覆盖 AVX512 转换分支、NaN/非规格化 offset、跨线程原锁调度。

真实文件服务另有 CTest，所有写入都在构建目录新建的测试子目录内。未执行 `initialize_platform` 的屏保/电源设置修改，也未验证完整 D3D 游戏或回放等价性。真实短写失败 -2 尚无主机故障注入测试。

精确恢复中保留：显示模式比较为 **signed int32 < 10**，没有下界；fallback 时钟比较 offset 与毫秒、随后 offset 乘 1000 的原顺序；写入器 CP932 与 `std::filesystem` 活动文件代码页分开；配置初始化保留 padding 及 flags 高位。`default_configuration()` 将原未初始化栈 padding 定义为零，避免未定义读取，不能声称未初始化字节的序列化相同。短文件原分支会越界，新解码器明确拒绝。原写入失败日志缺少 `%s` 实参，源码使用实际路径。路径转换溢出显式报错，原错误域未保证等价。

`evidence/` 保存从哈希锁定 EXE 提取的指令及字符串证据，`extract_evidence.py` 可重建。完整游戏还依赖渲染、输入、音频、ANM 和实体系统源码；这里没有这些系统的占位实现。
