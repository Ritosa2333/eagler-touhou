# 独立联机统计服务

收集有界、同源的匿名线路样本并返回聚合结果。服务通过 `common/contracts/` 验证产品身份，源码和启动不依赖 Launcher。

```sh
npm test
npm start
```

用 `EAGLER_NETPLAY_TELEMETRY_PORT`、`EAGLER_NETPLAY_TELEMETRY_ORIGIN` 和 `EAGLER_NETPLAY_TELEMETRY_STATE` 配置端口、允许的来源及仓库外状态文件。
代理 `/netplay-stats/` 时关闭访问日志。采样客户端可以来自其他壳，但仍须遵循相同字段、隐私约束和采样频率。
