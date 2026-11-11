# TH04 Relay 临时部署说明

本文面向接收仓库的部署人员及 AI 助手。当前 `th04-coop-lab` 已采用 DOSBox-X，部署入口沿用房间、WebRTC 和 WebSocket 服务。当前启动、性能及验证范围见 [README](README.md)；下文公网网络路径的讨论来自早期部署记录，不代表已完成异地公网验收。

## 目的与部署方式

局域网联机流畅，但通过 Cloudflare Quick Tunnel 公网联机明显卡顿。希望借国内节点做同设备、同场景对照，排查网络路径的影响；目前尚未证明 Cloudflare 是唯一原因，也不保证换节点后达到 60 帧。

推荐把现有房间服务、WebSocket Relay 和网页文件一起放到服务器：

```text
玩家 A 浏览器 ── HTTP(S) / WebSocket ── 国内服务器
玩家 B 浏览器 ── HTTP(S) / WebSocket ── 国内服务器
```

游戏和 DOSBox-X 模拟器在各玩家浏览器内运行。服务器负责提供静态资源、管理房间、转发操作消息，不运行游戏模拟器，不传输游戏画面或音频流。

## UDP 打洞接口（负责人后续填写）

公网直连入口使用浏览器 WebRTC ICE。浏览器不能直接打开原生 UDP 端口；所谓 UDP 打洞由 ICE、STUN 和必要时的 TURN 完成。当前客户端已经预留 `th04NetplayUdp` 接口，负责人只需填写部署配置，不需要修改 DOSBox-X 或回滚代码。

启用入口：

```text
https://你的域名/lan.html?network=public-udp&rollback=off
```

配置文件是 `web/netplay/udp-config.js`。首测可以直接填写 STUN：

```js
export const udpConfig={
  credentialEndpoint:'',
  iceServers:[{urls:'stun:stun.example.cn:3478'}],
  iceTransportPolicy:'all'
};
```

生产环境推荐只填写同源短期凭据接口：

```js
export const udpConfig={
  credentialEndpoint:'/integration/ice',
  iceServers:[],
  iceTransportPolicy:'all'
};
```

接口接收 JSON：

```json
{"room":"0037","token":"短期房间令牌","role":"guest"}
```

接口返回浏览器标准 RTC 配置：

```json
{
  "iceServers":[
    {"urls":["stun:stun.example.cn:3478"]},
    {"urls":["turn:turn.example.cn:3478?transport=udp"],"username":"临时用户名","credential":"临时密码"}
  ],
  "iceTransportPolicy":"all"
}
```

`iceTransportPolicy` 只能是 `all` 或 `relay`。`all` 先尝试 UDP 直连，失败时允许 TURN；`relay` 强制使用 TURN，适合验证兜底路径。长期 TURN 密钥只能留在服务端，浏览器只能收到短期凭据。凭据接口必须与网页同源，或由同源反向代理转发。

当前信令仍复用 `/api/signal` 和 `/api/events`。它们只交换 offer、answer、ICE candidate，不承载游戏输入。三人局需要为每一对玩家建立 ICE 连接。负责人接入正式大厅时，保持 `host=0`、`guest=1`、`guest2=2` 的身份编号，并继续单独传 TH04 座位号。

连接成功后，页面会从 `getStats()` 显示 `host/srflx/prflx` 直连或 `relay` TURN 路径。只收到 candidate 不能算打洞成功，必须等候选对状态为 succeeded。直连失败时不得让游戏假装进入 UDP 模式，应显示 ICE/TURN 错误并允许回退到 `direct-ws` 或 `public-ws`。

当前输入通道仍由 TH04 的 WebRTC 适配层管理。后续负责人提供生产传输层时，输入建议使用无序、零重传 DataChannel，控制和离线协商继续使用可靠有序通道；不能让控制消息和每帧输入共用一个可靠队列。

**实际启动入口是 `lan_server.py`，不要单独运行 `relay_server.py`。** 后者是前者调用的模块，依赖同一进程里的房间身份和状态。

如果只能提供转发入口，也可以用反向隧道把服务器公网端口转发到开发者电脑的整个 HTTP 服务，见下文。两种方式都不要求玩家自己有公网 IP。

## 环境和文件

