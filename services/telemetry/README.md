# 服务状态与联机遥测

服务按配置探测主站、下载、联机和 TURN，提供 `/uptime/` 使用的历史与当前状态。联机线路样本和聚合 JSON 继续由同一服务接收，作品身份通过 [`common/contracts/`](../../common/contracts/README.md) 校验。

## 运行

需要 Node.js 22.13 或更新版本；服务无第三方运行依赖。在本目录执行：

```sh
npm test
npm start
```

默认监听 `127.0.0.1:18153`。通过下表配置服务：

| 变量 | 默认值 | 用途 |
| --- | --- | --- |
| `EAGLER_NETPLAY_TELEMETRY_PORT` | `18153` | HTTP 端口 |
| `EAGLER_NETPLAY_TELEMETRY_ORIGIN` | 必填 | 接收样本的网页来源，例如 `https://example.com` |
| `EAGLER_NETPLAY_TELEMETRY_STATE` | 空 | 仓库与网站目录之外的持久化 JSON 文件路径 |
| `EAGLER_NETPLAY_STATUS_CONFIG` | 空 | 探测目标与下载组显示配置的 JSON 文件 |

状态页面随 Launcher 发布到 `/uptime/`，反向代理将 `/uptime/summary` 和 `/netplay-stats/` 转发到本服务，关闭这些接口的访问日志。旧页面地址重定向到 `/uptime/`；`/netplay-stats/summary` 与 `/netplay-stats/sample` 保持原接口。代理示例见 [`nginx.conf`](../../launcher/examples/deployment/nginx.conf)。

## 探测配置

```json
{
  "showDownloads": true,
  "targets": [
    {"id":"website", "label":"站点页面", "kind":"site", "url":"https://example.com/"},
    {"id":"download", "label":"下载节点", "kind":"resource", "url":"https://downloads.example.com/probe.bin"},
    {"id":"relay", "label":"房间与信令", "kind":"websocket", "url":"wss://example.com/eagler-netplay/"},
    {"id":"turn", "label":"TURN 节点", "kind":"turn", "url":"stun:turn.example.com:3478"}
  ]
}
```

`showDownloads: false` 隐藏下载服务整组，其余分组直接接续。目标还可用 `group` 指定 `site`、`download`、`netplay` 或 `turn`。探测地址由运营配置管理；公开接口只提供名称、状态与聚合指标。

目标和 `loadNodes` 可配置 `publicId`、`publicLabel`，例如 `turn-n04`、`TURN N04`。公开页面与 JSON 使用这些编号，历史仍按内部 `id` 归档。内部名称、探测地址和编号对照由运营者保存到网站目录之外。

公开节点负载只提供编号、名称、状态和采样时间。运营配置中的 `adminToken` 至少包含 32 个字符；管理员向 `/netplay-stats/summary` 发送对应的 `Authorization: Bearer` 请求头，可读取内部名称、硬件与负载详情。凭据通过私有配置管理，响应禁止缓存。

服务每 30 秒进行 HTTP、资源 Range 请求、WebSocket 握手或 STUN Binding 检查。TURN 的 STUN 检查反映 UDP 端口可达性。`/uptime/summary?range=24h` 支持 `24h`、`7d`、`90d`，可用率按已完成探测计算，未采样区间以无数据呈现。探测超过 90 秒未更新时，当前状态转为未知。

历史使用一天的五分钟明细与九十天的小时聚合，随原有遥测状态文件原子保存。服务重启后继续累积历史。线上状态服务的诊断聚合、事件和节点上报也保留在 `/netplay-stats/summary` 与 `/netplay-stats/nodes/<id>`；`loadNodes`、`infrastructureUrl` 配置沿用原契约。

## 修改与验证

实现和测试分别位于 [`src/netplay-telemetry.mjs`](src/netplay-telemetry.mjs) 与 [`tests/telemetry.test.mjs`](tests/telemetry.test.mjs)。运行 `npm test` 检查字段校验、采样限制和聚合结果；许可证见 [`LICENSE`](LICENSE)。
