# 部署服务

以下步骤从交接包根目录执行。先确认域名、Web 根目录和服务安装路径。

## 1. Relay

准备 Node.js 22.13 或更新版本，将包内 `services/relay/` 放到服务目录：

```sh
npm --prefix /opt/eagler-touhou/services/relay ci --omit=dev
sudo cp services/relay/relay.env.example /etc/eagler-touhou/relay.env
sudo chmod 600 /etc/eagler-touhou/relay.env
```

填写环境文件，检查 systemd 模板中的服务路径与用户，再安装服务：

```sh
sudo cp services/relay/eagler-netplay-relay.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable --now eagler-netplay-relay
```

把 loopback `18142` 代理为 WSS。用自己的地址运行两个检查：

```sh
node services/verify/verify-public-relay-fallback.mjs wss://YOUR_DOMAIN/eagler-netplay/
node services/verify/verify-public-targeted-relay.mjs wss://YOUR_DOMAIN/eagler-netplay/
node services/configure-deployed-site.mjs --site=/var/www/eagler-touhou --relay=wss://YOUR_DOMAIN/eagler-netplay/
```

启用公开昵称时，按环境模板配置 SQLite 和 API。`nickname-worker.mjs` 与 `netplay-relay.mjs` 同目录；数据库由服务用户独占写入。移动运行中的状态需一并处理 WAL/SHM，普通文件备份前停止服务。

## 2. TURN

需要中继穿透时安装 coturn，填写 `services/turn/turn.env.example`，用包内 renderer 生成配置。coturn 与 Relay 使用相同共享密钥；防火墙与云安全组开放所选监听端口和中继端口范围。

## 3. 结束 HTTP 数据迁移

浏览器数据迁移完成后运行：

```sh
node services/configure-deployed-site.mjs --site=/var/www/eagler-touhou --origin-migration=disable
```

切换最终 HTTPS/HSTS 示例，检查配置，再运行 `services/verify/` 中对应验证器。
