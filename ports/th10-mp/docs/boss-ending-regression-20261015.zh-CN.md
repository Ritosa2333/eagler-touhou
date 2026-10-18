# TH10 MP Ending 输入与一面 Boss 碰撞排查

基线：真实 MP 源码 `th10-eagler`，`eagler-mp` 的 `0999524`。构建显式使用 `worktrees/th11-multiplayer/third_party/eagler-common` 的 `62345858e1a4b101074cdc04720eac44a7c89cf0`，与 MP gitlink 一致。保留本树原有 common 检出状态，不把依赖差异混入修改。

## Ending

战斗结束后 `multiplayer_session.configured` 仍然为真，但网络输入不再推进。原来的 `AppFrames::update_input()` 因而一直复制停更的 host lane，物理 Z/Ctrl 到不了 Ending。现在只在真实网络游戏 owner 活跃时消费联机输入，其余场景由原输入设备 owner 采样。

真实 WASM 回归在 Ending 画面 14 复现旧行为：按 Z 前后 raw/pressed 都是 0，页面等待仅自然递减。修复后 Z 的 raw/pressed 均为 1，页面等待立即清零；随后 Ctrl 的 raw 为 256，脚本继续前进。证据：`artifacts/mp-contact-baseline/ending-3.json` 与 `artifacts/mp-contact-fixed/ending-final.json`。

最终正式 MP 编译、打包通过，WASM SHA-256：`17a4025fee33fed9740e6da1b9cdd3d6b7d338e326e8cfdcc2b20b2a1b855ba4`，输出 `artifacts/fixes-20261015/runtime-final/`。8 项打包/诊断隔离测试通过。Boss/Ending fixture 仅进入 `--multiplayer-fixtures`，不进入正式 Runtime。

## Boss 闪退：死亡清除激光耗尽动画池

增加 `portable/multiplayer/check-boss-contact.py`，通过限定的起始位置/残机设置，让真实一面 ECL、碰撞、死亡和回滚继续执行。Normal 常规死亡没有重现，后来在 Lunatic、P2 最后残机死亡、八帧延迟输入纠错的场景稳定触发 `unreachable`。调用栈为 `Frame::clear_lasers → cancel_lasers → cancel_effect → AnmManager::create_at → AnmVm::update`。临时失败探针明确记录 `ANM overflow pool exhausted`，随后父脚本 43 创建子脚本 42 失败。探针现已删除。

激光每个消除采样会创建父/子动画，旧 MP 溢出池只有 2048 槽，加上原生 4096 槽仍不足以容纳该爆发。改为 8192 个溢出槽，并以同一容量常量驱动存储、RollbackPoolCapture、原生/跨图哈希和诊断计数。保留激光数量、消除采样、动画脚本和原生执行顺序；新增静态储存约 5.6 MiB，闲置槽不作为活跃动画逐帧复制。未声称任意资源/任意内存压力均不会耗尽。

原始失败证据：`artifacts/mp-contact-baseline/contact-p2-lunatic-spirit-survival.json`，以及带失败探针的 `contact-trap-probe.json`。

修复后的相同 Lunatic 场景完成 26 次碰撞死亡，其中 15 次在符卡期间，经过最后残机转幽灵和延迟输入纠错，推进到第二面且没有原生/浏览器错误。每 tick 观察到的原生槽峰值为 4095，溢出槽峰值为 2995，实际超过旧的 2048 限制。证据：`artifacts/mp-contact-fixed/contact-p2-lunatic-spirit.json`。这是受控起始条件的真实 WASM 回归，不冒充人类无辅助通关或移动设备验证。

`portable/check-anm-overflow-rollback.mjs` 在完整 8192 槽上通过原始字节恢复、释放重用、首写屏障、容量失败与旧分配实现反例检查。记录：`artifacts/anm-overflow-rollback/run-2026-10-15T05-47-10-128Z/receipt.json`。驱动支持显式 `EAGLER_COMMON_ROOT`，保留源码闭包校验及精确外部依赖边界。Ending 的最终同构建回归见 `artifacts/mp-contact-fixed/ending-capacity-final.json`。

最终构建另通过原生决死纠错及三人回滚完整 owner/音频比较，分别记录于 `artifacts/mp-contact-fixed/deathbomb-capacity-final.json` 和 `artifacts/mp-contact-fixed/rollback-3p-final.json`。正式包与源码哈希一致。

用户已要求提交并更新主站两作 MP。发布以当前线上版本为底，仅更新 TH10/TH11 的 MP Runtime 和对应清单，保留启动器、其他游戏、资源包及现有 relay。发布身份、公网逐文件校验和回退目录记录于工作区 `artifacts/mp-runtime-fixes-publish-20261015/publish/`，以该目录的实际发布结果为准。
