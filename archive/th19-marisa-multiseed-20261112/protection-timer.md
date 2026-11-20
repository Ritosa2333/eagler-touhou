# VS 出生保护计时

VS 敌机的出生无敌与接触保护计时按全局时钟递减。Aura 将普通敌机位移减半，保护计时每帧仍减 1。直接采集原版两个保护计时器及其时钟绑定，种子 20261112 的敌机在 16222 帧计时为 1，16224 帧为 0；16225 帧受到 4 点伤害。修复后死亡奖励和后续弹道按原版顺序执行。

构建 `187c053f97ffeccaa5424fa5dd8778d4dcbabd157d0bd1f2aa0ea50f08653c67` 完成 16300 帧对拍：主 RNG、CPU 输入、自机位置、敌弹数量和精确坐标全部一致，16260—16285 帧回滚无差异。

- [原版计时器与旧Web状态](protection-timer-diagnostic.json)
- [修复后短测结果](protection-timer-window.json)
- [16278 帧修复前](screenshots/seed2026-16278-before/frame-16278.png)
- [16278 帧修复后](screenshots/seed2026-16278-after/frame-16278.png)

左右图左侧 Web，右侧原版，累计帧数为玩法差异，动画 RNG 单列。完整回归另行记录。

完整回归见[五种子结果](protection-timer-five-seeds.json)。首次敌弹差异依次为 22016、20299、16428、20849、25238 帧，均完成 36000 帧采样，尚未全部对齐。原基准报告 `th19-web-Akkae4` 在 36000 帧的主 RNG、CPU 输入、自机位置、敌弹数量和精确坐标全部一致。
