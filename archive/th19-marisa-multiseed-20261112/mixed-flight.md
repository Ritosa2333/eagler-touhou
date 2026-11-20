# 魔理沙飞行攻击到达顺序

魔理沙 C2 飞行攻击与普通 EX 飞行攻击共享原版注册顺序。接收场的递增编号记录两种来源的创建先后，先处理编号更新的已到达攻击，再执行对应 C2 的同步脚本，保持激光原点与随机角度的配对。

构建 7fb1b84cae6f22f13c211473fba3aec2b13fa79f039be9412f23c3d8635633fa 的种子 1 前 32880 帧主 RNG、CPU 输入、自机与敌弹精确坐标一致。

- [32875 帧激光原点配对证据](mixed-flight-origins.json)
- [短测结果](mixed-flight-window.json)
- [32875 帧修复前](screenshots/seed1-32875-before/frame-32875.png)
- [32875 帧修复后](screenshots/seed1-32875-after/frame-32875.png)
