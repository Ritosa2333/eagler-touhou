# TH12 / TH15 多人挑战模式

挑战模式将生命栏改为各玩家的累计 Miss，禁用 Bomb 与兔机护盾，保留原生死亡、掉 P 和重生流程。隐藏残机库存继续承接 Extend 与赠残；零残死亡不触发幽灵或团灭。Miss 独立计数、跨面保留，并在 `uint32` 上限饱和。

TH12 的 SDL 与旧版浏览器 HUD 同步显示数字；通关录像使用实际结局／结算状态。TH15 的玩家会话保存挑战状态与 Miss，回滚快照同时恢复。观战章节初始化使用第 3 版编码保存各座位计数。多人游戏规则版本分别更新为 TH12 第 8 版、TH15 第 24 版；历史规则录像按原有运行时契约读取，已登记的 TH15 第 20 版兼容入口保留其原有规则。

## 验证

[验证数据](validation.json)覆盖 TH12 六机体、TH15 四机体的双人和三人挑战流程：连续 12 次零残死亡、原生掉 P、重生、禁雷、隐藏库存奖励、换面、回滚、重新开局、计数饱和以及普通模式对照。TH15 额外覆盖直接护盾入口与数字 HUD；两作均覆盖赠残与独立 Miss。

TH12 原生回归 3 项、相关模块测试 44 项、TH15 原生回归 17 项通过。两作各 9 项来源检查通过。两作生产浏览器版本编译通过。TH15 双人／三人浏览器夹具在一面到二面的真实换面后，用独立 Wasm 的原生 Replay 入口恢复章节，各玩家 Miss、隐藏残机、禁雷状态一致。夹具使用本地原作资源及生产游戏对象文件。

原生复现命令：

```sh
cmake -S ports/th12-mp -B artifacts/challenge-th12 -DBUILD_TESTING=ON
cmake --build artifacts/challenge-th12 --config Release --target th12_native_smoke th12_multiplayer_challenge th12_multiplayer_menu_safety
ctest --test-dir artifacts/challenge-th12 -C Release --output-on-failure
cmake -S ports/th15-mp/portable/rollback -B artifacts/challenge-th15 -DTH15_TEST_ARCHIVE="$TH15_TEST_ARCHIVE"
cmake --build artifacts/challenge-th15 --config Release
ctest --test-dir artifacts/challenge-th15 -C Release --output-on-failure
```

`TH15_TEST_ARCHIVE` 指向本地合法提供的日文 TH15 1.00b 数据归档。原生构建使用 C++17／C++20 编译器；Windows 可使用 Visual Studio CMake 生成器。

## 源码历史

两作多人目录的标准 `git subtree split --prefix=ports/<game>-mp --quiet HEAD` 导出通过，上游来源提交均为祖先，导出树与当前源码目录树一致，见[导出核验](subtree-verification.json)。普通版源码树保持原样。目录投影沿各作品已登记来源刷新，功能提交不携带子树控制标记。
