# 弹开首帧子机更新

原版在弹开首帧先使用位移前的自机坐标更新子机，随后冻结弹开期间的子机发射点。网页版现在保留这一次位置更新，后续弹开帧沿用冻结位置。

种子 42 第 25964 帧，旧版左束子机激光原点 x 为 -82.8984375，原版为 -83.5100021。偏移使网页版多命中一次 18 伤害，敌机提前死亡并多消耗一次主 RNG。原版回调日志记录每次命中前后长度和返回伤害，首帧坐标修复后该窗口主 RNG、CPU、自机与敌弹精确坐标全部一致。

构建 8a42ff06106f98d9ea9c39abcec4008c42d8d450e0e703ce882e5e62e1ec2e1f 已通过种子 42 前 25970 帧和原基准累计 36000 帧。

- [修复后短测](repel-option-first-window.json)
- [原基准回归](repel-option-first-baseline.json)
- [第 25964 帧修复前左右图](screenshots/seed42-25964-before/frame-25964.png)
- [第 25964 帧修复后左右图](screenshots/seed42-25964-after/frame-25964.png)

接触帧更新构建 26d1a0c09cc6f0881b6f173641ec559042557a72a464a408e0799dd0dca4eafb 将这一次更新放在 frame=0 的接触帧。种子 42 前 25970 帧全部玩法一致，种子 123456789 前 22122 帧全部玩法一致，覆盖其第 14582 帧的回归窗口。

- [接触帧修复短测](repel-option-contact-window.json)
- [种子 123456789 回归](repel-option-contact-seed123.json)
