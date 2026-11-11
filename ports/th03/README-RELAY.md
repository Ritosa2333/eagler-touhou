# TH03 Relay 使用与部署说明

面向玩家、服务器部署人员及接手部署的 AI 助手，对应 2026-10-08 的 TH03 实现。基本操作见 [README](README.md)，TURN 和临时公网隧道的详细配置见 [公网联机说明](README-PUBLIC.md)。

## Relay 负责什么

推荐将网页、房间服务和 WebSocket Relay 一起部署到同一个服务器：

```text
玩家 A 浏览器 ── HTTP(S) / WebSocket ── TH03 服务器
玩家 B 浏览器 ── HTTP(S) / WebSocket ── TH03 服务器
```

各玩家在浏览器内运行游戏和 DOSBox-X 模拟器；服务器提供资源、管理房间并转发输入，不运行模拟器，不传输游戏画面或音频流。服务器需保存完整的网页和游戏资源，玩家首次访问还会下载 WASM、磁盘及音乐。

| 方式 | 游戏输入路径 | 服务要求 |
| --- | --- | --- |
| WebRTC 自动打洞 | ICE 选择设备直连或已配置的 TURN | 网页／房间信令，公网 STUN；TURN 兜底需配置 |
| 强制 TURN | WebRTC 仅使用 TURN 候选 | 网页／信令和可用 TURN 凭据 |
| TCP Relay | 同源 WebSocket／WSS 转发 | 本文的 Python 服务与 websockets 依赖 |

本文重点是第三种方式。它不需要 TURN 账号、Cloudflare 账号或 cloudflared。Cloudflare Quick Tunnel 是可选的公网入口，不能代替 TURN。

**实际启动入口为 `lan_server.py`。不要单独运行 `relay_server.py`**：Relay 依赖同一进程内的房间身份、准备和开局状态。

## 环境与文件

推荐 Linux、Python 3.10+；Windows 同样可运行。依赖以 `requirements-relay.txt` 为准，目前为 `websockets>=14,<17`。不需要 GPU、数据库、Node.js 或服务器端模拟器。

双人临时测试可从 1 vCPU／1 GiB 资源预算开始，这是建议起点，并非经过压测确认的最低配置。选择两位玩家都能稳定访问的节点；只有 IPv6 的节点要求双方网络均有可用 IPv6。

保持以下目录关系：

```text
th03-netplay-lab/
  lan_server.py
  relay_server.py
  turn_service.py
  requirements-relay.txt
  web/                     # 同一版本，完整保留所有子目录
    index.html
    local.html
    solo.html
    lan.html
    disks/
    native/
    netplay/
    vendor/
    bgm/
    ...
```

`turn_service.py` 被入口直接导入，即使不使用 TURN 也不能遗漏。不要把本机 `turn-config.local.json` 加入公开分发包。

## Windows 局域网与临时公网

局域网：

1. 在 TH03 目录运行一次 `install-relay.bat`。
2. 启动 `start-lan.bat`，保持窗口打开，默认监听 TCP 9877。
3. 双方打开启动窗口提供的局域网地址，在“连接设置与帮助”选择“服务器转发（WebSocket）”，再建房、加入、准备、开始。

临时公网：

1. 保持上述房间服务运行，安装可从 PATH 调用的 cloudflared。
2. 运行 `start-public-tunnel.bat`。
3. **包括房主在内，双方打开窗口输出的 TCP 完整链接**，也可从 `public-test-url.txt` 读取。
4. 保持服务和隧道两个窗口打开。隧道重启后地址会变化，重新分享新链接。

自定义本地端口时，手动使用 `python lan_server.py --port 你的端口` 和 `python public_tunnel.py --port 你的端口`。TCP Relay 经隧道转发，其延迟受隧道路径影响。

## Linux 最快启动：HTTP / WebSocket

在项目目录执行：

```bash
cd /实际路径/th03-netplay-lab
python3 -m venv .venv
.venv/bin/python -m pip install -r requirements-relay.txt
.venv/bin/python lan_server.py --bind 0.0.0.0 --port 9877
```

若缺少 venv，先按发行版安装 `python3-venv`。在云安全组和系统防火墙放行选定的 **TCP** 端口。前台运行需保持会话存活；长期部署可用现有 systemd 等服务管理方式托管。

双方使用这个完整入口：

```text
http://服务器IP:9877/lan.html?network=public&transport=ws
```

不带 `transport=ws` 的公网入口默认走 WebRTC，不能用于确认 TCP Relay 已连通。网页自动从当前来源生成 `ws://服务器IP:9877/relay`，无需硬编码 IP，也无需另开 9876 端口。

房主创建房间并分享邀请链接；双方准备后由房主开始。房主可在开局前选择左侧 P1 或右侧 P2，客机自动分配到另一侧；更换席位需双方重新准备，角色在游戏内选择。两人必须使用同一个服务，不能一人访问 localhost、另一人访问独立部署的服务器。

HTTP 可用于临时键盘联机对照；手柄等浏览器功能可能要求安全上下文，正式使用推荐 HTTPS。

## 域名与 Nginx：HTTPS / WSS

使用已有有效证书的独立域名，将 Python 服务限制在回环地址：

```bash
.venv/bin/python lan_server.py --bind 127.0.0.1 --port 9877
```

在 Nginx 的 HTTPS `server` 块配置：

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

检查并重载 Nginx 后，双方使用：

```text
https://你的域名/lan.html?network=public&transport=ws
```

浏览器自动连接 `wss://你的域名/relay`。公网只需开放 HTTPS 入口，Python 的 9877 无需对公网开放。

部署时保持以下条件：

