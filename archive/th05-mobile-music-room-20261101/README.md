# TH05 手机触控、Music Room 与原版音效

[测试站](https://test.touhou.vip/) 接入 640×480 原生触控坐标转换、C++ thprac 手机折叠面板，并恢复 Music Room 的 TH01–04 分类和全部 84 首原有编曲。音效恢复 uth05win 的 15 个 `SE/s01.wav`～`s15.wav`，TH05 的 PR #143 原版 BGM 与“幺乐团专辑版”继续可选。

## 运行行为

- 手机菜单支持点按确认、滑动选择和双指返回；游戏内拖动按灵敏度直接驱动原生自机，对话长按使用 Ctrl 跳过。
- 在触控布局设置中开启 thprac 按键，点击“作弊和高级设置”显示原生作弊、高级设置和信息面板。八个工具使用 TH05 自身的 F1–F7、U 语义，界面直接操作原 C++ 状态。
- UI 和游戏在按下时确定指针归属。原生按钮支持快速点按；从被动信息区开始的拖动可交回游戏，避免面板截断移动。
- 原版 SE 保留双声道数据，触发立即播放。重复同一音效执行原有 Stop／Reset／Play，不同音效可同时播放，音乐关闭时音效继续工作。
- Music Room 保留 TH01–05 共 107 首曲目。TH01–04 的 Arrange／Special MIX 来自原始 `MUSIC.DAT`，通过原 uth05win 的 PMDWin 离线制备为 Ogg；浏览器播放音频，游戏逻辑直接运行 C++ Wasm。

## 发布身份与验证

发布目录为 `/var/www/eagler-touhou-test/releases/th05-mobile-music-room-20261101-62b3d441ba86`，Runtime generation 为 `4a145e0ee939bf8b2ff25a4a19fde88cdb677fd8409012dd8530a983fe783966`，内容版本为 `9addb79494c191c6`。[发布回执](published.json)保留上一版目录及主站、beta 站身份；Nginx 配置保持原值。

| 证据 | 验证范围 |
| --- | --- |
| [Android 输入路径](public-android.json) | 公网启动器、菜单确认、角色移动、八个工具逐项切换、高级设置展开收起、原版 SE 输出 |
| [iOS 输入路径](public-ios.json) | 同一公网流程，通过 TouchEvent 路径验证 |
| [桌面中文与专辑版](public-browser.json) | 中文包、Unifont、thprac、Akyu 实际播放与原版 SE |
| [日文原版](public-vanilla-browser.json) | thprac 关闭、原版 BGM 全量解码、原版 SE |
| [公网曲目验收](public-android-music-room.json) | 从真实 Music Room 切换 TH04、TH03、TH02、TH01，并逐首解码全部 84 首音频，确认非静音 |
| [公网资源身份](public-files.json) | 273 个文件、188,724,949 字节校验一致，原 TH05 多人地址跳转正常 |
| [模块测试](unit.log) | 12 项协议、坐标、指针归属、对话长按、WAV 与音效并发检查 |
| [作品契约](port-check.json) | TH05 normal 的 11 项检查通过 |

手机自动化使用 Chromium 的触控与设备模拟，分别覆盖 Android PointerEvent 和 iOS TouchEvent 路径；结果对应这些浏览器环境。公共 HTTP 校验覆盖每一份发布音频文件的字节身份。

[作弊面板](public-android-cheats.png) · [高级设置](public-ios-advanced.png)

原版 SE 身份见[音效清单](../../ports/th05/resources/audio/sfx-original.json)，84 首编曲的来源和循环点见[Music Room 音频清单](../../ports/th05/resources/audio/music-room.json)。当前构建与操作说明见[开发指南](../../ports/th05/docs/DEVELOPMENT.md)。
