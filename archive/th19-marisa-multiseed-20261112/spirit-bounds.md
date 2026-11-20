# 转换灵体的当前身体边界

CPU 登记已转换灵体前复核当前身体尺寸，完整离开场地的灵体不再参与避障。种子 42 中，动物灵在转换后物理尺寸从 32 缩到约 29.33，位置 `(119.18,463.76)` 已完全在场外。该次复核保留 ECL 的转换与移动顺序，同时避免多登记一帧 CPU 障碍。

构建 `d9e29861dbbd2c077ae9597799af30995d830ee85e1fe9ea6c58f67b06f5309f` 对拍到 20320 帧，主 RNG、CPU 输入、自机位置、敌弹数量及精确坐标全部一致，20288—20310 帧回滚无差异。

- [原版与Web退场边界诊断](spirit-bounds-diagnostic.json)
- [修复后短测结果](spirit-bounds-window.json)
- [20298 帧修复前](screenshots/seed42-20298-before/frame-20298.png)
- [20298 帧修复后](screenshots/seed42-20298-after/frame-20298.png)

左右图左侧 Web，右侧原版，累计帧数为玩法差异，动画 RNG 单列。完整回归另行记录。

完整回归见[五种子结果](spirit-bounds-five-seeds.json)。种子 42 首次敌弹坐标差异推进到 24910 帧。原基准报告 `th19-web-hxqkjM` 在 36000 帧的主 RNG、CPU 输入、自机位置、敌弹数量及精确坐标全部一致。53 项测试及文档检查通过。
