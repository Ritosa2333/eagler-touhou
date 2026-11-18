# th09 ImGui Overlay 原型

在独立 SDL3 窗口运行。大厅和房间使用现有手机布局的侧边视口，游戏内聊天放在左下角。F2 收起和恢复当前界面，Alt 打开游戏内聊天，常用九句支持数字键。使用仓库的 thprac ImGui 和指针归属工具，配色、作品封面、快捷发言 ID 来自 Launcher。

## 运行

在 Windows PowerShell 中执行以下命令。构建需要 UCRT64 g++、SDL3 开发包、nlohmann/json 头文件、Bun 和 ffmpeg。默认参数对应当前工作区。

```powershell
.\launcher\examples\imgui-overlay\build.ps1
$out = 'D:/workspace/eagler/artifacts/th09-windows/imgui-overlay'
bun .\launcher\examples\imgui-overlay\bridge.mts "$out/th09-imgui-overlay.exe"
```

默认使用演示房间。连接实际 Relay 时先运行 `npm ci --prefix launcher/examples/imgui-overlay`，再追加 `--relay=wss://...`。准备与开局通过桌面启动器的 `EAGLER_DESKTOP_OVERLAY` 接口调用对应版本的原生运行包。

```powershell
bun .\launcher\examples\imgui-overlay\bridge.mts --self-test
bun .\launcher\examples\imgui-overlay\bridge.mts --verify-relay ws://127.0.0.1:18142/
& "$out/th09-imgui-overlay.exe" "$out/game.json" "$out/game.bmp" 640 480
```

`--verify-relay` 在本地 Relay 上创建、加入、准备并开局。测试用本地 HTTP 替身核对桌面命令和实际开局参数。

## 接入范围

| 模块 | 原型入口 |
| --- | --- |
| 大厅 | 查询、筛选、建房、按房间号加入、旁观申请、网络连接诊断 |
| 等待室 | 选席、准备、昵称、移出与旁观管理、难度、公开方式、回滚、输入延迟、蓄力规则、TURN 与 Swarm、开局交接 |
| 游戏内 | 恢复 F2 界面、常用和全部发言、消息与草稿、静音、举报入口、回房入口 |
| 桌面 | 游戏检测、资源检查、Swarm 测速和开局参数采用启动器现有契约 |

独立演示使用 SDL 渲染。th09 的 F2 入口通过 GLES 使用同一套控件，并将联机请求交给随运行包启动的后台进程。自由发言取决于 Relay 公布的账号与聊天状态。
