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

Launcher 对当前文件依次尝试这些地址，连接失败、HTTP 错误、流中断、十五秒无字节进展时换节点。多节点下载首段十秒低于 64 KiB/s 时提前换节点，最后一个节点允许持续的低速传输。完成后的文件进入既有完整性校验和 Package 原子提交，取消操作立即停止下载。

节点 origin 使用 HTTPS，可含端口。资源节点保留文件路径与查询参数，支持 GET、HEAD、Range 及跨域读取，暴露 Content-Length、Content-Range、Accept-Ranges、ETag。Package 语言资源支持 `language/` 和 `language-updates/` 路径。Runtime 字体仍由 Runtime 发布源直接提供。

## 发布库存与验证

`inventoryFile` 是以资源路径为键、以 `{bytes, sha256}` 为值的 JSON。先将资源传入节点并核对文件摘要，再发布库存。未配置发布库存时，服务从节点 Package 清单和 Range/CORS 探测生成库存；并发上限四，三个站点共享同轮探测。

测试覆盖跨站探测合并、并发上限、内容不匹配排除、有序备用节点、版本参数和绝对语言路径。发布时通过公网验证路由发现、Range、跨域读取与完整安装，并用失败节点验证自动切换。更换分流服务不重启 Relay 或 TURN。

下载测速对照同一文件、相同 Range 和冷热缓存，分别记录客户端直连、客户端代理与服务器出口，并同时测试 HTTP/1.1 和 HTTP/2。记录最终 URL、协议、首包、完整下载时间与内容校验，将代理路径差异与协议问题分别定位。
