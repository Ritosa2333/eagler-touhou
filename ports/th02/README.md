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

用 `TH02_NATIVE_BUILD` 可指定另一 SoEW 构建目录。脚本启动 Launcher 和 Relay，测试入口为 [书库](http://127.0.0.1:18320/?test=th02) 和 [联机大厅](http://127.0.0.1:18320/lobby.html?test=th02)。Runtime 包含 SoEW WASM，DATA 提供原生素材、字形和游戏音源，幺乐团录音由 Launcher 按曲下载。

## 源码许可与部署审核

SoEW Native 自有代码按 [GPL-3.0-only](native-solo/LICENSE) 分发，保留上游 [MIT 版权声明](native-solo/LICENSE-MIT-ORIGINAL)。Runtime 随包发布许可证和 [NOTICE](native-solo/NOTICE)。NOTICE 登记编入 WASM 的 th98port 版本和组件；公开部署需要有对应的分发授权，以及与二进制匹配的源码。游戏 DATA、字体和独立音源的权利与代码许可证分开核对。

## 触控与网页设置

书库和联机大厅共用触控布局及设置。限速滑动可沿任意方向移动，松手停止追踪。单人滑动写入原生录像，联机滑动进入同步输入；房间禁止无限速时，滑动遵守机体限速。单人触控提供 Backspace 与 F12，打开后可直接点按练习器面板。玩家只看见自己的判定点，观战保留全员视角。网页语言、音乐音源、音乐与音效音量、常显判定点和联机本机定位线由原生运行时读取。

## 联机与观战

建房后选难度、机体并准备即可开局。房间默认启用高速连射，房主关闭后大厅特殊玩法显示「禁用连射」。普通房和挑战房使用同一个引擎。联机按约 56.4 Hz 消费已确认的输入帧，经 WebRTC 或 Relay 传输；各席位按本机视角半透明绘制队友。观众可在开局后入场、暂停、回拖和返回直播。

任一席按 Esc 可暂停或继续全场。暂停期间仍交换已确认的输入，关卡时间停止。原生引擎在确认帧发布音效和曲目切换，浏览器独立驱动两个音轨。等待网络输入时 BGM 继续播放；观战赶进度和跳转时不播放历史音效。

安装 Launcher 和 Relay 的依赖，设置 `EAGLER_CHROME_PATH` 后运行：

```powershell
$env:PC98_LAUNCHER_URL='http://127.0.0.1:18320/'
node ports/th02/tests/launcher-native-browser.mjs
node ports/th02/tests/launcher-native-room-browser.mjs
node ports/th02/tests/launcher-native-browser.mjs --touch
node ports/th02/tests/launcher-native-browser.mjs --limited-touch
$env:TH02_TEST_TOUCH='1'; node ports/th02/tests/launcher-native-room-browser.mjs
node --test ports/th02/tests/hosted-touch.test.mjs ports/th02/tests/native-coop-peers.test.mjs
$env:TH02_TEST_SEATS='3'; $env:TH02_TEST_ROUTE='relay'
node ports/th02/tests/native-coop-network-browser.mjs
```

大厅测试覆盖建房、开局、任一席暂停和中途观战；设 `TH02_TEST_AKYU=1` 验证按曲下载与音效并行播放。传输测试验证缺帧时关卡停表、BGM 不中断以及恢复后的状态一致性；设 `TH02_TEST_SEATS=2`、`TH02_TEST_ROUTE=rtc` 覆盖 WebRTC 直连。
