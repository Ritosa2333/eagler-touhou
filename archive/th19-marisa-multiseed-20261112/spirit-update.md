# 转换灵体更新顺序

灵体转换初始化、位移、脚本更新与脚本结束移除遵循原版调用顺序。种子 123456789 第 20849 帧脚本返回后立即移除，额外死亡散射消除。

构建 aaa4b976a90878ae414c8c3e45179d4c68322295307cf7637425897366dbead6 的前 20870 帧主 RNG、CPU 输入、自机与敌弹精确坐标一致。

- [原版调用证据](spirit-conversion-calls.json)
- [短测结果](spirit-update-window.json)
- [20849 帧修复前](screenshots/seed123-20849-before/frame-20849.png)
- [20849 帧修复后](screenshots/seed123-20849-after/frame-20849.png)
