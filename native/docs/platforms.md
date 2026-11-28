# 平台启动与原生边界

所有平台运行同一个 `lib/main.dart`，页面、账号客户端、导航和作品目录共用。平台目录承接 Flutter 引擎、插件注册、窗口和系统配置，游戏执行继续使用统一原生占位。

| 编号 | 平台 | 已配置 | 尚需验收 |
| --- | --- | --- | --- |
| PLAT-01 | Windows | 既有原生窗口、插件注册、程序图标和启动参数。 | 拖动、边缘缩放和 Snap 操作；当前 Release 与实际入口记录见[窗口验收](library.md#桌面窗口)。 |
| PLAT-02 | macOS | 官方 Runner、macOS 12 起的工程配置、联网和用户选取文件读写权限、项目图标。会话使用本应用的系统钥匙串。 | 在 Mac 编译、登录恢复、文件选择、窗口按钮、输入法、签名与公证。 |
| PLAT-03 | Linux | 官方 GTK Runner、启动参数、桌面入口、图标及 WPE WebKit 账号容器注册。 | Linux 编译和分发依赖、账号网页、桌面入口、钥匙串、文件选择、音频和窗口行为。 |
| PLAT-04 | iOS | 官方 Runner 与 SceneDelegate、iOS 15 起的工程配置、钥匙串权限和 `touhouonline` 链接、项目图标。 | 在 Mac 编译，iPhone/iPad 的冷启动、返回手势、安全区、旋转、键盘和系统选择器。 |
| PLAT-05 | Android 与 Chromebook | 既有 Android Runner，三种架构的历史预览包；共用可调整窗口的界面。 | 当前竖屏在 Windows 验证。Chromebook 采用支持 Android 应用的设备路径，需实测窗口、键鼠、触屏和系统返回。Linux 容器另按 Linux 项验收。 |
| PLAT-06 | 鸿蒙 | 暂缓独立平台接入。 | 需要恢复此平台工作时，先核对 Flutter 发行版和所有原生插件。 |

macOS、Linux 与 iOS 目录来自 Flutter 3.50.1 官方模板。本机为 Windows，本轮仅核对工程配置、资源、现有依赖版本及 Dart 回归，没有把生成启动工程记为平台编译成功。[添加桌面平台](https://docs.flutter.dev/platform-integration/desktop#add-desktop-support-to-an-existing-flutter-app)

[初次平台配置记录](evidence/platform-configuration.json)校验了 Apple 属性文件、三个 iOS 应用签名配置、最低系统版本、地址协议、Linux 桌面入口和 35 个图标条目。其依赖对照反映当时的图标生成器接入；当前版本以 pubspec.lock 为准。这些静态检查不代表系统权限、签名或原生插件已运行通过。

## 系统配置

macOS 调试和发布均启用 `network.client` 与 `files.user-selected.read-write`，分别供账号、资源和联机请求，以及用户选取的导入与导出文件使用。调试配置沿用 Flutter 的 JIT 与调试网络权限。[Flutter 沙盒说明](https://docs.flutter.dev/platform-integration/macos/building#entitlements-and-the-app-sandbox)

账号会话通过 `flutter_secure_storage` 进入系统安全存储。macOS 使用本应用独立的 Keychain，按插件手册设置 `usesDataProtectionKeychain: false`，不启用跨应用钥匙串共享。iOS 的三个应用构建配置指向 `Runner.entitlements`。签名和系统提示须在 Apple 设备验收。[插件配置](https://pub.dev/packages/flutter_secure_storage_darwin)

iOS 注册与 Android 相同的 `touhouonline` 地址，由现有导航解析作品和邀请。自定义地址声明不等于网站 HTTPS 关联，后者还需要网站及应用签名配套。Linux 桌面入口将链接作为启动参数交给同一入口。[作品链接](library.md#作品链接)

图标源文件为 `assets/app-icon.png`，复制自网页项目图标。`dart run flutter_launcher_icons` 按配置生成 iOS 和 macOS 图标，iOS 使用浅色基底去除透明通道。Windows 和 Android 图标不由此配置覆盖，Linux 安装同一 PNG。[Flutter Launcher Icons](https://pub.dev/packages/flutter_launcher_icons)

## 对应平台构建

先在目标系统配置 Flutter，再运行 `flutter pub get`。以下命令在项目的 `native/` 目录执行。

| 系统 | 命令 | 产物与限制 |
| --- | --- | --- |
| Windows | `flutter build windows --release` | 完整分发 `build/windows/x64/runner/Release/`。 |
| macOS | `flutter build macos --release` | 由 Xcode 工具链生成 `.app`，分发前另做签名和公证。 |
| Linux | `flutter build linux --release` | 分发整个 `build/linux/<架构>/release/bundle/`，包括桌面入口、图标和库。 |
| iOS | `flutter build ios --release --no-codesign` | 仅用于 Mac 上编译检查；安装和发布需要 Xcode 签名。 |

Linux 除 Flutter 的 GTK、Clang、CMake、Ninja 与 pkg-config 开发环境外，当前插件还需要 libsecret 和 GStreamer 的开发库。运行时需要可用的系统钥匙串服务与相应音频库，发行版打包时应按插件当前手册核对。[安全存储](https://pub.dev/packages/flutter_secure_storage_linux) | [音频](https://pub.dev/packages/audioplayers_linux)

Windows 构建应用内账号网页还需要将 [NuGet CLI](https://learn.microsoft.com/en-us/nuget/install-nuget-client-tools?tabs=windows#nugetexe-cli) 所在目录加入构建进程的 PATH。运行环境需要 WebView2 Runtime，容器启动时检查可用版本，并将浏览器数据写入应用支持目录。[InAppWebView Windows 配置](https://inappwebview.dev/docs/intro#setup-windows)

当前锁定 InAppWebView 6.2.0-beta.3 及其官方 Linux 实现 0.1.0-beta.1，采用完整官方预发布依赖组。Linux 容器与其他平台共用账号入口、Cookie 和返回逻辑，已生成原生及 Dart 注册。Linux 的实际编译与引擎回归尚未完成。[官方 Linux 包](https://pub.dev/packages/flutter_inappwebview_linux)

Linux 的 WPE 插件还要求 epoxy、Wayland Server、libsecret、libwpe 与 WPE WebKit 2.0 开发库，后端使用 WPEPlatform 与其 headless 组件，或官方插件支持的 FDO 后端。可先使用以下命令检查已安装库；缺失时按发行版提供的软件包安装。底层 nativeapi 还需要 GTK 3、X11 和 Xi 开发库。具体版本与依赖探测以所锁插件的 CMake 和构建入口为准。[WPE 配置](https://github.com/pichillilorenzo/flutter_inappwebview/blob/master/flutter_inappwebview_linux/WPE_BACKEND.md)

```sh
pkg-config --exists gtk+-3.0 x11 xi epoxy libsecret-1 wayland-server wpe-1.0 wpe-webkit-2.0
pkg-config --exists wpe-platform-2.0 wpe-platform-headless-2.0 || pkg-config --exists wpebackend-fdo-1.0
```

Apple 端使用 Xcode；原生依赖由 Flutter 工具及插件的 Swift Package/CocoaPods 配置处理。工程当前最低系统版本分别为 macOS 12 和 iOS 15，设备显示及各插件实际行为仍以上表验收为准。[iOS 环境](https://docs.flutter.dev/platform-integration/ios/setup)
