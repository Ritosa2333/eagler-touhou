# Common 统一集成（2026-10-14）

本次把回滚阶段成果合回本仓库 eagler-mp，普通 eagler 不接收此多人实验线。

common 统一到 62345858e1a4b101074cdc04720eac44a7c89cf0。该提交同时包含根 main 的练习工具、触控/启动/Replay 成果，TH10 的稀疏池和批量快照，以及 TH11 的只读快照、即时输入桥和退休会话修复。

共用单一只读窗口状态机。窗口内源字节必须稳定，显式 EndReadOnlyCapture 后才能 EndFrame；保留失败排空、arena/heap 增长处理和快照计数。不改变普通 Touch 的同步语义。

此次修正 TH10 快照测试的显式关闭合同，并适配 BrowserPeerTransport::StopSpectators(bool) 测试替身。正式旧调用仍走默认 false。

验证：common 本机 40 项、TH10 完整多人规则测试、TH10 真正 Emscripten 批量快照含越界负例、TH11 WASI/Emscripten 日志、TH11 世界/原生玩法和网络回滚均通过。各作正式构建结果见本次合树交付记录。

性能结论没有因合树改变：TH10 r10p RAF guard 默认关闭；受控派发延迟收益不能替代正常恢复停显验收。既有约 124 ms 停显及未完成运动证据仍保留。此集成不声明手机、公网或整作性能完成，也不授权站点发布。

原始冻结候选和失败样本仍在 artifacts/rollback-smoothness，不应在清理工作树时丢弃。旧产物的 common 版本和源码身份不能代替统一版本的实际构建身份。
