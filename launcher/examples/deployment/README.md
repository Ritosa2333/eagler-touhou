# HTTP 配置示例

先按[自托管指南](../../docs/SELF_HOSTING.md)生成站点，再选择一个示例配置自己的服务器。

| 文件 | 用途 |
| --- | --- |
| [`nginx.conf`](nginx.conf) | MIME、缓存、根路径、旧挂载路径与可选 WebSocket 代理 |
| [`nginx-external-pair.conf`](nginx-external-pair.conf) | Hosted 资源站与 External 前端站的 CORS、重定向和范围请求 |

把配置中的域名、站点目录、证书和服务地址替换为自己的值，检查配置并重载服务器。在 `launcher/` 中验证实际站点：

```sh
npm run verify:deployed -- https://example.com/
```

然后在浏览器中完成首次安装、启动、离线重开；启用多人时再完成一次真实房间连接。
