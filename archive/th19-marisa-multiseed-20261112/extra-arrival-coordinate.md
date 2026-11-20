# EX 到达坐标的双阶段量化

种子 42 在 29600 帧窗口内，主 RNG、CPU 输入、自机位置和敌弹坐标全部一致。构建为 `0acff0ee97997e0a8bc315a8639269420d7ebf1cf74733b2670592fc71212be4`。

EX 到达敌机在启动 ECL 前分别量化绝对坐标和合成坐标。原版 `present-wx2g24w_` 记录到 29555 帧的敌机 4536 横坐标为 76.4400024。相同目标原始坐标 76.4538727 经首次 float 网格量化为 76.4499969，二次量化后得到原版值。另一个目标首次量化为 -4.80000019，二次量化为 -4.80999994。

补齐第二次量化后，两条固定激光出生位置匹配。第 29596 帧 CPU 障碍遮挡位图和移动选择恢复一致。

[原版坐标与浮点证据](extra-arrival-coordinate-diagnostic.json)

[29600 帧窗口验证](extra-arrival-coordinate-window.json)

左右对照位于 `screenshots/seed42-29596-before` 与 `screenshots/seed42-29596-after`。
