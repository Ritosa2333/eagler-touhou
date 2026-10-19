# 部署代理约定

这是生成的部署包。操作前阅读 `README.md` 和 `services/README.md`。

1. 向部署者确认 SSH 目标、Web 根目录、域名、证书归属、迁移阶段、Relay/TURN 地址和防火墙配置；缺失的值须询问。
2. 校验 `operator-checksums.txt`，保留交付的 `site/` 原样，配置部署副本。
3. 复制或替换前确认准确的 Web 根目录；Relay/TURN 密钥放在 `/etc/eagler-touhou/`，权限 `0600`。
4. 重载前运行 `nginx -t`，保留上一可用版本。
5. 数据迁移期间保持 HTTP `/migrate.html` 可达、HSTS 关闭；最终 HTTPS 阶段先移除 `shared.originMigration` 再开启 HSTS。
6. 分别记录文件、进程、HTTP、浏览器和游戏检查的实际结果。

校验和、`nginx -t` 和对应切换验证通过后记录部署结构完成；多人另需两个 Relay 探测通过，并在部署的 Site Manifest 配置已验证 WSS 地址。
