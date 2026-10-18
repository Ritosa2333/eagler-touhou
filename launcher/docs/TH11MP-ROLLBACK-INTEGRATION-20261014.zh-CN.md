# 地灵殿回滚启动契约整合

来源：experiment/th11-rollback-runtime，提交 b3d1ce0（产品和测试）及 74dfd43（验收记录）。该树基于 launcher-refactor 的 b6aa0ae，本树基于 main 的 b479f0a，共同祖先 74b2f12。仅移植产品声明、快照及 Runtime 参数合同，不合入目录重构。

- TH11 开启回滚时 D0/P12，关闭时纯延迟 P0。
- catalog 的 rollbackLimit/sendPredictionLimit 均为 12。
- 真实隔离 relay 的两人、三人、自动与手动纯延迟，以及真实 room service 和 React 组件测试通过。
- 公网游戏 Runtime 已是回滚版本，但当前 relay catalog 仍为 P0。2026-10-14 用户明确要求暂不重启 relay，因此本次提示删除前端发布保持旧的运行契约。恢复 P12 的源码已准备，不宣称公网回滚已恢复。
- 后续服务发布应从同一源码构建契约，核对 th11 声明与实际 2P/3P start -> Runtime options，避免只更新 Runtime 或只替换前端。

## 后续主站恢复

2026-10-14 用户随后明确授权重启。已恢复线上 relay 的 P12 产品声明，并发布匹配前端，release 为 `sha256-004eda7bc3ff5ac5ff1596cfe2923150abf9a4f8afdaad513fda32cdceb2766d`。未更新 TURN、端口、nginx 或 test 站。

真实公网 WSS 开局及不同 IP 配额隔离/释放通过。实际主站两端原生 Runtime 均为 D0/P12/auto=false，推进到 223/249 个确认帧且无原生错误；该项使用清单哈希一致的字体缓存。额外的固定时间帧数门槛没有通过，因此只确认回滚配置、开局和帧推进，不宣称帧率、流畅度或首次冷下载验收通过。证据保存于工作区 `artifacts/th11-rollback-restore-20261014/`。
