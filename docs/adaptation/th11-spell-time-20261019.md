# th11 多人符卡结算计时 | 2026-10-19

## 原因与处理

多人通过 `GameSession::update_multiplayer` 推进战斗，不经过 SDL 单人 `tick()` 里的 `SpellTiming::update`。因此结算时帧数和编码时间没有更新，HUD 解码无效值后显示 `999.99s`。

现在在 `GameBattle::update` 的多人分支、HUD 更新前采样。与 th15 多人的原则一致，使用符卡控制器累计的模拟帧，不读取本地墙钟，也不读取单人 Replay 的计时槽。多人采样与普通版共用 th11 原有时间舍入、编码和绘制格式。

`SpellTiming` 已纳入 `WorldRollback` 的恢复字段；中途旁观使用同步的符卡帧数和标志完成计时，不要求发布端的本地时间戳。符卡完成后只结算一次，网络等待和暂停期间不额外累加。

## 验证与交付

检查项：RUNTIME-02、MP-04、MP-07、MP-11。

`portable/check-spell-timing.mjs` 编译并执行真实 C++ 的 WASI 回归：不同长度、连续 20 张符卡、原有编码对照、回滚恢复、中途旁观和重复结算检查通过。th11 multiplayer Emscripten 构建及 Runtime 打包成功，WASM SHA-256 为 `85959971ffcf0d218923832b90966a55a8bfddcf4f5d0207e942223828a74227`。

真实生产 Runtime 的双客户端浏览器检查在 mode 0 和 mode 1 均通过，分别取得 101、102 个同帧一致样本；进入房间即显示标题，不进入 Demo，开局复用预加载实例，未出现 Runtime 错误。

test 已部署 `th11-spell-time-20261019-56dfffcf84bb`，App Shell `a50eefef3527fe6962e7`。交付 th11 multiplayer Runtime，使用测试站现有前端与资源入口。随版本提供 th11 的程序编码表精确路径，游戏包仍由原资源路径或手动导入提供。发布文件校验通过，覆盖 3405 个文件和 17 组 Runtime。

发布及回滚记录位于 `/var/tmp/th11-spell-time-test-20261019/`；回滚同时恢复版本链接及该目录备份的 Nginx 配置。测试源码不依赖原版游戏数据，实际运行和同步检查另用私有数据包。
