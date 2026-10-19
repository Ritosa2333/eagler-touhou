# Eagler Touhou 部署包

此包由一个已验证的 Release 生成。自动化部署先读 `AGENTS.md`，部署者按下面顺序操作。

## 1. 校验

在交接包根目录运行：

```sh
sha256sum --check operator-checksums.txt
```

在 `verification-report.json` 查看构建身份与已执行的检查，在站点描述符中查看本包的作品、语言和音乐。

## 2. 部署站点

把 `site/` 的副本用作静态 Web 根目录。按 `services/README.md` 配置 HTTPS，需要多人时配置自己的 Relay。

迁移 HTTP 浏览器数据期间使用 `services/nginx/nginx.migration-window.conf.example`，保留 HTTP `/migrate.html` 可访问并关闭 HSTS。迁移结束后，用 `services/configure-deployed-site.mjs` 移除迁移配置，切换最终 HTTPS 配置并开启一年 HSTS。

## 3. 验证

完成配置检查与对应切换验证，再在浏览器中安装、启动游戏并测试离线重开。多人站点完成 Relay 两项探测和真实房间连接。保留上一个可用站点副本。
