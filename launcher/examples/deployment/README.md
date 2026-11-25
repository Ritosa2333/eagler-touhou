# HTTP 配置示例

先按[自托管指南](../../docs/SELF_HOSTING.md)生成站点，再选择一个示例配置自己的服务器。

| 文件 | 用途 |
| --- | --- |
| [`nginx.conf`](nginx.conf) | 同域站点的 MIME、缓存、跨源隔离与可选 WebSocket 代理 |
| [`nginx-external-pair.conf`](nginx-external-pair.conf) | Hosted 资源站与 External 前端站的 CORS、重定向和范围请求 |

把配置中的域名、站点目录、证书和服务地址替换为自己的值。同域示例监听 HTTP，公网入口需要配置 HTTPS，也可以让前置代理终止 TLS。执行 `nginx -t` 通过后重载服务器。在 `launcher/` 中验证实际站点：

```sh
npm run verify:deployed -- https://example.com/
```

在浏览器中确认 Launcher 和 Runtime 页面的 `crossOriginIsolated` 为 `true`，再完成首次安装、启动和离线重开。启用同域 Relay 时，按 [Relay 部署指南](../../../services/relay/deploy/README.md)配置可信代理，并从两个不同网络验收。
