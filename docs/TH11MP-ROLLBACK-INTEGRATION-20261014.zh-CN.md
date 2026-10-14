# 地灵殿回滚启动契约整合

来源：experiment/th11-rollback-runtime，提交 b3d1ce0（产品和测试）及 74dfd43（验收记录）。该树基于 launcher-refactor 的 b6aa0ae，本树基于 main 的 b479f0a，共同祖先 74b2f12。仅移植产品声明、快照及 Runtime 参数合同，不合入目录重构。

- TH11 开启回滚时 D0/P12，关闭时纯延迟 P0。
- catalog 的 rollbackLimit/sendPredictionLimit 均为 12。
- 真实隔离 relay 的两人、三人、自动与手动纯延迟，以及真实 room service 和 React 组件测试通过。
- 公网游戏 Runtime 已是回滚版本，但当前 relay catalog 仍为 P0。2026-10-14 用户明确要求暂不重启 relay，因此本次提示删除前端发布保持旧的运行契约。恢复 P12 的源码已准备，不宣称公网回滚已恢复。
- 后续服务发布应从同一源码构建契约，核对 th11 声明与实际 2P/3P start -> Runtime options，避免只更新 Runtime 或只替换前端。