- 推荐 Linux，Python 3.10 或更新版本（例如 Ubuntu 22.04 / 24.04）。
- Python 依赖：`requirements-relay.txt`，当前固定 `websockets==16.0`。
- 两三人临时测试可先从共享的 1 vCPU / 1 GiB 资源预算开始；这是建议起点，不是经压测确认的最低配置。不需要 GPU、数据库、Node.js 或模拟器安装。
- 一个玩家可访问的公网 TCP 入口；端口可以自选，9868 只是示例。若只有 IPv6，需要所有玩家都有可用 IPv6。
- 中继操作消息较小，但首次访问还要下载 WASM、磁盘及按需音乐，不能把整个站点当成只有几 KB 的流量。

保持以下目录关系：

```text
th04-pc98-fast/
  lan_server.py
  relay_server.py
  turn_service.py
  requirements-relay.txt
  web/                       # 完整保留，包含子目录和生成文件
    lan.html
    netplay/
    vendor/
    bgm/
    th04-coop.hdi.gz
    ...
```

`turn_service.py` 虽然在本方案中不提供 TURN 服务，但被入口直接导入，不能漏掉。**本方案不需要 TURN 账号、Cloudflare 账号、cloudflared 或 TURN 密钥。** 不要上传 `turn-config.local.json`。

服务器会存放并提供 `web/` 游戏资源，但不会执行其中的游戏。之前“服务器无需存放游戏资源”的说法只适用于另行拆分静态托管的部署，不适用于本说明的完整部署。

## 最快启动：直接 HTTP / WebSocket

在收到的仓库根目录执行：

```bash
cd /实际路径/th04-pc98-fast
python3 -m venv .venv
.venv/bin/python -m pip install -r requirements-relay.txt
.venv/bin/python lan_server.py --bind 0.0.0.0 --port 9868
```

若系统没有 venv 模块，先按发行版安装 `python3-venv`。服务前台运行，保持会话存活；长期驻留可以由部署者使用现有 systemd 等方式托管。

在云安全组和系统防火墙中放行所选 TCP 端口。浏览器打开：

```text
http://服务器IP:9868/lan.html?network=direct-ws&rollback=off
```

两名玩家都访问这个服务器，建房、加入、选择不同座位、准备并开始。不能一方打开开发者电脑的 localhost，另一方打开服务器；两者是不同房间服务。

控制台打印的普通 `lan.html` 是历史 LAN 入口，**请使用上面带 `network=direct-ws` 的完整地址**。不带参数会选择 WebRTC 路径，不是本次 Relay 对照。

`network=public-ws` 也使用同源 WebSocket，它本身不会强制经过 Cloudflare；`direct-ws` 用于明确标识这次直接服务器部署。WebSocket 地址由网页来源自动生成，不需要在 JS 中硬编码服务器 IP。

HTTP 可以用于临时键盘联机对照；部分浏览器的手柄功能要求 HTTPS。需要手柄或完整浏览器权限时使用下面的 HTTPS 入口。

## 已有域名和 Nginx：HTTPS / WSS

Python 只监听回环地址：

```bash
.venv/bin/python lan_server.py --bind 127.0.0.1 --port 9868
```

在已有且证书正常的 HTTPS `server` 块中配置以下路由。示例假设独立域名根路径，没有其他应用占用这些路径：

```nginx
location / {
    proxy_pass http://127.0.0.1:9868;
    proxy_http_version 1.1;
    proxy_set_header Host $http_host;
    proxy_buffering off;
    proxy_read_timeout 120s;
}

location = /relay {
    proxy_pass http://127.0.0.1:9868;
    proxy_http_version 1.1;
    proxy_set_header Host $http_host;
    proxy_set_header Upgrade $http_upgrade;
    proxy_set_header Connection "upgrade";
    proxy_buffering off;
    proxy_read_timeout 120s;
    proxy_send_timeout 120s;
}
```

玩家入口：

```text
https://你的域名/lan.html?network=direct-ws&rollback=off
```

浏览器会自动使用 `wss://你的域名/relay`。只需要公网开放 HTTPS 入口，Python 的 9868 不必对公网开放。

注意：

- 保留浏览器原始 `Origin`，并使用 `$http_host` 保留外部 Host 和非默认端口。后端检查 Origin 与 Host 是否匹配，不要把 Host 改成 `127.0.0.1:9868`，也不要删除来源校验来绕过 403。
- 网页、`/api/*` 和 `/relay` 都必须可达同一服务；不能只代理 `/relay`。
- 当前前端使用绝对路径 `/api/*` 和 `/relay`，不要直接挂到 `/th04/` 子路径，除非同时适配路径。
- 本次对照应让域名直接到该节点，避免再次套 Cloudflare Tunnel 或其他改变路径的代理。

