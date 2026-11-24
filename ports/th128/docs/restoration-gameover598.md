# 实际失败与一次续关验收（598）

2026-11-16，使用既有 private100 和候选595，完成 A1/rank0 的新分支验证：同一局自然 Game Over → 原版菜单 Continue → 再次自然 Game Over → 原版菜单 Title 动作 → 完整失败成绩档案确认。

这不是无辅助通关验收。本测试为观察真实失败而保留有限生命，使用固定合法输入，不做躲避 AI、不强制修改生命、干劲或历史，也没有重复六线和 EX 成功流程。

## 结果

- 1 个实际 World，2573 次 tick 调用、2571 个原生记录帧。
- 2 次自然 Game Over，1 次原生 Continue，47 次原生菜单 tick。
- Continue 将分数恢复为0、干劲恢复为20000、续关数增至1；菜单期间 World 帧和游戏 RNG 均不推进。
- 续关后，原生录像保存入口明确拒绝，菜单录像项禁用。
- 第二次失败后，原生 Title 菜单动作完成；完整52232字节失败成绩由实际生产消费者核对后送入原生 ACK。
- 7条路线×5种难度的35个通关计数均为0，失败没有授予通关资格。
- 最终游戏 RNG 为 seed55661/calls9384。Title 动作后没有追加 World tick。
- 运行退出0、标准错误0字节、无首次失败文件，ROOT 已释放。

档案摘要：`5bb810003e5e4747be0fc0a1649b0a7b7f7a9c1cbaf4ae7144cb162161404c64`。

## 验收边界

World、ECL/ANM/MSG、原生输入记录、暂停菜单和成绩生产者是真实实现；JavaScript 使用实际 OriginalPauseController 与 persistOwnedNativeScoreCapture。音频设备输出和成绩存储为明确的模拟边界。

本项只确认 Title **菜单动作**已提交，未运行完整 Title/Main 资源准备或浏览器返回界面；也未测试等待中的 owner 被另一局替换。真实浏览器持久化、异步替换、续关后完整终局/结局仍各自待验。

498的原始记录已证明5次Continue，但最终因测试脚本错误尝试被禁用的第6次而失败；该记录保留。598只关闭上述新分支范围，不覆盖或重标498。

结构化摘要见 [acceptance-gameover598.json](acceptance-gameover598.json)。归档测试源码位于 `tests/gameover-continue/`，原始资产和运行核心不随源码分发。
