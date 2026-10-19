# TH07 共享结界与对话 · 2026-10-19

## 状态与结算

共享结界传播时绕过了 `HasCurrentMsgIdx()`，对话处理又将 `BORDER_READY` 与 `BORDER_ACTIVE` 一起按自然结束结算。结束只清理队友的激活状态，待开启状态残留后能再次触发共享开启和奖励。

修复保留对话期间的待开启状态，不生成结界效果，也不发放结束奖励。对话只结束已经激活的结界；自然结束拒绝未激活状态，且共享结束一次性清理所有参与者的激活及待开启状态。取消队友的待开启状态不改其生命状态、复活或无敌计时。Bomb 破除同样清理残留待开启状态。

普通单人分支不变；多人 Gameplay ABI 从 12 升到 13、Netplay ABI 从 13 升到 14，避免旧 Replay 或不同规则客户端混用。

## 验证

检查项：MP-RULE-02、MP-RULE-04、MP-04、MP-11。

`audit/source-contracts/shared-border-dialogue-test.mjs` 提取生产 C++ 状态转换与 GUI 结界结算分支，用 WASI 执行；画面及声音接口以计数器替代。`--before` 使用修复前 `dc78e4cf` 复现失败，当前代码通过：

- 2P、3P 在进入对话同帧触发结界，持续 600 帧等待不增加分数、奖分或结界效果。
- 对话结束正常开启，已有结界遇到对话只结算一次；再次等待 600 帧不重复领奖。
- 队友正在 Bomb 而处于待开启时，共享结束取消其待开启状态，但保留生命状态；Bomb 结束后不重新开启结界。
- 直接对待开启状态调用自然结束不发奖。

Web multiplayer 构建通过；实际生产 Runtime 双客户端预加载及开局检查通过，双方在相同逻辑帧的世界哈希一致。CherryMax 既有源码检查同步更新为当前 `GameplayAbi()` 的挑战模式分支声明，检查通过。状态转换测试是生产函数级回归，不代替完整关卡人工通关验收。

## test 交付

test 版本 `th07-border-dialogue-20261019-91d95c245e78`，Runtime 代次 `fcfbd3fafabd0d26cd1997b71fdde9a7b1ad03e2ee464761ca6328af8169f458`，App Shell `1b9f4695c864382fa763`。发布文件校验覆盖 3409 个文件、17 组 Runtime。仅替换 TH07 multiplayer Runtime，保留 test 现有前端及 TH11 计时修复；主站、beta、普通版、资源节点和联机服务不变。

发布与回滚记录位于 `/var/tmp/th07-border-dialogue-test-20261019/`，回滚链接为 `/var/www/eagler-touhou-test/rollback-before-th07-border-dialogue-20261019-91d95c245e78`。本次不调整 Nginx 配置。
