# TURN 配置工具

本目录用环境变量生成 coturn 配置。coturn 程序由服务部署方安装；Relay 与 coturn 共用短期凭据密钥。

## 生成配置

在私有环境文件中设置 `EAGLER_NETPLAY_TURN_REALM` 与至少 24 字节的 `EAGLER_NETPLAY_TURN_SHARED_SECRET`，然后从本目录执行：

```sh
set -a
. /path/to/private/turn.env
set +a
node render-coturn-config.cjs --output /path/outside/repository/turnserver.conf
```

工具将配置文件权限设为 `0600`。Relay 环境中的 `EAGLER_NETPLAY_TURN_SHARED_SECRET` 使用同一密钥；同时设置 `EAGLER_NETPLAY_TURN_URLS`，并可用 `EAGLER_NETPLAY_TURN_TTL_SECONDS` 调整凭据有效期。

## 端口与可选配置

| 项目 | 默认值 | 用途 |
| --- | --- | --- |
| `EAGLER_NETPLAY_TURN_LISTENING_PORT` | `3478` | TURN UDP/TCP 监听端口 |
| `EAGLER_NETPLAY_TURN_MIN_PORT`–`EAGLER_NETPLAY_TURN_MAX_PORT` | `49152`–`65535` | TURN Relay UDP 端口范围 |
| `EAGLER_NETPLAY_TURN_TLS_LISTENING_PORT` | `5349` | 配置证书后启用 TURN/TLS |
| `EAGLER_NETPLAY_TURN_CERT`、`EAGLER_NETPLAY_TURN_PKEY` | 空 | 必须同时提供以启用 TLS |

将 TURN 监听端口和 Relay UDP 范围加入主机及云防火墙规则。其他配额、带宽及地址选项见 [`turn.env.example`](turn.env.example)。

## 检查

```sh
node --test tests/test-netplay-service-config.mjs
```

测试覆盖渲染字段、密钥共用和无效配置。配置工具适用本目录 [`LICENSE`](LICENSE) 的 GPL-3.0-or-later 许可。
