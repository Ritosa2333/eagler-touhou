# 独立联机服务

Relay 为 Launcher 和其他兼容客户端提供房间、信令、WebSocket 回退、旁观历史及昵称服务。
协议与产品规则统一引用根 `common/contracts/`，服务源码、测试和构建不依赖 Launcher。

要求 Node.js 22.13 或更新版本。在本目录运行：

```sh
npm ci
npm test
npm start
npm run package -- /absolute/path/to/new-relay-package
```

发布包内含独立的 Relay、昵称 Worker、锁定的生产依赖清单和配置示例。
在发布包中执行 `npm ci --omit=dev` 后可直接启动，运行不需要源码仓库或前端构建。
`artifacts/` 是忽略的本地产物目录。

客户端接入与部署边界见 [协议](docs/protocol.md) 和 [部署](deploy/README.md)。
