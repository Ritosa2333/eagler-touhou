# 工程约定

目标是将用户指定的 TH20 程序恢复成完整、可编译、独立运行的 C++ 源码，并验证原版行为一致性。用户最新明确否定“保留原 EXE 机器码的局部替换版”作为交付，要求完整源码。此前 `dist/playable` 仅是历史局部替换实验，不满足当前目标。今后的交付程序不得运行、映射执行或内嵌原游戏机器码；原 EXE 只可作为只读分析和测试 oracle。原版资源/关卡数据可继续读取。禁止用空函数、假的返回值、近似弹幕、启动器或改名伪代码掩盖未恢复模块。

- 原游戏位于 `E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders`。默认只读，全部新产物写在工程内。
- 随附 readme 的 1.00a 标识过时；目标 EXE 内嵌 1.00c，SHA-256 必须匹配 `reports/source_manifest.json`。所有原 VA 以首选映像基址 `0x00400000` 表示。
- `native_recovered/` 是人工恢复、可编译并经机器码验证的 C++。`src/` 是独立 PE/ECL 解析器。`analysis/ghidra/pseudocode/` 是自动反编译证据，`scripts/recovered/` 是游戏专用 DSL；两者不能直接当作可编译 C++。
- `source_reconstruction/` 放新的独立源码模块和逐模块证据。未知外部依赖必须显式保留为未实现接口，记录未完成清单；编译一个静态库不能等同于完整游戏已链接或完成。
- 191 个资源脚本档案已从文本重建为原样字节。修改解析/编译工具后，运行 `python tools/verify_artifacts.py`，按需重跑恢复流程。不得靠回填原字节掩盖源码编译错误。
- 构建和已实现模块验证：`./build.ps1 -Validate`。该命令包含 C++ 测试、原文件哈希复核、ECL 扫描、资源校验、仿真与真实 CPU 差分。
- 增量可玩包构建：`./build_playable.ps1`，输出 `dist/playable/Play.cmd` 和 `th20_rebuilt.exe`。原版目录只读，启动脚本将 APPDATA 指向包内 userdata；实际 UI 验证另用 `incremental/verification` 的隔离副本。
- 增量链接由 `tools/build_playable.py` 完成：扩展导入表、七个 FF25 入口、完整重定位，保留原入口及原 IAT 地址。不可遗漏 ASLR 重定位或修改未验证函数。
- 游戏 ABI 以汇编为准：`timer_reset/mode/set/add` 除对象副作用还返回 `EAX=self`。实际 DLL ABI 测试见 `incremental/native_bridge/bridge_abi_validation.json`。
- `build_playable.ps1` 必跑实际 DLL ABI 检查和 `incremental/link_tests` 最终入口检查。后者通过两个基址的真实 FF25/IAT 路径，不能用直接调 DLL 的测试代替最终链接测试。
- 仿真器对某些 NaN payload 的传播与本机硬件不同。已有 330 个分歧输入全部用硬件原指令、x86 C++、x64 C++ 对照归因，见 `reports/native_nan_oracle_comparison.json`。不能仅为迎合仿真器修改已证实的原机行为。
- 原函数存在寄存器传参和 CRT 特殊 helper。自动伪代码里的普通函数签名不可靠，须核对原指令和调用者。
- 新恢复的函数需记录原地址、对象布局证据和适当的原机器码差分。浮点运算次序、32 位溢出、帧调度和 RNG 调用次数是玩法一致性的关键。
- 完整游戏验收需要同输入/同种子的逐帧状态和回放对照；局部函数通过不能代表游戏 1:1。

详细现状、工具缺口和下一步入口见 `README.md`、`reports/RECONSTRUCTION_ROADMAP.md` 与 `reports/binary_analysis.md`。
