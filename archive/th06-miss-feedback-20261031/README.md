# TH06 多人 Miss 与符卡呈现

挑战模式的生命行以固定字号、白色和全窗口坐标绘制独立 Miss 数字。计数在决死窗口结束时记录，达到 `uint32` 上限后饱和。多人恢复原生敌方符卡立绘绘制，六面道中咲夜符卡使用已加载的原生立绘与动画。

死亡粒子与 PICHUN 以两层条件输出：原生死亡判定完成，对应输入帧获得确认。粒子在确定性的模拟过程中生成并随快照恢复，绘制阶段读取确认标记；音效记录按局次和逻辑帧保存，重算会替换该帧的候选死亡，确认后的同一次死亡输出一次。确认输出不会推进游戏 RNG。多人规则 ABI 从 19 更新为 20，使录像与联机共同识别新的死亡反馈时序。

Rank 保留现有的人数分摊规则。单人死亡基础惩罚为 1600，双人每次为 800，三人每次为 533。完整游戏回归同时击中所有玩家，验证双人合计扣 1600、三人合计扣 1599。

## 原生与浏览器回归

[验证数据](validation.json)登记构建身份和检查范围。使用实际游戏对象、原生 Player 回调及回滚日志的双人／三人浏览器夹具覆盖：预测死亡后撤销计数和粒子、确认死亡音效去重、死亡判定窗口、实际 Rank 降幅、各玩家 Miss 字号／颜色／坐标，以及六面立绘资源。三人画面已目检，生命行分别显示 23、24、25，咲夜立绘与符卡名同时可见。

独立音效日志测试覆盖重复预测、连续 12 帧被撤销的候选死亡、跨局清理、嵌套作用域及 BGM 恢复。资源测试覆盖各人数的 Rank 分摊。普通版未启用多人／网络宏时，修改涉及的三个翻译单元编译通过。来源检查普通版 11 项、多人版 10 项通过。历史源码断言与现行实现的对应关系见验证数据。

浏览器夹具通过 CMake 的项目包含入口显式接入，不进入默认产品构建：

```text
cmake -S ports/th06 -B artifacts/th06-feedback-browser/web -G Ninja
  -DCMAKE_TOOLCHAIN_FILE=<Emscripten.cmake 的绝对路径>
  -DTH_EXTERNAL_ASSETS=ON -DTH_WEB_SHARED_FONT=ON
  -DTH_ENABLE_NETPLAY=ON -DTH_ENABLE_MULTIPLAYER_GAMEPLAY=ON
  -DTH_ENABLE_THCRAP=OFF -DTH_ENABLE_THPRAC=OFF
  -DCMAKE_PROJECT_th06_INCLUDE=<tests/miss-feedback-browser.cmake 的绝对路径>
cmake --build artifacts/th06-feedback-browser/web --target th06
```

设置 `TH06_TEST_ARCHIVES` 为本地合法提供的 TH06 日文 DAT 目录，`TH06_TEST_FONT` 为共享 Gothic 字体，再运行 `node ports/th06/tests/multiplayer-miss-feedback-browser.mjs`。`TH06_FEEDBACK_RUNTIME` 可指定夹具构建目录，`TH06_FEEDBACK_OUTPUT` 可指定回归结果目录。

## 目录历史

本轮标准 `git subtree split --prefix=ports/th06 --quiet HEAD` 导出树与源码树一致，修复提交不含子树控制标记，原来源基点仍是主仓库祖先。[导出核验](subtree-verification.json)同时记录当前与基线的导出：两者均未包含原来源基点。当前分支沿原基线追加两个修复／验证提交。
