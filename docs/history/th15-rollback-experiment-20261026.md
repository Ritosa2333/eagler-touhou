# TH15 多人回滚实验

`experiment/th15-rollback-20261026` 在独立工作区实现 TH15 多人逐帧恢复。原延迟模式继续作为房间默认值；开启回滚后使用十二帧纠错窗口，恢复到迟到输入之前，重演完成后继续推进新帧。

后续[多人压力验证](th15-multiplayer-stress-20261026.md)覆盖真实高刷、弱网和密集状态恢复：确认输入一致性通过，密集消弹道具的动画状态恢复出现分歧，CPU 6 倍限速下的回滚成本需要优化。当前分支继续作为实验版本。

快照分别保存动画分配和句柄世代、父子及嵌套对象、实体生命周期、并发 ECL 线程、计时器小数和前值、两套随机数、三名玩家的机体与炸弹、弹幕与四类激光、道具归属、背景、HUD 和会话输入。快照复用高水位存储，持有窗口内的动态对象与资源；章节重试保持原有实现。

重演执行原生更新和逻辑绘制，省去 GPU 提交与插值帧采样。音效和音乐按确认顺序提交，录像与观战写入实际输入。暂停、对话、机体工厂和场景切换在真实输入确认后执行。

FPS 统计可见画面的实际进展，原生六十帧逻辑与显示刷新分别计量。重复提交停滞画面、重演旧帧不会增加 FPS；独立的数字绘制在等输入时仍会更新。

原生验证包含动画图与溢出池、回调顺序、敌人和脚本、原版四类激光、完整战斗状态、协议、帧率及副作用。完整战斗使用原版数据，覆盖全部四种机体，在放弃不同输入的未来后完成 288 次逐帧一致性比较。浏览器验证覆盖双人、三人、迟到且不同的输入触发实际纠正、停滞零 FPS 与画面保留、确认帧录像、观战及离线播放。

原生检查使用 Windows CMake 和 MSVC：

```powershell
cmake -S ports/th15-mp/portable/rollback -B artifacts/th15-rollback-native -DTH15_TEST_ARCHIVE=<本地合法资源路径>
cmake --build artifacts/th15-rollback-native --config Release --parallel 4
ctest --test-dir artifacts/th15-rollback-native -C Release --output-on-failure
```

浏览器脚本为 `ports/th15-mp/portable/multiplayer/browser-test.mjs`。`TH15_TEST_TIMING_MODE=0` 验证回滚，`1` 验证原延迟模式；资源与共享字体通过现有测试环境变量提供。

TH15 普通版源码保持原样。多人来源沿原仓库祖先建立独立子树连接，记录于 `docs/migration/th15mp-eagler-20261026.json`。普通开发及目录投影不复制子树控制标记；连接提交独立登记基点，以标准 `git subtree split --prefix=ports/th15-mp --quiet HEAD` 核对来源祖先和源码树。
