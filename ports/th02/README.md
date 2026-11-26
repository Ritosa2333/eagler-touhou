# TH02 东方封魔录 · SoEW Native

[适配检查](ADAPTATION.md) · [来源与子树维护](../../docs/migration/th02-eagler-20261025.md)

书库单人、二至三人合作和观战都运行 SoEW Native。单人的练习、设置、成绩和 `.rep` 在游戏内管理；合作从 Eagler 大厅开局。

## 构建与启动

按 [SoEW Native Web](native-solo/WEB.md) 构建 `soew-full` 并准备素材、字形和音源。在仓库根目录运行：

```powershell
node ports/th02/scripts/build-eagler.mjs --native-build=ports/th02/native-solo/build-web
npm --prefix launcher run build:ui-rewrite
node ports/th02/scripts/dev.mjs
```

用 `TH02_NATIVE_BUILD` 可指定另一 SoEW 构建目录。脚本启动 Launcher 和 Relay，测试入口为 [书库](http://127.0.0.1:18320/?test=th02) 和 [联机大厅](http://127.0.0.1:18320/lobby.html?test=th02)。Runtime 包含 SoEW WASM；DATA 提供原生素材、字形和音源。

## 联机与观战

建房后选难度、机体并准备即可开局。房间默认启用高速连射，房主关闭后大厅特殊玩法显示「禁用连射」。普通房和挑战房使用同一个引擎。联机按约 56.4 Hz 消费已确认的输入帧，经 WebRTC 或 Relay 传输；各席位按本机视角半透明绘制队友。观众可在开局后入场、暂停、回拖和返回直播。

安装 Launcher 和 Relay 的依赖，设置 `EAGLER_CHROME_PATH` 后运行：

```powershell
$env:PC98_LAUNCHER_URL='http://127.0.0.1:18320/'
node ports/th02/tests/launcher-native-browser.mjs
node ports/th02/tests/launcher-native-room-browser.mjs
node --test ports/th02/tests/native-coop-peers.test.mjs
$env:TH02_TEST_SEATS='3'; $env:TH02_TEST_ROUTE='relay'
node ports/th02/tests/native-coop-network-browser.mjs
```

大厅测试走实际建房、就座、开局和中途观战流程；传输测试以独立 WASM 实例验证缺帧停表和恢复后的状态一致性。传输测试设 `TH02_TEST_SEATS=2`、`TH02_TEST_ROUTE=rtc` 可覆盖 WebRTC 直连。
