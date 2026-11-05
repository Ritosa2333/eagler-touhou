# TH03 公网联机

沿用 TH04 的浏览器联机方式，各端自己运行游戏，只同步输入。
首页提供“单机游戏 / 本地双人 / 局域网联机 / 公网联机”四个入口。基本操作见 [README](README.md)，完整的服务器转发部署、反向代理和排障见 [Relay 说明书](README-RELAY.md)。本文侧重临时公网隧道、WebRTC 和 TURN。公网连接设置提供：

| 方式 | 游戏输入路径 | 需要什么 |
| --- | --- | --- |
| UDP 自动打洞 + TURN 兜底 | WebRTC ICE 优先直连，必要时选择 TURN 中继 | 公网网页与信令入口；默认 STUN；TURN 需配置 |
| 强制 TURN 中继 | WebRTC 仅使用 TURN 候选 | 同上，必须配置 TURN |
| TCP 服务器转发 | 同源 WebSocket / WSS 中继 | 公网网页服务；安装 requirements-relay.txt |

浏览器不提供原生 UDP / TCP socket。UDP 打洞由 WebRTC、STUN 和 ICE 完成；TCP 模式使用 WebSocket。TURN 服务可能提供 UDP、TCP 或 TLS 连接，页面以实际选中的候选对显示路径和协议。收集到候选不等于连接成功。

## 本机临时公网入口（和 TH04 一样）

1. 在 **th03-netplay-lab** 运行一次 `install-relay.bat`，安装 TCP Relay 依赖。
2. 如需 TURN，运行 `configure-turn.bat`，填写自己的 Cloudflare TURN Key ID 和配套 API Token。和 TH04 使用同一种配置格式，本项目使用独立配置文件。
3. 运行 `start-lan.bat`，保留服务窗口，默认端口 **9877**。
4. 安装过 cloudflared 后，运行 `start-public-tunnel.bat`。未安装时可在终端执行 `winget install --id Cloudflare.cloudflared --exact`。
5. 窗口与 `public-test-url.txt` 会列出 UDP、强制 TURN、TCP 三个完整链接。**包括房主在内，双方打开同一个公网链接**，再建房、分享邀请、加入、准备、开始。

没有配置 TURN 时，自动模式仍可使用默认 `stun:stun.cloudflare.com:3478` 打洞，并明确提示未配置 TURN；遇到无法打洞的 NAT，可以切 TCP，或配置 TURN 后重新建房。已经配置但凭据服务报错时，会显示错误，不会把它伪装为 TURN 可用。

隧道只提供 HTTPS 网页、房间信令和 WSS；它不是 UDP 打洞或 TURN 服务。UDP 直连 / TURN 的输入走各自路径；TCP 输入经过隧道，因此其延迟取决于隧道线路。关闭隧道窗口后临时链接失效。自定义本地端口时运行 `python public_tunnel.py --port 你的端口`。

## 部署到自己的公网服务器

保留 `lan_server.py`、`relay_server.py`、`turn_service.py`、`requirements-relay.txt` 和完整 `web/`。Python 3.10+；已打包的网页可直接运行，无需构建模拟器。

```bash
python3 -m venv .venv
.venv/bin/python -m pip install -r requirements-relay.txt
.venv/bin/python lan_server.py --bind 127.0.0.1 --port 9877
```

在具有有效证书的 Nginx HTTPS server 中，代理整个站点：

```nginx
location / {
    proxy_pass http://127.0.0.1:9877;
    proxy_http_version 1.1;
    proxy_set_header Host $http_host;
    proxy_buffering off;
    proxy_read_timeout 120s;
}
location = /relay {
    proxy_pass http://127.0.0.1:9877;
    proxy_http_version 1.1;
    proxy_set_header Host $http_host;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "upgrade";
    proxy_buffering off;
    proxy_read_timeout 120s;
    proxy_send_timeout 120s;
}
```

玩家使用以下入口之一：