## 如果只提供内网穿透 / 反向隧道

可以让程序仍在开发者电脑运行，国内节点只转发连接：

```text
玩家浏览器 → 国内公网入口 → 反向隧道 → 开发者电脑 HTTP 服务
```

要求是开发者电脑主动建立隧道，转发整个 TCP 服务或全部 HTTP 路径，支持 WebSocket Upgrade 和持续双向连接。FRP、反向 SSH 隧道或已有等价设施均可，由部署者按现有环境选择。

外部端口与本地端口可以不同；若有 HTTP 代理，仍需保留外部 Host / Origin。玩家统一访问公网入口的 `lan.html?network=direct-ws&rollback=off`。

此方案无需在服务器装 Python 或复制游戏资源，但仍经过开发者的家庭/校园网络。把程序直接部署到服务器能进一步排除这段隧道和本地网络的影响。

## 构建与版本约束（给 AI 部署助手）

1. 仓库中的完整 `web/` 已是生成好的运行版本。仅部署原样文件时直接启动 Python 服务，**不需要先构建**，也不需要 DOS 原素材或反编译环境。
2. 本仓库分发已构建的运行文件，不附带构建工具。更新时使用同一版本的完整 `web/`，不要混用磁盘、包装器和版本清单。
3. 单进程运行：房间和中继状态保存在进程内存中。不要用多进程或多实例负载均衡随机分发请求；重启会清空房间，需要重新建房。
4. 这不是通用 TURN 服务，也不能直接换成 TH06 的现有 Relay。当前自有协议包含 E7 目标封装和 E8 来源封装，依赖本项目的身份校验。
5. 当前实现用于小规模对照测试，不是已完成性能与公网生产验收的托管服务。部署时不改游戏、同步协议和预测参数，先保持对照条件一致。

## 由玩家进行对照与反馈

先确认页面能加载、双方能进同一个房间，再进入实际战斗。页面能打开不等于 WebSocket 已连通，也不等于性能通过。

保持设备、玩家网络、关卡和回滚选项一致，对比旧 Tunnel 与新服务器。记录两端同一段正常战斗中的：

- 本机同步步/秒、游戏帧，以及主观卡顿情况；同步步不等同于不同画面的帧数。
- 应用层 RTT；它包含浏览器处理时间，不是纯网络 ping。
- 等待原因、预测领先、模拟/快照/重算耗时。

当前 `direct-ws` 默认关闭回滚。想做开启回滚的第二组对照，双方都换成：

```text
https://你的域名/lan.html?network=direct-ws&rollback=on
```

切换必须退出旧局，双方重新打开完整链接并重建房间。两端策略不同会被拒绝；退出按钮可能不保留 rollback 参数，请再次使用完整链接。

关闭回滚仍会因等待真实输入而降速，尤其现有锁步只有 2 步缓冲。因此“关闭后也卡”不能单独排除同步算法的问题，也不能证明只有服务器有问题。本轮目标是获得可比较的数据，不预设结论。现有诊断不能直接测量网络丢包率。

## 常见部署问题

| 现象 | 检查方向 |
|---|---|
| `No module named websockets` | 是否用同一个 venv 安装依赖并启动服务 |
| `No module named turn_service` | 上传时遗漏模块；即使不使用 TURN 也需要该文件 |
| 页面打不开 | 进程、监听地址、端口、安全组、防火墙、DNS |
| 网页正常，建房失败 | `/api/*` 是否代理到同一个 Python 服务；Host / Origin 是否一致 |
| `/relay` 返回 403 | 反代是否保留 Host（含端口）和 Origin；身份、房间状态是否有效 |
| WebSocket 400 / 502 / 超时 | Upgrade 头、HTTP/1.1、上游是否存活、长连接超时 |
| 等待 ICE 或索要 TURN 凭据 | 打开了其他网络模式，使用 `network=direct-ws` |
| 版本或策略不一致 | 同步整份 web，双方强制刷新、使用一致参数并重建房间 |
| 手机上手柄不可用 | 检查 HTTPS 安全上下文和浏览器权限，先用键盘设备确认服务链路 |

部署完成后，只需反馈：**完整玩家入口 URL、节点地区、是否经过额外隧道/代理，以及部署的仓库版本**。不要在反馈里发送 SSH 密码、私钥或房间 token。
