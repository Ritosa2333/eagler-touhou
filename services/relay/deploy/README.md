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

## TURN

TURN 独立于 Relay 部署，配置生成见 [`services/turn/`](../../turn/README.md)。Relay 与 coturn 的 `EAGLER_NETPLAY_TURN_SHARED_SECRET` 必须一致。
