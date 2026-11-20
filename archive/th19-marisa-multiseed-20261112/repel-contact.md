# 护盾弹开首帧敌弹取消

护盾弹开首帧保留普通弹的当前状态，后续无敌接触按原版时机取消敌弹。种子 1 第 25522 帧的敌弹提前取消消除。

构建 a57dc2fb30cf374ed8ed82ff8c7ae3e650a3296b26c3c863999341d1e45c04d2 的前 25540 帧主 RNG、CPU 输入、自机与敌弹精确坐标一致。

- [短测结果](repel-contact-window.json)
- [25522 帧修复前](screenshots/seed1-25522-before/frame-25522.png)
- [25522 帧修复后](screenshots/seed1-25522-after/frame-25522.png)
