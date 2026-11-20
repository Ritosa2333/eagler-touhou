# 魔理沙新增五种子验证

新增五种子在同一最终构建 `8c5e586168602a5912e60c7b3da91e2ffcb3f9e86e39e216906c2ce0898cc2a8` 各完成跨轮累计 36000 帧验证，连续五组全部通过，共 180000 帧。此前十组同构建回归全部通过，十五组共 540000 帧。

主 RNG、CPU 输入和自机位置逐帧一致。敌弹类型、颜色和数量一致，坐标按 0.0001 容差通过。共比对 539993 个敌弹采样帧，最大误差 6.103515625e-05。7 个换轮缺样本经相邻轮号和下一轮第 1 帧核验。

| 主种子 | 副种子 | 累计帧数 | 结果 |
| --- | --- | --- | --- |
| 123456789 | 987654321 | 36000 | [PASS](seed123456789-final-gameplay.json) |
| 20261118 | 18112026 | 36000 | [PASS](seed20261118-final-gameplay.json) |
| 424242 | 131071 | 36000 | [PASS](seed424242-final-gameplay.json) |
| 1073741823 | 524287 | 36000 | [PASS](seed1073741823-final-gameplay.json) |
| 19840124 | 20010401 | 36000 | [PASS](seed19840124-final-gameplay.json) |

转换狼灵在 ANM 推进前发布边界。完整消除的直线激光停止发放擦弹奖励。物理接触切出的固定激光残段从下一次激光管理器访问开始更新。

Windows Server，原版 D3D9On12/WARP 软件后端，Edge 网页版，魔理沙对魔理沙 CPU 对局，Lunatic，无卡。双端使用相同 CPU fixture 与主副种子，按战斗帧跨轮累计。纯动画差异按既定范围忽略。

截图采用左侧网页版、右侧原版，标注累计帧号和累计玩法差异帧数。

- [20261118 第 28071 帧修复后，玩法差异0帧](screenshots/seed20261118-after/frame-28071.png)
- [20261118 第 28071 帧修复前](screenshots/seed20261118-before/frame-28071.png)
- [19840124 第 21912 帧修复后，玩法差异0帧](screenshots/seed19840124-after/frame-21912.png)
- [19840124 第 21912 帧修复前](screenshots/seed19840124-before/frame-21912.png)
- [424242 第 32708 帧修复后，玩法差异0帧](screenshots/seed424242-after/frame-32708.png)
- [424242 第 32708 帧修复前](screenshots/seed424242-before/frame-32708.png)

[十五组完整机器验证结果](final-fifteen-gameplay.json) 保存构建、原版轨迹摘要、首差字段及敌弹坐标验证结果。
