# 东方鬼葬剑 1.10b 本地合作联机

本地房间使用 eagler-touhou Launcher、Relay 和各端已导入的 1.10b 游戏包。双人和三人共用一个原生关卡，各席位独立选择灵梦、魔理沙、妖梦的 A／B 机体。房主可选四档主线难度、Extra 和挑战模式。Extra 直接进入第七关。

- 资源加载完成后，房间播放原版标题曲并显示动态主菜单。准备与开始由房间控制，开局时同一个游戏画面进入房主选定的关卡。
- 残机、Bomb 和碎片按席位记录。耗尽残机进入半透明幽灵状态，通关后复活。靠近队友、松开射击并点按五次 Shift，可转移一条备用残机或救起幽灵。
- 按 C 发动特技。同一帧多人按 C 时由席位顺序决定谁先发动。一人发动后全队无敌，队友在特技期间按 C 无效。等原版机体脚本结束特技后，可以松开并重按 C。
- 任意席位可打开原版暂停菜单。在暂停或团灭菜单选择再挑战，会让全队回到主线第一关或 Extra 起点。暂停中也可以按 R 重开。结果页可用原版输入界面给多人 Replay 命名并保存。
- 房间的旁观席沿确认输入重建原关卡，支持中途加入和房间内重开。旁观和 Replay 使用同一组已确认的各席位输入。

支持 WebAssembly 多内存的浏览器可使用最多四帧预测。预测画面使用独立的 WebGL 缓冲，预测结束后恢复临时演算写过的 Wasm 内存页。存档、音频、切关和特技按键沿已确认的输入执行。房间也可选输入延迟模式。

## 构建与启动

在仓库根目录运行以下命令。引擎与游戏包路径见[单机联调说明](../thibp/README.md)。两个 Runtime 目标使用空目录。

~~~powershell
$env:EMSDK = 'PATH_TO_EMSDK'
$engine = 'PATH_TO_PH3_MODERN_ZIP'
$package = 'PATH_TO_THIBP_OFFLINE_PACKAGE'

python tools/ports/thibp/build_runtime.py $engine .cache/thibp-coop --coop-engine
python tools/ports/thibp/build_runtime.py $engine launcher/runtime/thibp
Copy-Item -LiteralPath .cache/thibp-coop -Destination launcher/runtime/thibp/multiplayer -Recurse
python tools/ports/thibp/prepare_launcher.py site-manifest $package .cache/thibp-mp-site.json --coop-engine

npm --prefix services/relay ci --omit=dev
$env:EAGLER_NETPLAY_RELAY_HOST = '127.0.0.1'
$env:EAGLER_NETPLAY_RELAY_PORT = '18143'
node services/relay/src/netplay-relay.mjs
~~~

另开一个终端，从仓库根目录设置启动器的本地站点清单。

~~~powershell
$env:EAGLER_DEVELOPMENT_SITE_MANIFEST = (Resolve-Path -LiteralPath .cache/thibp-mp-site.json).Path
$env:EAGLER_TOUHOU_NETPLAY_RELAY = 'ws://127.0.0.1:18143/'
$env:EAGLER_TOUHOU_HOST = '127.0.0.1'
Set-Location launcher
bun install --frozen-lockfile --ignore-scripts
node scripts/build-launcher.mjs
node scripts/serve.mjs 8834
~~~

在每个浏览器的[作品页](http://127.0.0.1:8834/?game=thibp)导入相同的游戏包，然后进入[联机大厅](http://127.0.0.1:8834/lobby.html?game=thibpmp)创建或加入房间。

## 回归

~~~text
node tools/ports/thibp-mp/check-room.mjs URL ZIP REPORT_DIR BROWSER PLAYWRIGHT 2 1
node tools/ports/thibp-mp/check-room.mjs URL ZIP REPORT_DIR BROWSER PLAYWRIGHT 3 4
~~~

房间回归从真实启动器导入游戏包，核对各席位同步、来宾输入和整队重开。`THIBP_TEST_GAMEOVER_RETRY=1` 验证团灭菜单的再挑战和重开后的联机输入。`THIBP_TEST_ROOM_TITLE=1` 检查房间等待时的原版主菜单、实际音频输出、预览输入和开局时的音乐切换。可通过 `THIBP_TEST_MIXED_LOADOUTS=1`、`THIBP_TEST_CHALLENGE=1`、`THIBP_TEST_RETRY_KEY=1` 检查选机、挑战模式和 R 键。`THIBP_TEST_VIEWER=1` 覆盖中途旁观、暂停、回到直播及重开接续，`THIBP_TEST_ROOM_REPLAY=1` 覆盖 Normal 重开后的原版命名保存和回放。`THIBP_TEST_ROLLBACK=1` 在 RTC 来宾按键前加入一段延迟突增，检查预测画面和实际输入纠正。

引擎回归使用单机说明中的 `check-browser.mjs` 入口。`--check-coop-engine` 搭配 `THIBP_TEST_COOP_SKILL=1` 可验证 C 键同时发动、全队无敌和解除限制。`THIBP_TEST_COOP_REWIND=1` 比较恢复前后的 Wasm 内存和确认帧的缓冲画面，并用相同的确认输入比对原生关卡状态。
