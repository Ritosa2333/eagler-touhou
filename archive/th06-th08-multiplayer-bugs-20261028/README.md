# TH06／TH08 多人索敌与 Last Spell 修复验证

基线为 `95f6b9836485568d1c870ca27285395d5dd6ad32`。TH06 修复提交 `6233d52af`，TH08 MP 修复提交 `f7025e30d`。审查分支对齐到 `5db7ef555`；这次上游推进涉及启动器与服务，相关作品和公共原生源码树一致。构建摘要、原生状态和分项结果见 [verification.json](verification.json)。

## 玩法结果

| 场景 | 构建与检查范围 | 结果 |
| --- | --- | --- |
| #148 红魔乡灵梦 A 索敌 | 直接编译实际 EnemyManager 的多人索敌片段；双人／三人、无人命中、混合伤害、目标优先级、同位置和无效席位 | 命中前各玩家即可取得目标；原条件复现失败，修复后通过 |
| #137 永夜返衔接 | TH08 MP 诊断 SDL 构建，原始 ECL；双人／三人，P1 死亡处理中或已经成为幽灵，共 4 组 | 任一有效玩家就绪后，保护阶段的 Boss 脚本继续推进超过 120 帧；原逻辑 4 组冻结，修复后通过 |
| 蓬莱之药被弹解除 | TH08 MP 诊断 SDL 构建，原始 ECL 与实际子弹碰撞；双人／三人、普通／挑战、P1／末席，共 8 组 | 自动解除启动并完成，原生 Last Spell 标记清除；原逻辑挑战模式 4 组失败，修复后全部通过 |
| 自动解除与挑战规则 | 实际 PlayerLife／PlayerBomb，普通及 MP 编译路径、全部 12 个机体 | 自动解除完成；挑战模式仍禁止手动 Bomb，库存保持规则值 |
| 正式多人构建 | `portable/build.mjs --multiplayer` | 编译完成；正式 WASM 中诊断 fixture 导出数量为 0 |

原生脚本检查通过诊断入口设置关卡与死亡初始条件，随后运行游戏持有者和原始 ECL。覆盖符卡 172（永夜返）和 144（蓬莱之药），共 12 组；网络传输验收与完整流程通关分别由对应驱动承担。

TH06 的改动位于多人编译分支，沿用原有目标优先级。TH08 的实现位于 `ports/th08-mp`；挑战模式允许原生自动解除持续完成。`EnemyDamageParticipant.life_state` 是临时查询结果，序列化状态布局保持既有定义。

## 执行

Windows PowerShell；本机 CMake、WASI SDK 34、Emscripten 6.0.9-git、Playwright Chromium／SwiftShader。构建环境使用仓库规定的 SDK 环境变量和本地合法资源。

```powershell
node --test ports/th06/tests/homing-target.test.mjs
node ports/th08-mp/portable/check-multiplayer-enemy-damage.mjs
node ports/th08-mp/portable/check-multiplayer-last-spell.mjs
node ports/th08-mp/portable/check-multiplayer-rules.mjs
node ports/th08-mp/portable/build.mjs --multiplayer-fixtures
# 使用 portable/multiplayer/serve.mjs 提供诊断构建与本地 DATA 后：
python -X utf8 ports/th08-mp/portable/multiplayer/check-last-spell.py --url http://127.0.0.1:8158/ --output artifacts/th08-last-spell.json
node ports/th08-mp/portable/build.mjs --multiplayer
```

对照构建使用当前诊断入口，将 `EnemySimulation.cpp` 和 `PlayerBomb.cpp` 两处运行逻辑还原为上述基线；验证构建使用修复提交。两者的 WASM 身份和逐场景末态保存在机器记录中。

## 既有检查项

构建隔离检查使用实际构建目录后，源码计划、网络导出、诊断头文件归属三项通过。另一个断言要求的 `mp_fixture_spectator_checkpoint_group_bytes` 在基线和修复诊断包均不存在。Hosted keyboard bridge 的既有正则断言与当前 shell 路由不一致，相关测试与 shell 的 Git blob 均和基线一致。机器记录保存这两个既有检查结果及对应身份。

适配状态清单采用同次 CI 的实际源码检查报告刷新：18 个目标的摘要与本地生成结果逐项一致，全部源码检查通过。执行身份、报告 Artifact 与核对结果保存在机器记录的 `sourceStatusRefresh`。
