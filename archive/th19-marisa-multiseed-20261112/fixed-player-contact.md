# 固定激光命中切段

原版 0x549754 在无护盾命中后进入 Pending 并返回接触 1。固定激光的 0x52b428 分支随后以自机为中心，用 32×32 矩形切段。Web 在同一命中入口保留激光编号，对该控制器执行对应切段。

种子 1 第 32880 帧的切段恢复原版下一帧的 CPU 避弹几何。构建 929daff3b8bd7e7d7b230ffade5c35f54cdc9f8ef2b7352a06df4a953285dcd7 前 32910 帧主 RNG、CPU 输入、自机与敌弹精确坐标全部一致。

- [原版切段掩码](fixed-player-contact-native.json)
- [短测结果](fixed-player-contact-window.json)
- [32881 帧修复后左右图](screenshots/seed1-32881-after/frame-32881.png)
