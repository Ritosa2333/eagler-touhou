# 香港镜像与统一 Relay 入口 | 2026-10-28

香港节点以同一发布代提供网站，所有联机入口共享一套房间。上游发布通过逐文件
SHA256 校验与原子目录切换到达镜像，现有对局通过入口转发持续连接。

## 域名与角色

| 入口 | 地址 | 角色 |
| --- | --- | --- |
| `touhouonline.com` | `103.73.161.228` | HK 网站入口，CF 灰云单 A，TTL 120 秒 |
| JP 发布源 | `74.82.198.36` | 上游发布、海外边缘同步源及保留站点 |
| `relay.touhouonline.com` | `103.73.161.228` | 香港 WSS 入口 |
| `cn-relay.touhouonline.com:8443` | `129.211.174.65` | 既有国内备用入口 |
| `relay.touhouonline.com:8443` | HK TCP 转发至 `74.82.198.36:8443` | 既有加密 TURN 备用入口 |

网页、Runtime 与 Service Worker 使用同一域名和同一组已发布字节。
主域名统一进入香港，CF 海外边缘继续跟随 JP 的发布源，资源分流沿用现有节点。
WS 首选香港入口，连接失败后由页面切换到国内备用入口，两个入口共享房间。

## 发布同步

JP 当前发布源为 `/var/www/eagler-touhou/current`。`eagler-hk-export.timer`
发布发布代、文件长度与 SHA256；HK 的 `eagler-hk-mirror.timer` 每分钟检查新发布，
使用只读 SSH 密钥拉取、校验并原子切换 `current`。发布代由源目录与
`sync-version.json` 标识，发布时更新其中任一项即可触发新一代镜像。

首次镜像来自 `relay-entry-20261027`，共 6,253 个文件，约 2.43 GB。
镜像标识为 `cee765aec6c153ab46e2747a4bc82f8adf816da5b77c5a449752f69d4b680a76`。
同步包括站点自有的语言包归档、资源节点清单和当前 TURN 配置。
镜像保留旧发布，复用相同文件的数据块与硬链接。

HK 使用独立的 DNS-01 证书 `touhouonline-hk`，自动续期后 reload Nginx。
HK 的 HTTP ACME 路径转发至 JP，使 JP 既有的 HTTP 验证继续使用公开域名。
上游 TLS 保留证书和主机名验证，证书链深度设为 4。

## 房间服务迁移

香港入口先转发至 JP 的现有房间进程。`eagler-hk-relay-cutover.timer`
每 30 秒检查 JP 的房间状态；存在进行中的对局时保持原服务。
对局全部结束后，迁移控制器刷新昵称状态，启用 HK 本地 Relay，令 JP 旧入口
反向转发至 HK，并停用 JP 原房间进程。等待中的大厅通过原重连流程进入新服务。

两端 `/root/hk-migration-20261028/core-moved.json` 标识房间进程已迁移。
交付检查时，入口已切换至香港，房间进程等待 JP 对局结束后自动切换。

HK 的 Relay 复用原运行包与配置，信任的内部代理为 `127.0.0.1`。
HK 公共代理信任 JP 与国内备用入口的精确地址；JP 过渡入口信任 HK 与国内备用入口。
各 WebSocket location 覆盖 `X-Real-IP`，地域规则按最终客户端地址执行。

HK 同步运行 CF TURN 凭据适配器与 900 GB 月度预算门禁，保留原 90 秒监控新鲜度要求。
邮箱和既有 TURN 分配服务保持原部署位置。

## 验证范围

| 检查 | 证据 |
| --- | --- |
| 镜像完整性 | 6,253 个文件逐项比对长度与 SHA256，连续同步轮次使用同一发布标识 |
| 国内入口 | 南京服务器分别直连 HK 与 JP，首页均为 200 |
| 日本准入 | JP 实际出口访问 HK 首页与 Relay，均为 403 |
| 海外 TURN 身份 | 内部适配器及可信公开代理链均返回 CF TURN 候选 |
| 浏览器联机 | 大厅目录、信令、`route=rtc`，选中 `relay/relay` 候选对并完成 10 次回显 |
| 客户端地址隔离 | 一公网地址的两个房间达到配额后，另一公网地址独立进房；离房后原地址恢复配额 |
| HTTP 契约 | 清单、SW、Web Manifest、robots MIME 正确，未知路径为 404 |
| TURN TLS | 限定 HK 8443 候选，完成证书验证、`relay/relay` 分配、`route=rtc` 与 10 次回显 |

浏览器记录来自本机当时的网络环境；国内与日本准入使用对应服务器的实际出口验证。

自动化脚本：[发布清单](export-site.py)、[镜像同步](mirror-site.py)、
[JP 切换控制](jp-cutover.py)、[HK 排空迁移](hk-cutover.py)。
对应记录：[DNS](dns-after.json)、[浏览器中继](browser-verification.json)、
[8443 加密中继](browser-tls-verification.json)。

## 回退入口

JP 原站点与运行包保留。JP 的 Nginx 快照位于
`/root/hk-migration-20261028/site-before-hk.conf`；房间切换前另存
`site-before-core-move.conf`。HK 保留 `site-bridge-relay.conf` 与
`site-local-relay.conf` 两种入口配置。

回退时先停用 HK 的迁移定时器。房间进程切换前可将 Relay A 与主域名单 A
改回 JP。房间进程已迁移时，在对局结束后将最新昵称状态带回 JP、
启用原 Relay、恢复 JP 站点快照并验证入口，然后调整 DNS。
切换控制密钥与 DNS 凭据保存在各服务器的 root 私有目录。

## 首次访问入口

首页首次连接与 WS 自动切换分别验证。首次访问尚未取得页面时，备用连接由
浏览器的 DNS 连接实现或可达的边缘入口建立；已安装的 Service Worker 不作为
新访客的前置条件。当前采用香港灰云直连，WS 使用国内备用入口。

Chromium 的 HTTPS 记录解析目前只接受原始域名或其规范域名，并忽略改用其他
端口的记录，跨域名的优先级记录不承担通用浏览器的首次访问切换。
实现依据见 [Chromium DNS 解析](https://github.com/chromium/chromium/blob/main/net/dns/dns_response_result_extractor.cc)。

## 国内入口采样

南京腾讯云与北京阿里云的实际出口直接请求同一发布的 HTML 和三个入口脚本。
每个入口使用三次独立的 HTML 连接，单次下载预算为 12 秒；记录完整传输时间、
响应文件摘要和 CF 机房。环境代理通过请求参数排除，出口由香港访问日志核对。

| 出口 | 香港 HTML 完整下载 | CF HTML 完整下载 | 约 206 KiB 入口脚本 |
| --- | --- | --- | --- |
| 南京腾讯云 | 0.24–0.26 秒 | 0.97–1.14 秒 | 香港 0.33 秒，CF 0.96 秒 |
| 北京阿里云 | 0.29–0.31 秒 | 1.92 秒；另外两次到 12 秒时尚未完成 | 香港 0.40 秒，CF 到 12 秒时尚未完成 |

CF 样本命中缓存，南京落点为 AMS、LHR，北京落点为 LAX。完成下载的 HTML 和
脚本摘要与香港发布一致，采样衡量同内容的网络传输时间。

北京出口 `8.130.105.151` 按已确认的国内节点身份加入地域覆盖，两端的每日更新
保留该地址。修正后，北京直接取得香港的完整页面。当前主域名继续直连香港，
CF 作为现有海外分发节点。原始采样与国内公开 WS 目录结果见
[入口采样](entry-quality.json)。
