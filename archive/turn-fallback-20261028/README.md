# TURN 选择同步

质量与容量选择器的结果独立发布到新站，UDP、TCP 与 TLS 入口使用同一节点。房间服务读取动态配置，为新连接签发匹配该节点的短期凭据。公共协议见[联机架构](../../docs/architecture/netplay.md)。

## 发布路径

1. 国内原监控在选择器同一把锁下导出节点、URL、评估时间和私有凭据配置。JP 的专用 SSH 密钥固定来源地址和只读命令。
2. JP 每 15 秒读取结果，校验节点与 180 秒时效，先 reload TLS 上游，再原子更新私有 TURN 配置和只读导出。
3. HK 每 15 秒单独读取小型导出，核对所选节点与 TLS 上游，再更新实际 Relay 读取的私有配置。

脚本为 [`export-selection.py`](export-selection.py) 和 [`sync-selection.py`](sync-selection.py)。参数、节点地址、受限密钥和共享密钥由服务器上的私有配置提供。网站镜像不再写入 TURN 配置，网页的大文件同步不占用选择发布任务。

每次同步记录所选节点、上游评估时间、最近成功时间和配置摘要；失效、倒退或不匹配的结果不替换当前配置。同步失败通过任务失败状态与日志呈现。配置切换对新连接生效，已有 TURN 分配沿原连接结束。

## 事件定位

2026-10-28 核对到原监控已选择 `best-beijing`，新站却沿用迁移时复制的上海配置。JP 向 HK 导出的也是这份静态配置，TLS 上游单独指向上海。断点位于选择结果到新站的发布链。

本次补齐受限导出、两段独立定时同步和 TLS 上游更新，移除 HK 网页镜像的 TURN 写入。既有节点优先级、质量判定、容量阈值和 CF TURN 预算策略沿用当前设置。

活动节点池、质量监控、容量监控及 1145 的探测目标已移除上海。正式房间服务在主线更新时载入 Best 与 1145 的 STUN 声明，当前候选配置完成同步。

## 验证与恢复资料

- [`test-sync-selection.py`](test-sync-selection.py) 的五项隔离检查覆盖备用切换、TLS 更新先于凭据发布、重复同步、reload 失败回退、过期与错误节点拒绝、HK 上游对应校验和版本倒退拒绝。使用临时文件与模拟 reload，不停用线上节点。
- [`selection-verification.json`](selection-verification.json) 记录国内监控、JP 和 HK 一致的选择评估时间，两段定时任务连续更新成功。
- [`browser-normal.json`](browser-normal.json) 与 [`browser-tls.json`](browser-tls.json) 使用公网房间协议交换信令，双方选中的候选均为 relay，实际地址为 Best，服务端进入 `rtc`，每次收发 10 个回显。
- 本轮浏览器普通路径选中 TCP 中继，TLS 路径选中 TLS 中继；另从国内原服务器完成双向 UDP、TCP ChannelBind 回显，各方向 480 个消息完整收到。
- 初次恢复选择同步时，TLS 入口 PID 保持 29801，HK 房间服务 PID 保持 3436，CF TURN 代理 PID 保持 1868。随后正式房间服务升级为 PID 17042，两项代理进程保持运行，见[主线发布](../main-refresh-20261028/README.md)。

JP 与 HK 的 `/root/turn-incident-20261028/` 保留此前私有配置和脚本副本。停用 `eagler-turn-selection-sync.timer` 可冻结当前已发布节点；恢复网页镜像脚本时同时核对 TURN 配置写入归属。上述资料与验证覆盖基础设施发布及实际浏览器数据通道。
