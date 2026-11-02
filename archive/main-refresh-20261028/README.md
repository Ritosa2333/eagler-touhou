# 主线发布与镜像一致性

主线 `5db7ef555002b00738a912cb73fcb84cd943c7ca` 统一发布到香港入口、日本发布源、国内备用入口及 Cloudflare。三个 VPS 与边缘使用同一份前端、Runtime 清单和 App Shell 身份，入口恢复继续沿既定的香港、日本、国内顺序执行。

## 发布身份

- Release：`sha256-7172c69ed3c2311121b79c4ff98a159afc4a3307b4f919a387106790ab94482e`
- App Shell：`64ab31890e418a7d1a42`
- Release 文件数：6483
- 日本发布目录：`/var/www/eagler-touhou/releases/main-5db7ef555-20261028`
- 上一日本版本：`/var/www/eagler-touhou/releases/web-9d5156ca39cd-g3eedgwt`

本轮本地 `main` 从 `9ed28346d54b6a99da740637aa04f7db546a2263` 前进 7 个提交，内容包括统一顶栏与图标、状态页、thprac Information 面板触控交接，以及 th08 多人版 Ending 与原生 Replay 验证。同步前的本地改动保留在工作区和独立备份中。

## Runtime 与发布能力

普通版 th06、th07、th08、th10、th11、th15 及 th08 多人版复用国内已发布并登记来源的触控修复构建；th12 普通版与 th10、th11、th15 多人版按当前主线重新构建。th12 的公共依赖锁定包含同一触控修复，普通版保留 thprac 和语言能力。

| Runtime | 当前代次 |
| --- | --- |
| th12 普通版 | `bfcf985060e4c1c9934e67b145f9967230449de26bc2e9b6ab1874e2a1bea262` |
| th10 多人版 | `5345d537ec2ad8232269858730cf02ea67a5102a6b96db9c98cb7194d6bbe6c7` |
| th11 多人版 | `620ff3a1fe2e2bde19e7808c83913cb6cd8b6f3fb56b7f482eab60b1cb44ff80` |
| th15 多人版 | `3c4097543165f0550335f9ef31ef7fa16150a0d9e1b9e0ba12fa0736bbc0277c` |

External 发布验证器按语言声明识别 th075 的站点语言 ZIP，逐项核对路径、长度与完整 SHA-256。游戏 DATA 与共享资源继续由资源节点和私有 R2 分发。

## 状态页与联机服务

新域名的状态服务位于香港，延续原有统计历史。主域名、备用入口和 CF 将 `/uptime/summary` 连接到同一服务，`/uptime/` 显示同一静态页面。CF 使用 Static Assets 提供状态页文档，动态摘要由原有 API 转发入口处理。

新版房间服务先经独立公网入口核对客户端 IP 与 TURN 数据通道，再于 2026-10-28 13:28:58（UTC+08:00）切换正式服务。正式服务沿用端口 19445、状态端口 18156 和原配置文件，启用新主线构建，正式进程 PID 为 17042，开机启动继续由原服务单元管理。

正式公网入口分别验证 TCP 与 TLS TURN：双方候选均为 relay，服务端选择 `rtc`，两次各收发 10 个回显。两个公网来源的房间配额分别计量，超额请求被拒绝，离开后对应配额释放。临时验证入口与进程已停用。

日本 TLS 桥接 PID 29801、香港 CF TURN 代理 PID 1868 保持运行，TURN 服务沿原生命周期工作。正式进程加载 Best 与 1145 的 STUN 声明，上海节点已退出当前候选配置。正式服务切换使用独立的 systemd 覆盖文件，原服务源文件与配置保留为回退资料。

WS 与 RTC 生命周期见[联机架构](../../docs/architecture/netplay.md)。信令断线保留已建立的 RTC 对局；大厅、席位和旁观属于房间进程。TURN 选择发布链和上海节点移除见 [TURN 选择同步](../turn-fallback-20261028/README.md)。

## 证据

- [`public-publication.json`](public-publication.json)：四个入口的 Release 身份、状态路由及 th09、th12、th10/11/15 多人运行包完整下载校验。
- [`public-status-routes.json`](public-status-routes.json)：补齐状态页入口后，使用浏览器 User-Agent 核对四个入口的页面与摘要。
- [`browser-tls-verification.json`](browser-tls-verification.json)：主站 MIME、真实 404、独立新版服务的双方 TLS TURN 数据通道。
- [`public-quota.json`](public-quota.json)：两个公网来源的配额隔离与配额释放。
- [`browser-offline.json`](browser-offline.json)：当前主站在线安装后断网刷新，页面与可见封面图由 App Shell 正常提供。
- [`relay-cutover.json`](relay-cutover.json)：正式服务的新旧进程、源码摘要、端口、可信代理和 STUN 配置。
- [`browser-production.json`](browser-production.json)、[`browser-production-tls.json`](browser-production-tls.json)：正式入口双方实际 TURN 数据通道。
- [`public-production-quota.json`](public-production-quota.json)：正式代理链上的客户端 IP 配额隔离与释放。
- [`production-final-status.json`](production-final-status.json)：切换后主站的站点声明与状态摘要。

回退保留上一站点发布、上一边缘版本、服务源文件与服务器私有配置备份。新发布前端与运行包时，网页恢复准备步骤继续作为导出和边缘同步的前置条件。
