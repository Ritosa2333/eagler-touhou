# Windows D3D8 Present 观察器

此测试工具为 TH06、TH07 的 32 位零售版构建 D3D8 代理，在游戏 `Present` 边界读取 Replay 状态。原始 `replay-verifier-present.bin` 是追加式 64 字节记录流，包含非活动行以标记 Demo 边界。

## 准备与构建

在装有 Visual Studio C++ x86 构建工具的 Windows 环境中，从此目录运行：

```powershell
.\build.ps1 -Game 6 -OutputDirectory C:\eagler\observer
```

`-Game` 可设为 `6` 或 `7`。将生成的 `d3d8.dll` 放到隔离的零售版游戏副本旁，并先把游戏配置设为有效窗口模式。若系统 D3D8 无法创建设备，可将未修改的 `d3d8to9.dll` 放在同目录供代理调用。

## 采集

从仓库根目录执行；路径替换为本机文件：

```powershell
python common/testkit/replay-verifier/windows-present-capture.py `
  --game 6 `
  --executable C:\oracle\th06.exe `
  --working-directory C:\oracle `
  --demo-count 1 `
  --output C:\eagler\th06-present.json
```

TH07 使用 `--game 7`。采集器启动游戏前检查零售版配置。`--timeout` 可调整等待秒数，默认 600 秒。只有观察到有效且递增的记录，采集才算完成。传入 `--injector PATH` 与 `--observer-dll PATH` 可启用单独注入；两个参数须同时提供。

采集器把标准化捕获结果写入 `--output` 指定的 JSON 文件；原始记录流保存在工作目录的 `replay-verifier-present.bin`。确认原始文件大小为 64 字节的整数倍后，可继续将记录转为标准化 JSONL，与候选运行结果比较。

下一步：[Replay Verifier](../README.md)。
