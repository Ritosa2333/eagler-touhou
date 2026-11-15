# TH17 主站分辨率与双席移动复测 · 2026-11-07

主站发布 TH17 多人版 `1.0.14-eagler-904ba36fc710`，启动器的游戏设置提供 1280×960 和 640×480。前端从线上 PC98／TH075 构建叠加 TH17 设置。公网按发布清单核对启动器入口和 TH17 两套运行包共 30 个文件。[发布回执](resolution-activation.json) · [公网校验](resolution-public-verification.json)

多人游戏设置抽屉通过 React Portal 挂载共用设置和标题栏，弹出列表位于抽屉上方。浏览器回归用例覆盖 TH17 桌面点击分辨率和 TH06 手机视口触控切换语言。旧版在点击 TH17 下拉触发器后未打开列表，修复版两种输入均能完成选择。[抽屉发布](drawer-activation.json) · [公网校验](drawer-public-verification.json) · [镜像校验](drawer-mirror-verification.json)

## 普通关三人压测

三个独立 Chromium 页面通过本地 Relay 开房，1280×960，普通难度一面，OGG、回滚和 WebGL2。每方向注入 60 ms 延迟和最多 15 ms 抖动。六倍 CPU 限速的单席滑动中，旧包在关卡时钟约 3113 的一次零补算回调里，确认处理耗时 24.30 ms，其中快照释放占 18.17 ms。另一席同段绘制耗时达到 28 ms。[分段旧包](baseline-trace.json)

快照以连续数组保存借用动画根的所有权和键，恢复时重建查询表。确认后优先让首张快照也等到画面提交再释放，沿用原快照环的两个暂存位置。应用性能检查分别记录未加分段计时的窗口提交和分段数据。

| 六倍限速，单席滑动 | 提交 FPS（P1 / P2 / P3） | 超过 25 ms 的间隔 | 一致确认帧 |
| --- | --- | --- | ---: |
| [旧包](baseline.json) | 57.04 / 54.80 / 57.48 | 101 / 183 / 91 | 3,853 |
| [快照优化包](optimized.json) | 59.49 / 58.94 / 59.54 | 26 / 40 / 18 | 3,853 |

[快照优化分段](optimized-trace.json)记录绘制、快照采集与确认释放。快照方案另通过原生会话和动物灵用例。

## 双席同时滑动

仅 P2 限速六倍。观测以窗口提交和画布采集视频的 `presentedFrames` 分别计数。采集视频回调的间隔单独保留，用于识别主线程忙碌造成的回调合并。

旧包单席滑动的画布采集约 59.6 FPS。P1、P2 同时滑动时，两席在 1,101 个相同的逻辑帧发送触控，P1、P3 的画布采集都降到 44.06 FPS，并分别发生 435、436 次修复期间未出画的刷新。P2 画布采集为 58.72 FPS。两组分别确认 2,201 和 2,204 个一致战斗帧。[单席滑动](asymmetric-single.json) · [双席滑动](asymmetric-both.json)

回滚常规补算片为四帧。在当前刷新预算能容纳剩余补算和一次绘制时，补算继续到当前回滚窗口内的修复终点。调试栏 `gap` 记录两次提交之间的墙钟间隔。

| 双席滑动 | 画布采集 FPS（P1 / P2 / P3） | 补算期间未出画 | 一致确认帧 |
| --- | --- | --- | ---: |
| [三人旧包](asymmetric-both.json) | 44.06 / 58.72 / 44.06 | 435 / 33 / 436 | 2,204 |
| [三人八帧预算](asymmetric-both-repair.json) | 53.97 / 59.96 / 53.93 | 135 / 0 / 136 | 2,202 |
| [三人完整窗口](asymmetric-both-window.json) | 59.90 / 59.95 / 59.90 | 0 / 0 / 0 | 2,200 |
| [双人旧包](2p-both-baseline.json) | 44.88 / 58.84 | 405 / 30 | 2,201 |
| [双人八帧预算](2p-both-repair.json) | 55.55 / 60.00 | 98 / 0 | 2,200 |
| [双人完整窗口](2p-both-window.json) | 59.56 / 59.61 | 0 / 0 | 2,201 |

完整窗口包为 `1.0.14-eagler-aad49cdebd08`，源提交 `e43707b52`。三人房滑动段最长提交间隔 35.93 ms，双人房 49.74 ms。该包的混合分辨率三人房核对 1,052 个一致确认帧。[混合分辨率复测](mixed-window-smoke.json)

18:06 主站发布完整窗口包。公网核对启动器入口和 TH17 两套运行包共 30 个文件，镜像核对同一发布标识、启动器入口哈希和多人 Wasm。[发布回执](dual-motion-activation.json) · [公网校验](dual-motion-public-verification.json) · [镜像校验](dual-motion-mirror-verification.json)

## 复测

资源路径沿用[多人绘制房测](../th17-render-presentation-20261107/README.md#复测)。双席滑动用 `TH17_ROOM_TOUCH_SEATS=0,1`、`TH17_ROOM_CPU_THROTTLE=6`、`TH17_ROOM_CPU_THROTTLE_SEAT=1`、`TH17_ROOM_SAMPLES=2200`。传入 `--launcher-preview --check-presentation --independent-pages --touch-drag --symmetric-delay --trace-gaps --observe-display`，用 `TH17_ROOM_SEATS` 选择 2 或 3 人。
