# thsss 练习

原生 session（`thpracRuntime.session = "native"`）。启动器只在开局前下发 `thpracEnabled` 与 `thpracLocale`。关卡、资源和工具由 Runtime 持有。对照 [PRACTICE-01..08](../../docs/adaptation/localization.md#practice-01)。

上游行为对照 [ShiningShootingStar_Prac](https://github.com/RUEEE/ShiningShootingStar_Prac)。界面只列出下面已经接到游戏循环的项。

| 操作 | 行为 |
| --- | --- |
| Backspace | 打开或关闭练习菜单 |
| F1 | 无敌（每帧把自机时间保持在无敌窗内） |
| F2 | 禁用炸弹 |
| F3 | 失焦不暂停 |
| 关卡按钮 | 跳到 `St1`–`St6`、`Bs1`–`Bs6`、`StEx` |
| 应用开局资源 | 残机 0–8、炸弹 0–8、Power 0–400，在自机存在时写入 |

`thpracEnabled` 为 false 时，上述入口不作用，原版 Stage Select 保持可用。

语言为 `zh-CN`、`ja-JP`、`en-US`。触控功能键走同一组 Backspace / F1–F3。`thprac-mouse` 的按下命中练习菜单上的按钮。

Replay 头在 `TouchDelta` 之后写一行 `Prac=1,无敌,禁炸弹,失焦不暂停,残机,炸弹,Power`。读档时还原这些值。没有旁路 sidecar。

本轮不做 F12 高级面板、分辨率修改，以及上游尚未完成的功能。