- 网页、`/api/*`（包含 `/api/ice`）和 `/relay` 均代理到同一个 Python 服务，不能只代理 Relay。
- 保留原始 Origin，并用 `$http_host` 保留外部 Host 及非默认端口。后端会比较来源与 Host，不要改成内部地址或关闭校验来绕过 403。
- 使用域名根路径。当前请求使用绝对路径，不支持未经适配就挂到 `/th03/` 子目录。
- 若目的是比较直达服务器与旧隧道的性能，域名应直接指向该节点，避免再次套上改变路径的隧道。

## 仅提供反向隧道的部署

也可以让 TH03 仍运行在玩家电脑，公网节点只转发：

```text
玩家浏览器 → 公网节点 → 反向隧道 → 电脑上的 TH03 房间服务
```

FRP、反向 SSH 或等价设施需转发整个 TCP 服务，或者代理全部网页／API／Relay 路径，并支持 WebSocket Upgrade 和持续双向连接。HTTP 代理仍需保留外部 Host／Origin。双方统一使用公网入口的 `lan.html?network=public&transport=ws`。

这种部署无需在节点复制资源或安装 Python，但仍经过电脑所在网络；完整部署到服务器则不依赖该电脑持续在线。

## WebRTC / TURN 与 Relay 的边界

公网 WebRTC 入口为：

```text
https://你的域名/lan.html?network=public
https://你的域名/lan.html?network=public&ice=relay
```

第二个入口强制 TURN，必须配置可用 TURN。自动模式未配置 TURN 时仍可尝试默认 STUN 打洞，失败后应结束并切换 TCP 入口重新建房，不会在同一局内静默换传输。

TH03 已有同源 `/api/ice` 和 `configure-turn.bat`／`configure_turn.py`；Cloudflare TURN 配置、短期凭据接口和自建 coturn 接入方法见 [公网联机说明](README-PUBLIC.md#turn-配置与凭据接口)。浏览器使用 WebRTC，不能直接打开原生 UDP socket；TURN 也可能采用 TCP／TLS 路径，以实际连接诊断为准。

## 版本、进程与协议约束

1. 完整 `web/` 已包含运行资源，原样部署无需构建工具或源磁盘。更新要成套替换，不能混用磁盘、原生补丁、JS／WASM 和指纹清单。
2. 房间和中继状态保存在内存中，只运行一个服务实例。不要使用多进程或随机负载均衡；重启服务后双方重新建房。
3. TH03 的房间身份协议为 `th03-lan/1`，同步协议为 `th03-rollback/2`，Relay 转发经校验的 JSON 消息。不能直接替换成 TH04 的 E7／E8 二进制转发模块或通用 TURN 服务。
4. 输入包含触控位移增量和暂停指令，依赖可靠有序传输。部署时保留同步及传输实现，不要照搬其他游戏的无序、零重传建议。
5. 回滚默认关闭，由房主在房间内控制；开启／关闭后双方重新准备，开始后锁定。不要用 URL 的 `rollback=on/off` 代替房间设置。
6. 本说明不代表公网生产压测或跨 NAT 验收已经完成，节点更换也不保证固定帧率。

## 联通检查与性能对照

先检查双方可以加载网页、进入同一房间，再准备并开始。确认页面连接信息显示“TCP 服务器转发”，且双方进入实际选人和对局；仅网页能打开不代表 WebSocket 成功。

本机已通过真实双端 Relay／WebRTC 同步回归；具体服务器、两位玩家网络和正式 TURN 账号仍需实测。比较隧道与直达服务器时，保持设备、玩家网络、游戏语言、CPU、场景和回滚设置一致，记录连接路径、延迟诊断、等待提示、正常战斗中的卡顿和输入感受。

回滚关闭时网络等待仍会降低推进速度；开启时快照和重算又会增加设备负担。应用层延迟包含浏览器处理时间，不等同于纯网络 ping，现有页面诊断也不能直接测出网络丢包率。

测试第二种连接或回滚策略时，先结束旧局、调整设置并重新建房。断线暂不支持续局。

## 常见问题

| 现象 | 检查方向 |
| --- | --- |
| 缺少 websockets 或提示安装 Relay 依赖 | 用启动服务的同一个 Python／venv 安装 requirements-relay.txt，再重启 |
| 缺少 turn_service | 上传时遗漏模块，即使不用 TURN 也需保留 |
| 网页打不开 | 进程、监听地址、端口、安全组、防火墙和 DNS |
| 网页正常，无法建房 | 是否误用了只供单机的 server.py；/api/* 是否指向同一个 lan_server.py |
| /relay 返回 403 | Host／Origin、房间身份、服务器转发模式和已开始状态 |
| /relay 返回 404 | 房间已结束或路由错误；从大厅重新建房，不直接打开 Relay URL |
| WebSocket 400／502／超时 | HTTP/1.1、Upgrade 头、上游进程和长连接超时 |
| 等待 ICE 或要求 TURN 配置 | 打开了 WebRTC 模式，使用带 transport=ws 的完整入口 |
| 版本不一致或同步错误 | 更新同一版本完整资源，双方刷新并重建房间 |
| 手机无法访问 localhost | 使用服务器或电脑的局域网地址，localhost 指向手机自己 |
| 手柄不可用 | HTTPS、浏览器支持和权限；先按手柄按钮激活 |
| 改用新节点仍卡顿 | 同时检查设备性能、后台限速、双方网络和回滚开销 |

交付部署结果时提供：**完整玩家入口 URL、节点地区、是否经过额外隧道／代理、部署版本及实际联通结果**。不要附带服务器密码、TURN 长期密钥或房间 token。
