# Relay 入口自动切换发布

网页优先连接 `relay.touhouonline.com`。建立连接失败或超时后，自动使用
`cn-relay.touhouonline.com:8443`，由 SSH129 反代到日本的同一个 Relay 进程。
大厅目录、房间与连接检测共享当前页面的选路结果，游戏启动沿用已选择的入口。

| 发布入口 | App Shell 构建 |
| --- | --- |
| touhouonline.com | 4d70fdfb91eb3da375ea |
| touhou.vip | 4f2b4db8401334161352 |
| 海外边缘节点 | 与 touhouonline.com 同步 |

浏览器分别验证两个线上大厅：阻断首选 WebSocket 后，备用入口成功返回
`directory`。两个入口的独立连接在同一个房间内双向交换信令。配额检查覆盖
同一客户端经不同入口累计两个房间、第三个房间限流、另一客户端独立获准，以及
离开后的配额释放。日本来源访问两个 Relay 入口均返回 403。

前端由已验证的 106 个文件组成，压缩传输约 2.7 MB。SSH129 直接接收构建产物。
原有游戏、Runtime 和资源逐文件核对，未修改部分的 SHA-256 与前一发布保持一致。
发布继续保留 th075 两个站点语言包和海外跳转资源的离线缓存行为。

两台机器的 `/root/relay-entry-20261027/site-state.json` 保存前一发布目录与当前目录。
网页目录采用原子切换，Relay 与 TURN 进程由现有服务继续承载。
Cloudflare 发布版本为 `ebca3d8b-eae2-4ad4-9653-ef62c5f33810`。
