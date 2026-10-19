# 联机遥测服务

Telemetry 按字段契约接收联机线路样本，并提供聚合页面与 JSON 统计。作品身份通过 [`common/contracts/`](../../common/contracts/README.md) 校验。

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

反向代理挂载 `/netplay-stats/`。页面路径为 `/netplay-stats/`，聚合 JSON 为 `/netplay-stats/summary`，样本接收路径为 `/netplay-stats/sample`。代理该路径时关闭访问日志。

## 修改与验证

实现和测试分别位于 [`src/netplay-telemetry.mjs`](src/netplay-telemetry.mjs) 与 [`tests/telemetry.test.mjs`](tests/telemetry.test.mjs)。运行 `npm test` 检查字段校验、采样限制和聚合结果；许可证见 [`LICENSE`](LICENSE)。
