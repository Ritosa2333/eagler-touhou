# TH11 MP 换关过渡回滚修复

基线：真实 MP 工作树 `worktrees/th11-multiplayer`，`eagler-mp` 的 `ceee5c6`。启动器验证使用 `worktrees/launcher-th09-spectator-main` 当前 `main`（`b667ab0`，包含本机既有未提交修改），没有使用目录重构分支，也没有修改启动器源码或发布服务。

原来的 `WorldRollback::CanCapture()` 将整个换关重叠期排除，120 帧结算等待和 30 帧背景衔接都退回逐帧确认。D0 联机下，这段动画的推进受网络往返延迟限制。现在同时保留新旧 Stage 的检查点、资源身份和形变状态，只在创建对象、激活新关、销毁旧 Stage 和释放旧资源前请求确认屏障。载入新关本身仍保留原有确认屏障。未改变原结算时钟、输入、碰撞和出场顺序。

验证：

- `node portable/multiplayer/world-rollback-test.mjs --suite world --output artifacts/transition-fix-all-stages`：39 个用例、761448 项检查通过。2P/3P 分别覆盖 1→2 到 5→6 的十次衔接，新旧 Stage 的完整状态及 Draw 命令经过正确输入、错误预测、撤销和重放比对。每次有 148 个可回滚过渡帧和 2 个实际确认屏障。
- `portable/multiplayer/rollback-browser-acceptance.mjs` 使用启动器 main 的真实目录契约、桌面 Chrome、隔离本地 relay、单向延迟 50±5 ms，10 秒浏览器联机检查通过，双端逻辑推进约 60 Hz。记录在 `artifacts/transition-fix-browser-chrome/receipt.json`。这是稳定游戏区间的真实浏览器回归，不把它描述为浏览器完整换关或移动设备性能验收。
- MP 正式编译、打包通过，WASM SHA-256：`6a1321aacad6bf320bf9a20079621d32721de20ef9c01f077bcc1bb37ae28fa8`，目录 `build-eagler-multiplayer/`。7 项打包隔离检查和 26 项托管输入/诊断检查通过。

浏览器验收脚本支持当前 main 的扁平目录和重构目录，明确选用传入的启动器路径。没有更换多人时序政策或削弱哈希、检查点、回放验证。

用户已要求提交并更新主站两作 MP。发布以当前线上版本为底，仅更新 TH10/TH11 的 MP Runtime 和对应清单，保留启动器、其他游戏、资源包及现有 relay。发布身份、公网逐文件校验和回退目录记录于工作区 `artifacts/mp-runtime-fixes-publish-20261015/publish/`，以该目录的实际发布结果为准。
