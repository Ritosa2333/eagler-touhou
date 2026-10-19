# Relay 服务

Relay 为联机客户端提供房间目录、等待室、信令、WebSocket 回退、旁观转发和昵称服务。产品规则与昵称规范从 [`common/contracts/`](../../common/contracts/README.md) 读取。

## 本地运行

需要 Node.js 22.13 或更新版本。在本目录安装依赖、运行测试并启动：

```sh
npm ci
npm test
npm start
```

默认监听 `0.0.0.0:18142`。用环境变量设置监听地址和端口；独立打包时，目标目录需为新目录：

```sh
EAGLER_NETPLAY_RELAY_HOST=127.0.0.1 EAGLER_NETPLAY_RELAY_PORT=18142 npm start
npm run package -- /absolute/path/to/new-relay-package
```

发布包包含 Relay、昵称 Worker、锁定的生产依赖和部署样例；进入发布目录后运行 `npm ci --omit=dev` 与 `npm start`。

## 常用配置

| 变量 | 默认值 | 作用 |
| --- | --- | --- |
| `EAGLER_NETPLAY_RELAY_HOST` | `0.0.0.0` | 监听地址 |
| `EAGLER_NETPLAY_RELAY_PORT` | `18142` | 监听端口 |
| `EAGLER_NETPLAY_TRUSTED_PROXIES` | 空 | 可信反向代理 IP，逗号分隔 |
| `EAGLER_NETPLAY_STUN_URLS` | `stun:stun.cloudflare.com:3478` | 客户端 ICE STUN 地址 |
| `EAGLER_NETPLAY_TURN_URLS` | 空 | 可选 TURN 地址；需配置共享密钥或静态凭据 |
| `EAGLER_NETPLAY_TURN_SHARED_SECRET` | 空 | 与 coturn 共用的短期凭据密钥 |

完整客户端入口见[协议参考](docs/protocol.md)，反向代理与运行配置见[部署说明](deploy/README.md)。测试源码位于 [`tests/`](tests/)，服务许可见 [`LICENSE`](LICENSE)。
