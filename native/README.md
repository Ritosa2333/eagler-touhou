# 东方 Online Flutter 启动器

桌面和移动启动器，布局和功能基准是网页提交 85941f0d56。游戏本体由统一原生入口预留。

当前设计还原集中于书架、游戏详情、单作设置和联机大厅，均覆盖桌面与手机。个人主页和全部账号界面使用正式站应用内网页，旧 Flutter 页面已归档。消息、云存档和联机配套继续支持，可复用网页；其余独立页面已归档并改用网页，详见[交付范围](docs/baseline.md#当前交付范围)。

## 本地运行

使用 Flutter 3.50.1。更换 SDK 前先复核 Windows 兼容补丁并完成对应验收。在仓库根目录运行以下命令。

```sh
cd native
flutter pub get
flutter analyze
flutter test --concurrency=1
flutter run -d windows
```

账号网页使用 [InAppWebView](https://inappwebview.dev/docs/intro/)，直接访问正式站。当前锁定提供 Linux 支持的 6.2.0-beta.3。Windows 还需要 WebView2 Runtime，网页数据放在应用支持目录；Linux 需要 WPE WebKit 与相应后端。五个平台共用网页容器与会话衔接，Linux 实际编译及各平台系统引擎回归待完成，详见[账号入口](docs/account.md)与[平台依赖](docs/platforms.md)。

Windows 构建依赖 Visual Studio 的 C++ 桌面开发工具、对应工具集的 C++ ATL、Windows SDK、NuGet CLI 和 Flutter 插件目录链接支持。运行 `flutter build windows --release` 后，可从 `build/windows/x64/runner/Release/` 启动 `touhouonline.exe`。分发时一并复制同目录的库和数据。

Windows 窗口使用自定义标题栏。桌面实窗回归运行 `flutter test integration_test/desktop_window_test.dart -d windows`，通过实际入口检查窗口按钮、独立页视口和窄窗口。设置 `TOUHOU_DESKTOP_FRAME_CAPTURE` 可把页面图像写到指定的外部目录。测试会短暂最大化、最小化和调整自己的窗口，并在结束时恢复尺寸；关闭请求会被测试拦截。该验收还需连接正式站，读取登录文档并检查 1280、390、320 px 下账号页返回原游戏详情的布局；还检查注册页隐私链接在同一 WebView 中打开，以及 History API 的原生事件。还会读取真实登录和注册表单，在三种窗口宽度核对字段、字体及横向溢出，并保存 WebView2 网页区域图像。不会填写或提交凭据。

Windows 在 `windows/runner/main.cpp` 选择官方支持的 Skia 渲染路径。本机曾出现偏好读取完成后持续白屏，同一份应用产物切到 Skia 后恢复显示。Impeller 的重复启动也曾正常，触发条件还未稳定复现。 [Flutter 的 Windows 渲染配置](https://docs.flutter.dev/perf/impeller#windows) | [FlClash 的相关兼容记录](https://github.com/chen08209/FlClash/blob/main/.agents/flutter-upgrade.md#impeller-on-windows)

FlClash 在 Flutter 3.50 已恢复默认 Impeller。此处的 Skia 设置来自本机兼容样本，不代表 FlClash 当前默认；同一 AOT 的两条路径检查及焦点范围见[渲染器对照](archive/performance-audit-20261128/report.md#同一-aot-的渲染器对照)。

Windows 渲染优先选择高性能 GPU，并在首帧前通过 Flutter `ShaderWarmUp` 预编译详情页常用绘制操作。双显卡机器可能增加启动等待和耗电。悬停重绘按作品行隔离，隐藏大厅暂停计时刷新，云存档哈希与字节比较交给工作 isolate。复验方法、两轮 Release 数据及设备范围见[性能验收记录](archive/performance-20261128/report.md)。

Windows 网页纹理使用 Flutter 实际选中的图形适配器。构建时对锁定的 `flutter_inappwebview_windows 0.7.0-beta.3` 应用四文件兼容补丁，核对输入与输出哈希，生成目录承载修改，Pub 缓存沿用原文件。真实公开个人主页的空白对照、竖屏与返回验证见[性能排查](archive/performance-audit-20261128/report.md#网页纹理适配器修复)。调整该依赖版本时须重新核对补丁。

同一构建模块在插件注销消息通道前检查引擎是否仍可用，防止网页随窗口关闭时访问已拆除的引擎。SDK 包装层源码与补丁输出按哈希核对，修改写入构建目录，SDK 原文件不变。

移除兼容设置前，先在曾出现问题的机器上验证默认渲染：启动、连续拖动四边一分钟、最小化恢复及横竖窗口往返均持续出图。SDK 升级按锁定版本独立验收。

当前移动布局在 Windows 上用 320 和 390 px 的 Flutter 控件与截图核对。按当前要求，不运行 Android 构建或设备测试。其他平台依赖与待验项见[平台配置](docs/platforms.md)。

在 Windows 预览包解压目录运行 `./touhouonline.exe 'https://touhouonline.com/?game=th07'`，可直接打开作品详情。已安装的 Android 应用可接收 `touhouonline://open/?game=th07`。 [地址与返回规格](docs/library.md#作品链接)

房间分享地址也可作为 Windows 启动参数。Android 应用链接使用 `touhouonline://open/?j=…` 并带上原邀请的 `j` 参数。大厅的房间号输入框可粘贴原网页邀请。 [邀请直达](docs/lobby.md#邀请直达)

Windows 上项目和 Pub 缓存跨盘时，两个插件的 Kotlin 增量缓存可能报错。遇到 `Could not close incremental caches` 时，在本机的 Gradle 用户属性里设置 `kotlin.incremental=false` 再构建。 [Kotlin 编译缓存](https://kotlinlang.org/docs/gradle-compilation-and-caches.html)

## 规格与证据

[当前截图索引](docs/evidence/current/README.md)与历史对照分开。评估当前界面前运行 `node tools/check-specs.mjs --current`，核对截图及采集源码。

- [页面索引与网页、Pencil 对照](docs/baseline.md)
- [网页、Pencil 与 Flutter 截图对照](docs/comparison.md)
- [书架与主导航](docs/library.md)
- [游戏详情、通关进度与单作设置](docs/game.md)
- [云存档](docs/cloud-saves.md)
- [账户、公告与私信](docs/account.md)
- [手机设置与网页资料入口](docs/profile.md)
- [主题与背景](docs/appearance.md)
- [联机大厅](docs/lobby.md)
- [联机房间](docs/room.md)
- [站点与帮助](docs/site-content.md)
- [实现和验收进度](docs/acceptance.md)
- [平台启动工程、系统配置与构建](docs/platforms.md)

修改规格后运行 `node tools/check-specs.mjs`，检查需求编号的唯一性和本地截图、文档链接。该检查不代替页面验收。

在 `native/` 运行 `bun tools/generate-catalog.mjs <站点发布清单>`。生成器读取同仓库的产品目录、输入预设及归档的网页触控坐标，打包作品清单、书架封面和设置模板。运行 `node tools/generate-progress-catalog.mjs` 从网页生成通关机体名称与符卡关卡表。

## 截图复现

网页取已发布的固定版本。从仓库根目录运行 `node native/tools/capture-web-baseline.mjs <输出目录>`，环境变量 `CHROME_PATH` 指向 Chromium 浏览器。

Flutter 控件图像在 Windows 下复现。将 `TOUHOU_CAPTURE_DIR` 指向截图目录，将 `TOUHOU_CAPTURE_FONTS` 指向 Flutter SDK 的 `bin/cache/artifacts/material_fonts`。在 `native/` 执行 `flutter test tools/capture_native_baseline_test.dart --concurrency=1`。游客公告从正式站读取，截图数据随公告内容变化。

过渡中间帧使用 `TOUHOU_MOTION_CAPTURE` 和 `TOUHOU_CAPTURE_FONTS`，运行 `flutter test test/launcher_navigation_test.dart --concurrency=1`。将 `TOUHOU_NAV_TRACE` 指到同一目录可同时输出坐标记录。

侧栏与设置折叠使用 `TOUHOU_UNFOLD_CAPTURE`、`TOUHOU_NAV_TRACE` 和 `TOUHOU_CAPTURE_FONTS`。运行 `flutter test test/launcher_navigation_test.dart --name "Sidebar fold|Settings fold" --concurrency=1`，记录普通、站内减少动画和系统减少动画。Prism 依赖固定到 FlClash 使用的提交，排版尺寸由应用显式配置。

主题与背景的网页场景为 `background-settings`。Flutter 设置 `TOUHOU_APPEARANCE_CAPTURE` 和 `TOUHOU_CAPTURE_FONTS`，运行 `flutter test test/launcher_background_test.dart --concurrency=1`。

大字号布局运行 `flutter test test/launcher_text_scale_test.dart --concurrency=1`，覆盖 150% 和 200% 的三语导航、详情、设置与返回。Windows 验收加载实际系统字体检查断词。需要截图时，先创建输出目录，再设置 `TOUHOU_TEXT_SCALE_CAPTURE` 和 `TOUHOU_CAPTURE_FONTS`。[排版验收](docs/appearance.md#大字号与换行)

共用头像运行 `flutter test test/account_avatar_test.dart test/account_transport_test.dart --concurrency=1`。需要公告入口截图时，设置 `TOUHOU_AVATAR_CAPTURE` 和 `TOUHOU_CAPTURE_FONTS`，输出目录须已存在。图片使用目录内机体样例，HTTP 检查在本机运行。[头像验收](docs/inbox.md#共用头像回归)

账号、个人主页、资料编辑及裁切统一使用正式站应用内网页，旧 Flutter 专用测试和采集工具已归档。当前 Windows 网页与容器检查复用 `integration_test/desktop_window_test.dart`，截图范围与待验项见[账号验收](docs/account.md#windows-真实网页区域)。

联机大厅对照可指定网页截图场景 lobby-live,lobby-disconnected,lobby-capped,lobby-membership,lobby-rating-card,network-check。Flutter 设置 TOUHOU_LOBBY_CAPTURE 和 TOUHOU_CAPTURE_FONTS 后运行 flutter test test/lobby_page_test.dart --concurrency=1。设置 TOUHOU_VERIFY_LIVE_LOBBY=1 后另跑 flutter test test/lobby_directory_test.dart，可从正式站读取公开房间列表并刷新。

联机房间对照用网页场景 room-waiting,room-chat,room-rules,room-timing,room-quick-picker,room-rating-card,board-rating-card。独立排行榜已改为网页。Flutter 设置 TOUHOU_ROOM_CAPTURE 和 TOUHOU_CAPTURE_FONTS 后运行 flutter test test/room_page_test.dart --concurrency=1。验证真实房间协议时先在 services/relay 安装仓库依赖，再在 native 设置 TOUHOU_VERIFY_RELAY_ROOMS=1 并运行 flutter test test/room_session_test.dart --concurrency=1。测试在本机临时端口启动 Relay 和隔离账号桥，验证房主接任、作品规则、时序配置及发言，结束后关闭。网页短语或语音更新后，在 native/ 运行 node tools/generate-room-phrases.mjs 同步。 [房间规则与视觉对照](docs/room.md)

邀请直达使用网页场景 `room-invite-direct`。Flutter 设置 `TOUHOU_ROOM_LINK_CAPTURE` 和 `TOUHOU_CAPTURE_FONTS`，运行 `flutter test test/room_page_test.dart --concurrency=1`。

房间内游戏设置的网页场景为 `room-game-display-th05,room-game-display-th05-en,room-game-display-th05-ja`。创建截图目录后，设置 `TOUHOU_ROOM_SETTINGS_CAPTURE` 和 `TOUHOU_CAPTURE_FONTS`，运行 `flutter test test/room_page_test.dart --plain-name "room game settings affect" --concurrency=1`。 [房间内游戏设置](docs/room.md#房间内游戏设置)

限速房间的网页场景为 `room-touch-policy,room-touch-policy-ja,room-touch-policy-en`。Flutter 设置 `TOUHOU_ROOM_POLICY_CAPTURE` 与 `TOUHOU_CAPTURE_FONTS` 后运行 `flutter test test/room_page_test.dart --plain-name "enforces the room touch movement policy" --concurrency=1`。 [限速房间对照](docs/room.md#限速房间的触控设置)

通关及符卡对照使用网页场景 game-progress-guest,game-progress,spell-map,spell-list。Flutter 设置 TOUHOU_PROGRESS_CAPTURE 和 TOUHOU_CAPTURE_FONTS 后运行 `flutter test test/game_progress_test.dart --concurrency=1`，从详情进入登录并验证当前版本、错误重试、最高分、符卡点按与返回。

真实存档回归将 `TOUHOU_PROGRESS_INPUT` 指向同一批账号解析结果的 JSON，数据以作品编号为键。Flutter 同一套测试校验该批 th06、th07、th08、th10、th11、th12 的具体通关结果。网页可指定 `real-progress-th08,real-progress-th10,real-progress-th11,real-progress-th12` 截取对应状态。 [对照截图](docs/comparison.md#真实存档通关状态)

MIDI 控件运行 `flutter test test/external_midi_test.dart --concurrency=1`。设置 `TOUHOU_MIDI_CAPTURE` 与 `TOUHOU_CAPTURE_FONTS` 可采集三种宽度的资源可用样例。[可用性与截图](docs/game.md#外部-midi)说明它与固定发布清单的区别。

Windows 原生枚举使用 `flutter build windows --release --target tools/verify_midi_devices.dart`。将 `TOUHOU_MIDI_REPORT` 环境变量指向源码目录外的输出 JSON，再运行该验证程序。程序读取 MIDI 输出列表、写报告并退出。验证后用 `flutter build windows --release --target lib/main.dart` 重新构建启动器。

th08 符卡专项可将 `TOUHOU_TH08_SPELL_STATISTICS` 指向构造存档经账号解析器生成的统计对象。运行 `flutter test test/game_progress_test.dart --plain-name "th08 separates"` 验证从详情切换正作、练习、Last Word 和列表。移动宽度在本机渲染。 [对照截图](docs/comparison.md#th08-符卡收取)

详情好友区的网页场景为 `game-friend-activity`。Flutter 设置 `TOUHOU_FRIENDS_CAPTURE` 和 `TOUHOU_CAPTURE_FONTS`，运行 `flutter test test/game_progress_test.dart --concurrency=1`。两端读取同一份好友样例。

云存档对照用网页场景 `cloud-saves`、`cloud-files-en` 和 `cloud-files-ja`。Flutter 设置 `TOUHOU_CLOUD_CAPTURE` 和 `TOUHOU_CAPTURE_FONTS` 后运行 `flutter test test/cloud_save_page_test.dart --concurrency=1`。协议、存档校验及真实 th09 文件用 `flutter test test/cloud_saves_test.dart --concurrency=1` 验证，真实输入通过 `TOUHOU_CAPTURE_TH09_SCORE` 指定。

帮助、关于、玩法指南、服务状态、支持与独立排行榜交给系统浏览器，沿用启动器语言。旧原生实现、专用测试与生成工具已移至 [归档目录](archive/web-only/README.md)，不参与构建、静态检查和默认测试。

导航回归使用 `flutter test test/launcher_navigation_test.dart --concurrency=1`。将 `TOUHOU_NAV_TRACE` 指向外部目录可保存切换帧的布局记录。书架排序用 `flutter test test/library_order_test.dart --concurrency=1`，同状态网页截图场景为 `library-reverse,library-list-reverse`。最近游玩及日期联动用 `flutter test test/library_history_test.dart --concurrency=1`，同状态网页场景为 `library-recent,game-last-played`。

键盘排序使用网页场景 `library-sort-keyboard`。运行书架排序测试时设置 `TOUHOU_KEYBOARD_CAPTURE` 与 `TOUHOU_CAPTURE_FONTS` 可保存 Flutter 对照。

## 界面翻译

启动器语言保存在本机。共享文案由 `node tools/sync-localizations.mjs` 从仓库的网页三语词库生成 ARB。Flutter 通过 `flutter gen-l10n` 生成委托，`flutter pub get` 后正常构建即可。

原生专用文案和占位参数也在同一脚本维护。运行 `node tools/sync-localizations.mjs --check` 核对核心界面、窗口按钮与必要配套文案是否与生成源一致。校验兼容 Windows 换行符。

移动和桌面入口的语言回归用 `flutter test test/launcher_language_test.dart --concurrency=1`。设置 `TOUHOU_GAME_SETTINGS_CAPTURE` 和 `TOUHOU_CAPTURE_FONTS` 可同时采集 th06 显示与存档面板。网页场景为 `interface-language,library-en,library-ja`。Flutter 设置 `TOUHOU_LANGUAGE_CAPTURE` 和 `TOUHOU_CAPTURE_FONTS`，运行 `flutter test tools/capture_native_language_test.dart --concurrency=1`。

## 游戏文件验证

在游戏详情选择“导入资源”，使用与发布清单同名、同大小及同 SHA-256 的 DATA 文件。导入写入本机应用支持目录。

验证命令为 `flutter test tools/verify_game_data_test.dart --concurrency=1`。运行前将 `TOUHOU_CAPTURE_TH06_DATA` 指向真实的 `th06.data`，将 `TOUHOU_CAPTURE_SCRATCH` 指向外部临时目录。测试会在该目录创建隔离的应用数据并在完成后清掉自己的测试副本。

改键与录像管理三语对照用 `flutter test test/input_replay_locale_test.dart --concurrency=1`。设置 `TOUHOU_INPUT_REPLAY_CAPTURE` 和 `TOUHOU_CAPTURE_FONTS` 输出图像，设置 `TOUHOU_CAPTURE_TH09_REPLAY` 可验证导入后的菜单。

触控工作台三语使用网页场景 `touch-layout,touch-layout-en,touch-layout-fighting-ja`。Flutter 设置 `TOUHOU_TOUCH_EDITOR_CAPTURE` 和 `TOUHOU_CAPTURE_FONTS` 后运行 `flutter test test/touch_layout_editor_test.dart --concurrency=1`。 [触控工作台对照](docs/game.md#触控工作台三语)

存档和录像从游戏设置进入。运行 `flutter test tools/verify_game_files_test.dart --concurrency=1` 前，将 `TOUHOU_CAPTURE_TH09_SCORE` 和 `TOUHOU_CAPTURE_TH09_REPLAY` 分别指向真实的 th09 `score.dat` 与 `th9_01.rpy`，并设置同一个临时目录变量 `TOUHOU_CAPTURE_SCRATCH`。截图测试也可使用录像变量采集已导入的录像与操作菜单。

th15 存档流程使用 `flutter test test/game_save_flow_test.dart --concurrency=1` 验证。设置 `TOUHOU_SAVE_FLOW_CAPTURE` 和 `TOUHOU_CAPTURE_FONTS` 后可重拍覆盖确认和文件按钮。 [同状态对照](docs/game.md#th15-存档导入)

花映塚联机录像用 `flutter test test/replay_match_test.dart test/lobby_page_test.dart --concurrency=1` 验证。设置 `TOUHOU_PUBLIC_MATCH_REPLAY` 可使用公开对局下载的录像字节，配合 `TOUHOU_LOBBY_CAPTURE` 和 `TOUHOU_CAPTURE_FONTS` 记录三种宽度的导入反馈及对局资料。 [录像规格与截图](docs/game.md#联机录像)


游戏资源回归使用 `flutter test test/game_import_test.dart tools/verify_game_data_test.dart --concurrency=1`。运行文件回归时设置 `TOUHOU_CAPTURE_TH06_DATA`、`TOUHOU_CAPTURE_RELEASE_ZIP` 和 `TOUHOU_CAPTURE_SCRATCH`。截图另需 `TOUHOU_PACKAGE_CAPTURE`、`TOUHOU_CAPTURE_FONTS` 和同版本站点清单 `TOUHOU_PACKAGE_MANIFEST`。 [导入规格](docs/game.md#游戏资源导入)
