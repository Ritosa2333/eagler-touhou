# 独立 C++ 源码构建

当前源码能够独立编译为 `th20_source.exe`，已实际进入菜单、演示与游戏关卡。仍在修复和验证整局流程，尚未证明所有角色、难度与关卡的 1:1 行为。它是从机器码分析恢复的实现，不是原作者的原始工程。

生产目标不链接、读取或执行原游戏 EXE。原版 `th20.dat`、`thbgm.dat` 是单独的资源输入，不包含在源码包内。隔离 CPU 对照测试需要原 EXE，但这些测试不链接到生产目标。

## 编译

已验证环境：Windows、Visual Studio 2019 C++ 工具链（14.29）、Windows SDK 10.0.19041、CMake 3.24，Win32/x86，C++20。不要改成 x64：对象布局、浮点次序与接口均按原 x86 还原。

```powershell
cmake -S source_reconstruction -B build_sources -G "Visual Studio 16 2019" -A Win32
cmake --build build_sources --config RelWithDebInfo --target th20_source --parallel 4
```

结果为 `build_sources/source_game/RelWithDebInfo/th20_source.exe`。这是实际游戏目标；无需构建名字含 `compare` 的隔离测试。

若已安装 PowerShell 7 和 ripgrep，也可直接运行打包脚本：

```powershell
.\build_source_game.ps1 -GameDirectory '你的游戏资源目录'
```

脚本校验两个 DAT 的哈希、冻结编译输入并生成 `dist/source_game/Play.cmd`。存档在包内 `userdata`，不修改原安装目录。

手工运行时，将两个 DAT 放到新 EXE 同目录，并以该目录为工作目录运行。建议设定独立 APPDATA：

```powershell
$env:APPDATA = Join-Path (Get-Location) 'test_userdata'
Set-Location build_sources/source_game/RelWithDebInfo
.\th20_source.exe
```

## 源码入口与验证

- `source_reconstruction/CMakeLists.txt`：完整模块组织；`link_probe/CMakeLists.txt`：生产目标的实际依赖。
- `program_entry/winmain.cpp`、`frame_schedulers.cpp`：程序入口与帧调度。
- `gameplay`、`player_entity`、`ecl_vm`：关卡、敌机、自机、脚本。
- `sprite_renderer`、`bullet_system`、`laser_system`：动画渲染、子弹、激光。
- `title_system`、`replay_system`、`progress_state`：菜单、录像与存档。

各模块及证据边界见 `source_reconstruction/README.md`；实际运行和已修复问题见 `reports/SOURCE_GAME_RUNTIME.md`。测试报告的哈希对应当时源码，不把历史报告直接视为修改后仍有效的证明。

`scripts/recovered/` 还包含关卡 ECL、动画 ANM、消息 MSG 和背景 STD 的可编辑脚本文本。它们由对应 C++ 虚拟机执行，是游戏专用脚本语言，不是 C++ 文件。既有报告记录了 191 个脚本档案的逐字节重编译对照；图像、音频和重编译二进制不放入源码包。若要重编这些脚本，`tools/asset_build_thtk.ps1` 提供固定版本 thtk 工具链的源码构建及本工程补丁，需另外下载其开源依赖；编译游戏 EXE 本身无需这些第三方工具。此包未包含原资源，重新验证资源对照需使用完整工程的已提取资产。

源码快照中的 `SOURCE_SNAPSHOT.json` 列出每个文件的 SHA-256。构建验证只证明此快照能够从源码链接，不能代替整游戏逐帧一致性验证。完整原始运行 dump、旧混合机器码实验和原游戏文件均不包含在源码快照内。
