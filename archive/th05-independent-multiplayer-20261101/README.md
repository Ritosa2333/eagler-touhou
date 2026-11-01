# TH05 独立多人版构建与验收

本构建在 `ports/th05-mp` 独立维护多人玩法、界面和同步输入，单人保留 `ports/th05`。两侧继承 uth05win 子树历史，使用目录投影和普通版到 MP 的同步记录。

多人侧栏采用原版字形和 PR 的紧凑资源布局。显示选项从多人启动器传入，各客户端可使用不同的血条样式。

- [关卡与资源验收](native.json)：两人／三人、七关、道中／Boss 共 28 组，每组 480 帧；客户端使用不同显示样式，世界校验一致；通关经过 Ending、Staff 和祝贺流程。
- [双人直连](two-player-rtc.json)与[三人中继](three-player-relay.json)：实际 WebAssembly、按键、滑动、短触控暂停和返回房间。
- [启动器房间](launcher-two-player.json)：通过真正的选人、准备、启动、游戏和返回入口，使用独立生产多人构建。

测试范围为桌面 Chromium 的原生 WebGL／WebAssembly 流程。手机性能沿各作品的设备验收记录。