```text
https://你的域名/lan.html?network=public
https://你的域名/lan.html?network=public&ice=relay
https://你的域名/lan.html?network=public&transport=ws
```

兼容 TH04 的旧链接参数 `network=public-udp`、`public-ws`、`direct-ws`，以及 TH03 原有 `transport=relay`。客机加入后自动采用房间连接设置；分享链接也保留这些设置。切换传输前先“结束并返回”，重新建房。

没有域名时，TCP 临时对照可直接监听 `--bind 0.0.0.0`，放行选定 TCP 端口，然后使用 `http://服务器IP:9877/lan.html?network=public&transport=ws`。完整浏览器权限及正式使用推荐 HTTPS。

必须代理 `/api/*`（含 `/api/ice`）和 `/relay` 到同一个进程，保留外部 Host（含端口）及原始 Origin。使用独立域名根路径，不要直接挂在 `/th03/` 子路径下。房间存在进程内存中，只运行一个实例；重启后需重新建房。

## TURN 配置与凭据接口

运行 `configure-turn.bat` 或服务器上的 `python configure_turn.py`。长期凭据只保存在项目根目录的 `turn-config.local.json`，不在 `web/`，已加入忽略列表。不要将配置文件发布到静态站点、提交仓库或贴到聊天。

服务端 `/api/ice` 接收 `{protocol:"th03-lan/1",room,token,role}`，校验房间身份、公网 RTC 模式和已开始状态，并限制每个座位的请求频率。调用 Cloudflare 获得 4 小时短期 TURN 凭据，按房间身份缓存；请求期间不占用房间锁。上游返回后再次检查身份，避免已离开的房间继续取得凭据。

浏览器只收到标准 `{iceServers,iceTransportPolicy}`，不会收到 Cloudflare API Token。单局接近 4 小时时请结束并重新建房获取新凭据；目前不做局内 ICE 凭据续期。

如果已有自己的 TURN / coturn 凭据服务，可把 `web/netplay/udp-config.js` 的 `credentialEndpoint` 改成同源认证接口，再重建同步清单。该接口需验证本项目房间身份，返回标准 RTC 配置：

```json
{
  "iceServers": [
    {"urls": ["stun:turn.example.com:3478"]},
    {"urls": ["turn:turn.example.com:3478?transport=udp", "turn:turn.example.com:3478?transport=tcp"], "username": "短期用户名", "credential": "短期密码"}
  ],
  "iceTransportPolicy": "all"
}
```

自己的 TURN 服务器需按其配置开放监听端口与中继端口范围；仅开放网页 TCP 端口不能代替 TURN 网络配置。长期共享密钥仍留在凭据服务端。

## 验证范围与试玩

截至 2026-10-08，已完成同步清单构建、静态检查、本机真实双端 Relay／WebRTC 同步回归，以及桌面／手机视口的房间和游戏启动测试。尚未验证实际跨 NAT 打洞、正式 TURN 账号、公网隧道或公网延迟；本机双端测试不等于跨设备公网验收。

由玩家分别测试：自动 UDP 的实际连接路径、强制 TURN、TCP；两端进入选人和对局后，检查输入同步、断线提示及退出后重新建房。页面会持续显示本机实际路径和 ICE RTT（浏览器未提供时不显示 RTT）。强制 TURN 成功应显示“TURN 中继”；TCP 应显示“TCP 服务器转发”。

所有网络模式默认关闭回滚；房主可在房间内开启，修改后双方重新准备，开始后锁定。URL 不能覆盖房间回滚策略。保留 TH03 现有回滚 / 锁步与可靠帧传输。帧包含触控增量和暂停指令，不能直接改成丢包即丢帧的零重传通道。公网延迟可能使锁步等待或回滚增多，新增传输不代表已经保证公网 60 帧。

修改运行代码后执行 `python tools/build_lockstep_runtime.py`，双方刷新并重新建房，避免混用资源版本。
