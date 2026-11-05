# 公网联机

新版原生移植继续使用旧多玉版的入口及部署参数：

- `start.bat` 启动四模式首页，本地默认端口 **9871**。
- `start-lan.bat` 启动房间服务，默认端口 **9874**。
- 房间服务开启后运行 `start-public-tunnel.bat`，默认转发到 `127.0.0.1:9874`。
- UDP：`/lan.html?network=public`；强制 TURN 附加 `&ice=relay`；WebSocket 附加 `&transport=ws`。
- TURN 配置文件仍为目录根部 `turn-config.local.json`，字段及配置工具保持兼容；该文件被 Git 忽略，覆盖发行文件时保留原本机配置即可。

固定域名、反向代理和端口可沿用旧部署。更新后重启房间服务，所有人刷新并重新建房。新旧核心的联机协议不同，旧页面不能混入新版房间。

完整操作、依赖和网络说明见 [联机说明](README-NETPLAY.md)。
