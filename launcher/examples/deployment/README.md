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

## 首访界面语言

在提供 Launcher 静态站点的 Nginx 上加载 `ngx_http_geoip2_module`，安装并定期更新 GeoLite2 Country 数据库。在 `http` 作用域包含 [GeoIP 配置](nginx-ui-country.http.conf)，在 Launcher 的 `server` 内包含[地区接口](nginx-ui-country.location.conf)。根据安装位置调整数据库路径；子目录挂载时，给接口路径加上相同的挂载前缀。

GeoIP 默认使用 Nginx 看到的客户端地址。若前面还有 CDN 或代理，先按该服务的文档配置 Real IP，只信任其代理网段；不要直接信任访客提供的转发头。地区接口不记录访问日志，不走共享缓存，也不需要登录。

`nginx -t` 通过后重载。访问 `/_eagler/ui-country` 应返回两个字母的地区代码，未知为 `ZZ`；用不同地区的出口和未保存语言的浏览器验证首访。
