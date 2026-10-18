# 东方锦上京 TH20 · 独立 C++ 源码重建

基于《东方锦上京 ～ Fossilized Wonders.》Windows 版恢复的 C++ 游戏工程。**当前已具备整套可独立构建的源码，完整游戏目标已编译、链接并实际运行。** 源码覆盖引擎、关卡脚本执行、自机与敌机、弹幕、渲染、音频、菜单、录像、存档和结局场景。

游戏程序由仓库内的源码生成，构建和运行不依赖原版 `th20.exe`。运行所需的 `th20.dat`、`thbgm.dat` 由使用者自行提供。这里的代码是依据目标二进制恢复、整理的实现。

**当前阶段：独立源码构建与实际运行已完成，整局 1:1 行为验收仍在进行。** 已有菜单、演示和实际跨关运行记录；全部角色、难度、关卡以及同输入、同种子的逐帧一致性仍需验证。

## 快速开始

### 环境

已验证的构建环境：

- Windows，Visual Studio 2019 C++ 工具链 14.29。
- Windows SDK 10.0.19041，CMake 3.24。
- **Win32 / x86，C++20**。对象布局与调用接口按原版 32 位程序恢复。
- 使用一键打包脚本时，另需 PowerShell 7 和 `rg`（ripgrep）。

```powershell
git clone https://github.com/Oracatt/Touhou20.git
Set-Location Touhou20
```

### 编译并运行

将 `-GameDirectory` 改为包含两个 DAT 文件的原游戏目录，在 PowerShell 7 中执行：

```powershell
.\build_source_game.ps1 -GameDirectory 'C:\Games\TH20'
.\dist\source_game\Play.cmd
```

脚本从源码构建 `th20_source.exe`，校验并复制两个资源包，生成启动脚本和构建清单。产物位于 `dist/source_game/`；通过 `Play.cmd` 启动时，存档写入包内 `userdata/`。原安装目录保持只读。

方向键移动，Z 确认 / 射击，X 炸弹，Shift 低速，Esc 暂停。

只编译游戏 EXE，可直接使用 CMake：

```powershell
cmake -S source_reconstruction -B build_sources -G "Visual Studio 16 2019" -A Win32
cmake --build build_sources --config RelWithDebInfo --target th20_source --parallel 4
```

结果位于 `build_sources/source_game/RelWithDebInfo/th20_source.exe`。手工运行时，需要把两个 DAT 放到 EXE 同目录，并以该目录为工作目录。详细环境、隔离存档和脚本工具链说明见 [SOURCE_BUILD.md](SOURCE_BUILD.md)。

## 源码内容

| 范围 | 主要目录 |
| --- | --- |
| 程序入口、窗口、帧调度、内存和共享状态 | `program_entry`、`platform_window`、`core_scheduler`、`runtime_core`、`runtime_state` |
| 资源包、输入与音频 | `archive`、`platform_services`、`input`、`audio_runtime` |
| 游戏会话、敌机、自机和 ECL 指令执行 | `game_session`、`gameplay`、`player_entity`、`ecl_vm` |
| 子弹、激光、Bomb、碰撞与道具 | `bullet_system`、`laser_system`、`bomb_system`、`damage_regions`、`item_system` |
| 异变石、武器覆盖层、符卡与计分 | `stone_menu`、`special_state`、`overlay_system`、`card_system`、`small_score` |
| ANM 动画、STD 背景、文字、HUD 与特效 | `sprite_renderer`、`stage_background`、`text_renderer`、`hud_system`、`effect_system`、`screen_effect` |
| 菜单、设置、暂停、帮助与成就 | `title_system`、`options_system`、`key_config`、`pause_system`、`help_system`、`notice_system`、`trophy_system` |
| 启动、关卡结算、通关、结局、录像与存档 | `startup_scene`、`stage_clear`、`stage_completion`、`ending_scene`、`replay_system`、`progress_state` |

以上目录均位于 [`source_reconstruction/`](source_reconstruction/README.md)。该文档列出各模块的实现内容、局部对照结果和具体验证范围。

## 已有构建与验证记录

| 项目 | 记录结果 |
| --- | --- |
| 独立源码构建 | 源码快照在空构建目录完整编译；实际生产依赖为 **54 个 MSBuild 项目、464 个编译输入** |
| 实际运行 | 已进入设置、主菜单、内置演示及实际游戏；诊断运行已有跨入第 3 关的状态记录 |
| ECL 指令覆盖 | 扫描 **21 个文件、730 个子程序、23,760 条指令**；原分发器的 **224 个实体 opcode、75 个通用 opcode** 均有具体源码处理器 |
| 局部回归 | Player **1,032,161** 项、Enemy **1,254,134** 项、共享池回归 **2,915,831** 项检查通过；各组范围与源码版本分别记录 |
| 可编辑资源脚本 | **299 个文本文件**；既有 **191 个 ECL / MSG / STD / ANM 档案**的逐字节重编译验证记录 |

