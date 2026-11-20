# 魔理沙 CPU 对战十种子验证

同一构建 `fdbbcb85203fba2d5cc205aedcc468b6090ce11e0685303eafdc8d2b44378446` 完成十组跨轮累计 36000 帧验证，共覆盖 360000 帧。主 RNG、CPU 输入和自机位置逐帧一致，敌弹类型、颜色和数量逐帧一致，坐标按 0.0001 容差通过。

共比对 359995 个敌弹采样帧，最大坐标误差为 6.103515625e-05。换轮缺样本通过相邻轮号及新一轮第 1 帧核验。

| 主种子 | 副种子 | 累计帧数 | 敌弹状态帧 | 换轮缺样本 | 结果 |
| --- | --- | --- | --- | --- | --- |
| 314159265 | 271828182 | 36000 | 36000 | 无 | [PASS](seed314159265-final-gameplay.json) |
| 65537 | 104729 | 36000 | 35999 | 31266 | [PASS](seed65537-final-gameplay.json) |
| 20261116 | 998244353 | 36000 | 36000 | 无 | [PASS](seed20261116-final-gameplay.json) |
| 161803398 | 141421356 | 36000 | 36000 | 无 | [PASS](seed161803398-final-gameplay.json) |
| 2147483646 | 1234567 | 36000 | 36000 | 无 | [PASS](seed2147483646-final-gameplay.json) |
| 7 | 7654321 | 36000 | 35999 | 26401 | [PASS](seed7-final-gameplay.json) |
| 1000003 | 999983 | 36000 | 35999 | 27132 | [PASS](seed1000003-final-gameplay.json) |
| 8675309 | 42424243 | 36000 | 35999 | 28448 | [PASS](seed8675309-final-gameplay.json) |
| 536870911 | 1000000007 | 36000 | 35999 | 27970 | [PASS](seed536870911-final-gameplay.json) |
| 2000000011 | 31415926 | 36000 | 36000 | 无 | [PASS](seed2000000011-final-gameplay.json) |

[完整机器验证结果](final-ten-gameplay.json) 保存构建、原版轨迹摘要、缺样本核验及坐标误差。

采集在 Windows Server 上运行原版与 Edge 网页版，使用魔理沙对魔理沙 CPU 对局、Lunatic 难度和无卡配置。原版采用 D3D9On12/WARP 软件后端，双端导入相同 CPU fixture 与主副种子。

固定激光被炸弹切断后，残段保留出生位置与完整长度，后续激光管理器统一推进。蓄力清场首次更新按控制器注册顺序观察新生固定激光。普通结算保留清场灵体注册计数，超时结算先注销已清场灵体。

左右对照图统一采用左侧网页版、右侧原版，并标注累计帧号、输入、自机坐标及截至当前帧的玩法差异数。

- [1000003 第 21803 帧修复前](screenshots/seed1000003-bomb-remnant-before/frame-21803.png)
- [1000003 第 21803 帧修复后](screenshots/seed1000003-bomb-remnant-after/frame-21803.png)
- [65537 第 28611 帧修复前](screenshots/seed65537-registration-before/frame-28611.png)
- [65537 第 28611 帧修复后](screenshots/seed65537-registration-after/frame-28611.png)
- [161803398 第 36000 帧修复前](screenshots/seed161803398-timeout-before/frame-36000.png)
- [161803398 第 36000 帧修复后](screenshots/seed161803398-timeout-after/frame-36000.png)

ANM 生命周期、234 组原版浮点随机数位模式、2000 弹槽饱和与复用、15000 退休敌人清理及检查点恢复测试通过。
