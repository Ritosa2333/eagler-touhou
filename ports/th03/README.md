# TH03 主项目适配

[适配检查](ADAPTATION.md) · [来源迁移](../../docs/migration/th03-eagler-20261025.md)
[原仓库说明](README-UPSTREAM.md) · [来源与子树维护](../../docs/migration/th03-eagler-20261025.md)

`ports/th03` 将《东方梦时空》的 DOSBox-X 运行时接入 Eagler Touhou。单人和多人均运行原游戏磁盘与模拟器核心；主项目负责安装、`/savesth03` 与 `/savesth03mp` 文件空间、两人对战房间、准备和开局。多人游戏保留梦时空原生选人流程，由两端同步进行。

在 monorepo 根目录运行：

```powershell
npm --prefix launcher run build:ui-rewrite
node ports/th03/scripts/dev.mjs
```

打开 `http://127.0.0.1:18330/?test=th03`。脚本从本目录代码与相邻 `../th03-netplay-lab/web` 资源 构建运行时和本地 `content/th03.data`，并启动主 Launcher 与 Relay。主项目存档文件为 `YUME.NEM`，配置文件为 `YUME.CFG`。TH03 房间为固定双人对战。

已分别构建 TH01–05 后，可在一个主项目入口切换五作。PowerShell 设置所需作品后启动同一服务：

```powershell
$env:EAGLER_DEVELOPMENT_GAMES='th01,th02,th03,th04,th05'
node ports/th03/scripts/dev.mjs
```

使用 `http://127.0.0.1:18330/?test=pc98` 查看游戏卡片，或 `http://127.0.0.1:18330/lobby.html?test=pc98` 从主大厅选择作品建房。

可重复验证：

```powershell
node ports/th03/tests/contracts.mjs
node ports/th03/tests/transport.mjs
node ports/th03/tests/browser.mjs
node ports/th03/tests/launcher-browser.mjs
node ports/th03/tests/launcher-room-browser.mjs
```

`browser.mjs` 使用真实 DOSBox-X、WASM 和 DATA 检查单人、双人同步、原生选人和 Relay；Launcher 测试覆盖 Package Store、主大厅预览、建房、落座、准备和开局。`PC98_TEST_RELAY=1` 可强制 Runtime 测试使用 Relay。运行日志与截图写入 `.cache/`。

入口仍标记为 `testOnly`。适配沿用原有选人、输入同步、暂停和文件写回路径；主项目录像工作台及多人观战不包含在这次接入范围。原实验仓库不被构建脚本修改。来源提交与 Git 子树状态见 [迁移登记](../../docs/migration/th03-eagler-20261025.md)。

本地验收覆盖主 Launcher 单人安装与呈现、双人建房与加入、座位、预载、游戏检查、准备开局、键盘按下与释放、协议结束后同房再开，以及离开房间。真实 DOSBox-X 双人 Relay 回滚模式已推进超过 500 个确认帧。游戏原生暂停菜单退出、移动设备与远程网络另作验收。

封面采用 THBWiki 原版外包装扫描，来源链接与 SHA-256 保存在 `artwork/cover-source.json`。

构建器默认使用当前 `ports/th03/web` 的代码；通过 `--assets=<原运行时 web 目录>` 提供游戏资源和预编译核心。`PC98_ASSETS_DIR` 可覆盖开发脚本默认的资源路径。源码基线与资源清单见 [source-import.json](source-import.json)，每次构建的实际输入哈希写入 `content/source.json`。