构建、运行和内存问题修复见 [源码版运行记录](reports/SOURCE_GAME_RUNTIME.md)；指令路由证据见 [ECL 覆盖审计](source_reconstruction/audit/ecl_opcode_coverage.md)；资源重编译证据见 [资源恢复报告](reports/assets_recovery.md)。

打包生成的 `dist/source_game/build_manifest.json` 记录实际编译输入、EXE 和资源的 SHA-256。测试报告只对应其中记录的源码版本；源码修改后应按影响范围重新验证。静态指令覆盖、局部函数对照和运行到某关分别证明不同范围，整局行为仍需独立核对。

### 后续验收

- 覆盖全部角色、异变石组合、难度、关卡和结局，以及暂停、续关、录像与存档流程。
- 使用与原版相同的输入和随机种子，逐帧比较实体、弹幕、碰撞、分数及 RNG 状态，再核对画面和音频。
- 继续检查跨模块生命周期、线程时序、资源释放和长时间运行。已修复的问题及尚未覆盖的组合保留在运行记录和各模块报告中。

## 项目结构与开发

| 路径 | 用途 |
| --- | --- |
| `source_reconstruction/` | 独立游戏源码、CMake 工程、模块测试及审计 |
| `native_recovered/` | 共用随机数与计时器 C++ 实现 |
| `scripts/recovered/` | 可编辑的 ECL、ANM、MSG、STD 专用脚本文本 |
| `src/`、`include/`、`tests/` | PE / ECL 分析工具与早期核心验证 |
| `analysis/` | 反汇编、伪代码、调用关系等逆向证据 |
| `reports/` | 构建、资源恢复、差分与运行记录 |
| `tools/` | 资源恢复、工具链构建、打包与核验脚本 |
| `incremental/` | 早期机器码局部替换实验及其测试记录 |

独立模块及源码测试入口：

```powershell
.\build_sources.ps1
```

带原函数 CPU 对照的测试使用 `-Validate -GameDirectory 'C:\Games\TH20'`，另需对应的原 EXE 和已提取资产；它们是独立测试工具。编译生产游戏目标使用上面的 `build_source_game.ps1` 或显式 `th20_source` CMake 目标。资源恢复与脚本回编说明见 [源码构建说明](SOURCE_BUILD.md) 和 [资源恢复报告](reports/assets_recovery.md)。

Git 跟踪源码、脚本文本及文本分析和验证证据。构建目录、`dist/`、原游戏资源、第三方下载、运行存档和大型转储由 `.gitignore` 排除。为保留报告中的精确源码哈希，仓库禁用自动换行转换。日常操作见 [Git 管理约定](VERSION_CONTROL.md)。

## 分析目标与历史记录

分析目标为 **TH20 ver 1.00c，PE32 / x86**；原 EXE 的 SHA-256 为 `a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897`，文件清单见 [source_manifest.json](reports/source_manifest.json)。

`analysis/ghidra/` 中的 6,928 个函数伪代码用于分析，游戏构建入口是 `source_reconstruction/`。可编辑关卡脚本由对应 C++ 虚拟机执行，格式说明与恢复证据各自保留。

早期 `dist/playable` 保留原引擎机器码并替换七个函数；相关结果保存在 [历史实验记录](reports/PLAYABLE_BUILD.md)。当前源码版输出到 `dist/source_game`，历史实验的游戏级对照结果不计作独立源码版的验收结果。

## 参考来源

- [Touhou Toolkit](https://github.com/thpatch/thtk)：资源恢复工具，固定提交 `892114a0fcaa0bbdaaecf3cb4ad56f758683fb40`；兼容补丁见资源恢复报告。
- [thprac 的 TH20 模块](https://github.com/touhouworldcup/thprac/blob/585fae1aad4b720f350655a44e3f2ec7fc0dfa25/thprac/src/thprac/thprac_th20.cpp)：计时器入口地址的查找线索；实现以指定原 EXE 的汇编及差分为依据。
- [thcrap BGM 实现](https://github.com/thpatch/thcrap/blob/master/thcrap_tsa/src/bgm.cpp)：音乐格式字段与循环点含义的参考。

外部工具与参考代码的许可证保存在各自目录。原游戏资源不随仓库分发。
