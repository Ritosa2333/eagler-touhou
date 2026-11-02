# 部署 Relay

本目录提供环境变量、Nginx 和 systemd 样例。Relay、昵称数据库和日志应放在静态网站目录之外。

## 准备

1. 在仓库中运行 `npm ci && npm test`；生产环境建议用 `npm run package -- /path/to/new-relay-package` 生成独立发布目录。
2. 将 [`relay.env.example`](relay.env.example) 复制到服务机的 `/etc/eagler-touhou/relay.env`，填写实际配置。真实密钥与 SQLite 数据保存在服务机的仓库目录之外。
3. 将 [`nginx-relay.conf.example`](nginx-relay.conf.example) 的 `location /netplay-relay/` 放进公网 HTTPS 虚拟主机。默认上游为 `127.0.0.1:18142`；Relay 应绑定同一回环地址。
4. 让代理覆盖 `X-Real-IP` 为真实客户端地址，并将代理的精确 IP 配到 `EAGLER_NETPLAY_TRUSTED_PROXIES`。回环 Nginx 使用 `127.0.0.1`。
5. 安装 [`eagler-netplay-relay.service`](eagler-netplay-relay.service)，按实际发布目录修改工作目录和启动路径，再启用服务。

## 检查

- `nginx -t` 通过，WebSocket Upgrade 请求可经 `/netplay-relay/` 建立连接。
- 服务监听地址与 Nginx 上游相同，环境文件中的可信代理 IP 与连接来源一致。
- 用公网代理验证来自不同 IP 的房间配额相互隔离；离开房间后配额释放。
- 双端分别验证直连和 TURN 能进入 `route=rtc`。连接失败时两端收到 `route=failed` 并回到等待室。

## 对战链路

默认由 WebRTC 承载玩家输入，ICE 在可用的直连与 TURN 候选间选择。Relay 负责房间、信令与旁观。`EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK=0` 阻止服务在 WebRTC 失败时选择 WebSocket 对战链路；新 Runtime 也默认拒绝该回退。切换已有站点时，应配套发布包含新客户端策略的全部多人 Runtime，并验证失败后的等待室恢复。

兼容旧对战链路时，服务端设置 `EAGLER_NETPLAY_GAMEPLAY_RELAY_FALLBACK=1`，同时由客户端明确设置 `netplayAllowWebsocketFallback=true`。

## TURN

TURN 独立于 Relay 部署，配置生成见 [`services/turn/`](../../turn/README.md)。Relay 与 coturn 的 `EAGLER_NETPLAY_TURN_SHARED_SECRET` 必须一致。

设置 `EAGLER_NETPLAY_TURN_CONFIG_FILE` 后，Relay 按请求读取动态配置，每秒最多读取一次。JSON 将节点地址和对应密钥一起发布：

```json
{"urls":["turn:turn.example.com:3478?transport=udp","turn:turn.example.com:3478?transport=tcp"],"sharedSecret":"SAME_AS_COTURN"}
```

配置文件放在静态站点之外，授予 Relay 运行用户读取权限，并用临时文件加原子替换发布。文件最多 16 KiB，包含 1–8 个 `turn:` 或 `turns:` 地址和非空共享密钥。初始配置来自环境变量；动态配置通过校验后整体生效，读取失败或内容无效时继续使用上一份有效配置。短期凭据始终由对应节点的密钥签发，浏览器收到地址、短期用户名和凭据。

房间测速的 `state.roomProbe.iceServers`、游戏信令的 `peers.iceServers` 和网络诊断的 `diagnostic-ready.iceServers` 共用当前配置。节点切换作用于后续下发，已建立的连接继续使用原节点。独立发布包包含同一读取逻辑；`npm test` 验证源码与发布包三个入口的配置和密钥同步切换。
