# 资源下载服务

资源分流服务按站点当前 Package 清单核对节点库存，同一路径、字节数和 SHA-256 一致的节点才参与下载。Launcher 在节点间自动接续文件下载，成功节点优先服务本次会话的后续资源。服务使用 Node.js 22.18+，无第三方运行依赖。

## 运行

```sh
node router.mjs /absolute/path/to/resources.json
npm test
```

配置示例：

```json
{
  "port": 18290,
  "sites": {
    "play.example.org": {
      "root": "/srv/launcher/current",
      "nodes": [
        {"id": "first", "origin": "https://assets.example.org", "weight": 1,
         "inventoryFile": "/srv/inventory/first.json"},
        {"id": "second", "origin": "https://mirror.example.org", "weight": 1,
         "inventoryFile": "/srv/inventory/second.json"}
      ]
    }
  }
}
```

服务绑定 IPv4 loopback。反向代理将资源请求转发到该端口，覆盖 `X-Resource-Site` 为当前站点域名，覆盖 `X-Resource-Client` 为真实客户端地址。反向代理同时将 `/_eagler/resource-routes` 转发到同一服务；CDN 保留该接口的回源请求。每个域名使用自己的发布目录；库存每十五秒刷新。`/_status` 提供各站节点匹配数量和错误。

## 下载与备用节点

普通 GET、HEAD 返回 307，保留原始路径和版本参数。客户端请求 `/_eagler/resource-routes?path=<已编码的资源路径与查询参数>` 获取 `eagler-touhou/resource-routes/1` JSON，包含 `path`、`bytes`、`sha256` 和有序 `sources`。第一个地址与普通分流结果一致，其余内容匹配的节点作为备用。标记 `emergency: true` 的节点排在常用节点之后。

Launcher 首次下载时并行比较两个节点，各请求最多 64 KiB 的 Range 首段，优先交付首段的节点继续下载，取消另一条请求。已有成功记录时优先复用该节点，400 毫秒尚未交付数据才请求备用。节点记录由同一页面会话的数据、音乐和语言包共用，失败节点在六十秒内排后，恢复后可重新参与选点。

路由发现限时 2.5 秒，连接与传输连续四秒没有字节进展时换节点。下载每三秒检查当前用户的实际速度，低于 100 KiB/s 时，用最多 64 KiB、约一秒的样本比较备用节点；备用速度高出当前至少 25% 才切换。比较期间当前下载继续，备用更慢时继续原下载。每个文件对同一备用节点比较一次，避免低速线路反复切换。初始首段比较也使用约一秒采样窗口，读取停滞由四秒超时结束。

切换使用 Range 接续已收到的前缀，核对 Content-Range 的起点、终点和总字节数，并复用选中样本。节点忽略 Range 返回完整 200 时重新组装该响应。完成后的文件进入既有完整性校验和 Package 原子提交，取消操作停止所有候选请求。以上策略由共用下载器应用于各作品，Package 描述文件继续使用本站相对路径。

有限并发和延后备用参考 [gRPC request hedging](https://grpc.io/docs/guides/request-hedging/)，续传遵循 [HTTP Range](https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Headers/Range) 的 206 与完整 200 响应语义。

节点 origin 使用 HTTPS，可含端口。资源节点保留文件路径与查询参数，支持 GET、HEAD、Range 及跨域读取，暴露 Content-Length、Content-Range、Accept-Ranges、ETag。Package 语言资源支持 `language/` 和 `language-updates/` 路径。Runtime 字体仍由 Runtime 发布源直接提供。

## 发布库存与验证

`inventoryFile` 是以资源路径为键、以 `{bytes, sha256}` 为值的 JSON。先将资源传入节点并核对文件摘要，再发布库存。未配置发布库存时，服务从节点 Package 清单和 Range/CORS 探测生成库存；并发上限四，三个站点共享同轮探测。

测试覆盖跨站探测合并、并发上限、内容不匹配排除、有序备用节点、版本参数和绝对语言路径。发布时通过公网验证路由发现、Range、跨域读取与完整安装，并用失败节点验证自动切换。更换分流服务不重启 Relay 或 TURN。

Launcher 的 `npm run test:resource-download:browser` 使用本地 HTTPS 节点验证浏览器跨域下载、节点无响应、中途停滞、持续低速、较慢备用和取消，并校验续传文件的完整 SHA-256。通过 `EAGLER_TEST_TLS_CERT`、`EAGLER_TEST_TLS_KEY` 指定测试证书与密钥，浏览器路径可用 `EAGLER_CHROME_PATH` 指定。

下载测速对照同一文件、相同 Range 和冷热缓存，分别记录客户端直连、客户端代理与服务器出口，并同时测试 HTTP/1.1 和 HTTP/2。记录最终 URL、协议、首包、完整下载时间与内容校验，将代理路径差异与协议问题分别定位。
