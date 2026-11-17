# TH17 滑动联机与魂魄复活 · 2026-11-07

多人版在补算或触及预测上限时，按 60 Hz 继续采样真实输入，战斗期间的队列至多领先两帧。对端可以先收到这两帧，补算完成后按序消耗。发送端把同一轮的常规重发合并到紧随其后的新输入包。补算中间帧保留 HUD 结算面板的状态变化，数字排版和字形绘制在呈现时执行。

残机耗尽的席位以半透明原机体显示为魂魄，使用各席独立的随机状态进行边界内漂移。退场时清除该席的射击、判定标记与子机。下一关激活时复活该席，保留已积累的碎片，恢复基础 Bomb、半满灵力和 120 帧无敌。魂魄状态随战斗快照恢复，旁观关卡快照携带退场席位。

## 滑动复测

双人及三人房通过本地 Relay 在独立 Chromium 页面运行。Intel 核显以 1280×960 渲染，启用 OGG。第二席依次静止、键盘移动、滑动并松手，滑动轨迹按实际时间生成。呈现帧率按实际画面提交计数，目标为 60 FPS。每席还统计超过 16.67 ms 和 25 ms 的呈现间隔。

常规档对每个方向注入 50 ms 延迟和最多 15 ms 抖动，对各渲染进程施加 4 倍 CPU 限速。加压档为每方向 100 ms 延迟、最多 25 ms 抖动和 6 倍 CPU 限速。基线使用此前符卡切入优化包 `1.0.14-eagler-44849dc7be98`，最终包为 `1.0.14-eagler-362f43c5a1ee`，源码为 `818ca8d25`。

| 滑动场景 | 实际呈现 FPS（P1 / P2 / P3） | 大于 25 ms 的间隔 | 确认帧 |
| --- | --- | --- | ---: |
| [常规三人基线](three-player-baseline.json) | 59.587 / 59.389 / 59.702 | 17 / 28 / 19 | 3,851 |
| [常规三人最终包](three-player-final.json) | 59.519 / 59.328 / 59.511 | 9 / 14 / 10 | 3,850 |
| [常规三人最终包复跑](three-player-repeat.json) | 59.988 / 59.936 / 59.962 | 3 / 8 / 8 | 3,853 |
| [常规双人最终包](two-player-final.json) | 59.981 / 60.003 | 2 / 2 | 3,851 |
| [三人加压基线](stress-baseline.json) | 52.050 / 42.105 / 51.947 | | 3,853 |
| [三人加压最终包](stress-final.json) | 55.575 / 44.945 / 55.624 | 193 / 640 / 184 | 3,850 |

常规档三人每席延迟注入包从约 15,770 个降到 7,950 个。最终包的两轮三人常规档分别录得 246.15 ms 和 38.64 ms 的最长间隔。加压档滑动席 P2 的未呈现回调从 925 降到 780 个。两轮加压里，P2 的滑动段补算数为零，长间隔采样中未推进的回调均处在 12 帧预测上限。常规双人两席的最长间隔分别为 40.05 ms 和 28.82 ms。

## 魂魄复活

三人房从一面打到二面。专用测试构建在逻辑帧 800 使 P3 在最后一条命受到一次命中，随后走正常死亡流程。P3 在一面有 9,380 个魂魄确认帧和 9,318 个不同位置，二面第 10,370 帧以 2.00P 复活。三端共有 10,342 个一致确认帧。[三人房报告](ghost-revival.json)

## 复测入口

环境变量沿用[换面长帧对照](../th17-frame-spikes-20261107/README.md#复测)。滑动档设置 `TH17_ROOM_SEATS=2` 或 `3`、`TH17_ROOM_DELAY_MS=50`、`TH17_ROOM_JITTER_MS=15` 和 `TH17_ROOM_CPU_THROTTLE=4`。

```powershell
node ports/th17-mp/web/tests/multiplayer/room-browser.mjs --launcher-preview --check-presentation --independent-pages --touch-comparison --symmetric-delay --trace-gaps
```

魂魄档使用 `--release --room-benchmark` 构建包，设置 `TH17_ROOM_SEATS=3`、`TH17_ROOM_DELAY_MS=145`、`TH17_ROOM_DIFFICULTY=1`、`TH17_ROOM_CPU_THROTTLE=4`。

```powershell
node ports/th17-mp/web/tests/multiplayer/room-browser.mjs --launcher-preview --check-presentation --independent-pages --through-stage-two --ghost-revive
```
