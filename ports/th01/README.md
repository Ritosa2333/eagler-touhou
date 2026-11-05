# TH01 主项目适配

[适配检查](ADAPTATION.md) · [来源迁移](../../docs/migration/th01-eagler-20261025.md)
[原仓库说明](README-UPSTREAM.md) · [来源与子树维护](../../docs/migration/th01-eagler-20261025.md)

`ports/th01` 将 TH01 的原生 C++／WebAssembly 核心接入 Eagler Touhou 主启动器。单人分数存于主项目 `/savesth01`，多人分数与单人隔离；建房、落座、准备、开局、Relay/WebRTC 传输和离开由主项目负责。游戏内模拟与多人同步继续使用 TH01 原有核心。

在 monorepo 根目录运行：

```powershell
npm --prefix launcher run build:ui-rewrite
node ports/th01/scripts/dev.mjs
```

打开 `http://127.0.0.1:18310/?test=th01`。脚本从本目录代码与相邻 `../th01-multiorb-lab/web` 资源 构建 `build-eagler/` 和本地 `content/th01.data`，并启动主 Launcher 与 Relay。单人存档文件为 `scores.dat`；多人使用独立 `/savesth01mp` 命名空间。该核心不使用 PC-98 模拟器。

可重复验证：

```powershell
node ports/th01/tests/contracts.mjs
node ports/th01/tests/transport.mjs
node ports/th01/tests/browser.mjs
node ports/th01/tests/launcher-browser.mjs
node ports/th01/tests/launcher-room-browser.mjs
```

`browser.mjs` 使用实际 WASM 和资源包检查单人呈现、存档协议与多人帧同步；Launcher 测试从主项目验证安装和房间入口。`PC98_TEST_PLAYERS=3` 可将 Runtime 测试切换至三人，`PC98_TEST_RELAY=1` 强制使用 Relay。运行日志和截图写入忽略目录 `.cache/`。

入口仍标记为 `testOnly`。当前适配保留原生游戏和输入同步；主项目录像播放工作台与原生命令录像编解码不在此适配中。原始实验仓库不被构建脚本修改。来源提交与 Git 子树状态见 [迁移登记](../../docs/migration/th01-eagler-20261025.md)。

本地验收覆盖主 Launcher 单人安装与呈现、双人建房与加入、座位、预载、游戏检查、准备开局、键盘按下与释放、协议结束后同房再开，以及离开房间。真实 WASM 三人 Relay 对局已推进超过 480 个确认帧。游戏原生暂停菜单退出、移动设备与远程网络另作验收。

封面采用 THBWiki 原版外包装扫描，来源链接与 SHA-256 保存在 `artwork/cover-source.json`。原生逻辑来源包括 ReC98 `debloated` 提交 `28d59d31331e27fcbe61d1b65bd8c0cd618757d8`；构建产物 `NOTICE.html` 保留这一来源和图标许可。

构建器默认使用当前 `ports/th01/web` 的代码；通过 `--assets=<原运行时 web 目录>` 提供游戏资源和预编译核心。`PC98_ASSETS_DIR` 可覆盖开发脚本默认的资源路径。源码基线与资源清单见 [source-import.json](source-import.json)，每次构建的实际输入哈希写入 `content/source.json`。
